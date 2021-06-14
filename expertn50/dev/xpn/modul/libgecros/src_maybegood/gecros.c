//      gecros.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "gecros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gecros_alloc.h"
#include "gecros_macros.h"
#include "gecros_tools.h"

//Hong 2016-08-08: change for Sebastian Gayler and Arne Poyda
int gecros_Germination(gecros *self);
int Emergence_GECROS(gecros *self);
double gecros_Vernalization_CERES(gecros *self); //added on 20170704
double gecros_DailyVernalization_SPASS(double fTempMin,double fTempMax, double fTemp, PRESPONSE pResp,
							  	   double fMinTemp, double fOptTemp, double  fMaxTemp, gecros *self);//added on 20170704
double gecros_RelativeTemperatureResponse_SPASS(double fTemp, double fMinTemp, double fOptTemp, double fMaxTemp);//added on 20170704
double gecros_PlantTemperature(gecros *self);//added on 20170704
//End of Hong

int    DefaultPlantParameters_GECROS(gecros *self);
int    PhasicDevelopment_GECROS(gecros *self);
int    Photosynthesis_GECROS(gecros *self);
int    BiomassGrowth_GECROS(gecros *self);
int    CanopyFormation_GECROS(gecros *self);
int    RootSystemFormation_GECROS(gecros *self);
int    OrganSenescence_GECROS(gecros *self);
int    PotentialNitrogenUptake_GECROS(gecros *self);
int    ActualNitrogenUptake_GECROS(gecros *self);

int    gecros_Init_Vars(gecros *self);
int    LeafAreaGrowth_GECROS(gecros *self);
int    PlantHeightGrowth_GECROS(gecros *self);
int    NitrogenFixation_GECROS(gecros *self);

//Hong for crop rotation on 20170131
int    gecros_load_ini_file(gecros *self);
int    gecros_PlantVariableInitiation(gecros *self);
//End of Hong

int    Astronomy_GECROS(gecros *self, int iJulianDay, double vLatitude);//, double INSP);
//double DailyThermalDayUnit_GECROS(double DS,double TMAX,double TMIN,double DIF,double DAYL,
//								  double TBD,double TOD,double TCD,double TSEN);//Hong
double DailyThermalDayUnit_GECROS(gecros *self, double DS,double TEMP,double DIF,double DAYL,
								  double TBD,double TOD,double TCD,double TSEN);
								  
//double Phenology_GECROS(gecros *self,double DS,double SLP,double DDLP,double SPSP,double EPSP, double PSEN,double MTDV,double MTDR,double TDU);
double Phenology_GECROS(gecros *self,double DS,double SLP,double DDLP,double SPSP,double EPSP, double PSEN,double MTDV,double MTDR,double TDU,double OptVernDays);//added double OptVernDays by Hong on 20170704
double DailyCanopyGrossPhotosynthesis_GECROS(gecros *self,double SC,double SINLD,double COSLD,double DAYL,double DSINBE,
          double DDTR,double TMPA,double HOUR,double DVP,double WNM,double C3C4,double LAI,double TLAI,
          double HT,double LWIDTH,double RD,double SD1,double RSS,double BLD,double NLV,double TNLV,
          double SLNMIN,double DWSUP,double CO2A,double LS,double EAJMAX,double XVN,double XJN,double THETA,
          double WCUL,double FVPD,double *PPCAN,double *APCANS,double *APCANN,double *APCAN,double *PTCAN,
          double *ATCAN,double *PESOIL,double *AESOIL,double *DIFS,double *DIFSU,double *DIFSH,double *DAPAR);

double kdiff(double vLAI,double BL,double SCP);
double kbeam(double SINB,double BL);
double refl(double SCP,double KB);
double ligab(double SCP,double KB,double KBP,double KDP,double PCB,double PCD, 
             double IB0,double ID0,double LAI,int IW);
double pan(double SLNT,double SLNMIN,double LAI,double KN,double KB,int IW);
double difla(double NRADC,double PT,double RBH,double RT);

int    PotentialLeafPhotosynthesis_GECROS(double FRAC,double DAYTMP,double DVP,double CO2A,double C3C4,
          double FVPD,double PAR,double NP,double RBW,double RBH,double RT, double ATRJ,double ATMTR,
		  double EAJMAX,double XVN,double XJN,double THETA, 
		  double *PLF,double *PT,double *RSW,double *NRADC,double *SLOPEL);

double PotentialEvaporation_GECROS(double DAYTMP,double DVP,double RSS,double RTS,double RBWS,double RBHS,
                                   double ATRJS,double ATMTR,double PT1,double WSUP1, double *NRADS);

double PotEvaporTransp_GECROS(double RSW,double RT,double RBW,double RBH,double ATRJ,double ATMTR,
                              double FRAC,double TLEAF,double DVP,double SLOPE,double VPD);

double NetLeafAbsRad(double ATRJ,double ATMTR,double FRAC,double TLEAF,double DVP);

double internalCO2(double TLEAF,double DVP,double FVPD,double CO2A,double C3C4);

double gcrsw(double PLEAF,double RDLEAF,double TLEAF,double CO2A,double CO2I,double RBW,double RT);

double photo(double C3C4,double PAR,double TLEAF,double CO2I,double NP,
             double EAJMAX,double XVN,double XJN,double THETA);

double darkr(double TLEAF,double NP,double XVN);

int    ActualLeafPhotosynthesis_GECROS(double DAYTMP,double PAR,double DVP,double CO2A,double C3C4,
                                       double FVPD,double NRADC,double AT,double PT,double RT,
                                       double RBH,double RBW,double RSW,double SLOPEL,double NP,
                                       double NPN,double EAJMAX,double XVN,double XJN,double THETA, 
                                       double *PLFAS,double *PLFAN,double *ADIF);

double betaf(double DVR, double TE, double TX, double TI);

int    sinkg(double DS, double SSG,double TOTC,double YG,double FD,double DCDR,double DCS,double DELT,
             double *DCDC,double *DCD,double *FLWC);

int    rnacc(double FNSH,double NUPT,double RWST,double STEMNC,double LNCMIN,double RNCMIN,
             double LNC,double RNC,double NLV,double NRT,double WLV,double WRT,double DELT,
             double CB,double CX,double TM,double DS,double SEEDNC,double RWSO,double LNLV,
             double LNRT,double *RNRT,double *RNST,double *RNLV,double *RTNLV,double *RNSO);


int gecros_integrate_small_time_step_vars(gecros *self);

G_DEFINE_TYPE(gecros, gecros, EXPERTN_MODUL_BASE_TYPE);


static void gecros_class_init(gecrosClass *klass){}


static void gecros_init(gecros *self)
{
 self->_init_done=0;
 self->_done_done=0;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// XN-GECROS Modul Functions:
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int gecros_Init_and_AllocMemory(gecros *self)
{
	if (self->_init_done!=0) return RET_SUCCESS;
	self->_init_done++;

	gecros_alloc_allocateGECROSVariables(self);	
	gecros_Init_Vars(self); //Hong: only read once  
	
	return RET_SUCCESS;
}

int gecros_Done_and_FreeMemory(gecros *self)
{	
	if (self->_done_done!=0) return RET_SUCCESS;
	self->_done_done++;
	/*gecros_alloc_freeAllocatedGECROSMemory(self);
	G_FREE_IF_NOT_0(self->ini_file);	*/
	return RET_SUCCESS;
}

// Phasic development
int gecros_Development(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PTIME pTi = xpn->pTi;
	  PMANAGEMENT pMa = xpn->pMa;
	  PPLANT pPl = xpn->pPl;

      gecros_integrate_small_time_step_vars(self);

      if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
		    pMa->pSowInfo->Year,pMa->pSowInfo->Month,pMa->pSowInfo->Day)<0))
      {
       return RET_SUCCESS;
      }

	  if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
		    pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)==0)&&(NewDay(pTi)))
      {
		  self->first_round += 1;
	  }
	  
 
if (NewDay(pTi)){
	  if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
		    pMa->pSowInfo->Year,pMa->pSowInfo->Month,pMa->pSowInfo->Day)==0)&&(self->first_round)>=0)			
      {
		  gecros_alloc_allocateGECROSVariables(self);
		  gecros_load_ini_file(self);
		  
		  
	  }
}
      expertn_modul_base_DevelopmentCheckAndPostHarvestManagement(xpn);

      //if NewDayAndPlantGrowing
	  //if PlantIsGrowing
	  if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
	  PhasicDevelopment_GECROS(self);

		
	  
      return RET_SUCCESS;
  }


// Gross Photosynthesis
int gecros_Photosynthesis(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PPLANT     pPl = xpn->pPl;
	  PWATER     pWa = xpn->pWa;
	  
	  PGECROSCARBON     pGPltC =self->pGecrosPlant->pGecrosCarbon;
	  PGECROSSOIL       pGS    = self->pGecrosPlant->pGecrosSoil;
    
      double dayTime; //Added by Hong
      gecros_integrate_small_time_step_vars(self);

	  //if NewDayAndPlantGrowing
      if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
      {
       //Wird nur berechnet wenn der Feldaufgang erreicht wurde
       //out-noted by Hong: if (pPl->pDevelop->iDayAftEmerg > 0)
       //if (xpn->pCl->pWeather->fSolRad > 0)
       if (pPl->pDevelop->iDayAftEmerg > 0)
		{ 
			dayTime= xpn->pTi->pSimTime->fTimeDay*(double)24.;

			// if ((dayTime >=self->SUNRIS)&&(dayTime<=self->SUNSET))
          {
		   Photosynthesis_GECROS(self);
	       }
		   
	     /* else
	      {
		   //Begin of hong on 20191023
//%** output ***
      self->fPotTraDay                       =  EPSILON;//Hong added 24/self-DAYL
	  self->fActTraDay                       =  EPSILON;
      
      pGPltC->fGrossStdgCropPhotosynR  = EPSILON;
      pGPltC->fGrossStdgCropPhotosynRN = EPSILON;
	  pPl->pPltCarbon->fGrossPhotosynR = EPSILON;
	  pGPltC->fUptRespCost =  EPSILON; //Test of hong
	  pGPltC->fUptRespCostR =  EPSILON;//Hong

	 // pGS->fDiffSoilAirTemp            = (double)0.0;

	  //%** output expertn ***
	  pWa->pEvap->fPotR                = EPSILON;
	  pWa->pEvap->fActR                = EPSILON; // EP 20130731 wird in 
	  //pWa->fPotETDay                   = (double)0.0;// EP 20130731 wird in "balance.c" berechnet
	  pWa->fPotETR					   = EPSILON; //FH 20180130
	  pWa->fActETR					   = EPSILON;//FH 20180130
	  pWa->fPotETdt                    = EPSILON;//EP 20130731
	  pPl->pPltWater->fPotTranspdt     = EPSILON; //EP 20130731
	  pPl->pPltWater->fActTranspdt     = EPSILON;//EP 20130731
	  pPl->pPltWater->fPotTranspR = EPSILON;
	  pPl->pPltWater->fActTranspR = EPSILON; //Hong 20190725: double with line 556 pPl->pPltWater->fActTranspR = pPl->pRoot->fUptakeR; 
     //end of Hong

	    } //End of else */
	   } //End of if ((pPl->pDevelop->iDayAftEmerg > 0)
	  } //End of if NewDayAndPlantGrowing
	  //pWa->fPotETdt                = pWa->fPotETDay * pTi->pTimeStep->fAct;
	  //pPl->pPltWater->fPotTranspdt = self->fPotTraDay * pTi->pTimeStep->fAct;
    
      return RET_SUCCESS;
  }

// Biomass Growth
int gecros_BiomassGrowth(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;
	double dayTime= xpn->pTi->pSimTime->fTimeDay*(double)24.;
	   
      gecros_integrate_small_time_step_vars(self);

	  //if NewDayAndPlantGrowing
      if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
      {
       //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
       //if ((pPl->pDevelop->iDayAftEmerg > 0)&&(xpn->pCl->pWeather->fSolRad > 0))
	   if (pPl->pDevelop->iDayAftEmerg > 0)
         // if ((dayTime >=self->SUNRIS)&&(dayTime<=self->SUNSET))
		   BiomassGrowth_GECROS(self);
      }
      
      return RET_SUCCESS;
  }

// Canopy Formation
int gecros_CanopyFormation(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;
      double dayTime= xpn->pTi->pSimTime->fTimeDay*(double)24.;
	  
      gecros_integrate_small_time_step_vars(self);

    //if NewDayAndPlantGrowing
	if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    {
     //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
	 if (pPl->pDevelop->iDayAftEmerg > 0)
	 {
		// if ((dayTime >=self->SUNRIS)&&(dayTime<=self->SUNSET))
	         CanopyFormation_GECROS(self);
	 }
    }

      return RET_SUCCESS;
  }

// Root System Formation
int gecros_RootSytem(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;
      double dayTime= xpn->pTi->pSimTime->fTimeDay*(double)24.;
	  
      gecros_integrate_small_time_step_vars(self);

    //if NewDayAndPlantGrowing
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    {
     //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
	 if (pPl->pDevelop->iDayAftEmerg > 0)
	 {
		// if ((dayTime >=self->SUNRIS)&&(dayTime<=self->SUNSET))
             RootSystemFormation_GECROS(self);
	 }
    }

      return RET_SUCCESS;
  }

// Crop Senescence
int gecros_CropSenescence(gecros *self)
  {
    expertn_modul_base *xpn = &(self->parent);
    PCHEMISTRY pCh = xpn->pCh;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	
    PCBALANCE	pCB = pCh->pCBalance; //Added by Hong on 20180731
	PSLAYER    pSL  = pSo->pSLayer;//->pNext;
	PCLAYER    pCL = pCh->pCLayer;//->pNext;
    PLAYERROOT pLR = pPl->pRoot->pLayerRoot;
	
	

    PGECROSCARBON     pGPltC = self->pGecrosPlant->pGecrosCarbon;
    PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;

    double DELT = (double)xpn->pTi->pTimeStep->fAct; //Hong

	int L,L1,NLAYR;
    double fCumDepth,fThickness,TRLDF;//
	double fDeadRootC,fDeadRootN;

      gecros_integrate_small_time_step_vars(self);

    //if NewDayAndPlantGrowing
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    {
     //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
     //if ((pPl->pDevelop->iDayAftEmerg > 0)&&(xpn->pCl->pWeather->fSolRad > 0))
     if (pPl->pDevelop->iDayAftEmerg > 0)
     {
      OrganSenescence_GECROS(self);

	        // Dead biomass to soil litter pools (sg 2005/11/14)

	        //leaves
			//pCh->pCProfile->fCLitterSurf += pGPltC->fCLeafLossR;
			//pCh->pCProfile->fNLitterSurf += pGPltN->fNLeafLossR;
			
			pCh->pCProfile->fCLitterSurf += pGPltC->fCLeafLossR*DELT; //Hong
			pCh->pCProfile->fNLitterSurf += pGPltN->fNLeafLossR*DELT; //Hong
            
			pCh->pCProfile->fCLeafLitterSurf += pGPltC->fCLeafLossR*DELT; //Hong for century_n
			pCB->dCInputSurf += pGPltC->fCLeafLossR*DELT; //Hong added on 20180731 for C-balance
			//pCB->dCInputCum += pGPltC->fCLeafLossR*DELT;
			
			//roots
			//if(pPl->pDevelop->fStageSUCROS>=pPl->pGenotype->fBeginSenesDvs)
			{
				NLAYR=pSo->iLayers-2;
				L = 0;
				fCumDepth = (double)0.0;

				while ((fCumDepth<=pPl->pRoot->fDepth)&&(L<NLAYR))
				{
					L++;
					fThickness =(double)0.1*pSL->fThickness; //cm
					fCumDepth +=(double)0.1*pSL->fThickness; //cm
					if (L<NLAYR-1) pSL =pSL ->pNext;
				}
				L1 = L;

				//Total Root Length Density Factor
				TRLDF =(double)0.0;
				pLR	  =pPl->pRoot->pLayerRoot;
				for (L=1;L<=L1;L++)
				{
					TRLDF += pLR->fLengthDensFac;
					pLR    = pLR->pNext;
				}

				fDeadRootC = pGPltC->fCRootLossR;
				fDeadRootN = pGPltN->fNRootLossR;

				pSL	= pSo->pSLayer->pNext;
				pLR	= pPl->pRoot->pLayerRoot;
				pCL = pCh->pCLayer->pNext;
				fCumDepth =0.0;
				for (L=1;L<=L1;L++)
				{
					fThickness =(double)0.1*pSL->fThickness; //cm
			        
					if(TRLDF > (double)0.0)
					{
						//pCL->fCLitter += fDeadRootC * pLR->fLengthDensFac/TRLDF;
						//pCL->fNLitter += fDeadRootN * pLR->fLengthDensFac/TRLDF;
						
						//Changed *DELT by Hong on 20171124
						pCL->fCLitter += fDeadRootC * pLR->fLengthDensFac/TRLDF*DELT;
						pCL->fNLitter += fDeadRootN * pLR->fLengthDensFac/TRLDF*DELT;
						
						pCB->dCInputProfile += fDeadRootC * pLR->fLengthDensFac/TRLDF*DELT; //Hong added on 20180731 for C-balance
						//pCB->dCInputCum += fDeadRootC * pLR->fLengthDensFac/TRLDF*DELT; 
						//Hong 20190507: balance for 0-30 cm profile:	
	                    fCumDepth +=(double)0.1*pSL->fThickness; //cm
	                   if (fCumDepth <=30.0)
	                   {
						   pCB->dCInputProfile_30 += fDeadRootC * pLR->fLengthDensFac/TRLDF*DELT;
					   }
						
					}

					pSL = pSL->pNext;
					pLR = pLR->pNext;
					pCL = pCL->pNext;
				} // End for(...)
			} //End dead root weight to soil litter pools
		} // End iDayAftEmerg > 0
    }//PlantIsGrowing

      return RET_SUCCESS;
  }

// Nitrogen Demand
int gecros_NitrogenDemand(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;

      gecros_integrate_small_time_step_vars(self);

    //if NewDayAndPlantGrowing
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    {
     //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
	 if (pPl->pDevelop->iDayAftEmerg > 0)
	 {
      NitrogenFixation_GECROS(self);
      PotentialNitrogenUptake_GECROS(self);
	 }
    }

      return RET_SUCCESS;
  }

// Nitrogen Uptake
int gecros_NitrogenUptake(gecros *self)
  {
    expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PPLANT     pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
    double DELT = (double)xpn->pTi->pTimeStep->fAct;
	
      gecros_integrate_small_time_step_vars(self);

    //if NewDayAndPlantGrowing
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    {
     //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
     if (pPl->pDevelop->iDayAftEmerg > 0)
	 {
	  ActualNitrogenUptake_GECROS(self);
	 }
    }
    ///*
	//if PlantIsGrowing
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    	{
		int L;
		PLAYERROOT	pLR	=pPl->pRoot->pLayerRoot;
		PCLAYER 	pSLN=pCh->pCLayer->pNext;

		for (L=1;L<=pSo->iLayers-2;L++)
			{
		    //Check the whether there are roots in this layer:
		    //if (pLR->fLengthDens==(double)0.0)		break;

            //Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
			pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*DELT;
			pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*DELT;		
			
			//pSLN->fNO3N=pSLN->fNO3N-min(pSLN->fNO3N,pLR->fActLayNO3NUpt)*pTi->pTimeStep->fAct;
			//pSLN->fNH4N=pSLN->fNH4N-min(pSLN->fNH4N,pLR->fActLayNH4NUpt)*pTi->pTimeStep->fAct;

			pLR =pLR ->pNext;
			pSLN=pSLN->pNext;
			}
    	}
    //*/

      pPl->pPltNitrogen->fCumActNUpt += pPl->pPltNitrogen->fActNUptR*DELT;

      return RET_SUCCESS;
  }

// Water Uptake
int gecros_ActualTranspiration(gecros *self)
  {
    expertn_modul_base *xpn = &(self->parent);
    PTIME      pTi = xpn->pTi;
    PWATER     pWa = xpn->pWa;
	PPLANT     pPl = xpn->pPl;
	PSPROFILE  pSo = xpn->pSo;

      gecros_integrate_small_time_step_vars(self);

	if ((pPl!=NULL)) pPl->pRoot->fUptakeR =(double)0;
	

	//if NewDayAndPlantGrowing
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
	{                   
	 int L;
	 double fExtWat;

	 PSWATER		pSWL=pSo->pSWater->pNext;
	 PWLAYER		pSLW=pWa->pWLayer->pNext; 
	 PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;
     
	 pPl->pRoot->fUptakeR =(double)0.0;
		
	 for (L=1;L<=pSo->iLayers-2;L++)
	 {
		 pLR->fActLayWatUpt = (double)0;

		 if(pPl->pRoot->fDepth > (double)0)
		 {
          if(L*pSo->fDeltaZ<=pPl->pRoot->fDepth*(double)10)
		  {
	       //LR->fActLayWatUpt = self->fActTraDay*(double)min((double)1,pSo->fDeltaZ/(pPl->pRoot->fDepth*(double)10));
	        pLR->fActLayWatUpt = pPl->pPltWater->fActTranspdt*(double)min((double)1,pSo->fDeltaZ/(pPl->pRoot->fDepth*(double)10));
			//SG 20111122: es darf nur bis zum PWP entleert werden
			fExtWat = max((double)0, pSLW->fContAct-pSWL->fContPWP);
			pLR->fActLayWatUpt = (double)min(fExtWat, pLR->fActLayWatUpt);

		  }
	      else if (((L-1)*pSo->fDeltaZ<=pPl->pRoot->fDepth*(double)10)
			      &&(pPl->pRoot->fDepth*(double)10 < L*pSo->fDeltaZ))
		  {
	        //pLR->fActLayWatUpt = self->fActTraDay*(double)min((double)1,
	        pLR->fActLayWatUpt = pPl->pPltWater->fActTranspdt*(double)min((double)1,
				                 (L*pSo->fDeltaZ-pPl->pRoot->fDepth*(double)10)/(pPl->pRoot->fDepth*(double)10));
			//SG 20111122: es darf nur bis zum PWP entleert werden
			fExtWat = max((double)0, pSLW->fContAct-pSWL->fContPWP);
			pLR->fActLayWatUpt = (double)min(fExtWat, pLR->fActLayWatUpt);
		  }
		  else
	      pLR->fActLayWatUpt = (double)0;
		 }
	 
	  pPl->pRoot->fUptakeR += pLR->fActLayWatUpt/pTi->pTimeStep->fAct;
	  pSLW->fContAct -= pLR->fActLayWatUpt/pSo->fDeltaZ; //[mm/mm/d]

      
	  pSWL=pSWL->pNext;
      pSLW=pSLW->pNext;
	  pLR =pLR ->pNext;
	 }		

	}	//	end if NewDayAndPlantGrowing

    /*
    if ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
    {

                //PSLAYER		pSL	=pSo->pSLayer->pNext;
				PWLAYER		pSLW=pWa->pWLayer->pNext; 
                //PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;

				int L;
			    for (L=1;L<=pSo->iLayers-2;L++)
			    	{
					pSLW->fContAct -= afActLayRootWatUpt[L]
						              /pSo->fDeltaZ
									  *pTi->pTimeStep->fAct; //mm/mm
                    
			      	//pSL =pSL ->pNext;
			      	pSLW=pSLW->pNext;
			      	//pLR =pLR ->pNext;
					}
     } //end if plant is growing   	
     */

   pPl->pPltWater->fActTranspR = pPl->pRoot->fUptakeR;
   pWa->fActTranspCum += pPl->pRoot->fUptakeR * pTi->pTimeStep->fAct;
  
      return RET_SUCCESS;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// XN-GECROS Internal Functions:
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


int gecros_Init_Vars(gecros *self)
{
	expertn_modul_base *xpn = &(self->parent);
	//DECLARE_XPN
	PGECROSSOIL pGS    = self->pGecrosPlant->pGecrosSoil;
//Begin of Hong: changed for C. Troost in Oct. 2016	
	char *ini_filename;	
	char *S;	
//End of Hong
		
//######## initiatlisierung der Variablen, nur einmal #############################
	//for Astronmy
	self->SC=0.0;
	self->DS0=0.0;
	self->SINLD=0.0;
	self->COSLD=0.0;
	self->DAYL=0.0;
	self->DDLP=0.0;
	self->DSINB=0.0;
	self->DSINBE=0.0;
	self->fEmergValue = 0.0;
	
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Model Coefficients/MODEL INPUTS 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double SCV   =      0.20;  //Scattering coefficient of leaves for visible radiation (PAR)
//double AMX   =     40.0;   //Assimilation rate at light saturation (kg CO2/ ha leaf/h )
//double EFF   =      0.45;  //Initial light use efficiency (kg CO2/J/ha/h m2 s )
//double KDIF  =      0.60;  //Extinction coefficient for diffuse light 
//double k     =      0.50;  //The average extinction coefficient for visible and near infrared radiation

///*
//%%% Crop parameters for pea (Pisum sativum L.) %%%
self->LEGUME = -1.; // LEGUME = 1. for leguminous crops;   = -1. for non-leguminous crops.
self->C3C4   = 1.; // C3C4   = 1. for C3 crops;           = -1. for C4 crops.
self->DETER  = 1.;// DETER  = 1. for determinate crops;  = -1. for indeterminate crops.
self->SLP    = -1.;// SLP    = 1. for short-day crops;    = -1. for long-day crops.
self->LODGE  = 1.; // LODGE  = 1. for cases with lodging; = -1. for cases without lodging.
self->VERN   =-1;  // VERN = 1. vernalization model SPASS; = 0. model CERES; =-1. without vernalization.

self->EG     =0.35;// efficiency germination (Par.)                (-)       crop  table 2, p46
self->CFV    =0.48;//carbon frac. in vegetative organs (Par.)   (g C g-1)   crop  table 2, p46
                     //pGPltC->fCFracVegetative=(double)CFV;;
self->YGV    =0.81;//growth efficiency of veg. organs (Par.)    (g C g-1 C)  crop  table 2, p46
                    //pGPar->fGrwEffVegOrg=(double)YGV;
self->FFAT   =0.02;//fat frac.          in storage organ (Par.)   (g g-1)    crop  table 2, p46 
self->FLIG   =0.06;//lignin frac.       in storage organ (Par.)   (g g-1)    crop  table 2, p46
self->FOAC   =0.04;//organic acid frac. in storage organ (Par.)   (g g-1)    crop  table 2, p46
self->FMIN   =0.03;//minerals frac.     in storage organ (Par.)   (g g-1)    crop  table 2, p46
self->LNCI   =0.055;//initial N conc. in living leaves             (g N g-1)  crop  table 3, p47
self->TBD    = 0.0;//base temperature for phenology               (°C)       crop  table 3, p47 
self->TOD    = 27.6;//optimum temperature for phenology           (°C)       crop  table 3, p47
self->TCD    = 36.0;//ceiling temperature for phenology           (°C)       crop  table 3, p47
self->TSEN   = 0.409;//curvature for temperature response         (-)        deflt table 4, p49
self->SPSP   = 0.2; //Dev.stage start of photo-sensitive phase    (-)        deflt table 4, p49
self->EPSP   = 0.7; //Dev.stage end of photo-sensitive phase      (-)        deflt table 4, p49
self->INSP   = -2.0;//Incl. of sun angle for photoper.daylgth.  (degree)     const table 1, p45
self->LWIDTH = 0.025;//Leaf width                                  (m)        crop  table 3, p47
self->RDMX   = 100;//maximal root depth                           (cm)       crop  table 3, p47
                    // -> pGPar->fRootDepthMax
self->CDMHT=345.;//stem dry weight per unit of plant height    (g m-2 m-1)   crop  table 3, p47
self->PMEH=0.8;//sigm.crv.inflec.frac. in ent. plthght.grwth.per. (-)        deflt table 4, p49   
self->PMES=0.5;//sigm.crv.inflec.frac. in entire seed grwth.per.  (-)        deflt table 4, p49
self->ESDI=1.35;//ESD for indeterminate crops                     (-)        deflt table 4, p49
       //-> ESD = INSW(DETER, ESDI, 1.); = (double)pGPPar->fStageEndSeedNum; 
       //ESD development stage for end of seed number determining period
self->CCFIX=6.;//carbon cost of symbiotic nitrogen fixation    (g C g-1 N)   const table 1, p45
                //-> pGPltC->fCNFixCost;
self->NUPTX=0.65;//maximum crop nitrogen uptake                (g N m-2 d-1) deflt table 4, p49
                  //-> pGPltN->fNUptMax;
self->SLA0 = 0.022;//specific leaf area constant               (m2 leaf g-1) crop  table 3, p47
self->SLNMIN = 0.5;//min. specific leaf N content              (g N m-2)     crop  table 3, p47
self->RNCMIN = 0.005;//min. N conc. in roots                   (g N g-1)     crop  table 3, p47
                      //-> pPltN->fRootMinConc;
self->STEMNC = 0.008;//nitrogen conc. in stems                 (g N g-1)     crop  table 3, p47
                      //-> pPltN->fStemActConc;
self->WRB = 0.25;//critical root weight density           (g m-2 cm-1 depth) const table 1, p45 (others)
self->EAJMAX = 48041.88;//activation energy of JMAX            (J mol-1)      crop  table 3, p47
self->XVN = 62.;//slope of lin.rel. btw. VCMAX and leaf N   (umol s-1 g-1 N) crop specific ???     
self->XJN = 124.;//slope of lin.rel. btw. VJMAX and leaf N  (umol s-1 g-1 N) crop specific ???
self->THETA = 0.7;//convexity for light response electron 
                   //transport (J2) in photosynthesis              (-)        const table 1, p45 (photo)

//%*********genotype specific parameters for cv. SOLARA ***********************************************
self->SEEDW=0.215;//Seed weight                               (g seed-1)   
self->SEEDNC=0.05;//Standard seed (storage organ) N conc.    (g N g-1)
self->BLD=50.;//Leaf angle from horizontal                      (degree)
self->HTMX=0.705;//Maximal crop height                           (m)
self->MTDV =34.76;//Minimum thermal days for vegetative phase   (d) 
self->MTDR =23.09;//Minimum thermal days for reproductive phase (d) 
self->PSEN = -0.0;//Photoperiod sensitivity (+ for SD, - for LD) (h-1)

//SG 20180410: additional parameters for sugarbeet
self->SINKBEET = 1000;
self->EFF = 27;
self->CFS = 0.52;


//%** Soil parameters
//double PNLS=1.
//double CLAY=23.4; WCMIN=0.05; WCFC=0.25; WCMAX=0.35
//PARAM DRPM=1.44; DPMR0=10.; RPMR0=0.3; BIOR=0.66; HUMR=0.02
//PARAM TOC=7193.; BHC=3500.; FBIOC=0.03; RN=1.; RA=1.
pGS->fProfileDepth = 150; // added by Hong after XN3 (marker 80009) 
self->RSS = 100.;//soil resistance, equiv.to stomatal resistance [s m-1] 
self->SD1 = 25.;  //thickness of upper soil layer [cm] //TCT=4.; TCP=1.; MULTF=1.
self->TCP = 1.;
self->WCMIN=0.05; //Hong 20170628
//self->SD1 = 2.; //Test of Hong
 
//%** Sensitivity-analysis options
   self->CO2A = (double)350;
   self->COEFR = (double)1.0;
   self->COEFV = (double)1.0;
   self->COEFT = (double)5.0;
   self->FCRSH = (double)0.5;
   self->FNRSH = (double)0.63;
   self->CB = (double)0.7;
   self->CX = (double)1.0;
   self->TM = (double)1.5;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	
//Begin of Hong: changed for C. Troost in Oct. 2016
    
	// Read from crop rotation INI File:
	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.gecros.filename");

	if (ini_filename==NULL) {
		PRINT_ERROR("Missing entry 'gecros.filename' in your configuration!");
	} else {

		//char *S2;
		S = expertn_modul_base_replace_std_templates(xpn,ini_filename);
		if (S!=NULL) {
			ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S);
			if (expertn_modul_base_PlantManagementRead(xpn,ini_filename,"gecros")!=0) {
				PRINT_ERROR("Error Read Crop Rotation INI File");
			}
			self->ini_filename = g_strdup_printf("%s",ini_filename);
			free(ini_filename);
			free(S);
		}

	}
	
 	self->first_round = 0;          
	
	// read crop INI file and initialize variables
	// gecros_load_ini_file(self);// Hong_ should be read on sowDate 
	
//End of Hong

	return RET_SUCCESS;
}

int gecros_load_ini_file(gecros *self)
{
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int option;
	
	//Begin of Hong: changed for C. Troost in Oct. 2016 
	char *read_filename, *S;
	char **varietynames;
	int varietynum;	
	double *dummy_in;
	int dummy_in_size;
	int i;
	PPLANT			pPl = xpn->pPl;
	PGENOTYPE		pGe = pPl->pGenotype;
	//End of Hong
    
    //SG20180530
    char spec[30];
   strcpy(spec,pPl->pGenotype->acCropCode);
    //end SG

//Begin of Hong:changed for C. Troost in Oct. 2016 

	S  = g_strdup_printf("Config.gecros.%s_%s", pPl->pGenotype->acCropName, pPl->pGenotype->acVarietyName);
	read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
	g_free(S);
	
	// if _gecros_crop_variety.ini doesn't exist in libgecros.cfg				
	if (read_filename==NULL)
		{  			 
			S  = g_strdup_printf("Missing entry 'gecros.%s_%s' in your options, if present 'gecros_%s_%s.ini'-file of the Global options is used", pPl->pGenotype->acCropName, pPl->pGenotype->acVarietyName,pPl->pGenotype->acCropName, pPl->pGenotype->acVarietyName);
		    PRINT_MESSAGE(xpn,4,S);
			g_free(S);
				 
			//2. Search for _gecros_crop_variety.ini in .xpi file 
			S  = g_strdup_printf("Global_Config.options.%s_%s_gecros", pPl->pGenotype->acCropName,pPl->pGenotype->acVarietyName);
		    read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
			g_free(S);
			
			 if (read_filename==NULL)
			   {
			     //if _gecros_crop_variety.ini doesn't exist in .xpi file
			     S  = g_strdup_printf("Entry 'global.%s_%s_gecros' is missing in your options!",pPl->pGenotype->acCropName, pPl->pGenotype->acVarietyName);
			     PRINT_MESSAGE(xpn,4,S);
			     g_free(S);
				 
			     //3. Search for _gecros_crop.ini in libgecros.cfg
		         S  = g_strdup_printf("Config.gecros.%s", pPl->pGenotype->acCropName);
		         read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
                 g_free(S);
				 
				  // if _gecros_crop.ini doesn't exist in libgecros.cfg
				 if (read_filename==NULL)
			      {				 
		            S  = g_strdup_printf("Missing entry 'gecros.%s' in your options, if present '%s.ini'-file of the Global options is used", pPl->pGenotype->acCropName,pPl->pGenotype->acCropName);
		            PRINT_MESSAGE(xpn,4,S);
		            g_free(S);
				 
				    //4. Search for _gecros_crop.ini in .xpi file
		            S  = g_strdup_printf("Global_Config.options.%s_gecros", pPl->pGenotype->acCropName);
				    read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
		            g_free(S);
					
					if (read_filename==NULL)
		              {			          
			           PRINT_ERROR("No INI file for GECROS found!")			           
			          }
					
		           }				 
				 
			   }
			
		}	
					
	if (read_filename!=NULL)
		{
			filename=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,read_filename);
			S= g_strdup_printf("INI file for GECROS: %s",filename);
			PRINT_MESSAGE(xpn,3, S);
			g_free(S);
		}	
