
#include "hpm_photosynthesis.h"
#include "hpm_math.h"
#include "hpm_water.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int hpm_canopy_gross_photosynthesis(hpm *self)
{
	double CO2air;
	double alpha;
	double Pmax;
	double z1CE,z2CE;
	double PCO2sc;
	double par; //[J (PAR) m^-2 s^-1]
	double PCO2dy1,PCO2dy ;
	//double Time;
	expertn_modul_base *xpn = &(self->parent);

	//Time = xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay;

	Pmax = calulateMaxGrossphotosynthesisRate_HPM(self);
	
	// "Wirkliche" CO2 concentration (7.3a):
	CO2air = self->Environment.CO2vpm*1.0e-6 * (273.15 / ((double)self->Environment.Tair + 273.15)) * (self->Environment.Pair * (1.9636/101.325));
	alpha = calculateCanopyPhotosynthesis_getAlpha(self,(double)self->Environment.Tair, CO2air, self->Water.aWsh);

	/* assuming Pmax varies down the canopy according to
	   exp(-kcan*LAI), suggested by Charles-Edwards (CE).
	   Pmax is the value at the top of the canopy.*/

	// par ist etwa 50 % von fSolRad
	par = xpn->pCl->pWeather->fPAR*1.0e6 / 86400.0;//CalculateActiveRadiation(self,(double)Time,self->Environment.DayLength, (double)xpn->pCl->pWeather->fSolRad * 1.0e6);
	
    

	//xpn->pCl->pWeather->fPAR = (float)(par * 1.0e-6); //--------------------------------------------------------------------------------------- STIMMT DIE EINHEIT?

	z1CE = alpha * self->parameter.photosynthesis.kcan * par / ( 1.0 - self->parameter.photosynthesis.XIleaf );
	z2CE = z1CE + Pmax;
	// Teil der Gleichung 3.2r
	PCO2sc = ( ( 1.0 - exp( -self->parameter.photosynthesis.kcan * (hpm_math_sum4(self->Plant.LAI4))) ) / ( 2.0 * self->parameter.photosynthesis.kcan * self->parameter.photosynthesis.ksi_ph) ) *
	         (z2CE - sqrt( z2CE * z2CE - 4.0 * self->parameter.photosynthesis.ksi_ph * z1CE * Pmax ));//[kg CO2 m^-2 s-1]

	
	PCO2dy1 = 86400.0 * PCO2sc; // 86400 s = 60*60*24s (= 1 day)
	PCO2dy = PCO2dy1 * ( self->Control.diurnal  + ( 1.0 - self->Control.diurnal ) * (self->Environment.DayLength / 24.0 )); //[kg CO2 m^-2 day-1]
	xpn->pPl->pPltCarbon->fGrossPhotosynR =  	(PCO2dy * mq_TO_ha_1);	// [kg CO2 m^-2 day-1] --> [kgCO2/ha.day]
		
	C_DEBUG(Pmax);
	C_DEBUG(par);
	C_DEBUG(z1CE);
	C_DEBUG(z2CE);
	C_DEBUG(PCO2sc);
	C_DEBUG(PCO2dy1);
	C_DEBUG(PCO2dy);
	return self->__ERROR;	
}


/*
 * Berechnet die Photosynthetische aktive Strahlung [J (PAR) m^-2 s^-1]
 *
in:
 * DayTime: Kann entweder die Tageszeit in [days] sein oder auch Simulationszeit (mit Datum, Zeit) in [days]: --> daraus wird Tageszeit berechnet
 * SunDuration: Sonnenscheindauer [h]
 * SolRad: Globalstrahlung [J / (m^2 * day)]
 *
 Steuerung:
 * diurnal: bewertet die Strahlung entweder mit einer Sinus funktion oder einer Schrittfunktion
 * PAR_sin: step funktion oder Sinusfunktion (==1.0)
 * light_on: Tag oder Nacht (nur fall diurnal = 1.0); wird automatisch geregelt
 *
ret:
 * Par: Photosynthetisch aktive Strahlung [ J (PAR) m^2-2 s^-1]
*/
double CalculateActiveRadiation(hpm *self,double DayTime,double SunDuration, double SolRad ) { // 7.5a
#define twopi (M_PI * 2.0)
	double par; //return value
	double y,daylsc;
	double tofday;
	double dayldy;
	double intpart;

	par = 0.0;
	daylsc = 3600.0 * SunDuration;// Daylength (secs day-1).
	if (daylsc==0.0) {
		par=0.0;

	} else {


		y = SolRad / daylsc;
		//_M("y",y);



		if (self->Control.diurnal==0.0) {
			par=y;
		} else if (self->Control.diurnal==1.0) {
			if  (self->Control.PAR_sin==1.0) {
				tofday = modf(DayTime,&intpart);
				dayldy  =  SunDuration / 24.0;
				par= self->Control.light_on * y *(1.0+cos(twopi * (tofday - 0.5)/ dayldy));

			} else {
				par= self->Control.light_on * y;

			}
		}
	}


	return par;
}

