#include "cr_proc.h"

CR_Proc::CR_Proc(BaseNode* node, BaseCommProxy* commproxy)
{
	this->node = node;
	this->commproxy = commproxy;
	this->scost = NULL;
	this->scost_param = NULL;
	this->neighbors = NULL;
	this->cluster_adjv = NULL;
	this->max_adjv = NULL;
	this->proc_state = this->PROC_INIT;
	this->life_state = this->LIFE_BABY;
	this->old_timer = 0;
	this->is_receive_data = false;
	this->energy_pre = 0;
}

CR_Proc::~CR_Proc()
{	
	struct NEIGHBOR* p = this->neighbors;
	struct NEIGHBOR* pre = NULL;
	while(p != NULL){
		pre = p;
		p = p->next;
		delete pre;
	}
	this->clear_scost();
	this->clear_scost_param();
	
}

void CR_Proc::clear_scost()
{
	struct SCOST* p = this->scost;
	struct SCOST* pre = NULL;
	while(p != NULL){
		pre = p;
		p = p->next;
		delete pre;
	}
	this->scost = NULL;
}

void CR_Proc::clear_scost_param()
{
	struct SCOST_PARAM* p = this->scost_param;
	struct SCOST_PARAM* pre = NULL;
	while(p != NULL){
		pre = p;
		p = p->next;
		delete pre;
	}
	this->scost_param = NULL;
}

void CR_Proc::set_cluster_adjv(struct adjv* a)
{
	this->cluster_adjv = a;
}

void CR_Proc::set_max_adjv(struct adjv* a)
{
	this->max_adjv = a;
}

void CR_Proc::init()
{
	this->neighbors = NULL;
	struct adjv* p_adjv = this->max_adjv;
	struct NEIGHBOR* p_ng = NULL;
	struct NEIGHBOR* pre_ng = NULL;
	while(p_adjv != NULL){
		p_ng = new NEIGHBOR();
		p_ng->addr = p_adjv->addr;
		p_ng->d = p_adjv->d;
		p_ng->d_tosink = this->network->nodes[p_adjv->addr]->d_tosink;
		//p_ng->announce_time = HEED_Proc::HEED_PERIOD + Flood_Proc::FLOOD_PERIOD;
		p_ng->announce_time = 0;
		p_ng->is_youth = false;
		if(pre_ng == NULL){
			this->neighbors = p_ng;
		}else{
			pre_ng->next = p_ng;
		}
		pre_ng = p_ng;
		p_adjv = p_adjv->next;
	}
}

void CR_Proc::change_neighbor(int addr, bool is_youth, double energy)
{
	struct NEIGHBOR* p = this->neighbors;
	while(p != NULL){
		if(p->addr == addr){
			p->is_youth = is_youth;
			p->announce_time = BaseNetwork::sim_timer;
			p->announce_energy = energy;
			//printf("change_neighbor: node: %d\n", p->addr);
			return;
		}
		p = p->next;
	}
}

