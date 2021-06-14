/***********************************************************************************************/
/*                                                                                             */
/* Prozedur    :   DenitXN()                                                         		   */
/* Beschreibung:   Denitrifikation                                                             */
/*                 Verallgemeinerung der                                                       */
/*			  	   Methode LEACHM 3.0 (Hutson & Wagenet, 1992)                                 */
/*                                                                                             */
/*                  ch                  10.11.96                                               */
/*				 	                                                                           */
/*					Zusaetzliche Modellparameter:                                              */
/*					                                                                           */
/* 					fN2Fraction 	= N2:(N2+N2O) der Denitrifikationsprodukte. [0<x<1]        */
/* 					                                                                           */
/* 					fN2OReduction	= Abbaurate von N2O zu N2.				    [1/day]        */
/*					                                                                           */
/*					iRewet			= REWETTING_ENHANCES_MAX_DEN            0 oder 1           */
/*									+ 2* REWETTING_RELEASES_NO3             0 oder 1           */
/*									+ 4* REWETTING_REDUCES_O2               0 oder 1           */
/*                                                                                             */
/*					iNO3Kin 		= NO3_KIN_SUPRALINEAR                	0 oder 1           */
/*									+ 2* NO3_KIN_MM                 		0 oder 1           */
/*					                                                                           */
/*					Für Ansatz LeachN: (exp_lp, 0, 0, 0, 3)                                    */
/*					                                                                           */
/***********************************************************************************************/
/* veränd. Var.		pCL->fNO3DenitR                                                            */
/*                  pCL->fN2ODenitR                                                            */
/*					pCL->fNO3N                                                                 */
/*					pCL->fN2ON                                                                 */
/*					pCL->fN2N                                                                  */
/*					                                                                           */
/*					Reduce_C_via_Denit:	pCL->fCLitter                                          */
/*										pCL->fCManure                                          */
/*										pSL->fCHumus                                           */
/*                                                                                             */
/***********************************************************************************************/
/*      stickstoff.c																		   */
/*      																					   */
/*      Copyright 2013:  																	   */
/*     Implementiert für XPN 5-0 von Christoph Thieme 										   */
/***********************************************************************************************/

//analog Century
#include "stickstoff.h"
#include "stickstoff_macros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// compability function to int_to_str
// base is ignored
char *int_to_str(int value, char *str, int base)
{
	if (base!=10)
		{
			PRINT_ERROR("base is not 10!")
		}
	sprintf(str,"%d",value);
	return str;
}

int leachn_denit_load(stickstoff *self)
{
	//expertn_modul_base *xpn = &(self->parent);


/*	PCHEMISTRY pCh = xpn->pCh;
	xpn_register_var *var_list = xpn->pXSys->var_list;

		PCHEMISTRY pCh = xpn->pCh;
		PSLAYER      pSL;
		PCLAYER      pCL;
		PCPROFILE    pCP;
		PCPARAM      pPA;
		PSWATER      pSW;
		PWLAYER      pWL;
		PHLAYER      pHL;
		PNFERTILIZER pNF;
		int iLayer;

		self->fDryDays = (double)0;
		self->fReWetEvent = 0;
		//self->fReWetDay = 0;
		//self->iReWetYear = 0;
		self->lastiyear = 0;
		self->lastfTimeY= 0.0;


		//Hardcode Workaround:
		N_ZERO_LAYER
		for(N_SOIL_LAYERS)
			{
				if(iLayer<8)
					{
						pCL->fNO3DenitMaxR=0.1667;
					}
				else
					{
						pCL->fNO3DenitMaxR=0.0;
					}

			}*/



	self->fDryDays = (double)0;
	self->fReWetEvent = 0;
	//self->fReWetDay = 0;
	//self->iReWetYear = 0;
	self->lastiyear = 0;
	self->lastfTimeY= 0.0;
	
	self->fDailyPrecRate=0.0;
	self->_fDailyPrecRate=0.0;
	
	//N2:(N2+N2O) der Denitrifikationsprodukte. [0<x<1],   N2/(N2+N2O) = 1, => NO,N2O
	self->fN2Fraction=0.5;	
	// Abbaurate von N2O zu N2.				    [1/day] 
	self->fN2OReduction=0.0;
	//  REWETTING_ENHANCES_MAX_DEN            0 oder 1
	// + 2* REWETTING_RELEASES_NO3             0 oder 1
    //+ 4* REWETTING_REDUCES_O2               0 oder 1          
	self->iRewet=7;
	// NO3_KIN_SUPRALINEAR                	0 oder 1
   //  + 2* NO3_KIN_MM                		0 oder 1      
	self->iNO3Kin=1;

	stickstoff_load_ini(self);
	
	//xpn_register_var_init_rec_struct( var_list,CLAYER, pCh->pCLayer,xpn_register_var_init_pdouble,fCLitter,"pCh.pCLayer.fCLitter",150.89888);

	return RET_SUCCESS;
}


