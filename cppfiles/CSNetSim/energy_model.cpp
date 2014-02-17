#include "energy_model.h"

double EnergyModel::E_INIT;  //J
double EnergyModel::E_ELEC;    //J/byte
double EnergyModel::E_FUSION;    //J/byte/source
double EnergyModel::E_AMP_FREESPACE;    //J/byte/m^2
double EnergyModel::E_AMP_MULTIPATH;   //J/byte/m^4
double EnergyModel::D_THRESHOLD;   //metre

EnergyModel::EnergyModel()
{
}

EnergyModel::~EnergyModel()
{
}

double EnergyModel::calFusion(int k)
{
	return EnergyModel::E_FUSION * k;
}

double EnergyModel::calReceive(int k)
{
	return EnergyModel::E_ELEC * k;
}

double EnergyModel::calTransmit(int k, double d)
{
	if(d <= EnergyModel::D_THRESHOLD){
		return (EnergyModel::E_ELEC + EnergyModel::E_AMP_FREESPACE * pow(d, 2)) * k;
	}else{
		return (EnergyModel::E_ELEC + EnergyModel::E_AMP_MULTIPATH * pow(d, 4)) * k;
	}
}
