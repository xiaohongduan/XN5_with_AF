//      hpm.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM_H__
#define __HPM_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS
//define TEST_HPM

#define HPM_TYPE				(hpm_get_type())
#define HPM(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			HPM_TYPE, hpm))
#define HPM_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			HPM_TYPE, hpmClass))
#define IS_HPM(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			HPM_TYPE))
#define IS_HPM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			HPM_TYPE))

typedef struct _hpm			hpm;
typedef struct _hpmClass		hpmClass;


typedef struct {
	double TSoil;
	double Namm;
	double Nnit;
	double theta_soil_max,theta_soil;//,Psi_so;
} struct_Soil_Vars;

typedef struct {
	double kcan;// = 0.5; // [m2 leaf (m2 ground)-1]-1. Canopy extinction coefficient.
	double XIleaf;// = 0.1; // leaf transmissionskoefficient
	double ksi_ph;// = 0.95;  //Dimensionless.  Leaf photosynthesis parameter.	Convexity parameter of photosynthesis non-rectangular hyperbola.

	double KCO2_Pmax;// =  1.28072e-3;  // CO2 m-3.  Michaelis-Menten constant for the CO2 dependence of Pmax, equivalent to 700 ppm at 20 oC.
	double mu_W;// = 18; // kg/(kg mole) molare Masse vom Wasser
	double Rgas;// = 8314.0; // J (kg mole)-1 K-1
	double Pmax20;// = 2.0e-6; // kg CO2 m-2 s-1. Maximum value of Pmax at 20 oC, saturating CO2, and everything optimum. At 350 vpm CO2, this gives a Pmax20 of 0.667e-6.
	double rmmCS;// = 28.5;  //Dimensionless. Relative molecular masses of C substrate (sucrose / 12 = 342 / 12 = 28.5) and of N substrate (nitrate = NO3 = 14 + 3 * 16 = 62).
	//Also, with units of kg (kg mole)-1,  these are the molar masses of C, N substrates.
	double rmmNS;// = 62.0;  // Dimensionless. Relative molecular masses of C substrate (sucrose / 12 = 342 / 12 = 28.5) and of N substrate (nitrate = NO3 = 14 + 3 * 16 = 62).
	//   Also, with units of kg (kg mole)-1,

	double alpham15;//=1.0e-8; /* [kg CO2 J-1]!Leaf photosynthetic efficiency at 15 C in ambient CO2 [0.05 mol CO2 (mol PAR)^-1]*/
	double beta;//=0.3e-6; /* [kg CO2 m^-2 s^-1]  Photorespiration parameter.*/
	double tau;//=0.0015; /* [m s^-1] CO2 conductance for leaf photosynthesis.*/
	double cTalpha;/*=0.015;  [(°C)^-1] Linear fractional decrease
                    in alpha (see above) per 1 deg C increase in air (leaf)
                    temperature. + 10 deg C gives 15 percent decrease in
                    alpha.  Ref: Loomis, RS and Connor, DJ: Crop Ecology.
                    CUP. 1992. p. 263.*/


	double crleaf;//0.15;  // Dimensionless. Leaf reflectivity coefficient. This is a plant parameter used only in the water submodel.
	//double rNetR_PAR;// = 1.4; // [j (net radiation) (J (Paar)^-1]Ratio of daily net radiation to PAR.
	double cpsiPr;// = 0.2; // kg plant structure (kg water)-1. Parameter of plant pressure potential characteristic.
	double epsilon;// = 2.0e6; //Pa. A sort of Young's modulus for the plant cell.

	double fSos_ac;// = 2.0; // Dimensionless.  Multiplier for plant storage material to give osmotically active amount.
	double rmm_osm;// = 20.0;  // Dimensionless [also, kg (kg mole)-1].  Mean relative molecular mass (molar mass) of osmoticum.



} STRUCT_PHOTOSYNTHESIS;