/*********************************************************************************************/
/* Function    :   Get_Rewet()                                                               */
/* Beschreibung:   Liefert Ergebis zwischen 0 und 1 zur Beschreibung einer Wiederbefeuchtung.*/
/*				   Hoehe der Wiederbefeuchtung richtet sich nach kumulierter                 */
/*                 pot. Evapotranspiration seit dem letzten Wiederbefeuchtungsereignis       */
/*                 (NS > 5 mm)                                                               */
/*                                                                                           */
/*              GSF: ch                     6.11.96                                          */
/*********************************************************************************************/
double Get_Rewet(int iLog, stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);

	//lokale Definition
	double 		 fWet;
	const double fRewetDuration 	= (double)7;//5;//3;//4;//7;       /* Dauer des rewet-Effekts */
	const double fRewetUp 		    = (double)3;//1;//1;//1;//2;       /* Verzoegerung bis zum max. Effekt */
	const double fRewetDown 		= (double)3;//1;//1;//2;//3;       /* Zeitpunkt der Abnahme des Effekts */
	
	double test;


#ifdef LOGFILE           			//Was soll das? bleibt das?                 
	char acdummy[80];
	char lpOut = acdummy;
#endif


	//xpn_time_compare vergleicht, ob das letzte Jahr älter als das jetztige ist
	if (((NewDay(xpn->pTi)) || (xpn->pTi->pSimTime->bFirstRound==TRUE)) && (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,self->lastiyear,self->lastfTimeY)>0))
		// Die Überprüfung erlaubt es, diese Funktion in mehreren Modulen aufzurufen.
		{


			//Zeitstempel wird am Anfang der Schleife gespeichert
			self->lastiyear = xpn->pTi->pSimTime->iyear;
			self->lastfTimeY= xpn->pTi->pSimTime->fTimeY;

			//Epsilon wird Addiert (Wert: 1.0e-9)
			xpn_time_add_dt(&self->lastiyear,&self->lastfTimeY,EPSILON);

			if (xpn->pCl->pWeather->fPanEvap > (double)0)        //[mm/day]
				{
					
					// ToTo: fPanEvap aufintegrieren????
					self->fDryDays += xpn->pCl->pWeather->fPanEvap;
				}
			else
				{
					self->fDryDays += (double)6.;
				}

			// RAINY_DAY    (self->fDailyPrecRate > (double)10)
			if  RAINY_DAY
			{
				/* Ein neues rewetting kann fruehestens sieben Tage nach dem letzten vorkommen: */				
					if(xpn_compare_to_added_date(xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay,7.0)>0)
						{
							//hier wird der neue Rewetting-Day gesetzt falls möglich
							self->fReWetDay  =	xpn->pTi->pSimTime->fTimeY;
							self->iReWetYear =	xpn->pTi->pSimTime->iyear;

							self->fReWetEvent = min((double)1, self->fDryDays/(double)100);

							//fRewetEvent = (double)1;//ep 310100  min((double)1, fDryDays/(double)100);
							// Normiert am Regenereignis 14.7.95 Damm
							//fRewetEvent*=min((double)1,self->fPreciRate/(double)30);
#ifdef LOGFILE

							if (self->fReWetEvent > (double)0.1)
								{
									if (iLog)
										{
											int_to_str((int)((double)100*self->fReWetEvent), lpOut, 10);
											strcat((LPSTR)lpOut,COMMENT_REWET_TXT);
											Message(0, lpOut);
										}
									//Fehlermeldung ersetzen!!!
									//if(iLog)...durch folgende Zeile ersetzen.
									//Message(xpn, 3, COMMENT_REWET_TXT)

								}
#endif
						}

					self->fDryDays = (double)0;

				}  /* RainyDay */
			else
				{
					// !!! Neue Idee:
					self->fDryDays -= self->fDailyPrecRate;
					self->fDryDays = max(self->fDryDays,(double)0);
				}

		} /* NewDay */

	test = 0.0;
	/*es wird abgefragt, ob ein Rewetting Ereignis schon länger als 7 Tage zurückliegt */
	if (xpn_compare_to_added_date(xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay, fRewetDuration)<0)
		{
			if (xpn_compare_to_added_date(xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay, fRewetUp)<0)
				{
				//test = -xpn_time_get_number_of_days(xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay);
				fWet = self->fReWetEvent * (-xpn_time_get_number_of_days(xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay)/fRewetUp);
				}
			else if (xpn_compare_to_added_date(xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY, self->iReWetYear,self->fReWetDay, fRewetDown)<0)
				fWet = self->fReWetEvent;
			else
				{
				fWet = self->fReWetEvent * ((double)1 + (xpn_time_get_number_of_days(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay)+ fRewetDown)/(fRewetDuration - fRewetDown));
				test = -(xpn_time_get_number_of_days(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY, self->iReWetYear, self->fReWetDay)+ fRewetDown);
				}
		}
	else
		fWet = (double)0;
		
	C_DEBUG(self->fReWetEvent);
	C_DEBUG(test);
	C_DEBUG(self->fReWetDay);
	C_DEBUG(xpn->pTi->pSimTime->fTimeY);

	return fWet;
}

