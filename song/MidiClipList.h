/*
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
#ifndef MIDICLIPLIST_H
#define MIDICLIPLIST_H

#include <set>

#include "../algo/List.h"
#include "../midi/MidiEvent.h"

#include "MidiClipLink.h"

namespace Dawe {


class MidiClipList : public List<nmiditicks_T,MidiClipLink>{
public:


	void dump(){
		printf("Dump MidiClipList =====\n");
		List<nmiditicks_T,MidiClipLink>::accessor a(*this);
		while(!a.isAtEnd()){
			MidiClipLink *c = a.getValPtr();
			printf("Clip [%d]\n",c->getTime());
			a.moveForward();
		}
	}




	class accessor : public Accessor<nmiditicks_T, MidiEvent>{

		// Accessor interface
	public:
		accessor(const MidiClipList & cliplist):cliplist(cliplist){
			a = 0;
			it=cliplist.begin();
			addClips();
		}
		MidiEvent *getValPtr() const{
			// not implemented
			assert(false);
			return 0;
		}
		MidiEvent getVal() const{
			std::set< Accessor<nmiditicks_T,MidiEvent> *> si;
			return a->getVal();

			return MidiEvent();
		}
		nmiditicks_T getKey() const{
			return a->getKey();
			return 0;
		}
		void moveForward(){
			a->moveForward();
			if (a->isAtEnd()){
				accessors.erase(a);
				a=0;
	/*			if (it==cliplist.end())
					return;
				a=(*it).createAccessor();
				++it;*/
			}
			findA();

			/*	std::set< Accessor<nmiditicks_T,MidiEvent> *>::iterator si;
			for (si=accessors.begin(); si!=accessors.end(); ++si){
				if (a->getKey()>(*si)->getKey()){
					a=(*si);
				}
			}*/

		}
		void moveBackward(){}
		bool isAtEnd() const{
			if (!accessors.empty())
				return false;
			return cliplist.end() == it;
		}
		bool isAtBegin() const{return true;}


		void findA(){
			std::set< Accessor<nmiditicks_T,MidiEvent> *>::iterator si;
			for (si=accessors.begin(); si!=accessors.end(); ++si){
				if (a==0){
					a=(*si);
					continue;
				}
				if (a->getKey()>(*si)->getKey()){
					a=(*si);
				}
			}
			addClips();
		}

		void addClips(){
			if (a==0){
				while (1){
					if (it==cliplist.end())
						return;
					a=(*it).createAccessor();
					accessors.insert(a);
					++it;
					if (!a->isAtEnd())
						break;
				}
			}
			while(it!=cliplist.end()){
				if((*it).getTime()<a->getKey()){
					Accessor<nmiditicks_T,MidiEvent> *a0;
					a0 = (*it).createAccessor();
					if (a0->isAtEnd())
						continue;

					accessors.insert(a0);
					if (a0->getKey()<a->getKey())
						a=a0;
					++it;
				}
				else{
					break;
				}
			}
		}

	private:
		Accessor<nmiditicks_T,MidiEvent> *a;
		std::set< Accessor<nmiditicks_T,MidiEvent> *> accessors;
		MidiClipList::iterator it;
		const MidiClipList & cliplist;


	};

	accessor * createAccessor(){
		return new MidiClipList::accessor(*this);
	}

};




}

#endif // MIDICLIPLIST_H
