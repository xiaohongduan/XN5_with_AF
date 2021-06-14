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
 * $History: xplant.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:08
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 01.03.98
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XPLANT_H
#define _INC_EXPERTN_XPLANT_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

/***********************************************************************************************
                     Crop Genotype Infomation Part
***********************************************************************************************/

#define MAX_FILE_STRINGLEN    30
#define MAX_DEPTH_PER_MEASURE 10 //ep 170999 maximal number of start value horizons 
#define MAX_DEVSTAGE          10 //ep 170799 maximal number of development stages

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  INCLUDE FILES, DEFINED VARIABLES AND TYPES 
//------------------------------------------------------------------------------------------------------------
//Expert_N System Data Variables

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Type Definitions
// #define DLL  CALLBACK _loadds

//#include "xtypes.h"

typedef struct Outputs  * POUTPUT;
typedef struct Outputs
    {
    double  fValue0;
    double  fValue1;
    double  fValue2;
    double  fValue3;
    double  fValue4;
    double  fValue5;
    double  fValue6;
    double  fValue7;
    double  fValue8;
    double  fValue9;
    }
    OUTPUT;

typedef struct CardinalTemperature  * PCARDTEMP;
typedef struct CardinalTemperature  //Cardinal temperture of biological processes
    {
    double  fMinTemp;       //Minimum temperature (C)
    double  fOptTemp;       //Optimal temperature (C)
    double  fMaxTemp;       //Maximum temperature (C)
    double  fOptTempRange;  //Range of the optimal temperature (from fOptTemp to fOptTemp+fOptTempRange)
    }
    CARDTEMP;

#define PCARDINALTEMPERATURES PCARDTEMP
#define CARDINALTEMPERATURE   CARDTEMP

typedef struct Transpiration  * PTRANSP;
typedef struct Transpiration    //Maximum and actual transpiration
    {
    double  fPotTransp;         //Potential transpiration of the crop  (mm)
    double  fActTransp;         //Actual transpiration of the crop     (mm)
    }
    TRANSP;

#define PTRANSPIRATION PTRANSP
#define TRANSPIRATION  TRANSP

typedef struct OrganNitrogenConcentration  * PORGANNC;
typedef struct OrganNitrogenConcentration
    {
    double  fMaxNc;             //Maximum nitrogen concentration at luxus nitrogen level (%)
    double  fOptNc;             //Optimum nitrogen concentration below which nitrogen stress occurs (%)
    double  fMinNc;             //Minimum nitrogen concentration below which no growth occurs (%)
    double  fActNc;             //Actual  nitrogen concentration of the organ (%)
    double  fMaxOptNc;          //The maximum value of fOptNc
    }
    ORGANNC;

#define PORGANNITROGENCONCENTRATION  PORGANNC
#define ORGANNITROGENCONCENTRATION   ORGANNC

typedef struct DevStage  * PDEVSTAGE;
typedef struct DevStage
    {
    double  fStageVR;               //Vegetative-Reproductive development stage (0-1-2)
    double  fStageDSC;              //Development stage for cereals (0-10)
    double  fStageCERES;            //Development stage used in CERES model (-)
    double  fZStage;                //ZCK development stage code (0-100)
    }
    DEVSTAGE;

#define DEVELOPMENTSTAGE    DEVSTAGE
#define PDEVELOPMENTSTAGE   PDEVSTAGE


typedef struct stvarietyInfo  * PVARIETYINFO;
typedef struct stvarietyInfo
    {
    int             iCode;
    char            acName[MAX_FILE_STRINGLEN];
    int             iValue;             
    PVARIETYINFO    pNext;
    PVARIETYINFO    pBack;
    }
    STVARIETYINFO;

typedef struct ststageParam  *PSTAGEPARAM;
typedef struct ststageParam
    {
    double  *afTempMax;
    int     afTempMaxLen;
    double  *afTempOpt;
    int     afTempOptLen;
    double  *afTempBase;
    int     afTempBaseLen;
    double  *afThermalTime;
    int     afThermalTimeLen;
    double  *afMaxDevR;
    int     afMaxDevRLen;
//ep 170799 
    //PSTAGEPARAM   pNext;
    //PSTAGEPARAM   pBack;
    }
    STSTAGEPARAM;

typedef struct Response  * PRESPONSE;
typedef struct Response
    {
    double  fInput;
    int     iInLen;
    double  fOutput;
    int     iOutLen;
    }
    RESPONSE;


