#include "base_node.h"

#ifdef _vc_
	#include "heed_proc.cpp"
#endif
#ifdef _vc_
	#include "flood_proc.cpp"
#endif
#ifdef _vc_
	#include "cr_proc.cpp"
#endif
#ifdef _vc_
	#include "edcr_proc.cpp"
#endif

double BaseNode::CLUSTER_RADIUS;   //metre
double BaseNode::MAX_RADIUS;    //metre
double BaseNode::SENSE_DATA_PERIOD;   //unit time

double BaseNode::DATA_PACKET_SIZE;    //byte
double BaseNode::CTRL_PACKET_SIZE;   //byte
double BaseNode::DATA_CTRL_PACKET_SIZE;  //byte

BaseNode::BaseNode(int address, double l_x, double l_y)
{
	this->commproxy = new BaseCommProxy();
	
	this->heed = new HEED_Proc(this, this->commproxy);
	this->flood = new Flood_Proc(this, this->commproxy);
	this->cr = new CR_Proc(this, this->commproxy);
	this->edcr = new EDCR_Proc(this, this->commproxy);
	
	//init energy is random for HEED
	//this->energy = EnergyModel::E_INIT * ( 0.5 + ( rand() / (RAND_MAX + 1.0) ) * 0.5 );
	this->energy = EnergyModel::E_INIT;
	this->addr = address;
	this->x = l_x;
	this->y = l_y;
	this->isAlive = true;
	this->pre_sense_time = BaseNetwork::sim_timer;
	this->is_sense_ready = false;
	this->d_tosink = sqrt(pow(l_x - SinkNode::SINK_X, 2) + pow(l_y - SinkNode::SINK_Y, 2));
	this->CH_addr = -1;
	
	this->rotate_timer = HEED_Proc::ROTATE_PERIOD;
	this->heed_timer = 0;
	this->flood_timer = 0;
	this->edcr_timer = 0;
	
}

BaseNode::~BaseNode()
{
	//delete communication proxy
	delete this->commproxy;
	
	//delete processor
	delete this->heed;
	delete this->cr;
	delete this->flood;
	delete this->edcr;
	
	//delete members of this node, if it is a CH
	struct MEMBER* p = this->members;
	struct MEMBER* pre = NULL;
	while(p != NULL){
		pre = p;
		p = p->next;
		delete pre;
	}
}

void BaseNode::printStr()
{
	//print this node
	printf("node %4d : location = ( %4f, %4f ), energy = %4f\n", this->addr, this->x, this->y, this->energy);
}

void BaseNode::onTimeOut()
{
///////////////////////////////////////////////// CR //////////////////////////////////////////////////
#ifdef _CR_
	this->checkSensor();
	if(this->rotate_timer < HEED_Proc::ROTATE_PERIOD){
		
		//this->cr->energy_consumed = max( this->cr->energy_consumed, this->cr->energy_pre_period - this->energy );
		this->cr->energy_consumed = this->cr->energy_pre_period - this->energy;
		this->cr->energy_pre_period = this->energy;
		
		//printf("sensing...");
		
		this->commproxy->clear_t_buf();
		
		//process all received sensor data msg
		int sense_packet_count = 0;
		struct msg* mp = this->commproxy->r_msg_buf;
		while(mp != NULL){
			if(mp->cmd == BaseNode::CMD_SENSE_DATA_FUSED){
				this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, mp->data_l, mp->data);
			}else if(mp->cmd == BaseNode::CMD_SENSE_DATA_UNFUSED){
				sense_packet_count ++;
			}else{
				//call CR rotation to process the msg
				this->cr->procMsg(mp);
			}
			mp = mp->next;
		}
		
		//call rotation
		this->cr->onTimeOut();
		
		//un CH send sensor data
		if(this->cr->life_state == this->cr->LIFE_ELECT){
			
		}else{
		if(this->is_sense_ready){
			if(this->addr == this->CH_addr){
				sense_packet_count ++;
			}else{
				//this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_UNFUSED, 0, NULL);
				this->commproxy->unicast(this->addr, this->CH_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_UNFUSED, 0, NULL);
			}
			//printf("node %d: send sensor data to node %d\n", this->addr, this->next_hop_addr);
		}
		if(sense_packet_count > 0 && this->addr == this->CH_addr){
			int* data = new int[1];
			data[0] = sense_packet_count;
#ifndef _singlehop_
			this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, sizeof(int), (char*)data);
#else
			this->commproxy->unicast(this->addr, SinkNode::SINK_ADDR, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, sizeof(int), (char*)data);
#endif
			this->energy -= EnergyModel::calFusion(sense_packet_count * BaseNode::DATA_PACKET_SIZE, sense_packet_count);
		}
		}
		
		this->commproxy->clear_r_buf();
		//run in this branch forever
		this->rotate_timer = 0;
		return;
	}
#ifdef _cr_heed_
	if(this->heed_timer < HEED_Proc::HEED_PERIOD){
		//printf("HEEDing...");
		this->heed->onTimeOut();
		this->heed_timer ++;
		return;
	}