/*
Function calculates the photosynthese rate (Pmax 3.2t) which depends on:
- ambient CO2 concentration
- water status
- relative stomatal opening
- tisue N concentration

in:
 * Environment.Tair
 * (Plant.TmaxPmax700, Plant.TmaxPmax350, Plant.CO2vpm)
 * Plant.CO2vpm
 * Plant.Pair
out:
 * Plant.aWsh
ret:
*/
double calulateMaxGrossphotosynthesisRate_HPM(hpm *self) { // 3.2t
	double Pmax; // return var
	double fTPmax; // temperatur funktion (3.11c)
	double fCO2_Pmax;
	double fWph; // Water status function: Qaudrat Water activity
	double CO2air; // CO2 konzetration in kg CO2 m-3
//	double Xi_sh; // Water Potential shoot J / (kg Water) --> to do
	double gstom; //[m s-1 (m2 leaf)-1] somatal conductance
	double fgstom_Pmax;
	double NshlamA; // kg N (m2 lamina)-1 total N content of lamina area
	double Ntotsh; // tot substrate concentration
	double SLA; // secific leaf area  [m2 / (lamina total DM)]
	double fN_Pmax;
	double Msh; // Gesamtmasse von shoot in kg / m^2
	double MSLam; // Strukturelle + Substrate Masse;
	double MCSLam; // Substrate C gehalt des Blattes
	double MNSLam; // Substrate N gehalt des Blattes
	double MNphlam;
	double MSsh,MXsh;
	double Pmax20,Nph_ul_A;
	


	double cPmax_Nph = 0.0015; //  !kg CO2 s-1 (kg photosynthetic N)-1.
	expertn_modul_base *xpn = &(self->parent);
	
	
	MXsh = self->Plant.MXss + self->Plant.MXLam;
	MSsh = self->Plant.MCSsh * self->parameter.photosynthesis.rmmCS/12.0 + self->Plant.MNSsh * self->parameter.photosynthesis.rmmNS/14.0 + self->Plant.MNph_ul * 100.0 / 14.0;
	Msh = MXsh + MSsh;
	// Photsynthesefunktion:
	fTPmax = calculatePlantTemperatureProcesses_getfTPMax_HPM(self,(double)self->Environment.Tair);

	// "Wirkliche" CO2 concentration (7.3a):
	CO2air = self->Environment.CO2vpm*1.0e-6 * (273.15 / ((double)self->Environment.Tair + 273.15)) * (self->Environment.Pair * (1.9636/101.325));

	// CO2 abhängigkeit:
	//fCO2_Pmax = 1.0/(1.0+parameter.photosynthesis.KCO2_Pmax/ CO2air);
	fCO2_Pmax = 1.0/(1.0+700.0/ self->Environment.CO2vpm);

	// Water acitivität:
	//Xi_sh = 1.0;
	//Water.aWsh = exp(mu_W * Xi_sh / (Rgas* ((double)Environment.Tair ) + 273.15)); // 6.7a

	fWph = QUAD(self->Water.aWsh); // 3.2s --> Wenn Wasser stress --> wird niedriger --> Pmax sinkt

	gstom = calculateLeafStomatalConductance(self,self->Water.theta_sh);
	
	xpn->pPl->pCanopy->fsto_res = 1.0/gstom;

	// Canopy conductance, gcanco  !m s-1 (m2 ground)-1.
	//*p->gcanco = *p->LAI * *p->gstom; // --> Wasser Model


	fgstom_Pmax = 1.0 - self->Control.water * ( 1.0 - gstom / self->Plant.gsmax );

	//Leaf area index / total structal dry mass
	MCSLam = self->Plant.MXLam / (self->Plant.MXLam+self->Plant.MXss) * self->Plant.MCSsh; // 3.1d
	MNSLam = self->Plant.MXLam / (self->Plant.MXLam+self->Plant.MXss) * self->Plant.MNSsh; // 3.1d
	MNphlam = self->Plant.MXLam / (self->Plant.MXLam+self->Plant.MXss) * self->Plant.MNph_ul;
	MSLam = MCSLam * self->parameter.photosynthesis.rmmCS / 12.0 + MNSLam * self->parameter.photosynthesis.rmmNS / 14.0 + MNphlam * 100.0 / 14.0; // Gl. 3.1e
	SLA = (double)xpn->pPl->pCanopy->fLAI / (self->Plant.MXLam + MSLam); // Equ: 3.1g



	// (double)pPl->pBiomass->fTotLeafWeight * 1.0e-4

	
	CHECK_0(Msh)
	Ntotsh = ( self->Plant.MNSsh + (self->parameter.plant.fNplX * (self->Plant.MXLam+self->Plant.MXss) )+self->Plant.MNph_ul ) / Msh; // 3.li

	// total N content by the spec. leaf area
	

	CHECK_0(SLA)
	NshlamA = Ntotsh / SLA; //3.1j


	// 3.2t fN_Pmax: ramp function
	fN_Pmax = 1.0;
	if (NshlamA <= 0.001) {
		fN_Pmax = 0.5;
	} else if ((NshlamA>0.001)&&(NshlamA<0.002)) {
		fN_Pmax = 0.5 * ( 1.0 + ( NshlamA- 0.001 ) / 0.001 );
	} else if (NshlamA>=0.002) {
		fN_Pmax = 1.0;
	} else {
		PRINT_ERROR("Error: calculating NshlamA");
		fN_Pmax = 0.5;
	}

	
	// Man kann Pmax20 folgendermaßen setzten:
	//Pmax20 = parameter.photosynthesis.Pmax20;
	// oder mit Photosynthetischen Stickstoff:
	Nph_ul_A = self->Plant.MNph_ul / hpm_math_sum4(self->Plant.LAI4);
	Pmax20 = cPmax_Nph * Nph_ul_A;

	C_DEBUG(self->Control.light_on);
	C_DEBUG(self->Water.theta_sh)
	C_DEBUG(gstom)
	C_DEBUG(Pmax20)
	C_DEBUG(fCO2_Pmax)
	C_DEBUG(fTPmax)
	C_DEBUG(fWph)
	C_DEBUG(fgstom_Pmax)
	
	Pmax = Pmax20 * fCO2_Pmax * fTPmax * fWph * fgstom_Pmax;// * fN_Pmax;

	return Pmax;

}

