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
		0,
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
	
	if(this->node->get_addr() != ClusteringSimModel::SINK_ADDR){
		dynamic_cast<INode_ClusteringMember*>(this->node)->get_mnmanager()->clear();
		dynamic_cast<INode_ClusteringMember*>(this->node)->get_mnmanager()->add(5);
		dynamic_cast<ECommProxy_InclusterChannel*>(this->node->get_commproxy())->inclustercast(this->node->addr, 1, 0x33, 0, NULL);
	}
	
	this->node->get_network()->get_clock()->try_set_tick(0.5);
	
	this->node->get_commproxy()->clear_r_buf();
}

int TestProc::process(Msg* msg)
{
	if(this->node->get_addr() == ClusteringSimModel::SINK_ADDR){
	}
	printf("%d: ", this->node->get_addr());msg->print();
	return 1;
}