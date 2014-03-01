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

public:
	static const char MSG_TYPE_INCLUSTERCAST = 0x03;
private:
	Network* network;
};

class ECommProxy_InclusterChannel : virtual public CommProxy
{
public:
	int inclustercast(int fromaddr, int size, char cmd, int data_l, char* data);
};

#endif // INCLUSTERCHANNEL_H