void CR_Proc::onTimeOut()
{
	this->sync_neighbor_adj();
	if( !this->check_ch_alive()){
		//printf("node %d: ch %d is dead\n", this->node->addr, this->node->CH_addr);
		//printf("node %d: ch %d is dead, try_to_become_baby\n",  this->node->addr, this->node->CH_addr);
		this->proc_state = this->PROC_INIT;
		this->life_state = this->LIFE_ELECT;
		this->energy_pre = this->node->energy;
		this->node->CH_addr = this->node->addr;
		//printf("ch dead\n");
		this->change_next_hop();
		
		if(!this->try_to_become_baby()){
			this->p_broadcast_ch = this->node->energy / EnergyModel::E_INIT;
			double k = rand() / (RAND_MAX + 1.0);
			//if(true){
			if( k < this->p_broadcast_ch){
				this->become_youth();
				//this->maxbroadcast_youth();
				this->p_broadcast_ch = 0;
			}
		}
		return;
	}
	if(this->life_state == this->LIFE_BABY || this->life_state == this->LIFE_INIT ){
		
	}else if(this->life_state == this->LIFE_ELECT){
		if(this->p_broadcast_ch > 0){
			this->p_broadcast_ch = this->p_broadcast_ch * 2;
			double k = rand() / (RAND_MAX + 1.0);
			if( k < this->p_broadcast_ch){
				this->become_youth();
				this->p_broadcast_ch = 0;
			}
		}
	}else if(this->life_state == this->LIFE_YOUTH){
		if(this->node->next_hop_addr == SinkNode::SINK_ADDR && this->node->d_tosink > this->D_TOSINK_THRESHOLD){
			//printf("next hop is sink\n");
			this->change_next_hop();
		}
		if(this->proc_state == this->PROC_WAITING_NEW_BABY){
#ifdef _print_cr_
			printf("node %d: young_timer = %d\n", this->node->addr, this->young_timer);
#endif
			this->young_timer --;
			if(this->young_timer < 0){
				this->proc_state = this->PROC_INIT;
				this->node->CH_addr = this->node->addr;
				//printf("new ch\n");
				this->change_next_hop();
				//printf("node %d: new ch\n",this->node->addr);
			}
		}
		if(this->proc_state == this->PROC_INIT){
			if(this->node->members == NULL && this->node->addr == this->node->CH_addr){
				//printf("node %d: single ch\n", this->node->addr);
				double k = rand() / (RAND_MAX + 1.0);
				if( k < 1 - this->node->energy / EnergyModel::E_INIT || this->node->energy / EnergyModel::E_INIT < 0.5 ){
					if(this->try_to_become_baby()){
						this->maxbroadcast_old();
					}
				}
			}
			else if(this->check_energy()){
				this->inclustercast_query_cost();
				this->proc_state = this->PROC_WAITING_RESPOND_COST;
				
				this->network->monitor->rotate_times ++;
				
//#ifdef _print_cr_
				//printf("node %d: ch starts rotation\n", this->node->addr);
//#endif
			}
		}else if(this->proc_state == this->PROC_WAITING_RESPOND_COST){
			this->proc_state = this->PROC_RESPOND_COST_RECEIVED;
		}else if(this->proc_state == this->PROC_RESPOND_COST_RECEIVED){
#ifdef _cost2_
			//if(true){
			if(this->is_youth_needed()){
				this->set_scost();
				this->waiting_youth_addr = this->find_best_youth();
			}else{
				this->waiting_youth_addr = -1;
			}
#else
			this->waiting_youth_addr = this->find_best_youth();
#endif
			
			if(this->waiting_youth_addr >= 0){
				//printf("node %d 's best youth is %d\n", this->node->addr, this->waiting_youth_addr);
				this->is_receive_youth = false;
				this->unicast_assign_youth(this->waiting_youth_addr);
				this->proc_state = this->PROC_WAITING_NEW_CH;
				this->old_timer = this->MAX_OLD_TIME;
			}else{
				this->proc_state = this->PROC_INIT;
				this->node->clear_member();
				this->node->members = NULL;
				//printf("node %d: i am old, but cannot find best youth, try_to_become_baby\n",  this->node->addr);
				if(this->try_to_become_baby()){
					this->maxbroadcast_old();
				}
			}
		}else if(this->proc_state == this->PROC_WAITING_NEW_CH){
			if(this->is_receive_youth){
				this->maxbroadcast_old();
				//this->node->CH_addr = this->waiting_youth_addr;
				//this->node->CH_addr = this->find_best_ch();
				//printf("node %d: to baby\n", this->node->addr);
				
				this->node->CH_addr = this->find_best_ch_d();
				this->node->CH_addr = this->find_assigned_or_best_ch_d();
				
				this->node->next_hop_addr = this->node->CH_addr;
				this->unicast_req_new_ch_incluster(this->node->CH_addr);
				this->node->clear_member();
				this->node->members = NULL;
				this->life_state = this->LIFE_BABY;
			}else{
				this->old_timer --;
			}
			if(this->old_timer < 0 && !this->is_receive_youth){
				this->proc_state = this->PROC_INIT;
				printf("i don't receive annoucement from assigned youth, try_to_become_baby\n");
				if(this->try_to_become_baby()){
					this->maxbroadcast_old();
				}
			}
		}else{
			
		}
	}else if(this->life_state == this->LIFE_OLD){
		/*
		if( ! this->is_receive_data ){
			this->old_timer ++;
		}else{
			this->old_timer = 0;
		}
		if(this->old_timer > this->MAX_OLD_TIME){
			this->life_state = this->LIFE_BABY;
		}*/
	}
}

