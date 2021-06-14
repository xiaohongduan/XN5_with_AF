#include "libtreemix.h"
#include <math.h>
#include <string.h>

//#define C_DEBUG(var) xpn_register_var_add_double(xpn->pXSys->var_list,#var,var,-1,TRUE);

int libtreemix_BiomassGrowth(libtreemix *self)
{
	/* Description */
	/*
	 * Calculates the Respiration and Renewal Demands
	 * of the trees compartments as well as 
	 * the nitrogen availability
	 * 
	*/
	
	/* Abbreviations */
	//expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER 		pPW  = xpn->pPl->pPltWater;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTNITROGEN	pPlN = xpn->pPl->pPltNitrogen;
	//PPLTCARBON		pPlC = xpn->pPl->pPltCarbon;
	//PCANOPY			pCan = xpn->pPl->pCanopy;
	
	/* Variables */
	int i;	// species looping variable
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double LeafDemand;
	double RootDemand;	
	
	/*Functions*/
	int libtreemix_CalcTotalMaintenanceRespiration(libtreemix *self, int i);
	int libtreemix_CalcLeafRenewalDemand(libtreemix *self, int i, double* LeafDemand);
	int libtreemix_CalcRootRenewalDemand(libtreemix *self, int i, double* RootDemand);
	int libtreemix_CalcAssimilateAvailability(libtreemix *self, int i, double LeafDemand, double RootDemand);
	int libtreemix_CalcNitrogenAvailability(libtreemix *self, int i, double LeafDemand, double RootDemand);
	int libtreemix_CalcBiomassGrowth(libtreemix *self, int i);
	/****************************************************************************************************************/
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		libtreemix_CalcTotalMaintenanceRespiration(self, i);
	
		libtreemix_CalcLeafRenewalDemand(self,i, &LeafDemand);

		libtreemix_CalcRootRenewalDemand(self,i, &RootDemand);

		/* eq. 97, [tN/ha*yr] */
		self->plant[i].LeafNtransR = (self->plant[i].NLf-self->plant[i].NLfDead)*self->plant[i].LeafDeathRate;
		self->plant[i].RootNtransR = self->plant[i].NFRt * self->plant[i].NRelocLit * (self->plant[i].CFRtMass/1000.0); //why not: *pBio->fRootDeathRate
		self->plant[i].NRelocTotR = self->plant[i].LeafNtransR + self->plant[i].RootNtransR;

		libtreemix_CalcAssimilateAvailability(self,i, LeafDemand, RootDemand);	// eq. 85
	
		libtreemix_CalcNitrogenAvailability(self,i, LeafDemand, RootDemand);	// eq. 86
	
		libtreemix_CalcBiomassGrowth(self,i);
	}	
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
}

