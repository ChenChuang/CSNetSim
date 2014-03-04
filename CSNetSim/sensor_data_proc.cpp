#include "sensor_data_proc.h"

SensorDataProc::SensorDataProc(Node* anode, double aperiod, double amax_wait, double aunit): 
	period(aperiod), 
	max_wait(amax_wait),
	unit(aunit),
	fused(0),
	unfused(0),
	node(anode),
	inode(dynamic_cast<INode_SensorDataProc*>(anode)),
	comm(dynamic_cast<ECommProxy_UnicastChannel*>(anode->get_commproxy())),
	sense_timer(new Timer(anode->get_network()->get_clock())),
	wait_timer(new Timer(anode->get_network()->get_clock())),
	wait_hop_timer(new Timer(anode->get_network()->get_clock()))
{
	this->comp = 0.1;
	this->max_wait_hop = 10;
}

SensorDataProc::~SensorDataProc()
{
	delete this->sense_timer;
	this->sense_timer = NULL;
	delete this->wait_timer;
	this->wait_timer = NULL;
	delete this->wait_hop_timer;
	this->wait_hop_timer = NULL;
}

void SensorDataProc::init()
{
	this->sense_timer->set_after(this->period);
	this->wait_timer->set_after(this->max_wait);
}

int SensorDataProc::process(Msg* msg)
{
	if(msg->cmd == SensorDataProc::CMD_SENSE_DATA_FUSED){
		this->fused += *(int*)msg->data;
		return 1;
	}else if(msg->cmd == SensorDataProc::CMD_SENSE_DATA_UNFUSED){
		this->unfused += *(int*)msg->data;
		return 1;
	}
	return -1;
}

void SensorDataProc::ticktock(double time)
{
	if(this->wait_hop_timer->is_timeout()){
		this->force_send();
	}
	if(this->fused > 0){
		this->send_fused();
	}
	if(this->sense_timer->is_timeout()){
		this->unfused += this->unit;
		this->sense_timer->set_after(this->period);
	}
	if(this->wait_timer->is_timeout()){
		if(this->inode->is_ch()){
			this->node->consume(EnergyModel::calFusion(this->unfused));
			this->fused += this->unfused;
			this->unfused = 0;
			this->send_fused();
		}else{
			this->send_unfused();
			this->send_fused();
		}
		this->wait_timer->set_after(this->max_wait);
	}
	this->node->get_network()->get_clock()->try_set_tick(
		std::min(this->wait_timer->get_time(), this->sense_timer->get_time()) - this->node->get_network()->get_clock()->get_time());
}

void SensorDataProc::send_fused()
{
	if(this->send(this->fused * this->comp, this->fused, SensorDataProc::CMD_SENSE_DATA_FUSED)){
		this->fused = 0;
	}
}

void SensorDataProc::send_unfused()
{
	if(this->send(this->unfused, this->unfused, SensorDataProc::CMD_SENSE_DATA_UNFUSED)){
		this->unfused = 0;
	}
}

bool SensorDataProc::send(double size, double data_l, char cmd){
	int next_hop = this->inode->get_next_hop();
	if(next_hop < 0){
		if(!this->wait_hop_timer->is_timing()){
			this->wait_hop_timer->set_after(this->max_wait_hop);
		}
		return false;
	}
	int* data = new int[1];
	data[0] = (int)data_l;
	this->comm->unicast(
		this->node->get_addr(), 
		next_hop, 
		(int)size, 
		cmd, 
		sizeof(double), (char*)data);
	delete[] data;
	this->wait_hop_timer->reset();
	return true;
}

void SensorDataProc::force_send(){
	int next_hop = this->inode->get_next_hop();
	if(next_hop < 0){
		next_hop = ClusteringSimModel::SINK_ADDR;
	}
	int* data = new int[1];
	data[0] = (int)(this->fused);
	this->comm->unicast(
		this->node->get_addr(), 
		next_hop, 
		(int)(this->fused * this->comp), 
		SensorDataProc::CMD_SENSE_DATA_FUSED, 
		sizeof(double), (char*)data);
	delete[] data;
	data = new int[1];
	data[0] = (int)(this->unfused);
	this->comm->unicast(
		this->node->get_addr(), 
		next_hop, 
		(int)(this->unfused * this->comp), 
		SensorDataProc::CMD_SENSE_DATA_FUSED, 
		sizeof(double), (char*)data);
	delete[] data;
	
	this->fused = 0;
	this->unfused = 0;
	
	this->wait_hop_timer->reset();
}
