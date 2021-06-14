//      schaaf.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//

#include "schaaf.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(schaaf, schaaf, EXPERTN_MODUL_BASE_TYPE);

static void schaaf_class_init(schaafClass* klass)
{
}

static void schaaf_init(schaaf* self)
{	
	// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
    // Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
    // Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden
}

// Our Modul Functions:

/*********************************************************************************/
/*  Name     : TSFertilizer                                                      */
/*                                                                               */
/*  Funktion : Die N-Zufuhr durch mineralische oder organische Duengemittel wird */
/*             nach Modellansaetzen von Godwin, die von Engel modifiziert wurden,*/
/*             integriert.                                                       */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*				pCh->pCLayer->fNO3N                                              */
/*				pCh->pCLayer->fNH4N                                              */
/*				pCh->pCLayer->fUreaN                                             */
/*				pCh->pCLayer->fNManure                                           */
/*				pCh->pCLayer->fCManure                                           */
/*				pCh->pCLayer->fManureCN                                          */
/*                                                                               	  */
/*********************************************************************************/

int schaaf_fertilizer(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);

    PNFERTILIZER pNF = xpn->pMa->pNFertilizer;
	
    if ((pNF != NULL) && (pNF->Day > 0)) {
		//Annotated by Hong on 20180515
		//1. first action of fertilization:

		if (self->first_fertil_done == 0){
			//1.1 for regular XN:
			if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
									xpn->pTi->pSimTime->mon,
									xpn->pTi->pSimTime->mday,
									pNF->Year,
									pNF->Month,
									pNF->Day) == 0)){
				self->first_fertil_done += 1;
				if ((strncmp(pNF->acCode, "RE\0", 2) == 0) || (strncmp(pNF->acCode, "FE\0", 2) == 0))
					schaaf_mineral_fertilization(self);
				if ((strncmp(pNF->acCode, "LA\0", 2) == 0) || (strncmp(pNF->acCode, "HA\0", 2) == 0))
					schaaf_organic_fertilization(self);
					
			}
			
			//1.2 for XN-MPMAS
			else if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
									xpn->pTi->pSimTime->mon,
									xpn->pTi->pSimTime->mday,
									pNF->pNext->Year,
									pNF->pNext->Month,
									pNF->pNext->Day) == 0)){
			
				xpn->pMa->pNFertilizer = xpn->pMa->pNFertilizer->pNext;// XN-MPMAS assigns fertilization data to  xpn->pMa->pNFertilizer->pNext (mpmas_coupling.c line 596, line627) Otherwise xpn->pMa->pNFertilizer->pNext remains empty and it won't switch to the 2. step for the next fetilization action.    						
				pNF = xpn->pMa->pNFertilizer;

				self->first_fertil_done += 1;
				if ((strncmp(pNF->acCode, "RE\0", 2) == 0) || (strncmp(pNF->acCode, "FE\0", 2) == 0))
					schaaf_mineral_fertilization(self);
				else if ((strncmp(pNF->acCode, "LA\0", 2) == 0) || (strncmp(pNF->acCode, "HA\0", 2) == 0))
					schaaf_organic_fertilization(self);
				else {
					char* S =  g_strdup_printf("Error in schaaf.c:schaaf_fertilizer\nUnknown fertilizer code: %s\n", pNF->acCode);
					PRINT_ERROR(S);// for debug
					g_free(S);
				}
					
			}
		}
		//2. next actions of fertilization (self->first_fertil_done!=0):
		else{
			    //valid for both regular XN and transfered fertilization data from XN-MPMAS (saved in pNF->pNext) 
				if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
									xpn->pTi->pSimTime->mon,
									xpn->pTi->pSimTime->mday,
									pNF->pNext->Year,
									pNF->pNext->Month,
									pNF->pNext->Day) == 0)) {

					xpn->pMa->pNFertilizer = xpn->pMa->pNFertilizer->pNext; //The data in xpn->pMa->pNFertilizer is required by InfiltrationOrgDuenger(self) and InfiltrationOrgDuengerRegen(self). Therefore xpn->pMa->pNFertilizer should not be switched to the next struc before the date of next fertilization action.   
					pNF = xpn->pMa->pNFertilizer;
					if ((strncmp(pNF->acCode, "RE\0", 2) == 0) || (strncmp(pNF->acCode, "FE\0", 2) == 0))
						schaaf_mineral_fertilization(self);
					else if ((strncmp(pNF->acCode, "LA\0", 2) == 0) || (strncmp(pNF->acCode, "HA\0", 2) == 0))
						schaaf_organic_fertilization(self);
                	                else {
        	                              	char* S =  g_strdup_printf("Error in schaaf.c:schaaf_fertilizer\nUnknown fertilizer code: %s\n", pNF->acCode);

	                                        PRINT_ERROR(S);// for debug
	                                        g_free(S);
	                                }
						
				}
		}
		

 /*       if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                   xpn->pTi->pSimTime->mon,
                                   xpn->pTi->pSimTime->mday,
                                   pNF->Year,
                                   pNF->Month,
                                   pNF->Day) == 0) &&
            xpn->pMa->pNFertilizer->pNext != NULL) {
            
            self->fertil_done = 0;*/

	}
	
	if (self->first_fertil_done >0)
	{
        InfiltrationOrgDuenger(self);
        InfiltrationOrgDuengerRegen(self);

        //MinerOrgDuengHoff(self);
        //NitrOrgNH4Hoff(self);
	}

    return RET_SUCCESS;
}

int schaaf_mineral_fertilization(schaaf* self) //Hong: =TSFertilizer()?
{
    expertn_modul_base* xpn = &(self->parent);
    PTIME pTi = xpn->pTi;
    PMANAGEMENT pMa = xpn->pMa;
    PCHEMISTRY pCh = xpn->pCh;
	PCBALANCE	pCB = pCh->pCBalance; //Added by Hong on 20180731
    double fumFacC;
    gchar* S, *S2;


    fumFacC = 0.4; // 40 % der org. Substanz im Duenger ist Kohlenstoff

    /*Faktor fumFacC wird in Abhaengugkeit von Duenger bestimmt. Bis
      der Duenger identifiziert werden kann standardmaessig auf 0.4 gesetzt.  */

    pMa->pNFertilizer->fCorgManure = pMa->pNFertilizer->fDryMatter * fumFacC;
    if (pMa->pNFertilizer->fNorgManure > EPSILON) {
        pMa->pNFertilizer->fOrgManureCN = pMa->pNFertilizer->fCorgManure / pMa->pNFertilizer->fNorgManure;
    } else
        pMa->pNFertilizer->fOrgManureCN = (double)0.1;

    /*Uebertragen der Duengergroessen auf die Oberflaechenschicht.  */
    pCh->pCProfile->fNO3NSurf += pMa->pNFertilizer->fNO3N;

    if (strncmp(pMa->pNFertilizer->acCode, "RE\0", 2) == 0) { // organische Duengung
        pCh->pCProfile->fNH4ManureSurf += pMa->pNFertilizer->fNH4N;
    } else {
        pCh->pCProfile->fNH4NSurf += pMa->pNFertilizer->fNH4N;
    }
//Begin of Hong: changed after XN3 on 20171201
/*
    pCh->pCProfile->fUreaNSurf += pMa->pNFertilizer->fUreaN;
    pCh->pCProfile->fNManureSurf += pMa->pNFertilizer->fNorgManure;
    pCh->pCProfile->fCManureSurf += pMa->pNFertilizer->fCorgManure;
    pCh->pCProfile->fCNManureSurf += pMa->pNFertilizer->fOrgManureCN;
*/
	
		//SG 20161009: For DAISY model - Partitioning of fNorgManure and fCorgManure
		// Scott Demyan: "current partition of 100% to AOM2 causes too little stabilization"
		// New partitioning:
		// AOM1 - 72% 
		// AOM2 - 18%
		// SOM2 - 10% 
		pCh->pCProfile->fUreaNSurf    += pMa->pNFertilizer->fUreaN;
		pCh->pCProfile->fNManureSurf  += (float)0.18*pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCManureSurf  += (float)0.18*pMa->pNFertilizer->fCorgManure;
		pCh->pCProfile->fCNManureSurf += pMa->pNFertilizer->fOrgManureCN;
		pCh->pCProfile->fNLitterSurf  += (float)0.72*pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCLitterSurf  += (float)0.72*pMa->pNFertilizer->fCorgManure;
		pCh->pCProfile->fCNLitterSurf += pMa->pNFertilizer->fOrgManureCN;
		pCh->pCProfile->fNHumusSurf  += (float)0.1*pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCHumusSurf  += (float)0.1*pMa->pNFertilizer->fCorgManure;
		pCh->pCProfile->fCNHumusSurf += pMa->pNFertilizer->fOrgManureCN;
		
		pCh->pCProfile->fCLeafLitterSurf  += (float)0.72*pMa->pNFertilizer->fCorgManure; //Hong for century_n
		pCh->pCProfile->fCMicLitterSurf   += (float)0.1*pMa->pNFertilizer->fCorgManure;//Hong for century_n
		pCB->dCInputSurf += (double)pMa->pNFertilizer->fCorgManure; //Hong added on 20180731 for C-balance
		//pCB->dCInputCum += (double)pMa->pNFertilizer->fCorgManure;
		
//End of Hong

    //InitOrgDuenger(self);
    //InfiltrationOrgDuenger(self);

    S2 = xpn_time_get_formated_date(pTi->pSimTime->iyear, pTi->pSimTime->fTimeY, FALSE);
    S = g_strdup_printf("%s: Fertilization: %s, %d kg/ha NO3-N, %d kg/ha NH4-N, %d kg/ha Urea-N",
                        S2,
                        pMa->pNFertilizer->acName,
                        (int)pMa->pNFertilizer->fNO3N,
                        (int)pMa->pNFertilizer->fNH4N,
                        (int)pMa->pNFertilizer->fUreaN);
    PRINT_MESSAGE(xpn, 3, S);
    g_free(S);
    g_free(S2);

    // InfiltrationOrgDuengerBeregnung(exp_p);*/
	

    return RET_SUCCESS;
}

