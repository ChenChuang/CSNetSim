#ifndef UNICASTCHANNEL_H
#define UNICASTCHANNEL_H

#include "channel.h" // Base class: Channel

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
