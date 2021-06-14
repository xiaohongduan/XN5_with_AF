//      spass_senescence.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_senescence.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int spass_crop_senescence(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PCLIMATE		pCl = xpn->pCl; 
	PPLANT			pPl = xpn->pPl;
	PMANAGEMENT		pMa = xpn->pMa;
	PTIME			pTi = xpn->pTi;

	double fRelDeathRateLv,fRelDeathRateRt,RDR,fFlvNc,fFrtNc;
 	double LAICR,DvsCR;
 	
	PWEATHER		pWth = pCl->pWeather;
	PBIOMASS		pBiom= pPl->pBiomass;
	PDEVELOP		pDev = pPl->pDevelop;
	PCANOPY     	pCan = pPl->pCanopy;   
	PPLTNITROGEN 	pPltN= pPl->pPltNitrogen;
	PTIMESTEP		pTS = pTi->pTimeStep;
	
	if ((pPl->pDevelop->iDayAftEmerg > 0) && (PLANT_IS_GROWING))
	{
	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		return RET_SUCCESS;

   	LAICR = pPl->pGenotype->fBeginShadeLAI;	//4.0; //Critical LAI over which leaf dies due to shading
    DvsCR = pPl->pGenotype->fBeginSenesDvs; //1.0
    
	//=========================================================================================
	// Effect of nitrogne content
	fFlvNc = (double)1.0/max((double)0.5,pPl->pPltNitrogen->fLeafRelNc);  //0.5 sg:doch wieder 0.5 statt 0.3
	fFrtNc = (double)1.0/max((double)0.5,pPl->pPltNitrogen->fRootRelNc);  //0.5

	
	//SG/18/05/99: Potato aus PLAGEN!
    if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
	  {
	   fFlvNc = (double)1.0/max((double)0.1,pPl->pPltNitrogen->fLeafRelNc);  //0.5
	   fFrtNc = (double)1.0/max((double)0.1,pPl->pPltNitrogen->fRootRelNc);  //0.5
	  }


	//lpPlt->pPltNitrogen->fNDEF2 = max((double)0.3,pPl->pPltNitrogen->fLeafRelNc);
	//=========================================================================================
	//Temperature Effect  
	RDR = spass_AFGENERATOR(pWth->fTempAir,pPl->pGenotype->LvDeathRateTemp);

	//=========================================================================================
	//Leaf death rate due to developmental age 
	if (pDev->fStageSUCROS<DvsCR)
		pPl->pCanopy->fLeafAgeDeathRate = (double)(0.0);
	else
		{
		 pPl->pCanopy->fLeafAgeDeathRate = RDR*(double)pow((double)pDev->fStageSUCROS, 1.5);
		
	//SG/18/05/99: Potato aus PLAGEN!
	    if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
	      {
	       pPl->pCanopy->fLeafAgeDeathRate = RDR*(double)(6.0*pow((double)pDev->fStageSUCROS-1, 3.0)); 
	       if (strcmp(pPl->pGenotype->acVarietyName,"Agria")==0)//Agria=^3 Christa=^1.5	
		     pPl->pCanopy->fLeafAgeDeathRate = RDR*(double)(6.0*pow(((double)pDev->fStageSUCROS-DvsCR)/((double)2.-DvsCR), 3.0)); 
	       if (strcmp(pPl->pGenotype->acVarietyName,"Christa")==0)	
		     pPl->pCanopy->fLeafAgeDeathRate = RDR*(double)(6.0*pow(((double)pDev->fStageSUCROS-DvsCR)/((double)2.-DvsCR), 1.5)); 
  	      }
			
		}

	//=========================================================================================
	//Leaf death rate due to shading   
	pPl->pCanopy->fLeafShadeDeathRate=(double)spass_LIMIT((double)0.0, RDR, RDR*(pCan->fLAI-LAICR)/LAICR);

	//=========================================================================================
 	//Relative leaf death rate due to nitrogen deficiency
    fRelDeathRateLv = fFlvNc*max(pPl->pCanopy->fLeafAgeDeathRate, pPl->pCanopy->fLeafShadeDeathRate);
 	pPl->pCanopy->fRelLeafDeathRate =fRelDeathRateLv; 
	
//	if (pDev->fZStage>=(double)8.5)
//	 	pCan->fRelLeafDeathRate = (double)2.0*fRelDeathRateLv; 
	//=========================================================================================
	//Dead leaf area index and biomass weight
	pPl->pCanopy->fLeafAreaLossRate = pCan->fLAI*pPl->pCanopy->fRelLeafDeathRate;
	pPl->pBiomass->fLeafDeathRate   = fRelDeathRateLv*pBiom->fLeafWeight;

	pCan->fLAI -= pPl->pCanopy->fLeafAreaLossRate*pTS->fAct;

	//===========================================================================================
	//Death rate of Root
	//===========================================================================================
	//Temperature Effect
	RDR = spass_AFGENERATOR(pWth->fTempAir,pPl->pGenotype->RtDeathRateTemp);

	//if (pDev->fStageSUCROS<(double)1) 
	if (pDev->fStageSUCROS<DvsCR) 
		fRelDeathRateRt= (double)0.0;
	else
		fRelDeathRateRt = RDR;

	//===========================================================================================
 	//Relative Root death rate due to nitrogen deficiency
    fRelDeathRateRt      = fFrtNc*fRelDeathRateRt;
	pPl->pBiomass->fRootDeathRate = fRelDeathRateRt*pBiom->fRootWeight;

	pPl->pBiomass->fDeadLeafWeight += pPl->pBiomass->fLeafDeathRate*pTS->fAct;
	pPl->pBiomass->fDeadRootWeight += pPl->pBiomass->fRootDeathRate*pTS->fAct;

	//Dry weight of different organs	                          
	pBiom->fLeafWeight 		-= pPl->pBiomass->fLeafDeathRate*pTS->fAct; 
	pBiom->fStemWeight  	-= pPl->pBiomass->fReserveTransRate*pTS->fAct;
	pBiom->fRootWeight  	-= pPl->pBiomass->fRootDeathRate*pTS->fAct;

	pPl->pBiomass->fTotLeafWeight 	= pBiom->fLeafWeight+pPl->pBiomass->fDeadLeafWeight; 
	pPl->pBiomass->fTotRootWeight  	= pBiom->fRootWeight+pPl->pBiomass->fDeadRootWeight;
	pPl->pBiomass->fTotStemWeight  	= pBiom->fStemWeight+pPl->pBiomass->fDeadStemWeight;

	pBiom->fBiomassAbvGround = pPl->pBiomass->fTotLeafWeight+pPl->pBiomass->fTotStemWeight+pBiom->fGrainWeight;
	pBiom->fTotalBiomass     = pBiom->fBiomassAbvGround + pBiom->fRootWeight;

  } //end PLANT_IS_GROWING
    return RET_SUCCESS;
  }


/*******************************************************************************
** EOF */