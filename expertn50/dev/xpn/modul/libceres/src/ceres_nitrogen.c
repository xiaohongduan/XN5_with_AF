#include "ceres_nitrogen.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int ceres_NitrogenConcentrationLimits(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	double ZS2;
	//double ZS3;
	//double ZS4;
	double  P1V, fStage;


	PBIOMASS            pBiom = pPl->pBiomass;
	PDEVELOP            pDev  = pPl->pDevelop;
	PPLTNITROGEN        pPltN = pPl->pPltNitrogen;

	P1V=(double)pPl->pGenotype->iVernCoeff;
	fStage = (double)0.1 * pDev->fDevStage;

	//============================================================================
	//Tops Nitrogen Concentration of the Plant - Stage dependant
	//============================================================================
	ZS2=(double)(fStage*fStage);
	//ZS3=(double)(fStage*fStage*fStage);
	//ZS4=(double)(fStage*fStage*fStage*fStage);


	//Wheat, Barley:
	if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
	    ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
		if (P1V>=(double)0.03)
			pPltN->fTopsOptConc=(double)(-5.0112400-6.350677*(double)fStage
			                             +14.95784*sqrt((double)fStage)
			                             +0.2238197*ZS2);
		else
			pPltN->fTopsOptConc=(double)( 7.4531813-1.7907829*(double)fStage
			                              +0.6092849*sqrt((double)fStage)
			                              +0.0933967*ZS2);
//      if (pDev->fDevStage>(double)6.0)
//              pPltN->fTopsOptConc -=(pDev->fDevStage-(double)6.0)*(double)0.140;

		pPltN->fTopsOptConc     =pPltN->fTopsOptConc*(double)0.01;
		//pPltN->fTopsMinConc   =(double)((2.97-0.455*(double)pDev->fStageXCERES)*0.01);
		// SPASS:
		pPltN->fTopsMinConc     = (double)0.005;

		if (pDev->iStageCERES>=5) {
			pPltN->fVegActConc              =pPltN->fTopsActConc;
			pPltN->fVegMinConc      =pPltN->fTopsMinConc;
		}

		//============================================================================
		//Roots Nitrogen Concentration of the Plant - Stage dependant
		//============================================================================
		pPltN->fRootOptConc     =(double)((2.10-0.14*sqrt((double)fStage))*0.01);
		pPltN->fRootMinConc     =((double)0.75)*pPltN->fRootOptConc;
	}//end wheat, barley

	//Maize:
	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
		pPl->pPltNitrogen->fTopsOptConc=(double)(exp(1.52-0.160*self->fXDevStage)/100.0);

		if (pPl->pDevelop->fStageXCERES<(double)4.0)
			pPl->pPltNitrogen->fTopsMinConc=(double)((1.25-0.20*self->fXDevStage)/100.0);
		else
			pPl->pPltNitrogen->fTopsMinConc=(double)0.0045;


		pPl->pPltNitrogen->fRootOptConc=(double)(1.06/100.0);


		self->fNFAC=(double)1.0-(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsActConc)
		            /(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsMinConc);

		if (self->fNFAC>(double)1.0) self->fNFAC=(double)1.0;

		if ((pPl->pDevelop->iStageCERES==3)||(pPl->pDevelop->iStageCERES==4))
			self->fNFAC=(double)(1.0-1.80*exp(-3.5*(double)self->fNFAC));

		if (self->fNFAC<(double)0.001)  self->fNFAC=(double)0.001;


		pPl->pPltNitrogen->fNStressPhoto=(double)1.0;
		pPl->pPltNitrogen->fNStressLeaf=(double)1.0;
		pPl->pPltNitrogen->fNStressTiller=(double)1.0;

		pPl->pPltNitrogen->fNStressLeaf=self->fNFAC;

		if (pPl->pPltNitrogen->fNStressFac<(double)0.8)
			pPl->pPltNitrogen->fNStressTiller=(double)0.2+self->fNFAC;

		pPl->pPltNitrogen->fNStressPhoto=self->fNFAC*(double)1.2+(double)0.2;

		if (self->fNFAC>(double)0.5)
			pPl->pPltNitrogen->fNStressPhoto=(double)0.4*self->fNFAC+(double)0.6;

		/*c  If(Nfac.lt.0.7)ndef1=nfac*(nfac+0.2) */

