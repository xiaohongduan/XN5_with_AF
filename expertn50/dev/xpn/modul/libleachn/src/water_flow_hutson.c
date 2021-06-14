//      water_flow_hutson.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "water_flow_hutson.h"
#include "stickstoff_util.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(water_flow_hutson, water_flow_hutson, EXPERTN_MODUL_BASE_TYPE);


static void water_flow_hutson_class_init(water_flow_hutsonClass *klass) {}


static void water_flow_hutson_init(water_flow_hutson *self)
{
	self->WCont=NULL;
	self->HCond=NULL;
	self->DWCap=NULL;
	self->MPotl=NULL;
}


/* This switch turns on a recalculation of hydraulic conductivites
   during the iteration solving Richards equation (not in LEACHM) */
#undef  NEW_ITERATION
/* This switch causes recalculation of soil water capacities only
   every 0.1 day */
#undef CHECK_CAPACITY_ONLY_NEWTENTHDAY

/* this switch causes the water transport function to keep matric potential
	in physically reasonable limits */
#undef CHECK_MATRIC_POTENTIAL

/* this switch causes the pde-solver to keep hydraulic conductivity bigger than 0 */
#undef KEEP_CONDUCTIVITY_POSITIVE

// Our Modul Functions:


int water_flow_hutson_load(water_flow_hutson *self)
{
	expertn_modul_base *xpn = &(self->parent);
	self->fLeachPonding = 0.0;
	self->fLostPondCum = 0.0;
	self->precip = 0.0;
	self->fPond_Day = xpn->pTi->pSimTime->mday;
	self->fPond_Mon = xpn->pTi->pSimTime->mon;
	self->fPond_Year = xpn->pTi->pSimTime->year;

	xpn_time_date_add_dt(&(self->fPond_Year),&(self->fPond_Mon),&(self->fPond_Day), 1);



	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL)) {
		PRINT_ERROR_ID(xpn,"Problem with hydraulic functions!");
	}

	// Boundary Condition aus der INI Datei leisen (falls undefiniert --> 1)
	xpn->pWa->iBotBC = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.leachn.bottombc",1);
	if (xpn->pWa->iBotBC==1) {
		xpn->pWa->FreeD=1;
	} else {
		xpn->pWa->FreeD=0;
	}
	// Mobil Immobil (Standartmäßig aus = 0)
	xpn->pWa->iMobImm = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.leachn.mobil",0);

	self->F1 = g_malloc0(sizeof(double)*xpn->pSo->iLayers);
	self->G1 = g_malloc0(sizeof(double)*xpn->pSo->iLayers);
	self->vorPot = g_malloc0(sizeof(double)*xpn->pSo->iLayers);

	return RET_SUCCESS;
}

int water_flow_hutson_done(water_flow_hutson *self)
{
	g_free(self->F1);
	g_free(self->G1);
	g_free(self->vorPot);
	return RET_SUCCESS;
}



/********************************************************************************************/
/*                                                                                          */
/*        Name:      Water1()                                                               */
/*                                                                                          */
/*        Function:  Calculates soil water flow                                             */
/*                                                                                          */
/*        Method:    Follows the methods of the model LeachM V 3.1 given in watflo.for      */
/*                   By Hutson & Wagenet (1992)                                             */
/*                                                                                          */
/*        Authors:   ch, 04.10.95                                                           */
/*                                                                                          */
/********************************************************************************************/
/*  changed variables:      pCL->pWeather->fRainBegin       	pWL->fHydrCond              */
/*							pWa->fRunOffCum                     pWL->fFluxDens              */
/*							pWa->fInfiltR                       pWL->fFlux                  */
/*							pWa->fPondWater                     pWL->fFluxDensOld           */
/*							pWa->fRunOffR                       pWL->fMaxEvapFD             */
/*							pWa->fPercolR                       pSW->fPotInflec             */
/*							pWa->pEvap->fActR                   pSW->fContSat               */
/*							pWL->fContAct                       pSW->fContInflec            */
/*							pWL->fContOld                       pWB->fProfileIce            */
/*							pWL->fMatPotAct                     pWB->fProfilStart           */
/*                         	pWL->fMatPotOld                     pWB->fProfil                */
/********************************************************************************************/

