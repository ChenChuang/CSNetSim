#ifndef CLUSTERINGSIMMODEL_H
#define CLUSTERINGSIMMODEL_H

#include "csnetsim.h"

namespace ClusteringSimModel
{
	extern double AREA_SIZE_X;
	extern double AREA_SIZE_Y;
	extern double NODE_NUM;
	extern double E_INIT;  //J
	
	extern double SINK_X;  //metre
	extern double SINK_Y;
	extern int SINK_ADDR;
	
	extern double CLUSTER_RADIUS;   //metre
	extern double MAX_RADIUS;    //metre
	
	extern double SENSE_DATA_PERIOD;   //unit time
	extern double SENSE_DATA_WAIT;
	
	extern double DATA_PACKET_SIZE;    //byte
	extern double CTRL_PACKET_SIZE;   //byte
	extern double DATA_CTRL_PACKET_SIZE;  //byte
	
	extern double MAX_SIM_TIME;    //second
	extern double RECORD_PERIOD;   //second	
	extern double DEFAULT_TICK;   //second
};

#endif // CLUSTERINGSIMMODEL_H
