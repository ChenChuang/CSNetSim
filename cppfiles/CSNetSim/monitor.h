#ifndef MONITOR_H
#define MONITOR_H

#include "compile_config.h"
#ifdef _vc_
	#include "mex.h"
	#include "mat.h"
#endif
#include <string>
#include "adj_g.h"
#include "base_node.h"
#include "base_network.h"
#include <cmath>
using namespace std;

class BaseNode;

/**
 * write results of simulation to *.mat
 * */
class Monitor
{
public:
	Monitor();
	~Monitor();

public:
	//record G
	void record_AdjG(string file_path, string var_name, AdjG* G);
	//record the WSN's residential energy on the end of each main loop
	void record_periodically(BaseNode** nodes);
	//record the clustering result
	void record_CH(BaseNode** nodes);
	//record the routing result
	void record_CH_ROUTE(BaseNode** nodes);
	//write the recorded data in array to .mat
	bool wirte_to_mat(string file_path, string var_name, double* array, int row_num, int col_num);

public:
	static double RECORD_PERIOD;
	
public:

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

#endif // MONITOR_H
