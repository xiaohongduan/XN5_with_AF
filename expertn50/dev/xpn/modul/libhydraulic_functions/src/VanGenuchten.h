#ifndef __VAN_GENUCHTEN_H__
#define __VAN_GENUCHTEN_H__



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
                double Alpha2, double N2, double M2, double W1, double W2);


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
                double Alpha2, double N2, double M2, double W1, double W2);
				

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
                double Alpha2, double N2, double M2, double W1, double W2);
				

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
                double Alpha2, double N2, double M2, double W1, double W2);



#endif