int CR_Proc::procMsg(struct msg* mp)
{
	switch ( mp->cmd ){
		case this->CMD_QUERY_COST:{
			this->receive_query_cost(mp->fromaddr);
			break;
		}
		case this->CMD_RESPOND_COST:{
			//using receive_respond_cost
#ifndef _cost2_
			double resp_cost = *(double*)(mp->data);
			this->receive_respond_cost(mp->fromaddr, resp_cost);
#else			
			//using receive_respond_cost2
			double* resp_cost_param = (double*)(mp->data);
			this->receive_respond_cost2(mp->fromaddr, resp_cost_param);
#endif
			break;
		}
		case this->CMD_ASSIGN_YOUTH:{
			this->receive_assign_youth(mp->fromaddr);
			break;
		}
		case this->CMD_ANNOUNCE_YOUTH:{
			this->receive_announce_youth(mp->fromaddr, *((double*)(mp->data)));
			break;
		}
		case this->CMD_ANNOUNCE_OLD:{
			this->receive_announce_old(mp->fromaddr);
			break;
		}
		case this->CMD_REQ_NEW_CH:{
			this->receive_req_new_ch(mp->fromaddr);
			break;
		}
		default:{
			//printf("CR_Proc::procMsg: error\n");
		}
	}
	return 1;
}
	
bool CR_Proc::check_energy()
{
	if(energy_pre <= 0){
		printf("node %d: error in check energy\n", this->node->addr);
		return false;
	}
	bool r = this->node->energy / this->energy_pre < 0.5 || this->node->energy < 10;
	//bool r = this->node->energy / this->energy_pre < 0.5 && (this->energy_pre - this->node->energy) / EnergyModel::E_INIT > 0.05 ;
	//bool r = (this->node->energy / this->energy_pre < 0.5 && (this->energy_pre - this->node->energy) / EnergyModel::E_INIT > 0.05) || this->node->energy < 6 * this->energy_consumed;
	//bool r = this->energy_pre - this->node->energy > 6;
	
	//printf("node %d: %f, %f\n", this->node->addr, this->node->energy, this->energy_pre);
	
	if(r){
#ifdef _print_cr_
		printf("node %d: rotate! %f, %f\n", this->node->addr, this->node->energy, this->energy_pre);
#endif
	}
	return r;
}

double CR_Proc::cal_cost()
{
	double s1 = this->node->energy;
	double s2 = 0;
	double s3 = 0;
	double s4 = 0;
	double maxe = 0;
	double d = 0;
	double c = 2;
	int n = 0;
	int m = 0;
	struct NEIGHBOR* p = this->neighbors;

	while( p!=NULL ){
		if(p->addr == this->node->CH_addr){
			d = p->d;
		}
		if(p->is_youth){
			if(p->d <= BaseNode::CLUSTER_RADIUS){
				s2 += max(0.0, p->announce_energy - ( BaseNetwork::sim_timer - p->announce_time ) * c);
				maxe = max( maxe, max(0.0, p->announce_energy - ( BaseNetwork::sim_timer - p->announce_time ) * c) );
				n ++;
			}else{
				s3 += p->announce_energy - ( BaseNetwork::sim_timer - p->announce_time ) * c;
			}
		}else{
			m ++;
		}
		p = p->next;
	}
	if(n == 0){
	}
	
	//return n*1000/(s1 + 0.0001) + s2 + 0/(s3 + 0.0001);
	//return 1/(s1 + 0.000001);
	//return ( n + 0.000001 )/(s1 > 200 ? s1 : 0.0000001);
	//return d /(s1 > 60 ? s1 : 0.0000001);
	return maxe * ( n + 0.000001 )/(s1 > 60 ? s1 : 0.0000001);
	//return d * maxe * ( n + 0.000001 )/(s1 > 60 ? s1 : 0.0000001);
	//return pow(2, d / BaseNode::CLUSTER_RADIUS) * maxe * ( n + 0.000001 )/(s1 > 60 ? s1 : 0.0000001);
	//return pow(2, d / BaseNode::CLUSTER_RADIUS) / (s1 > 60 ? s1 : 0.0000001);
	//return (s2 + 0.001)/(s1 > 50 ? s1 : 0.0000001);
	//return (s1 > 10 ? (s2 + 0.001)/n*(s1 + 0.0000001) : 1000000000);
	//return (s2 + 0.001)/n*(s1 + 0.0000001);
}

