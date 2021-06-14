//      balance.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "balance.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(balance, balance, EXPERTN_MODUL_BASE_TYPE);


static void balance_class_init(balanceClass *klass) {}


static void balance_init(balance *self)
{
	self->fCumRain=0.0;
	self->fPotDay=0.0;
	self->fPotCumEvap=0.0;
	self->fRunOffDay=0.0;
	self->fRunOffCum=0.0;
	self->fLeaching=0.0;
	self->fCumLeaching=0.0;
	self->fBalance=0.0;
	self->fCumBalance=0.0;	
	self->fWaterStorage = 0.0;
	self->fWaterStoragedt=0.0;
	self->fWaterInput = 0.0;
	self->fWaterOutput = 0.0;
	self->fActETCum = 0.0;
	self->fProfil = 0.0;
	self->fProfilStart = 0.0;
    self->fGroundHeatNeg = 0.0;
    self->fGroundHeatDay = 0.0;
    self->fHeatLatentDay = 0.0;
    self->fHeatSensDay = 0.0;
    self->fNetRadDay = 0.0;
    self->fResDay = 0.0;
}


int balance_load(balance *self)
{	
	//Added by Hong on 20181204 for WC-balance
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fProfil,"output.Balance.Soil Water Balance.WC in Profile [mm]",0.0);	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumInfilt,"output.Balance.Soil Water Balance.Infiltration [mm]",0.0);	
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumIntercept,"output.Water.Soil Water Balance.Cum Interception [mm]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumRunOff,"output.Water.Soil Water Balance.Cum Run Off [mm]",0.0);
	//End of Hong
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumRain,"output.Water.Soil Water Balance.Cum Rain [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumLeaching,"output.Balance.Soil Water Balance.Leaching [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fActETCum,"output.Balance.Soil Water Balance.Evapotranspiration [mm]",0.0);	
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fBalance,"output.Water.Soil Water Balance.Balance [mm]",0.0); //Hong 20190507: not necessary
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fCumBalance,"output.Balance.Soil Water Balance.Water Balance [mm]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dNProfile,"output.Balance.Soil N Balance.Nmin in Profile [kg N/ha]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dNInputCum,"output.Balance.Soil N Balance.N Input[kg N/ha]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNMinerCum,"output.Balance.Soil N Balance.Nmin mineralized [kg N/ha]",0.0);//added by Hong on 20190516
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNImmobCum,"output.Balance.Soil N Balance.Nmin immobilized [kg N/ha]",0.0);//added by Hong on 20190516
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNetNMinerCum,"output.Balance.N Balance.net Nmin (mineral_N-immob_N) [kg N/ha]",0.0);//added by Hong on 20190604			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNUptakeCum,"output.Balance.Soil N Balance.Nmin Uptake [kg N/ha]",0.0);			
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dN2OEmisCum,"output.Nitrogen.N Balance.Gaseous N Emission [kg N/ha]",0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNEmisCum,"output.Balance.Soil N Balance.Gaseous N Emission [kg N/ha]",0.0);	//Changed by Hong on 20190109		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dNTotalLeachCum,"output.Balance.Soil N Balance.N Leaching [kg N/ha]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dNBalance,"output.Balance.Soil N Balance.N Balance [kg N/ha]",0.0);		
	
	//Added by Hong on 20180807 for C-balance
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCProfile,"output.Balance.Soil C Balance.C in Profile [kg C/ha]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCInputCum,"output.Balance.Soil C Balance.C Input [kg C/ha]",0.0);
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCInputProfile,"output.Carbon.C Balance.C Input underground [kg C/ha]",0.0);
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCInputSurf,"output.Carbon.C Balance.C Input surface [kg C/ha]",0.0);		
		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dCO2EmisCum,"output.Balance.Soil C Balance.Gaseous C Emission from soil profile [kg C/ha]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dCO2SurfEmisCum,"output.Carbon.C Balance.Gaseous C Emission of soil surface [kg C/ha]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dCO2EmisCum,"output.Carbon.C Balance.Gaseous C Emission in profile [kg C/ha]",0.0);	
			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dDOCLeachCum,"output.Balance.Soil C Balance.DOC Leaching [kg C/ha]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCBalance,"output.Balance.Soil C Balance.C Balance [kg C/ha]",0.0);		
	//End of Hong
	
	//Added by Hong on 20190507 for C-balance in 30 cm profile
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCProfile_30,"output.Balance.Soil C Balance 0-30 cm.C in 0-30 cm [kg C/ha]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCInputCum_30,"output.Balance.Soil C Balance 0-30 cm.C Input 0-30 cm [kg C/ha]",0.0);
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCInputProfile_30,"output.Carbon.C Balance 0-30 cm.C Input underground 0-30 cm [kg C/ha]",0.0);
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCInputSurf,"output.Carbon.C Balance 0-30 cm.C Input surface [kg C/ha]",0.0);	 	 	
		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dCO2EmisCum_30,"output.Balance.Soil C Balance 0-30 cm.Gaseous C Emission from 0-30 cm soil [kg C/ha]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dCO2SurfEmisCum,"output.Carbon.C Balance 0-30 cm.Gaseous C Emission of soil surface [kg C/ha]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dCO2EmisCum_30,"output.Carbon.C Balance 0-30 cm.Gaseous C Emission of 0-30 cm [kg C/ha]",0.0);
				
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCProfile->dDOCLeachCum_30,"output.Balance.Soil C Balance 0-30 cm.DOC Leaching 0-30 cm [kg C/ha]",0.0);			
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->parent.pCh->pCBalance->dCBalance_30,"output.Balance.Soil C Balance 0-30 cm.C Balance [kg C/ha]",0.0);		
	//End of Hong
	 //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fWaterStorage,"output.Water.Temporal Changes.Storage [mm]",0.0); //Hong 20190507: relocated to  WC change in profile
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fWaterStorage,"output.Water.Soil Water Change in Profile.Temporal Changes of Storage [mm]",0.0);//Hong 20190507: not necessary?

    // FH 2019-07-11: added from xpn_output.c to have all variables put out on one place
	// Heat Fluxes (Energy Balance):
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Sensible Heat [W/m2]",&(self->parent.pHe->pHEBalance->fHeatSens),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Latent Heat [W/m2]",&(self->parent.pHe->pHEBalance->fHeatLatent),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance.Ground Heat [W/m2]",&(self->parent.pHe->pHEBalance->fGroundHeat),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Ground Heat [W/m2]",&(self->fGroundHeatNeg),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Net Radiation [W/m2]",&(self->parent.pHe->pHEBalance->fNetRad),-1,TRUE,TRUE);	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Residual [W/m2]",&(self->parent.pHe->pHEBalance->fRes),-1,TRUE,TRUE);
	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Sensible Heat [MJ/m2]",&(self->fHeatSensDay),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Latent Heat [MJ/m2]",&(self->fHeatLatentDay),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance.Ground Heat [W/m2]",&(self->parent.pHe->pHEBalance->fGroundHeat),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Ground Heat [MJ/m2]",&(self->fGroundHeatDay),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Net Radiation [MJ/m2]",&(self->fNetRadDay),-1,TRUE,TRUE);	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Residual [MJ/m2]",&(self->fResDay),-1,TRUE,TRUE);

