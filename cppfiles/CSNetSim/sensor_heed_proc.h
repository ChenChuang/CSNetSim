#ifndef SENSORHEEDPROC_H
#define SENSORHEEDPROC_H

#include "csnetsim.h" // Base class: Processor
#include "clustering_member.h"
#include "clustering_neighbor.h"
#include "clustering_comm_proxy.h"
#include "broadcast_channel.h"

class INode_SensorHeedProc;
struct Scost;

class SensorHeedProc : public Processor
{
public:
	SensorHeedProc(Node* anode);
	~SensorHeedProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
public:
	int proc_clustering();
	
	void cal_broadcast_cost();
	void receive_cost_msg(struct Msg* mp);
	void receive_ch_msg(struct Msg* mp);
	void receive_join_msg(struct Msg* mp);
	
	int get_least_cost_ch();
	int get_least_cost_final_ch();
	
	void cluster_head_msg();
	void join_cluster(int ch_addr);
	
	bool has_sch();
	void set_scost_type(int addr, char ch_state);
	void add_scost(int addr, double cost);
	void add_member(int addr);
	
	double calAMRP();

public:
	static const char PROC_OFF = 0x01;
	static const char PROC_GETREADY = 0x02;
	static const char PROC_MAIN = 0x03;
	static const char PROC_FINAL = 0x04;
	
	static const char NOT_CH = 0x01;
	static const char TENT_CH = 0x02;
	static const char FINAL_CH = 0x03;
	
	static const char CMD_COST = 0x11;
	static const char CMD_CH = 0x12;
	static const char CMD_JOIN = 0x13;
public:
	double c_prob;
	double p_min;
	
private:
	Node* node;
	INode_SensorHeedProc* inode;	
	SortedList<Scost>* costs;

	char proc_state;
	char ch_type;
	double ch_prob;
	double ch_prob_pre;
	int heed_count;
};

class INode_SensorHeedProc
{
public:
	virtual MnManager* get_mnmanager() = 0;
	virtual int get_ch_addr() = 0;
	virtual void set_ch_addr(int addr) = 0;
	virtual bool is_ch() = 0;
	virtual void start_route() = 0;
	virtual NgbManager* get_neighbors() = 0;
};

struct Scost
{
public:
	int addr;
	double cost;
	char type;
public:
	Scost(int aaddr, double acost, char atype) : addr(aaddr), cost(acost), type(atype) {}
	~Scost() {}
public:
	bool operator <(const Scost& a) const{
		return cost < a.cost;
	}
	bool operator ==(const Scost& a) const{
		return cost == a.cost;
	}
	bool is(const Scost& a) const{
		return addr == a.addr;
	} 
};

#endif // SENSORHEEDPROC_H
