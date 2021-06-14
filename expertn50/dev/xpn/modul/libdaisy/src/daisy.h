//      daisy.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __DAISY_H__
#define __DAISY_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define DAISY_TYPE				(daisy_get_type())
#define DAISY(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			DAISY_TYPE, daisy))
#define DAISY_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			DAISY_TYPE, daisyClass))
#define IS_DAISY(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			DAISY_TYPE))
#define IS_DAISY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			DAISY_TYPE))

typedef struct _daisy			daisy;
typedef struct _daisyClass		daisyClass;

struct _daisy
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double fCond_To_Cap;
	int __ERROR;
    
    int Frost_Rad_Flag;
    int Lower_boundary_cond;
	int too_high_or_low_temp;
};



// public class member function:
G_MODULE_EXPORT int daisy_load(daisy *self);
G_MODULE_EXPORT int daisy_run(daisy *self);

G_MODULE_EXPORT int daisy_calc_sensible_heat(daisy *self);
G_MODULE_EXPORT int daisy_calc_latent_heat(daisy *self);
G_MODULE_EXPORT int daisy_calc_latent_heat2(daisy *self);

G_MODULE_EXPORT int daisy_st_first_soil_layer(daisy *self);
G_MODULE_EXPORT int daisy_st_horten_from_balance(daisy *self);

double daisy_Get_Horton_Surface_Energy_Balance(daisy *self, double fTS);

struct _daisyClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType daisy_get_type (void);

#define  EPSILON  1.0e-9
#define C_DEBUG(var)\
\
	if CHECK_double_INVALID(var)\
	{\
	  char *S;\
	  S = (char*)malloc(250+sizeof(#var));\
	  sprintf(S,"Variable %s hat den Wert: %f",#var,var);\
	  PRINT_ERROR(S);\
	  free(S);\
	  exit(1);\
	} else xpn_register_var_add_double(xpn->pXSys->var_list,#var,var,__LINE__,TRUE);

	
#define CHECK_double_INVALID(val) ((val==-INFINITY)||(val==+INFINITY)||(val!=val))



// DAYSY specifisch:

int get_Cond_Cap_Heat(expertn_modul_base *xpn,EXP_POINTER);
int Solve_LGS(daisy *self,EXP_POINTER, int Frost_Rad_Flag);
double daisy_Get_Daisy_LowerBC(daisy *self,EXP_POINTER);
double  abspower(double x, double y);

#define THETA_MIN 0.1 
//define EPSILON 1.0e-9

#define FREEZING_OR_THAWING (pWa->pWLayer->fIce > EPSILON)

#define DENSITY_WATER   1.0     /* Dichte von Wasser [kg/l] */
#define DENSITY_ICE     0.92  /* Dichte von Eis    [kg/l] */
#define DENSITY_MINERAL 2.65  /* Dichte von Mineralien   [kg/l] */
#define DENSITY_HUMUS   1.30  /* Dichte von org. Substanz   [kg/l] */
    
#define SPECCAP_WATER   4192.0  /* Spez. Waermekap. von Wasser      [J/(kg K)] */
#define SPECCAP_ICE     2050.0  /* Spez. Waermekap. von Eis         [J/(kg K)] */
#define SPECCAP_MINERAL 750.0   /* Spez. Waermekap. von Mineralien  [J/(kg K)] */
#define SPECCAP_HUMUS   1926.0  /* Spez. Waermekap. Soil organic matter  [J/(kg K)] */

#define LAT_HEAT_MELT   334.0       /* Spezifische Schmelzwaerme (Kuchling, p. 603) */
                                         /* [kJ/kg]        */

#define DECLARE_TEMP_POINTER \
        PSLAYER pSL; \
        PSWATER pSW; \
        PWLAYER pWL; \
        PHLAYER pHL; \
        PHBALANCE pHB;
		
#define DECLARE_COMMON_VAR \
        int OK            = 1;\
        int iLayer        = 0;\
        double DeltaT     = pTi->pTimeStep->fAct;\
        double SimTime    = pTi->pSimTime->fTimeAct;
		
#define TEMP_ZERO_LAYER \
        pSL = pSo->pSLayer,\
        pSW = pSo->pSWater,\
        pWL = pWa->pWLayer,\
        pHL = pHe->pHLayer,\
        pHB = pHe->pHBalance,\
        iLayer = 0;
#define TEMP_FIRST_LAYER \
        pSL = pSo->pSLayer->pNext,\
        pSW = pSo->pSWater->pNext,\
        pWL = pWa->pWLayer->pNext,\
        pHL = pHe->pHLayer->pNext,\
        pHB = pHe->pHBalance,\
        iLayer = 1;  

// HL, SL, SW, WL
#define TEMP_UNTIL_BEFORE_LAST \
        ((pSL->pNext != NULL)&&\
         (pSW->pNext != NULL)&&\
         (pWL->pNext != NULL)&&\
         (pHL->pNext != NULL));


#define TEMP_UNTIL_LAST \
        ((pSL != NULL)&&\
         (pSW != NULL)&&\
         (pWL != NULL)&&\
         (pHL != NULL));

          
#define NEXT_TEMP_LAYER \
        pSL = pSL->pNext,\
        pSW = pSW->pNext,\
        pWL = pWL->pNext,\
        pHL = pHL->pNext,\
        iLayer++    


// Alle hier relevanten Zeiger werden von Schicht 1 bis n-1 benutzt:
#define TEMP_SOIL_LAYERS  TEMP_FIRST_LAYER TEMP_UNTIL_BEFORE_LAST NEXT_TEMP_LAYER

// Alle hier relevanten Zeiger werden von Schicht 0 bis n-1 benutzt:
#define TEMP_ALL_BUT_NOT_LAST  TEMP_ZERO_LAYER TEMP_UNTIL_BEFORE_LAST NEXT_TEMP_LAYER
                               
// Alle hier relevanten Zeiger werden von Schicht 0 bis n benutzt:
#define TEMP_ALL_LAYERS  TEMP_ZERO_LAYER TEMP_UNTIL_LAST NEXT_TEMP_LAYER

// Alle hier relevanten Zeiger werden von Schicht 1 bis n benutzt:
#define TEMP_ALL_BUT_NOT_FIRST  TEMP_ZERO_LAYER TEMP_UNTIL_BEFORE_LAST NEXT_TEMP_LAYER

#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)

G_END_DECLS

#endif /* __DAISY_H__ */

