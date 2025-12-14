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
#ifndef LIST_H
#define LIST_H

#include <list>
#include <vector>
#include <map>

#include <iterator>
#include  <iostream>
#include <cstdio>
#include <algorithm>

#include "Accessor.h"
#include "../VList.h"
namespace Dawe {

template <typename T>
struct ListDefaultComperator {
	bool operator()(T & a, T & b){
		return a<b;
	}
};


template <typename KeyType, typename ValType>
struct DefaultValKey{
	KeyType operator()(ValType & a) const{
		(void)a;
		return 0;
	}
};

template <typename KeyType, typename ValType,
	  typename Comperator=ListDefaultComperator<ValType>,
	  typename ValKey=DefaultValKey<KeyType,ValType> >
//typename Comperator=ListDefaultComperator<ValType> >
class List
{

public:
	typedef typename std::list<ValType>::iterator iterator;

	class Index {
	public:

		typedef typename std::vector< typename List<KeyType,ValType>::iterator >::iterator iterator;
		Index(List<KeyType,ValType> &list):
			list(list)
		{

			rebuild();
		}

		//std::vector< typename List<KeyType,ValType>::iterator >::iterator
		Index::iterator
		lower_bound (ValType & e){

			//return iterator();
			//iterator i;
			std::lower_bound(array.begin(),array.end(),e,Comperator());

		}

		void rebuild(){
			array.clear();
			typename List<KeyType,ValType>::iterator it;
			for (it=list.begin(); it!=list.end();++it){
				array.push_back(it);
			}
		}
	private:
		List<KeyType,ValType> & list;
		std::vector< typename List<KeyType,ValType>::iterator > array;

		//std::map<int, typename List<KeyType,ValType>::iterator > array;

	};

	List(bool indexed=false){
		list = std::list<ValType>();
		if (indexed)
			//		indexes[0]=new Index(*this);
			indexes.push_back(new Index(*this));

	}

	~List(){

	}

	/*void insert2 (ValType &e) {
		if (indexes.size()>0){
			Index * index = indexes[0];
			//index->lower_bound(e);
		}
	}*/

	/*	virtual iterator insert(ValType &e) {


		midieventarray_T::iterator it;
		it = std::lower_bound(array.begin(),array.end(),e.getTime(),TimeComperator());
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
*/


	class accessor : public Accessor<KeyType,ValType>{
	public:
		accessor(List<KeyType,ValType> & l):list(l){
			it=list.list.begin();
		}
		ValType *getValPtr() const {
			if (isAtEnd())
				return 0;
			return &(*it);
		}
		KeyType getKey() const{
			return /*ValKey()*/(*it).getKey();
		}
		inline bool isAtEnd() const {
			return it==list.end();
		}
		inline bool isAtBegin() const {return it==list.begin();}
		void moveForward(){if (!isAtEnd()) ++it;}
		void moveBackward(){if (!isAtBegin())--it;}
	private:
		List<KeyType,ValType> & list;
		typename List<KeyType,ValType>::iterator it;
	};

	virtual accessor getAccessor(){return accessor(*this);}
	accessor * createAccessor(){return new accessor(*this);}

	inline virtual iterator begin() const{
		return list.begin();
	}

	inline virtual iterator end() const{
		return list.end();
	}

	inline bool empty() const {
		return list.empty();
	}

	inline virtual void sort(){
		list.sort();
	}

	inline virtual ValType getValAtIndex(int index){
		iterator it = begin();
		std::advance(it,index);
		return *it;
	}

	inline virtual void push_back(const ValType & val){
		list.push_back(val);
	}
protected:
	mutable std::list<ValType> list;
	//	mutable std::vector< List<KeyType,ValType> *> indexes;

	mutable std::vector< Index *> indexes;

};


template <typename KeyType, typename ValType,
	  typename Comperator=ListDefaultComperator<ValType>,
	  typename ValKey=DefaultValKey<KeyType,ValType> >
class XList : public VList<KeyType,ValType>
{

public:
	typedef typename std::list<ValType>::iterator iterator;

	class Index : public VList<KeyType,ValType>{
	public:

		typedef typename std::vector< typename List<KeyType,ValType>::iterator >::iterator iterator;
		Index(XList<KeyType,ValType,Comperator> * list):
			list(list)
		{
			array = std::vector< typename std::list<ValType>::iterator >();
			rebuild();
		}

		//std::vector< typename List<KeyType,ValType>::iterator >::iterator
		Index::iterator	lower_bound (KeyType key) {

			return std::lower_bound(array.begin(),array.end(),key,Comperator()); //,key,Comperator());

		}

