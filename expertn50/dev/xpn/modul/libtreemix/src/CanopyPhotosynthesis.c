#include "libtreemix.h"
#include <math.h>
#include <string.h>

int libtreemix_Photosynthesis(libtreemix *self)
{
	/* Description */
	/*
	 * Calculation of the Canopy Photosynthesis
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	
	/* Variables */
	int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	//int CanopyProduction_Analytical();
	//int CanopyProduction_Numerical();
	int libtreemix_CanopyProductionNumericalAdvanced(libtreemix *self);
	/****************************************************************************************************************/

	/* 
	 * Original Bossel TD3 photoproduction is calculated
	 * only if
	 *		- bossel-leaf photo	model (1)		(section 30001)
	 *		- interception = Monsi-Saeki (1)	(section 30002)
	 *		- simple average day temp. (1)		(section 30007)
	 * of original Bossel TD3 is used.
	*/
	if((self->conf.Photosynthesis==1)&&(self->conf.Light_Interception==1)&&(self->conf.Weather_Input==1))
	{
		if(self->clim.Polar==0){
			//CanopyProduction_Analytical();
		}
		else{
			/* iJulianDay is within the Photo Period */
			if((self->clim.DayFirstSun < xpn->pTi->pSimTime->fTimeY)&&(xpn->pTi->pSimTime->fTimeY < self->clim.DayLastSun)){
				//CanopyProduction_Numerical();
			}
			else{
				for(i=0; i<self->conf.Species_Count; i++)
				{
					self->plant[i].Ipar = 0.0;
					self->plant[i].PhCanopyPrd = 0.0;
				}
			}
		}
	}
	/* Here, new routine, if (1) no polar region or (2) FirstSun < fTimeY < LastSun => no polar region */
	else if((self->clim.Polar==0)||((self->clim.DayFirstSun < xpn->pTi->pSimTime->fTimeY)&&(xpn->pTi->pSimTime->fTimeY < self->clim.DayLastSun)))
	{
		libtreemix_CanopyProductionNumericalAdvanced(self);
	}
	else
	{
		for(i=0; i<self->conf.Species_Count; i++)
		{
			self->plant[i].Ipar = 0.0;
			self->plant[i].PhCanopyPrd = 0.0;
		}
	}
	
	/***DAILY VARIABLES********************************************************************/	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(NewDay(xpn->pTi))
		{
			self->plant[i].Transpiration_Day = 0.0;
		}
		
		self->plant[i].Transpiration_Day += (self->plant[i].Transpiration*dt);
	}
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
};

