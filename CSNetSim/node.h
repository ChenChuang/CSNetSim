#ifndef BASENODE_H
#define BASENODE_H

#include "compile_config.h"

#include "comm_proxy.h"
#include "processor.h"
#include "msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

class Network;

class Node
{
public:
	Node(Network* anetwork, int aaddr, double ax, double ay, double aenergy);
	virtual ~Node();

public:
	virtual void ticktock();
	virtual void print();
	virtual void init();
	virtual void clear();
public:
	virtual CommProxy* get_commproxy() {return this->commproxy;}
	virtual Network* get_network() {return this->network;}

	virtual int get_addr() {return this->addr;}
	virtual bool is_alive() {return this->energy > 0;}
	virtual void consume(double e) {this->energy -= e;}
	
public:
	Network* network;
	CommProxy* commproxy;
	ProcsManager* procs_manager;
	
	int addr;
	double x;
	double y;
	double energy;

friend class Monitor;
};

#endif // BASENODE_H
