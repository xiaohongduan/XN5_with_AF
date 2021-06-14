#include "ceres_canopy_formation.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int ceres_canopy_formation_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PCANOPY pCan=pPl->pCanopy;
	PDEVELOP    pDev    =pPl->pDevelop;

	if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)||(strcmp(pPl->pGenotype->acCropCode,"WH")==0)) {
		ceres_PlantLeafNumber_WH_BA   (self);
		ceres_PlantLeafArea_WH_BA             (self);
		ceres_LeafSenescence_WH_BA    (self);
		ceres_Tillering_WH_BA                 (self);
		ceres_TillerDeath_WH_BA               (self);
//              ColdHardening_WH_BA             (exp_p);

		//====================================================================================
		//      LAI
		//====================================================================================
		pCan->fLAI=pCan->fPlantGreenLA*(double)0.0001;
		pCan->fLAI=max((double)0.0001,pCan->fLAI);

	}

	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
		ceres_PlantLeafNumber_MZ      (self);
		ceres_LeafSenescence_MZ       (self);

		pCan->fLAI=pCan->fPlantGreenLA*(double)0.0001;
		pCan->fLAI=max((double)0.0001,pCan->fLAI);
	}

	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		ceres_PlantLeafNumber_SF      (self);
		ceres_LeafSenescence_SF       (self);

		//Plant Leaf Area (m2/ha)
		pCan->fPlantLA += pPl->pCanopy->fLAGrowR;
		pCan->fPlantGreenLA = pPl->pCanopy->fPlantLA - pPl->pCanopy->fPlantSenesLA;
		pCan->fLAI=pCan->fPlantGreenLA*(double)0.0001;
		pCan->fLAI=max((double)0.0001,pCan->fLAI);

	}
	
	pPl->pCanopy->fPlantHeight = (double)pPl->pGenotype->fPlantHeight*
                     min(1.0, (double)pDev->fStageSUCROS)/100.0;


	return RET_SUCCESS;
}

int ceres_PlantLeafNumber_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	double              fMaxLeafNum;

	PDEVELOP    pDev    =pPl->pDevelop;
	PCANOPY     pCan    =pPl->pCanopy;

	fMaxLeafNum=(double)25.0;  //von CERES (auch INRA)

//      fMaxLeafNum     =(double)12.0;//Enli?

