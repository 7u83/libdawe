#include <iostream>

class Base {
public:
	virtual void say()=0;
};

class A  {
public:
	class A1 : public Base {
		virtual void say(){
			std::cout << "I AM A" << std::endl;
		}
	};

	typedef A1 Zumsel;
};


class B  {
public:
	class B1 : public Base {
		virtual void say(){
			std::cout << "IAM B1 Tube" << std::endl;
		}
	};
	typedef B1 Zumsel;
};

int main()
{
	Base *b;
	b = new B::Zumsel();
	b->say();
}