//End of Hong	
	

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error)) {
		PRINT_ERROR(error->message);
		return -1;
	}
//Begin of Hong:changed for C. Troost in Oct. 2016
    else
        PRINT_MESSAGE(xpn,5,"read crop_gecros.ini");
// End of Hong
 
	GET_INI_INT(option,"control","option");
    //GET_INI_STRING(pPl->pGenotype->acCropCode,"cropcode","CODE");
    //GET_INI_STRING(pPl->pGenotype->acCropName,"cropcode","NAME");
    //GET_INI_STRING(pPl->pGenotype->acVarietyName,"cropcode","VARIETY");
    GET_INI_STRING(pPl->pGenotype->acEcoType,"cropcode","ECOTYPE");
	GET_INI_STRING(pPl->pGenotype->acDaylenType,"cropcode","DAYLENGTH");
    GET_INI_STRING(pPl->pGenotype->acLvAngleType,"cropcode","LEAFANGLE");

	if (option==0)
	{
     DefaultPlantParameters_GECROS(self);
	}
	else
	{
	GET_INI_DOUBLE(self->LEGUME,"croptype","LEGUME");
	GET_INI_DOUBLE(self->C3C4,"croptype","C3C4");
    GET_INI_DOUBLE(self->DETER,"croptype","DETER");
	GET_INI_DOUBLE(self->SLP,"croptype","SLP");
	GET_INI_DOUBLE(self->LODGE,"croptype","LODGE");
	GET_INI_DOUBLE(self->VERN,"croptype","VERN");

	GET_INI_DOUBLE(self->CFV,"biomass","CFV");
	GET_INI_DOUBLE(self->EG,"biomass","EG");
	GET_INI_DOUBLE(self->YGV,"biomass","YGV");
	GET_INI_DOUBLE(self->FFAT,"biomass","FFAT");
	GET_INI_DOUBLE(self->FLIG,"biomass","FLIG");
	GET_INI_DOUBLE(self->FOAC,"biomass","FOAC");
	GET_INI_DOUBLE(self->FMIN,"biomass","FMIN");

	GET_INI_DOUBLE(self->LWIDTH,"morphology","LWIDTH");
	GET_INI_DOUBLE(self->RDMX,"morphology","RDMX");
	GET_INI_DOUBLE(self->CDMHT,"morphology","CDMHT");

	GET_INI_DOUBLE(self->TBD,"phenology","TBD");
	GET_INI_DOUBLE(self->TOD,"phenology","TOD");
	GET_INI_DOUBLE(self->TCD,"phenology","TCD");
	GET_INI_DOUBLE(self->TSEN,"phenology","TSEN");
	GET_INI_DOUBLE_OPTIONAL(pPl->pGenotype->fOptVernDays,"phenology","OptVernDays",0.0);
	GET_INI_INT_OPTIONAL(pPl->pGenotype->iVernCoeff,"phenology","VernCoeff",0);
	GET_INI_DOUBLE_OPTIONAL(pPl->pGenotype->fTempMinVern,"phenology","TempMinDevVern",-1.0);
	GET_INI_DOUBLE_OPTIONAL(pPl->pGenotype->fTempOptVern,"phenology","TempOptDevVern",2.0);
	GET_INI_DOUBLE_OPTIONAL(pPl->pGenotype->fTempMaxVern,"phenology","TempMaxDevVern",15.0);

	GET_INI_DOUBLE(self->NUPTX,"nitrogen","NUPTX");
	GET_INI_DOUBLE(self->RNCMIN,"nitrogen","RNCMIN");
	GET_INI_DOUBLE(self->STEMNC,"nitrogen","STEMNC");
	GET_INI_DOUBLE(self->SLNMIN,"nitrogen","SLNMIN");
	GET_INI_DOUBLE(self->LNCI,"nitrogen","LNCI");
	GET_INI_DOUBLE(self->SLA0,"nitrogen","SLA0");
	GET_INI_DOUBLE(self->CCFIX,"nitrogen","CCFIX");

	GET_INI_DOUBLE(self->EAJMAX,"photosynthesis","EAJMAX");
	GET_INI_DOUBLE(self->XVN,"photosynthesis","XVN");
	GET_INI_DOUBLE(self->XJN,"photosynthesis","XJN");
	GET_INI_DOUBLE(self->THETA,"photosynthesis","THETA");
	

	GET_INI_DOUBLE(self->SEEDW,"genotype","SEEDW");
	GET_INI_DOUBLE(self->SEEDNC,"genotype","SEEDNC");
	GET_INI_DOUBLE(self->BLD,"genotype","BLD");
	GET_INI_DOUBLE(self->HTMX,"genotype","HTMX");
	//Added by Hong on 20180126
	if (self->HTMX>5.0)
	self->HTMX=self->HTMX/100.0; //unit of input is [cm], GECROS needs unit of [m]
	//End of Hong
	GET_INI_DOUBLE(self->MTDV,"genotype","MTDV");
	GET_INI_DOUBLE(self->MTDR,"genotype","MTDR");

	GET_INI_DOUBLE(self->PMEH,"default","PMEH");
	GET_INI_DOUBLE(self->PMES,"default","PMES");
	GET_INI_DOUBLE(self->ESDI,"default","ESDI");
	GET_INI_DOUBLE(self->WRB,"default","WRB");

	GET_INI_DOUBLE(self->CO2A,"sensitivity","CO2A");
	GET_INI_DOUBLE(self->COEFR,"sensitivity","COEFR");
	GET_INI_DOUBLE(self->COEFV,"sensitivity","COEFV");
	GET_INI_DOUBLE(self->COEFT,"sensitivity","COEFT");

    GET_INI_DOUBLE(self->FCRSH,"sensitivity","FCRSH");
	GET_INI_DOUBLE(self->FNRSH,"sensitivity","FNRSH");

	GET_INI_DOUBLE(self->PNPRE,"sensitivity","PNPRE");
	GET_INI_DOUBLE(self->CB,"sensitivity","CB");
	GET_INI_DOUBLE(self->CX,"sensitivity","CX");
	GET_INI_DOUBLE(self->TM,"sensitivity","TM");
//Begin of Hong:changed for C. Troost in Oct. 2016 
/* Hong
	//GET_INI_DOUBLE(self->CLAY,"soil","CLAY");
	GET_INI_DOUBLE(self->WCMIN,"soil","WCMIN");
	GET_INI_DOUBLE(self->WCFC,"soil","WCFC");
	GET_INI_DOUBLE(self->MXPWC,"soil","MXPWC");
	GET_INI_DOUBLE(self->SD1,"soil","SD1");
*/
    //photoperiod
    GET_INI_DOUBLE(self->SPSP,"photoperiod","SPSP");
	GET_INI_DOUBLE(self->EPSP,"photoperiod","EPSP");
	GET_INI_DOUBLE(self->INSP,"photoperiod","INSP");
	GET_INI_DOUBLE(self->PSEN,"photoperiod","PSEN");
    
     //SG 20180409: sugarbeet model J. Rabe
     //sugarbeet specific
     if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
      {
             GET_INI_DOUBLE_OPTIONAL(self->SINKBEET,"sugarbeet specific","SINKBEET",1000);
	         GET_INI_DOUBLE_OPTIONAL(self->EFF,"sugarbeet specific","EFF",27);
	         GET_INI_DOUBLE_OPTIONAL(self->CFS,"sugarbeet specific","CFS",0.5);
      }

    //residue partition
    GET_INI_STRING_ARRAY_OPTIONAL(varietynames,varietynum,1,"missing","residue partition","VarietyName");	
	for(i=0; i<varietynum; i++)
		{
			deleteSpaceBegEnd(varietynames[i]);
			if(strcmp(pGe->acVarietyName,varietynames[i])==0) break;
			if((i==varietynum-1)&&(strcmp(pGe->acVarietyName,varietynames[varietynum-1])!=0))
				{
					i = 0;
					S  = g_strdup_printf("Variety \"%s\" not found in %s.ini - residue partition: Default values are used.", pGe->acVarietyName,pGe->acCropName);
					PRINT_ERROR(S);
					g_free(S);
					break;
				}
		}

	g_free(varietynames);
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,1.0,"residue partition","ResidueToAMO1_frac");//default value 1.0
	pPl->pGenotype->fResidueAMO1Frac = dummy_in[i];
	g_free(dummy_in);
	
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,0.4,"residue partition","C_fromDeadleaf_frac");//default value 0.4	
	pPl->pGenotype->fCDeadleafFrac = dummy_in[i];
	g_free(dummy_in);
	
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,1.0,"residue partition","N_fromDeadleaf_frac");//default value 1.0	
	pPl->pGenotype->fNDeadleafFrac = dummy_in[i];
	g_free(dummy_in);

	PRINT_MESSAGE(xpn,4,"Initialisation using input parameter values from crop_gecros.ini");
   // End of Hong
	}

	if (!strcmp(xpn->pPl->pGenotype->acCropCode,"PE"))
	{
		xpn->pMa->pSowInfo->fPlantDens = 400.;
		xpn->pMa->pSowInfo->fSowDepth = 10.;
		xpn->pMa->pSowInfo->fRowWidth = 50.;
	} //Hong:warum hier?

	g_key_file_free(keyfile);
	
    //Hong for crop rotation 20170131: to initialize plant variables:
    gecros_PlantVariableInitiation(self); 

	return RET_SUCCESS;
}

int gecros_PlantVariableInitiation(gecros *self)//Hong 20170127
{
	expertn_modul_base *xpn = &(self->parent);

    PPLTNITROGEN    pPltN = xpn->pPl->pPltNitrogen;
    PGENOTYPE       pGen  = xpn->pPl->pGenotype;
    PDEVELOP        pDev  = xpn->pPl->pDevelop;
    PCANOPY         pCan  = xpn->pPl->pCanopy;	
	PPLANT			pPl   = xpn->pPl;
	PBIOMASS		pGrw	=pPl->pBiomass;
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
	PSLAYER 		pSL		=xpn->pSo->pSLayer;	
    PMSOWINFO		pSI	= xpn->pMa->pSowInfo;
    PSPROFILE		pSo = xpn->pSo;	
	
	PGECROSCARBON     pGPltC;
    PGECROSNITROGEN   pGPltN;
    PGECROSCANOPY     pGCan;
    
    pGPltC = self->pGecrosPlant->pGecrosCarbon;
    pGPltN = self->pGecrosPlant->pGecrosNitrogen;
    pGCan  = self->pGecrosPlant->pGecrosCanopy;
		
	double 		CUMDEP,fThickness;
	int   			L,L1;
	char			*pCropCode;
	double fSowDepth;
	
	pCropCode = (char*)malloc(sizeof(pPl->pGenotype->acCropCode));
	strcpy(pCropCode,pPl->pGenotype->acCropCode);
	if(pSI->fSowDepth>(double)0.0)
		fSowDepth = pSI->fSowDepth;
	else
		fSowDepth = (double)3.0;
	//####################### Initialization specially for GECROS ###########################	
	//Begin of Hong: changed for C. Troost in Oct. 2016	
	  self->NPL = (double)xpn->pMa->pSowInfo->fPlantDens;
      xpn->pPl->pCanopy->fPlantDensity =  (double)xpn->pMa->pSowInfo->fPlantDens;
      self->FPRO   = (double)6.25*self->SEEDNC;
	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is 'storage organ'
	  if((strcmp(pCropCode,"SB")==0)||(strcmp(pCropCode,"OR")==0))
		  self->FPRO   = (double)6.25*self->STEMNC; //sugar beet

	  self->FCAR   = (double)1.-self->FPRO-self->FFAT-self->FLIG-self->FOAC-self->FMIN;
      self->CFO    = (double)0.444*self->FCAR + (double)0.531*self->FPRO + (double)0.774*self->FFAT
                    +(double)0.667*self->FLIG +(double)0.368*self->FOAC;
      self->YGO    = self->CFO/((double)1.275*self->FCAR + (double)1.887*self->FPRO 
		            + (double)3.189*self->FFAT + (double)2.231*self->FLIG + (double)0.954*self->FOAC)
					* (double)30./(double)12.;

      self->CLVI   = self->NPL*self->SEEDW*self->CFO*self->EG*self->FCRSH;
      self->CRTI   = self->NPL*self->SEEDW*self->CFO*self->EG*((double)1.-self->FCRSH);

//Hong 2016-08-08: change for Sebastian Gayler and Arne Poyda
//      self->NLVI   = self->LNCI*self->CLVI/self->CFV;
//      self->NRTI   = self->NPL*self->SEEDW*self->EG*self->LNCI*self->FCRSH/self->FNRSH - self->NLVI;

//AP20160616
	  self->NLVI   = min(self->FNRSH * self->NPL * self->SEEDW * self->EG * self->SEEDNC, self->LNCI * self->CLVI/self->CFV);
	  self->NRTI   = self->NPL * self->SEEDW * self->EG * self->SEEDNC - self->NLVI;

	  //SG 20180409: sugarbeet model J. Rabe - accounting for small seed number and low seed weights
	  if((strcmp(pCropCode,"SB")==0)||(strcmp(pCropCode,"OR")==0))
	  {
		self->CLVI   = self->NPL * self->SEEDW * self->EFF * self->CFS * self->EG * self->FCRSH;
		self->CRTI   = self->NPL * self->SEEDW * self->EFF * self->CFS * self->EG * (1.-self->FCRSH);

		self->NLVI   = self->LNCI* self->CLVI/self->CFV;
		self->NRTI   = self->NPL * self->SEEDW * self->EFF * self->EG * self->LNCI * self->FCRSH/self->FNRSH - self->NLVI;
	  }
//End of Hong      
	  
      self->LNCMIN = self->SLA0*self->SLNMIN;
      self->LAII   = self->CLVI/self->CFV*self->SLA0;
	  self->SLNBI  = self->NLVI/self->LAII;

      self->RDI    = max((double)2.,(double)xpn->pMa->pSowInfo->fSowDepth);
      self->HTI    = self->HTMX/(double)1000.;
    
      //output
      pGPltC->fCFracStorage = (double)self->CFO;
      pGen->fGrwEffStorage  = (double)self->YGO;

      pGPltC->fCLeaf        = (double)self->CLVI;
      pGPltC->fCStrctRoot   = (double)self->CRTI;

      pPltN->fLeafCont      = (double)self->NLVI;
      pGPltN->fNLeafTotCont = (double)self->NLVI;
      //SG20110909
	  pPltN->fRootCont      = (double)(self->NRTI*10.0); //[g/m2] --> [kg/ha]
      //pPltN->fRootCont      = (double)NRTI;

      pPltN->fLeafMinConc   = (double)self->LNCMIN;
      pPltN->fRootMinConc   = (double)self->RNCMIN;
      
	  pGCan->fLAICdeterm    = (double)self->LAII;
      pGCan->fLAITotal      = (double)self->LAII;
      pGCan->fLAIGreen      = (double)self->LAII;
      pGPltN->fNLeafSpecificContBottom = (double)self->SLNBI;

      xpn->pPl->pRoot->fDepth = (double)self->RDI;
      pCan->fPlantHeight      = (double)self->HTI;

//################################# End of for GECROS ###########################	


//#################### Initialization for XN5 structure #############################
//############# according to expertn_modul_base_PlantVariableInitiation ############# 
	
	//======================================================================================
	//Development Stage initialization 
	//======================================================================================
	pPl->pDevelop->fDevStage = (int)-99;
	pPl->pDevelop->fStageXCERES = (int)-99;
	pPl->pDevelop->fStageSUCROS = (int)-99;
	pPl->pDevelop->iDayAftEmerg = (int)0;

	SET_IF_99(pPl->pDevelop->fDevStage,(double)0.0); 
	SET_IF_99(pPl->pDevelop->fStageXCERES,(double)0.0);	
	SET_IF_99(pPl->pDevelop->fStageSUCROS,(double)0.0);
	
	//Begin of Hong 20171001
	pDev->fStageSUCROS =-1;	  
	//End of Hong	
	if (pSI->fPlantDens == (double)0.0) pSI->fPlantDens = (double)400; //Hong: könnte bleiben
	if (pSI->fRowWidth  == (double)0.0) pSI->fRowWidth  = (double)10;//Hong: könnte bleiben
	if (pSI->fSowDepth  == (double)0.0) pSI->fSowDepth  = (double)3;//Hong: könnte bleiben
	
	//======================================================================================
	//Plant biomass content initialization 
	//======================================================================================	
	pGrw->fLeafWeight   =(double)0;
	pGrw->fRootWeight   =(double)0;
	pGrw->fStemWeight   =(double)0;
	pGrw->fStovWeight   =(double)0;
	pGrw->fTotalBiomass =(double)0;
	pGrw->fGrainWeight	 =(double)0;
	//======================================================================================
	//Plant nitrogen content initialization
	//======================================================================================	
	pPltN->fLeafActConc  = 0;
	pPltN->fRootActConc  = 0;
	pPltN->fStemActConc  = 0;
	pPltN->fTopsActConc	=(double)0.050;//Hong: könnte bleiben
	pPltN->fGrainCont	=(double)0.0;
	
	//======================================================================================
	//Dry matter initialization
	//======================================================================================
	if ((strcmp(pCropCode,"WH")==0)
	        ||((strcmp(pCropCode,"BA")==0)||(strcmp(pCropCode,"S")==0))
	        ||((strcmp(pCropCode,"RY")==0)))
		{
			if (strcmp(pCropCode,"WH")==0)
				pGrw->fSeedReserv = (double)(0.045/2.0*10.0)*pSI->fPlantDens; // kg/ha
			if (strcmp(pCropCode,"BA")==0)
				pGrw->fSeedReserv = (double)(0.045/2.0*10.0)*pSI->fPlantDens; // kg/ha
			if (strcmp(pCropCode,"S")==0)
				pGrw->fSeedReserv = (double)(0.05/2.0*10.0)*pSI->fPlantDens; // kg/ha
			if (strcmp(pCropCode,"RY")==0)
				pGrw->fSeedReserv = (double)(0.035/2.0*10.0)*pSI->fPlantDens; // kg/ha
			//Biomass
			pGrw->fLeafWeight				= (double)0.5*pGrw->fSeedReserv;
			pGrw->fStemWeight				= (double)0.0*pGrw->fSeedReserv;
			pGrw->fStovWeight				= pGrw->fLeafWeight;
			pGrw->fRootWeight				= (double)0.5*pGrw->fSeedReserv;
		
			if ((strcmp(pCropCode,"WH")==0)||((strcmp(pCropCode,"RY")==0)))
				{
				//Test of Hong	pPltN->fLeafActConc		=(double)0.044;
				//Test of Hong	pPltN->fStemActConc		=(double)0.044;
					
				//Test of Hong	pPltN->fTopsActConc=(double)0.044;
				}
			if ((strcmp(pCropCode,"BA")==0)||((strcmp(pCropCode,"S")==0)))
				{
					//Test of Hong pPltN->fLeafActConc		=(double)0.045;
					//Test of Hong pPltN->fStemActConc		=(double)0.045;
					
					//Test of Hong pPltN->fTopsActConc=(double)0.045;
				}
		}
	//==========================================================================
	//MAIZE
	//==========================================================================
	if (strcmp(pCropCode,"MZ")==0)
		{			
			pGrw->fSeedReserv = (double)(0.3/2.0*10.0)*pSI->fPlantDens; // kg/ha
			pGrw->fLeafWeight				= (double)0.5*pGrw->fSeedReserv;
			pGrw->fStemWeight				= (double)0.0*pGrw->fSeedReserv;
			pGrw->fStovWeight				= pGrw->fLeafWeight;
			pGrw->fRootWeight				= (double)0.5*pGrw->fSeedReserv;
			pGrw->fTotalBiomass				= pGrw->fSeedReserv;
			
			//Test of Hong pPltN->fLeafActConc		=(double)0.044;
			//Test of Hong pPltN->fStemActConc		=(double)0.044;			
			//Test of Hong pPltN->fTopsActConc=(double)0.044;
		}
	
	//==========================================================================
	//Sugarbeet
	//==========================================================================
	if ((strcmp(pCropCode,"SB")==0)||(strcmp(pPl->pGenotype->acCropCode,"BS")==0)) // um Fehler von "BS" zu vermeiden
		{
			//SG20180424
            //if (pSI->fPlantDens<(double)50)
			//	pSI->fPlantDens=(double)90;
            if (pSI->fPlantDens<(float)5) //SG20160510
	   		pSI->fPlantDens=(float)8;
			//Biomass		
			pGrw->fLeafWeight				=(double)0.00136	*(double)10.0*pSI->fPlantDens;
			pGrw->fSeedReserv				=(double)0.05	*(double)10.0*pSI->fPlantDens;
			pGrw->fStemWeight				=(double)0.0	*(double)10.0*pSI->fPlantDens;
			pGrw->fGrainWeight	            =(double)0;
			pGrw->fStovWeight				= pGrw->fLeafWeight;
			pGrw->fRootWeight				=(double)0.5		*pGrw->fSeedReserv;
			pGrw->fTotalBiomass				=(double)0.0;
			//nitrogen
			pPltN->fLeafActConc		=(double)0.06;
			pPltN->fStemActConc		=(double)0.0;
			pPltN->fRootActConc=(double)0.0215;
			
		}

	//======================================================================================
	//Root length density initialization
	//======================================================================================
	//pPl->pRoot->fDepth= fSowDepth;
	pLR	  =pPl->pRoot->pLayerRoot;
	pSL	  =pSo->pSLayer->pNext;
	CUMDEP=(double)0.0;
	for (L=1; L<=pSo->iLayers-2; L++)
		{
			fThickness =(double)0.1*pSL->fThickness; //cm
			CUMDEP=CUMDEP+fThickness;
			//IF_CROP_IS_UNKNOWN
			pLR->fLengthDens=(double)0.20*pSI->fPlantDens/fThickness;
			if ((strcmp(pCropCode,"WH")==0)||(strcmp(pCropCode,"BA")==0))
				pLR->fLengthDens=(double)0.0024*pSI->fPlantDens/fThickness;
			if (strcmp(pCropCode,"PT")==0)
				pLR->fLengthDens=(double)0.2*pSI->fPlantDens/fThickness;
			if (CUMDEP>pPl->pRoot->fDepth)
				break;
			pSL=pSL->pNext;
			pLR=pLR->pNext;
		}
	pLR->fLengthDens = pLR->fLengthDens*
	                   ((double)1.0-(CUMDEP-pPl->pRoot->fDepth)/((double)0.1*pSL->fThickness));
	L1=L+1;
	pLR=pLR->pNext;
	if (L1<pSo->iLayers)
		{
			for (L=L1; L<=pSo->iLayers; L++)
				{
					pLR->fLengthDens=(double)0.0;
					pLR=pLR->pNext;
				}
		}	
//############## End of expertn_modul_base_PlantVariableInitiation ###################
     
	  PRINT_MESSAGE(xpn,5,"initialize plant variables");
	  
 return RET_SUCCESS;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    DefaultPlantParameters_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   DefaultPlantParameters_GECROS(gecros *self)
{
  expertn_modul_base *xpn = &(self->parent);
  
  PPLANT pPl = xpn->pPl;
  
  char spec[30], var[30];
  
  strcpy(spec,pPl->pGenotype->acCropCode);
  strcpy(var,pPl->pGenotype->acVarietyName);

  if (!strcmp(spec,"RY")) strcpy(spec,"WH");
  
  if (!strcmp(spec,"WH"))
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)+1;
   self->DETER  = (double)+1;
   self->SLP    = (double)+1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.25;
   self->FFAT   = (double)0.02;
   self->FLIG   = (double)0.06;
   self->FOAC   = (double)0.02;
   self->FMIN   = (double)0.02;

   //table 3, p.47
   self->TBD    = (double)0.0;
   self->TOD    = (double)25.0;
   self->TCD    = (double)37.0;
   self->LWIDTH = (double)0.01;
   self->CDMHT  = (double)460.;
   self->RDMX   = (double)130.;
   self->SLNMIN = (double)0.35;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.01;
   self->SLA0   = (double)0.028;
   self->EAJMAX = (double)48270;

   //genotype specific	   
   self->SEEDW  = (double)0.4;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)1.2;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;
  }

  if (!strcmp(spec,"BA"))
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)+1;
   self->DETER  = (double)+1;
   self->SLP    = (double)-1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.25;
   self->FFAT   = (double)0.01;
   self->FLIG   = (double)0.04;
   self->FOAC   = (double)0.02;
   self->FMIN   = (double)0.04;

   //table 3, p.47
   self->TBD    = (double)0.0;
   self->TOD    = (double)25.0;
   self->TCD    = (double)37.0;
   self->LWIDTH = (double)0.01;
   self->CDMHT  = (double)450;
   self->RDMX   = (double)130.;
   self->SLNMIN = (double)0.3;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.01;
   self->SLA0   = (double)0.031;
   self->EAJMAX = (double)30200;

   //genotype specific	   
   self->SEEDW  = (double)0.4;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)1.2;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;
  }

  if (!strcmp(spec,"MZ"))
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)-1;
   self->DETER  = (double)+1;
   self->SLP    = (double)-1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.25;
   self->FFAT   = (double)0.05;
   self->FLIG   = (double)0.11;
   self->FOAC   = (double)0.01;
   self->FMIN   = (double)0.01;

   //table 3, p.47
   self->TBD    = (double)8.0;
   self->TOD    = (double)30.0;
   self->TCD    = (double)42.0;
   self->LWIDTH = (double)0.05;
   self->CDMHT  = (double)570.;
   self->RDMX   = (double)145.;
   self->SLNMIN = (double)0.25;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.008;
   self->SLA0   = (double)0.022;
   self->EAJMAX = (double)70890;

   //genotype specific	   
   self->SEEDW  = (double)0.38;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)2.7;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;
  }

  if (!strcmp(spec,"PT"))
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)+1;
   self->DETER  = (double)+1;
   self->SLP    = (double)+1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.25;
   self->FFAT   = (double)0.0;
   self->FLIG   = (double)0.03;
   self->FOAC   = (double)0.05;
   self->FMIN   = (double)0.05;

   //table 3, p.47
   self->TBD    = (double)0.0;
   self->TOD    = (double)25.0;
   self->TCD    = (double)37.0;
   self->LWIDTH = (double)0.025;
   self->CDMHT  = (double)170.;
   self->RDMX   = (double)100.;
   self->SLNMIN = (double)0.35;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.01;
   self->SLA0   = (double)0.033;
   self->EAJMAX = (double)50000;//???

   //genotype specific	   
   self->SEEDW  = (double)0.4;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)1.2;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;

  }

  if (!strcmp(spec,"SF"))
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)+1;
   self->DETER  = (double)+1;
   self->SLP    = (double)+1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.45;
   self->FFAT   = (double)0.29;
   self->FLIG   = (double)0.05;
   self->FOAC   = (double)0.03;
   self->FMIN   = (double)0.03;

   //table 3, p.47
   self->TBD    = (double)8.0;
   self->TOD    = (double)30.0;
   self->TCD    = (double)42.0;
   self->LWIDTH = (double)0.1;
   self->CDMHT  = (double)270.;
   self->RDMX   = (double)145.;
   self->SLNMIN = (double)0.3;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.01;
   self->SLA0   = (double)0.025;
   self->EAJMAX = (double)50000;//???

   //genotype specific	   
   self->SEEDW  = (double)0.4;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)1.2;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;

  }

  if (!strcmp(spec,"RP"))
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)+1;
   self->DETER  = (double)+1;
   self->SLP    = (double)+1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.45;
   self->FFAT   = (double)0.48;
   self->FLIG   = (double)0.05;
   self->FOAC   = (double)0.02;
   self->FMIN   = (double)0.04;

   //table 3, p.47
   self->TBD    = (double)0.0;
   self->TOD    = (double)25.0;
   self->TCD    = (double)37.0;
   self->LWIDTH = (double)0.25;
   self->CDMHT  = (double)285.;
   self->RDMX   = (double)130.;
   self->SLNMIN = (double)0.3;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.01;
   self->SLA0   = (double)0.030;
   self->EAJMAX = (double)50000;//???

   //genotype specific	   
   self->SEEDW  = (double)0.4;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)2.7;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;
  }


  if (!strcmp(spec,"SB")) //Hong: SB=>sugar beet
  {
   self->LEGUME = (double)-1;
   self->C3C4   = (double)+1;
   self->DETER  = (double)+1;
   self->SLP    = (double)+1;
   self->LODGE  = (double)-1;
   self->VERN   = (double)-1;

   //table 2, p.46
   self->YGV    = (double)0.81;
   self->CFV    = (double)0.48;
   self->EG     = (double)0.25;
   self->FFAT   = (double)0.0;
   self->FLIG   = (double)0.05;
   self->FOAC   = (double)0.04;
   self->FMIN   = (double)0.04;

   //table 3, p.47
   self->TBD    = (double)0.0;
   self->TOD    = (double)25.0;
   self->TCD    = (double)37.0;
   self->LWIDTH = (double)0.08;
   self->CDMHT  = (double)150.;
   self->RDMX   = (double)120.;
   self->SLNMIN = (double)0.25;
   self->LNCI   = (double)0.05;
   self->RNCMIN = (double)0.005;
   self->STEMNC = (double)0.01;
   self->SLA0   = (double)0.02;
   self->EAJMAX = (double)50000;//???

   //genotype specific	   
   self->SEEDW  = (double)0.4;
   self->SEEDNC = (double)0.05;
   self->BLD    = (double)50.0;
   self->HTMX   = (double)1.2;
   self->MTDV   = (double)40.0;
   self->MTDR   = (double)50.0;
   self->PSEN   = (double)-0.0;
  }

   //photosynthesis parameters genotype specific ???
   //self->XJN = 2 * self->XVN !!! see p.44: in general XVN = 60 ?
   self->XVN    =(double)62.0;
   self->XJN    =(double)124.0;


   //table 1, p.45
   self->THETA = (double)0.7;
   self->INSP  = (double)-2;
   self->WRB   = (double)0.25;
   self->CCFIX = (double)6;

   //table 4, p.49
   self->PMEH  = (double)0.8;
   self->PMES  = (double)0.5;
   self->TSEN  = (double)1.0;
   self->PNPRE = (double)0.8;
//   self->TSEN  = (double)1.0; // Hong: doubled
   self->SPSP  = (double)0.2;
   self->EPSP  = (double)0.7;
   self->ESDI  = (double)1.35;
   self->NUPTX = (double)0.5; 

//Begin of Hong	   
   //added by Hong: following parameters are necessary for the model initialization!! after XN3 in function Init_GECROS(EXP_POINTER)
   self->CO2A = (double)350;
   self->COEFR = (double)1.0;
   self->COEFV = (double)1.0;
   self->COEFT = (double)5.0;
   self->FCRSH = (double)0.5;
   self->FNRSH = (double)0.63;
   self->PNPRE = (double)0.7;
   self->CB = (double)0.75;
   self->CX = (double)1.0;
   self->TM = (double)1.5;

   PRINT_MESSAGE(xpn,1,"'option'= 0 => Initialisation using default parameter values.");