//      pDev->fDTT=max((double)0.0,pDev->fDTT);

	if (pDev->iStageCERES<=2) {
//              pCan->fLeafAppearR=pDev->fDTT/pPl->pGenotype->fPhyllochronInterval;
		pCan->fLeafAppearR=pDev->fDTT/self->fPHINT;
		pDev->fCumPhyll += pCan->fLeafAppearR;

		if (pCan->fExpandLeafNum>=fMaxLeafNum) //Crop Reached Maximum Leaf Number on that Day
			pCan->fExpandLeafNum=fMaxLeafNum;
		else
			pCan->fExpandLeafNum=pDev->fCumPhyll+(double)2.0;
	}

	return (int)pCan->fExpandLeafNum;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Leaf Area
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int ceres_PlantLeafArea_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	double  CLG,PLAGMS,EGFT,fLeafGrowRate;

	PDEVELOP                pDev    =pPl->pDevelop;
	PBIOMASS                pBiom   =pPl->pBiomass;
	PCANOPY                 pCan    =pPl->pCanopy;
	PPLTWATER           pPltW   =pPl->pPltWater;
	PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;

	//==================================================================================
	//      Effect of Temperature on Leaf Extension Growth (EGFT)
	//==================================================================================
	EGFT=(double)1.0;

	if (pDev->iStageCERES<=2) {
		self->weather.fTempAve=(self->weather.fTempMax+self->weather.fTempMin)/((double)2.0);

		EGFT=(double)1.2-(double)0.0042*(self->weather.fTempAve-(double)17.0)
		     *(self->weather.fTempAve-(double)17.0);

		EGFT = max((double)0.0, min((double)1.0,EGFT));
	}

	//==================================================================================
	//      Leaf Area Growth in different stages
	//==================================================================================
	switch (pDev->iStageCERES) {
	case 1:
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

		break;

	case 2:
		//Plant Leaf area growth rate (cm2/plant.day)
		fLeafGrowRate = pBiom->fLeafGrowR/((double)10.0*pMa->pSowInfo->fPlantDens);

		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)    //for Wheat
			pCan->fLAGrowR=fLeafGrowRate*((double)115.0);

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)                   //for Barley
			pCan->fLAGrowR=fLeafGrowRate*((double)127.0);


		break;

	default:
		pCan->fLAGrowR=(double)0.0;
		break;

	}


	//Leaf Area Growth Rate (m2/ha.day)
	pCan->fLAGrowR *= pMa->pSowInfo->fPlantDens;

	//Plant Leaf Area (m2/ha)
	pCan->fPlantLA += pCan->fLAGrowR;

	return RET_SUCCESS;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Leaf Senecence
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int ceres_LeafSenescence_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;

	int     I;
	double  WaterFact,NitrogFact;
	

	PDEVELOP                pDev    =pPl->pDevelop;
	PCANOPY                 pCan    =pPl->pCanopy;
	PPLTWATER           pPltW   =pPl->pPltWater;
	PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;

	WaterFact=0.0;

	if ((int)pCan->fExpandLeafNum==0) {
		for (I=0; I<=29; I++)
			self->PLSC[I]=(double)0.0;
	}

	//====================================================================================
	//      Normal Leaf Senescence
	//====================================================================================
	switch (pDev->iStageCERES) {
	case 1:
		if (pDev->fCumPhyll<=(double)4.0)
			pCan->fLALossR=(double)0.0;
		else
			pCan->fLALossR=(self->PLSC[(int)pCan->fExpandLeafNum-4]
			                -self->PLSC[(int)pCan->fExpandLeafNum-5])
			               *pCan->fLeafAppearR;

		if ((pCan->fPlantSenesLA/pCan->fPlantLA>(double)0.4)&&(pCan->fLAI<(double)6.0))
			pCan->fLALossR=(double)0.0;


		break;

	case 2:
		if (pDev->fCumPhyll>(double)4.0)
			pCan->fLALossR=(self->PLSC[(int)pCan->fExpandLeafNum-4]
			                -self->PLSC[(int)pCan->fExpandLeafNum-5])
			               *pCan->fLeafAppearR;
		if ((pCan->fPlantSenesLA/pCan->fPlantLA>(double)0.4)&&(pCan->fLAI<(double)6.0))
			pCan->fLALossR=(double)0.0;

		break;

	case 3:
		pCan->fLALossR=((double)0.0003)*pDev->fDTT
		               *pCan->fPlantGreenLA;
		break;

	case 4:
		pCan->fLALossR=((double)0.0006)*pDev->fDTT
		               *pCan->fPlantGreenLA;
		break;

	case 5:

	{
		PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;
		//      while (i<5)  pStageParam=pStageParam->pNext;

		pCan->fLALossR=pCan->fPlantGreenLA*((double)2.0)
		               *pDev->fSumDTT*pDev->fDTT/
		               (pStageParam->afThermalTime[4]*
		                pStageParam->afThermalTime[4]);
	}

	break;
	}


	//====================================================================================
	//      Cumulative Leaf Area
	//====================================================================================
	self->PLSC[(int)pCan->fExpandLeafNum]=pCan->fPlantLA;

	//====================================================================================
	//      Leaf Senescence due to Water and Nitrogen Stress
	//====================================================================================
	if (pPltW->fStressFacPhoto<=(double)0.8) {
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)  //for Wheat
			WaterFact=(double)2.0-pPltW->fStressFacPhoto/((double)0.8);

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) //for Barley
			WaterFact=(double)1.5-pPltW->fStressFacPhoto/((double)1.6);
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		NitrogFact=(double)2.0-pPltN->fNStressFac/((double)0.8);

		pCan->fLALossR *= min(WaterFact,NitrogFact);
	}


	//Dead Leaf Area (m2/ha.day)
	pCan->fPlantSenesLA += pCan->fLALossR;

	pCan->fPlantGreenLA = pCan->fPlantLA-pCan->fPlantSenesLA;

	return RET_SUCCESS;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Tillering
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int ceres_Tillering_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	double  TC1,TC2,WaterFact,NitrogFact,RTSW,fMaxTillerNumSq,fStemWeight;
	PDEVELOP            pDev    =pPl->pDevelop;
	PBIOMASS            pBiom   =pPl->pBiomass;
	PCANOPY             pCan    =pPl->pCanopy;
	PMODELPARAM     pPltMP  =pPl->pModelParam;
	PPLTWATER           pPltW   =pPl->pPltWater;
	PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;

	double G3; //Genetischer Parameter fuer SingleTillerStemWeight-Berechnung

