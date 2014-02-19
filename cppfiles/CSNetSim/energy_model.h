#ifndef ENERGYMODEL_H
#define ENERGYMODEL_H

#include <math.h>

namespace EnergyModel
{
	//params of energy model
	extern double E_ELEC;    //J/byte
	extern double E_FUSION;    //J/byte/source
	extern double E_AMP_FREESPACE;    //J/byte/m^2
	extern double E_AMP_MULTIPATH;   //J/byte/m^4
	extern double D_THRESHOLD;   //metre
	
	//energy consumed in transmitting msg, k is the size of msg, d is the transmission distance
	extern double calTransmit(double k, double d);
	//energy consumed in receiving msg size of k
	extern double calReceive(double k);
	//energy consumed in fusing data size of k
	extern double calFusion(double k);
}

#endif // ENERGYMODEL_H
