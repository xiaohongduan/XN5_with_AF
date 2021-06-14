//      spass_growth.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_growth.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int spass_biomass_growth(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

/*	PCHEMISTRY		pCh = xpn->pCh; 
	PCLIMATE		pCl = xpn->pCl; 
	PHEAT			pHe = xpn->pHe ; 
	PLOCATION		pLo = xpn->pLo; 
	PMANAGEMENT		pMa = xpn->pMa; */
	PPLANT			pPl = xpn->pPl; 
/*	PSPROFILE		pSo = xpn->pSo; 
	PTIME			pTi = xpn->pTi; 
	PWATER			pWa = xpn->pWa;*/

	//PBIOMASS 	pBiom =pPl->pBiomass;

	//C_DEBUG(self->fAvailableAssim);
	//C_DEBUG(self->temp_par)
	//C_DEBUG(self->temp_pard)
    
	if((pPl->pDevelop->iDayAftEmerg >0)&&(PLANT_IS_GROWING))
	{
	spass_crop_maintenance(self);

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		return RET_SUCCESS;

	spass_AssimilatePartition(self);
	spass_OrganBiomassGrowth(self);
	}

    return RET_SUCCESS;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	AssimilatePartition
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int spass_AssimilatePartition(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);

	PPLANT			pPl = xpn->pPl; 

	//static double xtuber;
	double 		x,fTopFraction; 
	PRESPONSE 	pResp1,pResp2,pResp3;
	PBIOMASS 	pBm =pPl->pBiomass;

	x=pPl->pDevelop->fStageSUCROS;

	//SG/03/11/99, Kartoffel:
	//Da der Zeitpunkt der Knollenbildung unabhängig von den restlichen
	//phänologischen Entwicklungsstadien berechnet wird, erfolgt die
	//Assimilatverteilung zunächst nach XCERES und nach dem Beginn des
	//Knollenwachstums nach SUCROS.
	if(strcmp(pPl->pGenotype->acCropCode,"PT")==0)
	{
			if(pPl->pDevelop->fStageXCERES<=(double)1.0)
			{
				x=pPl->pDevelop->fStageXCERES/(double)2.0;
				self->spass_AssimilatePartition_xtuber = pPl->pDevelop->fStageSUCROS;
			}
			else if(pPl->pDevelop->fStageSUCROS<=(double)1.0)
			
				x = (double)(0.5+(self->spass_AssimilatePartition_xtuber-0.5)*(pPl->pDevelop->fStageSUCROS-self->spass_AssimilatePartition_xtuber)/(1.0-self->spass_AssimilatePartition_xtuber))
						*pPl->pDevelop->fStageSUCROS/self->spass_AssimilatePartition_xtuber;
			else
				x = pPl->pDevelop->fStageSUCROS;
	}

/*	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
			x=pPl->pDevelop->fStageXCERES;
*/


	//Get the pointer of the partitioning data 
	pResp1	= pPl->pGenotype->PartRoot;
	pResp2	= pPl->pGenotype->PartLeaf;
	pResp3	= pPl->pGenotype->PartStem;

	if ((pResp1[0].fInput==-99.9)||(pResp2[0].fInput==-99.9)||(pResp3[0].fInput==-99.9))
		{
		spass_DefaultAssimilatePartitioning(self);
		}
		
	else
		{
		//Root and Top Fraction
		pBm->fPartFracRoot = spass_AFGENERATOR(x, pPl->pGenotype->PartRoot)/pPl->pPltWater->fStressFacPartition;
		fTopFraction  	   = (double)1.0 - pBm->fPartFracRoot;         
	
		//Leaf, stem, grain fraction
		pBm->fPartFracLeaf = fTopFraction*spass_AFGENERATOR(x, pPl->pGenotype->PartLeaf);
	    pBm->fPartFracStem = fTopFraction*spass_AFGENERATOR(x, pPl->pGenotype->PartStem);
	
	    pBm->fPartFracGrain= fTopFraction-pBm->fPartFracLeaf-pBm->fPartFracStem;
		}
		
	pBm->fPartFracFruit	= pBm->fPartFracGrain;

	return RET_SUCCESS;
	}

