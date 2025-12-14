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
#ifndef ACCESSOR_H
#define ACCESSOR_H

#include "Iterator.h"

namespace Dawe {



template <typename KeyType,typename ValType>
class Accessor : public Iterator<KeyType, ValType>
{
public:
	Accessor(){}
	virtual ValType * getValPtr() const =0;
	virtual ValType getVal() const{
		return *getValPtr();
	}
	virtual KeyType getKey() const =0;
	virtual ValType getNextVal(){
		moveForward();
		return getVal();
	}
	virtual ValType * getNextValPtr(){
		moveForward();
		return getValPtr();
	}
	virtual void moveForward()=0;
	virtual void moveBackward()=0;

	virtual bool isAtEnd() const=0;
	virtual bool isAtBegin() const=0;
	//	virtual void seek(nmiditicks_T)=0;
	//	virtual void seekBegin()=0;

	Accessor<KeyType, ValType> operator ++(){
		moveForward();
		return *this;
	}

	Accessor<KeyType, ValType> operator --(){
		moveBackward();
		return *this;
	}

	ValType & operator *(){
		return *getValPtr();
	}

	/*	bool operator != (const Accessor & other){

	}
*/

};

}

#endif // ACCESSOR_H
