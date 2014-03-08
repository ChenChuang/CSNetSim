#ifndef SensorDataProc_H
#define SensorDataProc_H

#include "csnetsim.h"
#include "clustering_sim_model.h"
#include "broadcast_channel.h"
#include "unicast_channel.h"

class INode_SensorDataProc;

class SensorDataProc : public Processor
{
public:
	SensorDataProc(Node* anode, double aperiod, double amax_wait, double aunit);
	~SensorDataProc();
public:
	virtual void init();
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
	bool send(double size, double l, char cmd);
	void send_fused();
	void send_unfused();
	void force_send();
public:
	double period;
	double max_wait;
	double max_wait_hop;
	double unit;
	static const char CMD_SENSE_DATA_FUSED = 0x01;
	static const char CMD_SENSE_DATA_UNFUSED = 0x02;
public:
	double comp;
	double fused;
	double unfused;
	Node* node;
	INode_SensorDataProc* inode;
	ECommProxy_UnicastChannel* comm;
	Timer* sense_timer;
	Timer* wait_timer;
	Timer* wait_hop_timer;
};

class INode_SensorDataProc
{
public:
	virtual int get_next_hop() = 0;
	virtual bool is_ch() = 0;
	virtual double get_d_tosink() = 0;
};

#endif // SensorDataProc_H
