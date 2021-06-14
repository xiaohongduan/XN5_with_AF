//      spass_water.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_water.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*int spass_water_capture(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PCLIMATE	pCl = xpn ->pCl;
	PPLANT		pPl = xpn ->pPl;

	PWEATHER	pWth =pCl->pWeather;
    PCANOPY		pCan =pPl->pCanopy;
    PPLTWATER	pPltW=pPl->pPltWater;
    
	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		return RET_SUCCESS;
	
	spass_CanopyWaterInterception(self);
	//pPltW->fPotTranspDay	= PotentialTranspiration_SPASS(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	spass_potential_transpiration(self);

	spass_actual_transpiration(self);
	spass_PlantWaterStress(self);

    return RET_SUCCESS;
  }*/

int	spass_CanopyWaterInterception(spass *self)
	{
		
    expertn_modul_base *xpn = &(self->parent);

	PCLIMATE	pCl = xpn ->pCl;
	PPLANT		pPl = xpn ->pPl;
	PMANAGEMENT	pMa = xpn ->pMa;
	PTIME		pTi = xpn ->pTi;

	PWEATHER	pWth =pCl->pWeather;
    PCANOPY		pCan =pPl->pCanopy;
    PPLTWATER	pPltW=pPl->pPltWater;	

	double		fInterception;
	
	if (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pMa->pSowInfo->Year,pMa->pSowInfo->Month,pMa->pSowInfo->Day)<0)
		return RET_SUCCESS;
	
	fInterception = self->fInterceptionCapacity*pCan->fLAI;
	
	if (fInterception>pWth->fPreciRate)
		fInterception=pWth->fPreciRate;

	pPltW->fInterceptR = fInterception;
		
	return RET_SUCCESS;
	}

int	spass_potential_transpiration(spass *self)
	{

    expertn_modul_base *xpn = &(self->parent);	

	PTIME		pTi  = xpn ->pTi;
	PWATER		pWa  = xpn ->pWa;
	//PMANAGEMENT	pMa  = xpn ->pMa;
	PPLTWATER 	pPLW = xpn->pPl->pPltWater;
	PPLANT		pPl  = xpn->pPl;

	if (PLANT_IS_GROWING)
	{

	/* daily potential transpiration */
	pPLW->fPotTranspR = pWa->fPotETR - pWa->pEvap->fPotR - pPLW->fInterceptR;
	//pPl->pPltWater->fPotTranspDay = fPotTr;
	//fPotTraDay += fPotTrR*DeltaT;
	
	/* time step potential transpiration */
	//pPl->pPltWater->fPotTranspdt = pWa->fPotETdt - pWa->pEvap->fPotR * DeltaT;
	
	if(pPLW->fPotTranspR < 0.0)
	{
		pPLW->fPotTranspR = 0.0;
	}
	
	pPLW->fPotTranspdt = pPLW->fPotTranspR*pTi->pTimeStep->fAct;	
	
	}
	return RET_SUCCESS;
	}

int		spass_PlantWaterStress(spass *self)
	{

    expertn_modul_base *xpn = &(self->parent);	

	PPLANT		pPl = xpn->pPl;

	PPLTWATER 	pPltW = pPl->pPltWater;

//	double  WsFact[] = {3,  0.0, 1.0, 2.0};
//	double  WsDevR[] = {3,  1.0, 1.0, 1.0};
	
	//Stress factor on development rate
//	pPltW->fWsDevRate = AFGEN(pPltW->fSWDF1, WsFact, WsDevR);

	//Direct effect of air humidity on Pmax
	//pPltW->fWsPsRH  = AFGEN(VPDC );

	//=======================================================================================
	//	Plant water stress factor
	//=======================================================================================
	//Stress factor for CO2 assimilation
	//pPltW->fStressFacPhoto=min((double)1.0, pPltW->fActTranspDay/pPltW->fPotTranspDay);
	if (pPltW->fPotTranspR != 0.0) pPltW->fStressFacPhoto=min((double)1.0, pPltW->fActTranspR/pPltW->fPotTranspR);
	
	//Stress factor on expension growth
   	//pPltW->fStressFacLeaf = min((double)1.0, (double)0.67*pPl->pPltWater->fPotUptakedt/pPltW->fPotTranspDay);
   	if (pPltW->fPotTranspR != 0.0) pPltW->fStressFacLeaf = min((double)1.0, (double)0.67*pPl->pPltWater->fPotUptakedt/pPltW->fPotTranspR);

	//Stress factor on carbohydrate partitioning
	pPl->pPltWater->fStressFacPartition = min((double)1.0,(double)0.5+pPltW->fStressFacPhoto);

	if (pPl->pDevelop->iStageCERES<2)
		{
		pPltW->fStressFacPhoto		=(double)1.0;
		pPltW->fStressFacLeaf		=(double)1.0;
		pPltW->fStressFacTiller		=(double)1.0;
		pPl->pPltWater->fStressFacPartition			=(double)1.0;
		}

	return RET_SUCCESS;
	}


