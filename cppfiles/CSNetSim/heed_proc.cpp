#include "heed_proc.h"
#include <math.h>

double HEED_Proc::C_PROB = 0.07;
double HEED_Proc::P_MIN = 0.0001;
int HEED_Proc::istimetoroute;

HEED_Proc::HEED_Proc(BaseNode* node, BaseCommProxy* commproxy)
{
	this->node = node;
	this->commproxy = commproxy;
	this->init();
}

HEED_Proc::~HEED_Proc()
{
	this->finalize();
}

void HEED_Proc::finalize()
{
	struct SCH* p1 = this->sch;
	struct SCH* pre1 = NULL;
	while(p1 != NULL){
		pre1 = p1;
		p1 = p1->next;
		delete pre1;
	}
	struct SCOST* p2 = this->scost;
	struct SCOST* pre2 = NULL;
	while(p2 != NULL){
		pre2 = p2;
		p2 = p2->next;
		delete pre2;
	}
	this->node->clear_member();
}

void HEED_Proc::init()
{
	this->proc_state = HEED_Proc::PROC_INITIALIZE;
	this->ch_state = HEED_Proc::NOT_CH;
	this->sch = NULL;
	this->scost = NULL;
	this->node->members = NULL;

	this->c_prob = HEED_Proc::C_PROB;
	this->p_min = HEED_Proc::P_MIN;
	this->heed_count = 1;
	
	HEED_Proc::istimetoroute = -1;
}

void HEED_Proc::onTimeOut()
{
#ifdef _print_
	printf("%d : onTimeOut\n", this->node->addr);
#endif
	this->commproxy->clear_t_buf();
	//process all received msg
	struct msg* mp = this->commproxy->r_msg_buf;
	while(mp != NULL){
		this->procMsg(mp);
		mp = mp->next;
	}
	this->procClustering();
	
	//------------------------------------------------------------------
	if(this->node->members != NULL){
		//this->node->CH_addr = this->node->addr;
	}
	
	this->commproxy->clear_r_buf();
	
}

int HEED_Proc::procMsg(struct msg* mp)
{
#ifdef _print_
	printf("procMsg : type = %x, from = %d, to = %d, r = %f, cmd = %x, data_l = %d\n", mp->type, mp->fromaddr, mp->toaddr, mp->radius, mp->cmd, mp->data_l);
#endif

	if(mp->cmd == HEED_Proc::CMD_BROADCAST_COST){
		this->receive_cost_msg(mp);
	}else if(mp->cmd == HEED_Proc::CMD_CH_MSG){
		this->receive_ch_msg(mp);
	}else if(mp->cmd == HEED_Proc::CMD_JOIN){
		this->receive_join_msg(mp);
	}
	
	return 1;
}

