#include "node.h"

#include "network.h"

Node::Node(Network* anetwork, int aaddr, double ax, double ay, double aenergy):
	network(anetwork),
	commproxy(NULL),
	procs_manager(new ProcsManager(this)),
	addr(aaddr),
	x(ax),
	y(ay),
	energy(aenergy)
{
}

Node::~Node()
{
	delete this->procs_manager;
	this->procs_manager = NULL;
}

void Node::init()
{
	this->procs_manager->init();
}

void Node::clear()
{
	if(this->commproxy != NULL){
		this->commproxy->clear_t_buf();
		this->commproxy->clear_r_buf();
	}
}

void Node::print()
{
	printf("node %4d : location = ( %4f, %4f ), energy = %4f\n", this->addr, this->x, this->y, this->energy);
}

void Node::ticktock()
{
	this->commproxy->clear_t_buf();
	
	MsgIterator* msg_iter = this->commproxy->get_r_msg_iter();
	Msg* msg;
	ProcIterator* proc_iter;
	Processor* proc;
	int result;
	
	while(msg_iter->has_more()){
		msg = msg_iter->next();
		result = -1;
		proc_iter = this->procs_manager->get_proc_iter();
		while(proc_iter->has_more() && result <= 0){
			proc = proc_iter->next();
			if(proc->isTurnOn){
				result = proc->process(msg);
			}
		}
	}
	
	proc_iter = this->procs_manager->get_proc_iter();
	while(proc_iter->has_more()){
		proc = proc_iter->next();
		if(proc->isTurnOn){
			proc->ticktock(this->network->get_clock()->get_time());
		}
	}
	
	this->commproxy->clear_r_buf();
}