double* CR_Proc::cal_cost2()
{
	double* cost = new double[3];
	cost[1] = this->node->energy;
	
	int n, m;
	n = 0;
	m = 0;
	struct NEIGHBOR* p = this->neighbors;

	while( p!=NULL ){
		if(p->addr == this->node->CH_addr){
			cost[0] = p->d;
		}
		else if(p->d <= BaseNode::CLUSTER_RADIUS){
			if(p->is_youth){
				n ++;
			}else{
				m ++;
			}
		}
		p = p->next;
	}
	cost[2] = n;
	return cost;
}

int CR_Proc::find_best_youth()
{
	struct SCOST* p = this->scost;
	struct SCOST* p_best = this->scost;
	while( p!=NULL ){
		if(p->cost < p_best->cost){
			p_best = p;
		}
		p = p->next;
	}
	if(p_best == NULL){
#ifdef _print_cr_
		printf("node %d: error in find_best_youth\n", this->node->addr);
#endif
		return -1;
	}else{
#ifdef _print_cr_
		printf("node %d 's best youth is %d\n", this->node->addr, p_best->addr);
#endif
		return p_best->addr;
	}
}

int CR_Proc::find_best_next_hop()
{
	if(this->node->d_tosink <= this->D_TOSINK_THRESHOLD){
		return SinkNode::SINK_ADDR;
	}
	struct NEIGHBOR* p = this->neighbors;
	struct NEIGHBOR* p_best = NULL;
	//double c = 0.1;
	//double e_best = -10000;
	//double e;
	double d_best = -1;
	double d;
	while( p!=NULL ){
		if(p->is_youth && p->addr != this->node->next_hop_addr && p->d_tosink < this->node->d_tosink){
			if( p_best == NULL ){
				p_best = p;
				//e_best = p_best->announce_energy - ( BaseNetwork::sim_timer - p_best->announce_time ) * c;
				d_best = sqrt( pow(p->d_tosink, (p->d_tosink > EnergyModel::D_THRESHOLD ? 4:2)) + pow(p->d, 2) );
				//d_best = p->d_tosink;
			}else{
				//e = p->announce_energy - ( BaseNetwork::sim_timer - p->announce_time ) * c;
				d = sqrt( pow(p->d_tosink, (p->d_tosink > EnergyModel::D_THRESHOLD ? 4:2)) + pow(p->d, 2) );
				//d_best = p->d_tosink;
				//if(e > e_best){
				if(d < d_best){
					p_best = p;
					d_best = d;
					//e_best = e;
				}
			}
		}
		p = p->next;
	}
	if(p_best == NULL){
#ifdef _print_cr_
		printf("node %d: error in find_best_next_hop\n", this->node->addr);
#endif
		//return -2;
		return SinkNode::SINK_ADDR;
	}else{
		return p_best->addr;
	}
}
	