int libtreemix_CalcTotalMaintenanceRespiration(libtreemix *self, int i)
{
	/* Description */
	/*
	 * Calculates the Maintenance Respiration
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS	pBio = xpn->pPl->pBiomass;
	//PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	PWEATHER 	pWe  = xpn->pCl->pWeather;
	//PHLAYER		pHL  = xpn->pHe->pHLayer->pNext;
	
	/* Variables */	
	double hr = xpn->pTi->pSimTime->fTimeDay*24.0;		//actual hour of the day
	double dt = xpn->pTi->pTimeStep->fAct;
	double Tair = pWe->fTempAir;						//actual air Temperature
	double SinFun = 0.0;

	/* Functions */
	double libtreemix_CalcTempEffectWoodRespiration(libtreemix *self, int i, double Tair);
	double libtreemix_CalcTempEffectSoilRespiration(libtreemix *self, int i);
	double libtreemix_CalcDarkLeafRespirationTemperature(libtreemix *self, int i);

	//---------------------------------------------------------------------------------
	self->plant[i].kTResp		= 0.0;
	self->plant[i].kTRespNight	= 0.0;
	self->plant[i].kTRespSoil	= 0.0;
	
	self->clim.hr = hr;
	
	self->clim.SeasTime = (xpn->pTi->pSimTime->fTimeY+10.0)/365.25;
	//libtreemix_GetClimateValues(self, Tair, xpn->pCl->pWeather->fSolRad);
	if(!self->clim.Polar)
	{
		SinFun = sin(2.0*M_PI*self->clim.SeasTime-(M_PI/2.0));	// part of eq. 49, 54

		self->clim.DayLength = 12.0+(self->clim.hAmp/2.0)*SinFun;	// eq. 49
		self->clim.NightFrac = 1.0-(self->clim.DayLength/24.0);
	}
	
	

	/* Calculation Of The specific Temperature Effects On Respiration */
	/*----------------------------------------------------------------*/
	self->plant[i].kTResp 		= libtreemix_CalcTempEffectWoodRespiration(self, i, Tair);									// eq 36, [-]
	self->plant[i].kTRespSoil	= libtreemix_CalcTempEffectSoilRespiration(self, i);										// eq 37, [-]
	
	if((self->conf.Weather_Input == 1)||(self->conf.Weather_Input == 4))
	{
		self->plant[i].kTRespNight	= self->clim.NightFrac * self->plant[i].kTResp;				//leaf resp. (night fraction)
	}
	else if(self->conf.Weather_Input == 5)	// small time step calculation
	{		
		if(self->conf.PhotoResp == 1)	// daily time steps
		{
			if(self->conf.Photosynthesis == 1)
			{
				self->plant[i].kTRespNight = libtreemix_CalcTempEffectWoodRespiration(self, i, Tair);	// choose for bossel config
			}
			else if((self->conf.Photosynthesis == 2)||(self->conf.Photosynthesis == 3))
			{
				self->plant[i].kTRespNight = libtreemix_CalcTempEffectWoodRespiration(self, i, Tair)*self->clim.NightFrac;
			}
		}
		else if(self->conf.PhotoResp == 2)
		{
			// in night hours photosynthesis ceases
			// Thus, the respiration term also Leaf Respiration takes place only in night hours, for day it is included in the photosynthesis
			
			
			//if(((hr <= (12.0-(self->clim.DayLength/2.0))) || (hr >= (12.0+(self->clim.DayLength/2.0)))))
			if(pWe->fPAR < 0.01)
			{			
				self->plant[i].kTRespNight = self->plant[i].kTResp;	
			}
			else
			{
				self->plant[i].kTRespNight = 0.0;
				self->plant[i].kTResp_Bossel = 0.0;
				self->plant[i].kTResp_Falge = 0.0;
			}
		}
	}

	/* eq. 72, Specific Respiration Rates,  */
	/*--------------------------------------*/
	
	// leaves
	if(self->conf.Photosynthesis == 1)
	{
		// is being calculated in CanopyPhotosynthesis
		//self->plant[i].MainRespLf = self->plant[i].kTRespNight*self->plant[i].RespLf*(self->plant[i].CLfMass/1000.0); //*self->clim.NightFrac;
	}
	else if((self->conf.Photosynthesis == 2)||(self->conf.Photosynthesis == 3))
	{
		// For the Falge model, maintenance respiration is calculated in the photosynthesis model as it is integrated as Rd!
		
		// [tC/ha*yr]			  = µmol/m2/s												  * m2/m2										  * conversion factor
		//self->plant[i].MainRespLf = (self->plant[i].darkresp298 * self->plant[i].kTRespNight) * (self->plant[i].CrArPot * self->plant[i].LAI) * 3.786912;
		//self->plant[i].MainRespLf = 0.0; //self->plant[i].MainRespLfLay_R;
		
		// for output
		self->plant[i].MainRespLf_Bossel = self->plant[i].kTResp_Bossel*self->plant[i].RespLf*(self->plant[i].CLfMass/1000.0); //*self->clim.NightFrac;
		
		
		/* from [µmol/m2*s] (leaf) to [tC/ha*yr] (canopy), with temperature effect :
		 * 3.78432: 1[µmolC/m2*s] -> 12 µgC/m2*s -> 0.12 gC/ha*s -> 10368 gC/ha*d -> 3786912 gC/ha*yr -> 3.786 [tC/ha*yr]
		 */
		//old formulation, caused way to much respiration needs  and lead sooner or later to forest dieback
		//double dark_lf_resp_base = self->plant[i].darkresp298 * self->plant[i].totaltoprojm2factor;
		//self->plant[i].MainRespLf = self->plant[i].kTRespNight * dark_lf_resp_base * 3.786912 * self->plant[i].LAI * self->plant[i].CrArea;
		//self->plant[i].MainRespLf = self->plant[i].kTRespNight * dark_lf_resp_base * 3.786912 * self->plant[i].LAI * self->plant[i].CrArPot;		
		//self->plant[i].MainRespLf = self->plant[i].kTRespNight*self->plant[i].RespLf*(self->plant[i].CLfMass/1000.0); //*self->clim.NightFrac;
		
	}

	if(self->clim.T_soil < 0.0)
	{
		self->plant[i].MainRespLf = 0.0;
	}
	
	if(self->plant[i].LAI > 0.0)	
	{ 
		// calc. resp. of Leaf for Above Canopy Prod., factor 100: [tC/ha] -> [gC/m2]
		self->plant[i].SigmaC -= (100.0*(self->plant[i].MainRespLf / (self->plant[i].LAI*self->plant[i].CrArPot)/self->plant[i].CLfDens));
		if(self->plant[i].SigmaC < 0.0)
		{
			self->plant[i].SigmaC = 0.0;
		}
	}	
	
	// wood	
	self->plant[i].MainRespSt = self->plant[i].kTResp*self->plant[i].RespWd*self->plant[i].CWdResp;
	
	// fruit
	self->plant[i].MainRespGr = self->plant[i].kTResp*self->plant[i].RespFr*(self->plant[i].RespPeriodFr/12.0)*self->plant[i].CFrMass; 
	
	// (fine) roots
	self->plant[i].MainRespRt = self->plant[i].kTRespSoil*self->plant[i].RespRt*(self->plant[i].CFRtMass/1000.0);
	
	//Total Maintenance Respiration [tC/ha*yr], without Leaf Respiration, cause it is already subtracted in photosynthesis model!
	self->plant[i].MaintRespR = self->plant[i].MainRespSt + self->plant[i].MainRespGr + self->plant[i].MainRespRt;
	
	if(self->plant[i].CAss <= 0.0)
	{
		self->plant[i].MaintRespR = 0.0;	// no assimilates->no respiration
	}
	
	
	// Daily variables
	//============================================================================================================================
	if(NewDay(xpn->pTi))
	{
		self->plant[i].MaintResp__Day = 0.0;
		self->plant[i].MainRespLf_Day = 0.0;
		self->plant[i].MainRespRt_Day = 0.0;
		self->plant[i].MainRespSt_Day = 0.0;
	}
	self->plant[i].MainRespLf_Day += (self->plant[i].MainRespLf/365.25*dt);
	self->plant[i].MainRespRt_Day += (self->plant[i].MainRespRt/365.25*dt);
	self->plant[i].MainRespSt_Day += (self->plant[i].MainRespSt/365.25*dt);
	self->plant[i].MaintResp__Day += (self->plant[i].MaintRespR/365.25*dt);
	
	
	// Accumulated Respiration [tC/ha]
	//================================
	self->plant[i].MaintRespLf_Sum	+= self->plant[i].MainRespLf/365.25*dt;	
	self->plant[i].MaintRespRt_Sum 	+= self->plant[i].MainRespRt/365.25*dt;
	self->plant[i].MaintRespSt_Sum 	+= self->plant[i].MainRespSt/365.25*dt;
	self->plant[i].MaintResp_Sum 	+= self->plant[i].MaintRespR/365.25*dt;
	
	
	return RET_SUCCESS;
}

