#ifndef COMMPROXY_H
#define COMMPROXY_H

#include "compile_config.h"
#include "msg.h"
#include <string.h>

class CommProxy
{
public:
	CommProxy();
	~CommProxy();
public:
	int unicast(int fromaddr, int toaddr, int size, char cmd, int data_l, char* data);
	int broadcast(int fromaddr, double radius, int size, char cmd, int data_l, char* data);
	int inclustercast(int fromaddr, int size, char cmd, int data_l, char* data);
	int receive(struct Msg* t_msg);
	void clear_t_buf();
	void clear_r_buf();
	MsgIterator* t_msg_iter();
	MsgIterator* r_msg_iter();
private:
	//called by unicast and broadcast to push msg into t_msg_buf
	int push_msg(char type, int fromaddr, int toaddr, double radius, int size, char cmd, int data_l, char* data);
	void clear_buf((MsgNode*)& buf);
	
public:
	static const char MSG_TYPE_BROADCAST = 0x01;
	static const char MSG_TYPE_UNICAST = 0x02;
	static const char MSG_TYPE_INCLUSTERCAST = 0x03;
private:
	MsgNode* t_msg_buf;
	MsgNode* r_msg_buf;
	MsgIterator* t_msg_iter;
	MsgIterator* r_msg_iter;

friend class Monitor;
};

#endif // COMMPROXY_H
