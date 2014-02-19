#ifndef CLUSTERINGSIMMODEL_H
#define CLUSTERINGSIMMODEL_H

class ClusteringSimModel
{
public:
	static double AREA_SIZE_X;
	static double AREA_SIZE_Y;
	static double NODE_NUM;
	static double SINK_X;  //metre
	static double SINK_Y;
	static int SINK_ADDR;
	static double CLUSTER_RADIUS;   //metre
	static double MAX_RADIUS;    //metre
	static double SENSE_DATA_PERIOD;   //unit time
	static double DATA_PACKET_SIZE;    //byte
	static double CTRL_PACKET_SIZE;   //byte
	static double DATA_CTRL_PACKET_SIZE;  //byte
	static double MAX_SIM_TIME;    //second
	static double RECORD_PERIOD;   //second	
	static double E_INIT;  //J
};

#endif // CLUSTERINGSIMMODEL_H