double calculateCanopyPhotosynthesis_getAlpha(hpm *self,double Tair, double CO2air, double aWsh) {
	double fTa;
	double alpha;
	double fWph;
	double fCO2alpha;
	double cCO2_alpha = 100.0;

	if (Tair <= 15.0) {
		fTa = 1.0;
	} else {
		fTa = 1.0 - self->parameter.photosynthesis.cTalpha*( 350.0 / self->Environment.CO2vpm )*(Tair-15.0);
	}

	fWph = QUAD(aWsh); // fwph = (aWsh)^qWph with qWph = 2.0
	//fCO2alpha = 1.0 - parameter.photosynthesis.beta/(parameter.photosynthesis.tau* CO2air);
	fCO2alpha = 1.0 - cCO2_alpha / self->Environment.CO2vpm;
	alpha = self->parameter.photosynthesis.alpham15*fCO2alpha*fTa*fWph;

	return alpha;

}

/*
Berechnet Temperatur funktion (maximum) ist von der CO2 concentration der Luft abhängig
in:
 * Plant.TmaxPmax700
 * Plant.TmaxPmax350
 * Plant.CO2vpm
out:
 * pPl->pGenotype->fTempOptPs // Optimale Photsynthesetemperatur
 * pPl->pGenotype->fTempMaxPs // Maximale Photosynthestemp
 * pPl->pGenotype->fTempMinPs // Minimale Photosynthestemp = 0.0
ret:
 * fTMax // Temperature function
*/

double calculatePlantTemperatureProcesses_getfTPMax_HPM(hpm *self, double T) { // --> 3.11c
	double fTPmax; // return var
	expertn_modul_base *xpn = &(self->parent);

	
	xpn->pPl->pGenotype->fTempOptPs = (self->parameter.temperature.TmaxPmax350+((self->parameter.temperature.TmaxPmax700-self->parameter.temperature.TmaxPmax350)/350.0)* self->Environment.CO2vpm-(self->parameter.temperature.TmaxPmax700-self->parameter.temperature.TmaxPmax350));

	// Maximale photosynth. temp:
	//pPl->pGenotype->fTempMaxPs = (float)(((1.0+parameter.temperature.qfT)* (double)pPl->pGenotype->fTempOptPs - parameter.temperature.T0p)/parameter.temperature.qfT);
	xpn->pPl->pGenotype->fTempMaxPs = (((1.0+self->parameter.temperature.qfT)* (double)xpn->pPl->pGenotype->fTempOptPs - self->parameter.temperature.T0)/self->parameter.temperature.qfT);
	xpn->pPl->pGenotype->fTempMinPs = 0.0;

	// Photosynthese, falls richtige Temperatur
	if ((T > (double)xpn->pPl->pGenotype->fTempMinPs) && (T < (double)xpn->pPl->pGenotype->fTempMaxPs)) { // T0 < T < T0'max
		double zfT; // Nenner
		zfT = 1.0 / (pow(self->parameter.temperature.Tref - (double)xpn->pPl->pGenotype->fTempMinPs, self->parameter.temperature.qfT ) * ( (double)xpn->pPl->pGenotype->fTempMaxPs - self->parameter.temperature.Tref ));
		fTPmax = self->parameter.temperature.mfT*pow(T-(double)xpn->pPl->pGenotype->fTempMinPs,self->parameter.temperature.qfT)*((double)xpn->pPl->pGenotype->fTempMaxPs-T)*zfT;
	} else {
		fTPmax = 1.0e-10; // in documentation: 0.0; Why?
	}
	
	return fTPmax;
}