typedef struct {

	double TmaxPmax350;// =  30.0; // 30.0 C.  Temperature maximum of light-saturated photosynthesis, Pmax, at 350 vpm CO2.
	// Optimale Temp. für Photosynthes bei 350 vpm &
	// kann vllt aus den Vars: pPl->pGenotype->fTempMaxPs bzw. &pPl->pGenotype->fTempMaxPs
	// errechnet werden.

	double TmaxPmax700;// = 35.0; // 35.0 C.  Temperature maximum of light-saturated photosynthesis, Pmax, at 700 vpm CO2.
	// Optimale Temp. für Photosynthes bei 700 vpm &
	// kann vllt aus den Vars: pPl->pGenotype->fTempMaxPs bzw. &pPl->pGenotype->fTempMaxPs
	// errechnet werden.

	double T0p;// = 45.0;  /**<oC.  Temperatures at which the general plant	temperature function is zero.*/
	double qfT;// = 1.9;   /**<Temperature response parameter; gives a cubic.  wird für photosynthese benötigt*/
	double Tref;// = 23.0;  /**<oC.  Reference temperature at which temperature functions = 1.*/
	double mfT;// = 1.0; /**< !Dimensionless.  A multiplier for the temperature function.*/
	double T0;// = 0.0; //°C
} STRUCT_TEMPERATURE;

typedef struct {
	double gsmaxA;// = 0.005; //0.005 m s-1 (m2 leaf)-1. Maximum and minimum values of stomatal conductance at ambient CO2, 350 vpm.
	double gsminA;// = 0.00005; // m s-1 (m2 leaf)-1. Maximum and minimum values of stomatal conductance at ambient CO2, 350 vpm.
	double cgsCO2;//=2.0;       // Dimensionless.  A doubling of ambient CO2 concentration (CO2vpm) reduces stomatal conductance
	// (gstom) by 0.4.  See calculation of stomatal
	// conductance at end of plant submodel section.
	double theta_gsmin;// = 0.65; //  Dimensionless. Value of shoot relative water content, thetash, when stomates are closed.
	double theta_gsmax;// = 0.85; //  Dimensionless. Value of plant relative water content, thetash, when stomates are fully open.
	double thetasoil_max;// = 0.4; //m3 water (m3 soil)-1.  Maximum value of soil relative water content.

	double cWTpl;// = 0.005;  // kg water (kg structural DM)-1 [J (kg water)-1]-1 day-1. Parameter for Water Transport in Plant. It is assumed independent of temperature.
	double cWrs_rt;// = 0.5e6; //  m s-1. Root surface to plant resistance parameter.
	double qpsi_soW;// = 3.5; //Dimensionless. Parameter of soil water characteristic.
	// TODO: sollte eigentlich zeischen 3 und 4 sein --> 6.2h

	//double Ksomax;// = 0.1; //  kg m-3 s.  Parameter of soil hydraulic conductivity characteristic.
	double KWrs_rt;// = 1.0;  // kg structure m-2. Root surface to plant resistance parameter.*/
	double cWso_rs;// = 80.0;  // m2. Soil to root surface resistance parameter.
	double roWpl20;

	double knuK;//0.4; // (dimensionslos) Karmans constant
	double chcanLAI;//0.026; // [m / (m^2 leaf  / (m^2 ground))^-1]

	double rho_Wa;// = 1000.0;// Dichte vom Wassser kg water / m^3


} STRUCT_WATER;

