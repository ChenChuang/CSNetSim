#ifndef BASENETWORK_H
#define BASENETWORK_H

#include "compile_config.h"
#ifdef _vc_
	#include "adj_g.cpp"
#else
	#include "adj_g.h"
#endif

#include "node.h"
#include "sink_node.h"
#include "energy_model.h"
#include "time_manager.h"
#include "monitor.h"
#include "msg.h"
#include <stdio.h>
#include <cmath>

class Node;
class Monitor;
class SinkNode;

class Network
{
public:
	Network(double* x, double* y);
	~Network();
public:
	void run();
	int communicate();
	Node* get_node(int addr);
	Monitor* get_monitor() {return this->monitor;}
	bool is_alive(int addr);
	double get_time();
	void try_set_tick(double tick);
	
public:
	static double MAX_SIM_TIME;
	static int NODE_NUM;
	
	TimeManager* clock;

	Node** nodes;

	AdjG* cluster_radius_G;
	AdjG* max_radius_G;

	Monitor* monitor;
		
friend class Monitor;
};

#endif // BASENETWORK_H
