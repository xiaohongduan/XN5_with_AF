#include "libtreemix.h"
#include <math.h>




int libtreemix_potential_transpiration_run(libtreemix *self)
{
	/* Description */
	/*
	 * Calculates the potential transpiration 
	 * from the water balance in dependence of
	 * of the plants LAI
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	PPLTWATER pPW = xpn->pPl->pPltWater;
	
	/* Variables */
	int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double PreciRate_temp;
	
	/*Functions*/
	int libtreemix_Rutter_Model(libtreemix *self, int i);
	/****************************************************************************************************************/
	
	PreciRate_temp = 0.0;
	
	pPW->fInterceptR = 0.0;
	pPW->fInterceptdt = 0.0;
	pPW->fPotTranspR = 0.0;
	pPW->fPotTranspdt = 0.0;
	
	self->clim.BulkPrecR = xpn->pCl->pWeather->fBulkPreciRate;
	self->clim.LiquPrecR = xpn->pCl->pWeather->fLiquPreciRate;

	/* Potential Transpiration and Rainfall Interception for actual timestep */
	/* ===================================================================== */
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if((self->silv[i].PlantGrowth == 1) && (self->silv[i].Maturity == 0))
		{
			self->plant[i].PotTrans = 0.0;
			
			if(self->plant[i].LAI > 0.3)
			{
				// Rainfall Interception
				//-----------------------------------------
				if(self->conf.Rainfall_Interception == 1)
				{
					libtreemix_Rutter_Model(self, i);
					
					// Canopy precipitation
					PreciRate_temp += MAX(0.0, (self->plant[i].ThroughfallR*self->plant[i].TreeDistr));
					if(PreciRate_temp < 0.0)
					{
						PreciRate_temp = 0.0;
					}
					
					// XN-variables
					pPW->fInterceptR   += (self->plant[i].InterceptionR*self->plant[i].TreeDistr);
					pPW->fInterceptdt  += (self->plant[i].Interception_dt*self->plant[i].TreeDistr);
				}
				else
				{
					self->plant[i].InterceptionR = 0.0;
				}
				
				
				
				// Potential Transpiration
				//--------------------------------------------------------------------
				if((self->conf.Photosynthesis == 2)||(self->conf.Photosynthesis == 3))
				{
					// potential transpiration from Falge model (Photosynthesis)
					self->plant[i].PotTrans = self->plant[i].TransCan_dt/dt;
				}
				else
				{
					// potential transpiration from Penman-Monteith model (Evapotranspiration)
					self->plant[i].PotTrans = MAX(0.0, xpn->pWa->fPotETR - xpn->pWa->pEvap->fPotR);
				}				
				
				pPW->fPotTranspR += self->plant[i].PotTrans*self->plant[i].TreeDistr;	// for whole stand
			}
			else
			{
				self->plant[i].InterceptionR = 0.0;
				self->plant[i].PotTrans = MAX(0.0, xpn->pWa->fPotETR - xpn->pWa->pEvap->fPotR);
				pPW->fPotTranspR += self->plant[i].PotTrans*self->plant[i].TreeDistr;	// for whole stand
			}
		}
		else
		{
			xpn->pPl->pDevelop->bPlantGrowth = 0;
			xpn->pPl->pDevelop->bMaturity = 1;
			
			self->silv[i].PlantGrowth = 0;
			self->silv[i].Maturity = 1;		
			
			self->plant[i].InterceptionR = 0.0;
			pPW->fPotTranspR = 0.0;			
			self->plant[i].PotTrans = 0.0;			
		}
		
		// Potential transpiration (time step)
		self->plant[i].PotTransdt = self->plant[i].PotTrans*dt;
		
		pPW->fPotTranspdt  += (self->plant[i].PotTransdt*self->plant[i].TreeDistr);
		
	}
	
	self->clim.CanPreciRate = PreciRate_temp;
	self->clim.CanPreci_dt 	= PreciRate_temp/dt;
	self->clim.CanPreci_Sum += self->clim.CanPreci_dt;
	
	self->clim.Preci_Sum += (xpn->pCl->pWeather->fBulkPreciRate*dt);
	
	
	/* DAILY VARIABLES */
	/*******************/	
	if(NewDay(xpn->pTi))
	{
		for(i=0; i<self->conf.Species_Count; i++)
		{
			self->plant[i].PotTransDay = 0.0;
		}
		
		pPW->fInterceptDay = 0.0;
		pPW->fPotTranspDay = 0.0;		
		self->clim.CanPreci_Day = 0.0;
	}
	
	// complete canopy precipitation
	self->clim.CanPreci_Day += self->clim.CanPreci_dt;	
	
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		// interception
		pPW->fInterceptDay += (self->plant[i].Interception_dt*self->plant[i].TreeDistr);

		// pot. transpiration
		self->plant[i].PotTransDay += self->plant[i].PotTransdt;
		pPW->fPotTranspDay += (self->plant[i].PotTransDay*self->plant[i].TreeDistr);
	}	
	
	
	if(self->conf.Rainfall_Interception == 1)
	{		
		self->clim.PreciRateControl = self->clim.CanPreciRate + pPW->fInterceptR;
		self->clim.ThroughfallR = self->clim.CanPreciRate;
		self->clim.InterceptR	= pPW->fInterceptR;
		
		/* PreciRate AND LiquPreciRate = Canopy Precipitation [mm/d], rate */
		xpn->pCl->pWeather->fPreciRate 		= self->clim.CanPreciRate;
		xpn->pCl->pWeather->fLiquPreciRate 	= self->clim.CanPreciRate;

	}
	else
	{
		self->clim.ThroughfallR	= xpn->pCl->pWeather->fBulkPreciRate;
		
	}
	
	
	return RET_SUCCESS;
};