int water_flow_hutson_run(water_flow_hutson *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCLIMATE pCl = xpn->pCl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;

	int iFlagSolver=0; // Parameter

	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;
//    double SimTime    = pTi->pSimTime->fTimeAct;


	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;


	PEVAP     pEV = pWa->pEvap;



	double f1, f2;

	for (pWL = pWa->pWLayer; pWL!=NULL; pWL=pWL->pNext) {
		pWL->fMatPotOld = pWL->fMatPotAct;
	}



	pSL = pSo->pSLayer,
	pSW = pSo->pSWater,
	pWL = pWa->pWLayer,
	iLayer = 0;

	
	/*if (NewDay(pTi))
	{	
		self->precip = pCl->pWeather->fLiquPreciRate;
	} else
	{
		self->precip = 0.0;
	}*/
	
	self->precip = pCl->pWeather->fLiquPreciRate * DeltaT;

// ***************************************************************
// Ponding
// ***************************************************************

	//if ( //(NewDay(pTi)) &&
	if //NewDay(xpn->pTi) &&

	((xpn_time_compare_date(xpn->pTi->pSimTime->year,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,self->fPond_Year,self->fPond_Mon,self->fPond_Day)>=0)
	  && (self->fLeachPonding > EPSILON)) {
		self->fLostPondCum     += self->fLeachPonding;                       // [mm]

		// Ponding as RunOff
		//pWa->fRunOffDay  += self->fLeachPonding;                   // [mm]
		//pWa->fRunOffCum  += self->fLeachPonding;                   // [mm]
		pWa->fRunOffR     =self->fLeachPonding / DeltaT;          // [mm/day]

		self->fLeachPonding = (double)0;
		/* if ponding changes upper boundary set flag here: fInfilt = CONST_RAIN_RATE; */
	}


// ***************************************************************
// 3.  Rain event:
// ***************************************************************

	//if (pWB->fReservoir > EPSILON)
	if (self->precip > EPSILON) {
		
		   // Delete Ponding if rain event:
		   if (self->fLeachPonding > EPSILON)
		     {
		      self->precip -= self->fLeachPonding;

		      if (self->precip < (double)0)
		      {
		      self->precip = (double)0;
		      }

		      self->fLostPondCum    += self->fLeachPonding;

		      // Ponding as RunOff
		      pWa->fRunOffDay += self->fLeachPonding;
		      pWa->fRunOffCum += self->fLeachPonding;
			  pWa->fRunOffR     =self->fLeachPonding / DeltaT;          // [mm/day]
			  

		    self->fLeachPonding  = (double)0;

		     }
		

//  sets time limit for end of infiltration
// ***************************************************************

		xpn_time_date_add_dt(&(self->fPond_Year),&(self->fPond_Mon),&(self->fPond_Day), PONDING_TIME);


//  diminish amount of infiltration by runoff
//*****************************************************************************

		/*if (NewDay(pTi))
			{
				// this rate is not multiplied with DeltaT since it will be subtracted only once.
				pWB->fReservoir -= pWa->fRunOffR;

				pWB->fReservoir -=  pWa->fPondWater;
			}*/

		

		//if (NewDay(xpn->pTi)) {
			self->precip -= pWa->fRunOffR;
			self->precip -= pWa->fPondWater;
		//}
//   calculates water uptake capacity at begin of rain event:
// ***************************************************************
		//pWa->fPotInfiltration = pWB->fReservoir;
		pWa->fPotInfiltration = self->precip;

		if (pWa->fPotInfiltration < (double)0) {
			pWa->fPotInfiltration = (double)0;
		}

		f1 = WaterCapacity(self);

		if ( f1 < pWa->fPotInfiltration) {

			f2 = pWa->fPotInfiltration - f1;

			pWa->fPotInfiltration   = f1;    /* Ponding */
			pWa->fInfiltR   = pWa->fPotInfiltration;

			self->fLeachPonding += f2;


			if (f2 >= (double)10) {
				/*itoa((int)f2,lpOut,10);
				strcat(lpOut,COMMENT_SET_PONDING);
				Message(0,lpOut);*/
				char *S;
				S = g_strdup_printf("%f mm Water is Ponding.",f2);
				PRINT_ERROR_ID(xpn,S);
				g_free(S);
			}

#undef PONDING

#ifndef PONDING
			self->fLostPondCum     += self->fLeachPonding*xpn->pTi->pTimeStep->fAct;                       // [mm]

			// Ponding as RunOff
			pWa->fRunOffDay  += self->fLeachPonding*xpn->pTi->pTimeStep->fAct;                   // [mm]
			pWa->fRunOffCum  += self->fLeachPonding*xpn->pTi->pTimeStep->fAct;                   // [mm]
			// pWa->fRunOffR     = fLeachPonding / DeltaT;          // [mm/day]

			if (self->fLeachPonding >= (double)10) {
				/*itoa((int)(fLeachPonding+(double)0.5),lpOut,10);
				strcat(lpOut,COMMENT_PONDING_RUNS_OFF);
				Message(0,lpOut);*/
				char *S;
				S = g_strdup_printf("%f mm Ponding Becomes RunOff.",self->fLeachPonding+(double)0.5);
				PRINT_ERROR_ID(xpn,S);
				g_free(S);
			}

			self->fLeachPonding = (double)0;
#endif

		} else { /* f1 >= pWB->fReservoir */
			pWa->fInfiltR = CONST_RAIN_RATE;   // standard value 100 mm/d


		}

		//pWB->fReservoir = (double)0;
		self->precip=0.0;

	} // 3. Regen

	else

//   recalculates water uptake capacity during infiltration
//   every 0.1 day:
// ***************************************************************

//if (NewTenthDay(pTi))
	{
		/* Adds ponding to infiltration */
		if (self->fLeachPonding > EPSILON) {
			pWa->fPotInfiltration    += self->fLeachPonding;
			self->fLeachPonding  = (double)0;
		}

		/* Infiltration */
		if (pWa->fPotInfiltration > EPSILON) {
			f1 = WaterCapacity(self);   // [mm] calculated once a day

			if ( f1 < pWa->fPotInfiltration) {
				pWa->fInfiltR   = f1;    /* Ponding */   // [mm/day]

				self->fLeachPonding = pWa->fPotInfiltration - f1;

				pWa->fPotInfiltration    = f1;

			} else {
				pWa->fInfiltR = CONST_RAIN_RATE; /* standard value  100 mm/d  */
			}
		}  // Infiltration

	}   // evt. NewTenthday



	/****************************************************************************** */
	/* 4a. Caculates water sink/source due to freezing/thawing:                     */
	/*     Berechne ThetaSat, Theta aktuell                                         */

	Freezing(self);

	/****************************************************************************** */
	/* 4b. Calculates plant water uptake:                                           */
	/*     lpPDZ->fwasser                                                           */

	CalculateSink(self);


//*****************************************************************************
// 5. Calculates new matrix potentials and unsaturated hydraulic conductivities
//*****************************************************************************

	if (iFlagSolver==0) {
		SolveRG(self);
	} else {
		//SolvePD(exp_p);
	}

//*****************************************************************************
// 6. Calculates new water contents and water fluxes
//*****************************************************************************

	pSL = pSo->pSLayer,
	pSW = pSo->pSWater,
	pWL = pWa->pWLayer,
	iLayer = 0;

	// Test !!!
	if ( pWL->fMatPotAct < -1e6) {
#ifdef CHECK_MATRIC_POTENTIAL
		pWL->fMatPotAct = (double)-1e6;
#endif
		// better stop using this solver in such situations
		PRINT_ERROR_ID(xpn,"Matric Potential in Layer 0 very low!");
	}

	// Schicht 0:
	pWL->fFluxDens = pWL->fHydrCond
	                 * ((double) 1 - (pWL->pNext->fMatPotAct
	                                  + pWL->pNext->fMatPotOld
	                                  - pWL->fMatPotAct
	                                  - pWL->fMatPotOld)
	                    / ((double)2.0 * pSo->fDeltaZ)); // [mm/day]

	pWL->fFlux     = pWL->fFluxDens * DeltaT;          // [mm]


	/********************************************************************/
	/*  Diminishing of infiltration water:                              */

	if (pWL->fFlux > (double) 0) {

		if (pWL->fFlux > pWa->fPotInfiltration) {

#ifdef LOGFILE
			if (pWL->fFlux >= pWa->fPotInfiltration + (double)0.1) {
				Message(0,ERROR_INFILTRATION_TOO_HIGH);
			}
#endif

			pWL->fFlux     = pWa->fPotInfiltration;
			pWL->fFluxDens = pWL->fFlux / DeltaT;

			pWa->fPotInfiltration   = (double)0;
		} // Fluss > SickerW
		else
			pWa->fPotInfiltration  -= pWL->fFlux;

	} /* Flux > 0 */


	for (SOIL_LAYERS0(pWL,pWa->pWLayer)) {
		pWL->fFluxDensOld = pWL->fFluxDens;
	}

	for ( pSL = pSo->pSLayer->pNext,
	      pSW = pSo->pSWater->pNext,
	      pWL = pWa->pWLayer->pNext,
	      iLayer = 1;
	      ((pSL->pNext != NULL)&&
	       (pSW->pNext != NULL)&&
	       (pWL->pNext != NULL));
	      pSL = pSL->pNext,
	      pSW = pSW->pNext,
	      pWL = pWL->pNext,
	      iLayer++
	    ) { //  Schicht i = 1 bis  n-1

		if ( pWL->fMatPotAct < -1e6) {

#ifdef CHECK_MATRIC_POTENTIAL
			pWL->fMatPotAct = (double)-1e6;
#endif

			// better stop using this solver in such situations
			PRINT_ERROR_ID(xpn,"Matric Potential very low!");

		}

		if ( pWL->fMatPotAct > pSW->fMinPot) {

#ifdef CHECK_MATRIC_POTENTIAL
			pWL->fMatPotAct = pSW->fMinPot;
#endif

			// better stop using this solver in such situations
			PRINT_ERROR_ID(xpn,"Matric Potential too high in soil!");

		}

		/*     Water contents of soil profile */
		pWL->fContOld = WATER_CONTENT(pWL->fMatPotOld);
		pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);

		/*     unsaturated hydraulic conductivities */
		pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
		                 ? ((CONDUCTIVITY(pWL->fMatPotOld) + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld))
		                    / (double)2.0)
		                 : CONDUCTIVITY(pWL->fMatPotOld);                     // [mm/day]



		/*    Water flux densities within profile */
		pWL->fFluxDens = pWL->fHydrCond
		                 * ((double) 1 - (pWL->pNext->fMatPotAct
		                                  + pWL->pNext->fMatPotOld
		                                  - pWL->fMatPotAct
		                                  - pWL->fMatPotOld)
		                    / ((double)2.0 * pSo->fDeltaZ));

		// Water flow per layer
		pWL->fFlux = pWL->fFluxDens * DeltaT;

	} // for all soil layers

	// This is here, because system does not update it anymore!

	pWL->fMatPotOld = pWL->fMatPotAct;


	//Water flow from bottom layer = Leaching
	pWa->fPercolR = pWL->pBack->fFluxDens;


	// Water flow into/from top layer means infiltration/ evaporation (depending on sign)
	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;

	iLayer = 0;

	if (pWL->fFluxDens  >= (double) 0) {                       // zero layer
		pWa->fInfiltR     = pWL->fFluxDens;
		pWa->pEvap->fActR = (double)0;
	}

	else {
		pWa->fInfiltR     = (double)0;
		pWa->pEvap->fActR = ((double)-1 * pWL->fFluxDens);
	}


