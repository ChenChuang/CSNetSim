#include "clustering_network.h"

ClusteringNetwork::ClusteringNetwork(double* x, double* y): 
	Network(x, y, ClusteringSimModel::NODE_NUM, ClusteringSimModel::MAX_SIM_TIME, ClusteringSimModel::DEFAULT_TICK)
{
	this->sensor_nodes_num = ClusteringSimModel::NODE_NUM - 1;
	this->nodes[0] = new SinkNode(this, 0, ClusteringSimModel::SINK_X, ClusteringSimModel::SINK_Y);
	for(int i = 1; i < ClusteringSimModel::NODE_NUM; i ++){
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
	
	for(int i = 1; i < ClusteringSimModel::NODE_NUM; i ++){
		dynamic_cast<SensorNode*>(this->nodes[i])->init_neighbors(this->cluster_radius_channel->get_ajdv(i));
	}
}

ClusteringNetwork::~ClusteringNetwork()
{
	for(int i = 0; i < this->nodes_num; i ++){
		delete this->nodes[i];
		this->nodes[i] = NULL;
	}
	
	delete this->max_radius_channel;
	this->max_radius_channel = NULL;
	delete this->cluster_radius_channel;
	this->cluster_radius_channel = NULL;
	delete this->unicast_channel;
	this->unicast_channel = NULL;
	delete this->incluster_channel;
	this->incluster_channel = NULL;
	
	delete this->monitor;
	this->monitor = NULL;
}

double ClusteringNetwork::d_between(int addr1, int addr2)
{
	double x1 = this->nodes[addr1]->x;
	double y1 = this->nodes[addr1]->y;
	double x2 = this->nodes[addr2]->x;
	double y2 = this->nodes[addr2]->y;
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

bool ClusteringNetwork::is_alive(int addr)
{
	return this->nodes[addr]->is_alive();
}
