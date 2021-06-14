#include "libtreemix.h"
#include <math.h>

int libtreemix_NitrogenDemand(libtreemix *self)
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
	//PCANOPY	pCan 	= xpn->pPl->pCanopy;
	//PWEATHER 	pWe  	= xpn->pCl->pWeather;
	//PSLAYER		pSL		= xpn->pSo->pSLayer;
	//PROOT		pRt		= xpn->pPl->pRoot;
	//PSWATER		pSWL	= xpn->pSo->pSWater;
	//PWLAYER	   	pSLW	= xpn->pWa->pWLayer;
	//PCLAYER     pSLN	= xpn->pCh->pCLayer;	//chemistry
	//PLAYERROOT	pLRt	= xpn->pPl->pRoot->pLayerRoot;
	
	/* Variables */
	int i;	// species looping variable
	//double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	
	/*Functions*/
	
	/****************************************************************************************************************/
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		self->plant[i].LeafContent = self->plant[i].NLf*(self->plant[i].CLfMass/1000.0);
		//pPlN->fLeafCont = pPlN->fLeafActConc*pBio->fLeafWeight/(float)1000.0;

		self->plant[i].RootContent = self->plant[i].NFRt*(self->plant[i].CFRtMass/1000.0);
		//pPlN->fRootCont = pPlN->fRootActConc*pBio->fRootWeight/(float)1000.0;
		
		self->plant[i].GrainContent = self->plant[i].NFr*(self->plant[i].CFrMass/1000.0);
		//pPlN->fGrainCont = pPlN->fGrainConc*pBio->fFruitWeight/(float)1000.0;
		
		self->plant[i].TotalContent = self->plant[i].LeafContent + self->plant[i].RootContent + self->plant[i].GrainContent + (self->plant[i].NWd/1000.0);
		//pPlN->fTotalCont = pPlN->fLeafCont+pPlN->fRootCont+pPlN->fGrainCont+(float)(PSTATEVAR->NWd/1000.0); //[tN/ha]

		if(self->plant[i].CWdWeight > 0.0)
		{
			self->plant[i].NConcWd = self->plant[i].NWd/self->plant[i].CWdWeight;
		}

		// sum up the cohorts
		pPlN->fLeafCont += (self->plant[i].LeafContent*self->plant[i].TreeDistr);
		pPlN->fRootCont += (self->plant[i].RootContent*self->plant[i].TreeDistr);
		pPlN->fGrainCont += (self->plant[i].GrainContent*self->plant[i].TreeDistr);
		pPlN->fTotalCont += (self->plant[i].TotalContent*self->plant[i].TreeDistr);
		
	}
	
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
};