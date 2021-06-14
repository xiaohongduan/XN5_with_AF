#include "xylemwaterflow.h"

///////////////////////////////////////////////// Phtotsynthesis // Leuning 1995 following Farquhar et al. 1980
double XWF_CLASS::getA(  double AirTemp,	// temperature [K]
			  double CO2,		// CO2 content of the air [mol CO2 / mol air]
			  double PAR		// photon irridiance [mol m-2 s-1]
			)
{
	// Photosynthesis rate limited by Rubisco activity
	double Vc = this->getVc(AirTemp, CO2);
	// Rate of Ribulose Bisphosphate regeneration through electron transport
	double Vj = this->getVj(AirTemp,CO2,PAR);
	// net biochemical demand for CO2
	return min(Vc,Vj);//[mol CO2 m-2 s-1]
}
/////////////////////////////////////////////////

///////////////////////////////////////////////////
// Rate of Ribulose Bisphosphate regeneration through electron transport
double XWF_CLASS::getVj( double AirTemp,// temperature [K]
			  double CO2,// CO2 content of the air[mol CO2 / mol air] 
			  double PAR // photon irridiance [mol m-1 s-1]
			  )
{
	double Vj;
	//intercellular CO2 content of the air, value from PLATHO beech.gtp
	double ci = CO2 * 0.7;
	// Michaelis constant of CO2 
	double GAMMAstar = this->getGAMMAstar(AirTemp);
	// electron transport rate
	Vj= getJ(PAR,AirTemp)*0.25* (ci-GAMMAstar)/(ci+2.0*GAMMAstar);// Leuning 1995 eq. A2
	return Vj; //[mol CO2 m-2 s-1]
}
///////////////////////////////////////////////////
// Photosynthesis rate limited by Rubisco activity
double XWF_CLASS::getVc (	double AirTemp,//[K]
				double CO2 //[mol CO2 / mol air], CO2 content of the air
				)
{
	double Vc;
	// maxima catalytic activity of Rubisco
	double Vlmax = this->getVlmax(AirTemp);
	// CO2 compensation point in absence of day respiration
	double GAMMAstar = this->getGAMMAstar(AirTemp);
	// Michaelis constants
	double Kc = this->getKc(AirTemp);
	double Ko = this->getKo(AirTemp);
	// intercellular CO2 content of the air, value from PLATHO beech.gtp
	double ci = CO2 * this->conf.ci; 

	Vc = Vlmax * (ci-GAMMAstar)							
		/ ( ci + Kc*(1.0+this->conf.oi/Ko) );// Leuning 1995 eq. A1
	return Vc; //[mol CO2 m-2 s-1]
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////	Subfunctions
////////////////////////////////////////////////////////

//// maxima catalytic activity of Rubisco
//const double V_ref_lmax 	= 100e-6;		//max phototsynthetic capacity at Tref [mol CO2 m-2 s-1] 
//const double EaV			= 58520.0;		//activation energy for Vcmax [J mol-1]
//const double R			= 8.3144621; 	//universal gas constant [J mol-1 K-1]
//const double Tref			= 293.2;		//reference Temperature [K]
//const double S			= 700.0;		//Entropy term for Jmax and Vcmax [J mol-1 K-1]
//const double EdV			= 220000.0; 	//deactivation energy for Vcmax [J mol-1]
double XWF_CLASS::getVlmax ( double  AirTemp) //K, [mol CO2 m-2 s-1]
{
	double Vlmax =	 this->conf.V_ref_lmax * exp ( (this->conf.EaV/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref/AirTemp) ) 
					/(1.0 + exp((this->conf.S*AirTemp-this->conf.EdV)/(this->conf.R*AirTemp))); 
	//double Vlmax =	 this->conf.V_ref_lmax * exp ( (this->conf.EaV/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref/AirTemp) ) 
	//				/sqrt(this->conf.Tref/AirTemp) ; 
	return Vlmax; 
}

//// CO2 compensation point in absence of day respiration
//const double gamma0		= 28.0e-6;	//parameter in Gamma* [mol mol-1]
//const double gamma1		= 0.0509;	//parameter in Gamma*
//const double gamma2		= 0.001;	//parameter in Gamma*
//const double Tref			= 293.2;		//reference Temperature [K]
double XWF_CLASS::getGAMMAstar ( double AirTemp )
{
	double GAMMAstar = this->conf.gamma0 * ( 1.0 + this->conf.gamma1*(AirTemp-this->conf.Tref) + this->conf.gamma2 * pow(AirTemp-this->conf.Tref, 2.0));
	return GAMMAstar; //[mol mol-1]
}

//// Michaelis constant of CO2
//const double Kc_ref		= 302.0e-6; 	//Michaelis coefficient for CO2 [mol mol-1]
//const double Ec			= 59430.0;		//activation energy for Kc [J mol-1]
//const double R			= 8.3144621; 	//universal gas constant [J mol-1 K-1]
//const double Tref			= 293.2;		//reference Temperature [K]
double XWF_CLASS::getKc ( double AirTemp )
{
	double Kc = this->conf.Kc_ref * exp(((this->conf.Ec)/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref/AirTemp));
	return Kc; 
}

//// Michaelis constant of O2
//const double Ko_ref		= 256.0e-3; 	//Michaelis coefficient for O2 [mol mol-1]
//const double Eo			= 36000.0;		//activation energy for Ko [J mol-1]
//const double R			= 8.3144621; 	//universal gas constant [J mol-1 K-1]
//const double Tref			= 293.2;		//reference Temperature [K]
double XWF_CLASS::getKo ( double AirTemp )
{
	double Ko = this->conf.Ko_ref * exp(((this->conf.Eo)/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref/AirTemp));
	return Ko; 
}


//// potential rate of whole change elektron transport
//const double J_ref_max 	= 50e-6;  		//potential rate of electron transport [mol m-2 s-1] //62 57 48 45 //Platho beech.gtp, see also Leuning 1997, Tauzat et al. 2003 Appendix B
//const double R			= 8.3144621; 	//universal gas constant [J mol-1 K-1]
//const double S			= 700.0;		//Entropy term for Jmax and Vcmax [J mol-1 K-1]
//const double Tref			= 293.2;		//reference Temperature [K]
//const double EaJ			= 37000.0;		//activation energy for Jmax [J mol-1]
//const double EdJ			= 220000.0; 	//deactivation energy for Jmax [J mol-1]
double XWF_CLASS::getJmax ( double  AirTemp) //K
{
	//double z =  exp ( (this->conf.EaJ/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref /AirTemp) );
	//double n =  ( 1.0 + exp((this->conf.S*AirTemp-this->conf.EdJ)/(this->conf.R *AirTemp)) ); 
	double Jmax =	 this->conf.J_ref_max * exp ( (this->conf.EaJ/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref /AirTemp) ) 
					/( 1.0 + exp((this->conf.S *AirTemp -this->conf.EdJ)/(this->conf.R *AirTemp)) ); 
	//double Jmax =	 this->conf.J_ref_max * exp ( (this->conf.EaJ/(this->conf.R*this->conf.Tref))*(1.0-this->conf.Tref /AirTemp) ) 
	//				/sqrt(this->conf.Tref/AirTemp) ; 
	return Jmax; 
}


//// electron transport rate
//const double alpha		= 0.2;		//quantum yield [-]
//const double Theta		= 0.95;		//parameter of the hyperbola
double XWF_CLASS::getJ ( double PAR, double AirTemp)//input and return in [mol m-2 s-2], T[K]
{
	double Jmax = this->getJmax( AirTemp );
	double a = this->conf.Theta;
	double b = -(this->conf.alpha *PAR+ Jmax);
	double c = this->conf.alpha*PAR*Jmax;
	double D = b*b-4.0*a*c;
	if(D < 0.0)
		PRINT_ERROR_ID(xpn, "error getJ: determinant < 0");

	return (-b - sqrt(D)) / (2.0*a);
}


