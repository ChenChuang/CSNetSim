#ifndef SINKCOMMPROXY_H
#define SINKCOMMPROXY_H

#include "comm_proxy.h" // Base class: CommProxy
#include "broadcast_channel.h" // Base class: ECommProxy_BroadcastChannel
#include "unicast_channel.h" // Base class: ECommProxy_UnicastChannel
#include "netcast_channel.h"

class SinkCommProxy : public ECommProxy_BroadcastChannel, public ECommProxy_UnicastChannel, public ECommProxy_NetcastChannel
{
public:
	SinkCommProxy();
	~SinkCommProxy();

};

#endif // SINKCOMMPROXY_H
