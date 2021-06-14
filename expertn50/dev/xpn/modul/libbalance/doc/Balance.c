/**********************************************************************************/
/*                                                                                */
/*  module  :   Cum_Bil1.C                                                        */
/*  purpose :   Kumulation der berechneten Raten und Bilanzierung                 */
/*  date  :     dm, 03.12.96                                                      */
/*              ch, 14.1.97                                                       */
/*                                                                                */
/**********************************************************************************/

// Sammlung aller Header-Dateien:
#include  "xinclexp.h" 
#include  "xh2o_def.h"
#include  "xn_def.h"
#include "xlanguag.h"     

//Module defined include files
//#include "PLANTMOD.H"

// from util_fct.c
extern int WINAPI Message(long, LPSTR);
extern int NewDay(PTIME);
extern int EndDay(PTIME);

int WINAPI cum_bil1(EXP_POINTER);
int WINAPI Set_Day_Zero(EXP_POINTER);

//from fitting.c

extern int bFittingRunning;
extern int bFittingLoaded;
extern void FittingAuxiliary(EXP_POINTER);

#include <stdio.h>
FILE *FF;
#define SQR(x) ((x)*(x))

/**********************************************************************************/
/*                                                                                */
/*  module  :   Cum_Bil1.C                                                        */
/*  purpose :   Kumulation der berechneten Raten und Bilanzierung                 */
/*              für den Wasser- und den Stickstoffhaushalt                        */
/*  date  :     dm, 03.12.96                                                      */
/*                                                                                */
/**********************************************************************************/
/*veränd. Var.		pEV->fPotDay                   pCL->fNImmobCum                */
/*                  pWB->fPotCumEvap               pCP->fNImmobDay                */
/*                  pPl->pPltWater->fCumUptake     pCP->fNImmobCum                */
/*                  pPl->pPltWater->fCumDemand     pCP->fNLitterImmobDay          */
/*					pWB->fProfil                   pCL->fCO2ProdDay               */
/*					pWB->fProfileIce               pCL->fCO2ProdCum               */
/*					pWa->fLeaching                 pCP->fCO2ProdDay               */
/*					pWa->fCumLeaching              pCP->fCO2ProdCum               */
/*					pWB->fInput                    pCL->fUreaHydroDay             */
/*                                                 pCL->fUreaHydroCum             */
/*					pWa->fCumInfiltration          pCP->fUreaHydroDay             */
/*					pWa->pEvap->fActDay            pCP->fUreaHydroCum             */
/*                  pWB->fActCumEvap               pCP->fNH3VolatDay              */
/*                  pWa->fActETCum                                                */                  
/*                  pWa->fPotETCum                                                */
/*                  pWB->fOutput                   pCP->fNH3VolatCum              */
/*					pWB->fBalance                  pCP->fN2OEmisDay               */
/*                                                 pCP->fN2OEmisCum               */
/*                  pCL->fNO3DenitDay              pCP->fUreaLeachDay             */
/*                  pCL->fN2ODenitDay              pCP->fNH4LeachDay              */
/*                  pCL->fNO3DenitCum              pCP->fNO3LeachDay              */
/*                  pCL->fN2ODenitCum              pCP->fN2ODrainDay              */
/*					pCL->fNH4NitrDay               pCP->fUreaLeachCum             */
/*					pCL->fNH4NitrCum               pCP->fNH4LeachCum              */
/*					pCL->fNH4ToN2OCum              pCP->fNO3LeachCum              */
/*					pCP->fNO3DenitDay              pCP->fN2ODrainCum              */
/*					pCP->fN2ODenitDay              pCP->fNTotalLeachDay           */
/*					pCP->fNO3DenitCum              pCP->fNTotalLeachCum           */
/*					pCP->fN2ODenitCum              pCB->fNO3NProfile              */
/*					pCP->fNH4NitrDay               pCB->fNH4NProfile              */
/*					pCP->fNH4NitrCum               pCB->fN2ONProfile              */
/*					pCP->fNH4ToN2OCum              pCB->fN2NProfile               */
/*					pCL->fNManureMinerDay          pCB->fNProfileAct              */
/*					pCL->fNManureMinerCum          pCB->fUreaNProfile             */
/*					pCL->fNLitterMinerDay          pCB->fNOutputCum               */
/*					pCL->fNLitterMinerCum          pCB->fNInputCum                */
/*					pCL->fNHumusMinerDay           pCB->fNBalance                 */
/*					pCL->fNHumusMinerCum                                          */
/*					pCP->fNMinerDay                                               */
/*					pCP->fNMinerCum                                               */
/*					pCL->fNImmobDay                                               */
/**********************************************************************************/
int WINAPI cum_bil1(EXP_POINTER)
{
 #define kgPhaTomgPsqm    (float)100.0

 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER

		PEVAP     	pEV; 
		PCLAYER   	pCL;
		PCPROFILE 	pCP;
		PCBALANCE 	pCB;
		PHLAYER 	pHL;

 float DeltaZ   = pSo->fDeltaZ;
 float DayTime  = pTi->pSimTime->fTimeDay; 
 static float *SoilTempAverage;
               
 static PWMEASURE pWM; //Listen werden im Simulationsverlauf gelesen -> static
 static PNMEASURE pNM; 

 FILE* FWM;
 FILE* FNM;
 static char WM_OutputFileName[40];
 static char NM_OutputFileName[40];

 static float fSquareSumWaterContent[20], fSquareSumNO3[20], fSquareSumNH4[20], fSquareSumN2O[20];
 static float fTotalSquareSumWaterContent, fTotalSquareSumNO3, fTotalSquareSumNH4, fTotalSquareSumN2O;
 float faCont[MAXSCHICHT], 	fActCont, fSquDiffCont;
 float faNO3[MAXSCHICHT], 	fActNO3,  fSquDiffNO3;
 float faNH4[MAXSCHICHT], 	fActNH4,  fSquDiffNH4;
 float faN2O[MAXSCHICHT], 	fActN2O,  fSquDiffN2O;

 static int iWMLayerNotConstant;
 static int iNMLayerNotConstant;
 static float afWMDepthLast[20];
 static float afNMDepthLast[20];

 float fLoDepth, fUpDepth, fLoFrac,	fUpFrac;
 int   i,  iMLayer, iLoNum , iUpNum;
 float fSimDepth;


/*********************************************************************************/
/*                                                                               */
/*                                Wasserbilanz                                   */
/*                                                                               */
/*********************************************************************************/
 
/********************************* Kumulation ************************************/
 
 float bal_old_h2o, f1;
 
 pEV = pWa->pEvap;
 pWB = pWa->pWBalance;
 
 /* Weather: */
 if (NewDay(pTi)&& pCl->pWeather->fRainAmount > EPSILON)
	{
	  pCl->fCumRain += pCl->pWeather->fRainAmount;
	} 

 // potentielle Evaporation
 pEV->fPotDay     += pEV->fPotR * DeltaT;  
 pWB->fPotCumEvap += pEV->fPotR * DeltaT;


 /* RunOff */
 pWa->fRunOffDay += pWa->fRunOffR * DeltaT;                        
 pWa->fRunOffCum += pWa->fRunOffR * DeltaT;                        

 // Berechnung des Flusses (Menge) aus der letzten Schicht
 pWa->fLeaching    += pWa->fPercolR * DeltaT;   
 pWa->fCumLeaching += pWa->fPercolR * DeltaT;   

 // Profilsumme Nullsetzen:
 pWB->fProfil     = (float)0;
 pWB->fProfileIce = (float)0;

 for (H2O_SOIL_LAYERS) //  Schicht i = 1 bis  n-1
    {
     // Profilsumme bilden:
     pWB->fProfil     += pWL->fContAct * DeltaZ;
    
     // Als Bilanzgroesse wird der gravimetrische Eisgehalt verwendet       
     pWB->fProfileIce += pWL->fIce * DeltaZ / (float)1.1;  
     
     pWL->fFluxDay += pWL->fFluxDens  * DeltaT;     
    }


 H2O_ZERO_LAYER
   
   if (pWa->fInfiltR  > (float) 0)                         // 0.te Schicht
      {
        pWB->fInput           += pWa->fInfiltR * DeltaT;          
        pWa->fInfiltDay       += pWa->fInfiltR * DeltaT;          
        pWa->fCumInfiltration += pWa->fInfiltR * DeltaT;        
      }
    
   if (pWa->pEvap->fActR > (float) 0)
      {
        pWa->pEvap->fActDay += pWa->pEvap->fActR * DeltaT;
        pWB->fActCumEvap    += pWa->pEvap->fActR * DeltaT;
		pWa->fActETCum      += pWa->pEvap->fActR * DeltaT;
      }

 // Wasseraufnahme durch Pflanze
   if ((pPl!=NULL))
      {
   if (pPl->pRoot->fUptakeR > (float) 0)
   		{
        pWa->fActTranspCum 				+= pPl->pRoot->fUptakeR * DeltaT;
        pPl->pPltWater->fActTranspDay 	+= pPl->pRoot->fUptakeR * DeltaT;
        pPl->pPltWater->fCumUptake 		+= pPl->pRoot->fUptakeR * DeltaT;
	    pWa->fActETCum                	+= pPl->pRoot->fUptakeR * DeltaT;
        }

		pPl->pPltWater->fPotTranspDay +=  pPl->pPltWater->fPotTranspdt;
		pPl->pPltWater->fCumDemand    +=  pPl->pPltWater->fPotTranspdt;
        
       }

   pWa->fPotETCum += pWa->fPotETdt;     
 
   pWB->fOutput = pWa->fActETCum + pWa->fCumLeaching;


/******************************** Bilanzierung ***********************************/    
    

    bal_old_h2o   = pWB->fBalance;
    
    pWB->fBalance = pWB->fProfil  
                  + pWB->fProfileIce
                  + pWB->fOutput      
                  - pWB->fProfilStart 
                  - pWB->fInput;
                     
    bal_old_h2o = pWB->fBalance - bal_old_h2o; // Gibt den Zuwachs der Bilanz an.
    
    if ((bal_old_h2o > ERROR_TOLERANCE)||(ERROR_TOLERANCE + bal_old_h2o < 0 ))
    {
     #ifdef LOGFILE                            
      char   acdummy[80];

      if ((bal_old_h2o>-1000)&&(bal_old_h2o<1000))
      {
      itoa((int)(bal_old_h2o), acdummy,10);
      strcpy(lpOut,acdummy); 

      strcat(lpOut,"mm "); 

      strcat(lpOut,ERROR_IN_WATER_BALANCE_TXT); 
      Message(0, lpOut);
      }
      else
      {
      strcpy(lpOut,ERROR_IN_WATER_BALANCE_TXT); 
      Message(3, lpOut);
      return -1;
      }
      
      
     #endif
    }

    
/*********************************************************************************/
/*                                                                               */
/*                             Stickstoffbilanz                                  */
/*                                                                               */
/*********************************************************************************/
 
 pCL = pCh->pCLayer;
 pCP = pCh->pCProfile;
 pCB = pCh->pCBalance;
 fSimDepth=(float)0;

 for (pCL = pCh->pCLayer->pNext;
     (pCL->pNext!=NULL);
      pCL = pCL->pNext)
     {
      fSimDepth+=DeltaZ;
/*********************** Denitrifikation und Nitrifikation ***********************/
      
	  //Kumulation über Schichten
      pCL->fNO3DenitDay += pCL->fNO3DenitR * DeltaT;
      pCL->fN2ODenitDay += pCL->fN2ODenitR * DeltaT;
      pCL->fNO3DenitCum += pCL->fNO3DenitR * DeltaT;      
      pCL->fN2ODenitCum += pCL->fN2ODenitR * DeltaT;
      
      pCL->fNH4NitrDay  += pCL->fNH4NitrR  * DeltaT;
      pCL->fNH4NitrCum  += pCL->fNH4NitrR  * DeltaT;     
      pCL->fNH4ToN2OCum += pCL->fNH4ToN2OR * DeltaT;
    
      //Kumulation über das Profil
      pCP->fNO3DenitDay += pCL->fNO3DenitR * DeltaT;
      pCP->fN2ODenitDay += pCL->fN2ODenitR * DeltaT;
      pCP->fNO3DenitCum += pCL->fNO3DenitR * DeltaT; // jetzt Kumulation hier
      pCP->fN2ODenitCum += pCL->fN2ODenitR * DeltaT; 

      pCP->fNH4NitrDay  += pCL->fNH4NitrR * DeltaT;
      pCP->fNH4NitrCum  += pCL->fNH4NitrR * DeltaT;  // jetzt Kumulation hier
    
      pCP->fNH4ToN2OCum += pCL->fNH4ToN2OR * DeltaT; 


/***************************** Mineralisierung ***********************************/

      pCL->fNManureMinerDay += pCL->fManureMinerR * DeltaT;
      pCL->fNManureMinerCum += pCL->fManureMinerR * DeltaT;   
   
      pCL->fNLitterMinerDay += pCL->fLitterMinerR * DeltaT;
      pCL->fNLitterMinerCum += pCL->fLitterMinerR * DeltaT;
      
      pCL->fNHumusMinerDay  += pCL->fHumusMinerR * DeltaT;
      pCL->fNHumusMinerCum  += pCL->fHumusMinerR * DeltaT;
   
      pCP->fNLitterMinerDay += pCL->fLitterMinerR * DeltaT;
      pCP->fNManureMinerDay += pCL->fManureMinerR * DeltaT;
      pCP->fNHumusMinerDay  += pCL->fHumusMinerR * DeltaT;

      pCP->fNLitterMinerCum += pCL->fLitterMinerR * DeltaT;
      pCP->fNManureMinerCum += pCL->fManureMinerR * DeltaT;

      pCP->fNHumusMinerCum  += pCL->fHumusMinerR * DeltaT;

      pCP->fNMinerDay  += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR) * DeltaT;
      pCP->fNMinerCum  += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR) * DeltaT;     

      pCP->fNLitterImmobDay  += pCL->fNLitterImmobR * DeltaT;      
      pCP->fNLitterImmobCum  += pCL->fNLitterImmobR * DeltaT;      
     
      pCP->fNManureImmobDay  += pCL->fNLitterImmobR * DeltaT;      
      pCP->fNManureImmobCum  += pCL->fNManureImmobR * DeltaT;      
     
      pCL->fNImmobDay  += pCL->fNImmobR * DeltaT;       
      pCL->fNImmobCum  += pCL->fNImmobR * DeltaT;       
      pCP->fNImmobDay  += pCL->fNImmobR * DeltaT;      
      pCP->fNImmobCum  += pCL->fNImmobR * DeltaT;
    
	  if(fSimDepth <= (float)300) 
		  pCP->fNetNMinerCum30 += (pCL->fLitterMinerR + pCL->fManureMinerR + pCL->fHumusMinerR 
		                           - pCL->fNLitterImmobR) * DeltaT;
      
	  pCL->fCO2ProdDay += pCL->fCO2ProdR * DeltaT;
      pCL->fCO2ProdCum += pCL->fCO2ProdR * DeltaT;
      pCP->fCO2ProdDay += pCL->fCO2ProdR * DeltaT;      
      pCP->fCO2ProdCum += pCL->fCO2ProdR * DeltaT;

