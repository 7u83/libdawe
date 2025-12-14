/*
 * Copyright (C) 2021  7u83 <7u83@mail.ru>
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

#include <iostream>

#include "Track.h"

namespace Dawe {

Track::Track()
{

}

void Track::remove(Clip *clip)
{
	nmiditicks_T pos = reverse[clip];
	nmiditicks_T pos2 = pos+clip->getDuration();

	clipmap_T::iterator it = clips.find(pos);
	while(1) {
		nmiditicks_T lpos = it->first;
		(&it->second)->erase(clip);
		(&it->second)->refs--;
		if ((&it->second)->refs==0){
			clips.erase(it++);
		}else{
			++it;
		}
		if (lpos==pos2)
			break;
	}
	return;
}




void Track::insert(miditick_T pos, Clip *clip)
{
	// if the clip is already inserted, do nothing.
	if (reverse.find(clip)!=reverse.end())
		return;

	miditick_T pos2 = pos+clip->getDuration();


	// Special case, the list is empty,
	// insert the very first element
	if (clips.empty()){
		(&clips[pos])->insert(clip);
		clips[pos2]=ClipMapElem(1);
		reverse[clip]=pos;
		return;
	}


	// There is at least one element in the list

	// find the element, that would be the element
	// befor our insert position
	clipmap_T::iterator it0;
	it0 = clips.lower_bound(pos);
	if (it0!=clips.begin()){
		if (it0->first != pos)
			--it0;
	}

	if (it0->first >= pos){
		// There is no element before our insert
		// position, but maybe an element exact at our
		// insert position. So insert our clip here,
		// creating a new element or updating the alement
		// at our insert position.
		(&clips[pos])->insert(clip);
		it0=clips.find(pos);
	}
	else{
		// Tehre is an element befor our insert position.
		// Create a copy of this element.
		ClipMapElem nc(it0->second);

		// Add our element to the copy and insert the
		// copy at our isertposition, which creates for sure
		// a new entry in the map.
		nc.insert(clip);
		clips[pos]=nc;
		it0 = clips.find(pos);
	}


	// Now update elements following our insert position.

	do{
		//		std::set<Clip*> *last=&it0->second;

		// remember the current element for later use;
		ClipMapElem *last = &it0->second;
		++it0;

		if (it0==clips.end()){
			// if we are at the end of the list
			// put an empty element at our end position
			// in it.
			ClipMapElem nc(1);
			clips[pos2]=nc;
			break;
		}


		if (it0->first<pos2){
			(&it0->second)->insert(clip);
		} else if (it0->first==pos2){
			(&it0->second)->refs++;
			break;
		} else {
			//std::set<Clip*> nc(*last);
			ClipMapElem nc(*last);
			nc.erase(clip);
			clips[pos2]=nc;
			break;
		}
	}while(1);

	reverse[clip]=pos;

}


void Track::dump()
{
	std::cout << "----------------" << std::endl;
	//std::cout <<"CLIPS:" << clips.size() << std::endl;
	for (clipmap_T::iterator it=clips.begin(); it != clips.end(); ++it){
		std::set<Clip *>cl=it->second.clips;
		std::cout << "REFS:" << it->second.refs << std::endl;

		//	std::cout << "MARKER:" << cl.size() << std::endl;
		if (cl.empty()){
			std::cout << it->first << ":" << "<empty>" << std::endl;
		}
		else {



			for (std::set<Clip *>::iterator it2=cl.begin(); it2!=cl.end(); ++it2){
				//	std::cout << (*it2)->getName() << std::endl;

				std::cout << it->first << ":" << (*it2)->getName() << std::endl;
			}
		}
	}
}

/*
MidiEvent *Track::accessor::getValPtr() const {
	nmiditicks_T clstart = it->first;
	if (clstart >= current){


		std::set<Clip*>::iterator cli;
		ClipMapElem el = it->second;
		for (cli=el.clips.begin(); cli!=el.clips.end(); ++cli ){
			MidiClip * cc=(MidiClip*)(*cli);
			MidiIndex *tsi = cc->getTimeSigIndex();
			MidiIndex::Accessor * tsa = tsi->createAccessor();


		}

	}




	MidiEvent *e=0;
	//ClipMapElem el = *it;

	//			for (cli=el.clips.begin(); cli!=el.clips.end(); cli++ ){

	//			}

	return 0;
}
*/


