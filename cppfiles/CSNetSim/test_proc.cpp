#include "test_proc.h"

TestProc::TestProc(Node* anode): node(anode)
{
}

TestProc::~TestProc()
{
}

void TestProc::ticktock(double time)
{
	this->node->get_commproxy()->clear_t_buf();
	this->node->print();
	dynamic_cast<ECommProxy_BroadcastChannel*>(this->node->get_commproxy())->broadcast(
		this->node->addr, 
		ClusteringSimModel::CLUSTER_RADIUS, 
		1, 
		0x11,
		1,
		NULL);
	this->node->get_commproxy()->clear_r_buf();
}