//End of Hong
return 1;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      PhasicDevelopment_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int PhasicDevelopment_GECROS(gecros *self)
    {
      expertn_modul_base *xpn = &(self->parent);
	  PLOCATION pLo = xpn->pLo; 
	  PTIME pTi = xpn->pTi;
	  PPLANT pPl = xpn->pPl;

	  PDEVELOP      pDev = pPl->pDevelop;
	  PGECROSSOIL   pGS  = self->pGecrosPlant->pGecrosSoil;	  
//Hong 2016-08-08: change for Sebastian Gayler and Arne Poyda
      //PMANAGEMENT pMa = xpn->pMa;
	  
	  int day,month,year;
//End of Hong
      int    iJulianDay;
      double dLatitude;
	  double DAYL;
      double DS,TDU,CTDU,DVR;
	  double TMAX,TMIN,DIFS,TBD,TOD,TCD,TSEN;
	  double fMaxTemp,fOptTemp,fMinTemp;

	  //double DELT = (double)1;
	  double DELT = (double)xpn->pTi->pTimeStep->fAct;

      double DVS[]={0.0, 0.4, 0.55, 0.656, 0.91, 1.00, 1.15, 1.50, 1.95, 2.0};
	  double  VR[10];
      int i;

//Hong, 2016-08-08: change for Sebastian Gayler and Arne Poyda
//SG 20160706: für Berechnung von Keimung und Feldaufgang in Abhängigkeit von Umwelteinflüssen
	  int iGermination, iEmerg;
//added by Hong for vernalization
      double fTemp;
      fTemp = xpn->pCl->pWeather->fTempAir;
//End of Hong

	  //SG 20160706: Berechnung von Keimung (Germination) und Feldaufgang (Emergence) 
	 // pPl->pDevelop->fStageXCERES=(double)7.0;
	  
	  if (pDev->fStageSUCROS < 0)
	  {
		  
//		  if(pPl->pModelParam->iEmergenceDay>0) //Wenn "Date of Emergence" in Datenbank oder xnd angegeben, dann erfolgt Feldaufgang an diesem Tag (=SUCROS)		    
			   if (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->EmergenceYear,pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceDay)==0)
		    {
			  iGermination = 1;
              iEmerg = -1;
			  pDev->fStageSUCROS= (double)-0.5;

//			  if(pTi->pSimTime->fTimeAct >= (double)(pPl->pModelParam->iEmergenceDay-1)) // Hong: replaced by following 5 lines
	          day=pTi->pSimTime->mday;
	          month=pTi->pSimTime->mon;
	          year=pTi->pSimTime->year;
		 	  xpn_time_date_add_dt(&year,&month,&day,1.0);
              if (xpn_time_compare_date(year,month,day,pPl->pModelParam->EmergenceYear, pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceDay)>=0) 	  				  				  				  
			  {
				  //initially zero
				   pDev->fStageSUCROS= (double)0.0;
				   pDev->iDayAftEmerg = 0;
				   pDev->fCumTDU = (double)0.0;
	   
				   CTDU =(double)0;
				   DS =(double)0;

				   iEmerg = 1;
			  }
		     }
		  
		  else //SG 20160706: Berechnung von Keimung (Germination) und Feldaufgang (Emergence) in Abhängigkeit von Umwelteinflüssen nach CERES
		  {
			  if (pDev->fStageSUCROS < -0.51)
			  {
				  //Germination (nach Ceres):
				  iGermination = gecros_Germination(self); //aus Ceres.c
		      
				 if (iGermination < 1)
				 {
					 pDev->fDevR = (double)0.0;//rate
					 return -1;
				 }
				 else
				 {
					  pDev->fStageSUCROS= (double)-0.5;
					  iEmerg = -1;
				 }
			  } 
			  else //emergence
			  {
				  iEmerg = Emergence_GECROS(self);

			  			   
				  if (iEmerg == 1)
				  {
					pDev->fStageSUCROS= (double)0.0;
					pDev->iDayAftEmerg = 0;
					pDev->fCumTDU = (double)0.0;
	   
					CTDU =(double)0;
					DS =(double)0;
				  }
			  }
				
		  } //Ende der Berechnung von Keimung und Feldaufgang nach CERES
		  			  
		  if (iEmerg <1)
		  {
			 pDev->fDevR = (double)0.0;//rate
			 return -1;
		  }

  } 	//Ende der Berechnung von Keimung und Feldaufgang, if (pDev->fStageSUCROS < 0)

      //-----------------------------------------------------------------------------------------------
	  // End of calculating germination and emergence
	  //-----------------------------------------------------------------------------------------------

//End of Hong

    //-- Photoperiod, solar constant and daily extraterrestrial radiation
	  iJulianDay  = (int)pTi->pSimTime->fTimeY;
      dLatitude   = (double)pLo->pFarm->fLatitude;

      Astronomy_GECROS(self,iJulianDay, dLatitude);

//-------------------------------------------------------------------------------------

      DS   = (double)pDev->fStageSUCROS;
	  
	  //Begin of Hong
      TMAX = (double) self->weather.fTempMax;     //[°C]
      TMIN = (double) self->weather.fTempMin;     //[°C]	  
	  //TMAX = (double)xpn->pCl->pWeather->fTempMax;    //[°C] //Hong for test
      //TMIN = (double)xpn->pCl->pWeather->fTempMin;    //[°C]
	  //TEMP= pCl->pWeather->fTempAir;                 //[°C]
	  //End of Hong
	  
	  //DIFS = max(0,pCl->pWeather->fTempAir-xpn->pHe->pHLayer->pNext->fSoilTemp); //[°C]
	  DIFS = (double)pGS->fDiffSoilAirTemp; //[°C]
	  DAYL = self->DAYL;
      TBD  = self->TBD;//base temperature for phenology     [°C]  crop  table 3, p47 
      TOD  = self->TOD;//optimum temperature for phenology  [°C]  crop  table 3, p47
      TCD  = self->TCD;//ceiling temperature for phenology  [°C]  crop  table 3, p47
      TSEN = self->TSEN;//curvature for temperature response [-]  deflt table 4, p49

	  //%-- Developmental stage (DS) & cumulative thermal units (CTDU)      
      TDU =(double)0;
	  CTDU=(double)pDev->fCumTDU;//Hong
	  
      //TDU=DailyThermalDayUnit_GECROS(DS,TMAX,TMIN,DIFS,DAYL,TBD,TOD,TCD,TSEN);
	  TDU=DailyThermalDayUnit_GECROS(self,DS,fTemp,DIFS,DAYL,TBD,TOD,TCD,TSEN);//Hong for test
	  pDev->fDTT = (double)TDU;
	
	  //Vernalization Process nach CERES
	  if (self->VERN == (double)0)
	  {
	   gecros_PlantTemperature(self);     
	   gecros_Vernalization_CERES(self);//activated by Hong 20170704
	  }
      
      ///*
	  //Vernalization Process nach SPASS
	  if (self->VERN > (double)0)
	  {
	  //Begin of Hong
      TMAX = (double) self->weather.fTempMax;     //[°C]
      TMIN = (double) self->weather.fTempMin;     //[°C]	  
	  //TMAX = (double)xpn->pCl->pWeather->fTempMax;    //[°C] //Hong for test
      //TMIN = (double)xpn->pCl->pWeather->fTempMin;    //[°C]
	  //End of Hong
	   fMinTemp= pPl->pGenotype->fTempMinVern;	
	   fOptTemp= pPl->pGenotype->fTempOptVern;
	   fMaxTemp= pPl->pGenotype->fTempMaxVern; 
       
	   //fMinTemp= (double) -1.3;	
	   //fOptTemp= (double)  4.9;
	   //fMaxTemp= (double) 15.7; 
	   
	  // pDev->fVernUnit = gecros_DailyVernalization_SPASS(fTempMin,fTempMax,pPl->pGenotype->VernRateTemp,fMinTemp,fOptTemp,fMaxTemp); 	   
	   pDev->fVernUnit = gecros_DailyVernalization_SPASS(TMAX,TMIN,fTemp,pPl->pGenotype->VernRateTemp,fMinTemp,fOptTemp,fMaxTemp,self);//added on 20170704 by Hong
	   //pDev->fCumVernUnit += pDev->fVernUnit;
	   pDev->fCumVernUnit += pDev->fVernUnit*DELT; //activated on 20170704 by Hong
      
	  }//*/
       
	  //DVR=Phenology_GECROS(self,DS,self->SLP,self->DDLP,self->SPSP,self->EPSP,self->PSEN,self->MTDV,self->MTDR,TDU);
      DVR=Phenology_GECROS(self,DS,self->SLP,self->DDLP,self->SPSP,self->EPSP,self->PSEN,self->MTDV,self->MTDR,TDU, pPl->pGenotype->fOptVernDays);//added on 20170704 by Hong

    //%--  Phenological development
      if (pDev->fStageSUCROS<0)
      {
      //initially zero
       pDev->fStageSUCROS= (double)0.0;
       pDev->iDayAftEmerg = 0;
       pDev->fCumTDU = (double)0.0;

       CTDU =(double)0;
       DS =(double)0;
      }
      else
      {
       DS = (double)pDev->fStageSUCROS;   
       
	  //output from fStageSUCROS to pDev->fStageWang
      for (i=0;i<10;i++) VR[i]=(double)DVS[i];
      if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1]))
            pDev->fDevStage=(double)(10.0*(1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0])));
      if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2]))
            pDev->fDevStage=(double)(10.0*(2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1])));
      if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3]))
            pDev->fDevStage=(double)(10.0*(3.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2])));
      if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4]))
            pDev->fDevStage=(double)(10.0*(4.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3])));
      if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5]))
            pDev->fDevStage=(double)(10.0*(5.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4])));
      if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6]))
            pDev->fDevStage=(double)(10.0*(6.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5])));
      if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7]))
            pDev->fDevStage=(double)(10.0*(7.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6])));
      if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8]))
            pDev->fDevStage=(double)(10.0*(8.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7])));
      if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9]))
            pDev->fDevStage=(double)(10.0*(9.0+0.2*(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8])));
      if (pDev->fStageSUCROS>VR[9])
            pDev->fDevStage=(double)92.0; 

     }// if (pDev->fStageSUCROS>=0) else
	
//-------------------------------------------------------------------------------------
	  //output
      pDev->fDevR = (double)DVR;//rate
	 
 	return 1;
    }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Astronomy_GECROS (from the SUCROS model)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates astronomic daylength,           *
*           diurnal radiation characteristics such as the daily        *
*           integral of sine of solar elevation and solar constant.    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DOY     R4  Daynumber (Jan 1st = 1)                       -      I  *
*  LAT     R4  Latitude of the site                        degree   I  *
*  INSP    R4  Inclination of sun angle for computing DDLP degree   I  *
*  SC      R4  Solar constant                             J m-2 s-1 O  *
*  SINLD   R4  Seasonal offset of sine of solar height       -      O  *
*  COSLD   R4  Amplitude of sine of solar height             -      O  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)       h      O  *
*  DDLP    R4  Photoperiodic daylength                       h      O  *
*  DSINBE  R4  Daily total of effective solar height       s d-1    O  *
*                                                                      *
*  FATAL ERROR CHECKS (execution terminated, message)                  *
*  condition: LAT > 67, LAT < -67                                      *
*                                                                      *
*  FILE usage : none                                                   *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int Astronomy_GECROS(gecros *self, int iJulianDay, double dLatitude)//, double INSP)
      {
      double DEC,AOB;
      int    nHiWord;
      //double INSP;
	  double RAD;
      self->INSP = (double)-2;//dInclSunAngle;//Inclination of sun angle
      RAD  = (double)(PI/180);
      //Check on input range of parameters
      if (dLatitude>67.0)     nHiWord=0; //Latitude too high
      if (dLatitude<-67.0)    nHiWord=0; //Latitude too low

      //Declination of the sun as function of iJulianDaynumber (iJulianDay)
      DEC = -asin( sin(23.45*RAD)*cos(2.0*PI*(iJulianDay+10.0)/365.0));
      
      //SINLD, COSLD and AOB are intermediate variables
      self->SINLD = sin(RAD*dLatitude)*sin(DEC);
      self->COSLD = cos(RAD*dLatitude)*cos(DEC);
      AOB   = self->SINLD/self->COSLD;
      
      //iJulianDaylength (DAYL) 
      self->DAYL   = 12.0*(1.0+2.0*asin(AOB)/PI);
      self->DDLP   = 12.0*(1.+2.*asin((-sin(self->INSP*RAD)+self->SINLD)/self->COSLD)/PI);
      //SINB = SINLD + COSLD*cos(2.0*PI(fSolarTime-12.0)/24.0), fSolarTime [h]
      self->DSINB  = 3600.0*(self->DAYL*self->SINLD+24.0*self->COSLD*sqrt(1.0-AOB*AOB)/PI);//daily integral over SINB
      self->DSINBE = 3600.0*(self->DAYL*(self->SINLD+0.4*(self->SINLD*self->SINLD+self->COSLD*self->COSLD*0.5))
                  +12.0*self->COSLD*(2.0+3.0*0.4*self->SINLD)*sqrt(1.0-AOB*AOB)/PI);//daily integral 
      //for the calculation of actual global radiation from daily global radiation (input)

      //Solar constant (SC) and daily extraterrestrial radiation (DS0) 
      self->SC  = 1367.0*(1.0+0.033*cos(2.0*PI*((double)iJulianDay-10.0)/365.0));
      self->DS0 = self->SC*self->DSINB;
    
      return 1;
      }
      

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      DailyThermalDayUnit_GECROS 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates the daily amount of thermal day *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DS      R4  Development stage                            -       I  *
*  TMAX    R4  Daily maximum temperature                    oC      I  *
*  TMIN    R4  Daily minimum temperature                    oC      I  *
*  DIF     R4  Daytime plant-air temperature differential   oC      I  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)      h       I  *
*  TBD     R4  Base temperature for phenology               oC      I  *
*  TOD     R4  Optimum temperature for phenology            oC      I  *
*  TCD     R4  Ceiling temperature for phenology            oC      I  *
*  TSEN    R4  Curvature for temperature response           -       I  *
*  TDU     R4  Daily thermal-day unit                       -       O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double DailyThermalDayUnit_GECROS(double DS,double TMAX,double TMIN,double DIF,double DAYL,
//								  double TBD,double TOD,double TCD,double TSEN)
double DailyThermalDayUnit_GECROS(gecros *self, double DS,double TEMP,double DIF,double DAYL,
								  double TBD,double TOD,double TCD,double TSEN)
    {
	  expertn_modul_base *xpn = &(self->parent);	
	  //PPLANT pPl = xpn->pPl;
      //PGECROSSOIL     pGS    = pGecrosPlant->pGecrosSoil;
	  //PPLANT          pPl    = GetPlantPoi();

	  //int i;
      double TMEAN,TT,SUNRIS,SUNSET,TD,TU,TDU;
	  double dayTime;
	  //double TBD0,TOD0,TCD0;
      
      //%---timing for sunrise and sunset
      SUNRIS = (double)12.0 - (double)0.5*DAYL;
      SUNSET = (double)12.0 + (double)0.5*DAYL;
//Begin of Hong        	  
	  dayTime= xpn->pTi->pSimTime->fTimeDay*(double)24.;

      //%---mean daily temperature
      DIF    = max((double)0.0,DIF);//DIF soil-air temperature difference
      //TMEAN  = (TMAX + TMIN)/(double)2.0;	  
      TT     = (double)0.0;

      //%---diurnal course of temperature
//      for (i=1;i<=24;i++)
//         {
//          if ((i >= SUNRIS)&&(i <= SUNSET))
     //       if ((dayTime >= SUNRIS)&&(dayTime <= SUNSET)) 
     //           TD = TMEAN+DIF+(double)0.5*fabs(TMAX-TMIN)*cos((double)0.2618*(double)(i-14));
//          else
 //               TD = TMEAN    +(double)0.5*fabs(TMAX-TMIN)*cos((double)0.2618*(double)(i-14));
        TD=TEMP;

   //%---assuming development rate at supra-optimum temperatures during
   //    the reproductive phase equals that at the optimum temperature     
      if (DS > (double)1.0)
          TD = min(TD,TOD);
      else
          TD = TD;
   
   //%---vernalisation for wheat and barley
   /*   double TBD0, TCD0, TOD0; //Test of Hong
	  TBD0=TBD;
	  TCD0=TCD;
	  TOD0=TOD;

	  //if ((!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")))
	  //if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))
	  if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
	  { 
      if((DS>=(double)0.0)&&(DS < (double)0.4))
      //if(DS < (double)0.4)
	  {
      TBD=(double)-1.3;
	  TCD=(double)15.7;	  
	  TOD=(double) 4.9;
	  }//parameter of Streck et al. (2003) AFM115_139-150
	  else
	  {
	   TBD=TBD0;
	   TCD=TCD0;
	   TOD=TOD0;
	  }
	  }// Vernalization*/
	  

	  //%---instantaneous thermal unit based on bell-shaped temperature response
      if ((TD < TBD)||(TD > TCD))
         TU = (double)0.0;
      else
         TU = pow(((TCD-TD)/(TCD-TOD))*pow((TD-TBD)/(TOD-TBD),(TOD-TBD)/(TCD-TOD)),TSEN);
            
//      TT = TT + TU/(double)24.0;

//        }//end for
//End of Hong
      //%---daily thermal unit
      TDU = TU;


      return TDU;
      }



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Phenology_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates the daily amount of thermal day *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DS      R4  Development stage                            -       I  *
*  TMAX    R4  Daily maximum temperature                    oC      I  *
*  TMIN    R4  Daily minimum temperature                    oC      I  *
*  DIF     R4  Daytime plant-air temperature differential   oC      I  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)      h       I  *
*  TBD     R4  Base temperature for phenology               oC      I  *
*  TOD     R4  Optimum temperature for phenology            oC      I  *
*  TCD     R4  Ceiling temperature for phenology            oC      I  *
*  TSEN    R4  Curvature for temperature response           -       I  *
*  TDU     R4  Daily thermal-day unit                       -       O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double Phenology_GECROS(gecros *self, double DS,double SLP,double DDLP,double SPSP,double EPSP,double PSEN, double MTDV,double MTDR,double TDU)
double Phenology_GECROS(gecros *self,double DS,double SLP,double DDLP,double SPSP,double EPSP, double PSEN,double MTDV,double MTDR,double TDU,double OptVernDays) //added by Hong on 20170704
{
 expertn_modul_base *xpn = &(self->parent);
 PPLANT pPl = xpn->pPl;
 double MOP,DLP,EFP,DVR;
 double CVU, EFV;

 double BasVernDays; //for Vernalization following SPASS, activated by Hong on 20170704

      //%---determining if it is for short-day or long-day crop
      if (SLP < (double)0.0)
         {
           MOP =(double)18.0; //minimum optimum photoperiod for long-day crop
           DLP = min(MOP,DDLP);
         }
      else
      {
            MOP =(double)11.0; //minimum optimum photoperiod for short-day crop
            DLP = max(MOP,DDLP);
      }

      //%---effect of photoperiod on development rate
       if ((DS < SPSP)||(DS > EPSP)) 
          EFP = (double)1.0;
       else
          EFP = max((double)0.0, (double)1.0-PSEN*(DLP-MOP));

      //%---development rate of vegetative and reproductive phases
       if ((DS>=(double)0.0)&&(DS<(double)1.0))
          DVR = (double)1./MTDV*TDU*EFP;
       else
          DVR = (double)1./MTDR*TDU;
		  
      ///*
	  //%---vernalisation--- wheat and barley
	   EFV = (double) 1;

	   if (self->VERN > (double)0)
	  {
	   CVU = (double) xpn->pPl->pDevelop->fCumVernUnit;
       //EP20100708 nach Streck et al. (2002)
	   //deactivated by Hong
	   //EFV = abspowerDBL(CVU,(double)5)/(abspowerDBL((double)22.5,(double)5)+abspowerDBL(CVU,(double)5));
	   //End of Hong
	   //STRECK, N.A.; WEISS, A.; BAENZINGER, P.S. A generalized vernalization response function for winter wheat. 
	   //Agronomy Journal, v.95, p.155-159, 2003.

       //SG 20110801: nach SPASS (Wang), activated by Hong on 20170704:
	   BasVernDays = 0.174*pPl->pGenotype->fOptVernDays; 
       if (pPl->pGenotype->fOptVernDays==(double)0.0)
			EFV = 1.0;
       else
			EFV = (CVU-BasVernDays)/(pPl->pGenotype->fOptVernDays-BasVernDays);
	   
       EFV =min(1.0,max(0.0,EFV));
      }
 
	  if(self->VERN == (double)0) EFV = (double) pPl->pDevelop->fVernEff;

	  if(self->VERN < (double)0) EFV = (double) 1;

      
	  //if (DS<(double)0.4) DVR = (double)1./MTDV*TDU*EFP*EFV;
	  if ((DS>=(double)0.0)&&(DS<(double)0.4)) 
		  DVR = (double)1./MTDV*TDU*EFP*EFV;
	  if ((DS>=(double)0.4)&&(DS<(double)1.0)) 
		  DVR = (double)1./MTDV*TDU*EFP;
	  if (DS>=(double)1.0)                     
		  DVR = (double)1./MTDR*TDU;		   		
  						   
      return DVR;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Photosynthesis_GECROS (Gross Photosynthesis and Transpiration) 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int Photosynthesis_GECROS(gecros *self)
  {
      expertn_modul_base *xpn = &(self->parent);
	  PTIME pTi = xpn->pTi;
      PCLIMATE pCl = xpn->pCl;
	  PPLANT pPl = xpn->pPl;
	  PSPROFILE pSo = xpn->pSo;	
	  PWATER pWa = xpn->pWa;

	  PSWATER     pSW  = pSo->pSWater;
      PWLAYER     pWL  = pWa->pWLayer;
      PPLTCARBON  pCbn = pPl->pPltCarbon;
      
      PGECROSCARBON   pGPltC = self->pGecrosPlant->pGecrosCarbon;
	  PGECROSSOIL     pGS    = self->pGecrosPlant->pGecrosSoil;

      RESPONSE VLS[]={{0.}, {0.}, {2.5}, {0.}};
      char pType[5];      //SG/17/05/99: wg. Unterscheidung der Pflanzenarten

	  double PPCAN,APCANS,APCANN,APCAN,PTCAN,ATCAN,PESOIL,AESOIL,DIFS,DIFSU,DIFSH,DAPAR;
      double d1,d2;
      double DDTR,TMAX,TMIN,DVP,WNM,C3C4,LAI,LAIC,TLAI,HT,RD,NLV,TNLV;
      double FVPD,LS;
      double WCUL,DWSUP,WUL;
	  double TMPA;
	  //*
      int iLayer;
	  double f1;
	  double WLL,PWP;
	  //*/
	  double DeltaZ = pSo->fDeltaZ;
	
	  double HOUR = (double)xpn->pTi->pSimTime->fTimeDay*24; //Hong

      double CO2A; //SG20190212


//Check if C3 or C4 crop
      strcpy(pType,"C3\0");

      if ((!strcmp(pPl->pGenotype->acCropCode,"MI"))
            ||(!strcmp(pPl->pGenotype->acCropCode,"MZ"))
            ||(!strcmp(pPl->pGenotype->acCropCode,"SG")))
            strcpy(pType,"C4\0");

//input parameters
      //Begin of Hong
	 // DDTR = (double) pCl->pWeather->fSolRad*1E6; //[J/(m2*d)]
	  DDTR = xpn->pCl->pWeather->fGlobalStrahlung; //Test of Hong
	  TMAX = (double) self->weather.fTempMax; //[°C]
	  TMIN = (double) self->weather.fTempMin;  //[°C]
	  TMPA = (double) pCl->pWeather->fTempAir; //[°C]
       //End of Hong
      d1   = (double)0.611 * exp((double)17.269*TMAX/((double)237.3+TMAX));
      d2   = (double)0.611 * exp((double)17.269*TMIN/((double)237.3+TMIN));
      DVP  = (double) pCl->pWeather->fHumidity/((double)50./d1+(double)50./d2); 
	//  d1= (double)0.611 * exp((double)17.269*TMPA/((double)237.3+TMPA));
	//  DVP  = (double) pCl->pWeather->fHumidity/((double)50./d1+(double)50./d1);
	  //daily mean vapour pressure [kPa], FAO Report 1990 by Martin Smith, Annex V, eq. (15)
      WNM  = max((double)0.1,(double)pCl->pWeather->fWindSpeed);  //[m s-1]      

      if ((0==strcmp(pType,"C3\0"))||(0==strcmp(pType,"c3\0")))   
            C3C4=(double)1; //C3 plants
      else
            C3C4=(double)-1;//C4 plants

      LAI  = (double) self->pGecrosPlant->pGecrosCanopy->fLAIGreen;//[m2 m-2]
      LAIC = (double) self->pGecrosPlant->pGecrosCanopy->fLAICdeterm;//[m2 m-2]
      TLAI = (double) self->pGecrosPlant->pGecrosCanopy->fLAITotal;//[m2 m-2]
      HT   = (double) pPl->pCanopy->fPlantHeight; //[m]
      //LW   = (double) pPl->pCanopy->fLeafWidth; //[m]
	  RD   = (double)pPl->pRoot->fDepth; //[cm] 
	  RD   = max(RD,2.); //Test of Hong
      // RD   = max(RD,max(2.,self->SD1)); //Test of Hong
//-----------------------------------------------------------------------------------------------
      ///*    
	  WUL  = (double)0;
      WLL  = (double)0;
      PWP  = (double)0;

     //SG 20111103: first layer pSW=pSo->pSWater->pNext (see xh2o_def.h)
	 //for (pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater,iLayer=1;
	 for (pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater->pNext,iLayer=1;
		 (pWL->pNext!=NULL) && (pSW->pNext!=NULL);pWL=pWL->pNext,pSW=pSW->pNext,iLayer++)    
     {
     if (iLayer*DeltaZ <= (double)(RD*10.))
	 {
	    WUL += (double)(pWL->fContAct*DeltaZ);
	    PWP += (double)(pSW->fContPWP*DeltaZ);
	 }
	 else if (((double)RD*10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= (double)RD*10.+DeltaZ))
     {
        f1 = (iLayer*DeltaZ-(double)(RD*10.))/DeltaZ;
	    WUL+= (double)(pWL->fContAct*((double)1-f1)*DeltaZ);
	    PWP+= (double)(pSW->fContPWP*((double)1-f1)*DeltaZ);
		WLL+= (double)(pWL->fContAct*f1*DeltaZ);
     }
	 else
		WLL+= (double)pWL->fContAct*DeltaZ;
     }//for
	 //*/

	 //SG 20111103: 
	 /* 
	    WUL ist hier der Gesamtwassergehalt in der durchwurzelten Bodenschicht.
	    In der Gecros-Dokumentation allerdings nur der Anteil, der das Wasser 
	    "which is unavailable for removal by evapotranspiration" übersteigt.
	    Deshalb muss meiner Meinung nach PWP von WUL abgezogen werden.
	 */
	  DWSUP  = max(0.1,(WUL-PWP)/self->TCP+0.1); //SG
	  //DWSUP  = max(0.1,WUL/self->TCP+0.1); //EP
	  if (PWP>WUL) DWSUP=0.1;

	  if (pGS->fWaterContUpperLayer==(double)0) pGS->fWaterContUpperLayer = (double)WUL;
      if (pGS->fWaterContLowerLayer==(double)0) pGS->fWaterContLowerLayer = (double)WLL;
      //if (pGS->fETDailyWaterSupply ==(double)0) pGS->fETDailyWaterSupply  = (double)DWSUP;

	  //SG 20111103: 
	  // pGS->fWaterContUpperLayer und pGS->fWaterContLowerLayer in "ActualNitrogenUptake_GECROS" 
	  // nötig und müssen deshalb immer (nicht nur beim ersten Zeitschritt) berechnet werden	
	  pGS->fWaterContUpperLayer = (double)WUL;
      pGS->fWaterContLowerLayer = (double)WLL;
      pGS->fETDailyWaterSupply  = (double)DWSUP;
      //*/
//-----------------------------------------------------------------------------------------------

      //SD1 Parameter (soil depth of upper layer) //[cm]
      //RSS Parameter (soil resistance equiv. to stomata resistance)
      //BLD Parameter (leaf angle from horizontal)
      TNLV = (double)self->pGecrosPlant->pGecrosNitrogen->fNLeafTotCont;
      NLV  = (double)pPl->pPltNitrogen->fLeafCont;
      //SLNMIN Parameter (minimum leaf N content for Photosynthesis)

      //CO2A Parameter (ambient CO2 concentration)

      //SG20190212 - variable atmospheric CO2-concentrations from daily weather input
      CO2A = (pCl->pWeather->fAtmCO2ppm > 199.99?pCl->pWeather->fAtmCO2ppm:self->CO2A);

        //  LS   = INSW(self->LODGE,(double)0.,(double)AFGENERATOR(pPl->pDevelop->fStageSUCROS,VLS));//LS = (double)0.;
    // 20190116 Troost, for now (VLS pointer wrongly increased in AFGENERATOR), since no Lodging input possible:
   	LS = (double)0.;

      //EAJMAX Parameter (Farquahar Photosynthesís model)
      //XVN Parameter (Farquhar Photosynthesis model)
      //XJN Parameter (Farquhar Photosynthesis model)
      //THETA Parameter (Farquhar Photosynthesis model)
      //WUL  = (double)pGS->fWaterContUpperLayer;//[mm]

//Changed by Hong after XN3 on 20170623
	  //WCUL = (WUL+self->WCMIN*10.*RD)/10./NOTNUL(RD);
//SG 20111103: 
	/*Wie oben: WUL ist hier der Gesamtwassergehalt in der durchwurzelten Bodenschicht.
	 In der Gecros-Dokumentation allerdings nur der Anteil, der das Wasser "which is 
	 unavailable for removal by evapotranspiration" übersteigt.
	 Deshalb muss meiner Meinung nach WCMIN*10*RD (=PWP) nicht noch zu WUL dazu addiert werden.*/
//	  WCUL = (WUL+WCMIN*10.*RD)/10./NOTNUL(RD);
	  WCUL = WUL/10./NOTNUL(RD); //[m3 m-3]
      //WCUL = (double)pWa->pWLayer->pNext->fContAct;//Water content of upper layer [mm mm-1] 
      //define for upper layer -> to do !!! ep20071016
//End of Hong
	  


      FVPD = INSW(C3C4,(double)0.195127,(double)0.116214);

//gross photosynthesis rate

 /*     pCbn->fGrossPhotosynR = (double)DailyCanopyGrossPhotosynthesis_GECROS(self,self->SC,self->SINLD,self->COSLD,self->DAYL,self->DSINBE,
          DDTR,TMPA,HOUR,DVP,WNM,C3C4,LAIC,TLAI,HT,self->LWIDTH,RD,self->SD1,self->RSS,self->BLD,NLV,TNLV,self->SLNMIN,
          DWSUP,self->CO2A,LS,self->EAJMAX,self->XVN,self->XJN,self->THETA,WCUL,FVPD, &PPCAN,&APCANS,&APCANN,&APCAN,
                 &PTCAN,&ATCAN,&PESOIL,&AESOIL,&DIFS,&DIFSU,&DIFSH,&DAPAR);*/
  
   //SG20190212 (self->CO2A durch CO2A ersetzt)
   pCbn->fGrossPhotosynR = (double)DailyCanopyGrossPhotosynthesis_GECROS(self,self->SC,self->SINLD,self->COSLD,self->DAYL,self->DSINBE,
          DDTR,TMPA,HOUR,DVP,WNM,C3C4,LAIC,TLAI,HT,self->LWIDTH,RD,self->SD1,self->RSS,self->BLD,NLV,TNLV,self->SLNMIN,
          DWSUP,CO2A,LS,self->EAJMAX,self->XVN,self->XJN,self->THETA,WCUL,FVPD, &PPCAN,&APCANS,&APCANN,&APCAN,
                 &PTCAN,&ATCAN,&PESOIL,&AESOIL,&DIFS,&DIFSU,&DIFSH,&DAPAR);

/*
//%** output ***
      pGPltC->fGrossStdgCropPhotosynR  = (double)APCANS;
      pGPltC->fGrossStdgCropPhotosynRN = (double)APCANN;
	  pPl->pPltCarbon->fGrossPhotosynR = (double)APCAN;

	  pGS->fDiffSoilAirTemp            = (double)DIFS;

//%** output expertn ***
	  pWa->pEvap->fPotR                = (double)PESOIL;// * pTi->pTimeStep->fAct;
	  // EP wird in water1h.c/hydrus berechnet und sollte hier nicht überschrieben werden.
	  pWa->pEvap->fActR                = (double)AESOIL;// * pTi->pTimeStep->fAct;
	  // SG 20111107: pPl->pPltWater->fPotTranspDay und pPl->pPltWater->fActTranspDay  
	  // dürfen nicht ausserhalb von balance.c gesetzt werden (werden dort in jedem 
	  // Zeitschritt aus fPotTranspdt bzw. fActTranspdt aufaddiert!)
	  //pPl->pPltWater->fPotTranspDay    = (double)PTCAN;
	  //pPl->pPltWater->fActTranspDay    = (double)ATCAN;
	  //pPl->pRoot->fUptakeR             = (double)ATCAN;
	  //pWa->fPotETDay                   = (double)(PESOIL+PTCAN);
	  pWa->fPotETdt                    = (double)(PESOIL+PTCAN)* pTi->pTimeStep->fAct;
      pPl->pPltWater->fPotTranspdt     = (double)PTCAN* pTi->pTimeStep->fAct;
	  pPl->pPltWater->fPotTranspR 	   = PTCAN;
      pPl->pPltWater->fActTranspdt     = (double)ATCAN* pTi->pTimeStep->fAct;

*/	 
//Begin of hong
//%** output ***
      self->fPotTraDay                       = (double)PTCAN;//Hong added 24/self-DAYL
      self->fActTraDay                       = (double)ATCAN;
      //pWa->pEvap->fPotDay              = (double)PESOIL;// EP 20130731 wird in "balance.c" berechnet
      //pWa->pEvap->fActDay              = (double)AESOIL;// EP 20130731 wird in "balance.c" berechnet
      pGPltC->fGrossStdgCropPhotosynR  = (double)APCANS;
      pGPltC->fGrossStdgCropPhotosynRN = (double)APCANN;
	  pPl->pPltCarbon->fGrossPhotosynR = (double)APCAN;

	  pGS->fDiffSoilAirTemp            = (double)DIFS;

	  //%** output expertn ***
	 pWa->pEvap->fPotR                = (double)PESOIL;
	 pWa->pEvap->fActR                = (double)AESOIL; // EP 20130731 wird in "water1h.c"(HYDRUS) berechnet
	  // SG 20111107: pPl->pPltWater->fPotTranspDay und pPl->pPltWater->fActTranspDay 
	  // dürfen nicht ausserhalb von balance.c gesetzt werden (werden dort in jedem 
	  // Zeitschritt aus fPotTranspdt bzw. fActTranspdt aufaddiert!)
	  //pPl->pPltWater->fPotTranspDay    = (double)PTCAN;
	  //pPl->pPltWater->fActTranspDay    = (double)ATCAN;
	  //pPl->pRoot->fUptakeR             = (double)ATCAN;
	  pWa->fPotETDay                   = (double)(PESOIL+PTCAN);// EP 20130731 wird in "balance.c" berechnet
	  pWa->fPotETR					   = (double) pWa->fPotETDay; //FH 20180130
	  pWa->fActETR					   = (double) (AESOIL + ATCAN);//FH 20180130
	  //pWa->fActETDay				   = fActTraDay + pWa->pEvap->fActDay; //SG20160609
      
	  //pWa->fPotETdt                    = pWa->fPotETDay * pTi->pTimeStep->fAct;
          //pPl->pPltWater->fPotTranspdt     = pPl->pPltWater->fPotTranspDay * pTi->pTimeStep->fAct;
	  pWa->fPotETdt                    = pWa->fPotETDay * pTi->pTimeStep->fAct;//EP 20130731
          pPl->pPltWater->fPotTranspdt     = (double)PTCAN * pTi->pTimeStep->fAct; //EP 20130731
          pPl->pPltWater->fActTranspdt     = (double)ATCAN * pTi->pTimeStep->fAct;//EP 20130731
	 pPl->pPltWater->fPotTranspR = (double)PTCAN;
//Hong	 pPl->pPltWater->fActTranspR = (double)ATCAN;

//end of Hong

      return RET_SUCCESS;
  }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      BiomassGrowth_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   BiomassGrowth_GECROS(gecros *self)
      {
      expertn_modul_base *xpn = &(self->parent);
	  
	  PMANAGEMENT pMa = xpn->pMa;
	  PPLANT      pPl = xpn->pPl;
    //  PWLAYER      pWL   = xpn->pWa->pWLayer;

      PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PGENOTYPE    pGen  = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
     // PCANOPY      pCan  = pPl->pCanopy;

      PGECROSBIOMASS    pGPltB = self->pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON     pGPltC = self->pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY     pGCan  = self->pGecrosPlant->pGecrosCanopy;
      //PGECROSPARAMETER  pGPar  = self->pGecrosPlant->pGecrosParameter;
	  PGECROSSOIL       pGS    = self->pGecrosPlant->pGecrosSoil;
      //double            fTemp;

	  //SG 20180410: für Crop?-Abfrage
	  char spec[30], var[30];
	  strcpy(spec,pPl->pGenotype->acCropCode);
	  strcpy(var,pPl->pGenotype->acVarietyName);

      double CFO,YGO,LNCMIN;
      /*
      double FPRO,FCAR;
      double CLVI,CRTI,NPL,NLVI,NRTI;
      double LAII,SLNBI,RDI,HTI;
      */
      double DS,TDU,CTDU;
      double RCLV,ASSA,FCSH,FCLV,LCLV;//,YGV;
      double RCSST,FCSST;
      double RCSRT,FCRVR,LCRT;
      double RCSO,FCSO,CREMS,CREMR;
      double CLV,CLVD,CLVDS,LVDS,CSST,CSO,CSRT,CRTD,CSH,CRT,CTOT;
      double HI,WLV,WST,WSO,WRT,WSH,WSHH,WLVD,WRTD,WTOT;//,CFV;
      double RWST,RWSO,RWLV,RWRT;
      double CRVS,CRVR,RCRVS,RCRVR;

      double FNSH,NUPT,LNC,RNC,NLV,NRT,DELT;
      //double CB,CX,TM,STEMNC;
      double LNLV,LNRT;
      double RNRT,RNST,RNLV,RTNLV,RNSO;
      double NST,NSO,TNLV,NLVD,NRTD,NSH;
      double NSHH,NTOT;//,CFV;
      double HNC,PNC,ONC;
      double PSO;
      double KW,KN,NBK,TLAI;
      //double DXT,SLN,SLNT,SLNB,SLNNT,LAI;
      double SLN,SLNT,SLNNT,SLNBC,LAI;
      double NCR,NDEMA,DERI,SHSA;//,YGV,NUPTX;

      double DCSS,DCSR,ESD;//locals
      double LAIC,LAIN,FLWCS,FLWCT;//input
      double CSRTN;//input
      double DCDS;//input & output
      double FCRVS;//locals
      double GAP,CREMSI,CREMRI,RNRES,NRES;//locals
      double NREOE,NREOF;//input & output
      double RNREOE,RNREOF;//locals
      double TSN,TSW;//output
      double DVR,DCDSC,DCDSR,DCDTR;
      double RDCDSR,FDS,RDCDTR,IFSH,FDH,DCDT,DCDTC,DCST;//,YGV;
      
      double LWLVM,LWLV;
      double KCRN,LWRT;

      double RMUN,RMUA,RMUS,RMLD,RMUL,RRMUL,RMRE,RM,RX,RG,RESTOT;
      //double CCFIX;//CFO;
      double NUPTN,NUPTA;//,YGV;
      double NFIX;

      //double RLAI;//,CFV;
      double RSLNB,SLNB;

      //double NDEM,NDEMP,RNDEMP,NSUP,NSUPP,RNSUPP,NFIXE,NFIXD,NFIXT,NFIXR,RNFIXR;//TCP;
	  double NDEM,NDEMP,RNDEMP,NFIXR;
	  //double NDEM;
	  double NSUPP,RNSUPP;

      double APCAN=(double)0;
      double DIFS=(double)0;
      /*
      int       iJulianDay;
      double    dLatitude;
      
	  double      DVS[]={0.0, 0.4, 0.55, 0.656, 0.91, 1.00, 1.15, 1.50, 1.95, 2.0};
      double       VR[10];
      int i;
      */
      double RMN;//,YGV;
      //double NDEMD,NDEMAD,HNCCR,SHSAN;
      //double FWS,NAUL,NNUL,NSUPA,NSUPN;
      //double RA,RD,RN,WCFC,WCMIN,WUL;

      //double HT,RHT;
	  double DCDTP,RDCDTP;
      //double KR,RD,RRD;

      /*
	  int iLayer;
      double DeltaZ=pSo->fDeltaZ;
	  double f1;
	  double DWSUP,WCMIN,WUL,WLL;
      */
	  //double WCMIN;
      double TMIN,TMAX,DAVTMP,NAVTMP,TAVSS;
	  double TBD,TOD,TCD,TSEN;  

	  double BLD = self->BLD;//Leaf angle from horizontal (degree)
	  double CB = self->CB;//sensitivity parameter, factor for initial N concent. of seed-fill (-) 
	  double CFV = (double)self->CFV;//carbon frac. in vegetative organs (g C g-1) crop table2,p46  
	  double CX = self->CX;//sensitivity parameter, factor for final N concent. of seed-fill (-)
	  double CDMHT = self->CDMHT;//stem dry weight per unit of plant height (g m-2 m-1) crop table3,p47
	  double DETER = self->DETER;//DETER  = 1. for determinate crops;  = -1. for indeterminate crops.
	  double ESDI = self->ESDI;//ESD for indeterminate crops (-) deflt table4,p49
	  double HTMX = self->HTMX;//Maximal crop height (m)
	  double NUPTX = self->NUPTX;//maximum crop nitrogen uptake (g N m-2 d-1) deflt table4,p49
	                             //-> pGPltN->fNUptMax;
	  double PMEH = self->PMEH;//sigm.crv.inflec.frac. in ent. plthght.grwth.per. (-) deflt table4,p49
	  double PMES = self->PMES;//sigm.crv.inflec.frac. in entire seed grwth.per. (-) deflt table4,p49
	  double PNPRE = self->PNPRE;//sensitivity parameter
	  double RDMX = self->RDMX;//maximal root depth (cm) crop table3,p47 -> pGPar->fRootDepthMax
	  double RNCMIN = self->RNCMIN;//min. N conc. in roots (g N g-1) crop table3,p47 
	                               //-> pPltN->fRootMinConc;
	  double SEEDW = self->SEEDW;//Seed weight (g seed-1)   
	  double SEEDNC = self->SEEDNC;//Standard seed (storage organ) N conc. (g N g-1)
	  double SLA0 = self->SLA0;//specific leaf area constant (m2 leaf g-1) crop table3,p47
	  double SLNMIN = self->SLNMIN;//min. specific leaf N content (g N m-2) crop table3,p47
	  double STEMNC = self->STEMNC;//nitrogen conc. in stems (g N g-1) crop table3,p47
	                               //-> pPltN->fStemActConc;
	  double TM = self->TM;//sensitivity parameter dev.stage when transition from CB to CX is fastest (-)   
	  double WRB = self->WRB;//critical root weight density (g m-2 cm-1 depth) const table1,p45
	  double YGV = self->YGV;//growth efficiency of veg. organs (g C g-1 C) crop table2,p46
                             //pGPar->fGrwEffVegOrg;
    //SG20180410
    double SINKBEET = self->SINKBEET;//sink strength of beet in sugarbeet model by J. Rabe
      //double DELT = (double)1;
	  DELT = (double)xpn->pTi->pTimeStep->fAct;
	  
/*	  
      if ((pMa->pSowInfo != NULL)&&(pTi->pSimTime->fTimeAct == (double)pMa->pSowInfo->iDay)) 
        Init_GECROS(exp_p);

//-------------------------------------------------------------------------------------

 
    //%-- Photoperiod, solar constant and daily extraterrestrial radiation
      iJulianDay  = (int)pTi->pSimTime->iJulianDay;
      dLatitude   = (double)pLo->pFarm->fLatitude;
      
      Astronomy_GECROS(iJulianDay, dLatitude);

//-------------------------------------------------------------------------------------

      DS   = (double)pDev->fStageSUCROS;
      TMAX = (double)pCl->pWeather->fTempMax;    //[°C]
      TMIN = (double)pCl->pWeather->fTempMin;    //[°C]
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
      TBD  = (double)fparTBD;//base temperature for phenology     [°C]  crop  table 3, p47 
      TOD  = (double)fparTOD;//optimum temperature for phenology  [°C]  crop  table 3, p47
      TCD  = (double)fparTCD;//ceiling temperature for phenology  [°C]  crop  table 3, p47
      TSEN = (double)fparTSEN;//curvature for temperature response [-]  deflt table 4, p49

	  //%-- Developmental stage (DS) & cumulative thermal units (CTDU)      
      TDU =(double)0;
      TDU=DailyThermalDayUnit_GECROS(DS,TMAX,TMIN,DIFS,DAYL,TBD,TOD,TCD,TSEN);

	  DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
      pDev->fDevR = (double)DVR;//rate

      
    //%--  Phenological development
      if (pDev->fStageSUCROS<0)
      {
      //initially zero
       pDev->fStageSUCROS= (double)0.0;
       pDev->iDayAftEmerg = 0;
       pDev->fCumTDU = (double)0.0;

       CTDU =(double)0;
       DS =(double)0;
      }
      else
      {
       DS = (double)pDev->fStageSUCROS;   
       //DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
       
	  //output
       //pDev->fDevR = (double)DVR;
       //pDev->fStageSUCROS += pDev->fDevR;
       //pDev->fCumTDU += (double)TDU;

	  //output from fStageSUCROS to pDev->fStageWang
      for (i=0;i<10;i++) VR[i]=(double)DVS[i];
      if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1]))
            pDev->fDevStage=(double)(10.0*(1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0])));
      if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2]))
            pDev->fDevStage=(double)(10.0*(2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1])));
      if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3]))
            pDev->fDevStage=(double)(10.0*(3.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2])));
      if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4]))
            pDev->fDevStage=(double)(10.0*(4.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3])));
      if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5]))
            pDev->fDevStage=(double)(10.0*(5.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4])));
      if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6]))
            pDev->fDevStage=(double)(10.0*(6.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5])));
      if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7]))
            pDev->fDevStage=(double)(10.0*(7.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6])));
      if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8]))
            pDev->fDevStage=(double)(10.0*(8.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7])));
      if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9]))
            pDev->fDevStage=(double)(10.0*(9.0+0.2*(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8])));
      if (pDev->fStageSUCROS>VR[9])
            pDev->fDevStage=(double)92.0; 

     }// if (pDev->fStageSUCROS>=0) else
*/
//-------------------------------------------------------------------------------------
      //%-- Canopy photosynthesis, transpiration and soil evaporation
      //Photosynthesis_GECROS(exp_p);

//-------------------------------------------------------------------------------------
//*/

	  DS   = (double)pDev->fStageSUCROS;
      DVR  = (double)pDev->fDevR;//rate
	  TDU  = (double)pDev->fDTT;
	  CTDU = (double)pDev->fCumTDU;
	//Begin of Hong
	  TMAX = (double) self->weather.fTempMax;     //[°C]
      TMIN = (double) self->weather.fTempMin;     //[°C]	  
	//End of Hong
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
	  TBD  = (double)self->TBD;//base temperature for phenology     [°C]  crop  table 3, p47 
      TOD  = (double)self->TOD;//optimum temperature for phenology  [°C]  crop  table 3, p47
      TCD  = (double)self->TCD;//ceiling temperature for phenology  [°C]  crop  table 3, p47
      TSEN = (double)self->TSEN;//curvature for temperature response [-]  deflt table 4, p49

	  //%-- Developmental stage (DS) & cumulative thermal units (CTDU)      
      //input
      CLV    = (double)pGPltC->fCLeaf;
      CLVD   = (double)pGPltC->fCLeafDead;
      CLVDS  = (double)pGPltC->fCLeafDeadSoil;
      CSST   = (double)pGPltC->fCStem;
      CSO    = (double)pGPltC->fCStorage;

      CSRT   = (double)pGPltC->fCStrctRoot;
      CRTD   = (double)pGPltC->fCRootDead;
        
      //CFV    = (double)pGPltC->fCFracVegetative;
      CFO    = (double)pGPltC->fCFracStorage;
      CRVS   = (double)pGPltC->fCStemReserve;
      CRVR   = (double)pGPltC->fCRootReserve;
      
      //%** Biomass formation
      WLV    = CLV  / CFV;
	  WST    = CSST / CFV + CRVS/0.444; //other crops
	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is storage organ
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
		  WST    = CSST / CFO + CRVS/0.444; //sugar beet

	  WSO    = CSO  / CFO;
      WSH    = WLV  + WST + WSO;
      WRT    = CSRT / CFV + CRVR/0.444;
      WTOT   = WSH  + WRT;

      WLVD   = CLVD / CFV;
      WRTD   = CRTD / CFV;
      WSHH   = WSH  + (WLVD-CLVDS/CFV);
      HI     = WSO  / WSHH;
 
      CSH    = CLV  + CSST + CRVS + CSO;
      CRT    = CSRT + CRVR;
      CTOT   = CSH  + CRT;

      //output
      pPltB->fLeafWeight      = (double)(WLV*10); //[g m-2] --> [kg ha-1]
      pPltB->fStemWeight      = (double)(WST*10); //[g m-2] --> [kg ha-1]
      pGPltB->fStorageWeight  = (double)WSO;
      pPltB->fRootWeight      = (double)(WRT*10); //[g m-2] --> [kg ha-1]
      pGPltB->fShootWeight    = (double)WSH;
      pGPltB->fShtWghtExShLvs = (double)WSHH;
      pPltB->fTotalBiomass    = (double)(WTOT*10);//[g m-2] --> [kg ha-1]
      pGPltB->fHarvestIndex   = (double)HI;

      pPltB->fGrainWeight     = (double)(WSO*10); //[g m-2] --> [kg ha-1]
	  pPltB->fFruitWeight     = pPltB->fGrainWeight; //Hong: necassary for XN5
      pPltB->fStovWeight      = pPltB->fLeafWeight+pPltB->fStemWeight;
        if(!strcmp(spec,"SB"))
             pPltB->fStovWeight      = pPltB->fLeafWeight;
      
      pGPltB->fLeafDeadWeight = (double)(WLVD); 
      pGPltB->fRootDeadWeight = (double)(WRTD);
	  pPltB->fDeadLeafWeight  = (double)(WLVD*10);//[g m-2] --> [kg ha-1]
	  pPltB->fDeadRootWeight  = (double)(WRTD*10);//[g m-2] --> [kg ha-1]

     //SG 2017...: für AgMIP - Biomasse inkl. abgestorbene Organe
     // pPltB->fTotalBiomass   += (pPltB->fDeadLeafWeight+pPltB->fDeadRootWeight);
	 // pPltB->fStovWeight     += pPltB->fDeadLeafWeight;

      pGPltC->fCShoot        = (double)CSH;
      pGPltC->fCRoot         = (double)CRT;
      pGPltC->fCPlant        = (double)CTOT;


	  //%** input
      LNCMIN = (double)pPltN->fLeafMinConc;
      NLV    = (double)pPltN->fLeafCont;
      NST    = (double)pPltN->fStemCont;
      NSO    = (double)pGPltN->fNStorageCont;
// SG20110909
	  NRT    = (double)pPltN->fRootCont/10.0; //[g/m2]<--[kg/ha]
//    NRT    = (double)pPltN->fRootCont;
 
      //%** Nitrogen concentration of biomass
      NSH    = NST + NLV + NSO;
      NSHH   = NSH +(WLVD-CLVDS/CFV)*LNCMIN;
      NTOT   = NSH + NRT;


      LNC   = NLV / WLV;
      RNC   = NRT / WRT;
      HNC   = NSH / WSH;
      ONC   = INSW(-WSO, NSO/NOTNUL(WSO), 0.);
      PNC   = NTOT/ WTOT;

	  pGPltN->fNShootCont     = (double)NSH;
      pPltN->fLeafActConc     = (double)LNC;
      pPltN->fRootActConc     = (double)RNC;
      pPltN->fTotalCont       = (double)NTOT;
      pGPltN->fShootActConc   = (double)HNC;
      pGPltN->fStorageActConc = (double)ONC;
      pPltN->fFruitActConc     = (double)ONC;
      pGPltN->fTotalActConc   = (double)PNC;

      //SG 20160704: wird für die Berechnung von pCP->fNLitterSurf benötigt (Siehe plant.c, "DevelopmentCheckAndPostHarvestManagement")
	  pPltN->fStemActConc    = NST / (WST + 1e-9); //SG 20160704: für N im Litter 
	  pPltN->fTopsActConc=pPltN->fVegActConc= (pPltN->fLeafActConc*pPltB->fLeafWeight + pPltN->fStemActConc*pPltB->fStemWeight)
												/(pPltB->fLeafWeight+pPltB->fStemWeight);
	 // printf("pPltN->fStemActConc=%f\n", pPltN->fStemActConc);//Test of Hong											

      ///*
      //%** Extinction coefficient of nitrogen and wind
      LAIC = (double)pGCan->fLAICdeterm;//input
      TNLV = (double)pGPltN->fNLeafTotCont;//Input
      TLAI = LAIC + CLVD /CFV*SLA0;
      
      KW   = kdiff(TLAI,BLD*3.141592654/180.,0.2);
      KN   = KW*(TNLV-SLNMIN*TLAI);
      NBK  = SLNMIN*(1.-exp(-KW*TLAI));
      KN   = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));
      //*/
	  ///*
      NREOE  = (double)pGPltN->fNRemobToSeedE;//Input
      NREOF  = (double)pGPltN->fNRemobToSeedF;//Input

      ESD    = INSW(DETER, ESDI, 1.);
      NRES   = NREOF + (NREOE-NREOF)*(ESD-1.)/NOTNUL(min(DS,ESD)-1.);
      TSN    = NRES/PNPRE/SEEDNC/SEEDW;
      TSW    = WSO/NOTNUL(TSN)*1000.;
      PSO    = 6.25*WSO*ONC;

      pGPltB->fTotalSeedNum    = (double)TSN;
