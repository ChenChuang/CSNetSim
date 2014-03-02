#include "netcast_channel.h"

NetcastChannel::NetcastChannel(Network* anetwork) : network(anetwork)
{
}

NetcastChannel::~NetcastChannel()
{
}

int NetcastChannel::communicate(Msg* msg)
{
	if(msg->type == NetcastChannel::MSG_TYPE_NETCAST){
		double k = msg->size;
		double t_energy = EnergyModel::calTransmit(k, 0);
		double r_energy = EnergyModel::calReceive(k);
		for(int addr=0; addr<this->network->nodes_num; addr++){
			if(addr != msg->fromaddr){
				this->network->node(addr)->get_commproxy()->receive(msg);
				this->network->node(addr)->consume(r_energy);
			}
		}
		this->network->node(msg->fromaddr)->consume(t_energy);
		dynamic_cast<IMonitor_Channel*>(this->network->get_monitor())->record_communicate(msg, t_energy + r_energy*(this->network->nodes_num-1));
		return 1;
	}
	return -1;
}

int ECommProxy_NetcastChannel::netcast(int fromaddr, int size, char cmd, int data_l, char* data)
{
	return this->send(NetcastChannel::MSG_TYPE_NETCAST, fromaddr, -1, -1, size, cmd, data_l, data);
}