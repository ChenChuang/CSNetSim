#include "sink_node.h"

SinkNode::SinkNode(Network* anetwork, int aaddr, double ax, double ay): 
	Node(anetwork, aaddr, ax, ay, 1)
{
	this->commproxy = new SinkCommProxy();
	
	this->testproc = new TestProc(this);
	//this->procs_manager->add(this->testproc);
	
	this->dataproc = new SinkDataProc();
	this->procs_manager->add(this->dataproc);
	
	this->routeproc = new SinkRouteProc(this);
	this->procs_manager->add(this->routeproc);
}

SinkNode::~SinkNode()
{
	delete this->commproxy;
	this->commproxy = NULL;
	delete this->testproc;
	this->testproc = NULL;
}

void SinkNode::start_route()
{
	dynamic_cast<SinkRouteProc*>(this->routeproc)->start_route();
}