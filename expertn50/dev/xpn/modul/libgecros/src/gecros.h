//      gecros.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __GECROS_H__
#define __GECROS_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define GECROS_TYPE				(gecros_get_type())
#define GECROS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			GECROS_TYPE, gecros))
#define GECROS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			GECROS_TYPE, gecrosClass))
#define IS_GECROS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			GECROS_TYPE))
#define IS_GECROS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			GECROS_TYPE))
//Test of Hong
#define SET_IF_99(x1,y1) if (x1==-99.0) x1=(y1);
#define  EPSILON         1e-9
//#define  EPSILON_WC         1e-2 //Test of Hong

//End of Test
typedef struct _gecros			gecros;
typedef struct _gecrosClass		gecrosClass;

typedef struct stGecrosGenotype * PGECROSGENOTYPE;
typedef struct stGecrosGenotype
	{
		double fMaxPlantGrowR; // [1/d]

		double fVcmaxSu;
		double fJmaxSu;
		double fVcmaxSh;
		double fJmaxSh;
		double fLAImax;

	}
	GECROSGENOTYPE;


typedef struct stGecrosBiomass * PGECROSBIOMASS;
typedef struct stGecrosBiomass
	{
		double fStorageWeight;//WSO [g m-2] dry weight of storage organs 
		double fStorageGrowR;//RWSO [g m-2 d-1] rate of increase in storage organs
		double fTotalSeedNum;//TSN
		double f1000GrainWeight;//TSW
		double fShtToRtActivity;//DERI 
		double fShootActivity;//SHSA

		double fStemWeightChangeR;//RWST 
		double fLeafDeadWeight;//WLVD [g m-2] dry weight of dead leaves
		double fLeafWeightLossR;//LWLV [g m-2 d-1] rate of weight loss in leaves due to senescence
		double fShootWeight;//WSH [g m-2] dry weight of shoot (above-ground) organs
		double fShtWghtExShLvs;//WSHH [g m-2] dry weight of shoot organs excluding shedded leaves
		double fRootDeadWeight;////WRTD [g m-2] dry weight of dead roots
		double fRootWeightLossR;//LWRT [g m-2 d-1] rate of weight loss in roots due to senescence

		double fHarvestIndex;//HI [g g-1] harvest index
		
	}
	GECROSBIOMASS;


typedef struct stGecrosParameter * PGECROSPARAMETER;
typedef struct stGecrosParameter  
	{
		int	  iStageGecros;
		int   iniGecros;
		double fStageEndSeedNum;//ESD
		double fStageEndSeedNumI;//ESDI
		double fGrwEffVegOrg;//YGV [g C g-1 C] growth efficiency of veg. organs (Genotype)    

		double fNFracRemobPrevE;//PNPRE - (Plant Nitrogen)
		double fSeedWeight;//SEEDW [g seed-1] (Plant Biomass)
		double fNSeedActConc;//SEEDNC [g N g-1] (Plant Nitrogen)

		double fPMEH;//PMEH [-] (Plant Biomass)
		double fPMES;//PMES [-] (Plant Biomass)

		double fStemPerHeightDryWeight;//CDMHT [g m-2 m-1] (Plant Biomass)
		double fPlantHeightMax;//HTMX [m] (Plant Biomass)
		double fRootDepthMax;//RDMX [cm] (Plant Biomass)
	}
	GECROSPARAMETER;

