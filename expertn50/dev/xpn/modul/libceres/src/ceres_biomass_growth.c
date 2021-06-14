#include "ceres_biomass_growth.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int ceres_biomass_growth_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	//      double          fStemWeight;	
	PBIOMASS        pBiom=pPl->pBiomass;
	PDEVELOP        pDev =pPl->pDevelop;
	PCANOPY         pCan =pPl->pCanopy;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20180731

	//wheat, barley:
	if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)||(strcmp(pPl->pGenotype->acCropCode,"WH")==0)) {
		//====================================================================================
		//      Organ Death
		//====================================================================================
		pPl->pBiomass->fLeafDeathRate = pBiom->fLeafWeight*((double)0.000267)*pDev->fDTT
		                                *((double)1.0-pCan->fPlantSenesLA/pCan->fPlantLA);
		pPl->pBiomass->fLeafDeathRate = max((double)0.0,pPl->pBiomass->fLeafDeathRate);
		pPl->pBiomass->fStemDeathRate = pBiom->fStemWeight*((double)0.000267)*pDev->fDTT;
		pPl->pBiomass->fRootDeathRate = ((double)0.4)*pBiom->fRootGrowR
		                                +((double)0.005)*pBiom->fRootWeight;

		pPl->pBiomass->fDeadLeafWeight += pPl->pBiomass->fLeafDeathRate;
		pPl->pBiomass->fDeadStemWeight += pPl->pBiomass->fStemDeathRate;
		pPl->pBiomass->fDeadRootWeight += pPl->pBiomass->fRootDeathRate;

		//====================================================================================
		//      Organ Dry weight
		//====================================================================================
		pBiom->fLeafWeight += pBiom->fLeafGrowR - pPl->pBiomass->fLeafDeathRate;
		pBiom->fStemWeight += pBiom->fStemGrowR - pPl->pBiomass->fStemDeathRate;
		pBiom->fRootWeight += pBiom->fRootGrowR - pPl->pBiomass->fRootDeathRate;

		pBiom->fGrainWeight+= pBiom->fGrainGrowR;
//      pBiom->fEarWeight        =pBiom->fGrainWeight;

		//====================================================================================
		//      Leaf Senescence and Biomass Regulation in Stage 1
		//====================================================================================
		if ((pCan->fPlantLA-pCan->fPlantSenesLA)<=(double)0.035*pCan->fTillerNum) {
			if ((pDev->iStageCERES<=1)&&(pBiom->fSeedReserv==(double)0.0)) {
				pBiom->fSeedReserv=(double)0.05*(pBiom->fLeafWeight+pBiom->fRootWeight);

				if(pBiom->fSeedReserv>(double)0.05)
					pBiom->fSeedReserv=(double)0.05;

				pBiom->fLeafWeight=pBiom->fLeafWeight-(double)0.5*pBiom->fSeedReserv;
				pBiom->fRootWeight=pBiom->fRootWeight-(double)0.5*pBiom->fSeedReserv;
				
				pCB->dCInputSurf += pBiom->fSeedReserv; //Hong added on 20180731 for C-balance
				//pCB->dCInputCum += pBiom->fSeedReserv;
			}
		}

		//====================================================================================
		//Total weight
		//====================================================================================
		//pBiom->fStoverWeight= pBiom->fLeafWeight+pBiom->fStemWeight+pBiom->fSeedReserv;

		pPl->pBiomass->fTotLeafWeight   =pBiom->fLeafWeight;
		pPl->pBiomass->fTotRootWeight   =pBiom->fRootWeight;
		pPl->pBiomass->fTotStemWeight   =pBiom->fStemWeight;

		pBiom->fBiomassAbvGround        = pBiom->fLeafWeight
		                                  +pBiom->fStemWeight
		                                  +pBiom->fGrainWeight
		                                  +pBiom->fSeedReserv;

		pBiom->fTotalBiomass            = pBiom->fBiomassAbvGround
		                                  +pBiom->fRootWeight;

		//======================================================================================
		//                              Potential Shoot and Root Growth
		//======================================================================================
		/* fPotShootGrowRate = fPotBiomGrowRate
		                                 *((double)1.0-pBiom->fRootGrowR/pBiom->fBiomGrowR);
		 fPotRootGrowRate  = fPotBiomGrowRate
		                                     *pBiom->fRootGrowR/pBiom->fBiomGrowR;*/

		// Im Original-CERES steht folgende Berechnung:

		self->fPotRootGrowRate = self->fPotBiomGrowRate*((double)60.0-pDev->fStageXCERES*(double)8.0)/(double)100.0;
		self->fPotShootGrowRate = self->fPotBiomGrowRate - self->fPotRootGrowRate;

		//====================================================================================
		//      Minimum stem weight
		//====================================================================================
		if (pDev->iStageCERES==3) {
			self->iDuration4=0;
			self->iDuration5=0;
		}

		if (pDev->iStageCERES==4)
			self->iDuration4++;
		if (self->iDuration4==1)
			pPl->pModelParam->fMinStemWeight= pBiom->fStemWeight;

		if (pDev->iStageCERES==5)
			self->iDuration5++;

		/*  SG 2005 11 03: verlegt nach Phasic_Development
		        //====================================================================================
		        //      Grain number and weight
		        //====================================================================================
		        if (iDuration5==1)
		        {
		                pPl->pPltNitrogen->fVegActConc  =pPl->pPltNitrogen->fTopsActConc;
		                pPl->pPltNitrogen->fVegMinConc  =pPl->pPltNitrogen->fTopsMinConc;

		            pPl->pCanopy->fPlantGreenLA=pPl->pCanopy->fPlantLA
		                                                                                -pPl->pCanopy->fPlantSenesLA;

		                fStemWeight =pBiom->fStemWeight/((double)10.0*pMa->pSowInfo->fPlantDens);

		        pCan->fGrainNum = fStemWeight*pPl->pGenotype->fStorageOrganNumCoeff;
		        pCan->fGrainNumSq       = pCan->fGrainNum*pMa->pSowInfo->fPlantDens;

		        if (pCan->fGrainNumSq<(double)100.0)
		                {
		                pCan->fGrainNumSq       =(double)1.0;
		                pCan->fEarNum           =(double)0.0;
		                }

		//      pBiom->fGrainWeight=(double)0.0035*pCan->fGrainNum;
		        pBiom->fGrainWeight=(double)0.0035*pCan->fGrainNum*(double)10.0*pMa->pSowInfo->fPlantDens; //SG 2005 11 03
		        } */
	}//end wheat, barley

	//MAIZE
	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {

		pBiom->fRootWeight  += (double)0.5*pBiom->fRootGrowR -
		                       (double)0.005*pBiom->fRootWeight;
		pBiom->fStemWeight  += pBiom->fStemGrowR;
		pBiom->fLeafWeight  += pBiom->fLeafGrowR;
		self->fEarWeight                      += self->fEarGrowR;
		pBiom->fGrainWeight += pBiom->fGrainGrowR;

		pBiom->fStovWeight  = pBiom->fLeafWeight + pBiom->fStemWeight + self->fEarWeight;

		pBiom->fBiomassAbvGround = pBiom->fLeafWeight + pBiom->fStemWeight +
		                           self->fEarWeight + pBiom->fGrainWeight;
		pBiom->fTotalBiomass = pBiom->fBiomassAbvGround + pBiom->fRootWeight;
	}


	//SUNFLOWER
	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		//after anthesis 0.5% of root weight is lost daily as a result of root senescence
		if((pPl->pDevelop->iStageCERES>=4)&&(pPl->pDevelop->iStageCERES<=6))
			pBiom->fRootWeight  += pBiom->fRootGrowR - (double)0.005*pBiom->fRootWeight;
		else
			pBiom->fRootWeight  += pBiom->fRootGrowR;

		pBiom->fLeafWeight  += pBiom->fLeafGrowR;
		pBiom->fStemWeight  += pBiom->fStemGrowR;
		self->fHeadWeight                     += self->fHeadGrowR;
		self->fPericarpWeight         += self->fPericarpGrowR;
		self->fEmbryoWeight           += self->fEmbryoGrowR;
		pBiom->fGrainWeight = self->fEmbryoWeight + self->fPericarpWeight;

		pBiom->fStovWeight  = pBiom->fLeafWeight + pBiom->fStemWeight + self->fHeadWeight;

		pBiom->fBiomassAbvGround = pBiom->fLeafWeight + pBiom->fStemWeight + self->fHeadWeight +
		                           self->fPericarpWeight + pBiom->fGrainWeight;

		if(pDev->iStageCERES <= 2)
			pBiom->fGreenLeafWeight = pBiom->fLeafWeight;

		//calculate Oil percent in the grain
		if(pDev->iStageCERES == 5)
			self->fOilPercent = (double)100*self->fOil/pBiom->fGrainWeight;


	}//end Sunflower

	xpn->pPl->pBiomass->fFruitWeight = pBiom->fGrainWeight;
	return RET_SUCCESS;
}

int ceres_CropMaintenance_run(ceres *self)
{
	return RET_SUCCESS;
}

int ceres_BiomassPartition_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)||(strcmp(pPl->pGenotype->acCropCode,"WH")==0))
		ceres_BiomassPartition_WH_BA(self);

	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
		ceres_BiomassPartition_MZ(self);

	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0)
		ceres_BiomassPartition_SF(self);


	return RET_SUCCESS;
}

