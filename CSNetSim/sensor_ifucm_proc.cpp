#include "sensor_heed_proc.h"

SensorIfucmProc::SensorIfucmProc(Node* anode) : node(anode)
{
	this->inode = dynamic_cast<INode_SensorIfucmProc*>(this->node);
	this->min_tick = 0.01;
	this->tent_p = 0.25;
	//this->heed_time = log(1/0.0001) / log(2) * this->min_tick;
	this->ifucm_time = 1;
	this->route_time = 1;
	this->stable_time = ClusteringSimModel::SENSE_DATA_PERIOD * 5;
	
	this->tents = new SortedList<ifucm::Tent>();
	this->timer = new Timer(this->node->get_network()->get_clock());
}

SensorIfucmProc::~SensorIfucmProc()
{
	delete this->tents;
	this->tents = NULL;
	delete this->timer;
	this->timer = NULL;
}

void SensorIfucmProc::init()
{
	this->proc_state = SensorIfucmProc::PROC_SLEEP;
}

double SensorIfucmProc::start_clustering()
{
	this->inode->set_ch_addr(-1);
	this->inode->set_next_hop(-1);
	this->proc_state = SensorIfucmProc::PROC_GETREADY;
	this->timer->set_after(this->ifucm_time);
	return this->ifucm_time;
}

void SensorIfucmProc::exit_clustering()
{
	if(this->inode->get_ch_addr() < 0){
		this->inode->set_ch_addr(this->node->get_addr());
	}
	this->proc_state = SensorIfucmProc::PROC_SLEEP;
	this->timer->set_after(this->route_time + this->stable_time);
}

void SensorIfucmProc::ticktock(double time)
{
	if(this->timer->is_timeout()){
		if(this->proc_state == SensorIfucmProc::PROC_SLEEP){
			this->start_clustering();
			this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		}else{
			this->exit_clustering();
			this->inode->start_route();
		}
	}else{
		if(this->proc_state == SensorIfucmProc::PROC_SLEEP){
			this->node->get_network()->get_clock()->try_set_tick(
				this->timer->get_time() - this->node->get_network()->get_clock()->get_time());
		}else{
			this->proc_clustering();
		}
	}
}

int SensorIfucmProc::process(Msg* msg)
{
	switch(msg->cmd)
	{
	case SensorIfucmProc::CMD_COMPETE:
	{
		this->receive_compete_msg(msg);
		return 1;
	}
	case SensorIfucmProc::CMD_QUIT:
	{
		this->receive_quit_msg(msg);
		return 1;
	}
    case SensorIfucmProc::CMD_CH:
    {
        this->receive_ch_msg(msg);
        return 1;
    }
	case SensorIfucmProc::CMD_JOIN:
	{
		this->receive_join_msg(msg);
		return 1;
	}
	}
	return 0;
}

void SensorIfucmProc::add_member(int addr)
{
	this->inode->get_mnmanager()->add(addr);
}

void SensorIfucmProc::add_tent(int addr, double cost)
{
	this->tents->add(new ifucm::Tent(addr, cost));
}

void SensorIfucmProc::cal_radius_cost()
{
}

void SensorIfucmProc::broadcast_compete_msg()
{
	double* data = new double[1];
	data[0] = this->cost;
	this->add_tent(this->node->get_addr(), this->cost);
	this->comm->broadcast(
		this->node->get_addr(), this->radius, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_COMPETE, 
		sizeof(double), (char*)data);
	delete[] data;
}

void SensorIfucmProc::broadcast_ch_msg()
{
	this->comm->broadcast(
		this->node->get_addr(), ClusteringSimModel::CLUSTER_RADIUS, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_CH, 
		0, NULL);
	delete[] data;
}

int SensorIfucmProc::get_least_cost_tent()
{
	ifucm::Tent* sc = NULL;
	this->tents->seek(0);
	while(this->tents->has_more()){
		sc = this->tents->next();
		return sc->addr;
	}
	return -1;
}

void SensorIfucmProc::join_cluster(int ch_addr)
{
	if(this->inode->is_ch()){
		return;
	}
	this->inode->set_ch_addr(ch_addr);
	this->comm->unicast(
		this->node->get_addr(), ch_addr, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorIfucmProc::CMD_JOIN, 
		0, NULL);
}

int SensorIfucmProc::proc_clustering()
{	
	switch(this->proc_state)
	{
	case SensorIfucmProc::PROC_SLEEP:
	{
		break;
	}
	case SensorIfucmProc::PROC_DONE:
	{
		break;
	}
	case SensorIfucmProc::PROC_GETREADY:
	{
		this->tents->clear();
	    this->cost = -1;
        this->radius = -1;
        this->is_quit = false;     
	
        if(rand() / (RAND_MAX + 1) < this->tent_p){    
		    this->proc_state = SensorIfucmProc::PROC_TENT;
            this->cal_radius_cost();
            this->broadcast_compete_msg(); 
        }else{
		    this->proc_state = SensorIfucmProc::PROC_NOTTENT;
        }
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	case SensorIfucmProc::PROC_TENT:
	{
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	case SensorIfucmProc::PROC_NOTTENT:
	{
        break;
    }
	case SensorIfucmProc::PROC_FINAL:
	{
		
		this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
		break;
	}
	}
	return 1;
}

void SensorHeedProc::receive_ch_msg(Msg* msg)
{
     
}

void SensorHeedProc::receive_compete_msg(Msg* msg)
{
	this->add_tent(msg->fromaddr, ((double*)(msg->data))[0]);
}

void SensorHeedProc::receive_join_msg(Msg* msg)
{
	this->inode->set_ch_addr(this->node->get_addr());
	this->add_member(msg->fromaddr);
}