typedef struct stGecrosCarbon * PGECROSCARBON;
typedef struct stGecrosCarbon 
	{
        double   fCPlant;//CTOT [g C m-2] total C amount of living plant
		double   fCLeaf;//CLV [g C m-2] C amount of living leaves
		double   fCLeafDead;//CLVD [g C m-2] C amount of dead leaves
        double   fCLeafDeadSoil;//CLVDS [g C m-2] C amount of dead leaves already litter in soil
		double   fCLeafDeadSoilR;//LVDS  [g C m-2 d-1] transfer rate of C amount of dead leaves to soil
		double   fCShoot;//CSH [g C m-2] amount of C in living shoot organs
	    double   fCStem;//CSST [g C m-2] amount of C in structural stems
		double   fCStorage;//CSO [g C m-2] amount of C in storage organs
		double   fCFracVegetative;//CFV [-] C fraction in vegetative organs
		double   fCFracStorage;//CFO [-] C fraction in storage organs
	    double   fCRoot;//CRT [g C m-2] amount of C in living roots (incl. reserves)
	    double   fCRootDead;//CRTD [g C m-2] amount of C in dead roots 
	    double   fCStrctRoot;//CSRT [g C m-2] amount of C in structural roots
	    double   fCStrctRootN;//CSRTN N-determined CSRT
		double   fCStemReserve;//CRVS [g C m-2] C amount in stem reserves
		double   fCRootReserve;//CRVR [g C m-2] C amount in root reserves

		double   fCFracPartToLeaf;//FCLV [-] fraction of new shoot C partitioned to leaves
		double   fCFracPartToStem;//FCSST [-] fraction of new shoot C partitioned to str. stems
		double   fCFracPartToShoot;//FCSH [-] fraction of new C partitioned to shoot
		double   fCFracPartToRoot;
		double   fCFracPartToRootReserve;//FCRVR [-] fraction of new root C partitioned to root reserves
        double   fCFracPartToStemReserve;//FCRVS [-] fraction of new shoot C partitioned to stem reserves
        double   fCFracPartToStorage;//FCSO [-] fraction of new C partitioned to storage organs

		double   fCFlowToSink;//FLWC [g C m-2 d-1] flow of new C to sink
		double   fCFlowToStorage;//FLWCS [g C m-2 d-1] flow of new C to storage organs
		double   fCFlowToStem;//FLWCT [g C m-2 d-1] flow of new C to structural stems

		double   fCDlyDemandSink;
		double   fCDlyDemandOrgan;
		double   fCDlyDemandStorage;//DCDS [g C m-2 d-1] daily C demand for filling of storage organs
		double   fCActDemandSeed;//DCDSC [g C m-2 d-1] actual C demand for seed filling 
		double   fCDlyDemandShoot;
		double   fCDlyDemandRoot;
		double   fCDlyDemandStem;//DCDT [g C m-2 d-1] daily C demand for structural stems
		double   fCActDemandStem;//DCDTC [g C m-2 d-1] actual C demand for structural stems
		double   fCPrvDemandStem;//DCDTP [g C m-2 d-1] previous C demand for structural stems
		double   fCPrvDmndStmChangeR;//RDCDTP [g C m-2 d-1] previous C demand for structural stems change rate

		double   fCShortFallDemandSeed; //DCDSR [g C m-2] short fall C demand for seed fill in previous time steps
		double   fCShortFallDemandSeedR;//RDCDSR [g C m-2] change rate of short fall C demand for seed fill 
		                               //in previous time steps
		double   fCShortFallDemandStem; //DCDTR [g C m-2] short fall C demand for structural stems in previous time steps
		double   fCShortFallDemandStemR; //RDCDTR [g C m-2] change rate of short fall C demand for structural stems 
		                               //in previous time steps
		double   fCDlySupplySink;
		double   fCDlySupplyShoot;//DCSS [g C m-2 d-1] daily new C supply for shoot growth
		double   fCDlySupplyRoot;//DCSR [g C m-2 d-1] daily new C supply for root growth
		double   fCDlySupplyStem;//DCST [g C m-2 d-1] daily new C supply for stem growth

        double   fCLeafChangeR;//RCLV [g C m-2 d-1] rate of change in C amount of living leaves
	    double   fCStemChangeR;//RCSST [g C m-2 d-1] rate of change in C amount of structural stems
		double   fCStemRsrvChangeR;//RCRVS [g C m-2 d-1] rate of change in C amount of stem reserves
		double   fCRootChangeR;//RCSRT [g C m-2 d-1] rate of change in C amount of structural roots
		double   fCRootRsrvChangeR;//RCRVR [g C m-2 d-1] rate of change in C amount of root reserves
		double   fCStorageChangeR;//RCSO [g C m-2 d-1] rate of change in C amount of storage organs
	    double   fCLeafLossR;//LCLV [g C m-2 d-1] rate of C loss in leaves due to senescence
	    double   fCRootLossR;//LCRT [g C m-2 d-1] rate of C loss in roots due to senescence
        double   fCRootToStorageR;//CREMR [g C m-2 d-1] remobilized C from root reserves to storage organs
        double   fCStemToStorageR;//CREMS [g C m-2 d-1] remobilized C from stem reserves to storage organs
        double   fCRootToStorageRI;//CREMRI [g C m-2 d-1] remob. C from root reserves to storage organs intermediate
        double   fCStemToStorageRI;//CREMSI [g C m-2 d-1] remob. C from stem reserves to storage organs intermediate
		double   fCSeedGrowthGap;//GAP [g C m-2 d-1] gap between C supply and C demand for seed growth 

		double   fNetStdgCropPhotosynR;//APCANS-RM-RX
		double   fGrossStdgCropPhotosynR;//APCANS [g CO2 m-2 d-1] Act. standing crop CO2 assimilation
		double   fGrossStdgCropPhotosynRN;//APCANN [g CO2 m-2 d-1] APCANS with small plant-N increment
		double   fMaintRespRN;//RMN [g CO2 m-2 d-1] maintenance respiration rate with small plant-N increment
		double   fFixRespCost;//RX [g CO2 m-2 d-1] respirative cost of N2 fixation
		double   fUptRespCost;//RMUL [g CO2 m-2 d-1] resp. cost of phloem loading and uptake of minerals and N
		double   fUptRespCostR;//RRMUL rate of change of RMUL
		double   fCNFixCost;//CCFIX [g C g-1 N] carbon cost of symbiotic nitrogen fixation 
	}
	GECROSCARBON;

