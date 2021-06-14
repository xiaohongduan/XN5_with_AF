//      xpn_output.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __XPN_OUTPUT_H__
#define __XPN_OUTPUT_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define XPN_OUTPUT_TYPE				(xpn_output_get_type())
#define XPN_OUTPUT(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			XPN_OUTPUT_TYPE, xpn_output))
#define XPN_OUTPUT_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			XPN_OUTPUT_TYPE, xpn_outputClass))
#define IS_XPN_OUTPUT(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			XPN_OUTPUT_TYPE))
#define IS_XPN_OUTPUT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			XPN_OUTPUT_TYPE))

typedef struct _xpn_output			xpn_output;
typedef struct _xpn_outputClass		xpn_outputClass;

typedef struct
{
	// Radiation
	double emissivity, latentHeat, sensibleHeat, Albedo, netRadiation,solRadiation;
	
	// Temperature
	double DayMeanTemp;
	double DayMeanTempcalc;
	double *soil_temp_mean,*soil_temp_calc;
	
	// cum heat:
	double cum_lat_heat,cum_sen_heat,cum_net_rad,cum_sol_rad;
} struct_Heat;


typedef struct
{
	// soil water
	double *water_content;
	double *matric_pot;
	double *hydraulic_conduct, *hydraulic_conduct_calc;
	
	// zwischen values for output reasons
	double precip_zwischen, canopyprecip_zwischen, intercept_zwischen, evap_zwischen, transp_zwischen, evapotransp_zwischen, infiltration_zwischen, runoff_zwischen, drain_zwischen;
	double pot_transp_zwischen, pot_evapotransp_zwischen, pot_evapo_zwischen;
	
	//rates
	double precip_mean; //potETR, actETR, potER, actER, potTR, actTR, InfiltrationR, RunoffR, DrainR, EIR;
	
	// dt
	//double precip_dt, potET_dt, actET_dt, potE_dt, actE_dt, potT_dt, actT_dt, EI_dt, Infiltration_dt, Runoff_dt, Drain_dt;
	
	// precipitation
	double precip_mean_calc, precip_day, precip_sum, canopyprecip_day, canopyprecip_sum;
	
	// humidity
	double relHum, specHum;

	// evapotranspiration, evaporation, transpiration (Rate, dt, Day, Sum)	
	double potET_Day, actET_Day;
	double potET_Sum, actET_Sum;	
	
	double potE_Day, actE_Day;
	double potE_Sum, actE_Sum;	
	
	double potT_Day, actT_Day;
	double potT_Sum, actT_Sum;
	
	// infiltration
	double Infiltration_Day, Infiltration_Sum;
	
	// runoff
	double Runoff_Day, Runoff_Sum;
	
	// drainage/leakage
	double Drain_Day, Drain_Sum;
	
	// Interception
	double EI_Day, EI_Sum;
	
	// Balance
	double Input;	// Precipitation - Interception Loss(?)
	double Output;	// Output = Drainage + act. Evapotranspiration
	double Balance;	// Input - Output
	double BalanceXN;	// balance from XN var
	
	double WaterIce_Profile;	// fWaterStorage, Water and Ice in Profile [mm]
	
	//Water Flux Density
	double *FluxDens_Day, *FluxDensZwischen_Day;
	//double FluxDens_dt, FluxDens_Day, FluxDens_Sum;
	
} struct_Water;

typedef struct
{
  double *link_to_var;
  char* name;
} struct_special_var;

typedef struct
{
  double var;
  double **link_to_vars;
  int link_to_vars_count;
  double mul_fact;
  char* name;
} struct_special_var_profile;

struct _xpn_output
{
	expertn_modul_base parent;
	/* add your public declarations here */
	struct_Heat heat;
	struct_Water water;
	int soil_numbers;
	
	PHLAYER *APHLayer;
	PWLAYER *APWLayer;
	
	BOOL bWriteAverage;
	
	double dt_sum_count;
	double dt_sum;
	
   	// Atmosphere:
	double cum_dust1,cum_dust2,cum_dust3,cum_dust4,cum_dust5;
	
	// Nitrogen:
	double no3profile, no3lower23proflile, no3lower13profile, nh4profile, urealprofil;
	double no3_30cm, no3_60cm, no3_90cm;
	//double *no3_content;
	//double *nh4_content;
	
	double fNO3LeachCum,fNMinerCum,fNO3DenitCum,fNH4NitrCum,fNImmobCum;
	
	double NReleaseFrehOrgMatter;
	
	double fUreaHydroDay,fNHumusMinerDay, fNH4NitrDay,fNO3DenitDay,fNImmobDay,fNLitterMinerDay,fNManureMinerDay;
	
	double fNLitterImmobDay,fNHumusImmobDay,fNManureImmobDay;
	
	// Carbon
	double CLitter,CManure,CHumus,NLitter,NManure,NHumus;
	
	double CLitter30, CManure30, CHumus30, NLitter30, NManure30, NHumus30;
	double CLitter60, CManure60, CHumus60, NLitter60, NManure60, NHumus60;
	double CLitter90, CManure90, CHumus90, NLitter90, NManure90, NHumus90;	
	double CLitterProf, CManureProf, CHumusProf, NLitterProf, NManureProf, NHumusProf;
	
	double CNRatioHumus,CNRatioLitter,CNRatioManure;
	
	// Plant
	double Nuptake, H2Ouptake;
	double DevStage, LAI, TotRLD, Diameter, PlantDens,Height;	// Height
	double RootMass, StemMass, BranchMass, LeafMass, FruitMass, WoodMass, StrawMass,AboveWoodMass; //StrawMass and AboveWoodMass added by Hong
	double AboveMass,TotalMass;
	double AssimilatePool;
	double RootWaterUp;
    double fPotTranspR;
    double fActTranspR;
    double fSolRadDay, fPARDay, fSolRadDay_zwischen, fPARDay_zwischen;
	
	double *RLD;
		
		// Plant Nitrogen
	double NRoot, NStem, NLeaves, NGrain, NAboveGround;
    
    double rooting_depth;
	
	
	
	//===================================
	int timesave;
	
	int bno_txt_output;
	
	//int timesave_output_Year;
	//double timesave_output_TimeY;
	
	double intfSolRad, intfPar;
	
	double output_time_step;
	char *output_time_step_string;
	
	struct_timespan timespan;
	
	int    output_first;
	STRUCT_XPN_TIME xpn_time_old;
	char *varlist;
	
	
	/// definition of special outputs, which fits to the measurement values:
	
	double *c_n_ratio_litter; // C-N Ratio Array i-layer
	double *c_n_ratio_manure; // C-N Ratio Array i-layer
	double *c_n_ratio_humus; // C-N Ratio Array i-layer
	
	double *wfps;		// water filled pore space Array i-layer
	
	char *special_output_def_file;
	
	
	int spec_vars_count;
	struct_special_var *spec_vars;
	
	int spec_vars_profile_count;
	struct_special_var_profile *spec_vars_profile;
		
};


// public class member function:
G_MODULE_EXPORT int xpn_output_reg_var(xpn_output *self);
G_MODULE_EXPORT int xpn_output_calc_var(xpn_output *self);
G_MODULE_EXPORT int xpn_output_done(xpn_output *self);

int xpn_output_load_special_output_def_file(xpn_output *self);

struct _xpn_outputClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType xpn_output_get_type (void);

#define  EPSILON 1.0e-9
#define kgPhaTomgPsqm 100.0




G_END_DECLS

#endif /* __XPN_OUTPUT_H__ */