//      G3 = (double)2.0; //Spring and Winter Barley (2 rows)
//      G3 = (double)4.0; //Spring and WinterBarley (6 rows)
//      G3 = (double)2.0; //Spring and Winter Wheat (Europe West)

	G3 = (double)2.3;

//      Umrechnung auf "physiologische" Einheiten (alter Code)
//      G3 = (double)(1.0 + (G3-1.0)*0.5);
//      G3 = (double)(-0.005 + G3*0.2);

// SG 24.11.04: aus source code CERES von INRA (readpara.for)
	G3 = (double)(-0.005 + G3*0.35);



	//Maximum Tiller number per square meter
	//fMaxTillerNumSq=(double)3000.0; von CERES
	fMaxTillerNumSq=(double)2000.0;

	//Stem weight (g/plant)
	fStemWeight = pBiom->fStemWeight /((double)10.0*pMa->pSowInfo->fPlantDens);

	switch (pDev->iStageCERES) {
	case 1:
		//Previous day's tiller number (pCan->fTillerNumOld: Tillers/plant)
		pCan->fTillerNumOld=pCan->fTillerNum;

		//Today's Tiller Number pCan->fTillerNum (Tillers/plant)
		if (pDev->fCumPhyll>=(double)2.5) {
			TC1=((double)(-2.5))+pDev->fCumPhyll;

			if (pCan->fTillerNumSq>=fMaxTillerNumSq)
				TC2=(double)0.0;
			else
				TC2=(double)(2.50E-7*pow((fMaxTillerNumSq-pCan->fTillerNumSq),3.0));

			WaterFact =(double)1.4*pPltW->fStressFacLeaf-(double)0.4;
			NitrogFact=(double)1.4*pPltN->fNStressTiller-(double)0.4;

			if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) //for Wheat
				pCan->fTillerNum += pCan->fLeafAppearR
				                    *min(WaterFact,NitrogFact)
				                    *min(TC1,TC2);

			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) //for Barley
				pCan->fTillerNum += pCan->fLeafAppearR
				                    *min(WaterFact,min(NitrogFact,pPltMP->fBarleyGrwParam))
				                    *min(TC1,TC2);
			//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		}

		if (pCan->fTillerNum<(double)1.0)
			pCan->fTillerNum=(double)1.0;

		break;

	case 2:
		//Tiller Number yesterday (Tillers/plant)
		pCan->fTillerNumOld=pCan->fTillerNum;

		//Single Tiller Weight (g/Tiller)
