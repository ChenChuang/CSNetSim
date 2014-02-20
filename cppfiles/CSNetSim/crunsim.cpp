#include "compile_config.h"

#ifdef _MATLAB_
	#include "mex.h"
	#include "mat.h"
#endif

#include "csnetsim.h"
#include "clustering_network.h"
#include "clustering_sim_model.h"
#include "energy_model.h"

void crunsim(double* x, double* y)
{
	ClusteringNetwork* network = new ClusteringNetwork(x, y);
	network->run();
	delete network;
}

#ifdef _MATLAB_
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	int param_num;
	double* params;
	
	// check params
	if (nrhs != 3){
		mexErrMsgTxt("3 input required.");
		return;
	}
	if (nlhs != 0){
		mexErrMsgTxt("0 output required.");
		return;
	}	
	if (mxGetM(prhs[0]) != 1 && mxGetN(prhs[0]) != 1){
		mexErrMsgTxt("param matrix should be 1*N or N*1.");
		return;
	}
	param_num = mxGetM(prhs[0]);
	if (param_num == 1){
		param_num = mxGetN(prhs[0]);
	}
	params = mxGetPr(prhs[0]);
	
	printf("received param num = %d\n", param_num);
	
	// receive params
	double area_size_x = params[0];
	double area_size_y = params[1];
	double node_num = params[2];
	double sink_x = params[3];  //metre
	double sink_y = params[4];
	double sink_addr = params[5];

	double cluster_radius = params[6];   //metre
	double max_radius = params[7];    //metre
	double sense_data_period = params[8];   //unit time

	double data_packet_size = params[9];    //byte
	double ctrl_packet_size = params[10];   //byte
	double data_ctrl_packet_size = params[11];  //byte

	double e_init = params[12];  //J
	double e_elec = params[13];    //J/byte
	double e_fusion = params[14];    //J/byte/source
	double e_amp_freespace = params[15];    //J/byte/m^2
	double e_amp_multipath = params[16];   //J/byte/m^4
	double d_threshold = params[17];   //metre

	double max_sim_time = params[18];    //second
	double record_period = params[19];    //second

	double* nodes_x = mxGetPr(prhs[1]);
	double* nodes_y = mxGetPr(prhs[2]);
	
	// check model
	if(sink_addr != 0){
		mexErrMsgTxt("sinknode's address should be 0");
		return;
	}
	
	ClusteringSimModel::AREA_SIZE_X = area_size_x;
	ClusteringSimModel::AREA_SIZE_Y = area_size_y;
	ClusteringSimModel::CLUSTER_RADIUS = cluster_radius;
	ClusteringSimModel::MAX_RADIUS = max_radius;
	ClusteringSimModel::SENSE_DATA_PERIOD = sense_data_period;	
	ClusteringSimModel::DATA_PACKET_SIZE = data_packet_size;
	ClusteringSimModel::CTRL_PACKET_SIZE = ctrl_packet_size;
	ClusteringSimModel::DATA_CTRL_PACKET_SIZE = data_ctrl_packet_size;	
	ClusteringSimModel::E_INIT = e_init;	
	ClusteringSimModel::NODE_NUM = node_num;
	ClusteringSimModel::SINK_X = sink_x;
	ClusteringSimModel::SINK_Y = sink_y;
	ClusteringSimModel::SINK_ADDR = sink_addr;
	ClusteringSimModel::MAX_SIM_TIME = max_sim_time;
	ClusteringSimModel::RECORD_PERIOD = record_period;
	
	EnergyModel::E_ELEC = e_elec;
	EnergyModel::E_FUSION = e_fusion;
	EnergyModel::E_AMP_FREESPACE = e_amp_freespace;
	EnergyModel::E_AMP_MULTIPATH = e_amp_multipath;
	EnergyModel::D_THRESHOLD = d_threshold;
	
	printf("params passed\n");
	
	// call crunsim
	crunsim(nodes_x, nodes_y);
}
#endif