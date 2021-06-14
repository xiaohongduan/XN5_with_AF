#ifndef __BROOKS_AND_COREY_H__
#define __BROOKS_AND_COREY_H__


				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);

#endif