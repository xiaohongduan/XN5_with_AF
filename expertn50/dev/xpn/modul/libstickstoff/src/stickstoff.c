//      stickstoff.c
//
//      Bearbeitet <Christoph Thieme> 2012
//

#include "stickstoff.h"
#include "stickstoff_macros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


G_DEFINE_TYPE(stickstoff, stickstoff, EXPERTN_MODUL_BASE_TYPE);

static void stickstoff_class_init(stickstoffClass *klass) {}
static void stickstoff_init(stickstoff *self) {self->ini_filename=NULL;}

int stickstoff_load(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);
	//Makro für Zugriff auf Struktur
	PCHEMISTRY pCh = xpn->pCh;
	
	stickstoff_load_ini(self);

	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR_ID(xpn,"Problem with hydraulic functions!");
		}

	/* Überprüfe korrektes Einlesen: */
	Test_Range(pCh->pCParam->fNitrifNO3NH4Ratio, (double)1,(double)100,"fNitrifNO3NH4Ratio");
	Test_Range(pCh->pCParam->fNitrifNH4ToN2O, (double)0,(double)1,"fNitrifNH4ToN2O");

	if (pCh->pCParam->fNitrifNH4ToN2O > (double)0)
		{
			char *S;
			S = g_strdup_printf("nitrification: N2O : NO3 = %d /1000",(int)(pCh->pCParam->fNitrifNH4ToN2O*1000.0));
			PRINT_MESSAGE(xpn,3,S);
			g_free(S);
		}


	return RET_SUCCESS;
 

}


int stickstoff_run(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);

	//
	PCHEMISTRY 	pCh = xpn->pCh;
	PCLIMATE 	pCl = xpn->pCl; //pCl kann man auch auskommentieren und es gibt keine Fehlermeldung!! komisch
	PHEAT 		pHe = xpn->pHe;
	PMANAGEMENT pMa = xpn->pMa;
	PSPROFILE 	pSo = xpn->pSo;
	PTIME 		pTi = xpn->pTi;
	PWATER 		pWa = xpn->pWa;

	//DECLARE N-POINTER
	PSLAYER      pSL;	
	PCLAYER      pCL;	
	PCPROFILE    pCP;
	PCPARAM      pPA;
	PSWATER      pSW;
	PWLAYER      pWL;
	PHLAYER      pHL;
	PNFERTILIZER pNF;
	


	N_ZERO_LAYER

	//DECLARE_COMMON_VAR
	//int OK            = 1;
	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;



	struct ncorr  corr = {(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1};
	/* static */
	double wmin,wlow,whigh;
	double fNitrifRate;



	/* Berechnung erfolgt von Schicht 1 bis vorletzte  */
	for (N_SOIL_LAYERS)
		{
			//Reduktionsfunktion der Temperatur:
			//fMinerQ10:  Q10-Wert für Mineralisierung
			//fMinerTemp: Basistemperatur für Mineralisierung
			//corr.Temp geht in die Nitrifikationsrate ein

			corr.Temp = abspowerDBL(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0));

			wmin = WATER_CONTENT((double)-155330);
			wlow =  WATER_CONTENT((double)-20000);

			whigh = max(wmin,(pSL->fPorosity - (double)0.08));
			wlow  = min(whigh,(wmin + (double)0.1));

			/* ch 1.3.96 	Eisgehalt in Feuchte-Funktion */
			corr.Feucht = Polygon4(pWL->fContAct + pWL->fIce,
			                       wmin,(double)0, wlow,(double)1, whigh,(double)1, pSL->fPorosity, pPA->fMinerSatActiv);

			/* Nitrifikations - Rate  */
			fNitrifRate = pCL->fNH4NitrMaxR * corr.Temp * corr.Feucht;

			/* Nitrifikation  */
			pCL->fNH4NitrR  = fNitrifRate *  max((double)0.0, (pCL->fNH4N - pCL->fNO3N / pCh->pCParam->fNitrifNO3NH4Ratio));
			pCL->fNH4ToN2OR = pCh->pCParam->fNitrifNH4ToN2O * pCL->fNH4NitrR;

			/* Nitrifikations- Ergebnis: */
			pCL->fNH4N        -= pCL->fNH4NitrR * DeltaT;
			pCL->fNO3N        += ((double)1 - pCh->pCParam->fNitrifNH4ToN2O) * pCL->fNH4NitrR * DeltaT;
			pCL->fN2ON        += pCh->pCParam->fNitrifNH4ToN2O * pCL->fNH4NitrR * DeltaT;
			
			//Funktionen zum Mitschneiden
			C_DEBUG_LOC_IN_ARRAY(corr.Feucht,iLayer); 
		}
	
	
	return RET_SUCCESS;
}


int stickstoff_done(stickstoff *self)
{
	G_FREE_IF_NOT_0(self->ini_filename);
	return RET_SUCCESS;
}