//                      pPltMP->fSingleTillerStemWeight += pPl->pGenotype->fStorageOrganNumCoeff
//                                                                *(double)0.0889*pDev->fDTT
//                                                                                                //*pDev->fSumDTT/(pPl->pGenotype->fPhyllochronInterval
//                                                                                                //*pPl->pGenotype->fPhyllochronInterval)
//                                                                                                *pDev->fSumDTT/(fPHINT*fPHINT)
//                                                                                                *min(pPltN->fNStressLeaf,pPltW->fStressFacPhoto);

		//SG/19/11/99:
		// In CERES wird hier der genetische Parameter G3 verwendet. fStorageOrganNumCoeff ist aber G2!
		// Da für G3 keine Expert-N-Variable vorhanden ist, wird G3 oben gesetzt und umgerechnet (vgl.
		// PlantSimInit in alter CERES-Version

		pPltMP->fSingleTillerStemWeight += G3
		                                   *(double)0.0889*pDev->fDTT
		                                   //*pDev->fSumDTT/(pPl->pGenotype->fPhyllochronInterval
		                                   //*pPl->pGenotype->fPhyllochronInterval)
		                                   *pDev->fSumDTT/(self->fPHINT*self->fPHINT)
		                                   *min(pPltN->fNStressLeaf,pPltW->fStressFacPhoto);

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) //for Barley  ?????????????
			pPltMP->fSingleTillerStemWeight=G3
			                                *(double)(1.18*10E-6)
			                                *pDev->fSumDTT*pDev->fSumDTT;
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		RTSW=fStemWeight/pPltMP->fSingleTillerStemWeight/pCan->fTillerNum;

		//Tiller Number (Tillers/plant)
		pCan->fTillerNum += pCan->fTillerNum*pDev->fDTT*(double)0.005
		                    *(RTSW-(double)1.0);

		if (pCan->fTillerNum<(double)1.0)
			pCan->fTillerNum=(double)1.0;

		break;

	case 3:


		// Wegen G3 siehe case 2:
		pPltMP->fSingleTillerStemWeight += G3
		                                   *pDev->fDTT*((double)0.25)
		                                   //         /pPl->pGenotype->fPhyllochronInterval
		                                   /self->fPHINT
		                                   *min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) //for Barley
			pPltMP->fSingleTillerStemWeight=G3
			                                *((double)0.6+(double)0.00266*pDev->fSumDTT);
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		RTSW=fStemWeight/pPltMP->fSingleTillerStemWeight/pCan->fTillerNum;

		pCan->fTillerNum += pCan->fTillerNum*pDev->fDTT*(double)0.005
		                    *(RTSW-(double)1.0);
		if (pCan->fTillerNum<(double)1.0)
			pCan->fTillerNum=(double)1.0;

		break;

	}

	//Tiller Number per square meter (Tillers/m2)
	pCan->fTillerNumSq=(pCan->fTillerNum+(double)1.0)*pMa->pSowInfo->fPlantDens;
//SG:
//      pCan->fTillerNumSq=pCan->fTillerNum*pMa->pSowInfo->fPlantDens;


	return RET_SUCCESS;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Tiller Death
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int ceres_TillerDeath_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	double          DTN;
	PDEVELOP    pDev    =pPl->pDevelop;
	PCANOPY     pCan    =pPl->pCanopy;

	switch (pDev->iStageCERES) {
	case 1:
		//Today's Senescenced Tiller Numbers per square meter (Tillers/m2)
		DTN=(pCan->fTillerNumOld-pCan->fTillerNum)
		    *pMa->pSowInfo->fPlantDens;

		//Total senesecenced tiller number till today (Tillers)
		if (DTN>(double)0.0)
			pCan->fSenesTillerNumSq=pCan->fSenesTillerNumSq+DTN;

		break;

	case 2:

		if ((pCan->fTillerNumOld-pCan->fTillerNum)>(double)0.0)
			DTN=(pCan->fTillerNumOld-pCan->fTillerNum)
			    *pMa->pSowInfo->fPlantDens;
		else
			DTN=(double)0.0;

		pCan->fSenesTillerNumSq=pCan->fSenesTillerNumSq+DTN;

		break;

	}

	return RET_SUCCESS;
}

int ceres_PlantLeafNumber_MZ(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PPLANT pPl = xpn->pPl;

	double      fPhyllFac;

	PDEVELOP    pDev    =pPl->pDevelop;
	PCANOPY     pCan    =pPl->pCanopy;

	if ((pDev->iStageCERES<=3)&&(pCan->fExpandLeafNum<=pPl->pModelParam->fMaxLeafNum)) {
		if(pDev->fCumPhyll<(double)5.0)
			fPhyllFac = (double)0.66+(double)0.068*pDev->fCumPhyll;
		else
			fPhyllFac = (double)1.0;

		pCan->fLeafAppearR=pDev->fDTT/((double)38.9*fPhyllFac);

	}
	//else
	//      pCan->fLeafAppearR=(double)0.0;

	pDev->fCumPhyll += pCan->fLeafAppearR;
	pCan->fExpandLeafNum = pCan->fLeafTipNum = pDev->fCumPhyll;//+(double)1.0;

	return RET_SUCCESS;
}

