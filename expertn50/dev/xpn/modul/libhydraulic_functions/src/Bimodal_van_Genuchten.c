#include "Bimodal_van_Genuchten.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_functions.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : C2_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             according to bimodal Campbell              */
/*                                                        */
/*  Author   : ep 6.10.97                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double C2_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    double  Hb=(double)0.0;

    f1 = min(Hmin,Hakt);
    Hb=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0* Ca)));
//    Hb=(double)-40.0;
    Hb= min(Hmin,Hb);

    if (f1 > Hb)
    {
     f2=Tsat;
//     f2=W1;
    }
    else if ((f1 <= Hb)&&(f1 > Hc))
    {
//     f2 = Tsat * (double)(abspowerDBL((f1/(double)-40.0),((double)-1.0/Ca)));
     f2 = W1 * (double)(abspowerDBL((f1/(double)-40.0),((double)-1.0/Ca)));
    }
    else
    {
//     f2 = Tsat * (double)(abspowerDBL((f1/Hc),((double)-1.0/Cb)));
     f2 = Tc * (double)(abspowerDBL((f1/Hc),((double)-1.0/Cb)));
    }
    
  return f2;
}  /*  end  */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : C2_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             according to bimodal Campbell              */
/*                                                        */
/*  Author   : ep 6.10.97                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double C2_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0;
	double Hb=(double)0.0;

    f1 = min(Hmin,Hakt);
    Hb=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0* Ca)));
//    Hb=(double)-40.0;
	Hb= min(Hmin,Hb);
    if (f1 > Hb)
     {
      f2 = (double)0.0;
     }
//    else if ((f1 <= (double)-40.0)&&(f1 > Hc))
    else if (f1 > Hc)
     {
      f2 = (double)(abspowerDBL((f1/(double)-40.0),((double)-1.0/Ca)));
      f2 *= ((double)-1.0/(Ca*f1)); 
      f2 *= W1;
      f2=max(f2,(double)1e-37); 
     }
    else //f1 <= Hc 
     {
      f2 = (double)(abspowerDBL((f1/Hc),((double)-1.0/Cb)));
      f2 *= ((double)-1.0/(Cb*f1)); 
      f2 *= Tc;
      f2=max(f2,(double)1e-37); 
     }

  return f2;
} //  end   

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : C2_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *             according to Bimodal Campbell       
 *  Author   : ep 6.10.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double C2_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0;
  double Hb=(double)0.0;

  Hb=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0 * Ca)));
//  Hb=(double)-40.0;
  Hb= min(Hmin,Hb);

  if (Takt >= Tsat)
  {
   f2 = Hb;
  }
  else if (Takt > Tc)
  {
   f2 = (double)-40.0 * ((double)abspowerDBL(Takt/W1,((double)-1.0 * Ca)));
  }
  else  
  {
   f2 = Hc * ((double)abspowerDBL(Takt/Tc,((double)-1.0 * Cb)));
  } 
      
  f2=max(f2,(double)-1e37);
      
  return f2;
}  /*  end MPotl      */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : C2_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             according to bimodal Campbell              */
/*                                                        */
/*  Author   : ep 6.10.97                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double C2_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)0.5;    /* pore interaction parameter  */
    double f0=(double)0.0,f7=(double)0.0,f8=(double)0.0;
    double f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;
    double f4=(double)0.0,f5=(double)0.0,f6=(double)0.0;
    
    Takt=C2_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha,N,M,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,Alpha2,N2,M2,W1,W2);

    W1=Tsat;
//    f8=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0* Ca)));
//    Ksat=(double)0.1*Ksat;
    f8=(double)-40.0;
    if (Hakt > f8)
    {
     f0 = Ksat;
    }
    else if ((Hakt <= f8)&&(Hakt > Hc))
    {
     f2 = (double)abspowerDBL((Tc/W1),(Cb+(double)1.0));
     f3 = (double)abspowerDBL((Tc/W1),(Ca+(double)1.0));
     f4 = f2/(Hc*(Cb+(double)1.0))+((double)1.0-f3)/((double)-40.0*(Ca+(double)1.0));
     f5 = f4 * f4;
     f6 = (double)abspowerDBL((Takt/W1),(Ca+(double)1.0));
     f7 = f2/(Hc*(Cb+(double)1.0))+(f6-f3)/((double)-40.0*(Ca+(double)1.0));
     f0 = Ksat * (double)abspowerDBL((Takt/W1),RP)/f5 * f7 *f7;
    }
    else //if (Hakt <= Hc)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = (double)abspowerDBL((Tc/W1),(Cb+(double)1.0));
     f3 = (double)abspowerDBL((Tc/W1),(Ca+(double)1.0));
     f4 = f2/(Hc*(Cb+(double)1.0))+((double)1.0-f3)/((double)-40.0*(Ca+(double)1.0));
     f5 = f4 * f4;
     f6 = (double)2.0 * Cb + (double)2.0 + RP;
     f7 = (double)abspowerDBL((Tc/W1),f6)/f5;
     f0 = Ksat * f7 * (double)abspowerDBL((Hc/Hakt),f1)/((Hc*(Cb+(double)1.0))*(Hc*(Cb+(double)1.0)));
     f0 = Ksat * (double)abspowerDBL((Takt/W1),f6)/f5/((Hc*(Cb+(double)1.0))*(Hc*(Cb+(double)1.0)));
    }
 
    f0=max(f0,(double)1e-300);

  return f0;
}  /*  end      */