//SG20110909: für Ausgabe in *.rfp
	  pPl->pCanopy->fGrainNum = (double)TSN / pMa->pSowInfo->fPlantDens; //[grains m-2]
      pGPltB->f1000GrainWeight = (double)TSW; 
      pGPltN->fSeedProtein     = (double)PSO;
	  //*/
	  ///*

      KCRN   = -log(0.05)/6.3424/CFV/WRB/RDMX;
      CSRTN  = 1./KCRN*log(1.+KCRN*max(0.,(NRT*CFV-CRVR*RNCMIN))/RNCMIN);

      LCRT   = max(min(CSRT-1.E-4,CSRT-min(CSRTN,CSRT)),0.)/DELT;//rate
      LWRT   = LCRT/CFV;
      LNRT   = LWRT*RNCMIN;
      //*/
      ///*
      LAIC   = (double)pGCan->fLAICdeterm;//input
      LAIN   = log(1.+KN*max(0.,NLV)/SLNMIN)/KN;
      LAI    = min(LAIN, LAIC);
      TLAI   = LAIC + CLVD /CFV*SLA0;	  
      //if(LAIN>=LAIC) TLAI = LAIC + CLVD /CFV*SLA0;
      //
      //%** output ***
	  //
      pGPltC->fCStrctRootN=(double)CSRTN;
      pGCan->fLAINdeterm = (double)LAIN;
      pGCan->fLAIGreen   = (double)LAI;
      pGCan->fLAITotal   = (double)TLAI;
      //*/ //leicht erhöhte Biomasse
      /*
      LAIC   = (double)pGCan->fLAICdeterm;//input
      LAIN   = (double)pGCan->fLAINdeterm;
      LAI    = (double)pGCan->fLAIGreen;
      TLAI   = (double)pGCan->fLAITotal;	  
      */

      //%-- Canopy photosynthesis, transpiration and soil evaporation
      //Photosynthesis_GECROS(exp_p);

      //DS   = (double)pDev->fStageSUCROS;
      //Begin of Hong
	  TMAX = (double) self->weather.fTempMax;     //[°C]
      TMIN = (double) self->weather.fTempMin;     //[°C]	 
	  //End of Hong
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
	  /*
      TBD    = (double)fparTBD;//base temperature for phenology          (°C)       crop  table 3, p47 
      TOD    = (double)fparTOD;//optimum temperature for phenology       (°C)       crop  table 3, p47
      TCD    = (double)fparTCD;//ceiling temperature for phenology       (°C)       crop  table 3, p47
      TSEN   = (double)fparTSEN;//curvature for temperature response     (-)        deflt table 4, p49
      */
	  //TDU=DailyThermalDayUnit_GECROS(DS,TMAX,TMIN,DIFS,DAYL,TBD,TOD,TCD,TSEN);
      
	  //DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
      //pDev->fDevR = (double)DVR;//rate

      ///*
      //%** Specific leaf nitrogen and its profile in the canopy
      ///*
      SLN    = NLV/LAI;
      SLNT   = NLV             *KN/(1.-exp(-KN*LAI));
      SLNBC  = NLV*exp(-KN*LAI)*KN/(1.-exp(-KN*LAI));
      SLNNT  = (NLV+0.001*NLV) *KN/(1.-exp(-KN*LAI));
      
      SLNB   = (double) pGPltN->fNLeafSpecificContBottom;
      RSLNB  = (SLNBC-SLNB)/DELT;//rate
        
      ///*
      APCAN  = (double)pPltC->fGrossPhotosynR;
      RMUL   = (double)pGPltC->fUptRespCost;//integral
      RMRE   = max(min(44./12.*0.218*(NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL), 0.);
      RMN    = min(44./12.*0.218*(1.001*NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL);
      RMN    = max(0., min(APCAN-1.E-5,RMUL) + max(RMN, 0.));
      RM     = max(0., min(APCAN-1.E-5,RMUL) + RMRE);

      pGPltC->fMaintRespRN  = (double)RMN;
      pPltC->fMaintRespR    = (double)RM;
/*
      NDEMP  = (double)pGPltN->fNDmndPrev;
      NSUPP  = (double)pGPltN->fNPlantSuppPrev; 
      NSUP   = (double)pGPltN->fNPlantSupply;
      RNSUPP = (NSUP-NSUPP)/DELT;//rate

      NFIXD  = max(0., NDEMP-NSUPP);// /DELT;
      NFIXE  = max(0., APCAN-1.E-5-RM)/NOTNUL(CCFIX)*12./44.;
      NFIX   = INSW(LEGUME, 0., min(NFIXE, NFIXD));//rate
      RX     = 44./12.*(CCFIX*NFIX);
      pGPltC->fFixRespCost  = (double)RX;
*/          
///*
	  //NitrogenFixation_GECROS(exp_p);

	  //NFIX   = (double)pGPltN->fNPlantSuppPrev; 
	  NFIX   = (double)pGPltN->fNFixation; //SG 20160722
      NSUPP  = (double)pGPltN->fNPlantSuppPrev; 
      RNSUPP = (double)pGPltN->fNPltSppPrvChangeR;//rate
	  RX     = (double)pGPltC->fFixRespCost;

	  NFIXR  = (double)pGPltN->fNFixationReserve; //SG 20170221

//*/   
      //Begin of Hong
	  TMAX = (double) self->weather.fTempMax;     //[°C]
      TMIN = (double) self->weather.fTempMin;     //[°C]	  
	  //End of Hong
	  DIFS   = (double)pGS->fDiffSoilAirTemp;       //[°C]
	  DAVTMP = 0.29*TMIN + 0.71*TMAX;
      NAVTMP = 0.71*TMIN + 0.29*TMAX;
      TAVSS  = ((DAVTMP+DIFS)+NAVTMP)/2.;	  
      LVDS   = (CLVD-CLVDS)/10.*(TAVSS-TBD)/(TOD-TBD);//rate
 
      ASSA   = (double)pPl->pPltCarbon->fGrossPhotosynR - RM - RX;
      pPltC->fNetPhotosynR = (double)ASSA;
      pGPltC->fNetStdgCropPhotosynR = pGPltC->fGrossStdgCropPhotosynR -(double)(RM+RX);// erhöht Biomasse
      
	  ///*
      pGPltN->fNLeafSpecificCont=(double)SLN;
      
	  PotentialNitrogenUptake_GECROS(self);

	  RMN   = (double)pGPltC->fMaintRespRN;
	  DERI  = (double)pGPltB->fShtToRtActivity;
      //SHSA   = 12./44.*YGV*((double)pPl->pPltCarbon->fGrossPhotosynR -RM -RX)/ CSH;
      SHSA  = (double)pGPltB->fShootActivity;
      //SHSA  = max((double)pGPltB->fShootActivity,0.0000001);
      NDEM  = (double)pGPltN->fNDemand;
      NDEMA = (double)pGPltN->fNDmndActDrv;
      pPltN->fTotalDemand      = (double)NDEM * (double)10;// [g N m-2] = 10.0 [kg N ha-1] 
      //*/
	  /* 
      //%** Crop nitrogen demand and uptake (g N m-2 d-1)
      SHSA   = 12./44.*YGV*((double)pPl->pPltCarbon->fGrossPhotosynR -RM -RX)/ CSH;
      SHSAN  = 12./44.*YGV*((double)pGPltC->fGrossStdgCropPhotosynRN -RMN-RX)/ CSH;
      pGPltB->fShootActivity   = (double)SHSA;
      
      DERI   = max(0.,(SHSAN - SHSA)/(0.001*NTOT/CTOT));
      pGPltB->fShtToRtActivity = (double)DERI;
      /*
      //%-- Nitrogen partitioning between shoots and roots
      HNCCR  = LNCI*exp(-0.4*DS);
      NDEMD  = INSW(DS-1., WSH*(HNCCR-HNC)*(1.+NRT/NSH)/DELT, 0.);
      NDEMA  = CRT * SHSA*SHSA/NOTNUL(DERI);
      NDEMAD = INSW(LNC-1.5*LNCI, max(NDEMA, NDEMD), 0.);
      NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,NDEMAD), 0.);
      
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 SG20140312:
	 Vorschlag von Joachim Ingwersen zur Simulation der Winterung
    if DS < DSCRIT // DSCRIT = 0.25
    	NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD))
    else
    	NDEM   = INSW(SLNMIN-SLN+1.E-5,  min(NUPTX,NDEMAD))
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
   
      //NFIXR  = (double)pGPltN->fNFixationReserve;
      //RNFIXR = NFIX - min(NDEM,NFIXR/TCP);//rate
      //pGPltN->fNFixReserveChangeR = (double)RNFIXR;
           
      /*
      pGPltN->fNDmndActDrv     = (double)NDEMA;
      pGPltN->fNDemand         = (double)NDEM;
      //pPltN->fTotalDemand      = (double)NDEM / (double)10;// 1.0 [kg N ha-1] = [g N m-2]  
      pPltN->fTotalDemand      = (double)NDEM * (double)10;// [g N m-2] = 10.0 [kg N ha-1] 
      //*/

	  NDEMP  = (double)pGPltN->fNDmndPrev;	
      RNDEMP = (NDEM-NDEMP)/DELT;//rate
	 // RNDEMP = (NDEM-NDEMP)/1.0;//rate //Test of Hong
      pGPltN->fNDmndPrvChangeR = (double)RNDEMP;

      
//-----------------------------------------------------------------------------------------

      NCR   = INSW(SLNT-SLNMIN,0.,min(NUPTX,NDEMA))
             /(YGV*(double)pGPltC->fNetStdgCropPhotosynR*12./44.);
      FNSH  = 1./(1.+NCR*DERI/NOTNUL(SHSA)*CSH/CRT*NRT/NSH);
      FCSH  = 1./(1.+NCR*DERI/NOTNUL(SHSA));

      pGPltN->fNToCFracNewBiomass = (double)NCR;
	  pGPltN->fNFracPartShoot     = (double)FNSH;
      pGPltC->fCFracPartToShoot   = (double)FCSH;
	        
      NUPTN  = (double)pPltN->fActNO3NUpt/10;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      NUPTA  = (double)pPltN->fActNH4NUpt/10;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      //NUPT   = (double)pPltN->fActNUpt/10;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      NUPT   = (double)pPltN->fActNUpt/10 + min(NDEM, NFIXR/self->TCP);//[kg N ha-1 d-1] --> [g N m-2 d-1]
	  
	  if (NUPT >0)
	  {
		  int Hong;
		  Hong=1;
	  }
      
      //%** Maintenance and total respiration (g CO2 m-2 d-1)
      RMUN   = 44./12.*2.05*NUPTN;
      RMUA   = 44./12.*0.17*NUPTA;
      RMUS   = 0.06* 0.05/0.454*YGV*ASSA;
      RMLD   = 0.06*(1.-FCSH)*ASSA;
      RRMUL  = (RMUN+RMUA+RMUS+RMLD-RMUL)/DELT;//rate
	  pGPltC->fUptRespCostR = (double)RRMUL;
	  //pGPltC->fUptRespCostR =0.0;//Hong
	  //RRMUL=0.0;//Hong
      ///*
      //*/
      YGO    = (double)pGen->fGrwEffStorage;//parameter
	  DCDSR  = (double)pGPltC->fCShortFallDemandSeed;

      DCSS   = 12./44.*    FCSH *ASSA;
      DCSR   = 12./44.*(1.-FCSH)*ASSA;
      pGPltC->fCDlySupplyShoot  = (double)DCSS;
      pGPltC->fCDlySupplyRoot   = (double)DCSR;

      FDS = betaf(DVR,1.,PMES*1.,LIMIT(1.,2.,DS)-1.);

	  //SG 20180409: sugarbeet model J. Rabe - sink strength of storage organs from parameter 'SINKBEET'
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
		  sinkg(DS,1.,SINKBEET*CFO,YGO,FDS,DCDSR,DCSS,DELT,&DCDSC,&DCDS,&FLWCS); //sugar beet
	  else
		  sinkg(DS,1.,TSN*SEEDW*CFO,YGO,FDS,DCDSR,DCSS,DELT,&DCDSC,&DCDS,&FLWCS); //other crops

      CRVS   = (double)pGPltC->fCStemReserve;//Input
      CRVR   = (double)pGPltC->fCRootReserve;//Input
      GAP    = max(0., DCDS-DCSS);
      CREMSI = min(0.94*CRVS, CRVS/NOTNUL(CRVS+CRVR)*GAP)/0.94;
      CREMRI = min(0.94*CRVR, CRVR/NOTNUL(CRVS+CRVR)*GAP)/0.94;
      CREMS  = INSW(DCDS-DCSS, 0., CREMSI);
      CREMR  = INSW(DCDS-DCSS, 0., CREMRI);

      pGPltC->fCSeedGrowthGap   = (double)GAP;
      pGPltC->fCStemToStorageRI = (double)CREMSI;
      pGPltC->fCRootToStorageRI = (double)CREMRI;
      pGPltC->fCStemToStorageR  = (double)CREMS;
      pGPltC->fCRootToStorageR  = (double)CREMR;
	  //pGPltC->fCStemReserve     = (double)CRVS;
	  //pGPltC->fCRootReserve     = (double)CRVR;


      pGPltC->fCShortFallDemandSeed  = (double)DCDSR;
      pGPltC->fCFlowToSink           = (double)FLWCS;
      pGPltC->fCDlyDemandStorage     = (double)DCDS;
      pGPltC->fCActDemandSeed        = (double)DCDSC;



	  //%** Daily carbon flow for structural stem growth
      DCDTR  = (double)pGPltC->fCShortFallDemandStem;
      DCDTP  = (double)pGPltC->fCPrvDemandStem;

      DCST   = DCSS - FLWCS;
	  IFSH   = LIMIT(0.,1.,DCST/NOTNUL(DCDTP));
      FDH = betaf(DVR,(1.+ESD)/2.,PMEH*(1.+ESD)/2.,min((1.+ESD)/2.,DS));      

	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is 'storage organ'
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
		  sinkg(DS,0.,CDMHT*HTMX*CFO,YGO,FDH*IFSH,DCDTR,DCST,DELT,&DCDTC,&DCDT,&FLWCT); //sugar beet
	  else
		  sinkg(DS,0.,CDMHT*HTMX*CFV,YGV,FDH*IFSH,DCDTR,DCST,DELT,&DCDTC,&DCDT,&FLWCT); //other crops

	  RDCDTP = (DCDTC-DCDTP)/DELT;//rate

      //%-- Output
      pGPltC->fCDlySupplyStem        = (double)DCST;
      pGPltC->fCFlowToStem           = (double)FLWCT;
      pGPltC->fCDlyDemandStem        = (double)DCDT;
      pGPltC->fCActDemandStem        = (double)DCDTC;
      self->pGecrosPlant->pGecrosCarbon->fCPrvDmndStmChangeR = (double)RDCDTP;


      //FCSST  = INSW(DS-(ESD+0.2), FLWCT/NOTNUL(DCSS), 0.);
	  //--> Fraction of available assimilates used for stem growth (vorher)
     //SG20130304: verhindern, dass alles in Stängel und nichts in Blätter geht:
	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is 'storage organ'
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
		   FCSST  = min(0.4,INSW(DS-(ESD+0.2), FLWCT/NOTNUL(DCSS), 0.)); //sugar beet
    else
            FCSST  = min(0.5,INSW(DS-(ESD+0.2), FLWCT/NOTNUL(DCSS), 0.));//other crops
	  //--> Fraction of available assimilates used for stem growth, limitiert auf höchstens 50%
      FCSO   = FLWCS/NOTNUL(DCSS);
	  //--> Fraction of available assimilates used for grain filling (in der vegetativen Phase = 0)
      FCLV   = REAAND(LAIN-LAIC,ESD-DS)*(1.-FCSO-FCSST);
	  // --> Fraction of available assimilates used for leave growth

        
      FCRVS  = max(0.,1. - FCLV - FCSO - FCSST);
      FCRVR  = INSW(CSRTN-CSRT, 1., 0.);

      //output
      pGPltC->fCFracPartToLeaf        = (double)FCLV;
      pGPltC->fCFracPartToStem        = (double)FCSST;
      pGPltC->fCFracPartToStorage     = (double)FCSO;
      pGPltC->fCFracPartToStemReserve = (double)FCRVS;
      pGPltC->fCFracPartToRootReserve = (double)FCRVR;
      ///*
      ESD    = INSW(DETER, ESDI, 1.);
      LWLVM  = (LAIC-min(LAIC,LAIN))/SLA0/DELT;
//SG 20110801: geringeres Absterben der Blätter wg. Messdaten PAK 346 (Kraichgau)
	  LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.01*WLV);  //original: 0.03
//    LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.03*WLV);
	  
      LCLV   = LWLV*CFV;//rate
      LNLV   = min(LWLV,LWLVM)*LNCMIN + (LWLV-min(LWLV,LWLVM))*LNC;//rate
      LCRT   = max(min(CSRT-1.E-4,CSRT-min(CSRTN,CSRT)),0.)/DELT;//rate
      LWRT   = LCRT/CFV;
      LNRT   = LWRT*RNCMIN;//rate
      //*/
	  
	  

      pGPltC->fCStrctRootN=(double)CSRTN;
      pGPltB->fRootWeightLossR = (double)LWRT;
      pGPltC->fCRootLossR = (double)LCRT;
      pGPltN->fNRootLossR = (double)LNRT;

      RCLV   = 12./44.*ASSA*    FCSH *    FCLV  *YGV - LCLV;//rate
      RCSST  = 12./44.*ASSA*    FCSH *    FCSST *YGV;//rate
	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is 'storage organ'
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
		  RCSST  = 12./44.*ASSA*    FCSH *    FCSST *YGO;//sugar beet rate

	  RCSO   = 12./44.*ASSA*FCSH*FCSO*YGO + 0.94*(CREMS+CREMR)*YGO;//rate
      RCSRT  = 12./44.*ASSA*(1.-FCSH)*(1.-FCRVR)*YGV - LCRT;//rate
      RCRVS  = FCRVS*DCSS - CREMS;//rate
      RCRVR  = FCRVR*DCSR - CREMR;//rate
      RDCDSR = max(0.,(DCDSC-RCSO/YGO))-(FLWCS-min(DCDSC,DCSS));//rate
      RDCDTR = max(0.,(DCDTC-RCSST/YGV))-(FLWCT-min(DCDTC,DCST));//rate
	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is 'storage organ'
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
	      RDCDTR = max(0.,(DCDTC-RCSST/YGO))-(FLWCT-min(DCDTC,DCST));//sugar beet rate

      RWLV   = RCLV / CFV;
      RWST   = RCSST/ CFV + RCRVS/0.444;
	  //SG 20180409: sugarbeet model J. Rabe - 'stem' is 'storage organ'
	  if(!strcmp(spec,"SB")||!strcmp(spec,"OR"))
		  RWST   = RCSST/ CFO + RCRVS/0.444;//sugar beet

	  RWSO   = RCSO / CFO;
      RWRT   = RCSRT/ CFV + RCRVR/0.444;

      //output
      pGPltB->fLeafWeightLossR = (double)LWLV;
      pGPltC->fCLeafLossR = (double)LCLV;
      pGPltN->fNLeafLossR = (double)LNLV;

      pGPltB->fRootWeightLossR = (double)LWRT;
      pGPltC->fCRootLossR = (double)LCRT;
      pGPltN->fNRootLossR = (double)LNRT;
 

      pGPltC->fCLeafChangeR    = (double)RCLV;
      pGPltC->fCStemChangeR    = (double)RCSST;
      pGPltC->fCRootChangeR    = (double)RCSRT;
      pGPltC->fCStorageChangeR = (double)RCSO;

      pPltB->fStemGrowR       = (double)RWST;
      pGPltB->fStorageGrowR   = (double)RWSO;
      pPltB->fLeafGrowR       = (double)RWLV;
      pPltB->fRootGrowR       = (double)RWRT;
  
      pGPltC->fCRootRsrvChangeR = (double)RCRVR;
      pGPltC->fCStemRsrvChangeR = (double)RCRVS;
      pGPltC->fCShortFallDemandStemR = (double)RDCDTR;
      pGPltC->fCShortFallDemandSeedR = (double)RDCDSR;

      RNRES  = NUPT-(LNCMIN*(RCLV+LCLV)+RNCMIN*(RCSRT+LCRT)+STEMNC*RCSST)/CFV;
      RNREOE = INSW (DS-ESD, RNRES, 0.);
      RNREOF = INSW (DS-1.0, RNRES, 0.);

      RG     = 44./12.*((1.-YGV)/YGV*(RCLV+RCSST+RCSRT+LCLV+LCRT)+(1.-YGO)/YGO* RCSO);
      RESTOT = RM+RX+RG + 44./12.*0.06*(CREMS+CREMR); 


      //%**output maintenance respiration       
      pGPltN->fNRemobToSeedR   = (double)RNRES;
      pGPltN->fNRemobToSeedER  = (double)RNREOE;//rate
      pGPltN->fNRemobToSeedFR  = (double)RNREOF;//rate

      pPltC->fGrowthRespR = (double)RG;
      pPltC->fTotRespR    = (double)RESTOT;

	  // N accumulation rates of plant organs
      rnacc(FNSH,NUPT,RWST,STEMNC,LNCMIN,RNCMIN,LNC,RNC,NLV,NRT,WLV,WRT,DELT,
            CB,CX,TM,DS,SEEDNC,RWSO,LNLV,LNRT,&RNRT,&RNST,&RNLV,&RTNLV,&RNSO);

      //%-- output
      pGPltN->fNRootAccumR    = (double)RNRT;//rate
      pGPltN->fNStemAccumR    = (double)RNST;//rate
      pGPltN->fNLeafAccumR    = (double)RNLV;//rate
      pGPltN->fNLeafTotAccumR = (double)RTNLV;//rate
      pGPltN->fNStorageAccumR = (double)RNSO;//rate

	  //LeafAreaGrowth_GECROS(exp_p);
	  //PlantHeightGrowth_GECROS(exp_p);
      //CanopyFormation_GECROS(exp_p);
	  //RootSystemFormation_GECROS(exp_p);
	  /* 
	  //%---rate of LAI driven by carbon supply
      RLAI   =  INSW(RWLV, max(-LAIC+(double)1.E-5,SLA0*RWLV), SLA0*RWLV);//rate
      //%---rate of LAI driven by nitrogen during juvenile phase    
      if ((LAIC < (double)1)&&(DS < (double)0.5))
      RLAI  = (SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV);
      //RLAI   = max(0.,(SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV));//rate    
      pCan->fLAGrowR     = (double)RLAI;
      */

	  DS   = max(0., DS + DVR*DELT);
	  pDev->fStageSUCROS = (double)DS;
	  //pDev->fDevStage    = (double)DS;

	  CTDU   = max(0., CTDU  + TDU*DELT);
	  //CTDU   = max(0., CTDU  + TDU);//Hong
	  pDev->fCumTDU  = (double)CTDU;

	        //%** Carbon accumulation
      CLV    = max(0.01, CLV + RCLV*DELT);
	  pGPltC->fCLeaf = (double)CLV;

      CLVD   = max(0., CLVD  + LCLV*DELT);
	  pGPltC->fCLeafDead = (double)CLVD;

      CLVDS  = max(0., CLVDS + LVDS*DELT);
      pGPltC->fCLeafDeadSoil = (double)CLVDS; 

      CSST   = max(0., CSST  + RCSST*DELT);
	  pGPltC->fCStem = (double)CSST;

      CSO    = max(0., CSO   + RCSO*DELT);
	  pGPltC->fCStorage =(double)CSO;

      CSRT   = max(0., CSRT  + RCSRT*DELT); 	  
	  
	  pGPltC->fCStrctRoot =(double)CSRT;

      CRTD   = max(0., CRTD  + LCRT*DELT);
      pGPltC->fCRootDead = (double)CRTD;

	  NRT    = max(0,NRT+RNRT*DELT);
	  //SG20110909
	  pPltN->fRootCont = (double)(NRT*10.0); //[g/m2] --> [kg/ha]
      //pPltN->fRootCont = (double)NRT; 

	  //SG20110909: für Ausgabe in *.rfp
	  pPltN->fStovCont  = (double)(NSH*10.0); //[g/m2] --> [kg/ha]
      pPltN->fGrainCont = (double)(NSO*10.0); //[g/m2] --> [kg/ha]

        

						
      NST = (double)pPltN->fStemCont;
	  NST    = max(0,NST+RNST*DELT);
      pPltN->fStemCont = (double)NST;
        
      NLV    = max(0,NLV+RNLV*DELT);
      pPltN->fLeafCont = (double)NLV;
        
      TNLV   = max(0,TNLV+RTNLV*DELT);
      pGPltN->fNLeafTotCont = (double)TNLV;
        
      NSO    = max(0,NSO+RNSO*DELT);
      pGPltN->fNStorageCont = (double)NSO;
      
	  NLVD =(double)pGPltN->fNLeafDeadCont;
      NLVD   = max(0,NLVD+LNLV*DELT);
      pGPltN->fNLeafDeadCont = (double)NLVD;

      NRTD = (double)pGPltN->fNRootDeadCont; 
      NRTD   = max(0,NRTD+LNRT*DELT);
      pGPltN->fNRootDeadCont = (double)NRTD;

      CRVS   = max(0,CRVS+RCRVS*DELT);
	  pGPltC->fCStemReserve     = (double)CRVS;

      CRVR   = max(0,CRVR+RCRVR*DELT);
	  pGPltC->fCRootReserve     = (double)CRVR;

      NREOE  = max(0,NREOE+RNREOE*DELT);
	  pGPltN->fNRemobToSeedE   = (double)NREOE;

      NREOF  = max(0,NREOF+RNREOF*DELT);
      pGPltN->fNRemobToSeedF   = (double)NREOF;

      DCDSR  = max(0,DCDSR+RDCDSR*DELT);
      pGPltC->fCShortFallDemandSeed  = (double)DCDSR;

      DCDTR  = max(0,DCDTR+RDCDTR*DELT);
      pGPltC->fCShortFallDemandStem  = (double)DCDTR;

      SLNB   = max(0,SLNB+RSLNB*DELT);
      pGPltN->fNLeafSpecificContBottom = (double)SLNB;

      //LAIC   = max(0,LAIC+RLAI);
      //pGCan->fLAICdeterm = (double)LAIC;

      RMUL   = max(0,RMUL+RRMUL*DELT);
      pGPltC->fUptRespCost  = (double)RMUL;
 
      /*
      NDEMP  = max(0,NDEMP+RNDEMP);
      pGPltN->fNDmndPrev          = (double)NDEMP;
      
      NSUPP  = max(0,NSUPP+RNSUPP);
      pGPltN->fNPlantSuppPrev     = (double)NSUPP;
      
      NFIXT  = (double)pGPltN->fNFixationTotal;
      NFIXT  = max(0,NFIXT+NFIX);
	  pGPltN->fNFixationTotal     = (double)NFIXT;

      NFIXR  = max(0,NFIXR+RNFIXR);
      pGPltN->fNFixationReserve   = (double)NFIXR;
      */
      DCDTP  = max(0,DCDTP+RDCDTP*DELT);
      self->pGecrosPlant->pGecrosCarbon->fCPrvDemandStem = (double)DCDTP;

	  /*
      HT     =(double)pPl->pCanopy->fPlantHeight; 
      RHT    = (double)pGCan->fPlantHeightGrowR;
      HT     = max(0,HT+RHT);
	  RHT    = min(HTMX-HT, FDH*HTMX*IFSH);//rate
      pPl->pCanopy->fPlantHeight = (double)HT;
      pGCan->fPlantHeightGrowR   = (double)RHT;
      
      KR     = -log(0.05)/RDMX;
      RD     =(double)pPl->pRoot->fDepth; 
      RRD    = INSW(RD-RDMX, min((RDMX-RD)/DELT,(RWRT+LWRT)/(WRB+KR*(WRT+WRTD))), 0.);//rate
      RD     = max(0,RD+RRD);
      pPl->pRoot->fDepthGrowR = (double)RRD; 
      pPl->pRoot->fDepth = (double)RD;
      */
	  /*
	  TDAPAR = MAX(0.,INTGRL(TDAPAR, DAPAR, DELT))
      TPCAN  = MAX(0.,INTGRL(TPCAN, APCAN, DELT))
      TRESP  = MAX(0.,INTGRL(TRESP, RESTOT, DELT))
      TTCAN  = MAX(0.,INTGRL(TTCAN, ATCAN, DELT))
      TNUPT  = MAX(0.,INTGRL(TNUPT, NUPT, DELT))
      */

	  //-------------------------------------------------------------------------------------

      return 1;
      }//Growth_GECROS

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      CanopyFormation_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//int  LeafAreaGrowth_GECROS(EXP_POINTER);
//int  PlantHeightGrowth_GECROS(EXP_POINTER);