int spass_DefaultAssimilatePartitioning(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);

	PPLANT			pPl = xpn->pPl; 

	double 		x,vFractionRoot,vFractionLeaf,vFractionStem,vFractionStorage,vFractionSupport;
	double		fTopFraction,fPartFracSupp; 
	PBIOMASS 	pBm =pPl->pBiomass;
	
	x=(double)pPl->pDevelop->fStageSUCROS;
	//printf("StageSucros %f \n", pPl->pDevelop->fStageSUCROS);
	
	
	if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)	//Source: SPASS (Wang,1997, p109)
		{
		vFractionRoot =0.3022*pow(x,4)-1.2418*pow(x,3)+1.8016*x*x-1.3653*x+0.4999;
		vFractionRoot =(x<=1.0) ? (vFractionRoot) : (0.0);
		
		vFractionLeaf =-5.382*pow(x,4)+12.189*pow(x,3)-8.0485*x*x+0.2266*x+1.0058;
		vFractionStem =5.382*pow(x,4)-12.189*pow(x,3)+8.0485*x*x-0.2266*x-0.0058;
		//FH 09/2012 Test
		//vFractionLeaf =-(-5.382*pow(x,4)+12.189*pow(x,3)-8.0485*x*x+0.2266*x+1.0058);
		//vFractionStem =-(5.382*pow(x,4)-12.189*pow(x,3)+8.0485*x*x-0.2266*x-0.0058);	
		
		vFractionStem =(x<1.0) ? (min(1.0,vFractionStem)):((1.25-x)/(1.25-1.0));

//		vFractionSupport =0.0;

		vFractionSupport =(x<=1.1) ? (max(0.0,1.0-vFractionLeaf-vFractionStem)):(0.0);
		}

	if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)	//Source: Penning de Vries et al., 1989, p89
		{
		/*
		stimmt so nicht!!
		vFractionRoot =2E-6*pow(x,4)-2E-4*pow(x,3)+5E-3*x*x-0.0778*x+0.5729;
		vFractionLeaf =-5.382*pow(x,4)+12.189*pow(x,3)-8.0485*x*x+0.2266*x+1.0058;
		vFractionStem =5.382*pow(x,4)-12.189*pow(x,3)+8.0485*x*x-0.2266*x-0.0058;
		*/
		
		//ebenfalls nach Penning de Vries et al., 1989, p89
		vFractionRoot =4.851063149*pow(x,4) -8.443020487*pow(x,3) +3.49673349*x*x -0.554776147*x +0.65;
		vFractionRoot =(x<=1.0) ? (vFractionRoot) : (0.0);

		vFractionLeaf = 4.368259694*pow(x,4) -7.999268252*pow(x,3) +3.786273302*x*x -0.975264604*x +0.82;
		vFractionLeaf =(x<=1.0) ? (vFractionLeaf) : (0.0);

		vFractionStem = -4.366483841*pow(x,4) +7.996527499*pow(x,3) -3.785398215*x*x +0.975354092*x +0.18;

		vFractionStem =(x<1.0) ? (min(1.0,vFractionStem)):((1.25-x)/(1.25-1.0));
//		vFractionStem =(x<0.95) ? (min(1.0,vFractionStem)):((1.21-x)/(1.21-0.95));

		vFractionSupport =(x<=1.1) ? (max(0.0,1.0-vFractionLeaf-vFractionStem)):(0.0);
		}
			
	if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)	//Source: Spitters et al., 1986, p179
		{
		//x=(double)pPl->pDevelop->fStageWang;                                        
		//x=(double)pPl->pDevelop->fStageXCERES;                                        
			
		vFractionRoot 	 = 1.0 - spass_LIMIT(0.5, 1.0, 0.5+0.5*(x-1.0));
		vFractionLeaf 	 = spass_LIMIT(0.0, 0.6, 0.6-(x-1.0));
		vFractionStorage = spass_LIMIT(0.0, 1.00, x-1.0);
		
		//PLAGEN:
		/*vFractionRoot 	 = 1.0 - LIMIT(0.5, 1.0, 0.5+0.5*(x-1.0));
		vFractionLeaf 	 = LIMIT(0.0, 0.6, 0.6-(x-1.0));
		vFractionStorage = LIMIT(0.0, 1.00, x-1.0);*/

		//Spitters et al.:
		/*vFractionRoot 	 = 1.0 - LIMIT(0.8, 1.0, 0.8+0.2*(x-1.0));
		vFractionLeaf 	 = LIMIT(0.0, 0.75, 0.75-(x-1.0));
		vFractionStorage = LIMIT(0.0, 1.00, x-1.0);*/


		vFractionStem 	 = 1.0-vFractionLeaf-vFractionStorage;

		vFractionSupport =0.0;
		}
	
	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0)	//Grain Maize - Source: Penning de Vries et al., 1989, p89-90
		{
		vFractionRoot =0.5*(1.37-x)/(1.37-0.0);
		vFractionLeaf =3.4937*pow(x,5)-15.738*pow(x,4)+25.665*pow(x,3)-17.777*x*x+3.986*x+0.49;
		vFractionLeaf =spass_LIMIT(0.0, 0.7, vFractionLeaf);
		vFractionStem =5.8302*pow(x,5)-18.436*pow(x,4)+17.878*pow(x,3)-5.1831*x*x+0.0413*x+0.51;
		vFractionStem =spass_LIMIT(0.0, 0.7,vFractionStem);

		vFractionSupport =(x<=1.1) ? (max(0.0,1.0-vFractionLeaf-vFractionStem)):(0.0);
		}



	//Root fraction
	pBm->fPartFracRoot = (double)spass_LIMIT(0.0, 1.0, vFractionRoot/pPl->pPltWater->fStressFacPartition);
	//printf("%f %f %f \n", x, vFractionRoot, pPl->pPltWater->fStressFacPartition);
	fTopFraction = (double)1.0-pBm->fPartFracRoot;         

	//Leaf, stem, grain fraction
	fPartFracSupp = fTopFraction*(double)spass_LIMIT(0.0, 1.0, vFractionSupport);

	pBm->fPartFracLeaf = fTopFraction*(double)spass_LIMIT(0.0, 1.0, vFractionLeaf);
	pBm->fPartFracStem = fTopFraction*(double)spass_LIMIT(0.0, 1.0, vFractionStem+fPartFracSupp);
	
    pBm->fPartFracGrain= fTopFraction-pBm->fPartFracLeaf-pBm->fPartFracStem;
	

	return RET_SUCCESS;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	OrganBiomassGrowth
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int spass_OrganBiomassGrowth(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);

	PCLIMATE		pCl = xpn->pCl; 
	PPLANT			pPl = xpn->pPl;
	PMANAGEMENT		pMa = xpn->pMa;


	double PrtcLv,PrtcSt,PrtcRt,PrtcSo,EffPrtn,EffCH2O,EffGrn,ASRQLV,ASRQST,ASRQRT,ASRQSO,fAvailableAssim;
	double fGrwRespLv,fGrwRespSt,fGrwRespRt,fGrwRespSo,fGrwRespRes,CO2Lv,CO2St,CO2Rt,CO2So;
	double fTempMin,fTempMax,fTempOpt,fTempFunc,fGrainCarboDemand,fCarboSupply,fTemp;

	double fSeedReserv;// Wasserfaktor;

	PWEATHER		pWth =pCl->pWeather;
	PBIOMASS		pBiom=pPl->pBiomass; 
	PCANOPY			pCan =pPl->pCanopy; 
	PDEVELOP		pDev =pPl->pDevelop;
	PPLTCARBON 		pPltC=pPl->pPltCarbon;
	PPLTWATER 		pPltW=pPl->pPltWater;
	PPLTNITROGEN 	pPltN=pPl->pPltNitrogen;
	PTIMESTEP		pTi = xpn->pTi->pTimeStep;
    
	//---------------------------------------------------------------------------------
	//Effect of Nitrogen Content
	//Protein content of different organs
	EffPrtn =(double)0.440; 	//Efficiency coefficient for conversion of photosythate to protein
	EffCH2O =(double)0.825; 	//Efficiency coefficient for conversion of photosythate to structural carbohydrate
	EffGrn  =(double)0.690; 	//Efficiency coefficient for conversion into grain biomass

	PrtcLv = (double)6.25*pPl->pPltNitrogen->fLeafActConc;
	PrtcSt = (double)6.25*pPl->pPltNitrogen->fStemActConc;
	PrtcRt = (double)6.25*pPltN->fRootActConc;
	PrtcSo = (double)6.25*pPltN->fGrainConc;

	//Assimilate requirement for conversion of carbohydrates to dry matter	
	ASRQLV = PrtcLv*EffPrtn +((double)1.0-PrtcLv)*EffCH2O;
	ASRQST = PrtcSt*EffPrtn +((double)1.0-PrtcSt)*EffCH2O;
	ASRQRT = PrtcRt*EffPrtn +((double)1.0-PrtcRt)*EffCH2O;
	ASRQSO = PrtcSo*EffGrn  +((double)1.0-PrtcSo)*EffCH2O;

	spass_ReserveTranslocation(self);


	//SG/07/04/99/////////////////////////////////////////////////////////////////////////
	//
	// Wasserfaktor für ersten Tag mit fStageSUCROS>0.
	// (Aktuelle Transpiration wird vor fStageSUCROS berechnet und
	//  ist deshalb am ersten Tag == 0).
	//////////////////////////////////////////////////////////////////////////////////////

	
	/* if (fPotTraDay == (double)0.0)
				Wasserfaktor = (double)1.0;
			else
				Wasserfaktor = fActTraDay/fPotTraDay;
	*/

	//SG/20/05/99: Sonderfall Kartoffel

	if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
		{
			fSeedReserv = (double)1.5*(double)10.0*pMa->pSowInfo->fPlantDens;
			
			if (pBiom->fSeedReserv>fSeedReserv)
				pBiom->fSeedReserv -= fSeedReserv*pTi->fAct;
			else
				fSeedReserv = (double)0.0;
		}
	else
			fSeedReserv = (double)0.0;
	
	//printf("%f %f \n", pPltC->fGrossPhotosynR, pPltW->fStressFacPhoto);

	pPltC->fGrossPhotosynR *= pPltW->fStressFacPhoto;//SG/26/05/99: pPltW->fStressFacPhoto statt Wasserfaktor
	
	//printf("%f %f %f %f \n", pPltC->fGrossPhotosynR, fSeedReserv, pPltC->fMaintRespR, pPl->pBiomass->fReserveTransRate);

	//SG/20/05/99: fSeedReserv wg. Potato
	fAvailableAssim=(double)(30.0/44.0)*max((double)0.0, pPltC->fGrossPhotosynR + fSeedReserv-pPltC->fMaintRespR)
									+pPl->pBiomass->fReserveTransRate*(double)(1.1111*0.947); 

	//fTemp = 0.0;
	////Calculate daily mean-temperature
	//if(self->fTemp_zwischen[self->iMeanTemp]!=pWth->fTempAir)
	//{
	//	self->fTemp_zwischen[self->iMeanTemp] = pWth->fTempAir;
	//	self->iMeanTemp++;
	//	if (self->iMeanTemp == (int)(1.0/pTi->fAct)) self->iMeanTemp = 0;
	//}
	//for (i = 0; i<(int)(1.0/pTi->fAct);i++)
	//	fTemp += self->fTemp_zwischen[i]*pTi->fAct;
	fTemp = pWth->fTempAir;

	