/*********************************************************************************/
/*  Name     : LAFertilizer                                                      */
/*                                                                               */
/*  Funktion : Die N-Zufuhr durch mineralische oder organische Duengemittel wird */
/*             nach Modellansaetzen von Godwin, die von Engel modifiziert wurden,*/
/*             integriert.                                                       */
/*                                                                               */
/*  Autor    :                                                                   */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*				pCh->pCLayer->fNO3N                                              */
/*				pCh->pCLayer->fNH4N                                              */
/*				pCh->pCLayer->fUreaN                                             */
/*				pCh->pCLayer->fNManure                                           */
/*				pCh->pCLayer->fCManure                                           */
/*				pCh->pCLayer->fManureCN                                          */
/*                                                                               */
/*********************************************************************************/
int schaaf_organic_fertilization(schaaf* self) //Hong = LAFertilizer()
{
    expertn_modul_base* xpn = &(self->parent);

    PMANAGEMENT pMa = xpn->pMa;
    PCHEMISTRY pCh = xpn->pCh;
    PTIME pTi = xpn->pTi;
	PCBALANCE	pCB = pCh->pCBalance; //Added by Hong on 20181016

    char acdummy3[80];
    double fumFacC;
    int iDummy;
    double fLitterPart, fHumusPart;
    char* lpOut3 = acdummy3;
    gchar* S, *S2;

    fumFacC = (double)0.4; // 40 % der org. Substanz im Duenger ist Kohlenstoff

    /*Faktor fumFacC wird in Abhaengugkeit von Duenger bestimmt. Bis
      der Duenger identifiziert werden kann standardmaessig auf 0.4 gesetzt.  */

    pMa->pNFertilizer->fCorgManure = pMa->pNFertilizer->fDryMatter * fumFacC;
    iDummy = 3 + strcspn(pMa->pNFertilizer->acCode, "LA:");
    strcpy(lpOut3, pMa->pNFertilizer->acCode);

    if (iDummy < (int)strlen(lpOut3)) {
        lpOut3 += iDummy;
        fLitterPart = (double)atof(lpOut3) / (double)100;
    } else
        fLitterPart = (double)0;

    fLitterPart = min((double)1, fLitterPart);
    fLitterPart = max((double)0, fLitterPart);
    iDummy = 3 + strcspn(lpOut3, "HA:");

    if (iDummy < (int)strlen(lpOut3)) {
        lpOut3 += iDummy;
        fHumusPart = (double)atof(lpOut3) / (double)100;
        lpOut3 -= iDummy;
    } else
        fHumusPart = (double)0;

    fHumusPart = min((double)1 - fLitterPart, fHumusPart);
    fHumusPart = max((double)0, fHumusPart);

    /* 25.06.01 auch unnnoetig?
    if (pMa->pNFertilizer->fNorgManure > EPSILON)
    {
    pMa->pNFertilizer->fOrgManureCN = pMa->pNFertilizer->fCorgManure / pMa->pNFertilizer->fNorgManure;
    }
    else
    pMa->pNFertilizer->fOrgManureCN = (double)0.1;

     * */ /*Uebertragen der Duengergroessen auf die Oberflaechenschicht.  */
    pCh->pCProfile->fNO3NSurf += pMa->pNFertilizer->fNO3N;
    pCh->pCProfile->fNH4NSurf += pMa->pNFertilizer->fNH4N;
    pCh->pCProfile->fUreaNSurf += pMa->pNFertilizer->fUreaN;

    pCh->pCProfile->fNManureSurf += ((double)1 - fLitterPart - fHumusPart) * pMa->pNFertilizer->fNorgManure;
    pCh->pCProfile->fCManureSurf += ((double)1 - fLitterPart - fHumusPart) * pMa->pNFertilizer->fCorgManure;

    pCh->pCProfile->fNLitterSurf += fLitterPart * pMa->pNFertilizer->fNorgManure;
    pCh->pCProfile->fCLitterSurf += fLitterPart * pMa->pNFertilizer->fCorgManure;

    pCh->pCProfile->fNHumusSurf += fHumusPart * pMa->pNFertilizer->fNorgManure;
    pCh->pCProfile->fCHumusSurf += fHumusPart * pMa->pNFertilizer->fCorgManure;

    pCh->pCProfile->fCLeafLitterSurf += fLitterPart * pMa->pNFertilizer->fCorgManure;//hong for century_n
	pCh->pCProfile->fCMicLitterSurf   += fHumusPart * pMa->pNFertilizer->fCorgManure;//hong for century_n
	pCB->dCInputSurf += (double)pMa->pNFertilizer->fCorgManure; //Hong added on 20181016 for C-balance
	//pCB->dCInputCum += (double)pMa->pNFertilizer->fCorgManure;

    /* 19.8.97 unnötig:
    if (pCh->pCProfile->fNManureSurf > EPSILON)
            {
            pCh->pCProfile->fCNManureSurf = pCh->pCProfile->fCManureSurf/ pCh->pCProfile->fNManureSurf;
            }
            else
                    pCh->pCProfile->fCNManureSurf = (double)0.1;
    if (pCh->pCProfile->fNLitterSurf > EPSILON)
            {
            pCh->pCProfile->fCNLitterSurf = pCh->pCProfile->fCLitterSurf/ pCh->pCProfile->fNLitterSurf;
            }
            else
                    pCh->pCProfile->fCNLitterSurf = (double)0.1;
    */

    // InfiltrationOrgDuengerBeregnung(exp_p);

    S2 = xpn_time_get_formated_date(pTi->pSimTime->iyear, pTi->pSimTime->fTimeY, FALSE);
    S = g_strdup_printf("%s: Fertilization: %s, Litter Part %.2f %%, Humus Part %.2f %%, Manure Part %.2f %%",
                        S2,
                        pMa->pNFertilizer->acName,
                        fLitterPart * 100.,
                        fHumusPart * 100.,
                        ((double)1 - fLitterPart - fHumusPart) * 100.);
    PRINT_MESSAGE(xpn, 3, S);
    g_free(S);
    g_free(S2);
	

    return RET_SUCCESS;
}


