#include "sensor_heed_proc.h"

SensorHeedProc::SensorHeedProc(Node* anode) : node(anode)
{
	this->inode = dynamic_cast<INode_SensorHeedProc*>(this->node);
	this->min_tick = 0.01;
	this->c_prob = 0.07;
	this->p_min = 0.0001;
	//this->heed_time = log(1/0.0001) / log(2) * this->min_tick;
	this->heed_time = 1;
	this->stable_time = ClusteringSimModel::SENSE_DATA_PERIOD * 5;
	
	this->tents = new SortedList<heed::Tent>();
	this->timer = new Timer(this->node->get_network()->get_clock());
}

SensorHeedProc::~SensorHeedProc()
{
	delete this->tents;
	this->tents = NULL;
	delete this->timer;
	this->timer = NULL;
}

void SensorHeedProc::init()
{
	this->proc_state = SensorHeedProc::PROC_SLEEP;
}

double SensorHeedProc::start_clustering()
{
	this->inode->set_ch_addr(-1);
	this->inode->set_next_hop(-1);
	this->proc_state = SensorHeedProc::PROC_GETREADY;
	this->timer->set_after(this->heed_time);
	return this->heed_time;
}

void SensorHeedProc::exit_clustering()
{
	if(this->inode->get_ch_addr() < 0){
		this->inode->set_ch_addr(this->node->get_addr());
	}
	this->proc_state = SensorHeedProc::PROC_SLEEP;
	this->timer->set_after(this->stable_time);
}

void SensorHeedProc::ticktock(double time)
{
	if(this->timer->is_timeout()){
		if(this->proc_state == SensorHeedProc::PROC_SLEEP){
			this->start_clustering();
			this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		}else{
			this->exit_clustering();
			this->inode->start_route();
		}
	}else{
		if(this->proc_state == SensorHeedProc::PROC_SLEEP){
			this->node->get_network()->get_clock()->try_set_tick(
				this->timer->get_time() - this->node->get_network()->get_clock()->get_time());
		}else{
			this->proc_clustering();
		}
	}
}

int SensorHeedProc::process(Msg* msg)
{
	switch(msg->cmd)
	{
	case SensorHeedProc::CMD_COST:
	{
		this->receive_cost_msg(msg);
		return 1;
	}
	case SensorHeedProc::CMD_CH:
	{
		this->receive_ch_msg(msg);
		return 1;
	}
	case SensorHeedProc::CMD_JOIN:
	{
		this->receive_join_msg(msg);
		return 1;
	}
	}
	return 0;
}

void SensorHeedProc::add_member(int addr)
{
	this->inode->get_mnmanager()->add(addr);
}

void SensorHeedProc::set_tent_type(int addr, char ch_type)
{
	heed::Tent* sc = this->tents->find(heed::Tent(addr, 0, 0x00));
	sc->type = ch_type;
}

void SensorHeedProc::add_tent(int addr, double cost)
{
	this->tents->add(new heed::Tent(addr, cost, SensorHeedProc::NOT_CH));
}

double SensorHeedProc::calAMRP()
{
	double sum = 0;
	int count = 0;
	double d;
	NgbManager* ngbs = this->inode->get_neighbors();
	ngbs->seek(0);
	while(ngbs->has_more()){
		d = ngbs->next()->d;
		if(d <= ClusteringSimModel::CLUSTER_RADIUS){
			count ++;
			sum += EnergyModel::calTransmit(1000, d);
		}
	}
	return sum / count;
}

void SensorHeedProc::cal_broadcast_cost()
{
	double* data = new double[1];
	data[0] = this->calAMRP();
	this->add_tent(this->node->get_addr(), data[0]);
	dynamic_cast<ClusteringCommProxy*>(this->node->get_commproxy())->broadcast(
		this->node->get_addr(), ClusteringSimModel::CLUSTER_RADIUS, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorHeedProc::CMD_COST, 
		sizeof(double), (char*)data);
}

void SensorHeedProc::cluster_head_msg()
{
	if(this->ch_type == SensorHeedProc::TENT_CH || this->ch_type == SensorHeedProc::FINAL_CH){
		char *data = new char[1];
		data[0] = this->ch_type;
		dynamic_cast<ClusteringCommProxy*>(this->node->get_commproxy())->broadcast(
			this->node->get_addr(), ClusteringSimModel::CLUSTER_RADIUS, 
			ClusteringSimModel::CTRL_PACKET_SIZE, 
			SensorHeedProc::CMD_CH, 
			1, data);
	}
}

int SensorHeedProc::get_least_cost_ch()
{
	heed::Tent* sc = NULL;
	this->tents->seek(0);
	while(this->tents->has_more()){
		sc = this->tents->next();
		if(sc->type == SensorHeedProc::TENT_CH || sc->type == SensorHeedProc::FINAL_CH){
			return sc->addr;
		}
	}
	return -1;
}

int SensorHeedProc::get_least_cost_final_ch()
{
	heed::Tent* sc = NULL;
	this->tents->seek(0);
	while(this->tents->has_more()){
		sc = this->tents->next();
		if(sc->type == SensorHeedProc::FINAL_CH){
			return sc->addr;
		}
	}
	return -1;
}

