//      ceres_phenology.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_PHENOLOGY_H__
#define __CERES_PHENOLOGY_H__

#include "ceres.h"

G_BEGIN_DECLS

int ceres_phenological_development_run(ceres *self);
int ceres_DevelopmentCheckAndPostHarvestManagement_run(ceres *self);

void ceres_SetAllPlantRateToZero(ceres *self);

int ceres_Initialize_MZ_SF(ceres *self);
int ceres_DevelopmentBeforeEmergence(ceres *self);
double ceres_DailyThermTime(ceres *self);
int ceres_Germination(ceres *self);
double ceres_Emergence(ceres *self);
double ceres_Vernalization_CERES(ceres *self);
int ceres_DevelopmentBeforeEmergence(ceres *self);
int ceres_DevelopmentAfterEmergence(ceres *self);
double ceres_spass_DaylengthAndPhotoperiod(double fLatitude, int nJulianDay, int nID);
int ceres_CERES_TO_COMVR(ceres *self);

G_END_DECLS

#endif /*__CERES_PHENOLOGY_H__*/