#ifndef COMMPROXY_H
#define COMMPROXY_H

#include "compile_config.h"
#include "msg.h"
#include <string.h>

class CommProxy
{
public:
	CommProxy();
	virtual ~CommProxy();
public:
	int receive(struct Msg* t_msg);
	void clear_t_buf();
	void clear_r_buf();
	MsgIterator* get_t_msg_iter();
	MsgIterator* get_r_msg_iter();
	int push_msg(char type, int fromaddr, int toaddr, double radius, int size, char cmd, int data_l, char* data);
	void clear_buf(MsgNode* buf);
	
private:
	MsgNode* t_msg_buf;
	MsgNode* r_msg_buf;
	MsgIterator* t_msg_iter;
	MsgIterator* r_msg_iter;

friend class Monitor;
};

#endif // COMMPROXY_H