// ****************************************************************************
// 7. Calculates unsaturated hydraulic conductivity of top layer for evaporation
// ****************************************************************************

	pWL->pNext->fHydrCond = CONDUCTIVITY(pWL->pNext->fMatPotAct);   // first layer

	pWL->fHydrCond        = pWL->pNext->fHydrCond;  // zero layer = first layer

	pEV->fMaxR = pWL->fHydrCond *
	             ((double)3000 * (double)102.2 + pWL->pNext->fMatPotAct) / pSo->fDeltaZ;



	return RET_SUCCESS;
}


/*********************************************************************
 *                                                                   *
 *  Name     : WaterCapacity()                                       *
 *  Funktion : Berechnet Wasseraufnahmekapazität in den ersten       *
 *             ungesättigten Schichten.                              *
 *              ch 25.9.95                                           *
 *  nach:       C. Sperr   / 24.08.94                                *
 *                                                                   *
 *********************************************************************/
double WaterCapacity(water_flow_hutson *self)
{

	expertn_modul_base *xpn = &(self->parent);

	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	int iLayer        = 0;




	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;


	double  fThetaMax, fMinLeitf, WasserKap;
	int    SaettSchicht;
	double DeltaZ       = pSo->fDeltaZ;



	fMinLeitf    = pSo->pSWater->fCondSat;
	WasserKap    = (double)0.0;
	SaettSchicht = pSo->iLayers;

	for (pSL = pSo->pSLayer->pNext,
	     pSW = pSo->pSWater->pNext,
	     pWL = pWa->pWLayer->pNext,
	     iLayer = 1;
	     ((pSL->pNext != NULL)&&
	      (pSW->pNext != NULL)&&
	      (pWL->pNext != NULL));
	     pSL = pSL->pNext,
	     pSW = pSW->pNext,
	     pWL = pWL->pNext,
	     iLayer++    ) { // i= 1 bis n-1
		// gibt die minimale gesättigte Leitfähigkeit in einer der Schichten an.
		fMinLeitf = min(fMinLeitf,pSW->fCondSat);

		// Suche nach erster gesättigter Schicht oder nach einer Eis-haltigen Schicht:
		if (SaettSchicht > iLayer) {
			// Addiere Kapazität bis zur ersten ges. Schicht
			fThetaMax = (pWL->fIce < EPSILON) ?
			            WATER_CONTENT((double)0.999 * pSW->fMinPot)
			            :
			            // pSW->fContSat;
			            // Test !!! Keine Infiltration in Eishaltige Schicht
			            pWL->fContAct;

			if (fThetaMax > pWL->fContAct)
				WasserKap += (fThetaMax - pWL->fContAct) * DeltaZ;
			else
				SaettSchicht = iLayer;


			if ((pSW->fCondSat < CONST_RAIN_RATE) ||
			    (pWL->fContAct >= (double)0.999 * pWL->fContMax)||
			    (pWL->fIce > EPSILON)) {
				SaettSchicht = iLayer;
			}

		}   /* Saettschicht > iLayer */

	} // for

	return WasserKap;
} // WaterCapacity()