int HEED_Proc::procClustering()
{
#ifdef _test_
	double k;
	struct Adjv* p = this->cluster_Adjv->next;
	while(p != NULL){
		k = rand() / (RAND_MAX + 1.0);
		//int toaddr;
		if(k < 0.1){
			//toaddr = (int)(rand() * BaseNetwork::NODE_NUM / (RAND_MAX + 1.0));
			//toaddr = toaddr < BaseNetwork::NODE_NUM ? toaddr : BaseNetwork::NODE_NUM - 1;
			//this->commproxy->unicast(this->node->addr, toaddr, BaseNode::DATA_PACKET_SIZE, (char)(0x13), 0, NULL);
			this->commproxy->unicast(this->node->addr, p->addr, BaseNode::DATA_PACKET_SIZE, (char)(0x13), 0, NULL);
#ifdef _print_
			printf("unicast : from = %d, to = %d, cmd = %x, data_l = %d\n", this->node->addr, toaddr, (char)(0x13), 0);
#endif
		}
		p = p->next;
	}
#endif
	
	if(this->proc_state == HEED_Proc::PROC_INIT){
		return 1;
	}else if(this->proc_state == HEED_Proc::PROC_INITIALIZE){
		this->compute_broadcast_cost();
		this->ch_state = HEED_Proc::NOT_CH;
		
		this->ch_prob = max( this->c_prob * this->node->energy / EnergyModel::E_INIT, this->p_min );
		//this->ch_prob = max( this->c_prob * ( this->node->energy + 10 / pow(2, (double)BaseNetwork::sim_timer) * rand() / (RAND_MAX + 1.0) ) / EnergyModel::E_INIT, this->p_min );
		
		this->proc_state = HEED_Proc::PROC_MAIN;
		this->ch_prob_pre = 0;
	}else if(this->proc_state == HEED_Proc::PROC_MAIN){
		if(this->ch_prob_pre < 1){
			if(this->sch != NULL){
				if( this->get_least_cost() == this->node->addr ){					
					if( this->ch_prob == 1 ){
						if(this->ch_state != HEED_Proc::FINAL_CH){
							this->ch_state = HEED_Proc::FINAL_CH;
							this->node->CH_addr = this->node->addr;
							this->add_sch(this->node->addr, this->ch_state);
							this->cluster_head_msg();
						}
					}else{
						if(this->ch_state == HEED_Proc::NOT_CH){
							this->ch_state = HEED_Proc::TENTATIVE_CH;
							this->add_sch(this->node->addr, this->ch_state);
							this->cluster_head_msg();
						}
					}
				}
				
			}else if(this->ch_prob == 1){
				if(this->ch_state != HEED_Proc::FINAL_CH){
					this->ch_state = HEED_Proc::FINAL_CH;
					this->node->CH_addr = this->node->addr;
					this->add_sch(this->node->addr, this->ch_state);
					this->cluster_head_msg();
				}
			}else if( rand() / (RAND_MAX + 1.0) <= this->ch_prob ){
				if(this->ch_state == HEED_Proc::NOT_CH){
					this->ch_state = HEED_Proc::TENTATIVE_CH;
					this->add_sch(this->node->addr, this->ch_state);
					this->cluster_head_msg();
				}
			}
			this->ch_prob_pre = this->ch_prob;
			this->ch_prob = min( this->ch_prob * 2, 1.0 );
		}else{
			this->proc_state = HEED_Proc::PROC_FINAL;
		}
	}else if(this->proc_state == HEED_Proc::PROC_FINAL){
#ifdef _print_
		printf("final---node: %d, %x, %d, %d\n", this->node->addr, this->ch_state, this->get_least_cost(), this->get_least_cost_final());
#endif
		if( this->ch_state != HEED_Proc::FINAL_CH ){
			int least_ch = this->get_least_cost_final();
			if( least_ch >= 0 ){
				this->join_cluster(least_ch);
				this->proc_state = HEED_Proc::PROC_INIT;
			}else{
				double r = rand() / (RAND_MAX + 1.0);
				if(this->heed_count > 4 || r < HEED_Proc::C_PROB * pow(2, (double)(this->heed_count))){
					this->node->CH_addr = this->node->addr;
					this->ch_state = HEED_Proc::FINAL_CH;
					this->cluster_head_msg();
					this->proc_state = HEED_Proc::PROC_INIT;
#ifdef _print_
					printf("node final to ch: %d\n", this->node->addr);
#endif
				}else{
					this->proc_state = HEED_Proc::PROC_FINAL;
					this->heed_count ++;
					return 1;
				}
				
			}
		}else{
			this->node->CH_addr = this->node->addr;
			this->ch_state = HEED_Proc::FINAL_CH;
			this->cluster_head_msg();
		}
		this->proc_state = HEED_Proc::PROC_INIT;
	}
	return 1;
}

void HEED_Proc::set_cluster_Adjv(struct Adjv* a)
{
	this->cluster_Adjv = a;
}

void HEED_Proc::set_max_Adjv(struct Adjv* a)
{
	this->max_Adjv = a;
}

void HEED_Proc::cluster_head_msg()
{
	if(this->ch_state == HEED_Proc::TENTATIVE_CH || this->ch_state == HEED_Proc::FINAL_CH){
		char *data = new char[1];
		data[0] = this->ch_state;
		this->commproxy->broadcast(this->node->addr, BaseNode::CLUSTER_RADIUS, BaseNode::CTRL_PACKET_SIZE, HEED_Proc::CMD_CH_MSG, 1, data);
		//printf("node %d: become CH %x\n", this->node->addr, this->ch_state);
	}
#ifdef _CR_
	if(this->ch_state == HEED_Proc::FINAL_CH){
		this->node->cr->life_state = CR_Proc::LIFE_YOUTH;
		this->node->cr->proc_state = CR_Proc::PROC_INIT;
		this->node->cr->energy_pre = this->node->energy;
	}
#endif
}

void HEED_Proc::compute_broadcast_cost()
{
	double* data = new double[1];
	data[0] = this->calAMRP();
	this->commproxy->broadcast(this->node->addr, BaseNode::CLUSTER_RADIUS, BaseNode::CTRL_PACKET_SIZE, HEED_Proc::CMD_BROADCAST_COST, sizeof(double), (char*)data);
}

void HEED_Proc::receive_cost_msg(struct msg* mp)
{
	this->add_scost(mp->fromaddr, ((double*)(mp->data))[0]);
}

