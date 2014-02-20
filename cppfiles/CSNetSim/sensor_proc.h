#ifndef SENSORPROC_H
#define SENSORPROC_H

#include "csnetsim.h"
#include "clustering_sim_model.h"
#include "broadcast_channel.h"
#include "unicast_channel.h"

class SensorProc : public Processor
{
public:
	SensorProc(Node* anode, double aperiod, double aunit_l, double abuf_l);
	~SensorProc();
public:
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
	void send(char cmd);
	void sense();
public:
	double period;
	double unit_l;
	double buf_l;
	static const char CMD_SENSE_DATA_FUSED = 0x91;
	static const char CMD_SENSE_DATA_UNFUSED = 0x92;
private:
	double data_l;
	Node* node;
	Timer* sense_timer;
	Timer* wait_timer;
};

class INode_SensorProc
{
public:
	virtual int get_next_hop() = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
};

#endif // SENSORPROC_H
