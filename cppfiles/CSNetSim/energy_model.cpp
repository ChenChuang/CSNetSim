#include "energy_model.h"

namespace EnergyModel
{
	double E_ELEC = 400.0e-6;    //J/byte
	double E_FUSION = 40.0e-6;    //J/byte/source
	double E_AMP_FREESPACE = 80.0e-9;    //J/byte/m^2
	double E_AMP_MULTIPATH = 0.0104e-9;   //J/byte/m^4
	double D_THRESHOLD = 75;   //metre
	
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