#endif
#ifdef _cr_time_complete_
	if(this->edcr_timer < EDCR_Proc::EDCR_PERIOD){
		//printf("EDCRing...%d",this->edcr_timer);
		if(this->edcr_timer == 0){
			this->edcr->finalize();
			this->edcr->init();
			BaseNetwork::sim_slot = 0.0001;
		}
		this->edcr->onTimeOut();
		this->edcr_timer ++;
		/*
		if(this->edcr_timer == EDCR_Proc::EDCR_PERIOD){
			EDCR_Proc::istimetoroute = 1;
			this->flood->init();
		}*/
		return;
	}
#endif
	if(this->flood_timer < Flood_Proc::FLOOD_PERIOD){
		//printf("Flooding...");
		
#ifdef _singlehop_
		this->flood->onTimeOut();		
#else
		if(this->CH_addr == this->addr){
			this->cr->maxbroadcast_youth();
		}
#endif
		this->flood_timer ++;
		return;
	}
	
	
	
#ifdef _singlehop_
	if(this->CH_addr == this->addr){
		this->next_hop_addr = SinkNode::SINK_ADDR;
	}
#else
	struct msg* mp = this->commproxy->r_msg_buf;
	while(mp != NULL){
		this->cr->procMsg(mp);
		mp = mp->next;
	}
	if(this->CH_addr == this->addr){
		this->next_hop_addr = this->cr->find_best_next_hop();
		//printf("node %d: nexthop: %d\n", this->addr, this->next_hop_addr);
	}
#endif
	BaseNetwork::sim_slot = 1;
	this->rotate_timer = 0;
	this->heed_timer = 0;
	this->edcr_timer = 0;
	this->flood_timer = 0;
	this->cr->energy_pre = this->energy;
#endif
	
	
///////////////////////////////////////////////////// HEED ////////////////////////////////////////////////////////
#ifdef _HEED_
	this->checkSensor();
	if(this->rotate_timer < HEED_Proc::ROTATE_PERIOD){
		
		//printf("sensing...");
		
		this->commproxy->clear_t_buf();
		//process all received sensor data msg
		int sense_packet_count = 0;
		struct msg* mp = this->commproxy->r_msg_buf;
		while(mp != NULL){
			if(mp->cmd == BaseNode::CMD_SENSE_DATA_FUSED){
#ifndef _singlehop_
				this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, mp->data_l, mp->data);
#endif
			}else if(mp->cmd == BaseNode::CMD_SENSE_DATA_UNFUSED){
				sense_packet_count ++;
			}
			mp = mp->next;
		}
		//un CH send sensor data
		if(this->is_sense_ready){
			if(this->addr == this->CH_addr){
				sense_packet_count ++;
			}else{
				//this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_UNFUSED, 0, NULL);
				this->commproxy->unicast(this->addr, this->CH_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_UNFUSED, 0, NULL);
			}
			//printf("node %d: send sensor data to node %d\n", this->addr, this->next_hop_addr);
		}
		if(sense_packet_count > 0 && this->addr == this->CH_addr){
			int* data = new int[1];
			data[0] = sense_packet_count;
#ifndef _singlehop_
			this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, sizeof(int), (char*)data);
#else
			this->commproxy->unicast(this->addr, SinkNode::SINK_ADDR, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, sizeof(int), (char*)data);
#endif
			this->energy -= EnergyModel::calFusion(sense_packet_count * BaseNode::DATA_PACKET_SIZE, sense_packet_count);
			//printf("%d\n", sense_packet_count);
			//printf("fusion: e = %f\n", EnergyModel::calFusion(sense_packet_count * BaseNode::DATA_PACKET_SIZE, sense_packet_count));
			//printf("node %d: send sensor data %d to node %d\n", this->addr, sense_packet_count, this->next_hop_addr);
		}
		
		this->commproxy->clear_r_buf();
		
		this->rotate_timer ++;
		return;
	}
	if(this->heed_timer < HEED_Proc::HEED_PERIOD){
		//printf("HEEDing...");
		if(this->heed_timer == 0){
			this->heed->finalize();
			this->heed->init();
			BaseNetwork::sim_slot = 0.04;
		}
		this->heed->onTimeOut();
		this->heed_timer ++;
		if(this->heed_timer == HEED_Proc::HEED_PERIOD){
			HEED_Proc::istimetoroute = 1;
			this->flood->init();
		}
		return;
	}
	if(this->flood_timer < Flood_Proc::FLOOD_PERIOD){
		//printf("Flooding...");
		this->flood->onTimeOut();
		this->flood_timer ++;
		return;
	}
	this->rotate_timer = 0;
	this->heed_timer = 0;
	this->flood_timer = 0;
	BaseNetwork::sim_slot = 1;
#endif

