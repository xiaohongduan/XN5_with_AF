#include "Hutson_and_Cass.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_functions.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : HC_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             after Hutson und Cass (1992) LEACHN        */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double HC_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    f1 = min(Hmin,Hakt);

    if ((f1-Hc) <= (double)0.0)
    {
     f2 = Tmin + (Tsat-Tmin) * (double)(abspowerDBL(f1/Ca,(double)-1.0/Cb));
    }
    else
    {
     f2 =((Tsat-Tmin)*((double)1.0 - (Tc-Tmin)/(Tsat-Tmin))*f1*f1);
     f2 =(f2/((Ca*Ca)*((double)(abspowerDBL((Tc-Tmin)/(Tsat-Tmin),(double)-2.0*Cb)))));
     f2 = Tsat - f2;
    }
    
  return f2;
}  /*  end  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : HC_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             after Hutson und Cass (1992) LEACHN        */
/*                                                        */
/*  Author   : cs 22.6.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double HC_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;

    f1 = min(Hmin,Hakt);

    if ((f1 - Hc) <= (double)0.0)
     {
      f2 = (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
      f2 *= ((double)-1.0*Tsat/(Cb*f1));
      f2=max(f2,(double)1e-37); 
     }
    else /*f1-Hc > 0.0 */
     {
      f3 = Ca*Ca*((double)abspowerDBL((Tc/Tsat),((double)-2.0*Cb)));
      f2 = (double)-2.0*Tsat*((double)1.0-(Tc/Tsat))*f1/f3;

     }

  return f2;
}  /*  end   */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : HC_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *  Author   : ep 19.10.93, ep 25.02.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double HC_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;
  f1 = Takt/Tsat;

  if (Takt <= Tc)
    {
     f3 = Ca * ((double)abspowerDBL(f1,((double)-1.0 * Cb)));
    }
  else //if (Takt > Tc)
    { 
     f2 = Tc/Tsat;
     f3 =  (double)sqrt((double)((double)1.0 - f1)) * Ca;
     f3 *= (double)(abspowerDBL(f2,((double)-1.0 * Cb)));
     f3 /=((double)sqrt((double)((double)1.0 - f2)));
    }
      
  f3=max(f3,(double)-1e37);
      
  return f3;
}  /*  end MPotl      */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : HC_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             after Hutson und Cass (1992) LEACHN        */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double HC_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)1.0;    /* pore interaction parameter  */
    double f1,f2,f3;

    if (Hakt <= Hc)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = Ksat * (double)abspowerDBL((Ca/Hakt),f1);
    }
    else 
    {
///*
     f1 =((HC_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha,N,M,Ca,Cb,
                    Hc,Tc,Hmin,Hvor,Tvor,Alpha2,N2,M2,W1,W2)-Tmin)/(Tsat-Tmin));
//*/
//     f1 = (Takt-Tmin)/(Tsat-Tmin); 
     f3 =  (double)2.0*Cb + (double)2.0 + RP;  /* ==Cb(2.0+((2.0+RP)/Cb)) */
     f2 = Ksat * (double)(abspowerDBL(f1,f3));
    }    
     f2=max(f2,(double)1e-300);

  return f2;
}  /*  end      */
