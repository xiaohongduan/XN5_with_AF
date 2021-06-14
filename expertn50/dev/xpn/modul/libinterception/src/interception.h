//      interception.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __INTERCEPTION_H__
#define __INTERCEPTION_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define INTERCEPTION_TYPE				(interception_get_type())
#define INTERCEPTION(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			INTERCEPTION_TYPE, interception))
#define INTERCEPTION_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			INTERCEPTION_TYPE, interceptionClass))
#define IS_INTERCEPTION(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			INTERCEPTION_TYPE))
#define IS_INTERCEPTION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			INTERCEPTION_TYPE))

/*----------- INI File Parser (http://developer.gnome.org/glib/2.28/glib-Key-value-file-parser.html) -----------*/
#define GET_INI_INTEGER(var,groupname,key) \
						error = NULL; \
						var = g_key_file_get_integer (keyfile,groupname,key,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \

#define GET_INI_DOUBLE(var,groupname,key) \
						error = NULL; \
						var = g_key_file_get_double (keyfile,groupname,key,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \

#define C_DEBUG(var) \
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);
	
//#define NewDay(pz) ((pz->pSimTime->fTimeAct == (double) 0) || ((int)(pz->pSimTime->fTimeOld) != (int)(pz->pSimTime->fTimeAct)))? 1 : 0

typedef struct _interception			interception;
typedef struct _interceptionClass		interceptionClass;

struct _interception
{
	expertn_modul_base parent;
	/* add your public declarations here */
	
	/* Variables for the RUTTER MODEL */
	double SaturationCap;	// Saturation Capacity
	
	// drainage
	double b;
	double a_help;
	
	// throughfall
	double p;		// throughfall coefficient
	
	
	// XN Variables
	double T;	// temp [°C]
	double RH;	// rel. humidity [-]
	double wind;
	double elevation;
	
	// Constants
	double lambda;	// latent heat of vapourization of water, 
	double rho_c;	// product of density and spec. heat of dry air
	double gamma;	// psychrometric constant
	
	// Radiation	
	double Ra_h;	// extraterrestrial irradiance [MJ/m2*h], rate
	double Ra_hDay;	// extraterrestrial irradiance [MJ/m2*d], integrated day sum
	double Ra_d;	// extraterrestrial irradiance [MJ/m2*d], rate
	double RaDay;	// integrated extraterrestrial radiation
	
	double Rs;		// global radiation/solar radiation [MJ/m2], rate
	double RsDay;	// integrated solar radiation
	
	double Rs0;		// clear sky solar radiation [MJ/m2], rate
	
	double Rns;
	double Rnl;
	double Rn;		// net radiation [MJ/m2*d], rate
	double Rn_dt;	// net radiation
	double RnDay;	// net radiation [MJ/m2], integrated day sum	
	
	double omega;	// sunset hour angle in radians
	double n;		// actual sunshine duration
	double N;		// daylight hours, max. sunshine duration
	double fcd;		// cloudiness factor
	
	double Delta;
	double es;		// [kPa], eqn 37, Saturation Vapor Pressure
	double ea;		// [kPa], eqn 37, 41, Actual Vapor Pressure	
	
	// Interception
	double CanopyInputR;	// [mm/d]
	double CanopyInput_dt;	// [mm]
	double CanopyInput_Day;	// [mm/d]
	
	double InterceptionR;	// [mm/d]
	double Interception_dt;
	double Interception_Day;
	
	double Epot_R;			// [mm/d]
	double Epot_dt;
	double Epot_Day;		// [mm/d]
	double Epot_Yr;
	
	double Eact_R;			// [mm/d]
	double Eact_dt;
	double Eact_Day;
	double Eact_Yr;
	
	double CanopyWatR, CanopyWatR_old;			// Canopy Water [mm] 
	double CanopyWat_dt, CanopyWat_dt_old;
	double CanopyWat_Day, CanopyWat_Day_old;
	
	double C_S_dt;				// Canopy Storage / Storage Capacity
	
	double FreeThroughfallR;	// [mm/d]
	double FreeThroughfall_dt;	// [mm]
	double FreeThroughfall_Day;	// [mm/d]
	double FreeThroughfall_Yr;
	
	double ThroughfallR;	// [mm/d]
	double Throughfall_dt;
	double Throughfall_Day;	// [mm/d]
	double Throughfall_Yr;
	
	double DrainageR; 		// [mm/d]
	double Drainage_dt;		// [mm]
	double Drainage_dt2;	// [mm]
	double Drainage_Day;	// [mm]
	
	double PreciRate_dt;	// [mm]
	double PreciRate_Day;	// [mm/d]
	double PreciRate_Yr;
	
	/* Balance Variables */
	double Bal_R;
	double Bal_T;
	double Bal_E;
	double Bal_I;
	double Bal_D;
	
	/* Variables for the REGRESSION MODEL */
	double SI_R;
	double SI_dt;
	double SI_Day;
	double SI_Yr;
	double SImax;
	
	int __INIT_DONE;
	int __ERROR;
};


// public class member function:
G_MODULE_EXPORT int interception_Rutter_Model(interception *self);
G_MODULE_EXPORT int interception_regression(interception *self);
G_MODULE_EXPORT int interception_load(interception *self);
G_MODULE_EXPORT int interception_load_config(interception *self,const char* configfile);
G_MODULE_EXPORT int interception_register_var(interception *self);

struct _interceptionClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType interception_get_type (void);

G_END_DECLS

#endif /* __INTERCEPTION_H__ */