int CR_Proc::find_best_ch()
{
	struct NEIGHBOR* p = this->neighbors;
	struct NEIGHBOR* p_best = NULL;
	double c = 2;
	double e_best = -10000;
	double e;
	while( p!=NULL ){
		if(p->is_youth && p->d < BaseNode::CLUSTER_RADIUS && p->addr != this->node->CH_addr){
			if( p_best == NULL ){
				p_best = p;
				e_best = p_best->announce_energy - ( BaseNetwork::sim_timer - p_best->announce_time ) * c;
			}else{
				e = p->announce_energy - ( BaseNetwork::sim_timer - p->announce_time ) * c;
				if(e > e_best){
					p_best = p;
					e_best = e;
				}
			}
		}
		p = p->next;
	}
	if(p_best == NULL){
#ifdef _print_cr_
		printf("node %d: error in find_best_ch\n", this->node->addr);
#endif
		return -1;
	}else{
#ifdef _print_cr_
		printf("node %d: best ch is %d, energy = %f\n", this->node->addr, p_best->addr, p_best->announce_energy);
#endif
		return p_best->addr;
	}
}

int CR_Proc::find_assigned_or_best_ch_d()
{
	struct NEIGHBOR* p = this->neighbors;
	struct NEIGHBOR* p_best = NULL;
	double c = 2;
	double t_best = 10000;
	double t;
	while( p!=NULL ){
		if(p->is_youth && p->d < BaseNode::CLUSTER_RADIUS && p->addr != this->node->CH_addr){
			if( p_best == NULL ){
				p_best = p;
				t_best = BaseNetwork::sim_timer - p_best->announce_time;
			}else{
				t = BaseNetwork::sim_timer - p->announce_time;
				if(t < t_best){
					p_best = p;
					t_best = t;
				}
			}
		}
		p = p->next;
	}
	if( p_best != NULL && t_best <= 2 * BaseNetwork::sim_slot){
		return p_best->addr;
	}
	
	return this->find_best_ch_d();
}

int CR_Proc::find_best_ch_d()
{
	struct NEIGHBOR* p = this->neighbors;
	struct NEIGHBOR* p_best = NULL;
	double d_min = -10000;
	double d;
	while( p!=NULL ){
		if(p->is_youth && p->d < BaseNode::CLUSTER_RADIUS && p->addr != this->node->CH_addr && p->addr != this->node->addr){
			if( p_best == NULL ){
				p_best = p;
				d_min = p_best->d;
			}else{
				d = p->d;
				if(d < d_min){
					p_best = p;
					d_min = d;
				}
			}
		}
		p = p->next;
	}
	if(p_best == NULL){
#ifdef _print_cr_
		printf("node %d: error in find_best_ch\n", this->node->addr);
#endif
		return -1;
	}else{
#ifdef _print_cr_
		printf("node %d: best ch is %d, d = %f\n", this->node->addr, p_best->addr, p_best->d);
#endif
		return p_best->addr;
	}
}


void CR_Proc::inclustercast_query_cost()
{
#ifdef _print_cr_
	printf("node %d inclustercast_query_cost() to: ", this->node->addr);
	struct MEMBER* p = this->node->members;
	while( p!=NULL ){
		printf("%d, ", p->addr);
		p = p->next;
	}
	printf("\n");
#endif
	this->clear_scost();
	this->clear_scost_param();
	this->commproxy->inclustercast(this->node->addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::DATA_PACKET_SIZE, this->CMD_QUERY_COST, 0, NULL);
}

