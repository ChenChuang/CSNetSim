#include "mn_manager.h"

MnManager::MnManager(BaseNode* anode): node(anode), members(NULL)
{
}

MnManager::~MnManager()
{
	this->clear();
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
		if(!this->node->get_network()->is_alive(p->addr)){
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

MnNode* MnManager::find(int addr)
{
	struct MnNode* p = this->members;
	struct MnNode* pp = NULL;
	while(p != NULL){
		if(p->addr == addr){
			break;
		}
		pp = p;
		p = p->next;
	}
	return p;
}