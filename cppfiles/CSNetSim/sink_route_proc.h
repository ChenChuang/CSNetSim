#ifndef SINKROUTEPROC_H
#define SINKROUTEPROC_H

#include "csnetsim.h" // Base class: Processor

class SinkRouteProc : public Processor
{
public:
	SinkRouteProc(Node* anode);
	~SinkRouteProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
	void start_route();

private:
	Node* node;
};

#endif // SINKROUTEPROC_H