int HEED_Proc::get_least_cost()
{
	struct SCH* p = this->sch;
	struct SCH* least_p = p;
	while(p != NULL){
		if( p->cost < least_p->cost ){
			least_p = p;
		}
		p = p->next;
	}
	if(least_p == NULL){
		return -1;
	}else{
		return least_p->addr;
	}
}

int HEED_Proc::get_least_cost_final()
{
	struct SCH* p = this->sch;
	struct SCH* least_p = p;
	bool hasfinal = false;
	while(p != NULL){
		if( p->ch_state == HEED_Proc::FINAL_CH ){
			hasfinal = true;
			if( p->cost < least_p->cost ){
				least_p = p;
			}
		}
		p = p->next;
	}
	if(hasfinal){
		return least_p->addr;
	}else{
		return -1;
	}
}

void HEED_Proc::join_cluster(int ch_addr)
{
	if(this->node->members != NULL){
		return;
	}
	this->ch_state = HEED_Proc::NOT_CH;
	this->node->CH_addr = ch_addr;
	this->commproxy->unicast(this->node->addr, ch_addr, BaseNode::CTRL_PACKET_SIZE, HEED_Proc::CMD_JOIN, 0, NULL);
#ifdef _CR_
	this->node->cr->life_state = CR_Proc::LIFE_BABY;
#endif
#ifdef _print_
	printf("node %d ch_state : %x, : join cluster %d\n", this->node->addr, this->ch_state, ch_addr);
#endif
}

void HEED_Proc::add_member(int addr)
{
	struct MEMBER* p = this->node->members;
	struct MEMBER* pre = NULL;
	while(p != NULL){
		if(p->addr == addr){
			return;
		}
		pre = p;
		p = p->next;
	}
	struct MEMBER* m = new MEMBER();
	m->addr = addr;
	m->next = NULL;
	if(pre == NULL){
		this->node->members = m;
	}else{
		pre->next = m;
	}
}

void HEED_Proc::add_sch(int paddr, char pch_state)
{
	struct SCH* p = this->sch;
	struct SCH* pre = NULL;
	while(p != NULL){
		if( p->addr == paddr ){
			p->ch_state = pch_state;
			return;
		}
		pre = p;
		p = p->next;
	}
	//create new SCH
	struct SCH* ch = new SCH();
	ch->addr = paddr;
	ch->ch_state = pch_state;
	ch->cost = -1;
	ch->next = NULL;
	struct SCOST* c = this->scost;
	while(c != NULL){
		if(c->addr == paddr){
			ch->cost = c->cost;
			break;
		}
		c = c->next;
	}
	//creating new SCH fail
	if(ch->cost < 0){
		delete ch;
		return;
	}
	//add new SCH to this->sch
	if(pre == NULL){
		this->sch = ch;
	}else{
		pre->next = ch;
	}
}

void HEED_Proc::add_scost(int addr, double cost)
{
	struct SCOST* p = this->scost;
	struct SCOST* pre = NULL;
	while(p != NULL){
		if(p->addr == addr){
			return;
		}
		pre = p;
		p = p->next;
	}
	struct SCOST* c = new SCOST();
	c->addr = addr;
	c->cost = cost;
	//-----------use distance as cost
	/*
	struct Adjv* pp = this->cluster_Adjv;
	while(pp != NULL){
		if(pp->addr == addr){
			c->cost = pp->d;
		}
		pp = pp->next;
	}*/
	//-----------
	
	c->next = NULL;
	if(pre == NULL){
		this->scost = c;
	}else{
		pre->next = c;
	}
}

void HEED_Proc::receive_ch_msg(msg* mp)
{
#ifdef _CR_
#ifdef _singlehop_
	if(mp->data[0] == this->FINAL_CH){
		this->node->cr->change_neighbor(mp->fromaddr, true, this->node->energy);
	}
#endif
#endif
	//printf("node %d: receive_ch_msg from node %d\n", this->node->addr, mp->fromaddr);
	this->add_sch(mp->fromaddr, mp->data[0]);
}

void HEED_Proc::receive_join_msg(msg* mp)
{
	this->node->CH_addr = this->node->addr;
#ifdef _CR_
	this->node->cr->life_state = CR_Proc::LIFE_YOUTH;
	this->node->cr->proc_state = CR_Proc::PROC_INIT;
	this->node->cr->energy_pre = this->node->energy;
#endif
	this->add_member(mp->fromaddr);
}

double HEED_Proc::calAMRP()
{
	double sum = 0;
	int count = 0;
	struct Adjv* p = this->cluster_Adjv;
	if(p == NULL){
		return 0;
	}
	while(p != NULL){
		count ++;
		sum += EnergyModel::calTransmit(1000, p->d);
		p = p->next;
	}
	return sum / count;
}
