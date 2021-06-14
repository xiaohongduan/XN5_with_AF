#include "ceres_transpiration.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


double ceres_PotentialTranspiration_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PWATER pWa = xpn->pWa;
	//double fPotPlantTransp,fTrsRatio;

	double fPotTr;

	//printf("%f %f \n", self->__weather.fEvapPotDay, self->weather.fEvapPotDay);

	/* daily potential transpiration */
	fPotTr = self->__weather.fPotETDay	 - self->__weather.fEvapPotDay;

	//pPl->pPltWater->fPotTranspDay = fPotTr;
	self->weather.fPotTraDay = fPotTr;

	/* time step potential transpiration */
	//pPl->pPltWater->fPotTranspdt = pWa->fPotETdt - pWa->pEvap->fPotR * DeltaT;

	return fPotTr;


	/*
	        fTrsRatio=(double)1.0;//TranspirationRatio(lpCtr,lpClm,lpPlt,lpSoil,lpMan);


	        if (pPl->pCanopy->fLAI<=(double)3.0)
	                fPotPlantTransp = pWa->fPotETDay*
	                                                (double)(1.0-exp(-pPl->pCanopy->fLAI))*fTrsRatio;
	        else
	                fPotPlantTransp = pWa->fPotETDay * fTrsRatio;

	        if (pWa->pEvap->fActDay > (double)0)
	        {
	        if (fPotPlantTransp+pWa->pEvap->fActDay>pWa->fPotETDay*fTrsRatio)
	                fPotPlantTransp=pWa->fPotETDay*fTrsRatio-pWa->pEvap->fActDay;
	        }
	        else
	        {
	        if (fPotPlantTransp+pWa->pEvap->fPotDay>pWa->fPotETDay*fTrsRatio)
	                fPotPlantTransp=pWa->fPotETDay*fTrsRatio-pWa->pEvap->fPotDay;
	        }

	        if (fPotPlantTransp<(double)0.01)
	                fPotPlantTransp=(double)0.01;



	    //pPl->pPltWater->fPotTranspDay=fPotPlantTransp;


	        return fPotPlantTransp;
	        */
}

int ceres_ActualTranspiration_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	int   L;
	double TRWU,WUF,TLL,RWUMX,fExtWater,fTotSoilWater,fThickness,fContAct;
	double DUL, fRAT, fPAW;

	PSLAYER                 pSL             =pSo->pSLayer;
	PSWATER                 pSWL    =pSo->pSWater;
	PWLAYER                 pSLW    =pWa->pWLayer;
	PLAYERROOT              pLR             =pPl->pRoot->pLayerRoot;
	PPLTWATER           pPltW   =pPl->pPltWater;


	//RWUMX =(double)0.03;
	fThickness = 0.0;
	RWUMX =pPl->pGenotype->fMaxWuptRate;


	pLR     =pPl->pRoot->pLayerRoot;

	for (L=1; L<=pSo->iLayers-2; L++) {
		pLR->fActWatUpt         =(double)0.0;
		pLR->fActLayWatUpt      =(double)0.0;
		pLR->fPotWatUpt         =(double)0.0;
		pLR->fPotLayWatUpt      =(double)0.0;

		pLR =pLR ->pNext;
	}