int spass_actual_transpiration(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PSPROFILE		pSo		=xpn->pSo;
	PWATER			pWa		=xpn->pWa;
	PPLANT			pPl		=xpn->pPl;
	//PMANAGEMENT		pMa		=xpn->pMa;
	PTIME			pTi		=xpn->pTi;

	PSLAYER 		pSL		=pSo->pSLayer;
	PSWATER			pSWL	=pSo->pSWater;
	PWLAYER	   		pSLW	=pWa->pWLayer; 
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
	//PROOT			pRT		=pPl->pRoot;  
    PPLTWATER		pPltW	=pPl->pPltWater;

	int   			L;
	double 		TRWU,WUF,TLL,RWUMX,fExtWater,fTotSoilWater,fThickness,fContAct;
	 
	if ((pPl->pDevelop->iDayAftEmerg > 0) && (PLANT_IS_GROWING))
	{


    //RWUMX =(double)0.03;
	//RWUMX =pPl->pGenotype->fMaxWuptRate;   

	//SG 20120126: 
	// Abnahme der maximalen Wasseraufnahmerate RWUMX (eigentl. des Gesamtwiderstands 
	// des Xylems inkl. Stomatawiderstand!) nach Beginn der Seneszenz
    RWUMX =pPl->pGenotype->fMaxWuptRate * (double)min(1.0,max(0.0,((double)2.1 - pPl->pDevelop->fStageSUCROS)/((double)2.1 - pPl->pGenotype->fBeginSenesDvs)));
             

	pLR	=pPl->pRoot->pLayerRoot;

    for (L=1;L<=pSo->iLayers-2;L++)
    	{
		pLR->fActWatUpt	   	=(double)0.0;
		pLR->fActLayWatUpt 	=(double)0.0;
		pLR->fPotWatUpt		=(double)0.0;
		pLR->fPotLayWatUpt	=(double)0.0;

      	pLR =pLR ->pNext;
		}
/*
   	if (pPltW->fPotTranspDay<=(double)0.001)
   		{
        pPltW->fActTranspDay	=(double)0.0;
    	pPltW->fStressFacPhoto 	= (double)1.0;
		pPltW->fStressFacLeaf 	= (double)1.0;
 		return 0;
		}
*/	
	if (pPltW->fPotTranspR<=(double)0.001)
   		{
        pPltW->fActTranspR		= (double)0.0;
    	pPltW->fStressFacPhoto 	= (double)1.0;
		pPltW->fStressFacLeaf 	= (double)1.0;
		spass_PlantWaterStress(self);	
 		return RET_SUCCESS;
		}

	//=======================================================================================
	//			Root Water Uptake
	//=======================================================================================
	L 	  = 1;
	TRWU  =(double)0.0;
	pPl->pPltWater->fCumShortage = (double)0.0;
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	pLR	=pPl->pRoot->pLayerRoot;
	while ((pLR->fLengthDens!=(double)0.0)&&(L<pSo->iLayers-2))
		{
		fThickness = (double)0.1*pSL->fThickness; //mm to cm
		
        //Root water uptake per unit root length
        fExtWater = max((double)0.0,pSLW->fContAct-pSWL->fContPWP);

		pPl->pPltWater->fCumShortage += fExtWater;

        if (pSLW->fContAct<=pSWL->fContPWP)
        	pLR->fPotWatUpt=(double)0.0;
        else
        	{
        	pLR->fPotWatUpt=(double)(2.67*1E-3*exp(min(62.0*(double)fExtWater,10.0))
            								/(6.68-log((double)pLR->fLengthDens)));
            pLR->fPotWatUpt=min(RWUMX,pLR->fPotWatUpt);
								
			CHECK_VALID(pLR->fPotWatUpt)
            }

		//Root water uptake per layer
		//printf("%f %f %f \n", pLR->fPotWatUpt, fThickness, pLR->fLengthDens);
		pLR->fPotLayWatUpt=pLR->fPotWatUpt*fThickness*pLR->fLengthDens*(double)10.0
								*((double)0.18+(double)0.00272*(pLR->fLengthDens-(double)18.0)
			 					*(pLR->fLengthDens-(double)18.0));
		if ((pPl->pDevelop->fCumPhyll<(double)4.0)&&(pPl->pCanopy->fLAI<(double)1.0))
			pLR->fPotLayWatUpt *= ((double)3.0-(double)2.0*pPl->pCanopy->fLAI);

		TRWU += pLR->fPotLayWatUpt;
		L 	 ++;

      	pSL =pSL ->pNext;
      	pSWL=pSWL->pNext;
      	pSLW=pSLW->pNext;
      	pLR =pLR ->pNext;
      	}
   
	pPl->pPltWater->fPotUptakedt=TRWU*pTi->pTimeStep->fAct;
	//=======================================================================================
	//			Root Water Uptake Modification  and Soil Water Content
	//=======================================================================================
	/*
	if (pPltW->fPotTranspDay<=TRWU)
    	WUF = pPltW->fPotTranspDay/TRWU;
	*/
	if (pPltW->fPotTranspR<=TRWU)
		WUF = pPltW->fPotTranspR/TRWU;
    else
        WUF = (double)1.0;


    TLL=(double)0.0;

	fTotSoilWater=(double)0.0;

 	pPl->pPltWater->fActUptakedt=(double)0.0;

	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	pLR	=pPl->pRoot->pLayerRoot;
    for (L=1;L<=pSo->iLayers-2;L++)
    	{
		pLR->fActWatUpt	 	=  pLR->fPotWatUpt   *WUF;
		pLR->fActLayWatUpt 	=  pLR->fPotLayWatUpt*WUF;//pTi->pTimeStep->fAct;

		fContAct  = pSLW->fContAct;
		fContAct -= pLR->fActLayWatUpt*pTi->pTimeStep->fAct/pSL->fThickness; //mm/mm

		if (fContAct<pSWL->fContPWP)
			{
			pLR->fActWatUpt	   *= fContAct/pSWL->fContPWP;
			pLR->fActLayWatUpt *= fContAct/pSWL->fContPWP;
//			pSLW->fContAct=pSWL->fContPWP;
			}

		fTotSoilWater 	+= fContAct*fThickness*(double)10.0;

		pPl->pPltWater->fActUptakedt += pLR->fActLayWatUpt*pTi->pTimeStep->fAct;
		TLL   +=pSWL->fContPWP;
		
		CHECK_VALID(pLR->fActLayWatUpt)
		

		//pSLW->fContAct = fContAct;

      	pSL =pSL ->pNext;
      	pSWL=pSWL->pNext;
      	pSLW=pSLW->pNext;
      	pLR =pLR ->pNext;
		}

//    lpSoil->lpWater->fTotExtSoilWater = lpSoil->lpWater->fTotSoilWater-TLL;

/*
   if (pPltW->fPotTranspDay>=TRWU)
        pPltW->fActTranspDay=TRWU;
    else
        pPltW->fActTranspDay=pPltW->fPotTranspDay;
        
	pPl->pRoot->fUptakeR = pPltW->fActTranspDay;    
	
*/

   if (pPltW->fPotTranspR>=TRWU)
        pPltW->fActTranspR=TRWU;
    else
        pPltW->fActTranspR=pPltW->fPotTranspR;
        
		//{double t_act_rate;
		//t_act_rate = pPltW->fActTranspR;
	//C_DEBUG(t_act_rate);
		//}
		
   pPl->pRoot->fUptakeR = pPl->pPltWater->fActUptakedt/pTi->pTimeStep->fAct;    
	//eigentlich: 	pPl->pRoot->fUptakeR = fActTraDay;    
	

	//=======================================================================================
	//	Water Deficit and water demand
	//=======================================================================================
//	pPltW->fWaterShortage     =pPltW->fPotTranspdt-pPltW->fActTranspDay;	
//	pPltW->fCumWaterDemand   += pPltW->fPotTranspDay;
//	pPltW->fCumWaterShortage += pPltW->fWaterShortage;


	spass_PlantWaterStress(self);

	//for (L=1;L<=pSo->iLayers-2;L++)
	//   	{
	//	//self->afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
	//	//pSLW->fContAct -= self->afActLayRootWatUpt[L]*pTi->pTimeStep->fAct/pSo->fDeltaZ;
	//	pSLW->fContAct -= pLR->fActLayWatUpt*pTi->pTimeStep->fAct/pSo->fDeltaZ;
	//	pSL =pSL ->pNext;
	//	pSLW=pSLW->pNext;
	//	pLR =pLR ->pNext;
	//	}

	//pLR     =pPl->pRoot->pLayerRoot;
 //   for (L=1;L<=pSo->iLayers-2;L++)
 //       {
 //       self->afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
 //       pLR =pLR ->pNext;
 //       }

    pSL     =pSo->pSLayer->pNext;
    pSLW	=pWa->pWLayer->pNext; 
    pLR     =pPl->pRoot->pLayerRoot;

    for (L=1;L<=pSo->iLayers-2;L++)
        {
		self->afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
        pSLW->fContAct -= self->afActLayRootWatUpt[L] *pTi->pTimeStep->fAct/pSL->fThickness;
//                                                            /DeltaZ
//                                                            *DeltaT;//mm/mm
//                                                            /pSL->fThickness
		
        //mm/mm
        CHECK_VALID(pSLW->fContAct)
		pSL =pSL ->pNext;
        pSLW=pSLW->pNext;
        pLR =pLR ->pNext;
        }

  } //end PLANT_IS_GROWING

    return RET_SUCCESS;
  }

