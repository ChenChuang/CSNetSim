#include "sensor_node.h"

SensorNode::SensorNode(Network* anetwork, int aaddr, double ax, double ay, double energy): 
	Node(anetwork, aaddr, ax, ay, energy), 
	mnmanager(new MnManager(this)),
	ngbs(new NgbManager()),
	ch_addr(-1),
	next_hop(-1),
	d_tosink(sqrt(pow(ax - ClusteringSimModel::SINK_X, 2) + pow(ay - ClusteringSimModel::SINK_Y, 2)))
{
	this->commproxy = new ClusteringCommProxy();
	
	this->testproc = new TestProc(this);
	//this->procs_manager->add(this->testproc);
	
	this->dataproc = new SensorDataProc(this, 
		ClusteringSimModel::SENSE_DATA_PERIOD, 
		ClusteringSimModel::SENSE_DATA_PERIOD, 
		ClusteringSimModel::DATA_PACKET_SIZE);
	this->procs_manager->add(this->dataproc);
	
	this->routeproc = new SensorRouteProc(this);
	this->procs_manager->add(this->routeproc);
	
	this->heedproc = new SensorHeedProc(this);
	this->procs_manager->add(this->heedproc);
	
	this->lcrproc = new SensorLcrProc(this);
	this->procs_manager->add(this->lcrproc);
	
	this->ecpfproc = new SensorEcpfProc(this);
	this->procs_manager->add(this->ecpfproc);
}

SensorNode::~SensorNode()
{
	delete this->mnmanager;
	this->mnmanager = NULL;
	
	delete this->ngbs;
	this->ngbs = NULL;
	
	delete this->commproxy;
	this->commproxy = NULL;
	
	delete this->testproc;
	this->testproc = NULL;
	
	delete this->dataproc;
	this->dataproc = NULL;
	
	delete this->routeproc;
	this->routeproc = NULL;
	
	delete this->heedproc;
	this->heedproc = NULL;
	
	delete this->lcrproc;
	this->lcrproc = NULL;
	
	delete this->ecpfproc;
	this->ecpfproc = NULL;
}

void SensorNode::print()
{
	printf("node %4d : location = ( %4f, %4f ), energy = %4f, ch = %4d, nexthop = %4d, d_tosink = %4f\n", 
		this->addr, this->x, this->y, this->energy, this->ch_addr, this->next_hop, this->d_tosink);
}

void SensorNode::init()
{
	this->procs_manager->init();
	this->dataproc->turn_on();
#ifdef _HEED_
	this->heedproc->turn_on(); this->heedproc->start_clustering();
#endif
#ifdef _LCR_
	this->lcrproc->turn_on(); this->lcrproc->start();
#endif
#ifdef _ECPF_
	this->ecpfproc->turn_on(); this->ecpfproc->start_clustering();
#endif
}

void SensorNode::start_route()
{
	this->routeproc->turn_on();
	this->routeproc->start_route();
}

void SensorNode::stop_route()
{
	this->routeproc->turn_off();
}

void SensorNode::start_cluster_route()
{
	this->heedproc->turn_on();
	this->heedproc->start_clustering();
}

void SensorNode::exit_cluster_route()
{
	this->heedproc->exit_clustering();
	this->heedproc->turn_off();
	this->routeproc->turn_off();
}

void SensorNode::init_neighbors(Adjv* adjv)
{
	Adjv* p = adjv;
	while(p != NULL){
		this->ngbs->add(new Ngb(p->addr, p->d));
		p = p->next;
	}
}

double SensorNode::get_neighbor_d(int addr)
{
	Ngb* ngb = this->ngbs->find(Ngb(addr, 0));
	if(ngb != NULL){
		return ngb->d;
	}
	return -1;
}