//      if (pPl->pPltNitrogen->fNStressPhoto>(double)1.0) pPl->pPltNitrogen->fNStressPhoto=(double)1.0;
		if (pPl->pPltNitrogen->fNStressLeaf>(double)1.0) pPl->pPltNitrogen->fNStressLeaf=(double)1.0;
		if (pPl->pPltNitrogen->fNStressTiller>(double)1.0) pPl->pPltNitrogen->fNStressTiller=(double)1.0;
	}//end maize

	//Sunflower:
	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		//optimal nitrogen concentrations
		if (pPl->pGenotype->OptLvNc[0].fInput==(double)0.0)
			pPltN->fLeafOptConc = (double)(0.0294*exp(-0.326*self->fXDevStage) + 0.0326);
		else
			pPltN->fLeafOptConc = ceres_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptLvNc);

		if (pPl->pGenotype->OptStNc[0].fInput==(double)0.0)
			pPltN->fStemOptConc = (double)(0.0329*exp(-0.516*self->fXDevStage) + 0.0125);
		else
			pPltN->fStemOptConc = ceres_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptStNc);

		if (pPl->pGenotype->OptRtNc[0].fInput==(double)0.0)
			pPltN->fRootOptConc = (double)(0.0361*exp(-0.521*self->fXDevStage) + 0.0105);
		else
			pPltN->fRootOptConc = ceres_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptRtNc);

		//für Head kein Eintrag in Sunflower.gtp
		self->fNHeadOptConc = (double)(0.2137*exp(-0.6*self->fXDevStage) + 0.016);


		//minimal nitrogen concentrations
		if (pPl->pGenotype->MinLvNc[0].fInput==(double)0.0)
			pPltN->fLeafMinConc = pPltN->fLeafOptConc - (double)0.02;
		else
			pPltN->fLeafMinConc = ceres_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinLvNc);

		if (pPl->pGenotype->MinStNc[0].fInput==(double)0.0)
			pPltN->fStemMinConc = pPltN->fStemOptConc - (double)0.0112;
		else
			pPltN->fStemMinConc = ceres_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinStNc);

		if (pPl->pGenotype->MinRtNc[0].fInput==(double)0.0)
			pPltN->fRootMinConc = pPltN->fRootOptConc - (double)0.0062;
		else
			pPltN->fRootMinConc = ceres_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinRtNc);

		//für Head kein Eintrag in Sunflower.gtp
		self->fNHeadMinConc = self->fNHeadOptConc - (double)0.00895;

		pPltN->fTopsOptConc = (pPltN->fLeafOptConc*pBiom->fGreenLeafWeight +
		                       pPltN->fStemOptConc*pBiom->fStemWeight + self->fNHeadOptConc*self->fHeadWeight)
		                      /(pBiom->fStovWeight - pBiom->fSenesLeafWeight);

		pPltN->fTopsMinConc = (pPltN->fLeafMinConc*pBiom->fGreenLeafWeight +
		                       pPltN->fStemMinConc*pBiom->fStemWeight + self->fNHeadMinConc*self->fHeadWeight)
		                      /(pBiom->fStovWeight - pBiom->fSenesLeafWeight);

	}//end sunflower


	pPltN->fTopsOptConc     =max((double)0.0,pPltN->fTopsOptConc);
	pPltN->fTopsMinConc     =max((double)0.0,pPltN->fTopsMinConc);
	pPltN->fLeafOptConc     =max((double)0.0,pPltN->fLeafOptConc);
	pPltN->fLeafMinConc     =max((double)0.0,pPltN->fLeafMinConc);
	pPltN->fStemOptConc     =max((double)0.0,pPltN->fStemOptConc);
	pPltN->fStemMinConc     =max((double)0.0,pPltN->fStemMinConc);
	pPltN->fRootOptConc     =max((double)0.0,pPltN->fRootOptConc);
	pPltN->fRootMinConc     =max((double)0.0,pPltN->fRootMinConc);
	self->fNHeadOptConc           =max((double)0.0,self->fNHeadOptConc);
	self->fNHeadMinConc           =max((double)0.0,self->fNHeadMinConc);


	return RET_SUCCESS;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double DLL AFGENERATOR(double fInput, RESPONSE* pfResp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function interpolates linearly a value from a RESPONSE table using input fInput
//Inputs:	1. fInput	- the input value (-)
//			2. pfResp	- Pointer to the first element of the given RESPONSE table
//Outputs:	The interpolated value at fInput (-)
//Functions Called:
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double ceres_AFGENERATOR(double fInput, RESPONSE* pResp)
{

	RESPONSE *pfResp; //, *pResp1;

	double	 fSlope,fOutput;
	int	  	 nPoints,i;

	fOutput=0.0;

	pfResp = pResp;

	if (pResp[0].iInLen >= pResp[0].iOutLen)
		nPoints = pfResp[0].iOutLen;
	else
		nPoints = pfResp[0].iInLen;

	if (fInput<=pfResp[0].fInput)
		return pfResp[0].fOutput;

	if (nPoints > 0) {
		if (fInput>=pfResp[nPoints-1].fInput)
			return pfResp[nPoints-1].fOutput;


		for (i=0; i<nPoints-1; i++) {
			if (	((fInput>=pfResp[i].fInput)&&(fInput<pfResp[i+1].fInput))
			        ||  ((fInput<=pfResp[i].fInput)&&(fInput>pfResp[i+1].fInput))   )
				break;
		}

		fSlope = (pfResp[i+1].fOutput- pfResp[i].fOutput)/(pfResp[i+1].fInput-pfResp[i].fInput);
		fOutput= pfResp[i].fOutput + fSlope*(fInput-pfResp[i].fInput);
	}

	return fOutput;
}

int ceres_PlantNitrogenDemand(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	double                  fNewGrwNdem; //fTRNLOS;
	PBIOMASS                pBiom=pPl->pBiomass;
	PPLTNITROGEN    pPltN=pPl->pPltNitrogen;

	if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
	    ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)
	    ||(strcmp(pPl->pGenotype->acCropCode,"MZ")==0)) {

		if(strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
			//===========================================
			//              Potential Shoot and Root Growth
			//===========================================
			self->fPotShootGrowRate = self->fPotBiomGrowRate
			                          *((double)1.0-pBiom->fRootGrowR/pBiom->fBiomGrowR);
			self->fPotRootGrowRate  = self->fPotBiomGrowRate
			                          *pBiom->fRootGrowR/pBiom->fBiomGrowR;
		}


		//Nitrogen demand of potential new growth of tops fNewGrwNdem(g N/plant)
		fNewGrwNdem=pBiom->fMaxShootGrowR*pPltN->fTopsOptConc;

		if (pPl->pDevelop->fStageXCERES<=(double)1.2)
			fNewGrwNdem=(double)0.0;

		pBiom->fStovWeight=pBiom->fLeafWeight+pBiom->fStemWeight+self->fEarWeight;

		//Plant tops demand for nitrogen: fTNDEM(kg N/ha)
		pPltN->fShootDemand     =(double)max(0.0,pBiom->fStovWeight*
		                                     (pPltN->fTopsOptConc-pPltN->fTopsActConc)+fNewGrwNdem);

		//Plant root demand for nitrogen: fRNDEM(kg N/ha)
		pPltN->fRootDemand      =(double)max(0.0,pBiom->fRootWeight*(pPltN->fRootOptConc-
		                                     pPltN->fRootActConc)+self->fPotRootGrowRate*pPltN->fRootOptConc);

		//Crop nitrogen demand per ha: fANDEM(Kg N/ha)
		pPltN->fTotalDemand=(double)max(0.0,pPltN->fShootDemand+pPltN->fRootDemand);
	}

	//Sunflower:
	if(strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		//optimal N-concentration of pericarp and embryo
		self->fNEmbryoOptConc = (double)(0.0225+0.0200*self->fNFAC);
		self->fNPericarpOptConc = (double)(0.0050+0.0100*self->fNFAC);

		//reset N loss by roots due to root exsudation
		//fTRNLOS = (double)0.0;

		if(self->iJPEPE == 1 ) {
			self->fNHeadCont = (double)0.042* self->fHeadWeight;
			pPltN->fStemCont -= self->fNHeadCont;
			self->iJPEPE = 0;
		}

		// Actual N concentrations in plant parts before N uptake
		pPltN->fRootActConc=pPltN->fRootCont/pBiom->fRootWeight;
		pPltN->fStemActConc=pPltN->fStemCont/pBiom->fStemWeight;
		pPltN->fLeafActConc=self->fNGreenLeafCont/pBiom->fGreenLeafWeight;

		if(self->fHeadWeight > (double)0.0)
			self->fNHeadActConc=self->fNHeadCont/self->fHeadWeight;

		pPltN->fTopsActConc=(pPltN->fStemCont+self->fNGreenLeafCont+self->fNHeadCont)/
		                    (pBiom->fStovWeight-pBiom->fSenesLeafWeight);

		//      fTotWeight2 = pBiom->fStemWeight + pBiom->fGreenLeafWeight + fHeadWeight
		//              + fPericarpWeight + fEmbryoWeight;

		//      fNTopsCritConc = (pPltN->fLeafOptConc * pBiom->fGreenLeafWeight +
		//              pPltN->fStemOptConc * pBiom->fStemWeight + fNHeadOptConc * fHeadWeight +
		//              (double)0.015 * fPericarpWeight + (double)0.0425 * fEmbryoWeight)/fTotWeight2;


		pPltN->fRootDemand      =pBiom->fRootWeight*(pPltN->fRootOptConc-pPltN->fRootActConc);
		//+pBiom->fRootGrowR*pPltN->fRootOptConc;

		pPltN->fStemDemand      =pBiom->fStemWeight*(pPltN->fStemOptConc-pPltN->fStemActConc);
		//+pBiom->fStemGrowR*pPltN->fStemOptConc;

		pPltN->fLeafDemand      =pBiom->fGreenLeafWeight*(pPltN->fLeafOptConc-pPltN->fLeafActConc);
		//+pBiom->fLeafGrowR*pPltN->fLeafOptConc;


		if((self->fHeadWeight > (double)0.0) || (self->fHeadGrowR > (double)0.0))
			self->fNHeadDemand = self->fHeadWeight*(self->fNHeadOptConc-self->fNHeadActConc); //+ fHeadGrowR*fNHeadOptConc;

		if(self->fPericarpWeight > (double)0.0)
			self->fNPericarpDemand = self->fPericarpGrowR * self->fNPericarpOptConc;
		else
			self->fNPericarpDemand = (double)0.0;

		if(self->fEmbryoWeight > (double)0.0)
			self->fNEmbryoDemand   = self->fEmbryoGrowR * self->fNEmbryoOptConc;
		else
			self->fNEmbryoDemand = (double)0.0;

		pPltN->fTotalDemand = pPltN->fRootDemand+pPltN->fStemDemand+pPltN->fLeafDemand+
		                      self->fNHeadDemand + self->fNPericarpDemand + self->fNEmbryoDemand; //[kg(N)/ha]

		if(pPltN->fTotalDemand==(double)0.0)
			pPltN->fTotalDemand = (double)(0.001*pMa->pSowInfo->fPlantDens*10.0);

	}


	return RET_SUCCESS;
}

