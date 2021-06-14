#include "libtreemix.h"
#include <math.h>

int libtreemix_RootLengthGrowth(libtreemix *self)
{
	/* Description */
	/*
	 * Computes root length density distribution in soil
	 * From model SPASS (Wang, 1997)
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW 	= xpn->pPl->pPltWater;
	//PGENOTYPE	pGen 	= xpn->pPl->pGenotype;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	//PPLTNITROGEN	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY	pCan 	= xpn->pPl->pCanopy;
	//PWEATHER 	pWe  	= xpn->pCl->pWeather;
	PSLAYER		pSL		= xpn->pSo->pSLayer->pNext;
	//PROOT		pRt		= xpn->pPl->pRoot;
	PSWATER		pSWL	= xpn->pSo->pSWater->pNext;
	PWLAYER	   	pSLW	= xpn->pWa->pWLayer->pNext;
	PCLAYER     pSLN	= xpn->pCh->pCLayer->pNext;	//chemistry
	PLAYERROOT	pLRt	= xpn->pPl->pRoot->pLayerRoot->pNext;
	PPLANT		pPl		= xpn->pPl;
	
	/* Variables */
	int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	int L;
	int L1;
	int NumL;
	
	double DepthCum;
	double DepthMax;
	double RtLengthNew;
	double RtLengthDead;
	double Thickness;
	//double SWDefRel;		//Soil water deficit factor [-]
	double RtGrowthFacNew;	//Root length density factor
	double TRLDF;	//Total Root Length Density Factor [-]
	double TRLD;		//Total Root Length Density
	double RNLF;
	double RLDTemp;	// temporal variable
	
	double TotalLength = 0.0;
		
	/*Functions*/
	
	/****************************************************************************************************************/
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		if(self->conf.Soil_Processes == 1)
		{
			//There is no Root Development in BOSSEL, 1996 (?)
		}
		else if(self->conf.Soil_Processes == 2)
		{
			//initialize variables
			DepthCum = 0.0;
			RtLengthNew = 0.0;
			RtLengthDead = 0.0;
			Thickness = 0.0;
			RtGrowthFacNew = 0.0;
			TRLDF = 0.0;
			TRLD = 0.0;
			RNLF = 0.0;	
			RLDTemp = 0.0;
			NumL = xpn->pSo->iLayers-2;
			DepthMax = 0.0;			
			
			
			pSL = xpn->pSo->pSLayer->pNext;
			for(L=1; L<=xpn->pSo->iLayers-2; L++)
			{
				DepthMax += 0.1*pSL->fThickness; // [mm]->[cm]
				pSL = pSL->pNext;
			}
			
			// The whole Soil Column is rooted
			self->plant[i].DepthMax = DepthMax;			
			
			// Here Time Step dependent!!!!
			RtLengthNew  = (double)(1e3 * self->plant[i].RtGrR/365.25*dt * self->plant[i].RtLengthSpec*1e-5);
			RtLengthDead = (double)(1e3 *(self->plant[i].RootDeathRate+self->plant[i].CLossRt)/365.25*dt*self->plant[i].RtLengthSpec*1e-5);
			
			self->plant[i].RtLengthNew	= RtLengthNew;
			self->plant[i].RtLengthDead = RtLengthDead;
			
			/* Factor Calculation for Root Growth */
			DepthCum = 0.0;
			L = 0;

			pSL	 = xpn->pSo->pSLayer->pNext;
			pSWL = xpn->pSo->pSWater->pNext;
			pSLW = xpn->pWa->pWLayer->pNext; 
			pSLN = xpn->pCh->pCLayer->pNext;
			pLRt = xpn->pPl->pRoot->pLayerRoot->pNext;
			while((DepthCum <= DepthMax) && (L < NumL))
			{	
				L++;

				Thickness = 0.1*pSL->fThickness; //Conversion to cm
				DepthCum += Thickness; //Conversion to cm

				self->plant[i].NO3Nmgkg[L] = 0.01*pSL->fBulkDens*pSL->fThickness;
				self->plant[i].NH4Nmgkg[L] = 0.01*pSL->fBulkDens*pSL->fThickness;
				
				// Soil water deficit factor for root growth
				self->plant[i].SWDefRel[L] = (pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);				
				if(self->plant[i].SWDefRel[L] < 0.25)
				{
					self->plant[i].RtWaterStress[L] = max(0.01, (4.0*self->plant[i].SWDefRel[L]));
				}
				else
				{
					self->plant[i].RtWaterStress[L] = 1.0;
				}
				

				// Mineral nitrogen availability effect factor on root growth
				self->plant[i].RtNStress[L] = 1.0-(1.17*exp(-0.15*((pSLN->fNO3N/self->plant[i].NO3Nmgkg[L])+(pSLN->fNH4N/self->plant[i].NH4Nmgkg[L]))));
				if(self->plant[i].RtNStress[L] < 0.01)
				{
					self->plant[i].RtNStress[L] = 0.01;
				}
				
				// Root Length density factor to calculate new root growth distribution
				RtGrowthFacNew = exp(-4.0*(DepthCum-(0.5*Thickness))/DepthMax);
				self->plant[i].RtLengthDensFac[L-1] = RtGrowthFacNew*Thickness*min(self->plant[i].RtWaterStress[L], self->plant[i].RtNStress[L]);
				if(self->plant[i].RtLengthDensFac[L-1] == 0.0)
				{
					PRINT_MESSAGE(xpn,3,"RtLengthDensFac == 0.0");
				}

				if(L<NumL-1)
				{
					pSL = pSL ->pNext;
					pSWL = pSWL->pNext;
					pSLW = pSLW->pNext;
					pSLN = pSLN->pNext;
					pLRt = pLRt->pNext;
				}
			}
			L1 = L;
			
			/* total root length density factor */
			TRLDF	= 0.0;
			TRLD	= 0.0;			
			for(L=1; L<=L1; L++)
			{
				TRLDF += self->plant[i].RtLengthDensFac[L-1];
				TRLD += self->plant[i].RtLengthDens[L-1];
			}
			self->plant[i].TRLD = TRLD;
			
			/* root length density calculation */
			if(TRLDF >= RtLengthNew*0.00001)
			{
				RNLF = RtLengthNew/(TRLDF+EPSILON);
			}
			else
			{
				return 0;
			}

			pSL		= xpn->pSo->pSLayer->pNext;
			pLRt	= xpn->pPl->pRoot->pLayerRoot->pNext;
			for(L=1; L<=L1; L++)
			{
				Thickness = 0.1*pSL->fThickness;
				
				self->plant[i].RLDTemp[L] = (self->plant[i].RtLengthDensFac[L-1] * RNLF - RtLengthDead * self->plant[i].RtLengthDens[L-1] / TRLD) / Thickness;
				//if(RLDTemp <= 0.0)
				//{
				//	PRINT_MESSAGE(xpn,3,"RLDTemp <= 0.0");
				//}
				
				self->plant[i].RtLengthDens[L-1] += self->plant[i].RLDTemp[L]; //(self->plant[i].RtLengthDensFac[L-1] * RNLF - RtLengthDead * self->plant[i].RtLengthDens[L-1] / TRLD) / Thickness;
				self->plant[i].RtLengthDens[L-1] = max(EPSILON, self->plant[i].RtLengthDens[L-1]);
				self->plant[i].LyFc[L] = self->plant[i].RtLengthDens[L-1]/TRLD;
				
				// mean root length density for forest stand
				pLRt->fLengthDens = (self->plant[i].RtLengthDens[L-1]*self->plant[i].TreeDistr);

				pSL = pSL->pNext;
				pLRt = pLRt->pNext;
			}
			
			// for XN output
			TotalLength += (TRLD*self->plant[i].TreeDistr);
		}
		
		
	}
	
	// give to XN structure for (output)
	pPl->pRoot->fTotalLength = TotalLength;
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
};