/*******************************************************************************************
 *
 *  Name     :  Freezing()
 *
 *	Function :  changes ThetaSat
 *				calculates water sink and source term due to freezing or thawing
 *              assuming no plant water uptake
 *
 *              ch 8.1.97
 *
 ******************************************************************************************
 */
int Freezing(water_flow_hutson *self)
{

	expertn_modul_base *xpn = &(self->parent);
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
//	double fCont[100];





	int iLayer        = 0;




	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;



	for (pSL = pSo->pSLayer->pNext,
	     pSW = pSo->pSWater->pNext,
	     pWL = pWa->pWLayer->pNext,
	     iLayer = 1;
	     ((pSL->pNext != NULL)&&
	      (pSW->pNext != NULL)&&
	      (pWL->pNext != NULL));
	     pSL = pSL->pNext,
	     pSW = pSW->pNext,
	     pWL = pWL->pNext,
	     iLayer++   ) {
		pWL->fIceOld = pWL->fIce;

		/* fIce ist vol. Eisgehalt */
		pWL->fIce += (double)1.1 * pWL->fWaterFreezeR * pTi->pTimeStep->fOld;

		if (!Test_Range(xpn,pWL->fIce, (double)0, (double)1, "Ice content")) {
			pWL->fIce = (double)0;
		}

		if (!Test_Range(xpn,pWL->fIce,  (double)0, (pSL->fPorosity - THETA_MIN),"Ice content")) {
			pWL->fIce = pSL->fPorosity - THETA_MIN;
		}

		pSW->fContSat = pSL->fPorosity - pWL->fIce;


		if ((pWL->fIce > pWL->fIceOld + EPSILON)||(pWL->fIce < pWL->fIceOld - EPSILON)) {
			pSW->fContInflec = (double)2* pSW->fCampB * pSW->fContSat / ((double)1 + (double)2* pSW->fCampB);

			/* overwrites all other water sinkterms */
			// pWL->fContOld    = WATER_CONTENT(pWL->fMatPotAct);

			/* To calculate correct waterbalance the actual soil water content is recalculated
			considering the change in ice content */
			pWL->fContAct   -= (pWL->fIce - pWL->fIceOld) / (double)1.1;

			if (!Test_Range(xpn,pWL->fContAct, THETA_MIN, (double)1, "Water content")) {
				pWL->fContAct = THETA_MIN;
			}

			if (!Test_Range(xpn,pWL->fContAct, THETA_MIN, pSW->fContSat, "Water content")) {
				pWL->fContAct = pSW->fContSat;
			}

		}
		//fCont[iLayer] = pWL->fContAct;
	} /* for */



	return RET_SUCCESS;
}