/*********************************************************************************/
/*  Name     : InfiltrationOrgDuenger                                                  */
/*                                                                               */
/*  Funktion : Am Ausbringungstag versickern die org. Duenger im Boden. Diese    */
/*             Infiltration in den Boden wird hier berechnet.                    */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 02.02.98                                                   */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
int InfiltrationOrgDuenger(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);
    PSPROFILE pSo = xpn->pSo;
    PWATER pWa = xpn->pWa;
    PMANAGEMENT pMa = xpn->pMa;
    PCHEMISTRY pCh = xpn->pCh;
    PHEAT pHe = xpn->pHe;
    PNFERTILIZER pFE = pMa->pNFertilizer;
	PTIME pTi = xpn->pTi;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20190801

    double dT= pTi->pTimeStep->fAct;//Hong
    double infNH4, inforgN, inforgC, infFeuchte;
    double feuchte;

    infNH4 = (double)0.0;
    inforgN = (double)0.0;
    inforgC = (double)0.0;
    infFeuchte = (double)0.0;

    /* Feuchtebedingungen fuer die Infiltration.*/
    infFeuchte = pWa->pWLayer->pNext->fContAct / pSo->pSWater->pNext->fContFK;

    /* Infiltration findet nut statt, wenn der Boden nicht gefroren ist.*/
    if ((pHe->pHLayer->pNext->fSoilTemp > (double)0.0)){
        /* Eine Infiltration wird fuer die Fraktionen Ammonium,
           org. N und org. Substanz berechnet. */

        if (!strncmp(pMa->pNFertilizer->acCode, "RE\0", 2)) { // organische Duengung
            infNH4 =
                pFE->afInfParam[0] * pCh->pCProfile->fNH4ManureSurf *
                (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens)) *
                (pFE->afInfParam[1] + (pFE->afInfParam[2] * (double)(log((double)pCh->pCProfile->fNH4ManureSurf))));
        }

        if ((!strcmp(pFE->acCode, "RE005\0")) || (!strcmp(pFE->acCode, "RE006\0")) ||
            (!strcmp(pFE->acCode, "RE007\0"))) {
            inforgN = (double)0.06 * pCh->pCProfile->fNManureSurf *
                      (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
            inforgC = (double)0.06 * pCh->pCProfile->fCManureSurf *
                      (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
        }

        if ((!strcmp(pFE->acCode, "RE008\0")) || (!strcmp(pFE->acCode, "RE009\0")) ||
            (!strcmp(pFE->acCode, "RE010\0")) || (!strcmp(pFE->acCode, "RE011\0"))) {
            inforgN = (double)0.15 * pCh->pCProfile->fNManureSurf *
                      (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
            inforgC = (double)0.15 * pCh->pCProfile->fCManureSurf *
                      (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
        }

        if ((!strcmp(pFE->acCode, "RE012\0")) || (!strcmp(pFE->acCode, "RE013\0")) ||
            (!strcmp(pFE->acCode, "RE014\0")) || (!strcmp(pFE->acCode, "RE025\0")) ||
            (!strcmp(pFE->acCode, "RE026\0"))) {
            inforgN = (double)0.2 * pCh->pCProfile->fNManureSurf *
                      (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
            inforgC = (double)0.2 * pCh->pCProfile->fCManureSurf *
                      (double)(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
        }

        if ((!strcmp(pFE->acCode, "RE015\0")) || (!strcmp(pFE->acCode, "RE016\0")) ||
            (!strcmp(pFE->acCode, "RE017\0")) || (!strcmp(pFE->acCode, "RE018\0")) ||
            (!strcmp(pFE->acCode, "RE019\0")) || (!strcmp(pFE->acCode, "RE020\0")) ||
            (!strcmp(pFE->acCode, "RE021\0")) || (!strcmp(pFE->acCode, "RE022\0")) ||
            (!strcmp(pFE->acCode, "RE027\0"))) {
            inforgN = (double)0.0;
            inforgC = (double)0.0;
        }

        /* Beruecksichtigung der Feuchtebedingungen */

        if ((infFeuchte > (double)1.2) && (infFeuchte < (double)0.4)) {
            infNH4 = infNH4 * (double)0.9;
        } else {
            feuchte = infFeuchte - (double)0.8;
            infNH4 = infNH4 * ((double)1.1 - ((double)1.25 * (double)pow((double)feuchte, (double)2)));
        }

        /* Bilanzierung */

//*dT added by Hong on 20171128	
       // pCh->pCProfile->fNH4ManureSurf -= infNH4;
        // pCh->pCLayer->pNext->fNH4Manure += infNH4;

        // pCh->pCProfile->fNManureSurf -= inforgN;
        // pCh->pCLayer->pNext->fNManure += inforgN;

        // pCh->pCProfile->fCManureSurf -= inforgC;
        // pCh->pCLayer->pNext->fCManure += inforgC;
			
		pCh->pCProfile->fNH4ManureSurf -= infNH4*dT;
        pCh->pCLayer->pNext->fNH4Manure += infNH4*dT;

        pCh->pCProfile->fNManureSurf -= inforgN*dT;
        pCh->pCLayer->pNext->fNManure += inforgN*dT;

        pCh->pCProfile->fCManureSurf -= inforgC*dT;
        pCh->pCLayer->pNext->fCManure += inforgC*dT;
		
		pCB->dCInputProfile += inforgC*dT; //Hong for C balance on 20190801
		pCB->dCInputProfile_30 += inforgC*dT;
		pCB->dNInputCum += inforgN*dT;
		//pCB->dNInputCum += (inforgN+infNH4)*dT;
		
		
        if (pCh->pCLayer->pNext->fNManure > (double)0.0) {
            pCh->pCLayer->pNext->fManureCN = pCh->pCLayer->pNext->fCManure / pCh->pCLayer->pNext->fNManure;

            pCh->pCProfile->fCNManureSurf = pCh->pCProfile->fCManureSurf / pCh->pCProfile->fNManureSurf;
        }
    }

    return RET_SUCCESS;
} /*===== Ende der Berechnung der Infiltration =============*/

/*********************************************************************************/
/*  Name     : InfiltrationOrgDuengerRegen                                       */
/*                                                                               */
/*  Funktion : Durch den Niederschlag wird der an der Oberflaeche aufliegnde     */
/*             organische Duenger in den Boden eingewaschen. Dies wird hier      */
/*             berechnet.                                                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 02.02.98                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
int InfiltrationOrgDuengerRegen(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);
    PMANAGEMENT pMa = xpn->pMa;
    PCHEMISTRY pCh = xpn->pCh;
    PCLIMATE pCl = xpn->pCl;
    PNFERTILIZER pFE = pMa->pNFertilizer;
	PTIME pTi = xpn->pTi;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20190801



    double dT;

    double sicknh4, sickorgN, sickorgC, wirkNied;
    dT = pTi->pTimeStep->fAct;

    // if (pCl->pWeather->fPreciRate > (double)5.0)
    if ((pCl->pWeather->fLiquPreciRate > (double)5.0))  {
        wirkNied = ((double)1.0 / (double)(sqrt((double)pCl->pWeather->fPreciRate + (double)0.75))) -
                   ((double)0.02 * (pCl->pWeather->fPreciRate - (double)0.25));
        // wirkNied = ((double)1.0/(double)(sqrt((double)pCl->pWeather->fLiquPreciRate + (double)0.75)))
        //					- ((double)0.02 * (pCl->pWeather->fLiquPreciRate - (double)0.25));
        if (wirkNied > (double)0.0)
            wirkNied = (double)0.0;

        if (pFE != NULL) {
            if ((!strcmp(pFE->acCode, "RE015\0")) || (!strcmp(pFE->acCode, "RE016\0")) ||
                (!strcmp(pFE->acCode, "RE017\0")) || (!strcmp(pFE->acCode, "RE018\0")) ||
                (!strcmp(pFE->acCode, "RE019\0")) || (!strcmp(pFE->acCode, "RE020\0")) ||
                (!strcmp(pFE->acCode, "RE021\0")) || (!strcmp(pFE->acCode, "RE022\0")) ||
                (!strcmp(pFE->acCode, "RE027\0"))) {
                if (pCh->pCProfile->fNH4ManureSurf > (double)0.0) {
                    sicknh4 = pCh->pCProfile->fNH4ManureSurf * ((double)1.0 - wirkNied);
                    //pCh->pCProfile->fNH4ManureSurf -= sicknh4; //Hong: incorrect!
                    //pCh->pCLayer->pNext->fNH4Manure += sicknh4;
					pCh->pCProfile->fNH4ManureSurf -= sicknh4*dT; //Hong changed on 20171128
                    pCh->pCLayer->pNext->fNH4Manure += sicknh4*dT;
					
                }

                if (pCh->pCProfile->fNManureSurf > (double)0.0) {
                    sickorgN = pCh->pCProfile->fNManureSurf * (double)0.004 * ((double)1.0 - wirkNied);
                    //pCh->pCProfile->fNManureSurf -= sickorgN;
                    //pCh->pCLayer->pNext->fNManure += sickorgN;
					pCh->pCProfile->fNManureSurf -= sickorgN*dT; //Hong changed on 20171128
                    pCh->pCLayer->pNext->fNManure += sickorgN*dT; //Hong changed on 20171128
					
                }

                if (pCh->pCProfile->fCManureSurf > (double)0.0) {
                    sickorgC = pCh->pCProfile->fCManureSurf * (double)0.004 * ((double)1.0 - wirkNied);									
					//pCh->pCProfile->fCManureSurf -= sickorgC;
                    //pCh->pCLayer->pNext->fCManure += sickorgC;
					pCh->pCProfile->fCManureSurf -= sickorgC*dT; //Hong changed on 20171128
                    pCh->pCLayer->pNext->fCManure += sickorgC*dT; //Hong changed on 20171128
					pCB->dCInputProfile += sickorgC*dT; //Hong for C balance on 20190801
		            pCB->dCInputProfile_30 += sickorgC*dT;
									
                }
            } 
			else {
                if (pCh->pCProfile->fNH4ManureSurf > (double)0.0) {
                    sicknh4 = pCh->pCProfile->fNH4ManureSurf * ((double)1.0 - wirkNied);
                    //pCh->pCProfile->fNH4ManureSurf -= sicknh4;
                    //	pCh->pCLayer->pNext->fNH4Manure     += sicknh4;
					pCh->pCProfile->fNH4ManureSurf -= sicknh4*dT; //Hong changed on 20171128
                		                
				}

                if (pCh->pCProfile->fNManureSurf > (double)0.0) {
                    sickorgN = pCh->pCProfile->fNManureSurf * (double)0.02 * ((double)1.0 - wirkNied);
                    //pCh->pCProfile->fNManureSurf -= sickorgN;
                    //pCh->pCLayer->pNext->fNManure += sickorgN;
					pCh->pCProfile->fNManureSurf -= sickorgN*dT; //Hong changed on 20171128
                    pCh->pCLayer->pNext->fNManure += sickorgN*dT; //Hong changed on 20171128
					
                }

                if (pCh->pCProfile->fCManureSurf > (double)0.0) {
                    sickorgC = pCh->pCProfile->fCManureSurf * (double)0.02 * ((double)1.0 - wirkNied);
                    //pCh->pCProfile->fCManureSurf -= sickorgC;
                    //pCh->pCLayer->pNext->fCManure += sickorgC;
					pCh->pCProfile->fCManureSurf -= sickorgC*dT; //Hong changed on 20171128
                    pCh->pCLayer->pNext->fCManure += sickorgC*dT; //Hong changed on 20171128
					pCB->dCInputProfile += sickorgC*dT; //Hong for C balance on 20190801
		            pCB->dCInputProfile_30 += sickorgC*dT;						
                }
            }
        }
    }
	
    return RET_SUCCESS;
} /*===== Ende der Berechnung der Infiltration durch Regen =============*/

/*********************************************************************************/
/*  Name     : NitrOrgNH4Hoff                                                    */
/*                                                                               */
/*  Funktion : Methode zur Berechnung der Freisetzung des Ammonium aus dem       */
/*             Pool der organische Duenger in den NH4-Boden-Pool. Modell         */
/*			   nach Hoffmann& Ritchie mit Anpassungen von Schaaf.                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 16.02.98                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*             pCL->fNH4Manure                                                   */
/*             pCL->fNH4N                                                        */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
int MinerOrgDuengHoff(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);

    PCLAYER pCL = xpn->pCh->pCLayer;
    PSWATER pSW = xpn->pSo->pSWater;
    PSLAYER pSL = xpn->pSo->pSLayer;
    PCHEMISTRY pCh = xpn->pCh;
    PSPROFILE pSo = xpn->pSo;
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
	PCBALANCE	pCB = pCh->pCBalance; //Added by Hong on 20190801

    double dT;

    double minorgN, minSurforgN, lai, freeOrgSub, freeOrgC;
    /* Berechnung der Mineralisation des organischen Stickstoffs,
                                   - der nach Duengung an der Bodenoberflaeche liegt,
                                   - der durch Versickerung und/oder Bearbeitung in den
                                     Bodenpool ueberfuehrt wurde.
                     Dies muss getrennt berechnet werden, da andere Bedingungen an
                     der Oberflaeche und im Boden zugrunde gelegt werden.*/

    dT = pTi->pTimeStep->fAct;

    // ReduktionsFunktionen(exp_p);

    minSurforgN = (double)0.0;
    minorgN = (double)0.0;
    freeOrgSub = (double)0.0;
    freeOrgC = (double)0.0;

    if (pPl != NULL) {
        lai = pPl->pCanopy->fLAI;
    } else {
        lai = (double)0.0;
    }

    /*  Berechnung der Mineralisation aus dem an der Bodenoberflaeche
        liegenden org. Stickstoffs. Methode wird erst gerechnet wenn erste
        org. Duengung erfolgt ist. */
    if (pCh->pCProfile->fNManureSurf > (double)0.0) {
        minSurforgN = (double)0.035 * ((double)1.2 - ((double)exp((double)-0.3 * (double)lai))) *
                      pCh->pCProfile->fNManureSurf * pCh->pCLayer->pNext->fNH4NitrCoeff *
                      pCh->pCLayer->pNext->fTempCoeff;

        /*  Die berechnete mineralisierte org. N-Menge wird zu 35% dem NH4-Pool
            der Oberflaeche, zu 35%  dem NH4-Pool des allgemeinen Ammoniumpools
            der ersten Bodenschicht und zu 30% dem Humuspool der ersten Bodenschicht
                zugeordnet. Neben der mineralisierten org. N-Menge muss auch die organische
                Substanz aus dem organischen Duenger vermindert werden, da bei der
                Freisetzung des Stickstoffes Kohlenstoff veratmet wird.  */

        pCh->pCProfile->fNH4NSurf += (double)0.35 * minSurforgN * dT;
        pCh->pCLayer->pNext->fNH4N += (double)0.35 * minSurforgN * dT;
        pSo->pSLayer->pNext->fNHumus += (double)0.3 * minSurforgN * dT;

        pCh->pCLayer->pNext->fManureMinerR += (double)0.35 * minSurforgN * dT; // ep 120999 zur Bilanzierung?

        if (pCh->pCProfile->fDryMatterManureSurf > (double)0.0) {
            freeOrgSub = pCh->pCProfile->fDryMatterManureSurf / (pCh->pCProfile->fNManureSurf / minSurforgN);

            pCh->pCProfile->fDryMatterManureSurf -= freeOrgSub * dT;
            pSo->pSLayer->pNext->fHumus += freeOrgSub * dT;
        }
    }
    if (pCh->pCProfile->fCManureSurf > (double)0.0) {
        freeOrgC = pCh->pCProfile->fCManureSurf / (pCh->pCProfile->fNManureSurf / minSurforgN);

        pCh->pCProfile->fCManureSurf -= freeOrgC * dT;
        pSo->pSLayer->pNext->fCHumus += freeOrgC * dT;
		pCB->dCInputProfile += freeOrgC * dT; //Hong added on 20190801 for C-balance
	    pCB->dCInputProfile_30 += freeOrgC * dT;
					
    }

    /*  Der organische Stickstoff an der Bodenoberflaeche
            unterliegt der Versickerung in den Boden oder wird durch
            eine Bodenbearbeitung eingearbeitet. der im Boden be-
            findliche Pool unterliegt folgender Mineralisation. Methode
            wird erst gerechnet wenn erste org. Duengung erfolgt ist. */

    ///* //sa/gsf folgendes ist in der Bilanzierung nicht erfasst,
    // wird deshalb weggelassen
    for (pCL = pCh->pCLayer->pNext, pSW = pSo->pSWater->pNext, pSL = pSo->pSLayer->pNext;
         ((pCL->pNext != NULL) && (pSW->pNext != NULL) && (pSL->pNext != NULL));
         (pCL = pCL->pNext, pSW = pSW->pNext, pSL = pSL->pNext)) {
        minorgN = (double)0.035 * pCL->fNManure * pCL->fNH4NitrCoeff * pCL->fTempCoeff;

        pCL->fNManure -= minorgN * dT;
        pCL->fNH4N += minorgN * (double)0.7 * dT;
        pSL->fNHumus += minorgN * (double)0.3 * dT;

        pCL->fManureMinerR += minorgN * (double)0.7 * dT; // ep 120999 zur Bilanzierung?
        pCL->fNImmobR += minorgN * (double)0.3 * dT; // ep 120999 zur Bilanzierung?

        if (minorgN > (double)0.0) {
            freeOrgC = pCL->fCManure / (pCL->fNManure / minorgN);
            pCL->fCManure -= freeOrgC * dT;						
            pSL->fCHumus += freeOrgC * dT;
			
        }
    }

    //*/ //sa/gsf
    return RET_SUCCESS;
} /*  Ende Berechnung der Mineralisation org. N aus org. Duenger*/

/*********************************************************************************/
/*  Name     : NitrOrgNH4Hoff                                                    */
/*                                                                               */
/*  Funktion : Methode zur Berechnung der Freisetzung des Ammonium aus dem       */
/*             Pool der organische Duenger in den NH4-Boden-Pool. Modell         */
/*			   nach Hoffmann& Ritchie mit Anpassungen von Schaaf.                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 16.02.98                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*             pCL->fNH4Manure                                                   */
/*             pCL->fNH4N                                                        */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/

int NitrOrgNH4Hoff(schaaf* self)
{

    expertn_modul_base* xpn = &(self->parent);

    double minRate;

    PCLAYER pCL = xpn->pCh->pCLayer;
    PSWATER pSW = xpn->pSo->pSWater;
    PSLAYER pSL = xpn->pSo->pSLayer;
    PTIME pTi = xpn->pTi;
	double dT = pTi->pTimeStep->fAct;

    for (pCL = pCL->pNext, pSW = pSW->pNext, pSL = pSL->pNext;
         ((pCL->pNext != NULL) && (pSW->pNext != NULL) && (pSL->pNext != NULL));
         pCL = pCL->pNext, pSW = pSW->pNext, pSL = pSL->pNext) {
        if (xpn->pCh->pCLayer->fNH4Manure != 0.0)
            printf("%f \n", xpn->pCh->pCLayer->fNH4Manure);
        minRate = (double)0.075 * xpn->pCh->pCLayer->fNH4Manure * xpn->pCh->pCLayer->fNH4NitrCoeff *
                  xpn->pCh->pCLayer->fTempCoeff;
        xpn->pCh->pCLayer->fNH4Manure -= minRate * dT;
        xpn->pCh->pCLayer->fNH4N += minRate * dT;
    }

    return RET_SUCCESS;
} /*  Ende Berechnung der Nitrifikation des Ammoniums aus org. Duenger*/

/*********************************************************************************/
/*  Name     : InfiltrationOrgDuengerRegen                                       */
/*                                                                               */
/*  Funktion : Durch den Niederschlag wird der an der Oberflaeche aufliegnde     */
/*             organische Duenger in den Boden eingewaschen. Dies wird hier      */
/*             berechnet.                                                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 02.02.98                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int InfiltrationOrgDuengerBeregnung(EXP_POINTER)
{

    PMIRRIGATION pIR = pMa->pIrrigation;
	PCBALANCE	    pCB = pCh->pCBalance; //Added by Hong on 20190801

    double sicknh4, sickorgN, sickorgC, wirkNied;
    ;

    if ((pCl->pWeather->fLiquPreciRate > (double)5.0) && (pIR != NULL)) {
        wirkNied = ((double)1.0 / (double)(sqrt((double)pIR->fAmount + (double)0.75))) -
                   ((double)0.02 * (pIR->fAmount - (double)0.25));
        if (wirkNied > (double)0.0)
            wirkNied = (double)0.0;

        {
            if (pCh->pCProfile->fNH4ManureSurf > (double)0.0) {
                sicknh4 = pCh->pCProfile->fNH4ManureSurf * ((double)1.0 - wirkNied);
                pCh->pCProfile->fNH4ManureSurf -= sicknh4;
                pCh->pCLayer->pNext->fNH4Manure += sicknh4;
            }

            if (pCh->pCProfile->fNManureSurf > (double)0.0) {
                sickorgN = pCh->pCProfile->fNManureSurf * (double)0.02 * ((double)1.0 - wirkNied);
                pCh->pCProfile->fNManureSurf -= sickorgN;
                pCh->pCLayer->pNext->fNManure += sickorgN;
            }

            if (pCh->pCProfile->fCManureSurf > (double)0.0) {
                sickorgC = pCh->pCProfile->fCManureSurf * (double)0.02 * ((double)1.0 - wirkNied);
                pCh->pCProfile->fCManureSurf -= sickorgC;
                pCh->pCLayer->pNext->fCManure += sickorgC;
				pCB->dCInputProfile += sickorgC; //Hong for C balance on 20190801
				pCB->dCInputProfile_30 += sickorgC;
				
            }
        }
    }

    return 1;
} /*===== Ende der Berechnung der Infiltration durch Beregnung =============*/

int read_fertilizers(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);
    PMANAGEMENT pMa = xpn->pMa;
    PNFERTILIZER fertil_first, fertil, fertil_act;

    GKeyFile* keyfile;
    GKeyFileFlags flags;
    GError* error = NULL;
    GDate* date;
    const char* filename;
    char* ini_filename;

    int fertilizer_count, i, start_i, j;

    int fertil_start_year, fertil_start_mon, fertil_start_day;

    // Read from INI File:
    ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list, "Config.schaaf.filename");

    if (ini_filename == NULL) {
        PRINT_ERROR("Missing entry 'schaaf.filename' in your configuration. If present, fertilization from database "
                    "will be used.");
        return 0;
    } else {

        char* S2;
        S2 = expertn_modul_base_replace_std_templates(xpn, ini_filename);
        if (S2 != NULL) {
            ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);

            /* Create a new GKeyFile object and a bitwise list of flags. */
            keyfile = g_key_file_new();

            flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

            /* Load the GKeyFile from keyfile.conf or return. */
            if (!g_key_file_load_from_file(keyfile, ini_filename, flags, &error)) {
                PRINT_ERROR("Error Read SCHAAF INI File");
                return -1;
            }

            filename = ini_filename;

            // Read mineral fertilization
            GET_INI_STRING_ARRAY_OPTIONAL(self->min_date, self->min_date_len, 1, "9999-09-09", "mineral", "date");
            GET_INI_STRING_ARRAY_OPTIONAL(
                self->min_fertilizer, self->min_fertilizer_len, 1, "no fertilizer", "mineral", "fertilizer");
            GET_INI_STRING_ARRAY_OPTIONAL(self->min_code, self->min_code_len, 1, "no fertilizer", "mineral", "code");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->min_n_tot, self->min_n_tot_len, 1, 0.0, "mineral", "n_tot_min");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->min_no3n, self->min_no3n_len, 1, 0.0, "mineral", "no3n");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->min_nh4n, self->min_nh4n_len, 1, 0.0, "mineral", "nh4n");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->min_urea, self->min_urea_len, 1, 0.0, "mineral", "urea");

            CHECK_LEN(self->min_date_len, self->min_fertilizer_len);
            CHECK_LEN(self->min_date_len, self->min_code_len);
            CHECK_LEN(self->min_date_len, self->min_n_tot_len);
            CHECK_LEN(self->min_date_len, self->min_no3n_len);
            CHECK_LEN(self->min_date_len, self->min_nh4n_len);
            CHECK_LEN(self->min_date_len, self->min_urea_len);

            // Read organic fertilization
            GET_INI_STRING_ARRAY_OPTIONAL(self->org_date, self->org_date_len, 1, "9999-09-09", "organic", "date");
            GET_INI_STRING_ARRAY_OPTIONAL(
                self->org_fertilizer, self->org_fertilizer_len, 1, "no fertilizer", "organic", "fertilizer");
            GET_INI_STRING_ARRAY_OPTIONAL(self->org_code, self->org_code_len, 1, "no fertilizer", "organic", "code");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->org_amount, self->org_amount_len, 1, 0.0, "organic", "amount");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(
                self->org_dry_matter, self->org_dry_matter_len, 1, 0.0, "organic", "dry_matter");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->org_subst, self->org_subst_len, 1, 0.0, "organic", "org_subst");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->org_n_tot, self->org_n_tot_len, 1, 0.0, "organic", "n_tot_org");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->org_nh4n, self->org_nh4n_len, 1, 0.0, "organic", "nh4n");

            CHECK_LEN(self->org_date_len, self->org_fertilizer_len);
            CHECK_LEN(self->org_date_len, self->org_code_len);
            CHECK_LEN(self->org_date_len, self->org_amount_len);
            CHECK_LEN(self->org_date_len, self->org_dry_matter_len);
            CHECK_LEN(self->org_date_len, self->org_subst_len);
            CHECK_LEN(self->org_date_len, self->org_n_tot_len);
            CHECK_LEN(self->org_date_len, self->org_nh4n_len);

            if ((strcmp(self->min_fertilizer[0], "no fertilizer") != 0) &&
                strcmp(self->org_fertilizer[0], "no fertilizer") != 0)
                fertilizer_count = self->min_date_len + self->org_date_len;
            else if ((strcmp(self->min_fertilizer[0], "no fertilizer") != 0) &&
                     strcmp(self->org_fertilizer[0], "no fertilizer") == 0)
                fertilizer_count = self->min_date_len + 1;
            else if ((strcmp(self->min_fertilizer[0], "no fertilizer") == 0) &&
                     strcmp(self->org_fertilizer[0], "no fertilizer") != 0)
                fertilizer_count = self->org_date_len + 1;
            else
                fertilizer_count = 2;

            // Merge fertilizations
            // 1. Allocate memory
            self->ges_date = g_malloc0(sizeof(char*) * fertilizer_count);
            self->ges_fertilizer = g_malloc0(sizeof(char*) * fertilizer_count);
            self->ges_code = g_malloc0(sizeof(char*) * fertilizer_count);
            self->ges_n_tot = g_malloc0(sizeof(double) * fertilizer_count);
			self->ges_n_org_tot = g_malloc0(sizeof(double) * fertilizer_count);
            self->ges_no3n = g_malloc0(sizeof(double) * fertilizer_count);
            self->ges_nh4n = g_malloc0(sizeof(double) * fertilizer_count);
            self->ges_urea = g_malloc0(sizeof(double) * fertilizer_count);
            self->ges_dry_matter = g_malloc0(sizeof(double) * fertilizer_count);
            self->ges_subst = g_malloc0(sizeof(double) * fertilizer_count);
            self->ges_amount = g_malloc0(sizeof(double) * fertilizer_count);
            // 2.  Add mineral and organic fertilizations to merged arrays and remove spaces
            for (i = 0; i < fertilizer_count; i++) {
                if (i < self->min_date_len) {
                    deleteSpaceBegEnd(self->min_date[i]);
                    self->ges_date[i] = g_strdup_printf("%s", self->min_date[i]);
                    deleteSpaceBegEnd(self->min_fertilizer[i]);
                    self->ges_fertilizer[i] = g_strdup_printf("%s", self->min_fertilizer[i]);
                    deleteSpaceBegEnd(self->min_code[i]);
                    self->ges_code[i] = g_strdup_printf("%s", self->min_code[i]);
                    self->ges_n_tot[i] = self->min_n_tot[i];
                    self->ges_no3n[i] = self->min_no3n[i];
                    self->ges_nh4n[i] = self->min_nh4n[i];
                    self->ges_urea[i] = self->min_urea[i];
                } else {
                    j = i - self->min_date_len;
                    deleteSpaceBegEnd(self->org_date[j]);
                    self->ges_date[i] = g_strdup_printf("%s", self->org_date[j]);
                    deleteSpaceBegEnd(self->org_fertilizer[j]);
                    self->ges_fertilizer[i] = g_strdup_printf("%s", self->org_fertilizer[j]);
                    deleteSpaceBegEnd(self->org_code[j]);
                    self->ges_code[i] = self->org_code[j];
                    self->ges_n_org_tot[i] = self->org_n_tot[j];
                    self->ges_nh4n[i] = self->org_nh4n[j];
                    self->ges_dry_matter[i] = self->org_dry_matter[j];
                    self->ges_subst[i] = self->org_subst[j];
                    self->ges_amount[i] = self->org_amount[j];
                }
            }

            // Order dates chronologically
            // Set first date for comparison
            date = convert_str_to_gdate(self->min_date[0]);
            fertil_start_day = g_date_get_day(date);
            fertil_start_mon = g_date_get_month(date);
            fertil_start_year = g_date_get_year(date);

            start_i = 0;
            for (i = start_i; i < fertilizer_count; i++) {
                g_date_free(date);
                date = convert_str_to_gdate(self->ges_date[i]);
                if (xpn_time_compare_date(fertil_start_year,
                                          fertil_start_mon,
                                          fertil_start_day,
                                          g_date_get_year(date),
                                          g_date_get_month(date),
                                          g_date_get_day(date)) > 0) {
                    // Permutation of the data
                    self->ges_date_zwischen = self->ges_date[start_i];
                    self->ges_fertilizer_zwischen = self->ges_fertilizer[start_i];
                    self->ges_code_zwischen = self->ges_code[start_i];
                    self->ges_n_tot_zwischen = self->ges_n_tot[start_i];
					self->ges_n_org_tot_zwischen = self->ges_n_org_tot[start_i];//changed by EP and Hong on 20180702: es war self->ges_n_org_tot_zwischen = self->ges_n_tot[start_i] 
                    self->ges_no3n_zwischen = self->ges_no3n[start_i];
                    self->ges_nh4n_zwischen = self->ges_nh4n[start_i];
                    self->ges_urea_zwischen = self->ges_urea[start_i];
                    self->ges_dry_matter_zwischen = self->ges_dry_matter[start_i];
                    self->ges_subst_zwischen = self->ges_subst[start_i];
                    self->ges_amount_zwischen = self->ges_amount[start_i];

                    self->ges_date[start_i] = self->ges_date[i];
                    self->ges_fertilizer[start_i] = self->ges_fertilizer[i];
                    self->ges_code[start_i] = self->ges_code[i];
                    self->ges_n_tot[start_i] = self->ges_n_tot[i];
					self->ges_n_org_tot[start_i]  = self->ges_n_org_tot[i];
                    self->ges_no3n[start_i] = self->ges_no3n[i];
                    self->ges_nh4n[start_i] = self->ges_nh4n[i];
                    self->ges_urea[start_i] = self->ges_urea[i];
                    self->ges_dry_matter[start_i] = self->ges_dry_matter[i];
                    self->ges_subst[start_i] = self->ges_subst[i];
                    self->ges_amount[start_i] = self->ges_amount[i];

                    self->ges_date[i] = g_strdup_printf("%s", self->ges_date_zwischen);
                    self->ges_fertilizer[i] = g_strdup_printf("%s", self->ges_fertilizer_zwischen);
                    self->ges_code[i] = g_strdup_printf("%s", self->ges_code_zwischen);
                    self->ges_n_tot[i] = self->ges_n_tot_zwischen;
					self->ges_n_org_tot[i] = self->ges_n_org_tot_zwischen;
                    self->ges_no3n[i] = self->ges_no3n_zwischen;
                    self->ges_nh4n[i] = self->ges_nh4n_zwischen;
                    self->ges_urea[i] = self->ges_urea_zwischen;
                    self->ges_dry_matter[i] = self->ges_dry_matter_zwischen;
                    self->ges_subst[i] = self->ges_subst_zwischen;
                    self->ges_amount[i] = self->ges_amount_zwischen;

                    // Set new start date for permutation
                    g_date_free(date);
                    date = convert_str_to_gdate(self->ges_date[start_i]);
                    fertil_start_day = g_date_get_day(date);
                    fertil_start_mon = g_date_get_month(date);
                    fertil_start_year = g_date_get_year(date);

                    i = start_i;

                    continue;
                }

                // If date[start_i] does not need permutation anymore, test date[start_i+1]
                if (i == fertilizer_count - 1) {
                    start_i += 1;
                    i = start_i;
                    g_date_free(date);
                    date = convert_str_to_gdate(self->ges_date[start_i]);
                    fertil_start_day = g_date_get_day(date);
                    fertil_start_mon = g_date_get_month(date);
                    fertil_start_year = g_date_get_year(date);
                    continue;
                }

                // Exit loop if the last array members are reached
                if (start_i == fertilizer_count - 1)
                    break;
            }

            fertil_first = NULL;
            fertil_act = NULL;
			
			//Added by Hong on 20190513
			//Get Start and Stop Year
			PSIMTIME		pST = xpn->pTi->pSimTime;
			STRUCT_XPN_TIME str_time;
			int 			startyear,startmon,startday,stopyear,stopmon,stopday;
			int j;
			xpn_time_get_struc(&str_time, pST->iStart_year, pST->fStart_TimeY);
	        startyear = str_time.year;
		    startmon =  str_time.mon;
	        startday =  str_time.mday;
	        xpn_time_get_struc(&str_time, pST->iStop_year, pST->fStop_TimeY);
	        stopyear = str_time.year;
			stopmon =  str_time.mon;
	        stopday =  str_time.mday;
			j=0.0;
            //End of Hong

            for (i = 0; i < fertilizer_count; i++) {
                // Struktur mit 0 Initialisieren
                fertil = g_malloc0_n(1, sizeof(STNFERTILIZER));
                fertil->acName = self->ges_fertilizer[i];
                g_date_free(date);
                date = convert_str_to_gdate(self->ges_date[i]);
                fertil->Day = g_date_get_day(date);
                fertil->Month = g_date_get_month(date);
                fertil->Year = g_date_get_year(date);
                fertil->fTotalN = self->ges_n_tot[i];
                fertil->fNO3N = self->ges_no3n[i];
                fertil->fNH4N = self->ges_nh4n[i];
                fertil->fUreaN = self->ges_urea[i];
                fertil->acCode = self->ges_code[i];
                fertil->fDryMatter = self->ges_dry_matter[i];
                fertil->fNorgManure = self->ges_n_org_tot[i];
				
				//Test of Hong
				InitOrgDuenger(fertil);
				//End of TEst
				
				//added by Hong on 20190513
				if (xpn_time_compare_date(startyear,
				                          startmon,
										  startday,
                                         fertil->Year,
                                         fertil->Month,
                                         fertil->Day) < 0)
				
				{
					if (j == 0) {
                    fertil_first = fertil;
                    fertil_act = fertil;
                } else {
                    fertil_act->pNext = fertil;
                    fertil->pBack = fertil_act;
                    fertil_act = fertil_act->pNext;
				}
				j++;
				}
				
				if (xpn_time_compare_date(stopyear,
				                          stopmon,
										  stopday,
                                         fertil->Year,
                                         fertil->Month,
                                         fertil->Day) < 0)
											 break;
				
            /*    if (i == 0) {
                    fertil_first = fertil;
                    fertil_act = fertil;
                } else {
                    fertil_act->pNext = fertil;
                    fertil->pBack = fertil_act;
                    fertil_act = fertil_act->pNext;
                }*/
				//End of Hong
            }
            if (fertilizer_count != 0) {
                // Letzter hat -1 als Wert
                fertil = g_malloc0_n(1, sizeof(STNFERTILIZER));
                fertil->Day = 0;
                fertil->Month = 0;
                fertil->Year = 0;
                fertil_act->pNext = fertil;
                fertil->pBack = fertil_act;
                fertil_act = fertil_act->pNext;
            }

            pMa->pNFertilizer = fertil_first;
            //self->fertil_done = 0;
			self->first_fertil_done = 0;// changed by Florian on 20171220

            self->ini_filename = g_strdup_printf("%s", ini_filename);
            free(ini_filename);
            free(S2);
            g_key_file_free(keyfile);

            // Free the variables
            g_free(self->min_date);
            g_free(self->min_fertilizer);
            g_free(self->min_code);
            g_free(self->org_date);
            g_free(self->org_fertilizer);
            g_free(self->org_code);
            g_free(self->ges_date);
            g_free(self->ges_fertilizer);
            g_free(self->ges_code);
            g_free(self->min_n_tot);
            g_free(self->min_no3n);
            g_free(self->min_nh4n);
            g_free(self->min_urea);
            g_free(self->org_amount);
            g_free(self->org_dry_matter);
            g_free(self->org_subst);
            g_free(self->org_n_tot);
            g_free(self->org_nh4n);
            g_free(self->ges_n_tot);
            g_free(self->ges_no3n);
            g_free(self->ges_nh4n);
            g_free(self->ges_urea);
            g_free(self->ges_amount);
            g_free(self->ges_dry_matter);
            g_free(self->ges_subst);
            g_free(self->ges_date_zwischen);
            g_free(self->ges_fertilizer_zwischen);
            g_free(self->ges_code_zwischen);

            g_date_free(date);
            if (error != NULL)
                g_error_free(error);
        }

        // Auf das neueste Datum stellen:

        {
            PNFERTILIZER pNF = xpn->pMa->pNFertilizer;

            if ((pNF != NULL) && (pNF->Day > 0)) {
                while ((pNF == NULL) || ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                         xpn->pTi->pSimTime->mon,
                                         xpn->pTi->pSimTime->mday,
                                         pNF->Year,
                                         pNF->Month,
                                         pNF->Day) > 0)&& (xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                         xpn->pTi->pSimTime->mon,
                                         xpn->pTi->pSimTime->mday,
                                         pNF->pNext->Year,
                                         pNF->pNext->Month,
                                         pNF->pNext->Day) < 0))) {
                    xpn->pMa->pNFertilizer = xpn->pMa->pNFertilizer->pNext;
                    pNF = xpn->pMa->pNFertilizer;
                }
            }
        }
    }

    return RET_SUCCESS;
}

