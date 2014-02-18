#include "unicast_channel.h"

UnicastChannel::UnicastChannel(Network* anetwork): network(anetwork)
{
}

UnicastChannel::~UnicastChannel()
{
}

int UnicastChannel::communicate(Msg* msg)
{
	if(msg->type == CommProxy::MSG_TYPE_UNICAST){
		Node* t = this->network->get_node(msg->fromaddr);
		Node* r = this->network->get_node(msg->toaddr);
		double d = sqrt( pow( t->x - r->x, 2 ) + pow( t->y - r->y, 2 ) );
		double k = msg->size;
		if(r->is_alive()){
			r->commproxy()->receive(msg);
			double t_energy = EnergyModel::calTransmit(k, d);
			double r_energy = EnergyModel::calReceive(k);
			t->consume(t_energy);
			r->consume(r_energy);
			if(msg->cmd != Node::CMD_SENSE_DATA_FUSED && msg->cmd != Node::CMD_SENSE_DATA_UNFUSED){
				this->network->get_monitor()->rotate_overhead += (t_energy + r_energy);
			}
		}
		return 1;
	}else{
		return -1;
	}
}
