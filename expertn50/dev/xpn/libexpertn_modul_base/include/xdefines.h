/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:  Christian Haberbosch
 *
 *------------------------------------------------------------------------------
 *
 * Description: 
 *
 *   Sammlung der in verschiedenen Projekten benötigten Definitionen.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: xdefines.h $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:32
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * MAX_SIM_DAYS defined twice (removed).
 * 
 *   14.9.95, 18.9.95, 5.10.95, 4.11.95, 15.4.96
 *
*******************************************************************************/
 
// ANMERKUNG CH. BAUER:
// Es gibt zu viele "define" Header Files (define.h, xdefine.h, defines.h, xdefines.h)
// mit sich ueberschneidenden und doppelten Definitionen. Wird Zeit, dass hier mal
// aufgeraeumt wird, um exception faults zu reduzieren.

#ifndef _INC_EXPERTN_XDEFINES_H
#define _INC_EXPERTN_XDEFINES_H

#include "xpn_defines.h"


// **********************************************************************************
//    Programmbeeinflussende defines  (Schalter)
// **********************************************************************************

//#undef debug
//#define  Messages 0

// **********************************************************************************
//    Konstanten
// **********************************************************************************
/*
#define  EPSILON         1e-9
#define  GradToRad       0.0174533
#define  MAXSCHICHT       300//100*/
#ifndef PI
#define  PI 3.14159265358979323846
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define max(x,y) MAX(x,y)
#define min(x,y) MIN(x,y)

//#define LOGFILE                                           
     



// **********************************************************************************
//    LongPointer und Variablen-Abkürzungen
// **********************************************************************************

#define EXP_POINTER PCHEMISTRY pCh,PCLIMATE pCl,PHEAT pHe,PLOCATION pLo,PMANAGEMENT pMa,PPLANT pPl,PSPROFILE pSo,PTIME pTi,PWATER pWa 
#define exp_p pCh,pCl,pHe,pLo,pMa,pPl,pSo,pTi,pWa

#define RET_SUCCESS 0

#define DECLARE_XPN \
        expertn_modul_base *xpn = &(self->parent); \
        PCHEMISTRY pCh = xpn->pCh; \
        PCLIMATE pCl = xpn->pCl; \
        PHEAT pHe = xpn->pHe ; \
        PLOCATION pLo = xpn->pLo; \
        PMANAGEMENT pMa = xpn->pMa; \
        PPLANT pPl = xpn->pPl; \
        PSPROFILE pSo = xpn->pSo; \
        PTIME pTi = xpn->pTi; \
        PWATER pWa = xpn->pWa;





#define SOIL_LAYERS0(x,y) (x=y, iLayer = 0);(x->pNext!=NULL)&&(iLayer < pSo->iLayers-1);(x = x->pNext, iLayer++)
#define SOIL_LAYERS1(x,y) (x=y, iLayer = 1);(x->pNext!=NULL)&&(iLayer < pSo->iLayers-1);(x = x->pNext, iLayer++)

#define SOIL2_LAYERS0(v,w,x,y) (v=w, x=y, iLayer = 0);(x->pNext!=NULL)&&(v->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1);\
							(v = v->pNext, x = x->pNext, iLayer++)
#define SOIL2_LAYERS1(v,w,x,y) (v=w, x=y, iLayer = 1);(x->pNext!=NULL)&&(v->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1);\
							(v = v->pNext, x = x->pNext, iLayer++)



// #define Top3Layers(x,y) x->pNext->y + x->pNext->pNext->y + x->pNext->pNext->pNext->y 
// #define Top6Layers(x,y) x->pNext->y + x->pNext->pNext->y + x->pNext->pNext->pNext->y + x->pNext->pNext->pNext->pNext->y + x->pNext->pNext->pNext->pNext->pNext->y + x->pNext->pNext->pNext->pNext->pNext->pNext->y


#endif // #ifndef _INC_EXPERTN_XDEFINES_H

/**********************************************************************************
** EOF */
