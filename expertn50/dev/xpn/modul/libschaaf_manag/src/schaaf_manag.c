//      schaaf_manag.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "schaaf_manag.h"
#include "schaaf_manag_load.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


G_DEFINE_TYPE(schaaf_manag, schaaf_manag, EXPERTN_MODUL_BASE_TYPE);


static void schaaf_manag_class_init(schaaf_managClass *klass){}


static void schaaf_manag_init(schaaf_manag *self)
{
gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init var:
self->__LOAD_DONE=0;

/* Hong
// print Output Message if Modul is used from Expert N
S  = g_strdup_printf("%s\t%d\tInit MODUL schaaf management!",__FILE__,__LINE__);
fprintf(stdout,"%s\n",S);
fflush(stdout);
g_free(S);
 */
 
}


int schaaf_manag_load(schaaf_manag *self)
{
	schaaf_manag_load_ini(self);
		
	return RET_SUCCESS;
}


// Our Modul Functions:

/*********************************************************************************/
/*  Name     : SoilLoosing                                                       */
/*                                                                               */
/*  Funktion : Durch Bodenbearbeitungsmassnahmen werden durch die Lockerung      */
/*             zahlreiche Kenngroessen waehrend der Simulation veraendert. Die   */
/*             integrieretn Modellansaetze sind dem Programm EPIC entnommen      */
/*             und wurden modifiziert.                                           */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
 int TSSoilLoosening(schaaf_manag *self)
  {
       expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
       	   
       PTIME            pTi = xpn->pTi;
       PMANAGEMENT      pMa = xpn->pMa;
	   PTILLAGE         tillage;
	   
	   gchar* S, *S2;
	   
	   if (NewDay(pTi)) {
        self->tillage_actual = NULL;
        for (tillage = pMa->pTillage; (tillage->pNext != NULL); tillage = tillage->pNext) {
            if ((pTi->pSimTime->mday == tillage->Day) && (pTi->pSimTime->mon == tillage->Month) &&
                (pTi->pSimTime->year == tillage->Year)) {
                self->tillage_actual = tillage;
				pMa->pTillage = tillage; //Test of Hong 
				InitBBGeraet(self);  //Test of Hong 
				
				S2 = xpn_time_get_formated_date(pTi->pSimTime->iyear, pTi->pSimTime->fTimeY, FALSE);
                S = g_strdup_printf("%s: Soil loosening: %.1f cm depth.", S2, (double)(tillage->fDepth)/10.0); // [cm]
                PRINT_MESSAGE(xpn, 3, S);
                g_free(S);
				g_free(S2);
				
                break;
            }
        }
      }
   
	   if (self->tillage_actual != NULL) {
           if ((xpn_time_compare_date(pTi->pSimTime->year,
                                   pTi->pSimTime->mon,
                                   pTi->pSimTime->mday,
                                   self->tillage_actual->Year,
                                   self->tillage_actual->Month,
                                   self->tillage_actual->Day) == 0)&&
              (self->tillage_done == 0) ) 
		
            {
	        self->tillage_done += 1;
	
		    // run functions:
            // Test of Hong InitBBGeraet(self); 
            LagerungNeu(self); 
		  
		 
		    // point to the next tillage date:
            if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                   xpn->pTi->pSimTime->mon,
                                   xpn->pTi->pSimTime->mday,
                                   self->tillage_actual->Year,
                                   self->tillage_actual->Month,
                                   self->tillage_actual->Day) == 0) &&
            self->tillage_actual->pNext != NULL) {
            self->tillage_actual = self->tillage_actual->pNext;
            self->tillage_done = 0;
           }
      
	     }/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */
        
      }
   
