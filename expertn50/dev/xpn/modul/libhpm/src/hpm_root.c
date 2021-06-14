#include "hpm_root.h"
#include "hpm_math.h"
#include "hpm_growth.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int hpm_root_length_growth(hpm *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double fTrt; // Gewichtungsfunktion für Temperatureinfluss
	double fWrt; // Water
	double Crt; // Substrate C konzentration
	double Nrt; // Substrate N konzentraion
	double rort_new; // Neue root density
	double Drort; // gesamt depth growth rate [m / d]
	double dt; // delta time  [in days]
	double Grt;
	//static double rort = 0.809533;
	struct_Soil_Vars soil_vars;


	dt = (double) xpn->pTi->pTimeStep->fAct;
	get_SoilVars(self,&soil_vars);

	// Calculation of root depth depthrt [m] 3.9c:
	// 1.0e-4; 1 ha = 1e4 m^2
//	pPl->pRoot->fDepth = (float)max(depthrtmin, ( (double)pPl->pBiomass->fRootWeight *1.0e-4 / *p->rort )); // Wurzel muss mindestens 2 cm tief sein == Masse / Dichte

// wurzelwachstum wird wie unter 5.3.2.5 (SPASS): dabei wird die tiefste Schicht herangezogen
// Die growthrate wird berechnet wie Formel 3.3c hpm (modified)


// Temperaturabhängigkeit: 3.11b
    //fTrt =1.0; //Test of Hong
	fTrt = calculatePlantTemperatureProcesses_getf_HPM(self,soil_vars.TSoil);

// Effect of Water: (3.11d / 6.7b)
    //fWrt =1.0; //Test of Hong
	fWrt = pow(self->Water.aWrt,self->parameter.plant.qWpl);
    //fWrt= MAX(fWrt, 1e-6);//Test of Hong
	
// Substrate C konzentration 3.3b
	CHECK_0(self->Plant.MXrt)
	Crt = self->Plant.MCSrt / self->Plant.MXrt;

// Substrate N konz.: 3.3b
	CHECK_0(self->Plant.MXrt)
	Nrt = self->Plant.MNSrt / self->Plant.MXrt;

	Grt = self->parameter.plant.mu20 * fTrt * fWrt * Crt * Nrt * self->Plant.MXrt; //3.3c

	rort_new = self->parameter.plant.rortmax - (self->parameter.plant.rortmax - self->parameter.plant.rortmin) * ( 1.0 - soil_vars.theta_soil / soil_vars.theta_soil_max );

	Drort = ( Grt * (rort_new - self->Plant.rort) / self->Plant.MXrt );

	self->Plant.rort += Drort*dt;

	xpn->pPl->pRoot->fDepth = MAX(0.02, (self->Plant.MXrt/(self->Plant.rort*1.0e-3)));

	//Added by Hong on 20180806
	xpn->pPl->pRoot->fDepth =MIN(xpn->pPl->pRoot->fDepth, 120);
	
	return self->__ERROR;	
}
