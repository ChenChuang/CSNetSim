#ifndef SENSORROUTEPROC_H
#define SENSORROUTEPROC_H

#include "csnetsim.h" // Base class: Processor
#include "clustering_comm_proxy.h"

struct Sch;
class INode_SensorRouteProc;

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
	int get_best_ch();
	
public:
	static const char PROC_OFF = 0x01;
	static const char PROC_GETREADY = 0x02;
	static const char PROC_CHOOSE = 0x03;
	
	static const char CMD_CH = 0x11;
public:
	double min_tick;
private:
	Node* node;
	INode_SensorRouteProc* inode;
	SortedList<Sch>* chs;
	
	char proc_state;
};

class INode_SensorRouteProc
{
public:
	virtual int get_ch_addr() = 0;
	virtual int get_next_hop() = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
	virtual double get_d_tosink() = 0;
	virtual double get_neighbor_d(int addr) = 0;
};

struct Sch
{
public:
	int addr;
	double d_tosink;
	double d;
public:
	Sch(int aaddr, double ad_tosink, char ad) : addr(aaddr), d_tosink(ad_tosink), d(ad) {}
	~Sch() {}
public:
	bool operator <(const Sch& a) const{
		return d_tosink + d < a.d_tosink + a.d;
	}
	bool operator ==(const Sch& a) const{
		return d_tosink + d == a.d_tosink + a.d;
	}
	bool is(const Sch& a) const{
		return addr == a.addr;
	} 
};

#endif // SENSORROUTEPROC_H
