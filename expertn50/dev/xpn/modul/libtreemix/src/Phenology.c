#include "libtreemix.h"
#include <math.h>

int libtreemix_Phenology(libtreemix *self)
{	
	/* Description */
	/*
	 * Calculates Thinning, Forest Dieback and Bud Burst
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	
	/* Variables */
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	int libtreemix_GetClimateValues(libtreemix *self, double T_air, double SolarRad);
	int libtreemix_ThinningScenario(libtreemix *self);
	int libtreemix_ForestDieback(libtreemix *self);
	
	/****************************************************************************************************************/
	
	
	libtreemix_GetClimateValues(self, xpn->pCl->pWeather->fTempAir, xpn->pCl->pWeather->fSolRad);
	libtreemix_ThinningScenario(self);
	
	
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("Fehler");
	return RET_SUCCESS;
};

int libtreemix_GetClimateValues(libtreemix *self, double T_air, double SolarRad)
{
	/*
		Computes
		- Temperatures
		- Daylength, Night Fraction
		- Radiation Receipt 
		for the current Julian Day
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);

	/* variables */
	double SinFun;	// intermediate variable
	
	/****************************************************************************************************************/
	
	self->clim.SeasTime = (xpn->pTi->pSimTime->fTimeY+10.0)/365.25;

	if(xpn->pTi->pSimTime->fTimeY == 1)
	{
		self->clim.NewYear = 1;
	}
	else
	{
		self->clim.NewYear = 0;
	}

	if(!self->clim.Polar)
	{
		SinFun = sin(2.0*PI*self->clim.SeasTime-(PI/2.0));	// part of eq. 49, 54

		self->clim.DayLength = 12.0+(self->clim.hAmp/2.0)*SinFun;	// eq. 49
		self->clim.NightFrac = 1.0-(self->clim.DayLength/24.0);

		/* eq. 54 */
		self->clim.IClearDay = self->clim.IAvgClear+(self->clim.IAmpClear/2.0)*SinFun;
		self->clim.ICloudDay = self->clim.IAvgCloud+(self->clim.IAmpCloud/2.0)*SinFun;
	}

	if((self->conf.Weather_Input == 3)||(self->conf.Weather_Input == 4))
	{
		self->clim.T_air = T_air;	// air temperature is being calculated or given by the wrf model
		self->clim.CloudFraction = 1.25*(1.0-self->clim.PARfactor*SolarRad*277.78/self->clim.IClearDay);
	}
	else if(self->conf.Weather_Input == 1)
	{
		//other weather module
	}

	if(self->clim.CloudFraction > 1.0)
	{
		self->clim.CloudFraction = 1.0;
	}
	else if(self->clim.CloudFraction < 0.0)
	{
		self->clim.CloudFraction = 0.0;
	}

	/* Soil Temperature */
	//self->clim.T_soil = (self->clim.damp_T*T_air)+((1.0-self->clim.damp_T)*TYear);
	// soil temperature is being calculated in the DAISY Heat transfer model
	
	
	return RET_SUCCESS;
}