double libtreemix_CalcTempEffectWoodRespiration(libtreemix *self, int i, double Tair)
{	
	double kTResp = 0.0;
	//Tair = *(self->mean_temp);
	
	/* Calc. Temperature Effect On Respiration */
	if(self->conf.Photosynthesis == 1)
	{
		kTResp = pow((Tair-self->plant[i].TRespZero)/(self->plant[i].TRespRef-self->plant[i].TRespZero), 2);	// eq. 36	
	}
	else if((self->conf.Photosynthesis == 2)||(self->conf.Photosynthesis == 3))
	{
		kTResp = exp(self->plant[i].DeltaHaresp*(1.0/(IDGASKONST*298.0)-1.0/(IDGASKONST*(Tair+273.16))));	// Falge		
	}
	
	if((Tair < self->plant[i].TRespZero)||(Tair > 40.0))
	{
		kTResp = 0.0;
	}
	
	
	// for debugging
	self->plant[i].kTResp_Bossel = pow((Tair-self->plant[i].TRespZero)/(self->plant[i].TRespRef-self->plant[i].TRespZero), 2);	// eq. 36	
	self->plant[i].kTResp_Falge  = exp(self->plant[i].DeltaHaresp*(1.0/(IDGASKONST*298.0)-1.0/(IDGASKONST*(Tair+273.16))));	// Falge
	
	
	return kTResp;
}

double libtreemix_CalcTempEffectSoilRespiration(libtreemix *self, int i)
{
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	PHLAYER		pHL  = xpn->pHe->pHLayer->pNext;
	
	/* Variables */
	double z;			// soil layer looping variable
	double TRespirationSoil = 0.0;
	double TsoilLower; // = xpn->pHe->pHLayer->pNext->fSoilTemp;	//actual soil Temperature
	double Tair =  xpn->pCl->pWeather->fTempAir;	// act air temp.
	//double Tair = *(self->mean_temp);				// mean air temp.
	
	/* Compute soil temperature according to original source code */
	for(z=1; z<=xpn->pSo->iLayers-2; z++)
	{
		TsoilLower = pHL->fSoilTemp;
	
		pHL = pHL->pNext;	
	}
	self->clim.T_soil = self->plant[i].damp_T*Tair+(1.0-self->plant[i].damp_T)*xpn->pCl->pAverage->fYearTemp;	//TsoilLower;

	/* calc. temperature effect on soil respiration */
	if(self->conf.Respiration == 1)
	{
		if((self->clim.T_soil < self->plant[i].TSoilZero)||(self->clim.T_soil > 40.0))
		{
			TRespirationSoil = 0.0;
		}
		else
		{
			TRespirationSoil = pow((self->clim.T_soil-self->plant[i].TSoilZero)/(self->plant[i].TSoilRef-self->plant[i].TSoilZero), 2);	// eq. 37
		}
	}
	else if(self->conf.Respiration == 2)
	{
		if(self->clim.T_soil < 0.0)
		{
			TRespirationSoil = self->plant[i].TSoilZero;
		}
		else
		{
			TRespirationSoil = self->plant[i].TSoilZero+self->plant[i].TSoilRef*pow(self->clim.T_soil, 2);
		}
	}
	return TRespirationSoil;
}