int ceres_NitrogenUptake_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	int         L,L1;
	double      fTotRtLenDens,fANDEM;
	double  fFNO3,fFNH4,fSMDFR,fRFAC,fNUF,fTRNU;
	double      fUNO3,fUNH4,SNO3min,SNH4min,RNO3U[MAXSCHICHT],RNH4U[MAXSCHICHT];

	PSLAYER pSL     =pSo->pSLayer->pNext;
	PSWATER pSWL=pSo->pSWater->pNext;
	PWLAYER pSLW=pWa->pWLayer->pNext;
	PCLAYER pSLN=pCh->pCLayer->pNext;

	PLAYERROOT       pLR  =pPl->pRoot->pLayerRoot;
	PBIOMASS         pBiom=pPl->pBiomass;
	PPLTNITROGEN pPltN=pPl->pPltNitrogen;

	

	fFNO3 = 0.0;
	fFNH4 = 0.0;
	L1 = 0;

	//===========================================================================================
	//Exam whether uptake occurs
	//===========================================================================================
	if ((pBiom->fRootWeight==(double)0.0))
		return 0;

	if (pBiom->fRootGrowR>(double)0.0)
		self->Uptake=1;
	if (self->Uptake==0)
		return 0;

	//======================================================================================
	//Layer Nitrogen Initiation and Transformation
	//======================================================================================
	fTotRtLenDens=(double)0.0;
	pLR=pPl->pRoot->pLayerRoot;

	for (L=1; L<=pSo->iLayers-2; L++) {
		RNO3U[L]=(double)0.0;
		RNH4U[L]=(double)0.0;

		fTotRtLenDens += pLR->fLengthDens;

		pLR->fActLayNO3NUpt=(double)0.0;
		pLR->fActLayNH4NUpt=(double)0.0;
		pLR=pLR->pNext;
	}

	//======================================================================================
	//Potential Nitrogen Uptake
	//======================================================================================
	fTRNU=(double)0.0;

	pSL             =pSo->pSLayer->pNext;
	pSWL    =pSo->pSWater->pNext;
	pSLW    =pWa->pWLayer->pNext;
	pSLN    =pCh->pCLayer->pNext;
	pLR             =pPl->pRoot->pLayerRoot;

	for (L=1; L<=pSo->iLayers-2; L++) {

		pSLN->fNO3Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;

		//Check the whether there are roots in this layer:
		if (pLR->fLengthDens==(double)0.0)          break;

		//The last layer of root:
		L1=L;

		//Potential nitrogen availability fFACTOR for NO3 (fFNO3) and NH4 (fFNH4) (0-1):
		if ((strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
		    ||(strcmp(pPl->pGenotype->acCropCode,"SG")==0)) {
			fFNO3=(double)(1.0-exp(-0.030*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(double)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));

			if (fFNO3<(double)0.04) fFNO3=(double)0.0;
			if (fFNH4<(double)0.04) fFNH4=(double)0.0;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"MI")==0) {
			fFNO3=(double)(1.0-exp(-0.035*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(double)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));
			if (fFNO3<(double)0.03) fFNO3=(double)0.0;
			if (fFNH4<(double)0.03) fFNH4=(double)0.0;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
			fFNO3=(double)(1.0-exp(-0.030*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(double)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));
			if (fFNO3<(double)0.04) fFNO3=(double)0.0;
			if (fFNH4<(double)0.04) fFNH4=(double)0.0;
		}

		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
		    ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
			fFNO3=(double)(1.0-exp(-0.0275*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(double)(1.0-exp(-0.0250*((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg)-0.5)));
			if (fFNO3<(double)0.03) fFNO3 *= (fFNO3/(double)0.03)*(fFNO3/(double)0.03)*(fFNO3/(double)0.03);//(double)0.0;
			if (fFNH4<(double)0.03) fFNH4 *= (fFNH4/(double)0.03)*(fFNH4/(double)0.03)*(fFNH4/(double)0.03);//(double)0.0;
		}

		if (fFNO3>(double)1.0) fFNO3=(double)1.0;
		if (fFNH4>(double)1.0) fFNH4=(double)1.0;

		//Soil moisture deficit fFACTOR (fSMDFR) affecting nitrogen uptake at low soil water:
		if (pSLW->fContAct<pSWL->fContFK)
			fSMDFR=(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		else
			fSMDFR=(pSWL->fContSat-pSLW->fContAct)/(pSWL->fContSat-pSWL->fContFK);

//              fSMDFR=(double)1.0;

		//fRFAC: Effect of water stress and root length density on potential nitrogen uptake
		fRFAC=pLR->fLengthDens*fSMDFR*fSMDFR*(double)0.1*pSL->fThickness*((double)100.0);

		//Potential nitrogen uptake from layer L: RNO3[L],RNH4U[L](kg N/ha)
		RNO3U[L]=fRFAC*fFNO3*((double)0.006);
		RNH4U[L]=fRFAC*fFNH4*((double)0.006);

		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) {
			RNO3U[L]=fRFAC*fFNO3*((double)0.009);
			RNH4U[L]=fRFAC*fFNH4*((double)0.009);
		}

		RNO3U[L]=fRFAC*fFNO3*pPl->pGenotype->fMaxNuptRate;
		RNH4U[L]=fRFAC*fFNH4*pPl->pGenotype->fMaxNuptRate;

		//Total Potential Root Nitrogen Uptake: fTRNU(kg N/ha)
		fTRNU=fTRNU+max((double)0.0,RNO3U[L]+RNH4U[L]);

		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
	}

	//======================================================================================
	//Nitrogen uptake factor relating supply and plant  demand: fNUF (dimensionless)
	//======================================================================================
	fANDEM=pPltN->fTotalDemand;

	if (fANDEM<=(double)0.0)
		fNUF=(double)0.0;
	else {
		if (fANDEM>fTRNU)               fANDEM=fTRNU;
		if (fTRNU==(double)0.0)         return 0;

		fNUF=fANDEM/fTRNU;
	}

	//======================================================================================
	//Actual Nitrogen Uptake  and Nitrogen Loss form Died Roots
	//======================================================================================
	fTRNU=(double)0.0;      //fTRNU is now the actual total root nitrogen uptake (kg N/ha)

	pPltN->fActNO3NUpt =(double)0.0;
	pPltN->fActNH4NUpt =(double)0.0;
	pPltN->fActNUptR    =(double)0.0;

	pSL             =pSo->pSLayer->pNext;
	pSWL    =pSo->pSWater->pNext;
	pSLW    =pWa->pWLayer->pNext;
	pSLN    =pCh->pCLayer->pNext;
	pLR             =pPl->pRoot->pLayerRoot;

	for (L=1; L<=L1; L++) {
		//Possible plant uptake from a layer:fUNO3,fUNH4(Kg N/ha)
		fUNO3=RNO3U[L]*fNUF;
		fUNH4=RNH4U[L]*fNUF;

		pSLN->fNO3Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;

		//Low limit of extractable nitrogen in layer L: SNO3min,SNH4min(Kg N/ha)
		SNO3min=(double)0.25*pSLN->fNO3Nmgkg;
		SNH4min=(double)0.50*pSLN->fNH4Nmgkg;
		//SNO3min=(double)1.25*pSLN->fNO3Nmgkg;
		//SNH4min=(double)2.5*pSLN->fNH4Nmgkg;

		//Actual plant uptake from a layer:fUNO3,fUNH4(Kg N/ha)
		if(fUNO3>pSLN->fNO3N-SNO3min)
			fUNO3=max((double)0.0, pSLN->fNO3N-SNO3min);

		if(fUNH4>(pSLN->fNH4N-SNH4min))
			fUNH4=max((double)0.0, pSLN->fNH4N-SNH4min);

		pPltN->fActNO3NUpt += fUNO3;
		pPltN->fActNH4NUpt += fUNH4;

		pLR->fActLayNO3NUpt= fUNO3;
		pLR->fActLayNH4NUpt= fUNH4;

		//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
//              pSLN->fNO3N=pSLN->fNO3N-fUNO3;
//              pSLN->fNH4N=pSLN->fNH4N-fUNH4;

		//Total actual root nitrogen uptake(kg N/ha)
		fTRNU=fTRNU+max((double)0.0,fUNO3+fUNH4);

		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
	}

	pPltN->fActNUptR = max((double)0.0,fTRNU);

	pPltN->fCumTotalDemand      +=  pPltN->fTotalDemand;
	pPltN->fCumActNUpt			+=  pPltN->fActNUptR;
	return RET_SUCCESS;
}