int   CanopyFormation_GECROS(gecros *self)
      {
       LeafAreaGrowth_GECROS(self);
       PlantHeightGrowth_GECROS(self);

       return 1;
      }


int   LeafAreaGrowth_GECROS(gecros *self)
     {
      expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;

      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PCANOPY      pCan  = pPl->pCanopy;

      PGECROSCARBON     pGPltC = self->pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY     pGCan  = self->pGecrosPlant->pGecrosCanopy;

	  double TLAI,KN,KW,NBK,LAIN,NLV,TNLV,RLAI,RWLV,DS,CLVD, CLVDS, DLAI, DLAIS;//,CFV;
      double LAI,LAIC,RNLV,RSLNB,SLNB;
	  double BLD,CFV,SLA0,SLNMIN;

      //double DELT = (double)1;
	  double DELT = (double)xpn->pTi->pTimeStep->fAct;

//-------------------------------------------------------------------------------------
      
      //%** Leaf area development
	  ///*
      RWLV   = (double)pPltB->fLeafGrowR;
      NLV    = (double)pPltN->fLeafCont;
      RNLV   = (double)pGPltN->fNLeafAccumR;
	  TNLV   = (double)pGPltN->fNLeafTotCont;
      SLNB   = (double)pGPltN->fNLeafSpecificContBottom; 
      RSLNB  = (double)pGPltN->fNLeafSpecificContBottomChangeR; 
      CLVD   = (double)pGPltC->fCLeafDead;
	  CLVDS  = (double)pGPltC->fCLeafDeadSoil;
      DS     = (double)pDev->fStageSUCROS;
	  //*/
      LAIC   = (double)pGCan->fLAICdeterm;	  
      RLAI   = (double)pCan->fLAGrowR;
	  TLAI   = (double)pGCan->fLAITotal;

	  SLA0   = (double)self->SLA0;
      CFV    = (double)self->CFV;
	
	  BLD    = (double)self->BLD;
      SLNMIN = (double)self->SLNMIN;

	  KW     = kdiff(TLAI,BLD*3.141592654/180.,0.2);
	  KN     = KW*(TNLV-SLNMIN*TLAI);
	  NBK    = SLNMIN*(1.-exp(-KW*TLAI));
      KN     = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));

      
      //%---rate of LAI driven by carbon supply
      RLAI   =  INSW(RWLV, max(-LAIC+(double)1.E-5,SLA0*RWLV), SLA0*RWLV);

	
      //%---rate of LAI driven by nitrogen during juvenile phase    
      if ((LAIC < (double)1)&&(DS < (double)0.5))
	      //RLAI  = (SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV);
          RLAI   = max(0.,(SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV)); 
	  
  //    if ((LAIC > (double)1)&&(DS < (double)0.5)) //Test of Hong
//	      RLAI  = (SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV);
         // RLAI   = max(0.,(SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV)); //Test of Hong	
		
		  
	 if (DS >1.0)
	 {
		 int Hong;
		 Hong =1;
	 }
	
	
      //%**********************************************************

      LAIC   = max(0,LAIC+RLAI*DELT);

	  LAIN   = log(1.+KN*max(0.,NLV)/SLNMIN)/KN;
	  
	  //LAIN=LAIN*DELT;//Test of Hong
	  LAI    = min(LAIN, LAIC);

	  //CFV  = (double)pGPltC->fCFracVegetative;
      //SLA    = LAI/WLV;
      DLAI   = (CLVD-CLVDS)/CFV*SLA0;//original
	  DLAIS  = CLVDS/CFV*SLA0;

      //TLAI   = LAIC + CLVD /CFV*SLA0;//original
	  TLAI   = LAIC + DLAI;

      //if(LAIN>=LAIC) TLAI   = LAIC + CLVD /CFV*SLA0;//original
      if(LAIN>=LAIC) TLAI   = LAIC + DLAI;
      
	  //%** output ***
      pCan->fLAGrowR     = (double)RLAI;
      pGCan->fLAICdeterm = (double)LAIC;
	  pPl->pCanopy->fLAI = (double)TLAI;	
      self->LAI =   (double)TLAI; //TESt of Hong 

      /*
	  pGCan->fLAINdeterm = (double)LAIN;
      pGCan->fLAIGreen   = (double)LAI;
      pGCan->fLAITotal   = (double)TLAI;
      */
//-------------------------------------------------------------------------------------

      return 1;
}



