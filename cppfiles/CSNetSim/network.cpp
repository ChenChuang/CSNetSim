#include "network.h"

Network::Network(double* x, double* y, int num, double amax_time, double default_tick): nodes_num(num), max_time(amax_time)
{
	this->clock = new Clock(0, default_tick);
	
	this->nodes = new Node* [num];
	for(int i = 0; i < num; i ++){
		this->nodes[i] = NULL;
	}
	
	this->monitor = NULL;
	
	this->channels = new ChannelsManager(this);
}

Network::~Network()
{
	delete this->clock;
	this->clock = NULL;
	delete[] this->nodes;
	this->nodes = NULL;
	delete this->monitor;
	this->monitor = NULL;
	delete this->channels;
	this->channels = NULL;
}

bool Network::check()
{
	for(int i=0; i < this->nodes_num; i++){
		if(this->nodes[i] == NULL){
			printf("some nodes are NULL\n");
			return false;
		}
		if(this->nodes[i]->commproxy == NULL){
			printf("some nodes' commproxy is NULL\n");
			return false;
		}
		if(this->nodes[i]->procs_manager == NULL){
			printf("some nodes' procs_manager is NULL\n");
			return false;
		}
	}
	if(this->monitor == NULL){
		printf("no monitor\n");
		return false;
	}
	if(this->clock == NULL){
		printf("no clock\n");
		return false;
	}
	if(this->channels == NULL || !this->channels->get_channel_iter()->has_more()){
		printf("no channel\n");
		return false;
	}
	return true;
}

void Network::run()
{
	if(!this->check()){
		printf("WSN simulation exits\n");
		return;
	}
	this->monitor->record_before_run();
	printf("WSN simulation running...\n");
	// main loop starts
	int addr;
	while(this->clock->get_time() < this->max_time){
		printf("----------------------------------time: %f----------------------------------\n", this->clock->get_time());
		
		this->clock->tick_setter_init();
		for(addr = 0; addr < this->nodes_num; addr ++){
			if(this->nodes[addr]->is_alive()){
				this->nodes[addr]->ticktock();
			}
		}
		this->communicate();		
		this->clock->tick_setter_exit();
		
		this->monitor->record_in_run();

		this->clock->ticktock();
	}
	// main loop ends
	printf("WSN simulation ends\n");
	this->monitor->record_after_run();
}

void Network::communicate()
{
#ifdef _DEBUG_
	printf("----------------------------communicating...------------------------------\n");
#endif
	int t_i;
	MsgIterator* msg_iter;
	Msg* msg;
	ChannelIterator* channel_iter;
	Channel* channel;
	int done;
	//process every node's t_msg_buf
	for(t_i = 0; t_i < this->nodes_num; t_i ++){
		//the node is dead
		if(this->nodes[t_i]->is_alive()){
			//process every msg in the node's t_msg_buf
			msg_iter = this->nodes[t_i]->get_commproxy()->get_t_msg_iter();
			while(msg_iter->has_more()){
				msg = msg_iter->next();
				done = -1;
				channel_iter = this->channels->get_channel_iter();
				while(done < 0 && channel_iter->has_more()){
					channel = channel_iter->next();
					done = channel->communicate(msg);
				}
			}
		}
		//end processing current node
	}
}



