//      transpn.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "transpn.h"
#include "stickstoff_util.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(transpn, transpn, EXPERTN_MODUL_BASE_TYPE);


static void transpn_class_init(transpnClass *klass) {}


static void transpn_init(transpn *self)
{
//	gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!


// print Output Message if Modul is used from Expert N
	

	self->WCont=NULL;
	self->HCond=NULL;
	self->DWCap=NULL;
	self->MPotl=NULL;

}

int transpn_flow_init (transpn *self)
{
	expertn_modul_base *xpn = &(self->parent);

	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR("Problem with hydraulic functions!");
		}
	return RET_SUCCESS;
}

// Our Modul Functions:

G_MODULE_EXPORT int transpn_flow(transpn *self)
{
	
	transpn_NTransp(self,0);
	
	return RET_SUCCESS;
}
/*
static int xpn->pWa->iMobImm;
static BOOL xpn->pWa->FreeD = TRUE; // aus hydrus
*/


//define abspower(x,y) abspowerDBL(x,y)












/**********************************************************************************************/
/* Procedur    :   Get_DON_TransformationRates()                                              */
/* Beschreibung:   Berechne DON-Umsatzraten                                                   */
/*                                                                                            */
/**********************************************************************************************/
/* geänderte Var.	pCL->fHumusToDONR                                                         */
/*					pCL->fLitterToDONR                                                        */
/*					pCL->fManureToDONR                                                        */
/*					pCL->fDONToHumusR                                                         */
/*					pCL->fDONToLitterR                                                        */
/*					pCL->fDONMinerR                                                           */
/**********************************************************************************************/
int transpn_get_DON_TransformationRates(transpn *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	
	PHEAT pHe = xpn->pHe ;
	
	PSPROFILE pSo = xpn->pSo;
	
	PWATER pWa = xpn->pWa;

	int iLayer        = 0;

	



	PWLAYER  pWL;
	PCLAYER  pCL;
	PSLAYER  pSL;
	PCPARAM  pPA;
	PHLAYER  pHL;
	PSWATER  pSW;

	struct ncorr corr= {(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1};

	

	/* 1. Variablen für die Reduktionsfunktionen*/
	double f1,wmin,wlow,whigh;
	//double f2,f3;

	pWL = pWa->pWLayer;
	pCL = pCh->pCLayer;
	pSL = pSo->pSLayer;
	pPA = pCh->pCParam;
	pHL = pHe->pHLayer;
	pSW = pSo->pSWater;
	
	/*if ((xpn->pTi->pSimTime->iyear==1983) && (xpn->pTi->pSimTime->fTimeY>=30.31999999988728))
		{
			printf("f");
		}*/

	for (iLayer=1,
	        pWL=pWa->pWLayer->pNext,
	        pCL=pCh->pCLayer->pNext,
	        pSL=pSo->pSLayer->pNext,
	        pSW = pSo->pSWater->pNext;
	        ((pWL->pNext!=NULL)&&
	         (pCL->pNext!=NULL)&&
	         (pSL->pNext!=NULL)&&
	         (pSW->pNext!=NULL));
	        pWL = pWL->pNext,
	        pCL = pCL->pNext,
	        pSL = pSL->pNext,
	        pSW = pSW->pNext,
	        iLayer++)
		{
			/* 1. Temperatur-Reduktionsfunktion*/
			/* Q10-Funktion */
			corr.Temp = abspowerDBL(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0));

			/* 2. Feuchte-Reduktionsfunktion*/
			/* Ansatz LeachN, verändert*/
			f1= (double)-153300.0;      //pF = 4.18 = PWP
			wmin = WATER_CONTENT(f1);
			f1= (double)-30660.0;       //pF = 3.49
			wlow = WATER_CONTENT(f1);
			whigh = MAX(wmin,(pSL->fPorosity - pPA->fMinerThetaMin));

			/* Original-Grenzwerte, beruhen auf Werten des volumetrischen Wassergehalts
			wmin  = pSW->fContPWP; //= PWP-Wert
			wlow  = wmin + (double)0.10;
			wmax  = pSL->fPorosity;
			whigh = wmax - (double)0.08; */

			corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce),
			                       wmin,(double)0,  wlow, (double)1 , whigh, (double)1,
			                       pSL->fPorosity, pPA->fMinerSatActiv);

			/* DON-Transformations-Raten (1/t) */
			pCL->fHumusToDONR  = pCL->fHumusToDONMaxR  * corr.Temp * corr.Feucht;// * pSL->fNHumus;
			pCL->fLitterToDONR = pCL->fLitterToDONMaxR  * corr.Temp * corr.Feucht;// * pCL->fNLitter;
			pCL->fManureToDONR = pCL->fManureToDONMaxR  * corr.Temp * corr.Feucht;// * pCL->fNManure;
			pCL->fDONToHumusR  = pCL->fDONToHumusMaxR  * corr.Temp * corr.Feucht;// * pCL->fDON;
			pCL->fDONToLitterR = pCL->fDONToLitterMaxR  * corr.Temp * corr.Feucht;// * pCL->fDON;
			pCL->fDONMinerR    = pCL->fDONMinerMaxR  * corr.Temp * corr.Feucht;

			// in transo.c, da dort auch die DOC-Mineralisierungsrate benoetigt wird
			//pCL->fDOMCN   = (pCL->fDON > EPSILON)? pCL->fDOC/pCL->fDON:(double)0.1;
			//f3     = (double)1 - pPA->fMinerEffFac * pCL->fDOMCN / pCL->fMicBiomCN;
			//pCL->fDONMinerR    *= f3;

		} /* for */

	/* Korrektur Schicht 0: */
	//pCh->pCLayer->fGasDiffCoef = pCh->pCLayer->pNext->fGasDiffCoef;


	return 1;
} /* Get_DON_TransformationRates */

