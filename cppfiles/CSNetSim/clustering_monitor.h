#ifndef CLUSTERINGMONITOR_H
#define CLUSTERINGMONITOR_H

#include "monitor.h" // Base class: Monitor

#include "compile_config.h"
#ifdef _vc_
	#include "mex.h"
	#include "mat.h"
#endif
#include <string>
#include "adj_g.h"
#include "node.h"
#include "network.h"
#include <cmath>
using namespace std;


class ClusteringMonitor : public Monitor, public IMonitor_Channel
{
public:
	ClusteringMonitor(Network* anetwork);
	~ClusteringMonitor();
public:
	void record_before_run();
	void record_in_run();
	void record_after_run();
	void record_communicate(Msg* msg, double energy);
private:
	void record_adjg(string file_path, string var_name, AdjG* G);
	void record_periodically(Node** nodes);
	void record_ch(Node** nodes);
	void record_next_hop(Node** nodes);
	
private:
	ClusteringNetwork* network;
	
	double *energy_sum;
	double *alive_sum;
	double *output_track;
	double *energy_snapshot;
	double *node_snapshot;
	double *rotate_times_track;
	double *rotate_overhead_track;
	
	double output;
	
	int record_count;
	int record_needed;
	
	int rotate_times;
	double rotate_overhead;
};

#endif // CLUSTERINGMONITOR_H
