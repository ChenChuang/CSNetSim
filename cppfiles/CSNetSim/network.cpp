#include "network.h"

Network::Network(double* x, double* y, int num): nodes_num(num)
{
	this->clock = new Clock(0, 1);
	
	this->nodes = new Node* [num];
	for(int i = 0; i < num; i ++){
		this->nodes[i] = NULL;
	}
	
	this->monitor = new Monitor(this);
	
	this->channels = new ChannelsManager(this);
}

Network::~Network()
{
	delete this->clock;
	for(int i = 0; i < this->nodes_num; i ++){
		delete this->nodes[i];
	}
	delete[] this->nodes;
	delete this->monitor;
	delete this->channels;
}

void Network::run()
{
	printf("WSN simulation running...\n");
	int addr;
	// main loop starts
	while(this->clock->get_time() < Network::MAX_SIM_TIME){
		printf("----------------------------------time: %f----------------------------------\n", this->clock->get_time());
		
		this->clock->tick_setter_init();
		for(addr = 0; addr < this->nodes_num; addr ++){
			if(this->nodes[addr]->is_alive()){
				this->nodes[addr]->on_time_out();
			}
		}
		this->communicate();		
		this->clock->tick_setter_exit();
		
		this->monitor->record();

		this->clock->ticktock();
	}
	// main loop ends
	printf("WSN simulation ends\n");
	printf("rotate times: %d\n", this->monitor->rotate_times);
	printf("rotate overhead: %f\n", this->monitor->rotate_overhead);
}

void Network::communicate()
{
#ifdef _print_
	printf("----------------------------commicating...------------------------------\n");
#endif
	int t_i;
	int k;
	double d;
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
			msg_iter = this->nodes[t_i]->commproxy()->t_msg_iter();
			while(msg_iter->has_more()){
				msg = msg_iter->next();
				done = -1;
				channel_iter = this->channels->channel_iter();
				while(done < 0 && channel_iter->has_more()){
					channel = channel_iter->next();
					done = channel->communicate(msg);
				}
			}
		}
		//end processing current node
	}
}



