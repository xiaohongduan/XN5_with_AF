#include "libtreemix.h"
#include <math.h>
#include <string.h>

int libtreemix_Senescence(libtreemix *self)
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
	//PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PROOT		pRt		= xpn->pPl->pRoot;
	//PSWATER		pSWL	= xpn->pSo->pSWater;
	//PWLAYER	   	pSLW	= xpn->pWa->pWLayer;
	//PCLAYER     pSLN	= xpn->pCh->pCLayer;	//chemistry
	//PLAYERROOT	pLR	= xpn->pPl->pRoot->pLayerRoot;
	PCLAYER		pChL =  xpn->pCh->pCLayer;
	PWLAYER	   	pWL	= xpn->pWa->pWLayer;
	PSWATER		pSW	= xpn->pSo->pSWater;
	PHLAYER		pHL = xpn->pHe->pHLayer;
	PSLAYER		pSL		= xpn->pSo->pSLayer;
	PLAYERROOT	pLRt	= xpn->pPl->pRoot->pLayerRoot;
	PTIME		pTi		= xpn->pTi;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20180731
	PPLANT      pPl = xpn->pPl;//Added by Hong on 20181126
	/* Variables */
	int i; // soil layer looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	int j; // species looping variable
	double fCumDepth;
	
	/*Functions*/
	int libtreemix_IntegrateModelEquations(libtreemix *self);
	
	/****************************************************************************************************************/
	
	for(j=0; j<self->conf.Species_Count; j++)
	{
		if(pTi->pSimTime->year >= self->plant[j].MortInitYr)
		{
			self->plant[j].MortNormInit = self->plant[j].MortNorm;
			self->plant[j].MortCrowdInit = self->plant[j].MortCrowd;
		}
		
		
		/* Mortality, BOSSEL, 1996, ch 3.6, [1/yr] */
		if(self->silv[j].Crowding == 1.0)
		{
			self->plant[j].MortTotal = self->plant[j].MortCrowdInit + self->plant[j].MortNormInit;
		}
		else
		{
			self->plant[j].MortTotal = self->plant[j].MortNormInit;
		}

		//Biomass is lost in all carbon compartments of the tree and in the tree itself:
		self->plant[j].DeadTree			= self->plant[j].MortTotal * self->plant[j].density;				// eq. 67, [trees/ha*yr]
		self->plant[j].CLossLf 			= self->plant[j].MortTotal * (self->plant[j].CLfMass/1000.0);		// eq. 68, [tC/ha*yr]
		self->plant[j].CLossRt 			= self->plant[j].MortTotal * (self->plant[j].CFRtMass/1000.0);		// eq. 69, [tC/ha*yr]
		self->plant[j].CLossDeadTrees 	= self->plant[j].MortTotal * (self->plant[j].CAss/1000.0);			// eq. 70, [tc/ha*yr]
		self->plant[j].CLossFr 			= self->plant[j].MortTotal * (self->plant[j].CFrMass/1000.0);		// eq. 71, [tC/ha*yr]

		/* eq. 111: Litter Fall, BOSSEL, 1996, ch 3.14, [kgC/ha*yr] */
		self->plant[j].LitLf = 1000.0*((1.0-self->plant[j].CReloc)*self->plant[j].LeafDeathRate+self->plant[j].CLossLf);
		self->plant[j].LitRt = 1000.0*((1.0-self->plant[j].CReloc)*self->plant[j].RootDeathRate+self->plant[j].CLossRt);
		
		/* eq. 96: death of individual trees plus deadwood losses of all trees (dieback and losses of branches etc.) */
		self->plant[j].TotDeadWdLoss = 1000.0*(self->plant[j].DeadTree*self->plant[j].CTrMass)+(self->plant[j].DeadWdLoss*self->plant[j].CWdWeight);
		
		self->plant[j].LitFr = (1000.0*self->plant[j].FruitDeathRate+self->plant[j].CLossFr);
		
		// Litter input rate [tC/ha*yr], eq. 111
		self->plant[j].LitInputDecomposition =
			(
				((1.0-self->silv[j].LitterRemoval)*(self->plant[j].LitLf+self->plant[j].LitFr))
			   +((1.0-self->silv[j].LitterRemoval)*(1.0-self->plant[j].UgWdFr)*(self->plant[j].TotDeadWdLoss+self->plant[j].CLossDeadTrees))
			   +  self->plant[j].LitRt
			   +((self->plant[j].UgWdFr)*(self->plant[j].TotDeadWdLoss+self->plant[j].CLossDeadTrees))
			)/1000.0;
		
		// Litter is divided to the compartments, for Litter Pools in Mineralisation FAGUS
		self->plant[j].LitGrRt = self->plant[j].UgWdFr*self->plant[j].TotDeadWdLoss;
		self->plant[j].LitBr = (1.0-self->plant[j].UgWdFr)*self->plant[j].BrWdFr*self->plant[j].TotDeadWdLoss;
		self->plant[j].LitSt = (1.0-self->plant[j].UgWdFr)*(1.0-self->plant[j].BrWdFr)*self->plant[j].TotDeadWdLoss;

		/* parts of eq. 112 */
		self->plant[j].NLossLf = 1000.0*(self->plant[j].NLfDead*self->plant[j].LeafDeathRate+self->plant[j].NLf*self->plant[j].CLossLf);	
		self->plant[j].NLossFr = self->plant[j].NFr *(self->plant[j].FruitDeathRate+self->plant[j].CLossFr);	
		self->plant[j].NLossWd = 1000.0*(self->plant[j].NConcWd*(self->plant[j].DeadWdLoss*self->plant[j].CWdWeight/1000.0 + self->plant[j].DeadTree*self->plant[j].CTrMass));
		self->plant[j].NLossRt = 1000.0*((1.0-self->plant[j].NRelocLit) * self->plant[j].NFRt*self->plant[j].RootDeathRate + self->plant[j].NFRt*self->plant[j].CLossRt);
		self->plant[j].NLossGrRt = self->plant[j].UgWdFr*self->plant[j].NLossWd;
		self->plant[j].NLossSt = (1.0-self->plant[j].UgWdFr)*(1.0-self->plant[j].BrWdFr)*self->plant[j].NLossWd;		
		self->plant[j].NLossBr = (1.0-self->plant[j].UgWdFr)*self->plant[j].BrWdFr*self->plant[j].NLossWd;
		
		// rate at which tree loses nitrogen in litterfall
		self->plant[j].NLossTot = self->plant[j].NLossLf + self->plant[j].NLossRt + self->plant[j].NLossWd + self->plant[j].NLossFr;
		
		// !IF: Soil Processes == 1: available for decomposition: Nitrogen input rate to Litter [tN/ha*yr], eq. 112
		self->plant[j].NInputLit = ((1.0-self->silv[j].LitterRemoval)*(self->plant[j].NLossLf + self->plant[j].NLossFr)) + 
							((1.0-self->silv[j].LitterRemoval)*(1.0-self->plant[j].UgWdFr)*self->plant[j].NLossWd) + 
							self->plant[j].NLossRt + 
							self->plant[j].NLossGrRt;


		/* actualize the Soil Organic Matter Pools over soil depth
		 * this is depending on the distribution of the single cohorts: [TreeDistr] 
		 */
		pChL = xpn->pCh->pCLayer->pNext;
		pWL  = xpn->pWa->pWLayer->pNext;
		pSW  = xpn->pSo->pSWater->pNext;
		pHL  = xpn->pHe->pHLayer->pNext;
		pSL  = xpn->pSo->pSLayer->pNext;
		pLRt = xpn->pPl->pRoot->pLayerRoot;
		fCumDepth= 0.0;
		for(i=1; i <= xpn->pSo->iLayers-2; i++)
		{
			if(i == 1)
			{				
				// Fresh Organic Carbon and Nitrogen [kgC/ha*dt]
				self->plant[j].FOCLfFr += (self->plant[j].LitLf/365.25*dt);
				self->plant[j].FONLfFr += (self->plant[j].NLossLf/365.25*dt);
				self->plant[j].FOCBrSt += ((self->plant[j].LitSt+self->plant[j].LitBr)/365.25*dt);
				self->plant[j].FONBrSt += ((self->plant[j].NLossSt+self->plant[j].NLossBr)/365.25*dt);

				// Litter, depends on Species Distribution
				//pChL->fClitter  += (self->plant[j].TreeDistr*(self->plant[j].LitLf+self->plant[j].LitBr+self->plant[j].LitSt)/365.0*dt);
				//pChL->fNLitter  += (self->plant[j].TreeDistr*(self->plant[j].NLossLf+self->plant[j].NLossBr+self->plant[j].NLossSt)/365.0*dt);

				// for the CENTURY model [kgC/ha]:
				xpn->pCh->pCProfile->fCLeafLitterSurf += (self->plant[j].TreeDistr * self->plant[j].LitLf/365.25*dt);
				xpn->pCh->pCProfile->fNLeafLitterSurf += (self->plant[j].TreeDistr * self->plant[j].NLossLf/365.25*dt);			    				
	            xpn->pCh->pCProfile->fCNLeafLitterSurf = 1.0 /self->plant[j].NLfDead;//added by Hong on 26032019 for agroforestry
				             
				xpn->pCh->pCProfile->fCBranchLitterSurf += (self->plant[j].TreeDistr * self->plant[j].LitBr/365.25*dt);
				xpn->pCh->pCProfile->fNBranchLitterSurf += (self->plant[j].TreeDistr * self->plant[j].NLossBr/365.25*dt);                 							  				 
                xpn->pCh->pCProfile->fCNBranchLitterSurf =1.0 /self->plant[j].NWdDead;//added by Hong on 26032019 for agroforestry
				
				xpn->pCh->pCProfile->fCStemLitterSurf += (self->plant[j].TreeDistr * self->plant[j].LitSt/365.25*dt);
				xpn->pCh->pCProfile->fNStemLitterSurf += (self->plant[j].TreeDistr * self->plant[j].NLossSt/365.25*dt);
				xpn->pCh->pCProfile->fCNStemLitterSurf = 1.0 /self->plant[j].NWdDead;//added by Hong on 26032019 for agroforestry
												
				//Hong added on 20180731 for C-balance, not needed		
				pCB->dCInputSurf += (self->plant[j].TreeDistr * self->plant[j].LitLf/365.25*dt) + (self->plant[j].TreeDistr * self->plant[j].LitBr/365.25*dt) + (self->plant[j].TreeDistr * self->plant[j].LitSt/365.25*dt); 
                 
				xpn->pCh->pCProfile->fCLitterSurf += (self->plant[j].TreeDistr * self->plant[j].LitLf/365.25*dt) + (self->plant[j].TreeDistr * self->plant[j].LitBr/365.25*dt) + (self->plant[j].TreeDistr * self->plant[j].LitSt/365.25*dt); 
                 
				// XN output [tC/ha]:
				self->plant[j].CLitLf_Sum += (self->plant[j].TreeDistr * self->plant[j].LitLf/1000.0/365.25*dt);
				self->plant[j].CLitBr_Sum += (self->plant[j].TreeDistr * self->plant[j].LitBr/1000.0/365.25*dt);
				self->plant[j].CLitSt_Sum += (self->plant[j].TreeDistr * self->plant[j].LitSt/1000.0/365.25*dt);
				
				//Added by Hong on 20181126
				pPl->pBiomass->fDeadLeafWeight_dt =(self->plant[j].TreeDistr * self->plant[j].LitLf/365.25*dt); //for agroforest
				pPl->pBiomass->fDeadLeafWeight +=pPl->pBiomass->fDeadLeafWeight_dt;//[kg/ha]
				
				pPl->pBiomass->fDeadStemWeight_dt =(self->plant[j].TreeDistr * self->plant[j].LitSt/365.25*dt);
				pPl->pBiomass->fDeadStemWeight +=pPl->pBiomass->fDeadStemWeight_dt;//[kg/ha]
				
				pPl->pBiomass->fDeadBranchWeight_dt = (self->plant[j].TreeDistr * self->plant[j].LitBr/365.25*dt);
				pPl->pBiomass->fDeadBranchWeight +=pPl->pBiomass->fDeadBranchWeight_dt;
				//End of Hong
				
				self->plant[j].CSurfaceLitterInputCENTURY += 	(self->plant[j].TreeDistr * self->plant[j].LitLf/365.25*dt) + 
																(self->plant[j].TreeDistr * self->plant[j].LitBr/365.25*dt) + 
																(self->plant[j].TreeDistr * self->plant[j].LitSt/365.25*dt);
			}

			/* root litter */
			if(self->plant[j].RtLengthDens[i-1] > 0.0)
			{
				// Litter, depends on Species Distribution
				pChL->fCLitter  += (self->plant[j].TreeDistr * (self->plant[j].LitRt+self->plant[j].LitGrRt)*self->plant[j].LyFc[i]/365.0*dt);
				pChL->fNLitter  += (self->plant[j].TreeDistr * (self->plant[j].NLossRt+self->plant[j].NLossGrRt)*self->plant[j].LyFc[i]/365.0*dt);

                //Added by Hong
				pLRt->fDeadFineRootWeight_dt = (self->plant[j].TreeDistr * self->plant[j].LitRt*self->plant[j].LyFc[i]/365.0*dt);//for agroforest
				pLRt->fDeadGrossRootWeight_dt = (self->plant[j].TreeDistr * self->plant[j].LitGrRt*self->plant[j].LyFc[i]/365.0*dt);
				
				pLRt->fDeadFineRootWeight_layer +=pLRt->fDeadFineRootWeight_dt;
				pLRt->fDeadGrossRootWeight_layer +=pLRt->fDeadGrossRootWeight_dt;
				
				xpn->pPl->pBiomass->fDeadRootWeight += pLRt->fDeadFineRootWeight_dt +pLRt->fDeadGrossRootWeight_dt;
				//End of Hong


				// for the CENTURY model:
				pChL->fCFineRootLitter += (self->plant[j].TreeDistr * (self->plant[j].LitRt*self->plant[j].LyFc[i])/365.25*dt);
				pChL->fNFineRootLitter += (self->plant[j].TreeDistr * self->plant[j].NLossRt*self->plant[j].LyFc[i]/365.25*dt);								
				pChL->fCNFineRootLitter = 1.0 /self->plant[j].NFRt;//added by Hong on 26032019 for agroforestry
								
				pChL->fCGrossRootLitter += (self->plant[j].TreeDistr * self->plant[j].LitGrRt*self->plant[j].LyFc[i]/365.25*dt);
				pChL->fNGrossRootLitter += (self->plant[j].TreeDistr * self->plant[j].NLossGrRt*self->plant[j].LyFc[i]/365.25*dt);				
				pChL->fCNGrossRootLitter = 1.0 /self->plant[j].NWdDead;//added by Hong on 26032019 for agroforestry				

				// used in nitrogen mineralization
				self->plant[j].FOCFiRt[i] += (self->plant[j].LitRt *self->plant[j].LyFc[i]/365.25*dt);
				self->plant[j].FONFiRt[i] += (self->plant[j].NLossRt *self->plant[j].LyFc[i]/365.25*dt);

				self->plant[j].FOCGrRt[i] += (self->plant[j].LitGrRt*self->plant[j].LyFc[i]/365.25*dt);
				self->plant[j].FONGrRt[i] += (self->plant[j].NLossGrRt*self->plant[j].LyFc[i]/365.25*dt);
				
			     //Hong added on 20180731 for C-balance			
				pCB->dCInputProfile += (self->plant[j].TreeDistr * (self->plant[j].LitRt*self->plant[j].LyFc[i])/365.25*dt)+ (self->plant[j].TreeDistr * self->plant[j].LitGrRt*self->plant[j].LyFc[i]/365.25*dt);				

				//Hong 20190507: balance for 0-30 cm profile:	
	           fCumDepth +=(double)0.1*pSL->fThickness; //cm
	           if (fCumDepth <=30.0)
	             {
					pCB->dCInputProfile_30 += (self->plant[j].TreeDistr * (self->plant[j].LitRt*self->plant[j].LyFc[i])/365.25*dt)+ (self->plant[j].TreeDistr * self->plant[j].LitGrRt*self->plant[j].LyFc[i]/365.25*dt);	
				 } 
			    //End of Hong
			}

			pChL = pChL->pNext;
			pWL  = pWL->pNext;
			pSW  = pSW->pNext;
			pHL  = pHL->pNext;
			pSL  = pSL->pNext;
			pLRt  = pLRt->pNext;
		}
		
		if(NewDay(xpn->pTi))
		{
			
		}
	}
	
	libtreemix_IntegrateModelEquations(self);
	
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
}

