#include "VanGenuchten.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math_functions.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_HCond()                                            */
/*                                                                   */
/*  Function : Calculation of hydraulic unsaturated conductivity     */
/*             following van Genuchten (1980) and Mualem (1976)      */
/*             HYDRUS 5.0                                            */
/*  Author   : ep 27.11.95                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double VG_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)
{
 //Hydraulic conductivity
 double m,n,Alfa,Qe,Qee,Qees,Qs,Qr,Qm,Hs,HH;
 double FFQ,FFQs,Kr,Ks,FK;
 double Bpar=(double)0.5;
 int Ppar=2;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;
      Ks=Ksat;

//      Hakt=(double) 0.1 * Hakt; // [mm] to [cm]
      
      m=(double)1-(double)1/n;
      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qees,(double)-1/m)-(double)1
                            ,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         Qee=abspowerDBL((double)1+abspowerDBL(-Alfa*HH,n),-m);
         Qe=(Qm-Qr)/(Qs-Qr)*Qee;
         FFQ =(double)1-abspowerDBL((double)1-abspowerDBL(Qee,(double)1/m),m);
         FFQs=(double)1-abspowerDBL((double)1-abspowerDBL(Qees,(double)1/m),m);
         if(FFQ<=(double)0) FFQ=m*abspowerDBL(Qee,(double)1/m);
         Kr=abspowerDBL(Qe,Bpar)*abspowerDBL(FFQ/FFQs,Ppar);
         FK=max(Ks*Kr,(double)1e-300);
        }  
      else
        {
         FK=Ks;
        }
 return FK;

}  /*  van Genuchten/Mualem conductivity */
/*------------------------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_WCont()                                            */
/*                                                                   */
/*  Function : Calculation of volumetric water content               */
/*             following van Genuchten (1980) and HYDRUS 5.0         */
/*                                                                   */
/*  Author   : ep  27.11.95                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
               
double VG_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)
{
 double m,n,Alfa,Qr,Qs,Qm,Qee,Qees,Hs,HH,FQ;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;

//      Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

      m=(double)1-(double)1/n;
//      m=M;
      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qees,(double)-1/m)-(double)1
                                     ,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         Qee=abspowerDBL((double)1+abspowerDBL(-Alfa*HH,n),-m);
         FQ=max(Qr+(Qm-Qr)*Qee,(double)1e-37);
        }
      else
        {
         FQ=Qs;
        }
          
 return FQ;
}  /*  end van Genuchten water content  */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_DWCap()                                          */
/*                                                                 */
/*  Function : Calculation of differential water capacity          */
/*             following van Genuchten (1980) and HYDRUS 5.0       */
/*  Author   : ep 27.11.95                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double VG_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double m,n,Alfa,Qr,Qs,Qm,Qees,Hs,HH,C1,C2,FC;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;

//      Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

      m=(double)1-(double)1/n;
//      m=M;
      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qees,(double)-1/m)-(double)1
                            ,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         C1=abspowerDBL((double)1+abspowerDBL(-Alpha*HH,n),-m-(double)1);
         C2=(Qm-Qr)*m*n*abspowerDBL(Alpha,n)*abspowerDBL(-HH,n-(double)1)*C1;
         FC=max(C2,(double)1e-300);
        } 
      else
        {
         FC=(double)0;
        } 

//      FC=(double)0.1 * FC; //[cm] to [mm]

 return FC;
}  /* end van Genuchten  DWC*/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_MPotl()                                          */
/*  Function : Calculation of matric potential from                */
/*             a given volumetric water content                    */
/*             following van Genuchten (1980) and HYDRUS 5.0       */
/*                                                                 */
/*  Autor    : ep, 12.4.96                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double VG_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double m,n,Alfa,Qr,Qs,Qm,Qe,Qee,QeeM,HMin,HH,FH;
 double test1,test2,test3;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;
      Qe= ((Takt-Tmin)/(Tsat-Tmin));

      m=(double)1-(double)1/n;
//      m=M;
      HMin=-abspowerDBL((double)1e300,((double)1/n));
      QeeM=abspowerDBL((double)1+abspowerDBL(-Alpha*HMin,n),-m);
      Qee=min(max(Qe*(Qs-Qr)/(Qm-Qr),QeeM),(double).999999999999999);
    /*  HH=(double)-1/Alpha*abspowerDBL(abspowerDBL(Qee,(double)-1/m)-(double)1
                                     ,(double)1/n); */
	  HH=-1.0/Alpha * abspowerDBL(abspowerDBL (Qee,-1.0 / m)-1.0
                                     ,1.0/n);
									 
	  test1 = abspowerDBL (Qee,-1.0 / (double)m);
	  test2 =  abspowerDBL(test1-1.0,1.0/n);
	  test3 = -1.0/Alpha * test2;
      FH=max(HH,(double)-1e37);
//      FH=(double)10.0 * FH; //[cm] to [mm]
 return FH;
}  /*  end VGMPotl  */
