#ifndef SENSORHEEDPROC_H
#define SENSORHEEDPROC_H

#include "csnetsim.h" // Base class: Processor
#include "clustering_member.h"

class SensorHeedProc : public Processor
{
public:
	SensorHeedProc(Node* anode);
	~SensorHeedProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
	
private:
	Node* node;
};

class INode_SensorHeedProc
{
	virtual MnManager* get_mnmanager() = 0;
	virtual int get_ch_addr() = 0;
	virtual void set_ch_addr(int addr) = 0;
	virtual bool is_ch() = 0;
	virtual void start_route() = 0;
};

#endif // SENSORHEEDPROC_H
