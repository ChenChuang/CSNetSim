#ifndef BASENETWORK_H
#define BASENETWORK_H

#include "compile_config.h"
#ifdef _vc_
	#include "adj_g.cpp"
#else
	#include "adj_g.h"
#endif

#include "base_node.h"
#include "sink_node.h"
#include "energy_model.h"
#include "monitor.h"
#include "msg.h"
#include <stdio.h>
#include <cmath>

class BaseNode;
class Monitor;
class SinkNode;

class BaseNetwork
{
public:
	BaseNetwork(double* x, double* y);
	~BaseNetwork();
	
public:
	//max sim time of main loop
	static double MAX_SIM_TIME;
	//num of nodes
	static int NODE_NUM;
	
	//timer of main loop
	static double sim_timer;
	static double sim_slot;
	
	//all nodes of this WSN
	BaseNode** nodes;
	//sink node
	SinkNode* sink;
	
	//AdjG under cluster_radius and max_radius
	AdjG* cluster_radius_G;
	AdjG* max_radius_G;
	
	//monitor of this WSN
	Monitor* monitor;
	
public:
	//implement virtual
	void run();
	//implement virtual
	int communicate();
	//using coordinate x, y to create this->nodes
	int createNodes(double* x, double* y);
	//refresh AdjG, some nodes may be out of energy and should be deleted from AdjG
	void clear_dead_from_AdjG();
	//transfer the pointer of Adjv to all nodes' processor. some algorithm assumes that node is aware of topology around it
	void setAdjv();
	//some of the network's params are accessible to all nodes, such as the time
	void setNetwork();
	//check whether two nodes are in each other's cluster range
	bool is_cluster_neighbor(int addr1, int addr2);
	
friend class Monitor;
};

#endif // BASENETWORK_H
