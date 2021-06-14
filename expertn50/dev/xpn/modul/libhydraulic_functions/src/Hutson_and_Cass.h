#ifndef __HUTSON_AND_CASS_H__
#define __HUTSON_AND_CASS_H__

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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
				
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
                double Alpha2, double N2, double M2, double W1, double W2);
				
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
                double Alpha2, double N2, double M2, double W1, double W2);


#endif