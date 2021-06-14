//      spass_canopy.h
//      
//      Copyright 2012 Florian Heinlein <florian.heinlein@helmholtz-muenchen.de>
//      



#ifndef __SPASS__CANOPY_H__
#define __SPASS__CANOPY_H__

#include "spass.h"

G_BEGIN_DECLS

//Assimilation, dissimilataion and growth
static int spass_MainStemLeafing(double fMaxLfInitRate,double fMaxLfEmergRate,double fTemp, double fDaylenEff, 
						      double fStageVR, PRESPONSE pResp,PCARDTEMP pCardTemp, POUTPUT pOut);
static double spass_LeafAreaGrowthRate(double fLfGrwRate,double  fStageVR,double fAveSpcLfWt, 
								   PRESPONSE pResp,PTRANSP pTransp, PORGANNC pLfNc);
static double spass_CerealTillering(double fLfEmergRate, double fExpLfNum, double fTillNum, double fTillDem, 
								double fStemWght,    double fPltDens,  double fStageVR, double DeltaT);

G_END_DECLS

#endif /* __SPASS_H__ */

