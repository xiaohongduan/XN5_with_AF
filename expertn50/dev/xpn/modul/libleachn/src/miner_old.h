
//      leachn_miner.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __LEACHN_MINER_H__
#define __LEACHN_MINER_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define LEACHN_MINER_TYPE				(leachn_miner_get_type())
#define LEACHN_MINER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			LEACHN_MINER_TYPE, leachn_miner))
#define LEACHN_MINER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			LEACHN_MINER_TYPE, leachn_minerClass))
#define IS_LEACHN_MINER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			LEACHN_MINER_TYPE))
#define IS_LEACHN_MINER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			LEACHN_MINER_TYPE))

typedef struct _leachn_miner			leachn_miner;
typedef struct _leachn_minerClass		leachn_minerClass;

struct _leachn_miner
{
	expertn_modul_base parent;
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


// public class member function:
G_MODULE_EXPORT int leachn_miner_run_load(leachn_miner *self);
G_MODULE_EXPORT int leachn_miner_run(leachn_miner *self);

struct _leachn_minerClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType leachn_miner_get_type (void);

G_END_DECLS

#endif /* __LEACHN_MINER_H__ */
