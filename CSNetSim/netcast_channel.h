#ifndef NETCASTCHANNEL_H
#define NETCASTCHANNEL_H

#include "csnetsim.h" // Base class: Channel

class NetcastChannel : public Channel
{
public:
	NetcastChannel(Network* anetwork);
	~NetcastChannel();
public:
	int communicate(Msg* msg);

public:
	static const char MSG_TYPE_NETCAST = 0x04;
private:
	Network* network;
};

class ECommProxy_NetcastChannel : virtual public CommProxy
{
public:
	int netcast(int fromaddr, int size, char cmd, int data_l, char* data);
};

#endif // NETCASTCHANNEL_H
