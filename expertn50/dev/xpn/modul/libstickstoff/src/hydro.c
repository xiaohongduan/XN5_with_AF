//      hydro.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


//#include "stickstoff.h"
//#include <glib.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//
//	/********************************************************************************************/
///*                                                                                          */
///* description :   Mineralisierung bzw. Immobilisierung                                     */
///*                 Methode  LeachM V3.0 (Hutson & Wagenet 1992)                             */
///*                 und Methode DNDC (Li, Frolking & Frolking, 1992)                         */
///*------------------------------------------------------------------------------------------*/
///*                                                                                          */
///*                 ep/gsf  18.5.94                                                          */
///*                 ch/gsf  13.4.95                                                          */
///*                         13.9.95  Einführung der Plot- Funktionen                         */
///*                         18.9.95 Aufspaltung in miner.c, nitrif.c                         */
///*                         11.3.96 Frost-Einfluss                                           */
///*                 dm/gsf  01.8.96 Implementieren der neuen Variablenstruktur               */
///********************************************************************************************/
///********************************************************************************************/
///* Procedur    :   HydroLEACHN()                                                            */
///* Beschreibung:   Harnstoff Hydrolyse                                                      */
///*                 Methode LEACHM 3.0 (Hutson & Wagenet, 1992)                              */
///*------------------------------------------------------------------------------------------*/
///*                                                                                          */
///*                 cs/age  16.02.93                            version    0.1               */
///*                 ep/gsf  28.02.94                                                         */
///*                                                                                          */
///********************************************************************************************/
///* veränd. Var.    pCL->fUreaN                                                              */
///*                 pCL->fNH4N                                                               */
///*                                                                                          */
///********************************************************************************************/

#include "stickstoff.h"
#include "stickstoff_macros.h"

int leachn_hydro_load(stickstoff *self)
{
	//expertn_modul_base *xpn = &(self->parent);
	stickstoff_load_ini(self);
	return RET_SUCCESS;
}

int leachn_hydro_run(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	
	PHEAT pHe = xpn->pHe ;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;

	double Q10,rm,Btem,Scorr,Tcorr,Wcorr,wmin,wlow,whigh;
	double f1;

	PSLAYER    pSL;
	PSWATER    pSW;
	PWLAYER    pWL;
	PHLAYER    pHL;
	PCLAYER    pCL;

	/* Initialisieren der Rektionsraten ?=> expinit.expfile, Eingabedaten */

	Q10   = (double)3.0;
	rm    = (double)1.0;
	Btem  = (double)20.0;
	Scorr = (double)0.6;

	/* Berechnung erfolgt von Schicht 2 bis vorletzte  */

	for (pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext,
	        pHL = pHe->pHLayer->pNext, pCL = pCh->pCLayer->pNext,
	        pSL = pSo->pSLayer->pNext;
	        ((pSW->pNext != NULL) && (pWL->pNext != NULL) &&
	         (pHL->pNext != NULL) && (pCL->pNext != NULL) &&
	         (pSL->pNext != NULL));
	        pSW = pSW->pNext, pWL = pWL->pNext,
	        pHL = pHL->pNext, pCL = pCL ->pNext,
	        pSL = pSL->pNext)
		{

			if (pCL->fUreaN>EPSILON)
				{
					Tcorr = abspowerDBL(Q10,((pHL->fSoilTemp - Btem)/(double)10.0));

					f1    = (double)-153300.0;
					wmin  = WATER_CONTENT(f1);
					f1    = (double)-30660.0;
					wlow  = WATER_CONTENT(f1);
					whigh = max(wmin,(pSL->fPorosity - (double)0.08));


					if (pWL->fContAct > whigh)
						{
							f1    = (pSL->fPorosity - pWL->fContAct) / (pSL->fPorosity - whigh);
							Wcorr = Scorr +((double)1.0 - Scorr) * abspowerDBL(f1,rm);
						}
					else
						{
							if (pWL->fContAct >= wlow)
								{
									Wcorr=(double)1.0;
								}
						}

					if (pWL->fContAct < wlow)
						{
							f1 =(max(pWL->fContAct,wmin) - wmin)/(wlow - wmin);
							Wcorr = abspowerDBL(f1,rm);
						}

					/* Hydrolyse - Rate  */
					pCL->fUreaHydroR = pCL->fUreaHydroMaxR * Tcorr * Wcorr * pCL->fUreaN;

					/* Hydrolyse   */
					pCL->fUreaN     -= pCL->fUreaHydroR  * pTi->pTimeStep->fAct;
					pCL->fNH4N      += pCL->fUreaHydroR  * pTi->pTimeStep->fAct;

				}
		}
	return 0;
}


int leachn_hydro_done(stickstoff *self)
{
	G_FREE_IF_NOT_0(self->ini_filename);
	return RET_SUCCESS;

}