typedef struct {
	double fTuN20;// = 1.0;  // Dimensionless.  Nitrogen	availability constants for nitrate uptake relative to ammonium uptake at 10 and 20 oC.
	double fTuN10;// = 0.5; //Dimensionless.  Nitrogen availability constants for nitrate uptake relative	to ammonium uptake at 10 and 20 oC.
	double c_uN4[4];// = {1.0,0.5,0.25,0.1}; // Dimensionless. Specific root activity (N uptake) weighting parameters; Mrt4[0] is weighted with 1, Mrt4[1] with c_uN2, etc.
	double KMXrtuN;// = 1.0; // kg root structural DM m-2.  Michaelis-Menten parameter causing diminishing returns on root uptake.
	double KNeffuN;// = 0.005; // kg N m-2.  Michaelis-Menten constant for N uptake from the effective soil N pool, NeffuN.
	double KCuN;// = 0.05; // [C].  N uptake Michaelis-Menten parameter.
	double JNuN;// = 0.005; // [N]. N uptake inhibition parameter.
	double sig_uN20;// = 1.0;   // kg N (kg root structure)-1 day-1.Root N uptake parameter at 20 oC.
	double qWuN;// = 3.0; //Effect of soil water content on N uptake.
	double cuNamm;// = 0.4;   // kg C (kg N)-1. Respiratory cost of N uptake from	soil ammonia, Namm.
	double cuNnit;// = 0.5;  // kg C (kg N)-1.  Respiratory cost of N	uptake from soil nitrate, Nnit.

	double fNshli_re_max;// = 0.4; //Dimensionless.  Maximum fractions of root, shoot litter N structure which can be recycled.
	double KNrec;// = 0.02;  // [N].  Michaelis-Menten constant, determines how much of the structural N is recycled.
	double fNSshli_min;// = 0.3;  // Dimensionless. The minimum fraction of N substrate lost in the senescing litter fluxes.
	double KNSshli;// = 0.01; // [N]. Michaelis-Menten constant for N substrate	loss in senescing litter fluxes.

	double fNrtli_re_max;// = 0.4; //Dimensionless.  Maximum fractions of root, shoot litter N structure which can be recycled.
	double kNSrt_ex20;// = 0.005; //day-1. Root exudation of root N substrate.
	double fNSrtli_min;// = 0.3; // Dimensionless.	The minimum fraction of	N substrate lost in the senescing litter fluxes.
	double KNSrtli;// = 0.01; // [N]. Michaelis-Menten constant for N substrate loss in senescing litter fluxes.
} STRUCT_NITROGEN;


typedef struct {
	double roC20;// = 0.2;   //m2 day (kg XDM)**(qroC - 1). C resistivity constant, 20 oC value.
	double roN20;// = 2.0;   //m2 day (kg XDM)**(qroN - 1). N resistivity constant, 20 oC value.

	// Structural
	// SHOOT:
	double kmai_sh120;// = 0.02;   //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double kmai_sh220;// = 0.02;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double kmai_sh320;// = 0.015;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double kmai_sh420;// = 0.01;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double KCmai;// = 0.03;  //[C]. Michaelis-Menten constant switching off shoot and root maintenance at low C substrate levels.

	// ROOT
	double kmai_rt120;// = 0.02;   //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double kmai_rt220;// = 0.02;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double kmai_rt320;// = 0.015;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
	double kmai_rt420;// = 0.01;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.

	// Substrate C
	double fCplX;// = 0.45;  //kg C (kg structure)-1.  Fractional carbon content of plant structural dry matter.
	double fNplX;// = 0.02; // kg N (kg structure)-1.  Fractional nitrogen content of plant structural dry matter.
	double YGpl;// = 0.75;  //Dimensionless. C yield factor for structural plant growth.
	double fCSshli_min;// = 0.3;  // Dimensionless.  The minimum fraction of	C substrate lost in the senescing litter fluxes.
	double KCSshli;// = 0.05; //[C]. Michaelis-Menten constant for C substrate loss in senescing litter fluxes
	double rCNrec_li;// = 2.7;  //kg C (kg N)-1. Ratio of C:N in recyclable structural dry matter.

	double kCSrt_ex20;// = 0.02; // day-1.  Root exudation of root C substrate.
	double fCSrtli_min;// = 0.3;  // Dimensionless. The minimum fraction of C substrate lost in the senescing litter fluxes.
	double KCSrtli;// = 0.05; //[C]. 	Michaelis-Menten constant for C substrate loss in senescing litter fluxes.

	double cSLAmax;// = 25.0;  //(m2 leaf) (kg structural DM)-1 Maximum value of incremental SLA.
	double cSLA_C;// = 2.5;  // [C]-1; incremental specific leaf area (SLA)	parameter, describes dependence on substrate C concentration
	double cSLA_W;// = 1.0;  // Dimensionless. Constant determining the	effect of plant relative water content on leaf area expansion (incremental specific leaf area, SLA).
	double flam;// = 0.7; // Dimensionless.  Fraction of new shoot growth partitioned to lamina.

	double qWpl;// = 20.0;
	double mu20;// = 400.0;//400.0;  //[(day [C] [N])-1].  Growth coefficient at 20 oC
	double kdegsh20;// = 0.001;//0.001;  // day-1. Degradation of dhoot structure.  Half-life: about 1 year.
	double kturnss20;// = 0.08;/*0.08; /* day-1.  Rate parameters for root, laminar, sheath and stem
	//				turnover at 20 oC.  Remember there are 4 compartments.
	//				Half-life = 4 * 0.69 / k days.
	//				For roots, lamina, sheath + stem, about 1 month.
	//				See Whitehead, p. 91. Conflicting data on root turnover.
	//				Maybe we need to treat coarse roots and fine roots
	//				differently (as in forest model).*/
	double kturnlam20;// = 0.08;/*0.08; /*day-1.  Rate parameters for root, laminar, sheath and stem
	/*				turnover at 20 oC.  Remember there are 4 compartments.
					Half-life = 4 * 0.69 / k days.
					For roots, lamina, sheath + stem, about 1 month.
					See Whitehead, p. 91. Conflicting data on root turnover.
					Maybe we need to treat coarse roots and fine roots
					differently (as in forest model).*/
	double kturnrt20;// = 0.08;/*0.08; /**<day-1.
	/*				Rate parameters for root, laminar, sheath and stem
					turnover at 20 oC.  Remember there are 4 compartments.
					Half-life = 4 * 0.69 / k days.
					For roots, lamina, sheath + stem, about 1 month.
					See Whitehead, p. 91. Conflicting data on root turnover.
					Maybe we need to treat coarse roots and fine roots
					differently (as in forest model).*/
	double kdegrt20;// = 0.001;//0.001; //day-1. Degradation of root, shoot structure.  Half-life: about 1 year.

	double depthrtmin;// = 0.02;// m


	double rortmax;// = 1.0; // kg structure (m3 soil)-1. Maximum density of root structural matter.
	double rortmin;// = 0.1; //kg structure (m3 soil)-1.  Minimum density of root structural matter.



} STRUCT_PLANT;


