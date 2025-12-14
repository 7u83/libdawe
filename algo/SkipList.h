#ifndef SKIPLIST3_H
#define SKIPLIST3_H

#include <list>
#include <vector>
#include <iostream>

namespace Dawe
{


static bool rndlist3[] = {
	false,
	false,
	false,
	false,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
};
static int rndptr3 =0;

template <typename T>
struct SkipListDefaultComperator {
	inline bool operator()(const T & a, const T & b){
		return a<b;
	}
};

template <typename ValType,
	  typename KeyType=double,
	  typename Comperator=SkipListDefaultComperator<ValType>
	  >
class SkipList
{
private:
	Comperator cmp;

	struct Node{
		Node(typename std::list<ValType>::iterator it)
			:val(it){
			left=right=down=0;
			skip=0;
		}

		typename std::list<ValType>::iterator val;
		Node *left, *right, *down;
		int skip;
	};
public:
	mutable std::list<ValType> list;

	unsigned int seed;
	inline bool rand(){
		int rval;

		//	rval = rndlist3[rndptr3++];
		//	return rval;

#define RANDOM 1
#if RANDOM==1
		seed = seed * 1103515245 + 12345;
		rval = (unsigned)(seed/65536) % 32768;
#elif RANDOM==2
		rval = rand_r(&seed);
#endif

		if (rval > 32768*7/16)
			return true;

		return false;

		return (rval & 8)  ? true : false;
	}

	Node *root;

	Node * insert0( Node * node, const ValType & val, bool *rnd, int *skip, int pos, int index,
			typename std::list<ValType>::iterator & replace){
		Node * n = node;
		int skip_count =0;
		int skips=0;


		while (n->right!=0){
			if (cmp(val,*(n->right->val)))
				//if (val < *(n->right->val))
				break;
			n=n->right;
			skip_count+=n->skip;
		}
		if (n->right)
			n->right->skip++;


		typename std::list<ValType>::iterator it,it0,itc, itn, itfirst;
		Node *nn;

		if (n->down!=0){
			nn = insert0(n->down,val,rnd,&skips,pos+skip_count,index,replace);
			if (nn)
				itn = nn->val;
		}
		else{

			if (n->left==0){
				it = list.begin();
				it0=list.end();
			}
			else {
				it = n->val;
				it0=it;
			}

			itfirst=list.end();
			int p=0;
			while(it!=list.end()){

				if (cmp(val,*it))
					break;

				//	std::cout << "P:" << pos+skip_count+p << " == " << index << std::endl;
				if (pos+skip_count+p==index)
					break;

				if (itfirst==list.end()){
					if (equal(val,*it))
						itfirst=it;
					else
						skips++;
				}
			/*	else{
					skips++;
				}*/
				++p;
				++it;
			}
			itn = list.insert(it,val);

			if (p==0 && equal(val,*it)){
				std::cout << "attention" << std::endl;
				replace = n->val;
				*rnd=false;
			}
			else{


				if (itfirst!=list.end())
					itn=itfirst;

				//			std::cout << "Position " << pos+skip_count << ""
				//				  << " is " << pos+skip_count+skips << std::endl;



				//	std::cout << "ITC " << *it << std::endl;

				nn=0;
				if (itn==it0) // && !(val<*it) && !(*it<val))
					*rnd=false;
				else
					*rnd=rand();
			}
			//*rnd=rand();
		}

		*skip = skip_count + skips;

		if (*rnd){
			Node *in = new Node(itn);
			in->down=nn;
			in->skip=skips;

			in->left=n;
			in->right=n->right;
			n->right=in;

			if (in->right){
				in->right->left=in;
				in->right->skip-=skips;
			}

			*rnd=rand();
			return in;
		}
		else {
			if (replace==n->val && replace != list.end()){
				std::cout << "replace" << std::endl;
				n->val=itn;
				return node;
			}
		}


		if (replace != list.end()){
			if (n->left==0)
				std::cout << "replace nil" << std::endl;
			else
				std::cout << "REPLACE=" << *replace << " With " << *(n->val) << std::endl;

		}



		return 0;
	}

public:
	/**
	 * @brief iterator
	 */
	typedef typename std::list<ValType>::iterator iterator;

	SkipList(int seed=3)
		:seed(seed)
	{
		root=new Node(list.end());
	}

