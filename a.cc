
#include <iostream>

class My {
	private:
		long x;
	public:
		void say(){
			std::cout << x << std::endl;

		}

		My( My & c){
			std::cout << "copy constructor called" << std::endl;
			x=c.x;
		}
		My(){

		}
	
};

int main()
{
	My var;
	My var1 = var;
	var.say();

	typedef enum {
		A=3,
		B=4,
		C=5
	} Type;
	char c = 13;

	Type x = (Type)c;

	printf("R: %d\n",x);


}