/*//Alternative
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Sensible Heat [W/m$^2\\!$]",&(self->parent.pHe->pHEBalance->fHeatSens),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Latent Heat [W/m$^2\\!$]",&(self->parent.pHe->pHEBalance->fHeatLatent),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Ground Heat [W/m$^2\\!$]",&(self->fGroundHeatNeg),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Net Radiation [W/m$^2\\!$]",&(self->parent.pHe->pHEBalance->fNetRad),-1,TRUE,TRUE);	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Rate.Residual [W/m$^2\\!$]",&(self->parent.pHe->pHEBalance->fRes),-1,TRUE,TRUE);
	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Sensible Heat [MJ/m$^2\\!$]",&(self->fHeatSensDay),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Latent Heat [MJ/m$^2\\!$]",&(self->fHeatLatentDay),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Ground Heat [MJ/m$^2\\!$]",&(self->fGroundHeatDay),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Net Radiation [MJ/m$^2\\!$]",&(self->fNetRadDay),-1,TRUE,TRUE);	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Balance.Energy Balance Day.Residual [MJ/m$^2\\!$]",&(self->fResDay),-1,TRUE,TRUE);
	    */
	    
    
    self->fWaterStoragedt = 0.0;
	self->waterstorage_init = 0.0;	
	return RET_SUCCESS;
}