void CR_Proc::receive_query_cost(int addr)
{
#ifdef _print_cr_
	printf("node %d receive_query_cost from %d\n", this->node->addr, addr);
#endif
	if(addr == this->node->addr){
		return;
	}
#ifndef _cost2_
	//using cal_cost
	double* data = new double[1];
	data[0] = this->cal_cost();
	this->commproxy->unicast(this->node->addr, this->node->CH_addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::DATA_PACKET_SIZE, this->CMD_RESPOND_COST, sizeof(double), (char*)data);
#else
	//using cal_cost2
	this->commproxy->unicast(this->node->addr, this->node->CH_addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::DATA_PACKET_SIZE, this->CMD_RESPOND_COST, sizeof(double) * 3, (char*)(this->cal_cost2()));
#endif
}
void CR_Proc::receive_respond_cost(int addr, double cost)
{	
	if(this->node->addr == addr){
		return;
	}
#ifdef _print_cr_
	printf("node %d receive_respond_cost from %d, cost = %f\n", this->node->addr, addr, cost);
#endif
	
	struct SCOST* p = this->scost;
	struct SCOST* pre = p;
	while( p != NULL ){
		if( p->addr == addr ){
			p->cost = cost;
			return;
		}
		pre = p;
		p = p->next;
	}
	p = new SCOST();
	p->addr = addr;
	p->cost = cost;
	p->next = NULL;
	if( pre == NULL ){
		this->scost = p;
	}else{
		pre->next = p;
	}
}

void CR_Proc::receive_respond_cost2(int addr, double* cost)
{
	if(this->node->addr == addr){
		return;
	}
#ifdef _print_cr_
	printf("node %d receive_respond_cost from %d, cost = %f\n", this->node->addr, addr, cost);
#endif
	
	struct SCOST_PARAM* p = this->scost_param;
	struct SCOST_PARAM* pre = p;
	while( p!=NULL ){
		if( p->addr == addr ){
			p->d = cost[0];
			p->e = cost[1];
			p->coverage = (int)(cost[2]);
			return;
		}
		pre = p;
		p = p->next;
	}
	p = new SCOST_PARAM();
	p->addr = addr;
	p->d = cost[0];
	p->e = cost[1];
	p->coverage = (int)(cost[2]);
	p->next = NULL;
	if( pre == NULL ){
		this->scost_param = p;
	}else{
		pre->next = p;
	}	
	//printf("%d\n",cost[2]);
}

bool CR_Proc::is_youth_needed()
{
	struct SCOST_PARAM* p = this->scost_param;
	while(p != NULL){
		if(p->coverage < 5){
			return true;
		}
		p = p->next;
	}
	return false;
}

void CR_Proc::set_scost()
{
	struct SCOST_PARAM* p1 = this->scost_param;
	struct SCOST_PARAM* p2;
	double cost;
	double sum_cover;
	double lamda = 2.7;
	while(p1 != NULL){
		cost = 0;
		sum_cover = 0;
		p2 = this->scost_param;
		while(p2 != NULL){
			if(this->network->is_cluster_neighbor(p1->addr, p2->addr) || (p1->addr == p2->addr)){
				sum_cover += pow(lamda, -(p2->coverage));
			}
			p2 = p2->next;
		}
		//cost = 1*(int)(10 - (p1->d / (0.1*this->node->CLUSTER_RADIUS) + 1)) + 
		//		10*(int)(p1->e / (0.001*EnergyModel::E_INIT) + 1) + 
		//		 1*(1-pow(lamda, -sum_cover));
		cost = 10*(1 - (p1->d / (this->node->CLUSTER_RADIUS) + 1)) + 
			   5*(p1->e / (0.001*EnergyModel::E_INIT)) + 
			   1*(sum_cover);
		cost = 1/cost;
		cost = (p1->e > 60 ? cost : 100000000);
		this->receive_respond_cost(p1->addr, cost);
		p1 = p1->next;
	}
}



void CR_Proc::unicast_assign_youth(int addr)
{
#ifdef _print_cr_
	printf("node %d unicast_assign_youth to %d\n", this->node->addr, addr);
#endif
	this->commproxy->unicast(this->node->addr, addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::DATA_PACKET_SIZE, this->CMD_ASSIGN_YOUTH, 0, NULL);
}
void CR_Proc::receive_assign_youth(int addr)
{
#ifdef _print_cr_
	printf("node %d receive_assign_youth from %d\n", this->node->addr, addr);
#endif
	//this->node->CH_addr = this->node->addr;
	//this->node->next_hop_addr = SinkNode::SINK_ADDR;
	this->become_youth();
}
	