int libtreemix_IntegrateModelEquations(libtreemix *self)
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
	PBIOMASS		pBio = xpn->pPl->pBiomass;
	PPLTCARBON	pPlC = xpn->pPl->pPltCarbon;
	//PPLTNITROGEN	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY	pCan 	= xpn->pPl->pCanopy;
	//PWEATHER 	pWe  	= xpn->pCl->pWeather;
	//PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PROOT		pRt		= xpn->pPl->pRoot;
	//PSWATER		pSWL	= xpn->pSo->pSWater;
	//PWLAYER	   	pSLW	= xpn->pWa->pWLayer;
	//PCLAYER     pSLN	= xpn->pCh->pCLayer;	//chemistry
	//PLAYERROOT	pLR	= xpn->pPl->pRoot->pLayerRoot;
	//PCLAYER		pChL =  xpn->pCh->pCLayer;
	//PWLAYER	   	pWL	= xpn->pWa->pWLayer;
	//PSWATER		pSW	= xpn->pSo->pSWater;
	//PHLAYER		pHL = xpn->pHe->pHLayer;
	//PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PLAYERROOT	pLRt	= xpn->pPl->pRoot->pLayerRoot;
	
	/* Variables */
	int i; // species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep	
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	// set total stand variables to zero
	xpn->pPl->pCanopy->fPlantDensity = 0.0;
	pBio->fLeafWeight = 0.0;
	pBio->fRootWeight = 0.0;
	pBio->fFruitWeight = 0.0;
	pBio->fWoodWeight = 0.0;
	pPlC->fCAssimilatePool = 0.0;
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		/* Leaf Weight */
		//orig: pBio->fLeafWeight += (1000.0*(pBio->fLeafGrowR-pBio->fLeafDeathRate-PRATES->CLossLf)*dt);		
		self->plant[i].CLfMass += ((1000.0*(self->plant[i].LfGrR - self->plant[i].LeafDeathRate - self->plant[i].CLossLf))/365.25*dt);	// [1/yr] -> [1/d]
		//printf("%f %f %f \n", self->plant[i].LfGrR ,self->plant[i].LeafDeathRate , self->plant[i].CLossLf);
        pBio->fLeafWeight += (self->plant[i].TreeDistr * self->plant[i].CLfMass);
		if(pBio->fLeafWeight < 0.0){
			pBio->fLeafWeight = 0.0;
		}

		/* Root Weight */
		//pBio->fRootWeight += (1000.0*(pBio->fRootGrowR-pBio->fRootDeathRate-PRATES->CLossRt)*dt);		
		self->plant[i].CFRtMass += ((1000.0*(self->plant[i].RtGrR - self->plant[i].RootDeathRate - self->plant[i].CLossRt))/365.25*dt);
		pBio->fRootWeight += (self->plant[i].TreeDistr * self->plant[i].CFRtMass);
		if(pBio->fRootWeight < 0.0){
			pBio->fRootWeight = 0.0;
		}

		/* Fruit Weight */
		//pBio->fFruitWeight += (1000.0*(pBio->fFruitGrowR-pBio->fFruitDeathRate-PRATES->CLossFr)*dt);		
		self->plant[i].CFrMass += ((1000.0*(self->plant[i].FrGrR - self->plant[i].FruitDeathRate - self->plant[i].CLossFr))/365.25*dt);
		pBio->fFruitWeight += (self->plant[i].TreeDistr * self->plant[i].CFrMass);
		if(pBio->fFruitWeight < 0.0){
			pBio->fFruitWeight = 0.0;
		}

		/* eq. 8, Wood Weight */
		//pBio->fWoodWeight += (1000.0*(PRATES->Increment-PTREEVAR->LitWd/1000.0)*dt);		
		self->plant[i].CWdWeight += (1000.0*(self->plant[i].Increment - self->plant[i].TotDeadWdLoss/1000.0)*dt/365.25);
		pBio->fWoodWeight += (self->plant[i].TreeDistr * self->plant[i].CWdWeight);
		if(pBio->fWoodWeight < 0.0){
			pBio->fWoodWeight = 0.0;
		}
		
		//Added by Hong on 20180705: fStemWeight and fBranchWeight added for output graphics
        xpn->pPl->pBiomass->fStemWeight=pBio->fWoodWeight*((1.0-self->plant[i].UgWdFr)*(1.0-self->plant[i].BrWdFr));
		xpn->pPl->pBiomass->fBranchWeight=pBio->fWoodWeight*((1.0-self->plant[i].UgWdFr)*self->plant[i].BrWdFr);
		
		/* Assimilate Pool */		
		//pPlC->fCAssimilatePool += (1000.0*(PTREEVAR->PhCanopyNlim+PRATES->CRelocGain -	pPlC->fMaintRespR-PRATES->CGrowthTot-PRATES->CLossDeadTrees)*dt);
		
		// [tC/ha*yr]
		self->plant[i].CAssGainR = self->plant[i].PhCanopyNlim + self->plant[i].CRelocGain;	
		self->plant[i].CAssLossR = self->plant[i].MaintRespR + self->plant[i].CGrTot + self->plant[i].CLossDeadTrees;
		
		self->plant[i].CAssBalanceR = self->plant[i].CAssGainR - self->plant[i].CAssLossR;
		
		// [kgC/ha*timestep]
		self->plant[i].CAssGain_dt =  1000.0*self->plant[i].CAssGainR/365.25*dt;
		self->plant[i].CAssLoss_dt =  1000.0*self->plant[i].CAssLossR/365.25*dt;
		self->plant[i].CAssBalance_dt = self->plant[i].CAssGain_dt - self->plant[i].CAssLoss_dt;
		
		self->plant[i].CAss += self->plant[i].CAssBalance_dt;
		//self->plant[i].CAss += ((1000.0*(self->plant[i].PhCanopyNlim + self->plant[i].CRelocGain - 
		//						self->plant[i].MaintRespR - 
		//						self->plant[i].CGrTot - 
		//						self->plant[i].CLossDeadTrees))/365.0*dt);

		pPlC->fCAssimilatePool += (self->plant[i].TreeDistr * self->plant[i].CAss);
		pBio->fStovWeight = pPlC->fCAssimilatePool;

		/* Plant Morphology*/
		//pCan->fPlantHeight += (PRATES->IncrH*dt);
		self->plant[i].H				+= (self->plant[i].IncrH/365.25*dt);		
		self->plant[i].D				+= (self->plant[i].IncrD/365.25*dt);
       //FH 20180221 Write height to XN-structure variable
        xpn->pPl->pCanopy->fPlantHeight = self->plant[i].H;
		
