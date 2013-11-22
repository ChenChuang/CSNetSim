#include "edcr_proc.h"

int EDCR_Proc::istimetoroute;
int EDCR_Proc::istimetorotate;
int EDCR_Proc::isfirsttime;

double EDCR_Proc::last_rotate;

EDCR_Proc::EDCR_Proc(BaseNode *node, BaseCommProxy *commproxy)
{
	this->node = node;
	this->commproxy = commproxy;
	this->isfirsttime = 1;
	EDCR_Proc::last_rotate = 0;
	this->init();
}

EDCR_Proc::~EDCR_Proc()
{
	this->finalize();
}

void EDCR_Proc::finalize()
{
	struct SCOST* p2 = this->scost;
	struct SCOST* pre2 = NULL;
	while(p2 != NULL){
		pre2 = p2;
		p2 = p2->next;
		delete pre2;
	}	
	this->node->clear_member();
}

void EDCR_Proc::init()
{
	this->proc_state = EDCR_Proc::PROC_INIT	;
	this->scost = NULL;
	this->node->members = NULL;
	
	EDCR_Proc::istimetoroute = -1;
	EDCR_Proc::istimetorotate = -1;
}

void EDCR_Proc::set_cluster_adjv(struct adjv* a)
{
	this->cluster_adjv = a;
}

void EDCR_Proc::onTimeOut()
{
#ifdef _print_
	printf("node %d : onTimeOut\n", this->node->addr);
#endif
	this->commproxy->clear_t_buf();
	//process all received msg
	struct msg* mp = this->commproxy->r_msg_buf;
	while(mp != NULL){
		this->procMsg(mp);
		mp = mp->next;
	}
	this->procClustering();
	
	this->commproxy->clear_r_buf();
}

int EDCR_Proc::procMsg(struct msg* mp)
{
#ifdef _print_
	printf("procMsg : type = %x, from = %d, to = %d, r = %f, cmd = %x, data_l = %d\n", mp->type, mp->fromaddr, mp->toaddr, mp->radius, mp->cmd, mp->data_l);
#endif

	if(mp->cmd == EDCR_Proc::CMD_QUERY_ENERGY){
		this->receive_query_energy(mp->fromaddr);
	}else if(mp->cmd == EDCR_Proc::CMD_RESPOND_ENERGY){
		this->receive_respond_energy(mp->fromaddr, *(double*)(mp->data));
	}else if(mp->cmd == EDCR_Proc::CMD_MAXENERGY){
		this->receive_maxenergy(mp->fromaddr, *(double*)(mp->data));
	}else if(mp->cmd == EDCR_Proc::CMD_CANDIDANCE){
		this->receive_candidance(mp->fromaddr, *(double*)(mp->data));
	}else if(mp->cmd == EDCR_Proc::CMD_JOIN){
		this->receive_join(mp->fromaddr);
	}
	
	return 1;	
}

int EDCR_Proc::procClustering()
{
	if(this->proc_state == this->PROC_INIT){
		this->proc_state = this->PROC_QUERY_ENERGY;
		//printf("node %d: start rotate\n", this->node->addr);
	}else if(this->proc_state == this->PROC_QUERY_ENERGY){
		if(this->node->addr == this->node->CH_addr){
			this->inclustercast_query_energy();
		}
		this->proc_state = this->PROC_WAITING_RESPOND_ENERGY;
		//printf("node %d: start query energy\n", this->node->addr);
	}else if(this->proc_state == this->PROC_WAITING_RESPOND_ENERGY){
		this->proc_state = this->PROC_RESPOND_ENERGY_RECEIVED;
	}else if(this->proc_state == this->PROC_RESPOND_ENERGY_RECEIVED){
		if(this->node->addr == this->node->CH_addr){
			this->maxenergy_3R = this->maxenergy_cluster;
			this->maxbroadcast_maxenergy();
		}else{
			this->maxenergy_3R = this->node->energy;
		}
		this->proc_state = this->PROC_MAXENERGY_3R_RECEIVED;
	}else if(this->proc_state == this->PROC_MAXENERGY_3R_RECEIVED){
		//printf("node %d: start timer\n", this->node->addr);
		this->wait_time = this->calWaitTime();
		this->wait_timer = 0;
		this->node->CH_addr = -1;
		this->proc_state = this->PROC_WAITING_TIMEOUT;
	}else if(this->proc_state == this->PROC_WAITING_TIMEOUT){
		this->wait_timer ++;
		if(this->wait_timer >= this->T_MAX){
			this->proc_state = this->PROC_FINAL;
		}else if(this->wait_timer >= this->wait_time){
			//printf("node %d: time out to be ch\n", this->node->addr);
			this->node->CH_addr = this->node->addr;
			this->energy_pre = this->node->energy;
			this->clusterbroadcast_candidance();
			this->wait_time = this->T_MAX + 1;
		}
	}else if(this->proc_state == this->PROC_FINAL){
		if(this->node->CH_addr != this->node->addr){
			int best_ch = this->find_best_ch();
			if(best_ch >= 0){
				this->unicast_join(best_ch);
			}else{
				this->node->CH_addr = this->node->addr;
				this->energy_pre = this->node->energy;
			}
		}
#ifdef _CR_
		if(this->node->CH_addr == this->node->addr){
			this->node->cr->life_state = CR_Proc::LIFE_YOUTH;
			this->node->cr->proc_state = CR_Proc::PROC_INIT;
			this->node->cr->energy_pre = this->node->energy;
		}else{
			this->node->cr->life_state = CR_Proc::LIFE_BABY;
		}
#endif
	}
	return 1;
}

