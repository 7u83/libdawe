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
#ifndef VLIST_H
#define VLIST_H

#include <cstddef>
#include <list>

/**
 * @file
 *
 * @brief Header and Implementation for VList object
 */

namespace Dawe
{

/**
 * @brief The VList class
 */
template <typename KeyType, typename ValType>
class VList
{
public:
//	typedef typename std::list<ValType>::iterator iterator;
	VList()	{}
	virtual bool empty() const = 0;
	inline virtual size_t size() const = 0;
	virtual ValType * getValPtrAtIndex(int index) const=0;
	virtual void deleteAtIndex(int index) =0;
	virtual void insert(ValType e)=0;
};

}

#endif // VLIST_H
