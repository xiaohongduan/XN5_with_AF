/*******************************************************************************
 *
 * Copyright  (c) by
 *
 * Author:      Sperr Chr.
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Priesack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Simulation time objects
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: xchemist.h $
 *
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:06
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 *
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XCHEMIST_H
#define _INC_EXPERTN_XCHEMIST_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

#define MAX_DEPTH_PER_MEASURE  10 //ep 170999 maximal number of start value horizons = 10

typedef struct cLayer  *PCLAYER;
typedef struct cLayer
{
	double	fNO3N; // kg/ha Nitrat-N-Gehalt
	double	fNO3NAds;  // kg/ha Adsorbierter Nitrat-N-Gehalt
	double	fNH4N; // kg/ha Ammonium-N-Gehalt
	double	fNH4NAds; // kg/ha Adsorbierter Ammonium-N-Gehalt
	double	fNH4NGas; // kg/ha Gasförmiger Ammonium-N-Gehalt
	double	fUreaN; // kg/ha Harnstoff-N-Gehalt
	double	fUreaNAds; // kg/ha Adsorbierter Harnstoff-N-Gehalt
	double	fDOC; // ?
	double	fDOCAds; // ?
	double	fDON; // ?
	double	fDONAds; // ?
	double  fDOMCN; // ?
	double	fNH4Manure; // ?
	double	fNManure; // kg/ha N-Gehalt des organischern Düngers
	double	fCManure; // kg/ha  C-Gehalt des organischern Düngers
	double	fManureCN; // kg/ha C:N Verhältnis des organischen Düngers
	double	fNLitter; // kg/ha N-Gehalt der Pflanzenreste
	double	fCLitter; // kg/ha C-Gehalt der Pflanzenreste
	double	fLitterCN; // kg/ha C:N Verhältnis der Pflanzenreste
	double	fN2ON; // kg/ha N2O-N Gehalt
	double  fN2ONGasConc;  // mmgN/l  N2O-Konzentration in Bodenluft
	double  fN2ONGasConcOld;// mmgN/l  N2O-Konzentration in Bodenluft im vorigen Zeitschritt
	double	fN2N;  // kg/ha N2-N Gehalt
	double  fN2NGasConc;  // ?
	double  fN2NGasConcOld;  // ?
	double  fNON; // ?
	double  fNONGasConc;  // ?
	double  fNONGasConcOld;  // ?
	double	fCO2C;  // kg/ha CO2-C Gehalt
	double  fCO2C_dt; // kg/ha CO2-C Gehalt in each time step, for century_n
	double	fCsolC;  // kg/ha  Gelöster Kohlenstoff
	double	fNsolC;  //  ?
	double	fCFreeFOM; // kg/ha Freier C-Gehalt aus frischer org. Substanz
	double	fCFreeHumus; // kg/ha Freier C-Gehalt aus Humus
	double	afNOrgFOMFrac[3]; // kg/ha
	double	afCOrgFOMFrac[3]; // kg/ha
	double  fNFOMVeryFast; // kg/ha
	double  fCFOMVeryFast; // kg/ha
	double	fNFOMFast; // kg/ha
	double	fCFOMFast; // kg/ha
	double	fNFOMSlow; // kg/ha
	double	fCFOMSlow; // kg/ha
	double	fNMicBiomFast; // kg/ha
	double	fCMicBiomFast; // kg/ha
	double	fNMicBiomSlow;
	double	fCMicBiomSlow;
	double	fNMicBiomDenit;
	double	fCMicBiomDenit;
	double  fNHumusFast;
	double  fCHumusFast;
	double  fNHumusSlow;
	double  fCHumusSlow;
	double  fNHumusStable;
	double  fCHumusStable;
	double  fNStrcLitter;
	double  fCStrcLitter;
	double  fLignFracStrcLit;
	double  fNMtbLitter;
	double  fCMtbLitter;
	double  fNGrossRootLitter;
	double  fCGrossRootLitter;
	double  fCNGrossRootLitter;//added by Hong on 26032019 for agroforestry
	double  fNFineRootLitter;
	double  fCFineRootLitter;
	double  fCNFineRootLitter;//added by Hong on 26032019 for agroforestry
	double	fChemX;
	double	fNO3Nmgkg;
	double	fNH4Nmgkg;
	double	fUreaNmgkg;
	double	fChemXmgkg;
	double	fNO3NSoilConc;
	double	fNO3NAdsConc;
	double	fNH4NSoilConc;
	double	fNH4NAdsConc;
	double	fNH4NGasConc;
	double	fUreaNSoilConc;
	double	fUreaNAdsConc;
	double	fUreaNGasConc;
	double	fDOCSoilConc;
	double	fDOCAdsConc;
	double	fDONSoilConc;
	double	fDONAdsConc;
	double	fNO3NSoilConcOld;
	double	fNO3NAdsConcOld;
	double	fNH4NSoilConcOld;
	double	fNH4NAdsConcOld;
	double	fNH4NGasConcOld;
	double	fUreaNSoilConcOld;
	double	fUreaNAdsConcOld;
	double	fUreaNGasConcOld;
	double	fDOCSoilConcOld;
	double	fDOCAdsConcOld;
	double	fDONSoilConcOld;
	double	fDONAdsConcOld;
	double  fDOCSltConcMob;
	double  fDOCSltConcImm;
	double  fDOCAdsConcMob;
	double  fDOCAdsConcImm;
	double  fDOCSltConcMobOld;
	double  fDOCSltConcImmOld;
	double  fDOCAdsConcMobOld;
	double  fDOCAdsConcImmOld;
	double	fNO3DenitMaxR;
	double	fN2ODenitMaxR;
	double	fNH4NitrMaxR;
	double	fUreaHydroMaxR;
	double	fLitterMinerMaxR;
	double	fManureMinerMaxR;
	double	fHumusMinerMaxR;
	double  fHumusToDONMaxR;
	double  fLitterToDONMaxR;
	double  fManureToDONMaxR;
	double  fDONToHumusMaxR;
	double  fDONToLitterMaxR;
	double  fDONMinerMaxR;
	double	fNO3DenitR;
	double	fN2ODenitR;
	double	fNH4NitrR;
	double	fUreaHydroR;
	double  fMinerR;
	double	fLitterMinerR;
	double	fManureMinerR;
	double	fHumusMinerR;
	double  fHumusToDONR;
	double  fLitterToDONR;
	double  fManureToDONR;
	double  fDONToHumusR;
	double  fDONToLitterR;
	double  fDONMinerR;
	double  fFOMVeryFastDecMaxR;
	double	fFOMFastDecMaxR;
	double	fFOMSlowDecMaxR;
	double	fMicBiomFastDecMaxR;
	double	fMicBiomSlowDecMaxR;
	double	fBioMFastNonlinDecMaxR;
	double	fBioMSlowNonlinDecMaxR;
	double	fHumusFastMaxDecMaxR;
	double	fHumusSlowMaxDecMaxR;
	double	fUreaNVolatR;
	double	fNH4VolatR;
	double	fNO3VolatR;
	double  fNImmobR;
	double  fNLitterImmobR;
	double  fNManureImmobR;
	double  fNHumusImmobR;
	double	fCO2ProdR;
	double	afFOMDecR[3];
	double	fHumusDecR;
	double	fNO3DenitDay;
	double	fN2ODenitDay;
	double	fNH4NitrDay;
	double	fNImmobDay;
	double	fUreaHydroDay;
	double	fNManureMinerDay;
	double	fNLitterMinerDay;
	double	fNHumusMinerDay;
	double	fCManureMinerDay;
	double	fCLitterMinerDay;
	double	fCHumusMinerDay;
	double	fCO2ProdDay;
	double	fUreaLeachDay;
	double	fNH4LeachDay;
	double	fNO3LeachDay;
	double	fUreaVolatDay;
	double	fNH4VolatDay;
	double	fNO3VolatDay;
	double	fNO3DenitCum;
	double	fN2ODenitCum;
	double	fNH4NitrCum;
	double	fNImmobCum;
	double	fUreaHydroCum;
	double	fNManureMinerCum;
	double	fNLitterMinerCum;
	double	fNHumusMinerCum;
	double	fCManureMinerCum;
	double	fCLitterMinerCum;
	double	fCHumusMinerCum;
	double	fCO2ProdCum;
	double	fNAnorg;
	double	fNH4NitrCoeff;
	double	fCNCoeff;
	double	fH2ONitrCoeff;
	double	fpHCoeff;
	double	fTempCoeff;
	double	fTempCoeffOld;
	double  fGasDiffCoef;
	double  fLiqDiffCoef;
	double	fH2ONitrCoeff_2;
	double	fPpmTOkg;
	double  fNH4ToN2OR;
	double  fNH4ToN2OCum;
	double  fMicBiomCN;
	double  fN2ONGasFlux;
	double  fN2ONGasFluxOld;
	double  fGasDiffusiv;
	double  fN2OKh;
	double	fCSoilMicBiom;
	double	fNSoilMicBiom;
	PCLAYER pNext;
	PCLAYER pBack;
}
CLAYER;


typedef struct nMeasure   *PNMEASURE;
typedef struct nMeasure
{
	int	lDate;
	int		iDay;
	double	afDepth[MAX_DEPTH_PER_MEASURE];
	double	afNO3N[MAX_DEPTH_PER_MEASURE];
	double	afNH4N[MAX_DEPTH_PER_MEASURE];
	double	afN2ON[MAX_DEPTH_PER_MEASURE];
	double	afN2ONEmis[MAX_DEPTH_PER_MEASURE];
	double	afNH4NEmis;
	PNMEASURE  pNext;
	PNMEASURE  pBack;
}
NMEASURE;

typedef struct cBalance   *PCBALANCE;
typedef struct cBalance
{
	double  fNO3NProfile;
	double  fNH4NProfile;
	double  fUreaNProfile;
	double  fNONProfile;
	double  fN2ONProfile;
	double  fN2NProfile;

	double	fNProfileStart;
	double	dNProfile;
	double	dNBalance;
	double	dNBalCorrect;

	double	fCProfileStart;
	double	dCProfile;
	double	dCBalance;
	//Added by Hong on 20180731, for C-balance
	double  fDOCProfile;
	double  fCsolProfile;
	double  fCLitterProfile;
    double  fCManureProfile;
	double  fCHumusProfile;
	double  dCBalCorrect; 
	//Hong 20190507: balance for 0-30 cm profile
	double	fCProfileStart_30;
	double	dCProfile_30;
	double	dCBalance_30;
	double  fDOCProfile_30;
	double  fCsolProfile_30;
	double  fCLitterProfile_30;
    double  fCManureProfile_30;
	double  fCHumusProfile_30;

	double	dCInputCum;
	double	dCInputSurf;
	double	dCInputProfile;
	double	dCOutputCum;
	double	dCInputCum_30;//Hong 20190507: balance for 0-30 cm profile
	double	dCInputProfile_30;
	double	dCOutputCum_30;
	double	dNInputCum;
	double	dNOutputCum;

	double  fNInputSoil;
	double  fNOutputSoil;
	double  fNBalanceSoil;
}
CBALANCE;

typedef struct cProfile   *PCPROFILE;
typedef struct cProfile
{
	double	fNO3NSurf;
	double	fNH4NSurf;
	double	fUreaNSurf;
	double	fDOCSurf;
	double	fDONSurf;
	double	fCNDOMSurf;
	double	fDryMatterLitterSurf;
	double	fNLitterSurf;
	double	fCLitterSurf;
	double	fCNLitterSurf;
	double	fDryMatterManureSurf;
	double	fNH4ManureSurf;
	double	fNManureSurf;
	double	fCManureSurf;
	double	fCNManureSurf;
	double	fDryMatterHumusSurf;
	double	fCHumusSurf;
	double	fNHumusSurf;
	double	fCNHumusSurf;
	double  fCStandCropRes;
	double  fNStandCropRes;
	double  fCStrLitterSurf;
	double  fNStrLitterSurf;
	double  fLignFracStrLitSurf;
	double  fCMtbLitterSurf;
	double  fNMtbLitterSurf;
	double  fCMicLitterSurf;
	double  fNMicLitterSurf;
	double  fCLeafLitterSurf;
	double  fNLeafLitterSurf;
	double  fCNLeafLitterSurf; //added by Hong on 26032019 for agroforestry
	double  fCBranchLitterSurf;
	double  fNBranchLitterSurf;
	double  fCNBranchLitterSurf;//added by Hong on 26032019 for agroforestry
	double  fCStemLitterSurf;
	double  fNStemLitterSurf;
	double  fCNStemLitterSurf;//added by Hong on 26032019 for agroforestry
	double	fCLitter;
	double	fNLitter;
	double	fCNLitter;
	double	fCManure;
	double	fNManure;
	double	fCNManure;
	double	fCHumus;
	double	fNHumus;
	double	fCNHumus;
	double  fDON;
	double  fDOC;
	double	fCSoilMicBiom;
	double	fNSoilMicBiom;
	double	fCNSoilMicBiom;
	double  fLigFracLeaf;
	double  fLigFracBranch;
	double  fLigFracStem;
	double  fLigFracFineRoot;
	double  fLigFracCoarseRoot;
	double  fFineBranchDecR;
	double  fLargeWoodDecR;
	double  fCoarseRootDecR;
	double	fNLitterMinerDay;
	double	fNManureMinerDay;
	double	fNHumusMinerDay;
	double	fNMinerDay;
	double  fCO2ProdDay;
	double  fNLitterImmobDay;
	double  fNManureImmobDay;
	double  fNHumusImmobDay;
	double	fNImmobDay;
	double	fUreaHydroDay;
	double	fNH4NitrDay;
	double  fNH4ToN2ODay;
	double	fNO3DenitDay;
	double	fN2ODenitDay;
	double	fN2OEmisDay;
	double  fNOEmisDay;
	double	fN2EmisDay;
	double	fNH3VolatDay;
	double	fCO2EmisDay;
	double	fCO2SurfEmisDay;
	// double	fCH4ImisDay; // verwendet stattdessen bitten fNO3NImisR
	double	fUreaLeachDay;
	double	fNH4LeachDay;
	double	fNO3LeachDay;
	double  fN2ODrainDay;
	double	fNTotalLeachDay;
	double  fDONLeachDay;
	double  fDOCLeachDay;
	double	fNUptakeDay;
	double	dNLitterMinerCum;
	double	dNManureMinerCum;
	double	dNHumusMinerCum;
	double	dNMinerCum;
	double  dNLitterImmobCum;
	double  dNManureImmobCum;
	double  dNHumusImmobCum;
	double	dNImmobCum;
	double  dNetNMinerCum;//added by Hong fo N balance
	double  dNetNMinerCum30;
	double	dCMinerCum;
	double	dCO2ProdCum;
	double  dUreaHydroCum;
	double	dNH4NitrCum;
	double  dNH4ToN2OCum;
	double	dNO3DenitCum;
	double	dN2ODenitCum;
	double	dN2OEmisCum;
	double  dNOEmisCum;
	double  dNEmisCum; //Added by Hong for N-Balance on 20190109
	double	dN2EmisCum;
	double	dNH3VolatCum;
	double	dCO2EmisCum;
	double	dCO2EmisCum_30;//Hong 20190507: balance for 0-30 cm profile
	double	dCO2SurfEmisCum;
	double	dCO2EmisCumSum; //Added by Hong for C-Balance on 20181112
	double	dCO2EmisCumSum_30;
	double	dCH4ImisCum;
	double  dN2ODrainCum;
	double	dUreaLeachCum;
	double	dNH4LeachCum;
	double	dNO3LeachCum;
	double	dNTotalLeachCum;
	double  dDONLeachCum;
	double  dDOCLeachCum;
	double  dDOCLeachCum_30;//Hong 20190507: balance for 0-30 cm profile
	double	dNUptakeCum;
	double  fCO2EmisR;
	double  fCO2SurfEmisR;
	double  fCH4ImisR; // Deposition (Rate für CH4)
	double  fNO3NImisR; // Deposition (Rate für NO3N)
	double  fNH4NImisR; // Deposition (Rate für NH4)
	double  fNO3LeachR;
	double  fN2OEmisR;
	double  fNOEmisR;
	double  fN2EmisR;
	double  fNFertilizerR;
	double  fNH3VolatR;
	double  fNH3VolatMaxR;
	double  fNH4ToN2OR;
	// 2019-03-28 new for agroforestry module:
	double  fUsedLeafWeight;
    double  fUsedStemWeight;
	double  fUsedBranchWeight;
    double  fUsedRootWeight;

}
CPROFILE;


typedef struct CParam   *PCPARAM;
typedef struct CParam
{
	double	fMolDiffCoef;
	double	fMinerEffFac;
	double	fMinerHumFac;
	double	fBioMassFastCN;
	double	fBioMassSlowCN;
	double	fBioMassDenitCN;
	double	fMinerSatActiv;
	double	fMinerTempB;
	double	fMinerQ10;
	double	fMinerThetaMin;
	double	fDenitThetaMin;
	double	fDenitKsNO3;
	double	fDenitKsCsol;
	double	fMaintCoeff;
	double	fDenitMaintCoeff;
	double	fYieldCoeff;
	double	fDenitYieldCoeff;
	double	fBioFastMaxGrowR;
	double	fBioSlowMaxGrowR;
	double	fDenitMaxGrowR;
	double	fDispersivity;
	double	fNitrifPot;
	double  fNitrifNO3NH4Ratio;
	double  fNitrifNH4ToN2O;
	double  fN2ORedMaxR;
	double  fDenitReWet;
	double  fDenitFreeze;
	double  fDenitThaw;
	double	afKd[6];
	double	afKh[6];
	double  fMobImmSltEC;
	double  fMobAdsSiteFrac;	
	PCPARAM  pNext;
	PCPARAM  pBack;
}
CPARAM;

typedef struct Atmos   *PATMOS;
typedef struct Atmos
{
	double dustr1; // dust size rate bin 1: 0.5um effective radius"     "ug/kg-dryair
	double dustr2; // dust size rate bin 2: 1.4um effective radius"     "ug/kg-dryair 
	double dustr3; // dust size rate bin 3: 2.4um effective radius"     "ug/kg-dryair 
	double dustr4; // dust size rate bin 4: 4.5um effective radius"     "ug/kg-dryair 
	double dustr5; // dust size rate bin 5: 8.0um effective radius"     "ug/kg-dryair 
	
}
ATMOS;


/*********************************************************************************************
*             SammelVariable   Chemistry
***********************************************************************************************/
typedef struct chemistry *PCHEMISTRY;
typedef struct chemistry
{
	PCLAYER			pCLayer;
	PNMEASURE		pNMeasure;
	PCBALANCE		pCBalance;
	PCPROFILE		pCProfile;
	PCPARAM			pCParam;
	PATMOS			pAtmos;	
}
CHEMISTRY;



#endif // #ifndef _INC_EXPERTN_XCHEMIST_H

/*******************************************************************************
** EOF */
