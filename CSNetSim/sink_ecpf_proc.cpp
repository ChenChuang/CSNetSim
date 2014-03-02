#include "sink_ecpf_proc.h"

SinkEcpfProc::SinkEcpfProc(Node* node)
{
	this->comm = dynamic_cast<ECommProxy_NetcastChannel*>(node->get_commproxy());
}

SinkEcpfProc::~SinkEcpfProc()
{
}

int SinkEcpfProc::process(Msg* msg)
{
	if(msg->cmd == SensorEcpfProc::CMD_NEED){
		this->comm->netcast(
			ClusteringSimModel::SINK_ADDR, 
			ClusteringSimModel::CTRL_PACKET_SIZE, SensorEcpfProc::CMD_CLUSTER, 
			0, NULL);
		return 1;
	}
	return -1;
}