int libtreemix_CanopyProductionNumericalAdvanced(libtreemix *self)
{
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PPLTCARBON pC = xpn->pPl->pPltCarbon;	
	PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	int i; 		// species loop
	int s; 		// canopy layer loop
	int LfLayer = 0;	// leaf layer loop
	
	double dt = xpn->pTi->pTimeStep->fAct;				// act. timestep [d]
	double hour = xpn->pTi->pSimTime->fTimeDay*24.0;	// act. hour of day
	
	double CanopyDepth = 0.0;
	
	double LfPhotosynthesisClear = 0.0;
	double LfPhotosynthesisCloud = 0.0;
	
	double LfLayPhotosynthesisdt 	= 0.0;
	double LfLayGPPdt 				= 0.0;
	double LfLayTranspirationdt		= 0.0;
	double LfLayStomConddt 			= 0.0;
	double LfLayMaintRespdt			= 0.0;
	
	double LfTranspirationClear = 0.0;
	double LfTranspirationCloud = 0.0;
	
	double LfConductanceClear = 0.0;
	double LfConductanceCloud = 0.0;
	
	double LfGPPCanopyLayer 			= 0.0;
	double LfPhotosynthesisCanopyLayer	= 0.0;
	double LfTranspirationCanopyLayer	= 0.0;
	double LfConductanceCanopyLayer		= 0.0;
	double LfMaintRespLayer 			= 0.0;
	
	double CanopyGPP			= 0.0;
	double CanopyPhotosynthesis = 0.0;
	double CanopyTranspiration	= 0.0;
	double CanopyConductance	= 0.0;
	double CanopyMaintResp		= 0.0;
	
	/* Functions */
	int libtreemix_FrostHardiness(libtreemix *self, int i);
	
	int libtreemix_Radiation(libtreemix *self, double hour);
	
	double libtreemix_PhotosynthesisBossel(libtreemix *self, int i, double radiation, double fTempAir, double fAtmCO2ppm);
	int libtreemix_LeafPhotosynthesisFALGE(libtreemix *self, int i, double radiation);
	int libtreemix_LeafPhotosynthesisFALGE_simple(libtreemix *self, int i, double radiation, int LfLayer);
	
	//extern int set_climate(libtreemix *self);
	/****************************************************************************************************************/	
	
	/* choose between the radiation input */
	if(self->conf.PhotoRad == 1)		// using calculated radiation for given location and time, diminished by cloudfraction and parfactor
	{
		// PAR [W/m2]
		libtreemix_Radiation(self, hour);
		
		self->clim.IAboveCanopyClear = self->clim.Iclear;
		self->clim.IAboveCanopyCloud = self->clim.Icloud;
	}
	else if(self->conf.PhotoRad == 2)	// using XN5's internally computed radiation fPAR!
	{
		// PAR [MJ/m2*d]
		self->clim.IAboveCanopyClear = pWe->fPAR;		
		self->clim.IAboveCanopyCloud = pWe->fPAR;
		
		/*if(pWe->fPAR > 0.0)
		{
			char S[128];
			sprintf(S,"%f \n", self->clim.IAboveCanopyClear);
			PRINT_MESSAGE(xpn,3,S);
		}
		 */
		
	}
	else if(self->conf.PhotoRad == 3)
	{
		double sinArgOfSine = sin(M_PI * (hour-12.0+self->clim.DayLength*0.5)/self->clim.DayLength);
		
		self->clim.SeasTime = (xpn->pTi->pSimTime->fTimeY+10.0)/365.25;
		
		if(!self->clim.Polar)
		{	
			double SinFun = sin(2.0*M_PI*self->clim.SeasTime-(M_PI/2.0));	// part of eq. 49, 54

			self->clim.DayLength = 12.0+(self->clim.hAmp/2.0)*SinFun;	// eq. 49
			self->clim.NightFrac = 1.0-(self->clim.DayLength/24.0);
			
			self->clim.Iclear = self->clim.IAvgClear + (self->clim.IAmpClear/2.0)*SinFun;	// eq. 54
			self->clim.Icloud = self->clim.IAvgCloud + (self->clim.IAmpCloud/2.0)*SinFun;	// eq. 54
			
			self->clim.Cloud = 1.25*(1.0-self->clim.PARfactor*pWe->fSolRad*277.78/self->clim.Iclear);
		}
		
		// photoactive solar radiation, eq. 60, [W/m2]
		self->clim.IAboveCanopyClear = (2.0*self->clim.Iclear/self->clim.DayLength)*sinArgOfSine*sinArgOfSine;
		if(self->clim.IAboveCanopyClear < 0.0){
			self->clim.IAboveCanopyClear = 0.0;
		}
		self->clim.IAboveCanopyCloud = self->clim.CloudFraction*self->clim.IAboveCanopyClear;
	}
	
	/*
	 * actualize CO2-concentration using a exponential function
	 * ========================================================
	 */
	self->clim.YrAct = xpn->pTi->pSimTime->iyear;
	self->clim.CO2_air = pWe->fAtmCO2ppm;
	
	if(self->clim.YrAct > 2100)
	{
		self->clim.CO2_air = 391.0;
	}	
	else if(self->clim.YrAct >= self->clim.CO2Yr)
	{
		if(self->clim.YrAct > self->clim.YrOld)
		{
			// regression function according to the data from:
			// http://co2now.org/Current-CO2/CO2-Now/noaa-mauna-loa-co2-data.html
			// correlation coefficient is 0.984 with factor 0.00423
			// to simulate a CO2 rise from 310 to approx. 550 ppm (2100), factor 0.00423
			// to simulate a CO2 rise from 310 to 700 ppm (2100), factor 0.00574
			
			pWe->fAtmCO2ppm = 310.0 * exp(self->clim.CO2_incr_fac * (xpn->pTi->pSimTime->iyear-1958));
			self->clim.CO2_air = pWe->fAtmCO2ppm;
		}
	}
	
	self->clim.YrOld = xpn->pTi->pSimTime->iyear;
	
	/*============================================================================================================*/
	for(i=0; i<self->conf.Species_Count; i++)
	{		
		// Calculate Frost Hardiness
		libtreemix_FrostHardiness(self, i);
		
		// Simple Model of Leaf Area Density => 2*(LAI+1), Filling ratio is calc. for all canopy layers the same
		//self->plant[i].NumLfLay	= (int)((self->plant[i].DPhotoLayers+1)*((int)self->plant[i].LAI)+1);
		self->plant[i].NumLfLay = (int)20.0;
		
		for(s=0; s < self->plant[i].NumLfLay; s++)
		{
			self->plant[i].LfFillFrac[s] = (self->plant[i].CrArPot * self->plant[i].LAI)/self->plant[i].NumLfLay;
		}
		
		self->plant[i].SigmaC = 0.0;
		
		/* 
		 * start: calc the momentaneous leaf photosynthesis of canopy 
		 * -1: to calc. also the potential leaf photo of an above canopy leaf (SigmaC)
		 */
		for(LfLayer=-1; LfLayer <= self->plant[i].NumLfLay; LfLayer++)
		{
			// calc. the canopy depth from NumLfLay
			CanopyDepth = self->plant[i].LAI * LfLayer / self->plant[i].NumLfLay;			
			
			/* eq. 55, radiation within canopy [W/m²] */
			// calc. the interception of light, and remaining light:
			if((self->conf.PhotoRad == 1)||(self->conf.PhotoRad == 3))	// using calculated radiation for given location and time, diminished by cloudfraction and parfactor
			{
				// [W/m2]
				if(CanopyDepth >= 0.0)
				{
					self->clim.IPARCanopyClear = self->clim.IAboveCanopyClear*self->plant[i].LightExt*exp(-self->plant[i].LightExt*CanopyDepth);	
					self->clim.IPARCanopyCloud = self->clim.CloudFraction*self->clim.IPARCanopyClear;
					self->clim.SunltFrac = 1.0 - self->clim.CloudyDaysFrac;
				}
				else
				{
					self->clim.IPARCanopyClear = self->clim.IAboveCanopyClear;
					self->clim.IPARCanopyCloud = self->clim.CloudFraction*self->clim.IPARCanopyClear;
					self->clim.SunltFrac = 1.0 - self->clim.CloudyDaysFrac;
				}
                //FH 20190516 Test fuer Transpiration
                self->clim.IPARCanopy = self->clim.IPARCanopyClear + self->clim.IPARCanopyCloud;
			}
			else if(self->conf.PhotoRad == 2)	// using XN5's internally computed radiation: fPAR = 0.5*fSolRad!
			{
				// [MJ/m2*d]
				if(CanopyDepth >= 0.0)
				{
					//self->clim.IPARCanopy		= pWe->fPAR*self->plant[i].LightExt*exp(-self->plant[i].LightExt*CanopyDepth);
					self->clim.IPARCanopy		= pWe->fPAR*exp(-self->plant[i].LightExt*CanopyDepth);
					
					self->clim.IPARCanopyClear 	= self->clim.IAboveCanopyClear*self->plant[i].LightExt*exp(-self->plant[i].LightExt*CanopyDepth);	
					self->clim.IPARCanopyCloud 	= self->clim.IPARCanopyClear;
					self->clim.SunltFrac 		= 1.0 - self->clim.CloudyDaysFrac;
				}
				else
				{
					self->clim.IPARCanopy		= pWe->fPAR;
					
					self->clim.IPARCanopyClear = self->clim.IAboveCanopyClear;
					self->clim.IPARCanopyCloud = self->clim.IPARCanopyClear;
					self->clim.SunltFrac = 1.0 - self->clim.CloudyDaysFrac;
				}
			}
			
			/* CONVERTING THE RADIATION */
			// for unit conversion see:
			// Möller D. 2003: Luft. De Gruyter.
			// Larcher W. 2003: Physiological plant ecology. 4th Edition. Springer.
			if(self->conf.Photosynthesis == 1)
			{
				if(self->conf.PhotoRad == 2)
				{
					// using fPAR [MJ/m2*d] and converting it to [W/m2] (*1000000/86400)
					self->clim.IPARCanopyClear = self->clim.IPARCanopyClear*11.574;
					self->clim.IPARCanopyCloud = self->clim.IPARCanopyCloud*11.574;
				}
			}
			else if((self->conf.Photosynthesis == 2)||(self->conf.Photosynthesis == 3))
			{
				if((self->conf.PhotoRad == 1)||(self->conf.PhotoRad == 3))
				{
					// using radiation from calculation accoding to Bossel, 1994
					// factor 4.4: 1[W/m2] = 1[J/m2*s] = 4.2...4.6 [µmol/m2*s];
					// 4.2 for daylight (diffuse)
					// 4.6 for daylight (sunny)
					self->clim.IPARCanopyClear = self->clim.IPARCanopyClear*4.4;
					self->clim.IPARCanopyCloud = self->clim.IPARCanopyCloud*4.4;
				}
				else if(self->conf.PhotoRad == 2) 
				{
					// using fPAR [MJ/m2*d] and converting it to [µmol/m2*s] using the mean value of 1 [W/m2] = 4.4 [µmol/m2*s]
					// factor 50.9: 1[J/m2*s] = 86400[J/m2*d] = 0.0864[MJ/m2*d] = 4.4[µmol/m2*s] => Dreisatz: 1[MJ/m2*d] = 50.9[µmol/m2*s]
					self->clim.IPARCanopyClear = self->clim.IPARCanopyClear*50.9;
					self->clim.IPARCanopyCloud = self->clim.IPARCanopyCloud*50.9;
					
					self->clim.IPARCanopy *= 50.9; 
					
				}				
			}
			
			//===============================================================================================================================
			// calc. the leaf photosynthesis and transpiration etc. (for cloudy and clear sky)
			if(self->conf.Photosynthesis == 1)
			{			
				// leaf photosynthesis rate, eq. 56, [mgCO2/dm²*h]
				LfPhotosynthesisClear = libtreemix_PhotosynthesisBossel(self, i, self->clim.IPARCanopyClear, pWe->fTempAir, pWe->fAtmCO2ppm);
				LfPhotosynthesisCloud = libtreemix_PhotosynthesisBossel(self, i, self->clim.IPARCanopyCloud, pWe->fTempAir, pWe->fAtmCO2ppm);
				
				LfLayPhotosynthesisdt = libtreemix_PhotosynthesisBossel(self, i, self->clim.IPARCanopyCloud, pWe->fTempAir, pWe->fAtmCO2ppm);
			}
			else if(self->conf.Photosynthesis == 2)
			{	
				//leaf photosynthesis rate, [µmolCO2/m2(proj)*s]
				if(hour > (12.0-(self->clim.DayLength/2.0)))
				{
					if(hour < (12.0+(self->clim.DayLength/2.0)))
					{						
						libtreemix_LeafPhotosynthesisFALGE(self, i, self->clim.IPARCanopyClear);
						LfPhotosynthesisClear	= self->plant[i].NetPhotoMol;
						LfTranspirationClear	= self->plant[i].Transpiration;
						LfConductanceClear		= self->plant[i].StomCond;
						
						libtreemix_LeafPhotosynthesisFALGE(self, i, self->clim.IPARCanopyCloud);
						LfPhotosynthesisCloud	= self->plant[i].NetPhotoMol;
						LfTranspirationCloud	= self->plant[i].Transpiration;
						LfConductanceCloud		= self->plant[i].StomCond;
					}
					else
					{
						LfPhotosynthesisClear	= 0.0;
						LfTranspirationClear	= 0.0;
						LfConductanceClear		= 0.0;
						
						LfPhotosynthesisCloud	= 0.0;
						LfTranspirationCloud	= 0.0;
						LfConductanceCloud		= 0.0;					
					}
                    //FH 04062019 Transpiration test
                    LfLayTranspirationdt = ( (1.0 - self->clim.CloudFraction )* LfTranspirationClear + self->clim.CloudFraction * LfTranspirationCloud)/24.0;
				}
				else
				{	
					LfPhotosynthesisClear	= 0.0;
					LfTranspirationClear	= 0.0;
					LfConductanceClear		= 0.0;
					
					LfPhotosynthesisCloud	= 0.0;
					LfTranspirationCloud	= 0.0;
					LfConductanceCloud		= 0.0;
				}
			}
			else if(self->conf.Photosynthesis == 3)
			{
				/*
				//leaf photosynthesis rate, [µmolCO2/m2(proj)*s]
				if(hour > (12.0-(self->clim.DayLength/2.0)))
				{
					if(hour < (12.0+(self->clim.DayLength/2.0)))
					{
						libtreemix_LeafPhotosynthesisFALGE_simple(self, i, self->clim.IPARCanopyClear);
						LfPhotosynthesisClear	= self->plant[i].NetPhotoMol;
						LfTranspirationClear	= self->plant[i].Transpiration_LayerR; // [mm/d]
						LfConductanceClear		= self->plant[i].StomCond;
						
						libtreemix_LeafPhotosynthesisFALGE_simple(self, i, self->clim.IPARCanopyCloud);
						LfPhotosynthesisCloud	= self->plant[i].NetPhotoMol;
						LfTranspirationCloud	= self->plant[i].Transpiration_LayerR; // [mm/d]
						LfConductanceCloud		= self->plant[i].StomCond;
					}
					else
					{
						LfPhotosynthesisClear	= 0.0;
						LfTranspirationClear	= 0.0;
						LfConductanceClear		= 0.0;
						
						LfPhotosynthesisCloud	= 0.0;
						LfTranspirationCloud	= 0.0;
						LfConductanceCloud		= 0.0;					
					}
				}
				else
				{
					LfPhotosynthesisClear	= 0.0;
					LfTranspirationClear	= 0.0;
					LfConductanceClear		= 0.0;
					
					LfPhotosynthesisCloud	= 0.0;
					LfTranspirationCloud	= 0.0;
					LfConductanceCloud		= 0.0;
				}
				 */
				 
				// calculation without cloud/clear conditions
                //printf("%f %f\n", pWe->fPAR, self->clim.IPARCanopy);
				libtreemix_LeafPhotosynthesisFALGE_simple(self, i, self->clim.IPARCanopy, LfLayer);
				
                // FH 04062019 Transpirationstest
				//LfLayTranspirationdt	= self->plant[i].Transpiration_Layerdt; 			// [mm]
				LfLayTranspirationdt	= self->plant[i].Transpiration_LayerR/24.0; 			// [mm]
				
                LfLayPhotosynthesisdt	= self->plant[i].NetPhotoMolLay;					// [µmol/m2/s]
				LfLayGPPdt				= self->plant[i].GPP_Layer;							// [µmol/m2/s]
				LfLayMaintRespdt 		= self->plant[i].MainRespLfLay_R;					// [µmol/m2/s]
			}
			
			/* Sum up the momental photosynthesis, transpiration and conductance */
			if(LfLayer > -1)
			{
				//LfPhotosynthesisCanopyLayer += (self->plant[i].LfFillFrac[LfLayer]*(((1.0-self->clim.SunltFrac)*LfPhotosynthesisCloud)+(self->clim.SunltFrac*LfPhotosynthesisClear)));
				//LfTranspirationCanopyLayer	+= (self->plant[i].LfFillFrac[LfLayer]*(((1.0-self->clim.SunltFrac)*LfTranspirationCloud)+(self->clim.SunltFrac*LfTranspirationClear)));
				LfConductanceCanopyLayer	+= (self->plant[i].LfFillFrac[LfLayer]*(((1.0-self->clim.SunltFrac)*LfConductanceCloud)+(self->clim.SunltFrac*LfConductanceClear)));
				
				// 
				LfPhotosynthesisCanopyLayer += (self->plant[i].LfFillFrac[LfLayer]*LfLayPhotosynthesisdt);	// [µmol/m2/s]
				LfTranspirationCanopyLayer  += (self->plant[i].LfFillFrac[LfLayer]*LfLayTranspirationdt);
				LfGPPCanopyLayer			+= (self->plant[i].LfFillFrac[LfLayer]*LfLayGPPdt);				// [µmol/m2/s]
				LfMaintRespLayer			+= (self->plant[i].LfFillFrac[LfLayer]*LfLayMaintRespdt);		// [µmol/m2/s]
				
			}
			else
			{
				double radiation = 0.0;
				
				radiation = 4.4 * self->clim.PARfactor * pWe->fSolRad * 277.78 * (M_PI/(2.0*self->clim.DayLength))* sin( M_PI*(hour-12.0+self->clim.DayLength*0.5)/self->clim.DayLength);
				
				libtreemix_LeafPhotosynthesisFALGE(self, i, radiation);
				
				LfPhotosynthesisClear	= self->plant[i].NetPhotoMol;
				LfTranspirationClear	= self->plant[i].Transpiration;
				LfConductanceClear		= self->plant[i].StomCond;
				
				self->plant[i].SigmaC	+= LfPhotosynthesisClear;
				
				LfPhotosynthesisCanopyLayer += 0.0;
				LfTranspirationCanopyLayer  += 0.0;
			}
		}
		CanopyGPP				= LfGPPCanopyLayer;				// [µmol/m2/s]
 		CanopyPhotosynthesis 	= LfPhotosynthesisCanopyLayer;	// [µmol/m2/s]
		CanopyTranspiration		= LfTranspirationCanopyLayer;	// [mm]
		CanopyConductance		= LfConductanceCanopyLayer;
		CanopyMaintResp			= LfMaintRespLayer;
		
		/*------------------------------------------------------------------------------*/
		
		/* [µmol/m2/s] */
		self->plant[i].NetPhotoMolCanopy 		= CanopyPhotosynthesis;
		self->plant[i].NetPhotoMolCanopy_Day 	+= self->plant[i].NetPhotoMolCanopy;
		self->plant[i].NetPhotoMolCanopy_Sum 	+= self->plant[i].NetPhotoMolCanopy;
		
		self->plant[i].GPP_Canopy				= CanopyGPP;
		self->plant[i].GPP_Canopy_Day			+= self->plant[i].GPP_Canopy;
		self->plant[i].GPP_Canopy_Sum			+= self->plant[i].GPP_Canopy;
		
		/*------------------------------------------------------------------------------*/
		/* CONVERTING UNITS */
		
		{
			// calc. SigmaC of Day
			self->plant[i].SigmaC *= 0.1584*99.5*self->plant[i].DPhotoTime;	// [gC/m2*yr]
			self->plant[i].SigmaC /= self->plant[i].CLfDens;				// [gC/g C Lf*yr]
			self->plant[i].SigmaC *= 0.45;									// [gC/g DM Lf*yr]
		}
		
		if(self->conf.Photosynthesis == 1)
		{
			/* Net Photosynthesis Production (Pgross, Falge model) of the Canopy 
			 * !(including Leaf Dark Respiration, Rd)!
			 * 
			 * for the Bossel model, Rd is included in Photosynthesis at Day time, 
			 * but has to be subtracted at night time (in BiomassGrowth.c)
			 */ 
			/* ------------------------------------------------------------------------------------------------------ */
			
			if(pWe->fPAR < 0.01)
			{			
				self->plant[i].kTRespNight = pow((pWe->fTempAir - self->plant[i].TRespZero)/(self->plant[i].TRespRef-self->plant[i].TRespZero), 2);	// eq. 36
				
				if((pWe->fTempAir < self->plant[i].TRespZero)||(pWe->fTempAir > 40.0))
				{
					self->plant[i].kTRespNight = 0.0;
				}
			}
			else
			{
				self->plant[i].kTRespNight = 0.0;
			}
			
			
			// Dark leaf respiration rate for the biomass growth submodel
			self->plant[i].MainRespLf = self->plant[i].kTRespNight*self->plant[i].RespLf*(self->plant[i].CLfMass/1000.0);	// [tC/ha*yr]
			
			
			// factor 2.389: [mgCO2/dm²/h] -> [tC/ha*yr]
			self->plant[i].PhCanopyPrd = (2.389*CanopyPhotosynthesis) - self->plant[i].MainRespLf;	// [tC/ha*yr]
			
		}		
		else if((self->conf.Photosynthesis == 2)||(self->conf.Photosynthesis == 3))
		{
			/* Net Photosynthesis Production (Pgross, Falge model) of the Canopy 
			 * !(including Leaf Dark Respiration, Rd)! */
			/* ------------------------------------------------------------------------------------------------------ */
			
			/* Conversion of units */
			// factor 0.1584: 	[µmolCO2/m2/s]		->	[gCO2/m2*h]	(1mol CO2 == 44g CO2)
			// factor 0.995:	1[gCO2/m2(proj)/d]	->	0.995[tC/ha*yr]			
			//self->plant[i].PhCanopyPrd = 0.1584*0.995*self->plant[i].DPhotoTime*CanopyPhotosynthesis;	// [tC/ha*yr]
			//self->plant[i].PhCanopyPrd = 0.315576*self->plant[i].NetPhotoMolCanopy;					// [tC/ha*yr]
			
			
			// factor 3.786912: 1[µmolCO2/m2*s]	== 3.786912[tC/ha*yr]
			// 1[µmolCO2/m2*s] = 44 [µgCO2/m2*s]*10000 = 0.44 [gCO2/ha*s]*86400 = 38016.0 [gCO2/ha*d]*365.25 = 13885344 [gCO2/ha*yr]/1000000 = 13.88 [tCO2/ha*yr]
			// 13.88 [tCO2/ha*yr]*(12/44) = 3.786912 [tC/ha*yr], (12/44) is the ratio of the molare mass of C to CO2 = (12 + 16 + 16)
			self->plant[i].PhCanopyPrd 			= self->plant[i].NetPhotoMolCanopy*3.786912;	// [µmol/m2/s] -> [tC/ha*yr]
			
			
			self->plant[i].PhCanopyPrd_kgm2yr 	= self->plant[i].PhCanopyPrd*0.1;				// -> [kgC/m2*yr]
			self->plant[i].PhCanopyPrd_dt		= self->plant[i].PhCanopyPrd/365.25*dt;
			
			
			/* Aggregation of time steps for output */
			self->plant[i].GrossCanopyPrd_R			= self->plant[i].GPP_Canopy*3.786912;			// [µmol/m2/s] -> [tC/ha*yr]
			self->plant[i].GrossCanopyPrd_dt		= self->plant[i].GrossCanopyPrd_R/365.25*dt;	// [tC/ha*yr] -> [tC/ha*d] -> [tC/ha*dt]
			
			self->plant[i].GrossCanopyPrd_Day		+= self->plant[i].GrossCanopyPrd_dt;			// [tC/ha*d], GPP_Day
			self->plant[i].GrossCanopyPrd_Sum		+= self->plant[i].GrossCanopyPrd_dt;			// [tC/ha]
			self->plant[i].GrossCanopyPrd_kgm2yr 	= self->plant[i].GrossCanopyPrd_R*0.1;			// -> [kgC/m2*yr]

			
			/* Canopy Transpiration */
			self->plant[i].TransCan_dt = CanopyTranspiration;				// [mm]			
			//self->plant[i].TransCan_dt *= self->plant[i].StressFacPh;		// aplying the stress factor for transpiration reduction
			self->plant[i].TransCan_Day += self->plant[i].TransCan_dt;
			
			/* Canopy Maintenance Respiration (Leaves) */
			self->plant[i].MainRespLfLay_R			= CanopyMaintResp*3.786912;						// [µmol/m2/s] -> [tC/ha*yr]
			self->plant[i].MainRespLfLay_dt			= self->plant[i].MainRespLfLay_R/365.25*dt;		// [tC/ha*yr] -> [tC/ha*d] -> [tC/ha*dt]
			self->plant[i].MainRespLfLay_Sum		+= self->plant[i].MainRespLfLay_dt;				// [tC/ha/ time period]
			
			// Dark leaf respiration rate for the biomass growth submodel
			self->plant[i].MainRespLf = self->plant[i].MainRespLfLay_R;		// [tC/ha*yr]
			
		}
		
		/*==========================================================================================================================================*/
		/* NUTRIENT LIMITATION [tC/ha*yr] */
		/*==========================================================================================================================================*/
		self->plant[i].PhCanopyNoN = (1.0-self->plant[i].DamagePh)*self->plant[i].PhCanopyPrd;
		self->plant[i].PhCanopyNlim	= pow(self->plant[i].kN, 2.0)*self->plant[i].PhCanopyNoN;
		
		// species specific, [tC/ha*yr] => [tC/ha*timestep]
		self->plant[i].PhCanopyNoN_dt	= self->plant[i].PhCanopyNoN*(dt/365.25)*self->plant[i].TreeDistr;
		self->plant[i].PhCanopyNlim_dt	= self->plant[i].PhCanopyNlim*(dt/365.25)*self->plant[i].TreeDistr;
		
		//=======================================================================
		if(NewDay(xpn->pTi))
		{
			// GPP
			self->plant[i].GPP_Canopy_Day			= 0.0;
			self->plant[i].GrossCanopyPrd_Day		= 0.0;
			
			// NPP
			self->plant[i].NetPhotoMolCanopy_Day 	= 0.0;
			self->plant[i].PhCanopyNoN_Day 			= 0.0;
			self->plant[i].PhCanopyNlim_Day 		= 0.0;
			
			// Transpiration
			self->plant[i].TransCan_Day 			= 0.0;
			
			// Dark Leaf Respiration
			self->plant[i].MainRespLfLay_Day		= 0.0;
			self->plant[i].MainRespLf_Bossel_Day 	= 0.0;
		}
		
		// Total Sums
		self->plant[i].PhCan_Tot 				+= self->plant[i].PhCanopyPrd/365.25*dt;	// [tC/ha]		
		self->plant[i].TransCan_Tot 			+= (self->plant[i].TransCan_dt);			// [mm]
		
		// Daily Sums
		self->plant[i].PhCanopyNoN_Day 			+= self->plant[i].PhCanopyNoN_dt;
		self->plant[i].PhCanopyNlim_Day 		+= self->plant[i].PhCanopyNlim_dt;
		self->plant[i].PhCanopyNlim_Yr 			+= self->plant[i].PhCanopyNlim_dt;		
		
		self->plant[i].MainRespLfLay_Day		+= self->plant[i].MainRespLfLay_dt;			// Falge,	[tC/ha/d]
		self->plant[i].MainRespLf_Bossel_Day 	+= self->plant[i].MainRespLf/365.25*dt;		// Bossel,	[tC/ha/d] 
		
		
		// set temporal variables to zero for next species
		LfGPPCanopyLayer = 0.0;
		LfPhotosynthesisCanopyLayer = 0.0;
		LfTranspirationCanopyLayer = 0.0;
		LfConductanceCanopyLayer = 0.0;
		LfMaintRespLayer = 0.0;
	}
	
	return RET_SUCCESS;
}

