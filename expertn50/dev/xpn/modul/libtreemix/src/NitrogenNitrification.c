#include "libtreemix.h"
#include <math.h>

int libtreemix_NitrogenNitrification(libtreemix *self)
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
	//int i;	// species looping variable
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	int libtreemix_CalcNitrification_FAGUS(libtreemix *self);
	
	/****************************************************************************************************************/
	
	if(self->conf.Soil_Processes == 2)
	{
		libtreemix_CalcNitrification_FAGUS(self);
	}
	
	return RET_SUCCESS;
}

int libtreemix_CalcNitrification_FAGUS(libtreemix *self)
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
	
	/* Variables */
	//int i;	// species looping variable
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double fakammon;
	double faklimit;
	double potnit, aktnit;
	double min1, min2, min3, min4;
	double ppmTokg;
	
	/*Functions*/
	int libtreemix_ReductionFunctions_FAGUS(libtreemix *self);
	
	/****************************************************************************************************************/
	
	PCLAYER akt0;
	PSLAYER akt1;

	libtreemix_ReductionFunctions_FAGUS(self);	// the reduction functions are only calculated once
	
	for(akt0 = xpn->pCh->pCLayer->pNext,
		akt1 = xpn->pSo->pSLayer->pNext;
			((akt0->pNext != NULL)&&(akt1->pNext != NULL));
		akt0 = akt0->pNext,
		akt1 = akt1->pNext
		)
	{
		ppmTokg = akt1->fBulkDens * 0.1 * (akt1->fThickness / 10.0);

		//if(akt0->fNH4N < 0.0)
		//{
		//	akt0->fNH4N = 0.0;
		//}

		fakammon =	1.0 - exp(-0.01363 * akt0->fNH4N);
		min1 =		min(akt0->fH2ONitrCoeff, fakammon);
		faklimit =	min(akt0->fTempCoeff, min1);
		potnit = akt0->fNH4NitrMaxR * exp(2.302 * faklimit);
		if(potnit < 0.05){
			potnit = 0.05;
		}                      
		else if(potnit > 1.0){
			potnit = 1.0;
		}

		min2 = min(akt0->fTempCoeff, akt0->fpHCoeff);
		min3 = min(akt0->fH2ONitrCoeff, min2);
		min4 = min(akt0->fNH4NitrMaxR, min3);
		min4 = min(potnit, min4);

		aktnit = min4 * 40.0 * (akt0->fNH4N/ppmTokg)/((akt0->fNH4N/ppmTokg) + 90.0);
		if(aktnit > 0.8){
			aktnit = 0.8;
		}

		akt0->fNH4NitrR = aktnit * akt0->fNH4N;	// * (float)2.0; //verdoppelt ep 071098
		if (akt0->fNH4NitrR  < 0.0){
			akt0->fNH4NitrR  = 0.0;
		}

		/*  nitrified N is being subtracted from the NH4 pool of soil layer and added to NO3 pool */		
		akt0->fNH4N -= akt0->fNH4NitrR;
		akt0->fNO3N += akt0->fNH4NitrR;
	}	
	
	return RET_SUCCESS;
}

