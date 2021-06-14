#ifndef CONFIG_H_
#define CONFIG_H_

struct config 
{
	double	delT; 
	double	delZ;
	double	dev; 
	char	*solver;
	int		pot_tra;
	int		iterbreak;
	int		HydraulicLift;
	char	*RootDistribution; //not used obviously
	char    *xylemfractionCalculation; //xylem to total stem area calculation: a: aranda 2005, c: constant
	int		order;
	double	internal_timestep_ini;
	int 	beep;
	double	contemp;
	int		maxStrahler; //max. Strahler order to get leaves
 	double	CtoK;						//CONVERSION			[°C -> K]
	double	GtoPAR;						//CONVERSION			[Wm-2]
	double	WtoUMOL;					//CONVERSION			[Wm-2 -> µmol m-2]
	double	cCO2;						//PARAMETER				[mol mol-1]  CO2 concentration 
	double	R;							//PARAMETER				[J mol-1 K-1]	universal gas constant 
	double 	ci;							//PLANT					intercellular CO2 concenttration fraction
	double	Tref;						//PHOTOSYNTHESIS		reference temperature [K]								//	Tuzet et al. 2003 and Leuning 1995
	double  J_ref_max;  				//PHOTOSYNTHESIS 		potential rate of electron transport [mol m-2 s-1] 		//	62 57 48 45 //	Platho beech.gtp, see also Leuning 1997, Tauzat et al. 2003 
	double	V_ref_lmax; 				//PHOTOSYNTHESIS		max phototsynthetic capacity at Tref [mol CO2 m-2 s-1] 	//	Tuzet et al. 2003 
	double	oi;							//PHOTOSYNTHESIS 		intercellular oxygen concentration [mol mol-1]			//	Tuzet et al. 2003 
	double	Ko_ref;						//PHOTOSYNTHESIS		Michaelis coefficient for O2 [mol mol-1]				//	Tuzet et al. 2003 and Leuning 1995 
	double	Kc_ref;						//PHOTOSYNTHESIS		Michaelis coefficient for CO2 [mol mol-1]				//	Tuzet et al. 2003 and Leuning 1995 
	double	Eo;							//PHOTOSYNTHESIS		activation energy for Ko [J mol-1]						//	Tuzet et al. 2003 and Leuning 1995 
	double	Ec;							//PHOTOSYNTHESIS		activation energy for Kc [J mol-1]						//	Tuzet et al. 2003 and Leuning 1995 
	double	EaJ;						//PHOTOSYNTHESIS		activation energy for Jmax [J mol-1]					//	Tuzet et al. 2003 (37.0e3) Leuning 1995 (79.5e3)
	double	EaV;						//PHOTOSYNTHESIS		activation energy for Vcmax [J mol-1]					//	Tuzet et al. 2003 (58520.0) Leuning 1995 (116.3e3)
	double	EdJ;						//PHOTOSYNTHESIS		deactivation energy for Jmax [J mol-1]					//	Tuzet et al. 2003 (220.0e3) Leuning 1995 (201.0e3)
	double	EdV;						//PHOTOSYNTHESIS		deactivation energy for Vcmax [J mol-1]					//	Tuzet et al. 2003 (220.0e3) Leuning 1995 (202.9e3)
	double	S;							//PHOTOSYNTHESIS		Entropy term for Jmax and Vcmax [J mol-1 K-1]			//	Tuzet et al. 2003 (700.0) Leuning 1995 (650) 
	double	alpha;						//PHOTOSYNTHESIS		quantum yield [-]										//	Tuzet et al. 2003 and Leuning 1995
	double	Theta;						//PHOTOSYNTHESIS		parameter of the hyperbola								//	Tuzet et al. 2003 (0.9) Leuning 1995 (0.95)
	double	gamma0;						//PHOTOSYNTHESIS		parameter in Gamma* [mol mol-1]							//	Tuzet et al. 2003 
	double	gamma1;						//PHOTOSYNTHESIS		parameter in Gamma*										//	Tuzet et al. 2003 
	double	gamma2;						//PHOTOSYNTHESIS		parameter in Gamma*										//	Tuzet et al. 2003 
	double	Rd_f;						//RESPIRATION			scaling constant										// 	Falge et al. 1996
	double	Ea_Rd;						//RESPIRATION			activation energy of Rd [J mol-1]						//  Leuning 1995
	double	Rd_T20;						//RESPIRATION			day respiration at T0 									// 	Leuning 1995   
};


#endif //CONFIG_H_