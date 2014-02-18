#include "adj_g.h"

AdjG::AdjG(double* x, double* y, double r, int n)
{
	this->radius = r;
	this->v_num = n;
	this->v = new Adjv*[n];
	int i, j;
	double d2;
	double r2 = pow(r, 2);
	//temp link-list array of type Adjv
	Adjv** t = new Adjv*[n];
	for(i = 0; i < n; i ++){
		this->v[i] = new Adjv(i, 0, NULL);
		t[i] = this->v[i];
	}
	//check all pairs of nodes whether they are in each other's radius
	for(i = 0; i < n; i ++){
		for(j = i+1; j < n; j++){
			d2 = pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2);
			if(d2 <= r2){
				//node i and j are in each other's radius
				//add new Adjv to i's link-list
				t[i]->next = new Adjv(j, sqrt(d2), NULL);
				t[i] = t[i]->next;
				//add new Adjv to j's link-list
				t[j]->next = new Adjv(i, sqrt(d2), NULL);
				t[j] = t[j]->next;
			}
		}
	}
	delete[] t;
}

AdjG::AdjG(Network* anetwork, double r, int n)
{
	this->radius = r;
	this->v_num = n;
	//allocate
	this->v = new Adjv*[n];
	int i, j;
	double d2;
	double r2 = pow(r, 2);
	//temp link-list array of type Adjv
	Adjv** t = new Adjv*[n];
	for(i = 0; i < n; i ++){
		this->v[i] = new Adjv(i, 0, NULL);
		t[i] = this->v[i];
	}
	//check all pairs of nodes whether they are in each other's radius
	for(i = 0; i < n; i ++){
		for(j = i+1; j < n; j++){
			d2 = pow(anetwork->node(i)->x - anetwork->node(j)->x, 2) + 
				 pow(anetwork->node(i)->y - anetwork->node(j)->y, 2);
			if(d2 <= r2){
				//node i and j are in each other's radius
				//add new Adjv to i's link-list
				t[i]->next = new Adjv(j, sqrt(d2), NULL);
				t[i] = t[i]->next;
				//add new Adjv to j's link-list
				t[j]->next = new Adjv(i, sqrt(d2), NULL);
				t[j] = t[j]->next;
			}
		}
	}
	delete[] t;
}

AdjG::~AdjG()
{
	int i;
	Adjv* p, np;
	for(i = 0; i < this->v_num; i ++){
		p = this->v[i];
		while(p != NULL){
			np = p->next;
			delete p;
			p = np;
		}
	}
	delete[] this->v;
}

void AdjG::print()
{
	int i;
	Adjv* p;
	for(i = 0; i < this->v_num; i ++){
		p = this->v[i];
		while(p != NULL){
			printf("%d:%f -> ", p->addr, p->d);
			p = p->next;
		}
		printf("\n");
	}
}

void AdjG::delete_node(int addr)
{
	int i;
	Adjv* p, np;
	for(i = 0; i < this->v_num; i ++){
		if(i == addr){
			//delete the whole link-list of node with addr
			p = this->v[addr]->next;
			while(p != NULL){
				np = p->next;
				delete p;
				p = np;	
			}
			this->v[addr]->next = NULL;
		}else{
			//delete all Adjv with addr in other nodes' link-list
			p = this->v[i];
			np = p->next;
			while(p != NULL && p->next != NULL){
				np = p->next;
				if(np->addr == addr){
					p->next = np->next;
					delete np;
				}
				p = p->next;
			}
		}
	}
}

Adjv* AdjG::neighbors_of(int addr){
	return this->v[addr];
}

bool AdjG::is_neighbor(int addr_a, int addr_b){
	Adjv* p = this->v[addr_a]->next;
	while(p != NULL){
		if(p->addr == addr_b){
			return true;
		}
		p = p->next;
	}
	return false;
}