int libtreemix_Radiation(libtreemix *self, double hour)
{
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	double Light;
	double seas_day;
	double soldec;
	//double hr = xpn->pTi->pSimTime->fTimeDay*24.0;		//actual hour of the day
	int iJulianDay = xpn->pTi->pSimTime->fTimeY;
	double dt = xpn->pTi->pTimeStep->fAct;
	//double sine;
	/****************************************************************************************************************/
	
	if(NewDay(xpn->pTi))
	{
		self->clim.Isumclear = 0.0;
		self->clim.Isumcloud = 0.0;
	}
		
		seas_day = ((double)iJulianDay+10.0)/365.25;
		soldec = (-23.4*cos(2.0*M_PI*seas_day))*M_PI/180.0;	// eq. 41
		self->clim.elev = sin(self->clim.Lat)*sin(soldec)+cos(self->clim.Lat)*cos(soldec)*cos(((2.0*M_PI)/24.0)*(hour+12.0));	// eq. 40
	
	
		if(self->clim.elev > 0.01)
		{
			Light = 1.0;
			// photoactive solar radiation, clear and cloudy sky, eq. 39, [W/m2]
			self->clim.Iclear = self->clim.SolarConstant*self->clim.PARfactor*self->clim.elev*exp(-self->clim.AtmAbs/self->clim.elev);	// eq. 39
			self->clim.Icloud = self->clim.CloudFraction*self->clim.Iclear;																// eq. 42
		}
		else
		{
			Light = 0.0;
			self->clim.Iclear = 0.0;
			self->clim.Icloud = 0.0;
			//self->clim.Cloud = 0.0;
		}
		
		self->clim.Isumclear += self->clim.Iclear*dt;
		self->clim.Isumcloud += self->clim.Icloud*dt;
		
	
	return RET_SUCCESS;
}