void EDCR_Proc::inclustercast_query_energy()
{
	this->maxenergy_cluster = this->node->energy;
	//this->commproxy->inclustercast(this->node->addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::CTRL_PACKET_SIZE, this->CMD_QUERY_ENERGY, 0, NULL);
	this->commproxy->inclustercast(this->node->addr, 0, this->CMD_QUERY_ENERGY, 0, NULL);
}

void EDCR_Proc::receive_query_energy(int addr)
{
	if(addr == this->node->CH_addr){
		double* data = new double[1];
		data[0] = this->node->energy;
		this->commproxy->unicast(this->node->addr, this->node->CH_addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::DATA_PACKET_SIZE, this->CMD_RESPOND_ENERGY, sizeof(double), (char*)data);
	}else{
		//error
	}
}


void EDCR_Proc::receive_respond_energy(int addr, double energy)
{
	if(energy > this->maxenergy_cluster){
		this->maxenergy_cluster = energy;
	}
}

void EDCR_Proc::maxbroadcast_maxenergy()
{
	double *data = new double[1];
	data[0] = this->maxenergy_cluster;
	this->commproxy->broadcast(this->node->addr, BaseNode::MAX_RADIUS, BaseNode::CTRL_PACKET_SIZE, this->CMD_MAXENERGY, sizeof(double), (char*)data);
}

void EDCR_Proc::receive_maxenergy(int addr, double energy)
{
	if(energy > this->maxenergy_3R){
		this->maxenergy_3R = energy;
	}
}

void EDCR_Proc::clusterbroadcast_candidance()
{
	double *data = new double[1];
	data[0] = this->node->energy;
	this->commproxy->broadcast(this->node->addr, BaseNode::CLUSTER_RADIUS, BaseNode::CTRL_PACKET_SIZE, this->CMD_CANDIDANCE, sizeof(double), (char*)data);
}

void EDCR_Proc::receive_candidance(int ch_addr, double energy)
{
	this->add_candidance(ch_addr, this->calCost(ch_addr, energy));
	if(this->scost != NULL && this->scost->next != NULL){
	//if(this->scost != NULL){
		this->wait_time = this->T_MAX + 1;
	}
}

void EDCR_Proc::unicast_join(int ch_addr)
{
	this->node->CH_addr = ch_addr;
	this->commproxy->unicast(this->node->addr, ch_addr, BaseNode::CTRL_PACKET_SIZE, this->CMD_JOIN, 0, NULL);
}

void EDCR_Proc::receive_join(int addr)
{
	this->add_member(addr);
}

void EDCR_Proc::add_candidance(int addr, double cost)
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

	c->next = NULL;
	if(pre == NULL){
		this->scost = c;
	}else{
		pre->next = c;
	}
}

void EDCR_Proc::add_member(int addr)
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

double EDCR_Proc::calWaitTime()
{
	int t;
	if(this->isfirsttime > 0){
		t = (int)(this->T_MAX * (1 - rand() / (RAND_MAX + 1.0)));
	}else{
		//t = (int)(this->T_MAX * (1 - this->node->energy / (this->maxenergy_3R + 1.0)) * (BaseNetwork::sim_timer / (BaseNetwork::MAX_SIM_TIME + 1.0) + 0.0)
		//		+ this->T_MAX * (1 - rand() / (RAND_MAX + 1.0)) * (1 - BaseNetwork::sim_timer / (BaseNetwork::MAX_SIM_TIME + 1.0) - 0.0));
		
		t = (int)(this->T_MAX * (1 - this->node->energy / (this->maxenergy_3R + 1.0)) * (0.8 + 0.0)
				+ this->T_MAX * (1 - rand() / (RAND_MAX + 1.0)) * (1 - 0.8 - 0.0));
				
		//t = (int)(this->T_MAX * (1 - rand() / (RAND_MAX + 1.0)));
	}
	
	if(t >= 0){
		return t;
	}
	//error
	return 0;
}

double EDCR_Proc::calCost(int ch_addr, double energy)
{
	struct adjv* p = this->cluster_adjv;
	while(p != NULL){
		if(p->addr == ch_addr){
			return pow(p->d, 2) / (this->node->energy + 1.0);
		}
		p = p->next;
	}
	//error
	return 10;
}

int EDCR_Proc::find_best_ch()
{
	struct SCOST* p = this->scost;
	struct SCOST* least_p = p;
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

bool EDCR_Proc::check_energy()
{
	return (this->node->energy < 0.7 * this->energy_pre);
}