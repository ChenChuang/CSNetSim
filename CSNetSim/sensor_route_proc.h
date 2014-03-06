#ifndef SENSORROUTEPROC_H
#define SENSORROUTEPROC_H

#include "csnetsim.h" // Base class: Processor
#include "clustering_comm_proxy.h"

struct Sch;
class INode_SensorRouteProc;
class INet_SensorRouteProc;

class SensorRouteProc : public Processor
{
public:
	SensorRouteProc(Node* anode);
	~SensorRouteProc();
public:
	virtual void init();
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
public:
	virtual void start_route();
	virtual int get_best_ch();
	
public:
	static const char PROC_SLEEP = 0x01;
	static const char PROC_GETREADY = 0x02;
	static const char PROC_CHOOSE = 0x03;
	
	static const char CMD_CH = 0x11;
public:
	double min_tick;
public:
	Node* node;
	INode_SensorRouteProc* inode;
	INet_SensorRouteProc* inetwork;
	SortedList<Sch>* chs;
	
	char proc_state;
	double radius;
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

class INet_SensorRouteProc
{
public:
	virtual bool is_alive(int addr) = 0;
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
#ifndef _IFUCM_
		return d_tosink + d < a.d_tosink + a.d;
#else
		return d_tosink + d < a.d_tosink + a.d;
		//return d < a.d;
#endif
	}
	bool operator ==(const Sch& a) const{
#ifndef _IFUCM_
		return d_tosink + d == a.d_tosink + a.d;
#else
		return d_tosink + d < a.d_tosink + a.d;
		//return d < a.d;
#endif
	}
	bool is(const Sch& a) const{
		return addr == a.addr;
	} 
};

#endif // SENSORROUTEPROC_H