/**********************************************************************************************/
/* Procedur    :   Get_Diffusion_Const()                                                      */
/* Beschreibung:   Berechne Diffusions-Koeffizienten                                          */
/*                                                                                            */
/**********************************************************************************************/
/* geänderte Var.	pCL->fGasDiffCoef                                                         */
/*					pCL->fLiqDiffCoef                                                         */
/**********************************************************************************************/
int transpn_get_Diffusion_Const(transpn *self)
{
	expertn_modul_base *xpn = &(self->parent);
        PCHEMISTRY pCh = xpn->pCh; 
        PSPROFILE pSo = xpn->pSo; 
        
        PWATER pWa = xpn->pWa;
	
	int iLayer        = 0;
	

	PWLAYER  pWL;
	PCLAYER  pCL;
	PSLAYER  pSL;

	double f1,f2;
	double fThetaMid, fGasMid, fGasVol;

	double DeltaZ = pSo->fDeltaZ;
	double fGDIF  = (double)11840;    /* estimated N2O  diffusion coeff. in air [cm^2 /d]
    									calculated with Fuller correlation (Perry, 1984) */


	for (iLayer=0,
	        pWL=pWa->pWLayer,
	        pCL=pCh->pCLayer,
	        pSL=pSo->pSLayer;
	        ((pWL->pNext!=NULL)&&
	         (pCL->pNext!=NULL)&&
	         (pSL->pNext!=NULL));
	        pWL = pWL->pNext,
	        pCL = pCL->pNext,
	        pSL = pSL->pNext,
	        iLayer++)
		{
			/* Gas - Diffusionskoeffizient -------------------------------------------- */

			fThetaMid = (pWL->pNext->fContAct+ pWL->pNext->fContOld
			             +  pWL->fContAct + pWL->fContOld)
			            / (double)4.0;

			fGasVol 	 = pSL->fPorosity
			               - (pWL->fIce + pWL->fIceOld + pWL->fContAct+ pWL->fContOld)
			               / (double)2;

			if (fGasVol < EPSILON)
				{
					fGasVol = (double)0;
				}

			fGasMid 	 = (fGasVol
			                + pSL->pNext->fPorosity
			                - (pWL->pNext->fIce + pWL->pNext->fIceOld + pWL->pNext->fContAct+ pWL->pNext->fContOld)
			                / (double)4)
			               / (double)2;


			/* Tortuositätsfaktor nach Millington und Quirk : */
			f1 = abspowerDBL(fGasVol,(double)3.330) / (pSL->fPorosity * pSL->fPorosity);


			pCL->fGasDiffCoef = fGDIF * f1;        /*[cm^2 / d] */

			f1 = fThetaMid * pCh->pCParam->fDispersivity  - (double)0.16 * DeltaZ;
			f1 = fThetaMid * ((double)-0.84+(double)1.264*pCh->pCParam->fDispersivity-(double)0.6458*DeltaZ);
			f2 = pWL->fFluxDens;
			if (f2 < (double)0.0)
				f2 *= (double)-1.0;

			f1 = MAX((double)0.0,f1) * f2 / fThetaMid;
			f2 = (double)exp((double)(pSL->fImpedLiqB * fThetaMid)); /* ch ?? Vorzeichen */

			pCL->fLiqDiffCoef = (pCh->pCParam->fMolDiffCoef * pSL->fImpedLiqA * f2 + f1)/(double)100;

			if ((iLayer == 0) && (pWL->fFluxDens == (double)0.0))
				pCL->fLiqDiffCoef = (double)0.0;

			/* Test: ohne Diffusion  !!! *-/
			pCL->fLiqDiffCoef = pCL->fGasDiffCoef = (double)0;  */
		} /* for */

	/* Korrektur Schicht 0: */
	pCh->pCLayer->fGasDiffCoef = pCh->pCLayer->pNext->fGasDiffCoef;


	return 1;
} /* Get_Diffusion_Const */


/**********************************************************************************************/
/* Procedur    :   Solve_LGS()                                                                */
/* Beschreibung:   Löser ...                                                                  */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var		pCL->fLiqDiffCoef                                                         */
/*					pCL->fGasDiffCoef                                                         */
/*					pCL->fNH4NSoilConc                                                        */
/*					pCL->fNO3NSoilConc                                                        */
/*					pCL->fN2ONGasConc                                                         */
/*					pCL->fUreaNSoilConc                                                       */
/**********************************************************************************************/
int transpn_solve_LGS(transpn *self)
{
	expertn_modul_base *xpn = &(self->parent);
        PCHEMISTRY pCh = xpn->pCh; 
        
        PSPROFILE pSo = xpn->pSo; 
        PTIME pTi = xpn->pTi; 
        PWATER pWa = xpn->pWa;
	
	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;
	

	PCLAYER pCL;
	PWLAYER pWL;
	PSLAYER pSL;

	double  *pA,*pB,*pC,*pD;
	double  *pAB,*pBB,*pCB,*pDB;
	double  *pF,*pG;

	int Stoff;

	double  DeltaZ = pSo->fDeltaZ;
	double f1,f2,f3,f4,fBeta1,fBeta2,fBeta3,fBeta4;
	double  fAG,fBG;
	double d1;

	/* -------------Speicher für Schichtvariable reservieren-------------- */

	pAB        = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pAB,0x0,(pSo->iLayers * sizeof(double)));
	pBB        = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pBB,0x0,(pSo->iLayers * sizeof(double)));
	pCB        = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pCB,0x0,(pSo->iLayers * sizeof(double)));
	pDB        = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pDB,0x0,(pSo->iLayers * sizeof(double)));


	pA          = (double *) malloc(pSo->iLayers  * sizeof(double));
	memset(pA,0x0,(pSo->iLayers * sizeof(double)));
	pB          = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pB,0x0,(pSo->iLayers * sizeof(double)));
	pC          = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pC,0x0,(pSo->iLayers * sizeof(double)));
	pD          = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pD,0x0,(pSo->iLayers * sizeof(double)));


	pF          = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pF,0x0,(pSo->iLayers * sizeof(double)));
	pG          = (double *) malloc(pSo->iLayers * sizeof(double));
	memset(pG,0x0,(pSo->iLayers * sizeof(double)));

	/*------------------ Speicher für Schichtvariable reserviert---------------- */


	/* Randbedingungen berichtigen  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

	/* Keine Diffusion in geloester Phase aus dem Profil */
	pCh->pCLayer->fLiqDiffCoef = (double)0.0; /* Schicht 0 */

	for (SOIL_LAYERS0(pCL,pCh->pCLayer));   /* Zeiger auf letzte Schicht */
