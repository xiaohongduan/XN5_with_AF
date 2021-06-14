//      stickstoff.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __STICKSTOFF_H__
#define __STICKSTOFF_H__
#include "stickstoff_util.h"
#include <expertn_modul_base.h>

//#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE) //aus daisy.h herauskopiert (ich weiß aber nicht mehr wieso)
//auskommentiert 29.1.13 (vielleicht sollte ich das ganz rausschmeißen. ich glaub das erfüllt nämlich keinen zweck)

#define STICKSTOFF_TYPE				(stickstoff_get_type())
#define STICKSTOFF(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			STICKSTOFF_TYPE, stickstoff))
#define STICKSTOFF_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			STICKSTOFF_TYPE, stickstoffClass))
#define IS_STICKSTOFF(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			STICKSTOFF_TYPE))
#define IS_STICKSTOFF_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			STICKSTOFF_TYPE))

typedef struct _stickstoff			stickstoff;			 
typedef struct _stickstoffClass		stickstoffClass;	 

struct _stickstoff
{
	expertn_modul_base parent;
	/* add your public declarations here */
	
	int __ERROR;
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
double DENIT_REWET_FACTOR;
double DENIT_FROST_FACTOR;
double SimTime;

double fDryDays; 			// Maß für Intensität der Austrocknung 
double fReWetEvent;			// Bewertung des aktuellen (oder letzten) rewet-Events (zwischen 0 und 1) 
double fReWetDay;			// Tag des Rewetting
int    iReWetYear;			// Rewetting Jahr
int    iReWetDay;			// Simulationszeit des aktuellen (oder letzten) Rewet-Ereignisses 

int    lastiyear;			// year YYYY z.B. 1981
double lastfTimeY;
double fDailyPrecRate; // Tagesrate (precipitation) vom Vortag (aufintegriert)
double _fDailyPrecRate; // Tagesrate (temporäre Variable)
 
// Übergabeparameter:
double fN2Fraction;
double fN2OReduction;
int iRewet;
int iNO3Kin;

//double fMinerQ10;			//xchemist in xpn-3

// statics:
double fCDiffSum;


char *ini_filename;

};


#define Message(a,b) PRINT_MESSAGE(xpn,4,b);


// **********************************************************************************
//    LongPointer und Variablen-Abkürzungen
// **********************************************************************************

#define  N_SOIL_LAYERS  	N_FIRSTLAYER N_STOPLAYER N_NEXTLAYER 		
		
#define  N_ZERO_LAYER  \
            pSL = xpn->pSo->pSLayer,\
            pCL = xpn->pCh->pCLayer,\
            pCP = xpn->pCh->pCProfile,\
            pPA = xpn->pCh->pCParam,\
            pSW = xpn->pSo->pSWater,\
            pWL = xpn->pWa->pWLayer,\
            pHL = xpn->pHe->pHLayer,\
            pNF = xpn->pMa->pNFertilizer;		

#define  N_FIRSTLAYER  iLayer=1,\
              pSL = xpn->pSo->pSLayer->pNext,\
              pCL = xpn->pCh->pCLayer->pNext,\
              pSW = xpn->pSo->pSWater->pNext,\
              pWL = xpn->pWa->pWLayer->pNext,\
              pPA = xpn->pCh->pCParam->pNext,\
              pHL = xpn->pHe->pHLayer->pNext;
              
#define  N_STOPLAYER \
             ((iLayer<xpn->pSo->iLayers-1)&&\
              (pSL->pNext!=NULL)&&\
              (pCL->pNext!=NULL)&&\
              (pSW->pNext!=NULL)&&\
              (pWL->pNext!=NULL)&&\
              (pPA->pNext!=NULL)&&\
              (pHL->pNext!=NULL));
              
#define  N_NEXTLAYER \
              iLayer++,\
              pSL = pSL->pNext,\
		      pCL = pCL->pNext,\
		      pSW = pSW->pNext,\
		      pWL = pWL->pNext,\
		      pPA = pPA->pNext,\
		      pHL = pHL->pNext


struct _stickstoffClass
{
	expertn_modul_baseClass parent_class; 	
};


int stickstoff_load_config(stickstoff *self);
int stickstoff_load_ini(stickstoff *self);
int EmitAllN2O(stickstoff *self);

// public class member function:
//G_MODULE_EXPORT int stickstoff_Nitrification(stickstoff *self);
G_MODULE_EXPORT int stickstoff_load(stickstoff *self);
				  
G_MODULE_EXPORT int stickstoff_run(stickstoff *self);
G_MODULE_EXPORT int stickstoff_done(stickstoff *self);
G_MODULE_EXPORT int leachn_denit_load(stickstoff *self);
G_MODULE_EXPORT int leachn_denit_run_call(stickstoff *self);
				  int lechn_denit_integrate_daily_vars(stickstoff *self);
G_MODULE_EXPORT int leachn_denit_done(stickstoff *self);
G_MODULE_EXPORT int leachn_miner_load(stickstoff *self);	
G_MODULE_EXPORT int leachn_miner_run(stickstoff *self);	
G_MODULE_EXPORT int leachn_miner_done(stickstoff *self);
G_MODULE_EXPORT int leachn_hydro_load(stickstoff *self);	
G_MODULE_EXPORT int leachn_hydro_run(stickstoff *self);	
G_MODULE_EXPORT int leachn_hydro_done(stickstoff *self);

int leachn_denit_run(stickstoff *self, double fN2Fraction, double fN2OReduction, int iRewet, int iNO3Kin);

#define RAINY_DAY    (self->fDailyPrecRate > (double)10)
//#define RAINY_DAY    (pCl->pWeather->fRainAmount > (double)15)

#define NO3_KIN_SUPRALINEAR (iNO3Kin >= 2)
#define NO3_KIN_MM ((iNO3Kin == 1)||(iNO3Kin == 3))

#define  REWETTING_ENHANCES_MAX_DEN ((pPA->fDenitReWet> 0)&&((iRewet == 1)||(iRewet == 3)||(iRewet == 5)||(iRewet == 7)))
//#define  REWETTING_ENHANCES_MAX_DEN  FALSE

#define  REWETTING_RELEASES_NO3     ((pPA->fDenitReWet> 0)&&((iRewet == 2)||(iRewet == 3)||(iRewet == 6)||(iRewet == 7)))
#define  REWETTING_REDUCES_O2       ((pPA->fDenitReWet> 0)&&(iRewet >= 4))

//#define   REWETTING_ONLY_WITH_CROP
#undef   REWETTING_ONLY_WITH_CROP

#define TEST_RATE(x,y) if (y < EPSILON) {y = EPSILON; x = (double)0;}  \
                       else if ((y - x * DeltaT) < EPSILON) x = (y - EPSILON) / DeltaT;

#define C_DEBUG(var) \
	//xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i) \
	/*{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}\
*/


G_MODULE_EXPORT GType stickstoff_get_type (void);
G_END_DECLS

#endif /* __STICKSTOFF_H__ */