// FH 2017-06-20
// Not working really well, therefore uncommented
/*int spass_PotEvapotraPenman(spass *self)
{

  expertn_modul_base *xpn = &(self->parent);

  PWEATHER    pWth = xpn->pCl->pWeather;   
  PEVAP       pEvp = xpn->pWa->pEvap;
  PDEVELOP    pDev = xpn->pPl->pDevelop; 
  PPLTCARBON  pPlC = xpn->pPl->pPltCarbon;
  PTIME		  pTi  = xpn->pTi;
  PLOCATION	  pLo  = xpn->pLo;
  PPLANT	  pPl  = xpn->pPl;
  PWATER	  pWa  = xpn->pWa;
  PMANAGEMENT pMa  = xpn->pMa;

  double vGlobalRadiation,vLongRadIn,vLongRadOut,vRadMax,vNetRad,vDry;
  double vPotTr,vPotTrRad,vPotTrDry;
  double vSatVP,vSatVPave,vActVP,vSlope,vWind,vWindDay,vWindR;
  double vTempDay,vTempAve,vTempMin,vRH,vDaylen,vCa,vCi;
  double vLeafWidth,vPltHght,vLAI,vPSgross,vPn,vMaxPs,vMaintLv;
  double Rbv,Rcv,Rlv,Rlvm,Rlvx,Rch,Rbh,vZr,vZo,vZd;
  double vBoltzm,vPsych,vLH,vVHair,vAlbedo,vPsych1;
  double DeltaT = pTi->pTimeStep->fAct;
  int i;

  Rch = 0.0;  

  //vGlobalRadiation = (double)  pWth->fSolRad*1E6;
  vGlobalRadiation = (double)  pWth->fPAR*1E6;
//  vTempDay     = (double)  ((float)0.71*pWth->fTempMax+(float)0.29*pWth->fTempMin);
 // vTempAve     = (double)  ((float)0.5*(pWth->fTempMax+pWth->fTempMin));
  vTempAve     = 0.0;
  //Calculate daily mean-temperature
  self->fTemp_zwischen[self->iMeanTemp] = pWth->fTempAir;
  self->iMeanTemp++;
  if (self->iMeanTemp == (int)(1.0/DeltaT)) self->iMeanTemp = 0;
  for (i = 0; i<(int)(1.0/DeltaT);i++)
	vTempAve += self->fTemp_zwischen[i]*DeltaT;

  vTempMin = min(self->fTemp_zwischen[0],self->fTemp_zwischen[1]);
  for (i = 2; i<(int)(1.0/DeltaT);i++)
	  vTempMin = min(vTempMin,self->fTemp_zwischen[i]);

  vTempDay     = 1.42*vTempAve - 0.42*vTempMin;

  
  vTempAve = pWth->fTempAir;
  vTempDay = pWth->fTempAir;

  vRH         = (double)  pWth->fHumidity;
  vWind       = (double)  pWth->fWindSpeed;

  vDaylen       = (double)  spass_DaylengthAndPhotoperiod(pLo->pFarm->fLatitude,
                                                     (int)pTi->pSimTime->fTimeY,1);
  vRadMax      = (double)  spass_ExtraterrestrialRadiation(pLo->pFarm->fLatitude, 
                                                       (int)pTi->pSimTime->fTimeY);

  if (vWind==-99.00)
    vWind=2.0;
  if (vRH==-99.00)
    vRH=65.0;
  vWindR   = max(0.2, 1.333*vWind);

  //______________________________________________________________________________
  //Constants
  vBoltzm = 4.8972E-3;   //Stefan-Boltznan constant   (J /m2/d/K4)
  vPsych  = 0.67;    //Psychrometric instrument constant (mbar/C)   
  vLH    = 2.4E6;  //Latent heat of evaporation of water (J/kg H2O)
  vVHair  = 1200;    //Volumetric heat capacity of the air (J/m2/K)
  vAlbedo = 0.25;     //Leaf albedo
  //vAlbedo = xpn->pSo->fAlbedo;

  
  //planted or nearly bare soil  
  /*if (pPl->pDevelop->iDayAftEmerg == 0)
  {
  vLAI  = (double)0;
  vPsych1 = vPsych*10;
  Rbh     = (double)0;
  Rcv     = 0.74*pow(log((2 - 0.1)/0.05),2)/(0.16*vWindR);
  }
  else
  {
  vLAI  = (double)  pPl->pCanopy->fLAI;
  //______________________________________________________________________________
  //Boundary Layer Resistance                                 
  vWindDay = 1.333*max(0.2,vWind);
  vLeafWidth = 0.01; //m
  
  Rbv = 0.5*172*sqrt(vLeafWidth/(vWindDay*0.6));
  
  //______________________________________________________________________________
  //Canopy resistance

  vPltHght = (double)pPl->pGenotype->fPlantHeight*
                     min(1.0, (double)pDev->fStageSUCROS)/100.0;
  
  vZd = max(0.10, 0.63*vPltHght);
  vZo = max(0.05, 0.10*vPltHght);
  vZr = max(2.00, 1.0+vPltHght);
  
  Rcv = 0.74*max(1.0,vLAI)*pow(log((vZr-vZd)/vZo),2)/(0.16*vWindR);

  //______________________________________________________________________________
  //Leaf resistance
  vCa   = xpn->pCl->pWeather->fAtmCO2ppm;
  vCi   = vCa*(double)pPl->pGenotype->fCiCaRatio;
  vMaxPs   = (double)pPl->pGenotype->fMaxGrossPs;
  vMaintLv= (double)pPlC->fMainRespRLeaf;
  vPSgross= (double)pPlC->fGrossPhotosynR;
  
  Rlvm = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(0.9*(vMaxPs+1E-9))/1.6-10.0;
  Rlvx = 2000.0; //s/m
  
  vPn = max(0.0, vPSgross/vDaylen - 0.25*(vMaintLv/0.75)/24.0);
  vPn = vPn/min(2.5, vLAI);
  Rlv = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(vPn+1E-9)/1.6-Rbv-Rcv;
  
  Rlv = (double)spass_LIMIT((double)Rlvm,(double)Rlvx,(double)Rlv);
    
  //______________________________________________________________________________
   //Apparent Psychrometric constant (mbar/C)
  Rbh = Rbv/0.93;
  Rch = Rcv;
  
  vPsych1  = vPsych*(Rlv+Rbv+Rcv)/(Rbh+Rch);//Psychrometric instrument constant (mbar/C)   


  } //else: planted or nearly bare soil

  Rch = Rcv;

  //______________________________________________________________________________
  //Saturated and actural vapour pressure of the air (mbar)
  vSatVP = 0.1*6.11*exp(17.4*vTempDay/(vTempDay+239.0));
  vSlope = 4158.6*10*vSatVP/((vTempDay+239.0)*(vTempDay+239.0));

  vSatVPave = 0.1*6.11*exp(17.4*vTempAve/(vTempAve+239.0));
  vActVP = vSatVPave*vRH/100.0;
  
  //______________________________________________________________________________
  //Radiation Term 
  vLongRadIn =vBoltzm*pow((vTempDay+273.0),4)*(0.618+0.00365*sqrt(10*vActVP));
  vLongRadOut=vBoltzm*pow((vTempDay+273.0),4);
 
   vNetRad = (1.0-vAlbedo)*vGlobalRadiation
    -(vLongRadOut-vLongRadIn)*(vGlobalRadiation/(0.75*vRadMax*1E6))*(vDaylen/24.0);

  vPotTrRad = vNetRad/vLH*(vSlope/(vSlope+vPsych1));
  
  //______________________________________________________________________________
  //Drying Term
  vDry = 10.0*(vSatVP-vActVP)*vVHair/(Rbh+Rch)*(vDaylen/24.0);
  vPotTrDry=vDry/vLH/(vSlope+vPsych1)*(24.0*3600.0);

  //printf("vDry %f, vLH %f, vSlope %f, vPsych1 %f \n", vDry, vLH, vSlope, vPsych1);

  vPotTr  = vPotTrRad*(1.0-exp(-0.5*vLAI)) + vPotTrDry*min(2.5,vLAI);


    // daily potential evapotranspiration 
  pWa->fPotETR = (double)vPotTrRad + (double)(vPotTrDry*(1.0+min(2.5,vLAI)));//max(1.0,min(2.5,vLAI)));

  // daily potential evaporation 
//  pEvp->fPotDay = pWa->fPotETDay - (float)vPotTr;
  pWth->fPanEvap = pWa->fPotETR - (double)vPotTr; 
  // Missbraucht da pWa->pEvap->fPotDay
  // in balance.c zu Null gesetzt wird!!


  // timestep potential (evapo-)transpiration 
//  EvTeiler = SolPartTime(pTi);
//
    //EvTeiler = (DeltaT < (double)0.11)? 
    //     SolPartTime(pTi)
    //     : (double)1;
//   
  /*pWa->fPotETdt =   pWa->fPotETR*DeltaT;

//  
  // daily potential evaporation 
//  pEvp->fPotDay = pWa->fPotETDay - (float)vPotTr;
  pEvp->fPotR = pWth->fPanEvap; //[mm/day];
  
    // potential evaporation rate     
      // pEvp->fPotR = pEvp->fPotDay;// [mm/day]
//       pEvp->fPotR = EvTeiler * pWth->fPanEvap/ DeltaT;// [mm/day]
      

  if (pPl != NULL)
  {
  // daily potential transpiration 
  pPl->pWater->fPotTranspDay = (float)vPotTr;
  // time step potential transpiration 
  pPl->pWater->fPotTranspdt = EvTeiler * pPl->pWater->fPotTranspDay;
  }
 

	pEvp->fPot = pEvp->fPotR*DeltaT;

  return RET_SUCCESS;

  }*/


