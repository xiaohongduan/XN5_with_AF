//modifizierte Kopie von transpn_util.h

#ifndef __STICKSTOFF_UTIL_H__
#define __STICKSTOFF_UTIL_H__


 
#include <xpn_register_var.h>

typedef struct ncorr  *LPNCORR;
struct ncorr  {
        double             DNrel,DNO3rel,DN2Orel;
        double             Feucht;
        double             Temp;
        double             NO3;
        double             ph,ph2;
        double             Csol;
        double             Lehm; 
        double             Frost; 
        double             WiederBefeucht;
		double			  Rate;
              };
			  

#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)
#define EPSILON 1.0e-9
#define kgPhaTomgPsqm 100.0

//Error Definition (vielleicht gibts da auch noch einen besseren Ort dafür)
#define   ERROR_NOT_POSITIVE_TXT   	    "Variable 0 or Negative!"


#define WATER_CONTENT(x) (*self->WCont)((double)x,\
        (double)pWL->fContAct,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2)
		
#define CONDUCTIVITY(x) (\
                     (double)(*self->HCond)((double)x,\
                     (double)pWL->fContAct,\
                     (double)pSW->fCondSat,\
                     (double)pSL->fPorosity,\
                     (double)pSW->fContRes,\
                     (double)pSW->fVanGenA,\
                     (double)pSW->fVanGenN,\
                     (double)pSW->fVanGenM,\
                     (double)pSW->fCampA,\
                     (double)pSW->fCampB,\
                     (double)pSW->fPotInflec,\
                     (double)pSW->fContInflec,\
                     (double)pSW->fMinPot,\
                     (double)pWL->fMatPotOld,\
                     (double)pWL->fContOld,\
                     (double)pSW->fVanGenA2,\
                     (double)pSW->fVanGenN2,\
                     (double)pSW->fVanGenM2,\
                     (double)pSW->fBiModWeight1,\
                     (double)pSW->fBiModWeight2))
					 
/* function call for hydraulic conductivity of next layer:                       */
///*
#define NEXT_CONDUCTIVITY(x)  (\
                     (double)(*self->HCond)((double)x,\
                     (double)pWL->pNext->fContAct,\
                     (double)pSW->pNext->fCondSat,\
                     (double)pSL->pNext->fPorosity,\
                     (double)pSW->pNext->fContRes,\
                     (double)pSW->pNext->fVanGenA,\
                     (double)pSW->pNext->fVanGenN,\
                     (double)pSW->pNext->fVanGenM,\
                     (double)pSW->pNext->fCampA,\
                     (double)pSW->pNext->fCampB,\
                     (double)pSW->pNext->fPotInflec,\
                     (double)pSW->pNext->fContInflec,\
                     (double)pSW->pNext->fMinPot,\
                     (double)pWL->pNext->fMatPotOld,\
                     (double)pWL->pNext->fContOld,\
                     (double)pSW->pNext->fVanGenA2,\
                     (double)pSW->pNext->fVanGenN2,\
                     (double)pSW->pNext->fVanGenM2,\
                     (double)pSW->pNext->fBiModWeight1,\
                     (double)pSW->pNext->fBiModWeight2))

/* function call for water capacity:  */
///*
#define CAPACITY(x) (double)(*self->DWCap)((double)x,\
                     (double)pWL->fContAct,\
                     (double)pSW->fCondSat,\
                     (double)pSW->fContSat,\
                     (double)pSW->fContRes,\
                     (double)pSW->fVanGenA,\
                     (double)pSW->fVanGenN,\
                     (double)pSW->fVanGenM,\
                     (double)pSW->fCampA,\
                     (double)pSW->fCampB,\
                     (double)pSW->fPotInflec,\
                     (double)pSW->fContInflec,\
                     (double)pSW->fMinPot,\
                     (double)pWL->fMatPotOld,\
                     (double)pWL->fContOld,\
                     (double)pSW->fVanGenA2,\
                     (double)pSW->fVanGenN2,\
                     (double)pSW->fVanGenM2,\
                     (double)pSW->fBiModWeight1,\
                     (double)pSW->fBiModWeight2)
			  
/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double  abspowerDBL(double x, double y);

/***********************************************************************************/
/* Setzt x auf EPSILON falls x<EPSILON und gibt Meldung aus						   */
/***********************************************************************************/

int Set_At_Least_Epsilon(double *x, const char *Message);

/***********************************************************************************/
/* Procedur    :   RelAnteil()                                                     */     
/* Beschreibung:   Von zwei echt positiven Zahlen x, y                             */
/*        wird der rel. Anteil von x an der Summe x+y berechnet.                   */
/*                                                                                 */
/*              GSF: ch                     14.9.95                                */
/***********************************************************************************/

double RelAnteil(double x, double y);

/***********************************************************************************/
/* Procedur    :   Test_Range()                                                    */      
/*                                                                                 */
/* Output:      0  Variable ausserhalb des Bereichs                                */
/*              1  Variable plausibel.                                             */
/*                                                                                 */
/*              GSF: ch         7.3.97                                             */
/***********************************************************************************/
int Test_Range(double x, double x1, double x2,const char* error);

/************************************************************************************/
/* Procedur    :   Polygon4()                                                      */
/* Op.-System  :   DOS                                                             */
/* Beschreibung:   Polygonzug mit 4 Eckpunkten als Korrekturfunktion               */
/*                 für Feuchte und Temperatur.                                     */
/*                                                                                 */
/*              GSF: ch                     14.9.95                                */
/***********************************************************************************/
double Polygon4(double x, double x0, double y0, double x1,double y1,double x2,double y2,double x3,double y3);

/*********************************************************************************************/
/* Procedur    :   Solut_Coeff()                                                             */
/* Beschreibung:   Approximiere Löslichkeit von N2O in Abhängigkeit von der Temperatur       */
/* Nach        :   Moraghan und Buresh (1977)                                                */
/* aus         :   Dissertation C. Braun (1994)                                              */
/*********************************************************************************************/
double Solut_Coeff(double Temp);

#ifndef INFINITY
#define INFINITY get_infinity()
#endif

#define CHECK_DOUBLE_INVALID(val) ((val==-INFINITY)||(val==+INFINITY)||(val!=val))

// function used from the macro INFINITY
double get_infinity();

#define xpn_register_var_check_inval(self) xpn_register_var_check_inval_(self,__FILE__,__LINE__);
void xpn_register_var_check_inval_(xpn_register_var *self,const char *FILE, const int LINE );


#endif