/*********************************************************************************************/
/* Procedur    :   Reduce_C_Via_Denit()                                                      */
/* Beschreibung:   Reduziert die Pools Litter, Manure und Humus während                      */
/*                 Denitrifikation.                                                          */
/*                                                                                           */
/*              GSF: ch                     18.9.95                                          */
/*                                                                                           */
/*********************************************************************************************/
/* veränd. Var.		pCL->fCLitter                                                            */
/*					pCL->fCManure                                                            */
/*					pSL->fCHumus                                                              */
/*********************************************************************************************/
int Reduce_C_Via_Denit(double fDenitrifCC, double DeltaT, PSLAYER pSL, PCLAYER pCL)
{
	double fLitterDenitrifCC, fManureDenitrifCC, fHumusDenitrifCC;

	/** Litter-C Abbau  */
	Set_At_Least_Epsilon(&pSL->fCHumus, "pSL->fCHumus");

	fLitterDenitrifCC = fDenitrifCC * RelAnteil(pCL->fCLitter, pCL->fCManure + pSL->fCHumus);
	pCL->fCLitter    -= fLitterDenitrifCC * DeltaT;

	/** Manure-C Abbau  */
	fManureDenitrifCC = fDenitrifCC * RelAnteil(pCL->fCManure, pCL->fCLitter + pSL->fCHumus);
	pCL->fCManure    -= fManureDenitrifCC * DeltaT;

	/** Humus-C Abbau  */
	fHumusDenitrifCC  = fDenitrifCC  * RelAnteil(pSL->fCHumus, pCL->fCManure + pCL->fCLitter);
	pSL->fCHumus      -= fHumusDenitrifCC * DeltaT;

	return 1;
}


/***********************************************************************************/
/* Setzt x auf EPSILON falls x < EPSILON										   */
/***********************************************************************************/

int Set_At_Least_Epsilon(double *x, const char *Message)
{
	if (*x < EPSILON)
		{
			*x = EPSILON;
			if (Message!=NULL)
				{
					PRINT_ERROR(Message);
				}
			return 0;
		}
	return 1;
}

/************************************************************************************/
/* Function: leach_denit_run_call													*/
/* Damit wird die "Run-Funktion" aufgerufen											*/
/************************************************************************************/

int leachn_denit_run_call(stickstoff *self)
{
	int ret;
	ret = lechn_denit_integrate_daily_vars(self);
	ret |= leachn_denit_run(self, self->fN2Fraction, self->fN2OReduction, self->iRewet, self->iNO3Kin); 	
											/*  N2/(N2+N2O) = 1, => No N2O
    										REWETTING_ENHANCES_MAX_DEN +1
    										REWETTING_RELEASES_NO3     +2
    										REWETTING_REDUCES_O2	   +4

    										NO3_KIN_SUPRALINEAR
    										NO3_KIN_MM 	=> Leach Kinetics
    										*/
	
	ret |= EmitAllN2O(self);
	return ret;
}


