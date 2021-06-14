#ifndef __BRUTSAERT_AND_GARDNER_H__
#define __BRUTSAERT_AND_GARDNER_H__

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2);
				
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2);
				
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
double M2_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);

#endif