if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||
	(strcmp(pPl->pGenotype->acCropCode,"BA")==0)||
	(strcmp(pPl->pGenotype->acCropCode,"MZ")==0))
	{
	//=========================================================================================
	//Grain demand
	//=========================================================================================
	if (pDev->fStageSUCROS>=(double)1.0)
	{
	fTempMin = pPl->pGenotype->fTempMinSoFill;
	fTempOpt = pPl->pGenotype->fTempOptSoFill;
	fTempMax = pPl->pGenotype->fTempMaxSoFill;  
	
	//fTemp = (double)0.5*(pWth->fTempMin+pWth->fTempMax);

	if (pPl->pGenotype->SoFillRateTemp[0].fInput==(double)-99.9)
		fTempFunc = spass_RelativeTemperatureResponse(fTemp,fTempMin,fTempOpt,fTempMax);
	else
		fTempFunc = spass_AFGENERATOR(fTemp, pPl->pGenotype->SoFillRateTemp);

	fGrainCarboDemand = pPl->pGenotype->fRelStorageFillRate*fTempFunc*pCan->fGrainNum
						*(double)1E+4*pMa->pSowInfo->fPlantDens*(double)1E-6/ASRQSO;

	fCarboSupply = min(fAvailableAssim,fGrainCarboDemand);
	
	if (fCarboSupply<=fAvailableAssim) //
		{ 
		pPl->pBiomass->fReserveTransRate  -= (fAvailableAssim-fCarboSupply)/((double)(1.1111*0.947));
		pPl->pBiomass->fStemReserveWeight += (fAvailableAssim-fCarboSupply)*pTi->fAct/((double)(1.1111*0.947));

		if (pPl->pBiomass->fStemReserveWeight/pBiom->fStemWeight>=pPl->pGenotype->fReserveRatio)
			{
			pPl->pBiomass->fLeafReserveWeight=pPl->pBiomass->fStemReserveWeight-pBiom->fStemWeight*pPl->pGenotype->fReserveRatio;
			}
		}
		 	
	fAvailableAssim=fCarboSupply;
	}               
	}

	//printf("%d %f \n", pPl->pDevelop->iDayAftSow, fAvailableAssim);

	//==========================================================================================
	//Biomass Growth Rate of Different Organs	
	if (fAvailableAssim==(double)0.0)
		{
		pBiom->fLeafGrowR 		= (double)0.0;
		pBiom->fRootGrowR 		= (double)0.0; 
		pBiom->fStemGrowR 		= (double)0.0; 
		pBiom->fGrainGrowR		= (double)0.0;
		}
	else
		{			
/*
		ASRQLV = (double)1.0/lpGen->fGrwEffLeaf;
		ASRQST = (double)1.0/lpGen->fGrwEffStem;
		ASRQRT = (double)1.0/lpGen->fGrwEffRoot;
		ASRQSO = (double)1.0/lpGen->fGrwEffStorage;
*/
		//---------------------------------------------------------------------------------
		//Growth rate, leaves, stems, roots and stem reserves
		pBiom->fLeafGrowR = fAvailableAssim*pBiom->fPartFracLeaf*ASRQLV;
		pBiom->fRootGrowR = fAvailableAssim*pBiom->fPartFracRoot*ASRQRT;
		//printf("%f %f %f \n", fAvailableAssim, pBiom->fPartFracLeaf, ASRQRT);
		pBiom->fStemGrowR = fAvailableAssim*pBiom->fPartFracStem*ASRQST;								
		pBiom->fGrainGrowR= fAvailableAssim*pBiom->fPartFracGrain*ASRQSO;
		}

	self->fAvailableAssim = fAvailableAssim;

	//==========================================================================================
	//Growth Respiration  and total respiration
	//CO2 producing factor
	CO2Lv 	=pPl->pGenotype->fCO2EffLeaf;		//(double)0.461;
	CO2St 	=pPl->pGenotype->fCO2EffStem;		//(double)0.408;
	CO2Rt 	=pPl->pGenotype->fCO2EffRoot;		//(double)0.406;
	CO2So 	=pPl->pGenotype->fCO2EffStorage;		//(double)0.347;
                                                          
	//Growth respiration
	fGrwRespLv = max((double)0.0,pBiom->fLeafGrowR *CO2Lv);
	fGrwRespSt = max((double)0.0,pBiom->fStemGrowR *CO2St);
	fGrwRespRt = max((double)0.0,pBiom->fRootGrowR *CO2Rt);
	fGrwRespSo = max((double)0.0,pBiom->fGrainGrowR*CO2So);

	fGrwRespRes= (double)max(0.0,(double)pPl->pBiomass->fReserveTransRate*1.111*0.053*44.0/30.0);
	
	//Total respiration and net carbon gain
	pPltC->fGrowthRespR = fGrwRespLv+fGrwRespSt+fGrwRespRt+fGrwRespSo+fGrwRespRes;
	pPltC->fTotRespR    = pPltC->fGrowthRespR + pPltC->fMaintRespR;
	pPltC->fNetPhotosynR= max((double)0.0, pPltC->fGrossPhotosynR-pPltC->fTotRespR);

	//Dry weight of different organs	                          
	pBiom->fLeafWeight 		+= pBiom->fLeafGrowR*pTi->fAct; 
	pBiom->fStemWeight  	+= pBiom->fStemGrowR*pTi->fAct;
	
	pBiom->fRootWeight  	+= pBiom->fRootGrowR*pTi->fAct;
	pBiom->fGrainWeight 	+= pBiom->fGrainGrowR*pTi->fAct;

	pPl->pBiomass->fStemReserveWeight =  max((double)0.0,pPl->pBiomass->fStemReserveWeight);

	pPl->pBiomass->fTotLeafWeight 	= pBiom->fLeafWeight; 
	pPl->pBiomass->fTotRootWeight  	= pBiom->fRootWeight;
	pPl->pBiomass->fTotStemWeight  	= pBiom->fStemWeight;

	pBiom->fBiomassAbvGround = pPl->pBiomass->fTotLeafWeight+pPl->pBiomass->fTotStemWeight+pBiom->fGrainWeight;

	pBiom->fBiomGrowR=pBiom->fLeafGrowR+pBiom->fRootGrowR+pBiom->fStemGrowR+pBiom->fGrainGrowR;

	//SG/16/04/99////////////////////////////////////////////////////////////
	//
	//	Biomass "above ground" wird in der Grafik nicht ausgegeben!
	//	Stattdessen Wert für "StovWeight", der aber nie aktualisiert wird.
	//	Deshalb nun die folgende Zeile:
	/////////////////////////////////////////////////////////////////////////

	pBiom->fStovWeight = pPl->pBiomass->fTotLeafWeight+pPl->pBiomass->fTotStemWeight;

	pBiom->fFruitWeight = pBiom->fGrainWeight;

	pBiom->fTotalBiomass		= pBiom->fBiomassAbvGround
 						  		 +pBiom->fRootWeight;
	return RET_SUCCESS;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	ReserveTranslocation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int spass_ReserveTranslocation(spass *self)
	{                 

	expertn_modul_base *xpn = &(self->parent);

	PPLANT			pPl = xpn->pPl; 

	//static double fTotalReservWeihgt=(double)0.0;
	double 		CFST;//CFLV,CFRT,CFSO;
	PBIOMASS	pBiom=pPl->pBiomass;
	PDEVELOP	pDev =pPl->pDevelop;
	PTIMESTEP	pTi = xpn->pTi->pTimeStep;
	
	//CFLV	=(double)0.459;	//C-content of leaves
	//CFRT	=(double)0.467;	//C-content of root
	CFST	=(double)0.494;	//C-content of stem
	//CFSO	=(double)0.471;	//C-content of Storage organs
	
	pPl->pBiomass->fStemReserveGrowRate=pBiom->fStemGrowR*pPl->pGenotype->fReserveRatio*CFST/(double)0.444;
	pPl->pBiomass->fStemReserveWeight +=pPl->pBiomass->fStemReserveGrowRate*pTi->fAct;

	//SG/06/05/99:
	//
	//Bedingung bei der if-Abfrage erweitert, damit fTotalReservWeihgt bei jeder
	//neuen Pflanze wieder auf 0 gesetzt wird!
	if (pBiom->fPartFracStem>(double)0.0||(pBiom->fPartFracStem==(double)0.0
									&&pDev->fStageSUCROS==(double)0.0))
		{
		pPl->pBiomass->fReserveTransRate = (double)0.0;
		self->spass_ReserveTranslocationfTotalReservWeihgt		= pPl->pBiomass->fStemReserveWeight;
		}
	else     
		{
		pPl->pBiomass->fReserveTransRate   = self->spass_ReserveTranslocationfTotalReservWeihgt*pDev->fDevR;
		pPl->pBiomass->fStemReserveWeight -= pPl->pBiomass->fReserveTransRate*pTi->fAct;
		}
		 
	return RET_SUCCESS;
	}

