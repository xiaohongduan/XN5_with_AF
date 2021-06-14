//      ceres_photosynthesis.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __CERES_PHOTOSYNTHESIS_H__
#define __CERES_PHOTOSYNTHESIS_H__

#include "ceres.h"

G_BEGIN_DECLS

int ceres_DailyCanopyGrossPhotosynthesis_run(ceres *self);

int ceres_DailyCanopyPhotosynthesis_WH_BA(ceres *self);
int ceres_DailyCanopyPhotosynthesis_MZ(ceres *self);
int ceres_DailyCanopyPhotosynthesis_SF(ceres *self);

G_END_DECLS

#endif /*__CERES_PHOTOSYNTHESIS_H__*/