///////////////////////////////////////////////////// EDCR ////////////////////////////////////////////////////////
#ifdef _EDCR_
	this->checkSensor();
	
	if(this->edcr_timer < EDCR_Proc::EDCR_PERIOD){
		//printf("EDCRing...%d",this->edcr_timer);
		if(this->edcr_timer == 0){
			this->edcr->finalize();
			this->edcr->init();
			BaseNetwork::sim_slot = 0.0001;
		}
		this->edcr->onTimeOut();
		this->edcr_timer ++;
		if(this->edcr_timer == EDCR_Proc::EDCR_PERIOD){
			EDCR_Proc::istimetoroute = 1;
			this->flood->init();
		}
		return;
	}
	if(this->flood_timer < Flood_Proc::FLOOD_PERIOD){
		//printf("Flooding...");
		this->flood->onTimeOut();
		this->flood_timer ++;
		return;
	}
	
	this->edcr->isfirsttime = -1;
	
	if( this->CH_addr == this->addr && this->edcr->check_energy()  && ( (BaseNetwork::sim_timer - EDCR_Proc::last_rotate) > EDCR_Proc::MIN_ROTATE_GAP)){
		//printf("energy consumed: %f", this->edcr->energy_pre - this->energy);
		EDCR_Proc::istimetorotate = 1;
		EDCR_Proc::last_rotate = BaseNetwork::sim_timer;
	}
	
	BaseNetwork::sim_slot = 1;
	this->edcr_timer = EDCR_Proc::EDCR_PERIOD;
	this->flood_timer = Flood_Proc::FLOOD_PERIOD;
	

	if(true){
		
		//printf("sensing...");
		
		this->commproxy->clear_t_buf();
		//process all received sensor data msg
		int sense_packet_count = 0;
		struct msg* mp = this->commproxy->r_msg_buf;
		while(mp != NULL){
			if(mp->cmd == BaseNode::CMD_SENSE_DATA_FUSED){
#ifndef _singlehop_
				this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, mp->data_l, mp->data);
#endif
			}else if(mp->cmd == BaseNode::CMD_SENSE_DATA_UNFUSED){
				sense_packet_count ++;
			}
			mp = mp->next;
		}
		//un CH send sensor data
		if(this->is_sense_ready){
			if(this->addr == this->CH_addr){
				sense_packet_count ++;
			}else{
				//this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_UNFUSED, 0, NULL);
				this->commproxy->unicast(this->addr, this->CH_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_UNFUSED, 0, NULL);
			}
			//printf("node %d: send sensor data to node %d\n", this->addr, this->next_hop_addr);
		}
		if(sense_packet_count > 0 && this->addr == this->CH_addr){
			int* data = new int[1];
			data[0] = sense_packet_count;
#ifndef _singlehop_
			this->commproxy->unicast(this->addr, this->next_hop_addr, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, sizeof(int), (char*)data);
#else
			this->commproxy->unicast(this->addr, SinkNode::SINK_ADDR, BaseNode::DATA_PACKET_SIZE, BaseNode::CMD_SENSE_DATA_FUSED, sizeof(int), (char*)data);
#endif
			this->energy -= EnergyModel::calFusion(sense_packet_count * BaseNode::DATA_PACKET_SIZE, sense_packet_count);
			//printf("%d\n", sense_packet_count);
			//printf("fusion: e = %f\n", EnergyModel::calFusion(sense_packet_count * BaseNode::DATA_PACKET_SIZE, sense_packet_count));
			//printf("node %d: send sensor data %d to node %d\n", this->addr, sense_packet_count, this->next_hop_addr);
		}
		
		this->commproxy->clear_r_buf();
		
		this->rotate_timer ++;
		return;
	}
#endif
}

void BaseNode::checkSensor()
{
	if(BaseNetwork::sim_timer - this->pre_sense_time >= BaseNode::SENSE_DATA_PERIOD){
		this->is_sense_ready = true;
		this->pre_sense_time = BaseNetwork::sim_timer;
	}else{
		this->is_sense_ready = false;
	}
}

void BaseNode::remove_member(int addr)
{
	//printf("1.62-");
	struct MEMBER* head = new MEMBER();
	if(head == NULL){
		return;
	}
	head->next = this->members;//printf("1.64-");
	struct MEMBER* p;
	struct MEMBER* temp;
	p = head;//printf("1.64-");
	
	while(p != NULL){
		if(p->next == NULL){
			break;
		}
		//printf("1.641-");
		if(p->next->addr == addr){
			temp = p->next;//printf("1.65-");
			p->next = p->next->next;//printf("1.66-");
			if(temp != NULL){
				delete temp;
			}
			break;
		}else{
			p = p->next;//printf("1.68-");
		}//printf("1.642-");
	}
	//printf("1.69-");
	this->members = head->next;
	delete head;
}

void BaseNode::clear_member()
{
	struct MEMBER* p = this->members;
	struct MEMBER* pre = NULL;
	while(p != NULL){
		pre = p;
		p = p->next;
		delete pre;
	}
	this->members = NULL;
}