/*******************************************************************************************
 *
 *  Name     :  CalculateSink()
 *
 *  Function :  Calculates water sink due to water uptake by plants and soil water freezing
 *				from change in soil water contents since last calculation of soil water flow
 *              assuming freezing and plant water uptake do not occur at the same time
 *
 *              ch 7.1.97
 *
 *******************************************************************************************/
int CalculateSink(water_flow_hutson *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PHEAT pHe = xpn->pHe ;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PWLAYER     pWL;
	PLAYERROOT  pLR;

	PSLAYER     pSL;
	PSWATER     pSW;


	int iLayer;

	//H2O_ZERO_LAYER

	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
	iLayer = 0;

	if (pPl == NULL) return -1;
	if (pPl->pRoot == NULL) return -1;
	if (pTi->pTimeStep->fAct < EPSILON) return -1;
	if (pHe->fFreezingDepth > EPSILON) return -1;


	pPl->pRoot->fUptakeR = (double)0;

	//for (SOIL2_LAYERS1(pWL,pWa->pWLayer->pNext,pLR,pPl->pRoot->pLayerRoot->pNext))
	for( pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,pWL = pWa->pWLayer->pNext,
	     pLR = pPl->pRoot->pLayerRoot->pNext,iLayer = 1;
	     ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL)&&(pLR->pNext != NULL));
	     pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,pLR = pLR->pNext,iLayer++) {
		/* this is the sink term in Richards Equation: */
		//pLR->fActLayWatUpt=(pWL->fContOld-pWL->fContAct)/pTi->pTimeStep->fAct; /* [1/day] */
		pLR->fActLayWatUpt=(WATER_CONTENT(pWL->fMatPotAct)-pWL->fContAct)/pTi->pTimeStep->fAct; /* [1/day] */
		/* It is assumed that H2O uptake only takes place, when soil is not freezing or thawing */
		if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON)) {
			pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * pSo->fDeltaZ;  /* [mm/day] */
		}
	} /* for */


	return RET_SUCCESS;
}

