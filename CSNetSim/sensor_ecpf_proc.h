#ifndef SENSORECPFPROC_H
#define SENSORECPFPROC_H

#include <fl/Headers.h>

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
class INet_SensorEcpfProc;

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
	void start_check();
	double start_clustering();
	void exit_clustering();
	int proc_clustering();
	
	void receive_ch_msg(struct Msg* mp);
	void receive_join_msg(struct Msg* mp);
	void receive_need_msg();
	
	int get_least_cost_ch();
	int get_least_cost_final_ch();
	
	void broadcast_ch_msg();
	void join_cluster(int ch_addr);
	
	bool has_sch();
	bool has_finals();
	void add_tent(int addr, char type, double cost);
	void add_member(int addr);
	
	double calDelay();
	double calFuzzyCost();
	
	bool check_energy();

public:
	static const char PROC_SLEEP = 0x01;
	static const char PROC_CHECK = 0x02;
	
	static const char PROC_GETREADY = 0x03;
	static const char PROC_WAIT = 0x04;
	static const char PROC_MAIN = 0x05;
	static const char PROC_FINAL = 0x06;
	static const char PROC_DONE = 0x07;
	
	static const char NOT_CH = 0x01;
	static const char TENT_CH = 0x02;
	static const char FINAL_CH = 0x03;
	
	static const char CMD_CH = 0x62;
	static const char CMD_JOIN = 0x63;
	static const char CMD_NEED = 0x64;
	static const char CMD_CLUSTER = 0x65;
	
public:
	double energy_thrd;
	int max_main_iter;
	double max_wait_self_time;
	
	double ecpf_time;
	double check_time;
	double route_time;
	double stable_time;
	
	double min_tick;
	double main_iter_tick;
	
private:
	Node* node;
	INode_SensorEcpfProc* inode;
	INet_SensorEcpfProc* inetwork;
	Clock* clock;
	ClusteringCommProxy* comm;

	char proc_state;
	char ch_type;
	double ch_prob;
	double ch_prob_pre;
	int ecpf_count;
	
	double energy_pre;
	bool is_cluster;
	double fuzzycost;
	int main_iter;
	
	SortedList<ecpf::Tent>* tents;
	Timer* timer;
	Timer* wait_timer;
};

class INode_SensorEcpfProc
{
public:
	virtual MnManager* get_mnmanager() = 0;
	virtual int get_ch_addr() = 0;
	virtual void set_ch_addr(int addr) = 0;
	virtual int get_next_hop() = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
	virtual void start_route() = 0;
	virtual void stop_route() = 0;
	virtual NgbManager* get_neighbors() = 0;
};

class INet_SensorEcpfProc
{
public:
	virtual double d_between(int addr1, int addr2) = 0;
	virtual bool is_alive(int addr) = 0;
};

namespace ecpf
{
	class FuzzyCostComputor
	{
	public:
		fl::Engine* engine;
		fl::InputVariable* degree;
		fl::InputVariable* centrality;
		fl::OutputVariable* cost;
		fl::RuleBlock* rules;
	public:
		FuzzyCostComputor();
		~FuzzyCostComputor();
		double cal(double centrality, double degree);
	};
	extern FuzzyCostComputor* fcc;
};

#endif // SENSORECPFPROC_H