//	NaturRueck(exp_p);

    return RET_SUCCESS;	   
	   
  }
  
  
 int TSSoilMixing(schaaf_manag *self)
  {
    expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);       

    PTIME            pTi = xpn->pTi;
	PMANAGEMENT      pMa = xpn->pMa;
	PTILLAGE         tillage;
	
	gchar* S, *S2;
	
	   if (NewDay(pTi)) {
        self->tillage_actual = NULL;
        for (tillage = pMa->pTillage; (tillage->pNext != NULL); tillage = tillage->pNext) {
            if ((pTi->pSimTime->mday == tillage->Day) && (pTi->pSimTime->mon == tillage->Month) &&
                (pTi->pSimTime->year == tillage->Year)) {
                self->tillage_actual = tillage;
				pMa->pTillage = tillage; //Test of Hong 
				InitBBGeraet(self);  //Test of Hong 
				
				S2 = xpn_time_get_formated_date(pTi->pSimTime->iyear, pTi->pSimTime->fTimeY, FALSE);
                S = g_strdup_printf("%s: Soil mixing: %.1f cm depth.", S2, (double)(tillage->fDepth)/10.0); // [cm]
                PRINT_MESSAGE(xpn, 3, S);
                g_free(S);
				g_free(S2);
				
                break;
            }
		}
       }

   
   
   
	   if (self->tillage_actual != NULL) {
         if ((xpn_time_compare_date(pTi->pSimTime->year,
                                   pTi->pSimTime->mon,
                                   pTi->pSimTime->mday,
                                   self->tillage_actual->Year,
                                   self->tillage_actual->Month,
                                   self->tillage_actual->Day) == 0)&&
            (self->tillage_done == 0) ) 
		
         {
	     self->tillage_done += 1;
	
		 // run the functions
         //Test of Hong InitBBGeraet(self);  	
 	     EinarbeitRueckstaende(self);  
         DurchMisch(self); 
		 		 		 
		 // point to the next tillage date:
         if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                   xpn->pTi->pSimTime->mon,
                                   xpn->pTi->pSimTime->mday,
                                   self->tillage_actual->Year,
                                   self->tillage_actual->Month,
                                   self->tillage_actual->Day) == 0) &&
            self->tillage_actual->pNext != NULL) {
            self->tillage_actual = self->tillage_actual->pNext;
            self->tillage_done = 0;
           }

	     }/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */

//	Bioturbation(exp_p); 		

      }  

	return RET_SUCCESS;
  }
 




/*********************************************************************************/
/*  Name     : TSIrrigation                                                      */
/*                                                                               */
/*  Funktion : Die Zufuhr von Wasser durch Beregnungsmassnahmen werden mit       */
/*             mit dieser Funktion beruecksichtigt.                              */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*				pCl->pWeather->fRainAmount                                       */
/*                                                                               */
/*********************************************************************************/
int TSIrrigation(schaaf_manag *self)
{
	
	  expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self); 
   
	  PTIME            pTi = xpn->pTi;
	  PMANAGEMENT      pMa = xpn->pMa;
//	  PWATER           pWa = xpn->pWa;
	  PCHEMISTRY       pCh = xpn->pCh;
	  
	  PMIRRIGATION     irrigation;
	  
	  gchar* S, *S2;
	  
	  
	  if (NewDay(pTi)) {
        self->irrigation_actual = NULL;
        for (irrigation = pMa->pIrrigation; (irrigation->pNext != NULL); irrigation = irrigation->pNext) {
            if ((pTi->pSimTime->mday == irrigation->Day) && (pTi->pSimTime->mon == irrigation->Month) &&
                (pTi->pSimTime->year == irrigation->Year)) {
                self->irrigation_actual = irrigation;
				S2 = xpn_time_get_formated_date(pTi->pSimTime->iyear, pTi->pSimTime->fTimeY, FALSE);
                S = g_strdup_printf("%s: Irrigation: %0.2f l.", S2, (double)(irrigation->fAmount));
                //S = g_strdup_printf("%d-%d-%d: Irrigation: %0.2f l", irrigation->Year, irrigation->Month, irrigation->Day, pIR->fAmount);
                PRINT_MESSAGE(xpn, 3, S);
				g_free(S2);
                g_free(S);
				
                break;
            }
        }
	}
	  
	  
	  if (self->irrigation_actual != NULL) {
         if ((xpn_time_compare_date(pTi->pSimTime->year,
                                   pTi->pSimTime->mon,
                                   pTi->pSimTime->mday,
                                   self->irrigation_actual->Year,
                                   self->irrigation_actual->Month,
                                   self->irrigation_actual->Day) == 0)&&
            (self->irrigation_done == 0) ) 
		
         {
	     self->irrigation_done += 1;
	
		 // Folge von Beregnung:
//???         pWa->pWBalance->fReservoir   += pMa->pIrrigation->fAmount; //Hong: wBalance has no member named fReservoir

		 pCh->pCProfile->fNO3NSurf    += pMa->pIrrigation->fNO3N;
		 pCh->pCProfile->fNH4NSurf    += pMa->pIrrigation->fNH4N;
		 //pCh->pCProfile->fUreaNSurf += pMa->pIrrigation->fUreaN;
		 pCh->pCProfile->fNManureSurf += pMa->pIrrigation->fNorgManure;
		 pCh->pCProfile->fCManureSurf += pMa->pIrrigation->fCorgManure;
		 
		              
		 xpn->pCl->pWeather->fPreciRate += self->irrigation_actual->fAmount; // Hong: Übernommen von shaaf.c-XN3 line 1182
         xpn->pCl->pWeather->fLiquPreciRate += self->irrigation_actual->fAmount; // Hong: Übernommen von shaaf.c-XN3 line 1183
		 
		 		 		 
		 // point to the next irrigation date:
         if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                   xpn->pTi->pSimTime->mon,
                                   xpn->pTi->pSimTime->mday,
                                   self->irrigation_actual->Year,
                                   self->irrigation_actual->Month,
                                   self->irrigation_actual->Day) == 0) &&
            self->irrigation_actual->pNext != NULL) {
            self->irrigation_actual = self->irrigation_actual->pNext;
            self->irrigation_done = 0;
           }

	     }/* Ende if (datum == pMa->pIrrigation->lDate) Beregnungstermin  */
       } 
	  	  
	  
  return RET_SUCCESS;
}      /*=====   Ende der Integration Beregnung  =================*/




