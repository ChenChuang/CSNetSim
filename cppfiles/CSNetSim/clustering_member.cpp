#include "clustering_member.h"

MnManager::MnManager(Node* anode): node(anode), members(NULL)
{
	this->mn_iter = new MnIterator(this->members);
}

MnManager::~MnManager()
{
	this->clear();
	delete this->mn_iter;
}

void MnManager::remove(int addr)
{
	struct MnNode* p = this->members;
	struct MnNode* pp = NULL;
	while(p != NULL){
		if(p->addr == addr){
			if(pp != NULL){
				pp->next = p->next;
			}else{
				this->members = NULL;
			}
			delete p;
		}
		pp = p;
		p = p->next;
	}
}

void MnManager::clear()
{
	struct MnNode* p = this->members;
	struct MnNode* np;
	while(p != NULL){
		np = p->next;
		delete p;
		p = np;
	}
	this->members = NULL;
}

void MnManager::clear_dead()
{	
	struct MnNode* p = this->members;
	struct MnNode* pp = NULL;
	while(p != NULL){
		if(!this->node->get_network()->node(p->addr)->is_alive()){
			if(pp != NULL){
				pp->next = p->next;
			}else{
				this->members = NULL;
			}
			delete p;
		}
		pp = p;
		p = p->next;
	}
}

void MnManager::add(int addr)
{
	struct MnNode* p = this->members;
	struct MnNode* pp = NULL;
	while(p != NULL){
		if(p->addr == addr){
			return;
		}
		pp = p;
		p = p->next;
	}
	p = new MnNode(addr);
	pp->next = p;
}

bool MnManager::has(int addr)
{
	struct MnNode* p = this->members;
	while(p != NULL){
		if(p->addr == addr){
			return true;
		}
		p = p->next;
	}
	return false;
}

MnIterator* MnManager::get_mn_iter()
{
	this->mn_iter->seek(this->members);
	return this->mn_iter;
}