int ceres_PlantNitrogenStress_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	double                  fPoolAvailability,RGNFIL, factor, fTempFac, fWatFac, fGrainIncNConc;
	PBIOMASS                pBiom=pPl->pBiomass;
	PPLTNITROGEN    pPltN=pPl->pPltNitrogen;



	//Wheat, Barley:
	if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
	    ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
		//======================================================================================
		//      Change in Plant Nitrogen Content due to nitrogen exudation
		//======================================================================================
		//Daily Change in plant nitrogen content due to exudation
		if (pPltN->fTotalDemand==(double)0.0) {
			pPltN->fStovContChange  =(double)0.0;
			pPltN->fRootContChange  =(double)0.0;
		} else {
			pPltN->fStovContChange  =pPltN->fShootDemand/pPltN->fTotalDemand
			                         *pPltN->fActNUptR;
			pPltN->fRootContChange  =pPltN->fRootDemand/pPltN->fTotalDemand
			                         *pPltN->fActNUptR;
		}

		//Stover and root nitrogen content:(g N/plant)
		pPltN->fStovCont        += pPltN->fStovContChange;
		pPltN->fRootCont            += pPltN->fRootContChange;


		//Actual nitrogen concentration for tops and root
		pBiom->fStovWeight=pBiom->fLeafWeight+pBiom->fStemWeight+self->fEarWeight;

		pPltN->fTopsActConc=pPltN->fStovCont/pBiom->fStovWeight;
		pPltN->fVegActConc =pPltN->fStovCont/pBiom->fStovWeight;
		pPltN->fRootActConc=pPltN->fRootCont/(pBiom->fRootWeight-(double)0.01*pBiom->fRootWeight);

		pPltN->fRootActConc     =max(pPltN->fRootMinConc,pPltN->fRootActConc);
		pPltN->fVegActConc      =max(pPltN->fVegActConc, pPltN->fVegMinConc);
		pPltN->fTopsActConc     =max(pPltN->fTopsActConc,pPltN->fTopsMinConc);

		//======================================================================================
		//      Nitrogen Available in the Stover and Root
		//======================================================================================
		//Potential Nitrogen Available in the Stover and root
		pPltN->fStovNPool =pBiom->fStovWeight*(pPltN->fVegActConc-pPltN->fVegMinConc);
		pPltN->fRootNPool       =pBiom->fRootWeight*(pPltN->fRootActConc-pPltN->fRootMinConc);

		pPltN->fStovNPool=max((double)0.0,pPltN->fStovNPool);
		pPltN->fRootNPool       =max((double)0.0,pPltN->fRootNPool);

		//Availability Factor
		pPltN->fNStressFac=(double)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
		                   /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

		pPltN->fNStressFac=min((double)1.0,max((double)0.0,pPltN->fNStressFac));

		fPoolAvailability=(double)0.15+((double)0.20)*pPltN->fNStressFac;

		//Actual Available Nitrogen in the Stover and root and total
		pPltN->fStovNLabile     =fPoolAvailability*pPltN->fStovNPool;
		pPltN->fRootNLabile             =fPoolAvailability*pPltN->fRootNPool;
		pPltN->fTotalNLabile    =pPltN->fStovNLabile+pPltN->fRootNLabile;

		//============================================================================
		//Grain nitrogen demand
		//============================================================================
		RGNFIL=   (double)4.829666-((double)3.2488)*pPl->pDevelop->fDTT
		          +((double)0.2503)*(self->weather.fTempMax-self->weather.fTempMin)
		          +((double)4.3067)*self->weather.fTempAve;

		if (self->weather.fTempAve<=(double)10.0) RGNFIL=((double)0.49)*self->weather.fTempAve;

		pPltN->fGrainDemand =RGNFIL*pPl->pCanopy->fGrainNum*((double)1.0E-6);                   //g/plant
		pPltN->fGrainDemand =pPltN->fGrainDemand*(double)10.0*pMa->pSowInfo->fPlantDens;//kg/ha
		pPltN->fGrainDemand =min(pPltN->fTotalNLabile,pPltN->fGrainDemand);

		//======================================================================================
		//Nitrogen translocation to grain
		//======================================================================================
		if (pPltN->fGrainDemand!=(double)0.0) {
			if (pPltN->fGrainDemand>pPltN->fStovNLabile) {
				pPltN->fStovCont        -=  pPltN->fStovNLabile;
				pPltN->fRootCont        -= (pPltN->fGrainDemand-pPltN->fStovNLabile);
				pPltN->fRootActConc             =  pPltN->fRootCont/pBiom->fRootWeight;
				pPltN->fVegActConc              = pPltN->fStovCont/pBiom->fStovWeight;
				pPltN->fTopsActConc             = pPltN->fVegActConc;
				pPltN->fStovNLabile=(double)0.0;
			} else {
				pPltN->fStovCont          -= pPltN->fGrainDemand;
				pPltN->fVegActConc              = pPltN->fStovCont/pBiom->fStovWeight;
				pPltN->fTopsActConc             = pPltN->fVegActConc;
			}
		}

		//======================================================================================
		//      Grain Nitrogen Content
		//======================================================================================
		pPltN->fGrainCont += pPltN->fGrainDemand;
		pPltN->fGrainConc  = pPltN->fGrainCont/(pBiom->fGrainWeight+(double)1.0E-9);

		//============================================================================
		//              Nitrogen Factors affecting Growth
		//============================================================================
		pPltN->fNStressFac=(double)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
		                   /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

		if (pPltN->fNStressFac<(double)0.02)                                    pPltN->fNStressFac=(double)0.02;
		if (pPl->pDevelop->fStageXCERES<=(double)1.10)  pPltN->fNStressFac=(double)1.0;

		pPltN->fNStressGrain=pPltN->fNStressFac*pPltN->fNStressFac;

		if(pPltN->fNStressGrain>(double)1.5)
			pPltN->fNStressGrain=(double)1.5;

		if (pPltN->fTopsActConc>=pPltN->fTopsOptConc)
			pPltN->fNStressFac=(double)1.0;

		pPltN->fNStressTiller=pPltN->fNStressFac*pPltN->fNStressFac;
		pPltN->fNStressPhoto=(double)0.10+(double)2.0*pPltN->fNStressFac;
		pPltN->fNStressLeaf=pPltN->fNStressFac;

		if (pPltN->fNStressLeaf>(double)1.0)            pPltN->fNStressLeaf=(double)1.0;
		if (pPltN->fNStressPhoto>(double)1.0)           pPltN->fNStressPhoto=(double)1.0;
		if (pPltN->fNStressTiller>(double)1.0)          pPltN->fNStressTiller=(double)1.0;

