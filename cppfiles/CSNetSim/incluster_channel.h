#ifndef INCLUSTERCHANNEL_H
#define INCLUSTERCHANNEL_H

#include "csnetsim.h"
#include "clustering_member.h"
#include "clustering_sim_model.h"

class InclusterChannel : public Channel
{
public:
	InclusterChannel(Network* anetwork);
	~InclusterChannel();
public:
	virtual int communicate(Msg* msg);

private:
	Network* network;
};

class INode_InclusterChannel
{
public:
	virtual MnManager* get_mnmanager() = 0;
	
};

#endif // INCLUSTERCHANNEL_H