int balance_run(balance *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PCLIMATE pCl = xpn->pCl;
	//PHEAT pHe = xpn->pHe ;
	//PLOCATION pLo = xpn->pLo;
	//PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PCLAYER		pCL;
	PCPROFILE	pCP;
	PCBALANCE	pCB;
	PSLAYER		pSL;
	PWLAYER		pWL;
	PSWATER  pSW;
	PPLTWATER	pPltW=pPl->pPltWater;
	
	double dt;
	int iLayer;
	double fProfile;
	double fProfileIce;
	double fFluxDay;
	double fSimDepth;
//	int i;
	/*********************************************************************************/
	/*                                                                               */
	/*                                Wasserbilanz                                   */
	/*                                                                               */
	/*********************************************************************************/

	/********************************* Kumulation ************************************/

//float bal_old_h2o, f1;

// pEV = pWa->pEvap;
// pWB = pWa->pWBalance;

	/* Weather: */
//if (NewDay(pTi)&& pCl->pWeather->fRainAmount > EPSILON)

	dt= pTi->pTimeStep->fAct;

	self->fCumRain += pCl->pWeather->fPreciRate*dt; 
	//self->fCumRain += pCl->pWeather->fLiquPreciRate*dt; 
	
	
	//Added by Hong: PreciRate*dt-> InfiltR+fInterceptR + fRunOffR	
	self->fCumInfilt += pWa->fInfiltR*dt;
	self->fCumIntercept += pPltW->fInterceptR*dt;
	self->fCumRunOff += pWa->fRunOffR*dt;
	//End of Hong
	
	//Water Output
	//pWa->fLeaching    += pWa->fPercolR * DeltaT;
	self->fCumLeaching += pWa->fPercolR * dt;
	//self->fActETCum += pWa->fActETR*dt; // Hong: pWa->fActETR ist inklusiv fInterceptR. 
	//self->fActETCum +=(xpn->pPl->pPltWater->fActTranspR + xpn->pWa->pEvap->fActR+pPltW->fInterceptR)*dt; //Hong:wenn nur Bodenprofil bilanziert, soll man fInterceptR ausschliessen.  
	self->fActETCum +=(xpn->pPl->pPltWater->fActTranspR + xpn->pWa->pEvap->fActR)*dt;
	
	//Hong: fActETR = fActTranspR +fActR +fInterceptR
	self->fWaterOutput = self->fActETCum + self->fCumLeaching;	
	
	//Water Input
	if (pWa->fInfiltR > (double)0.0)
		self->fWaterInput += pWa->fInfiltR*dt;//Hong: nur Bodenprofil bilanziert 
/*	
	if (xpn->pPl->pPltWater->fInterceptR>(double)0.0)
	    self->fWaterInput += xpn->pPl->pPltWater->fInterceptR*dt;
*/
	//self->fWaterInput = self->fCumInfilt+self->fCumIntercept+self->fCumRunOff;	
	
	//self->fWaterInput = self->fCumRain;
		
	//Water Storage
	self->fWaterStorage = 0.0;
	self->fProfil = 0.0;
	for (pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,pWL = pWa->pWLayer->pNext,iLayer = 1;((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,iLayer++)
{
		self->fWaterStorage += (pWL->fContAct-pWL->fContOld) * pSL->fThickness;
		self->fProfil += pWL->fContAct * pSL->fThickness;
}
	if (pTi->pSimTime->bFirstRound==1)
		{
			self->waterstorage_init = self->fWaterStorage;
			self->fProfilStart = self->fProfil;
		}

	self->fWaterStoragedt = self->fWaterStorage*dt;

	// corrected by Hong on 20181204 self->fBalance = self->fWaterStorage - self->waterstorage_init  + pWa->fPercolR* - pCl->pWeather->fPreciRate;
	self->fBalance = self->fWaterStorage - self->waterstorage_init  + pWa->fPercolR*dt - pCl->pWeather->fPreciRate*dt;

	self->fCumBalance = self->fWaterOutput-self->fWaterInput+self->fProfil-self->fProfilStart;
	
	if (self->fCumBalance>1.0)
	{
		int Hong;
		Hong=1;
	}
	
	fProfile = 0.0;
	fProfileIce = 0.0;
	fFluxDay = 0.0;
	for (pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,pWL = pWa->pWLayer->pNext,iLayer = 1;((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,iLayer++)
	{
	fProfile    += pWL->fContAct * pSL->fThickness;
	fProfileIce += pWL->fIce * pSL->fThickness / 1.1;
	fFluxDay += pWL->fFluxDens  * dt;
	}


	/*********************************************************************************/
	/*                                                                               */
	/*                                Stickstoffbilanz                               */
	/*                                                                               */
	/*********************************************************************************/

	pCL = pCh->pCLayer;
	pCP = pCh->pCProfile;
	pCB = pCh->pCBalance;	

	fSimDepth=(double)0.0;

	//for(pCL = pCh->pCLayer->pNext;pCL->pNext!=NULL;pCL=pCL->pNext)
		for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
		
	  fSimDepth+=pSL->fThickness;

	  /***************************** Mineralisierung ***********************************/

//Added by Hong on 20180124, for consistence in daisy_miner.c
      // pCP->dNMinerCum  += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR) * dt;     
     //if (pCL->fMinerR>0) //Hong: for Century_n
	 pCP->dNMinerCum  += (pCL->fMinerR) * dt;   //  pCL->fMinerR is right
    
//End of Hong    

      pCP->dNImmobCum  += pCL->fNImmobR * dt;
	     
	  pCP->dNetNMinerCum += (pCL->fMinerR - pCL->fNImmobR) * dt;//Added by Hong for output
	 
	  if(fSimDepth <= (float)300) 
		  //pCP->dNetNMinerCum30 += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR - pCL->fNLitterImmobR) * dt;
		  //pCP->dNetNMinerCum30 += (pCL->fMinerR - pCL->fNLitterImmobR) * dt; //Added by Hong on 20180124, for consistence in daisy_miner.c
		  pCP->dNetNMinerCum30 += (pCL->fMinerR - pCL->fNImmobR) * dt;
	}

/******************************* Volatisation ************************************/
      pCP->dNH3VolatCum += pCP->fNH3VolatR * dt;  

/******************************* N2O-Emission ************************************/
	if (pCP->fN2OEmisR > 0)	   
     {
      pCP->dN2OEmisCum += pCP->fN2OEmisR  * (float)24e-5 * dt;
	 }
	 
	 //Added by Hong on 20190109 for CENTURY N
	 if (pCP->fNOEmisR > 0)	   
     {
      pCP->dNOEmisCum += pCP->fNOEmisR  * (float)24e-5 * dt;
	 }
	 
	 if (pCP->fN2EmisR > 0)	   
     {
      pCP->dN2EmisCum += pCP->fN2EmisR  * (float)24e-5 * dt;
	 }
	 //End of Hong

	pCP->dNTotalLeachCum = pCP->dUreaLeachCum + pCP->dNH4LeachCum + pCP->dNO3LeachCum + pCP->dN2ODrainCum;      
//  fN2ODrainCum wird im Transport-Modell berechnet.

/******************************* Pflanzen-N-Entzug  ************************************/

	if (pPl != NULL)
	{
	pCP->dNUptakeCum += pPl->pPltNitrogen->fActNUptR * dt;
	}
	
	
	/****************************** Bilanzierung *************************************/
  
	// Profilsumme Null setzen
	pCB->fNO3NProfile  =  (double)0.0;
	pCB->fNH4NProfile  = (double)0.0;
	pCB->fN2ONProfile  = (double)0.0;
	pCB->fN2NProfile   = (double)0.0;
	pCB->fNONProfile = (double)0.0;
	pCB->dNProfile  = (double)0.0;
	pCB->fUreaNProfile = (double)0.0;

	pCL = pCh->pCLayer; //eingefügt, da sonst Programm nicht läuft (warum?)
	
	for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
      pCB->fNO3NProfile 	+= pCL->fNO3N;
      pCB->fNH4NProfile 	+= pCL->fNH4N;
      pCB->fN2ONProfile 	+= pCL->fN2ON;
      pCB->fN2NProfile  	+= pCL->fN2N;
	  pCB->fNONProfile  	+= pCL->fNON; 
      pCB->fUreaNProfile    += pCL->fUreaN;
     }


	//Gesamtstickstoff im Profil
	pCB->dNProfile = pCB->fNO3NProfile + pCB->fNH4NProfile + pCB->fN2ONProfile + pCB->fN2NProfile + pCB->fNONProfile + pCB->fUreaNProfile;

	if (pTi->pSimTime->bFirstRound==1)
		{
			pCB->fNProfileStart = pCB->dNProfile;
		}

	//pCB->dNOutputCum = pCP->dN2OEmisCum + pCP->dNH3VolatCum + pCP->dNUptakeCum + pCP->dNImmobCum - pCP->dNMinerCum + pCP->dNTotalLeachCum;
	
	pCB->dNOutputCum = pCP->dN2OEmisCum +pCP->dNOEmisCum +pCP->dN2EmisCum + pCP->dNH3VolatCum + pCP->dNUptakeCum + pCP->dNImmobCum - pCP->dNMinerCum + pCP->dNTotalLeachCum; //Changed by Hong on 20190109

	//pCB->dNBalCorrect = -pCP->dN2OEmisCum; //removed by Hong on 20190109
	pCP->dNEmisCum = pCP->dN2OEmisCum + pCP->dNOEmisCum +pCP->dN2EmisCum; //added by Hong on 20190109

	pCB->dNBalance = pCB->dNProfile + pCB->dNOutputCum - pCB->fNProfileStart - pCB->dNInputCum - pCB->dNBalCorrect;// Was ist das? - pCB->dNBalCorrect;
	
	
	
	/*********************************************************************************/
	/*                                                                               */
	/*                          (soil) Kohlenstoffbilanz                            */
	/*                                                                               */
	/*********************************************************************************/
	
	// Profilsumme Null setzen
	pCB->fDOCProfile  =  (double)0.0;
	pCB->fCLitterProfile  = (double)0.0;
	pCB->fCManureProfile  = (double)0.0;
	pCB->fCHumusProfile   = (double)0.0;	
	pCB->dCProfile  = (double)0.0;
	pCP->dCO2EmisCum =(double)0.0;
//Hong 20190507: balance for 0-30 cm profile:	
	pCB->fDOCProfile_30  =  (double)0.0;
	pCB->fCLitterProfile_30  = (double)0.0;
	pCB->fCManureProfile_30  = (double)0.0;
	pCB->fCHumusProfile_30   = (double)0.0;	
	pCB->dCProfile_30  = (double)0.0;
	pCP->dCO2EmisCum_30 =(double)0.0;

	pCL = pCh->pCLayer; //eingefügt, da sonst Programm nicht läuft (warum?)
	pSL = xpn->pSo->pSLayer;
	double fCumDepth; //Hong 20190507: balance for 0-30 cm profile

	fCumDepth = 0.0;
	for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
      pCB->fDOCProfile 	+= pCL->fDOC;
      pCB->fCLitterProfile 	+= pCL->fCLitter;
      pCB->fCManureProfile 	+= pCL->fCManure;
	  //Hohng 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	  {
		  pCB->fDOCProfile_30 	+= pCL->fDOC;
          pCB->fCLitterProfile_30 	+= pCL->fCLitter;
          pCB->fCManureProfile_30 	+= pCL->fCManure;
		  }
		  
     }
	 
	 pSL = xpn->pSo->pSLayer;
	 fCumDepth = 0.0;
	 for (SOIL_LAYERS1(pSL,pSL->pNext))
	{
      pCB->fCHumusProfile  	+= pSL->fCHumus;
	  
	  //Hohng 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	  {
		  pCB->fCHumusProfile_30  	+= pSL->fCHumus;
	  }
     }

	//Gesamtkohlenstoff im Profil (ohne surface):  
	pCB->dCProfile = pCB->fDOCProfile + pCB->fCLitterProfile + pCB->fCManureProfile + pCB->fCHumusProfile + pCB->fCsolProfile; //pCB->fCsolC = pCB->fDOC?
	pCB->dCProfile_30 = pCB->fDOCProfile_30 + pCB->fCLitterProfile_30 + pCB->fCManureProfile_30 + pCB->fCHumusProfile_30 + pCB->fCsolProfile_30; 
	
	//in daisy_miner.c: pCL->fCMicBiomSlow + pCL->fCMicBiomFast + pCL->fCHumusSlow + pCL->fCHumusFast =fCHumus
    if (pTi->pSimTime->bFirstRound==1)
		{
			pCB->fCProfileStart = pCB->dCProfile;
			pCB->fCProfileStart_30 = pCB->dCProfile_30;//Hong 20190507: balance for 0-30 cm profile
		}
	
	/******************************* CO2-Emission ************************************/
	pCL = pCh->pCLayer; //eingefügt, da sonst Programm nicht läuft (warum?)
			
    fCumDepth = 0.0;
	for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
      //pCP->dCO2EmisCum 	+= pCL->fCO2ProdR * dt;
	  //if (pCL->fCO2C >0)//Hong: for Century_n
	  pCP->dCO2EmisCum 	+= pCL->fCO2C;
	  //Hong 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	  {
		  pCP->dCO2EmisCum_30 	+= pCL->fCO2C;
	  }
      
     }
	
	pCP->dCO2EmisCumSum = pCP->dCO2EmisCum;
	
	pCP->dCO2EmisCumSum_30 = pCP->dCO2EmisCum_30;
	
	pCB->dCOutputCum = pCP->dCO2EmisCumSum + pCP->dDOCLeachCum;	
	pCB->dCOutputCum_30 = pCP->dCO2EmisCumSum_30 + pCP->dDOCLeachCum_30;
		
	//pCB->dCBalCorrect = pCP->dCO2EmisCum;
	pCB->dCBalCorrect = 0.0; //Hong: nicht sicher!
	
	pCB->dCInputCum = pCB->dCInputProfile;
	pCB->dCInputCum_30 = pCB->dCInputProfile_30; //Hong 20190507: balance for 0-30 cm profile
	
	pCB->dCBalance = pCB->dCProfile + pCB->dCOutputCum - pCB->fCProfileStart - pCB->dCInputCum - pCB->dCBalCorrect; 
	
	pCB->dCBalance_30 = pCB->dCProfile_30 + pCB->dCOutputCum_30 - pCB->fCProfileStart_30 - pCB->dCInputCum_30 - pCB->dCBalCorrect; //Hong 20190507: balance for 0-30 cm profile
	
	if (pCB->dCBalance>1)
	{int Hong;
	Hong=1;
	}
	/*********************************************************************************/
	/*                                                                               */
	/*                 Kohlenstoffbilanz mit Surface-Pools                           */
	/*                                                                               */
	/*********************************************************************************/
	