//      auskommentiert am 18.11.99. Gerste-Simulation funktioniert dann viel besser
		/*      if (pPl->pDevelop->iStageCERES<2)
		                {
		                //pPltN->fNStressPhoto=(double)1.0;
		                pPltN->fNStressLeaf=(double)1.0;
		                pPltN->fNStressTiller=(double)1.0;
		                }*/
	}//end wheat, barley

	//Maize:
	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
		//======================================================================================
		//      Change in Plant Nitrogen Content due to nitrogen exudation
		//======================================================================================
		//Daily Change in plant nitrogen content due to exudation
		if (pPltN->fTotalDemand==(double)0.0) {
			pPltN->fStovContChange  =(double)0.0;
			pPltN->fRootContChange  =(double)0.0;
		} else {
			pPltN->fStovContChange  =pPltN->fShootDemand/pPltN->fTotalDemand
			                         *pPltN->fActNUptR;
			pPltN->fRootContChange  =pPltN->fRootDemand/pPltN->fTotalDemand
			                         *pPltN->fActNUptR;
		}

		//Stover and root nitrogen content:(g N/plant)
		pPltN->fStovCont        += pPltN->fStovContChange;
		pPltN->fRootCont        += pPltN->fRootContChange;


		//Actual nitrogen concentration for tops and root
		pBiom->fStovWeight=pBiom->fLeafWeight+pBiom->fStemWeight+self->fEarWeight;

		pPltN->fTopsActConc=pPltN->fStovCont/pBiom->fStovWeight;
		pPltN->fVegActConc =pPltN->fStovCont/pBiom->fStovWeight;
		pPltN->fRootActConc=pPltN->fRootCont/(pBiom->fRootWeight-(double)0.01*pBiom->fRootWeight);

		pPltN->fRootActConc     =max(pPltN->fRootMinConc,pPltN->fRootActConc);
		pPltN->fVegActConc      =max(pPltN->fVegActConc, pPltN->fVegMinConc);
		pPltN->fTopsActConc     =max(pPltN->fTopsActConc,pPltN->fTopsMinConc);


		//============================================================================
		//Grain nitrogen demand
		//============================================================================

		fTempFac = (double)0.69 + (double)0.0125*(self->weather.fTempMax-self->weather.fTempMin)/(double)2.0;
		fWatFac  = (double)1.125 - (double)0.125 * pPl->pPltWater->fStressFacLeaf;

		//Grain nitrogen concentration on the day
		//CERES-Maize pp.84
		fGrainIncNConc = (double)((0.007+0.01*pPltN->fNStressLeaf)*max(fTempFac,fWatFac));
		//altesExpert-N:
		//fGrainIncNConc = (double)((0.004+0.013*pPltN->fNStressLeaf)*max(fTempFac,fWatFac));

		//Total nitrogen Demand of the growing grain
		pPltN->fGrainDemand = pBiom->fGrainGrowR*fGrainIncNConc;

		pPltN->fRootOptConc = (double)((2.5-0.25*self->fXDevStage)/100.0);

		//======================================================================================
		//Nitrogen translocation to grain
		//======================================================================================
		if (pPltN->fGrainDemand!=(double)0.0) {
			pPltN->fRootMinConc = (double)0.75*pPltN->fRootOptConc;
			if(pPltN->fRootActConc < pPltN->fRootMinConc)
				pPltN->fRootActConc = pPltN->fRootMinConc;

			pPltN->fVegActConc = pPltN->fStovCont/pBiom->fStovWeight;
			if(pPltN->fVegActConc < pPltN->fVegMinConc)
				pPltN->fVegActConc = pPltN->fVegMinConc;

			//======================================================================================
			//      Nitrogen Available in the Stover and Root
			//======================================================================================
			//Potential Nitrogen Available in the Stover and root
			pPltN->fStovNPool =pBiom->fStovWeight*(pPltN->fVegActConc-pPltN->fVegMinConc);
			pPltN->fRootNPool       =pBiom->fRootWeight*(pPltN->fRootActConc-pPltN->fRootMinConc);

			pPltN->fStovNPool=max((double)0.0,pPltN->fStovNPool);
			pPltN->fRootNPool       =max((double)0.0,pPltN->fRootNPool);

			//Availability Factor
			pPltN->fNStressFac=(double)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
			                   /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

			pPltN->fNStressFac=min((double)1.0,max((double)0.0,pPltN->fNStressFac));

			fPoolAvailability=(double)0.15+((double)0.25)*pPltN->fNStressFac;

			//Actual Available Nitrogen in the Stover and root and total
			pPltN->fStovNLabile     =fPoolAvailability*pPltN->fStovNPool;
			pPltN->fRootNLabile             =fPoolAvailability*pPltN->fRootNPool;
			pPltN->fTotalNLabile    =pPltN->fStovNLabile+pPltN->fRootNLabile;


			//////////////////////////////////////////////////////
			//Grain nitrogen demand regulation                                      //
			//////////////////////////////////////////////////////
			if(pPltN->fStovNPool < pPltN->fGrainDemand)
				pPltN->fGrainDemand = pPltN->fStovNPool;

			if(pPltN->fGrainDemand > pPltN->fStovNLabile) {
				pPltN->fStovCont -= pPltN->fStovNLabile;
				pPltN->fRootCont -= (pPltN->fGrainDemand - pPltN->fStovNLabile);

				pPltN->fRootActConc = pPltN->fRootCont/pBiom->fRootWeight;
			} else {
				pPltN->fStovCont -= pPltN->fGrainDemand;
				pPltN->fTopsActConc=pPltN->fStovCont/pBiom->fStovWeight;
				pPltN->fVegActConc =pPltN->fTopsActConc;
			}

		}

		//======================================================================================
		//      Grain Nitrogne Content
		//======================================================================================
		pPltN->fGrainCont += pPltN->fGrainDemand;
		pPltN->fGrainConc  = pPltN->fGrainCont/(pBiom->fGrainWeight+(double)1.0E-9);





		//============================================================================
		//              Nitrogen Factors affecting Growth
		//============================================================================
		pPltN->fNStressFac=(double)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
		                   /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

		if (pPltN->fNStressFac>(double)1.0)
			pPltN->fNStressFac=(double)1.0;

		pPltN->fNStressPhoto    =(double)1.0;
		pPltN->fNStressLeaf             =(double)1.0;
		pPltN->fNStressGrain    =(double)1.0;

		if ((pPl->pDevelop->iStageCERES==3)||(pPl->pDevelop->iStageCERES==4))
			pPltN->fNStressFac = (double)(1.0-1.8*exp(-3.5*pPltN->fNStressFac));

		if (pPltN->fNStressFac<(double)0.001)
			pPltN->fNStressFac=(double)0.001;

		if (pPltN->fNStressFac<(double)0.8)
			pPltN->fNStressGrain = (double)0.2 + pPltN->fNStressFac;

		pPltN->fNStressPhoto = pPltN->fNStressFac*(double)1.2 + (double)0.2;
		if(pPltN->fNStressFac > (double)0.5)
			pPltN->fNStressPhoto = pPltN->fNStressFac*(double)0.4 + (double)0.6;

		pPltN->fNStressLeaf=pPltN->fNStressFac;

		if (pPltN->fNStressLeaf         >(double)1.0)           pPltN->fNStressLeaf             =(double)1.0;
		if (pPltN->fNStressPhoto        >(double)1.0)           pPltN->fNStressPhoto    =(double)1.0;
		if (pPltN->fNStressGrain        >(double)1.0)           pPltN->fNStressGrain    =(double)1.0;

	}//end maize


