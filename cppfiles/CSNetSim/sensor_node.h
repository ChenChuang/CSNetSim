#ifndef SENSORNODE_H
#define SENSORNODE_H

#include "node.h" // Base class: Node
#include "incluster_channel.h" // Base class: INode_InclusterChannel

class SensorNode : public Node, public INode_InclusterChannel
{
public:
	SensorNode(Network* anetwork, int aaddr, double ax, double ay, double aenergy);
	~SensorNode();
public:
	void print();
public:
	MnManager* mnmanager() {return this->mnmanager;}
	int get_ch_addr() {return this->ch_addr;}
	void set_ch_addr(int addr) {this->ch_addr = addr;}
	int get_next_hop() {return this->next_hop;}
	void set_next_hop(int addr) {this->next_hop = addr;}
	
public:
	MnManager* mnmanager;	
	int ch_addr;
	int next_hop;
	double d_tosink;
	
friend class Monitor;
};

#endif // SENSORNODE_H
