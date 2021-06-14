#include "hpm_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int hpm_calc_albedo(hpm *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	double LAI,transm,theta;
	double crleaf = self->parameter.photosynthesis.crleaf;//0.15;  // Dimensionless. Leaf reflectivity coefficient. This is a plant parameter used only in the water submodel.
	double XIleaf = self->parameter.photosynthesis.XIleaf;//0.1;   // leaf transmissionskoefficient
	double kcan = self->parameter.photosynthesis.kcan;//0.5; //[m2 leaf (m2 ground)-1]-1. Canopy extinction	coefficient
	
	LAI = hpm_math_sum4(self->Plant.LAI4);
	
	//fPARabs_can ist der Teil, der von den Blätern absorbiert wird
	//fPARabs_can =  (1.0 - crleaf - XIleaf) / (1.0 - XIleaf) * (1.0 - exp(-kcan * LAI));
	///fPARabs_can =  (1.0 - crleaf - XIleaf) * (1.0 - exp(-kcan * LAI));
	// Wenn man davon ausgeht, dass vom Boden so gut wie nichts reflektiert wird:
	
	
	transm  = exp(-kcan * LAI) + XIleaf * (1.0 - exp(-kcan * LAI));
	xpn->pPl->pPltClimate->fPlantAlbedo = crleaf;
	
	theta = xpn->pWa->pWLayer->pNext->fContAct;
	if (theta <= 0.10)
		{
			xpn->pSo->fSoilAlbedo =  0.25;
		} 
	else if ((theta > 0.10) && (theta <= 0.25))
		{
			xpn->pSo->fSoilAlbedo =  0.35 - theta;
		}
	else // theta > 0.25
		{
			xpn->pSo->fSoilAlbedo =  0.10;
		}
	
	xpn->pSo->fAlbedo = xpn->pSo->fSoilAlbedo*transm + (1.0-transm)*xpn->pPl->pPltClimate->fPlantAlbedo;
	//xpn->pSo->fAlbedo = xpn->pPl->pPltClimate->fPlantAlbedo - (xpn->pPl->pPltClimate->fPlantAlbedo - xpn->pSo->fSoilAlbedo) * exp(-kcan*xpn->pPl->pCanopy->fLAI);
	
	
	
	
	
	
	
	
/*	C_DEBUG((1.0 - crleaf - XIleaf) / (1.0) * (1.0 - exp(-kcan * LAI)));
	C_DEBUG((crleaf) / (1.0) * (1.0 - exp(-kcan * LAI)));
	C_DEBUG((XIleaf) / (1.0) * (1.0 - exp(-kcan * LAI)));*/
	
	
	
	return RET_SUCCESS;
}