typedef struct stGecrosNitrogen * PGECROSNITROGEN;
typedef struct stGecrosNitrogen 
	{
		double   fNExtCoeff;//KN
        double   fNFixation;//NFIX [g N m-2 d-1] amount of symbiotically fixed N     
        //double   fNFixationDemand;//NFIXD [g N m-2 d-1] crop demand determined NFIX     
        //double   fNFixationEnergy;//NFIXE [g N m-2 d-1] available energy determined NFIX    
        double   fNFixationReserve;//NFIXR[g N m-2] reserve pool of symbiotically fixed N
		double   fNFixReserveChangeR;//RNFIXR
        double   fNFixationTotal;//NFIXT [g N m-2] total fixed N during growth

		double   fNToCFracNewBiomass;//NCR [g N g-1 C]
        double   fNToCFracNewBiomPrev;//NCRP [g N g-1 C]
		double   fNFracPartShoot;//FNSH [-] fracion of new N partitioned to shoot

		double   fNSeedConc;//SEEDNC [g N g-1] standard seed (storage organ) N concentration

		double   fNRemobToSeedR;//RNRES [g N m-2] vegetative-organ N remobilizable for seed growth
		                                        //rate
		double   fNRemobToSeedF;//NREOF [g N m-2] vegetative-organ N remobilizable for seed growth 
		                                       //till seed fill starts
		double   fNRemobToSeedE;//NREOE [g N m-2] vegetative-organ N remobilizable for seed growth 
		                                       //till end of seed number determining period
		double   fNRemobToSeedFR;//RNREOF [g N m-2] rate of change for vegetative-organ N remobilizable  
		                                 //for seed growth till seed fill starts 
		double   fNRemobToSeedER;//RNREOE [g N m-2] rate of change for vegetative-organ N remobilizable 
		                                 //for seed growth till end of seed number determining period
        double   fNInitFacSeedF;//CB [-] factor for initial N concentration of seed fill
		double   fNFinalFacSeedF;//CX [-] factor for final N concentration of seed fill
		double   fNFastSeedFDevStage;//TM [-] development stage when transition from CB to CX is fastest

		double   fNLeafLossR;//LNLV [g N m-2 d-1] rate of loss of leaf N amount due to senescence
		double   fNRootLossR;//LNRT [g N m-2 d-1] rate of loss of root N amount due to senescence

		double   fNShootCont;//NSH [g N m-2] content of N in living shoot
		double   fNShootContSoil;//NSHH [g N m-2] content of N in shoot excl. dead leaves in soil
		double   fNStorageCont;//NSO [g N m-2] content of N in storage organs

		double   fNLeafTotCont;//TNLV [g N m-2] content of total N in leaves (incl. dead leaves)
		double   fNLeafDeadCont;//NLVD [g N m-2] content of N in dead leaves 
		double   fNRootDeadCont;//NRTD [g N m-2] content of N in dead roots
		double   fNLeafSpecificCont;//SLN
		double   fNLeafSpecificContTop;//SLNT
		double   fNLeafSpecificContBottom;//SLNB
		double   fNLeafSpecificContBottomChangeR;//RSLNB

		double   fShootActConc;//HNC in xplant.h aufnehmen unter NITROGEN!
		double   fTotalActConc;//PNC
		double   fStorageActConc;//ONC
		double   fSeedProtein;//PSO

		double   fNLeafAccumR;//RNLV
		double   fNStemAccumR;//RNST
		double   fNStorageAccumR;//RNSO
		double   fNRootAccumR;//RNRT
		double   fNLeafTotAccumR;//RTNLV

		double   fNUptMax;//NUPTX [g N m-2 d-1] maximum crop N uptake 
		double   fNDmndActDrv;//NDEMA [g N m-2 d-1] activity-driven crop N demand

		double   fNDemand;//NDEM
		double   fNDmndPrev;//NDEMP
		double   fNDmndPrvChangeR;//RNDEMP

		double   fNPlantSupply;//NSUP [g N m-2 d-1] N supply to crop
		double   fNPlantSuppPrev;//NSUPP [g N m-2 d-1] previous N supply to crop
		double   fNPltSppPrvChangeR;//RNSUPP
	}
	GECROSNITROGEN;


