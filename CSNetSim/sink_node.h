#ifndef SINKNODE_H
#define SINKNODE_H

#include "csnetsim.h"
#include "clustering_sim_model.h"
#include "sink_comm_proxy.h"
#include "test_proc.h"
#include "sink_data_proc.h"

class SinkNode : public Node
{
public:
	SinkNode(Network* anetwork, int aaddr, double ax, double ay);
	~SinkNode();
public:
	void consume(double e) {}
	bool is_alive() {return true;}

public:
	TestProc* testproc;
	SinkDataProc* dataproc;
};

#endif // SINKNODE_H
