#include "base_network.h"

BaseNetwork::BaseNetwork(double* x, double* y)
{
	// clock
	this->clock = new TimeManager(0, 1);
	
	// nodes
	this->nodes = new BaseNode* [BaseNetwork::NODE_NUM];
	for(int i = 0; i < BaseNetwork::NODE_NUM; i ++){
		this->nodes[i] = new BaseNode(i, x[i], y[i]);
	}
	// sink node
	this->sink = new SinkNode();
	
	// create AdjG
	this->cluster_radius_G = new AdjG(x, y, BaseNode::CLUSTER_RADIUS, BaseNetwork::NODE_NUM);
	this->max_radius_G = new AdjG(x, y, BaseNode::MAX_RADIUS, BaseNetwork::NODE_NUM);

	// initialize nodes
	for(int i = 0; i < BaseNetwork::NODE_NUM; i ++){
		this->nodes[i]->init_procs();
	}
}

BaseNetwork::~BaseNetwork()
{
	delete this->clock;
	for(int i = 0; i < BaseNetwork::NODE_NUM; i ++){
		delete this->nodes[i];
	}
	delete[] this->nodes;
	delete this->sink;
	delete this->cluster_radius_G;
	delete this->max_radius_G;
}

double BaseNetwork::get_time()
{
	return this->clock->get_time();
}

void BaseNetwork::try_set_tick(double tick)
{
	this->clock->try_set_tick(tick);
}

bool BaseNetwork::is_alive(int addr){
	return this->nodes[addr]->is_alive();
}

void BaseNetwork::run()
{
	printf("WSN simulation running...\n");
	int addr;
	// main loop starts
	while(this->clock->get_time() < BaseNetwork::MAX_SIM_TIME){
		printf("----------------------------------time: %f----------------------------------\n", this->clock->get_time());
		
		this->clock->tick_setter_init();		
		this->sink->on_time_out();
		for(addr = 0; addr < BaseNetwork::NODE_NUM; addr ++){
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

int BaseNetwork::communicate()
{
#ifdef _print_
	printf("----------------------------commicating...------------------------------\n");
#endif
	int t_i;
	int k;
	double d;
	struct MsgIterator* msg_iter;
	struct Msg* msg;
	struct Adjv* vp;
	//process every node's t_msg_buf
	for(t_i = 0; t_i < BaseNetwork::NODE_NUM; t_i ++){
		//the node is dead
		if(!this->nodes[t_i]->is_alive()){
			continue;
		}
		//process every msg in the node's t_msg_buf
		msg_iter = this->nodes[t_i]->get_commproxy()->t_msg_iter();
		while(msg_iter->has_more()){
			msg = msg_iter->next();
			//k is msg's size, used for calculation of energy consumed
			k = msg->size;
			//UNICAST
			if(msg->type == BaseCommProxy::MSG_TYPE_UNICAST){
				if(msg->toaddr == SinkNode::SINK_ADDR){
					d = sqrt( pow( this->nodes[t_i]->x - SinkNode::SINK_X, 2 ) + pow( this->nodes[t_i]->y - SinkNode::SINK_Y, 2 ) );
					//d = 100;
					if(msg->cmd == BaseNode::CMD_SENSE_DATA_UNFUSED){
						this->monitor->output ++;
					}else if(msg->cmd == BaseNode::CMD_SENSE_DATA_FUSED){
						this->monitor->output += *(int*)(msg->data);
					}
				}else{
					//distance between sender and receiver
					d = sqrt( pow( this->nodes[t_i]->x - this->nodes[msg->toaddr]->x, 2 ) + pow( this->nodes[t_i]->y - this->nodes[msg->toaddr]->y, 2 ) );
					if(this->nodes[msg->toaddr]->is_alive()){
						this->nodes[msg->toaddr]->get_commproxy()->receive(msg);
						this->nodes[msg->toaddr]->energy -= EnergyModel::calReceive(k);
						if(msg->cmd != BaseNode::CMD_SENSE_DATA_FUSED && msg->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
							this->monitor->rotate_overhead += EnergyModel::calReceive(k);
						}
					}
				}
			}
			//BROADCAST
			else if(msg->type == BaseCommProxy::MSG_TYPE_BROADCAST){
				d = msg->radius;
				//choose diff AdjG for diff transmission radius
				if(msg->radius == BaseNode::CLUSTER_RADIUS){
					vp = this->cluster_radius_G->v[msg->fromaddr]->next;
				}else if(msg->radius == BaseNode::MAX_RADIUS){
					vp = this->max_radius_G->v[msg->fromaddr]->next;
				}else{	
				}
				//every node within the transmission radius should receive the msg
				while(vp != NULL){
					if(this->nodes[vp->addr]->is_alive()){
						this->nodes[vp->addr]->get_commproxy()->receive(msg);
						this->nodes[vp->addr]->energy -= EnergyModel::calReceive(k);
						if(msg->cmd != BaseNode::CMD_SENSE_DATA_FUSED && msg->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
							this->monitor->rotate_overhead += EnergyModel::calReceive(k);
						}
					}
					vp = vp->next;
				}
			}
			//INCLUSTERCAST
			else if(msg->type == BaseCommProxy::MSG_TYPE_INCLUSTERCAST){
				d = 0;
				struct MEMBER* p = this->nodes[t_i]->members;
				while( p != NULL ){
					d = max( d, sqrt( pow( this->nodes[t_i]->x - this->nodes[p->addr]->x, 2 ) + pow( this->nodes[t_i]->y - this->nodes[p->addr]->y, 2 ) ) );
					if(this->nodes[msg->toaddr]->is_alive()){
						this->nodes[p->addr]->get_commproxy()->receive(msg);
						this->nodes[p->addr]->energy -= EnergyModel::calReceive(k);
						if(msg->cmd != BaseNode::CMD_SENSE_DATA_FUSED && msg->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
							this->monitor->rotate_overhead += EnergyModel::calReceive(k);
						}
					}
					p = p->next;
				}
			}else{				
			}
			//sender's energy consumed
			this->nodes[t_i]->energy -= EnergyModel::calTransmit(k, d);
			if(msg->cmd != BaseNode::CMD_SENSE_DATA_FUSED && msg->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
				this->monitor->rotate_overhead += EnergyModel::calTransmit(k, d);
			}
		}
		//end processing current node
	}
	return 1;
}



