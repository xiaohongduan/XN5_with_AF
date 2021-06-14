//      ceres_biomass_growth.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_BIOMASS_GROWTH_H__
#define __CERES_BIOMASS_GROWTH_H__

#include "ceres.h"

G_BEGIN_DECLS

int ceres_biomass_growth_run(ceres *self);
int ceres_CropMaintenance_run(ceres *self);
int ceres_BiomassPartition_run(ceres *self);

int ceres_root_length_growth_run(ceres *self);

int ceres_BiomassPartition_WH_BA(ceres *self);
int ceres_BiomassPartition_SF(ceres *self);
int ceres_BiomassPartition_MZ(ceres *self);

double ceres_IncLA_SF(ceres *self,double LeafExpandNum, double LeafTipNum, double TempMax, double TempMin);
int ceres_NitrogenGrain_SF(ceres *self);

int ceres_RootExtension(ceres *self);
int ceres_spass_RootLengthDensity(ceres *self);

int ceres_PlantSenescense(ceres *self);

G_END_DECLS

#endif /*__CERES_BIOMASS_GROWTH_H__*/