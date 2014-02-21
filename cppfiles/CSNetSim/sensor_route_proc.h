#ifndef SENSORROUTEPROC_H
#define SENSORROUTEPROC_H

#include "processor.h" // Base class: Processor

class SensorRouteProc : public Processor
{
public:
	SensorRouteProc();
	~SensorRouteProc();

public:
	virtual void init();
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
};

class INode_SensorRouteProc
{
public:
	virtual int get_next_hop() = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
};

#endif // SENSORROUTEPROC_H