int libtreemix_CalcLeafRenewalDemand(libtreemix *self, int i, double* LeafDemand)
{
	/* Description */
	/*
	 * is a function of the loss rate, flush rate and current leaf goal discrepancies
	 * Calculating Leaf and Root Renewal, BOSSEL, 1996, chapter 3.8
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS	pBio = xpn->pPl->pBiomass;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	PWEATHER 	pWe  = xpn->pCl->pWeather;
	
	/* Variables */
	//int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double RedLAI;
	double Day = xpn->pTi->pSimTime->fTimeY;
	
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	/* Calculating reduced LAI from LAImaxH */
	if((self->conf.Tree_Geometry == 1)||(self->conf.Tree_Geometry == 2))
	{
		RedLAI = 1.0;
	}
	else
	{
		RedLAI = MIN((self->plant[i].H/self->plant[i].LAImaxH), 1);	// LAImaxH is not set
	}
	
	if(self->conf.Tree_Water == 0)	// no treewater model
	{
		// linear reduction in what leaf lim should have: LeafLim  = pBaldocchiPhotosynthesis->get_canopyStressFactor() * CronAr* LAImax * CLeaf/100.0 * RedLAI;
		self->plant[i].CLfLim = self->plant[i].CrArPot*self->plant[i].LAImax*(self->plant[i].CLfDens/100.0)*RedLAI;	// [tC/ha]
	}
	else if(self->conf.Tree_Water == 1)	// endejan???
	{	// same as above, but with stress factor
		// LeafLim  = CronAr * pPhotoprodPar->LAImax * CLeaf/100.0 * RedLAI;
		self->plant[i].CLfLim = self->plant[i].CrArPot*self->plant[i].LAImax*(self->plant[i].CLfDens/100.0)*RedLAI*self->plant[i].StressFacLf;	// [tC/ha]
	}
	self->plant[i].CLfLim_kg = self->plant[i].CLfLim*1000.0;	// for output [kgC/ha]
	
	/* calc. leaf loss rates and leaf demand based on phenology (bud burst) model: */
	if(self->conf.Phenology == 1)
	{
		if(self->clim.Tropical == TRUE)
		{
			self->plant[i].LeafDeathRate = (self->plant[i].TOLf*(1+self->plant[i].DamageLf)*(self->plant[i].CLfMass/1000.0));		// eq. 73
			*LeafDemand = self->plant[i].LeafDeathRate + (self->plant[i].CLfLim -(self->plant[i].CLfMass/1000.0))*self->plant[i].LfFlush;		// eq. 75
		}
		else
		{
			/* Seasonal Conditions */
			if(pWe->fTempAir < self->plant[i].T_veg) // cold season
			{				
				*LeafDemand = 0.0;	// eq. 79
				
				// eq. 78, 
				self->plant[i].LeafDeathRate = (self->plant[i].CLfMass/1000.0 - self->plant[i].CLfOld*(1.0-self->plant[i].TOLf))
											   *self->plant[i].LfFall*(1.0+self->plant[i].DamageLf);
			}			
			else	// vegetative season
			{				
				self->plant[i].LeafDeathRate = 0.0;	// eq. 80
				
				*LeafDemand = self->plant[i].LfFlush*(self->plant[i].CLfLim-(self->plant[i].CLfMass/1000.0));	// eq. 81
				
                //printf("%f \n", *LeafDemand);
				// for next time step
				self->plant[i].CLfOld = self->plant[i].CLfMass/1000.0;	// calculating the new 'LfOld' for next time step
			}
			
			if(self->plant[i].LeafDeathRate < 0.0)
			{
				self->plant[i].LeafDeathRate = 0.0;				
			}
		}
	}
	else if((self->conf.Phenology == 2)||(self->conf.Phenology == 3))
	{
		if((self->silv[i].Dormancy == FALSE)&&(Day <= self->plant[i].LfFallStart)&&(Day >= self->plant[i].LfFallEnd))
		{
			if(!strcmp(self->plant[i].type, "deciduous"))	//deciduous stand
			{ 				
                self->plant[i].LeafDeathRate = 0.0;
			}
			else
			{
				self->plant[i].LeafDeathRate = (self->plant[i].CLfMass/1000.0)*self->plant[i].TOLf;
			}
			*LeafDemand = self->plant[i].LfFlush*(self->plant[i].CLfLim-(self->plant[i].CLfMass/1000.0));	// eq. 81
		}
		else
		{
			if(!strcmp(self->plant[i].type, "deciduous"))	//deciduous stand
			{ 				
                self->plant[i].LeafDeathRate = (self->plant[i].CLfMass/1000.0)*self->plant[i].LfFall*(1.0+self->plant[i].DamageLf);
			}
			else
			{
				self->plant[i].LeafDeathRate = (self->plant[i].CLfMass/1000.0)*self->plant[i].TOLf;
			}
			*LeafDemand = 0.0;
		}
	}
	
	/*====================conversion to timestep dependent!!!===================================*/
	//*LeafDemand = *LeafDemand/365.0*dt; 
	self->plant[i].CLfDemand = *LeafDemand/365.25*dt;
	//self->plant[i].LeafDeathRate = (self->plant[i].LeafDeathRate/365.0*dt);		//[tC/ha*timestep]
	
	return RET_SUCCESS;
}

int libtreemix_CalcRootRenewalDemand(libtreemix *self, int i, double* RootDemand)
{
	/* Description */
	/*
	 * Computes fine root losses and renewal demand, differentiating between 
	 * all-year (tropical) and seasonal vegetation
	 * equation 82-84, BOSSEL, 1996
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS	pBio = xpn->pPl->pBiomass;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	//PWEATHER 	pWe  = xpn->pCl->pWeather;
	//PHLAYER pHL = xpn->pHe->pHLayer->pNext;
	
	/* Variables */
	//int i;	// species looping variable
	//int z;		// soil layer looping variable
	//int layers = 0;	// layer count of soil profile
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	//double Tsoil = 0.0;
		
	/*Functions*/
	
	/****************************************************************************************************************/	
	// eq. 82, [tC/ha*yr], no seas. distinction is made for fine root loss
	self->plant[i].RootDeathRate = self->plant[i].TORt*(1.0+self->plant[i].DamageFRt)*(self->plant[i].CFRtMass/1000.0);

	if(self->clim.T_soil >= self->plant[i].TRootZero)
	{
		*RootDemand = self->plant[i].RootDeathRate+(self->plant[i].LfFlush*((self->plant[i].CLfLim/self->plant[i].LfFRt)-(self->plant[i].CFRtMass/1000.0))); // eq. 83
	}
	else
	{
		*RootDemand = 0.0;	// eq. 84
	}
	
	
	/*====================conversion to timestep dependent!!!===================================*/
	//*RootDemand = *RootDemand/365.0*dt; 
	self->plant[i].CRtDemand = *RootDemand/365.25*dt;	// [tC/ha]
	//self->plant[i].RootDeathRate = (self->plant[i].RootDeathRate/365.0*dt);	//[tC/ha*timestep]
	
	return RET_SUCCESS;
}

