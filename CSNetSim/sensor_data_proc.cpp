#include "sensor_data_proc.h"

SensorDataProc::SensorDataProc(Node* anode, double aperiod, double amax_wait, double aunit_l, double abuf_l): 
	period(aperiod), 
	max_wait(amax_wait),
	unit_l(aunit_l),
	buf_l(abuf_l),
	data_l(0),
	node(anode),
	sense_timer(new Timer(anode->get_network()->get_clock())),
	wait_timer(new Timer(anode->get_network()->get_clock()))
{
}

SensorDataProc::~SensorDataProc()
{
	delete this->sense_timer;
	this->sense_timer = NULL;
	delete this->wait_timer;
	this->wait_timer = NULL;
}

void SensorDataProc::init()
{
	this->sense_timer->set_after(0);
	this->wait_timer->set_after(this->max_wait);
}

int SensorDataProc::process(Msg* msg)
{
	if(msg->cmd == SensorDataProc::CMD_SENSE_DATA_FUSED){
		dynamic_cast<ECommProxy_UnicastChannel*>(this->node->get_commproxy())->repost(msg, dynamic_cast<INode_SensorDataProc*>(this->node)->get_next_hop());
		return 1;
	}else if(msg->cmd == SensorDataProc::CMD_SENSE_DATA_UNFUSED){
		this->data_l += *(int*)msg->data;
		return 1;
	}
	return -1;
}

void SensorDataProc::ticktock(double time)
{
	this->sense();
	if(dynamic_cast<INode_SensorDataProc*>(this->node)->is_ch()){
		if(this->data_l >= this->buf_l || this->wait_timer->is_timeout()){
			this->node->consume(EnergyModel::calFusion(this->data_l));
			this->send(SensorDataProc::CMD_SENSE_DATA_FUSED);
			this->wait_timer->set_after(this->max_wait);
		}
	}else{
		if(this->data_l > 0){
			this->send(SensorDataProc::CMD_SENSE_DATA_UNFUSED);
		}
	}
	this->node->get_network()->get_clock()->try_set_tick(
		std::min(this->wait_timer->get_time(), this->sense_timer->get_time()) - this->node->get_network()->get_clock()->get_time());
}

void SensorDataProc::send(char cmd){
	int next_hop = dynamic_cast<INode_SensorDataProc*>(this->node)->get_next_hop();
	if(next_hop < 0){
		//return;
		next_hop = ClusteringSimModel::SINK_ADDR;
	}
	int* data = new int[1];
	data[0] = this->data_l;
	double size = ClusteringSimModel::DATA_PACKET_SIZE;
	if(cmd == SensorDataProc::CMD_SENSE_DATA_UNFUSED){
		size = size * this->data_l / this->unit_l;
	}
	dynamic_cast<ECommProxy_UnicastChannel*>(this->node->get_commproxy())->unicast(
		this->node->get_addr(), 
		next_hop, 
		size, 
		cmd, 
		sizeof(int), (char*)data);
	this->data_l = 0;
	delete[] data;
}

void SensorDataProc::sense()
{
	if(this->sense_timer->is_timeout()){
		this->sense_timer->set_after(this->period);
		this->data_l += this->unit_l;
	}
}