#include "sink_node.h"

SinkNode::SinkNode(Network* anetwork, int aaddr, double ax, double ay): 
	Node(anetwork, aaddr, ax, ay, 1)
{
	this->commproxy = new SinkCommProxy();
	this->testproc = new TestProc(this);
	this->procs_manager->add(this->testproc);
}

SinkNode::~SinkNode()
{
	delete this->commproxy;
	this->commproxy = NULL;
	delete this->testproc;
	this->testproc = NULL;
}