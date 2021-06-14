#include "libtreemix.h"
#include <math.h>

int libtreemix_NitrogenUptake(libtreemix *self)
{
	/* Description */
	/*
	 * 
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW 	= xpn->pPl->pPltWater;
	//PGENOTYPE	pGen 	= xpn->pPl->pGenotype;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	PPLTNITROGEN	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY		pCan 	= xpn->pPl->pCanopy;
	//PWEATHER 		pWe  	= xpn->pCl->pWeather;
	//PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PROOT			pRt		= xpn->pPl->pRoot;
	//PSWATER		pSWL	= xpn->pSo->pSWater;
	//PWLAYER	   	pSLW	= xpn->pWa->pWLayer;
	PCLAYER     	pSLN	= xpn->pCh->pCLayer;	//chemistry
	PLAYERROOT		pLR	= xpn->pPl->pRoot->pLayerRoot->pNext;
	PCHEMISTRY		pCh = xpn->pCh;
	PTIME 			pTi = xpn->pTi;
	
	/* Variables */
	int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	int L;
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(self->conf.Soil_Processes == 1)
		{
			self->plant[i].NUpAct_dt = self->plant[i].NUpActR*1000.0/365.25*dt;	//[kg(N)/ha*timestep]
			pPlN->fActNUptR = pPlN->fActNUpt;
		}
		else if(self->conf.Soil_Processes== 2)
		{
			// set layer variables to zero
			self->plant[i].ActNO3Up = 0.0;
			self->plant[i].ActNH4Up = 0.0;
			pPlN->fActNO3NUpt = 0.0;
			pPlN->fActNH4NUpt = 0.0;
			
			// if calculated leaf and root uptake rate is > 0
			if(self->plant[i].NUpLfRtR > 0.0)
			{
				// calc act. uptakes (dt) from rates, [kg(N)/ha*timestep]
				self->plant[i].NUpAct_dt = self->plant[i].NUpActR * 1000.0/365.25*dt;
				self->plant[i].NUpLfRt_dt = self->plant[i].NUpLfRtR*1000.0/365.25*dt;
				
				// Normalization Factor [-] = tN/ha*yr		/		tN/ha*yr
				//self->plant[i].NUpNormFac = self->plant[i].NUpPotR/self->plant[i].NUpLfRtR;
				self->plant[i].NUpNormFac = (self->plant[i].NUpActR/self->plant[i].NUpLfRtR);
				
				//pPlN->fActNUptR = pPlN->fActNUpt;
				
				// calc. act. uptake per numeric soil layer [kgN/ha*d]
				pLR	= xpn->pPl->pRoot->pLayerRoot->pNext;
				for (L=1; L <= xpn->pSo->iLayers-2; L++)
				{
					// units:	kgN/ha*d	<=		kgN/ha*d			*			[-]
					self->plant[i].ActNO3Up += (self->plant[i].NO3Up[L] * self->plant[i].NUpNormFac);
					self->plant[i].ActNH4Up += (self->plant[i].NH4Up[L] * self->plant[i].NUpNormFac);

					self->plant[i].ActLayNO3NUpt[L] = self->plant[i].NO3Up[L] * self->plant[i].NUpNormFac;
					self->plant[i].ActLayNH4NUpt[L] = self->plant[i].NH4Up[L] * self->plant[i].NUpNormFac;
					
					self->plant[i].NUpAct_1_Day += ((self->plant[i].ActLayNO3NUpt[L] + self->plant[i].ActLayNH4NUpt[L])*dt);

					pLR = pLR ->pNext;
				}				
			}
			else
			{	
				//no n-uptake at all over all layers
				self->plant[i].NUpActR = 0.0;
				self->plant[i].NUpAct_dt = 0.0;
				
				self->plant[i].NUpLfRtR = 0.0;
				self->plant[i].NUpLfRt_dt = 0.0;
				
				pPlN->fActNUptR = 0.0;				
				//pPlN->fCumActNUpt += 0.0;
				pPlN->fCumActNUpt = 0.0; //Hong

				pLR	= xpn->pPl->pRoot->pLayerRoot->pNext;
				for (L=1; L <= xpn->pSo->iLayers-2; L++)
				{
					self->plant[i].ActNO3Up += 0.0;
					self->plant[i].ActNH4Up += 0.0;
											
					self->plant[i].ActLayNO3NUpt[L] = 0.0;
					self->plant[i].ActLayNH4NUpt[L] = 0.0;
		      				
					pLR = pLR ->pNext;
				}
			}
			
			
		}
		
		//=======================================================================
		if(NewDay(xpn->pTi))
		{
			self->plant[i].NUpAct_1_Day = 0.0;
			self->plant[i].NUpAct_2_Day = 0.0;
			self->plant[i].NUpAct_3_Day = 0.0;
			
			self->plant[i].ActNO3Up_Day = 0.0;
			self->plant[i].ActNH4Up_Day = 0.0;
			
		}
		
		// [kgN/ha*day]
		self->plant[i].ActNO3Up_Day += (self->plant[i].ActNO3Up*dt);
		self->plant[i].ActNH4Up_Day += (self->plant[i].ActNH4Up*dt);		
		self->plant[i].NUpAct_2_Day += ((self->plant[i].ActNH4Up*dt) + (self->plant[i].ActNO3Up*dt));
		self->plant[i].NUpAct_3_Day += self->plant[i].NUpAct_dt;
		
		// total nitrogen uptake over simulation period [kgN/ha*dt]
		self->plant[i].NUpAct_Sum	+= self->plant[i].NUpAct_dt;
		self->plant[i].ActNO3Up_Sum += (self->plant[i].ActNO3Up*dt);
		self->plant[i].ActNH4Up_Sum += (self->plant[i].ActNH4Up*dt);
		
		//added by Hong on 20180608 (pPlN->fActNUptR was missed): 
		pPlN->fActNUptR = (((self->plant[i].ActNO3Up+self->plant[i].ActNH4Up))*self->plant[i].TreeDistr);
		pPlN->fCumActNUpt =self->plant[i].NUpAct_Sum;
		//End of Hong
		//pPlN->fCumActNUpt += (((self->plant[i].ActNO3Up+self->plant[i].ActNH4Up)*dt)*self->plant[i].TreeDistr);
		
		
	}
	
	/* Actualize Soil Nitrogen Content */
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(self->conf.Soil_Processes== 2)
		{
			pLR		= xpn->pPl->pRoot->pLayerRoot->pNext;
			pSLN	= xpn->pCh->pCLayer->pNext;
			
			for (L=1; L <= xpn->pSo->iLayers-2; L++)
			{
				pSLN->fNO3N -= self->plant[i].ActLayNO3NUpt[L]*self->plant[i].TreeDistr*dt;
				pSLN->fNH4N -= self->plant[i].ActLayNH4NUpt[L]*self->plant[i].TreeDistr*dt;

                if(pSLN->fNO3N < EPSILON)
				{
					pSLN->fNO3N = EPSILON;
				}
				if(pSLN->fNH4N < EPSILON)
				{
					pSLN->fNH4N = EPSILON;
				}				
				
//End of Test				
				
				pLR = pLR->pNext;
				pSLN = pSLN->pNext;
			}
		}
	}
	
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		/*
		 * NITROGEN DEPOSITION SCENARIOS
		 * Source: Atmospheric Deposition and Canopy Interactions
		 */
		if((pTi->pSimTime->iyear > 1970)&&(pTi->pSimTime->iyear < 1981))
		{
			pCh->pCProfile->fNO3NImisR = (36.0/365.25);
			pCh->pCProfile->fCH4ImisR  = (15.2/365.25);
		}
		else if((pTi->pSimTime->iyear > 1980)&&(pTi->pSimTime->iyear < 1991))
		{
			pCh->pCProfile->fNO3NImisR = (42.8/365.25);
			pCh->pCProfile->fCH4ImisR  = (15.3/365.25);
		}
		else if(pTi->pSimTime->iyear > 1990)
		{
			// [kgN/ha*d], average values from 1990-2002
			pCh->pCProfile->fNO3NImisR = (31.6/365.25);
			pCh->pCProfile->fCH4ImisR  = (11.4/365.25);
			
		}	
		else if(self->clim.YrActDeposition < pTi->pSimTime->iyear)
		{
			self->clim.YrCnt += 1;
			double increase = 0.0;
				
			if(self->conf.Nitrogen_Deposition == 1)
			{
				if(self->plant[i].NDeposTotal < 51.2)
				{
					// increase nitrogen deposition additional to deposition module, [kgN/ha*d]				
					increase = (self->plant[i].NO3_DepIncr/365.25);
					
					pCh->pCProfile->fNO3NImisR = (increase * self->clim.YrCnt)       + (self->plant[i].N_DepStart/365.25);
					pCh->pCProfile->fCH4ImisR  = ((increase*0.383)*self->clim.YrCnt) + ((self->plant[i].N_DepStart*0.383)/365.25); // average ratio NH4/NO3 = 0.383 = 38.3%

				}
			}
			else if(self->conf.Nitrogen_Deposition == 2)
			{
				// check if maximum N deposition is reached
				if(pCh->pCProfile->fNO3NImisR < 0.125)
				{
					// exponential function according to Jansen et al. 1995, [kgN/ha*d]
					if(pTi->pSimTime->iyear <= 1939)
					{
						pCh->pCProfile->fNO3NImisR = (7.0*exp(0.03*(pTi->pSimTime->iyear - 1880)))/365.25;
					}
					else if((pTi->pSimTime->iyear > 1939)&&(pTi->pSimTime->iyear < 1990))
					{
						pCh->pCProfile->fNO3NImisR = (10.0*exp(0.03*(pTi->pSimTime->iyear - 1940)))/365.25;
					}
				}
			}
			
			self->clim.YrActDeposition = pTi->pSimTime->iyear;
		}
		
		self->plant[i].NDeposTotal = (pCh->pCProfile->fNO3NImisR + pCh->pCProfile->fCH4ImisR)*365.25;	// [kgN/ha*yr]
	}
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
};