typedef struct stgenotype  *PGENOTYPE;
typedef struct stgenotype
    {
//  char    *acTypeName;
    char    *acEcoType;
    char    *acCropName;
    char    *acCropCode;
    char    *acVarietyName;
    char    *acDaylenType;          //Daylength Response Type (Long/Short/Medium)
    //int       iVarietyCode;
    //int       iBYSKID;
    //int       iC_Pathway;
    	
	double	fPhyllochronInterval; //SG 20190531
	
    //double    fCriticalDaylength;
    //int       iDaylengthTyp;
    int     iVernCoeff;
    //double    fGrainNumCoeff;
    //double    fGrainFillCoeff;
    //double    fGrainFillStage;
    //double    fMaxPhotosynR;
    //double    fPhotosynDefic;
    //double    fDarkResp;
    double  fRootExtTempMin;
    double  fRootExtTempOpt;
    double  fRootExtTempMax;
    double   fTempOpt;
    double   fTempMax;
    double   fTempBase;
//  double  fTempQ10;
//  char    acStageName[MAX_FILE_STRINGLEN];

    //char  acLvAngleType[MAX_FILE_STRINGLEN];//Canopy structure (leaf angle distribution type)
    char    *acLvAngleType;         //Canopy structure (leaf angle distribution type)
    double  fPlantHeight;           //Height of the plant at anthesis (cm)
    double  fCO2CmpPoint;           //CO2 compensation point at 20C (vppm)
    double  fCiCaRatio;             //Ratio of internal to external CO2 concentration
    double  fMaxGrossPs;            //Maximum net photosynthesis of the leaf (kg CO2/ha.h)
    double  fLightUseEff;           //Light use efficiency at low light (kg CO2/MJ)
    double  fMinRm;                 //Minimum value of mesophyll resistance to CO2 (s/m)
    double  fPsCurveParam;          //Photosynthesis light response curve number (-) 
    double  fDaylengthCoeff;
    double  fTempMinPs;             //Minimum temperature for photosynthesis process (C)
    double  fTempOptPs;             //Optimum temperature for photosynthesis process (C)
    double  fTempMaxPs;             //Maximum temperature for photosynthesis process (C)
    double  fMaintLeaf;             //Maintenance respiration coefficient leaves (1/d)
    double  fMaintStem;             //Maintenance respiration coefficient stems (1/d)
    double  fMaintRoot;             //Maintenance respiration coefficient roots (1/d)
    double  fMaintStorage;          //Maintenance respiration coefficient storage organs (1/d)
    //double    fMaintBranch;           //Maintenance respiration coefficient branches (1/d)
    //double    fMaintGrossRoot;        //Maintenance respiration coefficient gross roots (1/d)
    //double    fMaintTuber;            //Maintenance respiration coefficient tubers (1/d)
    double  fCO2EffLeaf;            //CO2 producing factor for leaf biomass growth (kgCO2/kg DM)
    double  fCO2EffStem;            //CO2 producing factor for stem biomass growth (kgCO2/kg DM)
    double  fCO2EffRoot;            //CO2 producing factor for root biomass growth (kgCO2/kg DM)
    double  fCO2EffStorage;         //CO2 producing factor for storage biomass growth (kgCO2/kg DM)
    //double    fCO2EffBranch;          //CO2 producing factor for branch biomass growth (kgCO2/kg DM)
    //double    fCO2EffGrossRoot;       //CO2 producing factor for gross root biomass growth (kgCO2/kg DM)
    //double    fCO2EffTuber;           //CO2 producing factor for tuber biomass growth (kgCO2/kg DM)
    double  fGrwEffLeaf;            //Growth efficiency for leaves (kgDM/kgCH2O)
    double  fGrwEffStem;            //Growth efficiency for stems (kgDM/kgCH2O)
    double  fGrwEffRoot;            //Growth efficiency for roots (kgDM/kgCH2O)
    double  fGrwEffStorage;         //Growth efficiency for storage organs (kgDM/kgCH2O)
    //double    fGrwEffBranch;          //Growth efficiency for branches (kgDM/kgCH2O)
    //double    fGrwEffGrossRoot;       //Growth efficiency for gross roots (kgDM/kgCH2O)
    //double    fGrwEffTuber;           //Growth efficiency for tubers (kgDM/kgCH2O)
    double  fDevelopConstant1;      //Minimum days of the vegetative phase (d)
    double  fDevelopConstant2;      //Minimum days of the reproductive phase (d)
    //char  acDaylenType[10];       //Daylength Response Type (Long/Short/Medium)
    double  fDaylenSensitivity;     //Daylength sensitivity (0-1)
    //double    fCritDaylen;            //Critical (minimum) Daylength (hr)
    double  fOptDaylen;             //Optimum Daylength (hr)
    double  fTempMaxDevVeg;         //Maximum temperature for the development in vegetative phase (C)
    double  fTempMinDevVeg;         //Minimum temperature for the development in vegetative phase (C)
    double  fTempOptDevVeg;         //Optimum temperature for the development in vegetative phase (C)
    double  fTempMaxDevRep;         //Maximum temperature for the development in reproductive phase (C)
    double  fTempMinDevRep;         //Minimum temperature for the development in reproductive phase (C)
    double  fTempOptDevRep;         //Optimum temperature for the development in reproductive phase (C)
    //double    fCritVernDays;          //Critical (minimum) vernalization units reqired (d)
    double  fOptVernDays;           //Optimal vernalization units (d)
    double  fVernSensitivity;       //Vernalization sensitivity (0-1)
    double  fTempMaxVern;           //Maximum temperature for vernalization (C)
    double  fTempOptVern;           //Optimum temperature for vernalization (C)
    double  fTempMinVern;           //Minimum temperature for vernalization (C)   
    double  fTempMaxLfApp;          //Maximum temperature for Leaf appearance (C)
    double  fTempOptLfApp;          //Optimum temperature for Leaf appearance (C)
    double  fTempMinLfApp;          //Minimum temperature for Leaf appearance (C)   
    double  fMaxLfInitRate;         //Maximum initiation rate for leaf primodia (Primodia/d)
    double  fMaxLfAppRate;          //Maximum leaf appearance rate (leaves/d)
    double  fTillerDemand;          //Maximum demand of assimilate for a single stem (g/stem)
    double  fStorageOrganNumCoeff;  //Coefficient for estimating the number of storage organs (type-dependent)
    double  fSpecLfWeight;          //Specific leaf weight (kg/ha leaf)
    double  fReserveRatio;          //Ratio of reserves to stem dry weight at anthesis (-)  
    double  fRelResvGrwRate;        //Relative reserve growth rate (1/d)
    double  fRelStorageFillRate;    //Relative growth rate of storage organs (plant-dependent)  
    double  fRelStorageNFillRate;   //Relative growth rate of storage organs Nitrogen(plant-dependent)  
    double  fTempMaxSoFill;         //Maximum temperature for growth of storage organs (C)
    double  fTempOptSoFill;         //Optimum temperature for growth of storage organs (C)  
    double  fTempMinSoFill;         //Minimum temperature for growth of storage organs (C)   
    double  fTempMaxSoNFill;        //Maximum temperature for growth of storage organs (C)
    double  fTempOptSoNFill;        //Optimum temperature for growth of storage organs (C)  
    double  fTempMinSoNFill;        //Minimum temperature for growth of storage organs (C)   
    double  fMaxRootExtRate;        //Maximum root extension rate (cm/d)
    double  fRootLengthRatio;       //Length per kg root (cm/g)
    double  fMaxWuptRate;           //Maximum water uptake rate of unit length of root (cm3/cm.d)
    double  fMaxNuptRate;           //Maximum nitrogen uptake rate of unit length of root (kgN/cm.d)
    double  fMaxRootDepth;          //Maximum rooting depth (m)
    double  fBeginSenesDvs;         //Development stage where senescense begins (-)
    double  fBeginShadeLAI;         //Critical leaf area where shading effects occur (1)   
    double  fRelDeathRateLv;        //Relative death rate leaves (1/d)
    double  fRelDeathRateRt;        //Relative death rate roots (1/d)
    double  fBaseTemperature1;      //for SUCROS
    double  fCriticalTemperatureSum1;//for SUCROS
    double  fBaseTemperature2;      //for SUCROS
    double  fCriticalTemperatureSum2;//for SUCROS
    //Hong: added for Scott Demyan et al. 2016.10.06
    double fResidueAMO1Frac; //fraction of residue divided to LitterSurf pool (AMO1), 0.0...1.0
    double fCDeadleafFrac;   //fraction of dead leaves divided to carbon pool, 0.0...1.0, 
    double fNDeadleafFrac;   //fraction of dead leaves divided to nitrogen pool, 0.0...1.0,  
    //Moritz added for new residue partitioning by lignin/N 23.08.18
    double lig_stem;
    double lig_leaves;
    double lig_roots;
    //End of Moritz
    
    //End of Hong
    RESPONSE    *PmaxTemp;          //Maximum photosynthesis rate to temperature (Temp-Pmax)
    RESPONSE    *PmaxAge;           //Development stage to P/Pmax
    RESPONSE    *DevRateVegTemp;    //Development rate at Veg. phase to temp (Temp-Rdev)
    RESPONSE    *DevRateRepTemp;    //Development rate at Rep. phase to temp (Temp-Rdev)
    RESPONSE    *DevRateVegPhtP;    //Development rate to photoperiod (PhP-Rdev)
    RESPONSE    *VernRateTemp;      //Rate of vernalization vs temperature 
    RESPONSE    *DVR;               //Development rate vs. EC stadium
    RESPONSE    *PartRoot;          //Partitioning factor for root vs development stage
    RESPONSE    *PartLeaf;          //Partitioning factor for Leaves vs development stage
    RESPONSE    *PartStem;          //Partitioning factor for stems vs development stage
    RESPONSE    *PartStorage;       //Partitioning factor for Storage organs vs development stage
    RESPONSE    *ReservGrwLag;      //Lag factor for reserve growth vs development stage
    RESPONSE    *ReservTransTc;     //Time constant for reserve translocation vs development stage
    RESPONSE    *SoFillLag;         //Lag factor for growth of storage organs vs development stage
    RESPONSE    *SoFillRateTemp;    //Relative growth rate of storage organs vs temperature
    RESPONSE    *SoNFillRateTemp;   //Relative growth rate of storage N vs temperature
    RESPONSE    *MaxLvNc;       //Maximun leaf nitrogen content vs development stage
    RESPONSE    *MaxStNc;        //Maximun stem nitrogen content vs development stage
    RESPONSE    *MaxRtNc;        //Maximun root nitrogen content vs development stage
    RESPONSE    *OptLvNc;           //Optimum leaf nitrogen content vs development stage
    RESPONSE    *OptStNc;           //Optimum stem nitrogen content vs development stage
    RESPONSE    *OptRtNc;           //Optimum root nitrogen content vs development stage
    RESPONSE    *MinLvNc;           //Minimun leaf nitrogen content vs development stage
    RESPONSE    *MinStNc;           //Minimun stem nitrogen content vs development stage
    RESPONSE    *MinRtNc;           //Minimun leaf nitrogen content vs development stage
    RESPONSE    *RootExtRateTemp;   //Root extension rate vs temperature
    RESPONSE    *RootExtRateWc;     //Root extension rate vs soil water content
    RESPONSE    *LvDeathRateTemp;   //Relative leaf death rate vs temperature
    RESPONSE    *RtDeathRateTemp;   //Relative root death rate vs temperature
    RESPONSE    *SpecLfWght;        //Specific leaf weight vs development stage
    RESPONSE    *LfAppRateTemp;     //Relative leaf appearance rate vs temperature

    PSTAGEPARAM  pStageParam;
    PVARIETYINFO pVarietyInfo;
    PGENOTYPE pNext;
    PGENOTYPE pBack;

    }
    STGENOTYPE;

