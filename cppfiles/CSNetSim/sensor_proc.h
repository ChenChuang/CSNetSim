#ifndef SENSOR_PROC_H
#define SENSOR_PROC_H

#include "processor.h" // Base class: Processor
#include "timer.h"

class Sensor_Proc : public Processor
{
public:
	Sensor_Proc(BaseNode* anode);
	~Sensor_Proc();
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
	BaseNode* node;
	Timer* sense_timer;
	Timer* wait_timer;
};

#endif // SENSOR_PROC_H
