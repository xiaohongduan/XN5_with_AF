//      ceres.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "ceres.h"
#include "ceres_phenology.h"
#include "ceres_biomass_growth.h"
#include "ceres_canopy_formation.h"
#include "ceres_nitrogen.h"
#include "ceres_photosynthesis.h"
#include "ceres_transpiration.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(ceres, ceres, EXPERTN_MODUL_BASE_TYPE);

#define NewDayAndPlantGrowing   ((NewDay(pTi))&&(xpn->pPl->pDevelop->bPlantGrowth==TRUE)&&(xpn->pPl->pDevelop->bMaturity==FALSE))
#define PlantIsGrowing                  ((xpn->pPl->pDevelop->bPlantGrowth==TRUE)&&(xpn->pPl->pDevelop->bMaturity==FALSE))

static void ceres_class_init(ceresClass *klass) {}


static void ceres_init(ceres *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL CERES!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
	self->__LOAD_DONE=0;
	self->__DONE_DONE=0;
}



// Our Modul Functions:

int ceres_load(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT			pPl = xpn->pPl;
	PNFERTILIZER fertil_first,fertil,fertil_act;
	char *ini_filename, *S;

	if (self->__LOAD_DONE!=0) {
		return RET_SUCCESS;
	}
	self->__LOAD_DONE=1;

	ceres_register_var(self);
	ceres_init_structures(self);
	self->fDaylengthYesterday=(double)12.0;
	ceres_Initialize_MZ_SF(self);

	// Read from INI File:
	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.ceres.filename");

	if (ini_filename==NULL) {
		PRINT_ERROR("Missing entry 'ceres.filename' in your configuration!");
	} else {
		char *S2;
		S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
		if (S2!=NULL) {
			ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
			if (expertn_modul_base_PlantManagementRead(xpn,ini_filename,"ceres")!=0) {
				PRINT_ERROR("Error Read CERES INI File");
			}
			self->ini_filename = g_strdup_printf("%s",ini_filename);
			free(ini_filename);
			free(S2);
		}

	}

	//self->iLimitRootGrowthPhase3 = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.ceres.Limit rootgrowth phase 3",0);

	self->TairMin = xpn_register_var_get_pointer(xpn->pXSys->var_list,"expertn_database.fTempMin");
	self->TairMax = xpn_register_var_get_pointer(xpn->pXSys->var_list,"expertn_database.fTempMax");
	self->TairMean = xpn_register_var_get_pointer(xpn->pXSys->var_list,"expertn_database.fTempMean");
	self->fSolRad = xpn_register_var_get_pointer(xpn->pXSys->var_list,"expertn_database.fSolRad");
	
	return RET_SUCCESS;
}

int ceres_done(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT			pPl = xpn->pPl;
	PGENOTYPE		pGe = pPl->pGenotype;


	if (self->__DONE_DONE!=0) {
		return RET_SUCCESS;
	}
	self->__DONE_DONE=1;
	g_free(self->afActLayRootWatUpt);
	self->afActLayRootWatUpt=NULL;
	g_free(pGe->PmaxTemp);
	g_free(pGe->PmaxAge);
	g_free(pGe->DevRateVegTemp);
	g_free(pGe->DevRateRepTemp);
	g_free(pGe->DevRateVegPhtP);
	g_free(pGe->VernRateTemp);
	g_free(pGe->DVR);
	g_free(pGe->PartRoot);
	g_free(pGe->PartLeaf);
	g_free(pGe->PartStem);
	g_free(pGe->PartStorage);
	g_free(pGe->ReservGrwLag);
	g_free(pGe->ReservTransTc);
	g_free(pGe->SoFillLag);
	g_free(pGe->SoFillRateTemp);
	g_free(pGe->SoNFillRateTemp);
	g_free(pGe->MaxLvNc);
	g_free(pGe->MaxStNc);
	g_free(pGe->MaxRtNc);
	g_free(pGe->OptLvNc);
	g_free(pGe->OptStNc);
	g_free(pGe->OptRtNc);
	g_free(pGe->MinLvNc);
	g_free(pGe->MinStNc);
	g_free(pGe->MinRtNc);
	g_free(pGe->RootExtRateTemp);
	g_free(pGe->RootExtRateWc);
	g_free(pGe->LvDeathRateTemp);
	g_free(pGe->RtDeathRateTemp);
	g_free(pGe->SpecLfWght);
	g_free(pGe->LfAppRateTemp);

	if (self->ini_filename!=NULL) free(self->ini_filename);
	self->ini_filename=NULL;
	return RET_SUCCESS;
}



