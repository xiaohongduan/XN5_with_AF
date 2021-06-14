//      daisy_miner.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __DAISY_MINER_H__
#define __DAISY_MINER_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define DAISY_MINER_TYPE				(daisy_miner_get_type())
#define DAISY_MINER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			DAISY_MINER_TYPE, daisy_miner))
#define DAISY_MINER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			DAISY_MINER_TYPE, daisy_minerClass))
#define IS_DAISY_MINER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			DAISY_MINER_TYPE))
#define IS_DAISY_MINER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			DAISY_MINER_TYPE))

typedef struct _daisy_miner			daisy_miner;
typedef struct _daisy_minerClass		daisy_minerClass;

struct _daisy_miner
{
	expertn_modul_base parent;
	/* add your public declarations here */
	// Hydraulic Functions:
	double (*WCont)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*HCond)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*DWCap)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
    double (*MPotl)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	
    //Hong: Temperaturabhängigkeit für DAISY Abbauraten (für Scott Demyan)
	int     iTempCorr;
	
	double  fParCAOM1Q10, fParCAOM2Q10,fParCBOM1Q10,fParCBOM2Q10,fParCSOM1Q10,fParCSOM2Q10,fParDBOM1Q10,fParDBOM2Q10,fParMBOM1Q10,fParMBOM2Q10;
	
	double  fParBOM1,fParSOM1,fParSOM2,fParEff;
	
	double  fEff_AOM1, fEff_AOM2, fEff_BOM1, fEff_BOM2, fEff_SOM1, fEff_SOM2; // changes 2016.10.06
	
	//End of Hong
	
	double  fCDiffSum, fNDiffSum; //added by Hong 20170517
		
	double SimTime; // ?
	char *ini_filename; // ?
	
	int __ERROR; //added by Hong on 20180124
};

#define Message(a,b) PRINT_MESSAGE(xpn,4,b);


// **********************************************************************************
//    LongPointer und Variablen-Abkürzungen
// **********************************************************************************

#define  N_SOIL_LAYERS  	N_FIRSTLAYER N_STOPLAYER N_NEXTLAYER 		
		
#define  N_ZERO_LAYER  \
            pSL = xpn->pSo->pSLayer,\
            pCL = xpn->pCh->pCLayer,\
            pCP = xpn->pCh->pCProfile,\
            pPA = xpn->pCh->pCParam,\
            pSW = xpn->pSo->pSWater,\
            pWL = xpn->pWa->pWLayer,\
            pHL = xpn->pHe->pHLayer,\
            pNF = xpn->pMa->pNFertilizer;		

#define  N_FIRSTLAYER  iLayer=1,\
              pSL = xpn->pSo->pSLayer->pNext,\
              pCL = xpn->pCh->pCLayer->pNext,\
              pSW = xpn->pSo->pSWater->pNext,\
              pWL = xpn->pWa->pWLayer->pNext,\
              pPA = xpn->pCh->pCParam->pNext,\
              pHL = xpn->pHe->pHLayer->pNext;
              
#define  N_STOPLAYER \
             ((iLayer<xpn->pSo->iLayers-1)&&\
              (pSL->pNext!=NULL)&&\
              (pCL->pNext!=NULL)&&\
              (pSW->pNext!=NULL)&&\
              (pWL->pNext!=NULL)&&\
              (pPA->pNext!=NULL)&&\
              (pHL->pNext!=NULL));
              
#define  N_NEXTLAYER \
              iLayer++,\
              pSL = pSL->pNext,\
		      pCL = pCL->pNext,\
		      pSW = pSW->pNext,\
		      pWL = pWL->pNext,\
		      pPA = pPA->pNext,\
		      pHL = pHL->pNext


#define DECLARE_COMMON_VAR \
	int OK            = 1;\
	int iLayer        = 0;\
	double DeltaT     = xpn->pTi->pTimeStep->fAct;

#define DECLARE_N_POINTER \
	PSLAYER      pSL;\
	PCLAYER      pCL;\
	PCPROFILE    pCP;\
	PCPARAM      pPA;\
	PSWATER      pSW;\
	PWLAYER      pWL;\
	PHLAYER      pHL;\
	PNFERTILIZER pNF;



#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)

// public class member function:
G_MODULE_EXPORT int daisy_miner_load(daisy_miner *self);
G_MODULE_EXPORT int daisy_miner_run(daisy_miner *self);
G_MODULE_EXPORT int daisy_miner_done(daisy_miner *self);

struct _daisy_minerClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType daisy_miner_get_type (void);

G_END_DECLS

#endif /* __DAISY_MINER_H__ */

