#ifndef MSG_H
#define MSG_H

#include "compile_config.h"

#include <stdio.h>

//nodes in WSN use this struct to communicate with each other
struct Msg
{
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
		rc(0) {}
	
	Msg(char atype, int afromaddr, int atoaddr, double aradius, int asize, char acmd, int adata_l, char* adata):
		type(atype),
		fromaddr(afromaddr),
		toaddr(atoaddr),
		radius(aradius),
		size(asize),
		cmd(acmd),
		data_l(adata_l), 
		rc(0) 
	{
		if (adata_l > 0 && adata != NULL) {
			this->data = new char[adata_l];
			memcpy(this->data, adata, adata_l);	
		}else{
			this->data = NULL;
		}
	}

	
	~Msg() {
		delete[] this->data;
		this->data = NULL;
	}
	
	void print(){
		printf("0x%02X, %d -> %d, %2.2f, %d, 0x%02X, %d\n", this->type, this->fromaddr, this->toaddr, this->radius, this->size, this->cmd, this->data_l);
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
		this->body->rc--;
		if(this->body->rc <= 0){
			delete this->body;
		}
		this->body = NULL;
		this->next = NULL;
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