		void rebuild(){
			array.clear();
			typename std::list<ValType>::iterator it;
			for (it=list->list.begin(); it!=list->list.end();++it){
				if (filter(*it))
					array.push_back(it);
			}
		}

		inline iterator begin() const {return array.begin();}
		inline iterator end() const {	return array.end();}
		inline virtual size_t size() const {return array.size();}
		inline virtual bool empty() const {return array.empty();}

		friend class XList<KeyType,ValType,Comperator>;
		virtual bool filter(const ValType & e) const{
			(void)e;
			return true;
		}

		void insert(ValType & e){
			list->insert2(e);
		}

		void dump(){
			for (size_t i=0; i<size(); i++){
				printf("IDX[%ld]: ",i);
				(*array[i]).dump();
			}
		}

		inline virtual ValType getValAtIndex(int index){
			return *(array[index]);
		}

		inline virtual ValType * getValPtrAtIndex(int index) const{
			return &(*(array[index]));
		}

		virtual void deleteAtIndex(int index){
			XList::iterator it = array[index];
			list->list.erase(it);
			array.erase(array.begin()+index);
			list->rebuild();
		}

	private:
		XList<KeyType,ValType,Comperator> * list;
		mutable std::vector< typename std::list<ValType>::iterator > array;

		void add (iterator &ai, typename std::list<ValType>::iterator & li){
			if (ai!=end()){
				array.insert(ai,li);
				return;
			}
			array.push_back(li);
		}

		void add (typename std::list<ValType>::iterator & li){
			if (!filter(*li))
				return;
			typename Index::iterator ai = lower_bound(*li);
			add(ai,li);
		}


		// VList interface
	public:
		virtual void insert(ValType e)
		{
			(void)e;
		}
	};

	XList(bool indexed=true){
		list = std::list<ValType>();
		if (indexed)
			//		indexes[0]=new Index(*this);
			indexes.push_back(new Index(this));
		cur_index=-12;

	}

	~XList(){

	}

	Index * getIndex(int n) {
		return indexes[n];
	}

	int addIndex(Index * index){
		indexes.push_back(index);
		return indexes.size()-1;
	}

	void deleteIndex(int n){
		delete indexes[n];
		indexes.erase(indexes.begin() + n);
	}

	void insert2 (ValType &e) {
		if (indexes.size()>0){
			Index * index = indexes[0];
			typename Index::iterator ai = index->lower_bound(e);
			iterator li;
			if (ai!=index->end()){
				li = list.insert(*ai,e);
			}
			else{
				list.push_back(e);
				li=end();
				--li;
			}
			index->add(ai,li);

			for (size_t i=1; i<indexes.size(); i++)
				indexes[i]->add(li);

		}
	}

	/*	virtual iterator insert(ValType &e) {


		midieventarray_T::iterator it;
		it = std::lower_bound(array.begin(),array.end(),e.getTime(),TimeComperator());
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
*/





	inline virtual iterator begin() const{
		return list.begin();
	}

	inline virtual iterator end() const{
		return list.end();
	}

	inline bool empty() const {
		return list.empty();
	}

	inline virtual void sort(){
		list.sort();
	}


	inline virtual size_t size() const
	{
		return list.size();
	}

	inline virtual ValType getValAtIndex(int index){
		iterator it = begin();
		std::advance(it,index);
		return *it;
	}

	inline virtual void push_back(const ValType & val){
		list.push_back(val);
	}
	friend class Index;
protected:
	mutable std::list<ValType> list;
	//	mutable std::vector< List<KeyType,ValType> *> indexes;

	mutable std::vector< Index *> indexes;

public:
	void rebuild(){
		for(size_t i=0; i<indexes.size(); i++)
			indexes[i]->rebuild();
	}

	// VList interface
public:
	virtual ValType *getValPtrAtIndex(int index) const{
		//	if (indexes.size()>0)
		//		return indexes[0]->getValPtrAtIndex(index);

		//		printf("GET VALPTR AT INDEX: %d\n",index);
		//		fflush(stdout);
		if (index==cur_index) {
			return &(*cur_it);
		}
		if (index==cur_index+1){
			cur_index++;
			cur_it++;
			return &(*cur_it);
		}

		if (index==cur_index-1){
			cur_index--;
			cur_it--;
			return &(*cur_it);
		}



		iterator it = begin();
		std::advance(it,index);
		cur_index = index;
		cur_it=it;
		return &(*it);

	}

	mutable int cur_index;
	mutable iterator cur_it;
};




}

#endif // LIST_H