int leachn_denit_run(stickstoff *self, double fN2Fraction, double fN2OReduction, int iRewet, int iNO3Kin)
{
	expertn_modul_base *xpn = &(self->parent);

	PCPARAM      pPA = xpn->pCh->pCParam;
	PTIME 		 pTi = xpn->pTi;
	PSPROFILE    pSo = xpn->pSo;
	PWATER       pWa = xpn->pWa;
	PCHEMISTRY   pCh = xpn->pCh;
	PHEAT 		 pHe = xpn->pHe;
	//PPLANT 		 pPl = xpn->pPl;

	PSLAYER      pSL = pSo->pSLayer->pNext;
	PCLAYER      pCL = pCh->pCLayer->pNext;
	PSWATER      pSW = pSo->pSWater->pNext;
	PWLAYER      pWL = pWa->pWLayer->pNext;
	PHLAYER      pHL = pHe->pHLayer->pNext;

	//DECLARE_COMMON_VAR analog XPN3
	int 	iLayer = 0;
	//int 	OK = 1;								//auskommentiert, da im Code nicht gefunden
	char  	acdummy[80];
	char* 	lpOut   = acdummy;	 				//LPSTR durch char* ersetzt
	char  	acdummy2[80];
	char* 	lpOut2	= acdummy2; 				//LPSTR durch char* ersetzt
	double 	DeltaT	= pTi->pTimeStep->fAct;


	struct ncorr  corr = { (double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1,(double)1};
	double fDenitrifCC;    /* C-Abbaurate durch Denitrifikation */
	double fCLimitDenR;    /* Beschränkung der Denitrifikation durch C-Pools  */

	double fcNO3rewet;
	double fMinWFPS;
	double fRewetFactor;
	double f1,f2;



	//double DeltaZ = pSo->fDeltaZ; auskommentiert am 9.8.13

	N_FIRSTLAYER

	if (SimStart(pTi))
		{
			if (fN2Fraction >EPSILON)
				{
					strcpy(lpOut,"denitrification: N2:(N2O+N2)= ");
					int_to_str((int)(fN2Fraction*100),lpOut2,10);
					strcat(lpOut,lpOut2);
					strcat(lpOut,"%.");

					Message(0, lpOut);
				}

			if (fN2OReduction >EPSILON)
				{
					strcpy(lpOut,"denitrification: N2O->N2 max= ");

					if (fN2OReduction > 1)
						{
							int_to_str((int)(fN2OReduction),lpOut2,10);
							strcat(lpOut,lpOut2);
							strcat(lpOut,"/day.");
						}
					else
						{
							int_to_str((int)(100*fN2OReduction),lpOut2,10);
							strcat(lpOut,lpOut2);
							strcat(lpOut,"%/day.");
						}

					Message(0, lpOut);
				}

			if (NO3_KIN_SUPRALINEAR)
				{
					strcpy(lpOut,"denitrification: LeachN-Kin. MaxR ");
					int_to_str((int)(pCh->pCLayer->pNext->fNO3DenitMaxR),lpOut2,10);
					strcat(lpOut,lpOut2);
					strcat(lpOut," 1/d, KsNO3 ");

					int_to_str((int)pPA->fDenitKsNO3,lpOut2,10);
					strcat(lpOut,lpOut2);
					strcat(lpOut," mg NO3-N/l.");

					Message(0, lpOut);
				}
			else
				{
					strcpy(lpOut,"denitrification: MM Kin.  KDenMax(1) ");
					int_to_str((int)(pCh->pCLayer->pNext->fNO3DenitMaxR),lpOut2,10);
					strcat(lpOut,lpOut2);
					strcat(lpOut," ng NO3-N/ml/d, KsNO3 ");
					//strcat(lpOut," g/ha/d per layer, KsNO3 ");

					int_to_str((int)pPA->fDenitKsNO3,lpOut2,10);
					strcat(lpOut,lpOut2);
					strcat(lpOut," mg NO3-N/l.");

					Message(0, lpOut);
				}

			if ((REWETTING_REDUCES_O2)||(REWETTING_RELEASES_NO3))
				{
					strcpy(lpOut,"denitrification: Rewet-Factor ");
					int_to_str((int)pPA->fDenitReWet,lpOut2,10);
					strcat(lpOut,lpOut2);

					Message(0, lpOut);
				}

			if (pPA->fDenitFreeze > EPSILON)
				{
					strcpy(lpOut,"denitrification: Freeze-Factor ");
					int_to_str((int)pPA->fDenitFreeze,lpOut2,10);
					strcat(lpOut,lpOut2);

					Message(0, lpOut);
				}

			if (pPA->fDenitThaw > EPSILON)
				{
					strcpy(lpOut,"denitrification: Thaw-Factor ");
					int_to_str((int)pPA->fDenitThaw,lpOut2,10);
					strcat(lpOut,lpOut2);

					Message(0, lpOut);
				}


		} //SimStart



	if ((REWETTING_REDUCES_O2)||(REWETTING_RELEASES_NO3))
		fRewetFactor        = Get_Rewet(1, self);
	else
		fRewetFactor        = Get_Rewet(0, self);

	corr.WiederBefeucht = (REWETTING_ENHANCES_MAX_DEN && (fRewetFactor > EPSILON)) ?

	                      /* ch 13.12.96      			*/
#ifdef REWETTING_ONLY_WITH_CROP
	                      fRewetFactor * pPA->fDenitReWet * pPl->pCanopy->fCropCoverFrac + (double)1
#endif
#ifndef REWETTING_ONLY_WITH_CROP
	                      fRewetFactor*pPA->fDenitReWet  +(double)1
#endif
	                      :
	                      (double)1;

	/* 26.8.97 Für Ausgabe wird eine globale Variable benötigt!
	Benutze die 2. Schicht der Variable pPA->fBioSlowMaxGrowR um den Korrekturfaktor abzuspeichern: */
	self->DENIT_REWET_FACTOR = corr.WiederBefeucht;

	/* ch 13.12.96     Test: 	fRewetFactor * pPA->fDenitReWet * fcropcover +(double)1 */

	fcNO3rewet          = (REWETTING_RELEASES_NO3 && (fRewetFactor > EPSILON)) ?
	                      fRewetFactor * (double)1   /* 0.66 */
	                      :
	                      0;

	/* Berechnung erfolgt von Schicht 1 bis vorletzte  */

	C_DEBUG(fRewetFactor);
	C_DEBUG(REWETTING_REDUCES_O2 && (fRewetFactor > EPSILON));

	for (N_SOIL_LAYERS)
		{


			/* Verringerung des Schwellenwertes bzgl Wassergehalt */
			if (REWETTING_REDUCES_O2 && (fRewetFactor > EPSILON))
				{
					/* 15.12.97 der Schwellenwert soll mithilfe des eingelesenen Werts pPA->fDenitReWet
							gesenkt werden, falls dieser < 1 ist. */
					fMinWFPS = (pPA->fDenitReWet<=(double)1)?
					           pPA->fDenitThetaMin * (1.0 - pPA->fDenitReWet* fRewetFactor)
					           :
					           pPA->fDenitThetaMin * (1.0 - (double)0.3333 * fRewetFactor);
				}
			else
				fMinWFPS = pPA->fDenitThetaMin;


			Set_At_Least_Epsilon(&pPA->fDenitKsNO3, "fDenitKsNO3");

			corr.Temp    = abspowerDBL(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0));

			corr.Feucht  = Polygon4(((pWL->fContAct + pWL->fIce) / pSL->fPorosity), fMinWFPS, (double)0, (double)1, (double)1, (double)2, (double)1, (double)3, (double)1);

			corr.Feucht *=  corr.Feucht; /*d=2 */


			//f2=pWL->fContAct*DeltaZ/(double)100.0;	// [mm] -> [dm] i.e. [mg l-1] -> [kg ha-1] !
			//f2=DeltaZ/(double)100.0;	// [mm] -> [dm] i.e. [mg l-1] -> [kg ha-1] !
			f2=(double)1;//alte Version: bei Umstellung Standardwerte in yinit.c nicht vergessen!

			if (NO3_KIN_MM)
				corr.NO3  = RelAnteil(pCL->fNO3N, pPA->fDenitKsNO3*f2);
			else
				corr.NO3  = (double)0.5 * pCL->fNO3N/f2 / pPA->fDenitKsNO3;   /* das stimmt fuer NO3 = Ks mit obiger Definition ueberein. */

			/* Frost-Einfluss */
			//if (pHL->fSoilTemp > (double)-5)
			{
				if (pWL->fWaterFreezeR > 0)
					corr.Frost = (double)1 + pPA->fDenitFreeze * pWL->fWaterFreezeR;
				else
					corr.Frost = (double)1 - pPA->fDenitThaw   * pWL->fWaterFreezeR;
			}
			/*	  else
				  {
					 corr.Frost = (double)1;
				  }
			*/
			/* 26.8.97 Für Ausgabe wird eine globale Variable benötigt!
			Benutze die 3. Schicht der Variable pPA->fBioSlowMaxGrowR um den Korrekturfaktor Frost abzuspeichern: */

			self->DENIT_FROST_FACTOR = ((NewDay(pTi)) || (xpn->pTi->pSimTime->bFirstRound==TRUE))?
			                           (double)1:
			                           max(self->DENIT_FROST_FACTOR, corr.Frost);

			/* Bei REWETTING_RELEASES_NO3 und bei Frostverstaerkung tritt NO3-Freigabe auf: */

			if (corr.Frost > (double)2 )
				{
					corr.NO3 = max((double)0.66, corr.NO3);
				}

			corr.NO3 = max(fcNO3rewet,corr.NO3); //ep 180100 auskommentiert!???
			//if(fcNO3rewet > 1) corr.NO3 = (double)1;//max((double)0.5,corr.NO3);
			//corr.NO3 = min((double)0.5,max(fcNO3rewet,corr.NO3));//ep 270100
			if (pWL->fContOld/pSL->fPorosity >= (double)0.55) corr.WiederBefeucht = (double)1;//ep 141009

			/* Denitrifikations - Rate  */
			fCLimitDenR = (double)0.1 * (pCL->fCLitter + pSL->fCHumus + pCL->fCManure)
			              * (double)56 /(double)72 /DeltaT ;

			pCL->fNO3DenitR = pCL->fNO3DenitMaxR*f2
			                  * corr.Temp * corr.Feucht * corr.NO3 * corr.Frost * corr.WiederBefeucht;

			if (NO3_KIN_SUPRALINEAR)
				{
					pCL->fNO3DenitR *= pCL->fNO3N;
				}
			else
				{
					//  lpXN->fNO3DenitR *= (double)15;  /* Mittlerer NO3-Gehalt in Kartoffelsimulation */
				}

			if (pCL->fNO3DenitR > fCLimitDenR)
				{
					pCL->fNO3DenitR = max((double)0,fCLimitDenR);
				}


			TEST_RATE(pCL->fNO3DenitR,pCL->fNO3N);			//Strichpunkt eingeführt

			/* C-Abbau durch Denitrifikation */

			fDenitrifCC = pCL->fNO3DenitR * (double)72/(double)56;

			Reduce_C_Via_Denit(fDenitrifCC, DeltaT, pSL, pCL);

			/* Denitrifikation */

			/**** results ***/

			pCL->fNO3N -= pCL->fNO3DenitR * DeltaT;
			pCL->fN2ON += pCL->fNO3DenitR * DeltaT;
			C_DEBUG_LOC_IN_ARRAY(pCL->fN2ON,iLayer);

			if ((fN2Fraction >= (double)0) && (fN2Fraction <= (double)1))
				{
					pCL->fN2ON   -= fN2Fraction * pCL->fNO3DenitR *  DeltaT;
					pCL->fNON  += fN2Fraction * pCL->fNO3DenitR * DeltaT;
				}

			C_DEBUG_LOC_IN_ARRAY(pCL->fN2ON,iLayer);
			/*
			Es wird ein Abbau von N2O zu N2 erster Ordnung angenommen.
			Abbaurate: fN2OReduction [1/day]
			*/
			if (fN2OReduction > EPSILON)
				{
					/* Test!!!
					: Nehme an, dass N2O->N2 bei niedrigen Temp. geringer ist als NO3->N2O
					if (pHL->fSoilTemp < pPA->fMinerTempB)
					corr.Temp    = abspower(pPA->fMinerQ10*(double)4,((pHL->fSoilTemp - pPA->fMinerTempB)/(double)10.0));
					*/

					f1 =  pCL->fNO3DenitMaxR*f2
					      * corr.Temp * corr.Feucht * corr.Frost * corr.WiederBefeucht;
					pCL->fN2ODenitR    = pCL->fN2ON * ((double)1 -(double)exp(-fN2OReduction * f1 * DeltaT))/ DeltaT;
					pCL->fN2N         += pCL->fN2ODenitR * DeltaT;
					pCL->fN2ON        -= pCL->fN2ODenitR * DeltaT;
				}


			/*
			     Deb_Corr(corr, iLayer, exp_p, 1); */

			{
				double WaterFilledPoreSpace;
				WaterFilledPoreSpace = ((pWL->fContAct + pWL->fIce) / pSL->fPorosity);
				C_DEBUG_LOC_IN_ARRAY(WaterFilledPoreSpace,iLayer);
			}

			C_DEBUG_LOC_IN_ARRAY(pCL->fN2ON,iLayer);
			C_DEBUG_LOC_IN_ARRAY(fMinWFPS,iLayer);
			C_DEBUG_LOC_IN_ARRAY(corr.Temp,iLayer);
			C_DEBUG_LOC_IN_ARRAY(corr.Feucht,iLayer);
			C_DEBUG_LOC_IN_ARRAY(corr.NO3,iLayer);
			C_DEBUG_LOC_IN_ARRAY(corr.Frost,iLayer);
			C_DEBUG_LOC_IN_ARRAY(corr.WiederBefeucht,iLayer);

		} /* Schichtweise */

	C_DEBUG(fRewetFactor);

	return RET_SUCCESS;
}


