#ifndef BROADCASTCHANNEL_H
#define BROADCASTCHANNEL_H

#include "csnetsim.h"

class BroadcastChannel : public Channel
{
public:
	BroadcastChannel(Network* anetwork, double aradius);
	~BroadcastChannel();
public:
	virtual int communicate(Msg* msg);

public:
	Network* network;
	double radius;
	AdjG* adjg;

friend class Monitor;
};

#endif // BROADCASTCHANNEL_H