//      if (pPltW->fPotTranspDay<=(double)0.001)
//              {
//        pPltW->fActTranspDay  =(double)0.0;
//      pPltW->fStressFacPhoto  = (double)1.0;
//              pPltW->fStressFacLeaf   = (double)1.0;
//              return 0;
//              }

	if (self->weather.fPotTraDay<=(double)0.001) {
		self->weather.fActTraDay      =(double)0.0;
		pPltW->fStressFacPhoto  = (double)1.0;
		pPltW->fStressFacLeaf   = (double)1.0;
		return 0;
	}

	//=======================================================================================
	//                      Root Water Uptake
	//=======================================================================================
	L         = 1;
	TRWU  =(double)0.0;
	pSL     =pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext;
	pLR     =pPl->pRoot->pLayerRoot;

	//ep 130899: (... || LR->pNext...))
	while (((pLR->fLengthDens!=(double)0.0)||(pLR->pNext->fLengthDens !=(double)0.0))
	      &&(L<pSo->iLayers-2)) {
		fThickness = (double)0.1*pSL->fThickness; //mm to cm
		
		//Root water uptake per unit root length
		fExtWater = max((double)0.0,pSLW->fContAct-pSWL->fContPWP);

		if (pSLW->fContAct<=pSWL->fContPWP)
			pLR->fPotWatUpt=(double)0.0;
		else {
			/*      pLR->fPotWatUpt=(double)(2.67*1E-3*exp(min(62.0*(double)fExtWater,10.0))
			                                                                /(6.68-log((double)pLR->fLengthDens)));
			*/
			//INRA-Ceres:

			/*if (L==23)
			printf("%d %f %f %f %f \n", L, pLR->fPotWatUpt, fExtWater, pLR->fLengthDens, log(pLR->fLengthDens));
			if (L==24)
				printf("Test \n")*/;


			pLR->fPotWatUpt=(double)(2.67*1E-3*exp(min(35.0*(double)fExtWater,10.0))
			                         /(6.68-log((double)pLR->fLengthDens)));

			pLR->fPotWatUpt=min(RWUMX,pLR->fPotWatUpt);
		}

		//Root water uptake per layer
		pLR->fPotLayWatUpt=pLR->fPotWatUpt*fThickness*pLR->fLengthDens*(double)10.0
		                   *((double)0.18+(double)0.00272*(pLR->fLengthDens-(double)18.0)
		                     *(pLR->fLengthDens-(double)18.0));
		if ((pPl->pDevelop->fCumPhyll<(double)4.0)&&(pPl->pCanopy->fLAI<(double)1.0))
		{
			//printf("factor %d %f %f %f \n", xpn->pTi->pSimTime->mday, pPl->pDevelop->fCumPhyll, pPl->pCanopy->fLAI, (double)3.0-(double)2.0*pPl->pCanopy->fLAI);
			pLR->fPotLayWatUpt *= ((double)3.0-(double)2.0*pPl->pCanopy->fLAI);
		}
		
		TRWU += pLR->fPotLayWatUpt;
		L        ++;

		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pLR =pLR ->pNext;
	}

	pPl->pPltWater->fPotUptakedt=TRWU;
	//=======================================================================================
	//                      Root Water Uptake Modification  and Soil Water Content
	//=======================================================================================

//      if (pPltW->fPotTranspDay<=TRWU)
//      WUF = pPltW->fPotTranspDay/TRWU;

	if (self->weather.fPotTraDay<=TRWU)
		WUF = self->weather.fPotTraDay/TRWU;
	else
		WUF = (double)1.0;

	TLL=(double)0.0;
	DUL=(double)0.0;

	fTotSoilWater=(double)0.0;

	pPl->pPltWater->fActUptakedt=(double)0.0;

	pSL     =pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext;
	pLR     =pPl->pRoot->pLayerRoot;
	for (L=1; L<=pSo->iLayers-2; L++) {
		pLR->fActWatUpt         =  pLR->fPotWatUpt *WUF;
		pLR->fActLayWatUpt      =  pLR->fPotLayWatUpt*WUF;

		fContAct  = pSLW->fContAct;
		fContAct -= pLR->fActLayWatUpt/pSL->fThickness; //mm/mm

		if (fContAct<pSWL->fContPWP) {
			pLR->fActWatUpt    *= fContAct/pSWL->fContPWP;
			pLR->fActLayWatUpt *= fContAct/pSWL->fContPWP;
//                      pSLW->fContAct=pSWL->fContPWP;
		}

		fTotSoilWater   += fContAct*fThickness*(double)10.0;

		pPl->pPltWater->fActUptakedt += pLR->fActLayWatUpt;
		TLL   += pSWL->fContPWP*fThickness*(double)10.0;
		DUL   += pSWL->fContFK*fThickness*(double)10.0;

		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pLR =pLR ->pNext;
	}


