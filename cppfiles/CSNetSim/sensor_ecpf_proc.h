#ifndef SENSORECPFPROC_H
#define SENSORECPFPROC_H

#include "csnetsim.h" // Base class: Processor
#include "clustering_member.h"
#include "clustering_neighbor.h"
#include "clustering_comm_proxy.h"
#include "broadcast_channel.h"

namespace ecpf
{
struct Tent
{
public:
	int addr;
	double cost;
	char type;
public:
	Tent(int aaddr, double acost, char atype) : addr(aaddr), cost(acost), type(atype) {}
	~Tent() {}
public:
	bool operator <(const Tent& a) const{
		return cost < a.cost;
	}
	bool operator ==(const Tent& a) const{
		return cost == a.cost;
	}
	bool is(const Tent& a) const{
		return addr == a.addr;
	} 
};
}

class INode_SensorEcpfProc;

class SensorEcpfProc : public Processor
{
public:
	SensorEcpfProc(Node* anode);
	~SensorEcpfProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
public:
	double start_clustering();
	void exit_clustering();
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
	void set_tent_type(int addr, char ch_state);
	void add_tent(int addr, double cost);
	void add_member(int addr);
	
	double calAMRP();

public:
	static const char PROC_SLEEP = 0x01;
	static const char PROC_GETREADY = 0x02;
	static const char PROC_MAIN = 0x03;
	static const char PROC_FINAL = 0x04;
	static const char PROC_DONE = 0x05;
	
	static const char NOT_CH = 0x01;
	static const char TENT_CH = 0x02;
	static const char FINAL_CH = 0x03;
	
	static const char CMD_COST = 0x61;
	static const char CMD_CH = 0x62;
	static const char CMD_JOIN = 0x63;
public:
	double c_prob;
	double p_min;
	double ecpf_time;
	double stable_time;
	double min_tick;
	
private:
	Node* node;
	INode_SensorEcpfProc* inode;	
	SortedList<ecpf::Tent>* tents;

	char proc_state;
	char ch_type;
	double ch_prob;
	double ch_prob_pre;
	int ecpf_count;
	
	Timer* timer;
};

class INode_SensorEcpfProc
{
public:
	virtual MnManager* get_mnmanager() = 0;
	virtual int get_ch_addr() = 0;
	virtual void set_ch_addr(int addr) = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
	virtual void start_route() = 0;
	virtual void stop_route() = 0;
	virtual NgbManager* get_neighbors() = 0;
};

#endif // SENSORECPFPROC_H