/******************************** Hydrolyse **************************************/
      pCL->fUreaHydroDay += pCL->fUreaHydroR  * DeltaT;   
      pCL->fUreaHydroCum += pCL->fUreaHydroR  * DeltaT;
      
      pCP->fUreaHydroDay += pCL->fUreaHydroR  * DeltaT;   
      pCP->fUreaHydroCum += pCL->fUreaHydroR  * DeltaT;    
     }


/******************************* Volatisation ************************************/
      pCP->fNH3VolatDay += pCP->fNH3VolatR * DeltaT;  
      pCP->fNH3VolatCum += pCP->fNH3VolatR * DeltaT;  


/******************************* CO2-Emission ************************************/
	  pCh->pCLayer->fCO2ProdDay +=pCh->pCLayer->fCO2ProdR * DeltaT;
	  pCh->pCLayer->fCO2ProdCum +=pCh->pCLayer->fCO2ProdR * DeltaT;
      pCP->fCO2EmisDay = pCP->fCO2ProdDay;      
      pCP->fCO2EmisCum = pCP->fCO2ProdCum;
      pCP->fCO2SurfEmisDay = pCh->pCLayer->fCO2ProdDay;      
      pCP->fCO2SurfEmisCum = pCh->pCLayer->fCO2ProdCum;

