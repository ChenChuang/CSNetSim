#include "sensor_heed_proc.h"

SensorHeedProc::SensorHeedProc(Node* anode) : node(anode)
{
	this->inode = dynamic_cast<INode_SensorHeedProc*>(this->node);
	this->c_prob = 0.07;
	this->p_min = 0.0001;
	this->costs = new SortedList<Scost>();
}

SensorHeedProc::~SensorHeedProc()
{
	delete this->costs;
	this->costs = NULL;
}

void SensorHeedProc::init()
{
	this->proc_state = SensorHeedProc::PROC_GETREADY;
}

int SensorHeedProc::process(Msg* msg)
{
	if(msg->cmd == SensorHeedProc::CMD_COST){
		this->receive_cost_msg(msg);
		return 1;
	}else if(msg->cmd == SensorHeedProc::CMD_CH){
		this->receive_ch_msg(msg);
		return 1;
	}else if(msg->cmd == SensorHeedProc::CMD_JOIN){
		this->receive_join_msg(msg);
		return 1;
	}
	return 0;
}

void SensorHeedProc::ticktock(double time)
{
	this->proc_clustering();
}

void SensorHeedProc::add_member(int addr)
{
	this->inode->get_mnmanager()->add(addr);
}

void SensorHeedProc::set_scost_type(int addr, char ch_type)
{
	Scost* sc = this->costs->find(Scost(addr, 0, 0x00));
	sc->type = ch_type;
}

void SensorHeedProc::add_scost(int addr, double cost)
{
	this->costs->add(new Scost(addr, cost, SensorHeedProc::NOT_CH));
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
	this->add_scost(this->node->get_addr(), data[0]);
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
	Scost* sc = NULL;
	this->costs->seek(0);
	while(this->costs->has_more()){
		sc = this->costs->next();
		if(sc->type == SensorHeedProc::TENT_CH || sc->type == SensorHeedProc::FINAL_CH){
			return sc->addr;
		}
	}
	return -1;
}

int SensorHeedProc::get_least_cost_final_ch()
{
	Scost* sc = NULL;
	this->costs->seek(0);
	while(this->costs->has_more()){
		sc = this->costs->next();
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
	case SensorHeedProc::PROC_OFF:
	{
		break;
	}
	case SensorHeedProc::PROC_GETREADY:
	{
		this->costs->clear();
		this->cal_broadcast_cost();
		this->ch_type = SensorHeedProc::NOT_CH;
		
		this->ch_prob = max( this->c_prob * this->node->energy / ClusteringSimModel::E_INIT, this->p_min );
		//this->ch_prob = max( this->c_prob * ( this->node->energy + 10 / pow(2, (double)BaseNetwork::sim_timer) * rand() / (RAND_MAX + 1.0) ) / EnergyModel::E_INIT, this->p_min );
		this->ch_prob_pre = 0;
		this->heed_count = 0;
		
		this->proc_state = SensorHeedProc::PROC_MAIN;
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
							this->set_scost_type(this->node->get_addr(), this->ch_type);
							this->cluster_head_msg();
						}
					}
					else
					{
						if(this->ch_type == SensorHeedProc::NOT_CH)
						{
							this->ch_type = SensorHeedProc::TENT_CH;
							this->set_scost_type(this->node->get_addr(), this->ch_type);
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
					this->set_scost_type(this->node->get_addr(), this->ch_type);
					this->cluster_head_msg();
				}
			}
			else if( rand() / (RAND_MAX + 1.0) <= this->ch_prob )
			{
				if(this->ch_type == SensorHeedProc::NOT_CH){
					this->ch_type = SensorHeedProc::TENT_CH;
					this->set_scost_type(this->node->get_addr(), this->ch_type);
					this->cluster_head_msg();
				}
			}
			this->ch_prob_pre = this->ch_prob;
			this->ch_prob = min( this->ch_prob * 2, 1.0 );
		}else{
			this->proc_state = SensorHeedProc::PROC_FINAL;
		}
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
				this->proc_state = SensorHeedProc::PROC_OFF;
			}
			else
			{
				if(this->heed_count > 4 || rand() / (RAND_MAX + 1.0) < this->c_prob * pow(2, (double)(this->heed_count))){
					this->inode->set_ch_addr(this->node->get_addr());
					this->ch_type = SensorHeedProc::FINAL_CH;
					this->cluster_head_msg();
					this->proc_state = SensorHeedProc::PROC_OFF;
				}else{
					this->heed_count ++;
					this->proc_state = SensorHeedProc::PROC_FINAL;
				}
				
			}
		}else{
			this->inode->set_ch_addr(this->node->get_addr());
			this->ch_type = SensorHeedProc::FINAL_CH;
			this->cluster_head_msg();
			this->proc_state = SensorHeedProc::PROC_OFF;
		}
		break;
	}
	}
	return 1;
}

void SensorHeedProc::receive_ch_msg(Msg* msg)
{
	this->set_scost_type(msg->fromaddr, msg->data[0]);
}

void SensorHeedProc::receive_cost_msg(Msg* msg)
{
	this->add_scost(msg->fromaddr, ((double*)(msg->data))[0]);
}

void SensorHeedProc::receive_join_msg(Msg* msg)
{
	this->inode->set_ch_addr(this->node->get_addr());
	this->add_member(msg->fromaddr);
}

bool SensorHeedProc::has_sch()
{
	Scost* sc = NULL;
	this->costs->seek(0);
	while(this->costs->has_more()){
		sc = this->costs->next();
		if(sc->type == SensorHeedProc::TENT_CH || sc->type == SensorHeedProc::FINAL_CH){
			return true;
		}
	}
	return false;
}