/*	// Profilsumme Null setzen
	pCB->fDOCProfile  =  (double)0.0;
	pCB->fCLitterProfile  = (double)0.0;
	pCB->fCManureProfile  = (double)0.0;
	pCB->fCHumusProfile   = (double)0.0;	
	pCB->dCProfile  = (double)0.0;
	pCP->dCO2EmisCum =(double)0.0;
//Hong 20190507: balance for 0-30 cm profile:	
	pCB->fDOCProfile_30  =  (double)0.0;
	pCB->fCLitterProfile_30  = (double)0.0;
	pCB->fCManureProfile_30  = (double)0.0;
	pCB->fCHumusProfile_30   = (double)0.0;	
	pCB->dCProfile_30  = (double)0.0;
	pCP->dCO2EmisCum_30 =(double)0.0;

	pCL = pCh->pCLayer; //eingefügt, da sonst Programm nicht läuft (warum?)
	pSL = xpn->pSo->pSLayer;
	double fCumDepth; //Hong 20190507: balance for 0-30 cm profile

	fCumDepth = 0.0;
	for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
      pCB->fDOCProfile 	+= pCL->fDOC;
      pCB->fCLitterProfile 	+= pCL->fCLitter;
      pCB->fCManureProfile 	+= pCL->fCManure;
	  //Hohng 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	  {
		  pCB->fDOCProfile_30 	+= pCL->fDOC;
          pCB->fCLitterProfile_30 	+= pCL->fCLitter;
          pCB->fCManureProfile_30 	+= pCL->fCManure;
		  }
		  
     }
	 
	 pSL = xpn->pSo->pSLayer;
	 fCumDepth = 0.0;
	 for (SOIL_LAYERS1(pSL,pSL->pNext))
	{
      pCB->fCHumusProfile  	+= pSL->fCHumus;
	  
	  //Hohng 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	  {
		  pCB->fCHumusProfile_30  	+= pSL->fCHumus;
	  }
     }

	//Gesamtkohlenstoff im Profil:  pCB->fCsolC = pCB->fDOC?
	pCB->dCProfile = pCP->fDOCSurf +pCP->fCLitterSurf+ pCP->fCManureSurf+ pCP->fCHumusSurf+pCB->fDOCProfile + pCB->fCLitterProfile + pCB->fCManureProfile + pCB->fCHumusProfile; 
	pCB->dCProfile_30 = pCP->fDOCSurf +pCP->fCLitterSurf+ pCP->fCManureSurf+ pCP->fCHumusSurf+pCB->fDOCProfile_30 + pCB->fCLitterProfile_30 + pCB->fCManureProfile_30 + pCB->fCHumusProfile_30; 
	
	//in daisy_miner.c: pCL->fCMicBiomSlow + pCL->fCMicBiomFast + pCL->fCHumusSlow + pCL->fCHumusFast =fCHumus
    if (pTi->pSimTime->bFirstRound==1)
		{
			pCB->fCProfileStart = pCB->dCProfile;
			pCB->fCProfileStart_30 = pCB->dCProfile_30;//Hong 20190507: balance for 0-30 cm profile
		}
	
	/******************************* CO2-Emission ************************************/
