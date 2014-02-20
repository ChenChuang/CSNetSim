#include "unicast_channel.h"

UnicastChannel::UnicastChannel(Network* anetwork): network(anetwork)
{
}

UnicastChannel::~UnicastChannel()
{
}

int UnicastChannel::communicate(Msg* msg)
{
	if(msg->type == UnicastChannel::MSG_TYPE_UNICAST){
		Node* t = this->network->node(msg->fromaddr);
		Node* r = this->network->node(msg->toaddr);
		double d = sqrt(pow(t->x - r->x, 2) + pow(t->y - r->y, 2));
		msg->radius = d;
		double k = msg->size;
		if(r->is_alive()){
			r->get_commproxy()->receive(msg);
			double t_energy = EnergyModel::calTransmit(k, d);
			double r_energy = EnergyModel::calReceive(k);
			t->consume(t_energy);
			r->consume(r_energy);
			dynamic_cast<IMonitor_Channel*>(this->network->get_monitor())->record_communicate(msg, t_energy + r_energy);
		}
		return 1;
	}else{
		return -1;
	}
}

int ECommProxy_UnicastChannel::unicast(int fromaddr, int toaddr, int size, char cmd, int data_l, char* data)
{
	return this->send(UnicastChannel::MSG_TYPE_UNICAST, fromaddr, toaddr, -1, size, cmd, data_l, data);
}

int ECommProxy_UnicastChannel::repost(Msg* msg, int toaddr)
{
	return this->send(UnicastChannel::MSG_TYPE_UNICAST, msg->toaddr, toaddr, -1, msg->size, msg->cmd, msg->data_l, msg->data);
}
