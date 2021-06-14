//      transpn.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __TRANSPN_H__
#define __TRANSPN_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define TRANSPN_TYPE				(transpn_get_type())
#define TRANSPN(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			TRANSPN_TYPE, transpn))
#define TRANSPN_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			TRANSPN_TYPE, transpnClass))
#define IS_TRANSPN(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			TRANSPN_TYPE))
#define IS_TRANSPN_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			TRANSPN_TYPE))

typedef struct _transpn			transpn;
typedef struct _transpnClass		transpnClass;

struct _transpn
{
	expertn_modul_base parent;
	
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
					
	
};


// public class member function:
G_MODULE_EXPORT int transpn_flow(transpn *self);
G_MODULE_EXPORT int transpn_flow_init (transpn *self);

/**********************************************************************************************/
/* Procedur    :   NTransp                                                                    */
/*                                                                                            */
/* Inhalt      :   Stickstoffverlagerung nach Hutson & Wagenet (LeachN 1994-Version 3.0)      */
/*                                                                                            */
/* Autoren     :   GSF: S.Storm/E.Priesack                  V1.0                              */
/* Datum       :   24.05.94                                                                   */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var.		pCh->pCParam->afKd[0]                 pCh->pCParam->afKh[0]               */
/*					pCh->pCParam->afKd[1]                 pCh->pCParam->afKh[1]               */
/*					pCh->pCParam->afKd[2]                 pCh->pCParam->afKh[2]               */
/*					pCh->pCParam->fDispersivity           pCh->pCParam->fMolDiffCoef          */
/*					pCP->fN2OEmisR                        pCL->fN2OKh                         */
/*					pCL->fUreaN                           pCL->fNH4N                          */
/*					pCL->fNO3N                            pCL->fN2ON                          */
/*					pCL->fUreaNSoilConc                   pCL->fNH4NSoilConc                  */
/*					pCL->fNO3NSoilConc                    pCL->fN2ONGasConc                   */
/*					pCL->fUreaNSoilConcOld                pCL->fNH4NSoilConcOld               */
/*					pCL->fNO3NSoilConcOld                 pCL->fN2ONGasConcOld                */
/*					pSL->fImpedLiqA                       pSL->fImpedLiqB                     */
/*                  pCB->fNProfileStart                   pCB->fNInputCum                     */
/*                  pCP->fNO3NSurf                        pCP->fNH4NSurf                      */
/*                  pCP->fUreaNSurf                                                           */
/*                                                                                            */
/*					Get_Diffusion_Const()		pCL->fGasDiffCoef                             */
/*												pCL->fLiqDiffCoef                             */
/*					Solve_LGS()                 pCL->fLiqDiffCoef                             */
/*												pCL->fGasDiffCoef                             */
/*												pCL->fNH4NSoilConc                            */
/*												pCL->fNO3NSoilConc                            */
/*												pCL->fN2ONGasConc                             */
/*												pCL->fUreaNSoilConc                           */
/*                                                                                            */
/**********************************************************************************************/
int transpn_NTransp(transpn *self, int N2O_IN_SOLUTION);

/**********************************************************************************************/
/* Procedur    :   Get_Diffusion_Const()                                                      */
/* Beschreibung:   Berechne Diffusions-Koeffizienten                                          */
/*                                                                                            */
/**********************************************************************************************/
/* geänderte Var.	pCL->fGasDiffCoef                                                         */
/*					pCL->fLiqDiffCoef                                                         */
/**********************************************************************************************/
int transpn_get_Diffusion_Const(transpn *self);

/**********************************************************************************************/
/* Procedur    :   Solve_LGS()                                                                */
/* Beschreibung:   Löser ...                                                                  */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var		pCL->fLiqDiffCoef                                                         */
/*					pCL->fGasDiffCoef                                                         */
/*					pCL->fNH4NSoilConc                                                        */
/*					pCL->fNO3NSoilConc                                                        */
/*					pCL->fN2ONGasConc                                                         */
/*					pCL->fUreaNSoilConc                                                       */
/**********************************************************************************************/
int transpn_solve_LGS(transpn *self);


struct _transpnClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType transpn_get_type (void);

#define C_DEBUG(var) 
	//xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i)
	/*{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}*/
	

G_END_DECLS

#endif /* __TRANSPN_H__ */