int spass_crop_maintenance(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);

	PCLIMATE		pCl = xpn->pCl;
	PPLANT			pPl = xpn->pPl;

	PWEATHER		pWth =pCl->pWeather;
	PBIOMASS		pBiom=pPl->pBiomass;   
	PPLTCARBON 		pPltC=pPl->pPltCarbon;
	

	double MainLv,MainSt,MainRt,MainSo,Maints,MaintMetab,TempRef,TempEf,MnDvs,fTotLvWeight;//,fTemp
	double fActLv,fActSt,fActRt;//,fActSo;
	double 	Q10;
	
	MainLv	=pPl->pGenotype->fMaintLeaf;		//(double)0.03;		Werte stehen in PLANT.gtp!
	MainSt	=pPl->pGenotype->fMaintStem;		//(double)0.015;		Im Moment stehen dort die
	MainRt	=pPl->pGenotype->fMaintRoot;		//(double)0.015;		Weizen-Werte aus PLAGEN.
	MainSo	=pPl->pGenotype->fMaintStorage;		//(double)0.01;			(0.16, 0.1, 0.15, 0.1) 
	
	//===============================================================================================
	//Temperature effect
	Q10   	= 2.0;
	TempRef	=(double)25.0;
	//fTemp =(double)0.5*(pWth->fTempMax+pWth->fTempMin);
	TempEf=(double)pow(Q10,(double)(pWth->fTempAir-TempRef)/10.0);
	//===============================================================================================
	
	//Nitrogen influence:
	//Activity coefficient accounting for the influence of nitrogen content on maintenance resp 


	fActLv = pPl->pPltNitrogen->fLeafRelNc;
	fActSt = pPl->pPltNitrogen->fStemRelNc;
	fActRt = pPl->pPltNitrogen->fRootRelNc;
	//===============================================================================================
	//Maintenance change due to aging
	fTotLvWeight=pBiom->fLeafWeight+pBiom->fSenesLeafWeight;

	if (fTotLvWeight==(double)0.0)
		MnDvs=(double)0.0;
	else
		MnDvs = pBiom->fLeafWeight/fTotLvWeight;   //SUCROS2
	//===============================================================================================
	//Maintenance of different organs. It is assumed that daytime maintenance respiration of 
	//leaves is reduced by 50% due to excess energy (0.75)
	pPltC->fMainRespRLeaf = 	MainLv*pBiom->fLeafWeight *TempEf*fActLv*(double)0.75 ;
	pPltC->fMainRespRStem = 	MainSt*pBiom->fStemWeight *TempEf*fActSt*MnDvs;
	pPltC->fMainRespRRoot =    MainRt*pBiom->fRootWeight *TempEf*fActRt;
	pPltC->fMainRespRGrain= 	MainSo*pBiom->fGrainWeight*TempEf*MnDvs; 
	 
	Maints  = 	MainLv*pBiom->fLeafWeight *TempEf*fActLv*(double)0.75 
			+	MainSt*pBiom->fStemWeight *TempEf*fActSt*MnDvs
			+	MainRt*pBiom->fRootWeight *TempEf*fActRt
			+	MainSo*pBiom->fGrainWeight*TempEf*MnDvs;

	//The metabolic components is 20% of gross ps and reduced 50% during the day
	MaintMetab=	pPltC->fGrossPhotosynR*(double)(0.5*0.2);
    Maints	= Maints+MaintMetab;

	//Maintenance respiration 	
	pPltC->fMaintRespR = Maints;      

	return RET_SUCCESS;
	}


/*******************************************************************************
** EOF */