/***********************************************************************************************
         Crop Development
***********************************************************************************************/

typedef struct stdevelop  *PDEVELOP;
typedef struct stdevelop
    {
    int iDayAftSow;
    int iDayAftEmerg;
    int iStageCERES;
    double  fStageXCERES;
    double  fStageWang;
    double  fDevStage;
    double  fStageSUCROS;
    double  fDevR;
    double  fDTT;
    double  fDaylengthEff;
    double  fVernUnit;
    double  fVernEff;
    double  fCumDTT;
    double  fSumDTT;
    double  fCumTDU;
    double  fCumPhyll;
    double  fCumVernUnit;
    double  fPhysTime;
    double  fCumPhysTime;
    double  fPhysTimeFct;
    double  fPhysTimeExp;
    double  fTimeOfStage1;
    double  fTimeOfStage2;
    double  fTimeOfStage3;
    double  fTimeOfStage4;
    double  fTimeOfStage5;
    double  fTimeOfStage6;
    double  fAge;

    double  fThermEffect;
    double  iDayAftAnthesis;

    int     bMaturity;
    int     bPlantGrowth;

    PDEVELOP pNext;
    PDEVELOP pBack;
    }
    STDEVELOP;


/***********************************************************************************************
                     Crop Growth Part
***********************************************************************************************/
typedef struct stbiomass  *PBIOMASS;
typedef struct stbiomass
    {
    double  fTotalBiomass;
    double  fBiomassAbvGround;
    double  fStovWeight;
    double  fRootWeight;
    double  fGrossRootWeight;
    double  fTuberWeight;
    double  fStemWeight;
    double  fBranchWeight;
    double  fLeafWeight;
    double  fFruitWeight;
    double  fGrainWeight;
    double  fWoodWeight;
    double  fGreenLeafWeight;
    double  fSenesLeafWeight;
    double  fDeadRootWeight ;
    double  fMaxBiomGrowR;
    double  fBiomGrowR;
    double  fMaxShootGrowR;
    double  fMaxRootGrowR;
    double  fRootGrowR;
    double  fMaxGrossRootGrowR;
    double  fGrossRootGrowR;
    double  fMaxTuberGrowR;
    double  fTuberGrowR;
    double  fMaxLeafGrowR;
    double  fLeafGrowR;
    double  fMaxStemGrowR;
    double  fStemGrowR;
    double  fMaxBranchGrowR;
    double  fBranchGrowR;
    double  fMaxFruitGrowR;
    double  fFruitGrowR;
    double  fGrainGrowR;
    double  fRelGrainFillR;
    double  fPlantTopFrac;
    double  fCapRoot;
    double  fCapStem;
    double  fCapLeaf;
    double  fCapFruit;
    double  fCapGrain;
    double  fAttritionRootR;
    double  fAttrtionStemR;
    double  fAttritionLeafR;
    double  fAttritionFruitR;
    double  fAttritionGrainR;
    double  fRedFracRoot;
    double  fRedFracStem;
    double  fRedFracLeaf;
    double  fRedFracFruit;
    double  fRedFracGrain;
    double  fPartFracRoot;
    double  fPartFracGrossRoot;
    double  fPartFracTuber;
    double  fPartFracStem;
    double  fPartFracBranch;
    double  fPartFracLeaf;
    double  fPartFracFruit;
    double  fPartFracGrain;
    double  fSeedReserv;

    double  fStemReserveGrowRate;
    double  fStemReserveWeight; 
    double  fReserveTransRate;  
    double      fLeafReserveWeight; 
    double  fFruitDeathRate; 
    double  fLeafDeathRate; 
    double  fBranchDeathRate; 
    double  fStemDeathRate;
    double  fRootDeathRate;
    double  fGrossRootDeathRate;
    double  fTuberDeathRate;
    double  fDeadLeafWeight;
    double  fDeadStemWeight;
	double  fDeadBranchWeight; //added by Hong for agroforestry
    double  fTotLeafWeight;
    double  fTotRootWeight;
    double  fTotStemWeight;
    // 2018-01-30 new for agroforestry module
	double  fDeadLeafWeight_dt;//Added by Hong on 20190220
	double  fDeadStemWeight_dt;
	double  fDeadBranchWeight_dt;

    PBIOMASS  pNext;
    PBIOMASS  pBack;
    }                      
    STBIOMASS;
    

