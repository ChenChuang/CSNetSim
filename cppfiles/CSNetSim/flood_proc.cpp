#include "flood_proc.h"

Flood_Proc::Flood_Proc(BaseNode* node, BaseCommProxy* commproxy)
{
	this->node = node;
	this->commproxy = commproxy;
	this->init();
}

Flood_Proc::~Flood_Proc()
{
}

void Flood_Proc::init()
{
	this->proc_state = Flood_Proc::PROC_WAIT;
}

void Flood_Proc::procMsg(struct msg* mp, double* min_d)
{
	if(mp->cmd == Flood_Proc::CMD_SINK){
		if(this->proc_state == Flood_Proc::PROC_WAIT){
			this->node->next_hop_addr = mp->fromaddr;
			this->proc_state = Flood_Proc::PROC_COMPLETED;
			this->commproxy->broadcast(this->node->addr, BaseNode::MAX_RADIUS, BaseNode::CTRL_PACKET_SIZE, Flood_Proc::CMD_SINK, 0, NULL);
		}else{
			struct adjv* p = this->max_adjv->next;
			while(p != NULL){
				if(p->addr == mp->fromaddr){
					break;
				}
				p = p->next;
			}
			if(p->d < *min_d){
				*min_d = p->d;
				this->node->next_hop_addr = mp->fromaddr;
			}
		}
	}
}

void Flood_Proc::onTimeOut()
{
	this->commproxy->clear_t_buf();
	struct msg* mp;
#ifdef _CR_
	mp = this->commproxy->r_msg_buf;
	while(mp != NULL){
		if(mp->cmd == Flood_Proc::CMD_SINK){
			this->node->cr->change_neighbor(mp->fromaddr, true, this->node->energy);
			printf("dd\n");
		}
		mp = mp->next;
	}
#endif
	if(this->proc_state == Flood_Proc::PROC_WAIT){
		if(this->node->CH_addr == this->node->addr){
			//process all received msg
			double min_d;
			mp = this->commproxy->r_msg_buf;
			while(mp != NULL && this->proc_state){
				this->procMsg(mp, &min_d);
				mp = mp->next;
			}
		}else{
			this->node->next_hop_addr = this->node->CH_addr;
			this->proc_state = Flood_Proc::PROC_COMPLETED;
		}
	}else{
	}
	this->commproxy->clear_r_buf();
#ifdef _singlehop_
#ifdef _HEED_
	if(this->node->CH_addr == this->node->addr){
		this->node->next_hop_addr = SinkNode::SINK_ADDR;
	}
#endif
#endif
}

void Flood_Proc::set_max_adjv(struct adjv* a)
{
	this->max_adjv = a;
}
