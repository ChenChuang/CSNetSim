#ifndef CLUSTERINGNETWORK_H
#define CLUSTERINGNETWORK_H

#include "network.h" // Base class: Network

class ClusteringNetwork : public Network
{
public:
	ClusteringNetwork(double* x, double* y, int num);
	~ClusteringNetwork();

private:
	Channel* max_radius_channel;
	Channel* cluster_radius_channel;
	Channel* unicast_channel;
	Channel* incluster_channel;
};

#endif // CLUSTERINGNETWORK_H
