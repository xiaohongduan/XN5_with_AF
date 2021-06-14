//      spass_canopy.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_canopy.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int spass_canopy_formation(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PCLIMATE		pCl = xpn->pCl; 
	PMANAGEMENT		pMa = xpn->pMa; 
	PPLANT			pPl = xpn->pPl;
	PTIMESTEP		pTi = xpn->pTi->pTimeStep;
	PSIMTIME		pSimTime = xpn->pTi->pSimTime;

	OUTPUT 		Out;
	CARDTEMP	CardTmp;    
	TRANSP		Transp;
	ORGANNC		OrgNc;
	PRESPONSE 	pResp;
	double 	fMaxLfInitRate, fMaxLfEmergRate,fDaylenEff,fStageVR,fStemWeight, fTemp;
	PWEATHER	pWth=pCl->pWeather;
	PDEVELOP	pDev=pPl->pDevelop;
	PCANOPY		pCan=pPl->pCanopy;
	PBIOMASS	pBm =pPl->pBiomass;
	PMSOWINFO	pSI =pMa->pSowInfo;

	//For multiple crops
	if ((xpn_time_compare_date(pSimTime->iyear,pSimTime->mon,pSimTime->mday,pMa->pSowInfo->Year,pMa->pSowInfo->Month,pMa->pSowInfo->Day)==0)&&(NewDay(xpn->pTi)))
		self->__CAN_START = 0;

	if (pPl->pDevelop->iDayAftEmerg < 1)	
		{
		pCan->fLAI = 0.000001;
		return RET_SUCCESS;
		}

	//Leaf Number initialization
	if ((pPl->pDevelop->iDayAftEmerg==1)&&(self->__CAN_START == 0)&&(NewDay(xpn->pTi)))
    	{ 
		pPl->pCanopy->fLeafPrimodiaNum = (double)0.0;
		pCan->fExpandLeafNum  = (double)0.0;
		pCan->fLAI = pCan->fLAISpass;
		self->__CAN_START++;
        }

	if (PLANT_IS_GROWING)
	{
	
	fTemp = pWth->fTempAir;

	//----------------------------------------------------------------------------------------------------
	//Main stem leaf numbers: input variables
	fMaxLfInitRate 	= pPl->pGenotype->fMaxLfInitRate;
	fMaxLfEmergRate	= pPl->pGenotype->fMaxLfAppRate;
	fDaylenEff 		= pDev->fDaylengthEff;
	fStageVR		= pDev->fStageSUCROS;
    pResp   		= pPl->pGenotype->LfAppRateTemp;
    CardTmp.fMinTemp= pPl->pGenotype->fTempMinLfApp;
    CardTmp.fOptTemp= pPl->pGenotype->fTempOptLfApp;
    CardTmp.fMaxTemp= pPl->pGenotype->fTempMaxLfApp;
	
	//Leaf emergence rate
	spass_MainStemLeafing(fMaxLfInitRate,fMaxLfEmergRate,fTemp, fDaylenEff,fStageVR,pResp,&CardTmp,&Out);

	//Leaf numbers
	pPl->pCanopy->fLeafPrimodiaNum   += Out.fValue1*pTi->fAct;
	pCan->fLeafAppearR  = Out.fValue2;
	pCan->fLeafTipNum += pCan->fLeafAppearR*pTi->fAct;
	pCan->fExpandLeafNum= pCan->fLeafTipNum;	

	//----------------------------------------------------------------------------------------------------
	//Main stem Tiller numbers
	pPl->pCanopy->fMainStemTillers = pCan->fExpandLeafNum-(double)2.5; //SG/25/05/99: wird nie verwendet!
	
	//Tiller number per m2
	pCan->fTillerNumSq = spass_CerealTillering(pCan->fLeafAppearR, pCan->fExpandLeafNum, pCan->fTillerNum,
										   pPl->pGenotype->fTillerDemand, pBm->fStemWeight,pSI->fPlantDens,fStageVR, pTi->fAct);
	
	//Tiller number per plant
    pCan->fTillerNum   = pCan->fTillerNumSq/pSI->fPlantDens;
	
	//if (xpn_time_compare_date(pSimTime->iyear,pSimTime->mon,pSimTime->mday-1,1994,1,01)>0)
	//	printf("fStage %f TillerNum %f TillerNumSQ%f LeafAppR %f ExpandLeafNr %f StemWeight %f \n", fStageVR, pCan->fTillerNum,pCan->fTillerNumSq, pCan->fLeafAppearR, pCan->fExpandLeafNum,pBm->fStemWeight);
	
	//----------------------------------------------------------------------------------------------------
	//Growth of Leaf area index
	/*
    Transp.fPotTransp = pPl->pPltWater->fPotTranspDay;
    Transp.fActTransp = pPl->pPltWater->fActTranspDay;
    */
	Transp.fPotTransp = fPotTraR;
    Transp.fActTransp = fActTraR;

//	OrgNc.fOptNc = pPl->pPltNitrogen->fLeafOptConc;
	OrgNc.fOptNc = pPl->pPltNitrogen->fLeafMaxConc;
	OrgNc.fMinNc = pPl->pPltNitrogen->fLeafMinConc;
	OrgNc.fActNc = pPl->pPltNitrogen->fLeafActConc;
	    

	//SG/13/04/99/////////////////////////////////////////////////////////////////////
	//
	//	Im ursprünglichen SPASS werden keine Transpirations- und Stickstoffwerte
	//	übergeben!!
	//	Deshalb sind Wasser- und N-Stress in der Funktion "LeafAreaGrowthRate"
	//	immer = 0.
	//
	//	pCan->fLAI += LeafAreaGrowthRate_SPASS(pBm->fLeafGrowR, fStageVR,pPl->pGenotype->fSpecLfWeight,pPl->pGenotype->SpecLfWght,
	//									   NULL, NULL);
	//
	//	Anpassung an PLAGEN:
	/////////////////////////////////////////////////////////////////////////////////////

	pCan->fLAI += spass_LeafAreaGrowthRate(pBm->fLeafGrowR, fStageVR,pPl->pGenotype->fSpecLfWeight,pPl->pGenotype->SpecLfWght,
										   &Transp, &OrgNc)*pTi->fAct;
	
	//=========================================================================================
	//If flowering occurs, calculate the grain number, LAIanthesis and days after anthesis	
	//=========================================================================================
	if (NewDay(xpn->pTi))
	{
	if (pDev->fStageSUCROS<(double)1.0)
		pPl->pDevelop->iDayAftAnthesis=(double)0.0;
	else
		pPl->pDevelop->iDayAftAnthesis++;
		
	if (pPl->pDevelop->iDayAftAnthesis==1)
		{
//		lpCan->fGrainNum=(double)3500.0E4+(double)1.4E4
//						 *(lpBiom->fLeafWeight+lpBiom->fStemWeight+lpBiom->fStemReserveWeight);
		

  		fStemWeight =pBm->fStemWeight/((double)10.0*pMa->pSowInfo->fPlantDens);	//g per plant
  		
    	pCan->fGrainNum	 = fStemWeight*pPl->pGenotype->fStorageOrganNumCoeff;  			//Grains per plant
    	pCan->fGrainNumSq= pCan->fGrainNum*pMa->pSowInfo->fPlantDens; //Grains per sq meter 	


//		fLAIanthesis =lpCan->fTotalLAI;
		} 
	}

	} //end PLANT_IS_GROWING
    
    if (self->set_LAI_to_0_after_maturity==1)
        {
            if (pPl->pDevelop->bMaturity==TRUE)
                {
                    pCan->fLAI = 0.0001;
                }

        }
	
	pPl->pCanopy->fPlantHeight = (double)pPl->pGenotype->fPlantHeight*
                     min(1.0, (double)pDev->fStageSUCROS)/100.0;
	
    return RET_SUCCESS;
  }
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: int WINAPI _loadds MainStemLeafing_SPASS(double fMaxLfInitRate,double fMaxLfEmergRate,double fTemp, double fDaylenEff, 
//		  						          double fStageVR, PRESPONSE pResp,PCARDTEMP pCardTemp, POUTPUT pOut)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the leaf primodia initiation rate and leaf emergence rate on 
//			the main stem of cereal crops
//Inputs:	1. fMaxLfInitRate	- Maximum leaf initiation rate at optimal temperature (lvs/d)
//			2. fMaxLfEmergRate	- Maximum leaf emergence rate at optimal temperature (Lvs/d) 	
//			3. fTemp			- Current daily average temperature (C) 	
//			4. fDaylenEff		- Daylength effect on development rate (-)
//			5. fStageVR			- Current VR development stage value (-) 	
///			6. pResp			- Pointer to a RESPONSE structure containing temperature response data 
//			7. pCardTemp		- pointer to a CARDTEMP structure containing the cardinal temperature for leafing
//			8. pOut				- Pointer to a OUTPUT structure holding the output values
//Outputs:	1. pOut->fValue1	- Calculated initiation rate of leaf primodia (Primodia/d)
//			2. pOut->fValue2	- Calculated emergence rate of leaf on main stem (Leaves/d)
//Functions Called:
//			None 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the temperature response function for leafing. If not, the cardinal temperatures
//			at pCardTemp will be used to construct the temperature response function. If pCardTemp equals NULL,
//			a minimum, optimum and maximum temperature of 0, 22 and 35C will be assumed respectively.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int spass_MainStemLeafing(double fMaxLfInitRate,double fMaxLfEmergRate,double fTemp, double fDaylenEff,
						  double fStageVR, PRESPONSE pResp,PCARDTEMP pCardTemp, POUTPUT pOut)
	{ 
	double fMinTemp,fOptTemp,fMaxTemp,fTempFunc;  
	double fLfPrimInitRate,fLfEmergRate,fDevFI,fDevFlg;

	fDevFI=(double)0.15; fDevFlg = (double)0.65;
	
	//The cardinal temperature for leaf initiation and emergence 
	if (pCardTemp==NULL)
		{
		fMinTemp=(double)0.0;
		fOptTemp=(double)22.0;
		fMaxTemp=(double)35.0;
		}
	else
		{
		fMinTemp=pCardTemp->fMinTemp;
		fOptTemp=pCardTemp->fOptTemp;
		fMaxTemp=pCardTemp->fMaxTemp;
		} 
	
	//The temperature function for leaf initiation and emergence 
	if ((pResp==NULL)||(pResp[0].fInput==(double)-99.9))
		fTempFunc = spass_RelativeTemperatureResponse(fTemp,fMinTemp,fOptTemp,fMaxTemp);
	else
		fTempFunc = spass_AFGENERATOR(fTemp, pResp);
		
	//Leaf initiation
	if ((fStageVR>=(double)0.0)&&(fStageVR<=fDevFI))  
		fLfPrimInitRate = fMaxLfInitRate*fTempFunc*fDaylenEff;//fThermEffect*fDaylenEffect;//Leaf appearing rate (leaves /day)
	else
		fLfPrimInitRate = (double)0.0;
		
	//Leaf Appearance
	fLfEmergRate=fMaxLfEmergRate*(double)fTempFunc*fDaylenEff;

	//Flag leaf 
	if (fStageVR>=fDevFlg)	fLfEmergRate = (double)0.0; 
	
	pOut->fValue1 = fLfPrimInitRate;
	pOut->fValue2 = fLfEmergRate;
	
	return RET_SUCCESS;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds CerealTillering_SPASS(double fLfEmergRate, double fExpLfNum, double fTillNum, double fTillDem, 
//											double fStemWght,    double fPltDens,  double fStageVR)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates tiller number per square meter of cereal crops 
//Inputs:	1. fLfEmergRate	- Leaf emergence rate (lvs/d) calculated by MainStemLeafing()
//			2. fExpLfNum	- Expended leaf number on main stem (leaves) 	
//			3. fTillNum		- Current tiller number per square meter (Tillers/m2) 	
//			4. fTillDem		- Assimilate demand per single tiller (g)
//			5. fStemWght	- Current dry weight of stems (kg/ha)
//			6. fPltDens		- Plant density (plants/m2)
//			7. fStageVR		- Current VR development stage value (-) 	
//Outputs:	1. Return		- Calculated tiller number per square meter (Tillers/m2)
//Functions Called:
//			None 
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_CerealTillering(double fLfEmergRate, double fExpLfNum, double fTillNum, double fTillDem, 
								double fStemWght,    double fPltDens,  double fStageVR, double DeltaT)
	{
    double fTotTillerDemand, fTillerNumber,fTillerNumSq;
	
	if (fStageVR>=(double)0.70) return fTillNum*fPltDens;

	if (fExpLfNum < (double)2.5) return fTillNum*fPltDens;
		
	//Tiller number per plant
	if (fStageVR<=(double)0.39)	//Jointing  			
	    //fTillerNumber = fTillNum+ max((double)0.0, fLfEmergRate*(fExpLfNum-(double)2.5));
		//SG/09/06/99/ Alternativvorschlag: (fMaxLfEmergRate in Plant.gtp halbiert)
		{fTillerNumber = fTillNum+fLfEmergRate*DeltaT;
		}
		else
	    fTillerNumber = fTillNum;
    
	//Total assimilate demand for all tillers per hacter
    fTotTillerDemand =fPltDens*fTillerNumber*fTillDem*fStageVR*fStageVR*fStageVR*(double)(1E-3*1E+4);
	
	//printf("%f %f %f %f %f \n",fExpLfNum, fTillerNumber, fStageVR, fStemWght, fTotTillerDemand);

	//printf("%f %f \n", fTillNum, fTillerNumber*fStemWght/fTotTillerDemand);

	//Tiller number per square meter
	//SG/25/05/99: Modifikation: fTillerNumSq kann nur wachsen
   	fTillerNumSq = max(fPltDens*fTillNum, fPltDens*max((double)1.0, fTillerNumber*min((double)1.0,fStemWght/(fTotTillerDemand))));
	return fTillerNumSq;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	double WINAPI _loadds LeafAreaGrowthRate_SPASS(double fLfGrwRate,double  fStageVR,double fAveSpcLfWt, 
//									   PRESPONSE pResp,PTRANSP pTransp, PORGANNC pLfNc) 
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates daily leaf area growth rate based on the leaf biomass growth rate and a 
//			development stage-dependent specific leaf weight 
//			the main stem of cereal crops
//Inputs:	1. fLfGrwRate		- Leaf biomass growth rate (kg/ha.d)
//			2. fStageVR			- Current VR development stage value (-) 	
//			3. fAveSpcLfWt		- Average specific leaf weight of the whole growing period (kg/ha leaf) 	
///			4. pResp			- Pointer to a RESPONSE structure containing stage dependent coefficients for
//								  correcting fAveSpcLfWt 
//			5. pTransp			- pointer to a TRANSP structure containing daily actual and potential transpiration
//			6. pLfNc			- Pointer to a ORGANNC structure containing leaf nitrogen concentration data
//Outputs:	1. Return 			- The calculated growth rate of leaf area index (1/d) 
//Functions Called:
//			AFGENERATOR 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the development stage response function for specific leaf weight. If not, the 
//			average specific leaf weight will be used through all the stages. If pTransp or pLfNc equals NULL,
//			the water stress or nitrogen stress will not be considered, i.e, leaf grows at optimal conditions.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_LeafAreaGrowthRate(double fLfGrwRate,double  fStageVR,double fAveSpcLfWt, 
								   PRESPONSE pResp,PTRANSP pTransp, PORGANNC pLfNc) 
	{
	double fLfAreaGrwRate,fSpcLfArea,fWaterIndex,fWaterStress,fLfNcIndex,fLfNStress;
	
	//Specific Leaf area
	if ((pResp==NULL))//||(pResp->fInput==(double)0.0))  
		fSpcLfArea = (double)1.0/fAveSpcLfWt;
	else
		fSpcLfArea = (double)1.0/(fAveSpcLfWt*spass_AFGENERATOR(fStageVR, pResp));  

	//Water Stress Factor
	if ((pTransp==NULL)||(pTransp->fPotTransp==(double)0.0))
		fWaterStress = (double)1.0;
	else
		{	
		fWaterIndex = pTransp->fActTransp/pTransp->fPotTransp;
		fWaterStress= (double)pow((double)fWaterIndex, 2);
	    }

	//SG/13/04/99//////////////////////////////////////////////////////////////////
	//
	//	Folgende Zeile aus PLAGEN:
	///////////////////////////////////////////////////////////////////////////////
	// if(fStageVR<=(double)0.1) 
	//	fWaterStress = (double)1.0;
	    
	//Nitrogen Stress Factor


	//SG/13/04/99/////////////////////////////////////////////////////////////
	//
	//	Angleichung an PLAGEN:
	//
	//if ((pLfNc==NULL)||(pLfNc->fOptNc==(double)0.0))
	//////////////////////////////////////////////////////////////////////////
	if((pLfNc==NULL)||(pLfNc->fActNc<=(double)0.0))
		fLfNStress = (double)1.0;
	else
		{
		fLfNcIndex  = (pLfNc->fActNc-pLfNc->fMinNc)/(pLfNc->fOptNc-pLfNc->fMinNc+(double)1e-9);
		fLfNStress = (double)min(1.0,sqrt((double)fLfNcIndex));
	    }

	//Leaf area index growth rate	
    fLfAreaGrwRate = fLfGrwRate*fSpcLfArea*fWaterStress*fLfNStress;

	return fLfAreaGrwRate;
	}


/*******************************************************************************
** EOF */
