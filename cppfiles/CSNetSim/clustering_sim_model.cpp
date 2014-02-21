#include "clustering_sim_model.h"

namespace ClusteringSimModel
{
	double AREA_SIZE_X = 50;
	double AREA_SIZE_Y = 50;
	double NODE_NUM = 10;
	double SINK_X = 0;  //metre
	double SINK_Y = 0;
	int SINK_ADDR = 0;
	double CLUSTER_RADIUS = 15;   //metre
	double MAX_RADIUS = 33;    //metre
	double SENSE_DATA_PERIOD = 1;   //unit time
	double DATA_PACKET_SIZE = 100;    //byte
	double CTRL_PACKET_SIZE = 25;   //byte
	double DATA_CTRL_PACKET_SIZE = 100;  //byte
	double MAX_SIM_TIME = 2;    //second
	double RECORD_PERIOD = 1;   //second
	double DEFAULT_TICK = 1;    //second
	double E_INIT = 2000;  //J
};