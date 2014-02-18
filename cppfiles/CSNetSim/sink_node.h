#ifndef SINKNODE_H
#define SINKNODE_H

#include "compile_config.h"

#include "energy_model.h"
#include "network.h"
#include "comm_proxy.h"
#include "node.h"
#include "msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

class Network;

class SinkNode
{
public:
	SinkNode();
	~SinkNode();
	
	void onTimeOut();
	void startFloodRoute();

public:
	Network* network;

public:
	//coordinate of sink
	static double SINK_X;
	static double SINK_Y;
	static int SINK_ADDR;

};

#endif // SINKNODE_H
