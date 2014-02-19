#include "sensor_node.h"

SensorNode::SensorNode(Network* anetwork, int aaddr, double ax, double ay, double energy): 
	Node(anetwork, aaddr, ax, ay, energy), 
	mnmanager(new MnManager(this)),
	ch_addr(-1),
	next_hop(-1),
	d_tosink(sqrt(pow(ax - ClusteringSimModel::SINK_X, 2) + pow(ay - ClusteringSimModel::SINK_Y, 2)))
{
}

SensorNode::~SensorNode()
{
	delete this->mnmanager;
}

void SensorNode::print()
{
	printf("node %4d : location = ( %4f, %4f ), energy = %4f, ch = %4d, nexthop = %4d, d_tosink = %4f\n", 
		this->addr, this->x, this->y, this->energy, this->ch_addr, this->next_hop, this->d_tosink);
}