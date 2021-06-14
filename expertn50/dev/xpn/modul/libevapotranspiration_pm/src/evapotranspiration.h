//      evapotranspiration.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __EVAPOTRANSPIRATION_H__
#define __EVAPOTRANSPIRATION_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define EVAPOTRANSPIRATION_TYPE				(evapotranspiration_get_type())
#define EVAPOTRANSPIRATION(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			EVAPOTRANSPIRATION_TYPE, evapotranspiration))
#define EVAPOTRANSPIRATION_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			EVAPOTRANSPIRATION_TYPE, evapotranspirationClass))
#define IS_EVAPOTRANSPIRATION(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			EVAPOTRANSPIRATION_TYPE))
#define IS_EVAPOTRANSPIRATION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			EVAPOTRANSPIRATION_TYPE))

typedef struct _evapotranspiration			evapotranspiration;
typedef struct _evapotranspirationClass		evapotranspirationClass;

typedef struct {
	double fTempMax;
	double fTempMin;
	double fTempAve;
	double fPotTraDay;
	double fEvapPotDay;
	double fPotETDay;
	double fActTraDay;
	
	double fWindSpeed;
	double fDaylySolRad;
	double fDailyHumidity;
	
	double fTimeY_save;
	int iyear_save;
	
} struct_weather;

struct _evapotranspiration
{
	expertn_modul_base parent;
	
	/* add your public declarations here */

	/* Radiation */
	double Rs;
	double Rs_Day;

	/* Evapotranspiration */
	double ETpot;
	double ETpot_dt;
	double ETpot_Day,ETpot_Day_;
	double ETpot_Year;
	double ETpot_Cum;
	
	double SEB_ETpot;
	double SEB_ETpot_dt;
	double SEB_ETpot_Day;
	double SEB_ETpot_Year;
	
	/* Evaporation */
	double Epot;
	double Epot_dt;
	double Epot_Day;
	double Epot_Year;
	double Epot_Cum;
	
	// for sensible heat:
	double pot_sens_heat;
	
	// for net radiation:
	double night_rad;
	double night_rad2;
	int night_count;
	double night_time;
	
	/* Evapotranspiration FAO */
	double fSunDuration;
	struct_weather weather;
	struct_weather __weather;
	double *TairMin,*TairMax,*TairMean,*fSolRad;
	double fTempYesterday;
	double fcumaero, fcumtrad;
	
	double cloud_cover_factor;
	
	double stomata_res_day;
	
	int iHaudeFalse, iHaudePan;
	int iHaudeTime;
	
	int __ERROR;
};


// public class member function:
G_MODULE_EXPORT int evapotranspiration_load(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_run(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_run65_0(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_run65_1(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_run65_2(evapotranspiration *self);
G_MODULE_EXPORT int evaporation_run1(evapotranspiration *self);
G_MODULE_EXPORT int evaporation_run2(evapotranspiration *self);
                int evaporation_run(evapotranspiration *self,int crop_cover_calc);
G_MODULE_EXPORT int evaporation_run3(evapotranspiration *self);
G_MODULE_EXPORT int act_evaporation_run(evapotranspiration *self);
G_MODULE_EXPORT int pot_transpiration_run(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_emissivity(evapotranspiration *self);

G_MODULE_EXPORT int penman_monteith_lh_run(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_lh_pot_run(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_sh_run(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_sh_run2(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_gh_run(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_gh_run_plus(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_gh_run_with_LAI(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_nr_load(evapotranspiration *self);
G_MODULE_EXPORT int penman_monteith_nr_run(evapotranspiration *self);

G_MODULE_EXPORT int evapotranspiration_FAO_run(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_FAO_load(evapotranspiration *self);

G_MODULE_EXPORT int evapotranspiration_haude_run(evapotranspiration *self);
G_MODULE_EXPORT int evapotranspiration_haude_load(evapotranspiration *self);


int evapotranspiration_run65(evapotranspiration *self, int opt_sfc);

// surface.c
void sfcdif1_loop(expertn_modul_base *xpn,int iter, double *moz, double *fm, double *fh, double *fv, double *mol,double *cm,double *ch, int *mozsign);
double surface_layer_get_ch(expertn_modul_base *xpn);


/*Calculate Net Radiation from Solar Radiation [MJ/m2*d]*/
double evapotranspiration_get_Rn(evapotranspiration *self,double *Rns,double *Rnl);

int evapotranspiration_integrate_small_time_step_vars(evapotranspiration *self);
int evapotranspiration_get_Haude_weather(evapotranspiration *self);
double SolPartTime(PTIME pTi);

double evapotranspiration_calc_crop_cover_frac1(double LAI);
double evapotranspiration_calc_crop_cover_frac2(double LAI,double kcan);

#define PlantIsGrowing                  ((xpn->pPl->pDevelop->bPlantGrowth==TRUE)&&(xpn->pPl->pDevelop->bMaturity==FALSE))
#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)
#define SQR(x) ((x)*(x))

#define C_DEBUG(var)
	//xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

struct _evapotranspirationClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType evapotranspiration_get_type (void);


G_END_DECLS

#endif /* __EVAPOTRANSPIRATION_H__ */