int libtreemix_actual_transpiration_run(libtreemix *self)
{
	/* Description */
	/*
	 * Calculates the actual transpiration according to 
	 * JONES & KINIRY, eds, 1986, RITCHIE & GODWIN 2000 => CERES-model,
	 * (see Dissertation Lenz, 2007 Uni Koeln, Victoria Ilse Shamim Lenz)
	 * by determining the potential and actual root water uptake
	 * of the plant
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn	= &(self->parent);
	
	PLAYERROOT 	pLR		= xpn->pPl->pRoot->pLayerRoot->pNext;
	PPLTWATER	pPW		= xpn->pPl->pPltWater;
	PROOT		pR		= xpn->pPl->pRoot;
	PSLAYER 	pSL		= xpn->pSo->pSLayer;
	PSWATER		pSWL	= xpn->pSo->pSWater;
	PWLAYER	   	pSLW	= xpn->pWa->pWLayer;
	PPLANT		pPl		= xpn->pPl;
	
	
	/* Variables */
	int i;					//species looping variable
	int L;					//soil layer looping variable
	
	double PotRtWaterUp;	// Total Potential Root Water Uptake
	double ActRtWaterUp_P;	// Total Actual Root Water Uptake in Profile
	double ActRtWaterUp_L;	// Total Actual Root Water Uptake from summed Layers
	double SoilWaterTot;	//	
	double SoilWaterAct;	// Actual Soil Water Content
	double SoilWaterMin;	// Term of extracted Soil Water
	double SoilWaterAv;		// Available Soil Water
	
	double PotTrans_Temp;	// temporal variable for deciding if actual transpiration happens
	
	//double *SoilWaterAv = calloc((xpn->pSo->iLayers), sizeof(double));	// Available Soil Water (actual water content - PWP or 0)
	//double *H20Up_Temp = calloc((xpn->pSo->iLayers), sizeof(double));
	
	double dt = xpn->pTi->pTimeStep->fAct;	// actual time step
	
	/* Functions */
	int libtreemix_ActualTranspirationFAGUS(libtreemix *self);

	/****************************************************************************************************************/
	
	if(pPl == NULL)
	{
		PRINT_MESSAGE(xpn,3,"There are no plants, which could transpire!?");
		//PRINT_ERROR("FEhler");
		return RET_SUCCESS;
	}
	if(self->conf.Soil_Processes == 2)
	{
		/* Setting All Values = 0.0 For Every Species */
		PotRtWaterUp = 0.0;
		ActRtWaterUp_P = 0.0;
		ActRtWaterUp_L = 0.0;
		SoilWaterTot = 0.0;
		SoilWaterAct = 0.0;
		SoilWaterMin = 0.0;
		SoilWaterAv = 0.0;
		PotTrans_Temp = 0.0;
		
		pPW->fPotUptake = 0.0;
		pPW->fPotUptakedt = 0.0;
		pPW->fActUptake = 0.0;
		pPW->fActUptakedt = 0.0;
		pR->fUptakeR = 0.0;
		pR->fUptakedt = 0.0;
		pPW->fActTranspR = 0.0;
		
		for(L=1; L<=(xpn->pSo->iLayers-2); L++)
		{				
			pLR->fActWatUpt	   	= 0.0;
			pLR->fActLayWatUpt 	= 0.0;
			
			pLR->fPotWatUpt		= 0.0;
			pLR->fPotLayWatUpt	= 0.0;
			
			pLR = pLR->pNext;
		}		
			
		for(i=0; i<self->conf.Species_Count; i++)
		{
			for(L=1; L<=(xpn->pSo->iLayers-2); L++)
			{				
				self->plant[i].PotLayRtWaterUp[L] = 0.0;
				self->plant[i].ActLayRtWaterUp[L] = 0.0;
			}
			
			self->plant[i].ActTrans = 0.0;
			self->plant[i].ActTransdt = 0.0;
			
			self->plant[i].PotRtWaterUpTot = 0.0;
			self->plant[i].ActRtWaterUpTot = 0.0;
		}
		
		//==================================================================================================================
		/* ============================BEGIN CALCULATION==================================== */
		//==================================================================================================================
	
		/* Ask if Potential Transpiration is larger 0.001 */
		for(i=0; i<self->conf.Species_Count; i++)
		{
			if((self->silv[i].PlantGrowth == 1) && (self->silv[i].Maturity == 0))
			{				
				
				if(self->plant[i].PotTrans <= 0.001)
				{
					self->plant[i].ActTrans = 0.0;
					self->plant[i].RtWaterUp = 0.0;
					pR->fUptakeR = 0.0;
					
					self->plant[i].StressFacPh = 1.0;
					self->plant[i].StressFacLf = 1.0;
					
					PotTrans_Temp += self->plant[i].PotTrans;
					
					self->plant[i].SWDefFac = (double)((self->plant[i].SWDefFac*xpn->pTi->pSimTime->fTimeY+self->plant[i].StressFacPh)/
						(xpn->pTi->pSimTime->fTimeY+1));
				}
			}
		}
		if(pPW->fPotTranspR == 0.0)
		{
			pPW->fActTranspR = 0.0;
			return RET_SUCCESS;
		}
		
		//==================================================================================================================
		/*-----------------------------Potential Root Water Uptake --------------------------------------*/
		//==================================================================================================================
		for(i=0; i<self->conf.Species_Count; i++)
		{
			if((self->silv[i].PlantGrowth == 1) && (self->silv[i].Maturity == 0))
			{
				/* first layers */
				pLR	 = xpn->pPl->pRoot->pLayerRoot->pNext;
				pSL  = xpn->pSo->pSLayer->pNext;
				pSWL = xpn->pSo->pSWater->pNext;
				pSLW = xpn->pWa->pWLayer->pNext;
				
				L = 1;
				
				while(((self->plant[i].RtLengthDens[L-1] != 0.0)||(self->plant[i].RtLengthDens[L] != 0.0))&&(L < (xpn->pSo->iLayers-2)))
				{
					//Test of Hong SoilWaterAv = MAX(0.0, (pSLW->fContAct - pSWL->fContPWP));	//[Vol%]
					//Test of Hong:
					SoilWaterAv = MAX(0.0, (pSLW->fContAct - (pSWL->fContPWP+EPSILON)));	//[Vol%]
					//orig CERES: self->plant[i].PotRtWaterUp = 0.00267*exp(62.0*SoilWaterAv)/(6.68-log(self->plant[i].RtLengthDens[L-1]));
					self->plant[i].PotRtWaterUp = 0.00267*exp(MIN(35.0*SoilWaterAv, 10.0))/(6.68-log(self->plant[i].RtLengthDens[L-1]));	// [cm³ H20/cm ROOT*day]
					
					if(self->plant[i].PotRtWaterUp > self->plant[i].WaterUpMax)	// WaterUpMax in paragraph: [Root], [cm3 H20/cm ROOT*day]
					{
						self->plant[i].PotRtWaterUp = self->plant[i].WaterUpMax;
					}
					//Test of Hong if(pSLW->fContAct <= pSWL->fContPWP)
					if(pSLW->fContAct <= (pSWL->fContPWP+EPSILON))
					{
						self->plant[i].PotRtWaterUp = 0.0;
					}
					
					//[mm H2O/day], rate
					self->plant[i].PotLayRtWaterUp[L-1] = (self->plant[i].PotRtWaterUp * pSL->fThickness * self->plant[i].RtLengthDens[L-1]);
					
					// species specific total potential uptake, [mm H2O/day], rate
					self->plant[i].PotRtWaterUpTot += (self->plant[i].PotLayRtWaterUp[L-1]);
					
					// next layer
					L++;
					pSL		= pSL->pNext;
					pSWL	= pSWL->pNext;
					pSLW	= pSLW->pNext;
					pLR		= pLR->pNext;				
				}
				
				self->plant[i].WaterUpFac = 1.0;
				if(self->plant[i].PotTrans <= self->plant[i].PotRtWaterUpTot)
				{
					self->plant[i].WaterUpFac = self->plant[i].PotTrans/self->plant[i].PotRtWaterUpTot;
				}			
			}
			
			// uptake of all species = TRWU in TREEDYN
			PotRtWaterUp += (self->plant[i].PotRtWaterUpTot*self->plant[i].TreeDistr);				
		}		
		// Potential Uptake Over Time => Cumulated Total Potential Root Water Uptake Over All Species
		pPW->fPotUptake = PotRtWaterUp;	// [mm/d], rate
		pPW->fPotUptakedt = pPW->fPotUptake*dt;
		/*-----------------------------Actual Root Water Uptake, summed --------------------------------------*/
		//pPW->fActUptake = 
		
		//==================================================================================================================
		/*-----------------------------Actual Root Water Uptake, species specific --------------------------------------*/
		//==================================================================================================================
		for(i=0; i<self->conf.Species_Count; i++)
		{				
			if((self->silv[i].PlantGrowth == 1) && (self->silv[i].Maturity == 0))
			{
				//CalcActualRootWaterUptake(exp_p, SoilWaterTot, WaterUpFac, SoilWaterAct);
				
				// Total Uptake Summed Up Over All Species FOR COMPLETE SOIL PROFILE = fActUptakedt in Treedyn
				self->plant[i].ActRtWaterUp	= self->plant[i].PotRtWaterUpTot * self->plant[i].WaterUpFac;
				self->plant[i].ActRtWaterUp = MIN(self->plant[i].PotTrans, self->plant[i].PotRtWaterUpTot);
				
				ActRtWaterUp_P += (self->plant[i].ActRtWaterUp*self->plant[i].TreeDistr);
				
				
				/* first layers */
				pLR = xpn->pPl->pRoot->pLayerRoot->pNext;
				pSL = xpn->pSo->pSLayer->pNext;
				pSWL = xpn->pSo->pSWater->pNext;
				pSLW = xpn->pWa->pWLayer->pNext;
				
				for(L=1; L <= (xpn->pSo->iLayers-2); L++)
				{
					self->plant[i].ActLayRtWaterUp[L-1] = (self->plant[i].PotLayRtWaterUp[L-1] 	* self->plant[i].WaterUpFac); //*10.0; 	// [mm/d]				
					
					// if Root water uptake is to large, then it is diminished
					SoilWaterAct = pSLW->fContAct;
					//orig: SoilWaterAct -= ((self->plant[i].ActLayRtWaterUp[L-1]*self->plant[i].TreeDistr)/pSL->fThickness);	// every species takes up water according to distribution
					
					/*Test of Hong SoilWaterMin = (self->plant[i].ActLayRtWaterUp[L-1]/(pSL->fThickness)); //*self->plant[i].TreeDistr;
					SoilWaterAct -= SoilWaterMin;
					*///Test of Hong
					//Test of Hong if(SoilWaterAct < pSWL->fContPWP)
					if(SoilWaterAct <= (pSWL->fContPWP+EPSILON))
					{
						//Test of Hong self->plant[i].ActLayRtWaterUp[L-1] *= (SoilWaterAct/pSWL->fContPWP);	//aus SPASS
						
						//Test of Hong
						self->plant[i].ActLayRtWaterUp[L-1]= 0;
						//End of Test
					}
					
					//Test of Hong 
					SoilWaterMin = (self->plant[i].ActLayRtWaterUp[L-1]/(pSL->fThickness)); //*self->plant[i].TreeDistr;
					SoilWaterAct -= SoilWaterMin;
					//End of Test
					
					// total actual water uptake for every species
					self->plant[i].ActRtWaterUpTot += self->plant[i].ActLayRtWaterUp[L-1];
					
					pSL	= pSL->pNext;
					pSWL = pSWL->pNext;
					pSLW = pSLW->pNext;
					pLR = pLR->pNext;					
				}
				ActRtWaterUp_L += (self->plant[i].ActRtWaterUpTot * self->plant[i].TreeDistr);
			}			
		}
		pPW->fActUptake = ActRtWaterUp_P;
		pR->fUptakeR = ActRtWaterUp_L;
		//==================================================================================================================
		/*-----------------------------Calculate Actual Transpiration and Stress Factors--------------------------------------*/
		//==================================================================================================================
		for(i=0; i<self->conf.Species_Count; i++)
		{				
			if((self->silv[i].PlantGrowth == 1) && (self->silv[i].Maturity == 0))
			{
				// actual transpiration
				self->plant[i].ActTrans = MIN(self->plant[i].PotTrans, self->plant[i].PotRtWaterUpTot);
				
				if(self->plant[i].PotTrans >= self->plant[i].PotRtWaterUpTot)
				{
					self->plant[i].ActTrans = self->plant[i].PotRtWaterUpTot;
				}
				else
				{
					self->plant[i].ActTrans = self->plant[i].PotTrans;
				}				
				
				
				
				self->plant[i].StressFacLf = 1.0;	//entspricht swdf2
				if((self->plant[i].PotRtWaterUpTot/self->plant[i].PotTrans) < 1.5)
				{
					self->plant[i].StressFacLf  = 0.67*(self->plant[i].PotRtWaterUpTot/self->plant[i].PotTrans);
				}

				self->plant[i].StressFacPh = 1.0;	//entspricht swdf1
				if(self->plant[i].PotTrans > self->plant[i].PotRtWaterUpTot)
				{		
					self->plant[i].StressFacPh = MAX((self->plant[i].PotRtWaterUpTot/self->plant[i].PotTrans), 0.2);	//original: 0.02
				}

				self->plant[i].SWDefFac = ((self->plant[i].SWDefFac*xpn->pTi->pSimTime->fTimeY+self->plant[i].StressFacPh)/ (xpn->pTi->pSimTime->fTimeY+1));
			}
			
			self->plant[i].ActTransdt = self->plant[i].ActTrans*dt;	// [mm/dt]
			
			pPW->fActTranspR += (self->plant[i].ActTrans*self->plant[i].TreeDistr);
			//pPW->fActTranspR = (self->plant[i].ActTrans*self->plant[i].TreeDistr); //changed by Hong on 20190529: falsch!!! soll man für i aufsummieren
			
		}
		
		
		// first layers
		pLR = xpn->pPl->pRoot->pLayerRoot->pNext;
		pSL = xpn->pSo->pSLayer->pNext;
		pSWL = xpn->pSo->pSWater->pNext;
		pSLW = xpn->pWa->pWLayer->pNext;

		// !!! Here the actual water uptake from the soil layers happens !!!			
		for(L=1; L <= (xpn->pSo->iLayers-2); L++)
		{
			for(i=0; i<self->conf.Species_Count; i++)
			{
				// every species takes up water according to distribution					
				pLR->fActLayWatUpt += (self->plant[i].ActLayRtWaterUp[L-1]*self->plant[i].TreeDistr*dt);
			}
			
			pSLW->fContAct -= (pLR->fActLayWatUpt/pSL->fThickness);	// [%]
			
			pLR = pLR->pNext;
			pSL = pSL->pNext;
			pSWL = pSWL->pNext;
			pSLW = pSLW->pNext;	
			
		}
		
		
	}//SoilProcesses
	
	/***DAILY VARIABLES********************************************************************/	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(NewDay(xpn->pTi))
		{
			self->plant[i].ActTransDay = 0.0;
			xpn->pWa->fActTranspDay = 0.0;
		}
		
		self->plant[i].ActTransDay += (self->plant[i].ActTrans*dt);
		xpn->pWa->fActTranspDay += self->plant[i].ActTransDay;
	}
	
	/* DEBUG VARIABLES */
	//C_DEBUG(pPW->fActTransp);
	//C_DEBUG(pR->fUptakeR);
	//C_DEBUG(pPW->fPotTransp);
	
	//C_DEBUG(ActRtWaterUp_P);
	//C_DEBUG(ActRtWaterUp_L);
	
	
	/* free */ 
	//free(SoilWaterAv);
	//free(H20Up_Temp);
	
	
	return RET_SUCCESS;
};

