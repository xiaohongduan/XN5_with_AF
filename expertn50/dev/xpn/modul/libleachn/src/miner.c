

#include "stickstoff.h"
#include "stickstoff_macros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define check_soil_temp(temp) if (temp>60.0) \
										{\
											PRINT_WARNING_ID(xpn,"Soil Temp too high");\
										}\
							  if (temp<-30.0) \
										{\
											PRINT_WARNING_ID(xpn,"Soil Temp too low");\
										}

int leachn_miner_load(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);

	stickstoff_load_ini(self);

	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR("Problem with hydraulic functions!");
		}
	//water_flow_hutson_run(self);
	//self->fCDiffSum=0.0;
	return RET_SUCCESS;
}

#define DECLARE_COMMON_VAR \
	int OK            = 1;\
	int iLayer        = 0;\
	double DeltaT     = xpn->pTi->pTimeStep->fAct;

#define DECLARE_N_POINTER \
	PSLAYER      pSL;\
	PCLAYER      pCL;\
	PCPROFILE    pCP;\
	PCPARAM      pPA;\
	PSWATER      pSW;\
	PWLAYER      pWL;\
	PHLAYER      pHL;\
	PNFERTILIZER pNF;



static double  abspower(double x, double y)
{

	double z;
	if (x < 0.0)
		{
			//sign = -1.0;
			x  *= -1.0;
		}

	if (y < 0.0)
		{
			y *= -1.0;
			if (x != (double) 0.0)  x = 1.0/x;
		}
	z = (double)pow((double)x,(double)y);
	return z;
}






///*********************************************************************************************/
///* Procedur    :   MinerLEACHN()                                                             */
///* Beschreibung:   Mineralisierung                                                           */
///*                 Methode nach LEACHM 3.0 (Hutson & Wagenet, 1992)                          */
///*-------------------------------------------------------------------------------------------*/
///*                                                                                           */
///*                 ep/gsf  14.03.94                                                          */
///*                 ch/gsf  19.04.95 (Neue Struktur"lpx->ntrans" für Steuervariable Biomasse) */
///*                         13.06.97 MinerLEACHN: Immobilisierung im Litter-Pool              */
///*                 ab/gsf  12.11.00 ausgeduennt                                              */
///*********************************************************************************************/
///* veränd. Var.		pCL->fCLitter                  pCL->fNH4N                                */
///*					pCL->fCManure                  pCL->fNO3N                                */
///*					pCL->fCO2C                     pCL->fNLitter                             */
///*					pCL->fCsolC                    pCL->fNManure                             */
///*					pCL->fHumusMinerR              pSL->fCHumus                              */
///*					pCL->fLitterMinerR             pSL->fNHumus                              */
///*                  pCL->fManureMinerR                                                       */
///*                  pCL->fCO2ProdR                 TEST_RANGE: pCL->fCManure                 */
///*					                                                                         */
///*********************************************************************************************/




