#include "base_comm_proxy.h"

BaseCommProxy::BaseCommProxy()
{
	this->t_msg_buf = NULL;
	this->r_msg_buf = NULL;
}

BaseCommProxy::~BaseCommProxy()
{
	clear_t_buf();
	clear_r_buf();
}

void BaseCommProxy::clear_t_buf()
{
	struct msg* p1;
	struct msg* p2;
	p1 = this->t_msg_buf;
	while(p1 != NULL){
		p2 = p1->next;
		delete_msg(p1);
		p1 = p2;
	}
	this->t_msg_buf = NULL;
}

void BaseCommProxy::clear_r_buf()
{
	struct msg* p1;
	struct msg* p2;
	p1 = this->r_msg_buf;
	while(p1 != NULL){
		p2 = p1->next;
		delete_msg(p1);
		p1 = p2;
	}
	this->r_msg_buf = NULL;
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

int BaseCommProxy::push_msg(char type, int fromaddr, int toaddr, double radius, int size, char cmd, int data_l, char* data){
	struct msg* t_msg = new msg();
	t_msg->type = type;
	t_msg->fromaddr = fromaddr;
	t_msg->toaddr = toaddr;
	t_msg->radius = radius;
	t_msg->size = size;
	t_msg->cmd = cmd;
	t_msg->data_l = data_l;
	t_msg->next = NULL;
	
#ifdef _print_
	printf("send_Msg : type = %x, from = %d, to = %d, r = %f, cmd = %x, data_l = %d\n", t_msg->type, t_msg->fromaddr, t_msg->toaddr, t_msg->radius, t_msg->cmd, t_msg->data_l);
#endif
	
	if (data_l > 0) {
		t_msg->data = new char[data_l];
		memcpy(t_msg->data, data, data_l);	
	}
	struct msg* p = this->t_msg_buf;
	if (p == NULL){
		this->t_msg_buf = t_msg;
	}else{
		while(p->next != NULL){
			p = p->next;
		}
		p->next = t_msg;
	}
	return 1;
}

int BaseCommProxy::onTransmitSucceed(struct msg* t_msg)
{
	struct msg* p = this->t_msg_buf;
	struct msg* pre = NULL;
	if (p == NULL){
		return -1;
	}
	while(p != NULL){
		if (p == t_msg){
			if (pre != NULL){
				pre->next = p->next;
			}else{
				this->t_msg_buf = NULL;
			}
			delete_msg(p);
			return 1;
		}else{
			p = p->next;
		}
	}
	return -1;
}

int BaseCommProxy::receive(struct msg* t_msg)
{
	struct msg* p = this->r_msg_buf;
	struct msg* r_msg = new msg();
	
	r_msg->type = t_msg->type;
	r_msg->fromaddr = t_msg->fromaddr;
	r_msg->toaddr = t_msg->toaddr;
	r_msg->size = t_msg->size;
	r_msg->cmd = t_msg->cmd;
	r_msg->data_l = t_msg->data_l;
	if (r_msg->data_l > 0) {
		r_msg->data = new char[r_msg->data_l];
		memcpy(r_msg->data, t_msg->data, r_msg->data_l);	
	}
	
	if (p == NULL){
		this->r_msg_buf = r_msg;
	}else{
		while(p->next != NULL){
			p = p->next;
		}
		p->next = r_msg;
	}
	return 1;
}

void BaseCommProxy::delete_msg(struct msg* p)
{
	if(p->data != NULL){
		delete p->data;
	}
	delete p;
}