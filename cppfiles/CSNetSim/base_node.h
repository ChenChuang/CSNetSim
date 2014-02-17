#ifndef BASENODE_H
#define BASENODE_H

#include "compile_config.h"

#include "energy_model.h"
#include "base_network.h"
#include "base_comm_proxy.h"
#include "heed_proc.h"
#include "edcr_proc.h"
#include "msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

class HEED_Proc;
class Flood_Proc;
class CR_Proc;
class EDCR_Proc;

struct ProcNode
{
	Processor* proc;
	struct ProcNode* next;
	ProcNode(Processor* aproc): proc(aproc), next(NULL) {}
	~ProcNode() {
		delete this->proc;
	}
}

class BaseNode
{
public:
	BaseNode(int address, double l_x, double l_y);
	~BaseNode();

public:
	//implement virtual
	void on_time_out();
	//implement virtual
	void print();
	void add_proc(Processor* proc);
	void init_procs();
	void clear_procs();
public:
	BaseCommProxy* get_commproxy() {return this->commproxy;}
	BaseNetwork* get_network() {return this->network;}
	MnManager* get_mnmanager() {return this->mnmanager;}
	int get_addr() {return this->addr;}
	int get_ch_addr() {return this->ch_addr;}
	void set_ch_addr(int addr) {this->ch_addr = addr;}
	int get_next_hop() {return this->next_hop;}
	void set_next_hop(int addr) {this->next_hop = addr;}
	bool is_alive() {return this->energy <= 0;}
	
public:
	static double CLUSTER_RADIUS;   //metre
	static double MAX_RADIUS;    //metre
	static double SENSE_DATA_PERIOD;   //unit time

	static double DATA_PACKET_SIZE;    //byte
	static double CTRL_PACKET_SIZE;   //byte
	static double DATA_CTRL_PACKET_SIZE;  //byte

	double energy;
	//address(or ID) of this node
	int addr;
	//coordinate
	double x;
	double y;
	
	//every node use its own communication proxy to communicate with other nodes. one node, one proxy!
	BaseCommProxy* commproxy;
	BaseNetwork* network;
	MnManager* mnmanager;
	ProcNode* procs;
	
	int ch_addr;
	int next_hop;
	double d_tosink;
	
friend class BaseNetwork;
friend class Monitor;
};

#endif // BASENODE_H
