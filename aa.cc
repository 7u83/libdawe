
#include <list>
#include <cstdio>

class A {
public:
	virtual void say(){
		printf("Hello from A\n");
	}
};

class B : public A {
public:
	virtual void say(){
		printf("Hello from parent: ");
		A::say();
		printf("Hello from B\n");
	}
};

int main()
{
	A a;
//	a.say();
	B b;
//	b.say();

	A & x = b;
	x.say();
		

}