typedef struct {
	double fNret;// = 0.01;		//Dimensionless. Fraction of N intake retained in animal.
	double fCplX;// = 0.45;  //kg C (kg structure)-1.  Fractional carbon content of plant structural dry matter.
	double rCNan;// = 3.5;	  //kg C (kg N)-1. C/N ratio in DM retained in animal.
	double IDMpl_anmax;// = 1.5;  //kg DM animal-1 day-1. Maximum animal intake rate, saturating LAI.
	double KLAIan;// = 1.0;       //m2 leaf (m2 ground)-1. Animal intake parameter, LAI for half-maximal intake.
	double qLAIan;// = 3.0;		//dimensionless. Animal intake sigmoidicity parameter. parameter of the intake : LAI response.
	double fNplX;// = 0.02; // kg N (kg structure)-1.  Fractional nitrogen content of plant structural dry matter.
	double rmmCS;// = 28.5;  //Dimensionless. Relative molecular masses of C substrate (sucrose / 12 = 342 / 12 = 28.5) and of N substrate (nitrate = NO3 = 14 + 3 * 16 = 62).
	//Also, with units of kg (kg mole)-1,  these are the molar masses of C, N substrates.
	double rmmNS;// = 62.0;  // Dimensionless. Relative molecular masses of C substrate (sucrose / 12 = 342 / 12 = 28.5) and of N substrate (nitrate = NO3 = 14 + 3 * 16 = 62).
	//   Also, with units of kg (kg mole)-1,
	double cLAIan4[4];// = {1.0,0.75,0.5,0.25};
	double cssan4[4];// = {1.0,1.0,1.0,1.0};
	double cssan;// = 0.0;	//!css_an is a weighting constant, = 0 for sheep who only eat lamina.
         //!if css_an = 1, then the animals eat lamina, and sheath + stem, 
         //!according to their availability.
	double cSAI;// = 4 !m2 (kg XDM)-1.
//           !This is a very simple assumption. A more complicated assumption
//           !using a constant height:diameter ratio would be
//           !SAI = 0.8 * MXss ** 0.6667
//           !If sheath and stem height (h):diameter (d) ratio = hss / dss
//           ! = rhdss, sheath and stem density = ross, therefore
//           !Area = Ass = pi * dss * hss = pi * rhdss * dss**2, and
//           !Volume Vss = (pi/4) * dss**2 * hss = (pi/4) * rhdss * dss**3
//           ! = MXss / ross.  Therefore
//           !dss = [4 * MXss / ( pi * ross * rhdss)] ** 0.3333, and
//           !Ass = (pi * rhdss)**0.3333  * (4 MXss / ross) ** 0.6667
//           !With pi = 3.14, rhdss = 100, ross = 100 kg stem structural DM
//           !m-3, this gives
//           !Ass = (3.14 * 100)**0.3333 * (4/100) ** 0.6667 * MXbr ** 0.6667
//           ! = 0.8 * MXss ** 0.6667
//           !Even more complicated would be to allow morphology to vary, as
//           !in the forest model.
//           !If MXss = 0.1 kg XDM m-2, SAI = 0.4 m2 m-2.  This gives
//           !an SAI of about 10 to 15% of LAI, depending on grazing.
}STRUCT_ANIMAL;


