//      water_flow_hutson.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __WATER_FLOW_HUTSON_H__
#define __WATER_FLOW_HUTSON_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define WATER_FLOW_HUTSON_TYPE				(water_flow_hutson_get_type())
#define WATER_FLOW_HUTSON(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			WATER_FLOW_HUTSON_TYPE, water_flow_hutson))
#define WATER_FLOW_HUTSON_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			WATER_FLOW_HUTSON_TYPE, water_flow_hutsonClass))
#define IS_WATER_FLOW_HUTSON(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			WATER_FLOW_HUTSON_TYPE))
#define IS_WATER_FLOW_HUTSON_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			WATER_FLOW_HUTSON_TYPE))

typedef struct _water_flow_hutson			water_flow_hutson;
typedef struct _water_flow_hutsonClass		water_flow_hutsonClass;

#define C_DEBUG(var)\
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

struct _water_flow_hutson
{
	expertn_modul_base parent;
	/* add your public declarations here */	
	double fLeachPonding;
	double fLostPondCum;
	int fPond_Day, fPond_Mon,fPond_Year;
	
	double precip;
	
	double   *F1,*G1;
	double   *vorPot;
	
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
					
	
};

/* delay [day] after which ponding will be treated as runoff:                    */
#define PONDING_TIME 1.0

/* constant rain rate:                                                           */
#define CONST_RAIN_RATE 100.0

/* Minimal (not freezing) water content                                          */
#define THETA_MIN 0.1

//define min(x,y) MIN(x,y)
//define max(x,y) MAX(x,y)


// public class member function:

G_MODULE_EXPORT int water_flow_hutson_load(water_flow_hutson *self);
G_MODULE_EXPORT int water_flow_hutson_run(water_flow_hutson *self);
G_MODULE_EXPORT int water_flow_hutson_done(water_flow_hutson *self);

double WaterCapacity(water_flow_hutson *self);
int Freezing(water_flow_hutson *self);
int CalculateSink(water_flow_hutson *self);
int SolveRG(water_flow_hutson *self);
double UpperBoundary(water_flow_hutson *self);
//int Set_At_Least_Epsilon(expertn_modul_base *xpn,double *x, const char *Message);


struct _water_flow_hutsonClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType water_flow_hutson_get_type (void);

G_END_DECLS

#endif /* __WATER_FLOW_HUTSON_H__ */

