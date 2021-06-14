

#include "stickstoff.h"
#include "stickstoff_macros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



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
	double DeltaT     = pTi->pTimeStep->fAct;

#define DECLARE_N_POINTER \
	PSLAYER      pSL;\
	PCLAYER      pCL;\
	PCPROFILE    pCP;\
	PCPARAM      pPA;\
	PSWATER      pSW;\
	PWLAYER      pWL;\
	PHLAYER      pHL;\
	PNFERTILIZER pNF;

//#define  N_ZERO_LAYER  \
//            pSL = pSo->pSLayer,\
//            pCL = pCh->pCLayer,\
//            pCP = pCh->pCProfile,\
//            pPA = pCh->pCParam,\
//            pSW = pSo->pSWater,\
//            pWL = pWa->pWLayer,\
//            pHL = pHe->pHLayer,\
//            pNF = pMa->pNFertilizer;
//
//#define  N_SOIL_LAYERS  	N_FIRSTLAYER N_STOPLAYER N_NEXTLAYER
//
//#define  N_FIRSTLAYER  iLayer=1,\
//              pSL = pSo->pSLayer->pNext,\
//              pCL = pCh->pCLayer->pNext,\
//              pSW = pSo->pSWater->pNext,\
//              pWL = pWa->pWLayer->pNext,\
//              pPA = pCh->pCParam->pNext,\
//              pHL = pHe->pHLayer->pNext;
//
//#define  N_STOPLAYER \
//             ((iLayer<pSo->iLayers-1)&&\
//              (pSL->pNext!=NULL)&&\
//              (pCL->pNext!=NULL)&&\
//              (pSW->pNext!=NULL)&&\
//              (pWL->pNext!=NULL)&&\
//              (pPA->pNext!=NULL)&&\
//              (pHL->pNext!=NULL));
//
//#define  N_NEXTLAYER \
//              iLayer++,\
//              pSL = pSL->pNext,\
//		      pCL = pCL->pNext,\
//		      pSW = pSW->pNext,\
//		      pWL = pWL->pNext,\
//		      pPA = pPA->pNext,\
//		      pHL = pHL->pNext

/////***********************************************************************************/
/////* Procedur    :   abspower()                                                     */
/////* Beschreibung:                   												   */
/////*                                                                                 */
/////*                                 										            */
/////***********************************************************************************/

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
	PCHEMISTRY 		pCh = xpn->pCh;
	PCLIMATE 		pCl = xpn->pCl;
	PHEAT 			pHe = xpn->pHe;
	PLOCATION 		pLo = xpn->pLo;
	PMANAGEMENT 	pMa = xpn->pMa;
	PPLANT 			pPl = xpn->pPl;
	PSPROFILE 		pSo = xpn->pSo;
	PTIME 			pTi = xpn->pTi;
	PWATER 			pWa = xpn->pWa;


	DECLARE_COMMON_VAR
	DECLARE_N_POINTER

	/*Variablen für die C-Flüsse*/
	double fNTotal, fCTotal;
	double rm, wmin,wlow,whigh, wmax;
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

	//double fBiomassToHumusR,fBiomassToCO2R,fBiomassToLitterR
	//double fCBiomass, fNBiomass

	struct ncorr corr= {(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1};

	/*Variablen für die Massenbilanz*/
	//double fCTotal,fNTotal;
	//double fCDiff,fNDiff;
	//static double fCDiffSum;
	//static double fNDiffSum;
	//double fFD = (double)0.02;   /* org.: 0.02 /-* Quotient aus Denitrifikanten und mikrobieller Biomasse  [1]  *-/ */

	N_ZERO_LAYER


//SurfaceMiner(self); //exp_p durch stickstoff *self ersetzt

	rm    = (double)1.0;    /*orig :1.0; */			//analog crikle

	/* Schichtweise Berechnung */
	for(N_SOIL_LAYERS) //( iLayer = 1; iLayer < pSo->iLayers - 1; iLayer++ )
		{



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
			pCL->fCsolC  = pCL->fCLitter * fLitterToMBiomassR;
			pCL->fCsolC += pCL->fCManure * fManureToMBiomassR;
			//pCL->fCsolC += pWL->fWaterFreezeR * lpXT->fIcefRedCsol;
			//pCL->fCsolC += pSL->fCHumus * fHumusMineralR * lpXT->fHumusToCsol;


			/* CN-Werte für Litter und Manure   */
			fNCLitter     = (pCL->fCLitter > EPSILON)?
			                pCL->fNLitter/pCL->fCLitter
			                :(double)0.1;

			fNCManure     = (pCL->fNManure > EPSILON)?
			                pCL->fNManure / pCL->fCManure
			                :(double)0.1;

			fNCHumus      = (pSL->fNHumus > EPSILON)?
			                pSL->fNHumus / pSL->fCHumus
			                :(double)-99;

			Test_Range(fNCHumus,(double)0.001,(double)1,"CN-Humus");

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
				fLitImmReduc = fNH4ToLitterC  = fNO3ToLitterC  = (double)0;

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
			//pCL->fMinerR = pCL->fHumusMinerR + pCL->fLitterMinerR + pCL->fManureMinerR;

			/* N-Bilanz */
			pCL->fNH4N += (pCL->fHumusMinerR  + pCL->fLitterMinerR  + pCL->fManureMinerR
			               -  fNH4ToLitterC - fNH4ToManureC) * DeltaT;
			pCL->fNO3N -= (fNO3ToLitterC + fNO3ToManureC) * DeltaT;

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

int leachn_miner_done(stickstoff *self)
{
	G_FREE_IF_NOT_0(self->ini_filename);
	return RET_SUCCESS;
}
