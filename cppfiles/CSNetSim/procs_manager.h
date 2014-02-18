#ifndef PROCSMANAGER_H
#define PROCSMANAGER_H

struct ProcNode
{
	Processor* body;
	struct ProcNode* next;
	ProcNode(Processor* aproc): body(aproc), next(NULL) {}
	~ProcNode() {}
}

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
}

class ProcsManager
{
public:
	ProcsManager(Node* anode);
	~ProcsManager();
public:
	Node* node() {return this->node;}
	void add(Processor* p);
	void clear();
	ProcIterator* proc_iter();
	
public:
	Node* node;
	ProcNode* procs;
	ProcIterator* proc_iter;
};

#endif // PROCSMANAGER_H