int libtreemix_ReductionFunctions_FAGUS(libtreemix *self)
{
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	
	/* variable */
	//int OK = 0;
	int i1;
	double minEvap;
	double fakA, fakN;
	double airdry;
	double w1, w2, t1;
	double ntotal, ppmTokg;
	double cn1, cn2;
	double f1, f2;

	PWLAYER	    akt0;
	PSWATER		akt1;
	PCLAYER		akt2;
	PHLAYER		akt3;
	PSLAYER		akt4;	

	// Calc. of minimal Evaporation loss
	minEvap = 0.9-(0.00038*(((xpn->pSo->pSLayer->pNext->fThickness)/10.0)-30.0)*(((xpn->pSo->pSLayer->pNext->fThickness)/10.0)-30.0));

	/* Calculation of Reduction Factors due to Wetness */
	for(
		akt0 = xpn->pWa->pWLayer->pNext, 
		akt1 = xpn->pSo->pSWater->pNext, 
		akt2 = xpn->pCh->pCLayer->pNext, 
		akt4 = xpn->pSo->pSLayer->pNext, 
		i1=1;
			((akt0->pNext != NULL) && (akt1->pNext != NULL) && (akt2->pNext != NULL) && (akt4->pNext != NULL));
		akt0 = akt0->pNext,
		akt1 = akt1->pNext,
		akt2 = akt2->pNext,
		akt4 = akt4->pNext,
		i1++
		)
	{
		/* Moisture Factor for Ammonification */
		airdry = (akt1->fContPWP * 0.5);			
		if(i1 == 1)
		{
			airdry = (akt1->fContPWP * minEvap);
		}
		fakA =  ((akt0->fContAct - airdry) / (akt1->fContFK - airdry));

		if (akt0->fContAct > akt1->fContFK)
		{
			w1 = ((akt0->fContAct - akt1->fContFK) / (akt4->fPorosity - akt1->fContFK));
			fakA = 1.0 - w1;
		}
		if (fakA < (float)0.0)
		{
			fakA = 0.0;
		}
		akt2->fNH4NitrCoeff = (float)fakA;

		/* Moisture Factor for Nitrification */
		fakN = ((akt0->fContAct - akt1->fContPWP) / (akt1->fContFK - akt1->fContPWP)*0.25);
		if(akt0->fContAct > ((akt1->fContFK - akt1->fContPWP)*0.25))
		{
			fakN = 1.0;
		}
		if (akt0->fContAct > akt1->fContFK)
		{
			w2 = ((akt0->fContAct - akt1->fContFK) / (akt4->fPorosity - akt1->fContFK));
			fakN = 1.0 - w2;
		}
		if (fakN < (float)0.0)
		{
			fakN = 0.0;
		}
		akt2->fH2ONitrCoeff = (float)fakN;
	 }

	
	/* Calculation of Reduction Factors due to Temperature */
	for(
		akt2 = xpn->pCh->pCLayer->pNext,
		akt3 = xpn->pHe->pHLayer->pNext;
			((akt2->pNext != NULL)&&(akt3->pNext != NULL));		
		akt2 = akt2->pNext,
		akt3 = akt3->pNext
		)
	{
		t1 = (double)(0.091*exp(0.08*akt3->fSoilTemp));
		if(akt3->fSoilTemp < (float)5.0)
		{
			t1 = (double)0.034*(akt3->fSoilTemp-(float)1.0);
		}
		if(akt3->fSoilTemp <= (float)1.0)
		{
			t1 = 0.0;
		}
		if(t1 < (float)0.0)
		{
			t1 = 0.0;
		}
		akt2->fTempCoeff = (float)t1;
	}

	
	/* Calculation of Reduction Factors due to Nitrogen */
	for(akt2 = xpn->pCh->pCLayer->pNext,
		akt4 = xpn->pSo->pSLayer->pNext;
			((akt2->pNext != NULL)&&(akt4->pNext != NULL));
		akt2 = akt2->pNext,
		akt4 = akt4->pNext
		)
	{
		ppmTokg = (double)(akt4->fBulkDens * 0.1 * (akt4->fThickness / 10));
		ntotal = (double)(akt2->fNH4N + akt2->fNO3N - (2.0 / ppmTokg));

		if (ntotal < (float)0.0){
			ntotal = (float)0.0;
		}

		cn1 = ((akt2->fCLitter) / (akt2->fNLitter + ntotal));	
		cn2 = exp((-0.693*(cn1-25.0))/25.0);

		if (cn2 > 1.0){
			cn2 = 1.0;
		}
		akt2->fCNCoeff = (float)cn2;
		akt2->fNAnorg = (float)ntotal;
	}

	/* Calculation of Reduction Factors due to pH */	
		// The following reduction function was given for wheat
		// if (ph(l).lt.6.) phn(l)=(ph(l)-4.5)/1.5
		// if (ph(l).gt.8.) phn(l)=9.-ph(l)
		// if (phn(l).lt.0.) phn(l)=0.
		// For beech the pH might be somewhat lowered 
		// limit pH = functions of oc
	for(
		akt2 = xpn->pCh->pCLayer->pNext,
		akt4 = xpn->pSo->pSLayer->pNext;
			((akt2->pNext != NULL)&&(akt4->pNext != NULL));
		akt2 = akt2->pNext,
		akt4 = akt4->pNext
		)
	{
		akt2->fpHCoeff = (float)1.0;

		if (akt4->fCHumus>(float)0.2){
			f1 = (double)(3.86-0.4*log(akt4->fCHumus)); // f1=2.5 b.30%C, =4.5 b.0.2%C
			f2 = (double)(5.5 -0.3*log(akt4->fCHumus)); // f2=4.5 b.30%C, =6.0 b.0.2%C
		}
		else{
			f1 = (double)4.5;
			f2 = (double)6.0;
		}

		if (akt4->fpH < f2){
			akt2->fpHCoeff = (float)((akt4->fpH-f1)/(f2-f1));
		}

		if (akt4->fpH > (float)8.0){
			akt2->fpHCoeff = (float)9.0 - akt4->fpH;
		}

		if (akt2->fpHCoeff < (float)0.0){
			akt2->fpHCoeff = (float)0.0;
		}
	}
	
	return RET_SUCCESS;
}