/*	pCL = pCh->pCLayer; //eingefügt, da sonst Programm nicht läuft (warum?)
			
    fCumDepth = 0.0;
	for (SOIL_LAYERS1(pCL,pCL->pNext))
	{
      //pCP->dCO2EmisCum 	+= pCL->fCO2ProdR * dt;
	  //if (pCL->fCO2C >0)//Hong: for Century_n
	  pCP->dCO2EmisCum 	+= pCL->fCO2C;
	  //Hong 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	  {
		  pCP->dCO2EmisCum_30 	+= pCL->fCO2C;
	  }
      
     }
	
	pCP->dCO2EmisCumSum=pCP->dCO2EmisCum+pCP->dCO2SurfEmisCum;
	
	pCP->dCO2EmisCumSum_30 =pCP->dCO2EmisCum_30 +pCP->dCO2SurfEmisCum;
	
	pCB->dCOutputCum = pCP->dCO2SurfEmisCum + pCP->dCO2EmisCum + pCP->dDOCLeachCum;	
	pCB->dCOutputCum_30 = pCP->dCO2SurfEmisCum + pCP->dCO2EmisCum_30 + pCP->dDOCLeachCum_30;
		
	//pCB->dCBalCorrect = pCP->dCO2EmisCum;
	pCB->dCBalCorrect = 0.0; //Hong: nicht sicher!
	
	pCB->dCInputCum = pCB->dCInputSurf +pCB->dCInputProfile;
	pCB->dCInputCum_30 = pCB->dCInputSurf + pCB->dCInputProfile_30; //Hong 20190507: balance for 0-30 cm profile
	
	pCB->dCBalance = pCB->dCProfile + pCB->dCOutputCum - pCB->fCProfileStart - pCB->dCInputCum - pCB->dCBalCorrect; 
	
	pCB->dCBalance_30 = pCB->dCProfile_30 + pCB->dCOutputCum_30 - pCB->fCProfileStart_30 - pCB->dCInputCum_30 - pCB->dCBalCorrect; //Hong 20190507: balance for 0-30 cm profile
*/	
	
	/*********************************************************************************/
	/*                                                                               */
	/*                        Wasserbilanz                               */
	/*                                                                               */
	/*********************************************************************************/
