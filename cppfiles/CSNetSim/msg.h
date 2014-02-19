#ifndef MSG_H
#define MSG_H

#include "compile_config.h"

//nodes in WSN use this struct to communicate with each other
struct Msg
{
	//3 types of transmission provided: CommProxy::MSG_TYPE_BROADCAST/UNICAST/MULTICAST
	//Noted that MULTICAST hasn't been developed
	char type;
	//addr of sender
	int fromaddr;
	//addr of receiver
	int toaddr;	
	//transmission radius of BROADCAST
	double radius;
	//packet size, not related to size of this "msg"
	int size;
	//command of this packet
	char cmd;
	//length in byte of data below
	int data_l;
	//data
	char* data;
	//referrence count
	int rc;
	
	Msg():
		type(-2),
		fromaddr(-2),
		toaddr(-2),
		radius(0),
		size(0),
		cmd(-2),
		data_l(0),
		data(NULL),
		rc(1) {}
	
	Msg(char atype, int afromaddr, int atoaddr, double aradius, int asize, char acmd, int adata_l, char* adata):
		type(atype),
		fromaddr(afromaddr),
		toaddr(atoaddr),
		radius(aradius),
		size(asize),
		cmd(acmd),
		data_l(adata_l), 
		rc(1) 
	{
		if (adata_l > 0) {
			this->data = new char[adata_l];
			memcpy(this->data, adata, adata_l);	
		}
	}

	
	~Msg() {
		if(--this->rc <= 0 && this->data != NULL) {
			delete this->data;
		}
	}
};

struct MsgNode
{
	struct Msg* body;
	struct MsgNode* next;
	MsgNode(Msg* amsg, MsgNode* anext): body(amsg), next(anext) {
		this->body->rc++;
	}
	MsgNode(): body(NULL), next(NULL) {}
	~MsgNode() {
		delete body;
	}
};

class MsgIterator
{
public:
	MsgIterator(MsgNode* head): ptr(head) {};
	~MsgIterator() {};
public:
	Msg* next()
	{
		Msg* msg = NULL;
		if(this->ptr != NULL){
			msg = this->ptr->body;
			this->ptr = this->ptr->next;
		}
		return msg;
	}
	bool has_more()
	{
		return this->ptr != NULL;
	}
	void seek(MsgNode* head)
	{
		this->ptr = head;
	}

private:
	MsgNode* ptr;
};

#endif