int schaaf_irrigation(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);
    PTIME pTi = xpn->pTi;
    PMANAGEMENT pMa = xpn->pMa;
    PMIRRIGATION pIR;
    char* S;

    if (NewDay(pTi)) {
        self->irr_actual = NULL;
        for (pIR = pMa->pIrrigation; (pIR->pNext != NULL); pIR = pIR->pNext) {
            if ((pTi->pSimTime->mday == pIR->Day) && (pTi->pSimTime->mon == pIR->Month) &&
                (pTi->pSimTime->year == pIR->Year)) {
                self->irr_actual = pIR;
                S = g_strdup_printf("%d-%d-%d: Irrigation: %0.2f l", pIR->Year, pIR->Month, pIR->Day, pIR->fAmount);
                PRINT_MESSAGE(xpn, 3, S);
                g_free(S);
				

                break;
            }
        }
    }

    if (self->irr_actual != NULL) {
        xpn->pCl->pWeather->fPreciRate += self->irr_actual->fAmount;
        xpn->pCl->pWeather->fLiquPreciRate += self->irr_actual->fAmount;
    }


   
    return RET_SUCCESS;
}

int read_irrigation(schaaf* self)
{
    expertn_modul_base* xpn = &(self->parent);

    PMIRRIGATION irr_first, irr, irr_act;
    PMANAGEMENT pMa = xpn->pMa;

    GKeyFile* keyfile;
    GKeyFileFlags flags;
    GError* error = NULL;
    const char* filename;
    char* ini_filename;
    int i;

    self->irr_date_len = 0;
    self->irr_actual = NULL;

    // Read from INI File:
    ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list, "Config.schaaf.filename");

    if (ini_filename == NULL) {
        PRINT_ERROR("Missing entry 'schaaf.filename' in your configuration. If present, fertilization from database "
                    "will be used.");
        return 0;
    } else {

        char* S2;
        S2 = expertn_modul_base_replace_std_templates(xpn, ini_filename);
        if (S2 != NULL) {
            ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);

            /* Create a new GKeyFile object and a bitwise list of flags. */
            keyfile = g_key_file_new();

            flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

            /* Load the GKeyFile from keyfile.conf or return. */
            if (!g_key_file_load_from_file(keyfile, ini_filename, flags, &error)) {
                PRINT_ERROR("Error Read SCHAAF INI File");
                return -1;
            }

            filename = ini_filename;

            GET_INI_STRING_ARRAY_OPTIONAL(self->irr_date, self->irr_date_len, 1, "9999-09-09", "irrigation", "date");
            GET_INI_DOUBLE_ARRAY_OPTIONAL(self->irr_amount, self->irr_amount_len, 1, 0.0, "irrigation", "amount");

            self->irr_gdate = g_malloc0(sizeof(GDate*) * self->irr_date_len);
            for (i = 0; i < self->irr_date_len; i++) {
                deleteSpaceBegEnd(self->irr_date[i]);
                self->irr_gdate[i] = convert_str_to_gdate(self->irr_date[i]);
            }

            CHECK_LEN(self->irr_date_len, self->irr_amount_len);

            // man könnte das jetzt noch in die XPN struktur schreiben ... oder auch nicht

            for (i = 0; i < self->irr_date_len; i++) {
                irr = g_malloc0_n(1, sizeof(STIRRIGATION));
                irr->Day = g_date_get_day(self->irr_gdate[i]);
                irr->Month = g_date_get_month(self->irr_gdate[i]);
                irr->Year = g_date_get_year(self->irr_gdate[i]);
                irr->fAmount = self->irr_amount[i];
                if (i == 0) {
                    irr_first = irr;
                    irr_act = irr;
                } else {
                    irr_act->pNext = irr;
                    irr->pBack = irr_act;
                    irr_act = irr_act->pNext;
                }
            }
            if (self->irr_date_len != 0) {
                irr = g_malloc0_n(1, sizeof(STIRRIGATION));
                irr->Day = 0;
                irr->Month = 0;
                irr->Year = 0;
                irr_act->pNext = irr;
                irr->pBack = irr_act;
                irr_act = irr_act->pNext;
            }
            pMa->pIrrigation = irr_first;

            g_free(self->irr_date);
            g_free(self->irr_amount);

            self->ini_filename = g_strdup_printf("%s", ini_filename);
            free(ini_filename);
            free(S2);
            g_key_file_free(keyfile);
        }
    }

    return RET_SUCCESS;
}