/*********************************************************************************/
/*  Name     : EmitAllN2O                                                        */
/*                                                                               */
/*  Funktion : Emittiert alles im Profil vorhandene N2O sofort.                  */
/*             Mithilfe dieser Funktion kann ein weiterer Abbau N2O -> N2        */
/*             verhindert werden.                                                */
/*  Autor    : Ch. Haberbosch                                                    */
/*  Datum	 : 17.07.97                                                          */
/*                                                                               */
/*  Change   : pCh->pCLayer->pNext->fNLitter                                     */
/*                                                                               */
/*********************************************************************************/
int EmitAllN2O(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME 		 pTi = xpn->pTi;
	PSPROFILE    pSo = xpn->pSo;
	PCHEMISTRY   pCh = xpn->pCh;

	int   iLayer;
	double fN2O_In_Profile = (double)0;
	double fNO_In_Profile  = (double)0;
	double fN2_In_Profile  = (double)0;

	PCLAYER      pCL= pCh->pCLayer;

	pCL->fN2ONGasConc               =
	    pCL->fN2ONGasConcOld            = (double)0;
	/* Athmosphaerische N2O-Konzentration nicht beachten */


	for(SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
		{
			if (pCL->fN2ON > EPSILON)
				{
					fN2O_In_Profile += pCL->fN2ON;
					fNO_In_Profile  += pCL->fNON;
					fN2_In_Profile  += pCL->fN2N;
				}

			pCL->fN2ONGasConc    =
			    pCL->fN2ONGasConcOld =
			        pCL->fN2ON           =
			            pCL->fNON            =
			                pCL->fN2N            = (double)0;
		}

	pCh->pCProfile->fN2OEmisR = fN2O_In_Profile/((double)24e-5 * pTi->pTimeStep->fAct);
	/*  [ mu g N /m^2 /h] */
	pCh->pCProfile->fNOEmisR  = fNO_In_Profile/((double)24e-5 * pTi->pTimeStep->fAct);
	/*  [ mu g N /m^2 /h] */
	pCh->pCProfile->fN2EmisR  = fN2_In_Profile/((double)24e-5 * pTi->pTimeStep->fAct);
	/*  [ mu g N /m^2 /h] */

	return RET_SUCCESS;
}      /* EmitAllN2O */


int lechn_denit_integrate_daily_vars(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);
	if ((NewDay(xpn->pTi)) || (xpn->pTi->pSimTime->bFirstRound==TRUE))
		{
			self->fDailyPrecRate=self->_fDailyPrecRate;
			self->_fDailyPrecRate=0.0;
		}
	self->_fDailyPrecRate += xpn->pCl->pWeather->fPreciRate*xpn->pTi->pTimeStep->fAct;
	return RET_SUCCESS;
}


int leachn_denit_done(stickstoff *self)
{
	G_FREE_IF_NOT_0(self->ini_filename);
	return RET_SUCCESS;
}
