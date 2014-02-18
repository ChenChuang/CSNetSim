#include "clustering_network.h"

ClusteringNetwork::ClusteringNetwork(double* x, double* y, int num): 
	Network(x, y, num)
{
	this->nodes[0] = new SinkNode(this, 0, 0, 0, 1);
	for(int i = 1; i < num; i ++){
		this->nodes[i] = new SensorNode(this, i, x[i], y[i], EnergyModel::E_INIT);
	}
}

ClusteringNetwork::~ClusteringNetwork()
{
}
