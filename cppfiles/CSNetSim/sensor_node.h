#ifndef SENSORNODE_H
#define SENSORNODE_H

#include "csnetsim.h"
#include "incluster_channel.h" // Base class: INode_InclusterChannel
#include "clustering_member.h"
#include "clustering_comm_proxy.h"
#include "clustering_sim_model.h"
#include "sensor_proc.h"
#include "test_proc.h"

class SensorNode : public Node, public INode_InclusterChannel, public INode_SensorProc
{
public:
	SensorNode(Network* anetwork, int aaddr, double ax, double ay, double aenergy);
	~SensorNode();
public:
	void print();
public:
	MnManager* get_mnmanager() {return this->mnmanager;}
	int get_ch_addr() {return this->ch_addr;}
	void set_ch_addr(int addr) {this->ch_addr = addr;}
	int get_next_hop() {return this->next_hop;}
	void set_next_hop(int addr) {this->next_hop = addr;}
	bool is_ch() {return this->ch_addr == this->addr;}
	
public:
	MnManager* mnmanager;	
	int ch_addr;
	int next_hop;
	double d_tosink;
	
	Processor* testproc;
	
friend class Monitor;
friend class ClusteringMonitor;
};

#endif // SENSORNODE_H
