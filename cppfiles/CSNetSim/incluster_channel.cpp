#include "incluster_channel.h"

InclusterChannel::InclusterChannel(Network* anetwork): network(anetwork)
{
}

InclusterChannel::~InclusterChannel()
{
}

int InclusterChannel::communicate(Msg* msg)
{
	if(msg->type == CommProxy::MSG_TYPE_INCLUSTERCAST){
		double d = 0;
		double k = msg->size;
		Node* t = this->network->node(msg->fromaddr);
		Node* r;
		MnIterator* mn_iter = dynamic_cast<INode_InclusterChannel*>(this->network->node(msg->fromaddr))->mnmanager()->mn_iter();
		double t_energy = 0;
		double r_energy = EnergyModel::calReceive(k);
		int r_count = 0;
		while(mn_iter->has_more()){
			r = this->network->node(mn_iter->next_addr());
			d = max(d, sqrt(pow(t->x - r->x, 2) + pow(t->y - r->y, 2)));
			if(r->is_alive()){
				r->commproxy()->receive(msg);
				r->consume(EnergyModel::calReceive(k));
				r_count++;
			}
		}
		t_energy = EnergyModel::calTransmit(k, d);
		t->consume(t_energy);
		if(msg->cmd != Node::CMD_SENSE_DATA_FUSED && msg->cmd != Node::CMD_SENSE_DATA_UNFUSED){
			this->network->monitor()->rotate_overhead += (t_energy + r_energy * r_count);
		}
		return 1;
	}else{
		return -1;
	}
}
