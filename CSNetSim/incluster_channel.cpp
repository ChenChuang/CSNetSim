#include "incluster_channel.h"

InclusterChannel::InclusterChannel(Network* anetwork): network(anetwork)
{
}

InclusterChannel::~InclusterChannel()
{
}

int InclusterChannel::communicate(Msg* msg)
{
	if(msg->type == InclusterChannel::MSG_TYPE_INCLUSTERCAST){
		double d = 0;
		double k = msg->size;
		Node* t = this->network->node(msg->fromaddr);
		Node* r;
		//MnManager* mn = dynamic_cast<INode_ClusteringMember*>(this->network->node(msg->fromaddr))->get_mnmanager();
		MnIterator* mn_iter = dynamic_cast<INode_ClusteringMember*>(this->network->node(msg->fromaddr))->get_mnmanager()->get_mn_iter();
		double t_energy = 0;
		double r_energy = EnergyModel::calReceive(k);
		int r_count = 0;
		while(mn_iter->has_more()){
			r = this->network->node(mn_iter->next_addr());
			d = std::max(d, sqrt(pow(t->x - r->x, 2) + pow(t->y - r->y, 2)));
			if(r->is_alive()){
				r->get_commproxy()->receive(msg);
				r->consume(EnergyModel::calReceive(k));
				r_count++;
			}
		}
		msg->radius = d;
		t_energy = EnergyModel::calTransmit(k, d);
		t->consume(t_energy);
		dynamic_cast<IMonitor_Channel*>(this->network->get_monitor())->record_communicate(msg, t_energy + r_energy*r_count);
		return 1;
	}else{
		return -1;
	}
}

int ECommProxy_InclusterChannel::inclustercast(int fromaddr, int size, char cmd, int data_l, char* data)
{
	return this->send(InclusterChannel::MSG_TYPE_INCLUSTERCAST, fromaddr, -1, -1, size, cmd, data_l, data);
}