int libtreemix_FrostHardiness(libtreemix *self, int i)
{
	/* Description */
	/*
	 * Calculation of the Frost Hardiness for coniferous trees
	 * (Ilkka Leinonen, Nov 1998) 
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	if(strcmp(self->plant[i].type, "coniferous") == 0)
	{
		double dltaPhotoCap;	// change of photosynthetic capacity
		double A_seas = 2.0;	// values of the frost hardiness model of LEINONEN (1996) 
		double C_seas = 600.0;
		double Spruce = 5000.0;

		dltaPhotoCap =	(100.0/(1.0+100.0*pow(A_seas, -(pWe->fTempAir-(Spruce/C_seas)))))-
						(100.0/(1.0+100.0*pow(A_seas, +(pWe->fTempAir-(Spruce/C_seas)))));

		dltaPhotoCap += Spruce;

		if(dltaPhotoCap<EPSILON)		{
			self->plant[i].K = EPSILON;	//avoid neagtive values
		}
		else if(dltaPhotoCap<6500.0){
			self->plant[i].K = dltaPhotoCap/6500.0;
		}
		else{
			self->plant[i].K = 1;
		}
	}
	else{
		self->plant[i].K = 1.0;	// for decidous trees
	}	
	
	return RET_SUCCESS;
}

double libtreemix_PhotosynthesisBossel(libtreemix *self, int i, double radiation, double fTempAir, double fAtmCO2ppm)
{
	/* Description */
	/*
	 * calculates the temperature effect on photosynthesis, eq. 38
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	double LfPhotoR; 
	double PhotoTreedyn;
	double PhotoBossel;
	
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	// eq. (38), calc. temperature effect on photosynthesis [-]
	if((self->plant[i].PhTzero < fTempAir)&&(fTempAir < ((self->plant[i].PhTopt+self->plant[i].PhTzero)/2.0))){
		self->plant[i].k_Tp = 2.0*pow((fTempAir-self->plant[i].PhTzero)/
			(self->plant[i].PhTopt-self->plant[i].PhTzero), 2.0);
	}
	else if((((self->plant[i].PhTopt+self->plant[i].PhTzero)/2.0) < fTempAir)&&(self->plant[i].k_Tp >= 0.0)){
		self->plant[i].k_Tp = 1.0-2.0*pow(((fTempAir - self->plant[i].PhTopt)/
			(self->plant[i].PhTopt-self->plant[i].PhTzero)), 2.0);
			
	}
	else{
		self->plant[i].k_Tp = 0.0;
	}
	
	if( self->plant[i].k_Tp < 0.0)
	{
		self->plant[i].k_Tp = 0.0;
	}
	
	

	// original coded in TREEDYN
	PhotoTreedyn = self->plant[i].k_Tp
					*(2.0-self->plant[i].PhCO2Fac+(self->plant[i].PhCO2Fac-1.0)*(pWe->fAtmCO2ppm/310.0))
					*self->plant[i].PhInitSlope*radiation*self->plant[i].PhProdMax/(self->plant[i].PhProdMax+self->plant[i].PhInitSlope*radiation);	

	// eq. 56, leaf photosynthesis rate [mgCO2/dm²*h]
	LfPhotoR = (self->plant[i].PhInitSlope*radiation)/(1.0+(self->plant[i].PhInitSlope*radiation/self->plant[i].PhProdMax));	
	
	// => both upper equations give the same results, except, that the upper equation includes the CO2 concentration!

	// eq. 58, instantaneous photosynthesis rate (original Bossel equation)
	PhotoBossel = (self->plant[i].PhProdMax/self->plant[i].LightExt)
					* log((1.0+(self->plant[i].PhInitSlope*radiation/self->plant[i].PhProdMax))
					/ (1.0+(self->plant[i].PhInitSlope*radiation/self->plant[i].PhProdMax)*exp(-self->plant[i].LightExt*self->plant[i].LAI)));
		
		
	PhotoBossel = self->plant[i].k_Tp*PhotoBossel;
	LfPhotoR = self->plant[i].k_Tp*LfPhotoR;
		
	return PhotoTreedyn;
	//return LfPhotoR;
	//return PhotoBossel;
}

int libtreemix_LeafPhotosynthesisFALGE(libtreemix *self, int i, double radiation)
{
	/* Description */
	/*
	 * Calculation of the Leaf Photosynthesis
	 * after the FALGE model
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	PWEATHER pWe = xpn->pCl->pWeather;
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	
	/* Variables */
	double rH = pWe->fHumidity/100.0;	// [%]->[-]	
	double AirPressure; //[hPa]
	
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	int libtreemix_PhotosynthesisAnalyticalBaldocchi(libtreemix *self, int i);
	/****************************************************************************************************************/	
	
	/* calculate Air Pressure (barometrische höhenformel) */
	if(pWe->fAtmPressure == 0.0)
	{
		double a1, a2, a3 = 0;
		a1 = 0.0065*xpn->pLo->pFarm->fAltitude;
		a2 = pWe->fTempAir + a1;
		a3 = (pWe->fTempAir/a2);
		
		AirPressure = 1013.25/pow(a3, -5.255);
	}
	else
	{
		AirPressure = pWe->fAtmPressure/100.0;	// [Pa] -> [hPa]
	}
	
	/* recalc. GFac with soil water stress:*/
    self->plant[i].GFac = self->plant[i].StressFacPh * self->plant[i].gfacbase;
	
	// calculation of the patial vapour pressures
	self->plant[i].ewleaf = 6.11  * exp(17.26938818 * pWe->fTempAir/(237.3+pWe->fTempAir));
	self->plant[i].ewair = rH* self->plant[i].ewleaf;
	self->plant[i].vpd = self->plant[i].ewleaf - self->plant[i].ewair;
	
	// calculations referring to the temperature
	self->clim.rtk = (pWe->fTempAir+273.16) * IDGASKONST;
	
	// calculations referring to the laminar layer	
	if(pWe->fWindSpeed < EPSILON)
	{
		self->plant[i].laminarCond = 0.001; // chged from 0.000001, this was to small, caused errors!!!
	}
	else
	{
		self->plant[i].dbl = 0.004 * sqrt(self->plant[i].WidthOfLeaf / (pWe->fWindSpeed*100.0)); // 100: [m/s]->[cm/s]
		self->plant[i].dwv = 2.126e-05 + 1.48e-07 * pWe->fTempAir;

		self->plant[i].laminarCond = self->plant[i].dwv/self->plant[i].dbl * 1000.0;
		self->plant[i].laminarCond *= (AirPressure*100.0)/self->clim.rtk;
		
		// only used for coniferous trees (set for decidious to -1) :
		if (self->plant[i].SigmaLeaf != -1)
		{
			self->plant[i].laminarCond /= (1.67 * pow(self->plant[i].SigmaLeaf, 0.43));
		}
	}
	
	// Berechnung der Werte in den Hilfsvariablen
	self->plant[i].ewr  = self->plant[i].ewair / self->plant[i].ewleaf;
	self->plant[i].ewdr = (self->plant[i].ewair - self->plant[i].ewleaf) / self->plant[i].ewleaf;
	
	
	// Berechnung der Modellgleichungen    
	self->plant[i].Ko    = self->plant[i].ko298 * exp(self->plant[i].DeltaHaKo * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));
	self->plant[i].Kc    = self->plant[i].kc298 * exp(self->plant[i].DeltaHaKc * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));
	
	self->plant[i].VcMax = self->plant[i].K * self->plant[i].vcmax298 * exp(self->plant[i].DeltaHaVcMax * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk))
						*(1.0 + exp((self->plant[i].DeltaSVcMax * 298.0 - self->plant[i].DeltaHdVcMax) / (IDGASKONST*298.0))) 
						/(1.0 + exp((self->plant[i].DeltaSVcMax * (pWe->fTempAir+273.16) - self->plant[i].DeltaHdVcMax) /self->clim.rtk));
	
	// calc. dark leaf respiration:
	self->plant[i].DarkResp = self->plant[i].darkresp298 * exp(self->plant[i].DeltaHaresp * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));

	self->plant[i].tau = self->plant[i].tau298 * exp(self->plant[i].DeltaHatau * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));
	
	self->plant[i].Pml = self->plant[i].K * self->plant[i].pml298 * (pWe->fTempAir+273.16) / 298.0 * exp(self->plant[i].DeltaHaPml * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk)) *
			   (1.0 + exp((self->plant[i].DeltaSPml * 298.0 - self->plant[i].DeltaHdPml) / (IDGASKONST*298.0))) /
			   (1.0 + exp((self->plant[i].DeltaSPml * (pWe->fTempAir+273.16)- self->plant[i].DeltaHdPml) /self->clim.rtk));
	
	self->plant[i].Pm = self->plant[i].alpha * radiation / sqrt(1.0 + pow(self->plant[i].alpha * radiation/self->plant[i].Pml, 2.0) );
	
	//self->plant[i].fac = 0.5;			// is read from ini file, new from Eva Falge, that dark respiration in light conditions are lower than in night
	self->plant[i].Gamma = 0.5 * pWe->fAtmO2ppm * 1000.0 / self->plant[i].tau;	
	
	libtreemix_PhotosynthesisAnalyticalBaldocchi(self, i);
	
	self->plant[i].StomCond = 1.0 / (1.0/self->plant[i].laminarCond + 1.0/self->plant[i].StomCond);
	self->plant[i].Transpiration = self->plant[i].vpd / AirPressure * self->plant[i].StomCond;
	self->plant[i].photosyn_brutto /= 0.00156;

	// factor 0.0648 is: mmolH2O/m2/s -> mm/d :
	self->plant[i].Transpiration *= 0.0648*24.0;

	// factor 1/400 is : mmolH2O/m2(total)/s -> cm/s
	self->plant[i].StomCond = self->plant[i].StomCond/400.0;

	/* 
	 * NET PHOTOSYNTHESIS, TRANSPIRATION AND CONDUCTANCE 
	 * is recalculated to microMol/m2(proj)/s:
	 * 
	 * factor TotalToProjM2Factor == 2.6 is 
	 * 
	 * mol CO2/m2(total)/s -> microMol CO2/m2(proj)/s :
	 * 
	 */ 
	self->plant[i].NetPhotoMol	 *= self->plant[i].totaltoprojm2factor;
	self->plant[i].StomCond 	 *= self->plant[i].totaltoprojm2factor;
	self->plant[i].Transpiration *= self->plant[i].totaltoprojm2factor;
	
	return RET_SUCCESS;
}

