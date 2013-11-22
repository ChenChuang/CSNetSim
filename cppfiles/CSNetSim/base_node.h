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

struct MEMBER
{
	int addr;
	struct MEMBER* next;
};

class HEED_Proc;
class Flood_Proc;
class CR_Proc;
class EDCR_Proc;

class BaseNode
{
public:
	BaseNode(int address, double l_x, double l_y);
	~BaseNode();

public:
	//implement virtual
	void onTimeOut();
	//implement virtual
	void printStr();
	//check if sensor's data is ready for transmitting
	void checkSensor();
	
	void remove_member(int addr);
	void clear_member();
	
public:
	static double CLUSTER_RADIUS;   //metre
	static double MAX_RADIUS;    //metre
	static double SENSE_DATA_PERIOD;   //unit time

	static double DATA_PACKET_SIZE;    //byte
	static double CTRL_PACKET_SIZE;   //byte
	static double DATA_CTRL_PACKET_SIZE;  //byte
	
	static const char CMD_SENSE_DATA_FUSED = 0x91;
	static const char CMD_SENSE_DATA_UNFUSED = 0x92;

	double energy;
	//address(or ID) of this node
	int addr;
	//coordinate
	double x;
	double y;
	//is this node out of energy?
	bool isAlive;
	
	//every node use its own communication proxy to communicate with other nodes. one node, one proxy!
	BaseCommProxy *commproxy;

	//processor of HEED
	HEED_Proc* heed;
	//processor of Flooding route
	Flood_Proc* flood;
	//processor of rotation
	CR_Proc* cr;
	//processor of EDCR
	EDCR_Proc* edcr;
	
	int CH_addr;
	struct MEMBER* members;
	int next_hop_addr;
	double d_tosink;
	
	int rotate_timer;
	int heed_timer;
	int flood_timer;
	int edcr_timer;
	bool is_sense_ready;
	double pre_sense_time;

friend class BaseNetwork;
friend class Monitor;
};

#endif // BASENODE_H
