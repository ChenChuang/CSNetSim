#ifndef BROADCASTCHANNEL_H
#define BROADCASTCHANNEL_H

#include "csnetsim.h"

class BroadcastChannel : public Channel
{
public:
	BroadcastChannel(Network* anetwork, double aradius);
	~BroadcastChannel();
public:
	int communicate(Msg* msg);
	Adjv* get_ajdv(int addr);

public:
	static const char MSG_TYPE_BROADCAST = 0x01;
private:
	Network* network;
public:
	double radius;
	AdjG* adjg;

friend class Monitor;
};

class ECommProxy_BroadcastChannel : virtual public CommProxy
{
public:
	int broadcast(int fromaddr, double radius, int size, char cmd, int data_l, char* data);
};

#endif // BROADCASTCHANNEL_H