//pCL->pBack->fLiqDiffCoef = pCL->pBack->fGasDiffCoef = (double)0.0;
	pCL->fLiqDiffCoef = pCL->fGasDiffCoef = (double)0.0;


	for (Stoff = 0; Stoff < 6; Stoff++)
		{

			f1 = (double)2.0 * (double)DeltaZ* (double)DeltaZ /(double)100;   /*15.4.96 ch da DiffCoeff in cm^2 /d */
			f2 = (double)2.0 * (double)DeltaZ;

			for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pWL, pWa->pWLayer->pNext))
				{
					pAB[iLayer] = (Stoff != 3) ? (double)(pCL->pBack->fLiqDiffCoef / f1) :
					              (double)((pCL->pBack->fN2OKh * pCL->pBack->fLiqDiffCoef + pCL->pBack->fGasDiffCoef) / f1);

					pBB[iLayer] = (Stoff != 3) ? (double)(pCL->fLiqDiffCoef / f1) :
					              (double)((pCL->fN2OKh * pCL->fLiqDiffCoef + pCL->fGasDiffCoef) / f1);


					/* ch 24.6.96 konvektiver Transport N2O: */
					pCB[iLayer] = (Stoff != 3)? (double)(pWL->pBack->fFluxDens / f2) :
					              (double)(pCL->fN2OKh * pWL->pBack->fFluxDens / f2);

					pDB[iLayer] = (Stoff != 3) ? (double)(pWL->fFluxDens / f2) :
					              (double)(pCL->fN2OKh * pWL->fFluxDens / f2);

				}
 
			//if (xpn->pWa->FreeD==TRUE)
			{
				pBB[pSo->iLayers-2] = (double)0;
			}


			/* Koeffizienten der tridiagonalen Matrix errechnen  */

			for (iLayer = 1, 
			        pSL = pSo->pSLayer->pNext,
			        pCL = pCh->pCLayer->pNext,
			        pWL = pWa->pWLayer->pNext; ((pSL->pNext!=NULL)&&
			                                    (pCL->pNext!=NULL)&&
			                                    (pWL->pNext!=NULL)); pSL = pSL->pNext,
			        pCL = pCL->pNext,
			        pWL = pWL->pNext,iLayer++)
				{
					if (pWL->fFluxDens > (double)0.0)
						{
							fBeta2 = (double)1.0;
							fBeta3 = (double)0.0;
						}
					else
						{
							fBeta2 = (double)0.0;
							fBeta3 = (double)1.0;
						}
					if (pWL->pBack->fFluxDens > (double)0.0)
						{
							fBeta1 = (double)1.0;
							fBeta4 = (double)0.0;
						}
					else
						{
							fBeta1 = (double)0.0;
							fBeta4 = (double)1.0;
						}

					/* Koeffizienten berechnen ------------------------------------------- */

					pA[iLayer] = (double)-1.0 * pAB[iLayer] - fBeta1 * pCB[iLayer];

					/*----------------- */

					pB[iLayer] = (Stoff != 3)?
					             (double)((pWL->fContAct
					                       + (pSL->fPorosity - pWL->fContAct - pWL->fIce) * pCh->pCParam->afKh[Stoff]
					                       + pSL->fBulkDens * pCh->pCParam->afKd[Stoff])
					                      / DeltaT)
					             :
					             (double)((pWL->fContAct * pCL->fN2OKh
					                       + pSL->fPorosity - pWL->fContAct - pWL->fIce)
					                      / DeltaT);

					if((xpn->pWa->iMobImm==1)&&(Stoff==5)) pB[iLayer] = (double)(pWL->fContMobAct/DeltaT);//pWL->fContAct/DeltaT;//
					//if((xpn->pWa->iMobImm==1)&&(Stoff==5)) pB[iLayer] = pWL->fContMobOld/DeltaT;//pWL->fContAct/DeltaT;//
					//if((xpn->pWa->iMobImm==1)&&(Stoff==5)) pB[iLayer] = (double)0.5*(pWL->fContMobAct+pWL->fContMobOld)/DeltaT;//pWL->fContAct/DeltaT;//


					pB[iLayer] += pAB[iLayer] + pBB[iLayer]
					              + fBeta2 * pDB[iLayer]
					              - fBeta4 * pCB[iLayer];

					/*----------------------- */

					pC[iLayer]= (double)-1.0 * pBB[iLayer] + fBeta3 * pDB[iLayer];
					/*---------------------- */

					f1 = pAB[iLayer] + fBeta1 * pCB[iLayer];

					f2 = (Stoff != 3)?
					     (double)((pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]
					               + (pSL->fPorosity - pWL->fContOld - pWL->fIceOld) * pCh->pCParam->afKh[Stoff])
					              / DeltaT)
					     :
					     (double)((pWL->fContOld * pCL->fN2OKh
					               + pSL->fPorosity - pWL->fContOld - pWL->fIceOld)
					              / DeltaT);

					if((xpn->pWa->iMobImm==1)&&(Stoff==5)) f2 = (double)(pWL->fContMobOld/DeltaT);//pWL->fContOld/DeltaT;//

					f2 -= pAB[iLayer] + fBeta2 * pDB[iLayer];
					f2 += fBeta4 * pCB[iLayer] - pBB[iLayer];

					f3 = pBB[iLayer] - fBeta3 * pDB[iLayer];

					switch(Stoff)
						{
						case 0:
							f4 = (double)((pCL->fUreaNSoilConcOld - pCL->fUreaNSoilConc)
							              * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);

							pD[iLayer] = (double)pCL->pBack->fUreaNSoilConcOld * f1
							             + (double)pCL->fUreaNSoilConcOld        * f2
							             - f4
							             + (double)pCL->pNext->fUreaNSoilConcOld * f3;
							break;

						case 1:
							f4 = (double)(pCL->fNH4NSoilConcOld - pCL->fNH4NSoilConc)
							     * (double)((pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);

							pD[iLayer] = (double)pCL->pBack->fNH4NSoilConcOld * f1
							             + (double)pCL->fNH4NSoilConcOld * f2
							             - f4
							             + (double)pCL->pNext->fNH4NSoilConcOld * f3;
							break;

						case 2:
							f4 = (double)((pCL->fNO3NSoilConcOld - pCL->fNO3NSoilConc)
							              * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);

							pD[iLayer]= (double)pCL->pBack->fNO3NSoilConcOld * f1
							            + (double)pCL->fNO3NSoilConcOld        * f2
							            - f4
							            + (double)pCL->pNext->fNO3NSoilConcOld * f3;
							break;

						case 3:
							f4 = (double)((pCL->fN2ONGasConcOld - pCL->fN2ONGasConc)
							              * pWL->fContOld * pCL->fN2OKh / DeltaT);

							pD[iLayer]= (double)pCL->pBack->fN2ONGasConcOld * f1
							            + (double)pCL->fN2ONGasConcOld        * f2
							            - f4
							            + (double)pCL->pNext->fN2ONGasConcOld * f3;
							break;

						case 4:
							f4 = (double)((pCL->fDONSoilConcOld - pCL->fDONSoilConc)
							              * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);

							pD[iLayer] = (double)pCL->pBack->fDONSoilConcOld * f1
							             + (double)pCL->fDONSoilConcOld      * f2
							             - f4
							             + (double)pCL->pNext->fDONSoilConcOld * f3;
							break;

						case 5:
							f4 = (double)((pCL->fDOCSoilConcOld - pCL->fDOCSoilConc)
							              * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);

							if(xpn->pWa->iMobImm==1)
								{
									pD[iLayer] =  (double)pCL->pBack->fDOCSltConcMobOld * f1
									              + (double)pCL->fDOCSltConcMobOld * f2
									              - f4
									              - (double)pCh->pCParam->fMobImmSltEC
									              *(double)(pCL->fDOCSltConcMobOld - pCL->fDOCSltConcImmOld)
									              //-(pWL->fContImmAct-pWL->fContImmOld)/DeltaT*pCL->fDOCSltConcMobOld
									              //-(double)0.5*(pWL->fContImmAct-pWL->fContImmOld)/DeltaT*pCL->fDOCSltConcMobOld
									              + (double)pCL->pNext->fDOCSltConcMobOld * f3;
								}
							else
								{
									pD[iLayer] = (double)pCL->pBack->fDOCSoilConcOld * f1
									             + (double)pCL->fDOCSoilConcOld * f2
									             - f4
									             + (double)pCL->pNext->fDOCSoilConcOld * f3;
								}

							break;
						} /*switch */


				}/* Schicht */

			/* Löser der Tridiagonalen Matrix   Obere RandSchicht                        */
			/*  Obere Randbedingung  --------------------------------------------------- */

			pF[1] = pC[1] / pB[1];

			switch(Stoff)
				{
				case 0:
					pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fUreaNSoilConcOld) / pB[1];
					break;

				case 1:
					pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fNH4NSoilConcOld) / pB[1];
					break;

				case 2:
					pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fNO3NSoilConcOld) / pB[1];
					break;

				case 3:
					pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fN2ONGasConcOld) / pB[1];
					break;

				case 4:
					pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fDONSoilConcOld) / pB[1];
					break;

				case 5:
					if(xpn->pWa->iMobImm==1)
						{
							pG[1] = (pD[1]-pA[1]*(double)pCh->pCLayer->fDOCSltConcMobOld) / pB[1];
						}
					else
						{
							pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fDOCSoilConcOld) / pB[1];
						}
					break;
				}

			/* Löser der Tridiagonalen Matrix   Profil - berechnung  */

			for (iLayer = 2; iLayer < pSo->iLayers-1; iLayer++)
				{
					pF[iLayer] = pC[iLayer]
					             / (pB[iLayer] - pF[iLayer-1] * pA[iLayer]);

					pG[iLayer] = (pD[iLayer] - pG[iLayer-1] * pA[iLayer])
					             / (pB[iLayer] - pF[iLayer-1] * pA[iLayer]);
				}

			/* Untere Randbedingung ---------------------------------------------------- */
			for (iLayer=0;iLayer!=pSo->iLayers;iLayer++)
			{
			C_DEBUG_LOC_IN_ARRAY(pG[iLayer],iLayer);
			}
 
			/* Zeiger auf letzte Listenelemente setzten */
			for (SOIL_LAYERS0(pCL, pCh->pCLayer->pNext));

			if (xpn->pWa->FreeD==FALSE)
				{
					//for (SOIL2_LAYERS0(pCL, pCh->pCLayer->pNext, pWL, pWa->pWLayer->pNext));

					pF[pSo->iLayers-2] = pC[pSo->iLayers-2] / (pB[pSo->iLayers-2] + (double)pCL->pBack->fLiqDiffCoef/f1 -
					                     pF[pSo->iLayers-3] * pA[pSo->iLayers-2]);
					pG[pSo->iLayers-2] = (pD[pSo->iLayers-2]
					                      - pG[pSo->iLayers-3]* pA[pSo->iLayers-2])// * pCh->pCLayer->fNO3NSoilConcOld))
					                     /(pB[pSo->iLayers-2]+ (double)pCL->pBack->fLiqDiffCoef/f1 - pF[pSo->iLayers-3] * pA[pSo->iLayers-2]);
				}

			//if (pWa->pWLayer->fFluxDens>(double)0)
			{
				/* anstelle der mixing cell ep 3.11.94 */
				pCL->fUreaNSoilConc = pCL->pBack->fUreaNSoilConcOld;
				pCL->fNH4NSoilConc  = pCL->pBack->fNH4NSoilConcOld;
				pCL->fNO3NSoilConc  = pCL->pBack->fNO3NSoilConcOld;
				pCL->fN2ONGasConc   = pCL->pBack->fN2ONGasConcOld;
				pCL->fDONSoilConc   = pCL->pBack->fDONSoilConcOld;
				pCL->fDOCSoilConc   = pCL->pBack->fDOCSoilConcOld;
				if(xpn->pWa->iMobImm==1) pCL->fDOCSltConcMob   = pCL->pBack->fDOCSltConcMobOld;
			}
			//else
			{
				//pCL->fUreaNSoilConc = (double)0;//pG[pSo->iLayers-2];
				//pCL->fNH4NSoilConc  = (double)0;//pG[pSo->iLayers-2];
				//pCL->fNO3NSoilConc  = (double)0;//pG[pSo->iLayers-2];
				//pCL->fN2ONGasConc   = (double)0;//pG[pSo->iLayers-2];
				//pCL->fDONSoilConc   = (double)0;//pG[pSo->iLayers-2];
				//pCL->fDOCSoilConc   = (double)0;//pG[pSo->iLayers-2];
			}

 
			/* Zeiger auf vorletzte Listenelemente setzten */
			/* Rückwärts einsetzen */

			for (iLayer = pSo->iLayers-2, pCL = pCL->pBack;
			        ((pCL->pBack!=NULL) && (iLayer > 0));
			        pCL = pCL->pBack, iLayer--)
				{
					switch(Stoff)
						{
						case 0:
							d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fUreaNSoilConc;
							pCL->fUreaNSoilConc = (double)MAX(0.0,d1);
							break;

						case 1:
							d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fNH4NSoilConc;
							pCL->fNH4NSoilConc = (double)MAX(0.0,d1);
							break;

						case 2:
							d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fNO3NSoilConc;
							pCL->fNO3NSoilConc = (double)MAX(0.0,d1);
							break;

						case 3:
							d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fN2ONGasConc;
							pCL->fN2ONGasConc = (double)MAX(0.0,d1);
							break;

						case 4:
							d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fDONSoilConc;
							pCL->fDONSoilConc = (double)MAX(0.0,d1);
							break;

						case 5:
							if(xpn->pWa->iMobImm==1)
								{
									d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fDOCSltConcMob;
									pCL->fDOCSltConcMob = (double)MAX(0.0,d1);
								}
							else
								{
									d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fDOCSoilConc;
									pCL->fDOCSoilConc = (double)MAX(0.0,d1);
								}
							break;
						}
				}
			 

			if((xpn->pWa->iMobImm==1)&&(Stoff==5))
				{
					for (iLayer=1,
					        pWL=pWa->pWLayer->pNext,
					        pCL=pCh->pCLayer->pNext;
					        ((pWL->pNext!=NULL)&&
					         (pCL->pNext!=NULL));
					        pWL = pWL->pNext,
					        pCL = pCL->pNext,
					        iLayer++)
						{
							fAG = pWL->fContImmOld/DeltaT - (double)0.5 *  pCh->pCParam->fMobImmSltEC*(double)3;
							fBG = pWL->fContImmAct/DeltaT - (double)0.5 *  pCh->pCParam->fMobImmSltEC;
							if (fBG==(double)0) fBG += (double)1e-9;
							pCL->fDOCSltConcImm = fAG/fBG * pCL->fDOCSltConcImmOld
							                      - (double)0.5*pCh->pCParam->fMobImmSltEC/fBG
							                      *(pCL->fDOCSltConcMob - pCL->fDOCSltConcMobOld*(double)3);

							pCL->fDOCSoilConc = (pWL->fContMobAct*pCL->fDOCSltConcMob + pWL->fContImmAct*pCL->fDOCSltConcImm)
							                    /pWL->fContAct;
							//pCL->fDOCSoilConc = (pWL->fContMobOld*pCL->fDOCSltConcMob + pWL->fContImmOld*pCL->fDOCSltConcImm)
							//                   /pWL->fContOld;
							//pCL->fDOCSoilConc = pCL->fDOCSltConcMob;
							//pCL->fDOCSoilConc = pCL->fDOCSltConcImm;
							//pCL->fDOCSoilConc = pCL->fDOCSltConcMob+pCL->fDOCSltConcImm;

						}
				}

		} /* for Stoff */


	/*------------------------------ Speicher für Schichtvariable Freigeben */
	free(pAB);
	free(pBB);
	free(pCB);
	free(pDB);

	free(pA);
	free(pB);
	free(pC);
	free(pD);

	free(pF);
	free(pG);

	return 1;
} /* Solve_LGS */


