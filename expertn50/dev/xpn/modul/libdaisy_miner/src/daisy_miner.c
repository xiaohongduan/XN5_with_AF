//      daisy_miner.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "daisy_miner.h"
#include "daisy_miner_util.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(daisy_miner, daisy_miner, EXPERTN_MODUL_BASE_TYPE);


static void daisy_miner_class_init(daisy_minerClass *klass){}


static void daisy_miner_init(daisy_miner *self)
{
gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//Hong: Temperaturabhängigkeit für DAISY Abbauraten (für Scott Demyan)		
self->iTempCorr=0;
// End of Hong
// print Output Message if Modul is used from Expert N
S  = g_strdup_printf("%s\t%d\tInit MODUL DAISY_MINER!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
fflush(stdout);
g_free(S);
}



// Our Modul Functions:

int daisy_miner_run(daisy_miner *self)
{
 
	expertn_modul_base *xpn = &(self->parent);
	PTIME 			pTi = xpn->pTi;
	PCHEMISTRY		pCh = xpn->pCh; // Hong

	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;
	PSLAYER      pSL;
	PCLAYER      pCL;
	PCPARAM      pPA;
	PSWATER      pSW;
	PWLAYER      pWL;
	PHLAYER      pHL;



  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/

  /* 1. Variablen für die C-Flüsse */
  double fCAOM1DecayR,fCAOM2DecayR,fCBOM1DecayR,fCBOM2DecayR,fCSOM1DecayR,fCSOM2DecayR;
  double fCAOM1ToBOM1R,fCAOM1ToBOM2R,fCAOM1ToCO2R;
  double fCAOM2ToBOM2R,fCAOM2ToCO2R;
  double fCBOM1ToBOM2R,fCBOM1ToSOM2R,fCBOM1ToCO2R;
  double fBOM2ToSOM2C,fBOM2InternC,fBOM2ToCO2C;
  double fSOM1ToBOM1C,fSOM1ToCO2C;
  double fSOM2ToSOM1C,fSOM2ToBOM1C,fSOM2ToCO2C;
  double fBOM1_DeathC,fBOM2_DeathC;
  double fBOM1_MainC,fBOM2_MainC;
 
  /* 2. Variablen für die C/N Verhältnisse */
  double fCN_FOMSlow     = (double)100.0;   /*! org.:  100*/
  double fCN_FOMFast     = (double) 20.0;   /*! org.:  unterschiedlich*/
  double fCN_MicBiomSlow = (double) 10.0;   /*! org.:  10 */
  double fCN_MicBiomFast = (double) 8.0;    /*! org.:  6  */
  double fCN_HumusSlow   = (double) 12.0;   /*! org.:  12 */
  double fCN_HumusFast   = (double)  8.0;   /*! org.:  10 */
  //Hong: Veränderung für Scott Demyan:
  double fCN_MicBiomDenit = (double)  8.0;  
  //End of Hong

  /* 3. Variablen für die Aufteilung der Stoffflüsse  */
  
//Hong: Veränderung für Scott Demyan  
  //const double fBOM1      = (double)0.5;    /* org.:  0.5 */
  //const double fSOM2      = (double)0.4;    /* org.:  0.4 */
  //const double fSOM1      = (double)0.1;    /* org.:  0.1 */
  //const double fEff       = (double)0.55;   /* org.:  0.6 */
   double fBOM1 =self->fParBOM1;          /* org.:  0.5 */
   double fSOM2 =self->fParSOM2;    /* org.:  0.4 */
   double fSOM1 =self->fParSOM1;    /* org.:  0.1 */
   double fEff =self->fParEff;   /* org.:  0.6 */
  
  
  /* 4. Sterbe- und Erhaltungsraten der Biomasse*/
  double BOM1death        = (double)0.001; /* org.:  0.01 */
  double BOM1maintenance  = (double)0.01;  /* org.:  0.1 */
  double BOM2death        = (double)0.01;  /* org.:  0.1 */
  double BOM2maintenance  = (double)0.01;  /* org.:  0.1 */
  
  /* 5. Variablen für die Abbau-Begrenzung bei mangelndem Nmin */
  double fNH4ImmMaxR = (double)0.5;
  double fNO3ImmMaxR = (double)0.5;
  double fRed,fNH4ImmN,fNO3ImmN;  

  /* 6. Variablen für die Berechnung der N-Mineralisation/N-Immobilisierung*/
  double help1,help2,help3,help4,help5,help6,help7,help8;
  double fToNH4R,fImmR,fNH4NR,fNO3NR;

  /* 7. Variablen für die Massenbilanz*/
  double fCTotal0,fNTotal0;                     /* Summe: AOM + BOM + SOM zu Beginn des Zeitschritts */
  double fCTotal1,fNTotal1;		       		   /* Summe: AOM + BOM + SOM zum Ende des Zeitschritts */
  
  double fCDiff,fNDiff;                         /* Differenz zwischen momentaner und anfaenglicher Summe*/
  //static double fCDiffSum  = (double)0.0; //Out-noted by Hong 20170517: no static variable!
  //static double fNDiffSum  = (double)0.0;
  

  /* 8. Variablen für die Original Reduktionsfunktionen*/
  const double T0= (double)0;              /* Parameter für Temperatur-Korrektur */
  const double T1= (double)20;
 
  //Hong: Temperaturabhängigkeit für DAISY Abbauraten (für Scott Demyan)
  double fTempCorr_CAOM1, fTempCorr_CAOM2;
  double fTempCorr_CBOM1, fTempCorr_CBOM2;
  double fTempCorr_CSOM1, fTempCorr_CSOM2;
  double fTempCorr_DBOM1, fTempCorr_DBOM2;
  double fTempCorr_MBOM1, fTempCorr_MBOM2;
  //End of Hong
 
 
  double f1;                              /* Parameter für Feuchte-Korrektur */
  double wmin,wlow,whigh,wmax;
  
  const double C0= (double)25.0;           /* Parameter für Ton-Korrektur */
  const double C1= (double)0.02;
  
  /*UNKLAR!!!                                 Quotient aus mikrobieller Biomasse und totalem org. C [1] */
  const double fFD = (double)0.02;          /* org.: 0.02   Quotient aus Denitrifikanten und mikrobieller Biomasse  [1]  */

  /* 9 Variablen für die Anteilsberechnung */
  double afAnt[4], fC_Old, fN_Old;

//Hong
  struct ncorr corr= {(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1};

//Hong
	pSL = xpn->pSo->pSLayer;
	pCL = xpn->pCh->pCLayer;
	pPA = xpn->pCh->pCParam;
	pSW = xpn->pSo->pSWater;
	pWL = xpn->pWa->pWLayer;
	pHL = xpn->pHe->pHLayer;
	


//activated by Hong on 20180724
SurfaceMiner(self); //exp_p durch stickstoff *self ersetzt
//Hong
//	rm    = (double)1.0;    /*orig :1.0; */			//analog crikle

for (N_SOIL_LAYERS)     //schichtweise Berechnung
  {
  /* mögliche Standardinitialisierung 
  if (SimStart(pTi))
  {
   pCL->fCFOMSlow     = (double)0.588 * pCL->fCLitter;
   pCL->fCFOMFast     = (double)0.392 * pCL->fCLitter;
   pCL->fCMicBiomSlow = (double)0.01  * pCL->fCLitter;
   pCL->fCMicBiomFast = (double)0.01  * pCL->fCLitter;
  }*/ /* Ende der Startinitialisierung */


  /***********************************************************************/
  /* Übertragen der Mengen in die spezifischen DAISY-Pools               */
  /* Litter         = AOM1 = FOMSlow                                     */
  /* Manure         = AOM2 = FOMFast                                     */
  /* Humus (Anteil) = BOM1 = MicBiomSlow                                 */
  /* Humus (Anteil) = BOM2 = MicBiomFast                                 */
  /* Humus (Anteil) = SOM1 = HumusSlow                                   */
  /* Humus (Anteil) = SOM2 = HumusFast                                   */
  /*	                                                                 */
  /* Berechnung der Anteile aus dem Humus anhand der relativen           */
  /* Verhältnisse zum Ende des vorherigen Zeitsschritts.                 */
  /***********************************************************************/

  pCL->fCFOMSlow = pCL->fCLitter;
  pCL->fCFOMFast = pCL->fCManure;

  pCL->fNFOMSlow = pCL->fNLitter;
  pCL->fNFOMFast = pCL->fNManure; 

  if(!SimStart(pTi))
  {
   fC_Old = pCL->fCMicBiomSlow + pCL->fCMicBiomFast +
               pCL->fCHumusSlow + pCL->fCHumusFast;

   if(fC_Old)
   {
    afAnt[0]  = pCL->fCMicBiomSlow / fC_Old;
    afAnt[1]  = pCL->fCMicBiomFast / fC_Old;
    afAnt[2]  = pCL->fCHumusSlow   / fC_Old;
    afAnt[3]  = pCL->fCHumusFast   / fC_Old;
	 	 
    pCL->fCMicBiomSlow = afAnt[0] * pSL->fCHumus;
    pCL->fCMicBiomFast = afAnt[1] * pSL->fCHumus;
    pCL->fCHumusSlow   = afAnt[2] * pSL->fCHumus;
    pCL->fCHumusFast   = afAnt[3] * pSL->fCHumus;
   }
	 
   fN_Old = pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
                pCL->fNHumusSlow + pCL->fNHumusFast;

   if(fN_Old)
   {
    afAnt[0]  = pCL->fNMicBiomSlow / fN_Old;
    afAnt[1]  = pCL->fNMicBiomFast / fN_Old;
    afAnt[2]  = pCL->fNHumusSlow   / fN_Old;
    afAnt[3]  = pCL->fNHumusFast   / fN_Old;
 	 	 
    pCL->fNMicBiomSlow = afAnt[0] * pSL->fNHumus;
    pCL->fNMicBiomFast = afAnt[1] * pSL->fNHumus;
    pCL->fNHumusSlow   = afAnt[2] * pSL->fNHumus;
    pCL->fNHumusFast   = afAnt[3] * pSL->fNHumus;
   }
  }
 
 
 
  /******************************************************************/
  /*                          Reduktionsfunktionen                  */
  /******************************************************************/
   	
	/* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
     // Hong    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); // refer to LEACHN miner.c, Q10-Funktion.
    // corr.Temp = abspowerDBL(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0));    // abspowerDBL() do the same calculation as abspower() #deactivated by Moritz
	
	//Hong: Temperaturabhängigkeit für DAISY Abbauraten (für Scott Demyan)
	/* Pool specific temperature functions begin below (Scott Demyan, 2015.07.06)*/	
	if (self->iTempCorr == 1)
	{
      fTempCorr_CAOM1 = abspowerDBL(self->fParCAOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/ // abspowerDBL() do the same calculation as abspower()
	  fTempCorr_CAOM2 = abspowerDBL(self->fParCAOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_CBOM1 = abspowerDBL(self->fParCBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	  fTempCorr_CBOM2 = abspowerDBL(self->fParCBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_CSOM1 = abspowerDBL(self->fParCSOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/ 
	  fTempCorr_CSOM2 = abspowerDBL(self->fParCSOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_DBOM1 = abspowerDBL(self->fParDBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	  fTempCorr_DBOM2 = abspowerDBL(self->fParDBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_MBOM1 = abspowerDBL(self->fParMBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	  fTempCorr_MBOM2 = abspowerDBL(self->fParMBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	   
	}
	
		if (self->iTempCorr == 3)
	{
      fTempCorr_CAOM1 = abspowerDBL(self->fParCAOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/ // abspowerDBL() do the same calculation as abspower()
	  fTempCorr_CAOM2 = abspowerDBL(self->fParCAOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_CBOM1 = abspowerDBL(self->fParCBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	  fTempCorr_CBOM2 = abspowerDBL(self->fParCBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_CSOM1 = abspowerDBL(self->fParCSOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/ 
	  fTempCorr_CSOM2 = abspowerDBL(self->fParCSOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_DBOM1 = abspowerDBL(self->fParDBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	  fTempCorr_DBOM2 = abspowerDBL(self->fParDBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
      fTempCorr_MBOM1 = abspowerDBL(self->fParMBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	  fTempCorr_MBOM2 = abspowerDBL(self->fParMBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0)); /* exponential*/
	   
	}
	/* Pool specific temperature functions end*/ 
	//End of Hong
	
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (double)-31622770;    //pF = 6.5
    wmin = WATER_CONTENT(f1);
    f1= (double)-3160;        //pF = 2.5
    wlow = WATER_CONTENT(f1);
    f1= (double)-316;         //pF = 1.5
    whigh = WATER_CONTENT(f1);
    f1= (double)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(double)0,  wlow, (double)1 , whigh, (double)1, 
							wmax, pPA->fMinerSatActiv);


   /* Original Reduktionsfunktionen 
    1. Temperatur-Reduktionsfunktion*/
	if (pHL->fSoilTemp >= T1 ) 
       corr.Temp = (double) exp((double)0.47 - (double)0.027 * (double)pHL->fSoilTemp
               + (double)0.00193 * (double)pHL->fSoilTemp * (double)pHL->fSoilTemp);
            
    else if (pHL->fSoilTemp >= T0) 
       corr.Temp = (double)0.1 * pHL->fSoilTemp;
                   
    else 
       corr.Temp = (double)0.0;                        
 

  /* 3. Tongehalt-Reduktonsfunktion*/
    if (pSL->fClay <= C0)
	   corr.Lehm = (double)1.0 - C1 * pSL->fClay;

     else
	   corr.Lehm = (double)1.0 - C1 * C0;
	 /* Reduktionsfunktion Ende */


	/******************************************************************/
    /*    C  Mineralisierungsraten (dC/dt )                           */
    /******************************************************************/
	
	/* 1. CN Werte berechnen */
	fCN_FOMSlow     = (pCL->fNFOMSlow && pCL->fCFOMSlow) ?
    	pCL->fCFOMSlow/pCL->fNFOMSlow
    	:fCN_FOMSlow; 

	fCN_FOMFast     = (pCL->fNFOMFast && pCL->fCFOMFast) ?
    	pCL->fCFOMFast/pCL->fNFOMFast
    	:fCN_FOMFast; 

  
 //Wenn der CN Wert der mikrobiellen Biomasse variabel sein soll 
 //Hong: re-activated for Scott Demyan:
 	fCN_MicBiomSlow     = (pCL->fNMicBiomSlow && pCL->fCMicBiomSlow) ?
    	pCL->fCMicBiomSlow/pCL->fNMicBiomSlow
    	:fCN_MicBiomSlow; 
  	
 	fCN_MicBiomFast     = (pCL->fNMicBiomFast && pCL->fCMicBiomFast) ?
    	pCL->fCMicBiomFast/pCL->fNMicBiomFast
    	:fCN_MicBiomFast; 
		
//Hong: added for Scott Demyan:		
		//SG 20160802
 	fCN_MicBiomDenit     = (pCL->fNMicBiomDenit && pCL->fCMicBiomDenit) ?
    	pCL->fCMicBiomDenit/pCL->fNMicBiomDenit
    	:fCN_MicBiomDenit; 	
 //End of hong

	fCN_HumusSlow     = (pCL->fNHumusSlow && pCL->fCHumusSlow) ?
    	pCL->fCHumusSlow/pCL->fNHumusSlow
    	:fCN_HumusSlow; 
	
	fCN_HumusFast     = (pCL->fNHumusFast && pCL->fCHumusFast) ?
    	pCL->fCHumusFast/pCL->fNHumusFast
    	:fCN_HumusFast; 
	
	/* Original-Abbaurraten, lokal veränderbar
    pCL->fFOMSlowDecMaxR      = (double)0.05;
    pCL->fFOMFastDecMaxR      = (double)0.005;
    pCL->fHumusSlowMaxDecMaxR = (double)0.00014;
    pCL->fHumusFastMaxDecMaxR = (double)0.0000027;*/
//Hong: out annotated for Scott Demyan:	
    //pCL->fMicBiomSlowDecMaxR  = BOM1death + BOM1maintenance;
    //pCL->fMicBiomFastDecMaxR  = BOM2death + BOM2maintenance;
    
    /* 1. Kohlenstoff-Mineralisierungs-Raten (dC/dt)*/
    fCAOM1DecayR    = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR       * corr.Temp * corr.Feucht;
    fCAOM2DecayR    = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR       * corr.Temp * corr.Feucht;
    fCBOM1DecayR    = pCL->fCMicBiomSlow * pCL->fMicBiomSlowDecMaxR   * corr.Temp * corr.Feucht * corr.Lehm;
    fCBOM2DecayR    = pCL->fCMicBiomFast * pCL->fMicBiomFastDecMaxR   * corr.Temp * corr.Feucht;
    fCSOM1DecayR    = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR  * corr.Temp * corr.Feucht * corr.Lehm;
    fCSOM2DecayR    = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR  * corr.Temp * corr.Feucht * corr.Lehm;

    fBOM1_DeathC    = pCL->fCMicBiomSlow * BOM1death                  * corr.Temp * corr.Feucht * corr.Lehm;
	fBOM2_DeathC    = pCL->fCMicBiomFast * BOM2death                  * corr.Temp * corr.Feucht;
    fBOM1_MainC     = pCL->fCMicBiomSlow * BOM1maintenance            * corr.Temp * corr.Feucht * corr.Lehm;
    fBOM2_MainC     = pCL->fCMicBiomFast * BOM2maintenance            * corr.Temp * corr.Feucht;

    //Hong: Temperaturabhängigkeit für DAISY Abbauraten (für Scott Demyan)
	if (self->iTempCorr == 1)
	{
    fCAOM1DecayR    = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR       * fTempCorr_CAOM1 * corr.Feucht;
    fCAOM2DecayR    = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR       * fTempCorr_CAOM2 * corr.Feucht;
    fCBOM1DecayR    = pCL->fCMicBiomSlow * pCL->fMicBiomSlowDecMaxR   * fTempCorr_CBOM1 * corr.Feucht * corr.Lehm;
    fCBOM2DecayR    = pCL->fCMicBiomFast * pCL->fMicBiomFastDecMaxR   * fTempCorr_CBOM2 * corr.Feucht;
    fCSOM1DecayR    = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR  * fTempCorr_CSOM1 * corr.Feucht * corr.Lehm;
    fCSOM2DecayR    = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR  * fTempCorr_CSOM2 * corr.Feucht * corr.Lehm;

    fBOM1_DeathC    = pCL->fCMicBiomSlow * BOM1death                  * fTempCorr_DBOM1 * corr.Feucht * corr.Lehm;
	fBOM2_DeathC    = pCL->fCMicBiomFast * BOM2death                  * fTempCorr_DBOM1 * corr.Feucht;
    fBOM1_MainC     = pCL->fCMicBiomSlow * BOM1maintenance            * fTempCorr_MBOM1 * corr.Feucht * corr.Lehm;
    fBOM2_MainC     = pCL->fCMicBiomFast * BOM2maintenance            * fTempCorr_MBOM2 * corr.Feucht;
	}
	if ((double)pHL->fSoilTemp > 10 )
	{	
	if (self->iTempCorr == 3)
	{
    fCAOM1DecayR    = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR       * fTempCorr_CAOM1 * corr.Feucht;
    fCAOM2DecayR    = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR       * fTempCorr_CAOM2 * corr.Feucht;
    fCBOM1DecayR    = pCL->fCMicBiomSlow * pCL->fMicBiomSlowDecMaxR   * fTempCorr_CBOM1 * corr.Feucht * corr.Lehm;
    fCBOM2DecayR    = pCL->fCMicBiomFast * pCL->fMicBiomFastDecMaxR   * fTempCorr_CBOM2 * corr.Feucht;
    fCSOM1DecayR    = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR  * fTempCorr_CSOM1 * corr.Feucht * corr.Lehm;
    fCSOM2DecayR    = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR  * fTempCorr_CSOM2 * corr.Feucht * corr.Lehm;

    fBOM1_DeathC    = pCL->fCMicBiomSlow * BOM1death                  * fTempCorr_DBOM1 * corr.Feucht * corr.Lehm;
	fBOM2_DeathC    = pCL->fCMicBiomFast * BOM2death                  * fTempCorr_DBOM1 * corr.Feucht;
    fBOM1_MainC     = pCL->fCMicBiomSlow * BOM1maintenance            * fTempCorr_MBOM1 * corr.Feucht * corr.Lehm;
    fBOM2_MainC     = pCL->fCMicBiomFast * BOM2maintenance            * fTempCorr_MBOM2 * corr.Feucht;
	}
	}
    //End of Hong


   /*************************************************************************/
   /*  Berechnung der Ammonium Bilanz bei jedem relvanten Poolübergang      */
   /*  Summe positiv = Nettomineralisierung                                 */
   /*  Summe negativ = Nettoimmobilisierung                                 */
   /*************************************************************************/	
// below out-notated since 2016.10.06 
/*  
    /*AOM1->BOM1
    help1 = fBOM1 * fCAOM1DecayR * ((double)1.0 / fCN_FOMSlow - fEff / fCN_MicBiomSlow); 
	
   /*AOM1->BOM2
    help2 = ((double)1.0 - fBOM1) * fCAOM1DecayR * ((double)1.0 / fCN_FOMSlow - fEff / fCN_MicBiomFast);

   /*AOM2->BOM2
    help3 = fCAOM2DecayR * ((double)1.0 / fCN_FOMFast - fEff / fCN_MicBiomFast);

   /*BOM1->BOM2
    help4 = fCBOM1DecayR / fCN_MicBiomSlow -  (fSOM2 / fCN_HumusFast + ((double) 1.0 - fSOM2) * fEff / fCN_MicBiomFast) * fBOM1_DeathC;

	/*SOM1->BOM1
    help5 = fCSOM1DecayR * ((double)1.0 / fCN_HumusSlow - fEff / fCN_MicBiomSlow);

   /*BOM2->BOM2
    help6 = fCBOM2DecayR / fCN_MicBiomFast -  (fSOM2 / fCN_HumusFast + ((double) 1.0 - fSOM2) * fEff / fCN_MicBiomFast) * fBOM2_DeathC;

	/*SOM2->BOM1
    help7 = fCSOM2DecayR * ((double)1.0 / fCN_HumusFast - fSOM1 / fCN_HumusSlow - fEff / fCN_MicBiomSlow + fEff * fSOM1 / fCN_MicBiomSlow);
//////////////////////////////////////////////////////////////////////////////    
*/

//Hong: changed for Scott Demyan et al.2016.10.06
  /*AOM1->BOM1*/
    help1 = fBOM1 * fCAOM1DecayR * ((double)1.0 / fCN_FOMSlow - self->fEff_AOM1 / fCN_MicBiomSlow); 
    
   /*AOM1->BOM2*/
    help2 = ((double)1.0 - fBOM1) * fCAOM1DecayR * ((double)1.0 / fCN_FOMSlow - self->fEff_AOM1 / fCN_MicBiomFast);

   /*AOM2->BOM2*/
    help3 = fCAOM2DecayR * ((double)1.0 / fCN_FOMFast - self->fEff_AOM2 / fCN_MicBiomFast);
	
   /*BOM1->BOM2*/
    help4 = fCBOM1DecayR / fCN_MicBiomSlow -  (fSOM2 / fCN_HumusFast + ((double) 1.0 - fSOM2) * self->fEff_BOM1 / fCN_MicBiomFast) * fBOM1_DeathC;
	
	/*SOM1->BOM1*/
    help5 = fCSOM1DecayR * ((double)1.0 / fCN_HumusSlow - self->fEff_SOM1 / fCN_MicBiomSlow);

   /*BOM2->BOM2*/
    help6 = fCBOM2DecayR / fCN_MicBiomFast -  (fSOM2 / fCN_HumusFast + ((double) 1.0 - fSOM2) * self->fEff_BOM2 / fCN_MicBiomFast) * fBOM2_DeathC;
	
   /*SOM2->BOM1*/
    help7 = fCSOM2DecayR * ((double)1.0 / fCN_HumusFast - fSOM1 / fCN_HumusSlow - self->fEff_SOM2 / fCN_MicBiomSlow + self->fEff_SOM2 * fSOM1 / fCN_MicBiomSlow);
// End of Hong	

   /*Summe über alle relevanten Flüsse*/
	help8 = help1 + help2 + help3 + help4 + help5 + help6 + help7;

	/*Entscheidung ob Mineralisierung oder Immobilisierung*/
	fToNH4R = (help8 > 0) ? help8 : 0;
    fImmR   = (help8 < 0) ? (double)-1 * help8 : 0;


   /*************************************************************/
   /*     Reduktion des AOM-Abbaus bei Nmin-Mangel!             */
   /*************************************************************/

	if(fImmR)
	{
     /*Maximale Immobilisierungsraten*/
  	 fNH4ImmN = fNH4ImmMaxR * pCL->fNH4N;
     fNO3ImmN = fNO3ImmMaxR * pCL->fNO3N;
	 
     /*Berechnung des Reduktionsfaktors*/  	
    fRed = (double)1.0;                      // Reduktionsfaktor =1 => keine Reduktion 

	 if (fImmR  > (fNH4ImmN + fNO3ImmN))
          fRed = (fNH4ImmN + fNO3ImmN) / fImmR;
	    
     help1 = fRed * help1;
     help2 = fRed * help2;
	 
	 //Added by Hong on 20180124: it was missed before, supposed to be a implementation bug!!!
	 help3 = fRed * help3;
	 //End of Hong
	 
	 fCAOM1DecayR  = fRed * fCAOM1DecayR;
	 fCAOM2DecayR  = fRed * fCAOM2DecayR;

	 help8 = help1 + help2 + help3 + help4 + help5 + help6 + help7;
     fImmR = help8 * (double)-1;
	}	
	
	/*Übertragung auf globale Variablen*/
 	pCL->fLitterMinerR  = fCAOM1DecayR/fCN_FOMSlow;
	pCL->fManureMinerR  = fCAOM2DecayR/fCN_FOMFast;
    pCL->fHumusMinerR   = fCSOM1DecayR/fCN_HumusSlow + 
	                      fCSOM2DecayR/fCN_HumusFast;
  
	pCL->fNLitterImmobR = ((help1 + help2) < 0) ? (double)-1 * (help1 + help2) : 0;
    pCL->fNManureImmobR = (help3 < 0) ? (double)-1 * help3 : 0;

	pCL->fNImmobR = fImmR;
    pCL->fMinerR = fToNH4R; 
	//pCL->fMinerR = pCL->fLitterMinerR +pCL->fManureMinerR +pCL->fHumusMinerR; //Added by Hong on 20190904
	
   /********************************************************************************/
   /*    Veraenderung der C-Pools pro Zeitschritt                                  */ 
   /*    Da CN-Verhältnis in den Pools konstant ist, ist eine separate             */
   /*    Modellierung des N redundant.                                             */
   /********************************************************************************/
    
	/*Gesamt C-Mengen vor dem Zeitschritt*/
	fCTotal0 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCMicBiomSlow
              + pCL->fCMicBiomFast + pCL->fCHumusSlow + pCL->fCHumusFast
              + pCL->fCO2C;

    pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_FOMSlow;
    pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_FOMFast; 
    pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_MicBiomSlow;
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_MicBiomFast;
	//Hong: added for Scott Demyan:
	pCL->fNMicBiomDenit= pCL->fCMicBiomDenit / fCN_MicBiomDenit;
	//End of Hong
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / fCN_HumusSlow;
    pCL->fNHumusFast   = pCL->fCHumusFast   / fCN_HumusFast;



	fNTotal0 = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNMicBiomSlow +
			   pCL->fNMicBiomFast + pCL->fNHumusSlow + pCL->fNHumusFast +
               pCL->fNH4N + pCL->fNO3N;

	/* Die Veränderung in den Pools pro Zeitschritt (direkt, ohne Berechnung der Flüsse) */
    pCL->fCFOMSlow     -= fCAOM1DecayR * DeltaT;
    
	pCL->fCFOMFast     -= fCAOM2DecayR * DeltaT; 

/*  // out-noted since 2016.10.06  
	pCL->fCMicBiomSlow += (fEff * (fCSOM1DecayR + ((double)1.0-fSOM1) * fCSOM2DecayR
	                       + fBOM1 * fCAOM1DecayR) - fCBOM1DecayR) * DeltaT;
*/		
    // Hong:changed for Scott Demyan et al. 2016.10.06 
    pCL->fCMicBiomSlow += (self->fEff_SOM1 * fCSOM1DecayR + ((double)1.0-fSOM1) * self->fEff_SOM2 * fCSOM2DecayR
	                       + fBOM1 * fCAOM1DecayR * self->fEff_AOM1 - fCBOM1DecayR) * DeltaT;    
	// End of Hong
			   						   
    pCL->fCHumusSlow   += (fSOM1*fCSOM2DecayR - fCSOM1DecayR) * DeltaT;
	
/* // out-noted since 2016.10.06
    pCL->fCMicBiomFast += (fEff * (((double)1.0-fSOM2) * (fBOM1_DeathC + fBOM2_DeathC) 
                          + ((double)1.0-fBOM1) * fCAOM1DecayR + fCAOM2DecayR) - fCBOM2DecayR) * DeltaT; 
*/
    // Hong:changed for Scott Demyan et al.2016.10.06 
    pCL->fCMicBiomFast += ((((double)1.0-fSOM2) * (self->fEff_BOM1*fBOM1_DeathC + self->fEff_BOM2*fBOM2_DeathC) 
                          + ((double)1.0-fBOM1) * fCAOM1DecayR * self->fEff_AOM1 + fCAOM2DecayR * self->fEff_AOM2) - fCBOM2DecayR) * DeltaT;
    // End of Hong
	
    pCL->fCHumusFast   += (fSOM2 * (fBOM1_DeathC + fBOM2_DeathC) - fCSOM2DecayR) * DeltaT;
	
/* // out-noted since 2016.10.06
    pCL->fCO2ProdR     = (((double)1.0 - fEff) * (fCAOM1DecayR + fCAOM2DecayR + fCSOM1DecayR + fCSOM2DecayR) + fCBOM1DecayR + fCBOM2DecayR
	                      - (fSOM1 - fEff * fSOM1) * fCSOM2DecayR - (fEff + fSOM2 - fEff * fSOM2) * (fBOM1_DeathC + fBOM2_DeathC));
*/
    // Hong:changed for Scott Demyan et al. 2016.10.06 
	pCL->fCO2ProdR     = ((double)1.0 - self->fEff_AOM1) * fCAOM1DecayR + ((double)1.0 - self->fEff_AOM2) * fCAOM2DecayR + ((double)1.0 - self->fEff_SOM1) * fCSOM1DecayR + ((double)1.0 - self->fEff_SOM2) *fCSOM2DecayR + fCBOM1DecayR + fCBOM2DecayR
	                      - (fSOM1 - self->fEff_SOM2 * fSOM1) * fCSOM2DecayR - (self->fEff_BOM1 + fSOM2 - self->fEff_BOM1 * fSOM2) * fBOM1_DeathC - (self->fEff_BOM2 + fSOM2 - self->fEff_BOM2 * fSOM2) * fBOM2_DeathC;
    // End of Hong
        
	pCL->fCO2C += pCL->fCO2ProdR * DeltaT;

   /*Die Raten pro Zeitschritt, eigentlich überflüssig aber für Punkt 4 und 5 nötig */
   /*Eventuell mit einem Fehler, da bei diesen Flüssen momentan keine Sterberate be-*/
   /*rücksichtigt wird.                                                             */

/* //out-noted since 2016.10.06
    fCAOM1ToBOM1R    = fEff * fBOM1 * fCAOM1DecayR;
    fCAOM1ToBOM2R    = fEff * ((double)1.0 - fBOM1) * fCAOM1DecayR;
    fCAOM1ToCO2R     = ((double)1.0 - fEff) * fCAOM1DecayR;

	fCAOM2ToBOM2R    = fEff * fCAOM2DecayR;
    fCAOM2ToCO2R     = ((double)1.0 - fEff) * fCAOM2DecayR;
		
		  
	fCBOM1ToBOM2R    = fEff *((double)1.0 - fSOM2) * fCBOM1DecayR;
    fCBOM1ToSOM2R    = fSOM2 * fCBOM1DecayR;
    fCBOM1ToCO2R     = ((double)1.0 - fEff) * ((double)1.0 - fSOM2) * fCBOM1DecayR;

    fBOM2InternC    = fEff * ((double)1.0 - fSOM2)* fCBOM2DecayR;  
    fBOM2ToSOM2C    = fSOM2 * fCBOM2DecayR;
    fBOM2ToCO2C     = ((double)1.0 - fEff) * ((double)1.0 - fSOM2) * fCBOM2DecayR;

    fSOM1ToBOM1C    = fEff * fCSOM1DecayR;
    fSOM1ToCO2C     = ((double)1.0 - fEff) * fCSOM1DecayR;

	fSOM2ToBOM1C    = fEff * ((double)1.0 - fSOM1) * fCSOM2DecayR;
    fSOM2ToSOM1C    = fSOM1 * fCSOM2DecayR;
    fSOM2ToCO2C     = ((double)1.0 - fEff) * ((double)1.0 - fSOM1) * fCSOM2DecayR;
*/
    // Hong:changed for Scott Demyan et al.	2016.10.06 	
	fCAOM1ToBOM1R    = self->fEff_AOM1 * fBOM1 * fCAOM1DecayR;
    fCAOM1ToBOM2R    = self->fEff_AOM1 * ((double)1.0 - fBOM1) * fCAOM1DecayR;
    fCAOM1ToCO2R     = ((double)1.0 - self->fEff_AOM1) * fCAOM1DecayR;
	
	fCAOM2ToBOM2R    = self->fEff_AOM2 * fCAOM2DecayR;
    fCAOM2ToCO2R     = ((double)1.0 - self->fEff_AOM2) * fCAOM2DecayR;
	
	fCBOM1ToBOM2R    = self->fEff_BOM1 *((double)1.0 - fSOM2) * fCBOM1DecayR;
    fCBOM1ToSOM2R    = fSOM2 * fCBOM1DecayR;
    fCBOM1ToCO2R     = ((double)1.0 - self->fEff_BOM1) * ((double)1.0 - fSOM2) * fCBOM1DecayR;

	fBOM2InternC    = self->fEff_BOM2 * ((double)1.0 - fSOM2)* fCBOM2DecayR;  
    fBOM2ToSOM2C    = fSOM2 * fCBOM2DecayR;
    fBOM2ToCO2C     = ((double)1.0 - self->fEff_BOM2) * ((double)1.0 - fSOM2) * fCBOM2DecayR;
	
	fSOM1ToBOM1C    = self->fEff_SOM1 * fCSOM1DecayR;
    fSOM1ToCO2C     = ((double)1.0 - self->fEff_SOM1) * fCSOM1DecayR;

	fSOM2ToBOM1C    = self->fEff_SOM2 * ((double)1.0 - fSOM1) * fCSOM2DecayR;
    fSOM2ToSOM1C    = fSOM1 * fCSOM2DecayR;
    fSOM2ToCO2C     = ((double)1.0 - self->fEff_SOM2) * ((double)1.0 - fSOM1) * fCSOM2DecayR;
	// End of Hong 


    /* UNKLAR!!!!*/
    /* 4. Veraenderung des BOMD-Pools pro Zeitschritt              */
    /*    Annahme: Während Dekompostierung wachsen Denitrifikanten */
    /*             wie die restliche labile mikrobielle Biomasse   */
      pCL->fCMicBiomDenit += fFD * (fCAOM1ToBOM1R + fCAOM1ToBOM2R + fCAOM2ToBOM2R + fSOM1ToBOM1C + fSOM2ToBOM1C)
                         * DeltaT;

    /* UNKLAR!!!!*/
	/* 5. Festlegung des Csol-Pools  */
    pCL->fCsolC = (fCBOM1DecayR + fCBOM2DecayR) * fSOM2 + fSOM1ToBOM1C + fSOM2ToBOM1C ;
                        
    pCh->pCLayer->fCsolC += pCL->fCsolC;
 
 

    /********************************************************************************/
    /*        Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt        */
    /********************************************************************************/

    /* 1. Ammonium-Netto-Mineralisierung/-Immobilisierung */	
    if (fToNH4R > 0) //Mineralisierung
	{
      pCL->fNH4N += fToNH4R * DeltaT;  
	} 
    else             //Immobilisierung
     { 
	  fNH4NR = fImmR * RelAnteil(pCL->fNH4N,pCL->fNO3N);  
      fNO3NR = fImmR * RelAnteil(pCL->fNO3N,pCL->fNH4N);  	
//	 fNH4NR = 1.0;
//	 fNO3NR = 1.0;     
      pCL->fNH4N -= fNH4NR * DeltaT;  
	  pCL->fNO3N -= fNO3NR * DeltaT;

	 }

    /* 2. Werte in den N-Pools */
    pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_FOMSlow;
    pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_FOMFast; 
    pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_MicBiomSlow;
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_MicBiomFast;
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / fCN_HumusSlow;
    pCL->fNHumusFast   = pCL->fCHumusFast   / fCN_HumusFast;


    /********************************************************************************/
    /* Übertragen der Veränderungen auf die Pools Litter, Manure,Humus u.           */
    /********************************************************************************/

    pCL->fCLitter = pCL->fCFOMSlow; 
    pCL->fNLitter = pCL->fNFOMSlow; 
 
    pCL->fCManure = pCL->fCFOMFast; 
    pCL->fNManure = pCL->fNFOMFast; 

    pSL->fCHumus = pCL->fCMicBiomSlow + pCL->fCMicBiomFast
                   + pCL->fCHumusSlow + pCL->fCHumusFast;
	if (pSL->fCHumus<EPSILON)
		{
			pSL->fCHumus=EPSILON;
		}
    pSL->fNHumus = pCL->fNMicBiomSlow + pCL->fNMicBiomFast
                   + pCL->fNHumusSlow + pCL->fNHumusFast;

	pCL->fCSoilMicBiom = pCL->fCMicBiomSlow + pCL->fCMicBiomFast;
	pCL->fNSoilMicBiom = pCL->fNMicBiomSlow + pCL->fNMicBiomFast;
    /*Übertragen auf ExpertN-Pools Ende */



  /*Massenbilanz*/ 
  fCTotal1 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCMicBiomSlow
              + pCL->fCMicBiomFast + pCL->fCHumusSlow + pCL->fCHumusFast
              + pCL->fCO2C;

  fNTotal1 = pCL->fNFOMSlow + pCL->fNFOMFast +   
			 pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
			 pCL->fNHumusSlow + pCL->fNHumusFast + 
             pCL->fNH4N + pCL->fNO3N;

    fCDiff = fCTotal1 - fCTotal0;    
    self->fCDiffSum += fCDiff;

    fNDiff = fNTotal1 - fNTotal0;
    self->fNDiffSum += fNDiff;
	
  }

	return RET_SUCCESS;
}


//Added by Hong on 20180724:
/*************************************************************************************/
/* Procedur    :   SurfaceMineralisierung                                            */
/* Beschreibung:   Mineralisierung der Oberflächenpools                              */
/*                 Methode Grundlage SOILN / NITS mit Veränderungen                  */
/*                                                                                   */
/*              GSF/ab: Axel Berkenkamp         07.06.01                             */
/*                  ab: Veränderung des C-Abbaus bei Nmin-Mangel 28.08.01            */
/*                                                                                   */
/*                                                                                   */
/*                                                                                   */
/*************************************************************************************/
/*	veränd. Var.	pCP->fCLitterSurf          pCP->fNLitterSurf                     */
/*					pCP->fCManureSurf          pCP->fNManureSurf                     */
/*					pCP->fCHumusSurf           pCP->fNHumusSurf                      */
/*                  pCL->fCO2C                 pCP->fNH4NSurf                        */
/*					pCL->fCO2ProdR(?)          pCP->fNO3NSurf                        */
/*                  pCP->fCNLitterSurf         pCP->fCNManureSurf                    */ 
/*                  pCL->fHumusMinerR(?)       pCL->fHumusImmobR(?)                  */
/*					pCL->fLitterMinerR(?)      pCL->fLitterImmobR(?)                 */
/*                  pCL->fManureMinerR(?)      pCL->fManureImmobR(?)                 */
/*					corr.Temp                  corr.Feucht                           */
/*                                                                                   */
/*************************************************************************************/
 int SurfaceMiner(daisy_miner *self)
 {
  expertn_modul_base *xpn = &(self->parent);	 
	 
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr ={(double)1};
  
  /******************************************************************/
  /*                Variablendeklaration                            */
  /******************************************************************/

  /*Hilfsvariablen*/
  double f1,f2,f3;
  double fRedLit,fRedMan,fEffNew;
  double NoImmLit, NoImmMan;
  double fMinerEffFac,fMicBiomCN,fMinerHumFac;

  /* Konstanten (1/dt) */
  double fNH4ImmMaxK = (double)0.1;
  double fNO3ImmMaxK = (double)0.1;
  double fLitterImmK,fManureImmK;
  double fLitterToNH4K,fManureToNH4K;

  /* Raten für die C und N-Flüsse (dC/dt bzw. dN/dt) */
  double fHumusMinerMaxR;  
  double fLitterMinerMaxR; 
  double fManureMinerMaxR; 
  double fNToLitterR;
  double fNH4ImmR,fNO3ImmR;
  double fCLitterToHumusR,fCLitterToLitterR,fCLitterToCO2R;
  double fCManureToHumusR,fCManureToLitterR,fCManureToCO2R;
  double fCHumusToCO2R;
  
  double fNH4ToLitterR,fNO3ToLitterR;
  double fNLitterToHumusR,fNManureToHumusR,fNManureToLitterR;
  double fNHumusToNH4R,fNLitterToNH4R,fNManureToNH4R;
  
  /*Variablen für die Surface-Flüsse (eigentlich auch Raten) */
  double fCLitterSurfDecay, fCManureSurfDecay, fCHumusSurfDecay;
  double fNLitterSurfDecay, fNManureSurfDecay, fNHumusSurfDecay;

  /*Variablen für die Massenbilanz*/
  double fCTotalSurf,fNTotalSurf;
  double fCDiffSurf,fNDiffSurf;
  static double fCDiffSum, fNDiffSum;
  /* Variablendeklaration Ende  */




  N_ZERO_LAYER /* Setzten der Pointer auf die nullte Schicht*/


  /******************************************************************/
  /*                          Reduktionsfunktionen                  */
  /******************************************************************/
  
  /* 1. Temperatur: Ansatz SOILN, Bezugswert: Tagesmittelwert der Luft*/ 
  corr.Temp = abspowerDBL(pPA->fMinerQ10,((xpn->pCl->pWeather->fTempAve - pPA->fMinerTempB)/(double)10.0));//Hong: abspowerDBL is the same abspower


  /* 2. Feuchte: Ansatz RESMAN, Bezugswert: Das wassergefüllte Porenvolumen
     der obersten Bodenschicht, Optimalwert WFPS = 60% */
  f1 = (pWL->pNext->fContAct + pWL->pNext->fIce) / pSL->pNext->fPorosity;

  if (f1 < (double)0.6)
    corr.Feucht = f1 / (double)0.6;

  else
    corr.Feucht = (double) 0.6 / f1;
  /*Reduktionsfunktion Ende*/


  /******************************************************************/
  /*    C und N -Mineralisierungsraten (dC/dt bzw. dN/dt)           */
  /******************************************************************/

fHumusMinerMaxR  = pCL->pNext->fHumusMinerMaxR;
fLitterMinerMaxR = pCL->pNext->fLitterMinerMaxR;
fManureMinerMaxR = pCL->pNext->fManureMinerMaxR;
fMinerEffFac     = pPA->pNext->fMinerEffFac;
fMicBiomCN       = pCL->pNext->fMicBiomCN;
fMinerHumFac     = pPA->pNext->fMinerHumFac;

  fCHumusSurfDecay  = pCP->fCHumusSurf  * fHumusMinerMaxR  * corr.Temp * corr.Feucht;
  fCLitterSurfDecay = pCP->fCLitterSurf * fLitterMinerMaxR * corr.Temp * corr.Feucht;
  fCManureSurfDecay = pCP->fCManureSurf * fManureMinerMaxR * corr.Temp * corr.Feucht;

  fNHumusSurfDecay  = pCP->fNHumusSurf  * fHumusMinerMaxR  * corr.Temp * corr.Feucht;
  fNLitterSurfDecay = pCP->fNLitterSurf * fLitterMinerMaxR * corr.Temp * corr.Feucht;
  fNManureSurfDecay = pCP->fNManureSurf * fManureMinerMaxR * corr.Temp * corr.Feucht;
  /* C und N -Mineralisierungsraten Ende */

  /******************************************************************/
  /* Entscheidung Immobilisierung/Mineralisierung beim Litter u.    */
  /* Manure Abbau. Der Wert hängt ab vom CN Wert der abgebauten     */
  /* Substanz, dem CN Wert der aufgebauten Substanz (Mikrobielle    */ 
  /* Biomasse und Humus) sowie dem Effektivitätsfaktor mit der die  */ 
  /* Mikros Kohlenstoff verwerten können.                           */
  /* Hilfsvariable positiv = Nettomineralisierung                   */
  /* Hilfsvariable negativ = Nettoimmobilisierung                   */                            
  /******************************************************************/

  /* CN-Werte */
  pCP->fCNLitterSurf = (pCP->fNLitterSurf > EPSILON)?
    	pCP->fCLitterSurf / pCP->fNLitterSurf
    	:(double)0.1; 
            
  pCP->fCNManureSurf = (pCP->fNManureSurf > EPSILON)?
    	pCP->fCManureSurf / pCP->fNManureSurf
      	:(double)0.1; 

  /*Mineralisierungs bzw. Immobilisierungsfaktor*/
   if (pCP->fCNLitterSurf > (double)0.1)
   {
    f2     = (double)1 / pCP->fCNLitterSurf - fMinerEffFac / fMicBiomCN;
	fLitterToNH4K = (f2 > 0)? f2 : 0;
    fLitterImmK   = (f2 < 0)? (double)-1 * f2 : 0;
   }

  else
   {
    fLitterToNH4K = (double)0;
    fLitterImmK   = (double)0;
   }

  if (pCP->fCNManureSurf > (double)0.1)
   {
    f3     = (double)1 / pCP->fCNManureSurf - fMinerEffFac / fMicBiomCN;
    fManureToNH4K = (f3 > 0)? f3 : 0;
    fManureImmK   = (f3 < 0)? (double)-1 * f3 : 0;
   }
  
  else
   {
    fManureToNH4K = (double)0;
    fManureImmK   = (double)0;
   }
  /* Entscheidung Immobilisierung/Mineralisierung Ende*/


  /*************************************************************************/
  /* Wenn die Immobilisierung mehr N-Bedarf entwickelt als mineralisches N */
  /* zur Verfügung steht, wird das Wachstum der Mikroorganismen und die    */
  /* Bildung von Humus unterbunden, sowie der C-Abbau reduziert            */
  /*************************************************************************/

  fNToLitterR = fLitterImmK * fCLitterSurfDecay + fManureImmK * fCManureSurfDecay;

  /*Maximale Immobilisierungsraten*/
  fNH4ImmR = fNH4ImmMaxK * pCP->fNH4NSurf;
  fNO3ImmR = fNO3ImmMaxK * pCP->fNO3NSurf;

  if (fNToLitterR  > (fNH4ImmR + fNO3ImmR))
   {  
    if (fLitterImmK)
	 {
	  NoImmLit = (double)0.0;
      fEffNew = fMicBiomCN / pCP->fCNLitterSurf;
      fRedLit = min(fEffNew,fMinerEffFac);
      fLitterToNH4K = (double)0;
      fLitterImmK   = (double)0;
     }
	else
	 {
	  NoImmLit = (double)1.0;
	  fRedLit  = (double)1.0;
	 }

    if (fManureImmK)
	 {
	  NoImmMan = (double)0.0;
      fEffNew = fMicBiomCN / pCP->fCNManureSurf;
      fRedMan = min(fEffNew,fMinerEffFac);
      fManureToNH4K = (double)0;
      fManureImmK   = (double)0;
     }
    
	else
	 {
	  NoImmMan = (double)1.0;
	  fRedMan  = (double)1.0;
	 }

	fNToLitterR = (double)0.0; 
   }
  
  else
   {
    fRedLit = (double)1.0;
	fRedMan = (double)1.0;
	NoImmLit = (double)1.0;
	NoImmMan = (double)1.0;
   }
   
  /*Berechnung des Reduktionsfaktors alte Variante  	
  fRed = (double)1.0;                      // Reduktionsfaktor =1 => keine Reduktion 

    fRed = (fNH4ImmR + fNO3ImmR) / fNToLitterR;

  if (fLitterImmK)
  {   
   fCLitterSurfDecay = fRed * fCLitterSurfDecay;
   fNLitterSurfDecay = fRed * fNLitterSurfDecay;
  }

  if (fManureImmK)
  {
   fCManureSurfDecay = fRed * fCManureSurfDecay;
   fNManureSurfDecay = fRed * fNManureSurfDecay;
  }

  fNToLitterR        = fRed * fNToLitterR;
  Berechnung Reduktion des Abbaus Ende */


  /******************************************************************/
  /* Veraenderung des C-Pools pro Zeitschritt                       */
  /******************************************************************/
    
  /* 1. Gesamt C vor dem Zeitschritt */
  fCTotalSurf = pCP->fCHumusSurf + pCP->fCLitterSurf + pCP->fCManureSurf + pCL->fCO2C;

  /* 2. Abbau des C-Litter Pools pro Zeitschritt */
  
  if(!NoImmLit)
   {
    fCLitterToCO2R    = fCLitterSurfDecay * fRedLit;
    fCLitterToHumusR  = (double)0.0;
    fCLitterToLitterR = (double)0.0;
   }

  else
   {
    fCLitterToHumusR  = fCLitterSurfDecay * fMinerEffFac * fMinerHumFac;
    fCLitterToCO2R    = fCLitterSurfDecay * ((double)1.0 - fMinerEffFac);
    fCLitterToLitterR = fCLitterSurfDecay * fMinerEffFac * ((double)1.0 - fMinerHumFac);
   }
  
  /* 3. Abbau des C-Manure Pools pro Zeitschritt */
  
  if(!NoImmMan)
   {  
    fCManureToCO2R    = fCManureSurfDecay * fRedMan;
    fCManureToHumusR  = (double)0.0;
    fCManureToLitterR = (double)0.0;
   }

  else
   {
    fCManureToHumusR  = fCManureSurfDecay * fMinerEffFac * fMinerHumFac;
    fCManureToCO2R    = fCManureSurfDecay * ((double)1.0 - fMinerEffFac);
    fCManureToLitterR = fCManureSurfDecay * fMinerEffFac * ((double)1.0 - fMinerHumFac);
   }

  /* 4. Abbau des C-Humus Pools */
  fCHumusToCO2R     = fCHumusSurfDecay;
  
  /* 5. Zunahme im CO2 Pool */
  pCL->fCO2ProdR = fCLitterToCO2R +  fCManureToCO2R + fCHumusToCO2R;

  /* 6. Veraenderung in den C-Pools */
  pCP->fCLitterSurf -= (fCLitterToHumusR + fCLitterToCO2R - fCManureToLitterR) * DeltaT;
  pCP->fCManureSurf -= (fCManureToHumusR + fCManureToCO2R + fCManureToLitterR) * DeltaT;
  pCP->fCHumusSurf  += (fCLitterToHumusR + fCManureToHumusR - fCHumusToCO2R) * DeltaT;
  pCL->fCO2C        += pCL->fCO2ProdR * DeltaT; // Hong: 0 layer (surface)
  
  //Added by Hong on 20180731
  //if (pCL->fCO2C>0.0)
    {
	  pCP->dCO2SurfEmisCum +=pCL->fCO2ProdR * DeltaT;
     }

  /* Veränderungen im C-Pool Ende */


  /********************************************************************************/
  /*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
  /********************************************************************************/
    
  /* 1. Gesamt N vor dem Zeitschritt */
  fNTotalSurf = pCP->fNHumusSurf + pCP->fNLitterSurf + pCP->fNManureSurf + pCP->fNH4NSurf + pCP->fNO3NSurf;

  /* 2. Immobilisierung im Litter-Pool */
  fNH4ToLitterR = fNToLitterR * RelAnteil(pCP->fNH4NSurf,pCP->fNO3NSurf);
  fNO3ToLitterR = fNToLitterR * RelAnteil(pCP->fNO3NSurf,pCP->fNH4NSurf);

  /* 3. Bildung von mikrobieller Biomasse */
  fNManureToLitterR = fCManureToLitterR / fMicBiomCN;


  /* 4. Humifizierung im Humus-Pool */
  if(!NoImmLit)
   fNLitterToHumusR = (double)0.0;
  else
   fNLitterToHumusR = fCLitterToHumusR / fMicBiomCN;

  if(!NoImmMan)
   fNManureToHumusR = (double)0.0;
  else
   fNManureToHumusR = fCManureToHumusR / fMicBiomCN;
   
  /* 5. Ammonium-Mineralisierung aus Humus, Litter und Manure */
  fNHumusToNH4R  = fNHumusSurfDecay;
  fNLitterToNH4R = fLitterToNH4K * fCLitterSurfDecay;
  fNManureToNH4R = fManureToNH4K * fCManureSurfDecay;

  /* 6. Veränderungen in den N Pools */
  pCP->fNH4NSurf += (fNHumusToNH4R  + fNLitterToNH4R  + fNManureToNH4R -  fNH4ToLitterR) * DeltaT;
  pCP->fNO3NSurf -= fNO3ToLitterR * DeltaT;
  
  pCP->fNLitterSurf += (fNToLitterR + fNManureToLitterR - fNLitterToNH4R - fNLitterToHumusR) * DeltaT;
  pCP->fNManureSurf -= (fNManureToHumusR + fNManureToLitterR + fNManureToNH4R) * DeltaT;
  pCP->fNHumusSurf += (fNLitterToHumusR + fNManureToHumusR - fNHumusToNH4R) * DeltaT;
  /* Veränderungen im N-Pool Ende */

  /* Übertragung auf globale Variablen */
  pCL->fNImmobR = fNToLitterR;
  pCL->fMinerR = fNHumusToNH4R + fNLitterToNH4R + fNManureToNH4R;
  pCL->fHumusMinerR  = fNHumusToNH4R;
  pCL->fLitterMinerR = fNLitterToNH4R;
  pCL->fManureMinerR = fNManureToNH4R;

  /********************************************************************************/
  /* Massenbilanz, die Summe der Veränderungen muss ungefär Null ergeben */
  /********************************************************************************/
  
  fCDiffSurf = pCP->fCHumusSurf + pCP->fCLitterSurf + pCP->fCManureSurf + pCL->fCO2C - fCTotalSurf;
  fNDiffSurf = pCP->fNHumusSurf + pCP->fNLitterSurf + pCP->fNManureSurf + pCP->fNH4NSurf + pCP->fNO3NSurf - fNTotalSurf;
  /*Massenbilanz Ende*/
  
  fCDiffSum += fCDiffSurf;
  fNDiffSum += fNDiffSurf;
  return 1;
 } /*Funktion Ende*/
//End of Hong