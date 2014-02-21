#ifndef SINKCOMMPROXY_H
#define SINKCOMMPROXY_H

#include "comm_proxy.h" // Base class: CommProxy
#include "broadcast_channel.h" // Base class: ECommProxy_BroadcastChannel
#include "unicast_channel.h" // Base class: ECommProxy_UnicastChannel

class SinkCommProxy : public ECommProxy_BroadcastChannel, public ECommProxy_UnicastChannel
{
public:
	SinkCommProxy();
	~SinkCommProxy();

};

#endif // SINKCOMMPROXY_H
