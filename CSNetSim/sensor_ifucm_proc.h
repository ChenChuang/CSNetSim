#ifndef SENSORIFUCMPROC_H
#define SENSORIFUCMPROC_H

#include "csnetsim.h" // Base class: Processor
#include "clustering_member.h"
#include "clustering_neighbor.h"
#include "clustering_comm_proxy.h"
#include "broadcast_channel.h"

namespace ifucm
{
struct Tent
{
public:
	int addr;
	double cost;
public:
	Tent(int addr, double cost) : addr(addr), cost(cost) {}
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

class INode_SensorIfucmProc;

class SensorIfucmProc : public Processor
{
public:
	SensorIfucmProc(Node* node);
	~SensorIfucmProc();
public:
	void init();
	int process(Msg* msg);
	void ticktock(double time);
public:
	double start_clustering();
	void exit_clustering();
	int proc_clustering();
    
	void cal_radius_cost();
    void broadcast_compete_msg();
	void receive_compete_msg(struct Msg* msg);
    void broadcast_quit_msg();
    void receive_quit_msg(struct Msg* msg);
    void broadcast_ch_msg();
	void receive_ch_msg(struct Msg* msg);
    void join_cluster(int ch_addr);
	void receive_join_msg(struct Msg* msg);
	
	int get_least_cost_tent();
	
	void add_tent(int addr, double cost);
	void add_member(int addr);

public:
	static const char PROC_SLEEP = 0x01;
	static const char PROC_GETREADY = 0x02;
	static const char PROC_TENT = 0x03;
    static const char PROC_NOTTENT = 0x04;
	static const char PROC_FINAL = 0x05;
	static const char PROC_DONE = 0x06;
	
	static const char CMD_COMPETE = 0x91;
    static const char CMD_QUIT = 0x92;
	static const char CMD_CH = 0x93;
	static const char CMD_JOIN = 0x94;
public:
    double tent_p;

	double ifucm_time;
	double route_time;
	double stable_time;
	
	double min_tick;
	
private:
	Node* node;
	INode_SensorIfucmProc* inode;	
	SortedList<ifucm::Tent>* tents;

	char proc_state;
	double cost;
    double radius;
    bool is_quit;    

	Timer* timer;
};

class INode_SensorIfucmProc
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

#endif // SENSORIFUCMPROC_H
