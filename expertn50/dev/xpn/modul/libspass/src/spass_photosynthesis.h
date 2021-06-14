//      spass_photosynthesis.h
//      
//      Copyright 2012 Florian Heinlein <florian.heinlein@helmholtz-muenchen.de>
//      



#ifndef __SPASS__PHOTOSYNTHESIS_H__
#define __SPASS__PHOTOSYNTHESIS_H__

#include "spass.h"


//Asiimilation, dissimilation and growth
static double			spass_LeafMaxGrossPhotosynthesis(double fPgmmax,double fTemp,double fCO2,double fCO2Cmp,double fCO2R,
										   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, PPLANT pPl, spass *self);

static double			spass_LeafLightUseEfficiency(double fMaxLUE, double fTemp, double fCO2, char* pType);
static double			spass_CanopyGrossPhotosynthesis(spass *self,double fPgMax, double fLUE, double fLAI, double fLatitude,
										  double nDay,double fHour, double fPARdir,double fPARdif, PPLANT pPl, PTIME pTi);

static double			spass_DailyCanopyGrossPhotosythesis(char* pType,double fPgmmax,double fMaxLUE,double fCO2Cmp,double fCO2R,
											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, double fLAI,
											  double fLatitude,double Day,double mday, int hour,
											double fRad,double fTemp,double fCO2, spass *self);

G_END_DECLS

#endif /* __SPASS_H__ */

