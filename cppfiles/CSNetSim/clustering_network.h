#ifndef CLUSTERINGNETWORK_H
#define CLUSTERINGNETWORK_H

#include "csnetsim.h"
#include "clustering_sim_model.h"
#include "sink_node.h"
#include "sensor_node.h"
#include "clustering_monitor.h"
#include "broadcast_channel.h"
#include "unicast_channel.h"
#include "incluster_channel.h"

class ClusteringNetwork : public Network
{
public:
	ClusteringNetwork(double* x, double* y);
	~ClusteringNetwork();
	
private:
	int sensor_nodes_num;
	Channel* max_radius_channel;
	Channel* cluster_radius_channel;
	Channel* unicast_channel;
	Channel* incluster_channel;
	
friend class Monitor;
friend class ClusteringMonitor;
};

#endif // CLUSTERINGNETWORK_H