typedef struct {
	STRUCT_PHOTOSYNTHESIS photosynthesis;
	STRUCT_TEMPERATURE temperature;
	STRUCT_WATER water;
	STRUCT_NITROGEN nitrogen;
	STRUCT_PLANT plant;
	STRUCT_ANIMAL animal;
} STRUCT_PARAM;

typedef struct { // HPM_Control
	// Steurung:
	double diurnal; // 0 oder 1 ( wenn 1, dann wird werden z.B. Sonneneinstrahlung über den Tag variiert
	double light_on; // wird automatisch durch die Sonneneinstrahlung gesteuert (nur wichtig, wenn diurnal = 1.0)
	double PAR_sin; // Falls diurnal==1 -> falls PAR_SIN==1, dann wird die Energieeinstrahlung für die Photosynthese sinusförmig angenommen, bei 0 wird eine Stepfunktion verwendet
	double water; // 1.0 Water Model == on
	//double sim_time;
	//Time Zone:
	double Time_Zone;
	double Lz;
	} T_Control;
	
typedef struct { // HPM_Plant

	// Vars:

	double MCSrt;  // Substrate C Masse [kg Substate C / (m^2)]
	double MNSrt;  // Substrate N Masse
	double MNSsh; // Subtrate N Masse shoot
	double MCSsh;
	double DMCSsh;
	double DMNSsh;
	double DMCSrt;
	double DMNSrt;
	//double Msh;  // total dry mass of sh --> fStemWeight (Expert N)
	double gsmax;


	double MLam4[4]; // Strukturelle Masse der Blätter, je schicht [kg struct DM / m^2]
	double MXLam; // Strukturelle gesmatmasse der Blätter
	double DMLam4[4];

	double Mss4[4]; // Structurelle Masse des sheath and stem [kg struct DM / m^2]
	double MXss; // Strukturelle Gesamt-Masse des sheath and stem
	double DMss4[4];

	double Mrt4[4]; // Structurelle Masse der root[kg struct DM / m^2]
	double MXrt;// Strukturelle Gesamt-Masse root
	double DMrt4[4];

	double LAI4[4]; // Leaf Area Index m^2/(m^2 ground)
	double DLAI4[4];

	double MNph_ul; //  kg photosyntetic N m-2 Photosyntehtc N in upper leaves of shoot
	double DMNph_ul;

	double MCTrt,MCTsh; // lg storage C m-2 C storage in root, shoot lamina
	double DMCTrt,DMCTsh;
	
	double rort;

} T_Plant;

typedef struct { // HPM_Environment
	// Atmosphäreneigenschaften
	double Tair; // Temperatur

	double Pair; // Atmosphärendruck [Pa]
	double DayLength; // [hour] Tageslänge
	double CO2vpm;  // in vpm: seems to be const. in Expert N --> no variable found
	/* double wind; // Windspeed [m/s]
	  double href; // reference height in which the wind speed is messured [m]*/

} T_Environment;


