#include "clustering_network.h"

ClusteringNetwork::ClusteringNetwork(double* x, double* y): 
	Network(x, y, ClusteringSimModel::NODE_NUM, ClusteringSimModel::MAX_SIM_TIME)
{
	this->sensor_nodes_num = num - 1;
	this->nodes[0] = new SinkNode(this, 0, ClusteringSimModel::SINK_X, ClusteringSimModel::SINK_Y);
	for(int i = 1; i < num; i ++){
		this->nodes[i] = new SensorNode(this, i, x[i], y[i], ClusteringSimModel::E_INIT);
	}
	
	this->max_radius_channel = new BroadcastChannel(this, ClusteringSimModel::MAX_RADIUS);
	this->cluster_radius_channel = new BroadcastChannel(this, ClusteringSimModel::CLUSTER_RADIUS);
	this->unicast_channel = new UnicastChannel(this);
	this->incluster_channel = new InclusterChannel(this);
	
	this->channels->add(this->max_radius_channel);
	this->channels->add(this->cluster_radius_channel);
	this->channels->add(this->unicast_channel);
	this->channels->add(this->incluster_channel);
	
	this->monitor = new ClusteringMonitor(this);
}

ClusteringNetwork::~ClusteringNetwork()
{
	for(int i = 0; i < this->nodes_num; i ++){
		delete this->nodes[i];
	}
	
	delete this->max_radius_channel;
	delete this->cluster_radius_channel;
	delete this->unicast_channel;
	delete this->incluster_channel;
	
	delete this->monitor;
}