	~SkipList(){
		deleteAll();
	}

	void deleteAll(){
		Node *head = root;
		Node *next;
		do {
			Node *n = head->right;
			while(n){
				next = n->right;
				delete n;
				n=next;
			}
			next=head->down;
			delete head;
			head=next;
		} while (head!=0);
	}

	void clear(){
		deleteAll();
		list.clear();
		root=new Node(list.end());
	}

	void insert (const ValType & val, int index=-1){

		//	std::cout << "IIINNNSERT:" << val << std::endl;

		//int level = v.size()-1;
		bool rnd=false;
		int skip=0;
		typename std::list<ValType>::iterator replace = list.end();

		Node *nn = insert0(root,val,&rnd,&skip,0,index, replace);

		while(rnd){
			Node *n0 = new Node(list.end());
			n0->down=root;
			root=n0;

			Node * n = new Node(nn->val);
			n->down=nn;
			n->skip=skip;
			n->left=n0;
			n0->right=n;
			nn=n;
			rnd = rand();
		}

	}

	iterator lower_bound(const ValType & val) const{
		Node *n=root;
		while(true) {
			while(n->right){
				if (val<*(n->right->val))
					break;
				n=n->right;
			}
			if ( n->down ==0)
				break;
			n=n->down;
		}
		typename std::list<ValType>::iterator it;

		if (n->left==0)
			it=list.begin();
		else
			it=n->val;

		while(it!=list.end()){
			if (!(*it<val))
				break;
			++it;
		}
		return it;
	}

	iterator lower_bound(KeyType key) const{
		Node *n=root;
		while(true) {
			while(n->right){
				if (! cmp(*(n->right->val),key) )
					break;
				n=n->right;
			}
			if ( n->down ==0)
				break;
			n=n->down;
		}
		typename std::list<ValType>::iterator it;

		if (n->left==0)
			it=list.begin();
		else
			it=n->val;

		while(it!=list.end()){
			if (!cmp(*it,key))
				break;
			++it;
		}
		return it;
	}

	iterator erase(iterator valit){

		Node *n=root;
		while(true) {
			while(n->right){

				/*			std::cout << "CMP"
					  << *valit
					  << "<" << *(n->right->val)
					  << " ." << n->skip
					  << std::endl;
					  */

				//if (*valit<*(n->right->val))
				if (cmp(*valit,*(n->right->val)))
					break;
				n=n->right;
			}
			Node *down = n->down;
			if (n->right)
				n->right->skip--;

			if (n->left!=0){
				if (valit==n->val){
					n->left->right = n->right;
					if (n->right){
						n->right->left=n->left;
						n->right->skip+=n->skip;
					}

					if (n->left->left==0 && n->right==0){
						if (n->left->down != 0){
							root=n->left->down;
							delete n->left;
						}
					}

					delete n;
				}
			}


			if (down==0)
				break;
			n=down;
		}
		return list.erase(valit);

	}


	iterator getIteratorAtIndex(int index) const{
		int i=0;
		Node *n = root;

		while(true) {
			while(n->right){
				Node *nr=n->right;

				if (i+nr->skip==index)
					return nr->val;
				if (i+nr->skip>index)
					break;
				i+=nr->skip;
				n=nr;
			}
			if (n->down==0)
				break;
			n=n->down;
		};

		typename std::list<ValType>::iterator it;

		if (n->left==0)
			it=list.begin();
		else
			it=n->val;

		while(i<index){
			++it;
			++i;
		}
		return it;
	}

	inline iterator getIterator(const Node * n) const{
		if (n->left==0)
			return list.begin();

		return n->val;
	}

	/*	inline bool equal(iterator i1,iterator i2){
		return !cmp(*i1,*i2) && !cmp(*i2,*i1);
	}
*/
	inline bool equal(const ValType & a,const ValType & b){
		return !cmp(a,b) && !cmp(b,a);
	}

	inline iterator findStart(iterator it) const {

		if (it==list.begin)
			return it;
		iterator i=it;
		do {
			--i;
			if (!equal(*i,*it))
				return it;
			--it;
		}while (i!=list.begin);
		return i;
	}


