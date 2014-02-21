#ifndef SINKROUTEPROC_H
#define SINKROUTEPROC_H

#include "processor.h" // Base class: Processor

class SinkRouteProc : public Processor
{
public:
	SinkRouteProc();
	~SinkRouteProc();

public:
	virtual void init();
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
};

#endif // SINKROUTEPROC_H
