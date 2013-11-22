#ifndef MSG_H
#define MSG_H

//nodes in WSN use this struct to communicate with each other
struct msg
{
	//3 types of transmission provided: BaseCommProxy::MSG_TYPE_BROADCAST/UNICAST/MULTICAST
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
	//pointer of next msg
	struct msg* next;
};

#endif