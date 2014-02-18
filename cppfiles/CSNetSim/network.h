#ifndef NETWORK_H
#define NETWORK_H

#include "compile_config.h"
#ifdef _vc_
	#include "adj_g.cpp"
#else
	#include "adj_g.h"
#endif

#include "node.h"
#include "sink_node.h"
#include "energy_model.h"
#include "clock.h"
#include "monitor.h"
#include "msg.h"
#include <stdio.h>
#include <cmath>

class Network
{
public:
	Network(double* x, double* y, int num);
	virtual ~Network();
public:
	virtual void run();
	virtual void communicate();
	Node* node(int addr) {return this->nodes[addr];}
	Monitor* monitor() {return this->monitor;}
	Clock* clock() {return this->clock;}
	
public:
	Clock* clock;
	Node** nodes;
	int nodes_num;
	ChannelsManager* channels;
	Monitor* monitor;
		
friend class Monitor;
};

#endif // NETWORK_H
