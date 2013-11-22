#include "compile_config.h"

#ifdef _vc_
	#define CRUNSIM
#endif
#ifdef CRUNSIM

#include "mex.h"
#include "mat.h"
#include "energy_model.cpp"
#include "base_comm_proxy.cpp"
#include "base_node.cpp"
#include "base_network.cpp"
#include "monitor.cpp"
#include <stdio.h>


int crunsim(double* x, double* y)
{
	
	Monitor* monitor = new Monitor();
	
	BaseNetwork* network = new BaseNetwork(x, y);
	network->monitor = monitor;
	//start main loop
	network->run();
	delete network;
	
	delete monitor;
	return 1;

}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	int param_num;
	double* params;
	
	//all params of WSN
	double AREA_SIZE_X;
	double AREA_SIZE_Y;
	double NODE_NUM;
	double SINK_X;  //metre
	double SINK_Y;
	int SINK_ADDR;

	double CLUSTER_RADIUS;   //metre
	double MAX_RADIUS;    //metre
	double SENSE_DATA_PERIOD;   //unit time

	double DATA_PACKET_SIZE;    //byte
	double CTRL_PACKET_SIZE;   //byte
	double DATA_CTRL_PACKET_SIZE;  //byte

	double E_INIT;  //J
	double E_ELEC;    //J/byte
	double E_FUSION ;    //J/byte/source
	double E_AMP_FREESPACE;    //J/byte/m^2
	double E_AMP_MULTIPATH ;   //J/byte/m^4
	double D_THRESHOLD;   //metre

	double MAX_SIM_TIME;    //second
	double RECORD_PERIOD;   //second
	
	double* nodes_x;
	double* nodes_y;
	
	double* result;
	
	//check params
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
	
	//receive params
	AREA_SIZE_X = params[0];
	AREA_SIZE_Y = params[1];
	NODE_NUM = params[2];
	SINK_X = params[3];  //metre
	SINK_Y = params[4];
	SINK_ADDR = params[5];

	CLUSTER_RADIUS = params[6];   //metre
	MAX_RADIUS = params[7];    //metre
	SENSE_DATA_PERIOD = params[8];   //unit time

	DATA_PACKET_SIZE = params[9];    //byte
	CTRL_PACKET_SIZE = params[10];   //byte
	DATA_CTRL_PACKET_SIZE = params[11];  //byte

	E_INIT = params[12];  //J
	E_ELEC = params[13];    //J/byte
	E_FUSION = params[14];    //J/byte/source
	E_AMP_FREESPACE = params[15];    //J/byte/m^2
	E_AMP_MULTIPATH = params[16];   //J/byte/m^4
	D_THRESHOLD = params[17];   //metre

	MAX_SIM_TIME = params[18];    //second
	RECORD_PERIOD = params[19];    //second

	/*if( max( mxGetM(prhs[1]), mxGetN(prhs[1]) ) != NODE_NUM || min( mxGetM(prhs[1]), mxGetN(prhs[1]) ) != 1 ||
		max( mxGetM(prhs[2]), mxGetN(prhs[2]) ) != NODE_NUM || min( mxGetM(prhs[2]), mxGetN(prhs[2]) ) != 1 ){
		mexErrMsgTxt("nodes x matrix and y matrix should be 1*NODE_NUM or NODE_NUM*1.");
		return;
	}*/

	nodes_x = mxGetPr(prhs[1]);
	nodes_y = mxGetPr(prhs[2]);
	
	//params delivered
	BaseNode::CLUSTER_RADIUS = CLUSTER_RADIUS;
	BaseNode::MAX_RADIUS = MAX_RADIUS;
	BaseNode::SENSE_DATA_PERIOD = SENSE_DATA_PERIOD;
	
	BaseNode::DATA_PACKET_SIZE = DATA_PACKET_SIZE;
	BaseNode::CTRL_PACKET_SIZE = CTRL_PACKET_SIZE;
	BaseNode::DATA_CTRL_PACKET_SIZE = DATA_CTRL_PACKET_SIZE;
	
	EnergyModel::E_INIT = E_INIT;
	EnergyModel::E_ELEC = E_ELEC;
	EnergyModel::E_FUSION = E_FUSION;
	EnergyModel::E_AMP_FREESPACE = E_AMP_FREESPACE;
	EnergyModel::E_AMP_MULTIPATH = E_AMP_MULTIPATH;
	EnergyModel::D_THRESHOLD = D_THRESHOLD;
	
	BaseNetwork::NODE_NUM = NODE_NUM;
	SinkNode::SINK_X = SINK_X;
	SinkNode::SINK_Y = SINK_Y;
	SinkNode::SINK_ADDR = SINK_ADDR;
	BaseNetwork::MAX_SIM_TIME = MAX_SIM_TIME;
	
	Monitor::RECORD_PERIOD = RECORD_PERIOD;
	
	printf("params passed\n");
	
	//call crunsim
	crunsim(nodes_x, nodes_y);


}



#endif