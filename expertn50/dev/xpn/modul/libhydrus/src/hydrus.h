//      hydrus.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HYDRUS_H__
#define __HYDRUS_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define HYDRUS_TYPE				(hydrus_get_type())
#define HYDRUS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			HYDRUS_TYPE, hydrus))
#define HYDRUS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			HYDRUS_TYPE, hydrusClass))
#define IS_HYDRUS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			HYDRUS_TYPE))
#define IS_HYDRUS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			HYDRUS_TYPE))

typedef struct _hydrus			hydrus;
typedef struct _hydrusClass		hydrusClass;

typedef double** Tableau;


struct _hydrus
{
	expertn_modul_base parent;
	int iBotBC; // Boundary Condition
	int mob_imm; // Schalter Mobil immopobil
	
	// Hydraulic Functions:
	double (*WCont)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*HCond)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*DWCap)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
    double (*MPotl)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
					
	
	// Arrays [iLayers]:
	double *afTS;
	double *afTR;
	
	double  *ThN,*ThO,*hNew,*hOld;
    double  *Sink;
    //double  hTab[NTabD];
    double *ThR,*ThS,*hSat,*ConSat;
    double *Con,*ConO,*Cap,*hTemp;
	
	double *v;
	double *vFlux;
	
	double *P,*R,*S;
	
	// other vars:
	
	double dCosAlf;
	double fCapillRiseR;
	
	BOOL  AtmBC;
	BOOL  TopInF;
	BOOL  KodBot;
	BOOL  BotInF;
	BOOL  FreeD;
	BOOL  SeepF;
	BOOL  qGWLF;
	
	BOOL WLayer;
	double KodTop;
	
	double  rTop,rTopOld,rBot,rBotOld,hTop,hBot,hBotOld,vTop,vBot;
	// ck_
	double vTopFlux,vBotFlux; 
    double  KBOld, KTOld, rMin, rMax;

    BOOL  MinStep, ConvgF;

	double t,tOld,tNew,tFix,tMax,dt,dtOld,DeltaTOld,DeltaT,DeltaZ;
    double dtMin,dtMax,dtMaxW,dtMaxC,dtInit,dtOpt,dMul1,dMul2;
    int   Iter, ItCum, ItMax, ItMin, IGoTo;
	
	int NTabD;
	Tableau  ConTab, CapTab, STab;
	double  *hTab;  //[NTabD];
	double cumRain,cumRainDay,cumRainDay_;
	
	BOOL use_infiltration_limit;// infiltration limit at the upper boundary --> ponding
	BOOL use_infiltration_limit_layer; // infiltration limit in every layer
	
	int __ERROR;
	
};


// public class member function:
G_MODULE_EXPORT int hydrus_water_flow_init(hydrus *self);
G_MODULE_EXPORT int hydrus_water_flow_run(hydrus *self);
G_MODULE_EXPORT int hydrus_water_flow_done(hydrus *self);


// HYDRUS SUB FUNCTIONS:

int hydrus_water_flow_genMat(hydrus *self, Tableau ConTab, Tableau CapTab, Tableau STab);
int hydrus_water_flow_setBC(hydrus *self);
int hydrus_water_flow_setMat(hydrus *self, Tableau ConTab, Tableau CapTab, Tableau STab);
int hydrus_water_flow_setEq(hydrus *self);
int hydrus_water_flow_changeBC(hydrus *self);
int hydrus_water_flow_solveEq(hydrus *self);
BOOL hydrus_water_flow_testCvg(hydrus *self);
int hydrus_water_flow_tmCont(hydrus *self);
int hydrus_water_flow_veloc(hydrus *self,int N,double dCosAlf);
void hydrus_rain_limit_infiltration(hydrus *self);
void hydrus_rain_limit_infiltration_layer(hydrus *self);

//flux q at lower boundary in relation to ground-water table z
double Fqh(double z, double a, double b);

#define C_DEBUG(var)
	//xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);
	
#define C_DEBUG_LOC_IN_ARRAY(var,i)
/*	{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}
*/

struct _hydrusClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType hydrus_get_type		(void);

G_END_DECLS

#endif /* __HYDRUS_H__ */