/******************************************************************************/
/*        Name:     SolveRG()                                                 */
/*                                                                            */
/*        Function: Calculates matric potentials and hydraulic conductivities */
/*                  by numerically solving Richards equation                  */
/*                                                                            */
/*        Authors:  ch 26.09.95, ep 15.04.94, cs 13.11.92, as 28.11.96        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
/*   veränd. Var.:		pWL->fContAct                                         */
/*						pWL->fDiffCap                                         */
/*						pWL->fHydrCond                                        */
/*						pWL->fMatPotAct                                       */
/*						pWL->pBack->fMatPotAct                                */
/******************************************************************************/
int SolveRG(water_flow_hutson *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;



	double   dR33, dR35;
	double   fMPAct,fMPOld,diff;
	int      iterat;
	int      iR9;

//  int OK            = 1;
	int iLayer        = 0;
//        char  acdummy[80];
	double DeltaT      = pTi->pTimeStep->fAct;
	//double SimTime     = pTi->pSimTime->fTimeAct;

	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;



	PLAYERROOT  pLR;
	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;

	iLayer = 0;

//******************************************************************************
// 1. Set upper boundary condition (F1(0), G1(0))
// *****************************************************************************

	self->G1[0] = UpperBoundary(self);

#ifdef EVATRA_OUTPUT
	Deb_EvaTra(exp_p, (double)G1[0]);
#endif

	self->F1[0] = (double)1;

	/* if a different upper boundary for ponding is considered chose
	(fInfilt > (double)999)?
	              (double)0 : (double)1;  */

// *****************************************************************************
// 2. Store old matric potentials, recalculate hydr. conductivities
// *****************************************************************************

	for (pSL = pSo->pSLayer,
	     pSW = pSo->pSWater,
	     pWL = pWa->pWLayer,
	     iLayer = 0; ((pSL != NULL)&&
	                  (pSW != NULL)&&
	                  (pWL != NULL));
	     pSL = pSL->pNext,
	     pSW = pSW->pNext,
	     pWL = pWL->pNext,
	     iLayer++      ) { // Layers 0 to n
		/* originally in ExpertN or LEACHM: within iteration loop, but MatPotOld does not change. */
		pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
		                 ? ((CONDUCTIVITY(pWL->fMatPotOld) + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld))
		                    / (double)2.0)
		                 : CONDUCTIVITY(pWL->fMatPotOld);                     // [mm/day]

		self->vorPot[iLayer] = pWL->fMatPotOld;                                   // [mm]

		// obsolete: F1[iLayer]   = G1[iLayer] = (double)0;

#ifdef KEEP_CONDUCTIVITY_POSITIVE
		if ( pWL->fHydrCond < EPSILON) {
			// better stop using this solver in such situations !!!
			Message(0,"Hydraulic Conductivity near 0!");

			pWL->fHydrCond = EPSILON;

		}
#endif

	}

//******************************************************************************
// 3. Solver of PDE
// *****************************************************************************

	for (diff = (double)9999.9,iterat=1; ((iterat <= 20)&&(diff >= (double)2.0*DeltaT)); iterat++) {


		/******************************************************************************
		 *           Hydraulic conductivities of soil profile
		 *           Differential water capacities of profile
		 */
		for (pSL = pSo->pSLayer,
		     pSW = pSo->pSWater,
		     pWL = pWa->pWLayer,
		     iLayer = 0; ((pSL != NULL)&&
		                  (pSW != NULL)&&
		                  (pWL != NULL));
		     pSL = pSL->pNext,
		     pSW = pSW->pNext,
		     pWL = pWL->pNext,
		     iLayer++      ) { // Layer 0 to n
#ifdef NEW_ITERATION
			pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
			                 ? ((CONDUCTIVITY(pWL->fMatPotAct) + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotAct))
			                    / (double)2.0)
			                 : CONDUCTIVITY(pWL->fMatPotAct);            // [mm/day]
#endif

			fMPAct = min(pWL->fMatPotAct,pSW->fMinPot);                // [mm]
			fMPOld = min(pWL->fMatPotOld,pSW->fMinPot);                // [mm]


			pWL->fDiffCap =((double)CAPACITY(fMPAct) + (double)CAPACITY(fMPOld))/(double)2;

		} // for  hydr. cond., diff. water capacities

		/*******************************************************************************
		 *                           Matrix - Coefficients
		 *   Layer 1  to   n-1
		 */
		if (pPl != NULL) {
			pLR = pPl->pRoot->pLayerRoot->pNext;
		}

		for (pSL = pSo->pSLayer->pNext,
		     pSW = pSo->pSWater->pNext,
		     pWL = pWa->pWLayer->pNext,
		     iLayer = 1; ((pSL->pNext != NULL)&&
		                  (pSW->pNext != NULL)&&
		                  (pWL->pNext != NULL));
		     pSL = pSL->pNext,
		     pSW = pSW->pNext,
		     pWL = pWL->pNext,
		     iLayer++    ) {
			dR33 = ((double)(2.0*pSo->fDeltaZ*pSo->fDeltaZ* pWL->fDiffCap)            // [mm/day]
			        / (double)DeltaT)
			       + (double)pWL->fHydrCond + (double)pWL->pBack->fHydrCond;

			dR35 =  (double)pWL->fHydrCond * (double)(pWL->pNext->fMatPotOld);        // [mm*mm/day]
			dR35 += (double)pWL->pBack->fHydrCond * (double)(pWL->pBack->fMatPotOld);
			dR35 += (dR33 - (double)2.0*(double)pWL->fHydrCond - (double)2.0*(double)pWL->pBack->fHydrCond)
			        *((double)(pWL->fMatPotOld));
			dR35 += (double)2.0*(double)pSo->fDeltaZ*
			        ((double)pWL->pBack->fHydrCond - (double)pWL->fHydrCond);

			if (pPl != NULL) {
				if (pLR != NULL) {
					dR35 -= (double)((double)2 * pLR->fActLayWatUpt *pSo->fDeltaZ * pSo->fDeltaZ);
					pLR = pLR->pNext;
				}
			}

			/*******************************************************************************
			 * ----------------------------   Forward solution   ------------------------- */

			self->F1[iLayer] = (double)pWL->fHydrCond/(dR33-((double)pWL->pBack->fHydrCond*self->F1[iLayer-1]));    // [-]

			self->G1[iLayer] =  dR35 + ((double)pWL->pBack->fHydrCond * self->G1[iLayer-1]);    // [mm]
			self->G1[iLayer] /= dR33 - ((double)pWL->pBack->fHydrCond * self->F1[iLayer-1]);
		} // for

		/*******************************************************************************
		 *                       Pointer to last element
		 *                               i = n
		 *
		 * ===================  Lower boundary condition   ==================
		 *
		 */

		if (pWa->iBotBC <= (int)0) pWa->iBotBC = (int)1; //initialisation if not done before
		//to default situation: "free drainage"

		if (pWa->iBotBC == (int)4) { // lysimeter
			pWa->fGrdWatLvlPot = pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(double)2;
			if (pWL->pBack->fMatPotAct > pWa->fGrdWatLvlPot - pSo->fDeltaZ) iR9 = (int)2;
			else iR9 = (int)3;
		}


		if (pWa->iBotBC != (int)4) iR9 = pWa->iBotBC;

		if ((iR9 == (int)2)||(iR9 == (int)5)) { // water table
			pWL->fMatPotAct = pWa->fGrdWatLvlPot;
			pWL->fMatPotOld = pWa->fGrdWatLvlPot;
		}

		if (iR9 == (int)1) // free drainage
			pWL->fMatPotAct = ((double)self->G1[iLayer - 1]  - pWL->fMatPotOld + pWL->pBack->fMatPotOld)
			                  / ((double)1.0 - (double)self->F1[iLayer - 1]);        // [mm]

		if (iR9 == (int)3) // zero flux
			pWL->fMatPotAct = ((double)self->G1[iLayer - 1] + (double)2.0*pSo->fDeltaZ - pWL->fMatPotOld + pWL->pBack->fMatPotOld)
			                  / ((double)1.0 - (double)self->F1[iLayer - 1]);        // [mm]

		pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);



		/*******************************************************************************
		 *
		 *                       ---- Backward solution ----------
		 *                                 from
		 *                              i = n-1 to 0
		 */
