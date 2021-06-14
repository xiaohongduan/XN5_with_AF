//      surface.c
//
//      Copyright 2014 Christian Klein <chrikle@berlios.de>
//

#include "evapotranspiration.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



void sfcdif1_loop(expertn_modul_base *xpn,int iter, double *moz, double *fm, double *fh, double *fv, double *mol,double *cm,double *ch, int *mozsign)
{
	//int iter=1; // function wird in einer for schleife iteriert
	//double moz; // input Monin-Obukhov stability (z/L)
	//double fm; // ψ_m((z-d0)/L)
	//double fh; // ψ_h((z-d0)/L)
	//double cm; drag coefficient for momentum
    //double ch;  drag coefficient for heat
	// double fv; // !friction velocity (m/s)
	// double mol; // !Monin-Obukhov length (m)
	//int mozsign; // number of times moz changes sign
	
	double vkc = 0.40; // von Karman constant
	double grav = 9.80616;   //acceleration due to gravity (m/s2)
	double rair   = 287.04;  //gas constant for dry air (j/kg/k)
	double cpair  = 1004.64; //heat capacity dry air at const pres (j/kg/k)
	double mpe = 1e-6; // prevents overflow error if division by zero
	
	double fhnew,fmnew; //
	double tvir; //virtual temperature (k)
	double sfctmp; // absolute Temperature [K]
	double ea,es;
	double h; // sensible heat flux (w/m2) [+ to atm]
	double rhoair;
	double zlvl; //reference height  (m)
	double zpd;//    !zero plane displacement (m)
	double zom; // roughness length, momentum, ground (m)
	double zoh;  //roughness length, sensible heat, ground (m)
	double mozold;
	
	if (iter == 1)
		{
			mozold = 0.0;
			*mozsign = 0;
		} else
		{
			mozold = *moz;
		}
	
	sfctmp = 273.16+xpn->pCl->pWeather->fTempAir;
	//Saturation Vapor Pressure (es)
	es = 0.6108*exp(17.27*xpn->pCl->pWeather->fTempAir/(xpn->pCl->pWeather->fTempAir+237.3)); //eqn. 37, [kPa/°C]

	//Actual Vapor Pressure (ea)
	//Method nr. 3 in table 4, eqn 37, 41, [kPa/°C]
	//using rel. humidity and temperature at the hour
	ea = xpn->pCl->pWeather->fHumidity/100.0 * es;
	
	tvir = sfctmp * 1.0/(1.0-0.378*ea/(xpn->pCl->pWeather->fAtmPressure*1.0e-3));
	
	h = -xpn->pHe->pHEBalance->fHeatSens;
	rhoair = (xpn->pCl->pWeather->fAtmPressure -0.378*ea*1e3) / (rair*sfctmp);
	
	zlvl = xpn->pCl->fWindMeasHeight;
	zpd = 0.67 * xpn->pPl->pCanopy->fPlantHeight;
	zom = 0.123 * xpn->pPl->pCanopy->fPlantHeight;
	zoh = 0.0123 * xpn->pPl->pCanopy->fPlantHeight;
	
	if (iter == 1)
		{
			*fv = 0.0;
			*moz = 0.0;
			*mol = 0.0;
		} else
		{
			double tmp1;
			tmp1 = vkc * (grav/tvir)*h / (rhoair*cpair);
			if (fabs(tmp1) < mpe) 
				{
					tmp1 = mpe;
				}
			// Kominiere die Gleichung  S.899 (2) und Gleichung S.900 (15)
			*mol = -1.0 * pow(*fv,3.0) / tmp1;
			*moz = min(((zlvl-zpd)/ *mol),1.0);
		}
		
	// accumulate number of times moz changes sign.

    
	
	if (mozold* (*moz) < 0.0) (*mozsign)++;
	//printf("%d moz: %f\n",(*mozsign),(*moz));
    if (*mozsign >= 2)
		{
			*moz = 0.0;
			*fm = 0.0;
			*fh = 0.0;
		}


	// evaluate stability-dependent variables using moz from prior iteration
	if (*moz < 0.0)  // free convection Ri_b < 0 (fouth one)
		{
			double tmp1,tmp2,tmp3;
			//S.900 (14) im Text
			tmp1 = pow(1.0 - 16.0* *moz,0.25);
			//S.900 (14) * 2
			tmp2 = log((1.0+tmp1*tmp1)/2.0);
			//S.900 (13) * Teil 1
			tmp3 = log((1.0+tmp1)/2.0);
			//S.900 (13) ganze Gleichung: ψ_m = ...
			fmnew = 2.0*tmp3 + tmp2 - 2.0*atan(tmp1) + M_PI/2.0;
			//S.900 (14): ψ_h = ...
			fhnew = 2.0*tmp2;
		}
	else
		{
			fmnew = -5.0* *moz; // ψ_m = ψ_h = -5.0 * z/l
			fhnew = fmnew;
		}
		
	// except for first iteration, weight stability factors for previous
	// iteration to help avoid flip-flops from one iteration to the next

    if (iter == 1)
		{
			*fm = fmnew;
			*fh = fhnew;       
		} else
		{
       
			*fm = 0.5 * (*fm+fmnew);
			*fh = 0.5 * (*fh+fhnew);
		}
		
		{
			
			double tmpcm; // temporary calculation for CM
			double tmpch; // temporary calculation for CH
			double cmfm;
			double chfh;
			
			tmpcm = log((zlvl-zpd) / zom);
			tmpch = log((zlvl-zpd) / zoh);
			cmfm = tmpcm - *fm;
			chfh = tmpch - *fh;
			if (fabs(cmfm) < mpe) 
				{
						cmfm = mpe;
				}
			if (fabs(chfh) < mpe) 
				{
						chfh = mpe;
				}
			*cm = vkc*vkc / (cmfm * cmfm);
			*ch = vkc*vkc / (cmfm * chfh);
		}
		
		// S.900 unter (15) u_star = k*U/[ln(z/z0)-ψ_m((z-d0)/L)]
		*fv = xpn->pCl->pWeather->fWindSpeed * sqrt(*cm);
}

double surface_layer_get_ch(expertn_modul_base *xpn)
{
	int niterc=20;
	int iter; 
	double moz; // input Monin-Obukhov stability (z/L)
	double fm; // ψ_m((z-d0)/L)
	double fh; // ψ_h((z-d0)/L)
	double fv; // !friction velocity (m/s)
	double mol; // !Monin-Obukhov length (m)
	double cm; // drag coefficient for momentum
	double ch; // drag coefficient for heat
	int mozsign; // number of times moz changes sign
	

	
	for (iter=1;iter < niterc+1;iter++)
		{
			
			sfcdif1_loop(xpn,iter,&moz,&fm,&fh,&fv,&mol,&cm,&ch,&mozsign);
		}

/*	C_DEBUG(xpn->pHe->pHEBalance->fHeatSens)
	C_DEBUG(moz);
	C_DEBUG(fm);
	C_DEBUG(fh);
	C_DEBUG(fv);
	C_DEBUG(mol);
	C_DEBUG(cm);
	C_DEBUG(ch);*/
	
	

	return ch;
}