int leachn_miner_run(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME 			pTi = xpn->pTi;

	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;
	PSLAYER      pSL;
	PCLAYER      pCL;
	PCPARAM      pPA;
	PSWATER      pSW;
	PWLAYER      pWL;
	PHLAYER      pHL;


	/*Variablen für die C-Flüsse*/
	double fNTotal, fCTotal;
	double rm, wmin,wlow,whigh;
	double f1,f3,f4;																	//auskommentiert 2.10.13
	double fHumusMineralR,fLitterToHumusR,fLitterToCO2R,fLitterToMBiomassR;
	double fManureToHumusR,fManureToCO2R,fManureToMBiomassR;
	double fNCHumus,fNCLitter,fNCManure,fLitterToNH4R,fManureToNH4R,fLitterImmR,fManureImmR;

	double fLitterToHumusCC,fLitterIntCC,fLitterToCO2CC;
	double fManureToHumusCC,fManureIntCC,fManureToCO2CC,fHumusToCO2CC, fHumusToLitterCC;
	double fLitterImmC,fLitImmReduc,fNH4ToLitterC;
	double fManureImmC,fManImmReduc,fNH4ToManureC;
	double fNO3ToLitterC,fNO3ToManureC;
	double fLitterToHumusC,fManureToHumusC,fHumusToLitterC;
	double fLitterToSolC, fManureToSolC; //Added by Hong on 20190808

	//double fBiomassToHumusR,fBiomassToCO2R,fBiomassToLitterR
	//double fCBiomass, fNBiomass

	struct ncorr corr= {(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1};

	/*Variablen für die Massenbilanz*/
	//double fCTotal,fNTotal;
	//double fCDiff,fNDiff;
	//static double fCDiffSum;
	//static double fNDiffSum;
	//double fFD = (double)0.02;   /* org.: 0.02 /-* Quotient aus Denitrifikanten und mikrobieller Biomasse  [1]  *-/ */

	pSL = xpn->pSo->pSLayer;
	pCL = xpn->pCh->pCLayer;
	pPA = xpn->pCh->pCParam;
	pSW = xpn->pSo->pSWater;
	pWL = xpn->pWa->pWLayer;
	pHL = xpn->pHe->pHLayer;
	
	self->__ERROR = 0;
	
	StandingPoolDecrease(self); //Added by Hong on 20180724


	SurfaceMiner(self); //exp_p durch stickstoff *self ersetzt, activated by Hong on 20180724

	rm    = (double)1.0;    /*orig :1.0; */			//analog crikle

	/* Schichtweise Berechnung */
	for(N_SOIL_LAYERS) //( iLayer = 1; iLayer < pSo->iLayers - 1; iLayer++ )
		{

			CHECK_VALID(pCL->fNO3N)

#ifdef DEB_TIME
			if (SimTime > START_TIME_DEB)
				DebTime(SimTime, iSchicht + 1000);
#endif

			/* Plausibilitätstest: */
#ifdef  TEST_RANGE
			AT_LEAST_EPSILON(pCL->fNLitter,"fNLitter\n in Miner.c");
			AT_LEAST_EPSILON(pCL->fNManure,"fNManure\n in Miner.c");
			AT_LEAST_EPSILON(pSL->fCHumus,  "fCHumus\n in Miner.c");
			AT_LEAST_EPSILON(pSL->fNHumus, "fNHumus\n in Miner.c");
#endif

			/* 1. Temperatur-Reduktionsfunktion*/
			/* Q10-Funktion */
			corr.Temp = abspowerDBL(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0));
			
			check_soil_temp(pHL->fSoilTemp);
			
			/*if (pHL->fSoilTemp>100.0)
				{
					printf("error\n");
				}*/

			/* 2. Feuchte-Reduktionsfunktion*/
			/* Ansatz LeachN, verändert, Herkunft der Grenzwerte unbekannt*/
			f1= (double)-153300.0;      //pF = 4.18 = PWP
			wmin = WATER_CONTENT(f1);
			f1= (double)-30660.0;       //pF = 3.49
			wlow = WATER_CONTENT(f1);
			whigh = max(wmin,(pSL->fPorosity - pPA->fMinerThetaMin));

// Original-Grenzwerte, beruhen auf Werten des volumetrischen Wassergehalts (auskommentiert 13.11.13)
//    wmin  = pSW->fContPWP; //= PWP-Wert
//    wlow  = wmin + (double)0.10;
//	  wmax  = pSL->fPorosity;
//    whigh = wmax - (double)0.08;

			corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce),
			                       wmin,(double)0,  wlow, (double)1 , whigh, (double)1,
			                       pSL->fPorosity, pPA->fMinerSatActiv);




//	 wmin = WATER_CONTENT((double)-155330);
//     wlow =  WATER_CONTENT((double)-20000);
//
//     whigh = max(wmin,(pSL->fPorosity - (double)0.08));
//     wlow  = min(whigh,(wmin + (double)0.1));
//
//
//	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce),
//                            wmin,(double)0,  wlow, (double)1 , whigh, (double)1,
//							wmax, pPA->fMinerSatActiv);

			corr.Feucht = abspower(corr.Feucht,rm);				//Zeile eingefügt analog crikle


			/* Kohlenstoff-Mineralisierungs-Konstanten (1/t) */
			fHumusMineralR    = pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
			fLitterToHumusR   = pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * pPA->fMinerHumFac;
			fLitterToCO2R     = pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * ((double)1.0 - pPA->fMinerEffFac);
			fLitterToMBiomassR= pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * ((double)1.0 - pPA->fMinerHumFac);
			fManureToHumusR   = pCL->fManureMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * pPA->fMinerHumFac;
			fManureToCO2R     = pCL->fManureMinerMaxR * corr.Temp * corr.Feucht * ((double)1.0 - pPA->fMinerEffFac);
			fManureToMBiomassR= pCL->fManureMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * ((double)1.0 - pPA->fMinerHumFac);

			/* ch 21.8.95 Csol nach Li et al. (DNDC) ist ein Indikator für Mineralisierungsaktivität (kein Pool)
			ab 23.11.00 dort aber als Indikator für die Mineralisierung des soil organic carbon verstanden
			und eigentlich als DOC für die Denitrifikation angesetzt!*/
			CHECK_VALID(pCL->fCLitter)
			
			pCL->fCsolC  = pCL->fCLitter * fLitterToMBiomassR;
			pCL->fCsolC   += pCL->fCManure * fManureToMBiomassR;				
			//pCL->fCsolC += pWL->fWaterFreezeR * lpXT->fIcefRedCsol;
			//pCL->fCsolC += pSL->fCHumus * fHumusMineralR * lpXT->fHumusToCsol;


			/* CN-Werte für Litter und Manure   */