int ceres_phenological_development(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	//if (pPl==NULL) return 1;

	xpn->pPl->pDevelop->iStageCERES;

	if NewDayAndPlantGrowing {
	ceres_phenological_development_run(self);
	}
	
	ceres_integrate_small_time_step_vars(self);
	expertn_modul_base_DevelopmentCheckAndPostHarvestManagement(xpn);

	return RET_SUCCESS;
}

int ceres_biomass_growth(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;


	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (xpn->pPl->pDevelop->iDayAftEmerg > 0) {
			ceres_CropMaintenance_run(self);
			ceres_BiomassPartition_run(self);
			ceres_biomass_growth_run(self);
		}
	}

	ceres_integrate_small_time_step_vars(self);

	return RET_SUCCESS;
}

int ceres_canopy_formation(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PPLANT pPl = xpn->pPl;

	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (pPl->pDevelop->iDayAftEmerg > 0)
			ceres_canopy_formation_run(self);
	}

	ceres_integrate_small_time_step_vars(self);

	return RET_SUCCESS;
}

int ceres_nitrogen_demand(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PPLANT pPl = xpn->pPl;

	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (pPl->pDevelop->iDayAftEmerg > 0) {
			ceres_NitrogenConcentrationLimits(self);
			ceres_PlantNitrogenDemand(self);
		}
	}
	
	ceres_integrate_small_time_step_vars(self);
	
	return RET_SUCCESS;
}

int ceres_nitrogen_uptake(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PPLANT pPl = xpn->pPl;
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;

	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (pPl->pDevelop->iDayAftEmerg > 0) {
			ceres_NitrogenUptake_run(self);
			ceres_PlantNitrogenStress_run(self);
		}
	}

	if PlantIsGrowing {
	int L;
	PLAYERROOT      pLR     =pPl->pRoot->pLayerRoot;
	PCLAYER         pSLN=pCh->pCLayer->pNext;

	for (L=1; L<=pSo->iLayers-2; L++) {
			//Check the whether there are roots in this layer:
			if (pLR->fLengthDens==(double)0.0)           break;
			
			//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
			pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct;
			pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct;

			pLR =pLR ->pNext;
			pSLN=pSLN->pNext;
		}
	}

	ceres_integrate_small_time_step_vars(self);

	return RET_SUCCESS;
}

int ceres_root_length_growth(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;

	if NewDayAndPlantGrowing
	ceres_root_length_growth_run(self);
	
	ceres_integrate_small_time_step_vars(self);	
	
	return RET_SUCCESS;
}

int ceres_crop_senescence(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PPLANT pPl = xpn->pPl;

	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (pPl->pDevelop->iDayAftEmerg > 0)
			ceres_PlantSenescense(self);
	}
	
	ceres_integrate_small_time_step_vars(self);
	
	return RET_SUCCESS;
}

int ceres_photosynthesis(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PPLANT pPl = xpn->pPl;

	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (pPl->pDevelop->iDayAftEmerg > 0)
			ceres_DailyCanopyGrossPhotosynthesis_run(self);
	}
	
	ceres_integrate_small_time_step_vars(self);
	
	return RET_SUCCESS;
}





int ceres_potential_transpiration(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PPLANT pPl = xpn->pPl;

	double EvTeiler;

	EvTeiler = (pTi->pTimeStep->fAct < (double)0.11)?
	           ceres_SolPartTime(pTi)
	           : (double)1.0;

	if ((int)NewDay(pTi)) self->weather.fPotTraDay=(double)0;

	if NewDayAndPlantGrowing {
	//Wird nur berechnet wenn der Feldaufgang erreicht wurde
	if (pPl->pDevelop->iDayAftEmerg > 0)
			self->weather.fPotTraDay=ceres_PotentialTranspiration_run(self);
	}

	if PlantIsGrowing {
	double DeltaT;
	DeltaT=pTi->pTimeStep->fAct;

	pPl->pPltWater->fPotTranspR = self->weather.fPotTraDay * EvTeiler / DeltaT;
	pPl->pPltWater->fPotTranspdt = self->weather.fPotTraDay*DeltaT * EvTeiler / DeltaT;
	// ep 210599 to avoid "initial value = daily value" in balance.c by using
	// pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay * DeltaT;

	// since pPl->pPltWater->fPotTranspDay is used as cumulative variable which
	// sums up the pPl->pPltWater->fPotTranspdt values and has to be zero at the
	// beginning of a new day
}

	ceres_integrate_small_time_step_vars(self);

return RET_SUCCESS;
}

