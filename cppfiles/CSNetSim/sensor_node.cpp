#include "snesor_node.h"

SensorNode::SensorNode(Network* anetwork, int aaddr, double ax, double ay, double energy): 
	Node(anetwork, aaddr, ax, ay, energy), 
	mnmanager(new MnManager(this)),
	d_tosink(sqrt(pow(ax - SinkNode::SINK_X, 2) + pow(ay - SinkNode::SINK_Y, 2))),
	ch_addr(-1),
	next_hop(-1)
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