/*

void Track::accessor::moveForward()
{
	//Accessor<nmiditicks_T,MidiEvent> *a;
	const CurrentClipsElem *a;
	while(1) {
		Clip *key;
		a = getFirstAccessor(&key);
		if (a==0)
			break;
		a->accessor->moveForward();
		if (a->accessor->isAtEnd()){
			printf("ERASE: %p\n",(void*)key);
			current_clips.erase(key);
			continue;
		}
		a= getFirstAccessor();
		break;
	}

	if (a==0){
		if( clipmap_it == track.clips.end())
			return;
		++clipmap_it;
		copyAccessors();
		return;
	}

	if (a->accessor->getKey()+a->offset>=clipmap_it->first){
		++clipmap_it;
		if( clipmap_it == track.clips.end())
			return;
		copyAccessors();
	}

}

MidiEvent *Track::accessor::getValPtr() const{
	//Accessor<nmiditicks_T,MidiEvent> *a = getFirstAccessor();
	const CurrentClipsElem *a = getFirstAccessor();

	if (a==0)
		return 0;
	printf("Offset here is: %d\n",a->offset);
	return a->accessor->getValPtr();

}

bool Track::accessor::isAtEnd() const
{
	//Accessor<nmiditicks_T,MidiEvent> *a = getFirstAccessor();
	const CurrentClipsElem *a = getFirstAccessor();
	if (a==0)
		return true;
	return false;
}


//Accessor<nmiditicks_T, MidiEvent> *
//Track::accessor::getFirstAccessor(Clip **key) const


const Track::accessor::CurrentClipsElem *
Track::accessor::getFirstAccessor(Clip **key) const
{
	if (current_clips.empty())
		return 0;
	std::map<Clip *,CurrentClipsElem>::const_iterator i;
	i=current_clips.begin();
	//Accessor<nmiditicks_T,MidiEvent> *ba = i->second.accessor;
	const CurrentClipsElem * ba = &(i->second);
	if (key!=0)
		*key=i->first;
	++i;
	while (i!=current_clips.end()){

		//Accessor<nmiditicks_T,MidiEvent> *a = i->second.accessor;
		const CurrentClipsElem *a = &(i->second);
		if (ba->accessor->getKey()+ba->offset>a->accessor->getKey()+ba->offset){
			ba=a;
			if (key!=0)
				*key=i->first;
		}

		++i;
	}
	//return ba->accessor;
	return ba;
}


void Track::accessor::updateCurrent(){
}

MidiEvent * Track::accessor::getwat(){
	std::map<Clip *,CurrentClipsElem>::iterator i;

	Accessor<nmiditicks_T,MidiEvent> * ba=0;
	for(i=current_clips.begin(); i!=current_clips.end(); ++i){
		//		MidiClip *c = dynamic_cast<MidiClip*>(i->first);
		Accessor<nmiditicks_T,MidiEvent> *a = i->second.accessor;
		if(ba==0){
			ba=a;
			continue;
		}
		if (ba->getKey()<a->getKey())
			continue;

		ba=a;
		//		MidiClip *c = dynamic_cast<MidiClip*>(i->first);
		//		nmiditicks_T t =  c->getTimeSigIndex()->getAccessor().getKey();
	}
	if (clipmap_it == track.clips.end()){
		if (ba!=0){
			//MidiEvent * e;
			ba->moveForward();
		}
		return 0;
	}

	if (ba!=0)
		if (ba->getKey()<clipmap_it->first){
			return 0;
		}
	return 0;
}

*/

/*
void Track::accessor::copyAccessors()
{
	if (clipmap_it == track.clips.end())
		return;

	ClipMapElem &e = clipmap_it->second;
	std::set<Clip*>::iterator i = e.clips.begin();
	while(i!=e.clips.end()){
		MidiClip *c = dynamic_cast<MidiClip*>(*i);
		printf("C: %p\n",(void*)c);
		if (current_clips.find(c)==current_clips.end()){
			printf("Not found. Adding %p\n",(void*)c);
			CurrentClipsElem n;
			n.accessor = c->getTimeSigIndex()->createAccessor();
			n.offset = clipmap_it->first;
			current_clips[*i]=n;
		}
		++i;
	}


}
*/

MidiEvent *Track::getNext()
{
//	accessor a = accessor(*this);
//	a.updateCurrent();




    assert(false);
//	exit(0);
	clipmap_T::iterator it;
	for(it=clips.begin(); it!=clips.end(); ++it){
		printf("TIME: %d\n",it->first);
		ClipMapElem *e = &(it->second);
		std::set<Clip*>::iterator it1;
		for (it1=e->clips.begin(); it1!=e->clips.end(); ++it1){
			Clip *c = *it1;
			printf("CLIP::: %p\n",(void*)c);
		}
	}
}
#include <set>
struct PointerLess {
	template<typename T>
	bool operator()(const T* a, const T* b) const {
		return *a < *b;
	}
};


void Track::init()
{
	// Example usage
	std::set<int*, PointerLess> mySet;




	std::set<int *> asd;

	clipmap_iterator = clips.begin();
}

const std::string &Track::getName() const
{
	return name;
}

void Track::setName(const std::string &newName)
{
	name = newName;
}

} // namespace Dawe
