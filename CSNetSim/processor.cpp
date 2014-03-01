#include "processor.h"

Processor::Processor() : isTurnOn(false)
{
}

Processor::~Processor()
{
}

ProcsManager::ProcsManager(Node* anode): node(anode), procs(NULL)
{
	this->proc_iter = new ProcIterator(this->procs);
}

ProcsManager::~ProcsManager()
{
	delete this->proc_iter;
	this->proc_iter = NULL;
	this->clear();
}

void ProcsManager::add(Processor* proc)
{
	ProcNode* p = this->procs;
	ProcNode* pp = NULL;
	while(p != NULL){
		pp = p;
		p = p->next;
	}
	p = new ProcNode(proc);
	if(pp != NULL){
		pp->next = p;
	}else{
		this->procs = p;
	}
}

void ProcsManager::init()
{
	ProcNode* p = this->procs;
	while(p != NULL){
		p->body->init();
		p = p->next;
	}
}

void ProcsManager::clear()
{
	ProcNode* p = this->procs;
	ProcNode* np;
	while(p != NULL){
		np = p->next;
		delete p;
		p = np;
	}
}

ProcIterator* ProcsManager::get_proc_iter()
{
	this->proc_iter->seek(this->procs);
	return this->proc_iter;
}