int libtreemix_PhotosynthesisAnalyticalBaldocchi(libtreemix *self, int i)
{
	/* Description */
	/*
	 * main iteration (or pseudo iteration) routine for calc. Wc, Wj etc.
	 * see BALDOCCHI (1994) An analytical solution for coupled
	 * leaf photosynthesis	and stomatal conductance models
	 * in Treephysiology 14, 1069-1079	
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	int j;
	double rH = pWe->fHumidity/100.0;	// [%]->[-]
	double Cint1;
	double Cint;	// internal CO2 concentration
	double Wc; 		// rate of carboxylation when RuBP (ribulose bisphosphate) is saturated
	double Wj; 		// rate of carboxylation when RuBP regeneration is limited by electron transport
	
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	double libtreemix_Baldocchi(libtreemix *self, double Wc, double Wj, double Cint, float rH, int i);
	/****************************************************************************************************************/
	
	Cint1 = 0.7*pWe->fAtmCO2ppm;
	Cint = Cint1;
	
	// newer Eva Falge version:
	// Wc and Wj take the form: (a*Ci - a*d) / (e*Ci + b), using eq. (3)
	Wc = self->plant[i].VcMax * (Cint1 - self->plant[i].Gamma) / (Cint1 + self->plant[i].Kc * (1.0 + pWe->fAtmO2ppm / self->plant[i].Ko));
	Wj = self->plant[i].Pm    * (Cint1 - self->plant[i].Gamma) / (Cint1 + 2.0 * self->plant[i].Gamma);
	
	for (j=0; j<111; j++)
	{
		Cint = libtreemix_Baldocchi(self, Wc, Wj, Cint, rH, i);

		if (fabs(Cint - Cint1) > 0.1)
		{
			Cint1 = Cint;
			Wc    = self->plant[i].VcMax * (Cint1 - self->plant[i].Gamma) / (Cint1 + self->plant[i].Kc * (1.0 + pWe->fAtmO2ppm / self->plant[i].Ko));
			Wj    = self->plant[i].Pm    * (Cint1 - self->plant[i].Gamma) / (Cint1 + 2.0 * self->plant[i].Gamma);
		}
		else 
		{
			break;
		}
	}
	
	self->plant[i].Cint = Cint;
	
	return RET_SUCCESS;
}

