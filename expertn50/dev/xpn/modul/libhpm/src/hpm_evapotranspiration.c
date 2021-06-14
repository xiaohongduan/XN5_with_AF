#include "hpm_evapotranspiration.h"
#include "hpm_water.h"
#include "hpm_photosynthesis.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "hpm_math.h"



int hpm_actual_evapotranspiration(hpm *self)
{
	expertn_modul_base *xpn = &(self->parent);
	//double rNetR_PAR = self->parameter.photosynthesis.rNetR_PAR;//1.4; // [j (net radiation) (J (Paar)^-1]Ratio of daily net radiation to PAR.
	double crleaf = self->parameter.photosynthesis.crleaf;//0.15;  // Dimensionless. Leaf reflectivity coefficient. This is a plant parameter used only in the water submodel.
	double XIleaf = self->parameter.photosynthesis.XIleaf;//0.1;   // leaf transmissionskoefficient
	double kcan = self->parameter.photosynthesis.kcan;//0.5; //[m2 leaf (m2 ground)-1]-1. Canopy extinction	coefficient
	double jNetR,jNetRabs_can;
	double LAI,fPARabs_can;
	//double mid_of_day,Lm,dusk_time,dawn_time;

//	tim = xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay;

	LAI = hpm_math_sum4(self->Plant.LAI4);

	self->Environment.DayLength = calculateDayLength((double) xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->fTimeY) * 24.0;
	self->Control.light_on = hpm_get_light_on(-xpn->pLo->pFarm->fLongitude,self->Control.Lz,self->Environment.DayLength,xpn->pTi->pSimTime->fTimeDay);
	

	C_DEBUG(self->Control.light_on);

	//self->Control.light_on = hpm_math_cpulse(xpn->pTi->pSimTime->fTimeDay, 0.5 * (1.0 - self->Environment.DayLength/24.0) - 0.000001,1.0,self->Environment.DayLength/24.0);

	// 7.5b [J m^-2 s-1] Instantaneous net radiation
	// ein teil der Strahlung ist der
	// jparsc ist etwa 50 % von fSolRad [J (PAR) m^-2 day-1] photosynthetic activ radiation
	//jparsc = xpn->pCl->pWeather->fPAR * 1.0e6;
	
	// Das hier ist Schmarrn und Thornleys Fehler:
	/*jparsc = xpn->pCl->pWeather->fPAR * 1.0e6 / 86400.0;
	
	//CalculateActiveRadiation(self,(double)tim,self->Environment.DayLength, xpn->pCl->pWeather->fSolRad * 1.0e6);
	jNetR = rNetR_PAR * jparsc;
	 */
	 // es gilt (wobei jNETR nicht dir net radiation ist):
	 jNetR = xpn->pCl->pWeather->fSolRad * 1.0e6 / 86400.0;
	 

	// 6.6a fraction of incident net radiation absorbed by the canopy
	// the same as the fraction as fraktion absorbed as ther fraction of PAR absorbed:
	fPARabs_can =  (1.0 - crleaf - XIleaf) / (1.0 - XIleaf) * (1.0 - exp(-kcan * LAI));

	// 6.6b [J m^-2 day -1] net Radiation flux absorbed by canopy
	jNetRabs_can = fPARabs_can * jNetR * 86400.0;

	C_DEBUG(jNetRabs_can);
	C_DEBUG(LAI);
//	C_DEBUG(jparsc);

	return RET_SUCCESS;
}

int hpm_actual_evaporation(hpm *self)
{

	//xpn->pWa->pEvap->fPot = xpn->pWa->pEvap->fPotR*dt;
	return RET_SUCCESS;
}
