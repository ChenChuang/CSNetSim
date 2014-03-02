#ifndef CLUSTERINGCOMMPROXY_H
#define CLUSTERINGCOMMPROXY_H

#include "csnetsim.h" // Base class: CommProxy
#include "broadcast_channel.h"
#include "unicast_channel.h"
#include "incluster_channel.h"
#include "netcast_channel.h"

class ClusteringCommProxy : 
	public ECommProxy_BroadcastChannel, 
	public ECommProxy_UnicastChannel, 
	public ECommProxy_InclusterChannel,
	public ECommProxy_NetcastChannel
{
public:
	ClusteringCommProxy();
	~ClusteringCommProxy();
};

#endif // CLUSTERINGCOMMPROXY_H
