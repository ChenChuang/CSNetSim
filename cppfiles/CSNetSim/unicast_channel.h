#ifndef UNICASTCHANNEL_H
#define UNICASTCHANNEL_H

#include "csnetsim.h"

class UnicastChannel : public Channel
{
public:
	UnicastChannel(Network* anetwork);
	~UnicastChannel();
public:
	virtual int communicate(Msg* msg);
	
public:
	static const char MSG_TYPE_UNICAST = 0x02;
private:
	Network* network;
};

class ECommProxy_UnicastChannel : virtual public CommProxy
{
public:
	int unicast(int fromaddr, int toaddr, int size, char cmd, int data_l, char* data);
	int repost(Msg* msg, int toaddr);
};

#endif // UNICASTCHANNEL_H
