//      hpm_math.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM__MATH_H__
#define __HPM__MATH_H__

#include "hpm.h"

G_BEGIN_DECLS

#define EPSILON 1.0e-34



#define mq_TO_ha (1.0e-4)  // Quadratmeter nach ha
#define mq_TO_ha_1 (1.0e4) // 1 / m^2 nach 1 / ha
#define ha_TO_mq (1.0e4)  // ha nach m^2
#define ha_TO_mq_1 (1.0e-4)  // 1 / ha nach 1 / m^2

// pulse function: res: 0 or 1
double hpm_math_cpulse(double t, double t_begin, double period,double width);
double hpm_get_light_on(double Lm,double Lz,double DayLength, double fTimeDay);

double hpm_math_sum4(double *var);
// set all values of an array with count 4 to value

// x*x
#define QUAD(x) (x*x)

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

G_END_DECLS

#endif /* __HPM_H__ */
