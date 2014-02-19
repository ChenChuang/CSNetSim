#include "energy_model.h"

namespace EnergyModel
{
	double E_ELEC;    //J/byte
	double E_FUSION;    //J/byte/source
	double E_AMP_FREESPACE;    //J/byte/m^2
	double E_AMP_MULTIPATH;   //J/byte/m^4
	double D_THRESHOLD;   //metre
	
	double calTransmit(double k, double d)
	{
		if(d <= D_THRESHOLD){
			return (E_ELEC + E_AMP_FREESPACE * pow(d, 2)) * k;
		}else{
			return (E_ELEC + E_AMP_MULTIPATH * pow(d, 4)) * k;
		}
	}

	double calReceive(double k) {return E_ELEC * k;}

	double calFusion(double k) {return E_FUSION * k;}
}