//Changed by EP on 20180629
			fNCLitter     = (pCL->fCLitter > EPSILON)?
			                pCL->fNLitter/pCL->fCLitter
			                :(double)0.1;

			fNCManure     = (pCL->fCManure > EPSILON)?
			                pCL->fNManure / pCL->fCManure
			                :(double)0.1;

			fNCHumus      = (pSL->fCHumus > EPSILON)?
			                pSL->fNHumus / pSL->fCHumus
			                :(double)-99;
/*							
			fNCLitter     = (pCL->fCLitter > EPSILON)?
			                pCL->fNLitter/pCL->fCLitter
			                :(double)0.1;

			fNCManure     = (pCL->fNManure > EPSILON)?
			                pCL->fNManure / pCL->fCManure
			                :(double)0.1;

			fNCHumus      = (pSL->fNHumus > EPSILON)?
			                pSL->fNHumus / pSL->fCHumus
			                :(double)-99;
*/			
			Test_Range(xpn,fNCHumus,(double)0.001,(double)1,"CN-Humus");

			/*Entscheidung Immobilisierung/Mineralisierung und
			  Berechnung der jeweiligen Konstanten */
			f3     = fNCLitter - pPA->fMinerEffFac / pCL->fMicBiomCN;

			fLitterToNH4R = (f3 > 0)? f3 : 0;
			fLitterImmR   = (f3 < 0)? (double)-1 * f3 : 0;

			f4     = fNCManure - pPA->fMinerEffFac / pCL->fMicBiomCN;
			fManureToNH4R = (f4 > 0)? f4 : 0;
			fManureImmR   = (f4 < 0)? (double)-1 * f4 : 0;


			/********************************************************************************/
			/*                  Veraenderung des C-Pools pro Zeitschritt                    */
			/********************************************************************************/

			fCTotal = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C;

			/* 1. Veraenderung des C-Litter Pools pro Zeitschritt */

			fLitterToHumusCC  = pCL->fCLitter * fLitterToHumusR;
			fLitterToCO2CC    = pCL->fCLitter * fLitterToCO2R;
			fLitterIntCC      = pCL->fCLitter * fLitterToMBiomassR;
			
			
				
			
