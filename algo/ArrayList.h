#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <vector>

#include "List.h"

namespace Dawe {


template <typename KeyType, typename ValType>
class ArrayList : public List <KeyType, ValType>
{
public:
	ArrayList()
	{

	}

protected:
	std::vector< typename std::list<ValType>::iterator > array;
};

} // namespace Dawe

#endif // ARRAYLIST_H
