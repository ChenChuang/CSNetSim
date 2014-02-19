#ifndef ENERGYMODEL_H
#define ENERGYMODEL_H

#include <math.h>

class EnergyModel
{
public:
	//params of energy model
	static double E_ELEC;    //J/byte
	static double E_FUSION;    //J/byte/source
	static double E_AMP_FREESPACE;    //J/byte/m^2
	static double E_AMP_MULTIPATH;   //J/byte/m^4
	static double D_THRESHOLD;   //metre
	
public:
	//energy consumed in transmitting msg, k is the size of msg, d is the transmission distance
	static double calTransmit(double k, double d)
	{
		if(d <= EnergyModel::D_THRESHOLD){
			return (EnergyModel::E_ELEC + EnergyModel::E_AMP_FREESPACE * pow(d, 2)) * k;
		}else{
			return (EnergyModel::E_ELEC + EnergyModel::E_AMP_MULTIPATH * pow(d, 4)) * k;
		}
	}
	//energy consumed in receiving msg size of k
	static double calReceive(double k) {return EnergyModel::E_ELEC * k;}
	//energy consumed in fusing data size of k
	static double calFusion(double k) {return EnergyModel::E_FUSION * k;}
};

#endif // ENERGYMODEL_H