int libtreemix_CalcAssimilateAvailability(libtreemix *self, int i, double LeafDemand, double RootDemand)
{
	/* Description */
	/*
	 * Rate at which assimilated Carbon is available
	 * 
	*/
	
	/* Abbreviations */
	//expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS	pBio = xpn->pPl->pBiomass;
	//PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	//PWEATHER 	pWe  = xpn->pCl->pWeather;
	
	/* Variables */
	//int i;	// species looping variable
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double AssiPotSupplyRate;	// rate at which assimilate becomes available [tC ha-1 yr-1]
	double CLfRtTotalDemand;	// rate at which assimilate is required for leaf and fine root demand [tC ha-1 yr-1]
		
	/*Functions*/
	
	/****************************************************************************************************************/
	
	// eq. 85, denominator, assimilate rate which is required for leaf and fine root growth
	CLfRtTotalDemand = self->plant[i].RespGr*(LeafDemand + RootDemand);
	if(CLfRtTotalDemand < EPSILON)
	{
		CLfRtTotalDemand = EPSILON;
	}
	//CLfRtTotalDemand = CLfRtTotalDemand/365.0*dt;
	self->plant[i].CLfRtTotDemand = CLfRtTotalDemand;
	
	// eq. 91, fruit loss rate
	self->plant[i].FruitDeathRate = self->plant[i].CFrMass * self->plant[i].TOFr;	
	
	// eq. 98, assimilate relocation before leaf and fine root abscission, [tC/ha*yr]
	self->plant[i].CRelocGain = self->plant[i].CReloc*(self->plant[i].LeafDeathRate + self->plant[i].RootDeathRate);
	
	// eq. 85, numerator [tC/ha*yr]
	AssiPotSupplyRate = (((self->plant[i].CAss/1000.0)-self->plant[i].CAssReserve)*self->plant[i].CUseSpec)
						+(self->plant[i].PhCanopyNlim - self->plant[i].MaintRespR);
	//AssiPotSupplyRate = AssiPotSupplyRate/365.0*dt;	
	self->plant[i].CAssSupply = AssiPotSupplyRate;
	
	self->plant[i].AssimilateBalance = AssiPotSupplyRate - CLfRtTotalDemand;	
	
	// eq. 85, kA [-]
	self->plant[i].kA = AssiPotSupplyRate/CLfRtTotalDemand;	//NOTE: the construction eff. fac. = 1/PRATES->RespGr!	
	if(self->plant[i].kA < 0.0)
	{
		self->plant[i].kA = 0.0;
	}
	else if(self->plant[i].kA>1.0)
	{
		self->plant[i].kA = 1.0;
	}
	
	return RET_SUCCESS;
}

int libtreemix_CalcNitrogenAvailability(libtreemix *self, int i, double LeafDemand, double RootDemand)
{
	/* Description */
	/*
	 * Rate at which assimilated Carbon is available
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS	pBio = xpn->pPl->pBiomass;
	//PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	//PWEATHER 	pWe  = xpn->pCl->pWeather;
	
	/* Variables */
	//int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
		
	/*Functions*/
	double libtreemix_NitrogenUptakeCERES(libtreemix *self, int i);
	
	/****************************************************************************************************************/
	
	/* eq. 86, denominator, [tC/ha*yr] */
	self->plant[i].LeafDemandRate = self->plant[i].kA*self->plant[i].NLf*LeafDemand;
	self->plant[i].RootDemandRate = self->plant[i].kA*self->plant[i].NFRt*RootDemand;
	self->plant[i].TotalDemandRate = self->plant[i].LeafDemandRate + self->plant[i].RootDemandRate;	// [tN/ha*yr]

	/* numerator */
	if(self->conf.Soil_Processes == 1)
	{
		self->plant[i].NUpLfRtR = self->plant[i].NUpSpecR*(self->plant[i].NSoil/1000.0)*(self->plant[i].CFRtMass/1000.0);
	}
	else if(self->conf.Soil_Processes == 2)
	{
		self->plant[i].NUpLfRtR = (libtreemix_NitrogenUptakeCERES(self, i)*365.25/1000.0); //* self->plant[i].TreeDistr;	// [kgN/ha*d]->[tN/ha*yr]
	}
	
	// [kgN/ha*timestep]
	//self->plant[i].NUpLfRtR = self->plant[i].NUpLfRtR*dt;		
	
	// kNLfRt [-]
	if(self->plant[i].TotalDemandRate > EPSILON)
	{
		self->plant[i].kNLfRt = self->plant[i].NUpLfRtR/self->plant[i].TotalDemandRate;
		if(self->plant[i].kNLfRt > 1.0)
		{
			self->plant[i].kNLfRt = 1.0;
		}
		else if(self->plant[i].kNLfRt < 0.0)
		{
			self->plant[i].kNLfRt = 0.0;
		}
	}
	else
	{
		self->plant[i].kNLfRt = 0.0;
	}
	
	if(NewDay(xpn->pTi))
	{		
		self->plant[i].NUpLfRt_Day = 0.0;
	}
	
	// [tN/ha]
	self->plant[i].NUpLfRt_Day += (self->plant[i].NUpLfRtR/365.25*dt);
	self->plant[i].NUpLfRt_Sum += (self->plant[i].NUpLfRtR/365.25*dt);
	
	return RET_SUCCESS;
}

