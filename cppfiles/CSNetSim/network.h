#ifndef NETWORK_H
#define NETWORK_H

#include "compile_config.h"

#include "channel.h"
#include "monitor.h"
#include "node.h"
#include "processor.h"

#include "msg.h"
#include "clock.h"
#include "timer.h"
#include "comm_proxy.h"
#include "adj_g.h"

#include "energy_model.h"

#include <stdio.h>
#include <cmath>

class Network
{
public:
	Network(double* x, double* y, int num, double max_time, double default_tick);
	virtual ~Network();
public:
	virtual bool check();
	virtual void run();
	virtual void communicate();
public:
	Node* node(int addr) {return this->nodes[addr];}
	Monitor* get_monitor() {return this->monitor;}
	Clock* get_clock() {return this->clock;}
	
public:
	Clock* clock;
	Node** nodes;
	int nodes_num;
	ChannelsManager* channels;
	Monitor* monitor;
	double max_time;
		
friend class Monitor;
};

#endif // NETWORK_H