typedef struct stGecrosCanopy * PGECROSCANOPY;
typedef struct stGecrosCanopy 
	{
		double fLAIGreen;//LAI
		double fLAITotal;//TLAI
		double fLAICdeterm;//LAIC C-determined LAI
		double fLAINdeterm;//LAIN N-determined LAI
		double fPlantHeightGrowR;//RHT
	}
    GECROSCANOPY;

typedef struct stGecrosSoil * PGECROSSOIL;
typedef struct stGecrosSoil 
	{
		double fProfileDepth;//150.
		double fWaterContUpperLayer;//WUL
		double fWaterContLowerLayer;//WLL
		double fWaterContInitial;//WCI
		double fWaterContInitFac;//MULTF
		double fPlantWaterCapacity;//PWC
		double fETDailyWaterSupply;//DWSUP
		double fWaterFlowToLowLayer;//RRUL

        double fSoilTemp;//TSOIL
		double fSoilTempR;//RTSOIL
        
		double fTotalOrgCarbon;//TOC
		double fMicroBiomC;//BIO
		double fMicBiomCRate;//RBIO
		double fMicBiomCDecR;//DECBIO
		double fHumusC;//HUM
		double fHumusCRate;//RHUM
		double fHumusCDecR;//DECHUM
		double fDecPlantMatC;//DPM
		double fDecPltMCRate;//RDPM
		double fDecPltMCDecR;//DECDPM
		double fDecPlantMatN;//DPN
		double fDecPltMNRate;//RDPN
		double fDecPltMNDecR;//DECDPN
		double fResPlantMatC;//RPM
		double fResPltMCRate;//RRPM
		double fResPltMCDecR;//DECRPM
		double fResPlantMatN;//RPN
		double fResPltMNRate;//RRPN
		double fResPltMNDecR;//DECRRPN
		double fCNPlantMat;//CNDRPM
		double fLitterC;//LITC
		double fLitterN;//LITN
		double fNH4NContUpperLayer;//NAUL
		double fNH4NContLowerLayer;//NALL
		double fNO3NContUpperLayer;//NNUL
        double fNO3NContLowerLayer;//NNLL
		double fNH4NContUpLayRate;//RNAUL
		double fNH4NContLwLayRate;//RNALL
		double fNO3NContUpLayRate;//RNNUL
        double fNO3NContLwLayRate;//RNNLL
		double fNH4NContProfile;//NA
		double fNO3NContProfile;//NN
		double fNH4NFertilizer;//FERNA
		double fNH4NVolatFert;//SFERNA
		double fNH4NVolatFertR;//RSFNA
		double fNH4NVolatilizRate;//VOLA
		double fNO3NFertilizer;//FERNN

		double fDiffSoilAirTemp;//DIFS
	}
    GECROSSOIL;

typedef struct stGecrosPlant * PGECROSPLANT;
typedef struct stGecrosPlant
{
	PGECROSPARAMETER		pGecrosParameter;
	PGECROSGENOTYPE			pGecrosGenotype;
	PGECROSBIOMASS			pGecrosBiomass;
	PGECROSCARBON			pGecrosCarbon;
	PGECROSNITROGEN			pGecrosNitrogen;
	PGECROSCANOPY			pGecrosCanopy;
	PGECROSSOIL             pGecrosSoil;
//	PGECROSPLANT			pNext;
//	PGECROSPLANT			pBack;
}
GECROSPLANT;

typedef struct {
	double fTempMax;
	double fTempMin;
	double fTempAve;
	double fPotTraDay;
	double fEvapPotDay;
	double fPotETDay;
	double fActTraDay;
	
	double fDaylySolRad;
	
	double fTimeY_save;
	int iyear_save;
} struct_weather;