//Out-noted by Hong, not necessary		
		//calculate new dbh for pinyon pine (used for spruce)
		/* for different calculations, the diameter at breast height is needed 
		* here the equation of Chojnacky (1999, technical note)
		* western journal of applied forestry, vol 14, no 1, january 1999 */
/*		self->plant[i].DBH = -6.818+(1.022*self->plant[i].D)+1.8879+1.8971+(-0.0339*self->plant[i].D);	//[cm]
		self->plant[i].DBH = self->plant[i].DBH/100.0;
		if(self->plant[i].DBH <= 0.0)
		{
			self->plant[i].DBH = 0.01;
		}
*/
//End of Hong	
		//calculate new form factor
		/* for spruce: form factor is being calculated from HD-ratio 
		 * according to Forschungszentrum Waldökosysteme, 1995 */	
		if(self->plant[i].Form_var == 1)
		{
			self->plant[i].Form = (self->plant[i].H/(self->plant[i].D*600.0)) + 0.32;
		}
		
		self->plant[i].density			-= (self->plant[i].DeadTree/365.25*dt);		// [trees/timestep]

		xpn->pPl->pCanopy->fLAI = self->plant[0].LAI;	//ATTN: only the first LAI is given to the Output Module
		xpn->pPl->pCanopy->fPlantDensity += self->plant[i].density;	// sum up over all species
		

		if(self->conf.Soil_Processes == 1)
		{
			self->plant[i].CLit += ((1000.0*(self->plant[i].LitInputDecomposition-self->plant[i].LitDecom-self->plant[i].LitHumif))/365.25*dt);
			self->plant[i].NLit += ((1000.0*(self->plant[i].NInputLit-self->plant[i].NTransHumif-self->plant[i].NLitMiner))/365.25*dt);
		
			self->plant[i].CSOM += ((1000.0*(self->plant[i].LitHumif-self->plant[i].HumDecom))/365.25*dt);
			self->plant[i].NSOM += ((1000.0*(self->plant[i].NTransHumif-self->plant[i].NHumMiner))/365.25*dt);

			self->plant[i].NSoil += ((1000.0*(self->plant[i].NDepos+self->plant[i].NFix+self->plant[i].NHumMiner+self->plant[i].NLitMiner-
				self->plant[i].NLeach-self->plant[i].NUpActR))/365.25*dt);
		}
		
		// [kgN/ha]
		self->plant[i].NWd += ((1000.0*(self->plant[i].Increment*((self->plant[i].b*self->plant[i].NWdResp) + ((1.0-self->plant[i].b)*self->plant[i].NWdDead)) 
							- self->plant[i].NConcWd*(self->plant[i].DeadTree*self->plant[i].CTrMass+self->plant[i].DeadWdLoss*self->plant[i].CWdWeight/1000.0))));
		
		self->plant[i].NWdTot += (self->plant[i].TreeDistr * self->plant[i].NWd);
	
	}

	//calculate actual species distribution and daily variables
	for(i=0; i<self->conf.Species_Count; i++)
	{
		self->plant[i].TreeDistr = self->plant[i].density/xpn->pPl->pCanopy->fPlantDensity;
		
		
		
		if(NewDay(xpn->pTi))
		{
			self->plant[i].CAssBalance_Day = 0.0;
			self->plant[i].CAssGain_Day = 0.0;
			self->plant[i].CAssLoss_Day = 0.0;
		}
		self->plant[i].CAssBalance_Day 	+= self->plant[i].CAssBalance_dt;	// [kgC/ha*d]
		self->plant[i].CAssGain_Day		+= self->plant[i].CAssGain_dt;		// [kgC/ha*d]
		self->plant[i].CAssLoss_Day		+= self->plant[i].CAssLoss_dt;		// [kgC/ha*d]
		
	}
	
	return RET_SUCCESS;
}