/**********************************************************************************************/
/* Procedur    :   NTransp                                                                    */
/*                                                                                            */
/* Inhalt      :   Stickstoffverlagerung nach Hutson & Wagenet (LeachN 1994-Version 3.0)      */
/*                                                                                            */
/* Autoren     :   GSF: S.Storm/E.Priesack                  V1.0                              */
/* Datum       :   24.05.94                                                                   */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var.		pCh->pCParam->afKd[0]                 pCh->pCParam->afKh[0]               */
/*					pCh->pCParam->afKd[1]                 pCh->pCParam->afKh[1]               */
/*					pCh->pCParam->afKd[2]                 pCh->pCParam->afKh[2]               */
/*					pCh->pCParam->fDispersivity           pCh->pCParam->fMolDiffCoef          */
/*					pCP->fN2OEmisR                        pCL->fN2OKh                         */
/*					pCL->fUreaN                           pCL->fNH4N                          */
/*					pCL->fNO3N                            pCL->fN2ON                          */
/*					pCL->fUreaNSoilConc                   pCL->fNH4NSoilConc                  */
/*					pCL->fNO3NSoilConc                    pCL->fN2ONGasConc                   */
/*					pCL->fUreaNSoilConcOld                pCL->fNH4NSoilConcOld               */
/*					pCL->fNO3NSoilConcOld                 pCL->fN2ONGasConcOld                */
/*					pSL->fImpedLiqA                       pSL->fImpedLiqB                     */
/*                  pCB->fNProfileStart                   pCB->fNInputCum                     */
/*                  pCP->fNO3NSurf                        pCP->fNH4NSurf                      */
/*                  pCP->fUreaNSurf                                                           */
/*                                                                                            */
/*					Get_Diffusion_Const()		pCL->fGasDiffCoef                             */
/*												pCL->fLiqDiffCoef                             */
/*					Solve_LGS()                 pCL->fLiqDiffCoef                             */
/*												pCL->fGasDiffCoef                             */
/*												pCL->fNH4NSoilConc                            */
/*												pCL->fNO3NSoilConc                            */
/*												pCL->fN2ONGasConc                             */
/*												pCL->fUreaNSoilConc                           */
/*                                                                                            */
/**********************************************************************************************/
int transpn_NTransp(transpn *self, int N2O_IN_SOLUTION)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	
	PHEAT pHe = xpn->pHe ;
	
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	
	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;





	PCLAYER    pCL;
	PCPROFILE  pCP ;
	PCBALANCE  pCB;

	PCPARAM    pPA;
	PHLAYER    pHL;
	PSLAYER    pSL;
	PWLAYER    pWL;
	PSWATER	   pSW;

	double DeltaZ = pSo->fDeltaZ;
	//double DeltaT = pTi->pTimeStep->fAct;
	double fGasMid;
	//double fPorosityOld;
	double f0;

	double     fDuengungAktuellNO3  = (double)0.0;
	double     fDuengungAktuellNH4  = (double)0.0;
	double     fDuengungAktuellUrea = (double)0.0;
	double     fDuengungAktuellDON  = (double)0.0;
	double     fDuengungAktuellDOC  = (double)0.0;

	pCP = pCh->pCProfile;
	pCB = pCh->pCBalance;
	pPA = pCh->pCParam;
	
	self->__ERROR=0;

	/* ************************* Kh-Wert fuer N2O berechnen. ************ */
	for ( SOIL2_LAYERS0(pCL, pCh->pCLayer->pNext, pHL, pHe->pHLayer->pNext))
		{
			pCL->fN2OKh = (N2O_IN_SOLUTION) ? Solut_Coeff(pHL->fSoilTemp) : (double)0.000001;
		}

