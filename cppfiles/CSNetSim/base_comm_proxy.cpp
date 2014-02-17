#include "base_comm_proxy.h"

BaseCommProxy::BaseCommProxy()
{
	this->t_msg_buf = NULL;
	this->r_msg_buf = NULL;
	this->t_msg_iter = new MsgIterator(t_msg_buf);
	this->r_msg_iter = new MsgIterator(r_msg_buf);
}

BaseCommProxy::~BaseCommProxy()
{
	clear_t_buf();
	clear_r_buf();
	delete this->t_msg_iter;
	delete this->r_msg_iter;
}

MsgIterator* BaseCommProxy::t_msg_iter(){
	this->t_msg_iter->seek(this->t_msg_buf);
	return this->t_msg_iter;
}

MsgIterator* BaseCommProxy::r_msg_iter(){
	this->r_msg_iter->seek(this->r_msg_buf);
	return this->r_msg_iter;
}

void BaseCommProxy::clear_t_buf()
{
	this->clear_buf(this->t_msg_buf);
}

void BaseCommProxy::clear_r_buf()
{
	this->clear_buf(this->r_msg_buf);
}

void BaseCommProxy::clear_buf((MsgNode*)& buf)
{
	MsgNode* p, np;
	p = buf;
	while(p != NULL){
		np = p->next;
		delete p;
		p = np;
	}
	buf = NULL;
}

int BaseCommProxy::push_msg(char type, int fromaddr, int toaddr, double radius, int size, char cmd, int data_l, char* data){
	Msg* t_msg = new Msg(type, fromaddr, toaddr, radius, size, cmd, data_l, data, NULL);
	MsgNode* t_msg_node = new MsgNode(t_msg, NULL);
	MsgNode* p = this->t_msg_buf;
	if (p == NULL){
		this->t_msg_buf = t_msg_node;
	}else{
		MsgNode* np = p->next;
		p->next = t_msg_node;
		t_msg_node->next = np;
	}
#ifdef _print_
	printf("send_Msg : type = %x, from = %d, to = %d, r = %f, cmd = %x, data_l = %d\n", t_msg->type, t_msg->fromaddr, t_msg->toaddr, t_msg->radius, t_msg->cmd, t_msg->data_l);
#endif
	delete t_msg;
	return 1;
}

int BaseCommProxy::receive(struct Msg* t_msg)
{
	MsgNode* r_msg_node = new MsgNode(t_msg, NULL);	
	MsgNode* p = this->r_msg_buf;
	if (p == NULL){
		this->r_msg_buf = r_msg_node;
	}else{
		MsgNode* np = p->next;
		p->next = r_msg_node;
		r_msg_node->next = np;
	}
	return 1;
}

int BaseCommProxy::unicast(int fromaddr, int toaddr, int size, char cmd, int data_l, char* data)
{
	return this->push_msg(BaseCommProxy::MSG_TYPE_UNICAST, fromaddr, toaddr, -1, size, cmd, data_l, data);
}

int BaseCommProxy::broadcast(int fromaddr, double radius, int size, char cmd, int data_l, char* data)
{
	return this->push_msg(BaseCommProxy::MSG_TYPE_BROADCAST, fromaddr, -1, radius, size, cmd, data_l, data);
}

int BaseCommProxy::inclustercast(int fromaddr, int size, char cmd, int data_l, char* data)
{
	return this->push_msg(BaseCommProxy::MSG_TYPE_INCLUSTERCAST, fromaddr, -1, -1, size, cmd, data_l, data);
}

int BaseCommProxy::repost(Msg* msg, int toaddr){
	return this->push_msg(BaseCommProxy::MSG_TYPE_UNICAST, msg->toaddr, toaddr, -1, msg->size, msg->cmd, msg->data_l, msg->data);
}