int libtreemix_Rutter_Model(libtreemix *self, int i)
{
	/* 
	 * RUTTER model, 
	 * Rutter et al. 1971, 
	 * Agricultural Meteorology 
	 * 
	 */
	
	/* Abbreviations */
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	PWEATHER pWe = xpn->pCl->pWeather;
	PPLTWATER pPW = xpn->pPl->pPltWater;	
	//PPLANT pPl = xpn->pPl;
	
	/* model funtions */
	//double get_Rn(interception *self);
	
	/* variables */	
	double Rs, Rn;
	double lambda, rho_c, gamma;
	
	//double a, b, K, p;			// coefficients for calculating DrainageR, throughfall coefficient [mm]	
	double T, RH, wind, Delta, es, ea;
	//double P, gamma; 						// [kPa/Celsius]
	double measurementheight, elevation;	//  [m]
	double dt;
	double f_calc, f, h, z, z_null, d, k, u, u_z, u_2m;	// height parameters [cm], von Karman Constant
	double res_a; 							// aerodynamic resistance [s/m]
	double Term1, Term2, Term3;
	double result1, result2, result3;
	
	/* Get Weather and Location Values */
	T = pWe->fTempAir;
	RH = pWe->fHumidity;
	Rs = pWe->fSolRad;			// [MJ/m2*day]
	Rn = pWe->fSolNet;			// [MJ/m2*day]
	wind = pWe->fWindSpeed;	
	measurementheight = xpn->pCl->fWindMeasHeight; //[m]
	elevation = xpn->pLo->pFarm->fAltitude;
	dt = xpn->pTi->pTimeStep->fAct;
	
	self->plant[i].PreciRateR = pWe->fBulkPreciRate;
	self->plant[i].PreciRate_dt = pWe->fBulkPreciRate * dt;
	if(pWe->fBulkPreciRate > 0.0)
	{
		//PRINT_MESSAGE(xpn,3,"P > 0.0mm");
	}
	
	/* Height Dependent Parameters */
	u = pWe->fWindSpeed*100;			// wind speed [m/s]*100 -> [cm/s]
	h = self->plant[i].H*100.0;			// stand height [cm], xpn->pPl->pCanopy->fPlantHeight;
	z = h + (measurementheight*100.0);	// height  [cm], at which ea, ea(T), and u are measured		 
	z_null = 0.1*h;						// roughness length [m], where u appears to go zero
	d = 0.75*h;							// zero plane displacement [cm]
	k = 0.41;							// von Karman constant [-]
	f = 9.2;							// product of res_a*u
	u_z = (u/4.87)*log(67.8*z-5.42); 	// u in height z [cm/s], logarithmic wind profile
	
	u_2m = wind*(log(2.0/z_null)/log(measurementheight/z_null));
	//C_DEBUG(u_2m);
	
	
	/* Constants, unit conversions see: "the ETo calculator" */
	lambda 	= 590.0;		// latent heat vaporization of water, [cal/cm3]			//2.45; 		// [MJ/kg]; 0.0024662 [MJ/cm3]
	rho_c 	= 2.9e-4;		// [cal/cm3*°C]		//1.2122e-9;	// [MJ/kg*Celsius]; 1.2122e-9 [MJ/cm3*Celsius]
	gamma	= 0.66;			// [mbar/°C]
	
	/* Drainage Coefficients */
	self->plant[i].a_Drain = log(2.88)-(self->plant[i].b_Drain*self->plant[i].SaturationCap);	// 0.002 mm/min = 2.88 mm/day
	self->plant[i].K_Drain = exp(self->plant[i].a_Drain);

	/* Calculating Throughfall */
	self->plant[i].CanopyInputR = (1.0-self->plant[i].ThroughfallCoeff)*pWe->fBulkPreciRate;
	self->plant[i].CanopyWatR = self->plant[i].CanopyInputR + self->plant[i].CanopyWatR_old;
	self->plant[i].FreeThroughfallR = self->plant[i].ThroughfallCoeff * pWe->fBulkPreciRate;
	
	/* [mm/dt] */
	self->plant[i].CanopyInput_dt = (1.0-self->plant[i].ThroughfallCoeff)*self->plant[i].PreciRate_dt;
	self->plant[i].CanopyWat_dt = self->plant[i].CanopyInput_dt + self->plant[i].CanopyWat_dt_old;
	self->plant[i].FreeThroughfall_dt = self->plant[i].ThroughfallCoeff * self->plant[i].PreciRate_dt;
	self->plant[i].C_S_dt = self->plant[i].CanopyWat_dt/self->plant[i].SaturationCap;	
	
	/* Calculating Atmospheric Parameters */	
	Delta = (2503.0*exp( 17.27*T/(T+237.3))/pow(T+237.3,2.0))/10.0; // [kPa/°C]/10.0 -> [mbar/°C], change of vapor pressure of water with temp.
	es = (0.6108*exp(17.27*T/(T+237.3)))/10.0; 						// [kPa/°C]/10.0 -> [mbar/°C]
	ea = (RH/100.0 * es)/10.0;										// [kPa/°C]/10.0 -> [mbar/°C], Method nr. 3 in table 4, using rel. humidity and temperature at the hour
	//P = 101.3*pow((293.0-0.0065*elevation)/293.0, 5.26);	//atmospheric pressure eqn34, Psychrometric Constant (γ)
	//gamma = 0.000665*P;										// eqn 35
	
	res_a = (f/u);	// approximation [s/cm]
	
	f_calc = (1.0/(k*k)*pow((log((z-d)/z_null)), 2.0));
	res_a = f_calc/u;	 // Aerodynamic Resistance, [s/cm]	
	
	/* calculate net Radiation */
	//Rn = get_Rn(self);	// /10000.0; // [MJ/m2] -> [MJ/cm2]
	Rn = Rn/0.041868/86400.0;			 //	[MJ/m2*day] -> [cal/cm2*sec]
	
	/* Calculating Evaporation from Canopy */	
	Term1 = Delta*Rn;					// [cal*mbar/cm2*s*°C]
	Term2 = rho_c*(es-ea)/res_a;		// [cal*mbar/cm2*s*°C]
	Term3 = lambda*(Delta+gamma);		// [cal*mbar/cm3*°C]
	
	result1 = Term1 + Term2;			// result: [cal*mbar/°C*cm2*s]
	result2 = result1/Term3;			// result: [cm/s]
	result3 = result2*10.0;				// conversion: [cm/s]->[mm/s]
	self->plant[i].Epot_R = result3*86400.0; 			// [mm/s] -> [mm/d]
	self->plant[i].Epot_dt = self->plant[i].Epot_R * dt;// [mm/dt]
	
	/* Getting DrainageR (as a rate) */	
	if(self->plant[i].CanopyWat_dt >= self->plant[i].SaturationCap)	// for C>=S
	{		
		self->plant[i].Drainage_dt = log(dt*self->plant[i].b_Drain*self->plant[i].K_Drain + exp(-self->plant[i].b_Drain*self->plant[i].CanopyWat_dt_old))/-self->plant[i].b_Drain;	// eq. 11
		self->plant[i].DrainageR = 2.88*exp(self->plant[i].b_Drain*(self->plant[i].CanopyWat_dt-self->plant[i].SaturationCap));	// eq. 10, geht gegen unendlich je größer das gespeicherte wasser in der canopy		
		self->plant[i].Eact_R = self->plant[i].Epot_R;
	}
	else if(self->plant[i].CanopyWat_dt < self->plant[i].SaturationCap)	// for C<S
	{
		self->plant[i].Drainage_dt = 0.0;
		self->plant[i].DrainageR = 0.0;
		
		if(self->plant[i].CanopyWat_dt == 0.0)	// Valente et al. 1995, Journal of Hydrology
		{
			self->plant[i].Eact_R = 0.0;
		}
		else
		{
			self->plant[i].Eact_R = self->plant[i].Epot_R * (self->plant[i].CanopyWat_dt/self->plant[i].SaturationCap);		// Epot is diminished by (C/S)
		}		
	}
	
	
	/* Interception rate == Actual Evaporation from Canopy*/
	self->plant[i].InterceptionR = self->plant[i].Eact_R;
	
	/* time step sums */
	self->plant[i].Eact_dt = self->plant[i].Eact_R*dt;
	self->plant[i].Interception_dt = self->plant[i].InterceptionR*dt;	
	self->plant[i].Throughfall_dt = self->plant[i].Drainage_dt + self->plant[i].FreeThroughfall_dt;
	self->plant[i].CanopyWat_dt_old = self->plant[i].CanopyWat_dt - self->plant[i].Drainage_dt - self->plant[i].Interception_dt;	// eq. 2
	
	self->plant[i].ThroughfallR = self->plant[i].Throughfall_dt/dt;	
	
	/* ==================================================================================================================================== */
	if(NewDay(xpn->pTi))
	{		
		self->plant[i].RnDay 				= 0.0;		
		self->plant[i].PreciRate_Day		= 0.0;
		self->plant[i].Interception_Day 	= 0.0;
		self->plant[i].Epot_Day 			= 0.0;
		self->plant[i].Eact_Day 			= 0.0;
		self->plant[i].Throughfall_Day 		= 0.0;		
		self->plant[i].Drainage_Day 		= 0.0;
		
		pPW->fInterceptDay = 0.0;
	}
	self->plant[i].RnDay 				+= (Rn*dt);								// [MJ/m2*dt]	
	self->plant[i].PreciRate_Day		+= (self->plant[i].PreciRate_dt);		// [mm/d]
	self->plant[i].Interception_Day		+= (self->plant[i].Interception_dt);	// [mm/d]	
	self->plant[i].Epot_Day 			+= (self->plant[i].Epot_dt);			// [mm/d]
	self->plant[i].Eact_Day 			+= (self->plant[i].Eact_dt);			// [mm/d]
	self->plant[i].Throughfall_Day 		+= (self->plant[i].Throughfall_dt);		// [mm/d], Free Throughfall + Drainage	
	self->plant[i].Drainage_Day 		+= (self->plant[i].Drainage_dt);		// [mm/d]
	self->plant[i].FreeThroughfall_Day	+= (self->plant[i].FreeThroughfall_dt);	// [mm/d]
	
	self->plant[i].Epot_Yr 				+= self->plant[i].Epot_dt;
	self->plant[i].Eact_Yr 				+= self->plant[i].Eact_dt;
	self->plant[i].Interception_Year 	= self->plant[i].Eact_Yr;	
	self->plant[i].Throughfall_Yr		+= self->plant[i].Throughfall_dt;
	self->plant[i].PreciRate_Yr 		+= self->plant[i].PreciRate_dt;
	self->plant[i].FreeThroughfall_Yr 	+= self->plant[i].FreeThroughfall_dt;
	self->plant[i].Drainage_Yr 			+= self->plant[i].Drainage_dt;
	
	self->plant[i].IE_P					= self->plant[i].Interception_Year/self->plant[i].PreciRate_Yr;
	
	self->plant[i].CanPreci_Yr			= (self->plant[i].Drainage_Yr + self->plant[i].FreeThroughfall_Yr);
	
	
	self->plant[i].Bal_I += (self->plant[i].PreciRate_dt - self->plant[i].Throughfall_dt);
	self->plant[i].Bal_E += (self->plant[i].PreciRate_dt - self->plant[i].Throughfall_dt);
	self->plant[i].Bal_T += ((self->plant[i].b_Drain*self->plant[i].PreciRate_dt) + self->plant[i].Drainage_dt);
	self->plant[i].Bal_R += (self->plant[i].PreciRate_dt);
	self->plant[i].Bal_P += (self->plant[i].Throughfall_dt+self->plant[i].Interception_dt);
	
	return RET_SUCCESS;
  }