double libtreemix_Baldocchi(libtreemix *self, double Wc, double Wj, double Cint, float rH, int i)
{
	/* Description */
	/*
	 * calc. the leaf photosynthesis with Baldocchi analytical solution
	 * relatively new Eva-Baldocchi-Version: (change from 29/8/96 + 30/9/96)
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	double aaa, bbb, eee;				//coefficients from eq. (2) u. (3)
	double ddd = self->plant[i].Gamma;
		// only intermediate variables
	double alpha, beta, gamma, theta;	
	double rrr, sss, ttt, uuu, vvv;
	double QQQQ, PPPP, DDDD;
	double aquad1, bquad1, cquad1;
	
	// simplifications for alpha, beta, gamma und theta
	//double mrh	= self->plant[i].GFac * rH/1.56;
	//double g_b	= self->plant[i].laminarCond;
	//double b_		= self->plant[i].StomCondMin;
	
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	if (Wc <= Wj)	// if Wc is minimal, the coefficients correspond to: see eq. (9)
	{
		aaa = self->plant[i].VcMax;		//max carboxyl. rate when RuBP is saturated
		bbb = self->plant[i].Kc * (1.0 + pWe->fAtmO2ppm / self->plant[i].Ko);
		eee = 1.0;
	}
	else			//if Wj is minimal, the coefficients correspond to:
	{   
		aaa = 4.0 * self->plant[i].Pm;		//potential rate of electron transport
		bbb = 8.0 * self->plant[i].Gamma;		//CO2 compensation point in the absence of dark respiration (resp = photosynth)
		eee = 4.0;
	}            

	// is read in from ini file
	//self->plant[i].fac = 1.0;

	//alpha = 1560*1560 * (1.0 + b_ / g_b - mrh);
	alpha = 1560*1560 * (1.0 + self->plant[i].StomCondMin / self->plant[i].laminarCond - self->plant[i].GFac * rH/1.56);	// eq. (11), GFac is supposed to be the dim.-less slope	
	
	 //===================solution for the cubic equation:==================
	if (fabs(alpha) > EPSILON)
	{
		//beta  = 1560 * pWe->fAtmCO2ppm * (mrh * g_b - 2.0 * b_ - g_b) - 1560*1560 * (self->plant[i].DarkResp * self->plant[i].fac * mrh);
		beta = 1560 * pWe->fAtmCO2ppm * (self->plant[i].GFac * rH / 1.56 * self->plant[i].laminarCond - 2.0 * self->plant[i].StomCondMin - self->plant[i].laminarCond) -
			1560 * 1560 * (self->plant[i].DarkResp * self->plant[i].fac * self->plant[i].GFac * rH/1.56); // rd==DarkResp

		//gamma = g_b * (pWe->fAtmCO2ppm * b_ + 1560 * self->plant[i].fac * self->plant[i].DarkResp * mrh);
		gamma = self->plant[i].laminarCond * (pWe->fAtmCO2ppm * self->plant[i].StomCondMin + 1560 * self->plant[i].fac * self->plant[i].DarkResp * self->plant[i].GFac * rH/1.56);

		//theta = 1560 * (mrh * g_b - b_);
		theta = 1560 * (self->plant[i].GFac * rH/1.56 * self->plant[i].laminarCond - self->plant[i].StomCondMin);

		//--------------------------------------------------------------------------------------------

		rrr = (eee*beta + bbb*theta - aaa*alpha + eee*alpha * self->plant[i].fac * self->plant[i].DarkResp) / (eee*alpha);

		sss = (eee*gamma * pWe->fAtmCO2ppm + bbb*gamma - aaa*beta + aaa*ddd*theta + 
			eee*self->plant[i].fac*self->plant[i].DarkResp*beta+bbb*self->plant[i].fac*self->plant[i].DarkResp*theta)/(eee*alpha);

		ttt = (-aaa*gamma * pWe->fAtmCO2ppm + aaa*ddd * gamma + eee*self->plant[i].fac * self->plant[i].DarkResp * gamma * pWe->fAtmCO2ppm + 
			bbb * self->plant[i].fac * self->plant[i].DarkResp * gamma) / (eee*alpha);

		PPPP = (3*sss - rrr*rrr) / 3.0;						// 3sss - rrr² / 3
		//PPPP = (3*sss - pow(rrr, 2))/3.0;

		QQQQ = 2*rrr*rrr*rrr / 27.0 - rrr*sss / 3.0 + ttt;	// 2rrr³/27 - rrr*sss/3 + ttt
		//QQQQ = 2*pow(rrr, 3) / 27.0 - rrr*sss / 3.0 + ttt;

		DDDD = pow(PPPP, 3) / 27.0 + pow(QQQQ, 2) / 4.0;		// 

		if (QQQQ < 0)       // change 5/11/96; `>` -> `<`
		{
			vvv = -sqrt(fabs(PPPP)/3.0);
		}
		else
		{
			vvv = sqrt(fabs(PPPP)/3.0);
		}

		uuu = QQQQ/(2.0*vvv*vvv*vvv);
		//uuu = QQQQ/(2.0*pow(vvv, 3));

		if (PPPP > 0)
		{
			// one real solution, conjugates:
			self->plant[i].NetPhotoMol = -2.0*vvv * sinh(log(uuu + sqrt(uuu*uuu + 1.0))/3.0) - rrr/3.0;
		}
		else
		{
			if (DDDD > 0)
			{
				//  one real solution, conjugates:
				self->plant[i].NetPhotoMol = -2*vvv * cosh(log(uuu - sqrt(uuu*uuu - 1))/3.0) - rrr/3.0;
			}
			else	      
			{
				// three real solutions:
				double NetPhotoMol1, NetPhotoMol2, NetPhotoMol3;

				NetPhotoMol1 = -2*vvv * cos(acos(uuu)/3.0) - rrr/3.0;
				NetPhotoMol2 = -2*vvv * cos((acos(uuu) + 2*PI)/3.0) - rrr/3.0;
				NetPhotoMol3 = -2*vvv * cos((acos(uuu) + 4*PI)/3.0) - rrr/3.0;

				if (alpha > 0)
				{
					self->plant[i].NetPhotoMol = NetPhotoMol2;
				
					if(NetPhotoMol1 > 0 && NetPhotoMol2 > 0 && NetPhotoMol3 > 0)
					{
						// minimum of all three:
						self->plant[i].NetPhotoMol = min3(NetPhotoMol1, NetPhotoMol2, NetPhotoMol3);
					}
					else
					{
						if (((NetPhotoMol1 * NetPhotoMol2 * NetPhotoMol3) < 0) && (max3(NetPhotoMol1, NetPhotoMol2, NetPhotoMol3) > 0))
						{
							self->plant[i].NetPhotoMol = min3(NetPhotoMol1, NetPhotoMol2, NetPhotoMol3);
						}
						else
						{
							self->plant[i].NetPhotoMol = max3(NetPhotoMol1, NetPhotoMol2, NetPhotoMol3);
						}
					}
				}
				else
				{
					self->plant[i].NetPhotoMol = NetPhotoMol3;
				}
			}
		}
	} //---END: cubic solution---	
	else	//===================quadratic solution:========================================
	{
		beta = -1560*pWe->fAtmCO2ppm * self->plant[i].StomCondMin - 1560*1560 * self->plant[i].DarkResp * self->plant[i].fac * self->plant[i].GFac*rH/1.56;
	  
		gamma = self->plant[i].laminarCond * (pWe->fAtmCO2ppm * self->plant[i].StomCondMin + 1560 * self->plant[i].fac * self->plant[i].DarkResp * self->plant[i].GFac*rH/1.56);
	  
		theta = 1560 * self->plant[i].laminarCond;

	  
		aquad1 = eee*beta + bbb*theta;
	  
		bquad1 = eee*gamma*pWe->fAtmCO2ppm - aaa*beta + eee * self->plant[i].fac * self->plant[i].DarkResp * beta + aaa*ddd*theta +
				 bbb*self->plant[i].fac*self->plant[i].DarkResp*theta + bbb*gamma;
	  
		cquad1 = -aaa*gamma*pWe->fAtmCO2ppm + eee * self->plant[i].fac * self->plant[i].DarkResp * gamma * pWe->fAtmCO2ppm +
			bbb * self->plant[i].fac * self->plant[i].DarkResp * gamma + aaa*ddd*gamma;
	  
		self->plant[i].NetPhotoMol = (-bquad1 - sqrt(bquad1 * bquad1 - 4*aquad1 * cquad1)) / (2.0*aquad1);

	} //---end of quadratic solution---

	self->plant[i].Cs = pWe->fAtmCO2ppm - self->plant[i].NetPhotoMol * 1560/self->plant[i].laminarCond;

	self->plant[i].StomCond = self->plant[i].GFac * 1000 * (self->plant[i].NetPhotoMol + self->plant[i].fac * self->plant[i].DarkResp) * rH / self->plant[i].Cs
		+ self->plant[i].StomCondMin;

	Cint = self->plant[i].Cs - self->plant[i].NetPhotoMol * 1560/self->plant[i].StomCond;

    self->plant[i].photosyn_brutto = 1E-6 * 1560.0 * (self->plant[i].NetPhotoMol + self->plant[i].fac * self->plant[i].DarkResp);

	//NetPhotoMol*=0.001; // now in mikromol	
	
	return Cint;
}

int libtreemix_LeafPhotosynthesisFALGE_simple(libtreemix *self, int i, double radiation, int LeafLayer)
{
	/* Description */
	/*
	 * calc. the leaf photosynthesis with Baldocchi analytical solution
	 * relatively new Eva-Baldocchi-Version: (change from 29/8/96 + 30/9/96)
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	PWEATHER pWe = xpn->pCl->pWeather;
	
	/* Variables */
	int control_switch = 1;
	
	double rbh, rt, lambda, Tleaf;
	double Cint = 0.0;
	double Wc = 0.0;
	double Wj = 0.0;
	
	double AirPressure = 0.0;
	
	double Pm = 0.0;
	
	double PnetRd1 = 0.0;	
	double Pnet1 = 0.0;
	double Pgross1 = 0.0;
	double Rd = 0.0;
	
	
	double StomCond = 0.0;
	
	double Transpiration = 0.0;
	double TranspirationR = 0.0;
	double Transpirationdt = 0.0;
	
	double rH = pWe->fHumidity/100.0;	// [%]->[-]
	double dt = xpn->pTi->pTimeStep->fAct;
	
	/****************************************************************************************************************/
	/* calculate Air Pressure (internationale höhenformel, wikipedia) */
	if(self->conf.AirPressure == 1.0)
	{
		double a1, a2 = 0;		
		a1 = 0.0065*xpn->pLo->pFarm->fAltitude;
		a2 = (pWe->fTempAir+273.16) + a1;	// a2 = Temperature at sea level, with a gradient of 0.65K/100m
		AirPressure = 1013.25*pow((1-(a1/a2)), 5.255);	// [hPa]
	}
	else
	{
		AirPressure = pWe->fAtmPressure/100.0;	// [Pa] -> [hPa]
	}
	self->clim.AirPressure = AirPressure;
	
	self->plant[i].GFac = self->plant[i].StressFacPh * self->plant[i].gfacbase;
	
	// calculation of the patial vapour pressures
	self->plant[i].ewleaf = 6.11  * exp(17.26938818 * pWe->fTempAir/(237.3+pWe->fTempAir));
	self->plant[i].ewair = rH* self->plant[i].ewleaf;
	self->plant[i].vpd = self->plant[i].ewleaf - self->plant[i].ewair;
	
	/* calculation of leaf temperature acc. to Diss. Lenz 2004, page 38, eq. 2.2.2-14 */
	rbh = 100*sqrt((self->plant[i].WidthOfLeaf/100.0)/pWe->fWindSpeed); 									// leaf boundary layer resistance to heat, rbh [s/m]
	rt	= 0.74 * pow(log((2 - 0.7*self->plant[i].H)/(0.1*self->plant[i].H)), 2) / (0.16*pWe->fWindSpeed); 	// turbulence resistance, rt [s/m]
	lambda = (2502.3 - 2.4289*pWe->fTempAir)*1000.0;														// latent heat of water vapourization
	Tleaf = pWe->fTempAir + ((rbh+rt)*(pWe->fSolRad - lambda*(self->plant[i].PotTransdt/1000.0))/1200.0 );
	
	// calculations referring to the temperature
	self->clim.rtk = IDGASKONST*(pWe->fTempAir+273.16);
	
	// temperature dependencies of the rates Ko, Kc, DarkResp and tau from the optimum of 25°C or 298K
	self->plant[i].Ko    	= self->plant[i].ko298 			* exp(self->plant[i].DeltaHaKo * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));	// [mmol/mol]
	self->plant[i].Kc    	= self->plant[i].kc298 			* exp(self->plant[i].DeltaHaKc * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));	// [µmol/mol]
	self->plant[i].DarkResp = self->plant[i].darkresp298 	* exp(self->plant[i].DeltaHaresp*(1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));	// [µmol/m2/s]
	self->plant[i].tau		= self->plant[i].tau298 		* exp(self->plant[i].DeltaHatau* (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk));
	
	{ /* strictly according to Falge et al 1996 */
		self->plant[i].Ko_Falge			= exp(self->plant[i].f_Ko   - (self->plant[i].DeltaHaKo  /self->clim.rtk));
		self->plant[i].Kc_Falge			= exp(self->plant[i].f_Kc   - (self->plant[i].DeltaHaKc  /self->clim.rtk));
		self->plant[i].DarkResp_Falge 	= exp(self->plant[i].f_resp - (self->plant[i].DeltaHaresp/self->clim.rtk));
		self->plant[i].tau_Falge		= exp(self->plant[i].f_tau  - (self->plant[i].DeltaHatau /self->clim.rtk));
	}
	
	// maximum carboxylation velocity [µmol CO2/m2*s]
	self->plant[i].VcMax = self->plant[i].K * self->plant[i].vcmax298 * exp(self->plant[i].DeltaHaVcMax * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk))
						*(1.0 + exp((self->plant[i].DeltaSVcMax * 298.0 - self->plant[i].DeltaHdVcMax) / (IDGASKONST*298.0))) 
						/(1.0 + exp((self->plant[i].DeltaSVcMax * (pWe->fTempAir+273.16) - self->plant[i].DeltaHdVcMax) /self->clim.rtk));
	
	{ /* strictly according to Falge et al 1996 */
		
		double numerator = 0.0;
		double denominator = 0.0;
		
		numerator = exp( 33.719 - self->plant[i].DeltaHaVcMax / self->clim.rtk );
		denominator = 1.0 + exp( self->plant[i].DeltaSVcMax*(pWe->fTempAir+273.16) - self->plant[i].DeltaHdVcMax ) / self->clim.rtk;
		
		/* eq. 5 */ 
		self->plant[i].VcMax_Falge = numerator / denominator;
	}
	
	
	// potential rate of RuBP regeneration
	self->plant[i].Pml = self->plant[i].K * self->plant[i].pml298 * (pWe->fTempAir+273.16)/298.0
					* exp(self->plant[i].DeltaHaPml * (1.0/(IDGASKONST*298.0) - 1.0/self->clim.rtk))
			  *(1.0 + exp((self->plant[i].DeltaSPml * 298.0 - self->plant[i].DeltaHdPml) / (IDGASKONST*298.0)))
			  /(1.0 + exp((self->plant[i].DeltaSPml * (pWe->fTempAir+273.16)- self->plant[i].DeltaHdPml) /self->clim.rtk));
			  
	{ /* strictly according to Falge et al 1996 */
		double numerator = 0.0;
		double denominator = 0.0;
		
		numerator = (pWe->fTempAir+273.16) * exp( 15.044 - self->plant[i].DeltaHaPml / self->clim.rtk );
		denominator = 1.0 + exp( self->plant[i].DeltaSPml * (pWe->fTempAir+273.16) - self->plant[i].DeltaHdPml ) / self->clim.rtk;
		
		self->plant[i].Pml_Falge = numerator/denominator;
	}
	
	// potential rate of electron transport
	Pm 					= self->plant[i].alpha * radiation / sqrt(1.0 + pow(self->plant[i].alpha * radiation/self->plant[i].Pml, 2.0) );
	
	self->plant[i].Pm 	= self->plant[i].alpha * radiation / sqrt(1.0 + pow(self->plant[i].alpha * radiation/self->plant[i].Pml, 2.0) );
	
	/* CO2 compensation point in the absence of dark respiration (resp = photosynth) [µmol/mol] */
	self->plant[i].Gamma 		= 0.5 * pWe->fAtmO2ppm * 1000.0 / self->plant[i].tau;
	
	Cint = pWe->fAtmCO2ppm * (1.0 - 1.6 * rH/self->plant[i].GFac);
	if(Cint < 200.0)
	{
		Cint = 200.0;
	}
	self->plant[i].Cint1 = Cint;
	
	/* RuBP-saturated (Rubisco-limited) rate of CO2 assimilation: (see Diss Lenz, pg 30), PAY ATTENTION TO UNITS!
	 * VcMax    ... maximum rate of carboxylation limitedby Rubisco activity 	[µmol CO2/m2*s]
	 * Cint     ... CO2 concentration at carboxylation site of chloroplasts		[µmol/mol]
	 * AtmO2ppm ... intercellular oxygen concentration (assumed to be constant) [mmol/mol]
	 * Kc, Ko   ... Michaelis-Menten constants for CO2 and O2, describing the kinetics of Rubisco [µmol/mol], [mmol/mol]	 
	 */
	Wc = self->plant[i].VcMax * Cint / (Cint + self->plant[i].Kc * (1.0 + pWe->fAtmO2ppm / self->plant[i].Ko));	// [µmol CO2/m2*s]
	
	/* electron transport-limited rate: */	
	Wj = Pm * Cint / (Cint + 2.0*self->plant[i].Gamma);
	
	/* Falge et al 1996:
	 * "Dark respiration is assumed to increase 
	 * from 0.5 Rd to Rd as PPFD decreases over the range 25 to 0 µmol m-2 s-1."
	*/ 
	if(radiation < 0.01)
	{
		self->plant[i].fac = 1.0;
	}
	else
	{
		self->plant[i].fac = 0.5;
	}
	
	// NetPhotMol [µmol/m2/s], choose min of Wc and Wj
	if(Wc > Wj)
	{		
		Pnet1 = (1.0-self->plant[i].Gamma/Cint)*Wj-(self->plant[i].fac*self->plant[i].DarkResp);
		Pgross1 = (1.0-self->plant[i].Gamma/Cint)*Wj;
	}
	else
	{
		Pnet1 = (1.0-self->plant[i].Gamma/Cint)*Wc-(self->plant[i].fac*self->plant[i].DarkResp);
		Pgross1 = (1.0-self->plant[i].Gamma/Cint)*Wc;
	}
	
	
	
	// Control, [mmol/m2*s]
	//PnetRd1 = 1000.0*(Pnet1+self->plant[i].fac*self->plant[i].DarkResp)*rH/pWe->fAtmCO2ppm;
	
	/* calculation of stomatal conductance and transpiration [mmol/m2/s] */
	StomCond = self->plant[i].StomCondMin+self->plant[i].GFac*1000.0*(Pnet1+self->plant[i].fac*self->plant[i].DarkResp)*rH/pWe->fAtmCO2ppm;
	
    //printf("%f %f %f %f %f %f %f \n",StomCond, rH, self->plant[i].GFac, Pnet1, self->plant[i].fac, self->plant[i].DarkResp,pWe->fAtmCO2ppm);
    
    self->plant[i].StomCond = StomCond/400.0; 						//[mmolH2O/m2(total)*s] -> [cm/s]
	
	Transpiration = self->plant[i].vpd/AirPressure * StomCond;						// [mmol/m2*s]
	TranspirationR = Transpiration * 1.5552;										// [mmolH2O/m2*s] -> [mm/m2*d] = [mm/d]
	Transpirationdt = TranspirationR * dt;											// [mm]
	self->plant[i].Transpiration_LayerR  = TranspirationR;							// [mm/d]
	
	
	control_switch = 0;
	//if(Transpiration > 0.0)
	if( control_switch == 1)
	//if(radiation > 0.0)
	{
		char S[128];
		
		//sprintf(S,"Stomatal Cond. [mmol/m2/s] in Layer %d: %f\n", LeafLayer, StomCond);
		//PRINT_MESSAGE(xpn,3,S);
		
		sprintf(S,"Transpiration [mm] in Layer %d is %f \n", LeafLayer, Transpirationdt);
		PRINT_MESSAGE(xpn,3,S);
		
		//sprintf(S,"Net Photosynthesis [µmol/m2/s] in Layer %d: %f\n", LeafLayer, Pnet1);
		//PRINT_MESSAGE(xpn,3,S);
	}
	
	/* ----------------------------------------------------------------------------------
	 net/gross primary production (photosynthesis), transpiration, and conductivity is recalculated to [µmol/m2(proj)*s]
	 factor totaltoprojm2factor==2.6:	[µmolCO2/m2(total)*s] -> [µmolCO2/m2(proj)*s]
	*/
	self->plant[i].NetPhotoMolLay 			= Pnet1; 		//* self->plant[i].totaltoprojm2factor;; 	// [µmol/m2/s]	
	self->plant[i].GPP_Layer				= Pgross1; 		//* self->plant[i].totaltoprojm2factor;		// [µmol/m2/s]	
	
	self->plant[i].Transpiration_Layerdt 	= Transpirationdt * self->plant[i].totaltoprojm2factor;		// [mm]
	self->plant[i].StomCond 	 			= StomCond * self->plant[i].totaltoprojm2factor;			// [cm/s]
	
	self->plant[i].MainRespLfLay_R = (self->plant[i].fac*self->plant[i].DarkResp);
	
	// old calculation
	self->plant[i].Transpiration *= self->plant[i].totaltoprojm2factor;
	
	return RET_SUCCESS;
}