//SG/07/05/99:	pCL != NULL durch pCL->pNext != NULL ersetzt!
	for (pCL = pCh->pCLayer; (pCL->pNext != NULL); pCL = pCL->pNext)
		{
			pCL->fUreaNSoilConcOld = pCL->fUreaNSoilConc;
			pCL->fNH4NSoilConcOld  = pCL->fNH4NSoilConc;
			pCL->fNO3NSoilConcOld  = pCL->fNO3NSoilConc;
			pCL->fN2ONGasConcOld   = pCL->fN2ONGasConc;
			pCL->fDONSoilConcOld = pCL->fDONSoilConc;
			pCL->fDOCSoilConcOld = pCL->fDOCSoilConc;
			if (xpn->pWa->iMobImm==1)
				{
					pCL->fDOCSltConcMobOld = pCL->fDOCSltConcMob;
					pCL->fDOCSltConcImmOld = pCL->fDOCSltConcImm;
				}
		}//for
///*
	if ((xpn->pWa->iMobImm==1)&&(SimStart(pTi)))//zu Testzwecken
		{
			for (pPA = pCh->pCParam; (pPA->pNext != NULL); pPA = pPA->pNext)
				{
					pPA->fMobImmSltEC=(double)0.05;
					pPA->fMobAdsSiteFrac=(double)1;
				}

			for (pCL = pCh->pCLayer; (pCL->pNext != NULL); pCL = pCL->pNext)
				{
					pCL->fDOCSltConcMobOld = (double)0;//iLayer;
					pCL->fDOCSltConcImmOld = (double)0;//iLayer;
					pCL->fDOCSltConcMob    = pCL->fDOCSltConcMobOld;
					pCL->fDOCSltConcImm    = pCL->fDOCSltConcImmOld;
				}

		}
