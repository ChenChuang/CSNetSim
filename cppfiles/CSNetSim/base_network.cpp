#include "base_network.h"
#include "sink_node.cpp"

double BaseNetwork::MAX_SIM_TIME;
double BaseNetwork::sim_timer;
double BaseNetwork::sim_slot;
int BaseNetwork::NODE_NUM;

BaseNetwork::BaseNetwork(double* x, double* y)
{
	this->sim_slot = 1;
	this->sim_timer = 0;
	
	//allocate memory for this->nodes;
	this->nodes = new BaseNode* [BaseNetwork::NODE_NUM];
	//initialize nodes
	for(int i = 0; i < BaseNetwork::NODE_NUM; i ++){
		this->nodes[i] = new BaseNode(i, x[i], y[i]);
	}
	//initialize sink
	this->sink = new SinkNode();
	
	//create AdjG
	this->cluster_radius_G = new AdjG(x, y, BaseNode::CLUSTER_RADIUS, BaseNetwork::NODE_NUM);
	this->max_radius_G = new AdjG(x, y, BaseNode::MAX_RADIUS, BaseNetwork::NODE_NUM);
	
	this->setAdjv();
	this->setNetwork();
	//record AdjG
	//this->monitor->record_AdjG("cluster_radius_G.mat", "cluster_radius_G", cluster_radius_G);
	//this->monitor->record_AdjG("max_radius_G.mat", "max_radius_G", max_radius_G);
	
#ifdef _CR_
	for(int i = 0; i < BaseNetwork::NODE_NUM; i++){
		this->nodes[i]->cr->init();
	}
#endif
}

BaseNetwork::~BaseNetwork()
{
	//release nodes
	for(int i = 0; i < BaseNetwork::NODE_NUM; i ++){
		delete this->nodes[i];
	}
	delete this->nodes;
	delete this->sink;
	delete this->cluster_radius_G;
	delete this->max_radius_G;
}

void BaseNetwork::run()
{
	printf("WSN simulation running...\n");
	int node_index;
	//main loop starts
	for(this->sim_timer = 0; this->sim_timer <= this->MAX_SIM_TIME; this->sim_timer += this->sim_slot){
		printf("----------------------------------time: %f----------------------------------\n", this->sim_timer);
		
		//some nodes may out of energy, refresh AdjG
		this->clear_dead_from_AdjG();printf("1");
		this->setAdjv();printf("2");
		
		//record WSN's condition periodically
		this->monitor->record_periodically(this->nodes);printf("3");

		//call the sink's onTimeOut()
		this->sink->onTimeOut();printf("4");

		//call every live node's onTimeOut()
		for(node_index = 0; node_index < BaseNetwork::NODE_NUM; node_index ++){
			if(this->nodes[node_index]->isAlive){
				this->nodes[node_index]->onTimeOut();
			}
		}printf("5");
		//exchange msgs among nodes
		this->communicate();printf("6");
	}
	
	this->monitor->record_periodically(this->nodes);printf("7");
	//record clustering result
	this->monitor->record_CH(this->nodes);printf("8");
	this->monitor->record_CH_ROUTE(this->nodes);printf("9");
	
	//main loop ends
	printf("WSN simulation ends\n");
	printf("rotate times: %d\n", this->monitor->rotate_times);printf("10");
	printf("rotate overhead: %f\n", this->monitor->rotate_overhead);printf("11\n");
	
}

