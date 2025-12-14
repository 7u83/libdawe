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
#ifndef MIDIINSTRUMENT_H
#define MIDIINSTRUMENT_H

#include <vector>
#include <map>
#include <string>


namespace Dawe {

class MidiInstrument
{
public:
	MidiInstrument();
};


class InstrumentData
{
public:

	typedef std::map<int, std::string> DataMap;

	// Constructor.
	InstrumentData()
		: m_pData(new DataRef()) {}
	// Copy constructor.
	InstrumentData(const InstrumentData& data)
		{ attach(data); }

	// Destructor.
	~InstrumentData()
		{ detach(); }

	// Assignment operator.
	InstrumentData& operator= (const InstrumentData& data)
	{
		if (m_pData != data.m_pData) {
			detach();
			attach(data);
		}
		return *this;
	}

	// Accessor operator.
	std::string& operator[] (int iIndex) const
		{ return m_pData->map[iIndex]; }

	// Property accessors.
	void setName(const std::string& sName)
		{ m_pData->name = sName; }
	const std::string& name() const { return m_pData->name; }

	void setBasedOn(const std::string& sBasedOn)
		{ m_pData->basedOn = sBasedOn; }
	const std::string& basedOn() const { return m_pData->basedOn; }

	// Indirect iterator stuff.
	typedef DataMap::iterator iterator;
	iterator begin() { return m_pData->map.begin(); }
	iterator end()   { return m_pData->map.end(); }

	typedef DataMap::const_iterator const_iterator;
	const_iterator constBegin() const { return m_pData->map.begin(); }
	const_iterator constEnd()   const { return m_pData->map.end(); }

	unsigned int count() const { return m_pData->map.size(); }

	bool contains(int iKey) const
		{ return m_pData->map.find(iKey) != m_pData->map.end(); }

protected:

	// Copy/clone method.
	void attach(const InstrumentData& data)
		{  m_pData = data.m_pData; ++(m_pData->refCount); }

	// Destroy method.
	void detach()
		{ if (--(m_pData->refCount) == 0) delete m_pData; }

private:

	// The ref-counted data.
	struct DataRef
	{
		// Default payload constructor.
		DataRef() : refCount(1) {};
		// Payload members.
		int     refCount;
		std::string name;
		std::string basedOn;
		DataMap map;

	} *m_pData;
};




class InstrumentDataList
	: public std::map<std::string, InstrumentData> {};

class InstrumentPatches
	: public std::map<int, InstrumentData> {};

class InstrumentNotes
	: public std::map<int, InstrumentData> {};

class InstrumentKeys
	: public std::map<int, InstrumentNotes> {};

class InstrumentDrumFlags
	: public std::map<int, int> {};

class InstrumentDrums
	: public std::map<int, InstrumentDrumFlags> {};

class Instrument
{
public:

	// Constructor.
	Instrument()
		: m_pData(new DataRef()) {}
	// Copy constructor.
	Instrument(const Instrument& instr)
		{ attach(instr); }

	// Destructor.
	~Instrument()
		{ detach(); }

	// Assignment operator.
	Instrument& operator= (const Instrument& instr)
	{
		if (m_pData != instr.m_pData) {
			detach();
			attach(instr);
		}
		return *this;
	}

	// Instrument title property accessors.
	void setInstrumentName(const std::string& sInstrumentName)
		{ m_pData->instrumentName = sInstrumentName; }
	const std::string& instrumentName() const
		{ return m_pData->instrumentName; }

	// BankSelMethod accessors.
	void setBankSelMethod(int iBankSelMethod)
		{ m_pData->bankSelMethod = iBankSelMethod; }
	int bankSelMethod() const { return m_pData->bankSelMethod; }

	void setUsesNotesAsControllers(bool bUsesNotesAsControllers)
		{ m_pData->usesNotesAsControllers = bUsesNotesAsControllers; }
	bool usesNotesAsControllers() const
		{ return m_pData->usesNotesAsControllers; }

	// Patch banks accessors.
	const InstrumentPatches& patches() const
		{ return m_pData->patches; }

	///
	/// \brief Retrieve patch/program list for given bank address.
	/// \param iBank
	/// \return InstrumentData
	///
	const InstrumentData& patch(int iBank) const;
	void setPatch(int iBank, const InstrumentData& patch)
		{ m_pData->patches[iBank] = patch; }

	void setBankName(int iBank, const std::string& sBankName)
		{ m_pData->patches[iBank].setName(sBankName); }
	const std::string& bankName(int iBank) const
		{ return m_pData->patches[iBank].name(); }

	void setProgName(int iBank, int iProg, const std::string& sProgName)
		{ m_pData->patches[iBank][iProg] = sProgName; }
	const std::string& progName(int iBank, int iProg) const
		{ return m_pData->patches[iBank][iProg]; }

	// Control names accessors.
	void setControllersName(const std::string& sControllersName)
		{ m_pData->controllers.setName(sControllersName); }
	const std::string& controllersName() const
		{ return m_pData->controllers.name(); }
	void setControllers(const InstrumentData& controllers)
		{ m_pData->controllers = controllers; }
	const InstrumentData& controllers() const
		{ return m_pData->controllers; }

	// RPN names accessors.
	void setRpnsName(const std::string& sRpnName)
		{ m_pData->rpns.setName(sRpnName); }
	const std::string& rpnsName() const
		{ return m_pData->rpns.name(); }
	void setRpns(const InstrumentData& rpns)
		{ m_pData->rpns = rpns; }
	const InstrumentData& rpns() const
		{ return m_pData->rpns; }

