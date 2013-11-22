#include "adj_g.h"

AdjG::AdjG(double* x, double* y, double r, int n)
{
	this->radius = r;
	this->v_num = n;
	//allocate
	this->v = new struct adjv*[n];
	int i, j;
	double d2;
	double r2 = pow(r, 2);
	//temp link-list array of type adjv
	struct adjv** t = new struct adjv*[n];
	for(i = 0; i < n; i ++){
		this->v[i] = new struct adjv();
		this->v[i]->addr = i;
		this->v[i]->d = 0;
		this->v[i]->next = NULL;
		t[i] = this->v[i];
	}
	//check all pairs of nodes whether they are in each other's radius
	for(i = 0; i < n; i ++){
		for(j = i+1; j < n; j++){
			d2 = pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2);
			if(d2 <= r2){
				//node i and j are in each other's radius
				//add new adjv to i's link-list
				t[i]->next = new struct adjv();
				t[i] = t[i]->next;
				t[i]->addr = j;
				t[i]->d = sqrt(d2);
				t[i]->next = NULL;
				//add new adjv to j's link-list
				t[j]->next = new struct adjv();
				t[j] = t[j]->next;
				t[j]->addr = i;
				t[j]->d = sqrt(d2);
				t[j]->next = NULL;
			}
		}
	}
	delete t;
}

//do not forget to release all adjv in destructor!
AdjG::~AdjG()
{
	int i;
	struct adjv* p1;
	struct adjv* p2;
	for(i = 0; i < this->v_num; i ++){
		p2 = this->v[i];
		while(p2 != NULL){
			p1 = p2->next;
			delete p2;
			p2 = p1;
		}
	}
	delete this->v;
}

void AdjG::printStr()
{
	int i;
	struct adjv* p;
	for(i = 0; i < this->v_num; i ++){
		p = this->v[i];
		while(p != NULL){
			printf("%d:%f -> ", p->addr, p->d);
			p = p->next;
		}
		printf("\n");
	}
}

void AdjG::deleteNode(int addr)
{
	int i;
	struct adjv* p1;
	struct adjv* p2;
	for(i = 0; i < this->v_num; i ++){
		if(i == addr){
			//delete the whole link-list of node with addr
			p2 = this->v[addr]->next;
			while(p2 != NULL){
				p1 = p2->next;
				delete p2;
				p2 = p1;	
			}
			this->v[addr]->next = NULL;
		}else{
			//delete all adjv with addr in other nodes' link-list
			p2 = this->v[i];
			p1 = p2->next;
			while(p1 != NULL){
				if(p1->addr == addr){
					p2->next = p1->next;
					delete p1;
					p1 = p2->next;
				}else{
					p2 = p1;
					p1 = p2->next;
				}
			}
		}
	}
}

