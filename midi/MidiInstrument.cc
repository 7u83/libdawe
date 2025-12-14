/*
 * Copyright (C) 2005-2022, rncbc aka Rui Nuno Capela. All rights reserved.
 * Copyright (C) 2023 7u83 <7u83@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 */
#include "MidiInstrument.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include <regex.h>

#include "../Exception.h"
#include "../tools/RegEx.h"

namespace Dawe{

MidiInstrument::MidiInstrument()
{

}

const InstrumentData &Instrument::patch(int iBank) const
{
	if (m_pData->patches.find(iBank) != m_pData->patches.end())
		return m_pData->patches[iBank];

	return m_pData->patches[-1];
}



InstrumentData& Instrument::notes ( int iBank, int iProg ) const
{
	if (m_pData->keys.find(iBank) != m_pData->keys.end() ) {
		if (m_pData->keys[iBank].find(iProg) != m_pData->keys[iBank].end()) {
			return m_pData->keys[iBank][iProg];
		} else {
			return m_pData->keys[iBank][-1];
		}
	}
	else if (iBank >= 0)
		return notes(-1, iProg);

	return m_pData->keys[-1][-1];
}


bool Instrument::isDrum ( int iBank, int iProg ) const
{
	if (m_pData->drums.find(iBank) != m_pData->drums.end() ) {
		if (m_pData->drums[iBank].find(iProg) != m_pData->drums[iBank].end()) {
			return (bool) m_pData->drums[iBank][iProg];
		} else {
			return (bool) m_pData->drums[iBank][-1];
		}
	}
	else if (iBank >= 0)
		return isDrum(-1, iProg);

	return false;
}




void InstrumentList::clearAll (void)
{
	clear();

	m_patches.clear();
	m_notes.clear();
	m_controllers.clear();
	m_rpns.clear();
	m_nrpns.clear();

	//m_files.clear();
}

// Special list merge method.
void InstrumentList::merge ( const InstrumentList& instruments )
{
	// Maybe its better not merging to itself.
	if (this == &instruments)
		return;

	// Names data lists merge...
	mergeDataList(m_patches, instruments.patches());
	mergeDataList(m_notes, instruments.notes());
	mergeDataList(m_controllers, instruments.controllers());
	mergeDataList(m_rpns, instruments.rpns());
	mergeDataList(m_nrpns, instruments.nrpns());

	// Instrument merge...
	InstrumentList::const_iterator it = instruments.begin();
	const InstrumentList::const_iterator& it_end = instruments.end();
	for ( ; it != it_end; ++it) {
		Instrument& instr = (*this)[it->first];
		instr = it->second;
	}
}

// Special instrument data list merge method.
void InstrumentList::mergeDataList (
		InstrumentDataList& dst, const InstrumentDataList& src )
{

	InstrumentDataList::const_iterator it = src.begin();
	const InstrumentDataList::const_iterator& it_end = src.end();
	for ( ; it != it_end; ++it)
		dst[it->first] = it->second;
}

// The official loaded file list.
const std::vector<std::string>& InstrumentList::files (void) const
{
	return m_files;
}

static std::string trim(const std::string& str) {
	// Find the first non-matching character from the left end
	size_t left = str.find_first_not_of(" \t\n\r");

	// Find the first non-matching character from the right end
	size_t right = str.find_last_not_of(" \t\n\r");

	// Return the trimmed string
	return (left == std::string::npos || right == std::string::npos) ? "" : str.substr(left, right - left + 1);
}





// File load method.
bool InstrumentList::load ( const std::string& sFilename )
{
	// Open and read from real file.
	std::ifstream file(sFilename.c_str());


	/*	QFile file(sFilename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
*/
	if (!file.is_open())
		return false;

	// Check for a soundfont...
	/*	if (loadSoundFont(&file)) {
		file.close();
		appendFile(sFilename);
		return true;
	}
*/

	// Check for a MIDINameDocument...
	/*	file.seek(0);

	QDomDocument doc;
	if (doc.setContent(&file) && loadMidiNameDocument(doc)) {
		file.close();
		appendFile(sFilename);
		return true;
	}
*/

	// Proceed with regulat Cakewalk .ins file...
	file.seekg(0);

	enum FileSection {
		None       = 0,
		PatchNames = 1,
		NoteNames  = 2,
		ControllerNames = 3,
		RpnNames   = 4,
		NrpnNames  = 5,
		InstrDefs  = 6
	} sect = None;

	Instrument     *pInstrument = 0;
	InstrumentData *pData = 0;


	RegEx rxTitle   ("^\\[([^]]+)\\]$");
	RegEx rxBasedOn ("^BasedOn=(.+)$");
	RegEx rxData    ("^([0-9]+)=(.+)$");
	RegEx rxBankSel ("^BankSelMethod=(0|1|2|3)$");
	RegEx rxUseNotes("^UsesNotesAsControllers=(0|1)$");
	RegEx rxPatch   ("^Patch\\[([0-9]+|\\*)\\]=(.+)$");
	RegEx rxControl ("^Control=(.+)$");
	RegEx rxRpn     ("^RPN=(.+)$");
	RegEx rxNrpn    ("^NRPN=(.+)$");
	RegEx rxKey     ("^Key\\[([0-9]+|\\*),([0-9]+|\\*)\\]=(.+)$");
	RegEx rxDrum    ("^Drum\\[([0-9]+|\\*),([0-9]+|\\*)\\]=(0|1)$");

	/*	QRegularExpression rxTitle   ("^\\[([^\\]]+)\\]$");
	QRegularExpression rxData    ("^([0-9]+)=(.+)$");
	QRegularExpression rxBasedOn ("^BasedOn=(.+)$");
	QRegularExpression rxBankSel ("^BankSelMethod=(0|1|2|3)$");
	QRegularExpression rxUseNotes("^UsesNotesAsControllers=(0|1)$");
	QRegularExpression rxControl ("^Control=(.+)$");
	QRegularExpression rxRpn     ("^RPN=(.+)$");
	QRegularExpression rxNrpn    ("^NRPN=(.+)$");
	QRegularExpression rxPatch   ("^Patch\\[([0-9]+|\\*)\\]=(.+)$");
	QRegularExpression rxKey     ("^Key\\[([0-9]+|\\*),([0-9]+|\\*)\\]=(.+)$");
	QRegularExpression rxDrum    ("^Drum\\[([0-9]+|\\*),([0-9]+|\\*)\\]=(0|1)$");

	QRegularExpressionMatch match;
*/
	const char * s0_127    = "0..127";
	const char * s1_128    = "1..128";
	const char * s0_16383  = "0..16383";
	const char * sAsterisk = "*";

	// Read the file.
	//unsigned int iLine = 0;
	//	QTextStream ts(&file);

	//	while (!ts.atEnd()) {
	while (!file.eof()) {

		// Read the line.
		//++iLine;


		//const QString& sLine = ts.readLine().simplified();

		std::string sLine;
		std::getline(file, sLine);

		sLine = trim(sLine);

		// If not empty, nor a comment, call the server...
		if (sLine.empty())
			continue;

		if (sLine.at(0) == ';')
			continue;

		// Check for section intro line...
		if (sLine.at(0) == '.') {
			pInstrument = 0;
			pData = 0;
			if (sLine == ".Patch Names") {
				sect = PatchNames;
				//	m_patches.clear();
				m_patches[s0_127].setName(s0_127);
				m_patches[s1_128].setName(s1_128);
			}
			else if (sLine == ".Note Names") {
				sect = NoteNames;
				//	m_notes.clear();
				m_notes[s0_127].setName(s0_127);
			}
			else if (sLine == ".Controller Names") {
				sect = ControllerNames;
				//	m_controllers.clear();
				m_controllers[s0_127].setName(s0_127);
			}
			else if (sLine == ".RPN Names") {
				sect = RpnNames;
				//	m_rpns.clear();
				m_rpns[s0_16383].setName(s0_16383);
			}
			else if (sLine == ".NRPN Names") {
				sect = NrpnNames;
				//	m_nrpns.clear();
				m_nrpns[s0_16383].setName(s0_16383);
			}
			else if (sLine == ".Instrument Definitions") {
				sect = InstrDefs;
				//  clear();
			}
			else {
				// Unknown section found...
				//				qWarning("%s(%d): %s: Unknown section.",
				//					sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
			}
			// Go on...
			continue;
		}

		// Now it depends on the section...
		switch (sect) {
			case PatchNames: {
				if (rxTitle.exec(sLine.c_str())) {
					// New patch name...
					const std::string sTitle = rxTitle.captured(1); // .captured(1);
					pData = &(m_patches[sTitle]);
					pData->setName(sTitle);
					break;
				}



				if (pData == 0) {
					//qWarning("%s(%d): %s: Untitled .Patch Names entry.",
					//	sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
					break;
				}

				if (rxBasedOn.exec(sLine.c_str())) {
					pData->setBasedOn(rxBasedOn.captured(1));
					break;
				}


				//match = rxData.match(sLine);
				if (rxData.exec(sLine.c_str())) {
					(*pData)[std::atoi(rxData.captured(1).c_str())] =
							rxData.captured(2);
				} else {
					//qWarning("%s(%d): %s: Unknown .Patch Names entry.",
					//	sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
				}
				break;
			}


			case NoteNames: {

				if (rxTitle.exec(sLine)) {
					// New note name...
					const std::string& sTitle = rxTitle.captured(1);
					pData = &(m_notes[sTitle]);
					pData->setName(sTitle);
					break;
				}
				if (pData == 0) {
				//	qWarning("%s(%d): %s: Untitled .Note Names entry.",
				//		sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
					break;
				}

				if (rxBasedOn.exec(sLine)) {
					pData->setBasedOn(rxBasedOn.captured(1));
					break;
				}

				if (rxData.exec(sLine)) {
					(*pData)[std::atoi( rxData.captured(1).c_str())] = rxData.captured(2);
				} else {
					//qWarning("%s(%d): %s: Unknown .Note Names entry.",
					//	sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
				}
				break;
			}


			case ControllerNames: {
				if (rxTitle.exec(sLine)) {
					// New controller name...
					const std::string& sTitle = rxTitle.captured(1);
					pData = &(m_controllers[sTitle]);
					pData->setName(sTitle);
					break;
				}
				if (pData == 0) {
					//qWarning("%s(%d): %s: Untitled .Controller Names entry.",
					//	sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
					break;
				}
				//match = rxBasedOn.match(sLine);
				if (rxBasedOn.exec(sLine)) {
					pData->setBasedOn(rxBasedOn.captured(1));
					break;
				}
				//match = rxData.match(sLine);
				if (rxData.exec(sLine)) {
					(*pData)[std::atoi(rxData.captured(1).c_str()) ] = rxData.captured(2);
				} else {
				//	qWarning("%s(%d): %s: Unknown .Controller Names entry.",
				//		sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
				}
				break;
			}

			case RpnNames: {
				if (rxTitle.exec(sLine)) {
					// New RPN name...
					const std::string& sTitle = rxTitle.captured(1);
					pData = &(m_rpns[sTitle]);
					pData->setName(sTitle);
					break;
				}
				if (pData == 0) {
				//	qWarning("%s(%d): %s: Untitled .RPN Names entry.",
				//		sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
					break;
				}
				//match = rxBasedOn.match(sLine);
				if ( rxBasedOn.exec(sLine)) {
					pData->setBasedOn(rxBasedOn.captured(1));
					break;
				}
				//match = rxData.match(sLine);
				if (rxData.exec(sLine)) {
					(*pData)[ std::atoi(rxData.captured(1).c_str())] = rxData.captured(2);
				} else {
				//	qWarning("%s(%d): %s: Unknown .RPN Names entry.",
				//		sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
				}
				break;
			}

			case NrpnNames: {
				//match = rxTitle.match(sLine);
				if (rxTitle.exec(sLine)) {
					// New NRPN name...
					const std::string& sTitle = rxTitle.captured(1);
					pData = &(m_nrpns[sTitle]);
					pData->setName(sTitle);
					break;
				}
				if (pData == 0) {
					//qWarning("%s(%d): %s: Untitled .NRPN Names entry.",
					//	sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
					break;
				}
				//match = rxBasedOn.match(sLine);
				if (rxBasedOn.exec(sLine)) {
					pData->setBasedOn(rxBasedOn.captured(1));
					break;
				}
				//match = rxData.match(sLine);
				if (rxData.exec(sLine)) {
					(*pData)[std::atoi(rxData.captured(1).c_str())] = rxData.captured(2);
				} else {
				//	qWarning("%s(%d): %s: Unknown .NRPN Names entry.",
				//		sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
				}
				break;
			}

			case InstrDefs: {
				//match = rxTitle.match(sLine);
				if (rxTitle.exec(sLine)) {
					// New instrument definition...
					const std::string& sTitle = rxTitle.captured(1);
					pInstrument = &((*this)[sTitle]);
					pInstrument->setInstrumentName(sTitle);
					break;
				}
				if (pInstrument == 0) {
					// New instrument definition (use filename as default)
					const std::string& sTitle = "the filename"; // QFileInfo(sFilename).completeBaseName();
					pInstrument = &((*this)[sTitle]);
					pInstrument->setInstrumentName(sTitle);
				}
				//match = rxBankSel.match(sLine);
				if (rxBankSel.exec(sLine)) {
					pInstrument->setBankSelMethod(
						std::atoi (rxBankSel.captured(1).c_str()));
					break;
				}
				//match = rxUseNotes.match(sLine);
				if (rxUseNotes.exec(sLine)) {
					pInstrument->setUsesNotesAsControllers(
						bool(std::atoi(rxUseNotes.captured(1).c_str())));
					break;
				}
				//match = rxPatch.match(sLine);
				if (rxPatch.exec(sLine)) {
					const std::string& cap1 = rxPatch.captured(1);
					const int iBank = (cap1 == sAsterisk ? -1 : std::atoi(cap1.c_str()));
					pInstrument->setPatch(iBank, m_patches[rxPatch.captured(2)]);
					break;
				}
				//match = rxControl.match(sLine);
				if (rxControl.exec(sLine)) {
					pInstrument->setControllers(m_controllers[rxControl.captured(1)]);
					break;
				}
				//match = rxRpn.match(sLine);
				if (rxRpn.exec(sLine)) {
					pInstrument->setRpns(m_rpns[rxRpn.captured(1)]);
					break;
				}
				//match = rxNrpn.match(sLine);
				if (rxNrpn.exec(sLine)) {
					pInstrument->setNrpns(m_nrpns[rxNrpn.captured(1)]);
					break;
				}
				//match = rxKey.match(sLine);
				if (rxKey.exec(sLine)) {
					const std::string& cap1 = rxKey.captured(1);
					const std::string& cap2 = rxKey.captured(2);
					const int iBank = (cap1 == sAsterisk ? -1 : std::atoi(cap1.c_str()));
					const int iProg = (cap2 == sAsterisk ? -1 : std::atoi(cap2.c_str()));
					pInstrument->setNotes(iBank, iProg,	m_notes[rxKey.captured(3)]);
					break;
				}
				//match = rxDrum.match(sLine);
				if (rxDrum.exec(sLine)) {
					const std::string& cap1 = rxDrum.captured(1);
					const std::string& cap2 = rxDrum.captured(2);
					const int iBank = (cap1 == sAsterisk ? -1 : std::atoi(cap1.c_str()));
					const int iProg = (cap2 == sAsterisk ? -1 : std::atoi(cap2.c_str()));
					pInstrument->setDrum(iBank, iProg,
						bool(std::atoi( rxDrum.captured(3).c_str())));
				} else {
					//qWarning("%s(%d): %s: Unknown .Instrument Definitions entry.",
					//	sFilename.toUtf8().constData(), iLine, sLine.toUtf8().constData());
				}
				break;
			}

			default:
				break;
		}

	}


	// Ok. We've read it all.
	file.close();

	// We're in business...
	appendFile(sFilename);

	return true;
}



} // namespace Dawe
