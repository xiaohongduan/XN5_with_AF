#ifndef __HYDRUS_MACRIOS_H__
#define __HYDRUS_MACRIOS_H__


/* function call for matrix potential:  */
///*
#define MATRIX_POTENTIAL(x) (float) (*self->MPotl)((double)pWL->fMatPotAct,\
        (double)x,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2)

#define WATER_CONTENT(x)  (float)(*self->WCont)((double)x,\
        (double)pWL->fContAct,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2)

#define CONDUCTIVITY(x) (\
                         (float)(*self->HCond)((double)x,\
                                 (double)pWL->fContAct,\
                                 (double)pSW->fCondSat,\
                                 (double)pSL->fPorosity,\
                                 (double)pSW->fContRes,\
                                 (double)pSW->fVanGenA,\
                                 (double)pSW->fVanGenN,\
                                 (double)pSW->fVanGenM,\
                                 (double)pSW->fCampA,\
                                 (double)pSW->fCampB,\
                                 (double)pSW->fPotInflec,\
                                 (double)pSW->fContInflec,\
                                 (double)pSW->fMinPot,\
                                 (double)pWL->fMatPotOld,\
                                 (double)pWL->fContOld,\
                                 (double)pSW->fVanGenA2,\
                                 (double)pSW->fVanGenN2,\
                                 (double)pSW->fVanGenM2,\
                                 (double)pSW->fBiModWeight1,\
                                 (double)pSW->fBiModWeight2))


#define NEXT_CONDUCTIVITY(x)  (\
                               (float)(*self->HCond)((double)x,\
                                       (double)pWL->pNext->fContAct,\
                                       (double)pSW->pNext->fCondSat,\
                                       (double)pSL->pNext->fPorosity,\
                                       (double)pSW->pNext->fContRes,\
                                       (double)pSW->pNext->fVanGenA,\
                                       (double)pSW->pNext->fVanGenN,\
                                       (double)pSW->pNext->fVanGenM,\
                                       (double)pSW->pNext->fCampA,\
                                       (double)pSW->pNext->fCampB,\
                                       (double)pSW->pNext->fPotInflec,\
                                       (double)pSW->pNext->fContInflec,\
                                       (double)pSW->pNext->fMinPot,\
                                       (double)pWL->pNext->fMatPotOld,\
                                       (double)pWL->pNext->fContOld,\
                                       (double)pSW->pNext->fVanGenA2,\
                                       (double)pSW->pNext->fVanGenN2,\
                                       (double)pSW->pNext->fVanGenM2,\
                                       (double)pSW->pNext->fBiModWeight1,\
                                       (double)pSW->pNext->fBiModWeight2))


/* function call for water capacity:  */
///*
#define CAPACITY(x) (float)(*self->DWCap)((double)x,\
        (double)pWL->fContAct,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2)


#define H2O_FIRST_LAYER \
	pSL = pSo->pSLayer->pNext,\
	      pSW = pSo->pSWater->pNext,\
	            pWL = pWa->pWLayer->pNext,\
	                  iLayer = 1;
//        pLR = pPl->pRoot->pLayerRoot->pNext,

#define H2O_UNTIL_BEFORE_LAST \
	((pSL->pNext != NULL)&&\
	 (pSW->pNext != NULL)&&\
	 (pWL->pNext != NULL));
//          (pLR->pNext != NULL));

#define H2O_UNTIL_LAST \
	((pSL != NULL)&&\
	 (pSW != NULL)&&\
	 (pWL != NULL));
//          (pLR != NULL)&&

#define NEXT_H2O_LAYER \
	pSL = pSL->pNext,\
	      pSW = pSW->pNext,\
	            pWL = pWL->pNext,\
	                  iLayer++
//           pLR = pLR->pNext,

//  layers  1 to n-1 :
#define H2O_SOIL_LAYERS  H2O_FIRST_LAYER H2O_UNTIL_BEFORE_LAST NEXT_H2O_LAYER

//  layers  0 to n :
#define H2O_ALL_LAYERS  H2O_ZERO_LAYER H2O_UNTIL_LAST NEXT_H2O_LAYER

//  layers  1 to n :
#define H2O_ALL_BUT_NOT_FIRST  H2O_FIRST_LAYER H2O_UNTIL_LAST NEXT_H2O_LAYER


#define  EPSILON  (float)1.0e-9


// Funktionen von ck:

#define free_ifn0(x) if (x!=NULL) free(x);

// Verschiedene Macros, die zur Kompatiblität zum Expert-N (Windows) definiert werden:

#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)




#endif
