//      ceres_transpiration.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_TRANSPIRATION_H__
#define __CERES_TRANSPIRATION_H__

#include "ceres.h"

G_BEGIN_DECLS

double ceres_PotentialTranspiration_run(ceres *self);
int ceres_ActualTranspiration_run(ceres *self);
int ceres_PlantWaterStress_run(ceres *self);

G_END_DECLS

#endif /*__CERES_TRANSPIRATION_H__*/