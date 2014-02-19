#ifndef SINKNODE_H
#define SINKNODE_H

#include "csnetsim.h"
#include "clustering_sim_model.h"

class SinkNode : public Node
{
public:
	SinkNode(Network* anetwork, int aaddr, double ax, double ay);
	~SinkNode();

public:
	void consume(double e) {}
	bool is_alive() {return true;}
	void on_time_out();
	void print();
};

#endif // SINKNODE_H
