#ifndef ENERGYMODEL_H
#define ENERGYMODEL_H

#include <math.h>

/**
 * Energy model used in WSN simulation
 * */
class EnergyModel
{
public:
	EnergyModel();
	~EnergyModel();
	
public:
	//energy consumed in transmitting msg, k is the size of msg, d is the transmission distance
	static double calTransmit(int k, double d);
	//energy consumed in receiving msg size of k
	static double calReceive(int k);
	//energy consumed in fusing data size of k
	static double calFusion(int k, int s);
	
public:
	//init energy of node's battery
	static double E_INIT;  //J
	//params of energy model
	static double E_ELEC;    //J/byte
	static double E_FUSION;    //J/byte/source
	static double E_AMP_FREESPACE;    //J/byte/m^2
	static double E_AMP_MULTIPATH;   //J/byte/m^4
	static double D_THRESHOLD;   //metre

};

#endif // ENERGYMODEL_H
