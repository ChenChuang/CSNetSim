#ifndef HEED_PROC_H
#define HEED_PROC_H

#include "base_comm_proxy.h"
#include "base_network.h"
#include "base_node.h"
#include "energy_model.h"
#include "adjv.h"
#include "cr_proc.h"
#include <stdio.h>
#include <stdlib.h>

struct SCH
{
	int addr;
	double cost;
	char ch_state;
	struct SCH* next;
};

struct SCOST
{
	int addr;
	double cost;
	struct SCOST* next;
};

class BaseNode;

class HEED_Proc
{
public:
	HEED_Proc(BaseNode *node, BaseCommProxy *commproxy);
	~HEED_Proc();
	
public:
	void finalize();
	void init();
	void onTimeOut();
	int procMsg(struct msg* mp);
	int procClustering();
	
	void set_cluster_adjv(struct adjv* a);
	void set_max_adjv(struct adjv* a);
	
	void compute_broadcast_cost();
	void receive_cost_msg(struct msg* mp);
	void receive_ch_msg(struct msg* mp);
	void receive_join_msg(struct msg* mp);
	int get_least_cost();
	int get_least_cost_final();
	void cluster_head_msg();
	void join_cluster(int ch_addr);
	void add_sch(int addr, char ch_state);
	void add_scost(int addr, double cost);
	void add_member(int addr);
	double calAMRP();

public:
	static const char PROC_INIT= 0x01;
	static const char PROC_INITIALIZE = 0x02;
	static const char PROC_MAIN = 0x03;
	static const char PROC_FINAL = 0x04;
	
	static const char NOT_CH = 0x01;
	static const char TENTATIVE_CH = 0x02;
	static const char FINAL_CH = 0x03;
	
	static const char CMD_BROADCAST_COST = 0x11;
	static const char CMD_CH_MSG = 0x12;
	static const char CMD_JOIN = 0x13;
	
	static const int ROTATE_PERIOD = 200;
	static const int HEED_PERIOD = 30;
	
	static double C_PROB;
	static double P_MIN;
	
	static int istimetoroute;

	double c_prob;
	double p_min;
	
public:
	BaseNode* node;
	BaseCommProxy* commproxy;
	
	struct adjv* cluster_adjv;
	struct adjv* max_adjv;
	
	double cost;
	char proc_state;
	char ch_state;
	double ch_prob;
	double ch_prob_pre;
	struct SCH* sch;
	struct SCOST* scost;
	
	int heed_count;

};

#endif // HEED_PROC_H
