#include "sensor_lcr_proc.h"

SensorLcrProc::SensorLcrProc(Node* anode) : node(anode)
{
	this->inode = dynamic_cast<INode_SensorLcrProc*>(anode);
	this->comm_proxy = dynamic_cast<ClusteringCommProxy*>(anode->get_commproxy());
	this->inetwork = dynamic_cast<INet_SensorLcrProc*>(anode->get_network());
	
	this->chs = new SortedList<lcr::Sch>();
	this->tent_params = new SortedList<lcr::TentParam>();
	this->tents = new SortedList<lcr::Tent>();
	
	this->wait_clustering_timer = new Timer(this->node->get_network()->get_clock());
	this->lcr_timer = new Timer(this->node->get_network()->get_clock());
	this->wait_newch_timer = new Timer(this->node->get_network()->get_clock());
	this->wait_mn_timer = new Timer(this->node->get_network()->get_clock());
	this->wait_self_timer = new Timer(this->node->get_network()->get_clock());
}

SensorLcrProc::~SensorLcrProc()
{
	delete this->chs;
	this->chs = NULL;
	
	delete this->tent_params;
	this->tent_params = NULL;
	
	delete this->tents;
	this->tents = NULL;
	
	delete this->wait_clustering_timer;
	this->wait_clustering_timer = NULL;
	
	delete this->lcr_timer;
	this->lcr_timer = NULL;
	
	delete this->wait_newch_timer;
	this->wait_newch_timer = NULL;
	
	delete this->wait_mn_timer;
	this->wait_mn_timer = NULL;
	
	delete this->wait_self_timer;
	this->wait_self_timer = NULL;
}

void SensorLcrProc::init()
{
	this->proc_state = SensorLcrProc::PROC_SLEEP;
}

void SensorLcrProc::start_clustering()
{
	this->proc_state = SensorLcrProc::PROC_WAIT_CLUSTERING;
	this->inode->start_clustering();
	this->wait_clustering_timer->set_after(this->clustering_time);
}

void SensorLcrProc::start_rotating()
{
	if(this->inode->is_ch()){
		this->proc_state = SensorLcrProc::PROC_CH_CHECK;
	}else{
		this->proc_state = SensorLcrProc::PROC_MN_CHECK;
	}
}

int SensorLcrProc::process(Msg* msg)
{
	switch(msg->cmd)
	{
	case SensorLcrProc::CMD_CH_QUERY:
	{
		this->mn_receive_query(msg->fromaddr);
		break;
	}
	case SensorLcrProc::CMD_MN_RESP:
	{
		this->ch_receive_resp(msg->fromaddr, (double*)(msg->data));
		break;
	}
	case SensorLcrProc::CMD_CH_ASSIGN:
	{
		this->mn_receive_assign(msg->fromaddr, *(int*)(msg->data));
		break;
	}
	case SensorLcrProc::CMD_NEWCH_ANC:
	{
		this->receive_newch_anc(msg->fromaddr, (double*)(msg->data));
		break;
	}
	case SensorLcrProc::CMD_NEWMN_ANC:
	{
		this->receive_newmn_anc(msg->fromaddr);
		break;
	}
	case SensorLcrProc::CMD_JOIN:
	{
		this->receive_join(msg->fromaddr);
		break;
	}
	}
	return 1;
}