struct _gecros
{
	expertn_modul_base parent;
	/* add your public declarations here */

	// Globale Variablen:
	PGECROSPLANT	pGecrosPlant;
	double SC,DS0,SINLD,COSLD,DAYL,DDLP,DSINB,DSINBE;//Astronomy
	double SUNRIS, SUNSET; //Added by Hong on 20191024

	double fEmergValue; //added by F.Heinlein and Hong 20170512 
	
    ///*
	//%%% Crop parameters for pea (Pisum sativum L.) %%%
	//[croptype]
	double LEGUME,C3C4,DETER,SLP,LODGE,VERN;
	//[biomass]
	double EG,CFV,YGV;
	double FFAT,FLIG,FOAC,FMIN;
	//[phenology]
	double TBD,TOD,TCD,TSEN;
	//[photoperiod]
	double SPSP,EPSP,INSP,PSEN;
	//[morphology]
	double LWIDTH,RDMX,CDMHT;
	//[default]
	double PMEH,PMES,ESDI,WRB;
	//[nitrogen]
	double CCFIX,NUPTX,SLA0,SLNMIN,RNCMIN,STEMNC;
	double LNCI;
    //[photosynthesis]                 
	double EAJMAX,XVN,XJN,THETA;
	//[genotype] 
	double SEEDW,SEEDNC,BLD,HTMX,MTDV,MTDR; 
    
    //SG20180410. additional paramterfor sugarbeet
    double SINKBEET, EFF, CFS;

    //%** Soil parameters
    //double PNLS=1.
    //double CLAY=23.4; WCMIN=0.05; WCFC=0.25; WCMAX=0.35
    //PARAM DRPM=1.44; DPMR0=10.; RPMR0=0.3; BIOR=0.66; HUMR=0.02
    //PARAM TOC=7193.; BHC=3500.; FBIOC=0.03; RN=1.; RA=1.
   double RSS;//soil resistance, equiv.to stomatal resistance [s m-1] 
   double SD1;  //thickness of upper soil layer [cm] 
   double TCT,TCP,MULTF;//TCT=4.; TCP=1.; MULTF=1.
   double WCMIN,WCMAX,WCFC,MXPWC;
 
   //%** Sensitivity-analysis options
   double CO2A,COEFR,COEFV,COEFT;
   double FCRSH,FNRSH;
   double PNPRE,CB,CX,TM;

   //%%% Initialisierung
   double FPRO,FCAR;
   double CFO,YGO,LNCMIN;
   double CLVI,CRTI,NPL,NLVI,NRTI;
   double LAII,SLNBI,RDI,HTI;

   //Test of Hong
   double LAI;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//*/
	
	double TCMIN;
	double fPotTraDay;
	double fActTraDay;
	
	struct_weather weather;
	struct_weather __weather;

	// System Variablen:
	double *TairMin;
	double *TairMax;
	double *TairMean;
	double *fSolRad;

	int _init_done;	
	int _done_done;	
	
	const char *ini_file[10];
	char *ini_filename;
	
	int first_round;
	//Hong for crop rotation
	int round;
	
	//Hong 2016-08-08: change for Sebastian Gayler and Arne Poyda
	int      iDayAftSow;
	//End of Hong
	
	int __ERROR; //added by Hong on 20180124
};

#define C_DEBUG(var) 
//	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i)
/*	{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}\*/

// public class member function:
G_MODULE_EXPORT int gecros_Init_and_AllocMemory(gecros *self);
G_MODULE_EXPORT int gecros_Development(gecros *self);
G_MODULE_EXPORT int gecros_Photosynthesis(gecros *self);
G_MODULE_EXPORT int gecros_BiomassGrowth(gecros *self);
G_MODULE_EXPORT int gecros_CanopyFormation(gecros *self);
G_MODULE_EXPORT int gecros_RootSytem(gecros *self);
G_MODULE_EXPORT int gecros_CropSenescence(gecros *self);
G_MODULE_EXPORT int gecros_NitrogenDemand(gecros *self);
G_MODULE_EXPORT int gecros_NitrogenUptake(gecros *self);
G_MODULE_EXPORT int gecros_ActualTranspiration(gecros *self);
G_MODULE_EXPORT int gecros_Done_and_FreeMemory(gecros *self);


struct _gecrosClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType gecros_get_type (void);

G_END_DECLS

#endif /* __GECROS_H__ */