int ceres_LeafSenescence_MZ(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;

	
	double fSenesLAOld;
	double fLeafSenesWaterFact,fLeafSenesLightFact,fLeafSenesTempFact,fTempAve;

	PDEVELOP                pDev    =pPl->pDevelop;
	PCANOPY                 pCan    =pPl->pCanopy;
	PPLTWATER           pPltW   =pPl->pPltWater;

	fSenesLAOld = pCan->fPlantSenesLA;

	if(pDev->iDayAftEmerg == 1)
		self->fNormLeafSenes = (double)0.0;
	//====================================================================================
	//      Normal Leaf Senescence
	//====================================================================================
	switch (pDev->iStageCERES) {
	case 1:
	case 2:
		self->fNormLeafSenes = pDev->fSumDTT*pCan->fPlantLA/(double)10000.0;
		break;

	case 3:
		self->fNormLeafSenes += pCan->fPlantLA/(double)1000.0;
		break;

	case 4:
		//nach CERES-Maize pp.81:
		self->fNormLeafSenes = pCan->fPlantLA*(double)(0.05 + 0.05*pDev->fSumDTT/170.0);
		//altes Expert-N:
		//fNormLeafSenes = pCan->fPlantLA*(double)(0.025*(1.0+4.0*pDev->fSumDTT/fP5));
		break;

	case 5:
		//nach CERES-Maize pp.82:
		self->fNormLeafSenes =pCan->fPlantLA*(double)(0.1+0.8*pow((pDev->fSumDTT+170.0)/self->fP5,3.0));
		//altes Expert-N:
		//fNormLeafSenes =pCan->fPlantLA*(double)(0.05+0.8*pow(pDev->fSumDTT/fP5,4.0));
		break;
	}



	//====================================================================================
	//      Leaf Senescence due to Drought Stress, Competition for Light and low temperature
	//====================================================================================

	//water:
	fLeafSenesWaterFact=(double)0.95+(double)0.05*pPltW->fStressFacPhoto;

	//Light:
	if(pCan->fLAI > (double)4.0)
		fLeafSenesLightFact = (double)1.0 - (double)0.008*(pCan->fLAI-(double)4.0);
	else
		fLeafSenesLightFact = (double)1.0;

	//Temperature:
	fLeafSenesTempFact = (double)1.0;
	fTempAve = (self->weather.fTempMin+self->weather.fTempMax)/(double)2.0;
	if(fTempAve < (double)6.0)
		fLeafSenesTempFact = (double)max(0.0,fTempAve/6.0);
	if(fTempAve <= (double)-5.0)
		fLeafSenesTempFact = (double)0.0;

	pCan->fLeafSenesR = pCan->fPlantGreenLA*(double)(1.0-min(fLeafSenesWaterFact,
	                    min(fLeafSenesLightFact,fLeafSenesTempFact)));

	pCan->fPlantSenesLA += pCan->fLeafSenesR;

	if(pCan->fPlantSenesLA < self->fNormLeafSenes)
		pCan->fPlantSenesLA = self->fNormLeafSenes;

	if(pCan->fPlantSenesLA >= pCan->fPlantLA)
		pCan->fPlantSenesLA = pCan->fPlantLA;

	pCan->fLALossR = pCan->fPlantSenesLA - fSenesLAOld;

	pCan->fPlantGreenLA = pCan->fPlantLA-pCan->fPlantSenesLA;
//              pPl->pBiomass->fLeafWeight -= pCan->fLALossR*pPl->pGenotype->fSpecLfWeight;

	return RET_SUCCESS;
}

int ceres_PlantLeafNumber_SF(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PDEVELOP    pDev = pPl->pDevelop;
	PCANOPY             pCan = pPl->pCanopy;
	double fMaxLeafNum, fXHY;

	double fTTE = (double)350;
	double fPhy1 = (double)39.0;
	double fPhy2 = (double)24.0;


	fMaxLeafNum = (double)self->iTLNO;

	//calculate total number of leaves appeared:
	if (pCan->fLeafTipNum<=(double)6)
		self->fPHINT = fPhy1;
	else
		self->fPHINT = fPhy2;

	pCan->fLeafAppearR=pDev->fDTT/self->fPHINT;
	pCan->fLeafTipNum += pCan->fLeafAppearR;

	if(pCan->fLeafTipNum>=fMaxLeafNum)
		pCan->fLeafTipNum = fMaxLeafNum;


	//calculate number of expanded leaves:
	if(pCan->fExpandLeafNum<=(double)6.0)
		fXHY = fPhy1;
	else
		fXHY = fPhy2;

	pCan->fLeafAppearR=pDev->fDTT/fXHY;


	if(pCan->fExpandLeafNum<=fMaxLeafNum) {
		if((pDev->fCumDTT-self->fP9)<=(fTTE+fPhy1))
			pCan->fExpandLeafNum=(double)0.0;
		else
			pCan->fExpandLeafNum+=pCan->fLeafAppearR;
	} else
		pCan->fExpandLeafNum = fMaxLeafNum;


	return RET_SUCCESS;
}

