#ifndef CLUSTERINGNETWORK_H
#define CLUSTERINGNETWORK_H

#include "network.h" // Base class: Network

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
	
friend class ClusteringMonitor;
};

#endif // CLUSTERINGNETWORK_H
