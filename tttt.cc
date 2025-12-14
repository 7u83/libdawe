#include <iostream>
#include <list>

struct A {
	virtual void say() { std::cout << "A" << std::endl; }
};

struct B : public A {
	virtual void say() { std::cout << "B" << std::endl; }
};


int main()
{
	A var;
	var = A();
	var.say();
	var = B();
	var.say();

	std::list<double> l;

	for (int i=0; i<5500000; i++){
		l.push_back(i);
	}

	std::list<double>::iterator it;
	double sum=0;
	for (it=l.begin(); it!=l.end(); ++it){
		sum += *it;
	}
	
	printf("Result: %f\n",sum);

	char z[]={1,2,3};
	z = new char[100];

	printf("S: %d\n",sizeof(z));

	return 0;
}