int libtreemix_ThinningScenario(libtreemix *self)
{	
	/* Description */
	/*
	 * Calculates Thinning Strategies
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PBIOMASS		pBio	= xpn->pPl->pBiomass;
	//PPLTCARBON		pPlC	= xpn->pPl->pPltCarbon;
	//PPLTNITROGEN	pPlN	= xpn->pPl->pPltNitrogen;
	//PCANOPY			pCan    = xpn->pPl->pCanopy;
    PSLAYER         pSL     = xpn->pSo->pSLayer;
    PLAYERROOT      pLR     = xpn->pPl->pRoot->pLayerRoot;
    PCLAYER         pChL    = xpn->pCh->pCLayer;
	PCBALANCE	    pCB     = xpn->pCh->pCBalance; //Added by Hong on 20180731
	
	/* Variables */
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double CutTreeFrac = 0.0;
	double Cut = 0.0;

	int i;	// looping variable for species
	int z;	// looping variable for soil layers
	
	double TmpWd, TmpLf, TmpRt, TmpFr = 0.0; // setting temp variables to zero for later summing up
	double fCumDepth;
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(self->silv[i].Harvesting == 1)
		{
			if(self->silv[i].ThinningCounter < self->silv[i].ThinningEvents)
			{
				// other formulation used for thinning start after x years from simulation start:
				//if(((xpn->pTi->pSimTime->iyear - xpn->pTi->pSimTime->iStart_year) + 1.0e-8) >= (self->silv[i].ThinningInterval[self->silv[i].ThinningCounter]))
				
				// new formulation for thinning events in specific years:
				if((xpn->pTi->pSimTime->iyear + 1.0e-8) >= (xpn->pTi->pSimTime->iStart_year+self->silv[i].ThinningInterval[self->silv[i].ThinningCounter])) //Changed by Hong on 20180620
					//if((xpn->pTi->pSimTime->iyear + 1.0e-8) >= (self->silv[i].ThinningInterval[self->silv[i].ThinningCounter]))
				{
					CutTreeFrac = (self->silv[i].ThinningFraction[self->silv[i].ThinningCounter]);
					self->silv[i].ThinningCounter++;
				}
			}

			if(CutTreeFrac <= 1.0e-8)
			{
				return RET_SUCCESS;
			}

			/* Thinning Algorithm must be improved!	*/
			Cut = 1-(1-CutTreeFrac);

			if(self->conf.Tree_Geometry == 2)
			{
				//Here is space for the PGM-module
			}

			if(self->conf.Soil_Processes == 1)
			{
				self->silv[i].CLit += Cut*(self->plant[i].CLfMass + self->plant[i].CFRtMass + self->plant[i].CFrMass +
					(self->plant[i].UgWdFr*(self->plant[i].CWdWeight + self->plant[i].CAss)));

				self->silv[i].NLit += Cut*(self->plant[i].NLf + self->plant[i].NFRt + self->plant[i].NFr +
					(self->plant[i].NWd*self->plant[i].UgWdFr));
			
			//Hong added on 20180731 for C-balance			
		/*pCB->dCInputSurf += Cut*(self->plant[i].CLfMass + self->plant[i].CFRtMass + self->plant[i].CFrMass +
					(self->plant[i].UgWdFr*(self->plant[i].CWdWeight + self->plant[i].CAss))); */
		/*pCB->dCInputProfile += Cut*(self->plant[i].CLfMass + self->plant[i].CFRtMass + self->plant[i].CFrMass +
					(self->plant[i].UgWdFr*(self->plant[i].CWdWeight + self->plant[i].CAss))); 	*/ //Test of Hong	
			//xpn->pCh->pCProfile->fCLitterSurf =xpn->pCh->pCProfile->fCLeafLitterSurf+ xpn->pCh->pCProfile->fCBranchLitterSurf + xpn->pCh->pCProfile->fCStemLitterSurf;//20181016, notwendig			
					
			}
			
			
			else if(self->conf.Soil_Processes == 2) //Hong on 20180620:for agroforst
			{
				pSL		= pSL->pNext;	
				pLR		= pLR->pNext;
				pChL    = pChL->pNext;

				/* 1% of branch mass and 5% of leaf mass are kept */
				//==================================================
				// Leaves and Fruits in Litter
				self->plant[i].FOCLfFr += (0.05 * Cut * self->plant[i].CLfMass);
				self->plant[i].FONLfFr += (0.05 * Cut * self->plant[i].CLfMass * self->plant[i].NLf);
				pChL->fCLitter += (float)((0.05 * Cut * self->plant[i].CLfMass)*self->plant[i].TreeDistr);
				pChL->fNLitter += (float)((0.05 * Cut * self->plant[i].CLfMass * self->plant[i].NLf)*self->plant[i].TreeDistr);	    

				self->plant[i].FOCBrSt += (double)(0.01 * Cut * self->plant[i].CWdWeight *(1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr);
				self->plant[i].FONBrSt += (double)(0.01 * Cut * self->plant[i].CWdWeight *(1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr * self->plant[i].NWdResp);
				pChL->fCLitter += (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr)*self->plant[i].TreeDistr);
				pChL->fNLitter += (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr * self->plant[i].NWdResp)*self->plant[i].TreeDistr);
				
				// CENTURY model variables:
				xpn->pCh->pCProfile->fCLeafLitterSurf += (float)((0.05 * Cut * self->plant[i].CLfMass)*self->plant[i].TreeDistr);
				xpn->pCh->pCProfile->fNLeafLitterSurf += (float)((0.05 * Cut * self->plant[i].CLfMass * self->plant[i].NLf)*self->plant[i].TreeDistr);

				xpn->pCh->pCProfile->fCBranchLitterSurf += (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr)*self->plant[i].TreeDistr);
				xpn->pCh->pCProfile->fNBranchLitterSurf += (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr * self->plant[i].NWdResp)*self->plant[i].TreeDistr);
				
				//Hong added on 20180731 for C-balance			
				/*pCB->dCInputSurf +=(float)((0.05 * Cut * self->plant[i].CLfMass)*self->plant[i].TreeDistr)+ (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr)*self->plant[i].TreeDistr);*/ //notwendig
				
				pCB->dCInputProfile +=(float)((0.05 * Cut * self->plant[i].CLfMass)*self->plant[i].TreeDistr)+ (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr)*self->plant[i].TreeDistr);//Added on 20190801
				pCB->dCInputProfile_30 +=(float)((0.05 * Cut * self->plant[i].CLfMass)*self->plant[i].TreeDistr)+ (float)((0.01 * Cut * self->plant[i].CWdWeight * (1.0-self->plant[i].UgWdFr) * self->plant[i].BrWdFr)*self->plant[i].TreeDistr);//Added on 20190801
                 
				//xpn->pCh->pCProfile->fCLitterSurf =xpn->pCh->pCProfile->fCLeafLitterSurf+ xpn->pCh->pCProfile->fCBranchLitterSurf + xpn->pCh->pCProfile->fCStemLitterSurf;//20181016, notwendig	 
				 
				// Cutted roots
				fCumDepth= 0.0;
				for(z=1; z <= xpn->pSo->iLayers-2; z++)
				{
					/* fine roots litter in soil litter [i] */
					self->plant[i].FOCFiRt[z] += (float)(Cut * self->plant[i].CFRtMass * self->plant[i].LyFc[z]);
					self->plant[i].FONFiRt[z] += (float)(Cut * self->plant[i].CFRtMass * self->plant[i].NFRt * self->plant[i].LyFc[z]);

                    //printf("Chemlayer %f \n", pChL->fCFineRootLitter);

					//Expert-N/Century - Variables:
					pChL->fCFineRootLitter += (float)((Cut * self->plant[i].CFRtMass * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					pChL->fNFineRootLitter += (float)((Cut * self->plant[i].CFRtMass * self->plant[i].NFRt * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					pChL->fCLitter += (float)((Cut * self->plant[i].CFRtMass * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					pChL->fNLitter += (float)((Cut * self->plant[i].CFRtMass * self->plant[i].NFRt * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);

					/* gross roots litter in soil litter [i] */
					self->plant[i].FOCGrRt[z] += (float)(Cut * self->plant[i].CWdWeight *self->plant[i].UgWdFr * self->plant[i].LyFc[z]);
					self->plant[i].FONGrRt[z] += (float)(Cut * self->plant[i].CWdWeight *self->plant[i].UgWdFr * self->plant[i].NConcWd * self->plant[i].LyFc[z]);

					//Expert-N/Century - Variables:
					pChL->fCGrossRootLitter += (float)((Cut * self->plant[i].CWdWeight * self->plant[i].UgWdFr * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					pChL->fNGrossRootLitter += (float)((Cut * self->plant[i].CWdWeight *self->plant[i].UgWdFr * self->plant[i].NConcWd * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					pChL->fCLitter	+= (float)((Cut * self->plant[i].CWdWeight * self->plant[i].UgWdFr * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					pChL->fNLitter	+= (float)((Cut * self->plant[i].CWdWeight *self->plant[i].UgWdFr * self->plant[i].NConcWd * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
					
					//Hong added on 20180731 for C-balance			
				pCB->dCInputProfile += (float)((Cut * self->plant[i].CFRtMass * self->plant[i].LyFc[z])*self->plant[i].TreeDistr) + (float)((Cut * self->plant[i].CWdWeight * self->plant[i].UgWdFr * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
				
				//Hong 20190507: balance for 0-30 cm profile:	
	           fCumDepth +=(double)0.1*pSL->fThickness; //cm
	           if (fCumDepth <=30.0)
	           {
		         pCB->dCInputProfile_30 += (float)((Cut * self->plant[i].CFRtMass * self->plant[i].LyFc[z])*self->plant[i].TreeDistr) + (float)((Cut * self->plant[i].CWdWeight * self->plant[i].UgWdFr * self->plant[i].LyFc[z])*self->plant[i].TreeDistr);
	           }

					pChL = pChL->pNext;
					pSL  = pSL->pNext; //Hong
				}

				// species specific masses
				self->silv[i].CStHarv = self->silv[i].CStHarv + (Cut*(self->plant[i].CStMass/1000.0));

				self->silv[i].CWdHarv = self->silv[i].CWdHarv + (Cut*(self->plant[i].CWdWeight/1000.0));
				self->silv[i].CTotWd = (self->plant[i].CWdWeight/1000.0)+self->silv[i].CWdHarv;
			
			    if (self->conf.Operating==1)//for forst, added by Hong
				{
			    self->plant[i].density	= (1.0-CutTreeFrac)*self->plant[i].density;
				
				self->plant[i].CWdWeight	= (1.0-Cut)*self->plant[i].CWdWeight;
				self->plant[i].CLfMass		= (1.0-Cut)*self->plant[i].CLfMass;
				self->plant[i].CFRtMass		= (1.0-Cut)*self->plant[i].CFRtMass;
				self->plant[i].CFrMass		= (1.0-Cut)*self->plant[i].CFrMass;
				
				self->plant[i].CAss		= (1.0-Cut)*self->plant[i].CAss;			
				self->plant[i].NWd		= (1.0-Cut)*self->plant[i].NWd;
				}
				
				else if (self->conf.Operating==2)//for agroforst, Added by Hong
				{
				self->plant[i].H=self->silv[i].heightAfterCut; //re-initialize height			
				self->plant[i].D=self->silv[i].diameterAfterCut; // re-initialize diameter
				//self->plant[i].density	= self->plant[i].density; //density remains								
				self->plant[i].CWdWeight	= (1.0-Cut)*self->plant[i].CWdWeight;
				self->plant[i].CLfMass		= (1.0-Cut)*self->plant[i].CLfMass;
				//self->plant[i].CFRtMass		= self->plant[i].CFRtMass; // roots remain				
				self->plant[i].CFrMass		= (1.0-Cut)*self->plant[i].CFrMass;
				self->plant[i].CAss		= (1.0-Cut)*self->plant[i].CAss;
				
				self->plant[i].NWd		= (1.0-Cut)*self->plant[i].NWd;
				}

				TmpWd += self->plant[i].CWdWeight*self->plant[i].TreeDistr;
				TmpLf += self->plant[i].CLfMass*self->plant[i].TreeDistr;
				TmpRt += self->plant[i].CFRtMass*self->plant[i].TreeDistr;
				TmpFr += self->plant[i].CFrMass*self->plant[i].TreeDistr;
                
            pSL     = xpn->pSo->pSLayer;
            pLR     = xpn->pPl->pRoot->pLayerRoot;
            pChL    = xpn->pCh->pCLayer;
                
			}
		}
	}
	
	
	return RET_SUCCESS;
}

int libtreemix_ForestDieback(libtreemix *self)
{
	/* Description */
	/*
	 * Sets Values for the dieback of the stand
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PBIOMASS		pBio	= xpn->pPl->pBiomass;
	//PPLTCARBON		pPlC	= xpn->pPl->pPltCarbon;
	PPLTNITROGEN	pPlN	= xpn->pPl->pPltNitrogen;
	//PCANOPY			pCan    = xpn->pPl->pCanopy;
    //PSLAYER         pSL     = xpn->pSo->pSLayer;
    //PLAYERROOT      pLR     = xpn->pPl->pRoot->pLayerRoot;
    //PCLAYER         pChL    = xpn->pCh->pCLayer;
	
	/* Variables */
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep

	int i;	// looping variable for species
	//int z;	// looping variable for soil layers
	
	/*Functions*/
	
	/****************************************************************************************************************/	
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(self->silv[i].StandAlive == 0)
		{
			return 1;
		}

		if((self->plant[i].CLfMass < EPSILON) && (self->plant[i].CFRtMass < EPSILON))
		{
			self->silv[i].StandAlive = 0;	// forest is dead

			/* Therefore results */
			self->plant[i].H		= 0.0;
     		self->plant[i].density	= 0.0;
     	
			self->plant[i].CLfMass	= 0.0;
     		self->plant[i].CFRtMass	= 0.0;
     		self->plant[i].CFrMass	= 0.0;
     		self->plant[i].CWdWeight= 0.0;
     		
			self->plant[i].LfGrR	= 0.0; 
     		self->plant[i].RtGrR	= 0.0; 
     		self->plant[i].FrGrR	= 0.0; 

     		pPlN->fLeafCont			= 0.0; 
     		pPlN->fRootCont			= 0.0; 
     		pPlN->fGrainCont		= 0.0;

			self->plant[i].CAss			= 0.0;
			self->plant[i].MaintRespR	= 0.0;

		
			self->plant[i].NWd			= 0.0;
			self->plant[i].D			= 0.0;		
			self->plant[i].CStMass		= 0.0;
			self->plant[i].CWdResp		= 0.0;
			self->plant[i].NUpActR		= 0.0;
			self->plant[i].Increment	= 0.0;
		}
	}
	
	return RET_SUCCESS;
}