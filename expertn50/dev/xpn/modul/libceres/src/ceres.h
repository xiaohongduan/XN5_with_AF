//      ceres.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_H__
#define __CERES_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define CERES_TYPE				(ceres_get_type())
#define CERES(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			CERES_TYPE, ceres))
#define CERES_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			CERES_TYPE, ceresClass))
#define IS_CERES(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			CERES_TYPE))
#define IS_CERES_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			CERES_TYPE))

typedef struct _ceres			ceres;
typedef struct _ceresClass		ceresClass;


typedef struct {
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

struct _ceres
{
	expertn_modul_base parent;
	/* add your public declarations here */
	
	// aus plant.h
	int   iStandingResidues;
	
	// aus spass:
	double fSWDF3;
	//extern part:
	
	gboolean            bChangeJulianDay,bCalculateJulianDay;
	
	//Plant Part

	int             iStageOld;
	int             iTLNO;
	double  fPHINT, fXDevStage, fP5, fP9, fO1, fNFAC;
	double  fDaylengthYesterday;//=(double)12;

	//Maize:
	double fEarWeight, fEarGrowR, fEarFraction;
	double fTasselLeafNum, fMaxPlantLA;
	int  iSlowFillDay;

	//Sunflower:
	double fStemWeightMin, fStemWeightMax;
	double fHeadWeight, fHeadGrowR, fPotHeadWeight, fHeadWeightMin, fHeadWeightMax;
	double fPericarpWeight, fPericarpGrowR, fPotPericarpGrowR;
	double fEmbryoWeight, fEmbryoGrowR;
	double fIGrainWeight, fOil, fOilPercent;
	double fNHeadActConc, fNHeadMinConc, fNHeadOptConc, fNHeadDemand, fNHeadCont;
	double fNPericarpActConc, fNPericarpOptConc, fNPericarpDemand, fNPericarpCont;
	double fNEmbryoActConc, fNEmbryoOptConc, fNEmbryoDemand, fNEmbryoCont;
	double fNGreenLeafActConc, fNGreenLeafCont, fNSenesLeafCont;
	//double fTotWeight2, fNTopsCritConc;
	double fSumPS, fRI1, fRM, fRFR, fGrowFactor, ppp;
	double fCumSenesLA, fSpecLeafArea, fAPLA;
	double fSenesLAN2, fSLOPEPE, fXPEPE, fYRAT;
	int iDurP, iSenesTime, iJPEPE;
	gboolean bSenesCode;
	
	
	// Biomass:
	double fPotShootGrowRate,fPotRootGrowRate,fPotBiomGrowRate,fActBiomGrowRate;
	
	double fTopFraction;
	
	//transpiration:
	double *afActLayRootWatUpt;
	
	// statics:
	int      iDuration4;
	int      iDuration5;
	double PLSC[30];
	double fNormLeafSenes;
	double fSGRO[12];
	int Uptake;
	int      iDayAftSow;
	double fEmergValue;         //Thermal time reqired from germination to emergence (Degree.days)
	
	
	// organization:
	char *ini_filename;
	
	int iLimitRootGrowthPhase3;
	
	struct_weather weather;
	struct_weather __weather;
	
	double *TairMin,*TairMax,*TairMean,*fSolRad;
	
	int __LOAD_DONE;
	int __DONE_DONE;
};


// public class member function:

G_MODULE_EXPORT int ceres_load(ceres *self);
G_MODULE_EXPORT int ceres_done(ceres *self);
G_MODULE_EXPORT int ceres_phenological_development(ceres *self);
G_MODULE_EXPORT int ceres_biomass_growth(ceres *self);
G_MODULE_EXPORT int ceres_canopy_formation(ceres *self);
G_MODULE_EXPORT int ceres_nitrogen_demand(ceres *self);
G_MODULE_EXPORT int ceres_nitrogen_uptake(ceres *self);

G_MODULE_EXPORT int ceres_potential_transpiration(ceres *self);
G_MODULE_EXPORT int ceres_actual_transpiration(ceres *self);
G_MODULE_EXPORT int ceres_photosynthesis(ceres *self);
G_MODULE_EXPORT int ceres_root_length_growth(ceres *self);
G_MODULE_EXPORT int ceres_crop_senescence(ceres *self);


void ceres_init_structures(ceres *self);
GDate *ceres_convert_str_to_gdate(char* S);
int ceres_integrate_small_time_step_vars(ceres *self);

int ceres_register_var(ceres *self);

double ceres_SolPartTime(PTIME pTi);

#define EPSILON 1.0e-6

struct _ceresClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType ceres_get_type (void);

G_END_DECLS

#endif /* __CERES_H__ */