//  fHumusToLitterCC  = pCL->fCLitter * fHumusShiftR;			//äquivalent zu crickle raus
			fHumusToLitterCC  = (double)0;

			pCL->fCLitter -=(fLitterToHumusCC + fLitterToCO2CC - fHumusToLitterCC)* DeltaT;

			/* 2. Veraenderung des C-Manure Pools pro Zeitschritt */

			fManureToHumusCC  = pCL->fCManure * fManureToHumusR;
			fManureIntCC      = pCL->fCManure * fManureToMBiomassR;
			fManureToCO2CC    = pCL->fCManure * fManureToCO2R;

			pCL->fCManure  -= (fManureToHumusCC + fManureToCO2CC)* DeltaT;

			/* 3. Veraenderung des C-Humus Pools */

			fHumusToCO2CC    = pSL->fCHumus * fHumusMineralR;

			pSL->fCHumus  += (fLitterToHumusCC +fManureToHumusCC -fHumusToCO2CC - fHumusToLitterCC)* DeltaT;

			/* 4. BioMasse Zuwachs */
			//pCL->fCMicBiomDenit += lpXT->MinerBio * fFD * (fLitterIntCC + fManureIntCC) * DeltaT;

			/* 5. CO2-C Zuwachs */
			pCL->fCO2ProdR = fLitterToCO2CC +  fManureToCO2CC + fHumusToCO2CC;

			pCL->fCO2C += pCL->fCO2ProdR * DeltaT;


			/********************************************************************************/
			/*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
			/********************************************************************************/

			fNTotal = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N;


			/* 1. Immobilisierung im Litter-Pool */
			fLitterImmC = fLitterImmR * (fLitterIntCC + fLitterToHumusCC + fLitterToCO2CC);

			/*ch, 17.7.97 max. 8% des NMin können am Tag immobilisiert werden: */
			if ((fLitterImmC < EPSILON)||((double)0.08*(pCL->fNO3N + pCL->fNH4N)>= fLitterImmC))
				fLitImmReduc =  (double)1.0;
			else
				{
					fLitImmReduc =  (double)0.015*(pCL->fNO3N + pCL->fNH4N)/ fLitterImmC;
				}

			if (pCL->fNO3N + pCL->fNH4N > EPSILON)
				{
					fNH4ToLitterC = fLitterImmC * fLitImmReduc * RelAnteil(pCL->fNH4N,pCL->fNO3N);
					fNO3ToLitterC = fLitterImmC * fLitImmReduc * RelAnteil(pCL->fNO3N,pCL->fNH4N);
					
					
				}
			else
				{
					fLitImmReduc = 0.0;
					fNH4ToLitterC  = 0.0;
					fNO3ToLitterC  = 0.0;
				}
				
			

			pCL->fNLitterImmobR = fNH4ToLitterC + fNO3ToLitterC; /*=fLitterImmC*/

			/* 2. Immobilisierung im Manure-Pool */
			fManureImmC = fManureImmR * (fManureIntCC + fManureToHumusCC + fManureToCO2CC);

			/*ch, 13.5.97, 13.1.97 max. 8% des NMin können am Tag immobilisiert werden: */
			if ((fManureImmC < EPSILON)||((double)0.08*(pCL->fNO3N + pCL->fNH4N)>= fManureImmC))
				fManImmReduc =  (double)1.0;
			else
				{
					fManImmReduc =  (double)0.08*(pCL->fNO3N + pCL->fNH4N)/ fManureImmC;
				}

			CHECK_VALID(pCL->fNO3N)
			if (pCL->fNO3N + pCL->fNH4N > EPSILON)
				{
					fNH4ToManureC = fManureImmC * fManImmReduc * RelAnteil(pCL->fNH4N,pCL->fNO3N);
					fNO3ToManureC = fManureImmC * fManImmReduc * RelAnteil(pCL->fNO3N,pCL->fNH4N);
				}
			else
				fNH4ToManureC = fNO3ToManureC = fManImmReduc = (double)0;

			pCL->fNManureImmobR = fNH4ToManureC + fNO3ToManureC;

			pCL->fNImmobR = pCL->fNManureImmobR + pCL->fNLitterImmobR;
			

			/* 3. Ammonium-Mineralisierung aus Humus, Litter und Manure */
			pCL->fHumusMinerR  = pSL->fNHumus  * fHumusMineralR;
			pCL->fLitterMinerR = fLitterToNH4R * (fLitterToHumusCC + fLitterToCO2CC + fLitterIntCC);
			pCL->fManureMinerR = fManureToNH4R * (fManureToHumusCC + fManureToCO2CC + fManureIntCC);
			pCL->fMinerR = pCL->fHumusMinerR + pCL->fLitterMinerR + pCL->fManureMinerR;//added by Hong on 20180124, for consistence in balance.c 

			/* N-Bilanz */
			pCL->fNH4N += (pCL->fHumusMinerR  + pCL->fLitterMinerR  + pCL->fManureMinerR
			               -  fNH4ToLitterC - fNH4ToManureC) * DeltaT;
			pCL->fNO3N -= (fNO3ToLitterC + fNO3ToManureC) * DeltaT;
			CHECK_VALID(pCL->fNO3N)
								
			/* 4. Litter-N Abbau */
			fHumusToLitterC = fHumusToLitterCC * fNCHumus; //sinnlos weil immer 0

			fLitterToHumusC = min((double)0.5 * pCL->fNLitter,
			                      (fLitterToHumusCC * fLitImmReduc / pCL->fMicBiomCN));

			pCL->fNLitter  += (pCL->fNLitterImmobR  - pCL->fLitterMinerR
			                   + fHumusToLitterC - fLitterToHumusC) * DeltaT;

			/* 5. Manure-N Abbau */
			fManureToHumusC = min((double)0.5 * pCL->fNManure,
			                      fManureToHumusCC * fManImmReduc / pCL->fMicBiomCN);

			pCL->fNManure += (pCL->fNManureImmobR - fManureToHumusC - pCL->fManureMinerR) * DeltaT;

			/* 6. Humus-N Abbau */
			pSL->fNHumus += (fLitterToHumusC + fManureToHumusC - fHumusToLitterC
			                 - pCL->fHumusMinerR)* DeltaT;

			//fCDiff = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C-fCTotal;					//äquivalent zu crickle raus
			//fNDiff = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N-fNTotal;
			//self->fCDiffSum    += fCDiff;
			//fNDiffSum    += fNDiff;

		}    /* Schichtweise */

	return 0;
}

