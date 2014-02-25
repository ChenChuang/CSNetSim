#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "compile_config.h"
#include "msg.h"

class Node;

class Processor
{
public:
	Processor();
	virtual ~Processor();
public:
	virtual void ticktock(double time) {}
	virtual int process(Msg* msg) {return -1;}
	virtual void init() {}
	virtual void turn_on() {this->isTurnOn = true;}
	virtual void turn_off() {this->isTurnOn = false;}
public:
	bool isTurnOn;
};

struct ProcNode
{
	Processor* body;
	struct ProcNode* next;
	ProcNode(Processor* aproc): body(aproc), next(NULL) {}
	~ProcNode() {}
};

class ProcIterator
{
public:
	ProcIterator(ProcNode* head): ptr(head) {};
	~ProcIterator() {};
public:
	Processor* next()
	{
		Processor* proc = NULL;
		if(this->ptr != NULL){
			proc = this->ptr->body;
			this->ptr = this->ptr->next;
		}
		return proc;
	}
	bool has_more() {return this->ptr != NULL;}
	void seek(ProcNode* head) {this->ptr = head;}

private:
	ProcNode* ptr;
};

class ProcsManager
{
public:
	ProcsManager(Node* anode);
	~ProcsManager();
public:
	Node* get_node() {return this->node;}
	void init();
	void add(Processor* p);
	void clear();
	ProcIterator* get_proc_iter();
	
public:
	Node* node;
	ProcNode* procs;
	ProcIterator* proc_iter;
};

#endif // PROCESSOR_H