//Sunflower:
	if(strcmp(pPl->pGenotype->acCropCode,"SF")==0) {

		if(pPltN->fTotalDemand > pPltN->fActNUptR) {
			//if N demand exceeds potential N uptake, then  reduce plant N demand to
			//potential N uptake and calculate a supply/demand faftor
			factor = pPltN->fActNUptR/pPltN->fTotalDemand;

			pPltN->fRootDemand      = pPltN->fRootDemand*factor;
			pPltN->fStemDemand      = pPltN->fStemDemand*factor;
			pPltN->fLeafDemand      = pPltN->fLeafDemand*factor;
			self->fNHeadDemand            = self->fNHeadDemand*factor;
			self->fNPericarpDemand        = self->fNPericarpDemand*factor;
			self->fNEmbryoDemand          = self->fNEmbryoDemand*factor;
		}

		pPltN->fRootCont += pPltN->fRootDemand;
		pPltN->fStemCont += pPltN->fStemDemand;
		pPltN->fLeafCont += pPltN->fLeafDemand;
		self->fNGreenLeafCont  += pPltN->fLeafDemand;
		self->fNSenesLeafCont   = pPltN->fLeafCont - self->fNGreenLeafCont;
		self->fNHeadCont       += self->fNHeadDemand;
		pPltN->fStovCont = pPltN->fLeafCont + pPltN->fStemCont + self->fNHeadCont;

		// Actual N concentrations in plant parts after N uptake
		pPltN->fRootActConc=pPltN->fRootCont/((double)0.99*pBiom->fRootWeight);
		pPltN->fStemActConc=pPltN->fStemCont/pBiom->fStemWeight;
		pPltN->fLeafActConc=self->fNGreenLeafCont/pBiom->fGreenLeafWeight;

		if(self->fHeadWeight > (double)0.0)
			self->fNHeadActConc=self->fNHeadCont/self->fHeadWeight;

		pPltN->fTopsActConc=(pPltN->fStemCont+self->fNGreenLeafCont+self->fNHeadCont)/
		                    (pBiom->fStovWeight-pBiom->fSenesLeafWeight);





		self->fNFAC=(double)1.0-(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsActConc)
		            /(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsMinConc);

		pPltN->fNStressFac=(double)1.0-(pPl->pPltNitrogen->fLeafOptConc-pPl->pPltNitrogen->fLeafActConc)
		                   /(pPl->pPltNitrogen->fLeafOptConc-pPl->pPltNitrogen->fLeafMinConc);

		if(self->fNFAC>(double)1.0)
			self->fNFAC = (double)1.0;

		if(self->fNFAC<(double)0.001)
			self->fNFAC = (double)0.001;

		if(pPltN->fNStressFac >= (double)0.5)
			pPltN->fNStressPhoto = pPltN->fNStressFac * (double)0.4 + (double)0.6;
		else if(pPltN->fNStressFac>(double)0.0)
			pPltN->fNStressPhoto = pPltN->fNStressFac * (double)1.2 + (double)0.2;
		else
			pPltN->fNStressPhoto = (double)0.2;

		//nitrogen deficit factor affecting expansion
		pPltN->fNStressLeaf = self->fNFAC;

		//nitrogen deficit factor affecting grain filling
		if(self->fNFAC<=(double)0.8)
			pPltN->fNStressGrain = (double)0.2 + self->fNFAC;
		else
			pPltN->fNStressGrain = (double)0.0;
	} //end sunflower

	return RET_SUCCESS;
}