/******************************* N2O-Emission ************************************/
 if (pCP->fN2OEmisR > 0)	   
     {
      pCP->fN2OEmisCum += pCP->fN2OEmisR  * (float)24e-5 * DeltaT;
      pCP->fN2OEmisDay += pCP->fN2OEmisR  * (float)24e-5 * DeltaT; 
	 }

   
  pCP->fNTotalLeachDay = pCP->fUreaLeachDay
      			       + pCP->fNH4LeachDay
    			       + pCP->fNO3LeachDay;
    			       + pCP->fN2ODrainDay;      
//  fN2ODrainDay wird im Transport-Modell berechnet.

  
  pCP->fNTotalLeachCum = pCP->fUreaLeachCum
      			       + pCP->fNH4LeachCum
    			       + pCP->fNO3LeachCum
    			       + pCP->fN2ODrainCum;      
//  fN2ODrainCum wird im Transport-Modell berechnet.

/******************************* Pflanzen-N-Entzug  ************************************/

	if (pPl != NULL)
	{
	pPl->pPltNitrogen->fCumActNUpt += pPl->pPltNitrogen->fActNUptR * DeltaT;

	pCP->fCumActNUpt += pPl->pPltNitrogen->fActNUptR * DeltaT;
	}
                                             

/****************************** Bilanzierung *************************************/
  
  // Profilsumme Null setzen
  pCB->fNO3NProfile  =  
  pCB->fNH4NProfile  = 
  pCB->fN2ONProfile  = 
  pCB->fN2NProfile   = 
  pCB->fNxOyNProfile = 
  pCB->fNProfileAct  = 
  pCB->fUreaNProfile = (float)0;
	   
  pCP->fCLitter      = 
  pCP->fNLitter      = 
  pCP->fCManure      = 
  pCP->fNManure      = 
  pCP->fCHumus       = 
  pCP->fNHumus       = 
  pCP->fCSoilMicBiom = 
  pCP->fNSoilMicBiom = (float)0;


  // Profilwerte des loeslichen N berechnen
  for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
     {
      pCB->fNO3NProfile 	+= pCL->fNO3N;
      pCB->fNH4NProfile 	+= pCL->fNH4N;
      pCB->fN2ONProfile 	+= pCL->fN2ON;
      pCB->fN2NProfile  	+= pCL->fN2N;
      pCB->fUreaNProfile    += pCL->fUreaN;
      pCB->fNxOyNProfile  	+= pCL->fNxOyN;
     }

  // Profilwerte der organischen Bodensubstanz berechnen
  for (SOIL2_LAYERS1(pSL,pSo->pSLayer->pNext,pCL,pCh->pCLayer->pNext))
     {
	  pCP->fCLitter      += pCL->fCLitter;
	  pCP->fNLitter      += pCL->fNLitter;
	  pCP->fCManure      += pCL->fCManure;
	  pCP->fNManure      += pCL->fNManure;
	  pCP->fCHumus       += pSL->fCHumus;
	  pCP->fNHumus       += pSL->fNHumus;
	  pCP->fCSoilMicBiom += pCL->fCSoilMicBiom;
	  pCP->fNSoilMicBiom += pCL->fNSoilMicBiom;

     }

  // Gesamtstickstoff im Profil
  pCB->fNProfileAct = pCB->fNO3NProfile
    				+ pCB->fNH4NProfile
      				+ pCB->fN2ONProfile
      				+ pCB->fN2NProfile
      				+ pCB->fNxOyNProfile
      				+ pCB->fUreaNProfile;
  
  // Gesamtoutput berechnen
  pCB->fNOutputCum  = pCP->fN2OEmisCum + pCP->fNH3VolatCum  //= fNUpperOutputCum
                    + pCP->fCumActNUpt + pCP->fNImmobCum - pCP->fNMinerCum  //= fNMiddleOutputCum
                    + pCP->fNTotalLeachCum;

  // Input wird im N-Transport-Modul berechnet.
  /*
  pCB->fNInputCum += pCP->fNFertilizerR * DeltaT;

  if (pCP->fN2OEmisR < 0)
	  {
	   pCB->fNInputCum  -= pCP->fN2OEmisR * (float)24e-5 * DeltaT; 
	  }
  */
  
  f1 = pCB->fNBalance;
    
  pCB->fNBalance = pCB->fNProfileAct    /* [kg N/ha] */
                 + pCB->fNOutputCum
                 - pCB->fNProfileStart
                 - pCB->fNInputCum
				 - pCB->fNBalCorrect;
                     
			 
  f1 = pCB->fNBalance - f1; /* Gibt den Zuwachs der Bilanz an. */
    
  if ((f1 > ERROR_TOLERANCE)||(ERROR_TOLERANCE + f1 < 0 ))
    {
     #ifdef LOGFILE                            
      
      
      char   acdummy[80];

      if ((f1>-300)&&(f1<300))	   //Achtung: vorher -30,+30 StefanAchatz
      {
      itoa((int)(f1*1000), acdummy,10);
      strcpy(lpOut,acdummy); 

      strcat(lpOut,"g/ha "); 

      strcat(lpOut,ERROR_IN_N_BALANCE_TXT); 
      Message(0, lpOut);
      }
      else
      {
      strcpy(lpOut,ERROR_IN_N_BALANCE_TXT); 
      Message(3, lpOut);            
      return -1;
      }
      
     #endif
    }

                                           
                                           