/*double spass_ExtraterrestrialRadiation(double fLatitude, int nJulianDay)
{
  //Stapper (1984)
  double vLatitude,vDay,vDec,RAD,AOB,SC,DAYL,DSINB;
  double  fRadMax;
  
  RAD     = PI/180.0;
  vLatitude = (double)fLatitude;
  vDay     = (double)nJulianDay;
  
  //Declination of the sun as function of iJulianDaynumber (iJulianDay)
  vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0));
  
  //AOB are intermediate variables
  AOB   = sin(RAD*vLatitude)*sin(vDec)/cos(RAD*vLatitude)*cos(vDec);
  
  //Daylength (DAYL) 
  DAYL   = 12.0*(1.0+2.0*asin(AOB)/PI);

  DSINB  = 3600.0*(DAYL*sin(RAD*vLatitude)*sin(vDec)+24.0*cos(RAD*vLatitude)*cos(vDec)
        *sqrt(1.0-AOB*AOB)/PI);

  //Solar constant (SC) 
  SC  = 1370.0*(1.0+0.033*cos(2.0*PI*vDay/365.0));
  
  fRadMax=(double)(SC*DSINB*1E-6);   //MJ/m2.d

  return fRadMax;
  }*/



//double alt_spass_potential_transpriation(spass *self)
//	{
//    expertn_modul_base *xpn = &(self->parent);
//
//	PCLIMATE	pCl = xpn->pCl;
//	PPLANT		pPl = xpn->pPl;
//	PTIME		pTi = xpn->pTi;
//	PLOCATION	pLo = xpn->pLo;
//
//	PWEATHER	pWth=pCl->pWeather;   
//	PDEVELOP	pDev=pPl->pDevelop; 
//	PPLTCARBON	pPlC=pPl->pPltCarbon;
//
//	double vGlobalRadiation,vLongRadIn,vLongRadOut,vRadMax,vNetRad,vDry;
//	double vPotTr,vPotTrRad,vPotTrDry;
//    double vSatVP,vSatVPave,vActVP,vSlope,vWind,vWindDay,vWindR,vTempDay,vTempAve,vRH,vDaylen,vCa,vCi;
//    double vLeafWidth,vPltHght,vLAI,vPSgross,vPn,vMaxPs,vMaintLv;
//    double Rbv,Rcv,Rlv,Rlvm,Rlvx,Rch,Rbh,vZr,vZo,vZd;
//    double vBoltzm,vPsych,vLH,vVHair,vAlbedo,vPsych1;
//
//	CARDTEMP CardTmp;	//SG/15/04/99: für Anpassung an PLAGEN;
//	ORGANNC OrgNc;
//	double	fCO2Cmp,fCO2R,fPgmmax,fMaxLUE;
//
//	    
//	vGlobalRadiation	= (double)	pWth->fSolRad*1E6;
//	vTempDay			= (double)  ((double)0.71*pWth->fTempMax+(double)0.29*pWth->fTempMin);
//	vTempAve			= (double)  ((double)0.5*(pWth->fTempMax+pWth->fTempMin));
//	vRH					= (double)	pWth->fHumidity;
//	vWind				= (double)	pWth->fWindSpeed;
//
//	vDaylen				= (double)  spass_DaylengthAndPhotoperiod(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay,1);
//	vRadMax 			= (double)  spass_ExtraterrestrialRadiation(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay);
//	vLAI				= (double)	pPl->pCanopy->fLAI;
//
//	if (vWind==-99.00)
//		vWind=2.0;
//	if (vRH==-99.00)
//		vRH=65.0;
//	//______________________________________________________________________________
//	//Constants
//	vBoltzm = 4.8972E-3; 	//Stefan-Boltznan constant   (J /m2/d/K4)
//	vPsych	= 0.67;		//Psychrometric instrument constant (mbar/C)	 
//	vLH		= 2.4E6;	//Latent heat of evaporation of water (J/kg H2O)
//	vVHair	= 1200;		//Volumetric heat capacity of the air (J/m2/K)
//    vAlbedo = 0.25;     //Leaf albedo
//	//______________________________________________________________________________
//	//Boundary Layer Resistance                                 
//	vWindDay = 1.333*max(0.2,vWind);
//	vLeafWidth = 0.01; //m
//	
//	Rbv = 0.5*172*sqrt(vLeafWidth/(vWindDay*0.6)); 
//	
//	//______________________________________________________________________________
//	//Canopy resistance
//	vPltHght = (double)pPl->pGenotype->fPlantHeight*min(1.0, (double)pDev->fStageSUCROS)/100.0;
//	vWindR   = max(0.2, 1.333*vWind);
//	
//	vZd = max(0.10, 0.63*vPltHght);
//	vZo = max(0.05, 0.10*vPltHght);
//	vZr = max(2.00, 1.0+vPltHght);
//	
//	Rcv = 0.74*max(1.0,vLAI)*pow(log((vZr-vZd)/vZo),2)/(0.16*vWindR);
//
//	//______________________________________________________________________________
//	//Leaf resistance
//	vCa 	= (double)CO2;
//	vCi 	= vCa*(double)pPl->pGenotype->fCiCaRatio;
//
//
//	//SG/15/04/99//////////////////////////////////////////////////////
//	//	Fehler!
//	//	vMaxPs ist nicht die maximale Photosyntheserate eines Blatts,
//	//	sondern die tatsächliche! (siehe Doktorarbeit, Gl.3.2.122)
//	//
//	//	vMaxPs 	= (double)pPl->pGenotype->fMaxGrossPs;
//
//	fPgmmax = pPl->pGenotype->fMaxGrossPs;
//	fMaxLUE = pPl->pGenotype->fLightUseEff;
//
//	CardTmp.fMinTemp = pPl->pGenotype->fTempMinPs;
//	CardTmp.fOptTemp = pPl->pGenotype->fTempOptPs;
//	CardTmp.fMaxTemp = pPl->pGenotype->fTempMaxPs;
//
//	OrgNc.fMinNc = pPl->pPltNitrogen->fLeafMinConc;
//	OrgNc.fActNc = pPl->pPltNitrogen->fLeafActConc;
//	OrgNc.fMaxOptNc = pPl->pPltNitrogen->fLeafOptConc;
//
//	fCO2Cmp = pPl->pGenotype->fCO2CmpPoint;
//	fCO2R = pPl->pGenotype->fCiCaRatio;
//	
//	vMaxPs = LeafMaxGrossPhotosynthesis_SPASS(fPgmmax,(double)vTempDay,
//		(double)vCa,fCO2Cmp,fCO2R,pPl->pGenotype->PmaxTemp,&CardTmp,&OrgNc,pPl);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	//SG/15/04/99//////////////////////////////////////////////////////////////
//	//
//	//	An dieser Stelle wird in PLAGEN die Brutto-Photosynthese neu berechnet.
//	//	
//	//	vPSgross= (double)pPlC->fGrossPhotosynR;
//
//	vPSgross = DailyCanopyGrossPhotosythesis_SPASS("C3",fPgmmax,fMaxLUE,fCO2Cmp,fCO2R,
//		pPl->pGenotype->PmaxTemp,&CardTmp,&OrgNc,(double)vLAI,pLo->pFarm->fLatitude, 
//		(int)pTi->pSimTime->iJulianDay,pWth->fSolRad,pWth->fTempMax,pWth->fTempMin,(double)vCa,pPl);
//	
//	Rlvm = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(0.9*(vMaxPs+1E-9))/1.6-10.0;
//	Rlvx = 2000.0; //s/m
//
//
//		vMaintLv= (double)pPlC->fMainRespRLeaf;
//
//	//////////////////////////////////////////////////////////////////////////
//
//	vPn = max(0.0, vPSgross/vDaylen - 0.25*(vMaintLv/0.75)/24.0);
//	vPn = vPn/min(2.5, vLAI);
//	Rlv = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(vPn+1E-9)/1.6-Rbv-Rcv;
//	
//	Rlv = (double)spass_LIMIT((double)Rlvm,(double)Rlvx,(double)Rlv);
//		
//	//______________________________________________________________________________
//   //Apparent Psychrometric constant (mbar/C)
//	Rbh = Rbv/0.93;
//	Rch = Rcv;
//	
//	vPsych1	= vPsych*(Rlv+Rbv+Rcv)/(Rbh+Rch);		//Psychrometric instrument constant (mbar/C)	 
//
//	//______________________________________________________________________________
//	//Saturated and actural vapour pressure of the air (mbar)
//	vSatVP = 0.1*6.11*exp(17.4*vTempDay/(vTempDay+239.0));
//	vSlope = 4158.6*10*vSatVP/((vTempDay+239.0)*(vTempDay+239.0));
//
//	vSatVPave = 0.1*6.11*exp(17.4*vTempAve/(vTempAve+239.0));
//	vActVP = vSatVPave*vRH/100.0;
//	
//	//______________________________________________________________________________
//	//Radiation Term 
//	vLongRadIn =vBoltzm*pow((vTempDay+273.0),4)*(0.618+0.00365*sqrt(10*vActVP));
//	vLongRadOut=vBoltzm*pow((vTempDay+273.0),4);
// 
// 	vNetRad = (1.0-vAlbedo)*vGlobalRadiation
//		-(vLongRadOut-vLongRadIn)*(vGlobalRadiation/(0.75*vRadMax*1E6))*(vDaylen/24.0);
//
//	vPotTrRad = vNetRad/vLH*(vSlope/(vSlope+vPsych1));
//	
//	//______________________________________________________________________________
//	//Drying Term
//	vDry = 10.0*(vSatVP-vActVP)*vVHair/(Rbh+Rch)*(vDaylen/24.0);
//	vPotTrDry=vDry/vLH/(vSlope+vPsych1)*(24.0*3600.0);
//	
//	vPotTr  = vPotTrRad*(1.0-exp(-0.5*vLAI)) + vPotTrDry*min(2.5,vLAI);
//
//	//pPl->pPltWater->fPotTranspDay = (double)vPotTr;
//	fPotTraDay = (double)vPotTr;
//	 
//
//
//
//	//?????????????????????????????????????????????????????????
//	//For setting the Expert_n Variable pPl->pPltWater->fPotTranspdt
//
//	//pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay*DeltaT;
//	
//	return (double)vPotTr;
//	}


/*******************************************************************************
** EOF */