void SensorLcrProc::ticktock(double time)
{
	switch(this->proc_state)
	{
	case SensorLcrProc::PROC_SLEEP:
	{
		break;
	}
	case SensorLcrProc::PROC_WAIT_CLUSTERING:
	{
		if(this->wait_clustering_timer->is_timeout())
		{
			this->inode->exit_clustering();
			this->start_rotating();
		}
		break;
	}
	case SensorLcrProc::PROC_MN_CHECK:
	{
		if(!this->check_ch_alive())
		{
			this->proc_state = SensorLcrProc::PROC_ISOLATED;
		}
		break;
	}
	case SensorLcrProc::PROC_CH_CHECK:
	{
		if(this->check_energy())
		{
			this->ch_send_query();
			this->proc_state = SensorLcrProc::PROC_CH_WAIT_TENTS;
		}
		break;
	}
	case SensorLcrProc::PROC_CH_WAIT_TENTS:
	{
		this->cal_tent_costs();
		int newch = this->get_least_cost_newch();
		if(newch > 0)
		{
			this->ch_send_assign(newch);
			this->proc_state = SensorLcrProc::PROC_CH_WAIT_NEWCH;
			this->wait_newch_timer->set_after(this->ch_wait_newch_time);
		}
		else
		{
			this->proc_state = SensorLcrProc::PROC_CH_CHECK;
		}
		break;
	}
	case SensorLcrProc::PROC_MN_WAIT_NEWCH:
	{
		if(this->wait_newch_timer->is_timeout())
		{
			this->proc_state = SensorLcrProc::PROC_ISOLATED;
		}
		break;
	}
	case SensorLcrProc::PROC_CH_WAIT_NEWCH:
	{
		if(this->wait_newch_timer->is_timeout())
		{
			this->newmn_send_anc();
			this->proc_state = SensorLcrProc::PROC_ISOLATED;
		}
		break;
	}
	case SensorLcrProc::PROC_NEWCH_WAIT_MN:
	{
		if(this->wait_mn_timer->is_timeout())
		{
			this->proc_state = SensorLcrProc::PROC_CH_CHECK;
		}
		break;
	}
	case SensorLcrProc::PROC_ISOLATED:
	{
		break;
	}
	}
}

void SensorLcrProc::ch_send_query()
{
	this->comm_proxy->inclustercast(this->node->get_addr(), 0, SensorLcrProc::CMD_CH_QUERY, 0, NULL);
}

void SensorLcrProc::mn_receive_query(int addr)
{
	if(this->proc_state != SensorLcrProc::PROC_MN_CHECK || addr != this->inode->get_ch_addr()){
		return;
	}
	int data_l = sizeof(double)*3 + sizeof(NgbManager*);
	char* data = new char[data_l];
	*((double*)(data) + 0) = this->inetwork->d_between(this->node->get_addr(), addr);
	*((double*)(data) + 1) = this->node->energy;
	*((double*)(data) + 2) = this->cal_fcd();
	*((NgbManager**)(data + sizeof(double)*3)) = this->inode->get_neighbors();
	this->comm_proxy->unicast(this->node->get_addr(), addr, 0, SensorLcrProc::CMD_MN_RESP, data_l, data);
	
	this->newch = -1;
	this->wait_newch_timer->set_after(this->mn_wait_newch_time);
	this->proc_state = SensorLcrProc::PROC_MN_WAIT_NEWCH;
}

void SensorLcrProc::ch_receive_resp(int addr, double* params)
{
	if(this->proc_state != SensorLcrProc::PROC_CH_WAIT_TENTS || !this->inode->get_mnmanager()->has(addr)){
		return;
	}
	this->add_tent_param(addr, params[0], params[1], params[2], (NgbManager*)(&params[3]));
}

void SensorLcrProc::ch_send_assign(int addr)
{
	int* data = new int(addr);
	this->comm_proxy->inclustercast(this->node->get_addr(), 0, SensorLcrProc::CMD_CH_ASSIGN, 1, (char*)data);
	
	this->wait_newch_timer->set_after(this->ch_wait_newch_time);
	this->proc_state = SensorLcrProc::PROC_CH_WAIT_NEWCH;
}

void SensorLcrProc::mn_receive_assign(int addr, int newch)
{
	if(this->proc_state != SensorLcrProc::PROC_MN_WAIT_NEWCH || addr != this->inode->get_ch_addr()){
		return;
	}
	if(addr == this->node->get_addr()){
		this->newch_send_anc();
		
		this->wait_newch_timer->reset();
		this->wait_mn_timer->set_after(this->newch_wait_mn_time);
		this->proc_state = SensorLcrProc::PROC_NEWCH_WAIT_MN;
	}
	this->newch = addr;
}

void SensorLcrProc::newch_send_anc()
{
	double* data = new double[2];
	data[0] = this->inode->get_d_tosink();
	data[1] = this->node->energy;
	this->comm_proxy->broadcast(
		this->node->get_addr(), ClusteringSimModel::MAX_RADIUS, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorLcrProc::CMD_NEWCH_ANC, 
		1, (char*)data);
}

void SensorLcrProc::receive_newch_anc(int addr, double* params)
{
	if(this->proc_state == SensorLcrProc::PROC_MN_WAIT_NEWCH && this->newch == addr){
		this->inode->set_ch_addr(addr);
		
		this->wait_newch_timer->reset();
		this->proc_state = SensorLcrProc::PROC_MN_CHECK;
	}
	this->add_ch(addr, params[0], params[1]);
}