/*****************************************************************************************
                                           HEAT
  as, 05.03.97
  
  Ermittelt die Tagesdurchschnittstemperatur der Bodenschichten
 *****************************************************************************************/

  
  if (SoilTempAverage==NULL) 
     {
       SoilTempAverage = (float *) malloc(pSo->iLayers * sizeof(float));
       memset(SoilTempAverage,0x0,(pSo->iLayers * sizeof(float)));
     }
  
  for (SOIL_LAYERS0(pHL,pHe->pHLayer))
  {
    SoilTempAverage[iLayer] += pHL->fSoilTemp * DeltaT;  
    
    if (EndDay(pTi))
      {
        pHL->fSoilTempAve = SoilTempAverage[iLayer];
        SoilTempAverage[iLayer] = (float)0;
      }
  }


/*********************************************************************************/
/*                                                                               */
/*                                Management                                     */
/*                                                                               */
/*********************************************************************************/

	/* Nachdem die durch den Bodenbearbeitungstermin gesteuerten Funktionen
	   abgearbeitet worden sind muss der Zeiger auf die naechste Struktur 
	   gestellt werden . */
	  
	  if (pMa->pTillage != NULL)
	  {
       if ((float)pTi->pSimTime->fTimeAct == (float)pMa->pTillage->iDay)
	   { 
		  if (pMa->pTillage->pNext != NULL)	
		    {	pMa->pTillage = pMa->pTillage->pNext;		
		    }
		  else
		    {	pMa->pTillage->iDay = (int)5000;
			}

	   }
	  }/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */


	/* Nachdem die durch den Duengungtermin gesteuerten Funktionen
	   abgearbeitet worden sind muss der Zeiger auf die naechste Struktur 
	   gestellt werden . */

	  if (pMa->pNFertilizer != NULL)
		if ((int)pTi->pSimTime->fTimeAct >= pMa->pNFertilizer->iDay)
		{
			if (pMa->pNFertilizer->pNext != NULL)
			{	pMa->pNFertilizer = pMa->pNFertilizer->pNext;
			}
			else
			{	pMa->pNFertilizer->iDay = (int)5000;
			}
		}/* Ende if */


	/* Nachdem die durch den Beregnungstermin gesteuerten Funktionen
	   abgearbeitet worden sind muss der Zeiger auf die naechste Struktur 
	   gestellt werden . */

	  if (pMa->pIrrigation != NULL)
		if (pTi->pSimTime->fTimeAct >= pMa->pIrrigation->iDay)
		{
			if (pMa->pIrrigation->pNext != NULL)
				pMa->pIrrigation = pMa->pIrrigation->pNext;		
			
		}/* Ende if (datum == pMa->pIrrigation->lDate) Beregnungstermin */


