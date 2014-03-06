#ifndef SENSORNODE_H
#define SENSORNODE_H

#include "csnetsim.h"
#include "incluster_channel.h" // Base class: INode_ClusteringMember
#include "clustering_member.h"
#include "clustering_neighbor.h"
#include "clustering_comm_proxy.h"
#include "clustering_sim_model.h"
#include "sensor_data_proc.h"
#include "sensor_route_proc.h"
#include "sensor_heed_proc.h"
#include "sensor_lcr_proc.h"
#include "sensor_ecpf_proc.h"
#include "sensor_ifucm_proc.h"
#include "test_proc.h"


class SensorNode : public Node, 
	public INode_ClusteringMember, 
	public INode_SensorDataProc, 
	public INode_SensorRouteProc, 
	public INode_SensorHeedProc, 
	public INode_SensorLcrProc,
	public INode_SensorEcpfProc,
	public INode_SensorIfucmProc
{
public:
	SensorNode(Network* anetwork, int aaddr, double ax, double ay, double aenergy);
	~SensorNode();
public:
	void print();
	void init();
public:
	void init_neighbors(Adjv* adjv);
	MnManager* get_mnmanager() {return this->mnmanager;}
	NgbManager* get_neighbors() {
		return this->ngbs;
		}
	int get_ch_addr() {return this->ch_addr;}
	void set_ch_addr(int addr) {this->ch_addr = addr;}
	int get_next_hop() {return this->next_hop;}
	void set_next_hop(int addr) {this->next_hop = addr;}
	bool is_ch() {return this->ch_addr == this->addr;}
	double get_d_tosink() {return this->d_tosink;}
	double get_data() {return this->dataproc->fused * this->dataproc->comp + this->dataproc->unfused;}
	void force_send_data() {this->dataproc->force_send();}
	double get_neighbor_d(int addr);
	void start_route();
	void stop_route();
	void start_cluster_route();
	void exit_cluster_route();
	
public:
	MnManager* mnmanager;
	NgbManager* ngbs;
	int ch_addr;
	int next_hop;
	double d_tosink;
	
	TestProc* testproc;
	SensorDataProc* dataproc;
	SensorHeedProc* heedproc;
	SensorRouteProc* routeproc;
	SensorLcrProc* lcrproc;
	SensorEcpfProc* ecpfproc;
	SensorIfucmProc* ifucmproc;
	
friend class Monitor;
friend class ClusteringMonitor;
};

#endif // SENSORNODE_H