// Pointer pWL to  list element n, i=n
		for ( diff=(double)0.0;
		      ((iLayer>0)&&(pWL->pBack!=NULL));
		      iLayer--,pWL = pWL->pBack) {
			pWL->pBack->fMatPotAct = (double)(self->F1[iLayer-1] * (double)(pWL->fMatPotAct)
			                                  + self->G1[iLayer-1]);

			if(iLayer>1)   diff += (double)fabs((double)(pWL->pBack->fMatPotAct - self->vorPot[iLayer-1]));

			self->vorPot[iLayer-1] = pWL->pBack->fMatPotAct;
		} /* for backwards  */

	}  /* end iteration (iterat = 1,20)   */

	return RET_SUCCESS;
} // SolveRG()


/*******************************************************************************************/
/*                                                                                         */
/*  Name     :  UpperBoundary()                                                            */
/*  Function :  Calculates upper boundary condition                                        */
/*  Authors  :  cs, 24.08.94                                                               */
/*              ch, 25.09.95                                                               */
/*                                                                                         */
/*******************************************************************************************/
/*  veränd. Var.:		pWa->pEvap->fActR                                                  */
/*						pWa->pEvap->fMaxR                                                  */
/*						pWL->fHydrCond                                                     */
/*						pWL->pNext->HydrCond                                               */
/*						pWL->fMatPotAct                                                    */
/*						pWL->fMatPotOld                                                    */
/*                                                                                         */
/*******************************************************************************************/
double UpperBoundary(water_flow_hutson *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;


	double     PotX, PotLow, PotHigh;
	double     fSurfaceFlux,fInfilt2, fDeviation;
	int       iterat;
	double     fMat2;


	double DeltaT     = pTi->pTimeStep->fAct;



	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;


	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;




//******************************************************************
// 1. case: Infiltration
//          consider ponding ?
//******************************************************************
	/*   if (fLeachPonding > EPSILON)
	    {
	    ******************************************************************
	    1a.  Ponding
	    ******************************************************************
		    fMat2 = pWL->fMatPotOld = pWL->fMatPotAct =  (double)0.0;
	       }
	     else
	    */
	//******************************************************************
	// 1b. Infiltration without Ponding
	//******************************************************************
	if (pWa->fPotInfiltration > EPSILON) {

//      fSurfaceFlux > EPSILON, since DeltaT<=1
		fSurfaceFlux = min(pWa->fInfiltR, pWa->fPotInfiltration / DeltaT); // [mm/day]

		//******************************************************************
		// Calculation of matric potential in layer 0 to guarantee
		// occurring of a flux of fSurfaceFlux in layer 1

		// matric potential to calculated for layer 0
		PotX = min(pWL->fMatPotOld, (double)-100.0);            // [mm]
		//? better:  PotX = (double)2*lpMPO->vorig;

		// Set interval limits for iteration:
		PotLow = pSW->fMinPot;                                 // [mm]
		PotHigh = (double)-3e5;                            // [mm]

		// calculates hydraulic conductivity of layer 1
		pWL->pNext->fHydrCond = NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld);    // [mm/day]


		//*********** Iteration  ***************************
		for (iterat=1; iterat<= 60; iterat++) {
			pWL->fHydrCond = (CONDUCTIVITY(PotX) + pWL->pNext->fHydrCond)/(double)2.0;

			fInfilt2 = pWL->fHydrCond
			           * ((double) 1 - (pWL->pNext->fMatPotOld - PotX)/ pSo->fDeltaZ);   // [mm/day]

			fDeviation = (double)fabs((double)1.0 - (fInfilt2/fSurfaceFlux));     // [-]

			if (fDeviation < (double)0.001) {
				iterat += 200;   // exit
			} else if ((PotHigh - PotLow  < (double)0.0001)&&(PotLow - PotHigh  < (double)0.0001)) {
				iterat += 100;   // exit
			} else {
				if (fInfilt2 < fSurfaceFlux) {
					PotHigh  = PotX;
					PotX     = (PotX + PotLow)/(double)2.0;
				} /* diminish matric potential of layer 0 */
				else {
					PotLow  = PotX;
					PotX    = (PotX + PotHigh)/(double)2.0;
				} /* diminish infiltration */

			} /* else fDeviation */
		}  /* end of iteration loop: iterat > 60 */

		if (iterat < 100) {
			PRINT_ERROR_ID(xpn,"Water Flow PDE Solver Did Not Converge.");
		}

		pWL->fMatPotOld  =  PotX;
		pWL->fMatPotAct  =  PotX;

		// von Neumann condition for Crank-Nicolson scheme:
		fMat2 = (double) (((fSurfaceFlux - pWL->fHydrCond)      // [mm]
		                   * 2.0
		                   * pSo->fDeltaZ /pWL->fHydrCond)
		                  -  pWL->fMatPotOld
		                  +  pWL->pNext->fMatPotOld);

	} // infiltration without ponding
	else

		//******************************************************************
		// 2. case:  Evaporation
		//******************************************************************
	{
		if ((pWa->pEvap->fActR * DeltaT <  EPSILON)||(pWa->pEvap->fPotR * DeltaT <  EPSILON)) {
			//fMat2 = (double)-1 * pSo->fDeltaZ;
			///*
			fMat2 = min((double)-1 * pSo->fDeltaZ ,(double)(-2) * pSo->fDeltaZ      // [mm]
			            - pWL->fMatPotOld + pWL->pNext->fMatPotOld);
			//*/
			// 15.11. no flux condition
			pWL->fMatPotAct = pWL->pNext->fMatPotAct - pSo->fDeltaZ;
			pWL->fMatPotOld = pWL->pNext->fMatPotOld - pSo->fDeltaZ;

		}  /* no evaporation, no flux */
		else {
			/* layer 0 */
			pWL->fHydrCond = CONDUCTIVITY(pWL->fMatPotOld);       // [mm/day]

			/* layer 1 */
			pWL->pNext->fHydrCond = NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld);

			/* layer 0 = average of layer 1 and layer 0   */
			pWL->fHydrCond = (pWL->fHydrCond + pWL->pNext->fHydrCond)/ (double)2.0;

			/* comparison of evaporation fluxdensity with maximal possible  */
			pWa->pEvap->fMaxR = pWL->fHydrCond
			                    * ((double)3000 *(double)102.2 + pWL->pNext->fMatPotOld)
			                    / pSo->fDeltaZ;    // [mm]

			if (pWa->pEvap->fActR > pWa->pEvap->fMaxR )
				// Limiting: soil hydraulic conductivity!
			{
				/*
				#ifdef LOGFILE
				 if ((pWa->pEvap->fActR - pWa->pEvap->fMax)*DeltaT > (double)1)
				    {
				     Message(0,ERROR_MAX_EVAP_TOO_HIGH);
				    }
				#endif
				*/

				fMat2 = - (double)3000 *(double)102.2 - pWL->pNext->fMatPotOld;    // [mm]
				pWa->pEvap->fActR = pWa->pEvap->fMaxR;                   // [mm/day]
			} else {
				// Limiting: evaporation rate!

				Set_At_Least_Epsilon(xpn,&pWL->fHydrCond, "fHydrCond");

				fMat2 = (- pWa->pEvap->fActR - pWL->fHydrCond)                  // [mm]
				        * (double)2.0 * pSo->fDeltaZ / pWL->fHydrCond
				        - pWL->fMatPotOld + pWL->pNext->fMatPotOld;

			}

		}   /* else evaporation  positive */
	}       /*  else no infiltration */

	return fMat2;
}  //  end    UpperBoundary



