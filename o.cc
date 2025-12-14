#include <vector>
#include <cstdio>


int main()
{
	std::vector<char> v;

	printf("S: %d\n",v.size());


	v.push_back('a');
	v.push_back('b');
	v.push_back('c');


/*	char *v = new char[3];
	v[0]='a';
	v[1]='b';
	v[2]='c';
*/

	int sum=0;
	for (int i=0; i<1000000000; i++){
		sum += v[0]+v[1]+v[2];
		v[0]+=sum&0xff;
		v[1]+=3+sum&0xff;
		v[2]=7-v[0];
	}
	printf("SUM %d\n",sum);

/*	for (int i=0; i<100000000; i++){
		v[0]='a';
		v[1]='b';
		v[2]='c';
	}
*/


/*	for (int i=0; i<100000; i++){
		std::vector<char> v1 = v;
	}

*/

}
