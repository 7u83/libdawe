#include <iostream>
#include <typeinfo>
#include <string>

class D {
public:
	virtual D & fun() = 0;
};

class E : public D {
public:
	virtual E & fun(){
		E * e = new E();

		return *e;
	}
};

class Zumsel {
	public:
		void zf(){
			std::cout << "ZF\n";
		}
};


int main()
{
//	D d = *(new E());

	E * e = new E();
	D & d(*e);

	Zumsel * nase = new Zumsel();
	nase->zf();

	std::string s = typeid(Zumsel).name();

	std::cout << "Typeinfo: " << s << std::endl;	
//#delete &d;
	delete e;

//	E e;
//	E e2;
//	e2 = e.fun();	
}