int BaseNetwork::communicate()
{
#ifdef _print_
	printf("----------------------------commicating...------------------------------\n");
#endif
	int t_i;
	int k;
	double d;
	struct msg* mp;
	struct msg* mp_temp;
	struct adjv* vp;
	//process every node's t_msg_buf
	for(t_i = 0; t_i < BaseNetwork::NODE_NUM; t_i ++){
		//the node is dead
		if(!this->nodes[t_i]->isAlive){
			continue;
		}
		//process every msg in the node's t_msg_buf
		mp = this->nodes[t_i]->commproxy->t_msg_buf;
		while(mp != NULL){
			//k is msg's size, used for calculation of energy consumed
			k = mp->size;
			//UNICAST
			if(mp->type == BaseCommProxy::MSG_TYPE_UNICAST){
				if(mp->toaddr == SinkNode::SINK_ADDR){
					d = sqrt( pow( this->nodes[t_i]->x - SinkNode::SINK_X, 2 ) + pow( this->nodes[t_i]->y - SinkNode::SINK_Y, 2 ) );
					//d = 100;
					if(mp->cmd == BaseNode::CMD_SENSE_DATA_UNFUSED){
						this->monitor->output ++;
					}else if(mp->cmd == BaseNode::CMD_SENSE_DATA_FUSED){
						this->monitor->output += *(int*)(mp->data);
					}
				}else{
					//distance between sender and receiver
					d = sqrt( pow( this->nodes[t_i]->x - this->nodes[mp->toaddr]->x, 2 ) + pow( this->nodes[t_i]->y - this->nodes[mp->toaddr]->y, 2 ) );
					//receiver receive the msg 
					this->nodes[mp->toaddr]->commproxy->receive(mp);
					//receiver's energy consumed
					this->nodes[mp->toaddr]->energy -= EnergyModel::calReceive(k);
					if(mp->cmd != BaseNode::CMD_SENSE_DATA_FUSED && mp->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
						this->monitor->rotate_overhead += EnergyModel::calReceive(k);
					}
				}
			}
			//BROADCAST
			else if(mp->type == BaseCommProxy::MSG_TYPE_BROADCAST){
				d = mp->radius;
				//choose diff AdjG for diff transmission radius
				if(mp->radius == BaseNode::CLUSTER_RADIUS){
					vp = this->cluster_radius_G->v[mp->fromaddr]->next;
				}else if(mp->radius == BaseNode::MAX_RADIUS){
					vp = this->max_radius_G->v[mp->fromaddr]->next;
				}else{	
				}
				//every node within the transmission radius should receive the msg
				while(vp != NULL){
					this->nodes[vp->addr]->commproxy->receive(mp);
					this->nodes[vp->addr]->energy -= EnergyModel::calReceive(k);
					if(mp->cmd != BaseNode::CMD_SENSE_DATA_FUSED && mp->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
						this->monitor->rotate_overhead += EnergyModel::calReceive(k);
					}
					vp = vp->next;
				}
			}
			//INCLUSTERCAST
			else if(mp->type == BaseCommProxy::MSG_TYPE_INCLUSTERCAST){
				d = 0;
				struct MEMBER* p = this->nodes[t_i]->members;
				while( p != NULL ){
					d = max( d, sqrt( pow( this->nodes[t_i]->x - this->nodes[p->addr]->x, 2 ) + pow( this->nodes[t_i]->y - this->nodes[p->addr]->y, 2 ) ) );
					this->nodes[p->addr]->commproxy->receive(mp);
					this->nodes[p->addr]->energy -= EnergyModel::calReceive(k);
					if(mp->cmd != BaseNode::CMD_SENSE_DATA_FUSED && mp->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
						this->monitor->rotate_overhead += EnergyModel::calReceive(k);
					}
					p = p->next;
				}
			}else{				
			}
			//sender's energy consumed
			this->nodes[t_i]->energy -= EnergyModel::calTransmit(k, d);
			if(mp->cmd != BaseNode::CMD_SENSE_DATA_FUSED && mp->cmd != BaseNode::CMD_SENSE_DATA_UNFUSED){
				this->monitor->rotate_overhead += EnergyModel::calTransmit(k, d);
			}
			mp_temp = mp;
			mp = mp->next;
			this->nodes[t_i]->commproxy->onTransmitSucceed(mp_temp);
		}
		//end processing current node
	}
	return 1;
}

void BaseNetwork::clear_dead_from_AdjG()
{
	int i, j;
	for(i = 0; i < BaseNetwork::NODE_NUM; i ++){
		if( this->nodes[i]->isAlive && this->nodes[i]->energy <= 0){
			//printf("1.1-");
			this->nodes[i]->isAlive = false;//printf("1.2-");
			this->nodes[i]->commproxy->clear_r_buf();//printf("1.3-");
			this->nodes[i]->commproxy->clear_t_buf();//printf("1.4-");
			this->cluster_radius_G->deleteNode(i);//printf("1.5-");
			this->max_radius_G->deleteNode(i);//printf("1.6-");
			if(this->nodes[i]->CH_addr != this->nodes[i]->addr){
				//printf("1.61-");this->nodes[this->nodes[i]->CH_addr]->remove_member(this->nodes[i]->addr);printf("1.7-");
			}
			//printf("1.8-\n");
		}
		this->nodes[i]->remove_member(i);
	}
}

void BaseNetwork::setAdjv()
{
	int i;
	for(i = 0; i < BaseNetwork::NODE_NUM; i ++){
		this->nodes[i]->heed->set_cluster_adjv(this->cluster_radius_G->v[i]);
		this->nodes[i]->heed->set_max_adjv(this->max_radius_G->v[i]);
		
		this->nodes[i]->cr->set_cluster_adjv(this->cluster_radius_G->v[i]);
		this->nodes[i]->cr->set_max_adjv(this->max_radius_G->v[i]);
		
		this->nodes[i]->edcr->set_cluster_adjv(this->cluster_radius_G->v[i]);
		
		this->nodes[i]->flood->set_max_adjv(this->max_radius_G->v[i]);
	}
}

void BaseNetwork::setNetwork()
{
	int i;
	for(i = 0; i < BaseNetwork::NODE_NUM; i ++){
		this->nodes[i]->cr->network = this;
	}
	this->sink->network = this;
}

bool BaseNetwork::is_cluster_neighbor(int addr1, int addr2)
{
	struct adjv* v1 = this->cluster_radius_G->v[addr1];
	struct adjv* p = v1->next;
	while(p!=NULL){
		if(p->addr == addr2){
			return true;
		}
		p = p->next;
	}
	return false;
}



