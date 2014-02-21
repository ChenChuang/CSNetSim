#ifndef TESTPROC_H
#define TESTPROC_H

#include "csnetsim.h"
#include "broadcast_channel.h"
#include "clustering_sim_model.h"

class TestProc : public Processor
{
public:
	TestProc(Node* anode);
	~TestProc();
public:
	virtual void ticktock(double time);
	virtual int process(Msg* msg) {return -1;}
	virtual void init() {}
public:
	Node* node;
};

#endif // TESTPROC_H
