//      spass_phenology.h
//      
//      Copyright 2012 Florian Heinlein <florian.heinlein@helmholtz-muenchen.de>
//      

#ifndef __SPASS__PHENOLOGY_H__
#define __SPASS__PHENOLOGY_H__

#include "spass.h"

G_BEGIN_DECLS

static int				spass_Germination(spass *self);

//Biological processes
static double			spass_ThermalDevelopment(double fTemp,PRESPONSE pResp,double fMinTemp,double fOptTemp,double fMaxTemp, spass *self);
static double			spass_DailyVernalization(double fTemp, PRESPONSE pResp, double fMinTemp, double fOptTemp, double  fMaxTemp, spass *self);
static double			spass_VernalizationEffect(double fCumVern, double fBasVern, double fCrtVern);
static double			spass_Photoperiodism(double fDaylen,PRESPONSE pResp, double fOptDaylen,double fSensF,char* acCropCode);

static int				spass_COMVR_TO_ZSTAGE_AND_CERES_WHEAT(spass *self);
static int				spass_COMVR_TO_ZSTAGE_AND_CERES_POTATO(spass *self);
static int				spass_COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(spass *self);

G_END_DECLS

#endif /* __SPASS_H__ */
