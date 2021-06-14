#include "libtreemix.h"
#include <math.h>

int libtreemix_NitrogenMineralisation(libtreemix *self)
{
	/* Description */
	/*
	 * 
	 * 
	*/
	
	/* Abbreviations */
	//expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW 	= xpn->pPl->pPltWater;
	//PGENOTYPE	pGen 	= xpn->pPl->pGenotype;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	//PPLTNITROGEN	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY	pCan 	= xpn->pPl->pCanopy;
	//PWEATHER 	pWe  	= xpn->pCl->pWeather;
	//PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PROOT		pRt		= xpn->pPl->pRoot;
	//PSWATER		pSWL	= xpn->pSo->pSWater;
	//PWLAYER	   	pSLW	= xpn->pWa->pWLayer;
	//PCLAYER     pSLN	= xpn->pCh->pCLayer;	//chemistry
	//PLAYERROOT	pLR	= xpn->pPl->pRoot->pLayerRoot;
	
	/* Variables */
	int i;	// species looping variable
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	int libtreemix_CalcLitterDecomposition(libtreemix *self, int i);
	int libtreemix_CalcNitrogenRates(libtreemix *self, int i);
	int libtreemix_CalcMineralization_FAGUS(libtreemix *self);
	
	/****************************************************************************************************************/
	
	if(self->conf.Soil_Processes == 1)
	{
		for(i=0; i<self->conf.Species_Count; i++)
		{
			libtreemix_CalcLitterDecomposition(self, i);
			libtreemix_CalcNitrogenRates(self, i);
		}
	}
	else if(self->conf.Soil_Processes == 2)
	{		
		libtreemix_CalcMineralization_FAGUS(self);
	}
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
}

int libtreemix_CalcLitterDecomposition(libtreemix *self, int i)
{	
	double CN;	//Carbon-Nitrogen Ratio [kgC/kgN]
	double LitTempFac;
	double HumTempFac;

	CN = self->plant[i].CSOM/self->plant[i].NSOM;	//C-N Ratio in Soil Organic Matter

	LitTempFac = self->plant[i].kTRespSoil;
	HumTempFac = self->plant[i].kTRespSoil;

	self->plant[i].LitHumif = self->plant[i].Humif*self->plant[i].Decomp*LitTempFac*self->plant[i].CLit/1000.0; 		// eq. 114
	self->plant[i].LitDecom = (1-self->plant[i].Humif)*self->plant[i].Decomp*LitTempFac*self->plant[i].CLit/1000.0; 	// eq. 115
	self->plant[i].NTransHumif = self->plant[i].LitHumif/CN;															// eq. 116
	self->plant[i].HumDecom = self->plant[i].HuMiner*HumTempFac*self->plant[i].CSOM/1000.0;								// eq. 117
	self->plant[i].NLitMiner = self->plant[i].Decomp*HumTempFac*self->plant[i].NLit/1000.0-self->plant[i].NTransHumif;	// eq. 118
	self->plant[i].NHumMiner = self->plant[i].HuMiner*HumTempFac*self->plant[i].NSOM/1000.0;							// eq. 119
	
	return RET_SUCCESS;
}

int libtreemix_CalcNitrogenRates(libtreemix *self, int i)
{
	self->plant[i].NLeach = self->plant[i].Leach*self->plant[i].NSoil/1000.0;	// eq. 120
	
	if(self->silv[i].StandAlive == TRUE)
	{
		self->plant[i].NFix = self->plant[i].NFixMax*self->plant[i].kTRespSoil*(self->plant[i].LAI/self->plant[i].LAImax)*self->plant[i].CrArPot;	// eq. 121
	}
	else
	{
		self->plant[i].NFix = 0.0;
	}
	
	return RET_SUCCESS;
}

