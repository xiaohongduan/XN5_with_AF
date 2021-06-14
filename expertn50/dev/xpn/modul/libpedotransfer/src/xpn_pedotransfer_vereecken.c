#include "xpn_pedotransfer_vereecken.h"


/************************************************************************/
/* Name:       double PedoTransferVereecken() 	                        */
/*									                		            */
/* Function:   Calculates coefficients for van Genuchten-Mualem         */
/*             Soul Hydraulic Functions 			            		*/	
/*                                                                      */
/* Reference:  Vereecken, H., Maes, J., Feyen, J. & Darius, P. (1989):  */
/*             Estimating the soil moisture retention characteristic    */
/*             from texture, bulk density and carbon content.           */
/*             Soil Science 148, 389-403.                      			*/	        
/*                                                                      */
/* Authors:    mb 95, ep 02.97                                          */  				
/************************************************************************/

double xpn_pedotransfer_Vereecken(double dClayF, double dSiltF, double dSandF, 
                             double dBD, double dCarbonF, double dPS, int i)

{
	double		d1;
	double		d2;
	double		d3;
	double		d4;
	double		d5;

	double      dVGA;
	double      dVGN; 
	double      dThsat;
	double      dThres;
	double		dPTF;

    /* saturated Theta */
	d1 = (double)0.283 * dBD;
	d2 = (double)0.001 * dClayF;
    dThsat = (double)0.81 - d1 + d2;

    /* residual Theta */
	d1 = (double) 0.005 * dClayF;
    d2 = (double) 0.014 * dCarbonF;
	dThres = (double) 0.015 + d1 + d2;

	/* Van Genuchten Alpha */
	d1 = (double)0.025 * dSandF;
	d2 = (double)0.351 * dCarbonF;
	d3 = (double)2.617 * dBD;
	d4 = (double)0.023 * dClayF;
	d5 = -(double)2.486 + d1 - d2 - d3 - d4;
	/* Log. nat. */
	d1 = exp((double)1);
	dVGA = pow(d1, d5);		   /* entlogarithmiert */
    dVGA = (double)0.1 * dVGA; /* convert [1/cm] to [1/mm] */
    
	/* Van Genuchten N */
	d1 = (double)0.009 * dSandF;
	d2 = (double)0.013 * dClayF;
	d3 = (double)0.00015 * dClayF * dClayF;
	d4 = (double)0.053 - d1 - d2 + d3;
	/* Log. nat. */
	d1 = exp((double)1);
	dVGN = pow(d1, d4);		   /* entlogarithmiert */

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=-(double)1/dVGA;
    else if (i==4)  dPTF=(double)1/(dVGN-(double)1);
    else if (i==5)  dPTF=dVGA;
    else if (i==6)  dPTF=dVGN;
    else if (i==7)  dPTF=(double)1;
//    else            dPTF=dKsat;

	return dPTF;

} /* end PedoTransferVereecken() */
