#ifndef __BIMODAL_VAN_GENUCHTEN_H__
#define __BIMODAL_VAN_GENUCHTEN_H__

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
                double Alpha2, double N2, double M2, double W1, double W2);
				
				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);

#endif