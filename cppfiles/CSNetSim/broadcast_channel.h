#ifndef BROADCASTCHANNEL_H
#define BROADCASTCHANNEL_H

#include "channel.h" // Base class: Channel

class BroadcastChannel : public Channel
{
public:
	BroadcastChannel(Network* anetwork, double aradius);
	~BroadcastChannel();
public:
	virtual int communicate(Msg* msg);

private:
	Network* network;
	double radius;
	AdjG* adjg;
};

#endif // BROADCASTCHANNEL_H
