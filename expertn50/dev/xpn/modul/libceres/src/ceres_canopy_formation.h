//      ceres_canopy_formation.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_CANOPY_FORMATION_H__
#define __CERES_CANOPY_FORMATION_H__

#include "ceres.h"

G_BEGIN_DECLS

int ceres_canopy_formation_run(ceres *self);

int ceres_PlantLeafNumber_WH_BA(ceres *self);
int ceres_PlantLeafArea_WH_BA(ceres *self);
int ceres_LeafSenescence_WH_BA(ceres *self);
int ceres_Tillering_WH_BA(ceres *self);
int ceres_TillerDeath_WH_BA(ceres *self);
int ceres_PlantLeafNumber_MZ(ceres *self);
int ceres_LeafSenescence_MZ(ceres *self);
int ceres_PlantLeafNumber_SF(ceres *self);
int ceres_LeafSenescence_SF(ceres *self);
G_END_DECLS

#endif /*__CERES_CANOPY_FORMATION_H__*/