int ceres_actual_transpiration(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	
	double EvTeiler;

	if ((int)NewDay(pTi)) self->weather.fActTraDay=(double)0;
	if (((int)NewDay(pTi))&&((pPl!=NULL))) pPl->pRoot->fUptakeR =(double)0.0;
	//ep 05.02.01  not reinitialized after harvest (in capacity water flow module)!

	// SolPartTime(pTi) can be found in evapotranspiration_fao.c
	EvTeiler = (pTi->pTimeStep->fAct < (double)0.11)?
	           ceres_SolPartTime(pTi)
	           : (double)1.0;
	//if(pPl->pPltWater->fActTranspR > 0.0)		   
	

	if NewDayAndPlantGrowing {
	int L;
	PLAYERROOT     pLR     =pPl->pRoot->pLayerRoot;

	//Wird nur berechnet wenn der Feldaufgang erreicht wurde

	if (pPl->pDevelop->iDayAftEmerg > 0) {
			ceres_ActualTranspiration_run(self);
			ceres_PlantWaterStress_run(self);
		}
	self->weather.fActTraDay = pPl->pPltWater->fActTranspR;
		//-------------------------------------------------------------------
		// If the time step is smaller than one day, the water uptake is
		// calculated from daily Wateruptake
		//-------------------------------------------------------------------

		for (L=1; L<=pSo->iLayers-2; L++) {
			self->afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
			pLR =pLR ->pNext;
		}


	}       //      end if NewDayAndPlantGrowing



	if PlantIsGrowing {

    PSLAYER         pSL     =pSo->pSLayer->pNext;
	PWLAYER         pSLW=pWa->pWLayer->pNext;
//                              PLAYERROOT      pLR     =pPl->pRoot->pLayerRoot;

	int L;
	
	//printf("%f \n", EvTeiler);
	//printf("vor %f \n", pPl->pPltWater->fActTranspR);
	pPl->pPltWater->fActTranspR = self->weather.fActTraDay * EvTeiler / pTi->pTimeStep->fAct;
	//printf("nach %f %f \n", pPl->pPltWater->fActTranspR, EvTeiler);
	
	for (L=1; L<=pSo->iLayers-2; L++) {
			pSLW->fContAct -= self->afActLayRootWatUpt[L] * EvTeiler / pTi->pTimeStep->fAct
			/pSL->fThickness
			*pTi->pTimeStep->fAct; //mm/mm
			
			//if (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,1982,12,24)>0)
			//if (L==23)
			//printf("%d %f %f \n", L, pSLW->fContAct, self->afActLayRootWatUpt[L]);
			
            pSL =pSL ->pNext;
			pSLW=pSLW->pNext;
			
//                              pLR =pLR ->pNext;
		}


	} //end if plant is growing
	
	ceres_integrate_small_time_step_vars(self);
	
	return RET_SUCCESS;
}

int ceres_integrate_small_time_step_vars(ceres *self)
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
		self->weather.fTempMax = self->__weather.fTempMax;
		self->weather.fTempMin = self->__weather.fTempMin;
		self->weather.fTempAve = self->__weather.fTempAve;
		//self->weather.fPotTraDay = self->__weather.fPotTraDay;
		//self->weather.fEvapPotDay = self->__weather.fEvapPotDay;
		//self->weather.fActTraDay = self->__weather.fActTraDay;
		self->weather.fDaylySolRad = self->__weather.fDaylySolRad;

		//printf("%f %f %f %f \n", self->weather.fTempMin, self->weather.fTempAve, self->weather.fTempMax, self->weather.fDaylySolRad);

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

double ceres_SolPartTime(PTIME pTi)
{
	double T1, T2, y;
	double DayTime     = pTi->pSimTime->fTimeDay;
	double DeltaT;

	DeltaT = pTi->pTimeStep->fAct;


	if ((DayTime <= (double)0.3)||(DayTime > (double)0.8)) {
		T1 = (double)0.0;
		T2 = (double)0.0;
	} else {
		T1 = max((double)0.0,(DayTime - DeltaT - (double)0.3));
		T2 = min((double)0.5,(DayTime - (double)0.3));
	} /* else DayTime  */


	y = (double)(cos((double)(2.0*PI*T1)) - cos((double)(2.0*PI*T2))) / (double)2.0;

	return y;
}

