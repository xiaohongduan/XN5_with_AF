#include "xpn_pedotransfer_RawlsBrakensiek.h"



/************************************************************************/
/* Name:       double PedoTransferRawlsBrakensiek() 	                */
/*									                		            */
/* Function:   Calculates coefficients for van Genuchten                */
/*             and Brooks&Corey-Campbell Soil Hydraulic Functions 	    */
/*                                                                      */
/* Reference:  Rawls, W.J., Gish, T.J., & Brakensiek, D.L.              */
/*             Estimating the soil water retention from soil physical   */
/*             properties and characteristics                           */
/*             Advances in Soil Science 16, 213-234                     */
/*             Springer Verlag, New York, 1991                          */	        
/*                                                                      */
/* Authors:    mb 95, ep 02.97                                          */  				
/************************************************************************/

double xpn_pedotransfer_RawlsBrakensiek(double dClayF, double dSiltF, double dSandF, 
                                   double dBD, double dCarbonF, double dPS, int i)

{
	double		d1;
	double		d2;
	double		d3;
	double		d4;
	double		d5;

    double      dCampA; 
    double      dCampB; 
    double      dThsat; 
	double      dThres;
	double		dPTF;


    /* saturated Theta */
    d1 = -(double)0.0015 * dSandF -(double)0.0022 * dClayF + (double)0.984 * dPS;
    d2 = (double)0.0001 * dClayF * dClayF +(double)0.0036 * dSandF * dPS -(double)0.011 * dClayF * dPS;
    d3 = -(double)0.0001 * dClayF * dClayF * dPS -(double)0.0024 * dPS * dPS * dSandF;
    d3 = d3 +(double)0.012 * dPS * dPS * dClayF;
    dThsat = (double)0.01162 + d1 + d2 + d3; 

    /* residual Theta */
	d1 = (double)0.0009 * dSandF +(double)0.005 * dClayF +(double)0.029 * dPS;
    d2 = -(double)0.00015 * dClayF * dClayF - (double)0.001 * dSandF * dPS;
    d3 = -(double)0.0002 * dClayF * dClayF * dPS * dPS;
    d4 = (double)0.0003 * dClayF * dClayF * dPS - (double)0.002 * dClayF * dPS * dPS;
	dThres = -(double) 0.018 + d1 + d2 + d3 + d4;

	/* Brooks-Corey bubbling pressure [mm]*/
	d1 = (double)0.185 * dClayF - (double) 2.484 * dPS;
	d2 = -(double)0.002*dClayF*dClayF-(double)0.044*dSandF*dPS-(double)0.6175*dClayF*dPS;
	d3 = (double)0.00144*dSandF*dSandF*dPS*dPS -(double)0.009*dClayF*dClayF*dPS*dPS;
	d4 = -(double)0.000013 *dSandF*dSandF*dClayF;
	d4 = d4 +(double)0.009*dClayF*dClayF*dPS -(double)0.0007*dSandF*dSandF*dPS;
	d4 = d4 +(double)0.000005*dClayF*dClayF*dSandF +(double)0.5*dPS*dPS*dClayF;
	d5 = (double)5.34 + d1 + d2 + d3 + d4;
	/* Log. nat. */
	d1 = exp((double)1);
	dCampA = -(double)10 * pow(d1, d5);	/* inverse logarithm & [cm] to [mm]*/

	/* Brooks-Corey pore size distribution index*/
	d1 = (double)0.018 * dSandF -(double)1.062 * dPS;
	d2 = -(double)0.00005*dSandF*dSandF -(double)0.003*dClayF*dClayF +(double)1.111*dPS*dPS;
	d2 = d2 -(double)0.031*dSandF*dPS; 
	d3 = (double)0.0003*dSandF*dSandF*dPS*dPS -(double)0.006*dClayF*dClayF*dPS*dPS;
    d3 = d3 -(double)0.000002 *dSandF*dSandF*dClayF +(double)0.008*dClayF*dClayF*dPS;
    d3 = d3 -(double)0.007*dPS*dPS*dClayF;
	d4 = -(double)0.784 + d1 + d2 + d3;
	/* Log. nat. */
	d1 = exp((double)1);
	dCampB = (double)1/pow(d1,d4);						/* entlogarithmiert */

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=dCampA;
    else if (i==4)  dPTF=dCampB;
    else if (i==5)  dPTF=-(double)10/dCampA; /* van Genuchten alpha in [cm-1] */
    //else if (i==5)  dPTF=-(double)1/dCampA; 
    else if (i==6)  dPTF=(double)1/dCampB + (double)1;
    else if (i==7)  dPTF=(double)1-(double)1/((double)1/dCampB + (double)1);
//    else            dPTF=dKsat;

	return dPTF;

} /* end PedoTransferRawlsBrakensiek() */
