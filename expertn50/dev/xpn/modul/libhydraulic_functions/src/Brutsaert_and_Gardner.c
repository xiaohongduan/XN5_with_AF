#include "Brutsaert_and_Gardner.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_functions.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
double M2_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{ 
  double WG = (double)0.0;
  double em1,em2,se;
  
//   Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

   em1 = (double)1 - (double)1/N1;
   em2 = (double)1 - (double)1/N2;

   se = Weight1 * abspowerDBL(((double)1 + abspowerDBL((Alpha1 * -Hakt),N1)),-em1)+
        Weight2 * abspowerDBL(((double)1 + abspowerDBL((Alpha2 * -Hakt),N2)),-em2);

 if (Hakt < (double)0.0)
 {
   WG = Tmin + (Tsat - Tmin) * se;
 }
 else
 {
   WG = Tsat;
 }
  return WG;

}  /*  Ende WasserGehalt      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{
 double tll,ttl,se,se1,se2,s1,s2,em1,em2,t1,t2;
 double K=(double)0;


// Hakt=(double)0.1 * Hakt; // [mm] to [cm]
 
 if (Hakt < (double)0)
 {
   em1 = (double)1 - (double)1/N1;
   em2 = (double)1 - (double)1/N2;
   
   se1 = abspowerDBL(((double)1 + abspowerDBL((Alpha1 * -Hakt),N1)),-em1);
   if (se1<=(double)0) t1 = em1 * abspowerDBL(se1,((double)1/em1));
   se2 = abspowerDBL(((double)1 + abspowerDBL((Alpha2 * -Hakt),N2)),-em2);
   if (se2<=(double)0) t2 = em2 * abspowerDBL(se2,((double)1/em2));
   se  = Weight1 * se1 + Weight2 * se2;
   se  = min(se,(double)1);
   
   s1  = (double)1/((double)1 + abspowerDBL((Alpha1 * -Hakt),N1));
   s2  = (double)1/((double)1 + abspowerDBL((Alpha2 * -Hakt),N2));

   
   t1 = (double)1-abspowerDBL(((double)1-abspowerDBL(se1,((double)1/em1))),em1);
   t2 =	(double)1-abspowerDBL(((double)1-abspowerDBL(se2,((double)1/em2))),em2);

   ttl = Weight1 * Alpha1 * t1 + Weight2 * Alpha2 * t2;
   tll = Weight1 * Alpha1 + Weight2 * Alpha2;

   K = Ksat * abspowerDBL(se,(double)0.5) * tll/ttl*tll/ttl;
  }
  else
  {
   K = Ksat;
  }
   
 return K;
 
}  /*  Bimodale van Genuchten  Leitf*/



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{
 double s1,s2,t1,t2,se,em1,em2;
 double Wakt,Wvor;
 double DWC=(double)0.0;
 double dd = (double) 1;
 double tolp = (double)0.01;
 
 if (fabs(Hakt-Hvor) >= tolp)
 {
   Wakt = M2_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                   Alpha2,N2,M2,Weight1,Weight2);
   Wvor = M2_WCont(Hvor,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                   Alpha2,N2,M2,Weight1,Weight2);
   DWC = (Wakt-Wvor)/(Hakt-Hvor);
 }
 else   
 {

//   Hakt=(double)0.1 * Hakt; // [mm] to [cm]
 
   if (Hakt < (double)0.0)
   {
     em1 = (double)1 - (double)1/N1;
     em2 = (double)1 - (double)1/N2;
   
     t1  = (double)1 + abspowerDBL((Alpha1*-Hakt),N1);
     t2  = (double)1 + abspowerDBL((Alpha2*-Hakt),N2);
   
     s1  = abspowerDBL((Alpha1 * - Hakt),(N1 - (double)1));
     s2  = abspowerDBL((Alpha2 * - Hakt),(N2 - (double)1));

     se = Weight1 * N1 * em1 * Alpha1 * abspowerDBL(t1,(-em1-(double)1)) * s1 +
          Weight2 * N2 * em2 * Alpha2 * abspowerDBL(t2,(-em2-(double)1)) * s2 ;

//   t  = (double)1 + abspowerDBL((Alpha * -Hakt),N);
//   s  = abspowerDBL((Alpha * - Hakt),(N - (double)1));
//   w  = abspowerDBL(((Tsat-Tmin)*t),((double)1/N -(double)2))
//        * (N - (double)1)* Alpha * s;

     DWC = (Tsat-Tmin) * se;
   }
   else
   {
     DWC = (double)1.0e-6;
   }
 
//   DWC=(double) 0.1 * DWC; 
 }

 return DWC;
}  /*  van Genuchten  DWC*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : M2_MPotl()                                          */
/*  Function : Since there is no easy way to calculate matric      */
/*			   potential from a given volumetric water content     */
/*             a dummy is used. That means matric potentials       */
/*             have to be read in.                                 */
/*                                                                 */
/*  Autor    : ch, 6.10.97                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 return Hakt;  
}  /*  end M2_MPotl  */