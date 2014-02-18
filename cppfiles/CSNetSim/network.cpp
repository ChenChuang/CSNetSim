#include "network.h"

Network::Network(double* x, double* y)
{
	// clock
	this->clock = new TimeManager(0, 1);
	
	// nodes
	this->nodes = new Node* [Network::NODE_NUM];
	this->nodes[0] = new SinkNode(this, 0, 0, 0, 1);
	for(int i = 1; i < Network::NODE_NUM; i ++){
		this->nodes[i] = new SensorNode(this, i, x[i], y[i], EnergyModel::E_INIT);
	}
	
	// create AdjG
	this->cluster_radius_G = new AdjG(x, y, Node::CLUSTER_RADIUS, Network::NODE_NUM);
	this->max_radius_G = new AdjG(x, y, Node::MAX_RADIUS, Network::NODE_NUM);

	// initialize nodes
	for(int i = 0; i < Network::NODE_NUM; i ++){
		this->nodes[i]->init_procs();
	}
}

Network::~Network()
{
	delete this->clock;
	for(int i = 0; i < Network::NODE_NUM; i ++){
		delete this->nodes[i];
	}
	delete[] this->nodes;
	delete this->cluster_radius_G;
	delete this->max_radius_G;
}

double Network::get_time()
{
	return this->clock->get_time();
}

void Network::try_set_tick(double tick)
{
	this->clock->try_set_tick(tick);
}

Node* Network::get_node(int addr)
{
	return this->nodes[addr];
}

bool Network::is_alive(int addr){
	return this->nodes[addr]->is_alive();
}

void Network::run()
{
	printf("WSN simulation running...\n");
	int addr;
	// main loop starts
	while(this->clock->get_time() < Network::MAX_SIM_TIME){
		printf("----------------------------------time: %f----------------------------------\n", this->clock->get_time());
		
		this->clock->tick_setter_init();
		for(addr = 0; addr < Network::NODE_NUM; addr ++){
			if(this->nodes[addr]->is_alive()){
				this->nodes[addr]->on_time_out();
			}
		}
		this->communicate();		
		this->clock->tick_setter_exit();
		
		this->monitor->record_periodically(this->nodes);
		this->monitor->record_periodically(this->nodes);
		this->monitor->record_CH(this->nodes);
		this->monitor->record_CH_ROUTE(this->nodes);
		
		this->clock->ticktock();
	}
	// main loop ends
	printf("WSN simulation ends\n");
	printf("rotate times: %d\n", this->monitor->rotate_times);
	printf("rotate overhead: %f\n", this->monitor->rotate_overhead);
}

int Network::communicate()
{
#ifdef _print_
	printf("----------------------------commicating...------------------------------\n");
#endif
	int t_i;
	int k;
	double d;
	MsgIterator* msg_iter;
	Msg* msg;
	MnIterator* mn_iter;
	int mn_addr;
	Adjv* vp;
	//process every node's t_msg_buf
	for(t_i = 0; t_i < Network::NODE_NUM; t_i ++){
		//the node is dead
		if(!this->nodes[t_i]->is_alive()){
			continue;
		}
		//process every msg in the node's t_msg_buf
		msg_iter = this->nodes[t_i]->commproxy()->t_msg_iter();
		while(msg_iter->has_more()){
			msg = msg_iter->next();

		}
		//end processing current node
	}
	return 1;
}



