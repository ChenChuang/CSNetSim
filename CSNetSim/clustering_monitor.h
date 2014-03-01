#ifndef CLUSTERINGMONITOR_H
#define CLUSTERINGMONITOR_H

#include "csnetsim.h"
#include "clustering_sim_model.h"
#include "sink_node.h"

#include <string>
#include <cmath>

class ClusteringNetwork;

class ClusteringMonitor : public Monitor, public IMonitor_Channel
{
public:
	ClusteringMonitor(ClusteringNetwork* anetwork);
	~ClusteringMonitor();
public:
	void record_before_run();
	void record_in_run();
	void record_after_run();
	void record_communicate(Msg* msg, double energy);
private:
	void record_xy(Node** nodes);
	void record_adjg(std::string file_path, std::string var_name, AdjG* G);
	void record_periodically(Node** nodes);
	void record_ch(Node** nodes);
	void record_next_hop(Node** nodes);
	
private:
	ClusteringNetwork* network;
	
	double *time;
	double *energy_sum;
	double *alive_sum;
	double *energy_snapshot;
	double *ch_snapshot;
	double *hop_snapshot;
	double output;
	double *output_track;
	int rotate_times;
	double *rotate_times_track;
	double rotate_overhead;
	double *rotate_overhead_track;
	double fnd;
	double lnd;
	
	int record_count;
	int max_records;	
	double step;
	Timer* timer;
};

#endif // CLUSTERINGMONITOR_H