/*********************************************************************************/
/*  Name     : InitBBGeraet                                                      */
/*                                                                               */
/*  Funktion : Die eingesetzten Bodenbearbeitungsgeraete werden bezueglich       */
/*             ihrer Lockerungs-, Misch- und Einarbeitungswirkung bewertet.      */
/*             Die bewerteten Parameter wurden im Modell EPIC definiert. Zur     */
/*             Integration wurden Veraenderungen nach Schaaf vorgenommen.        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*             pMa->pTillage->fEffMix                                            */
/*             pMa->pTillage->fEffLoose                                          */
/*             pMa->pTillage->fEffIncorp                                         */
/*                                                                               */
/*********************************************************************************/

int InitBBGeraet(schaaf_manag *self)
 {

	expertn_modul_base* xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PTILLAGE	tillage = pMa->pTillage;
	
	
	if (strcmp(tillage->acCode,"TI047\0")==0)        
	{ //Schaelpflug
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.75;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}

	if (strcmp(tillage->acCode,"TI048\0")==0)        	
	{ //Scharpflug
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.75;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI005\0")==0)        	
	{ //Volldrehpflug
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.95;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI049\0")==0)        	
	{ //Scheibenpflug
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.75;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI007\0")==0)        	
	{ //Scheibenpflug
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.75;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI030\0")==0)        	
	{ //Schichtengubber
		pMa->pTillage->fEffMix = (double)0.6;
		pMa->pTillage->fEffLoose = (double)0.55;
		pMa->pTillage->fEffIncorp = (double)0.55;
	}
	if (strcmp(tillage->acCode,"TI010\0")==0)        	
	{ //Schaelgrubber
		pMa->pTillage->fEffMix = (double)0.6;
		pMa->pTillage->fEffLoose = (double)0.55;
		pMa->pTillage->fEffIncorp = (double)0.35;
	}
	if (strcmp(tillage->acCode,"TI012\0")==0)        	
	{ //Federzahnegge
		pMa->pTillage->fEffMix = (double)0.2;
		pMa->pTillage->fEffLoose = (double)0.05;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI037\0")==0)        	
	{ //Feingrubber
		pMa->pTillage->fEffMix = (double)0.2;
		pMa->pTillage->fEffLoose = (double)0.05;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI032\0")==0)        	
	{ //Spatenrollegge
		pMa->pTillage->fEffMix = (double)0.2;
		pMa->pTillage->fEffLoose = (double)0.05;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI042\0")==0)        	
	{ //Ackerfraese
		pMa->pTillage->fEffMix = (double)0.8;
		pMa->pTillage->fEffLoose = (double)0.75;
		pMa->pTillage->fEffIncorp = (double)0.64;
	}
	if (strcmp(tillage->acCode,"TI036\0")==0)        	
	{ //Striegelegge
		pMa->pTillage->fEffMix = (double)0.2;
		pMa->pTillage->fEffLoose = (double)0.0;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI033\0")==0)        	
	{ //Loeffelegge
		pMa->pTillage->fEffMix = (double)0.5;
		pMa->pTillage->fEffLoose = (double)0.0;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI035\0")==0)        	
	{ //Ruettelegge
		pMa->pTillage->fEffMix = (double)0.7;
		pMa->pTillage->fEffLoose = (double)0.0;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI034\0")==0)        	
	{ //Kreiselegge
		pMa->pTillage->fEffMix = (double)0.7;
		pMa->pTillage->fEffLoose = (double)0.05;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI013\0")==0)        	
	{ //Saategge
		pMa->pTillage->fEffMix = (double)0.2;
		pMa->pTillage->fEffLoose = (double)0.0;
		pMa->pTillage->fEffIncorp = (double)0.1;
	}
	if (strcmp(tillage->acCode,"TI031\0")==0)        	
	{ //Scheibenegge
		pMa->pTillage->fEffMix = (double)0.7;
		pMa->pTillage->fEffLoose = (double)0.5;
		pMa->pTillage->fEffIncorp = (double)0.5;
	}
	if (strcmp(tillage->acCode,"TI043\0")==0)        	
	{ //Zinkenroter
		pMa->pTillage->fEffMix = (double)0.8;
		pMa->pTillage->fEffLoose = (double)0.64;
		pMa->pTillage->fEffIncorp = (double)0.64;
	}
	if (strcmp(tillage->acCode,"TI015\0")==0)        	
	{ //Rotortiller
		pMa->pTillage->fEffMix = (double)0.8;
		pMa->pTillage->fEffLoose = (double)0.64;
		pMa->pTillage->fEffIncorp = (double)0.64;
	}
	if (strcmp(tillage->acCode,"TI024\0")==0)        	
	{ //Pflug mit Untergrundlockerer
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.95;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI025\0")==0)        	
	{ // Volldrehpflug mit Packer
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.55;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI004\0")==0)        	
	{ // Beetpflug
		pMa->pTillage->fEffMix = (double)0.4;
		pMa->pTillage->fEffLoose = (double)0.65;
		pMa->pTillage->fEffIncorp = (double)0.75;
	}
	if (strcmp(tillage->acCode,"TI027\0")==0)        	
	{ // Spatenmaschine
		pMa->pTillage->fEffMix = (double)0.3;
		pMa->pTillage->fEffLoose = (double)0.75;
		pMa->pTillage->fEffIncorp = (double)0.5;
	}
	if (strcmp(tillage->acCode,"TI029\0")==0)
	{ // Schwergrubber
		pMa->pTillage->fEffMix = (double)0.65;
		pMa->pTillage->fEffLoose = (double)0.65;
		pMa->pTillage->fEffIncorp = (double)0.650;
	}
	if (strcmp(tillage->acCode,"TI038\0")==0)        	
	{ // Schleppegge
		pMa->pTillage->fEffMix = (double)0.0;
		pMa->pTillage->fEffLoose = (double)-0.05;
		pMa->pTillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI039\0")==0)        	
	{ // Cambridge-Walze
		pMa->pTillage->fEffMix = (double)0.0;
		pMa->pTillage->fEffLoose = (double)-0.5;
		pMa->pTillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI040\0")==0)        	
	{ // Crosskill-Walze
		pMa->pTillage->fEffMix = (double)0.0;
		pMa->pTillage->fEffLoose = (double)-0.5;
		pMa->pTillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI041\0")==0)        	
	{ // Ringel-Walze
		pMa->pTillage->fEffMix = (double)0.0;
		pMa->pTillage->fEffLoose = (double)-0.5;
		pMa->pTillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI044\0")==0)        	
	{ // Dammfraese
		pMa->pTillage->fEffMix = (double)0.55;
		pMa->pTillage->fEffLoose = (double)0.65;
		pMa->pTillage->fEffIncorp = (double)0.15;
	}
	if (strcmp(tillage->acCode,"TI045\0")==0)        	
	{ // Tiefenmeissel
		pMa->pTillage->fEffMix = (double)0.0;
		pMa->pTillage->fEffLoose = (double)-0.65;
		pMa->pTillage->fEffIncorp = (double)0.0;
	}
	if (strcmp(tillage->acCode,"TI046\0")==0)        	
	{ // Mulchgeraet
		pMa->pTillage->fEffMix = (double)0.15;
		pMa->pTillage->fEffLoose = (double)0.0;
		pMa->pTillage->fEffIncorp = (double)0.25;
	}

      /* Wenn fuer das gewaehlte Geraet keine Zuordnung stattgefunden 
           hat wird es hier mit Standarwerten belegt. */
		if ((pMa->pTillage->fEffMix ==(double)0.0) &&
		   (pMa->pTillage->fEffLoose ==(double)0.0) &&
		   (pMa->pTillage->fEffIncorp ==(double)0.0))
		   {
			pMa->pTillage->fEffMix = (double)0.1;
			pMa->pTillage->fEffLoose = (double)0.1;
			pMa->pTillage->fEffIncorp = (double)0.1;
		   }


   PRINT_MESSAGE(xpn,4,"InitBBGeraet geladen!")
   
   return RET_SUCCESS;
 }      /*===== Ende der Initialisierung der Bodenbearbeitungsgeraete =============*/
   
  
/*********************************************************************************/
/*  Name     : LagerungNeu                                                       */
/*                                                                               */
/*  Funktion : Die Lagerungsdichte wird nach jeder Bodenbearbeitung neu be-      */
/*             rechnet. Die integrierten Funktionen sind dem Modell EPIC         */
/*             entnommen und wurden nach Schaaf geaendert.                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
int LagerungNeu(schaaf_manag *self)
 {
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self); 
	
 	PSLAYER pSL= xpn->pSo->pSLayer;
	PMANAGEMENT pMa = xpn->pMa;
	PSPROFILE pSo= xpn->pSo;

	int		i1,anzahl;
	double   afanteilschicht[MAXSCHICHT];
	double   lagdichtevor[MAXSCHICHT];
	double   lagdichteEnt[MAXSCHICHT];
    double	bearbTiefe,aktTiefe,obereTiefe,anteil,lagletzteSchicht,diff,
    		lagvor,lagvoll,lockUnten,lockOben,differ;

		/* Berechnung des anteils eine Bodenschicht 
				an dem bearbeiteten Bodenvolumen. */
		bearbTiefe = pMa->pTillage->fDepth; // mm 
		aktTiefe =(double)0.0;

	    for (pSL = pSo->pSLayer->pNext,anzahl=0;
	                 (aktTiefe < bearbTiefe);
	         pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
	
			if (aktTiefe > bearbTiefe)
			{
			  anteil = (bearbTiefe - obereTiefe) / bearbTiefe;
			}
			else
			{
			  anteil = (pSL->fThickness / bearbTiefe);
			}
			afanteilschicht[anzahl] = anteil;
	    } 	/* Ende der Schleife zur Berechnung der Schichtanteile */


 	/* Berechnung der neuen Lagerungsdichte fuer die bearbeiteten 
 	   Simulationsschichten. Nur teilweise bearbeitete Bodenschichten
 	   werden gewichtet gelockert. */

 		/* Lokales Array wird initialisiert */
	    for (pSL = pSo->pSLayer->pNext,i1=0;
	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext,i1++)
		{
			lagdichtevor[i1] = pSL->fBulkDens;                   
			lagletzteSchicht = pSL->fBulkDens;                   
		}/* Ende lokale Initialisierung */


	 	/* Berechnung der entgueltigen Lagerungsdichte derzeit lokal, 
	 		da kein globale Variable vorhanden. */
	    for (pSL = pSo->pSLayer->pNext,i1=0;
	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext,i1++)
		{
			diff = ((lagletzteSchicht - pSL->fBulkDensStart) / (double ) 10.0);
			if (diff < (double)0.0) 
			{ diff = (double)0.0;}
			lagdichteEnt[i1] = diff;
		}/* Ende Initialisierung entgueltigen Lagerungsdichte*/

 		/* Veraenderung der bearbeiteten Bodenschichten. */
 	   
	    for (pSL = pSo->pSLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,i1++)
		{
 			/* Nur erste Bodenschicht wurde bearbeitet */
			if (i1 == (int)1)
            {
			  pSL->fBulkDens = (lagdichtevor[i1] - ((lagdichtevor[i1] 
			  				- (((double)2.0 * (pSL->fBulkDens + lagdichteEnt[i1])) / (double)3.0))
			  									 * pMa->pTillage->fEffLoose));

            }  /* Ende nur erste Schicht bearbeitet */
			else  /* Mehrere Bodenschicht wurden bearbeitet */
			{
			  if (i1 == anzahl) // Berechnung teilweise bearbeiteter Sim.schicht
			  {
			    /* Veraenderungen, die bei vollstaendiger Bearbeitung entstanden 
			       waeren werden nur anteilsmaessig realisiert */

				lagvor = lagdichtevor[i1];
				lagvoll =  (lagdichtevor[i1] - ((lagdichtevor[i1] 
			  				- (((double)2.0 * (pSL->fBulkDens + lagdichteEnt[i1])) / (double)3.0))
			  									 * pMa->pTillage->fEffLoose));
							  									 
				lockUnten = (pSL->fThickness * i1) - pMa->pTillage->fDepth;
				lockOben = pSL->fThickness - lockUnten;
				differ = ((lagvor - lagvoll)/ pSL->fThickness) * lockOben;
				pSL->fBulkDens = lagvor - differ;			
					
			  }/* Ende teilweise bearbeitete Sim.schicht */
			  else /* Berechnung Sim.schicht, die komplett bearbeitet wurden */
			  {
			    pSL->fBulkDens = (lagdichtevor[i1] - ((lagdichtevor[i1] 
			  	        			- (((double)2.0 * (pSL->fBulkDens + lagdichteEnt[i1])) / (double)3.0))
			  									 * pMa->pTillage->fEffLoose));
			  }/* Ende komplett bearbeiteter Sim.schicht */
			
			}  /* Ende mehrere Schichten bearbeitet */
			
		}   /* Ende Schleife der bearbeiteten Schichten bis Anzahl */

   PRINT_MESSAGE(xpn,4,"LagerungNeu geladen!")

   return RET_SUCCESS;
 }      /*===== Ende der Neuberechnung der Lagerungsdichte =============*/  
  
  
 
/*********************************************************************************/
/*  Name     : EinarbeitRueckstaende                                             */
/*                                                                               */
/*  Funktion : Bei der Durchfuehrung einer Bodenbearbeitung werden die an der    */
/*             Oberflaeche aufliegenden Materialien in den bearbeiteten Boden    */
/*             eingearbeitet. Diese Einarbeitung wird durch diese Funktion       */
/*             sichergestellt. Die Modellansaetze sind dem Modell EPIC entnommen */
/*             und wurden modifiziert integriert.                                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/    
 int EinarbeitRueckstaende(schaaf_manag *self)
 {
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	 
	PMANAGEMENT     pMa = xpn->pMa;
	PCHEMISTRY      pCh = xpn->pCh;
	PCPROFILE       pCP     = pCh->pCProfile;
	PCBALANCE	    pCB     = pCh->pCBalance; //Added by Hong on 20180731
// orig.:  PCLAYER   pCL = pCh->pCLayer;

	PCLAYER   pCL = xpn->pCh->pCLayer->pNext;
	PSLAYER   pSL = xpn->pSo->pSLayer->pNext;
 
    double	restMenge,tiefe,effektEinarbeitung; 
	
	restMenge = (double)0.0;	
	
	effektEinarbeitung = pMa->pTillage->fEffIncorp;					
	tiefe = pMa->pTillage->fDepth; // mm
	
	if (pCP->fNO3NSurf >(double)0.0)
	{
		restMenge = pCP->fNO3NSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNO3N    += (pCP->fNO3NSurf - restMenge);
//Hong		pCh->pCBalance->fNInputCum += (pCP->fNO3NSurf - restMenge); //Hong: fNInoutCum = dNinputCum in Xpn 5.0
		pCh->pCBalance->dNInputCum += (pCP->fNO3NSurf - restMenge);
		pCP->fNO3NSurf = restMenge;
		
	}

	if (pCP->fNH4NSurf >(double)0.0)
	{
		restMenge = pCP->fNH4NSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNH4N    += (pCP->fNH4NSurf - restMenge);
//Hong		pCh->pCBalance->fNInputCum += (pCP->fNH4NSurf - restMenge); //Hong: fNInoutCum = dNinputCum in Xpn 5.0
        pCh->pCBalance->dNInputCum += (pCP->fNH4NSurf - restMenge);
		pCP->fNH4NSurf = restMenge;
	}

	if (pCP->fUreaNSurf >(double)0.0)
	{
		restMenge = pCP->fUreaNSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fUreaN    += (pCP->fUreaNSurf - restMenge);
//Hong		pCh->pCBalance->fNInputCum += (pCP->fUreaNSurf - restMenge); // fNInoutCum = dNinputCum in Xpn 5.0
	    pCh->pCBalance->dNInputCum += (pCP->fUreaNSurf - restMenge);
		pCP->fUreaNSurf = restMenge;
	}
	
	if (pCP->fNStandCropRes >(double)0.0)
	{
		restMenge = pCP->fNStandCropRes * (double)exp((double)-0.0569* tiefe
				      * (effektEinarbeitung * effektEinarbeitung));
		pCP->fNLitterSurf += (pCP->fNStandCropRes - restMenge);
		pCP->fNStandCropRes = restMenge;
	}

	if (pCP->fCStandCropRes >(double)0.0)
	{
		restMenge = pCP->fCStandCropRes * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCP->fCLitterSurf += (pCP->fCStandCropRes - restMenge);
		//Hong added on 20180807 for C-balance
		pCP->fCLeafLitterSurf += (pCP->fCStandCropRes - restMenge); //for century_n
		pCB->dCInputSurf +=(pCP->fCStandCropRes - restMenge);
		//pCB->dCInputCum +=(pCP->fCStandCropRes - restMenge);
		
		pCP->fCStandCropRes = restMenge;
		
	}


	if (pCP->fNManureSurf >(double)0.0)
	{
		restMenge = pCP->fNManureSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNManure    += (pCP->fNManureSurf - restMenge);
		pCP->fNManureSurf = restMenge;
        
	}

	if (pCP->fCManureSurf >(double)0.0)
	{
		restMenge = pCP->fCManureSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fCManure    += (pCP->fCManureSurf - restMenge);
		pCB->dCInputProfile += (pCP->fCManureSurf - restMenge);
		pCB->dCInputProfile_30 += (pCP->fCManureSurf - restMenge);
		
		pCB->dCInputSurf -= (pCP->fCManureSurf - restMenge);
		pCP->fCManureSurf = restMenge;
		
		
		
	}

	if (pCP->fNLitterSurf >(double)0.0)
	{
		restMenge = pCP->fNLitterSurf * (double)exp((double)-0.0569 * tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNLitter    += (pCP->fNLitterSurf - restMenge);
		pCP->fNLitterSurf = restMenge;						 
	}

	if (pCP->fCLitterSurf >(double)0.0)
	{
		restMenge = pCP->fCLitterSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fCLitter    += (pCP->fCLitterSurf - restMenge);
		pCL->fCFineRootLitter += (pCP->fCLitterSurf - restMenge);//Hong for century_n
		pCB->dCInputProfile += (pCP->fCLitterSurf - restMenge);//Hong for century_n
		pCB->dCInputProfile_30 += (pCP->fCLitterSurf - restMenge);//Hong for century_n
		pCB->dCInputSurf -= (pCP->fCLitterSurf - restMenge);//Hong for century_n
		pCP->fCLitterSurf = restMenge;
		pCP->fCLeafLitterSurf = restMenge; //Hong for century_n
		
	}

	if (pCP->fNHumusSurf >(double)0.0)
	{
		restMenge = pCP->fNHumusSurf * (double)exp((double)-0.0569 * tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fNHumus    += (pCP->fNHumusSurf - restMenge);
		
		//Begin of Hong: change in XN3 
		//SG 20161009: For DAISY model - 100% of fNorgManure that is partitioned to "fNHumusSurf" are SOM2 (see TSFertilizer, manage.c line 334-349)
		pCL->fNHumusFast   += (pCP->fNHumusSurf - restMenge);
		//End of Hong
		
		pCP->fNHumusSurf = restMenge;						 
	}

	if (pCP->fCHumusSurf >(double)0.0)
	{
		restMenge = pCP->fCHumusSurf * (double)exp((double)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fCHumus    += (pCP->fCHumusSurf - restMenge);
		
	    pCL->fCHumusFast   += (pCP->fCHumusSurf - restMenge);//Hong: change in XN3
        pCB->dCInputProfile += (pCP->fCHumusSurf - restMenge);//for C balance
		pCB->dCInputProfile_30 += (pCP->fCHumusSurf - restMenge);//for C balance
		pCB->dCInputSurf -=(pCP->fCHumusSurf - restMenge);//for C balance
		pCP->fCHumusSurf = restMenge;
		pCP->fCMicLitterSurf = restMenge;//Hong for century_n
		
		//SG 20161009: For DAISY model - 100% of fCorgManure that is partitioned to "fCHumusSurf" are SOM2 (see TSFertilizer, manage.c line 334-349)
		
        pSL->fHumusCN    =pSL->fCHumus/pSL->fNHumus;
		
	}

  PRINT_MESSAGE(xpn,4,"Ernterueckstaende geladen!")

  return RET_SUCCESS;
 }      /*===== Ende der Einarbeitung Ernterueckstaende =============*/
 
 
/*********************************************************************************/
/*  Name     : DurchMisch                                                        */
/*                                                                               */
/*  Funktion :                                                                   */
/*                                                                               */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/  
 int DurchMisch(schaaf_manag *self) 
 {
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	PMANAGEMENT pMa = xpn->pMa;
	PCHEMISTRY pCh =xpn->pCh;
	PSPROFILE pSo= xpn->pSo;				
 	PSLAYER pSL;
 	PCLAYER	pCL;

	int		i1,anzahl;
	double   afanteilschicht[MAXSCHICHT];
    double	bearbTiefe,aktTiefe,obereTiefe,anteilschicht,anteil,no3ges,
    		nh4ges,ureages,humCges,humNges,humges,litterCges,litterNges,manureNges,
    		manureCges,mischeffekt;

		mischeffekt = pMa->pTillage->fEffMix;
		
		no3ges     = (double)0.0;
		nh4ges     = (double)0.0;
		ureages    = (double)0.0;
		humges     = (double)0.0;
		humCges    = (double)0.0;
		humNges    = (double)0.0;
		litterCges = (double)0.0;
		litterNges = (double)0.0;
		manureNges = (double)0.0;
		manureCges = (double)0.0;

		/* Berechnung des anteils eine Bodenschicht 
				an dem bearbeiteten Bodenvolumen. */
		bearbTiefe = pMa->pTillage->fDepth; //mm
		aktTiefe =(double)0.0;

	    for (pSL = pSo->pSLayer->pNext,anzahl=0;
	                 (aktTiefe < bearbTiefe);
	         pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
		
			if (aktTiefe > bearbTiefe)
			{
			  anteilschicht = (bearbTiefe - obereTiefe) / bearbTiefe;
			}
			else
			{
			  anteilschicht = (pSL->fThickness / bearbTiefe);
			}
			afanteilschicht[anzahl] = anteilschicht;
	    } 	/* Ende der Schleife zur Berechnung der Schichtanteile */


		/* Summierung des bis zur Einarbeitungstiefe vorhandenen Nitrat, Ammonium,
		    Menge und Stickstoff an frischer org. Substanz sowie Menge und Stickstoff
		    an Humus. Die letzte Schicht wird eventuell nur anteilsmaessig 
		    beruecksichtigt*/

		aktTiefe =(double)0.0;

	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
			if (aktTiefe < bearbTiefe)
			{ anteil = (double)1.0;}
			else
			{ anteil = (bearbTiefe - obereTiefe) / pSL->fThickness;
			}

			no3ges     += pCL->fNO3N * anteil;
			nh4ges     += pCL->fNH4N * anteil;
			ureages    += pCL->fUreaN * anteil;
			humges	   += pSL->fHumus * anteil;
			humCges    += pSL->fCHumus * anteil;
			humNges    += pSL->fNHumus * anteil;
			litterCges += pCL->fCLitter * anteil;
			litterNges += pCL->fNLitter * anteil;
			manureNges += pCL->fNManure * anteil;
			manureCges += pCL->fCManure * anteil;

        } 	/* Ende der Schleife zum Summieren */


		/* Gleichmaessige Verteilung der Gesamtmengen auf die betroffenen
		   Bodenschichten. Dabei wird der Mischeffekt des Bodenbearbeitungs-
		   geraetes beruecksichtigt. Die Verteilungsfunktion ist aus dem
		   Modell EPIC entnommen.  */


	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
			anteilschicht = afanteilschicht[i1-1];
		   if (i1 != anzahl)
		   {
			pCL->fNO3N    = (((double)1.0 - mischeffekt) * pCL->fNO3N) 
											+ (anteilschicht * mischeffekt * no3ges);											
			pCL->fNH4N    = (((double)1.0 - mischeffekt) * pCL->fNH4N) 
											+ (anteilschicht * mischeffekt * nh4ges);
			pCL->fUreaN   = (((double)1.0 - mischeffekt) * pCL->fUreaN) 
											+ (anteilschicht * mischeffekt * ureages);
			pCL->fCLitter = (((double)1.0 - mischeffekt) * pCL->fCLitter) 
											+ (anteilschicht * mischeffekt * litterCges);
			pCL->fNLitter = (((double)1.0 - mischeffekt) * pCL->fNLitter) 
											+ (anteilschicht * mischeffekt * litterNges);
			pCL->fNManure = (((double)1.0 - mischeffekt) * pCL->fNManure) 
											+ (anteilschicht * mischeffekt * manureNges);
			pCL->fCManure = (((double)1.0 - mischeffekt) * pCL->fCManure) 
											+ (anteilschicht * mischeffekt * manureCges);
			pSL->fHumus   = (((double)1.0 - mischeffekt) * pSL->fHumus) 
											+ (anteilschicht * mischeffekt * humges);
			pSL->fCHumus  = (((double)1.0 - mischeffekt) * pSL->fCHumus) 
											+ (anteilschicht * mischeffekt * humCges);
			pSL->fNHumus  = (((double)1.0 - mischeffekt) * pSL->fNHumus) 
											+ (anteilschicht * mischeffekt * humNges);											
		   } /* Ende der Schleife wenn akt. Schicht komplett bearbeitet */
		   else
		   {
			pCL->fNO3N    = (pCL->fNO3N * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNO3N * anteil ))
                                            +  (anteilschicht * mischeffekt * no3ges));											
			pCL->fNH4N    = (pCL->fNH4N * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * (pCL->fNH4N * anteil ))
                                            +  (anteilschicht * mischeffekt * nh4ges));
			pCL->fUreaN   = (pCL->fUreaN * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fUreaN * anteil ))
                                            +  (anteilschicht * mischeffekt * ureages));
			pCL->fCLitter = (pCL->fCLitter * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterCges));
			pCL->fNLitter = (pCL->fNLitter * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterNges));
			pCL->fNManure = (pCL->fNManure * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureNges));
			pCL->fCManure = (pCL->fCManure * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureCges));
			pSL->fHumus   = (pSL->fHumus * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humges));
			pSL->fNHumus  = (pSL->fNHumus * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fNHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humNges));
			pSL->fCHumus  = (pSL->fCHumus * ((double)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fCHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humCges));

		   } /* Ende der Schleife wenn akt. Schicht teilweise bearbeitet */

        } 	/* Ende der Schleife zum Verteilen auf die Bodenschichten */

   PRINT_MESSAGE(xpn,4,"Durchmischung geladen!")

   return RET_SUCCESS;
 }      /*===== Ende der Berechnung der Durchmischung=============*/
  
