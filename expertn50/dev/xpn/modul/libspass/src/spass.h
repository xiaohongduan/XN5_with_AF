//      spass.h
//      
//      Copyright 2012 Florian Heinlein <florian.heinlein@helmholtz-muenchen.de>
//      



#ifndef __SPASS_H__
#define __SPASS_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define SPASS_TYPE				(spass_get_type())
#define SPASS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			SPASS_TYPE, spass))
#define SPASS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			SPASS_TYPE, spassClass))
#define IS_SPASS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			SPASS_TYPE))
#define IS_SPASS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			SPASS_TYPE))

typedef struct _spass			spass;
typedef struct _spassClass		spassClass;

struct _spass
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
//zusätzliche Variablen für CERES-Maize und CERES-Sunflower
	int		iStageOld,iTLNO,iSlowFillDay;
	double	fPHINT, fXDevStage, fP5, fP9, fO1, fNFAC;

//Maize:
	double	fEarWeight, fEarGrowR, fEarFraction;
	double	fTasselLeafNum, fMaxPlantLA;

//Sunflower:
	double	fStemWeightMin, fStemWeightMax;
	double	fHeadWeight, fHeadGrowR, fPotHeadWeight, fHeadWeightMin, fHeadWeightMax;
	double	fPericarpWeight, fPericarpGrowR, fPotPericarpGrowR;
	double	fEmbryoWeight, fEmbryoGrowR;
	double	fIGrainWeight, fOil, fOilPercent;
	double	fNHeadActConc, fNHeadMinConc, fNHeadOptConc, fNHeadDemand, fNHeadCont;
	double	fNPericarpActConc, fNPericarpOptConc, fNPericarpDemand, fNPericarpCont;
	double	fNEmbryoActConc, fNEmbryoOptConc, fNEmbryoDemand, fNEmbryoCont;
	double	fNGreenLeafActConc, fNGreenLeafCont, fNSenesLeafCont;
	double	fSumPS, fRI1, fRM, fRFR, fGrowFactor, ppp;
	double	fCumSenesLA, fSpecLeafArea, fAPLA;
	double	fSenesLAN2, fSLOPEPE, fXPEPE, fYRAT;

//zusätzliche Variablen für SUCROS:
	double	fBaseTemperature1;
	double	fCriticalTemperatureSum1;
	double	fBaseTemperature2;
	double	fCriticalTemperatureSum2;


	int __INIT_DONE;
	int __DONE_DONE;
	int __ERROR;
	int __MEAN_DONE;
	int __CAN_START;
	char *ini_filename;

	//double *global_radiation,*par,
    double *fTemp_zwischen;

	double fDayTest;

	int iThermalDev, iDailyVern, iMeanTemp, iSolRad;
	double *fSolRad_zwischen;
	int iAveGrossPs;
	double *fAveGrossPs_zwischen;

	double fAvailableAssim;
	double fPgMax, fLUE, vGrossPs, vDayl, cumphotos, cumgrossphotos;
	double fPgmmaxP, fCO2FuncP, fTempFuncP, fLfNcFuncP;
	double fActNcP,fMinNcP,fMaxOptNcP;
	double fNUF;
	double fLeafRatio, fRootRatio, fStemRatio;
	double vSinHeight;
	double PARCum;
	double fTRNU_zwischen;
	
	double fInterceptionCapacity;

	double *afActLayRootWatUpt;
	
	// static vars:
	double spass_AssimilatePartition_xtuber;
	double spass_ReserveTranslocationfTotalReservWeihgt;
	double spass_COMVR_TO_ZSTAGE_AND_CERES_WHEATnode;
	int spass_DailyCanopyGrossPhotosythesisGaussian;
	double spass_DailyCanopyGrossPhotosythesisPARCum;
	
	// output for AgMIP	
    double yield; // t ha-1
    double total_above_ground_biomass; // t ha-1
    double total_et; // (over growing season) mm 
    double total_intercepted_par; // MJ ha-1
	double max_lai; // (over growing season) m²/m²
	int anthesis_year;
    int anthesis_date; //DOY
    int maturity_date; //DOY
    int maturity_year;
    int harvest_year;
	int harvest_date; // DOY
    double SOC_at_sowing_30; // 20cm oder 30cm g m-2
    double SOC_at_sowing; // whole profile
    double total_net_ecosystem_exchange; // over season g m-2 (???)
    double total_net_primary_productivity; // over season g m-2 (???)
    double total_N2ON; // over season kg ha-1
    double total_N2ON_an; // annual kg ha-1
    double total_N_leaching_1k5; // over season kg/ha (at 1.5m)
    double total_N_leaching_1k5_an; // annual kg/ha (at 1.5m)
    double total_water_drainage_1k5; // over season mm/ha
    double total_water_drainage_an_1k5; // annual mm/ha
    double max_root_depth; // season m;
    int water_stress_days;
    double means_stress;
    int n_stress_days;
    double nmean_stress;
    double mineral_N_150; // Soil mineral N at harvest from 0-150 cm depth (kg/ha)
    double N_fert_amounts; //kg/ha
    
    
    // help agmip:
    int saved_year;
    double _total_N2ON;
    double _total_N2ON_an;
    double _total_et;
    double _max_lai;
    double _total_intercepted_par;
    double _total_water_drainage_1k5;
    double _total_N_leaching_1k5_an;
    double _total_water_drainage_an_1k5;
    double _total_N_leaching_1k5;
    double _max_root_depth;
    double leachNCUMold;
    double leachNCUMold_veg;
    int season,oldseason;
    int _water_stress_days;
    double _means_stress;
    double _N_fert_amounts; //kg/ha
    
    double _fpot_day;
    double _fact_day;
    
    double _npot_day;
    double _nact_day;
    
    int _n_stress_days;
    double _nmean_stress;
    
   
	
	// debugvar:
	double temp_par,temp_pard;
	
	// harvest at fStageSUCROS >= 2 if possible
	int harvest_at_maturity;
    
    // set LAI to 0 of maturity date is reached:
    int set_LAI_to_0_after_maturity;
	
};

