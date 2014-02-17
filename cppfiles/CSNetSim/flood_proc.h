#ifndef FLOOD_PROC_H
#define FLOOD_PROC_H

#include "base_node.h"
#include "base_comm_proxy.h"
#include "Adjv.h"

class BaseNode;

class Flood_Proc
{
public:
	Flood_Proc(BaseNode* node, BaseCommProxy* commproxy);
	~Flood_Proc();

public:
	void init();
	void onTimeOut();
	void procMsg(struct msg* mp, double* min_d);
	void set_max_Adjv(struct Adjv* a);

public:
	BaseNode* node;
	BaseCommProxy* commproxy;
	
	struct Adjv* max_Adjv;
	
	char proc_state;

public:
	static const char PROC_WAIT = 0x01;
	static const char PROC_COMPLETED = 0x02;
	
	static const char CMD_SINK = 0x31;

#ifdef _singlehop_
	static const int FLOOD_PERIOD = 2;
#else	
#ifdef _CR_
	static const int FLOOD_PERIOD = 1;
#endif
#ifdef _HEED_
	static const int FLOOD_PERIOD = 20;
#endif
#ifdef _EDCR_
	static const int FLOOD_PERIOD = 20;
#endif
#endif
};

#endif // FLOODPROC_H
