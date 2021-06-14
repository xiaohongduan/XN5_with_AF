#include "Brooks_and_Corey.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_functions.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BC_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BC_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    f1 = min(Hmin,Hakt);

    if ((f1-Ca) <= (double)0.0)
    {
     f2 = Tmin+(Tsat-Tmin)* (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
    }
    else
    {
     f2 = Tsat;
    }
    
  return f2;
}  /*  end  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BC_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.6.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BC_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;

    f1 = min(Hmin,Hakt);

    if ((f1 - Ca) <= (double)0.0)
     {
      f2 = (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
      f2 *= ((double)-1.0/(Cb*f1)); 
      f2 *= (Tsat-Tmin);
      f2=max(f2,(double)1e-37); 
     }
    else /*f1-Ca > 0.0 */
     {
      f2 = (double)0.0;
     }

  return f2;
}  /*  end   */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : BC_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *             according to Brooks and Corey (1964)       
 *  Author   : ep 19.10.93, ep 25.02.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double BC_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0;

  f1 = (Takt-Tmin)/(Tsat-Tmin);

  f2 = Ca * ((double)abspowerDBL(f1,((double)-1.0 * Cb)));
      
  f2=max(f2,(double)-1e37);
      
  return f2;
}  /*  end MPotl      */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BC_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double BC_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)1.0;    /* pore interaction parameter  */
    double f1=(double)0.0,f2=(double)0.0,f3=(double)1.0;

    if (Hakt <= Ca)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = Ksat * (double)abspowerDBL((Ca/Hakt),f1);
    }
    else 
    {
     f2 = Ksat;
    }
     f2=max(f2,(double)1e-300);

  return f2;
}  /*  end      */
