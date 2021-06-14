//      spass_nitrogen.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_nitrogen.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>	
#include <math.h>
#include <string.h>

int spass_nitrogen_capture(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PPLANT		pPl = xpn->pPl;

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		return 0;

	spass_NitrogenConcentrationLimits	(self);
	spass_nitrogen_demand				(self);
	spass_nitrogen_uptake				(self);
	spass_PlantNitrogenStress			(self);

    return RET_SUCCESS;
  }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	NitrogenUptake_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int spass_nitrogen_uptake(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PSPROFILE		pSo		=xpn->pSo;
	PWATER			pWa		=xpn->pWa;
	PCHEMISTRY		pCh		=xpn->pCh;
	PPLANT			pPl		=xpn->pPl;

	PSLAYER 		pSL		=pSo->pSLayer;
	PSWATER			pSWL	=pSo->pSWater;
	PWLAYER	   		pSLW	=pWa->pWLayer; 
	PCLAYER     	pSLN	=pCh->pCLayer;
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;

	PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;

	PTIME			pTi		=xpn->pTi;
	PTIMESTEP		pTS		=pTi->pTimeStep;

    int		L,L1;
	//double 	fTotRtLenDens;  
	double	fANDEM,fFNO3,fFNH4,fSMDFR,fRFAC,fNUF,fTRNU;//,fTRNLOS;
    double 	fUNO3,fUNH4,SNO3min,SNH4min,RNO3U[MAXSCHICHT],RNH4U[MAXSCHICHT];
	//double	ammonium = (double)0.0;

	//C_DEBUG(self->fNUF);
	//C_DEBUG(self->fLeafRatio);
	//C_DEBUG(self->fRootRatio);
	//C_DEBUG(self->fStemRatio);
	//C_DEBUG(self->fTRNU_zwischen);
	
	L1 = 1;

	if ((pPl->pDevelop->iDayAftEmerg > 0) && (PLANT_IS_GROWING))
	{

	//if (self->vSinHeight==0)
	//	return RET_SUCCESS;

	//======================================================================================
	//Layer Nitrogen Initiation and Transformation
	//======================================================================================
	//fTotRtLenDens=(double)0.0;
    //pLR=pPl->pRoot->pLayerRoot;

	for (L=1;L<=pSo->iLayers-2;L++)
		{
		RNO3U[L]=(double)0.0;
		RNH4U[L]=(double)0.0;

		//fTotRtLenDens += pLR->fLengthDens;

		pLR->fActLayNO3NUpt=(double)0.0;
		pLR->fActLayNH4NUpt=(double)0.0;
		pLR=pLR->pNext;
		}
    pPltN->fActNUptR =(double)0.0;

	//======================================================================================
	//Potential Nitrogen Uptake
	//======================================================================================
	fTRNU=(double)0.0;

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;

	for (L=1;L<=pSo->iLayers-2;L++)
		{

		pSLN->fNO3Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;

	    //Check the whether there are roots in this layer:
	    if (pLR->fLengthDens==(double)0.0)		break;

		//The last layer of root:
		L1=L;

		//Potential nitrogen availability fFACTOR for NO3 (fFNO3) and NH4 (fFNH4) (0-1):
		if ((strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
		  ||(strcmp(pPl->pGenotype->acCropCode,"SG")==0)
		  ||(strcmp(pPl->pGenotype->acCropCode,"PT")==0))
			{
			fFNO3=(double)(1.0-exp(-0.030*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(double)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));

			if (fFNO3<(double)0.04) fFNO3=(double)0.0;
			if (fFNH4<(double)0.04) fFNH4=(double)0.0;
			}

 		if (strcmp(pPl->pGenotype->acCropCode,"MI")==0)
			{
			fFNO3=(double)(1.0-exp(-0.035*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(double)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));
		  	if (fFNO3<(double)0.03) fFNO3=(double)0.0;
		  	if (fFNH4<(double)0.03) fFNH4=(double)0.0;
			}

		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
		  ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
			{

		  	fFNO3=(double)(1.0-exp(-0.0275*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
		  	fFNH4=(double)(1.0-exp(-0.0250*((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg)-0.5)));
		  	if (fFNO3<(double)0.03) fFNO3 *= (fFNO3/(double)0.03)*(fFNO3/(double)0.03)*(fFNO3/(double)0.03);//(double)0.0;
		  	if (fFNH4<(double)0.03) fFNH4 *= (fFNH4/(double)0.03)*(fFNH4/(double)0.03)*(fFNH4/(double)0.03);//(double)0.0;
			}

		if (fFNO3>(double)1.0) fFNO3=(double)1.0;
		if (fFNH4>(double)1.0) fFNH4=(double)1.0;

		//Soil moisture deficit fFACTOR (fSMDFR) affecting nitrogen uptake at low soil water:
		if (pSLW->fContAct<pSWL->fContFK)
			fSMDFR=(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		else
			fSMDFR=(pSWL->fContSat-pSLW->fContAct)/(pSWL->fContSat-pSWL->fContFK);

		fSMDFR = max(min((double)1.0,fSMDFR),(double)0.0);

		//SG/27/05/99: bei ausreichender Wasserversorgung nur linearer Stress!
		//if(pPl->pPltWater->fActTranspDay>(double)1.0)		
		if(fActTraR>(double)1.0)		
			//fSMDFR=(double)1.0;                            
		{
			fRFAC=pLR->fLengthDens*fSMDFR*(double)0.1*pSL->fThickness*((double)100.0);
		}
		else
		{
		//fRFAC: Effect of water stress and root length density on potential nitrogen uptake
		fRFAC=pLR->fLengthDens*fSMDFR*fSMDFR*(double)0.1*pSL->fThickness*((double)100.0);
		//SG 20111115: für AgMip nur linearer Stress bei Wasserknappheit
		//fRFAC=pLR->fLengthDens*fSMDFR*(double)0.1*pSL->fThickness*((double)100.0);
		}
     
		//SG/10/11/99
		if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
		{
			fRFAC=pLR->fLengthDens*fSMDFR*(double)0.1*pSL->fThickness*((double)100.0);
		}

		//Potential nitrogen uptake from layer L: RNO3[L],RNH4U[L](kg N/ha)
		RNO3U[L]=fRFAC*fFNO3*((double)0.006);
		RNH4U[L]=fRFAC*fFNH4*((double)0.006);

		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)
			{
		 	RNO3U[L]=fRFAC*fFNO3*((double)0.009);
		 	RNH4U[L]=fRFAC*fFNH4*((double)0.009);
			}

		RNO3U[L]=fRFAC*fFNO3*pPl->pGenotype->fMaxNuptRate; 	//SG/13/04/99: Warum der Umstand, 
		RNH4U[L]=fRFAC*fFNH4*pPl->pGenotype->fMaxNuptRate;  //wenn dann doch die Werte aus Plant.gtp 
															//verwendet werden?

		//Total Potential Root Nitrogen Uptake: fTRNU(kg N/ha)
		fTRNU=fTRNU+max((double)0.0,RNO3U[L]+RNH4U[L]);
                                    
		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
		}

	self->fTRNU_zwischen += fTRNU*pTi->pTimeStep->fAct;

	//======================================================================================
	//Nitrogen uptake factor relating supply and plant  demand: fNUF (dimensionless)
	//======================================================================================
	fANDEM=pPltN->fTotalDemand;
	
/*	C_DEBUG(fANDEM);
	
	{
		char S[128];
		sprintf(S,"test %f",fANDEM);
		//PRINT_ERROR_ID(xpn,S);
		PRINT_MESSAGE(xpn,3,"Dies ist eine Nachricht");
	
	}*/
	
	if (fANDEM<=(double)0.0)
        fNUF=(double)0.0;
	else
		{
			//SG/17/05/99:Etwas anders bei Kartoffel (aus PLAGEN)
			if(strcmp(pPl->pGenotype->acCropCode,"PT")==0)
				fNUF = ((double)0.3*fANDEM>fTRNU)?((double)1.0):((double)0.3*fANDEM/fTRNU);
			else
			{
				if (fANDEM>fTRNU)	fANDEM=fTRNU;
      			fNUF=fANDEM/fTRNU;
			}
		
			if (fTRNU==(double)0.0) 	return 0;
      	}            

	self->fNUF = fNUF;

	//======================================================================================
	//Actual Nitrogen Uptake  and Nitrogen Loss form Died Roots
	//======================================================================================
	fTRNU=(double)0.0; 	//fTRNU is now the actual total root nitrogen uptake (kg N/ha)
	//fTRNLOS=(double)0.0;	//Total loss of nitrogen from root system:fTRNLOS (g N/plant)

	pPltN->fActNO3NUpt =(double)0.0;
	pPltN->fActNH4NUpt =(double)0.0;
    pPltN->fActNUptR    =(double)0.0;

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;

	for (L=1;L<=L1;L++)
		{
		//Possible plant uptake from a layer:fUNO3,fUNH4(Kg N/ha)
		fUNO3=RNO3U[L]*fNUF;
		fUNH4=RNH4U[L]*fNUF;

		/*{
		char S[1000];
		sprintf(S,"%d,%d,%0.2f,%0.2f\n",L,L1,pSL->fBulkDens,pSL->fThickness);
		PRINT_MESSAGE(xpn,4,S);
		}*/
		
		
		pSLN->fNO3Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;

		//Low limit of extractable nitrogen in layer L: SNO3min,SNH4min(Kg N/ha)
		SNO3min=(double)0.25*pSLN->fNO3Nmgkg;
		SNH4min=(double)0.50*pSLN->fNH4Nmgkg;

		//if (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,1994,04,15)>=0)
		//if(fUNH4>(pSLN->fNH4N-SNH4min))		
		//{
		//printf("%d-%d-%d %d yes \n", pTi->pSimTime->iyear, pTi->pSimTime->mon, pTi->pSimTime->mday, L);

		//	if(fUNO3>pSLN->fNO3N-SNO3min)
		//		printf("%d-%d-%d %d ja \n",pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,L);
		//	if(fUNH4>(pSLN->fNH4N-SNH4min))
		//		printf("%d-%d-%d %d ja \n",pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,L);
		//}else
		//printf("%d-%d-%d %d no \n", pTi->pSimTime->iyear, pTi->pSimTime->mon, pTi->pSimTime->mday, L);

		//Actual plant uptake from a layer:fUNO3,fUNH4(Kg N/ha)
		if(fUNO3>pSLN->fNO3N-SNO3min)
			fUNO3=max((double)0.0, pSLN->fNO3N-SNO3min);

		if(fUNH4>(pSLN->fNH4N-SNH4min))
			fUNH4=max((double)0.0, pSLN->fNH4N-SNH4min);


		pPltN->fActNO3NUpt += fUNO3*pTS->fAct;
		pPltN->fActNH4NUpt += fUNH4*pTS->fAct;

		pLR->fActLayNO3NUpt= fUNO3;
		pLR->fActLayNH4NUpt= fUNH4;

		//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
		//pSLN->fNO3N -= fUNO3*pTi->pTimeStep->fAct;
		//pSLN->fNH4N -= fUNH4*pTi->pTimeStep->fAct;

		//Total actual root nitrogen uptake(kg N/ha)
		fTRNU += max((double)0.0,fUNO3+fUNH4);//*pTS->fAct;

		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
		}

	pPltN->fActNUptR = max((double)0.0,fTRNU);

	pPltN->fCumActNUpt 	+=	pPltN->fActNUptR*pTS->fAct;

	//======================================================================================
	//	Change in Plant Nitrogen Content due to nitrogen (Nitrogen partition) 
	//======================================================================================
	if (pPltN->fTotalDemand==(double)0.0)
		{
		pPl->pPltNitrogen->fLeafNinc = (double)0.0;
		pPl->pPltNitrogen->fStemNinc = (double)0.0;
		pPl->pPltNitrogen->fRootNinc = (double)0.0;
		pPl->pPltNitrogen->fShootNinc= (double)0.0;
		}
	else
		{	
		pPl->pPltNitrogen->fLeafNinc = pPltN->fActNUptR*pPl->pPltNitrogen->fLeafDemand/pPltN->fTotalDemand; 
		pPl->pPltNitrogen->fStemNinc = pPltN->fActNUptR*pPl->pPltNitrogen->fStemDemand/pPltN->fTotalDemand;
		pPl->pPltNitrogen->fRootNinc = pPltN->fActNUptR*pPl->pPltNitrogen->fRootDemand/pPltN->fTotalDemand;
		pPl->pPltNitrogen->fShootNinc= pPl->pPltNitrogen->fLeafNinc+pPl->pPltNitrogen->fStemNinc ;
 		}

	self->fLeafRatio = pPl->pPltNitrogen->fLeafDemand/pPltN->fTotalDemand;
	self->fRootRatio = pPl->pPltNitrogen->fRootDemand/pPltN->fTotalDemand;
	self->fStemRatio = pPl->pPltNitrogen->fStemDemand/pPltN->fTotalDemand;

	spass_PlantNitrogenTranslocation(self);
	spass_PlantNitrogenStress(self);

	//aus plant.c

	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
        

	for (L=1;L<=pSo->iLayers-2;L++)
	////for (L=1;L<=L1;L++)
			{

	//		//printf("%d %f \n", L, pLR->fLengthDens);	
	//		//Check the whether there are roots in this layer:
		    if (pLR->fLengthDens==(float)0.0)		break;

	//		//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
			pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct;
			pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct;
                        
            self->_nact_day+=(pLR->fActLayNO3NUpt+pLR->fActLayNH4NUpt)*pTi->pTimeStep->fAct;

			pLR =pLR ->pNext;
			pSLN=pSLN->pNext;
			
			CHECK_VALID(pSLN->fNO3N)
		
			}
  } //end PLANT_IS_GROWING
  
    return RET_SUCCESS;
  }

int spass_PlantNitrogenTranslocation(spass *self)
	{
    expertn_modul_base *xpn = &(self->parent);

	PPLANT				pPl = xpn->pPl;
	PCLIMATE			pCl = xpn->pCl;
	PMANAGEMENT			pMa = xpn->pMa;
	PTIME				pTi = xpn->pTi;

	PBIOMASS			pBiom=pPl->pBiomass;
	PDEVELOP			pDev =pPl->pDevelop;
	PCANOPY				pCan =pPl->pCanopy;
	PPLTNITROGEN	 	pPltN=pPl->pPltNitrogen;  

	PWEATHER			pWth =pCl->pWeather;
	PMSOWINFO			pSI  =pMa->pSowInfo;
	
	double				fTransN,fTransNconst, fTempMin,fTempOpt,fTempMax,fTempFunc,fGrainNcumRate,fGrainDemand;//,fTemp
	RESPONSE			*pResp;  
	
//	double fNDemandMax;
	
	//Time constant for N translocation
	fTransNconst = (double)20.0;
	
	//SG/17/05/99:Etwas anders bei Kartoffel (aus PLAGEN)
	if(strcmp(pPl->pGenotype->acCropCode,"PT")==0)
		fTransNconst = (double)30.0;


	if ((pDev->fStageSUCROS>=(double)1.0)
		||((strcmp(pPl->pGenotype->acCropCode,"PT")==0)&&(pDev->fStageSUCROS>=(double)0.5)))
		{
		//===========================================================================================
		//Amount of translocatable nitrogen
		//===========================================================================================
		pPl->pPltNitrogen->fLeafTransNw=pBiom->fLeafWeight*(pPl->pPltNitrogen->fLeafActConc-pPl->pPltNitrogen->fLeafMinConc)+pPl->pPltNitrogen->fLeafNinc;
	    pPl->pPltNitrogen->fStemTransNw=pBiom->fStemWeight*(pPl->pPltNitrogen->fStemActConc-pPl->pPltNitrogen->fStemMinConc)+pPl->pPltNitrogen->fStemNinc;
	    pPl->pPltNitrogen->fRootTransNw=pBiom->fRootWeight*(pPltN->fRootActConc-pPl->pPltNitrogen->fRootMinConc)+pPl->pPltNitrogen->fRootNinc;
	
	    pPltN->fRootNLabile=pPl->pPltNitrogen->fRootTransNw;
	    pPltN->fStovNLabile=pPl->pPltNitrogen->fLeafTransNw+pPl->pPltNitrogen->fStemTransNw;

	    pPl->pPltNitrogen->fTotalTransNw=pPltN->fTotalNLabile=pPl->pPltNitrogen->fLeafTransNw+pPl->pPltNitrogen->fStemTransNw+pPl->pPltNitrogen->fRootTransNw;

		//===========================================================================================
		//Storage Nitrogen demand
		//===========================================================================================
		//The cordinal temperature of development rate
	    fTempMin= pPl->pGenotype->fTempMinSoNFill;
	    fTempOpt= pPl->pGenotype->fTempOptSoNFill;
	    fTempMax= pPl->pGenotype->fTempMaxSoNFill;
		pResp   = pPl->pGenotype->SoNFillRateTemp;
		
		//Calculating the daily thermal effect
		//fTemp = (double)0.5*(pWth->fTempMax+pWth->fTempMin);
	
		if (pPl->pGenotype->SoNFillRateTemp[0].fInput==(double)-99.9)
			fTempFunc = (double)spass_RelativeTemperatureResponse(pWth->fTempAir,fTempMin,fTempOpt,fTempMax);
		else
			fTempFunc = (double)spass_AFGENERATOR(pWth->fTempAir, pPl->pGenotype->SoNFillRateTemp);

		fGrainNcumRate=(double)pPl->pGenotype->fRelStorageNFillRate*fTempFunc;

        if (pBiom->fGrainGrowR>(double)0.0)
			{
			pPl->pPltNitrogen->fGrainDemand = fGrainNcumRate*pCan->fGrainNum*pSI->fPlantDens*(double)1E-2;
			fGrainDemand = (pBiom->fGrainWeight-pBiom->fGrainGrowR*pTi->pTimeStep->fAct)*max((double)0.0, (double)0.02-pPltN->fGrainConc)
					  +pBiom->fGrainGrowR*(double)0.02*pTi->pTimeStep->fAct;

			if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)			
			//	
			{
				double fTubMaxConc[]={(double)0.02, (double)0.015, (double)0.01};
				double dev[]={(double)0.5, (double)1.0, (double)2.0};
				double xTm=(double)0;


				if(strcmp(pPl->pGenotype->acVarietyName,"Christa")==0)
				{
					fTubMaxConc[0]=(double)0.02;
					fTubMaxConc[1]=(double)0.015;
					fTubMaxConc[2]=(double)0.01;
				}
				if(strcmp(pPl->pGenotype->acVarietyName,"Agria")==0)
				{
					fTubMaxConc[0]=(double)0.03;
					fTubMaxConc[1]=(double)0.025;//0.0225;
					fTubMaxConc[2]=(double)0.025;//0.015;
				}

				if ((pDev->fStageSUCROS>=dev[0])&&(pDev->fStageSUCROS<=dev[1]))
					xTm=fTubMaxConc[0]+(pDev->fStageSUCROS-dev[0])/(dev[1]-dev[0])*(fTubMaxConc[1]-fTubMaxConc[0]);

				if ((pDev->fStageSUCROS>=dev[1])&&(pDev->fStageSUCROS<=dev[2]))
					xTm=fTubMaxConc[1]+(pDev->fStageSUCROS-dev[1])/(dev[2]-dev[1])*(fTubMaxConc[2]-fTubMaxConc[1]);

				if (pDev->fStageSUCROS>=dev[2])
					xTm=fTubMaxConc[2];

				fGrainDemand = (pBiom->fGrainWeight-pBiom->fGrainGrowR*pTi->pTimeStep->fAct)*max((double)0.0, xTm-pPltN->fGrainConc)
								 +pBiom->fGrainGrowR*xTm;
			}

			
	//		fNDemandMax = pBiom->fGrainWeight * ((double)0.02 - pPltN->fGrainConc) + pBiom->fGrainGrowR*(double)0.02;

	//		pPl->pPltNitrogen->fGrainDemand = min(pPl->pPltNitrogen->fTotalTransNw/fTransNconst, min(fNDemandMax,max(pPl->pPltNitrogen->fGrainDemand,fGrainDemand)));
			pPl->pPltNitrogen->fGrainDemand = min(pPl->pPltNitrogen->fTotalTransNw/fTransNconst, max(pPl->pPltNitrogen->fGrainDemand,fGrainDemand));
			 
			}
		else
			pPl->pPltNitrogen->fGrainDemand = (double)0.0;	

		pPltN->fGrainDemand = pPl->pPltNitrogen->fGrainDemand;
		 
		//===========================================================================================
		//Total translocation
		//===========================================================================================
		fTransN = min(pPl->pPltNitrogen->fTotalTransNw, pPl->pPltNitrogen->fGrainDemand);
		
        pPl->pPltNitrogen->fGrainNinc = fTransN;
    
        pPl->pPltNitrogen->fLeafNtransRate=fTransN*pPl->pPltNitrogen->fLeafTransNw/(pPl->pPltNitrogen->fTotalTransNw+(double)1.0E-9);
        pPl->pPltNitrogen->fStemNtransRate=fTransN*pPl->pPltNitrogen->fStemTransNw/(pPl->pPltNitrogen->fTotalTransNw+(double)1.0E-9);
        pPl->pPltNitrogen->fRootNtransRate=fTransN*pPl->pPltNitrogen->fRootTransNw/(pPl->pPltNitrogen->fTotalTransNw+(double)1.0E-9);
		}
	else
		{
        pPl->pPltNitrogen->fGrainNinc     =(double)0.0;
        pPl->pPltNitrogen->fLeafNtransRate=(double)0.0;
        pPl->pPltNitrogen->fStemNtransRate=(double)0.0;
        pPl->pPltNitrogen->fRootNtransRate=(double)0.0;
        }

      return RET_SUCCESS;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenDemand_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int spass_nitrogen_demand(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PPLANT			pPl	  = xpn->pPl;
	PTIME			pTi   = xpn->pTi;

	PBIOMASS		pBiom = pPl->pBiomass;
    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;

	if ((pPl->pDevelop->iDayAftEmerg > 0) && (PLANT_IS_GROWING))
	{

	spass_NitrogenConcentrationLimits	(self);

	//SG/13/04/99/////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Achtung!
	//
	//	In SPASS erfolgt die Aktualisierung der Biomassen bereits vor der Berechnung
	//	des Stickstoffbedarfs. Dies hat zur Folge, dass hier der Bedarf der neu gebildeten
	//	Biomasse zweimal mitberücksichtigt ist.
	//	
	//pPl->pPltNitrogen->fLeafDemand =  pBiom->fLeafWeight*(pPl->pPltNitrogen->fLeafMaxConc-pPl->pPltNitrogen->fLeafActConc)
    //					+pBiom->fLeafGrowR*pPl->pPltNitrogen->fLeafMaxConc;
    //pPl->pPltNitrogen->fStemDemand =  pBiom->fStemWeight*(pPl->pPltNitrogen->fStemMaxConc-pPl->pPltNitrogen->fStemActConc)
    //					+pBiom->fStemGrowR*pPl->pPltNitrogen->fStemMaxConc;
    //pPl->pPltNitrogen->fRootDemand =  pBiom->fRootWeight*(pPl->pPltNitrogen->fRootMaxConc-pPltN->fRootActConc)
    //					+pBiom->fRootGrowR*pPl->pPltNitrogen->fRootMaxConc;
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	pPl->pPltNitrogen->fLeafDemand =  (pBiom->fLeafWeight-pBiom->fLeafGrowR*pTi->pTimeStep->fAct)*(pPl->pPltNitrogen->fLeafMaxConc-pPl->pPltNitrogen->fLeafActConc)
    					+pBiom->fLeafGrowR*pPl->pPltNitrogen->fLeafMaxConc*pTi->pTimeStep->fAct;
    pPl->pPltNitrogen->fStemDemand =  (pBiom->fStemWeight-pBiom->fStemGrowR*pTi->pTimeStep->fAct)*(pPl->pPltNitrogen->fStemMaxConc-pPl->pPltNitrogen->fStemActConc)
    					+pBiom->fStemGrowR*pPl->pPltNitrogen->fStemMaxConc*pTi->pTimeStep->fAct;
    pPl->pPltNitrogen->fRootDemand =  (pBiom->fRootWeight-pBiom->fRootGrowR*pTi->pTimeStep->fAct)*(pPl->pPltNitrogen->fRootMaxConc-pPltN->fRootActConc)
    					+pBiom->fRootGrowR*pPl->pPltNitrogen->fRootMaxConc*pTi->pTimeStep->fAct;
	
	//FH 29/01/13: Stickstoffbedarf muss größer 0 sein
	pPltN->fRootDemand = max(0.0,pPl->pPltNitrogen->fRootDemand);
	pPltN->fLeafDemand = max(0.0,pPl->pPltNitrogen->fLeafDemand);
	pPltN->fStemDemand = max(0.0,pPl->pPltNitrogen->fStemDemand);

	pPltN->fShootDemand= pPl->pPltNitrogen->fLeafDemand+pPl->pPltNitrogen->fStemDemand;

    pPltN->fTotalDemand= pPl->pPltNitrogen->fLeafDemand+pPl->pPltNitrogen->fStemDemand+pPl->pPltNitrogen->fRootDemand; 
            
    self->_npot_day+=pPltN->fTotalDemand*xpn->pTi->pTimeStep->fAct;

	} //end PLANT_IS_GROWING

    return RET_SUCCESS;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenStress_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int spass_PlantNitrogenStress(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PPLANT				pPl = xpn->pPl;

	PBIOMASS			pBiom=pPl->pBiomass;
	PPLTNITROGEN	 	pPltN=pPl->pPltNitrogen;  
	PDEVELOP			pDev =pPl->pDevelop;

	PTIMESTEP			pTi = xpn->pTi->pTimeStep;

	//SG/06/04/99//////////////////////////////////////////////////////////////////////////////////
	//
	// In PLAGEN wird der aktuelle N-Gehalt der einzelnen Organe etwas anders berechnet!
	//
	///////////////////////////////////////////////////////////////////////////////////////////////

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//Nitrogen Amount of different organs
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	
	pPl->pPltNitrogen->fDeadLeafNw += pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc*pTi->fAct;
	pPl->pPltNitrogen->fDeadRootNw += pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc*pTi->fAct;

	//SG/09/04/99/////////////////////////////////////////////////////////////////////
	//
	//	Achtung: im ursprünglichen PLAGEN stehen diese Gleichungen vor der
	//	Aktualisierung der Biomasse. SPASS (auch der alte Code) macht bei der
	//	Konzentrationsberechnung also einen Fehler, da auf die aktualisierte 
	//	Biomasse nocheinmal die aktuelle Zuwachsrate addiert wird.
	//////////////////////////////////////////////////////////////////////////////////

	if (pBiom->fLeafWeight!=(double)0.0)
	{
	pPl->pPltNitrogen->fLeafCont  += (pPl->pPltNitrogen->fLeafNinc-pPl->pPltNitrogen->fLeafNtransRate
					   -pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc)*pTi->fAct;
//	pPltN->fLeafCont  = pBiom->fLeafWeight*pPltN->fLeafActConc+pPltN->fLeafNinc-pPltN->fLeafNtransRate
//					   -pBiom->fLeafDeathRate*pPltN->fLeafMinConc;

	pPltN->fLeafActConc = pPltN->fLeafCont/(pBiom->fLeafWeight-pPl->pBiomass->fLeafDeathRate*pTi->fAct+(double)1.0E-6);
	//pPltN->fLeafActConc = pPltN->fLeafCont/(pBiom->fLeafWeight-pPl->pBiomass->fLeafDeathRate+(double)1.0E-6);
	//SG/14/04/99:	epsilon auf 1e-6 gesetzt, wegen PLAGEN!
	}

	if (pBiom->fStemWeight!=(double)0.0)
	{
	pPl->pPltNitrogen->fStemCont  += (pPl->pPltNitrogen->fStemNinc-pPl->pPltNitrogen->fStemNtransRate)*pTi->fAct;
//	pPltN->fStemCont  = pBiom->fStemWeight*pPltN->fStemActConc+pPltN->fStemNinc-pPltN->fStemNtransRate
//					   -pBiom->fStemDeathRate*pPltN->fStemMinConc; //in SPASS keine Death-Rate!
	
	pPltN->fStemActConc = pPltN->fStemCont/(pBiom->fStemWeight-pPl->pBiomass->fStemDeathRate*pTi->fAct+(double)1.0E-6);
	//pPltN->fStemActConc = pPltN->fStemCont/(pBiom->fStemWeight-pPl->pBiomass->fStemDeathRate+(double)1.0E-6);
	}

	if (pBiom->fRootWeight!=(double)0.0)
	{
	pPl->pPltNitrogen->fRootCont  += (pPl->pPltNitrogen->fRootNinc-pPl->pPltNitrogen->fRootNtransRate
  					   -pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc)*pTi->fAct;
//	pPltN->fRootCont  = pBiom->fRootWeight*pPltN->fRootActConc+pPltN->fRootNinc-pPltN->fRootNtransRate
//					   -pBiom->fRootDeathRate*pPltN->fRootMinConc;
	
	pPltN->fRootActConc = pPltN->fRootCont/(pBiom->fRootWeight-pPl->pBiomass->fRootDeathRate*pTi->fAct+(double)1.0E-6);
	//pPltN->fRootActConc = pPltN->fRootCont/(pBiom->fRootWeight-pPl->pBiomass->fRootDeathRate+(double)1.0E-6);

	}

	if (((pDev->fStageSUCROS>=(double)1.0)&&(pBiom->fGrainWeight!=(double)0.0))
		||((strcmp(pPl->pGenotype->acCropCode,"PT")==0)&&(pBiom->fGrainWeight>(double)0.0)))
	{
	pPl->pPltNitrogen->fGrainCont += pPl->pPltNitrogen->fGrainNinc*pTi->fAct;
//	pPltN->fGrainCont  = pBiom->fGrainWeight*pPltN->fGrainConc+pPltN->fGrainNinc; 
	// in PLAGEN -DeathRate*MinConc! DeathRate und MinConc sind in SPASS aber nicht bekannt!

	pPltN->fGrainConc= pPltN->fGrainCont/(pBiom->fGrainWeight+(double)1.0E-6);
	//pPltN->fGrainConc= pPltN->fGrainCont/(pBiom->fGrainWeight)+(double)1.0E-6;

	//sg19102000: Zur Vermeidung unrealistisch hoher N-Konzentrationen in den Körnern 
	//während der ersten Tage der Kornfüllung Einführung einer oberen Grenze
	if(pPltN->fGrainConc>=(double)0.04)
		pPltN->fGrainConc = (double)0.04;
	
	}

	   pPltN->fFruitActConc     = pPltN->fGrainConc; //SG 20190310

	//		alter SPASS-Code
/*
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//Nitrogen Amount of different organs
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPl->pPltNitrogen->fLeafCont  += pPl->pPltNitrogen->fLeafNinc-pPl->pPltNitrogen->fLeafNtransRate
					   -pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc;
	pPl->pPltNitrogen->fStemCont  += pPl->pPltNitrogen->fStemNinc-pPl->pPltNitrogen->fStemNtransRate;
	pPl->pPltNitrogen->fRootCont  += pPl->pPltNitrogen->fRootNinc-pPl->pPltNitrogen->fRootNtransRate
  					   -pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc;
	pPl->pPltNitrogen->fGrainCont += pPl->pPltNitrogen->fGrainNinc;

	//===========================================================================================
	//Nitrogen content of different organs
	//===========================================================================================
	pPl->pPltNitrogen->fLeafActConc = pPl->pPltNitrogen->fLeafCont/(pBiom->fLeafWeight+pBiom->fLeafGrowR-pPl->pBiomass->fLeafDeathRate+(double)1.0E-9);
	pPl->pPltNitrogen->fStemActConc = pPl->pPltNitrogen->fStemCont/(pBiom->fStemWeight+pBiom->fStemGrowR+(double)1.0E-9);
	
	pPltN->fRootActConc = pPl->pPltNitrogen->fRootCont/(pBiom->fRootWeight+pBiom->fRootGrowR-pPl->pBiomass->fRootDeathRate+(double)1.0E-9);

	if ((pDev->fStageSUCROS>=(double)1.0)&&(pBiom->fGrainWeight!=(double)0.0))
		pPltN->fGrainConc= pPl->pPltNitrogen->fGrainCont/(pBiom->fGrainWeight+pBiom->fGrainGrowR+(double)1.0E-9);
    

*/

    pPl->pPltNitrogen->fLeafActConc=(double)spass_LIMIT(pPl->pPltNitrogen->fLeafMinConc, pPl->pPltNitrogen->fLeafMaxConc, pPl->pPltNitrogen->fLeafActConc);
    pPl->pPltNitrogen->fStemActConc=(double)spass_LIMIT(pPl->pPltNitrogen->fStemMinConc, pPl->pPltNitrogen->fStemMaxConc, pPl->pPltNitrogen->fStemActConc);
   
	//if (pPl->pDevelop->fStageSUCROS<=(double)0.39)
	//	printf("stage %f transrate %f min %f act %f max %f \n", pPl->pPltNitrogen->fStemNtransRate, pPl->pDevelop->fStageSUCROS, pPl->pPltNitrogen->fStemMinConc, pPl->pPltNitrogen->fStemActConc, pPl->pPltNitrogen->fStemMaxConc);

    pPltN->fRootActConc=(double)spass_LIMIT(pPl->pPltNitrogen->fRootMinConc, pPl->pPltNitrogen->fRootMaxConc, pPltN->fRootActConc);

	//pPltN->fTopsActConc=pPltN->fVegActConc= (pPl->pPltNitrogen->fStemCont+pPl->pPltNitrogen->fLeafCont)
	//									  /(pBiom->fStemWeight+pBiom->fStemGrowR-pPl->pBiomass->fLeafDeathRate
	//									   +pBiom->fLeafWeight+pBiom->fLeafGrowR+(double)1.0E-9);
    
	pPltN->fTopsActConc=pPltN->fVegActConc= (pPltN->fLeafActConc*pBiom->fLeafWeight + pPltN->fStemActConc*pBiom->fStemWeight)
												/(pBiom->fLeafWeight+pBiom->fStemWeight);
    
	pPltN->fStovCont = pPltN->fLeafCont+ pPltN->fStemCont;

    if (pPltN->fGrainConc>=(double)1.0)
    	pPltN->fGrainConc=(double)0.07;      

    //SG/06/04/99/////////////////////////////////////////////////////////////////////////////
	//
	// In PLAGEN werden die relativen N-Gehalte der einzelnen Organe etwas anders berechnet.
	// Insbesondere fällt die Unterscheidung zwischen iStageCERES >2 und =<2 weg.
	// Ausserdem wird nicht die maximale sondern die optimale N-Konzentration berücksichtigt.
	//////////////////////////////////////////////////////////////////////////////////////////

	
	if((pPltN!=NULL)&&(pDev->fStageSUCROS>=(double)0.0))
	{
	
	pPltN->fLeafRelNc=(pPltN->fLeafActConc-pPltN->fLeafMinConc)/(pPltN->fLeafMaxConc-pPltN->fLeafMinConc+(double)1.0E-9);
    pPltN->fStemRelNc=(pPltN->fStemActConc-pPltN->fStemMinConc)/(pPltN->fStemMaxConc-pPltN->fStemMinConc+(double)1.0E-9);
    pPltN->fRootRelNc=(pPltN->fRootActConc-pPltN->fRootMinConc)/(pPltN->fRootMaxConc-pPltN->fRootMinConc+(double)1.0E-9);
	/*
	pPltN->fLeafRelNc=(pPltN->fLeafActConc-pPltN->fLeafMinConc)/(pPltN->fLeafOptConc-pPltN->fLeafMinConc+(double)1.0E-9);
    pPltN->fStemRelNc=(pPltN->fStemActConc-pPltN->fStemMinConc)/(pPltN->fStemOptConc-pPltN->fStemMinConc+(double)1.0E-9);
    pPltN->fRootRelNc=(pPltN->fRootActConc-pPltN->fRootMinConc)/(pPltN->fRootOptConc-pPltN->fRootMinConc+(double)1.0E-9);
	*/
	}

	else
	{
		pPltN->fLeafRelNc=(double)1.0;
	    pPltN->fStemRelNc=(double)1.0;
	    pPltN->fRootRelNc=(double)1.0;
	}

	
	
//		alter SPASS-Code

/*	pPl->pPltNitrogen->fLeafRelNc=(pPl->pPltNitrogen->fLeafActConc-pPl->pPltNitrogen->fLeafMinConc)/(pPl->pPltNitrogen->fLeafMaxConc-pPl->pPltNitrogen->fLeafMinConc+(double)1.0E-9);
    pPl->pPltNitrogen->fStemRelNc=(pPl->pPltNitrogen->fStemActConc-pPl->pPltNitrogen->fStemMinConc)/(pPl->pPltNitrogen->fStemMaxConc-pPl->pPltNitrogen->fStemMinConc+(double)1.0E-9);
    pPl->pPltNitrogen->fRootRelNc=(pPltN->fRootActConc-pPl->pPltNitrogen->fRootMinConc)/(pPl->pPltNitrogen->fRootMaxConc-pPl->pPltNitrogen->fRootMinConc+(double)1.0E-9);

 
	if (pPl->pDevelop->iStageCERES <= 2)
		{	  
	    pPl->pPltNitrogen->fLeafRelNc=(double)1.0;
	    pPl->pPltNitrogen->fStemRelNc=(double)1.0;
	    pPl->pPltNitrogen->fRootRelNc=(double)1.0;
        }

*/  	  
	  
	return RET_SUCCESS;
  }


/*******************************************************************************
** EOF */
