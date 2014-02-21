#ifndef TESTPROC_H
#define TESTPROC_H

#include "csnetsim.h"
#include "broadcast_channel.h"
#include "unicast_channel.h"
#include "incluster_channel.h"
#include "clustering_sim_model.h"
#include "clustering_member.h"

class TestProc : public Processor
{
public:
	TestProc(Node* anode);
	~TestProc();
public:
	virtual void ticktock(double time);
	virtual int process(Msg* msg);
	virtual void init() {}
public:
	Node* node;
};

#endif // TESTPROC_H
