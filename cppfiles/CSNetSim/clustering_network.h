#ifndef CLUSTERINGNETWORK_H
#define CLUSTERINGNETWORK_H

#include "network.h" // Base class: Network

class ClusteringNetwork : public Network
{
public:
	ClusteringNetwork(double* x, double* y, int num);
	~ClusteringNetwork();

public:
	virtual void communicate();
	virtual void run();
};

#endif // CLUSTERINGNETWORK_H
