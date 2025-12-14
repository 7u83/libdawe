#include <iostream>
#include <set>


class my
{
private:
public:
	int n;
	const char *s;
	my(int n,const char *s=NULL){
		this->s=s;
		this->n=n;
	}

	bool operator<(const my& other) const{
		if (n < other.n)
			return true;
		return false;
	}
};

class T
{
public:
	std::string & name;

	T(std::string & name){
		
	}
};

T * make_class()
{
	T * t = new T();
	return t;
}


int main(void)
{
	my x(3), y(4), z(1), a(3);
	std::set<my &> mys;
	mys.insert(x);
	mys.insert(y);
	mys.insert(z);
	mys.insert(a);

	for (const auto& xx : mys) {
        std::cout << xx.n << std::endl;
 	}

}


