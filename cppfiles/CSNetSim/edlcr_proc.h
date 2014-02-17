#ifndef EDLCR_PROC_H
#define EDLCR_PROC_H

#include "processor.h" // Base class: Processor

class EDLCR_Proc : public Processor
{
public:
	EDLCR_Proc();
	~EDLCR_Proc();

public:
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
};

#endif // EDLCR_PROC_H