void SensorLcrProc::newmn_send_anc()
{
	this->comm_proxy->broadcast(
		this->node->get_addr(), ClusteringSimModel::MAX_RADIUS, 
		ClusteringSimModel::CTRL_PACKET_SIZE, 
		SensorLcrProc::CMD_NEWMN_ANC, 
		0, NULL);
}

void SensorLcrProc::receive_newmn_anc(int addr)
{
	this->remove_ch(addr);
	if(this->inode->get_next_hop() == addr){
		this->reset_next_hop();
	}
}

void SensorLcrProc::send_join_newch()
{
	this->comm_proxy->unicast(this->node->get_addr(), this->inode->get_ch_addr(), 0, SensorLcrProc::CMD_JOIN, 0, NULL);
}

void SensorLcrProc::send_join()
{
	this->comm_proxy->unicast(this->node->get_addr(), this->inode->get_ch_addr(), ClusteringSimModel::CTRL_PACKET_SIZE, SensorLcrProc::CMD_JOIN, 0, NULL);
}

void SensorLcrProc::receive_join(int addr)
{
	this->inode->get_mnmanager()->add(addr);
}

double SensorLcrProc::cal_fcd()
{
	return this->chs->length();
}

void SensorLcrProc::cal_tent_costs()
{
	double sum_fcd = 0;
	double max_fc = 0;
	double fe = 0;
	double fd = 0;
	double fc = 0;
	double cost = 0;
	lcr::TentParam* ctp;
	lcr::TentParam* tp;
	NgbManager* ngbs;
	
	this->tent_params->seek(0);
	SortedListIter<lcr::TentParam>* iter = new SortedListIter<lcr::TentParam>(this->tent_params);
	while(this->tent_params->has_more()){
		ctp = this->tent_params->next();
		sum_fcd = 0;
		ngbs = ctp->ngbs;
		iter->reset();
		while(iter->has_more()){
			tp = iter->next();
			if(ngbs->find(Ngb(tp->addr, 0)) != NULL){
				sum_fcd += exp(-(tp->fcd));
			}
		}
		ctp->fc = sum_fcd;
		if(sum_fcd > max_fc){
			max_fc = sum_fcd;
		}
	}
	
	this->tents->clear();
	
	this->tent_params->seek(0);
	while(this->tent_params->has_more()){
		ctp = this->tent_params->next();
		fe = ctp->e / ClusteringSimModel::E_INIT;
		fd = 1 - ctp->d / ClusteringSimModel::CLUSTER_RADIUS;
		fc = ctp->fc / max_fc;
		cost = 100.0 * floor(fe * 10.0) + 
			10.0 * floor(fd * 10.0) + 
			1.0 * floor(fc * 10.0);
		this->tents->add(new lcr::Tent(ctp->addr, cost));
	}
	
	delete iter;
}

bool SensorLcrProc::check_ch_alive()
{
	return true;
}

bool SensorLcrProc::check_energy()
{
	return (this->node->energy/this->energy_pre < this->energy_thrd || 
		this->node->energy < this->energy_thrd_2);
}

void SensorLcrProc::reset_next_hop()
{
	int addr = this->get_least_cost_nexthop();
	if(addr >= 0){
		this->inode->set_next_hop(addr);
	}else{
		this->inode->set_next_hop(ClusteringSimModel::SINK_ADDR);
	}
}

void SensorLcrProc::add_tent_param(int addr, double d, double e, double fcd, NgbManager* ngbs)
{
	this->tent_params->add(new lcr::TentParam(addr, d, e, fcd, ngbs));
}

void SensorLcrProc::add_ch(int addr, double d_tosink, double anc_energy)
{
	this->chs->add(new lcr::Sch(
		addr, 
		this->inetwork->d_between(this->node->get_addr(), addr), 
		d_tosink, 
		this->node->get_network()->get_clock()->get_time(), 
		anc_energy));
}

void SensorLcrProc::remove_ch(int addr)
{
	this->chs->remove(lcr::Sch(addr, 0, 0, 0, 0));
}

int SensorLcrProc::get_least_cost_newch()
{
	lcr::Tent* t = this->tents->seek(0);
	if(t != NULL){
		return t->addr;
	}
	return -1;
}

int SensorLcrProc::get_least_cost_nexthop()
{
	lcr::Sch* sc = this->chs->seek(0);
	if(sc != NULL){
		return sc->addr;
	}
	return -1;
}
