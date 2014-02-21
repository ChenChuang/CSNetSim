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
	// this->node->print();
	dynamic_cast<ECommProxy_BroadcastChannel*>(this->node->get_commproxy())->broadcast(
		this->node->addr, 
		ClusteringSimModel::CLUSTER_RADIUS, 
		1, 
		0x11,
		1,
		NULL);
	int* a = new int(this->node->get_addr());
	dynamic_cast<ECommProxy_UnicastChannel*>(this->node->get_commproxy())->unicast(
		this->node->addr,
		ClusteringSimModel::SINK_ADDR,
		1,
		0x22,
		1,
		(char*)a);
	delete a;
	this->node->get_commproxy()->clear_r_buf();
}

int TestProc::process(Msg* msg)
{
	if(this->node->get_addr() == ClusteringSimModel::SINK_ADDR){
	}
	printf("%d: ", this->node->get_addr());msg->print();
	return 1;
}