void SensorHeedProc::join_cluster(int ch_addr)
{
	if(this->inode->is_ch()){
		return;
	}
	this->ch_type = SensorHeedProc::NOT_CH;
	this->inode->set_ch_addr(ch_addr);
	dynamic_cast<ClusteringCommProxy*>(this->node->get_commproxy())->unicast(
		this->node->get_addr(), ch_addr, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorHeedProc::CMD_JOIN, 
		0, NULL);
}

int SensorHeedProc::proc_clustering()
{	
	switch(this->proc_state)
	{
	case SensorHeedProc::PROC_SLEEP:
	{
		break;
	}
	case SensorHeedProc::PROC_DONE:
	{
		break;
	}
	case SensorHeedProc::PROC_GETREADY:
	{
		this->tents->clear();
		this->cal_broadcast_cost();
		this->ch_type = SensorHeedProc::NOT_CH;
		
		this->ch_prob = std::max( this->c_prob * this->node->energy / ClusteringSimModel::E_INIT, this->p_min );
		//this->ch_prob = max( this->c_prob * ( this->node->energy + 10 / pow(2, (double)BaseNetwork::sim_timer) * rand() / (RAND_MAX + 1.0) ) / EnergyModel::E_INIT, this->p_min );
		this->ch_prob_pre = 0;
		this->heed_count = 0;
		
		this->proc_state = SensorHeedProc::PROC_MAIN;
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	case SensorHeedProc::PROC_MAIN:
	{
		if( this->ch_prob_pre < 1.0 )
		{
			int lcost_ch = this->get_least_cost_ch();
			if( lcost_ch >= 0 )
			{
				if( lcost_ch == this->node->addr )
				{
					if( this->ch_prob >= 1.0 )
					{
						if(this->ch_type != SensorHeedProc::FINAL_CH)
						{
							this->ch_type = SensorHeedProc::FINAL_CH;
							this->inode->set_ch_addr(this->node->get_addr());
							this->set_tent_type(this->node->get_addr(), this->ch_type);
							this->cluster_head_msg();
						}
					}
					else
					{
						if(this->ch_type == SensorHeedProc::NOT_CH)
						{
							this->ch_type = SensorHeedProc::TENT_CH;
							this->set_tent_type(this->node->get_addr(), this->ch_type);
							this->cluster_head_msg();
						}
					}
				}
			}
			else if(this->ch_prob >= 1.0)
			{
				if(this->ch_type != SensorHeedProc::FINAL_CH)
				{
					this->ch_type = SensorHeedProc::FINAL_CH;
					this->inode->set_ch_addr(this->node->get_addr());
					this->set_tent_type(this->node->get_addr(), this->ch_type);
					this->cluster_head_msg();
				}
			}
			else if( rand() / (RAND_MAX + 1.0) <= this->ch_prob )
			{
				if(this->ch_type == SensorHeedProc::NOT_CH){
					this->ch_type = SensorHeedProc::TENT_CH;
					this->set_tent_type(this->node->get_addr(), this->ch_type);
					this->cluster_head_msg();
				}
			}
			this->ch_prob_pre = this->ch_prob;
			this->ch_prob = std::min( this->ch_prob * 2, 1.0 );
		}else{
			this->proc_state = SensorHeedProc::PROC_FINAL;
		}
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	case SensorHeedProc::PROC_FINAL:
	{
		if( this->ch_type != SensorHeedProc::FINAL_CH )
		{
			int lcost_final_ch = this->get_least_cost_final_ch();
			if( lcost_final_ch >= 0 )
			{
				this->join_cluster(lcost_final_ch);
				this->proc_state = SensorHeedProc::PROC_DONE;
			}
			else
			{
				if(this->heed_count > 2 || rand() / (RAND_MAX + 1.0) < this->c_prob * pow(2, (double)(this->heed_count))){
					this->inode->set_ch_addr(this->node->get_addr());
					this->ch_type = SensorHeedProc::FINAL_CH;
					this->cluster_head_msg();
					this->proc_state = SensorHeedProc::PROC_DONE;
				}else{
					this->heed_count ++;
					this->proc_state = SensorHeedProc::PROC_FINAL;
				}
				
			}
		}else{
			this->inode->set_ch_addr(this->node->get_addr());
			this->ch_type = SensorHeedProc::FINAL_CH;
			this->cluster_head_msg();
			this->proc_state = SensorHeedProc::PROC_DONE;
		}
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	}
	return 1;
}

void SensorHeedProc::receive_ch_msg(Msg* msg)
{
	this->set_tent_type(msg->fromaddr, msg->data[0]);
}

void SensorHeedProc::receive_cost_msg(Msg* msg)
{
	this->add_tent(msg->fromaddr, ((double*)(msg->data))[0]);
}

void SensorHeedProc::receive_join_msg(Msg* msg)
{
	this->inode->set_ch_addr(this->node->get_addr());
	this->add_member(msg->fromaddr);
}

bool SensorHeedProc::has_sch()
{
	heed::Tent* sc = NULL;
	this->tents->seek(0);
	while(this->tents->has_more()){
		sc = this->tents->next();
		if(sc->type == SensorHeedProc::TENT_CH || sc->type == SensorHeedProc::FINAL_CH){
			return true;
		}
	}
	return false;
}
