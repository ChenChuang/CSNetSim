#include "sensor_proc.h"

SensorProc::SensorProc(Node* anode, double aperiod, double aunit_l, double abuf_l): 
	period(aperiod), 
	unit_l(aunit_l),
	buf_l(abuf_l),
	data_l(0),
	node(anode),
	sense_timer(new Timer(anode->get_network()->get_clock())),
	wait_timer(new Timer(anode->get_network()->get_clock()))
{
}

SensorProc::~SensorProc()
{
	delete this->sense_timer;
	this->sense_timer = NULL;
	delete this->wait_timer;
	this->wait_timer = NULL;
}

int SensorProc::process(Msg* msg)
{
	if(msg->cmd == SensorProc::CMD_SENSE_DATA_FUSED){
		dynamic_cast<ECommProxy_UnicastChannel*>(this->node->get_commproxy())->repost(msg, dynamic_cast<INode_SensorProc*>(this->node)->get_next_hop());
		return 1;
	}else if(msg->cmd == SensorProc::CMD_SENSE_DATA_UNFUSED){
		this->data_l += *(int*)msg->data;
		return 1;
	}
	return -1;
}

void SensorProc::ticktock(double time)
{
	this->sense();
	if(dynamic_cast<INode_SensorProc*>(this->node)->is_ch()){
		if(this->data_l >= this->buf_l){
			this->node->consume(EnergyModel::calFusion(this->data_l));
			this->send(SensorProc::CMD_SENSE_DATA_FUSED);
		}
	}else{
		if(this->data_l > 0){
			this->send(SensorProc::CMD_SENSE_DATA_UNFUSED);
		}
	}
}

void SensorProc::send(char cmd){
	int* data = new int[1];
	data[0] = this->data_l;
	dynamic_cast<ECommProxy_UnicastChannel*>(this->node->get_commproxy())->unicast(
		this->node->get_addr(), 
		dynamic_cast<INode_SensorProc*>(this->node)->get_next_hop(), 
		ClusteringSimModel::DATA_PACKET_SIZE, 
		cmd, 
		sizeof(int), (char*)data);
	delete[] data;
}

void SensorProc::sense()
{
	if(this->sense_timer->is_timeout()){
		this->sense_timer->set_after(this->period);
		this->data_l += this->unit_l;
	}
}