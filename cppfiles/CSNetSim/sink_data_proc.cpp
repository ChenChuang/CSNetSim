#include "sink_data_proc.h"

SinkDataProc::SinkDataProc()
{
	this->data_l = 0;
}

SinkDataProc::~SinkDataProc()
{
}

void SinkDataProc::init()
{
	this->data_l = 0;
}

int SinkDataProc::process(Msg* msg)
{
	if(msg->cmd != SensorDataProc::CMD_SENSE_DATA_FUSED && msg->cmd != SensorDataProc::CMD_SENSE_DATA_UNFUSED){
		return -1;
	}
	this->data_l += (double)(*(int*)(msg->data));
	return 1;
}

void SinkDataProc::ticktock(double time)
{
}