int ceres_BiomassPartition_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);	

	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	double                  AWR,PTF,RGFILL;//EGFT,CLG,PLAGMS;
	PDEVELOP            pDev    =pPl->pDevelop;
	PBIOMASS            pBiom   =pPl->pBiomass;
	PCANOPY                     pCan    =pPl->pCanopy;
	PMODELPARAM         pPltMP  =pPl->pModelParam;
	PPLTWATER           pPltW   =pPl->pPltWater;
	PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;	
	PSTAGEPARAM             pStageParam=pPl->pGenotype->pStageParam;

	RGFILL=0.0;
	PTF=0.0;
	AWR=0.0;

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)    return 0;

	//For Barley special factor
	if (self->weather.fDaylySolRad<=(double)10.0)
		pPltMP->fBarleyGrwParam=(double)(1.0-pow(((double)self->weather.fDaylySolRad-10.0),2.0)
		                                 *(double)pMa->pSowInfo->fPlantDens*2.5E-5);
	else
		pPltMP->fBarleyGrwParam=(double)1.0;

	switch (pDev->iStageCERES) {
	case 1:
		//=================================================================================
		//Stage 1. Emergence-Terminal Spikelet: Leaf, Root Growth
		//=================================================================================
		if (pDev->fDTT<=(double)0.0)    PTF=(double)0.5;

		//Assimilation area to weight ratio (AWR m2/kg)
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)//for Wheat
			/* Enli*/       //  AWR=(double)80.0-(double)0.075*pDev->fCumTDU;
			//  AWR=(double)150.0-(double)0.075*pDev->fCumTDU;
			// INRA-CERES
			AWR=(double)240.0-(double)0.075*pDev->fCumTDU;

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)           //for Barley
			/* Enli         AWR=((double)80.0-((double)0.075)*pDev->fCumTDU)*((double)1.1);*/
			//      AWR=((double)150.0-((double)0.075)*pDev->fCumTDU)*((double)1.1);
			// INRA-CERES
			AWR=(double)240.0-(double)0.075*pDev->fCumTDU;


		AWR = AWR * (double)0.1;

		/*      SG 2005 11 03: auskommentiert, weil bereits am Vortag in PlantLeafArea_WH_BA berechnet
		                        //==================================================================================
		                        //      Effect of Temperature on Leaf Extension Growth (EGFT)
		                        //==================================================================================
		                        EGFT=(double)1.0;

		                        if (pDev->iStageCERES<=2)
		                                {
		                                pWth->fTempAve=(pWth->fTempMax+pWth->fTempMin)/((double)2.0);

		                                EGFT=(double)1.2-(double)0.0042*(pWth->fTempAve-(double)17.0)
		                                                                                         *(pWth->fTempAve-(double)17.0);

		                                EGFT = max((double)0.0, min((double)1.0,EGFT));
		                                }

		                //Plant leaf area growth rate on the main stem (PLAGMS: cm2/plant.day)
		                        CLG       =(double)7.5;
		                        PLAGMS=CLG*(double)sqrt(pDev->fCumPhyll)*pCan->fLeafAppearR
		                                                                                *min(pPltW->fStressFacLeaf,min(EGFT,pPltN->fNStressLeaf));

		            //Plant leaf area growth rate (cm2/plant.day)
		                        pCan->fLAGrowR=PLAGMS*((double)0.3+(double)0.7*pCan->fTillerNum);

		                        //Area growth rate(cm2/plant.day) are calculated based on 900 tillers
		                        //when tiller number more than 900
		                        if (pCan->fTillerNumSq>(double)900.0)
		                                pCan->fLAGrowR=PLAGMS*((double)900.0)/pMa->pSowInfo->fPlantDens;

		            //Leaf Area Growth Rate (m2/ha.day)
		            pCan->fLAGrowR *= pMa->pSowInfo->fPlantDens;

		 */
		//Leaf and root biomass growth rate  (kg/ha.day)
		pBiom->fLeafGrowR=pCan->fLAGrowR/AWR;
		pBiom->fRootGrowR=pBiom->fBiomGrowR-pBiom->fLeafGrowR;

		//Regulation of  Plant Leaf and Root Growth Rate based on Seed Reserv
		if (pBiom->fRootGrowR<=(double)0.35*pBiom->fBiomGrowR) {
			pBiom->fRootGrowR=(double)0.35*pBiom->fBiomGrowR;

			if (pBiom->fSeedReserv>(double)0.0)
				pBiom->fSeedReserv += pBiom->fBiomGrowR
				                      -pBiom->fLeafGrowR
				                      -pBiom->fRootGrowR;

			if (pBiom->fSeedReserv<(double)0.0)     pBiom->fSeedReserv=(double)0.0;

			if (pBiom->fSeedReserv==(double)0.0) {
				pBiom->fLeafGrowR       =pBiom->fBiomGrowR*((double)0.65);
				pCan->fLAGrowR=pBiom->fLeafGrowR*AWR;
			}
		}

		pBiom->fStemGrowR       =(double)0.0;
//                      pBiom->fEarGrowRate     =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;
		//=================================================================================
		//Plant Top Fraction (%)
		//=================================================================================
		PTF = pBiom->fLeafGrowR/(pBiom->fBiomGrowR+pBiom->fSeedReserv
		                         +(double)1.0E-9);

		break;

	case 2:
		//=================================================================================
		//Stage 2. Terminal Spikelet-End of Leaf Growth: Leaf, Root & Stem Growth
		//=================================================================================
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)            //for Wheat
			PTF=(double)0.70+((double)0.1)*min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)                           //for Barley
			PTF=(double)0.70+((double)0.1)
			    *min(pPltW->fStressFacPhoto,min(pPltN->fNStressLeaf,pPltMP->fBarleyGrwParam));

		//Root growth rate (g/plant.day)
		pBiom->fRootGrowR=((double)1.0-PTF)*pBiom->fBiomGrowR;

		//Stem growth rate (g/plant.day)
		//              pBiom->fStemGrowR=((double)0.15+(double)0.12*pDev->fSumDTT
		//                                                                      //        /pPl->pGenotype->fPhyllochronInterval)
		//                                                                                /fPHINT)
		//                                                                                *pBiom->fBiomGrowR*PTF;
		//INRA-Ceres:
		pBiom->fStemGrowR=((double)0.15+(double)0.15*pDev->fSumDTT/self->fPHINT)*pBiom->fBiomGrowR*PTF;

		//Leaf growth rate (g/plant.day)
		pBiom->fLeafGrowR=pBiom->fBiomGrowR-pBiom->fRootGrowR
		                  -pBiom->fStemGrowR;
//                      pBiom->fEarGrowRate     =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;

		break;

	case 3:
		//=================================================================================
		//Stage 3: End of Leaf Growth - End of Pre-Anthesis Ear Growth: Root Stem Growth
		//=================================================================================
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) //for Wheat
			PTF=(double)0.75+((double)0.1)*min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)           //for Barley
			PTF=(double)0.75+((double)0.1)
			    *min(pPltW->fStressFacPhoto,min(pPltN->fNStressLeaf,pPltMP->fBarleyGrwParam));

		//Biomass Growth Rate
		pBiom->fRootGrowR       =((double)1.0-PTF)*pBiom->fBiomGrowR;
		pBiom->fStemGrowR       =pBiom->fBiomGrowR*PTF;

		pBiom->fLeafGrowR       =(double)0.0;
//                      pBiom->fEarGrowRate     =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;

		break;

	case 4:
		//=================================================================================
		//Stage 4: End of Pre-Anthesis Ear Growth - Begin of Grain Filling: Root Stem Growth
		//=================================================================================
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) //for Wheat
			PTF=(double)0.80+((double)0.1)*min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)           //for Barley
			PTF=(double)0.80+((double)0.1)
			    *min(pPltW->fStressFacPhoto,min(pPltN->fNStressLeaf,pPltMP->fBarleyGrwParam));

		//Biomass Growth Rate
		pBiom->fRootGrowR       =pBiom->fBiomGrowR*((double)1.0-PTF);
		pBiom->fStemGrowR       =pBiom->fBiomGrowR*PTF;

		pBiom->fLeafGrowR       =(double)0.0;
//                      pBiom->fEarGrowRate     =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;

		break;

	case 5:
		//=================================================================================
		//Grain Filling Stage: Grain Growth
		//=================================================================================
		PTF=pPltMP->fMinStemWeight/(pBiom->fStemWeight+(double)1.0E-9)
		    *(double)0.35+(double)0.65;

		if (PTF>(double)1.0)    PTF=(double)1.0;


		//while (i<5) pStageParam=pStageParam->pNext;

		if (pBiom->fBiomGrowR==(double)0.0)
			pDev->fSumDTT=pStageParam->afThermalTime[4];

		//Biomass Growth Rate
		pBiom->fBiomGrowR=pBiom->fBiomGrowR
		                  *((double)1.0-((double)1.2-
		                                 (double)0.8*pPltMP->fMinStemWeight/pBiom->fStemWeight)
		                    *(pDev->fSumDTT+(double)100.0)/
		                    (pStageParam->afThermalTime[4]+(double)100.0));

		pBiom->fRootGrowR=pBiom->fBiomGrowR*((double)1.0-PTF);

		//Relative Grain Filling Rate (RGFILL)
		if (self->weather.fTempAve<=(double)10.0)
			RGFILL=((double)0.065)*self->weather.fTempAve;

		if (self->weather.fTempAve>(double)10.0)
			RGFILL=  (double)0.65
			         +((double)0.0787-(double)0.00328*(self->weather.fTempMax-self->weather.fTempMin))
			         *(double)pow(((double)self->weather.fTempAve-10.0),0.8);

		if (RGFILL>(double)1.0)
			RGFILL=(double)1.0;

		//Grain Growth Rate (g/plant.day)
		//SG/14/06/99:
		//pBiom->fGrainGrowR=RGFILL*pCan->fGrainNum*pPl->pGenotype->fGrainFillCoeff*(double)0.001;
		pBiom->fGrainGrowR=RGFILL*pCan->fGrainNum*pPl->pGenotype->fRelStorageFillRate*(double)0.001;

		//Grain Growth Rate (kg/ha.day)
		pBiom->fGrainGrowR *= (double)10.0*pMa->pSowInfo->fPlantDens;

		//Stem Growth Rate
		pBiom->fStemGrowR=pBiom->fBiomGrowR*PTF-pBiom->fGrainGrowR;

		/*
		                if (pBiom->fStemGrowR<(double)0.0)
		                        {
		                                if (pBiom->fStemWeight+pBiom->fStemGrowR<pPltMP->fMinStemWeight)
		                                        {
		                                pBiom->fStemGrowR       =pPltMP->fMinStemWeight-pBiom->fStemWeight;
		                                        pBiom->fGrainGrowR      = pBiom->fBiomGrowR*PTF-pBiom->fStemGrowR;
		                                        }

		                                }
		*/

		//INRA-Ceres

		if (pBiom->fStemWeight<pPltMP->fMinStemWeight) {
			pBiom->fStemGrowR       =(double)0.0;
			if(pBiom->fBiomGrowR*PTF<pBiom->fGrainGrowR)
				pBiom->fGrainGrowR      = pBiom->fBiomGrowR*PTF;
		}





		pBiom->fLeafGrowR       =(double)0.0;

		break;
	}

	//====================================================================================
	//      Organ Fraction
	//====================================================================================
	pBiom->fPartFracLeaf    = pBiom->fLeafGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracRoot    = pBiom->fRootGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracStem    = pBiom->fStemGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracFruit   = pBiom->fGrainGrowR/pBiom->fBiomGrowR;
