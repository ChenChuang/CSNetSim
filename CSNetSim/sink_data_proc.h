#ifndef SINKDATAPROC_H
#define SINKDATAPROC_H

#include "csnetsim.h" // Base class: Processor
#include "sensor_data_proc.h"

class SinkDataProc : public Processor
{
public:
	SinkDataProc();
	~SinkDataProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
public:
	double get_data_l() {return this->data_l;}
public:
	double data_l;
};

#endif // SINKDATAPROC_H
