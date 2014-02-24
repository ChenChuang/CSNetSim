#include "sensor_route_proc.h"

SensorRouteProc::SensorRouteProc(Node* anode) : node(anode)
{
	this->min_tick = 1;
	this->inode = dynamic_cast<INode_SensorRouteProc*>(this->node);
	this->chs = new SortedList<Sch>();
}

SensorRouteProc::~SensorRouteProc()
{
	delete this->chs;
	this->chs = NULL;
}

void SensorRouteProc::init()
{
	this->proc_state = SensorRouteProc::PROC_OFF;
}

int SensorRouteProc::process(Msg* msg)
{
	switch(msg->cmd)
	{
	case SensorRouteProc::CMD_CH:
	{
		if(this->inode->is_ch()){
			double* data = (double*)(msg->data);
			this->chs->add(new Sch(msg->fromaddr, data[0], this->inode->get_neighbor_d(msg->fromaddr)));
		}
		return 1;
	}
	}
	return -1;
}

void SensorRouteProc::ticktock(double time)
{
	switch(this->proc_state)
	{
	case SensorRouteProc::PROC_OFF:
	{
		return;
	}
	case SensorRouteProc::PROC_GETREADY:
	{
		if(this->inode->is_ch())
		{
			double* data = new double[1];
			data[0] = this->inode->get_d_tosink();
			dynamic_cast<ClusteringCommProxy*>(this->node->get_commproxy())->broadcast(
				this->node->get_addr(), ClusteringSimModel::MAX_RADIUS, 
				ClusteringSimModel::CTRL_PACKET_SIZE, 
				SensorRouteProc::CMD_CH, 
				sizeof(double), (char*)data);
		}
		this->proc_state = SensorRouteProc::PROC_CHOOSE;
		break;
	}
	case SensorRouteProc::PROC_CHOOSE:
	{
		if(this->inode->is_ch())
		{
			this->inode->set_next_hop(this->get_best_ch());
		}
		else
		{
			this->inode->set_next_hop(this->inode->get_ch_addr());
		}
		this->proc_state = SensorRouteProc::PROC_OFF;
		break;
	}
	}
	this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
}

void SensorRouteProc::start_route()
{
	this->proc_state = SensorRouteProc::PROC_GETREADY;
	this->chs->clear();
	this->inode->set_next_hop(-1);
	this->node->get_network()->get_clock()->try_set_tick(this->min_tick);
}

int SensorRouteProc::get_best_ch()
{
	this->chs->seek(0);
	Sch* sc;
	while(this->chs->has_more()){
		sc = this->chs->next();
		if(sc->d_tosink < this->inode->get_d_tosink()){
			return sc->addr;
		}
	}
	return ClusteringSimModel::SINK_ADDR;
}
