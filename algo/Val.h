#ifndef VAL_H
#define VAL_H


#include "../dawe.h"

#include <string>
#include <vector>

namespace Dawe {
class Val
{
public:
	typedef enum {
		UNDEF,
		UINT8,
		UINT16,
		UINT32,
		STR,
		VEC,
		nmiditicks_T
	} Type;

	Val(){
		type=UNDEF;
	}

	Val(std::string & str){
		type=STR;
		val.str=new std::string(str);
	}

	Val(const char * str){
		type=STR;
		val.str=new std::string(str);
	}

	Val(uint8_t v){
		type = UINT8;
		val.uint8=v;
	}

	const char * toCstr() const{
		switch(type){
		case STR:
			return val.str->c_str();
		default:
			return 0;
		}
	}

	Val & operator =(const char *str){
		clear();
		type=STR;
		val.str = new std::string(str);
		return *this;
	}



	~Val(){
		clear();
	}
private:
	Type type;
	union{
		uint8_t uint8;
		uint16_t uint16;
		std::string * str;
		std::vector<uint8_t> * vec;
	}val;

	void clear(){
		switch(type){
		case STR:
			delete val.str;
			break;
		case VEC:
			delete val.vec;
			break;
		default:
			return;
		}
	}

};

} // namespace Dawe

#endif // VAL_H
