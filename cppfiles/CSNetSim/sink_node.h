#ifndef SINKNODE_H
#define SINKNODE_H

#include "compile_config.h"

#include "energy_model.h"
#include "base_network.h"
#include "base_comm_proxy.h"
#include "base_node.h"
#include "msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

class BaseNetwork;

class SinkNode
{
public:
	SinkNode();
	~SinkNode();
	
	void onTimeOut();
	void startFloodRoute();

public:
	BaseNetwork* network;

public:
	//coordinate of sink
	static double SINK_X;
	static double SINK_Y;
	static int SINK_ADDR;

};

#endif // SINKNODE_H
