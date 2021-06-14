//      ceres_nitrogen.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_NITROGEN_H__
#define __CERES_NITROGEN_H__

#include "ceres.h"

G_BEGIN_DECLS

int ceres_NitrogenConcentrationLimits(ceres *self);
int ceres_PlantNitrogenDemand(ceres *self);

int ceres_NitrogenUptake_run(ceres *self);
int ceres_PlantNitrogenStress_run(ceres *self);

double ceres_AFGENERATOR(double fInput, RESPONSE* pResp);

G_END_DECLS

#endif /*__CERES_NITROGEN_H__*/