/*	
	self->fActETCum += pWa->fActETR*dt;
	self->fCumLeaching += pWa->fPercolR * dt;
	pCB->dCOutputCum = pCP->dCO2SurfEmisCum + pCP->dCO2EmisCum + pCP->dDOCLeachCum;
	
	pCB->dWCBalance = pCB->dWCProfile + pCB->dWCOutputCum - pCB->fWCProfileStart - pCB->dWCInputCum - pCB->dWCBalCorrect; 
	
*/	
// FH 2019-07-11 Added Energy balance calculation(s)
	/*********************************************************************************/
	/*                                                                               */
	/*                                Energy balance                */
	/*                                                                               */
	/*********************************************************************************/
    
    // Negative for consistency reasons -> http://www.met.reading.ac.uk/~swrhgnrj/teaching/MT23E/mt23e_notes.pdf
    // Positive: from surface into ground
    self->fGroundHeatNeg = -xpn->pHe->pHEBalance->fGroundHeat;

	// Energy Balance:
	xpn->pHe->pHEBalance->fRes = xpn->pHe->pHEBalance->fNetRad - xpn->pHe->pHEBalance->fHeatLatent + xpn->pHe->pHEBalance->fGroundHeat - xpn->pHe->pHEBalance->fHeatSens;	

    //Daily energy balance
    if(NewDay(pTi))
        {
        self->fGroundHeatDay = self->fGroundHeatDay_zwischen;
        self->fHeatLatentDay = self->fHeatLatentDay_zwischen;
        self->fHeatSensDay = self->fHeatSensDay_zwischen;
        self->fNetRadDay = self->fNetRadDay_zwischen;
        self->fResDay = self->fResDay_zwischen;
        
        self->fGroundHeatDay_zwischen = 0.0;
        self->fHeatLatentDay_zwischen = 0.0;
        self->fHeatSensDay_zwischen = 0.0;
        self->fNetRadDay_zwischen = 0.0;
        self->fResDay_zwischen = 0.0;
        }
        
        self->fGroundHeatDay_zwischen += self->fGroundHeatNeg*pTi->pTimeStep->fAct * 86400.0/1.0E6;
        self->fHeatLatentDay_zwischen += xpn->pHe->pHEBalance->fHeatLatent * pTi->pTimeStep->fAct * 86400.0/1.0E6;
        self->fHeatSensDay_zwischen += xpn->pHe->pHEBalance->fHeatSens* pTi->pTimeStep->fAct * 86400.0/1.0E6;
        self->fNetRadDay_zwischen += xpn->pHe->pHEBalance->fNetRad * pTi->pTimeStep->fAct * 86400.0/1.0E6;
        self->fResDay_zwischen += xpn->pHe->pHEBalance->fRes * pTi->pTimeStep->fAct * 86400.0/1.0E6;
        
        
        
	return RET_SUCCESS;
}


