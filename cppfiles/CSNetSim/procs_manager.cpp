#include "procs_manager.h"

ProcsManager::ProcsManager(Node* anode): node(anode), procs(NULL)
{
	this->proc_iter = new ProcIterator(this->procs);
}

ProcsManager::~ProcsManager()
{
	delete this->proc_iter;
	this->clear_procs();
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
	p = this->procs;
	while(p != NULL){
		p->proc->init();
	}
}

void ProcsManager::clear()
{
	ProcNode* p = this->procs;
	ProcNode* np;
	while(p != NULL){
		np = p->next;
		delete np;
		p = np;
	}
}

ProcIterator* ProcsManager::proc_iter()
{
	this->proc_iter->seek(this->procs);
	return this->proc_iter;
}