/********************************* Ende Management **********************************/
   
  
	  
  if (SimStart(pTi))	  
  {
	 pWM = pWa->pWMeasure;
     pNM = pCh->pNMeasure;
  }

  if(bFittingLoaded) FittingAuxiliary(exp_p);

	  /*
	  if(EndDay(pTi)){
		  FF = fopen("c:\\expertn.32b\\leach.dat","a");
		  fprintf(FF,"%i \t %6f \n",pTi->pSimTime->lTimeDate,pWa->fLeaching);
		  fclose(FF);
	  }
      */

  if( 1 && !bFittingLoaded && pWa->pWMeasure && pCh->pNMeasure) 
	          //Hallo Eckart: Hier steht dein if um die Ausgabe herauszunehmen 
		      //(Ausgabe Meßwert-Simwert-Vergleich / Objektfunktion)
  {
  if(EndDay(pTi)){

	  if(pTi->pSimTime->fTimeAct < (float)1.1) //Initialisierung am ersten Tag
	  {
		  for(i=0;i<20;i++)
		  {
			  fSquareSumWaterContent[i] = (float)0;
			  fSquareSumNO3[i] = (float)0;
			  fSquareSumNH4[i] = (float)0;
			  fSquareSumN2O[i] = (float)0;
		  }
		  fTotalSquareSumWaterContent = (float)0;
		  fTotalSquareSumNO3 = (float)0;
		  fTotalSquareSumNH4 = (float)0;
		  fTotalSquareSumN2O = (float)0;

		  iWMLayerNotConstant = 0;
		  iNMLayerNotConstant = 0;
		  
		  strcpy( WM_OutputFileName,"result\\");
		  strncat(WM_OutputFileName,pLo->pFieldplot->acName,8);
          strcat( WM_OutputFileName,".cwm");
		  FWM = fopen(WM_OutputFileName, "w");
												 
		  fprintf(FWM,"Date  Depth  Watercontent (Meas / Sim / Square of Diff.)\n");

		  fclose(FWM);

		  strcpy( NM_OutputFileName,"result\\");
		  strncat(NM_OutputFileName,pLo->pFieldplot->acName,8);
          strcat( NM_OutputFileName,".cnm");
		  FNM = fopen(NM_OutputFileName, "w");
												 
		  fprintf(FWM,"Date  Depth  NO3,NH4,N2O (Meas / Sim / Square of Diff.)\n");

		  fclose(FNM);

	  }

	  if(pWM->lDate>0 && pTi->pSimTime->lTimeDate == pWM->lDate)
	  {
		  pWL = pWa->pWLayer->pNext;
		  for(i=0; i<pSo->iLayers-2; i++){
			  faCont[i] = pWL->fContAct;
			  pWL = pWL->pNext;
		  }
		  
		  FWM = fopen(WM_OutputFileName, "a");

		  fLoDepth = (float)0;
		  iMLayer = 0;

		  while(pWM->afDepth[iMLayer] > (float)0)
		  {
			  if(pWM->pBack && afNMDepthLast[iMLayer] != pWM->afDepth[iMLayer])
				  iWMLayerNotConstant = 1;
			  afWMDepthLast[iMLayer] = pWM->afDepth[iMLayer];

			  fUpDepth = pWM->afDepth[iMLayer];
			  
			  iLoNum  = (int)(fLoDepth/pSo->fDeltaZ);
			  fLoFrac = fLoDepth - iLoNum*pSo->fDeltaZ;
			  iUpNum  = (int)(fUpDepth/pSo->fDeltaZ);
			  fUpFrac = fUpDepth - iUpNum*pSo->fDeltaZ;
			  
			  fActCont = (float)0;
			  
			  for(i=iLoNum; i<iUpNum; i++)
				  fActCont += faCont[i]*pSo->fDeltaZ;
			  fActCont += faCont[iUpNum-1]*fUpFrac - faCont[iLoNum-1]*fLoFrac;

			  fActCont /= (fUpDepth - fLoDepth);	//Hier steht jetzt der akuelle Wassergehalt,
												//gemittelt über die an der Meßschicht beteiligten
												//numerischen Schichten.

			  fSquDiffCont = pWM->afContent[iMLayer] != (float)-99 ? 
				  SQR(fActCont - pWM->afContent[iMLayer]*(float)0.01) : (float)0;
			  fSquareSumWaterContent[iMLayer] += fSquDiffCont;

			  fTotalSquareSumWaterContent += fSquDiffCont;

			  fprintf(FWM,"%6d %4.0f    ",pWM->lDate, pWM->afDepth[iMLayer]);
			  fprintf(FWM,"%8.4f  ",pWM->afContent[iMLayer]*(float)0.01);
			  fprintf(FWM,"%8.4f  ",fActCont);
			  fprintf(FWM,"%8.4f\n",fSquDiffCont);

			  fLoDepth = fUpDepth;
			  iMLayer++;
		  }

		  fclose(FWM);
		  if(pWM->pNext!=0)pWM = pWM->pNext;
	  }
	  

	  if(pNM->lDate>0 && pTi->pSimTime->lTimeDate == pNM->lDate)
	  {
		  pCL = pCh->pCLayer->pNext;
		  for(i=0; i<pSo->iLayers-2; i++){
			  faNO3[i] = pCL->fNO3N;
			  faNH4[i] = pCL->fNH4N;
			  faN2O[i] = pCL->fN2ON;
			  pCL = pCL->pNext;
		  }
		  
		  FNM = fopen(NM_OutputFileName, "a");

		  fLoDepth = (float)0;
		  iMLayer = 0;
		  while(pNM->afDepth[iMLayer] > (float)0)
		  {
			  if(pNM->pBack && afNMDepthLast[iMLayer] != pNM->afDepth[iMLayer])
				  iNMLayerNotConstant = 1;
			  afNMDepthLast[iMLayer] = pNM->afDepth[iMLayer];

			  fUpDepth = pNM->afDepth[iMLayer];
			  
			  iLoNum  = (int)(fLoDepth/pSo->fDeltaZ);
			  fLoFrac = fLoDepth - iLoNum*pSo->fDeltaZ;
			  iUpNum  = (int)(fUpDepth/pSo->fDeltaZ);
			  fUpFrac = fUpDepth - iUpNum*pSo->fDeltaZ;
			  
			  fActNO3 = (float)0;
			  fActNH4 = (float)0;
			  fActN2O = (float)0;
			  
			  for(i=iLoNum; i<iUpNum; i++){
				  fActNO3 += faNO3[i];
				  fActNH4 += faNH4[i];
				  fActN2O += faN2O[i];
			  }
			  fActNO3 += (faNO3[iUpNum-1]*fUpFrac - faNO3[iLoNum-1]*fLoFrac) / pSo->fDeltaZ;
			  fActNH4 += (faNH4[iUpNum-1]*fUpFrac - faNH4[iLoNum-1]*fLoFrac) / pSo->fDeltaZ;
			  fActN2O += (faN2O[iUpNum-1]*fUpFrac - faN2O[iLoNum-1]*fLoFrac) / pSo->fDeltaZ;

			  fSquDiffNO3 = pNM->afNO3N[iMLayer]!=(float)-99 ? SQR(fActNO3 - pNM->afNO3N[iMLayer]) : (float)0;
			  fSquDiffNH4 = pNM->afNH4N[iMLayer]!=(float)-99 ? SQR(fActNH4 - pNM->afNH4N[iMLayer]) : (float)0;
			  fSquDiffN2O = pNM->afN2ON[iMLayer]!=(float)-99 ? SQR(fActN2O - pNM->afN2ON[iMLayer]) : (float)0;

			  fSquareSumNO3[iMLayer] += fSquDiffNO3;
			  fSquareSumNH4[iMLayer] += fSquDiffNH4;
			  fSquareSumN2O[iMLayer] += fSquDiffN2O;

			  fTotalSquareSumNO3 += fSquDiffNO3;
			  fTotalSquareSumNH4 += fSquDiffNH4;
			  fTotalSquareSumN2O += fSquDiffN2O;

			  fprintf(FNM,"%6d %4.0f    ",pNM->lDate, pNM->afDepth[iMLayer]);
			  fprintf(FNM,"%8.4f  "  ,pNM->afNO3N[iMLayer]);
			  fprintf(FNM,"%8.4f  "  ,fActNO3);
			  fprintf(FNM,"%8.4f    ",fSquDiffNO3);
			  fprintf(FNM,"%8.4f  "  ,pNM->afNH4N[iMLayer]);
			  fprintf(FNM,"%8.4f  "  ,fActNH4);
			  fprintf(FNM,"%8.4f    ",fSquDiffNH4);
			  fprintf(FNM,"%8.4f  "  ,pNM->afN2ON[iMLayer]);
			  fprintf(FNM,"%8.4f  "  ,fActN2O);
			  fprintf(FNM,"%8.4f\n",  fSquDiffN2O);

			  iMLayer++;
			  fLoDepth = fUpDepth;
		  }

		  fclose(FNM);
		  if(pNM->pNext!=0)pNM = pNM->pNext;
	  }

	  if(pTi->pSimTime->fTimeAct+(float)1 >= (float)pTi->pSimTime->iSimDuration)
	  {
		  FWM = fopen(WM_OutputFileName, "a");

		  if(!iWMLayerNotConstant){
			  for(iMLayer=0;iMLayer<20 && fSquareSumWaterContent[iMLayer]>0.0;iMLayer++)
				  fprintf(FWM,"Watercontent, Sum Of Squares in Layer %3d : %f\n",
				  iMLayer+1,fSquareSumWaterContent[iMLayer]);
		  }

		  fprintf(FWM,"Watercontent, Total Sum Of Squares : %f\n",fTotalSquareSumWaterContent);

		  fclose(FWM);

		  FNM = fopen(NM_OutputFileName, "a");
		  
		  if(!iNMLayerNotConstant){
			  for(iMLayer=0;iMLayer<20 && 
				  (fSquareSumNO3[iMLayer]>0.0 ||
				  fSquareSumNH4[iMLayer]>0.0 ||
				  fSquareSumN2O[iMLayer]>0.0);  iMLayer++)
			  {
				  fprintf(FNM,"NO3, Sum Of Squares in Layer %3d : %f\n",iMLayer+1,fSquareSumNO3[iMLayer]);
				  fprintf(FNM,"NH4, Sum Of Squares in Layer %3d : %f\n",iMLayer+1,fSquareSumNH4[iMLayer]);
				  fprintf(FNM,"N2O, Sum Of Squares in Layer %3d : %f\n",iMLayer+1,fSquareSumN2O[iMLayer]);
			  }
		  }

		  fprintf(FNM,"NO3, Total Sum Of Squares : %f\n",fTotalSquareSumNO3);
		  fprintf(FNM,"NH4, Total Sum Of Squares : %f\n",fTotalSquareSumNH4);
		  fprintf(FNM,"N2O, Total Sum Of Squares : %f\n",fTotalSquareSumN2O);
		  
		  fclose(FNM);

	  }

   }//if endday
}
 return 1;
}  

 
/*************************************************************************************/
/*  Name     : Set_Day_Zero()                                                        */
/*  Autor    : Maier Daniel                                                          */
/*  Datum    : 01.11.96                                                              */
/*                                                                                   */
/*  Funktion : Setz kumulierte Tagesgrößen zu Tagesbeginn zurück auf Null            */
/*             Sollte vom System übernommen werden !!                                */
/*************************************************************************************/
/*  veränd. Var.:		pWE->fActDay                 pCL->fNO3DenitDay               */
/*						pWE->fPotDay                 pCL->fNH4NitrDay                */
/*                      pWa->fInfiltDay              pCL->fNLitterMinerDay           */
/*   					pWa->fLeaching               pCL->fNHumusMinerDay            */
/*   					pPW->fActTranspDay           pCL->fNImmobDay                 */
/*  					pPW->fPotTranspDay           pCL->fN2ODenitDay               */
/*   					pCP->fUreaLeachDay           pCL->fUreaHydroDay              */
/*   					pCP->fNH4LeachDay            pCL->fCO2ProdDay                */
/*   					pCP->fNO3LeachDay            pCL->fNLitterImmobDay           */
/*   					pCP->fNTotalLeachDay         pCL->fNManureImmobDay           */
/*   					pCP->fNH3VolatDay                                            */
/*   					pCP->fN2ODenitDay                                            */
/*   					pCP->fN2OEmisDay                                             */
/*   					pCP->fNO3DenitDay                                            */
/*   					pCP->fNH4NitrDay                                             */
/*   					pCP->fNMinerDay                                              */
/*   					pCP->fCO2ProdDay                                             */
/*   					pCP->fN2ODrainDay                                            */
/*                                                                                   */
/*************************************************************************************/
int WINAPI Set_Day_Zero(EXP_POINTER)
{
 PCLAYER   pCL;
 PWLAYER   pWL;
 PCPROFILE pCP = pCh->pCProfile;
 PEVAP     pWE = pWa->pEvap;
 PPLTWATER pPW = NULL;       
 int iLayer;

   if (pPl != NULL) 
   {
  	pPW = pPl->pPltWater;                     
    pPW->fActTranspDay   = (float)0;
    pPW->fPotTranspDay   = (float)0;
   }                   
   pWa->pWBalance->fReservoir = (float)0;


   pWE->fActDay         = (float)0;
   pWE->fPotDay         = (float)0;
   pWa->fInfiltDay      = (float)0;
   pWa->fRunOffDay      = (float)0;
   pWa->fLeaching       = (float)0;
   pWa->fRunOffR        = (float)0;

   pCP->fUreaLeachDay   = (float)0;
   pCP->fNH4LeachDay    = (float)0;
   pCP->fNO3LeachDay    = (float)0;
   pCP->fNTotalLeachDay = (float)0;
   pCP->fNH3VolatDay    = (float)0;
   pCP->fN2ODenitDay    = (float)0;
   pCP->fN2OEmisDay     = (float)0;
   pCP->fNO3DenitDay    = (float)0;
   pCP->fNH4NitrDay     = (float)0;
   pCP->fNMinerDay      = (float)0;
   pCP->fCO2ProdDay     = (float)0;        
   pCP->fN2ODrainDay    = (float)0;        

	pCP->fNLitterMinerDay    = (float)0;        
	pCP->fNManureMinerDay    = (float)0;        
	pCP->fNHumusMinerDay     = (float)0;        

	pCP->fNImmobDay          = (float)0;        
	pCP->fNLitterImmobDay    = (float)0;        
	pCP->fNManureImmobDay    = (float)0;        
	pCP->fUreaHydroDay       = (float)0;        

     for (SOIL_LAYERS0(pCL,pCh->pCLayer))
       {
       pCL->fNO3DenitDay     = (float)0;
       pCL->fNH4NitrDay      = (float)0;
       pCL->fNLitterMinerDay = (float)0;
       pCL->fNHumusMinerDay  = (float)0;     
       pCL->fNImmobDay       = (float)0;
       pCL->fN2ODenitDay     = (float)0;
       pCL->fUreaHydroDay    = (float)0;
       pCL->fCO2ProdDay      = (float)0; 
       }

     for (SOIL_LAYERS0(pWL,pWa->pWLayer))
         pWL->fFluxDay = (float)0; 

return 1;
}   


