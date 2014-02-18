#include "broadcast_channel.h"

BroadcastChannel::BroadcastChannel(Network* anetwork, double aradius): network(anetwork), radius(aradius)
{
	this->adjg = new AdjG(anetwork, aradius, Network::NODE_NUM);
}

BroadcastChannel::~BroadcastChannel()
{
	delete this->adjg;
}

int BroadcastChannel::communicate(Msg* msg)
{
	if(msg->type == CommProxy::MSG_TYPE_BROADCAST && msg->radius == this->radius){
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
				r->commproxy()->receive(msg);
				r->consume(r_energy);
				r_count++;
			}
			vp = vp->next;
		}
		t->consume(t_energy);
		if(msg->cmd != Node::CMD_SENSE_DATA_FUSED && msg->cmd != Node::CMD_SENSE_DATA_UNFUSED){
			this->network->monitor()->rotate_overhead += (t_energy + r_energy * r_count);
		}
		return 1;
	}else{
		return -1;
	}
}
