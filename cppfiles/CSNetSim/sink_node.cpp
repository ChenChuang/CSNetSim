#include "sink_node.h"

double SinkNode::SINK_X;
double SinkNode::SINK_Y;
int SinkNode::SINK_ADDR;

SinkNode::SinkNode()
{
}

SinkNode::~SinkNode()
{
}

void SinkNode::startFloodRoute()
{
	double d;
	for(int i = 0; i < BaseNetwork::NODE_NUM; i ++){
		d = sqrt( pow( this->network->nodes[i]->x - SinkNode::SINK_X, 2 ) + pow( this->network->nodes[i]->y - SinkNode::SINK_Y, 2 ) );
		if(d <= BaseNode::MAX_RADIUS){
			struct msg* mp = new msg();
			mp->cmd = Flood_Proc::CMD_SINK;
			mp->fromaddr = SinkNode::SINK_ADDR;
			mp->toaddr = this->network->nodes[i]->addr;
			mp->type = BaseCommProxy::MSG_TYPE_BROADCAST;
			mp->radius = BaseNode::MAX_RADIUS;
			mp->data_l = 0;
			mp->size = BaseNode::CTRL_PACKET_SIZE;
			mp->data = NULL;
			mp->next = NULL;
			this->network->nodes[i]->commproxy->receive(mp);
			delete mp;
		}
	}
}

void SinkNode::onTimeOut()
{
	//in HEED, clustering is followed by flood route
#ifdef _HEED_	
	if(HEED_Proc::istimetoroute > 0){
#ifndef _singlehop_
		this->startFloodRoute();
		HEED_Proc::istimetoroute = -1;
#endif
	}
#endif
	
	//in EDCR, clustering is followed by flood route
#ifdef _EDCR_
	if(EDCR_Proc::istimetoroute > 0){
#ifndef _singlehop_
		this->startFloodRoute();
		EDCR_Proc::istimetoroute = -1;
#endif
	}
	
	//in EDCR, the node whose energy is below a threshold will call the sink to start clustering
	if(EDCR_Proc::istimetorotate > 0){
		//printf("EDCR start rotate at %f\n", this->network->sim_timer);
		this->network->monitor->rotate_times ++;
			
		for(int node_index = 0; node_index < BaseNetwork::NODE_NUM; node_index ++){
			this->network->nodes[node_index]->flood_timer = 0;
			this->network->nodes[node_index]->edcr_timer = 0;
		}
		EDCR_Proc::istimetorotate = -1;
	}
#endif

}
