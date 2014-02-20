#include "broadcast_channel.h"

BroadcastChannel::BroadcastChannel(Network* anetwork, double aradius): network(anetwork), radius(aradius)
{
	this->adjg = new AdjG(anetwork, aradius, anetwork->nodes_num);
}

BroadcastChannel::~BroadcastChannel()
{
	delete this->adjg;
}

int BroadcastChannel::communicate(Msg* msg)
{
	if(msg->type == BroadcastChannel::MSG_TYPE_BROADCAST && msg->radius == this->radius){
		double d = msg->radius;
		double k = msg->size;
		Adjv* vp = this->adjg->v[msg->fromaddr]->next;
		Node* t = this->network->node(msg->fromaddr);
		Node* r;
		double t_energy = EnergyModel::calTransmit(k, d);
		double r_energy = EnergyModel::calReceive(k);
		int r_count = 0;
		while(vp != NULL){
			r = this->network->node(msg->toaddr);
			if(r->is_alive()){
				r->get_commproxy()->receive(msg);
				r->consume(r_energy);
				r_count++;
			}
			vp = vp->next;
		}
		t->consume(t_energy);
		dynamic_cast<IMonitor_Channel*>(this->network->get_monitor())->record_communicate(msg, t_energy + r_energy*r_count);
		return 1;
	}else{
		return -1;
	}
}

int ECommProxy_BroadcastChannel::broadcast(int fromaddr, double radius, int size, char cmd, int data_l, char* data)
{
	return this->push_msg(BroadcastChannel::MSG_TYPE_BROADCAST, fromaddr, -1, radius, size, cmd, data_l, data);
}