#define C_DEBUG(var) 
//	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i)
/*	{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}\*/

#define PLANT_IS_GROWING ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))

// public class member function:
G_MODULE_EXPORT int spass_phenological_development(spass *self); //PhasicDevelopment_SPASS
G_MODULE_EXPORT int spass_potential_transpiration(spass *self); //WaterCapture_SPASS
G_MODULE_EXPORT int spass_CanopyWaterInterception(spass *self); //WaterCapture_SPASS
G_MODULE_EXPORT int spass_biomass_growth(spass *self); //Biomassgrowth_SPASS
G_MODULE_EXPORT int spass_crop_maintenance(spass *self); //CropMaintenance_SPASS
G_MODULE_EXPORT int spass_root_length_growth(spass *self); //RootSystemFormation_SPASS
G_MODULE_EXPORT int spass_nitrogen_uptake(spass *self); //NitrogenCapture_SPASS
G_MODULE_EXPORT int spass_actual_transpiration(spass *self); //WaterCapture_SPASS
G_MODULE_EXPORT int spass_canopy_gross_photosynthesis(spass *self); //Photosynthesis_SPASS
G_MODULE_EXPORT int spass_canopy_formation(spass *self); //CanopyFormation_SPASS
G_MODULE_EXPORT int spass_nitrogen_demand(spass *self); //NitrogenCapture_SPASS
G_MODULE_EXPORT int spass_crop_senescence(spass *self); //OrganSenescence_SPASS
//G_MODULE_EXPORT int spass_water_capture(spass *self); //WaterCapture_SPASS
G_MODULE_EXPORT int spass_nitrogen_capture(spass *self); //NitrogenCapture_SPASS
//G_MODULE_EXPORT int spass_PotEvapotraPenman(spass *self); //Penman-Evapotranspiration

G_MODULE_EXPORT int spass_PlantVariableInitiation(spass *self);

void spass_init_agmip_vars(spass *self);
void spass_agmip_calc_an_vars(spass *self);
void spass_agmip_calc_season_vars(spass *self);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  LIBRARY EXPORT - SUB PROCESS FUNCTIONS
//------------------------------------------------------------------------------------------------------------
//Biological processes
double			spass_RelativeTemperatureResponse(double fTemp, double fMinTemp, double fOptTemp, double fMaxTemp);
//double			DVR[11];

//Climate/weather processes
double			spass_DaylengthAndPhotoperiod(double fLatitude, int nJulianDay, int nID);

//Nitrogen uptake
int				spass_NitrogenConcentrationLimits(spass *self);

//Water Uptake
double fPotTraR, fActTraR, fPotTraDay;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  TOOL FUNCTIONS
//------------------------------------------------------------------------------------------------------------
double			spass_AFGENERATOR(double fInput, RESPONSE* pfResp);
double			spass_LIMIT(double v1, double v2, double x);

struct _spassClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType spass_get_type (void);

G_END_DECLS

#endif /* __SPASS_H__ */