//Added by Hong on 20180724
/*************************************************************************************/
/* Procedur    :   StandingPoolDecrease                                              */
/* Beschreibung:   Abnahme des Standing-Pools                                        */
/*                 Grundlage RESMAN (Stott et al. 1995)                              */
/*                                                                                   */
/*              GSF/ab  Axel Berkenkamp         18.06.01                             */
/*                                                                                   */
/*************************************************************************************/
/*	veränd. Var.  pCh->pCProfile->fCLitterSurf    pCh->pCProfile->fNLitterSurf       */
/*				  pCh->pCProfile->fCStandCropRes  pCh->pCProfile->fNStandCropRes     */
/*                                                                                   */
/*************************************************************************************/
int StandingPoolDecrease(stickstoff *self)
 {
    expertn_modul_base *xpn = &(self->parent);
	PTIME 			pTi = xpn->pTi;	 
	PCPROFILE       pCP = xpn->pCh->pCProfile;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20180731
    /*Hilfsvariablen*/
    double fCDecrease,fNDecrease;
	  

  /*  Funktionsaufruf einmal täglich */
  if (NewDay(pTi))
  {
	  
   if (pCP->fCStandCropRes > (double)0.0 && pCP->fNStandCropRes > (double)0.0)
   {
    fCDecrease = (double)0.01 * pCP->fCStandCropRes;
    fNDecrease = (double)0.01 * pCP->fNStandCropRes;

	pCP->fCStandCropRes -= fCDecrease;
	pCP->fCLitterSurf += fCDecrease;

	pCP->fNStandCropRes -= fNDecrease;
	pCP->fNLitterSurf += fNDecrease;
	
	//Hong added on 20180807 for C-balance
	pCB->dCInputSurf += fCDecrease;
	//pCB->dCInputCum += fCDecrease;
   }
  }
 
 return 0;
 }