int libtreemix_CalcMineralization_FAGUS(libtreemix *self)
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
	//PPLTNITROGEN	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY	pCan 	= xpn->pPl->pCanopy;
	//PWEATHER 	pWe  	= xpn->pCl->pWeather;
	PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PROOT		pRt		= xpn->pPl->pRoot;
	PSWATER		pSW	= xpn->pSo->pSWater;
	PWLAYER	   	pWL	= xpn->pWa->pWLayer;
	//PCLAYER     pSLN	= xpn->pCh->pCLayer;	//chemistry
	PLAYERROOT	pLR	= xpn->pPl->pRoot->pLayerRoot;
	PCLAYER		pChL =  xpn->pCh->pCLayer;
	PHLAYER		pHL = xpn->pHe->pHLayer;
	
	/* Variables */
	int i;	// soil layer looping variable
	int j;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double TotalN, CN;			// species independent
		// fresh organic N and C from
	double FOCBrSt, FONBrSt;	//Branches and Stems
	double FOCLfFr, FONLfFr;	//Leaves and Fruits
	double FOCFiRt, FONFiRt;	//Fine Roots	
	double FOCGrRt, FONGrRt;	//Gross Roots
	double Rdecr, DecR;
	double DMinR;
	double rnac1, rnac2, rnac3, rnac4;
	double NNOM;				// mineralized nitrogen
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	pChL = pChL->pNext;
	pWL  = pWL->pNext;
	pSW  = pSW->pNext;
	pHL  = pHL->pNext;
	pSL  = pSL->pNext;
	pLR  = pLR->pNext;

	for(i=1; i<=xpn->pSo->iLayers-2; i++)
	{
		// set temporary variables to zero for each layer loop
		double FMinerR = 0.0;
		double afCOrgFOMFrac = 0.0;
		double ChemX = 0.0;
		
		// ----- Reduction through moisture -----
		pChL->fNH4NitrCoeff = (float)((pWL->fContAct - pSW->fContPWP*0.5)/(pSW->fContFK - pSW->fContPWP*0.5));
		if(pChL->fNH4NitrCoeff < (float)0.0){
			pChL->fNH4NitrCoeff = (float)0.0;
		}
		else if(pChL->fNH4NitrCoeff>(float)1.0){
			pChL->fNH4NitrCoeff = (float)1.0;
		}

		// ----- Reduction through temperature -----
		pChL->fTempCoeff = (0.000977*pHL->fSoilTemp*pHL->fSoilTemp);
		if(pHL->fSoilTemp < (float)0.0){
			pChL->fTempCoeff = (float)0.0;
		}
		else if(pChL->fTempCoeff > (float)1.0){
			pChL->fTempCoeff = (float)1.0;
		}
		
		// fNH4N und fNO3N [kg/ha], fBulkDens [kg/dm³] and fThickness [mm]. Where from the 2ppm??? ppm = [g(N)/t(Soil)]!
		TotalN = max(0.0, pChL->fNH4N + pChL->fNO3N - (2.0*pSL->fBulkDens*0.1*pSL->fThickness/10.0));
		if(TotalN <= 0.0){
			TotalN = 0.0;
		}

		// TOP LAYER!
		if(i==1)
		{
			// Influence of the C/N-Ratio in the Soil
			// CN = self->plant[i].FOCLfFr/(self->plant[i].FONLfFr + TotalN) ; 
			CN = xpn->pCh->pCProfile->fCLeafLitterSurf/(xpn->pCh->pCProfile->fNLeafLitterSurf + TotalN);			
			
			pChL->fCNCoeff = exp(-0.693*(CN-35.0)/35.0);
			if(pChL->fCNCoeff > (float)1.0)
			{
				pChL->fCNCoeff = (float)1.0;
			}
			
				
			// Do for all species!!!
			for(j=0; j<self->conf.Species_Count; j++)
			{
				self->plant[j].CNratio[i] = CN;
				self->plant[j].CNcoeff[i] = pChL->fCNCoeff;
				
				/*  Leaves and Fruits Litter: */
				Rdecr = self->plant[i].LeafMinLig;			
				if((self->plant[j].FOCLfFr/self->plant[j].initFOCLfS) > 0.8){
					Rdecr = self->plant[j].LeafMinCHy;
				}
				else if(((self->plant[j].FOCLfFr/self->plant[j].initFOCLfS) <= 0.8f) && ((self->plant[j].FOCLfFr/self->plant[j].initFOCLfS) > 0.25f)){
					Rdecr = self->plant[j].LeafMinCel ;
				}
				else if((self->plant[j].FOCLfFr/self->plant[j].initFOCLfS) < 0.0f){
					Rdecr = 0.0;
				}

				DecR = (double)(Rdecr * pChL->fNH4NitrCoeff * pChL->fTempCoeff * pChL->fCNCoeff);
				
				// Decomposition (Abbau):
				FOCLfFr = (DecR * self->plant[j].FOCLfFr);	//leaf litter C mineralization, decay
				FONLfFr = (DecR * self->plant[j].FONLfFr);	//leaf litter N mineralization

				self->plant[j].FOCLfFr -= FOCLfFr;
				self->plant[j].FONLfFr -= FONLfFr;
				
				//20% of mineralized N to Humus
				pSL->fNHumus += (float)(0.2 * (FONLfFr * self->plant[j].TreeDistr));	

				//immobilisation:
				rnac1 = min(TotalN, 0.02*FOCLfFr);

				TotalN -= (rnac1*self->plant[j].TreeDistr);
				self->plant[j].FONLfFr += rnac1;	//actualize N-pool of fresh org. matter

				/*  Stem and Branches/Twigs Litter  */
				//mineralization (kg/ha/d)*dt => [kg/ha*timestep]:
				FOCBrSt = (self->plant[j].SoilMinLit/365.25 * self->plant[j].FOCBrSt * min(pChL->fNH4NitrCoeff, pChL->fTempCoeff))*dt;
				FONBrSt = (self->plant[j].SoilMinLit/365.25 * self->plant[j].FONBrSt * min(pChL->fNH4NitrCoeff, pChL->fTempCoeff))*dt;

				self->plant[j].FOCBrSt -= FOCBrSt;
				self->plant[j].FONBrSt -= FONBrSt;

				pSL->fNHumus += (float)(0.2 * FONBrSt * self->plant[j].TreeDistr); //20% of mineralized N to Humus
				pSL->fCHumus += (float)(0.2 * FOCBrSt * self->plant[j].TreeDistr); //20% of mineralized C to Humus

				//immobilisation:
				rnac2 = min(TotalN, 0.02*FOCBrSt);

				TotalN -= (rnac2*self->plant[j].TreeDistr);
				self->plant[j].FONBrSt += rnac2; //actualize N-pools of fresh org. matter
			}
		}
		
		// ALL SOIL LAYERS!!
		// Do for all species
		for(j=0; j<self->conf.Species_Count; j++)
		{
			/*  Fine Roots Litter  */
			//mineralization (kg/ha/d)*dt => [kg/ha*timestep]:
			FOCFiRt = ((self->plant[j].SoilMinLit/365.25)*self->plant[j].FOCFiRt[i]*min(pChL->fNH4NitrCoeff, pChL->fTempCoeff))*dt;
			FONFiRt = ((self->plant[j].SoilMinLit/365.25)*self->plant[j].FONFiRt[i]*min(pChL->fNH4NitrCoeff, pChL->fTempCoeff))*dt;
		
			self->plant[j].FOCFiRt[i] -= FOCFiRt;
			self->plant[j].FONFiRt[i] -= FONFiRt;

			pSL->fCHumus += (float)(0.2 * FOCFiRt * self->plant[j].TreeDistr);	//20% to C-Humus
			pSL->fNHumus += (float)(0.2 * FONFiRt * self->plant[j].TreeDistr);	//20% of mineralized N to N-Humus

			//immobilisation:
			rnac4 = min(TotalN, (FOCFiRt*0.02));

			TotalN -= (rnac4*self->plant[j].TreeDistr);
			self->plant[j].FONFiRt[i] += rnac4; //actualize N-pools of fresh org. matter

			/*  Gross Roots Litter  */
			//mineralization (kg/ha/d)*dt => [kg/ha*timestep]:
			FOCGrRt = ((self->plant[j].SoilMinLit/365.25)*self->plant[j].FOCGrRt[i]*min(pChL->fNH4NitrCoeff, pChL->fTempCoeff))*dt;
			FONGrRt = ((self->plant[j].SoilMinLit/365.25)*self->plant[j].FONGrRt[i]*min(pChL->fNH4NitrCoeff, pChL->fTempCoeff))*dt;

			self->plant[j].FOCGrRt[i] -= FOCGrRt;
			self->plant[j].FONGrRt[i] -= FONGrRt;
		
			pSL->fCHumus += (float)(0.2 * self->plant[j].FOCGrRt[i] * self->plant[j].TreeDistr);	//20% to C-Humus
			pSL->fNHumus += (float)(0.2 * self->plant[j].FONGrRt[i] * self->plant[j].TreeDistr);	//20% of mineralized N to N-Humus

			//immobilisation:
			rnac3 = min(TotalN, FOCGrRt*0.02);

			TotalN -= (rnac3 * self->plant[j].TreeDistr);
			self->plant[j].FONGrRt[i] += rnac3; //actualize N-pools of fresh org. matter

			/*  MINERALIZATION OF SOIL HUMUS CARBON AND NITROGEN  */
			DMinR = self->plant[j].SoilMinHum/365.25*dt; //SG 2006/03/20 DMOD (1/a)-->(1/d)*dt--->(1/timestep)!!!!!
		 
			self->plant[j].CHumMinerR		= (double)(DMinR * pChL->fNH4NitrCoeff * pChL->fTempCoeff * pSL->fCHumus);
			self->plant[j].HumMinerRel[i]	= (double)(DMinR * pChL->fNH4NitrCoeff * pChL->fTempCoeff * pSL->fNHumus);

			pSL->fCHumus -= (float)(self->plant[j].CHumMinerR * self->plant[j].TreeDistr);
			pSL->fNHumus -= (float)(self->plant[j].HumMinerRel[i] * self->plant[j].TreeDistr); //pChL->fHumusMinerR;

			/*  BALANCING MINERALIZATION  */
			//total N-release from humus and litter, species-specific
			if(i==1)
			{
				self->plant[j].LitMinerRel[i] = FONLfFr + FONBrSt + FONFiRt + FONGrRt;
			
				NNOM = self->plant[j].HumMinerRel[i] + (0.8*self->plant[j].LitMinerRel[i]) - (rnac1+rnac2+rnac3+rnac4);

				//for N-Balance
				self->plant[j].NImmobR[i] = rnac1+rnac2+rnac3+rnac4 + (0.2*self->plant[j].LitMinerRel[i]);
				//pChL->fNImmobR = (float)(rnac1+rnac2+rnac3+rnac4 + 0.2*(pChL->fLitterMinerR));

				xpn->pCh->pCProfile->fCLeafLitterSurf	-= (float)(self->plant[j].TreeDistr * FOCLfFr);
				xpn->pCh->pCProfile->fNLeafLitterSurf	-= (float)(self->plant[j].TreeDistr * (FONLfFr - rnac1));
			
				xpn->pCh->pCProfile->fCBranchLitterSurf	-= (float)(self->plant[j].TreeDistr * (FOCBrSt * self->plant[j].FiBrWdFr));
				xpn->pCh->pCProfile->fNBranchLitterSurf	-= (float)(self->plant[j].TreeDistr * ((FONBrSt-rnac2) * self->plant[j].FiBrWdFr));
			
				xpn->pCh->pCProfile->fCStemLitterSurf	-= (float)(self->plant[j].TreeDistr * (FOCBrSt * (1.0-self->plant[j].FiBrWdFr)));
				xpn->pCh->pCProfile->fNStemLitterSurf	-= (float)(self->plant[j].TreeDistr * ((FONBrSt-rnac2) * (1.0-self->plant[j].FiBrWdFr)));
				
				//Hong added on 20181016 for C-balance			
				xpn->pCh->pCProfile->fCLitterSurf =xpn->pCh->pCProfile->fCLeafLitterSurf+ xpn->pCh->pCProfile->fCBranchLitterSurf + xpn->pCh->pCProfile->fCStemLitterSurf;//End of Hong, kein Einfluss

				pChL->fCFineRootLitter	-= (float)(self->plant[j].TreeDistr * FOCFiRt);
				pChL->fNFineRootLitter	-= (float)(self->plant[j].TreeDistr * (FONFiRt - rnac4));

				pChL->fCGrossRootLitter	-= (float)(self->plant[j].TreeDistr * FOCGrRt); 
				pChL->fNGrossRootLitter	-= (float)(self->plant[j].TreeDistr * (FONGrRt - rnac3));			

				pChL->fCLitter -= (float)(self->plant[j].TreeDistr * (FOCLfFr+FOCBrSt+FOCFiRt+FOCGrRt));
				pChL->fNLitter -= (float)(self->plant[j].TreeDistr * (FONLfFr+FONBrSt+FONFiRt+FONGrRt - rnac1-rnac2-rnac3-rnac4));
			}
			else if(i>1)
			{
				self->plant[j].LitMinerRel[i] = FONFiRt + FONGrRt;

				NNOM = self->plant[j].HumMinerRel[i] + (0.8*self->plant[j].LitMinerRel[i]) - (rnac3+rnac4);

				//for N-Balance
				self->plant[j].NImmobR[i] = rnac3+rnac4 + (0.2*self->plant[j].LitMinerRel[i]);
				//pChL->fNImmobR = (float)(rnac3+rnac4 + 0.2*(pChL->fLitterMinerR));

				pChL->fCFineRootLitter -= (self->plant[j].TreeDistr * FOCFiRt); 
				pChL->fNFineRootLitter -= (self->plant[j].TreeDistr * (FONFiRt - rnac4));
			
				pChL->fCGrossRootLitter -= (self->plant[j].TreeDistr * FOCGrRt); 
				pChL->fNGrossRootLitter -= (self->plant[j].TreeDistr * (FONGrRt - rnac3));

				pChL->fCLitter -= (self->plant[j].TreeDistr * (FOCFiRt + FOCGrRt));
				pChL->fNLitter -= (self->plant[j].TreeDistr * (FONFiRt + FONGrRt - rnac3-rnac4));  
			}

			// mineralized nitrogen
			ChemX += (NNOM*self->plant[j].TreeDistr);

			// denitrification: transferring the Pools FOM to afCOrgFOMFrac[1]
			afCOrgFOMFrac += (self->plant[j].FOCFiRt[i]*self->plant[j].TreeDistr);

			FMinerR += (self->plant[j].TreeDistr * (self->plant[j].LitMinerRel[i] + self->plant[j].HumMinerRel[i]));
			//pChL->fMinerR = pChL->fLitterMinerR + pChL->fHumusMinerR;			
		}

		pChL->fMinerR = FMinerR;
		pChL->afCOrgFOMFrac[1] = afCOrgFOMFrac;
		
		// Ammonium in Layer L:
		if(ChemX > 0.0)
		{
			pChL->fChemX = ChemX;
			pChL->fNH4N += pChL->fChemX;
		}

		pChL = pChL->pNext;
		pWL  = pWL->pNext;
		pSW  = pSW->pNext;
		pHL  = pHL->pNext;
		pSL  = pSL->pNext;
		pLR  = pLR->pNext;
	}
	

	
	return RET_SUCCESS;
}