//*/

	/*
	 * ch: lpHF->FDichte   > 1 um doubleing point error zu vermeiden
	 * 	if Abfrage.
	 */

	if ((pWa->pWLayer->fFluxDens>(double)1.0)&&
	        ((pCP->fNO3NSurf > EPSILON)||(pCP->fNH4NSurf > EPSILON)||
	         (pCP->fUreaNSurf > EPSILON)||(pCP->fDONSurf > EPSILON)||(pCP->fDOCSurf > EPSILON)))
		{

			pCL = pCh->pCLayer;         /*   0. Schicht  (virtuell) */
			pSL = pSo->pSLayer->pNext; /*   1. Schicht */
			pWL = pWa->pWLayer->pNext;

			pCL->fNO3NSoilConcOld  = pCP->fNO3NSurf  * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
			pCL->fNH4NSoilConcOld  = pCP->fNH4NSurf  * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
			pCL->fUreaNSoilConcOld = pCP->fUreaNSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
			pCL->fDONSoilConcOld   = pCP->fDONSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
			pCL->fDOCSoilConcOld   = pCP->fDOCSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
			if(xpn->pWa->iMobImm==1) pCL->fDOCSltConcMobOld = pCP->fDOCSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;

			fDuengungAktuellNO3  = pCL->fNO3NSoilConcOld * pWa->pWLayer->fFlux /kgPhaTomgPsqm;
			pCP->fNO3NSurf      -= fDuengungAktuellNO3;
			pCP->fNO3NSurf       = MAX((double)0.0,pCP->fNO3NSurf);

			fDuengungAktuellNH4  = pCL->fNH4NSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
			pCP->fNH4NSurf      -= fDuengungAktuellNH4;
			pCP->fNH4NSurf       = MAX((double)0.0,pCP->fNH4NSurf);

			fDuengungAktuellUrea = pCL->fUreaNSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
			pCP->fUreaNSurf     -= fDuengungAktuellUrea;
			pCP->fUreaNSurf      = MAX((double)0.0,pCP->fUreaNSurf);

			fDuengungAktuellDON = pCL->fDONSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
			pCP->fDONSurf     -= fDuengungAktuellDON;
			pCP->fDONSurf      = MAX((double)0.0,pCP->fDONSurf);

			fDuengungAktuellDOC = pCL->fDOCSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
			pCP->fDOCSurf     -= fDuengungAktuellDOC;
			pCP->fDOCSurf      = MAX((double)0.0,pCP->fDOCSurf);

		} //fertilizer application

 

	transpn_get_DON_TransformationRates(self);
 
	for (iLayer = 1,
	        pCL = pCh->pCLayer->pNext,
	        pWL = pWa->pWLayer->pNext,
	        pSL = pSo->pSLayer->pNext;
	        ((pCL->pNext !=NULL)&&
	         (pWL->pNext !=NULL)&&
	         (pSL->pNext !=NULL)); //ep 200899 auf ->pNext !=NULL gesetzt, da fuer letzte virtuelle Schicht sinnlos?
	        iLayer++,				//			und es tritt Fehler bei TestRange(Porosity - Water) auf!
	        pCL = pCL->pNext,
	        pWL = pWL->pNext,
	        pSL = pSL->pNext)
		{

			pCL->fNH4N+= pCL->fDONMinerR * pCL->fDON * DeltaT;
			//pCL->fNH4N+= MAX((double)0,pCL->fDONMINerR*pCL->fDON);

			pCL->fDOMCN   = (pCL->fDON > EPSILON)? pCL->fDOC/pCL->fDON:(double)0.1;
			f0 = (double)1 - pPA->fMinerEffFac * pCL->fDOMCN / pCL->fMicBiomCN;

			pCL->fDON += (pCL->fHumusToDONR*pSL->fNHumus + pCL->fLitterToDONR*pCL->fNLitter + pCL->fManureToDONR*pCL->fNManure
			              - (pCL->fDONToHumusR + pCL->fDONToLitterR + pCL->fDONMinerR*f0)*pCL->fDON)*DeltaT;
			//pCL->fDOC += (pCL->fHumusToDONR*pSL->fCHumus + pCL->fLitterToDONR*pCL->fCLitter + pCL->fManureToDONR*pCL->fCManure
			//            - (pCL->fDONToHumusR + pCL->fDONToLitterR + pCL->fDONMINerR)*pCL->fDOC)*DeltaT;

			pSL->fNHumus  += (pCL->fDONToHumusR*pCL->fDON - pCL->fHumusToDONR*pSL->fNHumus)*DeltaT;
			pSL->fCHumus  += (pCL->fDONToHumusR*pCL->fDOC - pCL->fHumusToDONR*pSL->fCHumus)*DeltaT;
			pCL->fNLitter += (pCL->fDONToLitterR*pCL->fDON - pCL->fLitterToDONR*pCL->fNLitter)*DeltaT;
			pCL->fCLitter += (pCL->fDONToLitterR*pCL->fDOC - pCL->fLitterToDONR*pCL->fCLitter)*DeltaT;
			pCL->fNManure -= pCL->fManureToDONR*pCL->fNManure*DeltaT;										
			pCL->fCManure -= pCL->fManureToDONR*pCL->fCManure*DeltaT;
				
			pCL->fUreaNSoilConc = pCL->fUreaN * (kgPhaTomgPsqm / (DeltaZ*
			                                     (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[0])));
			/*[kg N /ha /100 /mm] = kg N /m^2 10^-4 /100 /m 1000] */
			/* = g N /m^3 = mg N /l */
			pCL->fNH4NSoilConc  = pCL->fNH4N  * (kgPhaTomgPsqm / (DeltaZ*
			                                     (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[1])));
			pCL->fNO3NSoilConc  = pCL->fNO3N  * (kgPhaTomgPsqm / (DeltaZ*
			                                     (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[2])));
												 
			/*if (CHECK_DOUBLE_INVALID(pCL->fNO3NSoilConc))
				{
					printf("gf");
				}*/
			pCL->fDONSoilConc   = pCL->fDON  * (kgPhaTomgPsqm / (DeltaZ*
			                                    (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[4])));
			pCL->fDOCSoilConc   = pCL->fDOC  * (kgPhaTomgPsqm / (DeltaZ*
			                                    (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[5])));

			if (!Test_Range(xpn,pSL->fPorosity - pWL->fContOld - pWL->fIceOld, EPSILON, (double)1, "(Porosity - Water)"))
				{
					pSL->fPorosity = pWL->fContOld + pWL->fIceOld + (double)0.1 * pSL->fPorosity;
					
					
					
					//fPorosityOld = pWL->fContOld + pWL->fIceOld + (double)0.1 * pSL->fPorosity;
					//fPorosityOld = pWL->fContOld + pWL->fIceOld;
					//return -1;
				}
			/*
			        else
				    {
					fPorosityOld = pSL->fPorosity;
				    }
			*/

			pCL->fN2ONGasConc   = pCL->fN2ON  * kgPhaTomgPsqm / (DeltaZ*
			                      //(pWL->fContOld * pCL->fN2OKh + (fPorosityOld - pWL->fContOld - pWL->fIceOld)));
			                      (pWL->fContOld * pCL->fN2OKh + (pSL->fPorosity - pWL->fContOld - pWL->fIceOld)));
			//(pWL->fContAct * pCL->fN2OKh + (pSL->fPorosity - pWL->fContAct - pWL->fIce)));

			//  if (SimStart(pTi))
			pCL->fN2ONGasConcOld = pCL->fN2ONGasConc;
		}
 
	/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	 *		Bestimme Koeffizienten fuer Diffusion
	 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
	transpn_get_Diffusion_Const(self);
 
	/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	 *		Loese lin. Gleichungssystem
	 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
	transpn_solve_LGS(self);

 

	/* Umrechnung der Konzentrationen */

	/* alle  Schichten  */
	for ( pCL = pCh->pCLayer,
	        pWL = pWa->pWLayer,
	        pSL = pSo->pSLayer;
	        ((pCL !=NULL)&&(pWL !=NULL)&&(pSL !=NULL));
	        pCL = pCL->pNext,
	        pWL = pWL->pNext,
	        pSL = pSL->pNext)
		{
			pCL->fUreaN = pCL->fUreaNSoilConc * DeltaZ *
			              (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[0])/(double)100.0;
			pCL->fNH4N  = pCL->fNH4NSoilConc  * DeltaZ *
			              (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[1])/(double)100.0;
			pCL->fNO3N  = pCL->fNO3NSoilConc  * DeltaZ *
			              (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[2])/(double)100.0;
	  
			/*if (CHECK_DOUBLE_INVALID(pCL->fNO3N))
				{
					printf("gf");
				}*/
			/* ch ? */
			pCL->fN2ON  = pCL->fN2ONGasConc   * DeltaZ *
			              //(pWL->fContAct * pCL->fN2OKh + fPorosityOld - pWL->fContAct - pWL->fIce)
			              (pWL->fContAct * pCL->fN2OKh + pSL->fPorosity - pWL->fContAct - pWL->fIce)
			              / kgPhaTomgPsqm;

			pCL->fDON = pCL->fDONSoilConc * DeltaZ *
			            (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[4])/(double)100.0;
			pCL->fDOC = pCL->fDOCSoilConc * DeltaZ *
			            (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[5])/(double)100.0;
						
			CHECK_VALID(pCL->fUreaN)
			CHECK_VALID(pCL->fNH4N)		
			CHECK_VALID(pCL->fNO3N)
			CHECK_VALID(pCL->fN2ON)
			CHECK_VALID(pCL->fDON)
			CHECK_VALID(pCL->fDOC)	

			
			
		}

	///*
	for(pSW=pSo->pSWater,
	        pSL=pSo->pSLayer;
	        ((pSW!=NULL)&&(pSL!=NULL));
	        pSW=pSW->pNext,
	        pSL=pSL->pNext)
		{
			pSL->fPorosity=pSW->fContSat;
		}
	//*/
 
	/* ch 30.4.96 */
	/* Obere RB für gelöste Stoffe: q < 0 */
	if (pWa->pWLayer->fFlux < (double)0.0)
		{
			pWL = pWa->pWLayer;          /* Fluss in 1. oder 0. Schicht ? */
			pCL = pCh->pCLayer->pNext;

			pCL->fUreaN -= pWL->fFlux * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
			               / (double)2.0 / kgPhaTomgPsqm;

			pCL->fNO3N  -= pWL->fFlux * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
			               / (double)2.0 / kgPhaTomgPsqm;

			pCL->fNH4N  -= pWL->fFlux * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
			               / (double)2.0 / kgPhaTomgPsqm;

			pCL->fN2ON  -= pWL->fFlux * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc)
			               *  pCL->fN2OKh
			               / (double)2.0 / kgPhaTomgPsqm;

			pCL->fDON -= pWL->fFlux * (pCL->fDONSoilConcOld + pCL->fDONSoilConc)
			             / (double)2.0 / kgPhaTomgPsqm;
			pCL->fDOC -= pWL->fFlux * (pCL->fDOCSoilConcOld + pCL->fDOCSoilConc)
			             / (double)2.0 / kgPhaTomgPsqm;
						 
			CHECK_VALID(pCL->fUreaN)
			CHECK_VALID(pCL->fNH4N)			
			CHECK_VALID(pCL->fNO3N)
			CHECK_VALID(pCL->fN2ON)
			CHECK_VALID(pCL->fDON)
			CHECK_VALID(pCL->fDOC)
			
		} /* q(i=0) < 0 */


	/* ******************************************************************************/
	/*  Bilanzierung */
	/* ******************************************************************************/

	pPA = pCh->pCParam;
	pCL = pCh->pCLayer;
	pCP = pCh->pCProfile;
	pCB = pCh->pCBalance;
	pWL = pWa->pWLayer;
	pSL = pSo->pSLayer;

	/* 1. Oberer Rand:	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
	fGasMid = pSL->pNext->fPorosity
	          - (pWL->pNext->fContAct+ pWL->pNext->fContOld + pWL->pNext->fIce + pWL->pNext->fIceOld)
	          / (double)2;

	if (fGasMid > EPSILON)
		pCP->fN2OEmisR = (pCL->fGasDiffCoef + pCL->pNext->fGasDiffCoef) / (double)2
		                 * (pCL->pNext->fN2ONGasConc - pCL->fN2ONGasConc
		                    +  pCL->pNext->fN2ONGasConcOld - pCL->fN2ONGasConcOld) / (double)2
		                 * (double)1e5 / (double)24 / DeltaZ ;
	/* [cm^2 /d * mg N /l /mm /1e5 * 24  ]
	=  [e-4 m^2 /24 /h *e3 mu g *e3 /m^3 *e3 /m e-5 24]
	=  [ mu g N /m^2 /h] */

	pCP->fNOEmisR = (double)0;
	pCP->fN2EmisR = (double)0;

	//pCP->fNFertilizerR existiert noch nicht !!!
	//pCP->fNFertilizerR = (fDuengungAktuellNO3 + fDuengungAktuellNH4 + fDuengungAktuellUrea) / DeltaT;

	if ((fDuengungAktuellNO3 > EPSILON)|| (fDuengungAktuellNH4 > EPSILON)||
	        (fDuengungAktuellUrea > EPSILON))
		{
			pCB->dNInputCum += (double)(fDuengungAktuellNO3 + fDuengungAktuellNH4 + fDuengungAktuellUrea);

		}


	if (pCP->fN2OEmisR > 0)
		{
			// pCP->fN2OEmisCum += pCP->fN2OEmisR  * (double)24e-5 * DeltaT;   dm, ist in cum_bil1.c
		}
	else
		{
			pCB->dNInputCum  -= (double)pCP->fN2OEmisR * (double)24e-5 * (double)DeltaT;
		}
	/* [µg N / qm / h / 24 * 10^5 * d] = [kg N/ha] */


	if (fDuengungAktuellDON > EPSILON)
		{
			//pCB->fDONInputCum += fDuengungAktuellDON;
		}


	/*******************************  Leaching ***************************************/
 
	for (pCL = pCh->pCLayer, pWL = pWa->pWLayer;
	        ((pCL->pNext->pNext!=NULL)&&(pWL->pNext->pNext!=NULL));
	        //((pCL->pNext!=NULL)&&(pWL->pNext!=NULL));
	        pCL = pCL->pNext, pWL = pWL->pNext); // Zeiger auf vorletzte Schicht setzen

	if ((xpn->pWa->FreeD==TRUE)&&(pWL->fFlux > (double)0.0)) //pWa->fPercolR*DeltaT //???
		{
			pCP->fUreaLeachDay += pWL->fFlux
			                      * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
			                      / (double)2.0 / kgPhaTomgPsqm;
			pCP->fNH4LeachDay  += pWL->fFlux
			                      * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
			                      / (double)2.0 / kgPhaTomgPsqm;
			pCP->fNO3LeachDay  += pWL->fFlux
			                      * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
			                      / (double)2.0 / kgPhaTomgPsqm;
			// Variable noch nicht vorhanden
			//pCP->fN2ODrainDay  += pWL->fFlux
			//                               * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc)
			//                               * pCL->fN2OKh
			//                               / (double)2.0 / kgPhaTomgPsqm;

			pCP->fDONLeachDay += pWL->fFlux
			                     * (pCL->fDONSoilConcOld + pCL->fDONSoilConc)
			                     / (double)2.0 / kgPhaTomgPsqm;
			pCP->fDOCLeachDay += pWL->fFlux
			                     * (pCL->fDOCSoilConcOld + pCL->fDOCSoilConc)
			                     / (double)2.0 / kgPhaTomgPsqm;

			pCP->dUreaLeachCum += (double)(pWL->fFlux
			                               * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
			                               / (double)2.0 / kgPhaTomgPsqm);
			pCP->dNH4LeachCum  += (double)(pWL->fFlux
			                               * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
			                               / (double)2.0 / kgPhaTomgPsqm);
			pCP->dNO3LeachCum  += (double)(pWL->fFlux
			                               * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
			                               / (double)2.0 / kgPhaTomgPsqm);
			pCP->dN2ODrainCum  += (double)(pWL->fFlux
			                               * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc)
			                               * pCL->fN2OKh
			                               / (double)2.0 / kgPhaTomgPsqm);

			pCP->dDONLeachCum += (double)(pWL->fFlux
			                              * (pCL->fDONSoilConcOld + pCL->fDONSoilConc)
			                              / (double)2.0 / kgPhaTomgPsqm);
			pCP->dDOCLeachCum += (double)(pWL->fFlux
			                              * (pCL->fDOCSoilConcOld + pCL->fDOCSoilConc)
			                              / (double)2.0 / kgPhaTomgPsqm);
										  
			CHECK_VALID(pCP->fUreaLeachDay)
			CHECK_VALID(pCP->fDOCLeachDay)			
			CHECK_VALID(pCP->dUreaLeachCum)
			CHECK_VALID(pCP->dNH4LeachCum)
			CHECK_VALID(pCP->dNO3LeachCum)
			CHECK_VALID(pCP->dN2ODrainCum)
			CHECK_VALID(pCP->dDONLeachCum)
			CHECK_VALID(pCP->dDOCLeachCum)			
		}
	else
		{
			pCP->fUreaLeachDay += pWa->fPercolR * DeltaT * (pCL->pBack->fUreaNSoilConc+pCL->fUreaNSoilConc)/(double)2
			                      / kgPhaTomgPsqm;
			pCP->fNH4LeachDay  += pWa->fPercolR * DeltaT * (pCL->pBack->fNH4NSoilConc+pCL->fNH4NSoilConc)/(double)2
			                      / kgPhaTomgPsqm;
			pCP->fNO3LeachDay  += pWa->fPercolR * DeltaT * (pCL->pBack->fNO3NSoilConc+pCL->fNO3NSoilConc)/(double)2
			                      / kgPhaTomgPsqm;
			///*
			pCP->dUreaLeachCum += (double)(pWa->fPercolR*DeltaT*(pCL->pBack->fUreaNSoilConc+pCL->fUreaNSoilConc)/(double)2/kgPhaTomgPsqm
			                               +pCL->pBack->fLiqDiffCoef*DeltaT *(pCL->pBack->fUreaNSoilConc-pCL->fUreaNSoilConc)/DeltaZ);
			pCP->dNH4LeachCum  += (double)(pWa->fPercolR*DeltaT*(pCL->pBack->fNH4NSoilConc+pCL->fNH4NSoilConc)/(double)2/kgPhaTomgPsqm
			                               +pCL->pBack->fLiqDiffCoef*DeltaT *(pCL->pBack->fNH4NSoilConc-pCL->fNH4NSoilConc)/DeltaZ);
			pCP->dNO3LeachCum  += (double)(pWa->fPercolR*DeltaT*(pCL->pBack->fNO3NSoilConc+pCL->fNO3NSoilConc)/(double)2/kgPhaTomgPsqm
			                               +pCL->pBack->fLiqDiffCoef*DeltaT *(pCL->pBack->fNO3NSoilConc-pCL->fNO3NSoilConc)/DeltaZ);
			//*/
		}
		
	
	 
	return RET_SUCCESS;

} /* NVerlagerung */
