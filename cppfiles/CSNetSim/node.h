#ifndef BASENODE_H
#define BASENODE_H

#include "compile_config.h"

#include "energy_model.h"
#include "network.h"
#include "comm_proxy.h"
#include "heed_proc.h"
#include "edcr_proc.h"
#include "msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

class Node
{
public:
	Node(Network* anetwork, int aaddr, double ax, double ay, double aenergy);
	virtual ~Node();

public:
	virtual void on_time_out();
	virtual void print();
public:
	virtual CommProxy* commproxy() {return this->commproxy;}
	virtual Network* network() {return this->network;}

	virtual int get_addr() {return this->addr;}
	virtual bool is_alive() {return this->energy <= 0;}
	virtual void consume(double e) {this->energy -= e;}
	
public:
	double energy;
	int addr;
	double x;
	double y;

	CommProxy* commproxy;
	Network* network;
	ProcsManager* procs_manager;

friend class Monitor;
};

#endif // BASENODE_H