typedef struct {
	// Variablen, die im Pflanzenmodell gebraucht werden
	double aWrt; // Water activity (Wurzel)
	double aWsh; // Water activity (shoot)
	double theta_sh; // shoot water content
	//double theta_so; // soil water content
	double theta_rt; // root water content

	// Variablen, nur fürs Wassermodell
	double Wrt; //kg water m-2 ground.  Mass of water in the root.
	double Wsh; //kg water m-2 ground.  Mass of water in the shoot.
	//double Wso; //kg water m-2 ground.  Mass of water in the soil.
	double Wrt0;

	double RH;
	double ICSsh_ph;

} T_Water;

typedef struct {
	// Input und Output:
	double OXLam_an4[4]; // Blätter, die gefressen werden -> Momentan nicht implementiert --> 0
	double OXss_an4[4]; // Stiel und Stamm, die gefressen werden -> Momentan nicht implementiert --> 0
	double OLAI_an4[4]; // Leaf Area Index -> animal
	double OCSsh_an_gr; // Substrate C Anteil -> animal --> 0
	double ONSsh_an_gr; // shoot ouput N an animal
	double OCTsh_an;
	double IDMpl_an_gnd; // kg DM m-2 day-1. Daily intake rate from grazing.
	//double OSAI_an;  // !Sheath + stem area flux associated with flux OXss_an is
	//double OshAI_an; // output of green area (shoot area index) to grazing animals:
	//double OfshAI_an; //!OfshAI_an  output fraction of shoot area index:
	double n;
	double active; // 1.0 or 0.0
	double MCanret;
	double MNanret;
	double ONph_an;
} T_Animal;

typedef struct SHarvest_date *PSHARVEST_DATE;
typedef struct SHarvest_date{
	STRUCT_XPN_DATE date;
	double hharv;// = 0.07;  // m.  Sward is cut to a height of 3 cm.
	// tharv(i).  Times of harvest, stored in the real array, tharv (30)
	// are defined in the Initial Section, with tharv(1) = 66 (8 Mar),
	// tharv(2) = 73, tharv(3) = 80, ... harv(30) = 269.
	// s_harvi(i)   !Each of these weekly harvests can be switched on using
	//harvi(i), i = 1, 30, by setting s_harvi(i) = 1;
	// default s_harvi(i) = 0.  These are also defined in the Initial
	//Section.  !It is assumed that harvesting takes 1 day, i.e. the
	//material removed at harvest is removed over the course of 1 day.
	PSHARVEST_DATE pNext;
	PSHARVEST_DATE pBack;
} SHARVEST_DATE;

typedef struct {
	double OXLam_hv4[4]; // Blätter, die geärntet werden -> Momentran nicht implementiert --> 0
	double OXss_hv4[4];// Stiel und Stamm, die geerntet werden -> Momentan nicht implementiert --> 0
	double OLAI_hv4[4]; // Leaf Area Index -> harvest
	double OCSsh_hv;   // Substrate C Anteil -> harvest --> 0
	double ONSsh_hv;  // Shoot output N an harvest
	double ONph_hv;  // Photosynthesisch aktiver Stickstoff
	double OWsh_hv;

	double Sum_Lam_hv4[4];
	double Sum_ss_hv4[4];
	double Sum_CSsh_hv;
	double Sum_NSsh_hv;

	double fharv;  // fraction of the shoot material removed in a harvest
	double Pulseharv; // Pulsfunktion, wenn geerntet wird
	double active;    // 1.0 or 0.0
	
	
	PSHARVEST_DATE harvest_dates; // Harvest Dates
	PSHARVEST_DATE harvest_act_date; // act date of harvest
	
	STRUCT_XPN_DATE save_date; // intern harvest var
	
} T_Harvest;

typedef struct { // Assimlilation
	double OCSrt_so, ONSrt_so;
} T_Soil;