int  PlantHeightGrowth_GECROS(gecros *self)
    {
      expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;
	  
	  double HT,DS,DVR,DCDTC,DCST,DCDTP,FDH,ESD,IFSH,RHT;
      double RDCDTP; 

	  double DETER = (double)self->DETER;
      double ESDI  = (double)self->ESDI;
	  double PMEH  = (double)self->PMEH;
	  double HTMX  = (double)self->HTMX;

	  //double DELT =(double)1;
	  double DELT =(double)xpn->pTi->pTimeStep->fAct;
//-------------------------------------------------------------------------------------
      ///*  
      //%-- input
      DS    = (double)pPl->pDevelop->fStageSUCROS;
      DVR   = (double)pPl->pDevelop->fDevR;
	  
      DCDTC = (double)self->pGecrosPlant->pGecrosCarbon->fCActDemandStem;
      DCST  = (double)self->pGecrosPlant->pGecrosCarbon->fCDlySupplyStem;
	  //*/
      DCDTP  = (double)self->pGecrosPlant->pGecrosCarbon->fCPrvDemandStem;
	  RDCDTP = (double)self->pGecrosPlant->pGecrosCarbon->fCPrvDmndStmChangeR;
      HT     = (double)pPl->pCanopy->fPlantHeight;
	  RHT    = (double)self->pGecrosPlant->pGecrosCanopy->fPlantHeightGrowR;

      //%** Plant height or stem length (m)
      
	  ESD    = INSW(DETER, ESDI, 1.);
      FDH    = betaf(DVR,(1.+ESD)/2.,PMEH*(1.+ESD)/2.,min((1.+ESD)/2.,DS));

      DCDTP  = max(0,DCDTP+RDCDTP*DELT);
      RDCDTP = (DCDTC-DCDTP)/DELT;
      IFSH   = LIMIT(0.,1.,DCST/NOTNUL(DCDTP));

      HT     = max(0,HT+RHT*DELT);
	  RHT    = min(HTMX-HT, FDH*HTMX*IFSH);//rate

      //%-- output
      pPl->pCanopy->fPlantHeight = (double)HT;
      self->pGecrosPlant->pGecrosCanopy->fPlantHeightGrowR = (double)RHT;

      //pGecrosPlant->pGecrosCarbon->fCPrvDemandStem     = (double)DCDTC;
      //pGecrosPlant->pGecrosCarbon->fCPrvDemandStem     = (double)DCDTP;
	  //pGecrosPlant->pGecrosCarbon->fCPrvDmndStmChangeR = (double)RDCDTP;

//-------------------------------------------------------------------------------------


      return 1;
    }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Root depth 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   RootSystemFormation_GECROS(gecros *self)
     {
      expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;

      double RD,RWRT,LWRT,WRT,WRTD,KR,RRD;
	  
	  double RDMX = (double)self->RDMX;
	  double WRB  = (double)self->WRB;

	  //double DELT = (double)1;
	  double DELT = (double)xpn->pTi->pTimeStep->fAct;

//-------------------------------------------------------------------------------------
      
      //input
      RD     = (double)pPl->pRoot->fDepth;
      //RRD    = (double)pPl->pRoot->fDepthGrowR;
	  ///*
      //RDMX   = (double)pGecrosPlant->pGecrosParameter->fRootDepthMax;
      RWRT   = (double)pPl->pBiomass->fRootGrowR;
      LWRT   = (double)self->pGecrosPlant->pGecrosBiomass->fRootWeightLossR;
      WRT    = (double)pPl->pBiomass->fRootWeight/10;//[g m-2] <-- [kg ha-1]
      WRTD   = (double)self->pGecrosPlant->pGecrosBiomass->fRootDeadWeight;
      //*/
      //%** Rooting depth (cm)
	  KR     = -log(0.05)/RDMX;
      RRD    = INSW(RD-RDMX, min((RDMX-RD)/DELT,(RWRT+LWRT)/(WRB+KR*(WRT+WRTD))), 0.);//rate
      RD     = max(0,RD+RRD*DELT);

      //output
      pPl->pRoot->fDepthGrowR = (double)RRD; 
      pPl->pRoot->fDepth = (double)RD;
	  	  	  	  
//-------------------------------------------------------------------------------------

      return 1;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      OrganSenescence_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   OrganSenescence_GECROS(gecros *self)
      {   
      expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;

      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;

	  PGECROSBIOMASS    pGPltB = self->pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON     pGPltC = self->pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY     pGCan  = self->pGecrosPlant->pGecrosCanopy;

	  double ESD,LWLVM,LWLV,LCLV,LNLV;
      double LAIC,LAIN,WLV,DS;//,CFV,RDMX;
      double LNC,LNCMIN,RNCMIN;
      double KCRN,CSRTN,LWRT,LCRT,LNRT;
      double NRT,CRVR,CSRT;

      double CFV   = (double)self->CFV;
	  double DETER = (double)self->DETER;
      double ESDI  = (double)self->ESDI;
	  double SLA0  = (double)self->SLA0;
	  double RDMX  = (double)self->RDMX;
	  double WRB   = (double)self->WRB;
	 
	  //double DELT = (double)1;
	  double DELT = (double)xpn->pTi->pTimeStep->fAct;

//-------------------------------------------------------------------------------------
      
      //Senescence
      //%-- input --
	  ///*
      LAIC   = (double)pGCan->fLAICdeterm;
      LAIN   = (double)pGCan->fLAINdeterm;
      WLV    = (double)pPltB->fLeafWeight/10;//[kg ha-1] --> [g m-2]
      DS     = (double)pDev->fStageSUCROS;
      //CFV    = (double)pGPltC->fCFracVegetative;
      //RDMX   = (double)pGPar->fRootDepthMax;
      LNC    = (double)pPltN->fLeafActConc;
      LNCMIN = (double)pPltN->fLeafMinConc;
      RNCMIN = (double)pPltN->fRootMinConc;

      //SG20110909
	  NRT    = (double)pPltN->fRootCont/10.0; //[g/m2] <-- [kg/ha]
      //NRT    = (double)pPltN->fRootCont;
      CRVR   = (double)pGPltC->fCRootReserve;
      CSRT   = (double)pGPltC->fCStrctRoot;
      //*/

      //%** Leaf senescence
      ESD    = INSW(DETER, ESDI, 1.);

      LWLVM  = (LAIC-min(LAIC,LAIN))/SLA0/DELT;
      //SG 20110801:
	  LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.01*WLV);
      //LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.03*WLV);
      LCLV   = LWLV*CFV;
      LNLV   = min(LWLV,LWLVM)*LNCMIN + (LWLV-min(LWLV,LWLVM))*LNC;
      //LWLV   = 0.0;
      //LCLV   = 0.0;
      //LNLV   = 0.0;

      //%** Root senescence
      KCRN   = -log(0.05)/6.3424/CFV/WRB/RDMX;
      CSRTN  = 1./KCRN*log(1.+KCRN*max(0.,(NRT*CFV-CRVR*RNCMIN))/RNCMIN);
      LCRT   = max(min(CSRT-1.E-4,CSRT-min(CSRTN,CSRT)),0.)/DELT;
      LWRT   = LCRT/CFV;
      LNRT   = LWRT*RNCMIN;
      //LWRT   = 0.0;
      //LCRT   = 0.0;
      //LNRT   = 0.0;

      //%** output
      pGPltB->fLeafWeightLossR = (double)LWLV;
      pGPltC->fCLeafLossR = (double)LCLV;
      pGPltN->fNLeafLossR = (double)LNLV;

      pGPltC->fCStrctRootN=(double)CSRTN;
      pGPltB->fRootWeightLossR = (double)LWRT;
      pGPltC->fCRootLossR = (double)LCRT;
      pGPltN->fNRootLossR = (double)LNRT;

//-------------------------------------------------------------------------------------

      return 1;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      NitrogenFixation_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   NitrogenFixation_GECROS(gecros *self)
     {
      expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;
      
      PPLTCARBON   pPltC  = pPl->pPltCarbon;

	  PGECROSCARBON     pGPltC = self->pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;

	  double NDEM,NSUP,APCAN,RM,NFIXE,NFIXD,NFIX,NFIXT,NFIXR;
      double NDEMP,RNDEMP,NSUPP,RNSUPP;
      
	  //double DELT = (double)1;
	  double DELT = (double)xpn->pTi->pTimeStep->fAct;
      
	  double RNFIXR,RX;
      double CCFIX  = (double)self->CCFIX;
	  double LEGUME = (double)self->LEGUME;
      double TCP    = (double)1;//time constant

//-------------------------------------------------------------------------------------

      //%** input ***
      NDEM   = (double)pGPltN->fNDemand;
	  NDEMP  = (double)pGPltN->fNDmndPrev;
	  RNDEMP = (double)pGPltN->fNDmndPrvChangeR;

      NSUP   = (double)pGPltN->fNPlantSupply;
	  NSUPP  = (double)pGPltN->fNPlantSuppPrev;
	  RNSUPP = (double)pGPltN->fNPltSppPrvChangeR;

      APCAN  = (double)pPltC->fGrossPhotosynR;
      RM     = (double)pPltC->fMaintRespR;
	  
	  APCAN  = (double)pPltC->fGrossPhotosynR;
      RM     = (double)pPltC->fMaintRespR;	  
	  
      CCFIX  = (double)pGPltC->fCNFixCost;
      NFIXT  = (double)pGPltN->fNFixationTotal;
      NFIXR  = (double)pGPltN->fNFixationReserve;
      RNFIXR = (double)pGPltN->fNFixReserveChangeR;
	  
	  CCFIX  = self->CCFIX;
       
      //%** Nitrogen fixation (g N m-2 d-1)
	  RNDEMP = (NDEM-NDEMP)/DELT;
      NDEMP  = max(0,NDEMP+RNDEMP*DELT);
	  RNSUPP = (NSUP-NSUPP)/DELT;
	  NSUPP  = max(0,NSUPP+RNSUPP*DELT);

      NFIXE  = max(0., APCAN-1.E-5-RM)/NOTNUL(CCFIX)*12./44.;
      NFIXD  = max(0., NDEMP-NSUPP);// /DELT;
      //NFIXD  = max(0., NDEM-NSUP);
      NFIX   = INSW (LEGUME, 0., min(NFIXE, NFIXD));
      NFIXT  = max(0,NFIXT+NFIX*DELT);
      RNFIXR = NFIX - min(NDEM,NFIXR/TCP);
      NFIXR  = max(0,NFIXR+RNFIXR*DELT);

	  RX     = 44./12.*(CCFIX*NFIX);

 
      //%** output
      //pGPltN->fNDemand            = (double)NDEM;
      pGPltN->fNDmndPrev          = (double)NDEMP;
      pGPltN->fNDmndPrvChangeR    = (double)RNDEMP;

      //pGPltN->fNPlantSupply       = (double)NSUP;
      pGPltN->fNPlantSuppPrev     = (double)NSUPP;
      pGPltN->fNPltSppPrvChangeR  = (double)RNSUPP;

      pGPltN->fNFixation          = (double)NFIX;
      pGPltN->fNFixationTotal     = (double)NFIXT;
      pGPltN->fNFixationReserve   = (double)NFIXR;
      pGPltN->fNFixReserveChangeR = (double)RNFIXR;

	  pGPltC->fFixRespCost        = (double)RX;

//-------------------------------------------------------------------------------------

      return 1;
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      PotentialNitrogenUptake_WAVE
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int     PotentialNitrogenUptake_GECROS(gecros *self)
   {
      expertn_modul_base *xpn = &(self->parent);
	  PPLANT pPl = xpn->pPl;
	  
	  double DELT = (double)xpn->pTi->pTimeStep->fAct; //Hong

	  PBIOMASS     pPltB = pPl->pBiomass;
      PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PDEVELOP     pDev  = pPl->pDevelop;

	  PGECROSBIOMASS    pGPltB = self->pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON     pGPltC = self->pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;

	  double RX,RMN,NTOT,WSH,LNCMIN,WRT,RNCMIN,APCAN,RMUL,RMRE;
      double SHSA,SHSAN,RM,CSH,APCANN;//,YGV;
      double DERI,CTOT,NDEMA,CRT,NDEMD,DS,HNC,NRT,NSH,HNCCR;
      double NDEMAD,LNC,SLN,NDEM;

      double YGV  = (double)self->YGV;
	  double LNCI = (double)self->LNCI;
	  double NUPTX =(double)self->NUPTX;
	  double SLNMIN = (double)self->SLNMIN;

      //double DELT = (double)1;
	 // double DELT = (double)xpn->pTi->pTimeStep->fAct;
//-------------------------------------------------------------------------------------
      ///*
      //%** input ***
      NTOT   = (double)pPltN->fTotalCont;
      WSH    = (double)pGPltB->fShootWeight;
      WRT    = (double)pPltB->fRootWeight/10; //[g m-2] <-- [kg ha-1]
      LNCMIN = (double)pPltN->fLeafMinConc;
      RNCMIN = (double)pPltN->fRootMinConc;//Parameter
      APCAN  = (double)pPltC->fGrossPhotosynR;
      RX     = (double)pGPltC->fFixRespCost;
      RMUL   = (double)pGPltC->fUptRespCost;
      RM     = (double)pPltC->fMaintRespR;
      APCANN = (double)pGPltC->fGrossStdgCropPhotosynRN;
      RMN    = (double)pGPltC->fMaintRespRN;
      CTOT   = (double)pGPltC->fCPlant;
      CSH    = (double)pGPltC->fCShoot;
      CRT    = (double)pGPltC->fCRoot;
      DS     = (double)pDev->fStageSUCROS;
      //YGV    = (double)pGPar->fGrwEffVegOrg;

      HNC    = (double)pGPltN->fShootActConc;
      NSH    = (double)pGPltN->fNShootCont;
	  NRT    = (double)pPltN->fRootCont/10.0; //[g m-2] <-- [kg ha-1]
      LNC    = (double)pPltN->fLeafActConc;
      //LAI    = (double)pGCan->fLAIGreen;
      //NLV    = (double)pPltN->fLeafCont;
      SLN    = (double)pGPltN->fNLeafSpecificCont;
      //NUPTX  = (double)pGPltN->fNUptMax;

	  //%** Crop nitrogen demand and uptake (g N m-2 d-1)
      RMRE   = max(min(44./12.*0.218*(NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL), 0.);
      RMN    = min(44./12.*0.218*(1.001*NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL);
      RMN    = max(0., min(APCAN-1.E-5,RMUL) + max(RMN, 0.));
      RM     = max(0., min(APCAN-1.E-5,RMUL) + RMRE);

	  SHSA   = 12./44. * YGV*(APCAN -RM -RX)/ CSH;
      SHSAN  = 12./44. * YGV*(APCANN-RMN-RX)/ CSH;

      DERI   = max(0.,(SHSAN - SHSA)/(0.001*NTOT/CTOT));
	  //DERI   = max(0.,(SHSAN - SHSA)/DELT/(0.001*NTOT/CTOT));
      //DERI   = (double)fabs((SHSAN - SHSA)/(0.001*NTOT/CTOT));
	 // DERI = DERI *2; //Test of Hong
      //printf("DERI= %f\n", DERI);
      //%-- Nitrogen partitioning between shoots and roots
      HNCCR  = LNCI*exp(-0.4*DS);
	  //NDEMD  = INSW(DS-1., WSH*(HNCCR-HNC)*(1.+NRT/NSH)/DELT, 0.);//original, not correct!!! NDEMD is not a rate.
	  NDEMD  = INSW(DS-1., WSH*(HNCCR-HNC)*(1.+NRT/NSH), 0.);//changed by Hong, NUPTX has the unit (g N m-2 d-1) 
	  //NDEMD =NDEMD*0.005; //Test of Hong
      NDEMA  = CRT * SHSA*SHSA/NOTNUL(DERI);
	  //NDEMA= NDEMA*0.5;
	  //printf("NDEMA= %f\n", NDEMA);
	  //printf("NDEMD= %f\n", NDEMD);
      NDEMAD = INSW(LNC-1.5*LNCI, max(NDEMA, NDEMD), 0.);
	  // NDEMAD = NDEMAD*0.5; // Test of Hong 
      NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,NDEMAD), 0.);
	  
     // NDEM = NUPTX; //Test of Hong 		 
//Hong 2016-08-08: change for Sebastian Gayler and Arne Poyda

/*
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SG20140312:
	// Vorschlag von Joachim Ingwersen zur Simulation der Winterung
    if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))             // here we have to 
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))  // distinguish between
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WR"))) // winter and summer crops --> DB!!!
	{
		//	if (DS < 0.25) // DSCRIT = 0.25
		if ((fparVERN > -0.1) && (DS < 0.25)) // DSCRIT = 0.25
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD), 0.);
		else
    		NDEM   = INSW(SLNMIN-SLN+1.E-5,  min(NUPTX,NDEMAD), 0.);
	}
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/*
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SG20140312:
	// Vorschlag von Joachim Ingwersen zur Simulation der Winterung	
    if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)||(strcmp(pPl->pGenotype->acCropCode,"RP")==0)) // here we have to distinguish between winter and summer crops --> DB!!!
	{
		//	if (DS < 0.25) // DSCRIT = 0.25
		//if ((self->VERN > -0.1) && (DS < 0.25)) // DSCRIT = 0.25
		if ((self->VERN > -0.1) && (DS < 0.25)) // DSCRIT = 0.25
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD), 0.);//original
			//NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD)*DELT, 0.);//changed by Hong, NUPTX has the unit (g N m-2 d-1) 
		else
    		NDEM   = INSW(SLNMIN-SLN+1.E-5,  min(NUPTX,NDEMAD), 0.);//original
			//NDEM   = INSW(SLNMIN-SLN+1.E-5,  min(NUPTX,NDEMAD)*DELT, 0.); //changed by Hong, NUPTX has the unit (g N m-2 d-1) 
	}
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//Changed according to suggestion of Arne Poyda on 20180125 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SG20140312:
	// Vorschlag von Joachim Ingwersen zur Simulation der Winterung
	if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) // here we have to distinguish between winter and summer crops --> DB!!!// here we have to distinguish between winter and summer crops --> DB!!!
									
	{
		//	if (DS < 0.25) // DSCRIT = 0.25
		if ((self->VERN > -0.1) && (DS < 0.25)) // DSCRIT = 0.25
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD*0.5*DELT), 0.);
		else
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,NDEMAD), 0.);
	}

	 if(strcmp(pPl->pGenotype->acCropCode,"RP")==0)
	
    // AP20171030
	{
		if ((self->VERN > -0.1) && (DS < 0.30))
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD), 0.);
		else
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,NDEMAD), 0.);
	}
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		
//End of Hong	


      //%** output
      pGPltC->fMaintRespRN     = (double)RMN;
      pGPltB->fShtToRtActivity = (double)DERI;
      pGPltB->fShootActivity   = (double)SHSA;

      pGPltN->fNDmndActDrv     = (double)NDEMA;
	  pGPltN->fNDemand         = (double)NDEM;
      pPltN->fTotalDemand      = (double)NDEM * (double)10;// [g N m-2] = 10.0 [kg N ha-1] 
        
       return 1;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      ActualNitrogenUptake_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   ActualNitrogenUptake_GECROS(gecros *self)
     {
      expertn_modul_base *xpn = &(self->parent);

	  PCHEMISTRY pCh = xpn->pCh;
	  PPLANT pPl = xpn->pPl;
	  PSPROFILE pSo = xpn->pSo;	
	  PWATER pWa = xpn->pWa;

	  PSWATER      pSW  = pSo->pSWater->pNext;//changed by Hong
      PSLAYER      pSL  = pSo->pSLayer->pNext;
      PWLAYER      pSLW = pWa->pWLayer->pNext; 
      PCLAYER      pSLN = pCh->pCLayer->pNext;

      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PLAYERROOT   pLR   = pPl->pRoot->pLayerRoot;

      PGECROSNITROGEN   pGPltN = self->pGecrosPlant->pGecrosNitrogen;
	  PGECROSSOIL       pGS    = self->pGecrosPlant->pGecrosSoil;

      int L,iL,iLayer;
      double fDepth, fnh4p, fno3p, fh2o, fpwc, fpaw;
      double fnh4sup, fno3sup, fws, fnsup, fnh4up, fno3up, fnup;
      double frd, fpor, fndem, fnh4, fno3;
	  double fpd, f1, f2;

      double DeltaZ= pSo->fDeltaZ;
	  //double DELT = (double)1;
	  //double DELT = (double)xpn->pTi->pTimeStep->fAct;

	  double fwcfcwcmin = (double)0.0;//sg/hmgu 20090326
      double fnh4r=(double)10.;//*10 --> ganzes Profil [kg(N)/ha], unabh. von bulk density und Mächtigkeit
      double fno3r=(double)10.;//*10 --> ganzes Profil [kg(N)/ha]
	  //sg/hmgu 20090326 vgl. in CERES:
      //double fnh4r=(double)0.5 *(double)0.01*pSo->pSLayer->fBulkDens*DeltaZ;
      //double fno3r=(double)0.25*(double)0.01*pSo->pSLayer->fBulkDens*DeltaZ;
	

    /*
      FWS    = MIN(1., WUL/(RD*10.*(WCFC-WCMIN)))
      NSUPAS = MAX (0., NAUL+(MINAUL-NITRUL)*TCP-RD/150.*RA)/TCP
      NSUPNS = MAX (0., NNUL+(MINNUL-DENIUL)*TCP-RD/150.*RN)/TCP*FWS
	  NSUPA  = INSW(NSWI, NINPA, NSUPAS)
      NSUPN  = INSW(NSWI, NINPN, NSUPNS)
      NSUPA  = MAX (0., NAUL+(MINAUL-NITRUL)*TCP-RD/150.*RA)/TCP
      NSUPN  = MAX (0., NNUL+(MINNUL-DENIUL)*TCP-RD/150.*RN)/TCP*FWS
      NSUP   = NSUPA + NSUPN

      NUPTA  = MIN(NSUPA, NSUPA/NOTNUL(NSUP)*MAX(0.,NDEM-NFIXR/TCP))
      NUPTN  = MIN(NSUPN, NSUPN/NOTNUL(NSUP)*MAX(0.,NDEM-NFIXR/TCP))
      NUPT   = MAX(0., NUPTA + NUPTN + MIN(NDEM, NFIXR/TCP))
    */

      L = 1;
      fDepth =(double)0;
      fh2o = fpor = fnh4p = fno3p = (double)0;

      while (((pPl->pRoot->fDepth*(double)10)>=fDepth)&&(L<=pSo->iLayers-2))// if pPl->pRoot->fDepth in [cm]
         {
            fnh4p += pSLN->fNH4N;
            fno3p += pSLN->fNO3N;
            fh2o  += pSLW->fContAct*DeltaZ;
            fpor  += pSL->fPorosity*DeltaZ;

			fwcfcwcmin += (pSW->fContFK-pSW->fContPWP)*DeltaZ;// sg/hmgu 20090326
            fDepth += DeltaZ;

            L++;
            pSLN = pSLN->pNext;
			pSLW = pSLW->pNext;
            pSL  = pSL->pNext;
         }
      iL=L-2;

	/////////////////////////////////////////////////////////////////////////
	//SG 20111103: letzte Schicht nur anteilsmäßig

	fh2o = fpor = fnh4p = fno3p = fwcfcwcmin = (double)0;
	fno3r = fnh4r = (double)0.0;
	
	for (pSLW=pWa->pWLayer->pNext,pSL=pSo->pSLayer->pNext,pSW=pSo->pSWater->pNext,pSLN  = pCh->pCLayer->pNext,iLayer=1;
		(pSLW->pNext!=NULL) && (pSL!=NULL) && (pSW->pNext!=NULL) && (pSLN->pNext!=NULL);pSLW=pSLW->pNext,pSL=pSL->pNext,pSW=pSW->pNext,pSLN=pSLN->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= pPl->pRoot->fDepth*(double)10)
	 {
	    fnh4p += pSLN->fNH4N;
        fno3p += pSLN->fNO3N;
        fh2o  += max((double)0,(pSLW->fContAct-pSW->fContPWP))*DeltaZ;
        fpor  += pSL->fPorosity*DeltaZ;
		fwcfcwcmin += (pSW->fContFK-pSW->fContPWP)*DeltaZ;

		//SG 20111103: non-extractable mineral nitrogen from bulk density and thickness 
		fnh4r +=(double)0.5 *(double)0.01*pSL->fBulkDens*DeltaZ; //--> schichtweise
		fno3r +=(double)0.25*(double)0.01*pSL->fBulkDens*DeltaZ;
	 }
	 else if ((pPl->pRoot->fDepth*(double)10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= pPl->pRoot->fDepth*(double)10.+DeltaZ))
     {
		f1 = (iLayer*DeltaZ-pPl->pRoot->fDepth*(double)10)/DeltaZ;

	    fnh4p += pSLN->fNH4N*((double)1-f1);
        fno3p += pSLN->fNO3N*((double)1-f1);
        fh2o  += max((double)0,(pSLW->fContAct-pSW->fContPWP))*DeltaZ*((double)1-f1);
        fpor  += pSL->fPorosity*DeltaZ*((double)1-f1);
		fwcfcwcmin += (pSW->fContFK-pSW->fContPWP)*DeltaZ*((double)1-f1);

 		fnh4r +=(double)0.5 *(double)0.01*pSL->fBulkDens*DeltaZ*((double)1-f1); //--> schichtweise
		fno3r +=(double)0.25*(double)0.01*pSL->fBulkDens*DeltaZ*((double)1-f1);
}
	 else
	 {
	    break;
	 }
	}//for
	// End SG 20111103
/*	
	/////////////////////////////////////////////////////////////////////////
      pGS->fProfileDepth = fDepth;
	  frd  = pPl->pRoot->fDepth/pGS->fProfileDepth;
      fpwc = fpor/(double)NOTNUL((double)pPl->pRoot->fDepth)/(double)10;
      //fpwc = fwcfcwcmin;// sg/hmgu 20090326
	  //fws  =(double) min((double)1.,fh2o/fwcfcwcmin);//SG 20111103
      //fws  = (double)min((double)1.,fh2o/fpwc);
	  if (pGS->fPlantWaterCapacity == (double)0.0) pGS->fPlantWaterCapacity = fwcfcwcmin; 
      fpaw = (double)(pGS->fWaterContUpperLayer/NOTNUL(pPl->pRoot->fDepth*(double)10.*pGS->fPlantWaterCapacity));
	  fws = (double)min((double)1.,fpaw);

      //fnh4sup = max((double)0.,fnh4p-frd*fnh4r);
      //fno3sup = max((double)0.,fno3p-frd*fno3r)*fws;
	  //SG 20111103: non-extractable nitrogen now related to single soil layers:
	  fnh4sup = max((double)0.,fnh4p-fnh4r);
      fno3sup = max((double)0.,fno3p-fno3r)*fws;
 
      fnsup   = fnh4sup + fno3sup;
	  pGPltN->fNPlantSupply = fnsup/10;//[kg N ha-1] --> [g N m-2]

      fndem   = max((double)0.,pGPltN->fNDemand-pGPltN->fNFixationReserve)*10;// [g N m-2] --> [kg N ha-1]
      fnh4up  = min(fnh4sup,fnh4sup/(double)NOTNUL((double)fnsup)*fndem); 
      fno3up  = min(fno3sup,fno3sup/(double)NOTNUL((double)fnsup)*fndem);
      fnup    = max((double)0.,fnh4up+fno3up+min(pGPltN->fNDemand,pGPltN->fNFixationReserve)*10); 


    if (pGPltN->fNDemand<=(double)0)
    {
     pSL = pSo->pSLayer->pNext;// sg/hmgu 20090326
     
	 for (L=1;L<=pSo->iLayers-2;L++)
        {
         pLR->fActLayNO3NUpt = (double)0;
         pLR->fActLayNH4NUpt = (double)0;

         pSL=pSL->pNext;
         pLR=pLR->pNext;
        }
     
      pPltN->fActNO3NUpt = (double)0;
      pPltN->fActNH4NUpt = (double)0;
      pPltN->fActNUptR = (double)0;
    }
    else
	{
      //+++ output to expertn +++//
	  fpd =(pSo->iLayers-2)*DeltaZ;
	  frd = min(fpd-DeltaZ,(double)NOTNUL((double)pPl->pRoot->fDepth*10.));
	  f2  = max((double)1,(fpd-frd)/DeltaZ);

      pSLN  = pCh->pCLayer->pNext;
      pLR   = pPl->pRoot->pLayerRoot;
      pPltN = pPl->pPltNitrogen;
	  pPltN->fActNH4NUpt = (double)0;
	  pPltN->fActNO3NUpt = (double)0;

	  for (L=1;L<=pSo->iLayers-2;L++)
      {
       fnh4 = min(fnh4up*(pSLN->fNH4N/fnh4p),max((double)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(double)10.0));
       fno3 = min(fno3up*(pSLN->fNO3N/fno3p),max((double)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(double)10.0));
       //fnh4 = fnh4up*(pSLN->fNH4N/fnh4p);
       //fno3 = fno3up*(pSLN->fNO3N/fno3p);

       if(L*DeltaZ <= frd) 
	   {
        pLR->fActLayNH4NUpt = fnh4;//*DeltaZ/frd;
        pLR->fActLayNO3NUpt = fno3;//*DeltaZ/frd;
	   }
	   else if ((frd < L*DeltaZ)&&(L*DeltaZ <= frd + DeltaZ))
       {
        f1 = (L*DeltaZ-frd)/DeltaZ;
        pLR->fActLayNH4NUpt = (((double)1-f1)*fnh4);//*DeltaZ/frd);
        pLR->fActLayNO3NUpt = (((double)1-f1)*fno3);//*DeltaZ/frd);
	   }
	   else
	   {
		pLR->fActLayNH4NUpt = (double)0;
        pLR->fActLayNO3NUpt = (double)0;
	   }//if,else if,else
       if(pLR->pNext!=NULL)  pLR =pLR->pNext;
	   if(pSLN->pNext!=NULL) pSLN=pSLN->pNext;

	  }//for
	}//*/

      pGS->fProfileDepth = 150; 
	  //Test of Hong  
	 // pGS->fProfileDepth = pSo->fDepth/(double)10.0; //[cm]
	
	  frd  = pPl->pRoot->fDepth/pGS->fProfileDepth;
      fpwc = fpor/(float)NOTNUL((double)pPl->pRoot->fDepth)/(float)10;
      //fpwc = fwcfcwcmin;// sg/hmgu 20090326 
	  //SG 20111103: pGS->fPlantWaterCapacity wird nicht gesetzt. Deshalb fws aus schichtweiser Berechnung:
      fws  =(float) min((float)1.,fh2o/fwcfcwcmin);
	  //fws    = (float)min((float)1., pGS->fWaterContUpperLayer/NOTNUL(pPl->pRoot->fDepth
	  //                                                *(float)10.*pGS->fPlantWaterCapacity));

      //fnh4sup = max((float)0.,fnh4p-frd*fnh4r);
      //fno3sup = max((float)0.,fno3p-frd*fno3r)*fws;
      //SG 20111103: non-extractable nitrogen now related to single soil layers:
	  fnh4sup = max((float)0.,fnh4p-fnh4r);
      fno3sup = max((float)0.,fno3p-fno3r)*fws;
      fnsup   = fnh4sup + fno3sup;
	  pGPltN->fNPlantSupply = fnsup/10;//[kg N ha-1] --> [g N m-2]

      fndem   = max((float)0.,pGPltN->fNDemand-pGPltN->fNFixationReserve)*10;// [g N m-2] --> [kg N ha-1]
      fnh4up  = min(fnh4sup,fnh4sup/(float)NOTNUL((double)fnsup)*fndem); 
      fno3up  = min(fno3sup,fno3sup/(float)NOTNUL((double)fnsup)*fndem);
      fnup    = max((float)0.,fnh4up+fno3up+min(pGPltN->fNDemand,pGPltN->fNFixationReserve)*10); 


    if (pGPltN->fNDemand<=(float)0)
    {
     pSL = pSo->pSLayer->pNext;// sg/hmgu 20090326
     
	 for (L=1;L<=pSo->iLayers-2;L++)
        {
         pLR->fActLayNO3NUpt = (float)0;
         pLR->fActLayNH4NUpt = (float)0;

         pSL=pSL->pNext;
         pLR=pLR->pNext;
        }
     
      pPltN->fActNO3NUpt = (float)0;
      pPltN->fActNH4NUpt = (float)0;
      pPltN->fActNUptR = (float)0;
    }
    else
/*
    {
      pPltN->fActNO3NUpt = (float)0;
      pPltN->fActNH4NUpt = (float)0;

      pPltN = pPl->pPltNitrogen;
      pLR   = pPl->pRoot->pLayerRoot;
      pSL   = pSo->pSLayer->pNext;
      pSLW  = pWa->pWLayer->pNext; 
      pSLN  = pCh->pCLayer->pNext;
      //frd = max((float)1.,pPl->pRoot->fDepth*(float)10);
      fDepth = (float)DeltaZ;

	  for (L=1;L<=pSo->iLayers-2;L++)
         {
          // sg/hmgu 20090326
          frd = min((float)DeltaZ,max((float)0.0,pPl->pRoot->fDepth*(float)10.0-fDepth+(float)DeltaZ));
          frd = min((float)1.,frd/pPl->pRoot->fDepth/(float)10.0);

          fnh4 = min(fnh4up*frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
          fno3 = min(fno3up*frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
		  //fnh4 = min(fnh4up*DeltaZ/frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/frd));
          //fno3 = min(fno3up*DeltaZ/frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/frd));
          //fnh4 = min(fnh4up*frd,max((float)0.,pSLN->fNH4N-fnh4r));
          //fno3 = min(fno3up*frd,max((float)0.,pSLN->fNO3N-fno3r));
          //fnh4 = min(fnh4up/frd,pSLN->fNH4N-fnh4r/frd);
          //fno3 = min(fno3up/frd,pSLN->fNO3N-fno3r/frd);

		  if((pPl->pRoot->fDepth*(float)10)>=(fDepth-DeltaZ))
          {
             pLR->fActLayNH4NUpt = fnh4;
             pLR->fActLayNO3NUpt = fno3; 
             pPltN->fActNH4NUpt += fnh4;
             pPltN->fActNO3NUpt += fno3;
          }
          else
          {
             pLR->fActLayNH4NUpt = (float)0;
             pLR->fActLayNO3NUpt = (float)0; 
          }

          //uptake from soil --> in plant.c
          //pSLN->fNH4N -= pLR->fActLayNH4NUpt;
          //pSLN->fNO3N -= pLR->fActLayNO3NUpt;

		  
          fDepth += DeltaZ;// sg/hmgu

          pSL =pSL->pNext;
          pSLW=pSLW->pNext;
          pSLN=pSLN->pNext;
          pLR =pLR->pNext;
         }//for
      }
//*/
///*
	{
      //+++ output to expertn +++//
	  fpd =(pSo->iLayers-2)*DeltaZ;
	  frd = min(fpd-DeltaZ,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));
	  f2  = max((float)1,(fpd-frd)/DeltaZ);

      pSLN  = pCh->pCLayer->pNext;
      pLR   = pPl->pRoot->pLayerRoot;
      pPltN = pPl->pPltNitrogen;
	  pPltN->fActNH4NUpt = (float)0;
	  pPltN->fActNO3NUpt = (float)0;

	  for (L=1;L<=pSo->iLayers-2;L++)
      {		  	  
       //fnh4 = min(fnh4up*DeltaZ/frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fno3 = min(fno3up*DeltaZ/frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fnh4 = min(fnh4up*DeltaZ/frd,max((float)0.,pSLN->fNH4N));
       //fno3 = min(fno3up*DeltaZ/frd,max((float)0.,pSLN->fNO3N));
       //fnh4 = fnh4up*DeltaZ/frd;
       //fno3 = fno3up*DeltaZ/frd;
       //fnh4 = fnh4up*(pSLN->fNH4N/fnh4p)*iL*DeltaZ/frd;
       //fno3 = fno3up*(pSLN->fNO3N/fno3p)*iL*DeltaZ/frd;
       //fnh4 = min(fnh4up*(pSLN->fNH4N/fnh4p)*fDepth/frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fno3 = min(fno3up*(pSLN->fNO3N/fno3p)*fDepth/frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       fnh4 = min(fnh4up*(pSLN->fNH4N/fnh4p),max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       fno3 = min(fno3up*(pSLN->fNO3N/fno3p),max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       	   
	   //fnh4 = fnh4up*(pSLN->fNH4N/fnh4p);
       //fno3 = fno3up*(pSLN->fNO3N/fno3p);

       if(L*DeltaZ <= frd) 
	   {
        pLR->fActLayNH4NUpt = fnh4;//*DeltaZ/frd;
        pLR->fActLayNO3NUpt = fno3;//*DeltaZ/frd;
	   }
	   else if ((frd < L*DeltaZ)&&(L*DeltaZ <= frd + DeltaZ))
       {
        f1 = (L*DeltaZ-frd)/DeltaZ;
        pLR->fActLayNH4NUpt = (((float)1-f1)*fnh4);//*DeltaZ/frd);
        pLR->fActLayNO3NUpt = (((float)1-f1)*fno3);//*DeltaZ/frd);
	   }
	   else
	   {
		pLR->fActLayNH4NUpt = (float)0;
        pLR->fActLayNO3NUpt = (float)0;
	   }//if,else if,else
	   
	   pPltN->fActNH4NUpt += pLR->fActLayNH4NUpt;
       pPltN->fActNO3NUpt += pLR->fActLayNO3NUpt;

       if(pLR->pNext!=NULL)  pLR =pLR->pNext;
	   if(pSLN->pNext!=NULL) pSLN=pSLN->pNext;

	  }//for
	}//*/



      pPltN->fActNUpt= pPltN->fActNH4NUpt + pPltN->fActNO3NUpt;
      pPltN->fActNUptR=pPltN->fActNUpt;//per day, i.e. dt=1 !

      return 1;    
      }






