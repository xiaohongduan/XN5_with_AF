#include "xpn_pedotransfer_campbell.h"
#include <math.h>


/************************************************************/
/* Name:       double PedoTransferCampbell()			    */
/*									                		*/
/* Function:   Calculates coefficients for Brooks&Corey-    */
/*             Campbell Soil Hydraulic Functions 		    */	
/*                                                          */
/* Reference:  CAMPBELL, G.S.: Soil Physics with Basic      */
/*             Elsevier, Amsterdam-Oxford-New York-Tokio,   */
/*             pp. 150, 1985                                */
/*                                   				        */
/* Authors:    mb 95, ep 02.97                              */  				
/************************************************************/

double xpn_pedotransfer_campbell(double dClayF, double dSiltF, double dSandF, 
                            double dBD, double dCarbonF, double dPS, int i)
{
 #define DIAMETER_LIMIT_CLAY (double) 0.001 /* [mm] */
 #define DIAMETER_LIMIT_SILT (double) 0.026
 #define DIAMETER_LIMIT_SAND (double) 1.025

	double		dDg;
	double		dSigmaG;
	double		dSum;
	double		dA;
	double		dB;
	double		dA1;
	double		dA2;
	double		dPsiEs;
	double		x;
	double		y;
	double		dFactorDay;

    double      dThsat=dPS;
    double      dThres=(double)0;
    double      dCampA; 
    double      dCampB; 
    double      dKsat; 
    double      dPTF; 

	/********************************************************************************/
	/* Mean geometric particle-size diameter: dg [mm]                               */
	/* Calculated according to:                                                     */
	/* SHIRAZI, M.A. & L. BOERSMA: A Unifying Quantitative Analysis of Soil Texture */
	/* In: Soil Sci. Soc. Am. J., Vol. 48, 1984, S. 142-147                         */
	/********************************************************************************/
	dSum   = (double)0;
	dA     = (double)0;
	dDg    = (double)0;
	dSum   = dSum + (dClayF * log((double)DIAMETER_LIMIT_CLAY));
	dSum   = dSum + (dSiltF * log((double)DIAMETER_LIMIT_SILT));
	dSum   = dSum + (dSandF * log((double)DIAMETER_LIMIT_SAND));
	dA     = (double)0.01 * dSum;
	dDg = exp(dA);

	/********************************************************************************/
	/* Geometric standard deviation of particle-size diameter: SigmaG               */
	/* Calculated according to:                                                     */
	/* SHIRAZI, M.A. & L. BOERSMA: A Unifying Quantitative Analysis of Soil Texture */
	/* In: Soil Sci. Soc. Am. J., Vol. 48, 1984, S. 142-147                         */
	/********************************************************************************/
	dSum = (double)0;
	dSum   = dSum + (dClayF * (log((double)DIAMETER_LIMIT_CLAY) * log((double)DIAMETER_LIMIT_CLAY)));
	dSum   = dSum + (dSiltF * (log((double)DIAMETER_LIMIT_SILT) * log((double)DIAMETER_LIMIT_SILT)));
	dSum   = dSum + (dSandF * (log((double)DIAMETER_LIMIT_SAND) * log((double)DIAMETER_LIMIT_SAND)));
	dB = sqrt((double)0.01 * dSum - (dA * dA));
	dSigmaG = exp(dB);


	/*********************************************************************/
	/* Campbell B from: dg and SigmaG                                    */
	/* Calculated according to:                                          */
	/* CAMPBELL, G.S.: Soil Physics with Basic                           */
	/* Elsevier, Amsterdam-Oxford-New York-Tokio, pp. 150, 1985          */
	/*********************************************************************/
	dB=(double)1 / sqrt(dDg) + ((double)0.2 * dSigmaG);
	dCampB = dB;


	/*********************************************************************/
	/* Air Entry Value aus:                                              */
	/* Calculated according to:                                          */
	/* Berechnung nach:                                                  */
	/* CAMPBELL, G.S.: Soil Physics with Basic                           */
	/* Elsevier, Amsterdam-Oxford-New York-Tokio, pp. 150, 1985          */
	/*********************************************************************/
	dPsiEs = (double)-0.5 * ((double)1 / sqrt(dDg));
	x = dBD / (double)1.3;
	y = (double)0.67 * dB;
	dCampA = (dPsiEs * pow(x, y));
	dCampA = dCampA * 102.5; // convert from [kPa] to [mm]


	/*********************************************************************/
	/* Saturated Coductivity Ksat                                        */
	/* Calculated according to:                                          */
	/* CAMPBELL, G.S.: Soil Physics with Basic                           */
	/* Elsevier, Amsterdam-Oxford-New York-Tokio, pp. 150, 1985          */
	/*********************************************************************/
	dA1 = (double)3.92 * pow((double)10, (double)-5);
	dA2 = (pow(((double)1.3/dBD), ((double)1.3 * dB)));
	dA = dA1 * dA2;
	dB = exp(((double)-6.9 * (dClayF/(double)100)) - ((double)3.7 * (dSiltF/(double)100)));
	/* (m/s) */
	dKsat = dA * dB;

	/* m/s -> mm/d */
	/* m = 1000 mm */
	/* s = d/(60*60*24) */
	dFactorDay = (double)1000 * (double)60 * (double)60 * (double)24;
	dKsat = dKsat * dFactorDay;

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=dCampA;
    else if (i==4)  dPTF=dCampB;
    else if (i==5)  dPTF=-(double)10/dCampA; /* van Genuchten alpha in [cm-1] */
    //else if (i==5)  dPTF=-(double)1/dCampA; 
    else if (i==6)  dPTF=(double)1/dCampB + (double)1;
    else if (i==7)  dPTF=(double)1-(double)1/((double)1/dCampB + (double)1);
    else            dPTF=dKsat;

	return dPTF;

} /* end PedoTransferCampbell() */
