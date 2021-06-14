//      libtreemix.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>, Peter Hoffmann
//      



#ifndef __LIBTREEMIX_H__
#define __LIBTREEMIX_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define LIBTREEMIX_TYPE				(libtreemix_get_type())
#define LIBTREEMIX(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			LIBTREEMIX_TYPE, libtreemix))
#define LIBTREEMIX_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			LIBTREEMIX_TYPE, libtreemixClass))
#define IS_LIBTREEMIX(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			LIBTREEMIX_TYPE))
#define IS_LIBTREEMIX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			LIBTREEMIX_TYPE))

/*----------- INI File Parser (http://developer.gnome.org/glib/2.28/glib-Key-value-file-parser.html) -----------*/
#define GET_INI_INTEGER(var,groupname,key) \
						error = NULL; \
						var = g_key_file_get_integer (keyfile,groupname,key,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \

#define GET_INI_INT_OPTIONAL(var,groupname,key,std_value)\
{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (key_exists==FALSE) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (in file %s) is missing. Standard Value (%d) taken instead!\n",groupname,key,filename,std_value);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{\
				var = std_value;\
			} else\
			{\
				GET_INI_INTEGER(var,groupname,key);\
			}\
	}\

#define GET_INI_DOUBLE(var,groupname,key) \
						error = NULL; \
						var = g_key_file_get_double (keyfile,groupname,key,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \

#define GET_INI_STRING(var,groupname,key) \
						error = NULL; \
						var = g_key_file_get_string (keyfile,groupname,key,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \
						
#define GET_INI_STRING_LIST(var,groupname,key,length) \
						error = NULL; \
						var = g_key_file_get_string_list (keyfile,groupname,key,length,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \
						
#define GET_INI_DOUBLE_LIST(var,groupname,key,length) \
						error = NULL; \
						var = g_key_file_get_double_list(keyfile, groupname, key, length, &error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \
						
#define GET_INI_INTEGER_LIST(var,groupname,key,length) \
						error = NULL; \
						var = g_key_file_get_integer_list (keyfile,groupname,key,length,&error); \
						if (error!=NULL) \
						{ \
							gchar *S;\
							S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
							PRINT_ERROR(S);\
							g_free(S);\
						} \

#define C_DEBUG(var) \
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);
//#define NewDay(pz) (pz->pSimTime->fTimeDay==0.0||((int)(pz->pSimTime->fTimeY) != (int)(pz->pSimTime->fTimeY_old)))? 1 : 0
#define IsStartOfSimulation(pz) (pz->pSimTime->fTimeAct == (double) 0) ?1 :0

/* Constants */
#define EPSILON		1.0e-8		// value close to zero for if statements
#define MAX_LAYERS	300
#define HrsPerDay	24.0
#define	IDGASKONST	8.31		// ideal gas constant needed for Baldocchi [J/K*mol]
#define	CONCO2 		210.0		// O2 concentration in air

/* Bool Operators */
#ifndef BOOL
	#define BOOL	int
	#define TRUE	1
	#define FALSE	0
#endif

typedef struct _libtreemix			libtreemix;
typedef struct _libtreemixClass		libtreemixClass;

// [Configuration]
typedef struct		
{
	int Species_Count;		//number of different species to simulate
	gchar **Parameter_Files;	//name of the species config files
	int Operating; //choose of forst or agroforst, added by Hong on 20180621
	int Photosynthesis;
	int Light_Interception;
	int PhotoRad;			// which radiation input is used
	int PhotoResp;			// which temp. dependence is used
	int Tree_Water;
	int Phenology;
	int Respiration;
	int Tree_Geometry;
	int Tree_fast_growing; //Added by Hong on 20180627, for modified TREEDYN of Miehle et al.
	int Weather_Input;
	int AirPressure;		// read in (0) or calculated (1) in photosynthesis model
	int Radiation;
	int Soil_Processes;
	int Rainfall_Interception;
	int Nitrogen_Deposition;
} P_Conf;

// [Plant]
typedef struct		
{	
	/* Species Parameters */
	char *code;			// tree code
	char *name;			// latin designation
	char *type;			// coniferous, deciduous
	double density;		// [trees/ha]
	double TreeDistr;	// species distribution [-]  0...1
	double H;			// tree height [m]
	double D;			// stem diameter (base) [m]
	double DBH;			// diameter at breast height [m]
	double CWdMass;		// standing wood mass [kgC/ha]
	double CLfMass;		// [kgC/ha]
	double CLfOld;		// max. leaf mass reached during vegetation period [tC/ha yr-1]
	double CLfLim;		// Leaf Mass Limit [tC/ha]
	double CLfLim_kg;	// Leaf Mass Limit [kgC/ha]
	double CFRtMass;	// Fine Root Mass [kgC/ha]
	double CFrMass;		// [kgC/ha]
	double CAss;		// Assimilates in Wood [kgC/ha]
	double CAssReserve;	// Assimilate Reserve [tC/ha]
	double kN;			// nitrogen availability factor [-]
	double CSurplus;	// assimilate surplus supply rate [tC/ha*yr]
	double NSurplus;	// nitrogen surplus supply rate [tN/ha*yr]
	
	/* Morphology Parameters */
	int Form_var;		// variable form factor
	double Form;		// form factor [-]
	double BrWdFrCoeff;	// exponential factor for calculating the branch wood fraction
	double BrWdFr;		// branch wood fraction of above ground wood [-], is assumed to decrease expon. with increasing tree diam
	double FiBrWdFr;	// final brachwood fraction [-]
	double CWdDens;		// Density of Wood expressed in Carbon	[tC/m3]
	double CLfDens;		// Area Specific Carbon Leaf Mass		[gC/m2]
	double CWdWeight;	// calculated from eq. 15 and 16 in BOSSEL
	double CStMass;		// Stem Carbon Mass [kgC/ha]
	double CTrMass;		// Tree Carbon Mass [tC/tree]
	double Dmax;
	double Dred;	
	double b;			// ratio of respiring to total wood [-]
	double RespLay;		// Thickness of resp. Stemwood Layer [m]
	double CWdResp;		// Respiring Wood Mass [tC/ha]
	double LAI;			// Leaf Area Index [-]
	double LAImax;		// max. LAI [-]
	double LAImaxH;		// max. LAI, if tree is not mature [-]
	double LAItemp;		// temp. LAI [-]
	double LightExt;	// Light extinction coefficient [-]
	double LightFrac;	// Light Fraction Request for Production	[-]
	double LfFRt;		// Leaf to Fine Root Ratio [tC/tC] => !!! is RECIPROCAL of BOSSEL, 1996 lambda_RL !!!
	double FiFrLf;		// Final Fruit to Leaf Ratio [tC/tC]
	
	/* Geometric Relations */
	double CD;			// Crown to Stem-Diameter Ratio [m/m]
	double HD;			// Height to Base Diameter ratio [-]
	int    HDvar;		// variable HD ratio according to Forschungszentrum Waldökosysteme (Jansen, Martin, 1995)
	double HDmin;		//
	double HDmax;		//
	double CrArea;		// Crown Area of Tree [m2]
	double CrArPot;		// Potential Crown Area of Stand [ha/ha]
	
	/* Photosynthesis */
	double DPhotoTime;		// Time Step of Canopy Photosynthesis
	int DPhotoLayers;	// Number of Layers between Main Layers (Numeric Calculation of Canopy Photosynthesis)
	double CanopyDepth;
	
	double GrossCanopyPrd_R;
	double GrossCanopyPrd_kgm2yr;
	double GrossCanopyPrd_dt;
	double GrossCanopyPrd_Day;
	double GrossCanopyPrd_Sum;
	
	double CanPrd;
	double PhCanopyPrd;		// canopy production rate [tC/ha*yr]
	double PhCanopyPrd_kgm2yr;
	double PhCanopyPrd_dt;	// act. canopy production [tC/ha*timestep]
	double PhCanopyPrd_Day;
	double PhCanopyPrd_Yr;
	
	double PhCanopyAbove;	// above canopy production of leaves [kgC/kg Lf C]
	double PhCanopyNoN;		// daily canopy production without Nitrogen limitation
	double PhCanopyNoN_dt;
	double PhCanopyNoN_Day;
	double PhCanopyNoN_Yr;
	
	double PhCanopyNlim_TREEDYN;	// daily canopy production with Nitrogen limitation according orig. source code
	double PhCanopyNlim;			// daily canopy production with Nitrogen limitation
	double PhCanopyNlim_dt;
	double PhCanopyNlim_Day;
	double PhCanopyNlim_Yr;
	double PhCanopyNlim_temp;
	double PhCanopyNlim_h;
	
	double PhCan_Tot;				// summed canopy production without any deficiency factors [tC/ha]
	
	double TransCan_dt;
	double TransCan;
	double TransCan_Day;
	double TransCan_Tot;
	
	
	double PhotoNet_BOSSEL;	// Net Photosynthesis [mol/m2*s]	=> see further down (NetPhotoMol)
	double LfPhotoR;		// leaf photosynthesis rate [mgCO2/dm²*h]
	
	double PhProdMax;		// max. photoproduction [mgCO2/dm2*h]
	double PhInitSlope;		// init. slope photo sensivity curve [(mgCO2/dm2*h)/(W/m2)]
	double PhTopt;			// temperature of optimum photosynthesis [°C]
	double PhTzero;			// temperature of zero photosynthesis [°C]
	double PhCO2Fac;		// photosynthetic increment with doubled CO2 [-]
	
	double K;				// Frost Hardiness Factor [-]
	double k_Tp;			// Temperature effect on photosynthesis [-], eq. 38
	double StressFacPh;		// Stress Factor Photosynthesis
	double StressFacLf; 	// Stress Factor Leaf
	double StressFacTrans;	// Stress Factor Tact/Tpot
	
	double Ipar;			// annual PAR radiation rate [Wh/m2*yr]
	int NumLfLay;					//
	double LfFillFrac[MAX_LAYERS];	// Percentage of Layer filled with Leaves (100 layers max)
	
		//--- Parameters of the FALGE model ---
	double SigmaC;				// above canopy production of leaves	[kgC/kgCLf]
	double ko298;				// Michaelisconst. of Rubisco (ox.)		[mmol/mol]
	double kc298;				// Michaelisconst. of Rubisco (carb.)	[µmol/mol]
	double vcmax298;			// maximum carboxylation velocity		[µmol/m2*s]
	double darkresp298;			// Dark respiration						[µmol/m2/s]
	double tau298;				// apparent specifity factor of Rubisco [-]
	double pml298;				// Amax in maximum light and CO2 		[µmol/m2/s]
	
	double CO2conc;				// constant CO2 concentration 			[µmol/mol], [ppm]
	double CO2incr;				// CO2 increase per year				[µmol/mol], [ppm]
	
		//scaling constants for the original Falge model (p. 279) Falge et al 1996
	double f_Ko;
	double f_Kc;
	double f_tau;
	double f_resp;
	
		// Temperature dependence of Ko, Kc, Tau, respiration
	double DeltaHaKo;
	double DeltaHaKc;
	double DeltaHatau;
	double DeltaHaresp;
		//  Temperature dependence of Pml
	double DeltaHaPml;		// activation energy
	double DeltaHdPml;		// deactivation energy
	double DeltaSPml;		// entropie
		// Temperature dependence of VcMax
	double DeltaHaVcMax;	// activation energy
	double DeltaHdVcMax;	// deactivation energy
	double DeltaSVcMax;		// entropie
	
	double alpha;				// initial slope of the light response curve [molCO2/E]
	double GFac;				// = gfac already corrected by soil water status
	double gfacbase;			// sensitivity of stomata [-]
	double widthofleaf;			// width of leaf [cm]
	double sigmaleaf;			// coniferous leaf coefficient [-]
	double StomCond;                //=gpres;   stomatal conductance
	double StomCondMax;			// maximum stomatal conductance	[mmol/m2*s]	  
	double StomCondMin;			// minimum stomatal conductance	[mmol/m2*s] 
	double totaltoprojm2factor;	// total to projected leaf area factor(m2/m2)
	double lext;				// extinction coefficient k	(FRANCE THORNLEY, p167)	[-] 
	double prodlim;				// total to projected leaf area factor	[m2/m2]
		// Partial Vapour Pressures
    double ewleaf;		// Wasserdampfpartialdruck   im Blatt
	double ewair;		// Wasserdampfpartialdruck   in der Luft
	double ewr;         // Hilfsvariable
	double ewdr;		// Hilfsvariable
	double vpd;			// vapor pressure deficit
	//double rtk;		// Hilfsvariable ideale Gaskonstante * TempKelvin	==> see in climate structure
	double laminarCond;	// =ga;   Leitfaehigkeit der laminaren Schicht
	double dbl;         // Dicke der laminaren Schicht
    double dwv;         // Diffusionskoeffizient fuer Wasserdampf
	double Tleaf;		// Leaf Temperature
	double WidthOfLeaf; // [cm]
	double SigmaLeaf;	// =sigmal;  irgendwas mit der Dichte der Nadeln am Zweig (?) NULL Value = -1
	double Ko;			// [µmol/mol] =ko;  Michaelis-Menten Konstante fuer O2, used e.g. in Baldocchi()
	double Ko_Falge;	
	double Kc;			// [µmol/mol] =kc;  Michaelis-Menten Konstante fuer CO2, used e.g. in Baldocchi()
	double Kc_Falge;
	double VcMax;		// [µmolCO2/m2Leaf*s-1] =vcm, maximal carboxylation rate, used e.g. in Baldocchi()	
	double VcMax_Falge;
	
	double DarkResp;    	// Dark Leaf Respiration	
	double DarkResp_Falge;	// Dark Leaf Respiration strictly according to Falge et al 1996
	double DarkResp_dt;		// Dark Leaf Respiration for time step
	double DarkResp_Day;	// Dark Leaf Respiration for a Day
	double DarkResp_Sum;	// Dark Leaf Respiration over Simulation Period
	double tau;         // =tau; enzymspezifischer Faktor
	double tau_Falge;
	double Pml;			// potential rate of RuBP regeneration at light and CO2 saturation (Gayler version acc. to Falge et al 1996)
	double Pml_Falge;	// potential rate of RuBP regeneration at light and CO2 saturation strictly according to Falge et al 1996
	double Pm;          // =pm; potential rate of electron transport
	double Pm_Falge;
	double Cint1;
	double Cint;
	double Cint_Falge;	
	double fac;         // [-] new from Eva Falge, that dark respiration in light conditions are lower than in night
	double Gamma;       // [µmol/mol] =gamma; CO2 compensation point in the absence of dark respiration (resp = photosynth)
	double Gamma_Falge;
	double Wc;
	double Wc_Falge;
	double Wj;
	double Wj_Falge;
	double PnetRd;
	double NetPhotoMol;	// =pnet; die Nettophotosynthese [µmol/m2*s]
	double NetPhotoMolLay;
	double NetPhotoMol1;
	double NetPhotoMol2;
	double Pnet1;
	double Pnet2;
	
	double NetPhotoMol_Falge;
	double NetPhotoMolCanopy;	// summed NetPhotoMol for one time step [µmol/m2*s]
	double NetPhotoMolCanopy_Day;
	double NetPhotoMolCanopy_Sum;
	
	double GPP_Layer;
	double GPP_Canopy;
	double GPP_Canopy_Day;
	double GPP_Canopy_Sum;
	
	double Cs;          // CO2-Konz. an der Blattoberflaeche
    double Hs;          // relative Feuchte an der Blattoberflaeche
	double photosyn_brutto;	// brutto photosynthesis
	
	double Transpiration;	// calcultated with the FALGE module
	double Transpiration_Falge;
	double Transpiration_Day;
	
	double Transpiration_LayerR;
	double Transpiration_Layerdt;
	double Transpiration_LayerTot;
	
	// light interception sub module LEUNING
	double kd;					//extinc. coeff. for diffuse radiation & black leaves	[-]
	double sigmal;				//scattering coeff. for visible light					[-]
	double rhocd;				//canopy reflection coeff. for diffuse visible radiation[-]
	
	
	double WaterUpFac;
	
	/* Increment */
	double CanopyFormation;	// which of the cases occurs during the simulation period
	double IncrLimitation;	//C-limitation = 0, N-limitation = 1
	double Increment;	//Rate of Increment [tC/ha*yr]
	double IncrD;		//Diameter Increment Rate [m/yr]
	double IncrH;		//Height Increment Rate [m/yr]
	double Incr_fh;		//Reduction factor for Height increment
	double Incr_fd;		//Reduction factor for Diameter increment
	double Incr_Red;	//resulting reduction factor
	double Incr_p;		//factor for increment behaviour (see. p10 in Forschungszentrum Waldökosysteme, Jansen et al. 1995)
	double IncrD_Day;
	double IncrH_Day;
	double IncrD_Sum;	// sum over sim period [m]
	double IncrH_Sum;	// sum over sim period [m]
	
	/* Radiation */
	double Ra_h;	// extraterrestrial irradiance [MJ/m2*h], rate
	double Ra_hDay;	// extraterrestrial irradiance [MJ/m2*d], integrated day sum
	double Ra_d;	// extraterrestrial irradiance [MJ/m2*d], rate
	double RaDay;	// integrated extraterrestrial radiation
	
	double Rs;		// global radiation/solar radiation [MJ/m2], rate
	double RsDay;	// integrated solar radiation
	
	double Rs0;		// clear sky solar radiation [MJ/m2], rate
	
	double Rns;
	double Rnl;
	double Rn;		// net radiation [MJ/m2*d], rate
	double Rn_dt;	// net radiation
	double RnDay;	// net radiation [MJ/m2], integrated day sum
	
	/* Interception */
	double CanopyReflection;
	
	double CanopyInputR;	// [mm/d]
	double CanopyInput_dt;	// [mm]
	double CanopyInput_Day;	// [mm/d]
	
	double InterceptionR;	// [mm/d]
	double Interception_dt;
	double Interception_Day;
	double Interception_Year;
	
	double Epot_R;			// [mm/d]
	double Epot_dt;
	double Epot_Day;			// [mm/d]
	double Epot_Yr;
	
	double Eact_R;			// [mm/d]
	double Eact_dt;
	double Eact_Day;
	double Eact_Yr;
	
	double CanopyWatR, CanopyWatR_old;			// Canopy Water [mm] 
	double CanopyWat_dt, CanopyWat_dt_old;
	double CanopyWat_Day, CanopyWat_Day_old;
	
	double SaturationCap;	// Saturation Capacity [mm]
	double C_S_dt;				// Canopy Storage / Storage Capacity
	
	double ThroughfallCoeff;	// [-]
	double a_Drain;				// [-]
	double b_Drain;				// [-]
	double K_Drain;				// [-]
	
	double FreeThroughfallR;	// [mm/d]
	double FreeThroughfall_dt;	// [mm]
	double FreeThroughfall_Day;	// [mm/d]
	double FreeThroughfall_Yr;
	
	double ThroughfallR;	// [mm/d]
	double Throughfall_dt;
	double Throughfall_Day;	// [mm/d]
	double Throughfall_Yr;
	
	double DrainageR; 		// [mm/d]
	double Drainage_dt;		// [mm]
	double Drainage_dt2;	// [mm]
	double Drainage_Day;	// [mm]
	double Drainage_Yr;
	
	double CanPreci_Yr;
	
	double PreciRateR;		// [mm/d]
	double PreciRate_dt;	// [mm]
	double PreciRate_Day;	// [mm]
	double PreciRate_Yr;
	
	double IE_P;			// [-], ratio: Interception Loss / Precipitation
	
	// Balance
	double Bal_R;
	double Bal_T;
	double Bal_E;
	double Bal_I;
	double Bal_D;
	double Bal_P;
	
	/* Nitrogen Contents */
	double NWd;			// Nitrogen in Wood [kgN/ha]
	double NWdResp;		// N in respiring Wood, 	[tN/tC],	pPl->pPltNitrogen->fGrossRootActConc, fBranchActConc
	double NWdDead;		// N in dead Wood,			[tN/tC],	pPl->pPltNitrogen->fStemActConc
	double NLf;			// N in green Leaves,		[tN/tC],	pPl->pPltNitrogen->fLeafActConc
	double NLfDead;		// N in yellow Leaves 		[tN/tC],	(nitrogen content of leaves at abcission) [tN/tC]
	double NFr;			// N content of Fruits,		[tN/tC],	pPl->pPltNitrogen->fFruitActConc
	double NFRt;		// N content of Fine Roots, [tN/tC],	pPl->pPltNitrogen->fRootActConc
	double NConcWd;		// Average Nitrogen Concentration in Total Wood [tN/tC]
	double NWdTot;		//
	
		//
	double LeafContent;		// pPlN->fLeafCont
	double RootContent;		// pPlN->fRootCont
	double GrainContent;	// pPlN->fGrainCont
	double TotalContent;	// pPlN->fTotalCont
	
	/* Roots */
	double RtLength;		// Root Length
	double RtLengthSpec;	// specific root length [cm/g], pPl->pGenotype->fRootLengthRatio
	double RtLengthDensFac[50];	// Root Length Density Factor, pLRt->fLengthDensFac, starts with [0]
	double RtLengthDensFacTot;	// Total Root Length Density Factor
	double RtLengthDens[50];	// Root Length Density [cm/cm2]
	double RNLF;
	double WaterUpMax;		// max water uptake rate [cm³/cm*d]
	double NUpMax;			// max nitrogen uptake rate [mgN/cm*d]
	double TRootZero;
	double TRLD;			// total root length density
	
	double PotLayRtWaterUp[MAX_LAYERS];
	double PotLayRtWaterUpdt[MAX_LAYERS];	// actual root water uptake in soil layers, sum
	double PotLayRtWaterUpDay[MAX_LAYERS];	// actual root water uptake in soil layers, sum
	
	double ActLayRtWaterUp[MAX_LAYERS];	// actual root water uptake in soil layers, rate
	double ActLayRtWaterUpdt[MAX_LAYERS];	// actual root water uptake in soil layers, sum
	double ActLayRtWaterUpDay[MAX_LAYERS];	// actual root water uptake in soil layers, sum
	
	double PotRtWaterUp;
	double PotRtWaterUpdt;
	double PotRtWaterUpDay;
	
	double ActRtWaterUp;
	double ActRtWaterUpdt;
	double ActRtWaterUpDay;
	
	double ActRtWaterUpTot;
	double PotRtWaterUpTot;
	
	double RtWaterUp;		// rate
	double RtWaterUpdt;
	
	double SWDefFac;		// soil water deficiency factor
	
	/* Damage */
	double DamageLf;		// Leaf Damage [-]
	double DamagePh;		// Photosynthesis Damage [-]
	double DamageFRt;		// Fine Root Damage [-]
	
	/* Litter */
	double CNratio[40];
	double CNcoeff[40];
	double LitLf;			// pTreesVar->LitterRLeaf
	double LitGrRt;
	double LitRt;
	double LitBr;
	double LitSt;
	double LitWd;
	double LitFr;
	
	double CLitLf_Sum;
	double CLitSt_Sum;
	double CLitRt_Sum;
	double CLitBr_Sum;
	
	
	double UgWdFr;			// Below Ground Wood Fraction [-]
	double DeadWdLoss;		// Dead Wood Loss Rate [1/yr]
	double DeadWdLossTot;	// Total dead wood loss rate [kgC/ha*yr]
	
	double CReloc;		// Assimilate Relocation [-]
	double CRelocGain;	// Assimilate Relocation Gain [tC/ha*yr]
	double NRelocLit;	// Nitrogen Relocation for Litter [-]
	
		// assimilates
	double CAssGainR;		// assimilates, which are available [tC/ha*yr]
	double CAssGain_dt;
	double CAssGain_Day;
	double CAssLossR;		// assimilates, which are lost through respiration, growth, and dead wood loss [tC/ha*yr]
	double CAssLoss_dt;
	double CAssLoss_Day;
	double CAssBalanceR;	// balanced assimilate gain and loss [tC/ha*yr]
	double CAssBalance_dt;
	double CAssBalance_Day;
	
	/* Fresh Organic C & N */
	double LyFc[40];	// Root Length Distribution [-]
	
	double FOCLfFr;		// fresh organic carbon: leaves and fruits litter [tC/ha]
	double FOCBrSt;		// fresh organic carbon: branches, twigs and stems originated litter [tC/ha]
	double FOCGrRt[40];	// fresh organic carbon: gross roots litter in soil layer [i] [tC/ha]
	double FOCFiRt[40];	// fresh organic carbon: fine roots litter in soil layer i [tC/ha]	
	double FONLfFr;		// fresh organic nitrogen: leaves and fruits litter [tN/ha]
	double FONBrSt;		// fresh organic nitrogen: branches, twigs and stems originated litter [tN/ha]
	double FONGrRt[40];	// fresh organic nitrogen: gross roots litter in soil layer [i] [tN/ha]	
	double FONFiRt[40];	// fresh organic nitrogen: fine roots litter in soil layer i [tN/ha]	
	double initFOCLfS;	// initial leaf litter after shedding [tC/ha]
	double initFONLfS;	// initial leaf litter after shedding [tN/ha]
	
	double LitMinerRel[40];	// pChL->fLitterMinerR
	double HumMinerRel[40];	// pChL->fHumusMinerR
	double CHumMinerR;		// CHumusMinerR (lokal)
	double NImmobR[40];		// pChL->fNImmobR
	double ChemX[40];		// pChL->fChemX = NNOM, in nitrogenmineralization
	
	
	
	/* Mineralization */
	double CLit;	// Carbon in Litter [kgC/ha]
	double CSOM;		// Carbon in soil organic mass
	double NLit;	// N in Litter [kgN/ha]
	double NSOM;		// Nitrogen in soil organic mass
	
	double LitHumif;	// Litter Humification Rate [tC/ha*yr]
	double LitDecom;	// Litter Decomposition Rate [tC/ha*yr]	
	double HumDecom;	// Humus Decomposition Rate [tC/ha*yr]
	double NTransHumif;	// Nitrogen Transfer Rate in Humification [tN/ha*yr]
	double NLitMiner;	// Nitrogen Mineralization of Litter [tN/ha*yr]
	double NHumMiner;	// Nitrogen Mineralization of Humus [tN/ha*yr]
	double FMinerR;
	
	double Decomp;		// Normal Decomposition Rate [1/yr]
	double Humif;		// Humification Rate [1/yr]
	double HuMiner;		// Humus Mineralization Rate [1/yr]
	double Leach;		// Plant-available Nitrogen Leaching Rate [1/yr]
	double NLeach;		// Nitrogen Leaching [tN/ha*yr]
	double NFixMax;		// Maximum Nitrogen Fixation Rate [1/yr]
	double NFix;		// Nitrogen Fixation [tN/ha*yr]
	double NDepos;		// Nitrogen Deposition Rate [tN/yr]
	
	double NDeposTotal;	// total N Deposition [kgN/ha*yr]
	double NO3_DepIncr; // Nitrate Deposition Increase per year [kgN/ha*yr]
	int NO3_DepConstYr; // constant Nitrate Deposition from this year on
	int N_DepStartYr;
	double N_DepStart;
	
		// fagus variables
	double SoilMinLit;	//Litter
	double SoilMinRt;	//Fine Roots
	double SoilMinHum;	//Correction factor for Humus Mineralization
	double LeafMinLig;	//Lignin
	double LeafMinCel;	//Cellulose
	double LeafMinCHy;	//Carbohydrates
	
	/* Senescence and Mortality */
	double TOLf;		// Turnover Rate Leaves [1/yr]
	double TORt;		// Turnover Rate Fine Root [1/yr]
	double TOFr;		// Turnover Rate Fruit [1/yr]
	
	double TOLfMass;
	double TORtMass;
	
	
	/* (Evapo)Transpiration */
	double PotTransDay;	// sum
	double PotTransdt;	// sum
	double PotTrans;	// rate, [mm/d]
	
	double ActTransDay;	// sum
	double ActTransdt;	// sum
	double ActTrans;	// rate
	
	/* Soil */
	double TSoilRef;
	double TSoilZero;
	double T_veg;
	double damp_T;
	
	/* === Biomass Growth === */
		// --Respiration--
	double MainRespLf;	 // [tC/ha*yr], Leaves
	
	double MainRespLfLay_R;
	double MainRespLfLay_dt;
	double MainRespLfLay_Day;
	double MainRespLfLay_Sum;
	
	double MainRespLf_Bossel;
	double MainRespLf_Bossel_dt;
	double MainRespLf_Bossel_Day;
	
	double MainRespLf_Falge;
	
	double MainRespSt;	 // [tC/ha*yr], Stems
	double MainRespGr;	 // [tC/ha*yr], "Grains" (Fruits)
	double MainRespRt;	 // [tC/ha*yr], Roots
	double MaintRespR;	 // [tC/ha*yr], Total Maintenance Respiration 
	
	double MainRespSt_Day;
	double MainRespRt_Day;
	double MainRespLf_Day;
	double MaintResp__Day;
	
	double MaintRespLf_Sum;
	double MaintRespSt_Sum;
	double MaintRespGr_Sum;
	double MaintRespRt_Sum;
	double MaintResp_Sum;
	
	
	double DRespTime;		//Time Step of Respiration Calculation
	double TRespZero;		//Temperature of zero Activity [degC]
	double TRespRef;		//Reference Temperature, Temp effect on Respiration [degC]
	double RespPeriodFr;	//Fruit Respiration Period [mon], TauF in (Bossel, 1996, 1997, p207)
		// Respiration Rates
	double RespGr;		//Growth Resp. Rate [-]
	double RespLf;		//Leaf Resp. Rate [1/yr]
	double RespRt;		//Fine Root Resp. Rate [1/yr]
	double RespWd;		//Wood Resp. Rate [1/yr]
	double RespFr;		//Fruit Resp. Rate [1/yr]

		// Temperature Dependencies
	double kTResp;		// Respiration [-]
	double kTResp_Falge;
	double kTResp_Bossel;
	
	double kTRespNight;	// Leaf Respiration, (only nighttime) [-]
	double kTRespSoil;	// Soil Processes [-]
	
		// Nitrogen and Assimilate Relocation
	double LeafNtransR;
	double RootNtransR;
	double NRelocTotR;		// tot. Nitrogen Relocation Rate [tN/ha*yr]
	double CUseSpec;		// specific assimilate use rate [1/yr]
	double kA;				// Assimilate Availability Factor [-]
	double kNLfRt;			// Nitrogen Availability for Leaf and Root Renewal [-]
	
		// C/Assimilate Demand Rates
	double CLfDemand;
	double CRtDemand;
	double CTotDemand;
	double CAssSupply;	// rate at which assimilate becomes available [tC ha-1 yr-1]
	double CLfRtTotDemand;	// rate at which assimilate is required for leaf and fine root demand [tC ha-1 yr-1]
	double AssimilateBalance;	// Assimilate Production - Leaf/Root Demand [tC/ha*yr]
	
		// N Demand Rates
	double LeafDemandRate;	// [tN/ha*timestep] pPlN->fLeafDemand
	double RootDemandRate;	// [tN/ha*timestep]
	double TotalDemandRate;	// [tN/ha*timestep]
		
		// Death Rates
	double LeafDeathRate;	// leaf shedding rate [tC/ha*yr], lloss, pBio->fLeafDeathRate
	double FruitDeathRate;	// fruit loss rate [tC/ha*yr], floss, not used
	double RootDeathRate;	// fine root loss rate [tC/ha*yr], rloss
	
	int LfFallStart;	//Start of Leaf Fall [DOY]
	int LfFallEnd;	//End of Leaf Fall [DOY]
	double LfFall;		//Leaf Fall Rate [1/yr]
	double LfFlush;		//Leaf Flush Rate [1/yr]
	double LfBudBurst;	//Bud Burst Constant
	
		// --Growth Rates--
	double LfGrR;	// actual leaf renewal and growth rate [tC/ha*yr], lgrow (BOSSEL), pBio->fLeafGrowR (XN)
	double RtGrR;	// pBio->fRootGrowR (XN) [1/yr]
	double FrGrR;
	
	double CLfRtConstr;		// assimilate drain for leaf and root construction [tC/ha*yr]
	double CIncrConstr;		// increment under assimilate limitation and nitrogen limitation
	double CGrTot;			// total assimilate use for growth [tC/ha*yr]
	
		// Nitrogen Uptake
	double NSoil;			// plant available N in Soil [kgN/ha]
	double NUpSpecR;		// specific Nitrogen Uptake Rate [ha/yr/tC]
	
	
	double NUpActR;			// act. Nitrogen Uptake Rate [tN/ha*yr], pTreesVar->NUp
	double NUpAct_dt;		// act. Nitrogen Uptake Rate [kgN/ha*timestep]
	double NUpAct_1_Day;	// act. Nitrogen Uptake 	 [kgN/ha*day] 	= ((self->plant[i].ActLayNO3NUpt[L] + self->plant[i].ActLayNH4NUpt[L])*dt)
	double NUpAct_2_Day;	// act. Nitrogen Uptake 	 [kgN/ha*day]	= ((self->plant[i].ActNH4Up*dt) + (self->plant[i].ActNO3Up*dt))
	double NUpAct_3_Day;	// act. Nitrogen Uptake 	 [kgN/ha*day]	= 
	double NUpAct_Sum;		// act. Nitrogen Uptake 	 [kgN/ha*period]
	
	double NUpLfRtR;		// tot. Nitrogen Uptake Rate [tN/ha*yr], pTreesVar->NUpX
	double NUpLfRt_dt;		// tot. Nitrogen Uptake		 [tN/ha*timestep]
	double NUpLfRt_Day;		// tot. Nitrogen Uptake		 [tN/ha*day]
	double NUpLfRt_Sum;		// tot. Nitrogen Uptake over Simulation Period [kgN/ha]
	
	double NUpNormFac;		// Normalization Factor for Nitrogen Uptake [-]
	
	double NUpPotR;			// pot. Nitrogen Uptake Rate [tN/ha*yr]
	double NUpPot_Day;		// [tN/ha]
	
	
	
	double NO3Up[MAX_LAYERS];	// NO3-N Uptake (FAGUS-variable), [kgN/ha] 
	double NH4Up[MAX_LAYERS];	// NH4-N Uptake (FAGUS-variable), [kgN/ha] 

	double ActNO3Up;			// pPltN->fActNO3NUpt
	double ActNH4Up;			// pPltN->fActNH4NUpt
	
	double ActNO3Up_Day;			// pPltN->fActNO3NUpt
	double ActNH4Up_Day;			// pPltN->fActNH4NUpt
	
	double ActNO3Up_Sum;
	double ActNH4Up_Sum;

	double ActLayNO3NUpt[MAX_LAYERS];	// pLR->fActLayNO3NUpt
	double ActLayNH4NUpt[MAX_LAYERS];	// pLR->fActLayNH4NUpt

	double NO3Nmgkg[MAX_LAYERS];	// pSLN->fNO3Nmgkg
	double NH4Nmgkg[MAX_LAYERS];	// pSLN->fNH4Nmgkg
	
		// N Increase
	double NIncrLeaf;	// [tN/ha*timestep], amount of increased nitrogen in leaf due to uptake,  pPlN->fLeafNinc
	double NIncrRoot;	// [tN/ha*timestep], amount of increased nitrogen in root due to uptake,  pPlN->fRootNinc
	double NIncrFruit;	// amount of increased nitrogen in grain due to uptake, pPlN->fGrainNinc
	double NIncrStem;	// amount of increased nitrogen in grain due to uptake, pPlN->fStemNinc
	
	double IncrementTotal;

	/* === Canopy Formation === */
	double IncrSt;		// Stem increment
	
	/* === Root Length Growth === */
	double RtLengthNew;
	double RtLengthDead;
	double DepthMax;
	double SWDefRel[50];
	double RtWaterStress[50];
	double RtNStress[50];
	double RLDTemp[50];
	
	/* Loss Rates [tC/ha*yr]*/
	double CLossLf;		//Leaf
	double CLossRt;		//Fine Root
	double CLossFr;		//Fruit
	double CLossDeadTrees;	// Assimilate Loss in dead trees [tC/ha/yr]
	
	double DeadTree;			// Tree dieback rate [1/yr]
	double TotDeadWdLoss;		// Total dead wood loss rate [kgC/ha*yr]

	double NLossLf;		// pPltN->fLeafNlossRate
	double NLossWd;		// pTreesVar->NLossWood
	double NLossFr;		// pTreesVar->NLossFruit
	double NLossTot;	// pTreesVar->NLossTotal
	double NLossRt;		// pPlN->fRootNlossRate
	double NLossGrRt;	// pPltN->fGrossRootNlossRate
	double NLossSt;		// pPltN->fStemNlossRate
	double NLossBr;		// pPltN->fBranchNlossRate
	
	double LitInputDecomposition;	//Litter Input Rate (for Decomposition) [tC/ha*yr]
	double CSurfaceLitterInputCENTURY;		//
	double CUndergrLitterInputCentury;
	double NInputLit;	//Nitrogen Input Rate to Litter			[tN/ha*yr]
	
		// Mortality
	int MortInitYr;	//Beginning of Mortality [year]
	double MortNormInit;	//Mortality at beginning of simulation [1/yr]
	double MortCrowdInit;	//Mortality at beginning of simulation [1/yr]
	double MortNorm;	//Normal Mortality Rate [1/yr]
	double MortCrowd;	//Crowding Mortality Rate [1/yr]
	double MortTotal;			// Total Mortality Rate [1/yr]

} P_Plant;

typedef struct		// [Silviculture]
{
	int ThinningEvents;		//number of thinning events
	gdouble *ThinningInterval;
	gdouble *ThinningFraction;
	double LitterRemoval;		// litter removal fraction [-]
	int ThinningCounter;
	int ThinningMethod;
	double heightAfterCut; //added by Hong on 20180716
	double diameterAfterCut;
		
		// Bool Operators
	int StandAlive;
	int PlantGrowth;
	int Maturity;
	int Harvesting;
	double Crowding;
	int Dormancy;
	
	double CWdHarv;
	double CStHarv;
	double CTotWd;
	
	double CLit;		// Carbon in Litter [kgC/ha]
	double NLit;		// Nitrogen in Litter [kgN(ha]
} P_Silv;

typedef struct		// [Climate]
{	
	int NewYear;		// boolean
	int Polar;			// boolean
	int Tropical;		// boolean
	int Seasonal;		// boolean
	
	int YrCnt;			// 
	int YrOld;			// sim year from previous time step
	int YrAct;			// sim year from actual time step
	int CO2Yr;			// start year of CO2 increase
	int YrActDeposition;
	
	double SeasTime;	// seasonal time [day]
	double DayLength;	// day length [h]
	double NightFrac;
	double hAmp;		// day amplitude [h]
	double hr;			// hour
	double elev;		// solar elevation
	
	double Lat;			// geogr. latitude
	double DayFirstSun;
	double DayLastSun;
	double AtmAbs;		// atm. absorption
	double PARfactor;	
	double SolarConstant;
	double CloudFraction;
	double SunltFrac;		// [-]
	
	double IAmpClear;
	double IAmpCloud;
	double IAvgClear;
	double IAvgCloud;
	
	double IClearDay;
	double ICloudDay;
	
	double Iclear;
	double Icloud;
	double Isumclear;
	double Isumcloud;
	
	double IsumclearMax;
	double IsumclearMin;
	double IsumcloudMax;
	double IsumcloudMin;
	
	double PARClear;
	double PARCloud;
	double PARCanopy;	// substitues PARClear and PARCloud; Radiation comes directly from WRF, where radiation from clouds is already considered
	
	double IPARCanopy;
	double IPARCanopyClear;	// Radiation within the canopy, diminished by leaf layers [W/m²]
	double IPARCanopyCloud;
	
	double BulkPrecR;		// from input data [mm/d]
	double LiquPrecR;
	
	double PreciRate;		// Precipitation Rate [mm/d]
	double Preci_dt;		// Precipitation [mm/dt]
	double Preci_Day;		// Daily Precipitation [mm/d]
	double Preci_Sum;		// Precipitation Sum [mm]
	double PreciRateControl;	// Control: Throughfall + Interception (for all species) [mm/d]
	
	double InterceptR;
	double ThroughfallR;
	
	double CanPreciRate;	// Canopy Precipitation [mm/d]
	double CanPreci_dt;		// Canopy Precipitation [mm/dt]
	double CanPreci_Day;	// Daily Canopy Precipitation Sum [mm/d]
	double CanPreci_Sum;	// Canopy Precipitation Sum [mm]
	
	double AirPressure;
	double T_air;
	double T_soil;
	double CO2_air;
	double rtk;				// Hilfsvariable ideale Gaskonstante * TempKelvin
	double CO2_incr_fac;
	
	double IAboveCanopyClear;
	double IAboveCanopyCloud;
	double CloudyDaysFrac;
	double Cloud;
	
} P_Clim;


struct _libtreemix
{
	expertn_modul_base parent;
	/* add your public declarations here */
	
	/* Following structs are initialized only one time */
	P_Conf conf;
	P_Clim clim;
	
	/* Following structs are initialized depending on the species count */
	P_Plant *plant;
	P_Silv *silv;
	
	//double *mean_temp;

	
	int __INIT_DONE;
	int __ERROR;
};


// public class member function:
G_MODULE_EXPORT int libtreemix_load(libtreemix *self);

G_MODULE_EXPORT int libtreemix_potential_transpiration_run(libtreemix *self);
G_MODULE_EXPORT int libtreemix_actual_transpiration_run(libtreemix *self);
G_MODULE_EXPORT int libtreemix_Phenology(libtreemix *self);
G_MODULE_EXPORT int libtreemix_Photosynthesis(libtreemix *self);
G_MODULE_EXPORT int libtreemix_BiomassGrowth(libtreemix *self);
G_MODULE_EXPORT int libtreemix_CanopyFormation(libtreemix *self);
G_MODULE_EXPORT int libtreemix_RootLengthGrowth(libtreemix *self);
G_MODULE_EXPORT int libtreemix_NitrogenDemand(libtreemix *self);
G_MODULE_EXPORT int libtreemix_NitrogenUptake(libtreemix *self);
G_MODULE_EXPORT int libtreemix_Senescence(libtreemix *self);

G_MODULE_EXPORT int libtreemix_done(libtreemix *self);

// external functions
extern double min3(double x1,double x2,double  x3);
extern double max3(double x1,double x2,double  x3);
extern int IsSimStart(libtreemix *self);
extern int IsMemoryAllocated();

extern int libtreemix_set_tree_morphology(libtreemix *self, int i);
extern int libtreemix_set_tree_geometry(libtreemix *self, int i);
extern int libtreemix_set_rootlength_density(libtreemix *self, int i);
extern int libtreemix_set_litter_pools(libtreemix *self);

struct _libtreemixClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType libtreemix_get_type (void);

G_END_DECLS

#endif /* __LIBTREEMIX_H__ */
