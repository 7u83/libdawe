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
#ifndef MIDIEVENTARRAYLIST_H
#define MIDIEVENTARRAYLIST_H

#include <cstdio>
#include <list>
#include <vector>
#include <algorithm>


#include "MidiList.h"

namespace Dawe {

///
/// \brief The MidiArrayEventList class
///
class MidiEventArrayList : public MidiList
{


public:
	MidiEventArrayList(){
		//		list = midieventlist_T();
		array = midieventarray_T();
	}


	/**
	 * @brief empty
	 * @return
	 */
	inline bool empty() const {return list.empty();}


	virtual iterator insert(MidiEvent &e) {

		printf("Insert one array element"); e.dump();


		//		if (!is_indexed)
		//			buildIndex();
		// bool found = std::binary_search(v.begin(), v.end(), -3, MyComparator());
		midieventarray_T::iterator it;
		it = std::lower_bound(array.begin(),array.end(),e.getTime(),TimeComperator());
		/*if (it!=array.begin())
			--it;*/
		//midieventlist_T::iterator it0=list.end();
		iterator li;
		if (it!=array.end()){

			li=MidiList::insert(*it,e);
			//			int d=std::distance(array.begin(),it);
			//	fprintf(stderr,"INSERTER LIBDAWE %d - %d\n",d,e.getTime());

			array.insert(it,li);
		}
		else{
			//midieventlist_T::iterator li;
			list.push_back(e);
			li=end();
			--li;
			array.push_back(li);
		}
		return li;

		//is_indexed=false;
	}

	virtual iterator append(MidiEvent &e){
		iterator li;
		list.push_back(e);
		li=end();
		--li;
		array.push_back(li);
		return li;
	}

	virtual void updateAtIndex(int index,MidiEvent &e){
		MidiEvent *old = &(*array.at(index));
		if (e.getTime()==old->getTime()){
			*old=e;
		}
	}

	///
	/// \brief getIndexForTick
	/// \param t
	/// \return
	///
	int getIndexForTick(nmiditicks_T t){
		midieventarray_T::iterator it;
		it = std::lower_bound(array.begin(),array.end(),t,TimeComperator());
		return it-array.begin();
	}

	/*	void erase(MidiEventList::iterator it,int index){
		MidiEventList::erase(it);
		array.erase(array.begin()+index);
	}
*/

	virtual void deleteAtIndex(int index){
		iterator it = array[index];
		erase(it);
		array.erase(array.begin()+index);
	}

	/*	void insert(MidiEvent &event){
		midieventarray_T::iterator it;
		it = std::lower_bound(array.begin(),array.end(),t,TimeComperator());
		list.insert(it,event);
	}
*/

	/*void insertAtIndex(int index, MidiEvent &e){
		midieventlist_T::iterator it;
		if(array.empty())
			it=list.begin();
		else
			it=array[index];

		list.insert(it,e);
	}
*/



	const MidiEvent * getAtIndex(int n) const{
		/*if (!is_indexed)
			buildIndex();
*/
		return &(*array.at(n));
	}

	///
	/// \brief Get the size of the list
	/// \return size of list in elements
	///
	size_t size() const{
		return list.size();
	}
private:

	typedef std::vector<MidiList::iterator> midieventarray_T;

	struct TimeComperator {
		bool operator()(MidiList::iterator & a, nmiditicks_T time ) const {
			// compare absolute values
			return (*a).getTime() < time;
		}
	};



	/*	volatile mutable bool is_indexed;
	void buildIndex() const{
		midieventlist_T::iterator it;
		array.clear();
		for (it=list.begin(); it!=list.end(); ++it){
			array.push_back(it);
		}
		is_indexed=true;
	}
*/
protected:
	mutable midieventarray_T array;
};

}

#endif // MIDIEVENTARRAYLIST_H
