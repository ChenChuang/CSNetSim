#ifndef SENSORROUTEPROC_H
#define SENSORROUTEPROC_H

#include "csnetsim.h" // Base class: Processor

class SensorRouteProc : public Processor
{
public:
	SensorRouteProc(Node* anode);
	~SensorRouteProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
public:
	void start_route();
	
private:
	Node* node;
};

class INode_SensorRouteProc
{
public:
	virtual int get_next_hop() = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
};

#endif // SENSORROUTEPROC_H