/***********************************************************************************************
                     Plant Canopy  
***********************************************************************************************/

typedef struct stcanopy   *PCANOPY;
typedef struct stcanopy
    {
    double  fLAI;
    double  fLAISpass;
    double  fPlantHeight; // in m
    double  fLeafAppearR;
    double  fLeafTipNum;
    double  fExpandLeafNum;
    double  fLAGrowR;
    double  fLALossR;
    double  fNormLeafSenesR;
    double  fLeafSenesR;
    double  fPlantLA;
    double  fPlantSenesLA;
    double  fPlantGreenLA;
    double  fTillerNum;
    double  fTillerNumOld;
    double  fTillerNumSq;
    double  fSenesTillerNumSq;
    double  fEarNum;
    double  fEarNumSq;
    double  fGrainNum;
    double  fGrainNumSq;
    double  fPlantDensity;
    double  fCropCoverFrac;

    double  fLeafPrimodiaNum;
    double  fMainStemTillers;
    double  fLeafAgeDeathRate;
    double  fLeafShadeDeathRate;
    double  fRelLeafDeathRate; 
    double  fLeafAreaLossRate;
    
    double fRoughnessHeat; // [m] Roughness Length
    double fRoughnessVapor; // [m]Roughness Length
    
    double fkcan; // if not set --> 0.6 [m2 leaf (m2 ground)-1]-1. Canopy extinction coefficient.
    double fsto_res; // [s m-1] stomata resistiance stdval: 100.0

    PCANOPY  pNext;
    PCANOPY  pBack;
    }
    STCANOPY;