	// NRPN names accessors.
	void setNrpnsName(const std::string& sNrpnName)
		{ m_pData->nrpns.setName(sNrpnName); }
	const std::string& nrpnsName() const
		{ return m_pData->nrpns.name(); }
	void setNrpns(const InstrumentData& nrpns)
		{ m_pData->nrpns = nrpns; }
	const InstrumentData& nrpns() const
		{ return m_pData->nrpns; }

	///
	/// \brief Retrieve key/notes list for given (bank, prog) pair.
	/// \param iBank
	/// \param iProg
	/// \return Instrument data
	///
	InstrumentData& notes(int iBank, int iProg) const;
	void setNotes(int iBank, int iProg, const InstrumentData& notes)
		{ m_pData->keys[iBank][iProg] = notes; }

	const InstrumentKeys& keys() const
		{ return m_pData->keys; }


	///
	/// \brief Check if given (bank, prog) pair is a drum patch.
	/// \param bank
	/// \param prog
	/// \return
	///
	bool isDrum(int bank, int prog) const;


	void setDrum(int iBank, int iProg, bool bDrum)
		{ m_pData->drums[iBank][iProg] = (int) bDrum; }
	const InstrumentDrums& drums() const
		{ return m_pData->drums; }

protected:

	// Copy/clone method.
	void attach(const Instrument& instr)
		{  m_pData = instr.m_pData; ++(m_pData->refCount); }

	// Destroy method.
	void detach()
		{ if (--(m_pData->refCount) == 0) delete m_pData; }

private:

	// The ref-counted data.
	struct DataRef
	{
		// Default payload constructor.
		DataRef() : refCount(1),
			bankSelMethod(0), usesNotesAsControllers(false) {};
		// Payload members.
		int                       refCount;
		int                       bankSelMethod;
		bool                      usesNotesAsControllers;
		std::string                   instrumentName;
		InstrumentPatches patches;
		InstrumentData    controllers;
		InstrumentData    rpns;
		InstrumentData    nrpns;
		InstrumentKeys    keys;
		InstrumentDrums   drums;

	} *m_pData;
};


///
/// \brief A Cakewalk .ins file container class.
///
class InstrumentList : public std::map<std::string, Instrument>
{
public:

	// Open file methods.
	bool load(const std::string& sFilename);
	bool save(const std::string& sFilename) const;

	// The official loaded file list.
	const std::vector<std::string> & files() const;

	// Manage a file list (out of sync)
	void appendFile(const std::string& sFilename)
		{ m_files.push_back(sFilename); }

//	void removeFile(const std::string& sFilename)
//	{
//		const int iFile = m_files.indexOf(sFilename);
//		if (iFile >= 0)
//			m_files.removeAt(iFile);
//	}

	// Patch Names definition accessors.
	const InstrumentDataList& patches() const
		{ return m_patches; }
	const InstrumentData& patches(const std::string& sName)
		{ return m_patches[sName]; }

	// Note Names definition accessors.
	const InstrumentDataList& notes() const
		{ return m_notes; }
	const InstrumentData& notes(const std::string& sName)
		{ return m_notes[sName]; }

	// Controller Names definition accessors.
	const InstrumentDataList& controllers() const
		{ return m_controllers; }
	const InstrumentData& controllers(const std::string& sName)
		{ return m_controllers[sName]; }

	// RPN Names definition accessors.
	const InstrumentDataList& rpns() const
		{ return m_rpns; }
	const InstrumentData& rpns(const std::string& sName)
		{ return m_rpns[sName]; }

	// NRPN Names definition accessors.
	const InstrumentDataList& nrpns() const
		{ return m_nrpns; }
	const InstrumentData& nrpns(const std::string& sName)
		{ return m_nrpns[sName]; }

	// Clear all contents.
	void clearAll();

	// Special instrument list merge method.
	void merge(const InstrumentList& instruments);

	// Special MIDINameDocument loader.
//	bool loadMidiNameDocument(const QDomDocument& doc);

protected:

	// Internal instrument data list save method helpers.
//	void saveDataList(QTextStream& ts, const qtractorInstrumentDataList& list) const;
//	void saveData(QTextStream& ts, const qtractorInstrumentData& data) const;

	// Special instrument data list merge method.
	void mergeDataList(InstrumentDataList& dst,
		const InstrumentDataList& src);

	// Special SoundFont loader.
//	bool loadSoundFont(QFile *pFile);
//	void loadSoundFontPresets(QFile *pFile, int iSize);

	// Special MIDINameDocument loaders.
/*	void loadMidiDeviceNames(
		QDomElement *pElement);
	void loadMidiChannelNameSet(
		QDomElement *pElement, qtractorInstrument& instr);
	void loadMidiPatchBank(
		QDomElement *pElement, qtractorInstrument& instr, const std::string& sName);
	void loadMidiPatchNameList(
		QDomElement *pElement, qtractorInstrument& instr, const std::string& sName);
	void loadMidiNoteNameList(
		QDomElement *pElement, const std::string& sName);
	void loadMidiControlNameList(
		QDomElement *pElement, const std::string& sName);
*/
private:

	// To hold the names definition lists.
	InstrumentDataList m_patches;
	InstrumentDataList m_notes;
	InstrumentDataList m_controllers;
	InstrumentDataList m_rpns;
	InstrumentDataList m_nrpns;

	// To old the official file list.
//	std::stringList m_files;
	std::vector<std::string> m_files;

};





}


#endif // MIDIINSTRUMENT_H