//    if (pPltW->fPotTranspDay>=TRWU)
//        pPltW->fActTranspDay=TRWU;
//    else
//        pPltW->fActTranspDay=pPltW->fPotTranspDay;
//
//      pPl->pRoot->fUptakeR = pPltW->fActTranspDay;


	if (self->weather.fPotTraDay>=TRWU)
		pPltW->fActTranspR = TRWU;
	//	self->weather.fActTraDay=TRWU;
	else
		pPltW->fActTranspR = self->weather.fPotTraDay;
	//	self->weather.fActTraDay=self->weather.fPotTraDay;

	//pPl->pRoot->fUptakeR = fActTraDay;
	pPl->pRoot->fUptakeR = pPl->pPltWater->fActUptakedt;


	//=======================================================================================
	//      Soil Water Deficit Factors
	//=======================================================================================
//    if (pPl->pPltWater->fPotUptakedt/pPl->pPltWater->fPotTranspdt<(double)1.5)
//        pPl->pPltWater->fStressFacLeaf = ((double)0.67)*pPl->pPltWater->fPotUptakedt/pPl->pPltWater->fPotTranspdt;
	if (pPl->pPltWater->fPotUptakedt/self->weather.fPotTraDay<(double)1.5)
		pPl->pPltWater->fStressFacLeaf = ((double)0.67)*pPl->pPltWater->fPotUptakedt/self->weather.fPotTraDay;
	else
		pPl->pPltWater->fStressFacLeaf = (double)1.0;

	//INRA-Ceres:
	if(pPl->pDevelop->iStageCERES>=4) {
		if (pPl->pPltWater->fPotUptakedt/self->weather.fPotTraDay+(double)0.25<(double)1.5)
			pPl->pPltWater->fStressFacLeaf = (double)0.67*(pPl->pPltWater->fPotUptakedt/self->weather.fPotTraDay+(double)0.25);
		else
			pPl->pPltWater->fStressFacLeaf = (double)1.0;
	}


//    if (pPl->pPltWater->fPotTranspdt>=pPl->pPltWater->fPotUptakedt)
//        pPl->pPltWater->fStressFacPhoto       =pPl->pPltWater->fPotUptakedt/pPl->pPltWater->fPotTranspdt;
	if (self->weather.fPotTraDay>=pPl->pPltWater->fPotUptakedt)
		pPl->pPltWater->fStressFacPhoto =pPl->pPltWater->fPotUptakedt/self->weather.fPotTraDay;
	else
		pPl->pPltWater->fStressFacPhoto =(double)1.0;

	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		fPAW = (double)max(0.0,(fTotSoilWater - TLL)/(DUL - TLL));

		if(pWa->fPotETDay > (double)0.0)
			fRAT = (double)10.0*fPAW/pWa->fPotETDay;
		else
			fRAT = (double)1.0;

		fRAT = (double)min(1.0,fRAT);

		if((pPl->pPltWater->fStressFacPhoto < (double)1.0)&&
		   (fRAT > (double)0.5*pPl->pPltWater->fStressFacPhoto))
			pPl->pPltWater->fStressFacLeaf = (double)0.5*pPl->pPltWater->fStressFacPhoto;
		else
			pPl->pPltWater->fStressFacLeaf = fRAT;
	}



//  pPl->pPltWater->fShortage=pPl->pPltWater->fPotTranspdt-pPl->pPltWater->fActTranspdt;
	pPl->pPltWater->fShortage=self->weather.fPotTraDay-pPltW->fActTranspR;

//      pPl->pPltWater->fCumDemand   += pPl->pPltWater->fPotTranspdt;
	pPl->pPltWater->fCumShortage += pPl->pPltWater->fShortage;


	//eigentlich:   if (pPl->pDevelop->iStageCERES<2)
	//Test:
//      if (pPl->pDevelop->iStageCERES<2)
//              {
//              pPl->pPltWater->fStressFacPhoto         =(double)1.0;
//              pPl->pPltWater->fStressFacLeaf          =(double)1.0;
//              pPl->pPltWater->fStressFacTiller        =(double)1.0;
//              pPl->pPltNitrogen->fNStressPhoto        =(double)1.0;
//              pPl->pPltNitrogen->fNStressLeaf         =(double)1.0;
//              pPl->pPltNitrogen->fNStressTiller       =(double)1.0;
//
//              }

	return RET_SUCCESS;
}

int ceres_PlantWaterStress_run(ceres *self)
{
	return RET_SUCCESS;
}
