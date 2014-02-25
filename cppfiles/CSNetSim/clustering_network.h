#ifndef CLUSTERINGNETWORK_H
#define CLUSTERINGNETWORK_H

#include "csnetsim.h"
#include "clustering_sim_model.h"
#include "sink_node.h"
#include "sensor_node.h"
#include "sensor_lcr_proc.h"
#include "clustering_monitor.h"
#include "broadcast_channel.h"
#include "unicast_channel.h"
#include "incluster_channel.h"

class ClusteringNetwork : public Network, public INet_SensorLcrProc
{
public:
	ClusteringNetwork(double* x, double* y);
	~ClusteringNetwork();
public:
	double d_between(int addr1, int addr2);
	bool is_alive(int addr);
	
public:
	int sensor_nodes_num;
	BroadcastChannel* max_radius_channel;
	BroadcastChannel* cluster_radius_channel;
	UnicastChannel* unicast_channel;
	InclusterChannel* incluster_channel;
	
friend class Monitor;
friend class ClusteringMonitor;
};

#endif // CLUSTERINGNETWORK_H
