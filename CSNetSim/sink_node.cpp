#include "sink_node.h"

SinkNode::SinkNode(Network* anetwork, int aaddr, double ax, double ay): 
	Node(anetwork, aaddr, ax, ay, 1)
{
	this->commproxy = new SinkCommProxy();
	
	this->testproc = new TestProc(this);
	//this->procs_manager->add(this->testproc);
	
	this->dataproc = new SinkDataProc();
	this->procs_manager->add(this->dataproc);
	
	this->ecpfproc = new SinkEcpfProc(this);
	this->procs_manager->add(this->ecpfproc);
	
	this->dataproc->turn_on();
#ifdef _ECPF_
	this->ecpfproc->turn_on();
#endif
}

SinkNode::~SinkNode()
{
	delete this->commproxy;
	this->commproxy = NULL;
	delete this->testproc;
	this->testproc = NULL;
	delete this->dataproc;
	this->dataproc = NULL;
	delete this->ecpfproc;
	this->ecpfproc = NULL;
}