typedef struct { // Assimlilation
	//double OCsh_so,ONsh_so, OCrt_so,ONrt_so; // output of total Substrate to litter
	double OCSsh_so,ONSsh_so, OCSrt_so,ONSrt_so; // output C,N Substrate to litter
	double INSrt_so;
	//double OCXsh_so, ONXsh_so, OCXrt_so,ONXrt_so; // output C, N structural Mass to litter
	double OXsh_li, OXrt_li; // output structural Mass to litter

	double INammrt_so,INitrt_so; //
	double ONammrt_so,ONitrt_so; //
	double Nammrt_so,Nitrt_so;
	double ONrt_li; // Gesamtoutput N in Litter
	double ONsh_li; // Gesamtoutput N in Litter
	double OCsh_li;
	double OCrt_li;
	
	double RONrt_li; // Gesamtoutput N in Litter
	double RONsh_li; // Gesamtoutput N in Litter
	double ROCsh_li;
	double ROCrt_li;
	// unterstrukturen werden nur zur übergabe benutzt:
	double OXlam_deg,OXlam_li;
	double OXss_deg,OXss_li;
	double OXrt_deg;
	double MXli,MXrt,MXsh;
} T_Litter;

typedef struct { // Assimilation
	double RGsh,RGrt;
	double OCsrt_uN; // Teil der an Energie für den N uptake verbraucht wird
	double Rmrt,Rmsh; // Maintance Respiration [kg C /(m^2 day)]
	double R_Ph; // Photosynthese [kg C (m^2 day)]
} T_Respiration;	


typedef struct{
	double MPl;
	double Mrt;
	double Msh;
	double MSrt;
	double MXsh;
	double MSsh;
	double MXPl;
	double MSPl;
	
	double cum_harvest_mass;
	
	double Nnit;
	double Namm;
	double theta_soil;
	
	double Grt,Gsh;
	double fTsh,fWsh,Csh,Nsh;
	double fTrt,fWrt,Crt,Nrt;
	double psi_sh,psi_rt;
	
	double LAI,hcan;
} T_Output;



struct _hpm
{
	expertn_modul_base parent;
	/* add your public declarations here */
	
	T_Control Control;
	T_Plant Plant;
	T_Respiration Respiration;
	T_Litter Litter;
	T_Water Water;
	T_Harvest Harvest;
	T_Animal Animal;
	T_Environment Environment;
	
	T_Output Output;
	
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
	
	int __INIT_DONE;
	int __DONE_DONE;
	int __ERROR;
	
	#ifdef TEST_HPM	
	STRUCT_XNO_DATA* test_data;
	int test_count;	
	#endif
	
	char *ini_filename;
	
	int __CREATE_AUTO_INI_FILES;
	
	gboolean __USE_STATIC_ROOT_LENGTH; // TRUE or FALSE
	int __STATIC_ROOT_LENGTH; // Number of Layers
	

STRUCT_PARAM parameter;

};


// public class member function:


G_MODULE_EXPORT int hpm_canopy_gross_photosynthesis(hpm *self);
G_MODULE_EXPORT int hpm_biomass_growth(hpm *self);
G_MODULE_EXPORT int hpm_root_length_growth(hpm *self);

G_MODULE_EXPORT int hpm_water_uptake_hpm(hpm *self);
G_MODULE_EXPORT int hpm_water_uptake_feddes(hpm *self);

G_MODULE_EXPORT int hpm_nitrogen_uptake(hpm *self);
G_MODULE_EXPORT int hpm_actual_evapotranspiration(hpm *self);

G_MODULE_EXPORT int hpm_harvest_init(hpm *self);
G_MODULE_EXPORT int hpm_harvest_run(hpm *self);
G_MODULE_EXPORT int hpm_harvest_done(hpm *self);

G_MODULE_EXPORT int hpm_animal_init(hpm *self);
G_MODULE_EXPORT int hpm_animal_run(hpm *self);
G_MODULE_EXPORT int hpm_animal_done(hpm *self);

G_MODULE_EXPORT int hpm_calc_albedo(hpm *self);



G_MODULE_EXPORT int load_parameter(hpm *self,const char* paramfile);
G_MODULE_EXPORT int load_config(hpm *self,const char* configfile);

void get_SoilVars(hpm *self,struct_Soil_Vars *vars);
void create_output(hpm *self);

struct _hpmClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType hpm_get_type (void);


#define C_DEBUG(var)
//	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i) 
	/*{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}\
*/
// 1 / m2 --> 1/ha
#define calc_1m2_to_ha 10000

#ifdef TEST_HPM
int hpm_read_csv_file(hpm *self,char *weather_file_name_st,int min_columns);
double get_var(hpm *self,char *name);
#endif

G_END_DECLS

#endif /* __HPM_H__ */

