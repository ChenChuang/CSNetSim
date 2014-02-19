#ifndef CLUSTERINGMEMBER_H
#define CLUSTERINGMEMBER_H

#include "csnetsim.h"
#include "clustering_sim_model.h"

struct MnNode
{
	int addr;
	struct MnNode* next;
	MnNode(int aaddr): addr(aaddr), next(NULL) {};
};

class MnIterator
{
public:
	MnIterator(MnNode* head): ptr(head) {};
	~MnIterator() {};
public:
	int next_addr()
	{
		int addr = -1;
		if(this->ptr != NULL){
			addr = this->ptr->addr;
			this->ptr = this->ptr->next;
		}
		return addr;
	}
	bool has_more() {return this->ptr != NULL;}
	void seek(MnNode* head) {this->ptr = head;}

private:
	MnNode* ptr;
};

class MnManager
{
public:
	MnManager(Node* anode);
	~MnManager();
public:
	void remove(int addr);
	void clear();
	void add(int addr);
	bool has(int addr);
	void clear_dead();
	MnIterator* get_mn_iter();
	
public:
	Node* node;
	MnNode* members;
	MnIterator* mn_iter;
};

#endif // CLUSTERINGMEMBER_H
