//      spass_photosynthesis.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_photosynthesis.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int spass_canopy_gross_photosynthesis(spass *self)
  {
    expertn_modul_base *xpn = &(self->parent);

	PCLIMATE		pCl = xpn->pCl; 
	PPLANT			pPl = xpn->pPl;
	PLOCATION		pLo = xpn->pLo; 
	PTIME			pTi = xpn->pTi;

	PRESPONSE	pResp;
	CARDTEMP	CardTmp;
	ORGANNC		OrgNc;
	double 		fPgmmax, fMaxLUE,fCO2Cmp,fCO2R;

    PWEATHER	pWth=pCl->pWeather; 
	PCANOPY 	pCan=pPl->pCanopy;
    PPLTCARBON	pCbn=pPl->pPltCarbon;

	char pType[5];	//SG/17/05/99: wg. Unterscheidung der Pflanzenarten

	//C_DEBUG(self->fPgMax);
	//C_DEBUG(self->fLUE);
	//C_DEBUG(self->vGrossPs);
	//C_DEBUG(self->vDayl);
	//C_DEBUG(self->cumphotos);
	//C_DEBUG(self->fPgmmaxP);
	//C_DEBUG(self->fCO2FuncP);
	//C_DEBUG(self->fTempFuncP);
	//C_DEBUG(self->fLfNcFuncP);
	//C_DEBUG(self->fActNcP);
	//C_DEBUG(self->fMinNcP);
	//C_DEBUG(self->fMaxOptNcP);
	//C_DEBUG(self->cumgrossphotos);
	//C_DEBUG(self->PARCum);

	if ((pPl->pDevelop->fStageSUCROS>(float)0.0)&&(pPl->pDevelop->iDayAftEmerg >0)&&(PLANT_IS_GROWING))
	{

	strcpy(pType,"C3\0");

	if ((strcmp(pPl->pGenotype->acCropCode,"MI")==0)
		||(strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
		||(strcmp(pPl->pGenotype->acCropCode,"SG")==0))

		strcpy(pType,"C4\0");

	//OrgNc.fMaxOptNc = pPl->pPltNitrogen->fLeafOptConc;
	//if(pPl->pPltNitrogen->fLeafOptConc == (float)0.0)	
	OrgNc.fMaxOptNc = (double) 0.067;

    OrgNc.fActNc	= pPl->pPltNitrogen->fLeafActConc;
    OrgNc.fMinNc 	= pPl->pPltNitrogen->fLeafMinConc;
		//SG 2005/12/14:
    OrgNc.fOptNc	= pPl->pPltNitrogen->fLeafOptConc;
	OrgNc.fMaxOptNc = pPl->pGenotype->OptLvNc[0].fOutput;

	//=======================================================================================================
	//Photosynthesis
	//=======================================================================================================
	CardTmp.fMinTemp = pPl->pGenotype->fTempMinPs;
	CardTmp.fOptTemp = pPl->pGenotype->fTempOptPs;
	CardTmp.fMaxTemp = pPl->pGenotype->fTempMaxPs;
	fPgmmax = pPl->pGenotype->fMaxGrossPs;
	fMaxLUE = pPl->pGenotype->fLightUseEff;
	fCO2Cmp = pPl->pGenotype->fCO2CmpPoint;
	fCO2R   = pPl->pGenotype->fCiCaRatio;
	pResp	= pPl->pGenotype->PmaxTemp;


	pCbn->fGrossPhotosynR = spass_DailyCanopyGrossPhotosythesis(pType, fPgmmax,fMaxLUE,
										fCO2Cmp,fCO2R,pResp,&CardTmp, &OrgNc,
										pCan->fLAI,pLo->pFarm->fLatitude, pTi->pSimTime->fTimeY,
										pTi->pSimTime->fTimeY_old, pTi->pSimTime->hour,
										pWth->fSolRad,pWth->fTempAir,pCl->pWeather->fAtmCO2ppm,self);

  }// end PLANT_IS_GROWING
	return RET_SUCCESS;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds DailyCanopyGrossPhotosythesis(LPSTR pType,float fPgmmax,float fMaxLUE,float fCO2Cmp,float fCO2R,
//											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, float fLAI,
//											  float fLatitude,int nDay,
//											  float fRad,float fTmpMax,float fTmpMin,float fCO2,PPLANT pPl)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the daily canopy photosynthesis rate under optimal water condition
//Inputs:	1. pType		- Pointer to a string containing the type of the plant (C3 or C4)
//			2. fPgmmax		- Maximum gross leaf photosynthesis rate at optimal temperature, N level, both 
//							  CO2 and light saturation (kgCO2/ha.hr)
//			3. fMaxLUE		- Maximum leaf light use efficiency at low light, low temperature and low CO2
//							  ((kgCO2/ha leaf.hr)/(W/m2))
//			4. fCO2Cmp		- Compensation point of CO2 for photosynthesis (vppm)
//			5. fCO2R		- Ratio of CO2 concentration in stomatal cavity to that in the air (-)
///			6. pResp		- Pointer to a RESPONSE structure containing temperature response data for photosynthesis 
//			7. pCardTemp	- pointer to a CARDTEMP structure containing the cardinal temperature for photosynthesis
//			8. pLfNc		- Pointer to a ORGANNC structure containing leaf nitrogen concentration
//			9. fLAI			- Effective leaf area index (-)
//		   10. fLatitude	- Location latitude (Degree)
//		   11. nDay			- Julain day (-)
//		   12. fRad			- Daily global radiation (MJ/d)
//		   13. fTmpMax		- Daily maximum air temperature(C)
//		   14. fTmpMin		- Daily minimum air temperature(C)
//		   15. fCO2			- Current CO2 concentration in the air (vppm)
//Outputs:	1. Return   	- Calculated daily gross photosynthesis rate of unit leaf area (kgCO2/ha.day)
//Functions Called:
//			LeafMaxGrossPhotosynthesis
//			LeafLightUseEfficiency
//			CanopyGrossPhotosynthesis 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the temperature response function for photosynthesis. If not, the cardinal temperatures
//			at pCardTemp will be used to construct the temperature response function. If pCardTemp equals NULL,
//			a minimum, optimum and maximum temperature of 0, 22 and 35C will be assumed respectively.
//			If pLfNc equals NULL, no nitrogen stress will be considered.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_DailyCanopyGrossPhotosythesis(char* pType,double fPgmmax,double fMaxLUE,double fCO2Cmp,double fCO2R,
											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, double fLAI,
											  double fLatitude,double Day,double mDay, int hour,
											  double fRad,double fTemp,double fCO2, spass *self)
	{
	expertn_modul_base *xpn = &(self->parent);

	PCLIMATE		pCl = xpn->pCl; 
	PPLANT			pPl = xpn->pPl;
	PTIME			pTi = xpn->pTi;

	double 	vGrossPs,vDailyGrossPs,PAR,PARDIF,PARDIR;//,vAveGrossPsMax;
	double 	vSinHeight,vDayl,vAtmTrans,vDifFr;//vLAI,vGlobalRadiation;
	double  vLatitude,vDec,vSin,vCos,vSolarConst,vHour;//vRadExt,vRsc,vDailySin,vDailySinE;
	double	fLUE,fPgMax;
	int    	nGauss=5;
	int		i;
	//static int		Gaussian = 0;
    //double 	xGauss[]={0.0469101, 0.2307534, 0.5000000, 0.7692465, 0.9530899};
	double 	wGauss[]={0.1184635, 0.2393144, 0.2844444, 0.2393144, 0.1184635};

	double RAD	= PI/180.0;

	//static double PARCum = 0.0;
	//double PARold;

	double vAveGrossPs;

	//printf("%f %f \n", self->fDayTest, pPl->pDevelop->iDayAftSow - pTi->pTimeStep->fAct);

	//if(self->fDayTest>=(double)pPl->pDevelop->iDayAftSow-pTi->pTimeStep->fAct) 
	//{
	//	printf("%f %f \n", self->fDayTest, PARCum);
	//	vAveGrossPs = 0.0;
	//	PARCum= 0.0;
	//	printf("%f %f \n", self->fDayTest, PARCum);	
	//}


	//vLAI				= (double)fLAI;
	//vGlobalRadiation	= (double)fRad*1E6; 	//J/m2.d
	vLatitude 			= (double)fLatitude;

	//===========================================================================================
	//Daylength, Solar constant and daily extraterrestrial radiation
	//===========================================================================================
	//Declination of the sun as function of Daynumber (vDay)
	// muss es nicht heißen ?:
	Day = (int)Day;
	vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*((Day+10.0)/365.0)));

	//vSin, vCos and vRsc are intermediate variables
	vSin = sin(RAD*vLatitude)*sin(vDec);
	vCos = cos(RAD*vLatitude)*cos(vDec);
	//vRsc = vSin/vCos;

	//Astronomical daylength (hr)
	vDayl=12.0*(1.0+2.0*asin(vSin/vCos)/PI);         
	self->vDayl = vDayl;

	//integral of Sine of solar height vDailySin and integral of vDailySin with 
	//correction for lower atmospheric transmission at low solar elevations (vDailySinE)
	//vDailySin  = 3600.0*(vDayl*vSin+24.0*vCos*sqrt(1.0-vRsc*vRsc)/PI);
	//vDailySinE = 3600.0*(vDayl*(vSin+0.4*(vSin*vSin+vCos*vCos*0.5))
	//			+12.0*vCos*(2.0+3.0*0.4*vSin)*sqrt(1.0-vRsc*vRsc)/PI);

	//Solar constant(vSolarConst) and daily extraterrestrial (vRadExt) 
	vSolarConst  = 1370.0*(1.0+0.033*cos(2.0*PI*Day/365.0));	//J/m2.s
	//vRadExt		 = vSolarConst*vDailySin*1E-6;					//MJ/m2.s
    
	//===========================================================================================
	//Daily photosynthesis
	//===========================================================================================
	//Assimilation set to zero and three different times of the Day (vHour)
    vAveGrossPs 	= 0;
    //vAveGrossPsMax 	= 0;     
    
   	//FH/12/06/27 now use frequently calculated temperature values, not daily values
	//Daytime temperature
    //fTemp = (double)0.71*fTmpMax+(double)0.29*fTmpMin;
      
	if(pTi->pTimeStep->fAct<=0.2)//Gaussian yes/no
	{
//	for (i=0;i<nGauss;i++)
//		{
		//At the specified vHour, radiation is computed and used to compute assimilation
		//vHour = 12.0+vDayl*0.5*xGauss[i];
		vHour = pTi->pSimTime->fTimeDay*24.- xpn->pTi->pSimTime->fTimeZone;
		//Sine of solar elevation
		vSinHeight  = max(0.0, vSin + vCos*cos(2.0*PI*(vHour+12.0)/24.0));
		self->vSinHeight = vSinHeight;
		//Diffuse light fraction (vDifFr) from atmospheric transmission (vAtmTrans)
 		//PARold   		= 0.5* *(self->global_radiation) *vSinHeight*(1.0+0.4*vSinHeight)*1.E6/vDailySinE;
		//PAR = pCl->pWeather->fPAR*1E6/(3600.*24.);
		//PAR = pCl->pWeather->fSolRad*1E6/(3600.*24.);
		//PAR = 0.5*pCl->pWeather->fSolRad*1E6/(3600.*24.);

		//PAR = 0.5*pCl->pWeather->fSolRad*1E6*vSinHeight/(vDailySinE);
		PAR = pCl->pWeather->fPAR * 1.0e6/(3600.0*24.0);

		self->PARCum += PAR*pTi->pTimeStep->fAct;

		//PARCum += PAR*pTi->pTimeStep->fAct;// * wGauss[i];
		//self->PARCum = PARCum;
		
		self->temp_par=PAR;
		self->temp_pard=0.5*vSolarConst*vSinHeight;

		if (vSinHeight > 0)
		{
		vAtmTrans  	= PAR/(0.5*vSolarConst*vSinHeight);
		vAtmTrans = max(min(vAtmTrans,1.0),0.0);		

		if (vAtmTrans<=0.22) 
			vDifFr = 1.0;
		else
			{
			if ((vAtmTrans>0.22)&&(vAtmTrans<=0.35)) 
				vDifFr = 1.0-6.4*(vAtmTrans-0.22)*(vAtmTrans-0.22); 
			else
				vDifFr = 1.47-1.66*vAtmTrans;
			}
			
		vDifFr = max (vDifFr, 0.15+0.85*(1.0-exp(-0.1/vSinHeight)));
		
		//Diffuse PAR (PARDIF) and direct PAR (PARDIR)
		//PARDIF = min(PAR, vSinHeight*vDifFr*vAtmTrans*0.5*vSolarConst);
		PARDIF = min(PAR, vDifFr*PAR);
		}
		else
		{
			PARDIF = PAR;
		}
		PARDIR = PAR-PARDIF;
		
		

		//SG 20110810:
		//CO2-conc. from input file (*.xnm) for AgMIP-project
		//fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);		

		//Light response parameters
		fPgMax  = spass_LeafMaxGrossPhotosynthesis(fPgmmax,fTemp,fCO2,fCO2Cmp,fCO2R,pResp,pCardTmp,pLfNc,pPl,self);

		fLUE	= spass_LeafLightUseEfficiency(fMaxLUE,fTemp,fCO2,pType);

		////Canopy gross photosynthesis
		vGrossPs= spass_CanopyGrossPhotosynthesis(self,fPgMax,fLUE,fLAI,fLatitude,Day,vHour,
			                                      (double)PARDIR,(double)PARDIF,pPl,pTi);

		self->fPgMax = fPgMax;
		self->fLUE = fLUE;
		self->vGrossPs = vGrossPs;
		//self->fAveGrossPs_zwischen[self->iAveGrossPs] = vGrossPs;
		//self->iAveGrossPs++;
		//if (self->iAveGrossPs==end) self->iAveGrossPs = 0;

		////Integration of assimilation rate to a daily total (vAveGrossPs) 
		////vAveGrossPs += vGrossPs*pTi->pTimeStep->fAct;//*wGauss[i];
		//for (i = 0; i<end;i++)
		//	vAveGrossPs += self->fAveGrossPs_zwischen[i]*pTi->pTimeStep->fAct;
    	
		vAveGrossPs = vGrossPs;

		//if ((vSinHeight<=0.0)) return 0.0;
		//vDailyGrossPs= vAveGrossPs*vDayl;
		//FH 24/01/13 to account for small timesteps and 0 photosynthesis during nighttime multiplication
		// with factor 24.0/Daylength
		vDailyGrossPs= vAveGrossPs*vDayl*(24/vDayl);
		self->cumgrossphotos += vGrossPs*pTi->pTimeStep->fAct;
		self->cumphotos += vDailyGrossPs*pTi->pTimeStep->fAct;
	}
	else //Gaussian yes/no
	{
		if (self->spass_DailyCanopyGrossPhotosythesisGaussian == 0)
		{
		printf("Gaussian yes \n");
		self->spass_DailyCanopyGrossPhotosythesisGaussian = 1;
		}
		for (i=0;i<nGauss;i++)
		{
		//At the specified vHour, radiation is computed and used to compute assimilation
		//vHour = 12.0+vDayl*0.5*xGauss[i];
		vHour = pTi->pSimTime->fTimeDay*24.;
		//Sine of solar elevation
		vSinHeight  = max(0.0, vSin + vCos*cos(2.0*PI*(vHour+12.0)/24.0));
		//FH 10/2012 If vSinHeight <0 -> no photosynthesis -> exit
		//if ((vSinHeight<=0.0)) return 0.0;
		//Diffuse light fraction (vDifFr) from atmospheric transmission (vAtmTrans)
 		//PARold   		= 0.5* *(self->global_radiation) *vSinHeight*(1.0+0.4*vSinHeight)*1.E6/vDailySinE;
		PAR = pCl->pWeather->fPAR*1E6/(3600.*24.);
		//if ((vSinHeight<=0.0)) return 0.0;
		if ((vSinHeight>0.0))
		{
		vAtmTrans  	= PAR/(0.5*vSolarConst*vSinHeight);
		
		self->spass_DailyCanopyGrossPhotosythesisPARCum += PAR * wGauss[i];
		if (vAtmTrans<=0.22) 
			vDifFr = 1.0;
		else
			{
			if ((vAtmTrans>0.22)&&(vAtmTrans<=0.35)) 
				vDifFr = 1.0-6.4*(vAtmTrans-0.22)*(vAtmTrans-0.22); 
			else
				vDifFr = 1.47-1.66*vAtmTrans;
			}
			
		vDifFr = max (vDifFr, 0.15+0.85*(1.0-exp(-0.1/vSinHeight)));
		
		//Diffuse PAR (PARDIF) and direct PAR (PARDIR)
		//PARDIF = min(PAR, vSinHeight*vDifFr*vAtmTrans*0.5*vSolarConst);
		PARDIF = min(PAR, vDifFr*PAR);
		}
		else
		{
		PARDIF = PAR;
		}

		PARDIR = PAR-PARDIF;


		//SG 20110810:
		//CO2-conc. from input file (*.xnm) for AgMIP-project
		//fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);		

		//Light response parameters
		fPgMax  = spass_LeafMaxGrossPhotosynthesis(fPgmmax,fTemp,fCO2,fCO2Cmp,fCO2R,pResp,pCardTmp,pLfNc,pPl,self);
		
		fLUE	= spass_LeafLightUseEfficiency(fMaxLUE,fTemp,fCO2,pType);

		////Canopy gross photosynthesis
		vGrossPs= spass_CanopyGrossPhotosynthesis(self,fPgMax,fLUE,fLAI,fLatitude,Day,vHour,
			                                      (double)PARDIR,(double)PARDIF,pPl,pTi);

		//Integration of assimilation rate to a daily total (vAveGrossPs) 
		vAveGrossPs += vGrossPs*wGauss[i];
    	}
		vDailyGrossPs= vAveGrossPs * vDayl;
	}  //end Gaussian yes/no


    return (double)vDailyGrossPs;
    }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double WINAPI _loadds LeafMaxGrossPhotosynthesis(double fPgmmax,double fTemp,double fCO2,double fCO2Cmp,double fCO2R,
// 											   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, PPLANT pPl)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the maximum gross photosynthesis of unit leaf area at current temperature
//			current CO2 concentration, current leaf N level and light saturation
//Inputs:	1. fPgmmax		- Maximum gross leaf photosynthesis rate at optimal temperature, N level, both 
//							  CO2 and light saturation (kgCO2/ha.hr)
//			2. fTemp		- Current air temperature (C) 	
//			3. fCO2			- Current CO2 concentration in the air (vppm)
//			4. fCO2Cmp		- Compensation point of CO2 for photosynthesis (vppm)
//			5. fCO2R		- Ratio of CO2 concentration in stomatal cavity to that in the air (-)
///			6. pResp		- Pointer to a RESPONSE structure containing temperature response data for photosynthesis 
//			7. pCardTemp	- pointer to a CARDTEMP structure containing the cardinal temperature for photosynthesis
//			8. pLfNc		- Pointer to a ORGANNC structure containing leaf nitrogen concentration
//Outputs:	1. Return   	- Calculated maximum gross photosynthesis rate of unit leaf area (kgCO2/ha.hr)
//Functions Called:
//			RelativeTemperatureResponse
//			AFGENERATOR 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the temperature response function for photosynthesis. If not, the cardinal temperatures
//			at pCardTemp will be used to construct the temperature response function. If pCardTemp equals NULL,
//			a minimum, optimum and maximum temperature of 0, 22 and 35C will be assumed respectively.  
//			If pLfNc equals NULL, no nitrogen stress will be considered.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_LeafMaxGrossPhotosynthesis(double fPgmmax,double fTemp,double fCO2,double fCO2Cmp,double fCO2R,
										   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, PPLANT pPl,spass *self)
	{     
    double	fMinTemp,fOptTemp,fMaxTemp,fTempFunc,fCO2I,fCO2I340,fCO2Func,fLfNcFunc,fPmaxGross;
	double	fNcFact; //SG/17/05/99: wg. Unterscheidung Weizen/Kartoffel
    
	//------------------------------------------------------------------------    	
	//Temperature response and Effect of daytime temperature
	if (pCardTmp==NULL)
		{
		fMinTemp = (double)0.0;
		fOptTemp = (double)22.0;
		fMaxTemp = (double)35.0;
		}
	else
    	{
		fMinTemp = pCardTmp->fMinTemp;
		fOptTemp = pCardTmp->fOptTemp;
		fMaxTemp = pCardTmp->fMaxTemp;
		}		

	if ((pResp==NULL)||(pResp[0].fInput==(double)-99.9))
		fTempFunc = spass_RelativeTemperatureResponse(fTemp,fMinTemp,fOptTemp,fMaxTemp);
	else
		fTempFunc = spass_AFGENERATOR(fTemp, pResp);

	//------------------------------------------------------------------------    	
	//Effect of CO2 concentration of the air
	fCO2I 	= fCO2*fCO2R;
	fCO2I340= fCO2R*(double)340.0;

	fCO2Func= min((double)2.3,(fCO2I-fCO2Cmp)/(fCO2I340-fCO2Cmp)); //For C3 crops

	//------------------------------------------------------------------------    	
    //Effect of leaf nitrogen content
	if (pLfNc==NULL)
		fLfNcFunc = (double)1.0;
	else

		//SG/19/04/99////////////////////////////////////////////////////////////
		//
		//	Anpassung an PLAGEN: Optimale N-Konz. statt maximaler; deshalb auch Faktor 0.9
		//	weggelassen!
		//fLfNcFunc = (pLfNc->fActNc-pLfNc->fMinNc)/((double)0.9*pLfNc->fMaxOptNc-pLfNc->fMinNc);
		fLfNcFunc = (double) min((double)1.0,max((double)0.0,
		             (pLfNc->fActNc-pLfNc->fMinNc)/((double)0.9*pLfNc->fMaxOptNc-pLfNc->fMinNc)));
		////////////////////////////////////////////////////////////////////////

	self->fActNcP = pLfNc->fActNc;
	self->fMinNcP = pLfNc->fMinNc;
	self->fMaxOptNcP = pLfNc->fMaxOptNc;

		//fLfNcFunc = (pLfNc->fActNc-pLfNc->fMinNc)/(pLfNc->fMaxOptNc-pLfNc->fMinNc);

	//SG 2005/12/14: 
	//laut Dokumentation (Diss E.Wang) und Ur-SPASS:
	//fLfNcFunc = (double)pow((pLfNc->fActNc-pLfNc->fMinNc)/(pLfNc->fOptNc-pLfNc->fMinNc),2);
	//fLfNcFunc = (double)min(1.0,max(0.0,fLfNcFunc));


	//SG/17/05/99:
	//
	//	Sonderbehandlung für Kartoffel (aus PLAGEN)!

	fNcFact = (double)0.0;
	
	if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
		fNcFact = (double)pow((double)fLfNcFunc,1.0/3.0);

	fLfNcFunc = min((double)1.0, max(fNcFact,fLfNcFunc));	
	//fLfNcFunc = min((double)1.0, max((double)0.0,fLfNcFunc));


	//SG/08/04/99///////////////////////////////////////////////////////////////////
	//
	// In PLAGEN fLfNcFunc immer 1.0, solange fStageSUCROS < 0.0015!!!
	//
	// Ausserdem Fehler in PLAGEN, der beweirkt, dass fStageSUCROS an dieser Stelle
	// immer gleich 0 ist. Deshalb:
	////////////////////////////////////////////////////////////////////////////////
	// fLfNcFunc = (double)1.0;	//SG/16/04/99: auskommentiert, um zu sehen, wie SPASS ohne diesen Fehler rechnet!

	//------------------------------------------------------------------------    	
    //Maximum leaf gross photosynthesis
	fPmaxGross= max((double)1.0, fPgmmax*(fCO2Func*fTempFunc*fLfNcFunc));
	 	 
	self->fPgmmaxP = fPgmmax;
	self->fCO2FuncP = fCO2Func;
	self->fTempFuncP = fTempFunc;
	self->fLfNcFuncP = fLfNcFunc;

	return fPmaxGross;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double WINAPI _loadds LeafLightUseEfficiency(double fMaxLUE, double fTemp, double fCO2, LPSTR pType)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the light use efficiency of the leaf at low light level and 
//			current CO2 concentration and temperature
//Inputs:	1. fMaxLUE		- Maximum leaf light use efficiency at low light, low temperature and low CO2
//							  ((kgCO2/ha leaf.hr)/(W/m2))
//			2. fTemp		- Current air temperature (C) 	
//			3. fCO2			- Current CO2 concentration in the air (vppm)
//			4. pType		- Pointer to a string containing the type of the plant (C3 or C4)
//Outputs:	1. Return   	- Calculated light use efficiency at low light ((kgCO2/ha leaf.hr)/(W/m2))
//Functions Called:
//			None
//Comments:	For C3 plants the light use efficiency will decreased if temperature becomes or CO2 concentration 
//			becomes low due to the increased photorespiration. So that the effect of photorespiration is 
//			simulated in this function
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_LeafLightUseEfficiency(double fMaxLUE, double fTemp, double fCO2, char* pType)
	{     
	double fCO2PhotoCmp0,fCO2PhotoCmp,fEffPAR;
	 
	//Check whether a C3 or C4 crop
	if ((strcmp(pType,"C3\0")==0)||(strcmp(pType,"c3\0")==0))   //C3 plants
		fCO2PhotoCmp0=(double)38.0; //vppm
	else
		fCO2PhotoCmp0=(double)0.0;
		
	//Efect of Temperature
	fCO2PhotoCmp = fCO2PhotoCmp0*(double)pow(2.0,((double)fTemp-20.0)/10.0);

	//Efect of CO2 concentration
	fEffPAR	= fMaxLUE*(fCO2-fCO2PhotoCmp)/(fCO2+2.0*fCO2PhotoCmp);
	
	return fEffPAR;
	}		

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds CanopyGrossPhotosynthesis(float fPgMax, float fLUE, float fLAI, 
//										  float fLatitude,int nDay,float fHour, float fPARdir,float fPARdif)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the canopy gross photosynthesis rate for a given crop with fPgMax,fLUE,fLAI
//			at latitude (fLatitude) on day (julian day fDay) at fHour
//Inputs:	1. fPgMax	- Maximum leaf gross photosynthesis rate at light saturation (kgCO2/ha.hr)
//			2. fLUE		- Light use efficiency of the leaf at current conditions ((kgCO2/ha leaf.hr)/(W/m2)) 	
//			3. fLAI		- Effective LAI (-)
//			4. fLatitude- Location latitude (Degree)
//			5. nDay		- Julain day (-)
//			6. fHour	- Current time (Hour)
//			7. fPARdir	- Direct component of incident photosynthetic active radiation (W/m2)
//			7. fPARdif	- Diffuse component of incident photosynthetic active radiation (W/m2)
//Outputs:	1. Return   - Calculated canopy photosynthesis rate (kgCO2/ha.hr)
//Functions Called:
//			None
//Comments:	The input variable fPgMax and fLUE should be calculated using the following functions:
//			fPgMax = LeafMaxGrossPhotosynthesis(...);
//			fLUE   = LeafLightUseEfficiency(...)
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double spass_CanopyGrossPhotosynthesis(spass *self,double fPgMax, double fLUE, double fLAI, double fLatitude,
										  double nDay,double fHour, double fPARdir,double fPARdif, PPLANT pPl, PTIME pTi)
	{

 //   expertn_modul_base *xpn = &(self->parent);

	int		i,j;
	double  PMAX,EFF,vLAI,PARDIR,PARDIF,SINB;
	double 	SQV,REFH,REFS,CLUSTF,KDIRBL,KDIRT,FGROS,LAIC,VISDF,VIST,VISD,VISSHD,FGRSH;
	double 	FGRSUN,VISSUN,VISPP,FGRS,FSLLA,FGL,LAT,DAY,HOUR,DEC,vSin,vCos;
	double dummy_vis;	
	int    nGauss=5;
    double xGauss[]={0.0469101, 0.2307534, 0.5000000, 0.7692465, 0.9530899};
	double wGauss[]={0.1184635, 0.2393144, 0.2844444, 0.2393144, 0.1184635};
		   
	double RAD	= PI/180.0;

	double SCV	=	0.20; 	//Scattering coefficient of leaves for visible radiation (PAR)
	//double k	= 	0.50;	//The average extinction coefficient for visible and near infrared radiation
	double KDIF = 	0.60;	//Extinction coefficient for diffuse light 


  	//if WheatAndBarley  KDIF =0.6;
  	//if Potato			 KDIF =1.0;
  	//if SugarBeet		 KDIF =0.69;   WAVE, p5-12
    
    //PPLANT pPl=GetPlantPoi();

	if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)	KDIF=1.0;
    if (strcmp(pPl->pGenotype->acCropCode,"SB")==0)	KDIF=0.69;
    if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0)	KDIF=0.65;
							   
	PMAX   = (double)fPgMax;
	EFF    = (double)fLUE;
	vLAI   = (double)fLAI;
	PARDIR = (double)fPARdir;
	PARDIF = (double)fPARdif;
	LAT	   = (double)fLatitude;
	HOUR   = (double)fHour;
    DAY    = (double)nDay;

	// FH 30.08.12  if-Abfrage ob Strahlung überhaupt vorhanden

	FGROS = 0.0;

	//if ((PARDIR <= 0)&&(PARDIF <= 0)) return FGROS;

	//===========================================================================================
	//Sine of the solar height
	//===========================================================================================
	//Declination of the sun as function of Daynumber (vDay)
	DEC = -asin( sin(23.45*RAD)*cos(2.0*PI*(DAY+10.0)/365.0));
	
	//vSin, vCos and vRsc are intermediate variables
	vSin = sin(RAD*LAT)*sin(DEC);
	vCos = cos(RAD*LAT)*cos(DEC);

	//FH 30.08.12 (HOUR+12) -> (HOUR -12) wie in Dokumentation;

	SINB  = max(0.0000001,vSin + vCos*cos(2.0*PI*(HOUR-12.0)/24.0));


	//===========================================================================================
	//Reflection of horizontal and spherical leaf angle distribution
	SQV  = sqrt(1.0-SCV);
	REFH = (1.0-SQV)/(1.0+SQV);
	REFS = REFH*2.0/(1.0+2.0*SINB);

	//Extinction coefficient for direct radiation and total direct flux
	CLUSTF = KDIF / (0.8*SQV); 
	KDIRBL = (0.5/SINB) * CLUSTF;
	KDIRT  = KDIRBL * SQV;
	
	
	//===========================================================================================
	//Selection of depth of canopy, canopy assimilation is set to zero
    FGROS = 0;
	dummy_vis = 0.0;
	for (i=0;i<nGauss;i++)
		{
		LAIC = vLAI * xGauss[i];
		
		//Absorbed fluxes per unit leaf area: diffuse flux, total direct
		//flux, direct component of direct flux.
		VISDF = (1.0-REFH)*PARDIF*KDIF  *exp (-KDIF  *LAIC);
		VIST  = (1.0-REFS)*PARDIR*KDIRT *exp (-KDIRT *LAIC);
		VISD  = (1.0-SCV) *PARDIR*KDIRBL*exp (-KDIRBL*LAIC);

		//Absorbed flux (J/M2 leaf/s) for shaded leaves and assimilation of shaded leaves
		VISSHD = VISDF + VIST - VISD;

		 if (PMAX>0.0) 
		 	FGRSH = PMAX * (1.0-exp(-VISSHD*EFF/PMAX));
		 else
		    FGRSH = 0.0;
		    
		//Direct flux absorbed by leaves perpendicular on direct beam and
		//assimilation of sunlit leaf area
         VISPP  = (1.0-SCV) * PARDIR / SINB;

         FGRSUN = 0.0;
         for (j=0;j<nGauss;j++)
         	{
         	VISSUN = VISSHD + VISPP * xGauss[j];
			dummy_vis += VISSUN * wGauss[i];
			
			//self->intercepted_par += (2.*VISSHD+VISPP * wGauss[j] )*pTi->pTimeStep->fAct/1E6;

            if (PMAX>0.0) 
            	FGRS = PMAX * (1.0-exp(-VISSUN*EFF/PMAX));
            else
               	FGRS = 0.0;
               	
			FGRSUN = FGRSUN + FGRS * wGauss[j];
			
            }



		//Fraction sunlit leaf area (FSLLA) and local assimilation rate (FGL)
         FSLLA = CLUSTF * exp(-KDIRBL*LAIC);
         FGL   = FSLLA  * FGRSUN + (1.0-FSLLA) * FGRSH;

		//Integration of local assimilation rate to canopy assimilation (FGROS)
		 FGROS = FGROS + FGL * wGauss[i];
		 
		
         }
		
	// MJ / ha
	//if (1-FGROS/PMAX>0)
	//	self->_total_intercepted_par -= log(1-FGRS/PMAX)*PMAX/EFF * pTi->pTimeStep->fAct*(24.*3600./1.0E6*1.0E4);
	//if ((1.0-FGRSUN/PMAX)>0)
	//self->_total_intercepted_par -= log(1.0-FGRSUN/PMAX)*PMAX/EFF * pTi->pTimeStep->fAct*(24.*3600./1.0E6*1.0E4);
	//self->_total_intercepted_par +=  dummy_vis * pTi->pTimeStep->fAct*(24.*3600./1.0E6*1.0E4);
	//(&(self->parent))->pCl->pWeather->fPAR*pTi->pTimeStep->fAct*(24.*3600./1.0E6*1.0E4);
              
	FGROS = FGROS * vLAI;
	
	if (1.0-FGROS/PMAX>0)
		self->_total_intercepted_par -= log(1.0-FGROS/PMAX)*PMAX/EFF * pTi->pTimeStep->fAct*(24.*3600./1.0E6*1.0E4);
	

	return (double)FGROS;
	}


/*******************************************************************************
** EOF */
