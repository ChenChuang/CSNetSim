#ifndef EDCR_PROC_H
#define EDCR_PROC_H

#include "base_comm_proxy.h"
#include "base_network.h"
#include "base_node.h"
#include "energy_model.h"
#include "heed_proc.h"
#include <cmath>

class EDCR_Proc
{
public:
	EDCR_Proc(BaseNode *node, BaseCommProxy *commproxy);
	~EDCR_Proc();

	void finalize();
	void init();
	void set_cluster_adjv(struct adjv* a);
	
	void onTimeOut();
	int procMsg(struct msg* mp);
	int procClustering();
	
	void inclustercast_query_energy();
	void receive_query_energy(int addr);
	void receive_respond_energy(int addr, double energy);
	
	void maxbroadcast_maxenergy();
	void receive_maxenergy(int addr, double energy);
	
	void clusterbroadcast_candidance();
	void receive_candidance(int ch_addr, double energy);
	void unicast_join(int ch_addr);
	void receive_join(int addr);
	void add_candidance(int addr, double cost);
	void add_member(int addr);
	
	double calWaitTime();
	double calCost(int ch_addr, double energy);
	
	int find_best_ch();
	
	bool check_energy();
	
public:
	static const char PROC_INIT= 0x01;
	static const char PROC_QUERY_ENERGY = 0x02;
	static const char PROC_WAITING_RESPOND_ENERGY= 0x03;
	static const char PROC_RESPOND_ENERGY_RECEIVED = 0x04;
	static const char PROC_MAXENERGY_3R_RECEIVED= 0x05;
	static const char PROC_WAITING_TIMEOUT = 0x06;
	static const char PROC_FINAL = 0x07;
	
	static const char CMD_QUERY_ENERGY = 0x41;
	static const char CMD_RESPOND_ENERGY = 0x42;
	static const char CMD_MAXENERGY = 0x43;
	static const char CMD_CANDIDANCE = 0x44;
	static const char CMD_JOIN = 0x45;
	
	static const int T_MAX = 9000;
	
	static const int EDCR_PERIOD = 9500;
	
	static const int MIN_ROTATE_GAP = 10;
	
	static int istimetoroute;
	static int istimetorotate;
	
	static int isfirsttime;
	
	static double last_rotate;
	
public:
	BaseNode* node;
	BaseCommProxy* commproxy;
	
	char proc_state;
	int wait_timer;
	int wait_time;
	struct SCOST* scost;
	
	struct adjv* cluster_adjv;
	
	double maxenergy_3R;
	double maxenergy_cluster;
	
	double energy_pre;
	
	
};

#endif // EDCR_PROC_H
