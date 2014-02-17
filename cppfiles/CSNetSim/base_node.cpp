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

BaseNode::BaseNode(BaseNetwork* anetwork, int aaddr, double ax, double ay):
	network(anetwork),
	commproxy(new BaseCommProxy()),
	mnmanager(new MnManager(this)),
	procs(NULL),
	energy(EnergyModel::E_INIT),
	addr(aaddr),
	x(ax),
	y(ay),
	d_tosink(sqrt(pow(ax - SinkNode::SINK_X, 2) + pow(ay - SinkNode::SINK_Y, 2))),
	ch_addr(-1)
{
}

BaseNode::~BaseNode()
{
	//delete communication proxy
	delete this->commproxy;
	delete this->mnmanager;
	this->clear_procs();
}

void BaseNode::print()
{
	//print this node
	printf("node %4d : location = ( %4f, %4f ), energy = %4f\n", this->addr, this->x, this->y, this->energy);
}

void BaseNode::on_time_out()
{
	ProcNode* p;
	MsgIterator* msg_iter = this->commproxy->r_msg_iter();
	Msg* msg;
	int result;
	while(msg_iter->has_more()){
		msg = msg_iter->next();
		p = this->procs;
		result = -1;
		while(p != NULL && result <= 0){
			result = p->proc->process(msg);
		}
	}
	p = this->procs;
	while(p != NULL){
		p->proc->ticktock(this->network->get_time());
	}
}

void BaseNode::add_proc(Processor* proc)
{
	ProcNode* p = this->procs;
	ProcNode* pp = NULL;
	while(p != NULL){
		pp = p;
		p = p->next;
	}
	p = new ProcNode(proc);
	if(pp != NULL){
		pp->next = p;
	}else{
		this->procs = p;
	}
}

void BaseNode::init_procs()
{
	p = this->procs;
	while(p != NULL){
		p->proc->init();
	}
}

void BaseNode::clear_procs()
{
	ProcNode* p = this->procs;
	ProcNode* np;
	while(p != NULL){
		np = p->next;
		delete np;
		p = np;
	}
}