/***********************************************************************************************
         Plant Root
***********************************************************************************************/
typedef struct stroot        *PROOT;
typedef struct stlayerRoot   *PLAYERROOT;

 
typedef struct stlayerRoot
    {
    double  fLengthDens;
    double  fLengthDensR;
    double  fLengthDensFac;
    double  fPotWatUpt;
    double  fActWatUpt;
    double  fPotLayWatUpt;
    double  fActLayWatUpt;
    double  fWatStressRoot;
    double  fPotNO3NUpt;
    double  fActNO3NUpt;
    double  fPotLayNO3NUpt;
    double  fActLayNO3NUpt;
    double  fPotNH4NUpt;
    double  fActNH4NUpt;
    double  fPotLayNH4NUpt;
    double  fActLayNH4NUpt;
    double  fPotNUpt;
    double  fActNUpt;
    double  fPotLayNUpt;
    double  fActLayNUpt;
    double  fNStressRoot;
	double  fDeadFineRootWeight_dt;//Added by hong on 20190221 for agroforest
	double  fDeadGrossRootWeight_dt;
	double  fDeadFineRootWeight_layer;
	double  fDeadGrossRootWeight_layer;
	
    PLAYERROOT  pNext;
    PLAYERROOT  pBack;
    }
    STLAYERROOT;

 typedef struct stroot
    {
    double  fDepth;
    double  fDepthGrowR;
    double  fMaxDepth;
    double   fUptakeDay;
    double fUptakedt;
    double   fUptakeR;
    double  fTotalLength;
    PLAYERROOT    pLayerRoot;
    PROOT  pNext;
    PROOT  pBack;
    }
    STROOT;