//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Daily Canopy gross photosynthesis 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: The subroutine calculates daily total gross photosynthesis *
*           and transpiration by performing a Gaussian integration     *
*           over time. At five different times of the day, temperature *
*           and radiation are computed to determine assimilation       *
*           and transpiration whereafter integration takes place.      *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SC      R4  Solar constant                             J m-2 s-1 I  *
*  SINLD   R4  Seasonal offset of sine of solar height    -         I  *
*  COSLD   R4  Amplitude of sine of solar height          -         I  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)    h         I  *
*  DSINBE  R4  Daily total of effective solar height      s d-1     I  *
*  DDTR    R4  Daily global radiation                     J m-2 d-1 I  *
*  TMAX    R4  Daily maximum temperature                  oC        I  *
*  TMIN    R4  Daily minimum temperature                  oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  WNM     R4  daily average wind speed (>=0.1 m/s)       m s-1     I  *
*  C3C4    R4  Crop type (=1 for C3, -1 for C4 crops)     -         I  *
*  LAI     R4  (green)Leaf area index                     m2 m-2    I  *
*  TLAI    R4  Total Leaf area index                      m2 m-2    I  *
*  HT      R4  Plant height                               m         I  *
*  LWIDTH  R4  Leaf width                                 m         I  *
*  RD      R4  Rooting depth                              cm        I  *
*  SD1     R4  Depth of evaporative upper soil layer      cm        I  *
*  RSS     R4  Soil resistance,equivalent to leaf stomata s m-1     I  *
*  BLD     R4  Leaf angle from horizontal                 degree    I  *
*  KN      R4  Leaf nitrogen extinction coefficient       m2 m-2    I  *
*  KW      R4  Windspeed extinction coefficient in canopy m2 m-2    I  *
*  SLN     R4  Average leaf nitrogen content in canopy    g m-2     I  *
*  SLNT    R4  Top-leaf nitrogen content                  g m-2     I  *
*  SLNN    R4  Value of SLNT with small plant-N increment g m-2     I  *
*  SLNMIN  R4  Minimum or base SLNT for photosynthesis    g m-2     I  *
*  DWSUP   R4  Daily water supply for evapotranspiration  mm d-1    I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  LS      R4  Lodging severity                           -         I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax & leaf N   umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  WCUL    R4  Water content of the upper soil layer      m3 m-3    I  *
*  FVPD    R4  Slope for linear effect of VPD on Ci/Ca    (kPa)-1   I  *
*  PPCAN   R4  Potential canopy CO2 assimilation          g m-2 d-1 O  *
*  APCANS  R4  Actual standing-canopy CO2 assimilation    g m-2 d-1 O  *
*  APCANN  R4  APCANS with small plant-N increment        g m-2 d-1 O  *
*  APCAN   R4  Actual canopy CO2 assimilation             g m-2 d-1 O  *
*  PTCAN   R4  Potential canopy transpiration             mm d-1    O  *
*  ATCAN   R4  Actual canopy transpiration                mm d-1    O  *
*  PESOIL  R4  Potential soil evaporation                 mm d-1    O  *
*  AESOIL  R4  Actual soil evaporation                    mm d-1    O  *
*  DIFS    R4  Daytime average soil-air temp. difference  oC        O  *
*  DIFSU   R4  Daytime aver. sunlit leaf-air temp. diff.  oC        O  *
*  DIFSH   R4  Daytime aver. shaded leaf-air temp. diff.  oC        O  *
*  DAPAR   R4  Daily PAR absorbed by crop canopy          J m-2 d-1 O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double DailyCanopyGrossPhotosynthesis_GECROS(gecros *self,double SC,double SINLD,double COSLD,double DAYL,double DSINBE,
          double DDTR,double TMPA,double HOUR,double DVP,double WNM,double C3C4,double LAI,double TLAI,
            double HT,double LWIDTH,double RD,double SD1,double RSS,double BLD,double NLV,double TNLV,
            double SLNMIN,double DWSUP,double CO2A,double LS,double EAJMAX,double XVN,double XJN,double THETA,
            double WCUL,double FVPD,double *PPCAN,double *APCANS,double *APCANN,double *APCAN,double *PTCAN,
            double *ATCAN,double *PESOIL,double *AESOIL,double *DIFS,double *DIFSU,double *DIFSH,double *DAPAR)
      {
      //Input: SC,SINLD,COSLD,DAYL,DSINBE, s.o. und Astronomy; 
      //Input: DDTR=pWth->fSolRad,TMAX=pWth->fTempMax,TMIN=pWth->fTempMin, DVP aus pCl->pWeather->fHumidity;
      //Input: WNM=pWth->fWindSpeed,C3C4 Param,LAIC=pGCan->fLAIGreen,TLAI=pGCan->fLAITotal,
      //Input: HT=pPl->pCanopy->fPlantHeight,LWIDTH Param,RD=pPl->pRoot->fDepth;
      //Input: SD1 Param,RSS Param,BLD Param;
      //Input: KN,KW: lokal eingefügt ---> s.u. ep170407 
      //Input: SLN,SLNT,SLNNT: lokal eingefügt ---> s.u. ep190407
      //Input: SLNMIN Param;
      //Input: DWSUP soil water supply ---> to do !
      //Input: CO2A Param, LS lodging severity Param =INSW(LODGE, (double)0., AFGEN (VLS,DS));
      //Input: EAJMAX Param, XVN Param, XJN Param, THETA Param;
      //Input: WCUL water content upper soil layer ---> to do !
      //Input: FVPD Param = INSW(C3C4, 0.195127, 0.116214);

      //Output: PPCAN,APCANS,APCANN,APCAN,PTCAN,ATCAN,PESOIL,AESOIL,DIFS,DIFSU,DIFSH,DAPAR
	 // expertn_modul_base *xpn = &(self->parent);//Hong
      //int     i,j;
      //double  SUNRIS,HOUR;
      double  KBPPAR,KBPNIR;
	  double  PAR,NIR,ATMTR,FRDF,PARDF,PARDR,NIRDF,NIRDR,NRADS;
      double  SINB,DTR,DAYTMP,WSUP,WSUP1,WND,PSPAR,PSNIR;
      double  BL,KB,SCPPAR,SCPNIR,KDPPAR,KDPNIR,PCBPAR,PCBNIR,PCDPAR,PCDNIR;
      double  RT,RTS,KW,KN,NBK,SLNT,NPSU,NPSH,SLNN,NPSUN,NPSHN;
      double  APARSU,APARSH,ANIRSU,ANIRSH,APAR,ATRJSU,ATRJSH,ATRJS;
      
      //%---boundary layer resistance for canopy, sunlit and shaded leaves
      double  FRSU,FRSH,GBHLF,GBHC,GBHSU,GBHSH,RBHSU,RBWSU,RBHSH,RBWSH,RBHS,RBWS;

      double  PLFSU,PTSU,RSWSU,NRADSU,SLOPSU;
      double  PLFSH,PTSH,RSWSH,NRADSH,SLOPSH,IPP,IPT,PT1,IPE;
      double  IAE,IAT,ATSU,ATSH,ADIFSU,ADIFSH,ADIFS,IAPS,IAPNN,IAP;

      double  ACO2I,IPPL,SLN,IRDL,ARSWSU,IAPL,IAPN;
      double  PASSU,PANSU,PASSH,PANSH;
      ///*
      //int     nGauss=5;
      //double  xGauss[]={0.0469101, 0.2307534, 0.5000000, 0.7692465, 0.9530899};
      //double  wGauss[]={0.1184635, 0.2393144, 0.2844444, 0.2393144, 0.1184635};
    
      //%---output-variables set to zero and five different times of a day(HOUR)
        *PPCAN  = 0.;
        *APCANS = 0.;
        *APCANN = 0.;
        *APCAN  = 0.;
        *PTCAN  = 0.;
        *ATCAN  = 0.;
        *PESOIL = 0.;
        *AESOIL = 0.;
        *DIFS   = 0.;
        *DIFSU  = 0.;
        *DIFSH  = 0.;
        *DAPAR  = 0.;

        /*
        for (i=1;i<=nGauss;i++)
        {
         j=i-1;

         //%---timing for sunrise
         SUNRIS = 12.0 - 0.5*DAYL;
         */
		 //SUNRIS = 12.0 - 0.5*DAYL;
         //%---specifying the time (HOUR) of a day
         //(at HOUR, radiation is computed and used to compute assimilation)
		 //HOUR=dayTime-SUNRIS;
         //HOUR = SUNRIS+DAYL*xGauss[j];
	     //HOUR *= (double)24.0;
	         
         //%---sine of solar elevation
         //SINB  = max(0.0, SINLD+COSLD*cos(2.0*PI*(HOUR-12.0)/24.0));
		 SINB  = max(1E-12, SINLD+COSLD*cos(2.0*PI*(HOUR-12.0)/24.0));//Hong
    
         //%---daytime course of radiation  [J m-2 d-1] --> [J m-2 s-1]
         //Begin of Hong
		// DTR   = DDTR*(SINB*SC/1367.)/DSINBE; //original
		 //DTR   = xpn->pCl->pWeather->fGlobalStrahlung*1E6*(SINB*SC/1367.)/DSINBE;//the calculted fSol_Rad not used		
		// DTR = DDTR/24/3600 * (self->DSINB/DSINBE);
          DTR   = DDTR*1E6*(SINB*SC/1367.)/DSINBE;		
		 //End of Hong
		 
		 //C_DEBUG(DTR);
			
         //%---daytime course of air temperature 
		 double TMAX, TMIN;
		 TMAX = (double) self->weather.fTempMax;     //[°C]
         TMIN = (double) self->weather.fTempMin;     //[°C]	
         DAYTMP= TMIN+(TMAX-TMIN)*sin(PI*(HOUR+DAYL/2.-12.)/(DAYL+3.));
        // DAYTMP= (DAYTMP +TMPA)/2;//Test of Hong
		// DAYTMP= TMPA;
         
	     //%---daytime course of water supply  [mm d-1] --> [mm s-1]
         WSUP  = DWSUP*(SINB*SC/1367.)/DSINBE;
		// WSUP  = DWSUP/(24*3600);//Test of Hong
         WSUP1 = WSUP*SD1/RD;
         WSUP1 = min(WSUP,WSUP1);
         //%---daytime course of wind speed
         WND   = WNM;   //!no diurnal fluctuation is assumed here

         //%---total incoming PAR and NIR
         PAR   = 0.5*DTR;
         NIR   = 0.5*DTR;

        
         //%---diffuse light fraction (FRDF) from atmospheric transmission (ATMTR)
 /*        ATMTR = PAR/(0.5*SC*SINB);
             
         if (ATMTR<=0.22) 
           FRDF = 1.0;
         else
         {
          if ((ATMTR>0.22)&&(ATMTR<=0.35)) 
            FRDF = 1.0-6.4*(ATMTR-0.22)*(ATMTR-0.22); 
          else
            FRDF = 1.47-1.66*ATMTR;
         }
                  
         FRDF = max (FRDF, 0.15+0.85*(1.0-exp(-0.1/SINB)));
            
            
         //%---incoming diffuse PAR (PARDF) and direct PAR (PARDR)
         PARDF = PAR * FRDF;
         PARDR = PAR - PARDF;
*/
         
         //Hong: after SPASS
		 if (SINB > 0.0)
		{
		ATMTR  	= PAR/(0.5*SC*SINB);
		ATMTR = max(min(ATMTR,1.0),0.0);		

		if (ATMTR<=0.22) 
			FRDF = 1.0;
		else
			{
			if ((ATMTR>0.22)&&(ATMTR<=0.35)) 
				FRDF = 1.0-6.4*(ATMTR-0.22)*(ATMTR-0.22); 
			else
				FRDF = 1.47-1.66*ATMTR;
			}
			
		FRDF = max (FRDF, 0.15+0.85*(1.0-exp(-0.1/SINB)));
		
		//Diffuse PAR (PARDF) and direct PAR (PARDR)
		//PARDF = min(PAR, SINB*FRDF*ATMTR*0.5*SC);
		PARDF = min(PAR, FRDF*PAR);
		}
		else
		{
			PARDF = PAR;
		}
		PARDR = PAR-PARDF;
		//End of Test

         //%---incoming diffuse NIR (NIRDF) and direct NIR (NIRDR)
         NIRDF = NIR * FRDF;
         NIRDR = NIR - NIRDF;



         //%---extinction and reflection coefficients
         BL    = BLD*PI/(double)180.;   //leaf angle, conversion to radians

         KB    = kbeam(SINB,BL);

         SCPPAR = (double)0.2;           //leaf scattering coefficient for PAR
         SCPNIR = (double)0.8;           //leaf scattering coefficient for NIR
      
         KDPPAR = kdiff(TLAI,BL,SCPPAR);
         KDPNIR = kdiff(TLAI,BL,SCPNIR);

         KBPPAR = KB*sqrt((double)1.-SCPPAR);//scattered beam radiation extinction coefficient for PAR
         KBPNIR = KB*sqrt((double)1.-SCPNIR);//scattered beam radiation extinction coefficient for NIR
         
         PCBPAR = refl(SCPPAR,KB);//canopy beam radiation PAR reflection coefficient
         PCBNIR = refl(SCPNIR,KB);//canopy beam radiation NIR reflection coefficient

         PCDPAR = (double)0.057;          //canopy diffuse PAR reflection coefficient
         PCDNIR = (double)0.389;          //canopy diffuse NIR reflection coefficient

         //%---turbulence resistance for canopy (RT) and for soil (RTS)
         RT     = (double)0.74*pow(log((2.-0.7*HT)/(0.1*HT)),(double)2)/(pow((double)0.4,(double)2)*WND);
         RTS    = (double)0.74*pow(log(56.),(double)2)/(pow((double)0.4,(double)2)*WND);

         //%---fraction of sunlit and shaded components in canopy
         FRSU   = 1./KB/LAI*(1.- exp(-KB*LAI));
         FRSH   = 1.-FRSU;

         //Input: Extinction coefficient of nitrogen and wind ep170407
         KW     = kdiff(TLAI,BLD*3.141592654/180.,0.2);
            
         KN     = KW*(TNLV-SLNMIN*TLAI);
         NBK    = SLNMIN*(1.-exp(-KW*TLAI));

         KN     = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));

         //Input: Leaf N-content ep190407
         SLNT   = NLV*KN/((double)1.-exp(-KN*LAI));
         SLNN   = (NLV+(double)0.001*NLV)*KN/((double)1.-exp(-KN*LAI));
		// SLNN   = (NLV+(double)0.001*NLV*0.1)*KN/((double)1.-exp(-KN*LAI)); //Test of Hong
         SLN    = NLV/LAI;
         

         //%---boundary layer resistance for canopy, sunlit and shaded leaves
         GBHLF  = 0.01*sqrt(WND/LWIDTH);
         GBHC   = (1.-exp(- 0.5*KW    *LAI))/(0.5*KW   )*GBHLF;
         GBHSU  = (1.-exp(-(0.5*KW+KB)*LAI))/(0.5*KW+KB)*GBHLF;
         GBHSH  = GBHC - GBHSU;

         RBHSU  = 1./GBHSU;    //boundary layer resistance to heat,sunlit part
         RBWSU  = 0.93*RBHSU;  //boundary layer resistance to H2O, sunlit part
         RBHSH  = 1./GBHSH;    //boundary layer resistance to heat,shaded part
         RBWSH  = 0.93*RBHSH;  //boundary layer resistance to H2O, shaded part

         //%---boundary layer resistance for soil
         RBHS   = 172.*sqrt(0.05/max(0.1,WND*exp(-KW*TLAI)));
         RBWS   = 0.93*RBHS;

         //%---photosynthetically active nitrogen for sunlit and shaded leaves
         NPSU   = pan(SLNT,SLNMIN,LAI,KN,KB,0);
         NPSH   = pan(SLNT,SLNMIN,LAI,KN,KB,1);
         NPSUN  = pan(SLNN,SLNMIN,LAI,KN,KB,0);
         NPSHN  = pan(SLNN,SLNMIN,LAI,KN,KB,1);
         //%---absorbed PAR and NIR by sunlit leaves and shaded leaves
         APARSU = ligab(SCPPAR,KB,KBPPAR,KDPPAR,PCBPAR,PCDPAR,PARDR,PARDF,LAI,0);
         APARSH = ligab(SCPPAR,KB,KBPPAR,KDPPAR,PCBPAR,PCDPAR,PARDR,PARDF,LAI,1);
         ANIRSU = ligab(SCPNIR,KB,KBPNIR,KDPNIR,PCBNIR,PCDNIR,NIRDR,NIRDF,LAI,0);
         ANIRSH = ligab(SCPNIR,KB,KBPNIR,KDPNIR,PCBNIR,PCDNIR,NIRDR,NIRDF,LAI,1);
         APAR   = APARSU+APARSH;

         //%---absorbed total radiation (PAR+NIR) by sunlit and shaded leaves
         ATRJSU = APARSU+ANIRSU;
         ATRJSH = APARSH+ANIRSH;

         //%---absorbed total radiation (PAR+NIR) by soil
         PSPAR  = 0.1;//!soil PAR reflection
         PSNIR  = INSW(WCUL-(double)0.5,(double)0.52-(double)0.68*WCUL,(double)0.18);//!soil NIR reflection
         ATRJS  = (1.-PSPAR)*(PARDR*exp(-KBPPAR*TLAI)+PARDF*exp(-KDPPAR*TLAI))
                 +(1.-PSNIR)*(NIRDR*exp(-KBPNIR*TLAI)+NIRDF*exp(-KDPNIR*TLAI));

         //%---instantaneous potential photosynthesis and transpiration
         PotentialLeafPhotosynthesis_GECROS(FRSU,DAYTMP,DVP,CO2A,C3C4,FVPD,APARSU,NPSU,RBWSU,RBHSU,
                                            RT*FRSU,ATRJSU,ATMTR,EAJMAX,XVN,XJN,THETA, 
                                            &PLFSU,&PTSU,&RSWSU,&NRADSU,&SLOPSU);

         PotentialLeafPhotosynthesis_GECROS(FRSH,DAYTMP,DVP,CO2A,C3C4,FVPD,APARSH,NPSH,RBWSH,RBHSH,
                                           RT*FRSH,ATRJSH,ATMTR,EAJMAX,XVN,XJN,THETA, 
                                           &PLFSH,&PTSH,&RSWSH,&NRADSH,&SLOPSH);
         IPP    = PLFSU+ PLFSH;
         IPT    = PTSU + PTSH;
         PT1    = IPT  * SD1/RD;

         //%---instantaneous potential soil evaporation 
         IPE    = PotentialEvaporation_GECROS(DAYTMP,DVP,RSS,RTS,RBWS,RBHS,ATRJS,ATMTR,PT1,WSUP1,&NRADS);
		 
		   //Test of Hong
	   if (self->LAI> 3.0)
		   IPE= 0.0;
	   //End of TESt

         //%---instantaneous actual soil evaporation, actual canopy transpiration and photosynthesis
         IAE    = min(IPE,IPE/(PT1+IPE)*WSUP1);
         IAT    = min(IPT,PT1/(PT1+IPE)*WSUP1+WSUP-WSUP1);
         ATSU   = PTSU/IPT*IAT;
         ATSH   = PTSH/IPT*IAT;

         ADIFS = difla(NRADS,IAE,RBHS,RTS);
      
         ActualLeafPhotosynthesis_GECROS(DAYTMP,APARSU,DVP,CO2A,C3C4,FVPD,NRADSU,ATSU,PTSU,
                                         RT*FRSU,RBHSU,RBWSU,RSWSU,SLOPSU,NPSU,NPSUN,
                                         EAJMAX,XVN,XJN,THETA, &PASSU,&PANSU,&ADIFSU);
         ActualLeafPhotosynthesis_GECROS(DAYTMP,APARSH,DVP,CO2A,C3C4,FVPD,NRADSH,ATSH,PTSH,
                                        RT*FRSH,RBHSH,RBWSH,RSWSH,SLOPSH,NPSH,NPSHN,
                                        EAJMAX,XVN,XJN,THETA, &PASSH,&PANSH,&ADIFSH);
         IAPS   = PASSU + PASSH;
         IAPN   = PANSU + PANSH;

         //%---canopy photosynthesis if there is lodging
         ACO2I  = internalCO2(DAYTMP+ADIFSU,DVP,FVPD,CO2A,C3C4);
         //ASVP = noetig???
         IPPL   = photo(C3C4,((double)1.-SCPPAR)*PAR,DAYTMP+ADIFSU,ACO2I,SLN-SLNMIN,EAJMAX,XVN,XJN,THETA);
         IRDL   = darkr(DAYTMP+ADIFSU,SLN-SLNMIN,XVN);
         ARSWSU = (PTSU-ATSU)*(SLOPSU*(RBHSU+RT*FRSU)+(double).067*(RBWSU+RT*FRSU))/ATSU/(double).067
                  +PTSU/ATSU*RSWSU;
         IAPL   = (((double)1.6*RSWSU+(double)1.3*RBWSU+RT*FRSU)/((double)1.6*ARSWSU+1.3*RBWSU+RT*FRSU)
                   *(IPPL-IRDL)+IRDL)*((double)1.-exp(-LAI));
         IAP    = min(IAPS, (1.-LS)*IAPS+LS*IAPL);
         IAPNN  = min(IAPN, (1.-LS)*IAPN+LS*IAPL*IAPN/IAPS);
		 
		 if (LS>0.0)
		 {
			 int Hong;
			 Hong =1;
		 }
                  
      /*
          //---integration of assimilation rate and transpiration rate to a daily total
         *PPCAN  = *PPCAN  + IPP   * wGauss[j];
         *APCANS = *APCANS + IAPS  * wGauss[j];
         *APCANN = *APCANN + IAPNN * wGauss[j];
         *APCAN  = *APCAN  + IAP   * wGauss[j];
         *PTCAN  = *PTCAN  + IPT   * wGauss[j];
         *ATCAN  = *ATCAN  + IAT   * wGauss[j];
         *PESOIL = *PESOIL + IPE   * wGauss[j];
         *AESOIL = *AESOIL + IAE   * wGauss[j];
         *DIFS   = *DIFS   + ADIFS * wGauss[j];
         *DIFSU  = *DIFSU  + ADIFSU* wGauss[j];
         *DIFSH  = *DIFSH  + ADIFSH* wGauss[j];
         *DAPAR  = *DAPAR  + APAR  * wGauss[j];

           //DTGA = DTGA+FGROS*wGauss[j];

      }//for

      *PPCAN  = *PPCAN  * DAYL * (double)3600.;
      *APCANS = *APCANS * DAYL * (double)3600.;
      *APCANN = *APCANN * DAYL * (double)3600.;
      *APCAN  = *APCAN  * DAYL * (double)3600.;
      *PTCAN  = *PTCAN  * DAYL * (double)3600.;
      *ATCAN  = *ATCAN  * DAYL * (double)3600.;
      *PESOIL = *PESOIL * DAYL * (double)3600.;
      *AESOIL = *AESOIL * DAYL * (double)3600.;
      *DIFS   = *DIFS;
      *DIFSU  = *DIFSU;
      *DIFSH  = *DIFSH;
      *DAPAR  = *DAPAR  * DAYL * (double)3600.;
          
      //DTGA = DTGA * DAYL;

      */ 
	
	   
       *PPCAN  = IPP   * (double)24 * (double)3600; // [mm s-1] --> [mm d-1]
       *APCANS = IAPS  * (double)24 * (double)3600; // [mm s-1] --> [mm d-1]
       *APCANN = IAPNN * (double)24 * (double)3600; // [mm s-1] --> [mm d-1]
       *APCAN  = IAP   * (double)24 * (double)3600; // [mm s-1] --> [mm d-1]
      /* *PTCAN  = IPT   * DAYL * (double)3600; // [mm s-1] --> [mm d-1]
       *ATCAN  = IAT   * DAYL * (double)3600; // [mm s-1] --> [mm d-1]
       *PESOIL = IPE   * DAYL * (double)3600; // [mm s-1] --> [mm d-1]
       *AESOIL = IAE   * DAYL * (double)3600; // [mm s-1] --> [mm d-1] */
	   
	   *PTCAN  = IPT   * (double)24  * (double)3600; // [mm s-1] --> [mm d-1]
       *ATCAN  = IAT   * (double)24  * (double)3600; // [mm s-1] --> [mm d-1]
       *PESOIL = IPE   * (double)24  * (double)3600; // [mm s-1] --> [mm d-1]
       *AESOIL = IAE   * (double)24  * (double)3600; // [mm s-1] --> [mm d-1]
         
	   *DIFS   = ADIFS;
       *DIFSU  = ADIFSU;
       *DIFSH  = ADIFSH;
       *DAPAR  = APAR  * (double)24 * (double)3600; // [mm s-1] --> [mm d-1]
	   	  
     return *APCAN;//DTGA;
    }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// kdiff
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*----------------------------------------------------------------------*
*  Purpose: This subroutine calculates extinction coefficient for      *
*           diffuse radiation.                                         *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  LAI     R4  Total leaf area index                      m2 m-2    I  *
*  BL      R4  Leaf angle (from horizontal)               radians   I  *
*  SCP     R4  Leaf scattering coefficient                -         I  *
*  KDP     R4  Diffuse radiation extinction coefficient   m2 m-2    O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double kdiff(double vLAI, double BL, double SCP)
      {
        //PI    = 3.141592654
        double KB15,KB45,KB75,KDP;

         //%---extinction coefficient of beam lights from 15, 45 and 75o elevations
         KB15 = (double)kbeam(sin(15.*PI/180.),BL);
         KB45 = (double)kbeam(sin(45.*PI/180.),BL);
         KB75 = (double)kbeam(sin(75.*PI/180.),BL);

         //%---diffuse light extinction coefficient
         KDP   = (double)-1./vLAI*log((double)0.178*exp(-KB15*pow((double)1.-SCP,(double)0.5)*vLAI)
                         +(double)0.514*exp(-KB45*pow((double)1.-SCP,(double)0.5)*vLAI)
                         +(double)0.308*exp(-KB75*pow((double)1.-SCP,(double)0.5)*vLAI));

       return KDP;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// kbeam
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*----------------------------------------------------------------------*
*  Purpose: This subroutine calculates extinction coefficient for      *
*           direct beam radiation.                                     *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SINB    R4  Sine of solar elevation                    -         I  *
*  BL      R4  Leaf angle (from horizontal)               radians   I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    O  *
*----------------------------------------------------------------------*/
double kbeam(double SINB,double BL)
      {
       double B,OAV,KB;

        //%---solar elevation in radians
        B      = asin(NOTNUL(SINB));

       //%---average projection of leaves in the direction of a solar beam
       if (SINB >= sin(BL))
          OAV = SINB*cos(BL);
       else
          OAV = (double)2./(double)3.141592654*(SINB*cos(BL)*asin(tan(B)/tan(BL))
               + pow(pow(sin(BL),(double)2)-pow(SINB,(double)2),(double)0.5));

       //%---beam radiation extinction coefficient
       KB     = OAV/NOTNUL(SINB);

       return KB;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// refl
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates reflection coefficients.        *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SCP     R4  Leaf scattering coefficient                -         I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    I  *
*  KBP     R4  Scattered beam radiation extinction coeff. m2 m-2    O  *
*  PCB     R4  Canopy beam radiation reflection coeff.    -         O  *
*----------------------------------------------------------------------*/
double refl(double SCP, double KB)
      {
       double PH,PCB;

        //%---canopy reflection coefficient for horizontal leaves
        PH  = ((double)1.-sqrt((double)1.-SCP))/((double)1.+sqrt((double)1.-SCP));

       //%---Canopy beam radiation reflection coefficient
        PCB = (double)1.-exp((double)-2.*PH*KB/((double)1.+KB));

       return PCB;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ligab
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates absorbed light for sunlit and   *
*           shaded leaves.                                             *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SCP     R4  Leaf scattering coefficient                -         I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    I  *
*  KBP     R4  Scattered beam radiation extinction coeff. m2 m-2    I  *
*  KDP     R4  Diffuse radiation extinction coefficient   m2 m-2    I  *
*  PCB     R4  Canopy beam radiation reflection coeff.    -         I  *
*  PCD     R4  Canopy diffuse radiation reflection coeff. -         I  *
*  IB0     R4  Incident direct-beam radiation             J m-2 s-1 I  *
*  ID0     R4  Incident diffuse radiation                 J m-2 s-1 I  *
*  LAI     R4  (green)Leaf area index                     m2 m-2    I  *
*  ISU     R4  Absorbed radiation by sunlit leaves        J m-2 s-1 O  *
*  ISH     R4  Absorbed radiation by shaded leaves        J m-2 s-1 O  *
*----------------------------------------------------------------------*/
double ligab(double SCP,double KB,double KBP,double KDP,double PCB,double PCD, 
             double IB0,double ID0,double LAI, int IW)
      {
       double IC,ISU,ISH,RV;
        //%---total absorbed light by canopy
        IC     = (1.-PCB)*IB0*(1.-exp(-KBP*LAI))
                +(1.-PCD)*ID0*(1.-exp(-KDP*LAI));

       //%---absorbed light by sunlit and shaded fractions of canopy
        ISU    = (1.-SCP)*IB0*(1.-exp(-KB *LAI))+(1.-PCD)*ID0/(KDP+KB)*
                 KDP*(1.-exp(-(KDP+KB)*LAI))+IB0*((1.-PCB)/(KBP+KB)*KBP*
                 (1.-exp(-(KBP+KB)*LAI))-(1.-SCP)*(1.-exp(-2.*KB*LAI))/2.);
        ISH    = IC-ISU;

      if (IW==0) RV = ISU;
      else       RV = ISH;

       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// pan
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates photosynthetically active       *
*           nitrogen content for sunlit and shaded parts of canopy.    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SLNT    R4  Top-leaf nitrogen content                  g m-2     I  *
*  SLNMIN  R4  Minimum or base SLNT for photosynthesis    g m-2     I  *
*  LAI     R4  (green)Leaf area index                     m2 m-2    I  *
*  KN      R4  Leaf nitrogen extinction coefficient       m2 m-2    I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    I  *
*  NPSU    R4  Photosynthet. active N for sunlit leaves   g m-2     O  *
*  NPSH    R4  Photosynthet. active N for shaded leaves   g m-2     O  *
*----------------------------------------------------------------------*/
double pan(double SLNT, double SLNMIN, double LAI, double KN, double KB, int IW)
      {
       double NPC,NPSU,NPSH,RV;
       //%---total photosynthetic nitrogen in canopy
       NPC   = SLNT*(1.-exp(-KN*LAI))/KN-SLNMIN*LAI;

       //%---photosynthetic nitrogen for sunlit and shaded parts of canopy
       NPSU  = SLNT*(1.-exp(-(KN+KB)*LAI))/(KN+KB)
              -SLNMIN*(1.-exp(-KB*LAI))/KB;
       NPSH  = NPC-NPSU;

       if (IW==0) RV = NPSU;
       else       RV = NPSH;

       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// difla
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates leaf(canopy)-air temperature    *
*           differential.                                              *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 I  *
*  PT      R4  Potential leaf transpiration               mm s-1    I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  DIF     R4  Leaf-air temperature difference            oC        O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double difla(double NRADC,double PT,double RBH,double RT)
      {
       double DIF;

       double LHVAP  = 2.4e+6; //latent heat of water vaporization (J/kg)
       double VHCA   = 1200.;  //volumetric heat capacity (J/m3/oC)

        DIF = LIMIT(-25., 25., (NRADC-LHVAP*PT)*(RBH+RT)/VHCA);

       return DIF;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Potential Leaf Photosynthesis
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates potential leaf photosynthesis    *
*          and transpiration.                                          *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  FRAC    R4  Fraction of leaf classes (sunlit vs shaded) -        I  *
*  DAYTMP  R4  Air temperature                            oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  C3C4    R4  Crop type (=1. for C3, -1 for C4 crops)    -         I  *
*  FVPD    R4  Slope for linear effect of VPD on Ci/Ca    (kPa)-1   I  *
*  PAR     R4  Absorbed photosynth. active radiation      J m-2 s-1 I  *
*  NP      R4  Photosynthetically active N content        g m-2     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  ATRJ    R4  Absorbed global radiation                  J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax  & leaf N  umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  PLF     R4  Potential leaf photosynthesis              gCO2/m2/s O  *
*  PT      R4  Potential leaf transpiration               mm s-1    O  *
*  RSW     R4  Potential stomatal resistance to water     s m-1     O  *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 O  *
*  SLOPEL  R4  Slope of saturated vapour pressure curve   kPa oC-1  O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int  PotentialLeafPhotosynthesis_GECROS(double FRAC,double DAYTMP,double DVP,double CO2A,double C3C4,
                                double FVPD,double PAR,double NP,double RBW,double RBH,double RT,
                                        double ATRJ,double ATMTR,double EAJMAX,double XVN,double XJN,double THETA, 
                                         double *PLF,double *PT,double *RSW,double *NRADC,double *SLOPEL)
    {
     double SVP,SVPL,VPD,SLOPE,FRSW,FPLF,FLRD,FNRADC,FPT,FDIF,TLEAF,CO2I,FCO2I,LRD;
      
        //%---first-round calculation to determine leaf temperature
      FCO2I   = internalCO2(DAYTMP,DVP,FVPD,CO2A,C3C4);
      FPLF    = photo(C3C4,PAR,DAYTMP,FCO2I,NP,EAJMAX,XVN,XJN,THETA);
      FLRD    = darkr(DAYTMP,NP,XVN);

      SVP     = (double)0.611*exp((double)17.4*DAYTMP/(DAYTMP+(double)239.));
      VPD     = max((double)0., SVP-DVP);
      SLOPE   = (double)4158.6*SVP/pow(DAYTMP+(double)239.,(double)2);

      FRSW    = gcrsw(FPLF,FLRD,DAYTMP,CO2A,FCO2I,RBW,RT);
      FNRADC  = NetLeafAbsRad(ATRJ,ATMTR,FRAC,DAYTMP,DVP);
      FPT     = PotEvaporTransp_GECROS(FRSW,RT,RBW,RBH,ATRJ,ATMTR,FRAC,DAYTMP,DVP,SLOPE,VPD);
      FDIF    = difla(FNRADC,FPT,RBH,RT); 

      TLEAF   = DAYTMP + FDIF;

      //%---second-round calculation to determine potential photosynthesis and transpiration
      CO2I    = internalCO2(TLEAF,DVP,FVPD,CO2A,C3C4);
      *PLF    = photo(C3C4,PAR,TLEAF,CO2I,NP,EAJMAX,XVN,XJN,THETA);
      LRD     = darkr(TLEAF,NP,XVN);

      SVPL    = (double)0.611*exp((double)17.4*TLEAF/(TLEAF+(double)239.));
      *SLOPEL = (SVPL-SVP)/NOTNUL(TLEAF-DAYTMP);

      *RSW    = gcrsw(*PLF,LRD,TLEAF,CO2A,CO2I,RBW,RT);

      *NRADC  = NetLeafAbsRad(ATRJ,ATMTR,FRAC,TLEAF,DVP);
      *PT     = PotEvaporTransp_GECROS(*RSW,RT,RBW,RBH,ATRJ,ATMTR,FRAC,TLEAF,DVP,*SLOPEL,VPD);

     return 1;
    }

      
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Potential Evaporation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* SUBROUTINE PEVAP                                                     *
* Purpose: This subroutine calculates potential soil evaporation.      *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DAYTMP  R4  Air temperature                            oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  RSS     R4  Soil resistance,equivalent to leaf stomata s m-1     I  *
*  RTS     R4  Turbulence resistance for soil             s m-1     I  *
*  RBWS    R4  Soil boundary layer resistance to water    s m-1     I  *
*  RBHS    R4  Soil boundary layer resistance to heat     s m-1     I  *
*  ATRJS   R4  Absorbed global radiation by soil          J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  PT1     R4  Potential leaf transpiration using water   mm s-1    I  *
*              from upper evaporative soil layer                       *
*  WSUP1   R4  Water supply from upper evaporative soil   mm s-1    I  *
*              layer for evapotranspiration                            *
*  PESOIL  R4  Potential soil evaporation                 mm s-1    O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double PotentialEvaporation_GECROS(double DAYTMP,double DVP,double RSS,double RTS,double RBWS,double RBHS,
                                   double ATRJS,double ATMTR,double PT1,double WSUP1, double *NRADS)
      {
       double SVP,VPD,SLOPE,SVPS,SLOPES,FPESOL,FAESOL,FDIFS,TAVS;
       double FPE,FNRADS,PE,PESOIL;
        //%--- first-round calculation to estimate soil surface temperature (TAVS)
        SVP    = (double)0.611*exp((double)17.4*DAYTMP/(DAYTMP+(double)239.));
        VPD    = max((double)0., SVP-DVP);
        SLOPE  = (double)4158.6 * SVP/pow(DAYTMP + (double)239.,(double)2);
        FNRADS = NetLeafAbsRad(ATRJS,ATMTR,1.,DAYTMP,DVP);
        FPE    = PotEvaporTransp_GECROS(RSS,RTS,RBWS,RBHS,ATRJS,ATMTR,1.,DAYTMP,DVP,SLOPE,VPD);
        FPESOL = max((double)0., FPE);
        FAESOL = min(FPESOL,FPESOL/(PT1+FPESOL)*WSUP1);
        FDIFS  = difla(FNRADS,FAESOL,RBHS,RTS);
        TAVS   = DAYTMP + FDIFS;

        //%---second-round calculation to estimate potential soil evaporation
        SVPS   = (double)0.611*exp((double)17.4*TAVS/(TAVS+(double)239.));
        SLOPES = (SVPS-SVP)/NOTNUL(FDIFS);

        *NRADS = NetLeafAbsRad(ATRJS,ATMTR,1.,TAVS,DVP);
        PE     = PotEvaporTransp_GECROS(RSS,RTS,RBWS,RBHS,ATRJS,ATMTR,1.,TAVS,DVP,SLOPES,VPD);
        PESOIL = max((double)0., PE);

       return PESOIL;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Potential Evapotranspiration
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates leaf transpiration, using the    *
*          Penman-Monteith equation                                    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  RSW     R4  Potential stomatal resistance to water     s m-1     I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  ATRJ    R4  Absorbed global radiation                  J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  FRAC    R4  Fraction of leaf classes (sunlit vs shaded)-         I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  SLOPE   R4  Slope of saturated vapour pressure curve   kPa oC-1  I  *
*  VPD     R4  Saturation vapour pressure deficit of air  kPa       I  *
*  PT      R4  Potential leaf transpiration               mm s-1    O  *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double PotEvaporTransp_GECROS(double RSW,double RT,double RBW,double RBH,double ATRJ,double ATMTR,
                              double FRAC,double TLEAF,double DVP,double SLOPE,double VPD)
      { 
       double RV,PT,NRADC,PSR,PTR,PTD;

        //%---some physical constants
       double LHVAP  = 2.4E+6;    //latent heat of water vaporization(J/kg)
       double VHCA   = 1200.;     //volumetric heat capacity (J/m3/oC)
       double PSYCH  = 0.067;     //psychrometric constant (kPa/oC)

        // *---net absorbed radiation
        NRADC  = NetLeafAbsRad(ATRJ,ATMTR,FRAC,TLEAF,DVP);

        //%---intermediate variable related to resistances
        PSR    = PSYCH*(RBW+RT+RSW)/(RBH+RT);

        //%---radiation-determined term
        PTR    = NRADC*SLOPE        /(SLOPE+PSR)/LHVAP;

        //%---vapour pressure-determined term
        PTD    = (VHCA*VPD/(RBH+RT))/(SLOPE+PSR)/LHVAP;

        //%---potential evaporation or transpiration
        PT     = max((double)1.E-10,PTR+PTD);

        //%---return value
        RV     = PT;
       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Net Leaf Absorbed Radiation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates net leaf absorbed radiation      *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  ATRJ    R4  Absorbed global radiation                  J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  FRAC    R4  Fraction of leaf classes (sunlit vs shaded)-         I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double NetLeafAbsRad(double ATRJ,double ATMTR,double FRAC,double TLEAF,double DVP)
      {
       double RV,CLEAR,BBRAD,RLWN,NRADC;

        //%---Stefan-Boltzmann constant(J/m2/s/K4)
       double BOLTZM = 5.668E-8;  

        //%---net absorbed radiation
        CLEAR  = max((double)0., min((double)1., (ATMTR-(double)0.25)/(double)0.45));//sky clearness
        BBRAD  = BOLTZM*pow(TLEAF +(double)273.,(double)4);
        RLWN   = BBRAD*((double)0.56-(double)0.079*sqrt(DVP*(double)10.))*((double)0.1
                 +(double)0.9*CLEAR)*FRAC;
        NRADC  = ATRJ - RLWN;

        //%---return value
        RV = NRADC;
 
       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// internal CO2  
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates the internal CO2 concentration  *
*           as affected by vapour pressure deficit.                    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  FVPD    R4  Slope for linear effect of VPDL on Ci/Ca   (kPa)-1   I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  C3C4    R4  Crop type (=1. for C3, -1 for C4 crops)    -         I  *
*  SVPL    R4  Saturated vapour pressure of leaf          kPa       O  *
*  CO2I    R4  intercellular CO2 concentration            ml m-3    O  *
*----------------------------------------------------------------------*/
double internalCO2(double TLEAF,double DVP,double FVPD,double CO2A,double C3C4)
      {
       double SVPL,VPDL,KMC25,KMO25,O2,EAVCMX,EAKMC,EAKMO,EARD,RDVX25;
       double KMC,KMO,GAMMAX,RDVCX,GAMMA0,GAMMA,RCICA,CO2I;
        
        //%---air-to-leaf vapour pressure deficit
        SVPL   = (double)0.611*exp((double)17.4*TLEAF/(TLEAF+(double)239.));
        VPDL   = max((double)0., SVPL - DVP);

        //%---Michaelis-Menten const. for CO2 at 25oC (umol/mol)
        KMC25  = INSW(C3C4,(double)650.,(double)404.9); //greater KMC25 for C4 than C3

        //%---Michaelis-Menten const. for O2 at 25oC (mmol/mol)
        KMO25  = INSW(C3C4,(double)450.,(double)278.4); //greater KMO25 for C4 than C3

        //%---CO2 compensation point in absence of dark respiration (GAMMAX)
        O2     = (double) 210.;    //oxygen concentration(mmol/mol)
        EAVCMX = (double)65330.;   //energy of activation for Vcmx(J/mol)
        EAKMC  = (double)79430.;   //energy of activation for KMC (J/mol)
        EAKMO  = (double)36380.;   //energy of activation for KMO (J/mol)
        EARD   = (double)46390.;   //energy of activation for dark respiration(J/mol)
        RDVX25 = (double)0.0089;   //ratio of dark respiration to Vcmax at 25oC

        KMC    = KMC25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMC/(double)8.314);
        KMO    = KMO25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMO/(double)8.314);
        GAMMAX = (double)0.5*exp(-(double)3.3801+(double)5220./(TLEAF+(double)273.)/(double)8.314)*O2*KMC/KMO;

        //%---CO2 compensation point (GAMMA)
        RDVCX  = RDVX25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*(EARD-EAVCMX)/(double)8.314);
        GAMMA0 = (GAMMAX+RDVCX*KMC*((double)1.+O2/KMO))/((double)1.-RDVCX);
        GAMMA  = INSW (C3C4, GAMMA0/(double)10., GAMMA0);

        //%---internal/ambient CO2 ratio, based on data of Morison & Gifford (1983)
        RCICA  = (double)1.-((double)1.-GAMMA/CO2A)*((double)0.14+FVPD*VPDL);

        //%---intercellular CO2 concentration
        CO2I   = RCICA * CO2A;
      
       return CO2I;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Leaf CO2 conductance (GC) and stomatal resistance to water (RSW)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates overall leaf conductance        *
*           for CO2 (GC) and the stomatal resistance to water (RSW).   *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  PLEAF   R4  Gross leaf photosynthesis                  gCO2/m2/s I  *
*  RDLEAF  R4  Leaf dark respiration                      gCO2/m2/s I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  CO2I    R4  Internal CO2 concentration                 ml m-3    I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RSW     R4  Potential stomatal resistance to water     s m-1     O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double gcrsw(double PLEAF,double RDLEAF,double TLEAF,double CO2A,double CO2I,double RBW,double RT)
      {
       double GC,RSW;

        //%---potential conductance for CO2
        GC  = (PLEAF-RDLEAF)*((double)273.+TLEAF)/(double)0.53717/(CO2A-CO2I);

        //%---potential stomatal resistance to water
        RSW = max((double)1E-10, (double)1./GC-RBW*(double)1.3-RT)/(double)1.6;
        
       return RSW;
      }

      
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Photosyntesis Model 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates leaf photosynthesis, based on   *
*           a renewed Farquhar biochemistry (cf Yin et al.2004.        *
*           Plant, Cell & Environment 27:1211-1222)                    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  C3C4    R4  Crop type (=1. for C3, -1. for C4 crops)   -         I  *
*  PAR     R4  Leaf absorbed photosynth. active radiance  J m-2 s-1 I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  CO2I    R4  Intercellular CO2 concentration            ml m-3    I  *
*  NP      R4  Photosynthetically active leaf N content   g m-2     I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax  & leaf N  umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  PLEAF   R4  Gross leaf photosynthesis                  gCO2/m2/s O  *
*  RDLEAF  R4  Leaf dark respiration                      gCO2/m2/s O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double photo(double C3C4,double PAR,double TLEAF,double CO2I,double NP,
                   double EAJMAX,double XVN,double XJN,double THETA)
      {
       double KMC25,KMO25,O2,EAVCMX,EAKMC,EAKMO,DEJMAX,SJ,PHI2M,HH,UPAR,KMC,KMO,GAMMAX;
       double VCT,JT,VCMX,JMAX,FPSEUD,ZZ,CC,SF,FQ,FCYC,ALPHA2,X,J2,VC,VJ,ALF,PLEAF;
	   double KTMP;
       
        //%---Michaelis-Menten constants for CO2 and O2 at 25oC
        if(C3C4 < (double)0.)
        {
         KMC25  = (double)650.;   //greater KMC25 for C4 than C3; unit:(umol/mol)
         KMO25  = (double)450.;   //greater KMO25 for C4 than C3; unit:(mmol/mol)
        }
        else
      {
         KMC25  = (double)404.9;  //unit:(umol/mol)
         KMO25  = (double)278.4;  //unit:(mmol/mol)
      }
      

        //%---other constants related to the Farquhar-type photosynthesis model
        O2     = (double)210.;    //oxygen concentration(mmol/mol)
        EAVCMX = (double)65330.;  //energy of activation for Vcmx(J/mol)
        EAKMC  = (double)79430.;  //energy of activation for KMC (J/mol)
        EAKMO  = (double)36380.;  //energy of activation for KMO (J/mol)
        DEJMAX = (double)200000.; //energy of deactivation for JMAX (J/mol)
        //DEJMAX = (double)152044.; //energy of deactivation for JMAX (J/mol) - Müller et al., Ecol. Modelling 183 (2005), 183210
        SJ     = (double)650.;    //entropy term in JT equation (J/mol/K)
        PHI2M  = (double)0.85;    //maximum electron transport efficiency of PS II
        HH     = (double)3.;      //number of protons required to synthesise 1 ATP

		KTMP   = (double)1.;

        //%---PAR photon flux in umol/m2/s absorbed by leaf photo-systems
        UPAR   = (double)4.56*PAR;//4.56 conversion factor in umol/J

        //%---Michaelis-Menten constants for CO2 and O2 respectively
        KMC    = KMC25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMC/(double)8.314);
        KMO    = KMO25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMO/(double)8.314);

        //%---CO2 compensation point in the absence of dark respiration
        GAMMAX = (double)0.5*exp(-(double)3.3801+(double)5220./(TLEAF+(double)273.)/(double)8.314)*O2*KMC/KMO;

        //%---Arrhenius function for the effect of temperature on carboxylation
        VCT    = exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAVCMX/(double)8.314);

        //%---function for the effect of temperature on electron transport
        JT     = exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAJMAX/(double)8.314)*
                ((double)1.+exp(SJ/(double)8.314-DEJMAX/(double)298./(double)8.314))/
                ((double)1.+exp(SJ/(double)8.314-(double)1./(TLEAF+(double)273.)*DEJMAX/(double)8.314));
        
		//TLEAF = pCl->pWeather->fTempAve;
		//if (TLEAF < (double)5) KTMP = max((double)0.0001,TLEAF*TLEAF*TLEAF*TLEAF/(double)625);
		//if (TLEAF < (double)5) KTMP = max((double)0.0001,TLEAF*TLEAF/(double)25);
		//if (TLEAF <=(double)0) KTMP = (double)0.0001;

        //%---maximum rates of carboxylation(VCMX) and of electron transport(JMAX)
        VCMX   = XVN*VCT*NP*KTMP;
        JMAX   = XJN*JT *NP*KTMP;

        //%---CO2 concentration at carboxylation site & electron pathways and their stoichiometries
        FPSEUD = (double)0.;           //assuming no pseudocyclic e- transport
        if (C3C4 < (double)0.)
        {
         ZZ   = (double)0.2;          //CO2 leakage from bundle-sheath to mesophyll
         CC   = (double)10.*CO2I;     //to mimic C4 CO2 concentrating mechanism
         SF   = (double)2.*(CC-GAMMAX)/((double)1.-ZZ);
         FQ   = (double)1.-FPSEUD-(double)2.*((double)4.*CC+(double)8.*GAMMAX)/HH/(SF+(double)3.*CC+(double)7.*GAMMAX);
         FCYC = FQ;
      }
        else
      {
         CC   = CO2I;
         SF   = (double)0.;
         FQ   = (double)0.;
         FCYC = (double)1.-(FPSEUD*HH*(SF+(double)3.*CC+(double)7.*GAMMAX)/((double)4.*CC+(double)8.*GAMMAX)+1.)/
                           (HH*(SF+(double)3.*CC+(double)7.*GAMMAX)/((double)4.*CC+(double)8.*GAMMAX)-(double)1.);
        }

       //%--- electron transport rate in dependence on PAR photon flux
       ALPHA2 = ((double)1.-FCYC)/((double)1.+((double)1.-FCYC)/PHI2M);
       X      = ALPHA2*UPAR/max((double)1.E-10,JMAX);
       J2     = JMAX*((double)1.+X-pow(pow((double)1.+X,(double)2)-(double)4.*X*THETA,(double)0.5))/2./THETA;

       //%---rates of carboxylation limited by Rubisco and electron transport
       VC     = VCMX * CC/(CC + KMC*(O2/KMO+(double)1.));
       VJ     =   J2 * CC*((double)2.+FQ-FCYC)/HH/(SF+(double)3.*CC+(double)7.*GAMMAX)/((double)1.-FCYC);

       //%---gross rate of leaf photosynthesis
       ALF    = ((double)1.-GAMMAX/CC)*min(VC,VJ);
       PLEAF  = max((double)1.E-10, ((double)1.E-6)*44.*ALF);

      return PLEAF;
     }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Dark Respiration Model 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double darkr(double TLEAF,double NP,double XVN)
      {
       double EARD,RDVX25,RDT,RDLEAF;
        
        //%---rate of leaf dark respiration
        EARD   = (double)46390.;  //energy of activation for dark respiration(J/mol)
        RDVX25 = (double)0.0089;  //ratio of dark respiration to Vcmax at 25oC
        RDT    = exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EARD/(double)8.314);
        RDLEAF = ((double)1.E-6)*(double)44. *RDVX25*(XVN*NP) * RDT;
        
       return RDLEAF;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Actual Leaf Photosynthesis
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates actual leaf photosynthesis when  *
*          water stress occurs.                                        *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DAYTMP  R4  Air temperature                            oC        I  *
*  PAR     R4  Absorbed photosynth. active radiation      J m-2 s-1 I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  C3C4    R4  Crop type (=1. for C3, -1 for C4 crops)    -         I  *
*  FVPD    R4  Slope for linear effect of VPD on Ci/Ca    (kPa)-1   I  *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 I  *
*  AT      R4  Actual leaf transpiration                  mm s-1    I  *
*  PT      R4  Potential leaf transpiration               mm s-1    I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RSW     R4  Potential stomatal resistance to water     s m-1     I  *
*  SLOPEL  R4  Slope of saturated vapour pressure curve   kPa oC-1  I  *
*  NP      R4  Photosynthet. active leaf N content        g m-2     I  *
*  NPN     R4  NP with small plant-N increment            g m-2     I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax  & leaf N  umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  PLFAS   R4  Actual leaf photosynthesis                 gCO2/m2/s O  *
*  PLFAN   R4  PLFAS with small plant-N increment         gCO2/m2/s O  *
*  ADIF    R4  Actual leaf-air temperature difference     oC        O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int ActualLeafPhotosynthesis_GECROS(double DAYTMP,double PAR,double DVP,double CO2A,double C3C4,
                                double FVPD,double NRADC,double AT,double PT,double RT,
                                double RBH,double RBW,double RSW,double SLOPEL,double NP,
                                double NPN,double EAJMAX,double XVN,double XJN,double THETA, 
                                 double *PLFAS,double *PLFAN,double *ADIF)
    {
     double PSYCH,ATLEAF,ARSW,ACO2I,APLFN,ARDN,APLF,ARD;

      PSYCH  = (double)0.067;    //psychrometric constant (kPa/oC)

      //%---leaf temperature if water stress occurs
      *ADIF   = difla(NRADC,AT,RBH,RT);
      ATLEAF = DAYTMP + *ADIF;

      //%---stomatal resistance to water if water stress occurs
      ARSW   = (PT-AT)*(SLOPEL*(RBH+RT)+PSYCH*(RBW+RT))/AT/PSYCH+PT/AT*RSW;

      //%---potential photosynthesis at the new leaf temperature
      ACO2I  = internalCO2(ATLEAF,DVP,FVPD,CO2A,C3C4);
        // SVPA noetig???
      APLFN  = photo(C3C4,PAR,ATLEAF,ACO2I,NPN,EAJMAX,XVN,XJN,THETA);
      ARDN   = darkr(ATLEAF,NPN,XVN);

      APLF   = photo(C3C4,PAR,ATLEAF,ACO2I,NP,EAJMAX,XVN,XJN,THETA);
      ARD    = darkr(ATLEAF,NP,XVN);

      //%---actual photosynthesis under water stress condition
      *PLFAS  = ((double)1.6*RSW+(double)1.3*RBW+RT)/((double)1.6*ARSW+(double)1.3*RBW+RT)*(APLF-ARD)+ARD;
      *PLFAN  = ((double)1.6*RSW+(double)1.3*RBW+RT)/((double)1.6*ARSW+(double)1.3*RBW+RT)*(APLFN-ARDN)+ARDN;

     return 1;
    }      

/*----------------------------------------------------------------------*
*  SUBROUTINE BETAF                                                    *
*  Purpose: This subroutine calculates the dynamics of expected growth *
*           of sinks, based on the beta sigmoid growth equation        *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DVR     R4  Development rate                            d-1      I  *
*  TE      R4  Stage at which sink growth stops            -        I  *
*  TX      R4  Stage at which sink growth rate is maximal  -        I  *
*  TI      R4  Stage of a day considered                   -        I  *
*  FD      R4  Relative expected growth of a sink at a day d-1      O  *
*----------------------------------------------------------------------*/
double betaf(double DVR, double TE, double TX, double TI)
{
 double FD;
  
 FD = DVR*(2.*TE-TX)*(TE-TI)/TE/(TE-TX)/(TE-TX)*abspowerDBL((TI/TE),(TX/(TE-TX)));

return FD;
}

/*----------------------------------------------------------------------*
*  SUBROUTINE SINKG                                                    *
*  Purpose: This subroutine calculates carbon demand for sink growth.  *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DS      R4  Development stage                           -        I  *
*  SSG     R4  Stage at which sink growth starts           -        I  *
*  TOTC    R4  Total carbon in a sink at end of its growth g C/m2   I  *
*  YG      R4  Growth efficiency                           g C/g C  I  *
*  FD      R4  Relative expected growth of a sink at a day d-1      I  *
*  DCDR    R4  Shortfall of C demand in previous days      g C/m2   I  *
*  DCS     R4  Daily C supply for sink growth              g C/m2/d I  *
*  DELT    R4  Time step of integration                    d        I  *
*  DCDC    R4  C demand of the current day                 g C/m2/d O  *
*  DCD     R4  Daily C demand for sink growth              g C/m2/d O  *
*  FLWC    R4  Flow of current assimilated C to sink       g C/m2/d O  *
*----------------------------------------------------------------------*/
int sinkg(double DS, double SSG,double TOTC,double YG,double FD,double DCDR,double DCS,double DELT,
              double *DCDC,double *DCD,double *FLWC)
{
 //%---expected demand for C of the current time step
      *DCDC   = INSW (DS-SSG, 0., TOTC/YG*FD);

 //%---total demand for C at the time step considered
      *DCD    = *DCDC + max(0.,DCDR)/DELT;

 //%---flow of current assimilated carbon to sink
      *FLWC   = min(*DCD, DCS);

return 1;
}


/*----------------------------------------------------------------------*
*  SUBROUTINE RNACC                                                    *
*  Purpose: This subroutine calculates rate of N accumulation in organs*
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  FNSH    R4  Fraction of new N partitioned to shoot       -       I  *
*  NUPT    R4  Nitrogen uptake at a time step               gN/m2/d I  *
*  RWST    R4  Rate of stem weight                          g/m2/d  I  *
*  STEMNC  R4  Nitrogen concentration in stem               gN/g    I  *
*  LNCMIN  R4  Minimum N concentration in leaf              gN/g    I  *
*  RNCMIN  R4  Minimum N concentration in root              gN/g    I  *
*  LNC     R4  Nitrogen concentration in leaf               gN/g    I  *
*  RNC     R4  Nitrogen concentration in root               gN/g    I  *
*  NLV     R4  Canopy (green)leaf N content                 gN/m2   I  *
*  NRT     R4  (living)root N content                       gN/m2   I  *
*  WLV     R4  Canopy (green)leaf weight                    g/m2    I  *
*  WRT     R4  (living)Root weight                          g/m2    I  *
*  DELT    R4  Time step of simulation                      d       I  *
*  CB      R4  Factor for initial N concent. of seed-fill   -       I  *
*  CX      R4  Factor for final N concent. of seed-fill     -       I  *
*  TM      R4  DS when transition from CB to CX is fastest  -       I  *
*  DS      R4  Development stage                            -       I  *
*  SEEDNC  R4  Standard seed N concentration                gN/g    I  *
*  RWSO    R4  growth rate of seed                          g/m2/d  I  *
*  LNLV    R4  Loss rate of NLV due to senescence           gN/m2/d I  *
*  LNRT    R4  Loss rate of NRT due to senescence           gN/m2/d I  *
*  RNRT    R4  rate of N accumulation in root               gN/m2/d O  *
*  RNST    R4  rate of N accumulation in stem               gN/m2/d O  *
*  RNLV    R4  rate of N accumulation in leaf               gN/m2/d O  *
*  RTNLV   R4  Positive value of RNLV                       gN/m2/d O  *
*  RNSO    R4  rate of N accumulation in seed(storage organ)gN/m2/d O  *
*----------------------------------------------------------------------*/
int rnacc(double FNSH,double NUPT,double RWST,double STEMNC,double LNCMIN,double RNCMIN,
              double LNC,double RNC,double NLV,double NRT,double WLV,double WRT,double DELT,
              double CB,double CX,double TM,double DS,double SEEDNC,double RWSO,double LNLV,
              double LNRT,double *RNRT,double *RNST,double *RNLV,double *RTNLV,double *RNSO)
{
 double NSHN,NLVA,NRTA,NTA;
 double CDS,ENSNC,NGS,NONC;
 double NLVN,GNLV;
 double NRTN,GNRT;
 
      //%---amount of N partitioned to shoot
      NSHN   = FNSH * NUPT;

      //%---leaf N (NLVA) or root N (NRTA) available for remobilization
      NLVA   = INSW(LNCMIN-LNC, NLV-WLV*LNCMIN, 0.) / DELT;
      NRTA   = INSW(RNCMIN-RNC, NRT-WRT*RNCMIN, 0.) / DELT;
	  NLVA   = INSW(LNCMIN-LNC, NLV-WLV*LNCMIN, 0.) /1.0; //Test of hong
      NRTA   = INSW(RNCMIN-RNC, NRT-WRT*RNCMIN, 0.) /1.0;
	  
      NTA    = NLVA + NRTA;

      //%---rate of N accumulation in stem
      *RNST   = RWST * INSW(-NTA,STEMNC,0.);

      //%---expected N dynamics during seed(storage organ) filling
      CDS    = CB+(CX-CB)*(4.-TM-DS)/(2.-TM)*abspowerDBL((DS-1.),(1./(2.-TM)));
      ENSNC  = LIMIT(CB,CX,CDS) * SEEDNC;

      //%---rate of N accumulation in seed
      NGS    = NSHN - *RNST - ENSNC*RWSO;
      NONC   = max(0.,INSW(NTA+NGS,(NTA+NSHN-*RNST)/NOTNUL(RWSO),ENSNC));
      *RNSO   = RWSO*NONC;

      //%---rate of N accumulation in leaf
      NLVN   = INSW(NTA+NGS,-NLVA-LNLV,-NLVA/NOTNUL(NTA)*(-NGS)-LNLV);
	  // NLVN = NLVN *0.1;//Test of Hong
      GNLV   = INSW(NGS, NLVN, NSHN-*RNST-*RNSO-LNLV);
	  //GNLV   = INSW(NGS, NLVN, (NSHN-*RNST-*RNSO-LNLV)); //Test of Hong
      *RNLV   = max(-NLV+1.E-7, GNLV);
      *RTNLV  = max(0., *RNLV);

      //%---rate of N accumulation in root
      NRTN   = INSW(NTA+NGS, NUPT-NSHN-NRTA-LNRT,NUPT-NSHN-NRTA/NOTNUL(NTA)*(-NGS)-LNRT);
      GNRT   = INSW(NGS, NRTN, NUPT-NSHN-LNRT);
      *RNRT   = max(-NRT+5.E-8, GNRT);

return 1;
}

int gecros_integrate_small_time_step_vars(gecros *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCLIMATE pCl = xpn->pCl;
	PWEATHER pWe = pCl->pWeather;
	PWATER pWa = xpn->pWa;
	double dt;

	if ((pTi->pSimTime->fTimeY==self->weather.fTimeY_save) && (pTi->pSimTime->iyear == self->weather.iyear_save)) {
		return RET_SUCCESS;
	} else {
		self->weather.fTimeY_save = pTi->pSimTime->fTimeY;
		self->weather.iyear_save = pTi->pSimTime->iyear;
	}

	dt = pTi->pTimeStep->fAct;



	if ((int)NewDay(pTi)) {
		//Begin of Hong
		//self->weather.fTempMax = pCl->pWeather->fTempMax;
		//self->weather.fTempMin = pCl->pWeather->fTempMin;
		//self->weather.fTempAve = pCl->pWeather->fTempAve;
		//End of Hong		
		self->weather.fTempMax = self->__weather.fTempMax;
		self->weather.fTempMin = self->__weather.fTempMin;
		self->weather.fTempAve = self->__weather.fTempAve;
		//self->weather.fPotTraDay = self->__weather.fPotTraDay;
		//self->weather.fEvapPotDay = self->__weather.fEvapPotDay;
		//self->weather.fActTraDay = self->__weather.fActTraDay;
		self->weather.fDaylySolRad = self->__weather.fDaylySolRad;
		self->__weather.fTempMax=pWe->fTempAir_daily_models;
		self->__weather.fTempMin=pWe->fTempAir_daily_models;
		self->__weather.fTempAve=0.0;
		self->__weather.fEvapPotDay=0.0;
		self->__weather.fPotETDay = 0.0;
		self->__weather.fDaylySolRad=0.0;
	} else {
		if (pWe->fTempAir_daily_models > self->__weather.fTempMax) {
			self->__weather.fTempMax = pWe->fTempAir_daily_models;
		}
		if (pWe->fTempAir_daily_models < self->__weather.fTempMin) {
			self->__weather.fTempMin = pWe->fTempAir_daily_models;
		}

		self->__weather.fTempAve += pWe->fTempAir_daily_models * dt;
		self->__weather.fEvapPotDay += pWa->pEvap->fPotR * dt;
		self->__weather.fPotETDay += pWa->fPotETR*dt;
		self->__weather.fDaylySolRad += pWe->fSolRad_daily_models * dt;

	}


	/*self->weather.fTempAve = *(self->TairMean);
	self->weather.fTempMin = *(self->TairMin);
	self->weather.fTempMax = *(self->TairMax);
	self->weather.fDaylySolRad = *(self->fSolRad);*/
	return RET_SUCCESS;
}


//Hong 2016-08-08: change for Sebastian Gayler and Arne Poyda:

//aus ceres_phenology.c, ceres_Germination(ceres *self)
int gecros_Germination(gecros *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	//===========================================
	//Variable Declaration
	//===========================================
	double          fCumDepth,fSWSD,fSWSD1,fSWSD2;
	int                     L,NLAYER,iGermination;
	

	double  fSowDepth = (double)10.0*pMa->pSowInfo->fSowDepth; //mm

	PSLAYER         pSL     =pSo->pSLayer;
	PSWATER         pSWL=pSo->pSWater;
	PWLAYER         pSLW=pWa->pWLayer;

	if(pPl->pDevelop->fStageXCERES==(double)0.0) {
		self->iDayAftSow = 0;
	}

	NLAYER=pSo->iLayers-2;
	//==============================================
	//Get the Soil Water Pointer in Seed Depth
	//==============================================
	fCumDepth=(double)0.0;
	pSL     =pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext;

	for (L=1; L<=NLAYER; L++) {
		fCumDepth += pSL->fThickness;
		if (fCumDepth>=fSowDepth)break;
		pSL = pSL->pNext;;
		pSWL = pSWL->pNext;
		pSLW = pSLW->pNext;
	}

	//============================================================
	//Calculating iGermination Possibility Based on Soil Moisture
	//============================================================
	if (pSLW->fContAct>pSWL->fContPWP)
		//Soil water is sufficient to support Germination
	{
		iGermination=1; //Germination occurs on this day.
		self->iDayAftSow=1;
	} else
		//If pSLW->fContAct<=pSWL->fContPWP,
		// soil water may be a problem
	{
		fSWSD1=(pSLW->fContAct-pSWL->fContPWP)*((double)0.65);
		pSLW = pSLW->pNext;
		pSWL = pSWL->pNext;
		fSWSD2=(pSLW->fContAct-pSWL->fContPWP)*((double)0.35);
		fSWSD=fSWSD1+fSWSD2;

		if (fSWSD>=(double)0.02)        //Soil water is sufficient to support germination
			iGermination=1;         //Germination occurs on this day.
		else {                  //Soil water is not sufficient.
			iGermination=0;         //Germination still not occur.

			self->iDayAftSow++;

			if (self->iDayAftSow>=40) {
				//if ((iDayAftSow<90) && ( (CROP=='WH')||(CROP=='BA') ) )
				if ((self->iDayAftSow<90) &&
				    ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
				     ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)))
					iGermination=0;         //Germination still not occurs
				else
					iGermination=-1;        //Germination fails
			}
		}
	}

	//=========================================================================================
	//Calculating iGermination Possibility Based on Soil Temperature
	//=========================================================================================
//      pCl->pWeather->fTempAve=(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)/((double)2.0);
//
//      if (pCl->pWeather->fTempAve<=(double)0.0)
//              iGermination=0;

	return iGermination;
}



  int Emergence_GECROS(gecros *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  
	  double DELT = (double)xpn->pTi->pTimeStep->fAct; //Hong
	  //Emergence (nach Ceres):

	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	//Thermal time reqired from germination to emergence (Degree.days)

	double fP9, fTbase_Emerg, fDTT;
	int iEmergence;

    PPLANT pPl = xpn->pPl;
//	PDEVELOP pDev=pPl->pDevelop;
	PMANAGEMENT pMa = xpn->pMa;

	//=========================================================================================
	//Calculate the SumDTT needed for Emergence for different Crops
	//=========================================================================================
//		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG"))
		if (strcmp(pPl->pGenotype->acCropCode,"SG")==0)				
		{
			fP9=(double)20.0+((double)6.0)*pMa->pSowInfo->fSowDepth;
			fTbase_Emerg = 5;
		}

//		else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI")||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
		else if ((strcmp(pPl->pGenotype->acCropCode,"MI")==0)||(strcmp(pPl->pGenotype->acCropCode,"MZ")==0)) 	
		{
			fP9=(double)15.0+((double)6.0)*pMa->pSowInfo->fSowDepth; //CERES-MAIZE, page 74
			fTbase_Emerg = 6;
		}

//		else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		else if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)	
		{
			fP9=(double)40.0+((double)10.2)*pMa->pSowInfo->fSowDepth;
			fTbase_Emerg = 2;
		}

//		else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
		else if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)
		{
			fP9=(double)50.0+((double)10.4)*pMa->pSowInfo->fSowDepth;
			fTbase_Emerg = 2;
		}
	
//		else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
		else if (strcmp(pPl->pGenotype->acCropCode,"SF")==0)	
		{
			fP9=(double)66.0+((double)11.9)*pMa->pSowInfo->fSowDepth;
			fTbase_Emerg = 5;
		}

//		else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RP"))
		else if (strcmp(pPl->pGenotype->acCropCode,"RP")==0)	
		{
			fP9=(double)40.0+((double)10.2)*pMa->pSowInfo->fSowDepth;
			fTbase_Emerg = 2;
		}
//Added by Hong on 20180205 for AP		
		else if (strcmp(pPl->pGenotype->acCropCode,"SB")==0)	
		{
			fP9 = (double)127.0; //Pascal Kremer
			fTbase_Emerg = 3;
		}
//End of Hong
		else
		{
			fP9 = (double)100.0;
			fTbase_Emerg = 2;
		}

	//=========================================================================================
	//Calculate Emergence Information
	//=========================================================================================
//		pCl->pWeather->fTempAve = (pCl->pWeather->fTempAve > 0? pCl->pWeather->fTempAve:(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin/2));
		
		self->weather.fTempAve = (self->weather.fTempAve > 0? self->weather.fTempAve:(self->weather.fTempMax+self->weather.fTempMin/2));

		if (fP9>(double)150.0)
			self->fEmergValue=(double)-1.0;
		else
		{
			if(self->weather.fTempAve < fTbase_Emerg)
				fDTT = 0;
			else
				fDTT = self->weather.fTempAve - fTbase_Emerg;

			self->fEmergValue += fDTT/fP9*DELT;
		}

		if (self->fEmergValue < (double) 1.0)
			iEmergence = -1;
		else
			iEmergence = 1;
	    
    return iEmergence;
  }