//Test of hong
/*********************************************************************************/
/*  Name     : InitOrgDuenger                                                    */
/*                                                                               */
/*  Funktion : Fuer die organischen Duengemittel werden Parameter benoetigt      */
/*             um deren Infiltration in den Boden zu berechnen. Diese werden     */
/*             hier auf die entsprechenden Variablen geschrieben.                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pFE->afInfParam[0]                                                */
/*             pFE->afInfParam[1]                                                */
/*             pFE->afInfParam[2]                                                */
/*                                                                               */
/*********************************************************************************/
int InitOrgDuenger(PNFERTILIZER fertil)
{
    //expertn_modul_base* xpn = &(self->parent);
    //PMANAGEMENT pMa = xpn->pMa;
    //PNFERTILIZER pFE = fertil;

    // pFE->acCode korrekt voruebergehend acName
    if (!strcmp(fertil->acCode, "RE005\0")) {
        // Rindergülle 5% TS
        fertil->afInfParam[0] = (double)0.23;
        fertil->afInfParam[1] = (double)-0.25;
        fertil->afInfParam[2] = (double)0.50;
    }
    else if (!strcmp(fertil->acCode, "RE006\0")) {
        // Rindergülle 10% TS
        fertil->afInfParam[0] = (double)0.43;
        fertil->afInfParam[1] = (double)0.46;
        fertil->afInfParam[2] = (double)0.185;
    }
    else if (!strcmp(fertil->acCode, "RE007\0")) {
        // Rindergülle 15% TS
        fertil->afInfParam[0] = (double)0.53;
        fertil->afInfParam[1] = (double)0.56;
        fertil->afInfParam[2] = (double)0.185;
    }
    else if (!strcmp(fertil->acCode, "RE008\0")) {
        // Scheineguelle 3.5% TS
        fertil->afInfParam[0] = (double)0.33;
        fertil->afInfParam[1] = (double)-0.15;
        fertil->afInfParam[2] = (double)0.38;
    }
    else if (!strcmp(fertil->acCode, "RE009\0")) {
        // Scheineguelle 7% TS
        fertil->afInfParam[0] = (double)0.43;
        fertil->afInfParam[1] = (double)0.22;
        fertil->afInfParam[2] = (double)0.135;
    }
    else if (!strcmp(fertil->acCode, "RE010\0")) {
        // Scheineguelle 10.5% TS
        fertil->afInfParam[0] = (double)0.50;
        fertil->afInfParam[1] = (double)0.35;
        fertil->afInfParam[2] = (double)0.155;
    }
    else if (!strcmp(fertil->acCode, "RE011\0")) {
        // Scheineguelle 14% TS
        fertil->afInfParam[0] = (double)0.6;
        fertil->afInfParam[1] = (double)0.55;
        fertil->afInfParam[2] = (double)0.185;
    }
    else if (!strcmp(fertil->acCode, "RE012\0")) {
        // Gefluegelguelle 7% TS
        fertil->afInfParam[0] = (double)0.15;
        fertil->afInfParam[1] = (double)-0.15;
        fertil->afInfParam[2] = (double)0.38;
    }
    else if (!strcmp(fertil->acCode, "RE013\0")) {
        // Gefluegelguelle 14% TS
        fertil->afInfParam[0] = (double)0.35;
        fertil->afInfParam[1] = (double)0.35;
        fertil->afInfParam[2] = (double)0.125;
    }
    else if (!strcmp(fertil->acCode, "RE014\0")) {
        // Gefluegelguelle 21% TS
        fertil->afInfParam[0] = (double)0.55;
        fertil->afInfParam[1] = (double)0.55;
        fertil->afInfParam[2] = (double)0.185;
    }
    else if (!strcmp(fertil->acCode, "RE015\0")) {
        // Rindermist
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE016\0")) {
        // Schweinemist
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE017\0")) {
        // Haehnchenmist
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE018\0")) {
        // Putenmist
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE019\0")) {
        // Pferdemist
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE020\0")) {
        // Schafmist
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE021\0")) {
        // Huehnerfrischkot
        fertil->afInfParam[0] = (double)0.1;
        fertil->afInfParam[1] = (double)-0.1;
        fertil->afInfParam[2] = (double)0.3;
    }
    else if (!strcmp(fertil->acCode, "RE022\0")) {
        // Huehnertrockenkot
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    else if (!strcmp(fertil->acCode, "RE023\0")) {
        // Rinderjauche
        fertil->afInfParam[0] = (double)0.85;
        fertil->afInfParam[1] = (double)0.65;
        fertil->afInfParam[2] = (double)0.10;
    }
    else if (!strcmp(fertil->acCode, "RE024\0")) {
        // Schweinjauche
        fertil->afInfParam[0] = (double)0.23;
        fertil->afInfParam[1] = (double)-0.25;
        fertil->afInfParam[2] = (double)0.50;
    }
    else if (!strcmp(fertil->acCode, "RE025\0")) {
        // Klaerschlamm 5% TS
        fertil->afInfParam[0] = (double)0.15;
        fertil->afInfParam[1] = (double)-0.15;
        fertil->afInfParam[2] = (double)0.38;
    }
    else if (!strcmp(fertil->acCode, "RE026\0")) {
        // Klaerschlamm 35% TS
        fertil->afInfParam[0] = (double)0.55;
        fertil->afInfParam[1] = (double)0.65;
        fertil->afInfParam[2] = (double)0.175;
    }
    else if (!strcmp(fertil->acCode, "RE027\0")) {
        // Gruengutkompost
        fertil->afInfParam[0] = (double)0.0;
        fertil->afInfParam[1] = (double)0.0;
        fertil->afInfParam[2] = (double)0.0;
    }
    /* Wenn fuer das gewaehlte Geraet keine Zuordnung stattgefunden
     hat wird es hier mit Standarwerten belegt. */
    //if ((pFE->afInfParam[0] == (double)0.0) && (pFE->afInfParam[1] == (double)0.0) &&
    //    (pFE->afInfParam[2] == (double)0.0)) {
     else {
        fertil->afInfParam[0] = (double)0.1;
        fertil->afInfParam[1] = (double)0.1;
        fertil->afInfParam[2] = (double)0.1;
    }

    return RET_SUCCESS;
} /*===== Ende der Initialisierung der Bodenbearbeitungsgeraete =============*/

//End of Test