double libtreemix_NitrogenUptakeCERES(libtreemix *self, int i)
{
	/* Description */
	/*
	 * Nitrogen Uptake from the Soil Layers
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	PSLAYER pSL		= xpn->pSo->pSLayer->pNext;
	PSWATER	pSWL	= xpn->pSo->pSWater->pNext;
	PWLAYER	pSLW	= xpn->pWa->pWLayer->pNext; 
	PCLAYER pSLN	= xpn->pCh->pCLayer->pNext;
	PLAYERROOT	 pLR	= xpn->pPl->pRoot->pLayerRoot->pNext;
	//PBIOMASS	 pBio	= xpn->pPl->pBiomass;
	//PPLTNITROGEN pPlN	= xpn->pPl->pPltNitrogen;

	/* Variables */
	int L, L1;
	double FNO3 = 0.0;			// Nitrogen Availability Factor
	double FNH4 = 0.0;			// Nitrogen Availability Factor
	double SMDF;				// Soil Moisture Deficite Factor: affecting Nitrogen Uptake at low Soil Water Levels
	double RFAC;				// Effect of Water Stress and Root Length Density on Potential Nitrogen Uptake
	double SNO3min, SNH4min;	// Lower Limits of extractable nitrogen
	double TRNU;				// Total Root Nitrogen Uptake [kgN/ha]
	
	//double dt = xpn->pTi->pTimeStep->fAct;
	
	/* Function */
	
	/****************************************************************************************************************/
	/*----------------------------------------------*/
	/* Layer Nitrogen Initiation and Transformation */
	/*----------------------------------------------*/
	pLR	= xpn->pPl->pRoot->pLayerRoot->pNext;
	for(L=1; L <= xpn->pSo->iLayers-2; L++)
	{
		self->plant[i].NO3Up[L] = 0.0;
		self->plant[i].NH4Up[L] = 0.0;

		self->plant[i].ActLayNO3NUpt[L] = 0.0;
		self->plant[i].ActLayNH4NUpt[L] = 0.0;

		//pLR->fActLayNO3NUpt = 0.0;
		//pLR->fActLayNH4NUpt = 0.0;

		pLR = pLR->pNext;
	}
	
	/*---------------------------*/
	/* Potential Nitrogen Uptake */
	/*---------------------------*/
	TRNU = 0.0;

	pSL		= xpn->pSo->pSLayer->pNext;
	pSWL	= xpn->pSo->pSWater->pNext;
	pSLW	= xpn->pWa->pWLayer->pNext; 
	pSLN	= xpn->pCh->pCLayer->pNext;
	pLR		= xpn->pPl->pRoot->pLayerRoot->pNext;
	for(L=1; L <= xpn->pSo->iLayers-2; L++)
	{
		/* Conversion factors [kgN/ha] -> [mgN/kgSoil]
		 * fBulkDens  [kg/dm³]
		 * fThickness [mm] 
		 */
		 
		//pSLN->fNO3Nmgkg = (float)(0.01*pSL->fBulkDens*pSL->fThickness);
		//pSLN->fNH4Nmgkg = (float)(0.01*pSL->fBulkDens*pSL->fThickness);
		self->plant[i].NO3Nmgkg[L] = 0.01*pSL->fBulkDens*pSL->fThickness;
		self->plant[i].NH4Nmgkg[L] = 0.01*pSL->fBulkDens*pSL->fThickness;
		
		// check whether there are roots in this layer:
		if(self->plant[i].RtLengthDens[L-1] == 0.0){
			break;
		}
		L1 = L;
		
		// potential Nitrogen Availability Factor for NO3 and NH4; [-]
		FNO3 = 1.0-exp(-0.3*(pSLN->fNO3N/self->plant[i].NO3Nmgkg[L]));		
		FNH4 = 1.0-exp(-0.3*((pSLN->fNH4N/self->plant[i].NH4Nmgkg[L])-0.5));
				
		if(FNO3 < 0.003){
			FNO3 = 0.0;
		}
		else if(FNO3 > 1.0){
			FNO3 = 1.0;
		}		
		if(FNH4 < 0.003){
			FNH4 = 0.0;
		}
		else if(FNH4 > 1.0){
			FNH4 = 1.0;
		}

		// Soil Moisture Deficite Factor: affecting Nitrogen Uptake at low Soil Water Levels; [-]
		if(pSLW->fContAct < pSWL->fContFK)
		{
			SMDF = (pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		}
		else
		{
			SMDF = (pSWL->fContSat-pSLW->fContAct)/(pSWL->fContSat-pSWL->fContFK);
		}
		if(SMDF < 0.0){
			SMDF = 0.0;
		}
		else if(SMDF > 1.0){
			SMDF = 1.0;
		}
		
		if(L==1)
		{
			//C_DEBUG(SMDF)
		}

		// Effect of Water Stress and Root Length Density on Potential Nitrogen Uptake
		// RtlengthDens:	[cm/cm2]
		// fThickness:		[mm]*0.1=[cm]
		// 100.0: conversion of units later
		// Diss. Lenz; eq2.2.7-5
		RFAC = self->plant[i].RtLengthDens[L-1]*0.1*pSL->fThickness*100.0;
		
		/* maximum potential nitrogen uptake for the actual timestep, depending on the tree [kgN/ha*d] */
		//PRATES->NO3Up[L] = (double)(RFAC*FNO3*pPl->pGenotype->fMaxNuptRate);	
		//PRATES->NH4Up[L] = (double)(RFAC*FNH4*pPl->pGenotype->fMaxNuptRate);
		self->plant[i].NO3Up[L] = RFAC*FNO3*self->plant[i].NUpMax*SMDF;
		self->plant[i].NH4Up[L] = RFAC*FNH4*self->plant[i].NUpMax*SMDF;
		
		// Lower Limits of extractable nitrogen in layer L, [kgN/ha]
		SNO3min	= 0.25*self->plant[i].NO3Nmgkg[L];
		SNH4min	= 0.50*self->plant[i].NH4Nmgkg[L];

		/* Actual Plant Uptake from Layer L, [kgN/ha*d] */
		if(self->plant[i].NO3Up[L] > pSLN->fNO3N-SNO3min)
		{
			self->plant[i].NO3Up[L] = max(0.0, pSLN->fNO3N-SNO3min);
		}

		if(self->plant[i].NH4Up[L] > pSLN->fNH4N-SNH4min)
		{
			self->plant[i].NH4Up[L] = max(0.0, pSLN->fNH4N-SNH4min);
		}

		//Total Root Nitrogen Uptake: fTRNU [kgN/ha*d] dependent on tree distribution(!)
		TRNU += max(0.0, (self->plant[i].NO3Up[L]+self->plant[i].NH4Up[L]));

		pSL		= pSL->pNext;
		pSWL	= pSWL->pNext;
		pSLW	= pSLW->pNext;
		pSLN	= pSLN->pNext;
		pLR		= pLR->pNext;
	}

	return TRNU;	// [kgN/ha*d]
}

int libtreemix_CalcBiomassGrowth(libtreemix *self, int i)
{
	/* Description */
	/*
	 * Computes the possible Growth of leaves and fine roots, fruits and increment
	 * on the basis of available assimilates and nitrogen
	 * using an allocation hierarchy
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTCARBON		pPlC = xpn->pPl->pPltCarbon;
	PPLTNITROGEN 	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	//PWEATHER 	pWe  = xpn->pCl->pWeather;
	
	/* Variables */
	//int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double CLfGrowth;	// assimilate use for leaf growth [tC/ha*yr]
	double CRtGrowth;	// assimilate use for root growth [tC/ha*yr]
	double CDemandFr;	// assimilate demand of fruit [tC/ha*yr]
	double CLfRtConstr;	// assimilate drain for leaf and root construction [tC/ha*yr]
	double CFrConstr;	// assimilate drain for fruit construction [tC/ha*yr]
	double CIncrConstr;	// assimilate drain for increment [tC/ha*yr]
	double CSurplus;	// assimilate surplus supply rate [tC/ha*yr]
	double NSurplus;  	// nitrogen surplus supply rate [tN/ha*yr]
	double NDemandFr;	// nitrogen demand rate for fruit growth [tN/ha*yr]
	double NDemandIncr;	// nitrogen demand rate for increment [tN/ha*yr]
	double PFruitN;
	double PFruitC;
	double PIncrN;
	double PIncrC;
		
	/*Functions*/
	
	/****************************************************************************************************************/
	
	/* calc. amount of increased nitrogen due to uptake, [tN/ha*yr] */
	self->plant[i].NIncrLeaf = self->plant[i].LeafDemandRate*self->plant[i].kNLfRt;
	self->plant[i].NIncrRoot = self->plant[i].RootDemandRate; //*self->plant[i].kNLfRt; this is the original formulation as in eq. 89 (not nitrogen affected)

	/* calc. actual leaf and root renewal rate [tC/ha*yr] */
	if(self->plant[i].CAss > 0.0)
	{
		//		[tC/ha*yr] 	 = 			[tN/ha*yr]	   /		[tN/tC]
		self->plant[i].LfGrR = self->plant[i].NIncrLeaf/self->plant[i].NLf;
		self->plant[i].RtGrR = self->plant[i].NIncrRoot/self->plant[i].NFRt;
	}
	else
	{
		self->plant[i].LfGrR = 0.0;
		self->plant[i].RtGrR = 0.0;
	}

	/* calc. assimilate use for leaf and root grwoth, eq. 92, [tC/ha*yr] */
	CLfGrowth = self->plant[i].LfGrR*self->plant[i].RespGr;
	CRtGrowth = self->plant[i].RtGrR*self->plant[i].RespGr;
	// assimilate drain for leaf and root construction
	CLfRtConstr = CLfGrowth+CRtGrowth;
	self->plant[i].CLfRtConstr = CLfRtConstr;
	
	/* calc. the actualized amount of nitrogen */
	self->plant[i].NIncrLeaf = self->plant[i].NLf*self->plant[i].LfGrR;
	self->plant[i].NIncrRoot = self->plant[i].NFRt*self->plant[i].RtGrR;
	
	/* nitrogen rate available for increment, eq. 93, [tN/ha*yr] */
	NSurplus = (self->plant[i].NUpLfRtR - self->plant[i].NIncrLeaf - self->plant[i].NIncrRoot);	
	if(NSurplus < 0.0){
		NSurplus = 0.0;
	}
	self->plant[i].NSurplus = NSurplus*365.25*dt;	// [tN/ha]
	
	/* assimilate rate available for increment, eq. 92, [tC/ha*yr] */
	self->plant[i].TOLfMass = self->plant[i].TOLf*self->plant[i].CLfMass/1000.0;
	self->plant[i].TORtMass = self->plant[i].TORt*self->plant[i].CFRtMass/1000.0;
	
	
	CSurplus = ((self->plant[i].CAss/1000.0) - (self->plant[i].TOLfMass + self->plant[i].TORtMass))* self->plant[i].CUseSpec - self->plant[i].CLfRtConstr;
	
	if(CSurplus < 0.0)
	{
		CSurplus = 0.0;
	}
	// [kgC/ha]
	self->plant[i].CSurplus = 1000.0*CSurplus/365.25*dt;
	
	// determine possible fruit growth
	self->plant[i].FrGrR = 0.0;
	CFrConstr = 0.0;
	self->plant[i].NIncrFruit = 0.0;
	NDemandFr = 0.0;
	if((NSurplus>0)&&(CSurplus>0))
	{
		CDemandFr = (self->plant[i].TOFr*self->plant[i].FiFrLf*(self->plant[i].CLfMass/1000.0)*self->plant[i].RespGr);
		if(CSurplus < CDemandFr){
			CDemandFr = CSurplus;
		}

		NDemandFr = (self->plant[i].NFr*self->plant[i].FiFrLf*(self->plant[i].CLfMass/1000.0)*self->plant[i].TORt);
		if(NSurplus<NDemandFr){
			NDemandFr = NSurplus;
		}

		/* determine the limiting factor (nitrogen or assimilates) */
		PFruitN = NDemandFr/self->plant[i].NFr;
		PFruitC = CDemandFr/self->plant[i].RespGr;
		if(PFruitN<PFruitC){
			self->plant[i].FrGrR = PFruitN;
		}
		else{
			self->plant[i].FrGrR = PFruitC;
		}
		CFrConstr = self->plant[i].FrGrR*self->plant[i].RespGr;
		self->plant[i].NIncrFruit = self->plant[i].FrGrR*self->plant[i].NFr;

		/* calculate remaining surplusses of C and N */
		NSurplus -= self->plant[i].NIncrFruit;
		if(NSurplus<0)
		{
			NSurplus = 0.0;
		}
		
		CSurplus -= CFrConstr;
		if(CSurplus<0)
		{
			CSurplus = 0.0;
		}
	}

	/* calculation of wood increment if: C-, N- Surplus and vegetative season, [tC/ha*yr] */
	self->clim.SeasTime = (xpn->pTi->pSimTime->fTimeY+10.0)/365.25;
	if((self->clim.SeasTime>0.3)&&(self->clim.SeasTime<0.8)&&(CSurplus>0.0)&&(NSurplus>0.0))
	{
		// increment under assimilate limitation or nitrogen limitation
		/* eq. 94*/ PIncrC = CSurplus/self->plant[i].RespGr;															
		/* eq. 95*/ PIncrN = NSurplus/((self->plant[i].b*self->plant[i].NWdResp)+((1.0-self->plant[i].b)*self->plant[i].NWdDead));		
		if(PIncrN < PIncrC){
			self->plant[i].Increment = PIncrN;
			self->plant[i].IncrLimitation = 1.0;
		}
		else{
			self->plant[i].Increment = PIncrC;
			self->plant[i].IncrLimitation = 0.0;
		}

		CIncrConstr = self->plant[i].Increment*self->plant[i].RespGr;
		
		// see eq. 100, second last part
		self->plant[i].NIncrStem = self->plant[i].Increment*((self->plant[i].b *self->plant[i].NWdResp)+((1.0-self->plant[i].b)*self->plant[i].NWdDead));
	}
	else
	{
		// no increment out of season
		self->plant[i].Increment 	= 0.0;
		CIncrConstr 				= 0.0;
		self->plant[i].NIncrStem 	= 0.0;
		PIncrC 						= 0.0;
		PIncrN 						= 0.0;
	}
	self->plant[i].CIncrConstr = CIncrConstr;

	/* total assimilate demand is the sum of the individual growth rates divided by growth efficiency, eq. 99, [tC/ha*yr] */
	self->plant[i].IncrementTotal = self->plant[i].NIncrLeaf + self->plant[i].NIncrRoot + self->plant[i].NIncrFruit + self->plant[i].NIncrStem;
	self->plant[i].NUpActR = max(0.0, (self->plant[i].IncrementTotal - self->plant[i].NRelocTotR));
	
	// total nitrogen relocation can only be at max as the sum of the different nitrogen demands for growth of the tree components
	if(self->plant[i].NRelocTotR > self->plant[i].IncrementTotal){
		self->plant[i].NRelocTotR = self->plant[i].IncrementTotal;
	}
	
	/* Total nitrogen uptake as sum of the different n-demands, diminished by the nitrogen relocation rate, eq. 100, [tN/ha*yr] */	
	// warum wird hier PCIncrC genommen und nicht das berechnete increment aus der limitation? (94 u.95)
	NDemandIncr = PIncrC * ((self->plant[i].b*self->plant[i].NWdResp)+((1.0-self->plant[i].b)*self->plant[i].NWdDead));
	//NDemandIncr = self->plant[i].Increment * ((self->plant[i].b*self->plant[i].NWdResp)+((1.0-self->plant[i].b)*self->plant[i].NWdDead));
	
	self->plant[i].NUpPotR = self->plant[i].LeafDemandRate + self->plant[i].RootDemandRate + NDemandFr + NDemandIncr - self->plant[i].NRelocTotR;
	if(NewDay(xpn->pTi))
	{
		self->plant[i].NUpPot_Day = 0.0;
	}
	self->plant[i].NUpPot_Day += self->plant[i].NUpPotR*dt;

	// Calc. Stress Factors
	if((self->plant[i].NUpLfRtR - self->plant[i].NUpActR) >= 0.0)
	{
		self->plant[i].kN = 1.0;
		pPlN->fNStressPhoto = (float)1.0;
	}
	else
	{
		if(self->plant[i].NUpActR > EPSILON)
		{
			self->plant[i].kN = self->plant[i].NUpLfRtR/self->plant[i].NUpActR;
		}
		else
		{
			self->plant[i].kN = 0.0;
		}
		pPlN->fNStressPhoto = (float)self->plant[i].kN;
		
	}

	self->plant[i].CGrTot = CLfRtConstr + CFrConstr + CIncrConstr;		// [tc/ha*yr]
	
	return RET_SUCCESS;
}