//      pBiom->fEarFraction     = pBiom->fEarGrowRate/pBiom->fBiomGrowR;
	self->fTopFraction    = (double)1.0-pBiom->fPartFracRoot;

	return RET_SUCCESS;
}

int ceres_BiomassPartition_MZ(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	;
	int I;
	double fTempFac, fTemp;
	double fNewLeafWeight, fGRF, fExcess;

	PGENOTYPE               pGen    =pPl->pGenotype;
	PDEVELOP            pDev    =pPl->pDevelop;
	PBIOMASS            pBiom   =pPl->pBiomass;
	PCANOPY                     pCan    =pPl->pCanopy;
	PMODELPARAM         pPltMP  =pPl->pModelParam;
	PPLTWATER           pPltW   =pPl->pPltWater;
	PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;



	if (pPl->pDevelop->fStageSUCROS<(double)0.0)    return 0;

	pCan->fLAGrowR = (double)0.0;

	if(pDev->iStageCERES<=3) {
		if(pCan->fExpandLeafNum < (double)4.0) {
			//[cm^2/plant]
			pCan->fLAGrowR = (double)3.0*pCan->fExpandLeafNum*pCan->fLeafAppearR;
			pCan->fLAGrowR *= (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		} else if ((pCan->fExpandLeafNum >= (double)4.0)&&(pCan->fExpandLeafNum <= (double)12.0)) {
			pCan->fLAGrowR = (double)(3.5*pow(pCan->fExpandLeafNum,2)*pCan->fLeafAppearR);
			pCan->fLAGrowR *= (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		} else if ((pCan->fExpandLeafNum >= (double)12.0)&&(pCan->fExpandLeafNum < pPltMP->fMaxLeafNum-(double)3.0)) {
			pCan->fLAGrowR = (double)(3.5*170.0*pCan->fLeafAppearR);
			pCan->fLAGrowR *= (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		} else {
			pCan->fLAGrowR = (double)(3.5*170.0*pCan->fLeafAppearR/sqrt(pCan->fExpandLeafNum+ 5.0 -pPltMP->fMaxLeafNum));
			pCan->fLAGrowR *= (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}
	}

	//[m^2/ha]
	pCan->fPlantLA += pPl->pCanopy->fLAGrowR * pMa->pSowInfo->fPlantDens;

	switch (pDev->iStageCERES) {
	case 1: //only leaves and roots growth

		//[g/plant]
		fNewLeafWeight = (double)pow((pCan->fPlantLA/pMa->pSowInfo->fPlantDens)/267.0,1.25);//CERES-Maize, p.78
		//[kg/ha]
		fNewLeafWeight *= (double)10.0*pMa->pSowInfo->fPlantDens;

		pBiom->fLeafGrowR = (double)max(0.0,fNewLeafWeight - pBiom->fLeafWeight);
		pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR;

		//regulation based on seed reserv
		if(pBiom->fRootGrowR <= (double)0.25*pBiom->fBiomGrowR) {
			pBiom->fRootGrowR = (double)0.25*pBiom->fBiomGrowR;
			pBiom->fSeedReserv += pBiom->fBiomGrowR - pBiom->fLeafGrowR - pBiom->fRootGrowR;

			if(pBiom->fSeedReserv < (double)0.0) {
				pBiom->fSeedReserv = (double)0.0;
				pBiom->fLeafGrowR = (double)0.75*pBiom->fBiomGrowR;

				//[cm^2/plant]
				pCan->fPlantLA = (double)(267.0*pow((pBiom->fLeafWeight+pBiom->fLeafGrowR)
				                                    /(10.0*pMa->pSowInfo->fPlantDens),0.8));
				//[m^2/ha]
				pCan->fPlantLA *= pMa->pSowInfo->fPlantDens;
			}
		}


		pBiom->fStemGrowR       =(double)0.0;
		self->fEarGrowR                       =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;

		break;

	case 2: //only leaves and root Growth

		//[g/plant]
		fNewLeafWeight = (double)pow((pCan->fPlantLA/pMa->pSowInfo->fPlantDens)/267.0,1.25);//CERES-Maize, p.78
		//[kg/ha]
		fNewLeafWeight *= (double)10.0*pMa->pSowInfo->fPlantDens;
		pBiom->fLeafGrowR = (double)max(0.0,fNewLeafWeight - pBiom->fLeafWeight);

		if(pBiom->fLeafGrowR >= (double)0.75*pBiom->fBiomGrowR) {
			pBiom->fLeafGrowR = (double)0.75*pBiom->fBiomGrowR;

			//[cm^2/plant]
			pCan->fPlantLA = (double)(267.0*pow((pBiom->fLeafWeight+pBiom->fLeafGrowR)
			                                    /(10.0*pMa->pSowInfo->fPlantDens),0.8));
			//[m^2/ha]
			pCan->fPlantLA *= pMa->pSowInfo->fPlantDens;
		}

		self->fTasselLeafNum = pCan->fExpandLeafNum;

		pBiom->fRootGrowR       = pBiom->fBiomGrowR - pBiom->fLeafGrowR;
		pBiom->fStemGrowR       = (double)0.0;
		self->fEarGrowR                       = (double)0.0;
		pBiom->fGrainGrowR      = (double)0.0;

		break;

	case 3://Leaf, Root and Stem Growth

		//[g/plant]
		pBiom->fLeafGrowR = (double)0.00116 * pCan->fLAGrowR *
		                    (double)pow(pCan->fPlantLA/pMa->pSowInfo->fPlantDens,0.25);
		//[kg/ha]
		pBiom->fLeafGrowR *= (double)10.0*pMa->pSowInfo->fPlantDens;

		if(pCan->fExpandLeafNum < pPltMP->fMaxLeafNum - (double)3.0) {
			pBiom->fStemGrowR = pBiom->fLeafGrowR * (double)0.0182 *
			                    (double)pow(pCan->fExpandLeafNum-self->fTasselLeafNum,2.0);
		} else {
			//[kg/ha]
			pBiom->fStemGrowR = (double)(3.1*3.5)*pCan->fLeafAppearR*(double)10.0*pMa->pSowInfo->fPlantDens;
			pBiom->fStemGrowR *= (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}

		pBiom->fRootGrowR = pBiom->fBiomGrowR-pBiom->fLeafGrowR-pBiom->fStemGrowR;

		//Regulation
		if(pBiom->fRootGrowR <= (double)0.10*pBiom->fBiomGrowR) {
			if((pBiom->fLeafGrowR>(double)0.0)||(pBiom->fStemGrowR>(double)0.0)) {
				fGRF = (double)0.92*pBiom->fBiomGrowR/(pBiom->fLeafGrowR+pBiom->fStemGrowR);
				pBiom->fRootGrowR = (double)0.10*pBiom->fBiomGrowR;
			} else
				fGRF = (double)0.0;

			pBiom->fLeafGrowR *= fGRF;
			pBiom->fStemGrowR *= fGRF;
		}

/*		// upper limit for root growth
		if(self->iLimitRootGrowthPhase3 != 0)
		{
			if(pBiom->fRootGrowR > (double)0.4 * pBiom->fBiomGrowR)
			{
				pBiom->fRootGrowR = (double)0.4*pBiom->fBiomGrowR;
				fGRF = (double)0.6*pBiom->fBiomGrowR/(pBiom->fLeafGrowR+pBiom->fStemGrowR);
				printf("%f %f %f %f %f \n", fGRF, pBiom->fLeafGrowR, pBiom->fStemGrowR, pBiom->fRootGrowR, pBiom->fBiomGrowR);
			}
			pBiom->fLeafGrowR *= fGRF;
			pBiom->fStemGrowR *= fGRF;
		}*/

		self->fEarGrowR                       =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;

		break;

	case 4: //since now ear growth, leaf growth ceases

		//[g/plant]
		self->fEarGrowR = (double)(0.22*pDev->fDTT * min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf));
		//[kg/ha]
		self->fEarGrowR *= (double)10.0*pMa->pSowInfo->fPlantDens;

		pBiom->fStemGrowR = (double)0.4*self->fEarGrowR;
		pBiom->fRootGrowR = pBiom->fBiomGrowR-self->fEarGrowR-pBiom->fStemGrowR;

		//regulation
		if(pBiom->fRootGrowR <= (double)0.08*pBiom->fBiomGrowR) {
			if((self->fEarGrowR>(double)0.0)||(pBiom->fStemGrowR>(double)0.0)) {
				fGRF = (double)0.92*pBiom->fBiomGrowR/(self->fEarGrowR+pBiom->fStemGrowR);
				pBiom->fRootGrowR = (double)0.08*pBiom->fBiomGrowR;
			} else
				fGRF = (double)0.0;

			self->fEarGrowR                 *= fGRF;
			pBiom->fStemGrowR *= fGRF;
		}


		pBiom->fLeafGrowR       =(double)0.0;
		pBiom->fGrainGrowR      =(double)0.0;

		self->fSumPS += pBiom->fBiomGrowR;
		self->iDurP++;


		break;

	case 5: //grain filling


		pBiom->fRelGrainFillR = (double)0.0;

		for (I=1; I<=8; I++) {
			fTempFac=(double)(0.931+0.114*I-0.0703*I*I+0.0053*I*I*I);
			fTemp=self->weather.fTempMin+fTempFac*(self->weather.fTempMax-self->weather.fTempMin);

			if(fTemp > (double)6.0)
				pBiom->fRelGrainFillR += (double)((1.0-0.0025*pow(fTemp-26.0,2.0))/8.0);
		}

		// [g/plant/d]
		pBiom->fGrainGrowR = pBiom->fRelGrainFillR * pCan->fGrainNum *
		                     pGen->fRelStorageFillRate * (double)(0.001*(0.45+0.55*pPltW->fStressFacPhoto));
		// [kg/ha]
		pBiom->fGrainGrowR *= pMa->pSowInfo->fPlantDens * (double)10.0;

		fExcess = pBiom->fBiomGrowR - pBiom->fGrainGrowR;

		//regulation, based on stem and leaves reserve
		if(fExcess >= (double)0.0) {
			pBiom->fStemGrowR = (double)0.5*fExcess;
			pBiom->fRootGrowR = (double)0.5*fExcess;
			pBiom->fLeafGrowR = (double)0.0;
		} else {
			pBiom->fStemGrowR = fExcess;
			pBiom->fRootGrowR = (double)0.0;

			if(pBiom->fStemWeight - fExcess >= pPltMP->fMinStemWeight*(double)1.07)
				pBiom->fLeafGrowR = (double)0.0;
			else {
				if(pBiom->fLeafWeight > pPltMP->fMinLeafWeight) {
					pBiom->fStemGrowR = fExcess + pBiom->fLeafWeight * (double)0.005;
					pBiom->fLeafGrowR = -pBiom->fLeafWeight * (double)0.005;
				}

				if(pBiom->fStemWeight+pBiom->fStemGrowR < pPltMP->fMinStemWeight) {
					pBiom->fStemGrowR = pPltMP->fMinStemWeight - pBiom->fStemWeight;
					pBiom->fGrainGrowR = pBiom->fBiomGrowR;
				}
			}
		}

		if(pBiom->fStemWeight+pBiom->fStemGrowR > pPltMP->fMaxStemWeight)
			pBiom->fStemGrowR = (double)max(0.0,pPltMP->fMaxStemWeight - pBiom->fStemWeight);

		//      fEarGrowR = pBiom->fGrainGrowR;
		self->fEarGrowR = (double)0.0;


		//additional condition for end of grain filling:
		//when grain filling is very slow for two consecutive days,
		//physiological maturity occurs
		if(pBiom->fRelGrainFillR<=(double)0.1) {
			self->iSlowFillDay++;
			if(self->iSlowFillDay!=1)
				pDev->fSumDTT = (double)1000.0;
			else
				self->iSlowFillDay = 0;
		}


		break;
	}

	//====================================================================================
	//      Organ Fraction
	//====================================================================================
	pBiom->fPartFracLeaf    = pBiom->fLeafGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracRoot    = pBiom->fRootGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracStem    = pBiom->fStemGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracFruit   = pBiom->fGrainGrowR/pBiom->fBiomGrowR;
	self->fEarFraction                    = self->fEarGrowR/pBiom->fBiomGrowR;
	self->fTopFraction                    = (double)1.0-pBiom->fPartFracRoot;

	return RET_SUCCESS;
}

int ceres_BiomassPartition_SF(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	int I;
	double PLAG, fSLAX, fSLAMax, fSLAMin, LeafWeight, fMinGrowLeaf, fracCarb;
	double fTempRedFac, fTemp, fTempFac, fELOFT, fPEPE, fExcess;
	double fCPool, fCPool1, fCPool2, fPool1, fPool2, fCP, fCP2, fPoolFactor;
	double fCDemandTot, fCDemandGrain;


	PDEVELOP            pDev    =pPl->pDevelop;
	PBIOMASS            pBiom   =pPl->pBiomass;
	PCANOPY                     pCan    =pPl->pCanopy;
	PPLTWATER           pPltW   =pPl->pPltWater;
	PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;
	PSTAGEPARAM             pStageParam=pPl->pGenotype->pStageParam;

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)    return 0;

	LeafWeight = pBiom->fLeafWeight/pMa->pSowInfo->fPlantDens * (double)0.1; // [g/plant]
	fSLAX = (double)(1.0/sqrt(2.77E-5 - 2.007E-7 * LeafWeight));// [cm^2/g]
	fSLAMax = (double)1.4 * fSLAX;
	fSLAMin = (double)1.0 * fSLAX;

	//calcualte a temperature reduction factor for head, stem and pericarp growth:
	fTempRedFac = (double)0.0;
	for (I=1; I<=8; I++) {
		fTempFac=(double)(0.931+0.114*I-0.0703*I*I+0.0053*I*I*I);
		fTemp=self->weather.fTempMin+fTempFac*(self->weather.fTempMax-self->weather.fTempMin);

		if((fTemp<(double)4.0)||(fTemp>(double)45.0))
			fELOFT = (double)0.0;
		else if(fTemp<(double)17.0)
			fELOFT = (double)((fTemp-4.0)/13.0);
		else if(fTemp<(double)31.0)
			fELOFT = (double)1.0;
		else
			fELOFT = (double)((45.0 - fTemp)/14.0);

		fTempRedFac = fTempRedFac+ fELOFT/(double)8.;
	}


	switch (pDev->iStageCERES) {
	case 1:
	case 2: //biomass partitioning rules do not change between stage 1 and stage 2
		if(pCan->fLeafTipNum < (double)1.0)
			PLAG = (double)2.0; //cm^2/plant/day
		else
			PLAG = ceres_IncLA_SF(self,pCan->fExpandLeafNum, pCan->fLeafTipNum, self->weather.fTempMax, self->weather.fTempMin);

		PLAG = self->fRFR * PLAG;

		if(pPltW->fStressFacLeaf<(double)1.0)
			fSLAMax = fSLAMin;

		pBiom->fMaxLeafGrowR = PLAG / fSLAMin * pMa->pSowInfo->fPlantDens * (double)10.0; // [kg/(ha*day)]
		fMinGrowLeaf                     = PLAG / fSLAMax * pMa->pSowInfo->fPlantDens * (double)10.0;

		//fraction of assimilates available for leaf growth
		fracCarb = (double)0.57*pBiom->fBiomGrowR;

		//assign a maximum of 57% of accumulated drymatter to leaf growth
		if(pBiom->fMaxLeafGrowR <= fracCarb) {
			pBiom->fLeafGrowR = pBiom->fMaxLeafGrowR;
			pCan->fLAGrowR = pBiom->fMaxLeafGrowR * fSLAMin*(double)0.1; //[m^2/ha]
		} else if(fMinGrowLeaf > fracCarb) {
			pBiom->fLeafGrowR = fracCarb;
			pCan->fLAGrowR = pBiom->fLeafGrowR * fSLAMax*(double)0.1; //[m^2/ha]
		} else {
			pBiom->fLeafGrowR = fracCarb;
			pCan->fLAGrowR = PLAG * pMa->pSowInfo->fPlantDens; //[m^2/ha]
		}

		pBiom->fLeafGrowR =
		    pBiom->fLeafGrowR*(double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);

		pBiom->fStemGrowR = (double)0.245 * pBiom->fBiomGrowR;
		pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR- pBiom->fStemGrowR;

		pCan->fLAGrowR =
		    pCan->fLAGrowR*(double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);

		break;


	case 3:

		PLAG = ceres_IncLA_SF(self,pCan->fExpandLeafNum, pCan->fLeafTipNum, self->weather.fTempMax, self->weather.fTempMin);

		PLAG = self->fRFR * PLAG;//cm^2/plant/day

		if(pPltW->fStressFacLeaf<(double)1.0)
			fSLAMax = fSLAMin;

		pBiom->fMaxLeafGrowR = PLAG / fSLAMin * pMa->pSowInfo->fPlantDens * (double)10.0; // [kg/(ha*day)]
		fMinGrowLeaf = PLAG / fSLAMax * pMa->pSowInfo->fPlantDens * (double)10.0;

		if(pDev->fSumDTT < (double)150.0) {
			//fraction of assimilates available for leaf growth
			fracCarb = (double)0.57*pBiom->fBiomGrowR;

			//assign a maximum of 57% of accumulated drymatter to leaf growth
			if(pBiom->fMaxLeafGrowR <= fracCarb) {
				pBiom->fLeafGrowR = pBiom->fMaxLeafGrowR;
				pCan->fLAGrowR = pBiom->fMaxLeafGrowR * fSLAMin*(double)0.1; //[m^2/ha]
			} else if(fMinGrowLeaf > fracCarb) {
				pBiom->fLeafGrowR = fracCarb;
				pCan->fLAGrowR = pBiom->fLeafGrowR * fSLAMax*(double)0.1; //[m^2/ha]
			} else {
				pBiom->fLeafGrowR = fracCarb;
				pCan->fLAGrowR = PLAG * pMa->pSowInfo->fPlantDens; //[m^2/ha]
			}

			pBiom->fLeafGrowR =
			    pBiom->fLeafGrowR*(double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);

			pBiom->fStemGrowR = (double)0.245 * pBiom->fBiomGrowR;
			pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR- pBiom->fStemGrowR;

			pCan->fLAGrowR =
			    pCan->fLAGrowR*(double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}

		else {
			//assign a maximum of 60.5% of accumulated drymatter to stem growth
			pBiom->fMaxStemGrowR = (double)0.605 * pBiom->fBiomGrowR; //[kg/ha]

			if((pDev->fSumDTT > pStageParam->afThermalTime[2]-(double)180.0)
			   &&(self->fHeadWeight == (double)0.0)) {
				self->fHeadWeight = (double)0.05*pBiom->fStemWeight;
				self->fPotHeadWeight = (double)(22.1*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]
				pBiom->fStemWeight -= self->fHeadWeight;
				self->iJPEPE = 1;
			}


			if(self->fHeadWeight > (double)0.0)
				self->fHeadGrowR = (double)(1.71*fTempRedFac*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]
			else {
				self->fHeadGrowR = (double)0.0;
				pBiom->fMaxStemGrowR = fTempRedFac * pBiom->fMaxStemGrowR;
			}

			//steht nicht in Oilcrop-Sun!
			if(self->fHeadGrowR > pBiom->fBiomGrowR)
				self->fHeadGrowR = pBiom->fBiomGrowR;


			//fraction of assimilates available for leaf growth
			fracCarb = (double)0.90*pBiom->fBiomGrowR-pBiom->fMaxStemGrowR-self->fHeadGrowR;
			//limit the amount of assimilates for leaf growth to less than 29.5%
			//of assimilation rate
			if(fracCarb > (double)0.295*pBiom->fBiomGrowR)
				fracCarb = (double)0.295 * pBiom->fBiomGrowR;

			if(fracCarb <= (double)0.0) {
				fracCarb = (double)0.0;
				pBiom->fMaxStemGrowR = (double)max(0.0,0.90*pBiom->fBiomGrowR-self->fHeadGrowR);
				pBiom->fLeafGrowR = (double)0.0;
				pCan->fLAGrowR  = (double)0.0;
			} else if(pBiom->fMaxLeafGrowR <= fracCarb) {
				pBiom->fLeafGrowR = pBiom->fMaxLeafGrowR;
				pCan->fLAGrowR =  pBiom->fMaxLeafGrowR * fSLAMin*(double)0.1; //[m^2/ha]
			} else if(fMinGrowLeaf > fracCarb) {
				pBiom->fLeafGrowR = fracCarb;
				pCan->fLAGrowR =  pBiom->fLeafGrowR * fSLAMax*(double)0.1; //[m^2/ha]
			} else
				pCan->fLAGrowR =  PLAG * pMa->pSowInfo->fPlantDens; // [m^2/ha]


			pCan->fLAGrowR = pCan->fLAGrowR*(double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			pBiom->fLeafGrowR = pBiom->fLeafGrowR*(double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			self->fHeadGrowR = self->fHeadGrowR * (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			pBiom->fStemGrowR = pBiom->fMaxStemGrowR*(double)min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto);
			pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR - pBiom->fStemGrowR - self->fHeadGrowR;



		}


		//if thermal time from now to first anthesis is less than 130 °C-days,
		//then sum dry matter accumulation rate to fSumPS and count time
		if(pDev->fSumDTT > pStageParam->afThermalTime[2] - (double)130.0) {
			self->fSumPS += pBiom->fBiomGrowR;
			self->iDurP++;
		}

		break;

	case 4:

		//calculate potential pericarp and head growth as affected by temperature
		self->fPericarpGrowR = self->fPotPericarpGrowR * fTempRedFac;
		self->fHeadGrowR = (double)(1.71*fTempRedFac*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]

		if(self->fPericarpGrowR > pBiom->fBiomGrowR) {
			self->fPericarpGrowR = pBiom->fBiomGrowR;
			self->fHeadGrowR = (double)0.0;
		} else if (self->fPericarpGrowR + self->fHeadGrowR > pBiom->fBiomGrowR)
			self->fHeadGrowR = pBiom->fBiomGrowR - self->fPericarpGrowR;

		fExcess = (double)max(0.0,pBiom->fBiomGrowR - self->fPericarpGrowR - self->fHeadGrowR);


		if(pBiom->fStemWeight < self->fStemWeightMax) {
			pBiom->fStemGrowR = fExcess;
			pBiom->fRootGrowR = (double)0.0;
		} else {
			pBiom->fStemGrowR = (double)0.0;
			pBiom->fRootGrowR = fExcess;
		}

		pBiom->fLeafGrowR = (double)0.0;
		self->fHeadGrowR = self->fHeadGrowR * (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		self->fPericarpGrowR = self->fPericarpGrowR * (double)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		pBiom->fStemGrowR = pBiom->fStemGrowR*(double)min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto);
		pBiom->fRootGrowR = pBiom->fBiomGrowR-self->fHeadGrowR-self->fPericarpGrowR-pBiom->fStemGrowR;

		//calculate amount of nitrogen demanded by grain growth
		ceres_NitrogenGrain_SF(self);


		self->fSumPS += pBiom->fBiomGrowR;
		self->iDurP++;

		break;

	case 5:
		//calculate potential pericarp and head growth as affected by temperature
		if(pDev->fSumDTT < pStageParam->afThermalTime[3]+(double)180.0)
			self->fPericarpGrowR = self->fPotPericarpGrowR*fTempRedFac*(double)(0.70+0.30*pPltW->fStressFacPhoto);
		else
			self->fPericarpGrowR = (double)0.0;

		if(self->fHeadWeight < self->fHeadWeightMax)
			self->fHeadGrowR = (double)(1.71*fTempRedFac*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]
		else
			self->fHeadGrowR = (double)0.0;

		//calculate embryo growth rate [mg/embryo/day] as affected by temperature,
		//a genetic constant, water deficit and the growth factor based on head weight
		fPEPE = self->fGrowFactor * fTempRedFac * pPl->pGenotype->fRelStorageFillRate *(double)(0.70+0.30*pPltW->fStressFacPhoto);

		//calculate embryo growth rate [kg/ha/day]
		if(pCan->fGrainNum>(double)0.0)
			self->fEmbryoGrowR = pCan->fGrainNum * fPEPE * pMa->pSowInfo->fPlantDens * (double)0.01;
		else
			self->fEmbryoGrowR = self->ppp * fPEPE * pMa->pSowInfo->fPlantDens * (double)0.01;


		//calculated carbohydrate demand for total growth and grain growth
		fCDemandTot   = self->fEmbryoGrowR + self->fPericarpGrowR + self->fHeadGrowR;
		fCDemandGrain = self->fEmbryoGrowR + self->fPericarpGrowR;

		if(pBiom->fBiomGrowR > fCDemandTot) {
			//If ->fBiomGrowR exceeds the biomass required for both grain and head
			//growth then the excess biomass is allocated to stem or roots
			if(pBiom->fStemWeight < self->fStemWeightMax) {
				if(self->fStemWeightMax-pBiom->fStemWeight > pBiom->fBiomGrowR-fCDemandTot)
					pBiom->fStemGrowR = pBiom->fBiomGrowR-fCDemandTot;
				else {
					pBiom->fStemGrowR = self->fStemWeightMax - pBiom->fStemWeight;
					pBiom->fRootGrowR = pBiom->fBiomGrowR - fCDemandTot - pBiom->fStemGrowR;
				}
			} else {
				pBiom->fStemGrowR = (double)0.0;
				pBiom->fRootGrowR = pBiom->fBiomGrowR - fCDemandTot;
			}
		} else {
			pBiom->fStemGrowR = (double)0.0;
			pBiom->fRootGrowR = (double)0.0;

			if(pBiom->fBiomGrowR > fCDemandGrain) {
				//If fBiomGrowR exceeds the bimass required for grain but is insufficient
				//for both grain and head growth, then grain (pericarp+embryo) grows
				//potentially and head gets the excess biomass
				self->fHeadGrowR = pBiom->fBiomGrowR - fCDemandGrain;
			} else {
				self->fHeadGrowR = (double)0.0;

				//calculate translocatable biomass Pool [kg/ha/day]
				fCPool = (pBiom->fStemWeight - self->fStemWeightMin) +
				         (self->fHeadWeight - self->fHeadWeightMin);

				fCP = (pBiom->fStemWeight - self->fStemWeightMin)/(self->fStemWeightMax - self->fStemWeightMin);
				if(fCP > (double)0.3)
					fCP2 = (double)1.0;
				else
					fCP2 = (double)0.0;

				fCPool1 = (double)(0.79 * fCP2 * pMa->pSowInfo->fPlantDens * 10.0);


				fCP = (self->fHeadWeight - self->fHeadWeightMin)/(self->fHeadWeightMax - self->fHeadWeightMin);
				if(fCP > (double)0.3)
					fCP2 = (double)1.0;
				else
					fCP2 = (double)0.0;

				fCPool2 = (double)(0.42 * fCP2 * pMa->pSowInfo->fPlantDens * 10.0);

				fCPool = fCPool1 + fCPool2;



				if(fCPool > (fCDemandGrain - pBiom->fBiomGrowR)/(double)0.44) {
					//If fCPOOl is enough to satisfy the demand for biomass for use in growth,
					//calculate export reuirements from head and stem and resulting organ
					//biomass changes assuming that 100 units of biomass in fCPool are
					//equivalent to 44 units of biomass in the grain.

					fPool1 = fCPool1/fCPool;
					fPool2 = fCPool2/fCPool;

					pBiom->fStemWeight -= (self->fEmbryoGrowR + self->fPericarpGrowR -
					                       pBiom->fBiomGrowR)/(double)0.44 * fPool1;
					self->fHeadWeight -= (self->fEmbryoGrowR + self->fPericarpGrowR -
					                      pBiom->fBiomGrowR)/(double)0.44 * fPool2;
				} else {
					// If fCPool is not enough to satisfy the biomass requirement for growth,
					//calculate the effects of exporting all available labile biomass on
					//organ dry weight and grain growth.
					if(fCDemandGrain > (double)(0.1*pMa->pSowInfo->fPlantDens * 10.0))
						fPoolFactor = (fCPool*(double)0.44 + pBiom->fBiomGrowR)/fCDemandGrain;
					else
						fPoolFactor = (double)0.0;

					self->fEmbryoGrowR *= fPoolFactor;
					self->fPericarpGrowR *= fPoolFactor;

					pBiom->fStemWeight -= fCPool1;
					self->fHeadWeight -= fCPool2;

				} //end if (fCPool > fCDemandGrain - pBiom->fBiomGrowR)
			} //end if (pBiom->fBiomGrowR > fCDemandGrain)
		} //end if (BiomGrowR > fCDemandTot)

		//pBiom->fRootGrowR = pBiom->fBiomGrowR - fEmbryoGrowR -
		//fPericarpGrowR - fHeadGrowR - pBiom->fStemGrowR;


		ceres_NitrogenGrain_SF(self);


		self->fSumPS += pBiom->fBiomGrowR;
		self->iDurP++;

		break;
	}

	//====================================================================================
	//      Organ Fraction
	//====================================================================================
	pBiom->fPartFracLeaf    = pBiom->fLeafGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracRoot    = pBiom->fRootGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracStem    = pBiom->fStemGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracFruit   = pBiom->fGrainGrowR/pBiom->fBiomGrowR;
//      pBiom->fEarFraction     = pBiom->fEarGrowRate/pBiom->fBiomGrowR;
	self->fTopFraction    = (double)1.0-pBiom->fPartFracRoot;

	return RET_SUCCESS;
}

double ceres_IncLA_SF(ceres *self,double LeafExpandNum, double LeafTipNum, double TempMax, double TempMin)
{
	double fMaxLA, fLER, fTLNOi, fYLNOi;
	double TempAve, PLAG = (double)0.0, EGFT = (double)0.0;
//        double fTTE = (double)350.0;
	int i;

	//==================================================================================
	//      Effect of Temperature on Leaf Extension Growth (EGFT)
	//==================================================================================

	TempAve=(TempMax+TempMin)/((double)2.0);

	if((TempAve<(double)4.0)||(TempAve>(double)40.0))
		EGFT=(double)0.0;
	else if(TempAve<(double)24.0)
		EGFT = (TempAve - (double)4.0)/(double)20.0;
	else
		EGFT = ((double)40.0 - TempAve)/(double)16.0;

	EGFT = max((double)0.0, min((double)1.0,EGFT));

	//calculate leaf position where maximum individual leaf area occurs:
	fTLNOi = (double)(int)(1.54 + 0.61*self->iTLNO);

	//calculate maximum leaf area corresponding tp position fTLNOi:
	fYLNOi = (double)(150 + (fTLNOi - 6.) *74); //cm^2?


	for(i=(int)LeafExpandNum+1; i<=(int)LeafTipNum; i++) {
		if(i<=6)
			fMaxLA = (double)(i*25);
		else if (i<=(int)fTLNOi)
			fMaxLA = (double)(150 + (i-6)*74);
		else
			fMaxLA = fYLNOi - (double)(177*(i-(int)fTLNOi));

		if(fMaxLA < (double)0.0)
			fMaxLA = (double)1.0;


		fLER = fMaxLA * EGFT/(double)18.0;

		PLAG += fLER;
	}
	return PLAG;
}

int ceres_NitrogenGrain_SF(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	double fPR, fPO, fOilInc;
	double fNSink, fNSink1, fNSink2, fFracSink1, fFracSink2;
	//double fNStovActConc;
	double fNOut, fNOutLeaf;
	double fNPool, fNPool1, fNPool2;
	double fNPool1Leaf, fNPool1Stem, fNPool1Head;
	double fNPool2Leaf, fNPool2Root;
	double fNSupplyDemandR;
	//static double fOilFrac;

	PBIOMASS                pBiom = pPl->pBiomass;
	PPLTNITROGEN    pPltN = pPl->pPltNitrogen;

	//optimal N-concentration of pericarp and embryo
	self->fNEmbryoOptConc = (double)(0.0225+0.0200*self->fNFAC);
	self->fNPericarpOptConc = (double)(0.0050+0.0100*self->fNFAC);

	if((self->fEmbryoGrowR > (double)0.0)&&(pPl->pDevelop->fSumDTT>(double)230.0)) {
		fPR = (double)100.0*self->fEmbryoGrowR/(pPl->pCanopy->fGrainNum *
		                                        pPl->pGenotype->fRelStorageFillRate * pMa->pSowInfo->fPlantDens);

		if(fPR <= (double)0.7)
			fPO = self->fO1;
		else
			fPO = (double)exp(-1.4*(fPR-0.8))*self->fO1;

		//calculate oil accumulation [kg/ha/day]
		fOilInc =  fPO/(double)100.0 * self->fEmbryoGrowR * (self->fP5 - (double)170.0)/(self->fP5 - (double)230.0);
		self->fOil = self->fOil + fOilInc;
	} else {
		self->fOil = (double)0.0;
		//fOilFrac = (double)0.0;
	}


	//calculate nitrogen demand generated by grain N concentration for
	//new growth [kg(N)/ha]
	if (self->fEmbryoWeight > (double)0.0)
		fNSink1 = self->fEmbryoGrowR * self->fNEmbryoOptConc;
	else
		fNSink1 = (double)0.0;

	if(self->fPericarpWeight > (double)0.0)
		fNSink2 = self->fPericarpGrowR * self->fNPericarpOptConc;
	else
		fNSink2 = (double)0.0;

	fNSink = fNSink1 + fNSink2;

	if(fNSink != (double)0.0) {
		fFracSink1 = fNSink1/fNSink;
		fFracSink2 = fNSink2/fNSink;

		if(pPltN->fRootActConc < pPltN->fRootMinConc)
			pPltN->fRootActConc = pPltN->fRootMinConc;


		//actual N concentration
		//fNStovActConc = pPltN->fStovCont/pBiom->fStovWeight;

		//calculate leaf contribution to the two N pools
		fNPool1Leaf = (double)max(0.0,pBiom->fGreenLeafWeight *
		                          (pPltN->fLeafActConc - pPltN->fLeafMinConc));

		if(fNPool1Leaf > self->fXPEPE) {
			fNPool1Leaf -= self->fXPEPE;
			fNPool2Leaf  = self->fXPEPE;
		} else {
			fNPool2Leaf = fNPool1Leaf;
			fNPool1Leaf = (double)0.0;
		}

		//calculate stem contribution to NPool1
		fNPool1Stem = (double)max(0.0,pBiom->fStemWeight *
		                          (pPltN->fStemActConc - pPltN->fStemMinConc));

		//calculate head contribution to NPool1
		fNPool1Head = (double)max(0.0,self->fHeadWeight *
		                          (self->fNHeadActConc - self->fNHeadMinConc));

		//calculate the size of the first labile N pool in above ground parts
		//[kg(N)/ha]
		fNPool1 = fNPool1Leaf+fNPool1Stem+fNPool1Head;

		//calculate root contribution to NPool2
		fNPool2Root = (double)max(0.0,pBiom->fRootWeight *
		                          (pPltN->fRootActConc - pPltN->fRootMinConc));

		//calculate the size of the second labile N pool [kg(N)/ha]
		fNPool2 = fNPool2Leaf+fNPool2Root;

		fNPool = fNPool1 + fNPool2;


		//calculate (nitrogen supply)/(nitrogen demand) ratio
		fNSupplyDemandR = fNPool/fNSink;

		if(fNSupplyDemandR < (double)1.0)
			fNSink *= fNSupplyDemandR;

		//calculate limiting N concentration for green leaves that have lost all
		//available nitrogen
		self->fYRAT = (double)((0.009-0.0875*self->fNGreenLeafActConc)/0.9125);

		//reduce stem, head and stover N contents
		if(fNSink > fNPool1) {
			pPltN->fStemCont -= fNPool1Stem;
			self->fNHeadCont               -= fNPool1Head;
			pPltN->fStovCont -= fNPool1;

			//the amount of N that will be extracted from each organ contributing to NPool2
			//will be proportional to its labile N content
			fNOut   = fNSink - fNPool1;
			//root:
			pPltN->fRootCont -= fNOut*fNPool2Root/fNPool2;
			pPltN->fRootActConc = pPltN->fRootCont/pBiom->fRootWeight;
			//leaves:
			fNOutLeaf = fNOut*fNPool2Leaf/fNPool2 + fNPool1Leaf;
			pPltN->fLeafCont -= fNOutLeaf;

			self->fNGreenLeafCont -= fNOutLeaf-fNOutLeaf*self->fYRAT/(self->fNGreenLeafCont/pBiom->fGreenLeafWeight-self->fYRAT);
			self->fNSenesLeafCont  = pPltN->fLeafCont - self->fNGreenLeafCont;
		} else {
			pPltN->fLeafCont -= fNPool1Leaf/fNPool1*fNSink;
			self->fNGreenLeafCont  -= fNPool1Leaf/fNPool1*fNSink*((double)1.0+self->fYRAT
			                          /(self->fNGreenLeafCont/pBiom->fGreenLeafWeight-self->fYRAT));
			self->fNSenesLeafCont  = pPltN->fLeafCont - self->fNGreenLeafCont;

			pPltN->fStemCont -= fNPool1Stem/fNPool1*fNSink;
			self->fNHeadCont               -= fNPool1Head/fNPool1*fNSink;

			pPltN->fStovCont -= fNSink;
			//fNStovActConc = pPltN->fStovCont/pBiom->fStovWeight;
		}

		//actualize pericarp, embryo and grain N contens
		self->fNPericarpCont += fNSink * fFracSink2;
		self->fNEmbryoCont   += fNSink * fFracSink1;
	}

	pPltN->fGrainCont = self->fNPericarpCont + self->fNEmbryoCont;

	return RET_SUCCESS;
}

int ceres_root_length_growth_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	if (pPl->pDevelop->iStageCERES==9) {
		if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
		   ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
			pPl->pRoot->fDepth +=(double)0.1*pPl->pDevelop->fDTT;
		else
			pPl->pRoot->fDepth +=(double)0.15*pPl->pDevelop->fDTT;
	}

	if (pPl->pBiomass->fRootGrowR<=(double)0.0)
		return RET_SUCCESS;

	ceres_RootExtension(self);
	//RootExtension_SPASS(exp_p);
	ceres_spass_RootLengthDensity(self);
	return RET_SUCCESS;
}

int ceres_RootExtension(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PHEAT pHe = xpn->pHe ;

	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;

	PWATER pWa = xpn->pWa;
	int                             i,L;
	double                          fMaxRootDepthSoil,fMaxRootDepthCrop,DEPMAX,fThickness;
	double                          fDepth,fRelWc,fMoistRed,fNitroRed;

	PSLAYER                         pSL             =pSo->pSLayer->pNext;
	PSWATER                         pSWL    =pSo->pSWater->pNext;
	PHLAYER                         pSLH    =pHe->pHLayer->pNext;
	PWLAYER                         pSLW    =pWa->pWLayer->pNext;
	PCLAYER             pSLN    =pCh->pCLayer->pNext;
	PROOT                           pRT             =pPl->pRoot;


	//Constant Coefficient
	fMaxRootDepthSoil       = (double)200.0;        //cm depends on soil property
	//fMaxRootDepthCrop   = (double)150.0;  //cm depends on crop property
	//SG/20/04/99:
	//      besser
	fMaxRootDepthCrop = pPl->pGenotype->fMaxRootDepth;
	//      dann wird der Wert aus What.gtp verwendet!!!
	//      (entsprechend für fMaxRootDepthSoil?)

	DEPMAX=(double)0.0;
	pSL       =pSo->pSLayer->pNext;
	for (L=1; L<=pSo->iLayers-2; L++) {
		DEPMAX += (double)0.1*pSL->fThickness;  //cm
		pSL=pSL->pNext;
	}

	pRT->fMaxDepth=min(DEPMAX,min(fMaxRootDepthSoil,fMaxRootDepthCrop));

	//=========================================================================================
	//Root Depth Growth Rate
	//=========================================================================================
	fDepth      = (double)0.0;
	pSL             =pSo->pSLayer->pNext;
	pSLH    =pHe->pHLayer->pNext;
	pSLW    =pWa->pWLayer->pNext;
	pSWL    =pSo->pSWater->pNext;
	pSLN    =pCh->pCLayer->pNext;
	for (i=1; i<=pSo->iLayers-2; i++) {
		fThickness = (double)0.1*pSL->fThickness;

		if ((pRT->fDepth>fDepth)&&(pRT->fDepth<=fDepth+fThickness))
			break;
		fDepth += fThickness;

		if (i<pSo->iLayers-2) {

			//SG/09/04/99////////////////////////////////////////////////////////////
			//
			//      pSL bzw. pSL->pNext wird doch gar nicht gebraucht. Oder
			//      habe ich etwas übersehen?
			//      Aber: in PLAGEN wird immer der aktuelle Wassergehalt der tiefsten
			//      durchwurzelten Schicht verwendet. Deshalb muß hier pSLW auf pSLW->pNext
			//      gesetzt werden!!!
			//////////////////////////////////////////////////////////////////////////

			pSLW = pSLW->pNext;
			pSWL = pSWL->pNext;

			pSL = pSL ->pNext;
			pSLH= pSLH->pNext;
			pSLN= pSLN->pNext;
		}
	}// for

//      if ((lpBm->fRootGrowRate>(double)0.0)&&(pRT->fDepth<fMaxRootDepthSoil))
	if ((pRT->fDepth<fMaxRootDepthSoil)) {
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
		fRelWc =(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);

		if (fRelWc<(double)0.25)
			fMoistRed =max((double)0.0,(double)4.0*fRelWc);
		else
			fMoistRed = (double)1.0;

		//Soil nitrogen deficit factor for root growth
		fNitroRed = (double)((double)1.0-((double)1.17*exp((double)-0.15*
		                                  (double)(pSLN->fNO3N/pSLN->fNO3Nmgkg+pSLN->fNH4N/pSLN->fNH4Nmgkg))));

		if ((fNitroRed<(double)0.01)&&((strcmp(pPl->pGenotype->acCropCode,"BA")==0)
		                               ||(strcmp(pPl->pGenotype->acCropCode,"WH")==0)))
			fNitroRed=(double)0.01;
		else if (fNitroRed<(double)0.1)
			fNitroRed = (double)0.1;

		//SG/09/04/99//////////////////////////////////////////////////////////////////
		//
		//      In PLAGEN wird, wenn sich die Wurzeln ausschließlich in der obersten
		//      Schicht befinden, fMoistRed immer = 1.0 gesetzt!
		///////////////////////////////////////////////////////////////////////////////

		// if(i==1)
		//      fMoistRed = (double)1.0;                        //SG/05/05/99: Streß auch bei i=1!

		//pRT->fDepthGrowR = max((double)0.0, pPl->pDevelop->fDTT * (double)0.22
		//                     * min(pPl->pPltWater->fStressFacPhoto*(double)2.0,fMoistRed));//orig. CERES

		//INRA-Ceres:
		pRT->fDepthGrowR=max((double)0.0, pPl->pDevelop->fDTT * (double)0.08
		                     * min(pPl->pPltWater->fStressFacPhoto*(double)2.0,fMoistRed));


		pRT->fDepthGrowR = max((double)0.0, pPl->pGenotype->fMaxRootExtRate * fNitroRed * fMoistRed);
		/*
		        */                             //* fNitroRed * min(pPl->pPltWater->fStressFacPhoto*(double)2.0,fMoistRed));
		//ep 170304 pPl->pPltWater->fStressFacPhoto entspricht pPl->pPltWater->fSWDF1
	} else
		pRT->fDepthGrowR = (double)0.0;

	//=========================================================================================
	//Root Depth
	//=========================================================================================


	//SG/12/04/99///////////////////////////////////////////////////////////////////////////////
	//
	//      In PLAGEN wird die Wurzeltiefe erst nach der Berechnung der Wurzelverteilung
	//      aktualisiert!!
	////////////////////////////////////////////////////////////////////////////////////////////


	pRT->fDepth += pRT->fDepthGrowR;

	pRT->fDepth = min(pRT->fDepth,pRT->fMaxDepth);
	return RET_SUCCESS;
}
int ceres_spass_RootLengthDensity(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	int 	L,L1,NLAYR;
	double   fCumDepth,DEPMAX,TRLDF,RNLF,RLVF,fRelExtWater,fNewRootLength,fNewRootGrowFactor,fThickness;

	//SG 20120126
	double TRL,TRLLF, RLLR, RLLRLay;

	PSLAYER 			pSL		=pSo->pSLayer;
	PSWATER				pSWL	=pSo->pSWater;
	PWLAYER	   			pSLW	=pWa->pWLayer;
	PCLAYER     		pSLN	=pCh->pCLayer;
	PROOT				pRT		=pPl->pRoot;
	PLAYERROOT			pLR		=pPl->pRoot->pLayerRoot;
	PBIOMASS			pBm		=pPl->pBiomass;


	//if (pBm->fRootGrowR<=(double)0.0)

	////SG/12/04/99///////////////////////////////////////////////////////////////////////////////
	////
	////	In PLAGEN wird die Wurzeltiefe erst nach der Berechnung der Wurzelverteilung
	////	aktualisiert!!
	////	Die folgenden Zeilen wurden deshalb vom Ende der Funktion RootExtension_SPASS
	////	hierher verlegt.
	//////////////////////////////////////////////////////////////////////////////////////////////

	//{
	//	//	 pRT->fDepth += pRT->fDepthGrowR;

	//	//	 pRT->fDepth = min(pRT->fDepth,pRT->fMaxDepth);

	//	return 0;
	//}


	NLAYR=pSo->iLayers-2; //Anzahl Bodenschichten

	DEPMAX=(double)0.0;
	pSL	  =pSo->pSLayer->pNext;
	for (L=1; L<=NLAYR; L++) {
		DEPMAX += (double)0.1*pSL->fThickness;	//cm
		pSL=pSL->pNext;
	}

	//pRT->fMaxDepth=DEPMAX;	//SG/20/04/99: wird doch gar nicht gebraucht, oder?
	// Liefert nur falschen Wert am Ende bei Wurzellänge

	//=========================================================================================
	//		New Root Length Calculation (fNewRootLength, RLNEW)


	//SG 20111024: Werte aus spez. Wurzellänge (*.gtp, Zeile 2000080)
	fNewRootLength=pBm->fRootGrowR*pPl->pGenotype->fRootLengthRatio*(double)1E-5;

	if(pPl->pGenotype->fRootLengthRatio == (double)0.0) {
		fNewRootLength=pBm->fRootGrowR*((double)1.00)*(double)1E-1;

		//      Werte aus E.Wang, Tabelle 3.5 [cm/cm^2]
		//=========================================================================================
		if (strcmp(pPl->pGenotype->acCropCode,"SG")==0)
			fNewRootLength=pBm->fRootGrowR*((double)1.00)*(double)1E-1;
		if ((strcmp(pPl->pGenotype->acCropCode,"MI")==0)
		    ||(strcmp(pPl->pGenotype->acCropCode,"MZ")==0))
			fNewRootLength=pBm->fRootGrowR*((double)0.80)*(double)1E-1;
		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
		    ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
			fNewRootLength=pBm->fRootGrowR*((double)1.05)*(double)1E-1;
		if (strcmp(pPl->pGenotype->acCropCode,"SF")==0)
			fNewRootLength=pBm->fRootGrowR*((double)1.70)*(double)1E-1;
	}

	//=========================================================================================
	//		Factor Calculation for Root Growth
	//=========================================================================================
	fCumDepth	=(double)0.0;
	self->fSWDF3 		=(double)0.0;
	L = 0;
	TRL = 0.0;

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext;
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
	while ((fCumDepth<=pRT->fDepth)&&(L<NLAYR)) {
		L++;

		fThickness =(double)0.1*pSL->fThickness; //cm

		pSLN->fNO3Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;

		fCumDepth +=(double)0.1*pSL->fThickness; //cm
		//-------------------------------------------------------------------------------
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
		fRelExtWater=(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		if (fRelExtWater<(double)0.25)
			pLR->fWatStressRoot =max((double)0.0,(double)4.0*fRelExtWater);
		else
			pLR->fWatStressRoot = (double)1.0;

		//SG/12/04/99:	folgende Zeile aus PLAGEN:
		// if(L==1) pLR->fWatStressRoot = (double)1.0;	Streß auch bei L=1!


		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		//-------------------------------------------------------------------------------
		//Mineral nitrogen availability effect factor on root growth (pLR->fNStressRoot)
		if (TRUE) {
			pLR->fNStressRoot =(double)(1.0-(1.17*exp(-0.15*(double)(pSLN->fNO3N/pSLN->fNO3Nmgkg
			                                 +pSLN->fNH4N/pSLN->fNH4Nmgkg))));

			if ((pLR->fNStressRoot<(double)0.01)&&
			    ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
			     ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)))
				pLR->fNStressRoot=(double)0.01;
			else
				pLR->fNStressRoot=max((double)0.1,pLR->fNStressRoot);


			if((strcmp(pPl->pGenotype->acCropCode,"SF")==0)&&(pLR->fNStressRoot<(double)0.1))
				pLR->fNStressRoot = (double)0.1;
		} else
			pLR->fNStressRoot=(double)1.0;

		//SG/12/04/99:	folgende Zeile aus PLAGEN:
		//if(L==1) pLR->fNStressRoot = (double)1.0;	Streß auch bei L=1!


		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		//Root length density factor to calculate new root growth distribution
		fNewRootGrowFactor =(double)exp(-4.0*(double)((fCumDepth-(double)0.5*fThickness)/DEPMAX));
		//für Jörg (13.12.99):
		//fNewRootGrowFactor =(double)exp(-2.0*(double)((fCumDepth-(double)0.5*fThickness)/DEPMAX));
		pLR->fLengthDensFac = fNewRootGrowFactor*fThickness*min(pLR->fWatStressRoot,pLR->fNStressRoot); //[cm]
		if ((fCumDepth<pRT->fDepth)&&(L<NLAYR))
			self->fSWDF3 += (pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP)*fThickness; //[cm]

		TRL += pLR->fLengthDens*fThickness;

		if (L<NLAYR) {
			pSL =pSL ->pNext;
			pSWL=pSWL->pNext;
			pSLW=pSLW->pNext;
			pSLN=pSLN->pNext;
			pLR =pLR ->pNext;
		}
	}  // end while
	L1=L;
	//=========================================================================================
	//	Deepest root layer
	//=========================================================================================
	//if (L<=NLAYR)
	//{
	//    pSL=pSL->pBack;
	//	pLR=pLR->pBack;
	//}

	//auf Wunsch von Jörg Lippert geändert am 13.12.99
	if(fCumDepth>pRT->fDepth)
		pLR->fLengthDensFac=pLR->fLengthDensFac*((double)1.0-(fCumDepth-pRT->fDepth)/((double)0.1*pSL->fThickness));
	//pLR->fLengthDensFac=(double)0.0;


	//=========================================================================================
	//		Total Root Length Density Factor
	//=========================================================================================
	TRLDF =(double)0.0;
	pLR	  =pPl->pRoot->pLayerRoot;
	for (L=1; L<=L1; L++) {
		TRLDF += pLR->fLengthDensFac; //[cm]
		pLR    = pLR->pNext;
	}
	//=========================================================================================
	//		Root Length Density Calculation
	//=========================================================================================
	if ((TRLDF>(double)0)&&(TRLDF>=fNewRootLength*((double)0.00001)))
		RNLF = fNewRootLength/TRLDF; //[cm-2]
	else
		return 0;
		
	//printf("CERES %f %f %f\n", RNLF, fNewRootLength, TRLDF);

	//-------------------------------------------------------------------------------------
	//SG 20120126:
	// zu C) (siehe unten): Absterberate in der jeweiligen Schicht abhängig vom Wassergehalt (PLATHO)
//	TRL = pPl->pBiomass->fRootWeight*pPl->pGenotype->fRootLengthRatio*(double)1E-5; //total root length [cm/cm^2]
	RLLR = pPl->pBiomass->fRootDeathRate/pPl->pBiomass->fRootWeight*TRL; //[cm/cm^2]
//	RLLR = pPl->pBiomass->fRootDeathRate*pPl->pGenotype->fRootLengthRatio*(double)1E-5; //[cm/cm^2]
	TRLLF = 0.0; //total root length loss factor [-]

	pLR		= pPl->pRoot->pLayerRoot;
	for (L=1; L<=L1; L++) {
		//==================================
		//  Total Root Length Loss Factor
		//==================================
//        TRLLF += pLR->fLengthDens*fThickness*max(1.0-pLR->fWatStressRoot,1.0-pLR->fNStressRoot); //[cm/cm^2]
		TRLLF += pLR->fLengthDens*fThickness*(1.0-pLR->fWatStressRoot); //[cm/cm^2]
		pLR    = pLR->pNext;
	}
	//--------------------------------------------------------------------------------------

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext;
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
	fCumDepth=(double)0.0;
	
	pRT->fTotalLength = 0.0;
	
	for (L=1; L<=L1; L++) {
		//Fow Wheat and Barley
		fThickness =(double)0.1*pSL->fThickness; //cm

		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)|(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
			fCumDepth +=fThickness;


			////////////////////////////////////////////////////////////////////////////////////////
			//SG 20120126
			// Beim Absterben der Wurzeln geht auch Wurzellänge verloren!
			//
			// Original-SPASS:
			// pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-((double)0.01)*pLR->fLengthDens; //[cm/cm^3]

			//bzw.
			// RLLRLay = ((double)0.01)*pLR->fLengthDens;

			//SG 20120126:
			// zunehmendes Absterben der Wurzeln (hier: Wurzellängendichte) nach der Blüte (Seneszenz)
			// --> muestte eigentlich aus Absterberate der Wurzeln errechnet werden, am besten unter Berücksichtigung der
			// Ressourcenverfügbarkeit in den einzelnen Schichten wie in PLATHO

			// A) tägliche Absterberate von 0.01 d-1 (1%) nimmt nach der Blüte zu bis auf 0.05 d-1 (5%) zum Zeitpunkt der Abreife
			//    RLLRLay = ((double)0.01+(double)0.04*max(0.0,(pPl->pDevelop->fStageSUCROS-(double)1.0))/(double)1.0)*pLR->fLengthDens;

			// B) Absterberate in allen Schichten entsprechend des prozentualen gesamten Verlusts an Wurzelbiomasse
			//    RLLRLay = (double)max(0.0,pLR->fLengthDens*pBiom->fRootDeathRate/(pBiom->fRootWeight+EPSILON));

			// C) Absterberate in der jeweiligen Schicht abhängig von der Ressourcenverfügbarkeit (PLATHO)
			// In ungünstigen Schichten sterben mehr Wurzeln ab
	
			//if (L==23)
			//		printf("vor %d %f %f %f %f %f %f %f\n", L, fCumDepth, pLR->fLengthDens, pPl->pBiomass->fRootDeathRate, pPl->pBiomass->fRootWeight, pLR->fWatStressRoot, RLLRLay, EPSILON);

		
			if(TRLLF>0.0)
				RLLRLay = RLLR*pLR->fLengthDens*(1.0-pLR->fWatStressRoot)/TRLLF; //[cm/cm^3]
//				RLLRLay = RLLR*pLR->fLengthDens*max(1.0-pLR->fWatStressRoot,1.0-pLR->fNStressRoot)/TRLLF; //[cm/cm^3]
			else
				RLLRLay = max(0.0,pLR->fLengthDens*pPl->pBiomass->fRootDeathRate/(pPl->pBiomass->fRootWeight+EPSILON));

			//if (L==23)
			//		printf("nach %d %f %f %f %f %f %f %f\n", L, fCumDepth, pLR->fLengthDens, pPl->pBiomass->fRootDeathRate, pPl->pBiomass->fRootWeight, pLR->fWatStressRoot, RLLRLay, EPSILON);


			RLLRLay = max((double)0.01*pLR->fLengthDens,RLLRLay);
			//
			////////////////////////////////////////////////////////////////////////////////////////


			//FH 20130531 rückgängig wegen pLR->fLengthDens negativ
			//  pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(double)0.01*pLR->fLengthDens;

			//SG 20120126
			pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(double)RLLRLay;  //[m]


			if (fCumDepth<(double)115.0)
				pLR->fLengthDens=max((double)0.0,pLR->fLengthDens);
			else {
				if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)
					RLVF=(double)0.377-((double)0.0015)*fCumDepth;
				else
					RLVF=((double)0.00004)*(fCumDepth-(double)220.0)*(fCumDepth-(double)220.0);
				pLR->fLengthDens=min(RLVF,pLR->fLengthDens);
			}

		}

		//SG/17/05/99: Potato (ausPLAGEN)
		else if (strcmp(pPl->pGenotype->acCropCode,"PT")==0) {
			pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(double)0.005*pLR->fLengthDens;
			pLR->fLengthDens  = min((double)8.0, max((double)0.0,pLR->fLengthDens));
		}

		else if(strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
			if(pPl->pDevelop->iStageCERES < 4)
				pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness;
			else
				pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(double)0.005*pLR->fLengthDens;

			pLR->fLengthDens = (double)min(10.0,max(0.0,pLR->fLengthDens));
		} else { //For Other crops
			pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness
			                    -((double)0.005)*pLR->fLengthDens;
			pLR->fLengthDens=min((double)5.0, max((double)0.0,pLR->fLengthDens));
		}

		//printf("CERES %d %f \n", L, pLR->fLengthDens);

		pRT->fTotalLength += pLR->fLengthDens * fThickness;

		pSL =pSL->pNext;
		pLR =pLR->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
	}

	return RET_SUCCESS;
}

int ceres_PlantSenescense(ceres *self)
{	
	return RET_SUCCESS;
}