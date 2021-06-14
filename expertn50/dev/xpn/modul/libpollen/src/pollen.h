//      pollen.h
//
//      Copyright 2010 Christian Klein <chrikle@berlios>
//



#ifndef __POLLEN_H__
#define __POLLEN_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define POLLEN_TYPE				(pollen_get_type())
#define POLLEN(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
                                 POLLEN_TYPE, pollen))
#define POLLEN_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
                                 POLLEN_TYPE, pollenClass))
#define IS_POLLEN(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
                                 POLLEN_TYPE))
#define IS_POLLEN_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
                                 POLLEN_TYPE))

typedef struct _pollen			pollen;
typedef struct _pollenClass		pollenClass;

typedef struct
{
	double fTempMax;
	double fTempMin;
	double fTempAve;
	double fPotTraDay;
	double fEvapPotDay;
	double fPotETDay;
	double fActTraDay;

	double fDaylySolRad;

	double fTimeY_save;
	int iyear_save;
} struct_weather;


struct _pollen
{
	expertn_modul_base parent;
	/* add your public declarations here */

	// Pollen Modelle:
	int step;

	double pollination_amount;
	
	double pollen_storage,pollen_storage_amount;
	double pollen_release_trigger;


	// Wetter für Tageswerte (phenology):
	struct_weather weather;
	struct_weather __weather;

	// TSM Modell:

	// Start Day for phenology of perennial plants
	int start_mday;
	int start_mon;
    double start_julian_day;

	int start_ont_mday; //start day for bud ontogenesis in MM H3
	int start_ont_mon;	//start month for bud ontogenesis in MM H3

	int start_GDD_int; //start GDD (temperature sum) integration for duration until bud burst
	int start_RU_Dormancy_check; //start GDD (temperature sum) integration for duration of dormancy
	int start_RU_Anthesis_check;
	int start_RU_Ontogenesis_int; //start GDD (temperature sum) integration for duration of dormancy
	int z; //Marker to count how often the function was passed.

	double T_base;   // [Grad C] base temperature
	double GDDc_start; // cirtical cum temp for bud burst
	double GDDc_end; // critical cum temp for end of anthesis
	double GDD; // Cumulative Temperature after onset of bud development
	double GDD_BudDev; // Cumulative Temperature to compute duration between onset of bud development until bud burst
	double GDD_Anthesis; //Cumulative Temperature to compute duration of anthesis.
	double alpha; // variety/species specific parameter
	double beta; // variety/species specific parameter
	double DayLengthCrit; // Kritische Tageslänge

	double RU_Dormancy_c_start; // cirtical sum of relative units for bud dormancy
	double SD_Dormancy_c_end; // critical sum of relative units for end of dormancy
	double RU_Dormancy; // Relative units to comupte duration of durancy

	double RU_Ontogenesis_c_start; // cirtical sum of relative units for bud dormancy
	double RU_Ontogenesis_c_end; // critical sum of relative units for end of dormancy
	double RU_Ontogenesis; // Relative units to comupte duration of durancy

	double RU_Anthesis_c_start; // cirtical sum of relative units for anthesis termination
	double RU_Anthesis_c_end; // critical sum of relative units for end of anthesis
	double RU_Anthesis; // Relative units to comupte duration of anthesis

	double cum_RU;
	double cum_SD;
	double cum_SD_dormancy;
	double cum_RU_dormancy;
	double cum_RU_ontogenesis;
	double cum_RU_anthesis;
	double RU;
	double SD;


	int pollination; // 0  1
	
	double anthesis_start_date; // Day of Year
	double anthesis_stop_date; // Day of Year
};


// public class member function:

//Pollenproduktion
//G_MODULE_EXPORT int pollen_BiomassGrowth1(pollen *self);
//G_MODULE_EXPORT int pollen_BiomassGrowth2(pollen *self);
G_MODULE_EXPORT int pollen_const_release(pollen *self); 
G_MODULE_EXPORT int pollen_SILAM_Sofiev(pollen *self);
G_MODULE_EXPORT int pollen_load(pollen *self);

int pollen_integrate_small_time_step_vars(pollen *self);

//Phänologie
double check_fixed_ontogenesis_date_H1_H2(pollen* self);
double check_fixed_ontogenesis_date_H3(pollen* self);
double check_anthesis(pollen *self);
double check_bud_ontogenesis(pollen *self);
double check_bud_ontogenesis_H2(pollen *self);
double check_bud_ontogenesis_H3(pollen *self);	//returns 1.0
double check_bud_dormancy_H1(pollen *self);
double check_bud_dormancy_H2(pollen *self);
double check_bud_dormancy_H3(pollen *self);    //returns 1.0
double Temp_dependence_of_ontogenesis(pollen* self,double temperature);
double Temp_dependence_of_dormancy(pollen* self, double temperature);

  
//Phänologie Blüte  
//Temperature sums
G_MODULE_EXPORT int pollen_TSM_DT_load(pollen *self);
G_MODULE_EXPORT int pollen_TSM_phenol_dev_dt(pollen *self);
G_MODULE_EXPORT int pollen_TSM_DLen_DT_load(pollen *self);
G_MODULE_EXPORT int pollen_TSM_phenol_dev_DLen_dt(pollen *self);
G_MODULE_EXPORT int pollen_TSM_WC_DT_load(pollen *self);
G_MODULE_EXPORT int pollen_TSM_WC_phenol_dev_dt(pollen *self);
G_MODULE_EXPORT int pollen_TSM_DAILY_load(pollen *self);
G_MODULE_EXPORT int pollen_TSM_phenol_dev_daily(pollen *self);
G_MODULE_EXPORT int pollen_TSM_WC_DAILY_load(pollen *self);
G_MODULE_EXPORT int pollen_TSM_WC_phenol_dev_daily(pollen *self);

//multiplicative approaches of bud development rates
G_MODULE_EXPORT int pollen_MultiplikationModel_H1_load(pollen *self);
G_MODULE_EXPORT int pollen_MultiplikationModel_H2_load(pollen *self);
G_MODULE_EXPORT int pollen_MultiplikationModel_H3_load(pollen *self);
G_MODULE_EXPORT int pollen_MultiplikationModel_H1(pollen *self);
G_MODULE_EXPORT int pollen_MultiplikationModel_H2(pollen *self);
G_MODULE_EXPORT int pollen_MultiplikationModel_H3(pollen *self);

//Pollen release
   
G_MODULE_EXPORT int pollen_load(pollen *self);
G_MODULE_EXPORT int pollen_const_release(pollen *self);
//SILAM-Modell Pollen Freisetzungskriterien M. Sofiev
G_MODULE_EXPORT int Pollen_Release_SILAM_Sofiev_load(pollen *self);
G_MODULE_EXPORT int Pollen_Release_SILAM_Sofiev(pollen *self);

//constant emission rate

double Trigger_relHum(pollen *self);
double Trigger_Wind(pollen *self);
double Trigger_NS(pollen *self);


struct _pollenClass
{
	expertn_modul_baseClass parent_class;
};

#define C_DEBUG(var)\
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

G_MODULE_EXPORT GType pollen_get_type (void);

G_END_DECLS

#endif /* __POLLEN_H__ */
