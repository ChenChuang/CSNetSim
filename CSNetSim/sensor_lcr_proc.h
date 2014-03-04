#ifndef SENSORLCRPROC_H
#define SENSORLCRPROC_H

#include "csnetsim.h"
#include "clustering_neighbor.h"
#include "clustering_member.h"
#include "clustering_comm_proxy.h"

namespace lcr
{
	struct Sch;
	struct TentParam;
	struct Tent;
}

class INode_SensorLcrProc;
class INet_SensorLcrProc;

class SensorLcrProc : public Processor
{
public:
	SensorLcrProc(Node* anode);
	~SensorLcrProc();
public:
	virtual void init();
	virtual int process(Msg* msg);
	virtual void ticktock(double time);
public:
	void start();
	void start_rotating();
	void start_init_chs();
private:
	bool check_ch_alive();
	bool check_nexthop_alive();
	bool check_energy();
	
	void add_tent_param(int addr, double d, double e, double fcd, NgbManager* ngbs);
	void add_ch(int addr, double d_tosink, double anc_energy);
	void remove_ch(int addr);
	
	double cal_fcd();
	void cal_tent_costs();
	int get_least_cost_newch();
	int get_least_cost_nexthop();
	int get_best_ch();
	
	void ch_send_query();
	void mn_receive_query(int addr);
	void ch_receive_resp(int addr, double* params);
	
	void ch_send_assign(int addr);
	void mn_receive_assign(int addr, int newch);
	
	void newch_send_anc();
	void receive_newch_anc(int addr, double* params);
	
	void newmn_send_anc();
	void receive_newmn_anc(int addr);
	
	void send_join_newch();
	void send_join();
	void receive_join(int addr);
	
	void reset_next_hop();
	
public:
	static const char PROC_SLEEP = 0x01;
	static const char PROC_WAIT_CLUSTERING = 0x02;
	static const char PROC_INIT_CHS = 0x03;
	
	static const char PROC_MN_CHECK = 0x11;
	static const char PROC_MN_WAIT_NEWCH = 0x12;
	
	static const char PROC_CH_CHECK = 0x21;
	static const char PROC_CH_WAIT_TENTS = 0x22;
	static const char PROC_CH_WAIT_NEWCH = 0x23;

	static const char PROC_NEWCH_WAIT_MN = 0x31;
	static const char PROC_ISOLATED = 0x32;
	static const char PROC_ISOLATED_WAIT_SELF = 0x33;
	
	static const char CMD_CH_QUERY = 0x71;
	static const char CMD_MN_RESP = 0x72;
	static const char CMD_CH_ASSIGN = 0x73;
	
	static const char CMD_NEWCH_ANC = 0x74;
	static const char CMD_NEWMN_ANC = 0x75;
	
	static const char CMD_JOIN = 0x76;

	
public:
	double clustering_time;
	double init_chs_time;
	double lcr_time;
	double ch_wait_resp_time;
	double mn_wait_newch_time;
	double ch_wait_newch_time;
	double newch_wait_mn_time;
	double max_wait_self_time;
	double max_wait_hop_time;
	double min_wait_hop_time;
	double energy_thrd;
	double energy_thrd_2;
	double energy_thrd_3;
	double energy_thrd_4;
	
private:
	Node* node;
	INode_SensorLcrProc* inode;
	ClusteringCommProxy* comm_proxy;
	INet_SensorLcrProc* inetwork;
	
	SortedList<lcr::Sch>* chs;
	SortedList<lcr::TentParam>* tent_params;
	SortedList<lcr::Tent>* tents;
	
	Timer* wait_clustering_timer;
	Timer* lcr_timer;
	Timer* wait_resp_timer;
	Timer* wait_newch_timer;
	Timer* wait_mn_timer;
	Timer* wait_self_timer;
	Timer* wait_hop_timer;
	
	double energy_pre;
	int newch;

	char proc_state;

};

namespace lcr
{
struct Sch
{
public:
	int addr;
	double d;
	double d_tosink;
	long anc_time;
	double anc_energy;
	double cost;
public:
	Sch(int addr, double d, double d_tosink, double anc_time, double anc_energy) :
		addr(addr), d(d), d_tosink(d_tosink), anc_time(anc_time), anc_energy(anc_energy)
	{
		//this->cost = sqrt( pow(d_tosink, (d_tosink > EnergyModel::D_THRESHOLD ? 4:2)) + pow(d, 2) );
		this->cost = sqrt( pow(d_tosink, 2) + pow(d, 2) );
	}
	~Sch() {}
public:
	bool operator <(const Sch& a) const{
		return cost < a.cost;
	}
	bool operator ==(const Sch& a) const{
		return cost == a.cost;
	}
	bool is(const Sch& a) const{
		return addr == a.addr;
	} 
};

struct TentParam
{
public:
	int addr;
	double d;
	double e;
	double fcd;
	NgbManager* ngbs;
	double fc;
public:
	TentParam(int addr, double d, double e, double fcd, NgbManager* ngbs) :
		addr(addr), d(d), e(e), fcd(fcd), ngbs(ngbs), fc(0) {}
	~TentParam() {}
public:
	bool operator <(const TentParam& a) const{
		return d < a.d;
	}
	bool operator ==(const TentParam& a) const{
		return d == a.d;
	}
	bool is(const TentParam& a) const{
		return addr == a.addr;
	}
};

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

class INode_SensorLcrProc
{
public:
	virtual void start_cluster_route() = 0;
	virtual void exit_cluster_route() = 0;
	virtual MnManager* get_mnmanager() = 0;
	virtual int get_ch_addr() = 0;
	virtual void set_ch_addr(int addr) = 0;
	virtual int get_next_hop() = 0;
	virtual void set_next_hop(int addr) = 0;
	virtual bool is_ch() = 0;
	virtual void start_route() = 0;
	virtual NgbManager* get_neighbors() = 0;
	virtual double get_d_tosink() = 0;
	virtual double get_data() = 0;
	virtual void force_send_data() = 0;
};

class INet_SensorLcrProc
{
public:
	virtual double d_between(int addr1, int addr2) = 0;
	virtual bool is_alive(int addr) = 0;
};

#endif // SENSORLCRPROC_H
