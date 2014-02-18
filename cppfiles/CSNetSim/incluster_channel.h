#ifndef INCLUSTERCHANNEL_H
#define INCLUSTERCHANNEL_H

#include "channel.h" // Base class: Channel

class InclusterChannel : public Channel
{
public:
	InclusterChannel(Network* anetwork;);
	~InclusterChannel();
public:
	virtual int communicate(Msg* msg);

private:
	Network* network;
};

class INode_InclusterChannel
{
public:
	virtual MnManager* mnmanager() = 0;
	
}

#endif // INCLUSTERCHANNEL_H