int ceres_LeafSenescence_SF(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	int i;
	double wwww, fXXX, fYYY, fZZZ;
	double fDSenesLAN2, fDSenesLANW;
	double fNAvailableGreenLeaf, fLAEq;
	

	PCANOPY         pCan    = pPl->pCanopy;
	PPLTWATER       pPltW   = pPl->pPltWater;
	PBIOMASS        pBiom   = pPl->pBiomass;
	PPLTNITROGEN pPltN      = pPl->pPltNitrogen;


	if(pPl->pDevelop->iStageCERES ==3) {
		if(pCan->fLAI <= (double)1.2)
			self->iSenesTime = 0;

		// When LAI>1.2, commence counting time from now to the start
		// of leaf senescence driven by shade
		if((pCan->fLAI > (double)1.2)&&(self->bSenesCode==FALSE)) {
			self->iSenesTime++;
			self->fSGRO[self->iSenesTime-1] = pCan->fLAGrowR/pMa->pSowInfo->fPlantDens; //[cm^2/plant]
		}

		if(self->iSenesTime  == 13)
			self->bSenesCode = TRUE;

		//When iSenesTime exceeds 13 days then calculate the rate of shade
		//driven leaf senescence. fSGRO[1] is plant area growth rate 13 days ago.
		if(self->bSenesCode) {
			wwww = (double)(-.0182+.00004147*self->fSGRO[0]*pMa->pSowInfo->fPlantDens); //[m^2/m^2]

			for(i=0; i<11; i++)
				self->fSGRO[i] = self->fSGRO[i+1];

			self->fSGRO[11] = pCan->fLAGrowR/pMa->pSowInfo->fPlantDens; //[cm^2/plant];

			pCan->fLeafSenesR = wwww * (double)1e4; //[m^2/ha]
		} else
			pCan->fLeafSenesR = (double)0.0;


		//After the last 5 leaves have begun to expand and if water
		//stress is affecting expansion, then leaf senescence caused
		//by water stress is calculated as 3& per day of green plant
		//leaf area.
		if(((int)pCan->fExpandLeafNum>self->iTLNO-5)&&(pPltW->fStressFacLeaf<(double)0.8))
			pCan->fLeafSenesR = (double)max(pCan->fLeafSenesR,0.03*pCan->fPlantGreenLA);

		pCan->fPlantSenesLA = pCan->fPlantSenesLA + pCan->fLeafSenesR;

		//cumulative senesced leaf area
		self->fCumSenesLA += pCan->fLeafSenesR;

		//////////////////////////////////////////////////////////////////////
		//Adjust nitrogen contents and weights of green and dead leaves         //
		//                                                                                                                                      //
		//specific leaf area [m^2/kg]
		self->fSpecLeafArea = pCan->fPlantGreenLA/pBiom->fGreenLeafWeight;
		//calculate nitrogen concentration of green leaves
		self->fNGreenLeafActConc = self->fNGreenLeafCont/pBiom->fGreenLeafWeight;
		//calculate residual nitrogen concentration
		self->fYRAT = (double)((0.009 - 0.0875*self->fNGreenLeafActConc)/0.9125);

		//calculate the amount of nitrogen retranslocated from
		//senesced leaves to other organs [kg(N)/ha]
		pPltN->fLeafNtransRate = pCan->fLeafSenesR/self->fSpecLeafArea *
		                         (self->fNGreenLeafActConc - self->fYRAT);

		//adjust the weight of senesced leaves
		pBiom->fSenesLeafWeight += pCan->fLeafSenesR/self->fSpecLeafArea -
		                           pPltN->fLeafNtransRate * (double)6.25;

		//calculate residual nitrogen in senesced leaves
		self->fNSenesLeafCont += pCan->fLeafSenesR/self->fSpecLeafArea*self->fYRAT;

		//adjust the weight of green leaves
		pBiom->fGreenLeafWeight += pBiom->fLeafGrowR
		                           + pPltN->fLeafNtransRate * (double)6.25
		                           - pCan->fLeafSenesR/self->fSpecLeafArea;

		//Calculate the amount of nitrogen in green leaves
		self->fNGreenLeafCont -= pCan->fLeafSenesR/self->fSpecLeafArea*self->fYRAT;

		//calculate maximum permissible amount of nitrogen in green leaves
		fXXX = pBiom->fGreenLeafWeight * pPltN->fLeafOptConc;

		//if the calculated amount of N in green leaves is greater than the
		//maximum (optimum), then allocate the excess N to the stem and adjust
		//organ weights
		if(self->fNGreenLeafCont > fXXX) {
			fYYY = self->fNGreenLeafCont - fXXX;
			self->fNGreenLeafCont = fXXX;

			pBiom->fGreenLeafWeight -= fYYY*(double)6.25;
			pBiom->fLeafWeight      -= fYYY*(double)6.25;
			pBiom->fStemWeight              += fYYY*(double)6.25;

			pPltN->fStemCont               += fYYY;
			pPltN->fLeafCont               -= fYYY;
		}
	}

	if((pPl->pDevelop->iStageCERES == 4)||(pPl->pDevelop->iStageCERES == 5)) {
		//calculate senescence due to N demand/////////////////////////////////////////

		if(pCan->fPlantGreenLA > (double)0.1) {
			//calculate nitrogen concentration of green leaves
			self->fNGreenLeafActConc = self->fNGreenLeafCont/pBiom->fGreenLeafWeight;
			//calculate residual nitrogen concentration
			self->fYRAT = (double)((0.009 - 0.0875*self->fNGreenLeafActConc)/0.9125);
			//calculate amount of nitrogen available in green leaves
			fNAvailableGreenLeaf = self->fNGreenLeafCont-self->fYRAT*pBiom->fGreenLeafWeight;
			//calculate leaf area equivalent of the amount of N in green leaves [m^2/ha]
			fLAEq = self->fSLOPEPE * fNAvailableGreenLeaf;
			//calculated senesced leaf area [m^2/ha]
			fZZZ = self->fAPLA - fLAEq;

			//calculate increment in senesced leaf area attributable to N retranslocation
			if(fZZZ > self->fSenesLAN2)
				fDSenesLAN2 = fZZZ - self->fSenesLAN2;
			else
				fDSenesLAN2 = (double)0.0;

			//calculate increment in senesced leaf area attributable to
			//water stress
			if(pPltW->fStressFacLeaf < (double)0.8)
				fDSenesLANW = (double)0.06*pCan->fPlantGreenLA;
			else
				fDSenesLANW = (double)0.0;

			if(fDSenesLANW > fDSenesLAN2)
				fDSenesLAN2 = fDSenesLANW;

			//actualize senesced plant leaf area
			self->fSenesLAN2 += fDSenesLAN2;
			//specific leaf area [m^2/kg]
			self->fSpecLeafArea = pCan->fPlantGreenLA/pBiom->fGreenLeafWeight;

			self->fCumSenesLA += self->fCumSenesLA + self->fSenesLAN2;

			//reduce leaf weight according to the amount of N exported
			pBiom->fLeafWeight -= fDSenesLAN2/self->fSpecLeafArea *
			                      (self->fNGreenLeafActConc - self->fYRAT)*(double)6.25;
			pBiom->fStemWeight += fDSenesLAN2/self->fSpecLeafArea *
			                      (self->fNGreenLeafActConc - self->fYRAT)*(double)6.25;

			pBiom->fGreenLeafWeight -= fDSenesLAN2/self->fSpecLeafArea;
			pBiom->fSenesLeafWeight += fDSenesLAN2/self->fSpecLeafArea *
			                           (double)((1.0-6.25*(self->fNGreenLeafActConc - self->fYRAT)));

			pCan->fPlantGreenLA -= fDSenesLAN2;
			pCan->fPlantSenesLA += fDSenesLAN2;

		}
	}


	return RET_SUCCESS;
}
