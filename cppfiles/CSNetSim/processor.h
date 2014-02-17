#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "msg.h"

class Processor
{
public:
	Processor();
	~Processor();
public:
	virtual void ticktock(double time) {}
	virtual int process(Msg* msg) {return -1;}
	virtual void init() {}
};

#endif // PROCESSOR_H