/***********************************************************************************************
         Plant Carbon
***********************************************************************************************/

typedef struct stplantCarbon  *PPLTCARBON;
typedef struct stplantCarbon
    {
    double  fGrossPhotosynR;
    double  fNetPhotosynR;
    double  fPhotorespR;
    double  fMaintRespR;
    double  fGrowthRespR;
    double  fDarkRespR;
    double  fTotRespR;
    double  fNetPhotosyn;
    double  fMainRespRRoot;
    double  fMainRespRGrossRoot;
    double  fMainRespRTuber;
    double  fMainRespRStem;
    double  fMainRespRBranch;
    double  fMainRespRLeaf;
    double  fMainRespRGrain;
    double  fGrowRespRRoot;
    double  fGrowRespRGrossRoot;
    double  fGrowRespRTuber;
    double  fGrowRespRStem;
    double  fGrowRespRBranch;
    double  fGrowRespRLeaf;
    double  fGrowRespRGrain;
    double  fRedFctPhotosynLeaf;
    double  fRedFctPhotosynStem;
    double  fRedFctPhotosynGrain;
    double  fCAssimilatePool;
    PPLTCARBON  pNext;
    PPLTCARBON  pBack;
    }
    STPLTCARBON;


/***********************************************************************************************
                     Plant Water 
***********************************************************************************************/

typedef struct stplantWater  *PPLTWATER;
typedef struct stplantWater
    {
    double  fTotCont;
    double  fTopsCont;
    double  fRootCont;
    double  fLeafCont;
    double  fStemCont;
    double  fGrainCont;
    double  fPanFac;
    double  fPotTranspR;        /* New [mm]/day  Potential Transpiration Rate*/
    double  fActTranspR;        /* New [mm]/day  Actual Transpiration Rate*/
    double  fPotTranspdt;
    double  fActTranspdt;
    double  fPotTranspDay;
    double  fActTranspDay;
    double  fInterceptDay;
    double  fInterceptdt;   /* New [mm/dt] sollte fInterceptDay ersetzen */
    double  fInterceptR;        /* [mm/d], rate */
    double  fShortage;
    double  fRefDemand;
    double  fCumDemand;
    double  fCumUptake;
    double  fCumShortage;
    double  fStressFacPhoto;
    double  fStressFacLeaf;
    double  fStressFacTiller;

    double  fPotUptakedt;
    double  fPotUptake;
    double  fActUptakedt;
    double  fActUptake;
    double  fStressFacPartition;
    PPLTWATER  pNext;
    PPLTWATER  pBack;
    }
    STPLTWATER;


/***********************************************************************************************
                    Plant Nitrogen 
***********************************************************************************************/