/*************************************************************************************/
/* Procedur    :   SurfaceMineralisierung                                            */
/* Beschreibung:   Mineralisierung der Oberflächenpools                              */
/*                 Methode Grundlage SOILN / NITS mit Veränderungen                  */
/*                                                                                   */
/*              GSF/ab: Axel Berkenkamp         07.06.01                             */
/*                  ab: Veränderung des C-Abbaus bei Nmin-Mangel 28.08.01            */
/*                                                                                   */
/*                                                                                   */
/*                                                                                   */
/*************************************************************************************/
/*	veränd. Var.	pCP->fCLitterSurf          pCP->fNLitterSurf                     */
/*					pCP->fCManureSurf          pCP->fNManureSurf                     */
/*					pCP->fCHumusSurf           pCP->fNHumusSurf                      */
/*                  pCL->fCO2C                 pCP->fNH4NSurf                        */
/*					pCL->fCO2ProdR(?)          pCP->fNO3NSurf                        */
/*                  pCP->fCNLitterSurf         pCP->fCNManureSurf                    */ 
/*                  pCL->fHumusMinerR(?)       pCL->fHumusImmobR(?)                  */
/*					pCL->fLitterMinerR(?)      pCL->fLitterImmobR(?)                 */
/*                  pCL->fManureMinerR(?)      pCL->fManureImmobR(?)                 */
/*					corr.Temp                  corr.Feucht                           */
/*                                                                                   */
/*************************************************************************************/
 int SurfaceMiner(stickstoff *self)
 {
  expertn_modul_base *xpn = &(self->parent);	 
	 
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr ={(double)1};
  
  /******************************************************************/
  /*                Variablendeklaration                            */
  /******************************************************************/

  /*Hilfsvariablen*/
  double f1,f2,f3;
  double fRedLit,fRedMan,fEffNew;
  double NoImmLit, NoImmMan;
  double fMinerEffFac,fMicBiomCN,fMinerHumFac;

  /* Konstanten (1/dt) */
  double fNH4ImmMaxK = (double)0.1;
  double fNO3ImmMaxK = (double)0.1;
  double fLitterImmK,fManureImmK;
  double fLitterToNH4K,fManureToNH4K;

  /* Raten für die C und N-Flüsse (dC/dt bzw. dN/dt) */
  double fHumusMinerMaxR;  
  double fLitterMinerMaxR; 
  double fManureMinerMaxR; 
  double fNToLitterR;
  double fNH4ImmR,fNO3ImmR;
  double fCLitterToHumusR,fCLitterToLitterR,fCLitterToCO2R;
  double fCManureToHumusR,fCManureToLitterR,fCManureToCO2R;
  double fCHumusToCO2R;
  
  double fNH4ToLitterR,fNO3ToLitterR;
  double fNLitterToHumusR,fNManureToHumusR,fNManureToLitterR;
  double fNHumusToNH4R,fNLitterToNH4R,fNManureToNH4R;
  
  /*Variablen für die Surface-Flüsse (eigentlich auch Raten) */
  double fCLitterSurfDecay, fCManureSurfDecay, fCHumusSurfDecay;
  double fNLitterSurfDecay, fNManureSurfDecay, fNHumusSurfDecay;

  /*Variablen für die Massenbilanz*/
  double fCTotalSurf,fNTotalSurf;
  double fCDiffSurf,fNDiffSurf;
  static double fCDiffSum, fNDiffSum;
  /* Variablendeklaration Ende  */




  N_ZERO_LAYER /* Setzten der Pointer auf die nullte Schicht*/


  /******************************************************************/
  /*                          Reduktionsfunktionen                  */
  /******************************************************************/
  
  /* 1. Temperatur: Ansatz SOILN, Bezugswert: Tagesmittelwert der Luft*/ 
  corr.Temp = abspower(pPA->fMinerQ10,((xpn->pCl->pWeather->fTempAve - pPA->fMinerTempB)/(double)10.0));


  /* 2. Feuchte: Ansatz RESMAN, Bezugswert: Das wassergefüllte Porenvolumen
     der obersten Bodenschicht, Optimalwert WFPS = 60% */
  f1 = (pWL->pNext->fContAct + pWL->pNext->fIce) / pSL->pNext->fPorosity;

  if (f1 < (double)0.6)
    corr.Feucht = f1 / (double)0.6;

  else
    corr.Feucht = (double) 0.6 / f1;
  /*Reduktionsfunktion Ende*/


  /******************************************************************/
  /*    C und N -Mineralisierungsraten (dC/dt bzw. dN/dt)           */
  /******************************************************************/

fHumusMinerMaxR  = pCL->pNext->fHumusMinerMaxR;
fLitterMinerMaxR = pCL->pNext->fLitterMinerMaxR;
fManureMinerMaxR = pCL->pNext->fManureMinerMaxR;
fMinerEffFac     = pPA->pNext->fMinerEffFac;
fMicBiomCN       = pCL->pNext->fMicBiomCN;
fMinerHumFac     = pPA->pNext->fMinerHumFac;

  fCHumusSurfDecay  = pCP->fCHumusSurf  * fHumusMinerMaxR  * corr.Temp * corr.Feucht;
  fCLitterSurfDecay = pCP->fCLitterSurf * fLitterMinerMaxR * corr.Temp * corr.Feucht;
  fCManureSurfDecay = pCP->fCManureSurf * fManureMinerMaxR * corr.Temp * corr.Feucht;

  fNHumusSurfDecay  = pCP->fNHumusSurf  * fHumusMinerMaxR  * corr.Temp * corr.Feucht;
  fNLitterSurfDecay = pCP->fNLitterSurf * fLitterMinerMaxR * corr.Temp * corr.Feucht;
  fNManureSurfDecay = pCP->fNManureSurf * fManureMinerMaxR * corr.Temp * corr.Feucht;
  /* C und N -Mineralisierungsraten Ende */

  /******************************************************************/
  /* Entscheidung Immobilisierung/Mineralisierung beim Litter u.    */
  /* Manure Abbau. Der Wert hängt ab vom CN Wert der abgebauten     */
  /* Substanz, dem CN Wert der aufgebauten Substanz (Mikrobielle    */ 
  /* Biomasse und Humus) sowie dem Effektivitätsfaktor mit der die  */ 
  /* Mikros Kohlenstoff verwerten können.                           */
  /* Hilfsvariable positiv = Nettomineralisierung                   */
  /* Hilfsvariable negativ = Nettoimmobilisierung                   */                            
  /******************************************************************/

  /* CN-Werte */
  pCP->fCNLitterSurf = (pCP->fNLitterSurf > EPSILON)?
    	pCP->fCLitterSurf / pCP->fNLitterSurf
    	:(double)0.1; 
            
  pCP->fCNManureSurf = (pCP->fNManureSurf > EPSILON)?
    	pCP->fCManureSurf / pCP->fNManureSurf
      	:(double)0.1; 

  /*Mineralisierungs bzw. Immobilisierungsfaktor*/
   if (pCP->fCNLitterSurf > (double)0.1)
   {
    f2     = (double)1 / pCP->fCNLitterSurf - fMinerEffFac / fMicBiomCN;
	fLitterToNH4K = (f2 > 0)? f2 : 0;
    fLitterImmK   = (f2 < 0)? (double)-1 * f2 : 0;
   }

  else
   {
    fLitterToNH4K = (double)0;
    fLitterImmK   = (double)0;
   }

  if (pCP->fCNManureSurf > (double)0.1)
   {
    f3     = (double)1 / pCP->fCNManureSurf - fMinerEffFac / fMicBiomCN;
    fManureToNH4K = (f3 > 0)? f3 : 0;
    fManureImmK   = (f3 < 0)? (double)-1 * f3 : 0;
   }
  
  else
   {
    fManureToNH4K = (double)0;
    fManureImmK   = (double)0;
   }
  /* Entscheidung Immobilisierung/Mineralisierung Ende*/


  /*************************************************************************/
  /* Wenn die Immobilisierung mehr N-Bedarf entwickelt als mineralisches N */
  /* zur Verfügung steht, wird das Wachstum der Mikroorganismen und die    */
  /* Bildung von Humus unterbunden, sowie der C-Abbau reduziert            */
  /*************************************************************************/

  fNToLitterR = fLitterImmK * fCLitterSurfDecay + fManureImmK * fCManureSurfDecay;

  /*Maximale Immobilisierungsraten*/
  fNH4ImmR = fNH4ImmMaxK * pCP->fNH4NSurf;
  fNO3ImmR = fNO3ImmMaxK * pCP->fNO3NSurf;

  if (fNToLitterR  > (fNH4ImmR + fNO3ImmR))
   {  
    if (fLitterImmK)
	 {
	  NoImmLit = (double)0.0;
      fEffNew = fMicBiomCN / pCP->fCNLitterSurf;
      fRedLit = min(fEffNew,fMinerEffFac);
      fLitterToNH4K = (double)0;
      fLitterImmK   = (double)0;
     }
	else
	 {
	  NoImmLit = (double)1.0;
	  fRedLit  = (double)1.0;
	 }

    if (fManureImmK)
	 {
	  NoImmMan = (double)0.0;
      fEffNew = fMicBiomCN / pCP->fCNManureSurf;
      fRedMan = min(fEffNew,fMinerEffFac);
      fManureToNH4K = (double)0;
      fManureImmK   = (double)0;
     }
    
	else
	 {
	  NoImmMan = (double)1.0;
	  fRedMan  = (double)1.0;
	 }

	fNToLitterR = (double)0.0; 
   }
  
  else
   {
    fRedLit = (double)1.0;
	fRedMan = (double)1.0;
	NoImmLit = (double)1.0;
	NoImmMan = (double)1.0;
   }
   
  /*Berechnung des Reduktionsfaktors alte Variante  	
  fRed = (double)1.0;                      // Reduktionsfaktor =1 => keine Reduktion 

    fRed = (fNH4ImmR + fNO3ImmR) / fNToLitterR;

  if (fLitterImmK)
  {   
   fCLitterSurfDecay = fRed * fCLitterSurfDecay;
   fNLitterSurfDecay = fRed * fNLitterSurfDecay;
  }

  if (fManureImmK)
  {
   fCManureSurfDecay = fRed * fCManureSurfDecay;
   fNManureSurfDecay = fRed * fNManureSurfDecay;
  }

  fNToLitterR        = fRed * fNToLitterR;
  Berechnung Reduktion des Abbaus Ende */


  /******************************************************************/
  /* Veraenderung des C-Pools pro Zeitschritt                       */
  /******************************************************************/
    
  /* 1. Gesamt C vor dem Zeitschritt */
  fCTotalSurf = pCP->fCHumusSurf + pCP->fCLitterSurf + pCP->fCManureSurf + pCL->fCO2C;

  /* 2. Abbau des C-Litter Pools pro Zeitschritt */
  
  if(!NoImmLit)
   {
    fCLitterToCO2R    = fCLitterSurfDecay * fRedLit;
    fCLitterToHumusR  = (double)0.0;
    fCLitterToLitterR = (double)0.0;
   }

  else
   {
    fCLitterToHumusR  = fCLitterSurfDecay * fMinerEffFac * fMinerHumFac;
    fCLitterToCO2R    = fCLitterSurfDecay * ((double)1.0 - fMinerEffFac);
    fCLitterToLitterR = fCLitterSurfDecay * fMinerEffFac * ((double)1.0 - fMinerHumFac);
   }
  
  /* 3. Abbau des C-Manure Pools pro Zeitschritt */
  
  if(!NoImmMan)
   {  
    fCManureToCO2R    = fCManureSurfDecay * fRedMan;
    fCManureToHumusR  = (double)0.0;
    fCManureToLitterR = (double)0.0;
   }

  else
   {
    fCManureToHumusR  = fCManureSurfDecay * fMinerEffFac * fMinerHumFac;
    fCManureToCO2R    = fCManureSurfDecay * ((double)1.0 - fMinerEffFac);
    fCManureToLitterR = fCManureSurfDecay * fMinerEffFac * ((double)1.0 - fMinerHumFac);
   }

  /* 4. Abbau des C-Humus Pools */
  fCHumusToCO2R     = fCHumusSurfDecay;
  
  /* 5. Zunahme im CO2 Pool */
  pCL->fCO2ProdR = fCLitterToCO2R +  fCManureToCO2R + fCHumusToCO2R;

  /* 6. Veraenderung in den C-Pools */
  pCP->fCLitterSurf -= (fCLitterToHumusR + fCLitterToCO2R - fCManureToLitterR) * DeltaT;
  pCP->fCManureSurf -= (fCManureToHumusR + fCManureToCO2R + fCManureToLitterR) * DeltaT;
  pCP->fCHumusSurf  += (fCLitterToHumusR + fCManureToHumusR - fCHumusToCO2R) * DeltaT;
  pCL->fCO2C        += pCL->fCO2ProdR * DeltaT;
  
  //Added by Hong on 20180731
  //if (pCL->fCO2C>0.0)
    {
	  pCP->dCO2SurfEmisCum +=pCL->fCO2ProdR * DeltaT;
     }   	 
//End of Hong	 
  /* Veränderungen im C-Pool Ende */


  /********************************************************************************/
  /*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
  /********************************************************************************/
    
  /* 1. Gesamt N vor dem Zeitschritt */
  fNTotalSurf = pCP->fNHumusSurf + pCP->fNLitterSurf + pCP->fNManureSurf + pCP->fNH4NSurf + pCP->fNO3NSurf;

  /* 2. Immobilisierung im Litter-Pool */
  fNH4ToLitterR = fNToLitterR * RelAnteil(pCP->fNH4NSurf,pCP->fNO3NSurf);
  fNO3ToLitterR = fNToLitterR * RelAnteil(pCP->fNO3NSurf,pCP->fNH4NSurf);

  /* 3. Bildung von mikrobieller Biomasse */
  fNManureToLitterR = fCManureToLitterR / fMicBiomCN;


  /* 4. Humifizierung im Humus-Pool */
  if(!NoImmLit)
   fNLitterToHumusR = (double)0.0;
  else
   fNLitterToHumusR = fCLitterToHumusR / fMicBiomCN;

  if(!NoImmMan)
   fNManureToHumusR = (double)0.0;
  else
   fNManureToHumusR = fCManureToHumusR / fMicBiomCN;
   
  /* 5. Ammonium-Mineralisierung aus Humus, Litter und Manure */
  fNHumusToNH4R  = fNHumusSurfDecay;
  fNLitterToNH4R = fLitterToNH4K * fCLitterSurfDecay;
  fNManureToNH4R = fManureToNH4K * fCManureSurfDecay;

  /* 6. Veränderungen in den N Pools */
  //pCP->fNH4NSurf += (fNHumusToNH4R  + fNLitterToNH4R  + fNManureToNH4R -  fNH4ToLitterR) * DeltaT;
  pCP->fNO3NSurf -= fNO3ToLitterR * DeltaT;
  
  pCP->fNLitterSurf += (fNToLitterR + fNManureToLitterR - fNLitterToNH4R - fNLitterToHumusR) * DeltaT;
  pCP->fNManureSurf -= (fNManureToHumusR + fNManureToLitterR + fNManureToNH4R) * DeltaT;
  pCP->fNHumusSurf += (fNLitterToHumusR + fNManureToHumusR - fNHumusToNH4R) * DeltaT;
  /* Veränderungen im N-Pool Ende */

  /* Übertragung auf globale Variablen */
  pCL->fNImmobR = fNToLitterR;
  pCL->fMinerR = fNHumusToNH4R + fNLitterToNH4R + fNManureToNH4R;
  pCL->fHumusMinerR  = fNHumusToNH4R;
  pCL->fLitterMinerR = fNLitterToNH4R;
  pCL->fManureMinerR = fNManureToNH4R;

  /********************************************************************************/
  /* Massenbilanz, die Summe der Veränderungen muss ungefär Null ergeben */
  /********************************************************************************/
  
  fCDiffSurf = pCP->fCHumusSurf + pCP->fCLitterSurf + pCP->fCManureSurf + pCL->fCO2C - fCTotalSurf;
  fNDiffSurf = pCP->fNHumusSurf + pCP->fNLitterSurf + pCP->fNManureSurf + pCP->fNH4NSurf + pCP->fNO3NSurf - fNTotalSurf;
  /*Massenbilanz Ende*/
  
  fCDiffSum += fCDiffSurf;
  fNDiffSum += fNDiffSurf;
  return 1;
 } /*Funktion Ende*/
//End of Hong

int leachn_miner_done(stickstoff *self)
{
	G_FREE_IF_NOT_0(self->ini_filename);
	return RET_SUCCESS;
}
