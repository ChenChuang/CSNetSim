#ifndef BASECOMMPROXY_H
#define BASECOMMPROXY_H

#include "compile_config.h"
#include "msg.h"
#include <string.h>

class BaseCommProxy
{
public:
	BaseCommProxy();
	~BaseCommProxy();

public:
	//unicast(...) is called by node to unicast a msg to toaddr
	int unicast(int fromaddr, int toaddr, int size, char cmd, int data_l, char* data);
	//broadcast(...) is called by node to broadcast a msg to all nodes within radius
	int broadcast(int fromaddr, double radius, int size, char cmd, int data_l, char* data);
	int inclustercast(int fromaddr, int size, char cmd, int data_l, char* data);
	//receive(...) is called by network to pass the msg in src's transmit buffer to des's receive buffer
	int receive(struct msg* t_msg);
	//onTransmitSucceed(...) is called by network to delete the transmited msg from src's transmit buffer
	int onTransmitSucceed(struct msg* t_msg);
	//clear the transmit/receive buffer
	void clear_t_buf();
	void clear_r_buf();

private:
	//called by unicast and broadcast to push msg into t_msg_buf
	int push_msg(char type, int fromaddr, int toaddr, double radius, int size, char cmd, int data_l, char* data);
	void delete_msg(struct msg* p);
	
public:
	//see msg.type in msg.h
	static const char MSG_TYPE_BROADCAST = 0x01;
	static const char MSG_TYPE_UNICAST = 0x02;
	static const char MSG_TYPE_INCLUSTERCAST = 0x03;
	
	//link-list of msg to be sended
	struct msg* t_msg_buf;
	//link-list of msg received
	struct msg* r_msg_buf;

friend class Monitor;
};

#endif // BASECOMMPROXY_H
