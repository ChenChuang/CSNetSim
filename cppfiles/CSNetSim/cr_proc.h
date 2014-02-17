#ifndef CR_PROC_H
#define CR_PROC_H

#include "base_node.h"
#include "Adjv.h"
#include <cmath>

struct NEIGHBOR
{
	int addr;
	double d;
	double d_tosink;
	bool is_youth;
	long announce_time;
	double announce_energy;
	struct NEIGHBOR* next;
};

struct SCOST_PARAM
{
	int addr;
	double d;
	double e;
	int coverage;
	struct SCOST_PARAM* next;
};

class CR_Proc
{
public:
	CR_Proc(BaseNode* node, BaseCommProxy* commproxy);
	~CR_Proc();
	void set_cluster_Adjv(struct Adjv* a);
	void set_max_Adjv(struct Adjv* a);

public:
	void onTimeOut();
	int procMsg(struct msg* mp);
	
	void init();
	
	bool check_ch_alive();
	bool check_energy();
	double cal_cost();
	double* cal_cost2();
	int find_best_youth();
	int find_best_next_hop();
	int find_best_ch();
	int find_best_ch_d();
	int find_assigned_or_best_ch_d();
	
	void inclustercast_query_cost();
	void receive_query_cost(int addr);
	void receive_respond_cost(int addr, double cost);
	void receive_respond_cost2(int addr, double* costa);
	
	bool is_youth_needed();
	void set_scost();
	
	void unicast_assign_youth(int addr);
	void receive_assign_youth(int addr);
	
	void maxbroadcast_youth();
	void receive_announce_youth(int addr, double energy);
	
	void maxbroadcast_old();
	void receive_announce_old(int addr);
	
	void unicast_req_new_ch(int addr);
	void unicast_req_new_ch_incluster(int addr);
	void receive_req_new_ch(int addr);
	
	void change_next_hop();
	
	void change_neighbor(int addr, bool is_youth, double energy);
	void sync_neighbor_adj();
	
	bool try_to_become_baby();
	void become_youth();

	void print_neighbors();
	void clear_scost();
	void clear_scost_param();
public:
	static const char LIFE_BABY = 0x01;
	static const char LIFE_YOUTH = 0x02;
	static const char LIFE_OLD = 0x03;
	static const char LIFE_INIT = 0x04;
	static const char LIFE_ELECT = 0x05;
	
	static const char PROC_INIT = 0x01;
	static const char PROC_WAITING_RESPOND_COST = 0x02;
	static const char PROC_RESPOND_COST_RECEIVED = 0x03;
	static const char PROC_WAITING_NEW_CH = 0x04;
	static const char PROC_REQUESTING_NEW_CH = 0x05;
	static const char PROC_WAITING_NEW_BABY = 0x06;
	
	static const char CMD_QUERY_COST = 0x21;
	static const char CMD_RESPOND_COST = 0x22;
	static const char CMD_ASSIGN_YOUTH = 0x23;
	
	static const char CMD_ANNOUNCE_YOUTH = 0x24;
	static const char CMD_ANNOUNCE_OLD = 0x25;
	
	static const char CMD_REQ_NEW_CH = 0x26;
	//static const char CMD_ACCEPT_REQ_NEW_CH = 0x27;
	//static const char CMD_REJECT_REQ_NEW_CH = 0x28;
	
	static const int MAX_OLD_TIME = 1;
	static const int MAX_YOUNG_TIME = 3;
	
	static const int D_TOSINK_THRESHOLD = 40;
	
	char life_state;
	char proc_state;
	
	BaseNetwork* network;
	
	BaseNode* node;
	BaseCommProxy* commproxy;
	
	struct Adjv* cluster_Adjv;
	struct Adjv* max_Adjv;
	
	struct NEIGHBOR* neighbors;
	
	struct SCOST* scost;
	struct SCOST_PARAM* scost_param;
	
	int old_timer;
	bool is_receive_data;
	double energy_pre;
	bool is_receive_youth;
	int waiting_youth_addr;
	
	int young_timer;
	
	double energy_pre_period;
	double energy_consumed;
	
	double p_broadcast_ch;
};

#endif // CR_PROC_H
