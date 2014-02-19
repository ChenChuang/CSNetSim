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
	
private:
	Network* network;
};

#endif // UNICASTCHANNEL_H