typedef struct stplantNitrogen  *PPLTNITROGEN;
typedef struct stplantNitrogen
    {
    double  fNPool;

    double  fTopsMinConc;
    double  fTopsActConc;
    double  fTopsOptConc;
    double  fVegActConc;
    double  fVegMinConc;

    double  fFruitMinConc;
    double  fFruitOptConc;
    double  fFruitActConc;

    double  fLeafMaxConc;
    double  fLeafMinConc;
    double  fLeafOptConc;
    double  fLeafActConc;

    double  fStemMaxConc;
    double  fStemMinConc;
    double  fStemOptConc;
    double  fStemActConc;
    
    double  fBranchMinConc;
    double  fBranchOptConc;
    double  fBranchActConc;
    
    double  fRootMaxConc;
    double  fRootMinConc;
    double  fRootOptConc;
    double  fRootActConc;

    double  fGrossRootMinConc;
    double  fGrossRootOptConc;
    double  fGrossRootActConc;

    double  fTuberMinConc;
    double  fTuberOptConc;
    double  fTuberActConc;

    double  fGrainConc;
    double  fGrainConcChange;

    double  fRootCont;
    double  fGrossRootCont;
    double  fTuberCont;
    double  fStemCont;
    double  fBranchCont;
    double  fLeafCont;
    double  fGrainCont;
    double  fStovCont;
    double  fTotalCont;

    double  fPotGrnNcumRate;    //Potential nitrogen accumulation rate of individual grains (mg/d)

    double  fStovContChange;
    double  fRootContChange;

    double  fStovNLabile;
    double  fRootNLabile;
    double  fStovNPool;
    double  fRootNPool;
    double  fTotalNLabile;
    double  fTotalNPool;

    double  fLeafDemand;
    double  fStemDemand;
    double  fBranchDemand;
    double  fRootDemand;
    double  fGrossRootDemand;
    double  fTuberDemand;
    double  fGrainDemand;
    double  fShootDemand;
    double  fTotalDemand;
    double  fCumTotalDemand;
    double  fRevDemand;

    double  fPotNO3NUpt;
    double  fActNO3NUpt;
    double  fPotNH4NUpt;
    double  fActNH4NUpt;
    double  fPotNUpt;
    double  fActNUpt;
    double  fCumPotNUpt;
    double  fCumActNUpt;
    double  fMaxNUptR;
    double  fOptNUptR;
    double  fActNUptR;

    double  fLeafRelNc;         //Relative nitrogen concentration of leaves         (kg/kg)
    double  fStemRelNc;         //Relative nitrogen concentration of stems          (kg/kg)
    double  fRootRelNc;         //Relative nitrogen concentration of roots          (kg/kg)
    double  fShootRelNc;        //Relative nitrogen concentration of shoot          (kg/kg)

    double  fLeafNinc;          //Amount of increased nitrogen due to uptake in leaves (kg/ha/day)
    double  fStemNinc;          //Amount of increased nitrogen due to uptake in stems (kg/ha/day)
    double  fBranchNinc;        //Amount of increased nitrogen due to uptake in stems (kg/ha/day)
    double  fRootNinc;          //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
    double  fGrossRootNinc;     //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
    double  fTuberNinc;         //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
    double  fGrainNinc;         //Amount of increased nitrogen due to uptake in grains (kg/ha/day)
    double  fShootNinc;         //Amount of increased nitrogen due to uptake in shoots (kg/ha/day)

    double  fLeafNtransRate;        //Nitrogen loss rate due to translocation leaves (kg/ha/day)
    double  fRootNtransRate;        //Nitrogen loss rate due to translocation roots  (kg/ha/day)
    double  fGrossRootNtransRate;   //Nitrogen loss rate due to translocation roots  (kg/ha/day)
    double  fTuberNtransRate;       //Nitrogen loss rate due to translocation roots  (kg/ha/day)
    double  fBranchNtransRate;      //Nitrogen loss rate due to translocation roots  (kg/ha/day)
    double  fStemNtransRate;        //Nitrogen loss rate due to translocation stems  (kg/ha/day)
    double  fShootNtransRate;       //Nitrogen loss rate due to translocation shoots  (kg/ha/day)

    double  fDeadLeafNw;        //Amount of nitrogen in dead leaves     (kg/ha)
    double  fDeadRootNw;        //Amount of nitrogen in dead  roots     (kg/ha)
    double  fDeadStemNw;        //Amount of nitrogen in dead  stems     (kg/ha)

    double  fLeafTransNw;       //Amount of translocatable nitrogen in leaves (kg/ha)
    double  fRootTransNw;       //Amount of translocatable nitrogen in roots  (kg/ha)
    double  fStemTransNw;       //Amount of translocatable nitrogen in stems  (kg/ha)
    double  fStoverTransNw;     //Amount of translocatable nitrogen in crop   (kg/ha)
    double  fTotalTransNw;      //Amount of translocatable nitrogen in crop   (kg/ha)

    double  fLeafNlossRate;         //Nitrogen loss rate due to death of leaves (kg/ha/day)
    double  fStemNlossRate;         //Nitrogen loss rate due to death of stems  (kg/ha/day)
    double  fBranchNlossRate;       //Nitrogen loss rate due to death of leaves (kg/ha/day)
    double  fRootNlossRate;         //Nitrogen loss rate due to death of roots  (kg/ha/day)
    double  fGrossRootNlossRate;    //Nitrogen loss rate due to death of roots  (kg/ha/day)
    double  fTuberNlossRate;        //Nitrogen loss rate due to death of stems  (kg/ha/day)

    double  fNStressFac;
    double  fNStressPhoto;
    double  fNStressLeaf;
    double  fNStressTiller;
    double  fNStressGrain;


    PPLTNITROGEN  pNext;
    PPLTNITROGEN  pBack;
    }
    STPLTNITROGEN;