void CR_Proc::maxbroadcast_youth()
{
#ifdef _print_cr_
	printf("node %d maxbroadcast_youth, energy = %f\n", this->node->addr, this->node->energy);
#endif
	double *data = new double[1];
	data[0] = this->node->energy;
	this->commproxy->broadcast(this->node->addr, BaseNode::MAX_RADIUS, BaseNode::CTRL_PACKET_SIZE, this->CMD_ANNOUNCE_YOUTH, sizeof(double), (char*)data);
}

void CR_Proc::receive_announce_youth(int addr, double energy)
{
#ifdef _print_cr_
	printf("node %d receive_announce_youth from %d, energy = %f\n", this->node->addr, addr, energy);
#endif
	this->change_neighbor(addr, true, energy);
	if(this->life_state == this->LIFE_YOUTH && this->proc_state == this->PROC_WAITING_NEW_CH && this->waiting_youth_addr == addr){
		this->is_receive_youth = true;
	}
	//----------
	//--------------
}

void CR_Proc::maxbroadcast_old()
{
#ifdef _print_cr_
	printf("node %d maxbroadcast_old, energy = %f\n", this->node->addr, this->node->energy);
#endif
	this->commproxy->broadcast(this->node->addr, BaseNode::MAX_RADIUS, BaseNode::CTRL_PACKET_SIZE, this->CMD_ANNOUNCE_OLD, 0, NULL);
}

void CR_Proc::receive_announce_old(int addr)
{
#ifdef _print_cr_
	printf("node %d receive_announce_old from %d\n", this->node->addr, addr);
#endif
	this->change_neighbor(addr, false, 0);
	if(this->life_state == this->LIFE_BABY && this->node->CH_addr == addr){
		//int best_ch = this->find_best_ch();
		//int best_ch = this->find_best_ch_d();
		int best_ch = this->find_assigned_or_best_ch_d();
		if(best_ch >= 0){
			this->unicast_req_new_ch_incluster(best_ch);
			this->node->CH_addr = best_ch;
			this->node->next_hop_addr = this->node->CH_addr;
		}else{
//#ifdef _print_cr_
			printf("node %d: no best ch: \n", this->node->addr);
//#endif
			//this->receive_assign_youth(addr);
			/*
			this->life_state = this->LIFE_YOUTH;
			this->energy_pre = this->node->energy + 2*this->MAX_YOUNG_TIME;
			this->proc_state = this->PROC_WAITING_NEW_BABY;
			this->young_timer = this->MAX_YOUNG_TIME;
			*/
			
			this->life_state = this->LIFE_ELECT;
			//this->p_broadcast_ch = this->node->energy / EnergyModel::E_INIT * (0.9 + 0.1 * rand() / (RAND_MAX + 1.0));
			this->p_broadcast_ch = this->node->energy / EnergyModel::E_INIT * (BaseNetwork::sim_timer / BaseNetwork::MAX_SIM_TIME + (1-BaseNetwork::sim_timer / BaseNetwork::MAX_SIM_TIME) * rand() / (RAND_MAX + 1.0));
			double k = rand() / (RAND_MAX + 1.0);
			//if( true ){
			if( k < this->p_broadcast_ch){
				this->become_youth();
				this->p_broadcast_ch = 0;
			}
			//printf("node %d: i am youth because no best ch\n",  this->node->addr);
		}
	}else if(this->node->next_hop_addr = addr){
		//printf("node %d: ch old\n", this->node->addr);
		this->change_next_hop();
	}
}
	
void CR_Proc::unicast_req_new_ch(int addr)
{
#ifdef _print_cr_
	printf("node %d unicast_req_new_ch %d\n", this->node->addr, addr);
#endif
	this->commproxy->unicast(this->node->addr, addr, BaseNode::CTRL_PACKET_SIZE, this->CMD_REQ_NEW_CH, 0, NULL);
}

void CR_Proc::unicast_req_new_ch_incluster(int addr)
{
#ifdef _print_cr_
	printf("node %d unicast_req_new_ch %d\n", this->node->addr, addr);
#endif
	this->commproxy->unicast(this->node->addr, addr, BaseNode::DATA_CTRL_PACKET_SIZE - BaseNode::DATA_PACKET_SIZE, this->CMD_REQ_NEW_CH, 0, NULL);
}

