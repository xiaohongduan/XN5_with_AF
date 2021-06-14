//      xnoah.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __XNOAH_H__
#define __XNOAH_H__

#include <expertn_modul_base.h>
#include "xnoah_util.h"

G_BEGIN_DECLS

#define XNOAH_TYPE				(xnoah_get_type())
#define XNOAH(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			XNOAH_TYPE, xnoah))
#define XNOAH_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			XNOAH_TYPE, xnoahClass))
#define IS_XNOAH(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			XNOAH_TYPE))
#define IS_XNOAH_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			XNOAH_TYPE))

typedef struct _xnoah			xnoah;
typedef struct _xnoahClass		xnoahClass;




struct _xnoah
{
	expertn_modul_base parent;
	int first;
	
	PSTRUC_ADD_SOIL soil;
	
	PSTRUC_VEG_PARAM veg_param;
	PSTRUC_SOIL_PARAM soil_param;
	PSTRUC_GEN_PARAM gen_param;
		
	
	int vegtype;
	int soiltype;
	
	int is_urban; // entspricht der Noah Var ISURBAN (wird hier auf -1 gestetzt --> sollte nie vorkommen)
	
	double *AI,*BI,*CI; // Soil Matrix coefficients
	double *RHSTS; // right hand side tendency term
	double *RHSTSin;
	double *CIin;
	double *STCOUT,*STCIN;
	
	//double hcpct;  // volumetric heat capacity [J m-3 K-1] = C(theta)
	double ZBot; // depth[m] of lower boundary soil temperature
	
	gboolean itavg; // Temperatur averaging invoked
	
	
	// evapo_:
	
	int __ini_done;
	int __done_done;
	int nsoil;
	double *smc; // volumetric soil moisture
	double *sh2o; // UNFROZEN SOIL MOISTURE CONTENT (VOLUMETRIC FRACTION) NOTE: FROZEN SOIL MOISTURE = SMC - SH2O
	double shdfac; // green vegetation fraction
	double *smcdry; // dry soil moisture thresshold where direct evap from top layer end (volumetric)--> soil table
	double *smcmax; // maximum of canopy water capacity --> cmcmaxdata --> const in vegparam table
	double *smcwlt; // wilting point
	double *dwsat; // SAT soil diffusivity --> aus tabelle = BB*dksat*(satpsi/maxsmc), mit == SATDW IN SOILPARM
	double *satpsi; // Modellgröße AEV (Campell Alpha) [mm] = SATPSI * 1000.0
	double *frzfact;
	double *bexp; // campel beta const
	double *dksat; // fCondSat
	double *refsmc; // field capacity fContFK
	double *zsoil;
	
	double *rtdis; // Array [NRoot] rootdistributiion 
	int	   NRoot;
	
	// Outputvars:
	double ETA;// ACTUAL LATENT HEAT FLUX (W m-2: NEGATIVE, IF UP FROM SURFACE)
	double EDIR; // DIRECT SOIL EVAPORATION
	double EC; // CANOPY WATER EVAPORATION (W m-2) --> Interception
	double *ET; // NSoil
	double ETT; // TOTAL PLANT TRANSPIRATION (W m-2)
	
	double fxexp; //=2.0; onst in GENPARM
	double cmcmax; // = 0.5e-3 aus VEGPARM.tbl
	double cfactr; //  Parameter used in the canopy inteception calculation == 0.5 VEGPARM.TBL
	double sfctmp; // Air temperature (K) at level ZLVL
	double ch; // surface exchange coefficient [m/s]
	double th2; // air potential temperature at heigth zlvl above ground
	double fdown;
	double ssoil;
	double solnet;
	double sheat;
	double q2; // spec. humidity
	double q2sat; // sat spec. hum SAT SPECIFIC HUMIDITY AT HEIGHT ZLVL ABOVE GROUND (KG KG-1)
	double dqsdt2; //DQSDT2     SLOPE OF SAT SPECIFIC HUMIDITY CURVE AT T=SFCTMP (KG KG-1 K-1)
	double emissi; // Surface emissivity (between 0 and 1)
	double T1; // Skin temperature (K)
	double sfcprs; // surface pressure at height temp_messurement
	double t1v;
	double th2v;
	double epsca;
	double df;
	double df1;
	double sbeta;
	double czil; //Zilitinkevich constant, read from GENPARM.TBL and used to compute surface exchange coefficients
	double cm;
	double pc;
	double cmc;
	int ice;
	double prcp1;
	double T1_2;
	double etp;
	
char *weather_file_name;
STRUCT_XNO_DATA *climate_values;
	
};

// NOAH constants:




// public class member function:

G_MODULE_EXPORT int xnoah_load_and_init(xnoah *self);
G_MODULE_EXPORT int xnoah_run_emissivity(xnoah *self);
G_MODULE_EXPORT int xnoah_heat_transfer_done(xnoah *self);

G_MODULE_EXPORT int xnoah_run_load(xnoah *self);
G_MODULE_EXPORT int xnoah_run_transpiration(xnoah *self);
G_MODULE_EXPORT int xnoah_run_potential_evaportranspiration(xnoah *self);
G_MODULE_EXPORT int xnoah_run_senible_heat(xnoah *self);
G_MODULE_EXPORT int xnoah_run_done(xnoah *self);

void xnoah_calc_soil_vars(xnoah *self);

double xnoah_DEVAP(double etp1,double smc, double shdfac, double smcdry,double smcmax,double fxexp);

int xnoah_load_weather_file(xnoah *self);
int xnoah_free_weather_file(xnoah *self);



struct _xnoahClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType xnoah_get_type (void);

G_END_DECLS

#endif /* __XNOAH_H__ */