	Node * insertAtIndex0(Node *n, ValType val, int index,int i){



		while(n->right){
			Node *nr=n->right;

			//	if (i+nr->skip==index)
			//		return nr->val;
			if (i+nr->skip>index)
				break;
			i+=nr->skip;
			n=nr;
		}

		Node *nn;
		typename std::list<ValType>::iterator it, start;

		if (n->down!=0) {
			nn = insertAtIndex0(n->down,val,index,i);
		}
		else{

			it = getIterator(n);
			start=list.end();
			while(true){
				if (start==end())
					if (equal(val,*it) )
						start=it;

				if(!(i<index))
					break;
				++it;
				++i;
			}

			if (start==n->val && it==n->val){
				std::cout << "insert & replace " << std::endl;
			}
			else if (start==list.end()){
				std::cout << "insert & pull up" << std::endl;
			} else {
				std::cout << "just insert" << std::endl;

			}


		}
		std::cout << "start " << *it << std::endl;
		return n;
	}

	iterator insertAtIndex(ValType val, int index){
		insertAtIndex0(root,val,index,0);
		return root->val;
	}


	inline ValType * getValPtrAtIndex(int index) const{
		iterator it = getIteratorAtIndex(index);
		return &(*it);
	}

	inline ValType & operator[](int index){
		iterator it = getIteratorAtIndex(index);
		return *it;
	}

	void deleteAtIndex(int index){
		iterator it = getIteratorAtIndex(index);
		erase(it);
	}

	/**
	 * @brief begin
	 * @return
	 */
	inline iterator begin(){
		return list.begin();
	}

	/**
	 * @brief end
	 * @return
	 */
	inline iterator end(){
		return list.end();
	}

	/**
	 * @brief size
	 * @return
	 */
	inline size_t size() const{
		return list.size();
	}

	/**
	 * @brief empty
	 * @return
	 */
	inline bool empty() const{
		return list.empty();
	}



	//


	int height() const {
		Node *n=root;
		int h=0;
		while(n->down!=0){
			++h;
			n=n->down;
		}
		return h;
	}

	void print(){
		int h = height();
		std::cout << "HEIGHT: " << h << std::endl;

		Node *head = root;

		do {
			Node *n = head->right;
			std::cout << "[" << h << "]:";

			while(n){
				std::cout <<  *(n->val) <<
					      "(" << n->skip << ")"
					   << "," ;
				n=n->right;
			}
			std::cout << std::endl;
			head=head->down;
			h--;
		} while (head!=0);

		typename std::list<ValType>::iterator i;
		for(i=list.begin(); i!=list.end();++i){
			std::cout << *i <<",";
		}
		std::cout << std::endl;
	}
	bool verify(){
		if (list.size()<=1)
			return true;
		std::vector<iterator> v;
		typename std::list<ValType>::iterator it,itx;
		for(it =list.begin(); it!=list.end(); ++it){
			v.push_back(it);
		}

		Node *head = root;
		int h = height();
		while(head){
			//		std::cout << "verify level=" << h << std::endl;
			Node *n = head->right;
			int s=0;
			while(n) {
				s+=n->skip;
				itx=v[s];
				//if (v[s]!=n->val){

				if (itx!=n->val){
					std::cout << "VERIFY FAIL: level=" << h << ""
						  << " value=" << *(n->val) <<" "
						  << " endval" << (n->val==list.end())
						  << " val inm array=" << *itx
						  << " array end=" << (v[s]==list.end())
						  << " expected at:" << s << " found at:"
						  << std::endl;
					return false;
				}
				Node * old = n;
				n=n->right;
				if (n!=0){
					if (n->left != old){
						std::cout << "VERIY FAIL BACKPTR: at level="
							  << h << " "
							  << *(old->val) << "<- "
							  << *(n->val)
							  << std::endl;
						return false;
					}
				}


			}
			h--;
			head=head->down;
		}



		it=list.begin();
		ValType val = *it;
		++it;

		do {
			//			if (!(val<*it) ){

			if (!cmp(val,*it)){
				//								if (*it<val){
				if (cmp(*it,val)){
					std::cout << "VERIFY FAILED " << std::endl;
					return false;
				}
			}
			val = *it;
			++it;

		} while (it!=list.end());

		//std::cout << "VERIFY OK" << " HEIGHT: " << v.size() << std::endl;
		return true;
	}


};
}
#endif // SKIPLIST3_H