void CR_Proc::receive_req_new_ch(int addr)
{
#ifdef _print_cr_
	printf("node %d receive_req_new_ch from %d\n", this->node->addr, addr);
#endif
	if(this->node->CH_addr != this->node->addr){
		//printf("node %d: new ch\n",this->node->addr);
	}
	//this->node->CH_addr = this->node->addr;
	//this->change_next_hop();
	
	struct MEMBER* p = this->node->members;
	struct MEMBER* pre = p;
	while( p!=NULL ){
		if( p->addr == addr ){
			return;
		}
		pre = p;
		p = p->next;
	}
	p = new MEMBER();
	p->addr = addr;
	p->next = NULL;
	if( pre == NULL ){
		this->node->members = p;
	}else{
		pre->next = p;
	}
}	

void CR_Proc::change_next_hop()
{
#ifdef _singlehop_
	this->node->next_hop_addr = SinkNode::SINK_ADDR;
	//printf("node %d: new ch\n",this->node->addr);
#else
	int best_next_hop = this->find_best_next_hop();
	this->node->next_hop_addr = best_next_hop;
#ifdef _print_cr_
	printf("node %d change_next_hop to %d\n", this->node->addr, best_next_hop);
#endif
#endif
}

bool CR_Proc::try_to_become_baby()
{
	//printf("node %d: try to become baby! %f, %f\n", this->node->addr, this->node->energy, this->energy_pre);
	//int best_ch = this->find_best_ch();
	int best_ch = this->find_best_ch_d();
	if(best_ch >= 0){
		//printf("node %d: single ch to baby of %d\n", this->node->addr, best_ch);
		this->life_state = this->LIFE_BABY;
		this->proc_state = this->PROC_INIT;
		this->node->CH_addr = best_ch;
		this->node->next_hop_addr = this->node->CH_addr;
		this->unicast_req_new_ch(best_ch);
		this->p_broadcast_ch = 0;
	}else{
		//printf("node %d: single ch remain ch\n", this->node->addr);
	}
	return best_ch >= 0;
}

void CR_Proc::become_youth()
{
	this->life_state = this->LIFE_YOUTH;
	this->energy_pre = this->node->energy + 2*this->MAX_YOUNG_TIME;
	this->maxbroadcast_youth();
	this->proc_state = this->PROC_WAITING_NEW_BABY;
	this->young_timer = this->MAX_YOUNG_TIME;
}

bool CR_Proc::check_ch_alive()
{
	struct adjv* p = this->cluster_adjv;
	while( p != NULL ){
		if( p->addr == this->node->CH_addr ){
			return true;
		}
		p = p->next;
	}
	return false;
}

void CR_Proc::sync_neighbor_adj()
{
	struct NEIGHBOR* np = this->neighbors;
	struct NEIGHBOR* np_pre = NULL;
	struct adjv* ap = this->cluster_adjv;
	bool f;
	while( np != NULL ){
		ap = this->cluster_adjv;
		f = false;
		while( ap != NULL && !f){
			if( ap->addr == np->addr ){
				f = true;
			}
			ap = ap->next;
		}
		if(!f){
			if(np_pre == NULL){
				this->neighbors = np->next;
				np_pre = NULL;
				delete np;
				np = this->neighbors;
			}else{
				np_pre->next = np->next;
				delete np;
				np = np_pre->next;
			}
		}else{
			np_pre = np;
			np = np->next;
		}
	}
}

void CR_Proc::print_neighbors()
{
	printf("\n------------node %d-------------\n", this->node->addr);
	struct NEIGHBOR* p = this->neighbors;
	while(p != NULL){
		printf("addr: %d, d: %f, is_y: %d, t: %ld\n", p->addr, p->d, (p->is_youth ? 1 : 0), p->announce_time);
		p = p->next;
	}
	printf("\n");
}