/***********************************************************************************************
        Plant Microclimate
***********************************************************************************************/

typedef struct stplantClimate  *PPLTCLIMATE;
typedef struct stplantClimate
    {
    double  fCanopyTemp;
    double  fCanopyInterceptRad;
    double  fCanopyReflecRad;
    double  fCanopyTransmRad;
    double  fCanopyAbsorbRad;
    double  fPlantAlbedo; // [0..1] Plant Albedo
    PPLTCLIMATE  pNext;
    PPLTCLIMATE  pBack;
    
    }
    STPLTCLIMATE;

/***********************************************************************************************
        Plant Model Parameter 
***********************************************************************************************/

typedef struct stModelParam  *PMODELPARAM;
typedef struct stModelParam
    {
    int     iEmergenceDay; 
    //int       iHarvestDay;
    int   EmergenceDay;
    int   EmergenceMonth;
    int   EmergenceYear;    
    int   HarvestDay;
    int   HarvestMonth;
    int   HarvestYear;
    //long  lEmergenceDate;
    //long  lHarvestDate;
    
    double  fHarvestYield;
    char    cResidueCarryOff;
    double  fMaxRootDepth;
    //int       iMaxAboveBiomassDay;
    //int   iMaxRootBiomassDay;
    //long  lMaxAboveBiomassDate;
    //long  lMaxRootBiomassDate;
    int   MaxAboveBiomassDay;
    int   MaxAboveBiomassMonth;
    int   MaxAboveBiomassYear;
    int   MaxRootBiomassDay;
    int   MaxRootBiomassMonth;
    int   MaxRootBiomassYear;
    double  fTasselLeafNum;
    double  fMaxLeafNum;
    double  fMaxLA;
    double  fMinLeafWeight;
    double  fMinStemWeight;
    double  fMaxStemWeight;
    double  fSingleTillerStemWeight;
    double  fMaxCumNUpt;
    double  fMaxCumWatUpt;
    double  fHardnessIndex;
    int     iColdKill ;
    double  fBarleyGrwParam;
    double  fMaxCropCoverfrac;
    PMODELPARAM  pNext;
    PMODELPARAM  pBack;
    }
    STMODELPARAM;

typedef struct stPltMeasure   * PPLTMEASURE;
typedef struct stPltMeasure
    {
    //long  lDate;
    //int       iDay;   
    int     iyear; // year YYYY z.B. 1981
    double  fTimeY; // Days from Jan.
    double  fDevStage;
    double  fLAI;
    double  fAboveBiomass;
    double  fVegBiomass;
    double  fGenBiomass;
    double  fStemWeight;
    double  fLeafWeight;
    double  fGrainWeight;
    double  fRootWeight;
    double  fNConcAbvBiomass;
    double  fNAboveBiomass;
    double  fNVegBiomass;
    double  fNGenBiomass;
    double  fNStem;
    double  fNLeaf;
    double  fNGrain;
    double  fNRoot;
    double  fTotalN;
    double  fRootDepth;
    double  fTillers; // Plant Number
    double  fHeight;
    double  fCropCoverFrac;
    double   fCropETCoeff;
    int     iLeafNum;

    PPLTMEASURE pNext;
    PPLTMEASURE pBack;
    } 
    STPLTMEASURE;


/***********************************************************************************************
                Plant 
***********************************************************************************************/

typedef struct stplant  *PPLANT;
typedef struct stplant
    {                   
    POUTPUT        pOutput;
    PGENOTYPE      pGenotype;
    PDEVELOP       pDevelop;
    PBIOMASS       pBiomass;
    PCANOPY        pCanopy;
    PROOT          pRoot;
    PPLTCARBON     pPltCarbon;
    PPLTWATER      pPltWater;
    PPLTNITROGEN   pPltNitrogen;
    PPLTCLIMATE    pPltClimate; 
    PMODELPARAM    pModelParam;
    PPLTMEASURE    pPMeasure;
    PPLANT   pNext;
    PPLANT   pBack;
    }
    STPLANT;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Module Global Variables
// HANDLE   hModuleInstance; 



#endif // #ifndef _INC_EXPERTN_XPLANT_H

/*******************************************************************************
** EOF */