// End of Hong

//Begion of Hong on 20170622: Vernalisation
double gecros_Vernalization_CERES(gecros *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PPLANT pPl = xpn->pPl;
	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	double  fTempAveCrop, fTempMax,fTempMin;
	double  VF,VD,VD1,VD2,CumVD;

	PDEVELOP pDev=pPl->pDevelop;
    
	//Begin of Hong
	 //fTempMax = (double)xpn->pCl->pWeather->fTempMax;
	 //fTempMin = (double)xpn->pCl->pWeather->fTempMin;	 
	 fTempMax = (double)self->weather.fTempMax;
	 fTempMin = (double)self->weather.fTempMin;
	 double DELT = (double)xpn->pTi->pTimeStep->fAct;
	 //end of Hong

	CumVD=(double)pDev->fCumVernUnit; //Cumulative vernalization till today.

	//=========================================================================================
	//Plant Temperature Calculation
	//=========================================================================================
	//fTempAveCrop is the average plant crown temperature.
	fTempAveCrop=pPl->pPltClimate->fCanopyTemp;

	//=========================================================================================
	//Daily Vernalization Calculation
	//=========================================================================================
	if (fTempMax<=(double)0.0)           //if fTempMax lower than 0.0C, no vernalization occurs
		VD=0.0;                         //VD is the vernalization for a day.
	else {                              //if fTempMax>0.0, vernalization may or may not occur
		if (fTempMin>(double)15.0)       //if fTempMin>15.0 C, no vernalization occurs.
			VD=0.0;
		else {                      //if fTempMin<15.0 C, vernalization may occur.
			if((strcmp(pPl->pGenotype->acCropCode,"WM")==0)||(strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)||(strcmp(pPl->pGenotype->acCropCode,"RP")==0)) // here we have to distinguish between winter and summer crops --> DB!!!
	{
				//Vernalization for wheat barley and rape crops
				VD1=1.4-0.0778*(double)fTempAveCrop;
				VD2=0.5+13.44*
				    (double)(fTempAveCrop/((fTempMax-fTempMin+(double)3.0)*
				                           (fTempMax-fTempMin+(double)3.0)));
			} else {                //Vernalization for barly crops ?
				VD1=1.0-0.285*((double)fTempAveCrop-11.0);
				VD2=1.0-0.014*((double)fTempAveCrop-9.0)*((double)fTempAveCrop-9.0);
			}

			VD=max(0.0,min(1.0,min(VD1,VD2)));  //Daily vernalization value.
		}
	}

	//=========================================================================================
	//Cumulative Vernalization Days Calculation
	//=========================================================================================
	CumVD=CumVD+VD*DELT; //Cumulative vernalization value (cumulative vernalization days).

	//=========================================================================================
	//Anti-Vernalization
	//=========================================================================================
	//if ((CumVD<10.0)&&(self->weather.fTempMax>(double)30.0)) //Hong
	//	CumVD=CumVD-0.5*((double)self->weather.fTempMax-30.0);
   
    if ((CumVD<10.0)&&(fTempMax>(double)30.0)) //Hong
		CumVD=CumVD-0.5*((double)fTempMax-30.0);

	if (CumVD<0.0)
		CumVD=0.0;

	//=========================================================================================
	//Vernalization Factor
	//=========================================================================================
	//SG/14/06/99:
	//      wegen pPl->pGenotype->iVernCoeff = 0 VF immer gleich 1.
	//VF=1.0-(double)pPl->pGenotype->iVernCoeff*(50.0-CumVD); //vernalization factor.
	VF=min((double)1.0,1.0-(double)(1.0/pPl->pGenotype->iVernCoeff)*(50.0-CumVD)); //vernalization factor.

	if (VF<=0.0)    VF=0.0;
	if (VF>1.0)     VF=1.0;

	pDev->fVernEff      =(double)VF;
	pDev->fVernUnit     =(double)VD;
	pDev->fCumVernUnit  =(double)CumVD;

	return (double)VF;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds DailyVernalization_SPASS(double fTempMin, double fTempMax, PRESPONSE pResp,
//                                                   double fMinTemp, double fOptTemp, double  fMaxTemp)
//Author:    Enli Wang
//Date:        07.11.1996
//Purpose:    This function uses daily maximum and minimum temperature to calculate daily vernalization unit.
//            The unit ranges from 0.0 (min.or max. temperature) to 1.0 (optimal temperature).
//            The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp
//            contains zero data, then this function calculates the effect using the cardinal values
//            (fMinTemp,fOptTemp,fMaxTemp) and a predefined function RelativeTemperatureResponse_SPASS.
//Inputs:    1. fTempMin - Daily minimum temperature of the air (C)
//            2. fTempMax    - Daily maxmimum temperature of the air (C)
///            3. pResp    - Pointer to a RESPONSE structure containing temperature response data for vernalization
//            4. fMinTemp    - Minimum temperature for vernalization (C)
//            5. fOptTemp    - Optimum temperature for vernalization (C)
//            6. fMaxTemp    - Maximum temperature for vernalization (C)
//Outputs:    1. Return     - Daily vernalization unit (PVDs)
//Functions Called:
//            HourlyTemperature_SPASS
//            RelativeTemperatureResponse_SPASS
//            AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double gecros_DailyVernalization_SPASS(double fTempMin,double fTempMax,double fTemp, PRESPONSE pResp,
							  	   double fMinTemp, double fOptTemp, double  fMaxTemp, gecros *self)
     {
     //int         iLoop;
     //double         fTemp,fTempFunc,fVernUnit;
     double         fTempFunc,fVernUnit; 
     //Daily vernalization unit
     fVernUnit=(double)0.0;
     //for (iLoop=1; iLoop<=8; iLoop++)
         //{
         //3 Hour temperature
         //fTemp = HourlyTemperature_SPASS(iLoop,fTempMin,fTempMax);

         //Vernalization unit
         if ((pResp==NULL)||(pResp->fInput==(double)0.0))
             fTempFunc = gecros_RelativeTemperatureResponse_SPASS(fTemp,fMinTemp,fOptTemp,fMaxTemp);
         else
             fTempFunc = AFGENERATOR(fTemp, pResp);

         fVernUnit     += fTempFunc;
        // }

     //fVernUnit = fVernUnit/(double)8.0;

     return fVernUnit;
     }
	 
double gecros_RelativeTemperatureResponse_SPASS(double fTemp, double fMinTemp, double fOptTemp, double fMaxTemp)
{

    double vTemp, vTmin, vTopt, vTmax, p, vRelEff;

    vTemp = fTemp;
    vTmin = fMinTemp;
    vTopt = fOptTemp;
    vTmax = fMaxTemp;

    if ((fTemp <= fMinTemp) || (fTemp >= fMaxTemp))
        vRelEff = 0.0;
    else {
        p = log(2) / log((vTmax - vTmin) / (vTopt - vTmin));
        vRelEff =
            (2 * pow(vTemp - vTmin, p) * pow(vTopt - vTmin, p) - pow(vTemp - vTmin, 2 * p)) / pow(vTopt - vTmin, 2 * p);
    }

    return vRelEff;
}	 

/************************************************************************************************
                 Function PlantTemperature ()
************************************************************************************************/
double gecros_PlantTemperature(gecros *self)
     {
//=========================================================================================
     //Variable Declaration and Intitiation
//=========================================================================================
     expertn_modul_base *xpn = &(self->parent);
	 
	 double fTempMinCrop,fTempMaxCrop,fTempAveCrop;
    // char *lpCropType    =xpn->pPl->pGenotype->acCropCode;
     double fTempMax        =(double) self->weather.fTempMax; 
     double fTempMin        =(double) self->weather.fTempMin;
     double fSnow            =xpn->pCl->pWeather->fSnow;
	 //Begin of Hong
	 //fTempMax = (double)xpn->pCl->pWeather->fTempMax;
	 //fTempMin = (double)xpn->pCl->pWeather->fTempMin;
	 //end of Hong

     if (fSnow==(double)-99.00)
         fSnow=(double)0.0;

//=========================================================================================
     //When no snow, the Plant Crown Temperature is assumed to be the air temperature.
//=========================================================================================
     fTempMinCrop=fTempMin;        //fTempMinCrop -Minimum temperature of plant crown (C)
     fTempMaxCrop=fTempMax;      //fTempMaxCrop -Maximum temperature of plant crown (C)

//=========================================================================================
     //Snow raises the Plant Crown Temperature higher than air temperature.
//=========================================================================================
     if (fSnow>(double)15.0)    //fSnow -Precipitation in the form of Snow (mm)
         fSnow=(double)15.0;

     if (fTempMin<(double)0.0)
         fTempMinCrop=(double)2.0+fTempMin*
  ((double)0.4+((double)0.0018)*(fSnow-(double)15.0)*(fSnow-(double)15.0));

     if (fTempMax<(double)0.0)
         fTempMaxCrop=(double)2.0+fTempMax*
  ((double)0.4+((double)0.0018)*(fSnow-(double)15.0)*(fSnow-(double)15.0));

     //fTempAveCrop -Average temperature of plant crown (C)
     fTempAveCrop=(fTempMaxCrop+fTempMinCrop)/((double)2.0);

     xpn->pPl->pPltClimate->fCanopyTemp=fTempAveCrop;

     return fTempAveCrop;
     }
/************************************************************************************************
                 End of Function PlantTemperature ()
************************************************************************************************/