void ceres_init_structures(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	int i;
	self->afActLayRootWatUpt = g_malloc0(sizeof(double)*xpn->pSo->iLayers);
	self->iStandingResidues=0;
	self->fSWDF3=0.0;
	self->bChangeJulianDay=0;
	self->bCalculateJulianDay=0;
	self->iStageOld=0;
	self->iTLNO=0;
	self->fPHINT=0.0;
	self->fXDevStage=0.0;
	self->fP5=0.0;
	self->fP9=0.0;
	self->fO1=0.0;
	self->fNFAC=0.0;
	self->fDaylengthYesterday=0.0;
	self->fEarWeight=0.0;
	self->fEarGrowR=0.0;
	self->fEarFraction=0.0;
	self->fTasselLeafNum=0.0;
	self->fMaxPlantLA=0.0;
	self->iSlowFillDay=0.0;
	self->fStemWeightMin=0.0;
	self->fStemWeightMax=0.0;
	self->fHeadWeight=0.0;
	self->fHeadGrowR=0.0;
	self->fPotHeadWeight=0.0;
	self->fHeadWeightMin=0.0;
	self->fHeadWeightMax=0.0;
	self->fPericarpWeight=0.0;
	self->fPericarpGrowR=0.0;
	self->fPotPericarpGrowR=0.0;
	self->fEmbryoWeight=0.0;
	self->fEmbryoGrowR=0.0;
	self->fIGrainWeight=0.0;
	self->fOil=0.0;
	self->fOilPercent=0.0;
	self->fNHeadActConc=0.0;
	self->fNHeadMinConc=0.0;
	self->fNHeadOptConc=0.0;
	self->fNHeadDemand=0.0;
	self->fNHeadCont=0.0;
	self->fNPericarpActConc=0.0;
	self->fNPericarpOptConc=0.0;
	self->fNPericarpDemand=0.0;
	self->fNPericarpCont=0.0;
	self->fNEmbryoActConc=0.0;
	self->fNEmbryoOptConc=0.0;
	self->fNEmbryoDemand=0.0;
	self->fNEmbryoCont=0.0;
	self->fNGreenLeafActConc=0.0;
	self->fNGreenLeafCont=0.0;
	self->fNSenesLeafCont=0.0;
	self->fSumPS=0.0;
	self->fRI1=0.0;
	self->fRM=0.0;
	self->fRFR=0.0;
	self->fGrowFactor=0.0;
	self->ppp=0.0;
	self->fCumSenesLA=0.0;
	self->fSpecLeafArea=0.0;
	self->fAPLA=0.0;
	self->fSenesLAN2=0.0;
	self->fSLOPEPE=0.0;
	self->fXPEPE=0.0;
	self->fYRAT=0.0;
	self->iDurP=0.0;
	self->iSenesTime=0;
	self->iJPEPE=0;
	self->bSenesCode=0;
	self->fPotShootGrowRate=0.0;
	self->fPotRootGrowRate=0.0;
	self->fPotBiomGrowRate=0.0;
	self->fActBiomGrowRate=0.0;
	self->fTopFraction=0.0;
	self->iDuration4=0;
	self->iDuration5=0;
	for (i=0; i<30; i++) self->PLSC[i]=0.0;
	self->fNormLeafSenes=0.0;
	for (i=0; i<12; i++) self->fSGRO[i]=0.0;
	self->Uptake=0;
	self->iDayAftSow=0;
	self->fEmergValue=0.0;         //Thermal time reqired from germination to emergence (Degree.days)
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;	
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;		
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

}

GDate *ceres_convert_str_to_gdate(char* S)
{
	char yearstr[5],monthstr[3],daystr[3];
	int i;
	int day,month,year;
	GDate *test;

	if ((S==NULL)||(strlen(S) != 10))
		return ((GDate *)g_date_new_dmy((GDateMonth)01,(GDateMonth)01,(GDateMonth)9999));

	for (i=0; i<4; i++) {
		yearstr[i] = S[i];
	}
	yearstr[4]=0;
	monthstr[0]=S[5];
	monthstr[1]=S[6];
	monthstr[2]=0;
	daystr[0]=S[8];
	daystr[1]=S[9];
	daystr[2]=0;

	day = atoi(daystr);
	month = atoi(monthstr);
	year = atoi(yearstr);
	test = (GDate*)g_malloc(sizeof(GDate));
	test = ((GDate *)g_date_new_dmy((GDateMonth)day,(GDateMonth)month,(GDateMonth)year));
	if (g_date_valid(test) == TRUE)
		return test;
	else
		return ((GDate *)g_date_new_dmy((GDateMonth)01,(GDateMonth)01,(GDateMonth)9999));

}

int ceres_register_var(ceres *self)
{

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fTempMax,"ceres.weather.fTempMax",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fTempMin,"ceres.weather.fTempMin",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fTempAve,"ceres.weather.fTempAve",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fPotTraDay,"ceres.weather.fPotTraDay",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fEvapPotDay,"ceres.weather.fEvapPotDay",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fActTraDay,"ceres.weather.fActTraDay",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->weather.fDaylySolRad,"ceres.weather.fDaylySolRad",0.0);

	return RET_SUCCESS;
}
