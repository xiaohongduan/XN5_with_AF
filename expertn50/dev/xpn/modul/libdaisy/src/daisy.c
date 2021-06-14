//      daisy.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "daisy.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <glib.h>

G_DEFINE_TYPE(daisy, daisy, EXPERTN_MODUL_BASE_TYPE);

#define check_soil_temp(temp) if (temp>40.0) \
										{\
											PRINT_WARNING_ID(xpn,"Soil Temp too high");\
											self->too_high_or_low_temp=1;\
										}\
							  if (temp<-30.0) \
										{\
											PRINT_WARNING_ID(xpn,"Soil Temp too low");\
											self->too_high_or_low_temp=1;\
										}



static void daisy_class_init(daisyClass *klass) {}


static void daisy_init(daisy *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL DAISY!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
}


int lstrcmp(const char*str1,const char*str2)
{
	return strcmp(str1,str2);
}


int daisy_load(daisy *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
    self->fCond_To_Cap = 0.0;    
    
    self->Frost_Rad_Flag = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.DAISY Modul Heat Transfer.frost_rad_flag",1);
    self->Lower_boundary_cond = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.DAISY Modul Heat Transfer.lower_boundary_condition",2);
    
    if ((self->Frost_Rad_Flag<0) || (self->Frost_Rad_Flag>3))
        {
            self->Frost_Rad_Flag=1;
        }
    if ((self->Lower_boundary_cond < 0) || (self->Lower_boundary_cond > 2))
        {
         self->Lower_boundary_cond = 2;  
        }
        
        
    //LOWER_BOUNDARY = 2; /* 0: Konst. Temp,  */
	/* 1: Kein Fluß, */
	/* 2: Berechnete Temp. (Daisy)*/
        
	self->__ERROR=RET_SUCCESS;
	self->too_high_or_low_temp=0;
	return RET_SUCCESS;
}

// Our Modul Functions:


// ----------------------------------------------DAISY ----------------------------------------------------------

/**********************************************************************************************/
/* Name     :   Temp_D()                                                                      */
/*                                                                                            */
/* Inhalt   :   Berechnung der Bodentemperaturen und des Frostes                              */
/*                                                                                            */
/* Modul         : TEMP_DAI.c                                                                *
 * Inhalt        : Berechnung der Bodentemperaturen                                          *
 *                 und des Frostes                                                           *
 *                                                                                           *
 * Methode       : E.Hansen (DAISY)                                                          *
 *                                                                                           *
 * Author        : CH                                                                        *
 *                 Annahme: Schichtdicke konstant.                                           *
 * Date          : 16.8.95, 10.10.95, 23.9.96                                                *
 *                 16.11.95 ThetaSat wird durch Frost herabgesetzt                           *
 *                 06.02.96 temp_daisy: Frost-Senkenterme nach DAISY                         *
 *                          temp_d:     Frost durch Energieansatz                            *
 *				   18.06.96 Infiltrationswasser mit mittl. Tages-Temp.                       *
 *                 02.10.96 dm, Implementierung der neuen Variablenstruktur                  *
 *********************************************************************************************/
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:			pHL->fSoilTemp                                                    */
/*							pHL->fSoilTempOld                                                 */
/*							pSH->afPartHeatCond[]                                             */
/*							pWL->fIce                                                         */
/*							pWL->fWaterFreezeDay                                              */
/**********************************************************************************************/
int daisy_run(daisy *self)
{
	/* ****************************************************************** */
	/* !!!     DeltaZ besitzt in dieser Funktion die Einheit Meter !!!    */
	/* ****************************************************************** */

	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	PCHEMISTRY pCh = xpn->pCh;
	PCLIMATE pCl = xpn->pCl;
	PHEAT pHe = xpn->pHe ;
	PLOCATION pLo = xpn->pLo;
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;




	PWLAYER pWL;
	PHLAYER pHL;




	int iLayer        = 0;








	//int Frost_Rad_Flag = 1;



	pWL = pWa->pWLayer;
	pHL = pHe->pHLayer;
	iLayer = 0;

	/* ****************************************************************** */
	/*      Zurücksetzen der am Tag kumulierten Grössen:                  */
	/* ****************************************************************** */
	if (NewDay(pTi))
		{
			for (SOIL_LAYERS0(pWL,pWa->pWLayer))
				pWL->fWaterFreezeDay = (double) 0;
		}


	/* ****************************************************************** */
	/* Obere Randbedingung:                                               */
	/* ****************************************************************** */

	/* Aktuelle Lufttemperatur mit sinusförmigem Tagesgang  */
	/* Falls DeltaT >= 0.2 dann  Tagesdurchschnittstemperatur */

	pHL->fSoilTempOld = pHL->fSoilTemp;

//   pHL->fSoilTemp =	pCl->pWeather->fTempAve;
	pHL->fSoilTemp =	pCl->pWeather->fTempAir;
	
	check_soil_temp(pHL->fSoilTemp)
	
	///*
	/*pHL->fSoilTemp = (DeltaT < (double)0.2)?
	                 pCl->pWeather->fTempAve
				  + fRedTAmpDay *
				    ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin)/(double)2)
	               * (double)sin((double)2.0 * (double)PI
	               * (DayTime - (double)0.25 ))

	               : pCl->pWeather->fTempAve;*/
	/*	pHL->fSoilTemp = (DeltaT < (double)0.2)?
	                    pCl->pWeather->fTempAir
					  + fRedTAmpDay *
					    ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin)/(double)2)
	                  * (double)sin((double)2.0 * (double)PI
	                  * (DayTime - (double)0.25 ))

	                  : pCl->pWeather->fTempAir;*/

	//*/


	//pHL->fSoilTemp = Get_Horton_UpperBC(exp_p);

	//pHL->fSoilTemp = pCl->pWeather->fTempAve
	//                -fRedTAmpYear*(pCl->pWeather->fTempAve-pCl->pAverage->fYearTemp);

	if ((pWa->pWBalance->fSnowStorage>(double)1)
	        &&(pWa->pWBalance->fSnowWaterContent >= (double)0))
		pHL->fSoilTemp = (double) 0;

	/* ****************************************************************** */
	get_Cond_Cap_Heat(xpn,exp_p);
	/* ****************************************************************** */


	/* ****************************************************************** */
	Solve_LGS(self,exp_p, self->Frost_Rad_Flag);
	/* ****************************************************************** */

	//daisy_Get_Horton_Surface_Energy_Balance(self,pHe->pHLayer->pNext->fSoilTemp);


	return RET_SUCCESS;
}

/*********************************************************************************************/
/* Name             : Solve_LGS(exp_p)                                                       */
/* Inhalt			: Interne Funktion zur Berechnung des linearen Gleichungssystems.        */
/*                    Ergebnis: Neue Temperaturen, evt. Quelle-/Senke für Wassermodul        */
/*                    durch Tauen und Frieren                                                */
/* Autor            : C.Sperr                                                                */
/* Datum            : 19.10.95                                                               */
/*                                                                                           */
/*   ch, 06.02.96 Frost wird durch Energieansatz berechnet.                                 */
/*   as, 27.11.96		Header: Var.veränderungen einführen.                                */
/*                                                                                          */
/********************************************************************************************/
/*		veränd. Var.:		pHe->fFreezingDepth                                             */
/*							pWL->WaterFreezeR                                               */
/*							pWL->WaterFreezeDay                                             */
/*							pWL->fIce                                                       */
/*							pWL->fIceOld                                                    */
/*							pHL->fSoilTemp                                                  */
/*							pHL->fSoilTempOld                                               */
/*							pSo->fAlbedo                                                    */
/*							pSo->fSoilAlbedo                                                */
/*							pSW->fContSat                                                   */
/********************************************************************************************/
int Solve_LGS(daisy *self,EXP_POINTER, int Frost_Rad_Flag)
{
	PSLAYER pSL;
	PSWATER pSW;
	PWLAYER pWL;
	PHLAYER pHL;


	expertn_modul_base *xpn = &(self->parent);
	int iLayer        = 0;
	
	double DeltaT     = pTi->pTimeStep->fAct;
	


	double *pA,*pB,*pC,*pD;   /* Hilfsgroessen */
	double fA,fB,fC,fD,fQ;
	double fE,fR,fS;
	double *pF, *pG;
//	double fFrozenDepth;
	int iLayerAnz =  pSo->iLayers;
	double DeltaZ      = pSo->fDeltaZ;

	//const int LOWER_BOUNDARY = 2; /* 0: Konst. Temp,  */
	/* 1: Kein Fluß, */
	/* 2: Berechnete Temp. (Daisy) */

	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
	pHL = pHe->pHLayer;

	iLayer = 0;

	/* ******************************************************************* */
	/* Achtung!     DeltaZ besitzt in dieser Funktion die Einheit Meter!!! */
	/* ******************************************************************* */

	DeltaZ /= (double) 1000;
	
/*	if ((pTi->pSimTime->iyear == 1985) && (pTi->pSimTime->fTimeY >= 164.9166748100354))
		{
		
                PRINT_ERROR_ID(xpn,"Soil Temp too high");
										
		}*/


	if ((Frost_Rad_Flag==2)||(Frost_Rad_Flag == 3))   /* berücksichtige Strahlungsterm */
		{
			// Albedo
			/*if (pWL->pNext->fContAct >= (double)0.25)
			     {pSo->fSoilAlbedo = (double) 0.1;}
			else if (pWL->pNext->fContAct <= (double)0.1)
			     {pSo->fSoilAlbedo = (double) 0.25;}
			else
			     {pSo->fSoilAlbedo = (double) 0.35 - pWL->pNext->fContAct;}

			// Gesamt-Albedo
			// if (lpb->gesAlbedo == (double)0.0)
			  {
			   pSo->fAlbedo = pSo->fSoilAlbedo;
			  }*/

			fE = (double)0.9 + (double)0.18 * pWL->pNext->fContAct;
			fS = (double) 5.67e-8;
			/*  fR = fS * abspower(pCl->pWeather->fTempAve + (double)273.169, (double)4.0)
			          * ((double) 0.605 + (double) 0.048
			          * abspower((double)1370.0 * pCl->pWeather->fHumidity / (double)100.0, (double)0.5));*/
			fR = fS * abspower(pCl->pWeather->fTempAir + (double)273.169, (double)4.0)
			     * ((double) 0.605 + (double) 0.048
			        * abspower((double)1370.0 * pCl->pWeather->fHumidity / (double)100.0, (double)0.5));
		}


	/* Speicher für Schichtvariable reservieren */

	pA        = (double *) malloc(iLayerAnz * sizeof(double));
	memset(pA,0x0,(iLayerAnz * sizeof(double)));
	pB        = (double *) malloc(iLayerAnz * sizeof(double));
	memset(pB,0x0,(iLayerAnz * sizeof(double)));
	pC        = (double *) malloc(iLayerAnz * sizeof(double));
	memset(pC,0x0,(iLayerAnz * sizeof(double)));
	pD        = (double *) malloc(iLayerAnz * sizeof(double));
	memset(pD,0x0,(iLayerAnz * sizeof(double)));

	pF        = (double *) malloc(iLayerAnz * sizeof(double));
	memset(pF,0x0,(iLayerAnz * sizeof(double)));
	pG        = (double *) malloc(iLayerAnz * sizeof(double));
	memset(pG,0x0,(iLayerAnz * sizeof(double)));

	for (pSL = pSo->pSLayer->pNext,
	        pSW = pSo->pSWater->pNext,
	        pWL = pWa->pWLayer->pNext,
	        pHL = pHe->pHLayer->pNext,
	        iLayer = 1; ((pSL->pNext != NULL)&&
	                     (pSW->pNext != NULL)&&
	                     (pWL->pNext != NULL)&&
	                     (pHL->pNext != NULL)); pSL = pSL->pNext,
	        pSW = pSW->pNext,
	        pWL = pWL->pNext,
	        pHL = pHL->pNext,
	        iLayer++)
		{
			/* ******************************************************* */
			/* Numerische Groessen in den Einheiten J, m, d, K         */
			/* ******************************************************* */

			/* Daisy (5-47) (ohne Frost) */
			fA = (pHL->fCapacity + pHL->fCapacityOld) / (double)2; /* [kJ /m^3 /K] */
			fB = (double) 0;                                       /* [kJ /m^3 /d] */
			fC = (pHL->fConduct + pHL->fConductOld)/(double)2;     /* [ kJ/d/K/m] */

			/* Daisy (5-50) */
			fD = (pHL->pNext->fConduct - pHL->pBack->fConduct
			      +  pHL->pNext->fConductOld - pHL->pBack->fConductOld)
			     / ((double)4*DeltaZ);                               /* [ kJ/K/m^2 /d] */

			fQ =  SPECCAP_WATER
			      * (pWL->fFluxDens    + pWL->pBack->fFluxDens
			         +  pWL->fFluxDensOld + pWL->pBack->fFluxDensOld)
			      *  DENSITY_WATER
			      / (double)4000;        /* [ J/kg /K * mm /d * kg/l * 1000] = [ kJ /K /m^2 /d]   */
                  
            

			CHECK_VALID(pWL->fFluxDens)
			CHECK_VALID(pWL->pBack->fFluxDens)
			CHECK_VALID(pWL->fFluxDensOld)
			CHECK_VALID(pWL->pBack->fFluxDensOld)

			/* 2.2.96 mit Komponente zum vorherigen Zeitschritt. */

			if (iLayer==1)   /* Obere Randbedingung fuer Koeffizienten. */
				{
					/* Daisy (5-57 bis 5-60) */
					pA[1] = (double) 0;                                    /* [ kJ /K /m^3 /d] */

					pB[1]  = fA / DeltaT
					         + fC / (DeltaZ * DeltaZ);
					//              + fC/ ((double)2 * DeltaZ * DeltaZ);            /* = [kJ /m^3 /K /d]   */

					pC[1] =  fQ / ((double) 4 * DeltaZ);

					pC[1] -= fC/ ((double) 2 * DeltaZ * DeltaZ);           /* = [kJ /m^3 /K /d]   */

					pD[1] = fA * pHL->fSoilTemp / DeltaT;

					pD[1] +=  fC
					          * (pHL->pNext->fSoilTemp - (double)2 * pHL->fSoilTemp
					             +  pHe->pHLayer->fSoilTempOld + pHL->pBack->fSoilTemp)
					          / ((double)2 * DeltaZ * DeltaZ);

					/* ch orig:  + fTempAktuell) / DeltaZ; */

					pD[1] -=  fQ
					          /*ch orig unterscheidet zw. TempBoden und TempInfiltwasser) */
					          /* ch 18.6.96
					             es wird nicht mehr * (lpT->BodenTemp)
					             benutzt, da Infiltration stets um 0:00 Uhr stattfindet und
					             Temp. um diese Uhrzeit deutlich geringer als Tagesmittelwert.*/

					          * (pCl->pWeather->fTempAir) / ( (double) 4 * DeltaZ);        /* [kJ /m^3 /d] */

					if ((Frost_Rad_Flag==2)||(Frost_Rad_Flag == 3))
						{
//         pD[1] -= ((double)1.0 - lpb->gesAlbedo)
//                * lpk->aktlPtrTempParam->SolRad/(double)800.0 * lpk->aktlPtrTempParam->Tmax;
							/*
							           pD[1] -= ((double)1.0 - lpb->gesAlbedo)
							                  * abspower((lpk->aktlPtrTempParam->SolRad*(double)0.03),(double)0.5)
							                  * lpk->aktlPtrTempParam->Durchschn;
							*/
							pD[1] += (((double)1.0 - pSo->fAlbedo)
							          * pCl->pWeather->fSolRad * (double) 11.905 + fR
							          - fE * fS * abspower((pHL->fSoilTemp + (double)273.16), (double)4.0))
							         * (double) 84.0;
						} /* Radiation */

				} /*iLayer==1 */

			else
				{
					pA[iLayer] = (fD - fQ )/((double)4 * DeltaZ)
					             -  fC/ ( (double) 2 * DeltaZ * DeltaZ);        /* [ J /K /m^3 /d] */

					pB[iLayer] = fA / DeltaT  + fC/ ( DeltaZ * DeltaZ);

					pC[iLayer] = (fQ - fD) /((double)4 * DeltaZ)
					             -  fC/ ( (double) 2 * DeltaZ * DeltaZ);

					pD[iLayer] = fA * pHL->fSoilTemp / DeltaT
					             + fC * (pHL->pNext->fSoilTemp - (double)2 * pHL->fSoilTemp + pHL->pBack->fSoilTemp)
					             / ((double) 2 * DeltaZ * DeltaZ);

					pD[iLayer] += (fD - fQ )
					              * (pHL->pNext->fSoilTemp - pHL->pBack->fSoilTemp)
					              / ( (double) 4 * DeltaZ);

					pD[iLayer] -= fB;

					CHECK_0(DeltaT);
					CHECK_0(DeltaZ);
					CHECK_VALID(fD);
					CHECK_VALID(fQ);
					CHECK_VALID(pA[iLayer]);
					CHECK_VALID(pB[iLayer]);
					if (CHECK_double_INVALID (pC[iLayer]))
						{
							char S[10000];
							PRINT_ERROR("Invalid value")
							sprintf(S,"fQ: %f,fD: %f ,DeltaZ: %f ,fC %f",fQ,fD,DeltaZ,fC);
							PRINT_WARNING_ID((&(self->parent)),S);
						}
					CHECK_VALID(pC[iLayer]);
					CHECK_VALID(pD[iLayer]);
					if (CHECK_double_INVALID (pD[iLayer]))
						{
							char S[10000];
							sprintf(S,"fTair: %f, fA: %f,pHL->fSoilTemp: %f,DeltaT: %f,fC: %f,pHL->pNext->fSoilTemp: %f,DeltaZ: %f,fB: %f",pCl->pWeather->fTempAir,fA, pHL->fSoilTemp,DeltaT,fC,pHL->pNext->fSoilTemp,DeltaZ,fB);
							PRINT_WARNING_ID((&(self->parent)),S);
						}

				} /* iLayer>1 */
		} /* Schichtweise      */


	/* *********************************************** */
	/*      Löser des Gleichungssystems H T = D        */
	/*          mit H = tridiag(A,B,C)                 */
	/* *********************************************** */

	pF[1] =  (double)pC[1] / (double)pB[1];
	pG[1] = ((double)pD[1] - (double)pA[1]* (double)pHe->pHLayer->fSoilTemp) / (double)pB[1];


	/* rekursive Definition von F und G: */
	for (iLayer=2; iLayer< iLayerAnz -1; iLayer++)
		{
			pF[iLayer] =  (double)pC[iLayer]
			              / ((double)pB[iLayer] - (double)pF[iLayer-1] * (double)pA[iLayer]);
			pG[iLayer] = ((double)pD[iLayer] - (double)pA[iLayer]   * (double)pG[iLayer-1])
			             / ((double)pB[iLayer] - (double)pF[iLayer-1] * (double)pA[iLayer]);

			if (CHECK_double_INVALID (pG[iLayer]))
				{

					PRINT_ERROR_ID(xpn,"Invalid value of var pG[iLayer]");
				}
                
           /* if (pG[iLayer]>60.0)
										{
											PRINT_ERROR_ID(xpn,"Soil Temp too high");
										}*/

			CHECK_VALID(pG[iLayer]);
			CHECK_VALID(pF[iLayer]);
		}

	pHL->fSoilTempOld = pHL->fSoilTemp;


	/* **************************************************************** */
	/*      Untere Randbedingung:                                       */
	/* **************************************************************** */
	switch(self->Lower_boundary_cond)
		{
		case 0:  /*  Temp =konst = Temp(t=0) */
			break;

		case 1:  /* Keine Wärmebewegung */

			/* lpT Zeiger befindet sich auf letzter Schicht */
			pHL->fSoilTemp = pHL->pBack->fSoilTemp - pHL->fSoilTemp
			                 + (double)pG[iLayer-1];
							 						

			pHL->fSoilTemp /= ((double)1.0 + (double)pF[iLayer-1]);
			
			check_soil_temp(pHL->fSoilTemp);
			break;

		case 2:
			/* Dirichletbedingung mit sinusförmiger Temperatur  */
			/*          (Periode: 1Jahr)     */
            
			CHECK_VALID(pHL->fSoilTemp)
			//if ((NewDay(pTi)) || SimStart(pTi))
			pHL->fSoilTemp =  daisy_Get_Daisy_LowerBC(self,exp_p);
			check_soil_temp(pHL->fSoilTemp);

		} /* switch */
 
	CHECK_VALID(pHL->fSoilTemp);
    check_soil_temp(pHL->fSoilTemp);

	/* **************************** rückwärts errechnen: Tk => T(k-1) */
	/* Zeiger lpT, lpXH ist auf letzter Schicht, iLayer = 10 */

	for (iLayer--,     pHL = pHL->pBack;      /* Setze Zeiger auf letzte Bodenschicht: */
	        (iLayer>0) && (pHL->pBack != NULL);
	        iLayer--,     pHL = pHL->pBack)
		{
			/* Speichern der alten Werte: */
			pHL->fSoilTempOld = pHL->fSoilTemp;
			pHL->fSoilTemp    = (double)pG[iLayer] - (double)pF[iLayer] * pHL->pNext->fSoilTemp;
			CHECK_VALID(pHL->fSoilTemp)
			check_soil_temp(pHL->fSoilTemp);
			

		}
			


	if ((Frost_Rad_Flag==1)||(Frost_Rad_Flag == 3))
		{
			/* **************************************************************************** */
			/* Errechnen der Schmelz- und Friervorgänge,                                    */
			/* Korrektur der Temperatur:                                                    */
			/* **************************************************************************** */
			for (pSL = pSo->pSLayer->pNext,
			        pSW = pSo->pSWater->pNext,
			        pWL = pWa->pWLayer->pNext,
			        pHL = pHe->pHLayer->pNext,
			        iLayer = 1; ((pSL->pNext != NULL)&&
			                     (pSW->pNext != NULL)&&
			                     (pWL->pNext != NULL)&&
			                     (pHL->pNext != NULL)); pSL = pSL->pNext,
			        pSW = pSW->pNext,
			        pWL = pWL->pNext,
			        pHL = pHL->pNext,
			        iLayer++)
				{
					pWL->fWaterFreezeR = (double) 0; /* weder Frieren noch Tauen. */

					/* Frieren: */
					if ((pHL->fSoilTemp  < (double)0) && (pWL->fContAct > THETA_MIN + (double)0.01))
						{

// #ifdef FREEZING_SETS_MINIMAL_TIMESTEP
							/* Frost bedarf genügend kleinen Zeitschritts! *-/
							if (pTi->pSimTime->iTimeFlag1 == 0)
							   {
							     Set_Minimal_Time_Step(pTi);
							   }
							else
							   if (pTi->pSimTime->iTimeFlag2 > 20)
							      {
							         pTi->pSimTime->iTimeFlag2 = 20;     /-* entspricht ca. DeltaT = 0.001 *-/
							      }
							#endif*/

							/* fFreeze ist die Geschwindigkeit, mit der der vol.  */
							/* Wassergehalt frieren muss, um die Temperatur auf  */
							/* 0 Grad zu erhöhen. */
							pWL->fWaterFreezeR = (double)-0.001 * pHL->fSoilTemp * pHL->fCapacity   /* [1000 K * kJ/m^3/K] */
							                     / (DENSITY_WATER * LAT_HEAT_MELT * DeltaT);     /* [kJ/kg * kg/l *d] */
							/* = [1000 l/m^3d/] = [1/d] */
#ifdef LOG_FREEZING
							if (pWL->fIceOld == (double)0)
								{
#ifdef LOGFILE

									wsprintf(lpOut,"*        Temp_dai: Layer %d starts freezing.", iLayer);
									Message(0,lpOut);
#endif
								}
#endif

							if ((pWL->fWaterFreezeR * DeltaT <= pWL->fContAct - (THETA_MIN + EPSILON))&&
							        (pWL->fWaterFreezeR * DeltaT <= (double)10 * (pSW->fContSat - pWL->fContAct)))
								{
									/* Kein Wassermangel und kein Volumenmangel.  */
									/* => Frieren uneingeschränkt möglich */
									pHL->fSoilTemp = (double) 0;
								}
							else
								{
									/* Einschraenkung */
									double FrostNeeded = pWL->fWaterFreezeR;

									if (pWL->fWaterFreezeR * DeltaT > (double)10 * (pSW->fContSat - pWL->fContAct))
										{
											pWL->fWaterFreezeR = MAX((double)0,(double)10 * (pSW->fContSat - pWL->fContAct) / DeltaT);

											/*
											#ifdef LOGFILE
											wsprintf(lpOut,"!        Temp_dai, SolveLGS(): Freezing (%d mm) exceeds free pore space (%d mm) in Layer %d.", \
											iLayer, (int)(lpXI->fWaterFreeze * DeltaT), (int)(lpGG->ThetaSat - lpGH->aktuell));
											Message(0,lpOut);
											#endif
											*/
										}

									if (pWL->fWaterFreezeR * DeltaT > pWL->fContAct - (THETA_MIN + EPSILON))
										/* Wassermangel */
										{
#ifdef LOGFILE
											if (pWL->fWaterFreezeR * DeltaT - pWL->fContAct - (THETA_MIN + EPSILON) > (double)5 / DeltaZ)
												{
													Message(0, ERROR_FREEZING_EXCEEDS_H2O);
												}
#endif

											pWL->fWaterFreezeR = MAX((double)0,(pWL->fContAct - (THETA_MIN + EPSILON)) / DeltaT);
										}

									pHL->fSoilTemp *= (double)1 - pWL->fWaterFreezeR / FrostNeeded;
									
									check_soil_temp(pHL->fSoilTemp);

								}   /* Frieren nur eingeschraenkt moeglich. */

						} /* Frieren */

// #ifdef FREEZING_SETS_MINIMAL_TIMESTEP
					else
						/* Tauen: */
						if ((pHL->fSoilTemp  > (double)0)&&(pWL->fIce > (double)0))
							{
								/* Tauen bedarf genügend kleines Zeitschritts! *-/
								if (pTi->pSimTime->iTimeFlag1 == 0)
								   {
								     Set_Minimal_Time_Step(pTi);
								   }
								else
								   if (pTi->pSimTime->iTimeFlag2 > 20)
								      {
								         pTi->pSimTime->iTimeFlag2 = 20;     /-* entspricht ca. DeltaT = 0.001 *-/
								      } */

#ifdef LOG_FREEZING
#ifdef LOGFILE
								if (pWL->fIce >  pWL->fIceOld + EPSILON)
									{
										wsprintf(lpOut,"*        Temp_dai, SolveLGS(): Layer %d starts thawing.", iLayer);
										Message(0,lpOut);
									}
#endif
#endif

								/* fFreeze ist der vol. Wassergehalt, der schmelzen muss, um Temperatur auf O Grad */
								/* zu erniedrigen. */
								pWL->fWaterFreezeR = (double)-0.001 * pHL->fSoilTemp * pHL->fCapacity  /*   [1000 K * kJ/m^3/K] */
								                     / (DENSITY_WATER * LAT_HEAT_MELT * DeltaT);    /*   [kJ/kg * kg/l * d] */
								/* = [1000 l/m^3/d] = [1/d] */


								if (pWL->fWaterFreezeR * DeltaT + pWL->fIce  / (double)1.1 >= (double)0)
									{
										/* Tauen uneingeschränkt möglich => Temp erhöht sich nicht! */
										pHL->fSoilTemp = (double) 0;
									}
								else
									{
#ifdef LOG_FREEZING
#ifdef LOGFILE
										if (pWL->fWaterFreezeR >= (double)0)
											{
												wsprintf(lpOut,"*        Temp_dai, SolveLGS(): Layer %d thawed.", iLayer);
												Message(0,lpOut);
											}
#endif
#endif

										pHL->fSoilTemp    *= (double)1 + pWL->fIce / (pWL->fWaterFreezeR  * DeltaT  * (double)1.1);
										check_soil_temp(pHL->fSoilTemp);
										pWL->fWaterFreezeR = (double)-1 * pWL->fIce / (DeltaT * (double)1.1);
									}

							} /* Tauen  */

					/* Veränderung des Eisgehaltes im Wasserteil:
					   pWL->fIceOld = pWL->fIce;

					   /-* fIce ist vol. Eisgehalt *-/
					    pWL->fIce += (double)1.1 * pWL->fWaterFreezeR * DeltaT;
						pWL->fContAct -= pWL->fWaterFreezeR * DeltaT;

					   if (pWL->fIce < (double)0)
					      {
					        #ifdef LOGFILE
					         if (pWL->fIce + EPSILON < (double)0)
					           {
					    			 		 strcpy((LPSTR)lpOut,"fIce: ");
											 strcat((LPSTR)lpOut,RANGE_ERROR_TXT);
								      		 Message(0, lpOut);
					           }
					        #endif

					        pWL->fIce = (double)0;
					      }

					   pSW->fContSat = max(pSL->fPorosity - pWL->fIce, THETA_MIN);

					  if (pSW->fContSat < THETA_MIN)
					     {
					         #ifdef LOGFILE
					    			 		 strcpy((LPSTR)lpOut,"fContSat: ");
											 strcat((LPSTR)lpOut,RANGE_ERROR_TXT);

					           if (pSW->fContSat < THETA_MIN - (double)0.01)
								      		 Message(0, lpOut);
					           #endif
					         pSW->fContSat = THETA_MIN;
					     }

					*/

					pWL->fWaterFreezeDay += pWL->fWaterFreezeR * DeltaT;

				} /* For Layer */

		} /* Frost-Korrektur */
	/* Ende des Lösers des Gleichungssystems H T = D */

//	fFrozenDepth         =
//	    pHe->fFreezingDepth  = (double)0;

	for(SOIL_LAYERS1(pHL, pHe->pHLayer->pNext))
		{
			if (pHL->fSoilTemp <= (double)0)
				{
					pHe->fFreezingDepth = iLayer * DeltaZ;

					//				if (pHL->fSoilTemp + EPSILON < (double)0)
					//					fFrozenDepth = iLayer * DeltaZ;
				}
		}
		
	
	if (self->too_high_or_low_temp==1)
	{
		double lower_temp;
		double upper_temp;
								
		lower_temp = daisy_Get_Daisy_LowerBC(self,exp_p);
		upper_temp = xpn->pCl->pWeather->fTempAir;
				for (pSL = pSo->pSLayer->pNext,
							pSW = pSo->pSWater->pNext,
							pWL = pWa->pWLayer->pNext,
							pHL = pHe->pHLayer->pNext,
							iLayer = 1; ((pSL->pNext != NULL)&&
									(pSW->pNext != NULL)&&
									(pWL->pNext != NULL)&&
									(pHL->pNext != NULL)); pSL = pSL->pNext,
									pSW = pSW->pNext,
									pWL = pWL->pNext,
									pHL = pHL->pNext,
									iLayer++)
							{					
									pHL->fSoilTemp = (((double)iLayer-1.0)/(double)(xpn->pSo->iLayers-2))*(lower_temp-upper_temp)+upper_temp;
				
							}
		
			PRINT_ERROR_ID(xpn,"Soil Temperatures new initialized");
			self->too_high_or_low_temp=0;
	}


	/* Speicher für Schichtvariable freigeben */
	free(pA);
	free(pB);
	free(pC);
	free(pD);
	free(pF);
	free(pG);

	return 1;
} /* end Solve_LGS */

/*********************************************************************************/
/*                                                                               */
/*   Get_Daisy_LowerBC(exp_p);                                                   */
/*                                                                               */
/*   Berechnung der Temperaturbedingungen                                        */
/*   am unteren Profilrand                                                       */
/*   für Daisy-Temperaturmodell                                                  */
/*   Aufruf bei NewDay                                                           */
/*                                                                               */
/*   ( Sinusförmige Dirichletbed. mit jährlicher Periode)                        */
/*                                                                               */
/*   ch, 12.10.95                                                                */
/*   as, 27.11.96		Header: Variablenänderungen erfaßt.                      */
/*                                                                               */
/*********************************************************************************/
/*  veränd. Var.:		pCl->pAverage->fYearTemp                                 */
/*						pCl->pAverage->fMonthTempAmp                             */
/*                                                                               */
/*********************************************************************************/


double daisy_Get_Daisy_LowerBC(daisy *self,EXP_POINTER)
{
	int iLayer        = 0;

	PHLAYER pHL;

	double fD, fOmega, fZ,
	       fMiddleCap,fMiddleCond,
	       fLowerTemp,
	       fDayMaxTemp = (double)190; /*! ch Schätzwert. */
	/* Tag-Nr. mit max. Luft-Temperatur */
	double fMonthTempAmp;
	double fYearTemp;

	fMonthTempAmp = (&(self->parent))->pCl->pAverage->fMonthTempAmp;
	fYearTemp= (&(self->parent))->pCl->pAverage->fYearTemp;

// TODO: --> in Datenbank

	/* ******************************************************************* */
	/* Plausibilitätstest durchführen */
	/* ******************************************************************* */
	if (SimStart(pTi))
		{

			/*
			Da Mittlere Jahres-Temp. und -Amplitude nicht eingelesen werden,
			werden diese Werte fuer Scheyern hier gesetzt.
			*/

			//pCl->pAverage->fYearTemp      = (double)7.4;//fuer Scheyern
			//pCl->pAverage->fMonthTempAmp  = (double)10.4;//(double)10.4;//fuer Scheyern
			//pCl->pAverage->fMonthTempAmp  = (double)6;//fuer Solling



			/* ******************************************************************* */
			/*  Quotient aus mittlerer Waerme-Kapazitaet und Konduktivität berechnen */
			/* ******************************************************************* */
			fMiddleCap = fMiddleCond = (double)0;

			for (SOIL_LAYERS1(pHL, pHe->pHLayer->pNext))
				{
					fMiddleCap  += pHL->fCapacity;
					fMiddleCond += pHL->fConduct;
				}

			//Set_At_Least_Epsilon(&fMiddleCap, "fMiddleCap");

			self->fCond_To_Cap = fMiddleCond/fMiddleCap;
		} /* t=0 */

	fOmega = (double) 2 * (double)PI / (double)365;          /*[1/d] */

	fD = (double)sqrt((double)2 *self->fCond_To_Cap /fOmega);     /*[m] */

	fZ = (pSo->iLayers-2) * pSo->fDeltaZ / (double) 1000;  /*[m] */

	/* ******************************************************************* */
	/* Formel für untere RB ( Lösung der vereinfachten Wärmeleitungs-DGL)  */
	/* ******************************************************************* */




	fLowerTemp = fYearTemp
	             + fMonthTempAmp
	             * (double)exp(-fZ/fD)
	             * (double)cos((double)fOmega*(pTi->pSimTime->fTimeY - fDayMaxTemp) - fZ/fD);

	return fLowerTemp;
}

/***********************************************************************************/
/*  name    :    abspower(..)                                                      */
/*  function:    return                                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/
double  abspower(double x, double y)
{
	double sign = (double)1.0,z=(double)0.0;
	if (x < (double)0.0)
		{
			sign = (double)-1.0;
			x *= sign;
		}

	if (y < (double)0.0)
		{
			if (x != (double)0.0)
				x = (double)1.0/x;
			y *= (double)-1.0;
		}


	z = (double)pow((double)x,(double)y);
	return  z;
}





// Setzt x auf EPSILON falls x < EPSILON
// Setzt x auf EPSILON falls x < EPSILON
int Set_At_Least_Epsilon(expertn_modul_base *xpn,double *x, const char *Message)
{
	if (*x < EPSILON) {
		*x = EPSILON;
		if (Message!=NULL) {
			PRINT_WARNING_ID(xpn,Message);
		}
		return 0;
	}
	return 1;
}

/*********************************************************************************************/
/* Name             : Get_Cond_Cap_Heat(exp_p)                                               */
/* Inhalt			: Interne Funktion zur Berechnung von Waermekapazitaeten, Waerme         */
/*                    Waermeleitfaehigkeiten (nach De Vries mit Hilfe von Geometrie-Faktoren)*/
/* Autor            : C.Sperr geaendert durch C.Klein                                        */
/* Datum            : 10.10.95    August 2010                                                */
/*                                                                                           */
/*   as, 27.11.96 	Header: Variablenveränderungen erfaßt.                                   */
/*                                                                                           */
/*********************************************************************************************/
/*   veränd. Var.:			pHL->fCapacity                                                   */
/*							pHL->fCapacityOld                                                */
/*							pHL->fHeat                                                       */
/*							pHL->fHeatLatent                                                 */
/*							pHL->fConduct                                                    */
/*							pHL->fConductOld                                                 */
/*********************************************************************************************/
int get_Cond_Cap_Heat(expertn_modul_base *xpn,EXP_POINTER)
{
	PSLAYER pSL;
	PSWATER pSW;
	PWLAYER pWL;
	PHLAYER pHL;

	int iLayer        = 0;



	int i, k;
	int iContinMed;   /* Nummer des kontinuierlichen Mediums */
	double f1,f2;
	double fform[6];

	double DeltaZ      = pSo->fDeltaZ;

	double fPartWaermeLeitf5 = 2.2;     /* Eis   [W /m /K]  */
	double fWaterContMed     = 0.2;     /*! ch 17.8.95 Test  */
	/* Wasser immer kont. Medium */
	/* LeachM: 0.2 */
	/* Daisy berechnet dies mit WKP und fC */
	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
	pHL = pHe->pHLayer;

	iLayer = 0;

	/* ******************************************************************  */
	/* Achtung!     DeltaZ besitzt in dieser Funktion die Einheit Meter!!! */
	/* ******************************************************************  */

	DeltaZ /= 1000.0;


	/*  Waerme-Kapazitaet berechnen */
	for ( pSL = pSo->pSLayer->pNext,
	        pSW = pSo->pSWater->pNext,
	        pWL = pWa->pWLayer->pNext,
	        pHL = pHe->pHLayer->pNext,
	        iLayer = 1; ((pSL->pNext != NULL)&&
	                     (pSW->pNext != NULL)&&
	                     (pWL->pNext != NULL)&&
	                     (pHL->pNext != NULL)); pSL = pSL->pNext,
	        pSW = pSW->pNext,
	        pWL = pWL->pNext,
	        pHL = pHL->pNext,
	        iLayer++)
		{

			pHL->fCapacityOld  = pHL->fCapacity;

			pHL->fCapacity  = DENSITY_WATER   * SPECCAP_WATER   * pWL->fContAct;
			pHL->fCapacity += DENSITY_ICE     * SPECCAP_ICE     * pWL->fIce;
			pHL->fCapacity += DENSITY_MINERAL * SPECCAP_MINERAL * pSL->afPartVolFrac[0];  /* Mineralien */

			/* lpXH->fCapacity *= (double)1000; */

			/* [kg/l * J/kg /K ] = [J/l /K ] = [ kJ /m^3 /K]  */

			///*
			if ((!strcmp(pSL->acSoilID,"Str"))||
			        (!strcmp(pSL->acSoilID,"Hum")))
				{
					//pHL->fCapacity = DENSITY_HUMUS * SPECCAP_HUMUS;
					pHL->fCapacity  = DENSITY_WATER   * SPECCAP_WATER   * pWL->fContAct;
					pHL->fCapacity += DENSITY_HUMUS * SPECCAP_HUMUS * (1.0-pSL->fPorosity);
				}
			//*/

			/*lpXH->fHeat = lpXH->fCapacity * (lpT->BodenTemp + (double)273);    = [ kJ /m^3] */
			pHL->fHeat = pHL->fCapacity * pHL->fSoilTemp;                  /* = [ kJ /m^3] */

			pHL->fHeatLatent = pWL->fIce * LAT_HEAT_MELT        /* [ 1 * kJ/kg * kg/l / 1000] */
			                   * DENSITY_ICE        /* = [ kJ / m^3] */
			                   * 1000.0;


			//if (NewDay(pTi))
			/*if (NewTenthDay(pTi))*/
			{

				iContinMed = (pWL->fContAct > fWaterContMed)? 3 : 4; /* Wasser : Luft */

				pHL->fConductOld = pHL->fConduct; /* Speichern des alten Wertes */

				/* ****************************************************************** */
				/*   Berechnung der Waermeleitfähigkeiten                             */
				/*   nach De Vries mithilfe von Geometrie-Faktoren                    */
				/* ****************************************************************** */

				for (i = 0 ; i<=5; i++)   /* 5 Fraktionen */
					{
						for (k = 0, fform[i] = (double)0; k<3; k++)   /* drei Richtungen */
							{
								f1 = (i<5) ?
								     pSo->pSHeat->afPartHeatCond[i] / pSo->pSHeat->afPartHeatCond[iContinMed]
								     : fPartWaermeLeitf5 / pSo->pSHeat->afPartHeatCond[iContinMed]; /* Eis */

								f1 -= (double) 1;
								f1 *= pSo->pSHeat->afPartGeom[k][1]; /* Aufteilung:  0.125, 0.125, 0.75  */
								f1 += (double) 1;

								fform[i] += (double)1 / f1;
							} /* next k  */
					} /* next i */

				f1 = fform[1] * ((double)1- pSL->fPorosity) * pSo->pSHeat->afPartHeatCond[1]; /* Minerale */
				f2 = fform[1] * ((double)1- pSL->fPorosity);

				/* fehlt: org. Substanz */

				f1 += fform[3] * pWL->fContAct * pSo->pSHeat->afPartHeatCond[3]; /* Wasser */
				f2 += fform[3] * pWL->fContAct;

				f1 += fform[4] * ( pSL->fPorosity - pWL->fContAct - pWL->fIce) * pSo->pSHeat->afPartHeatCond[4]; /* Luft */
				f2 += fform[4] * ( pSL->fPorosity - pWL->fContAct - pWL->fIce);

				f1 += fform[5] * pWL->fIce * fPartWaermeLeitf5; /* Eis */
				f2 += fform[5] * pWL->fIce;


				Set_At_Least_Epsilon(xpn,&f2, "f2\n in get_Cond_Cap_Heat()");

				pHL->fConduct =  (double)86.4 * f1 / f2;   /* J/s/K/m/86.4 = [kJ/d/K/m] */

				///*
				if ((!strcmp(pSL->acSoilID,"Str"))||
				        (!strcmp(pSL->acSoilID,"Hum")))
					{
						pHL->fConduct =(
						                   (pSL->fPorosity - pWL->fContAct) * (double)0.025 //Luft 0.025
						                   +((double)1-pSL->fPorosity) * //(double)0.025 //Streu wie Luft
						                   ((double)0.2 * pWL->fContAct + (double)0.015) //Streu & Wassergehalt
						               )
						               * (double)3600.0 * (double)24.0/(double)1000; //[J/s/K/m]->[kJ/d/K/m]
					}
				//*/

			}  /* Neue Konduktivität nur bei NewTenthDay  */

		} /* Schichtweise */

	return 1;
} /* end Get_Cond */


int daisy_calc_latent_heat(daisy *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Tair,Tsoil,wind,ra,Tdew,lgea,es,ea,hum,Psi_Soil,Psi1;
	double z0h,z0v,lgz0h,lgz0v,lgz02;
	double SoilHum,AirHum,SoilHum0;
	double eta,E,L;
	//Air Temp  [°C]
	Tair = xpn->pCl->pWeather->fTempAir_zlvl;
	// Soil Temp [°C]
	Tsoil = xpn->pHe->fTempSurf;
	// rel. Humidity [%]
	hum = xpn->pCl->pWeather->fHumidity;
	// roughness length
	z0h = xpn->pPl->pCanopy->fRoughnessHeat;
	z0v = xpn->pPl->pCanopy->fRoughnessVapor;
	// Windspeed [m/s]
	wind = MAX(xpn->pCl->pWeather->fWindSpeed,1.E-4);

	// Matrix Potential [mm]
	Psi_Soil =  xpn->pWa->pWLayer->pNext->fMatPotAct;

	// ra: areodynamic boundary layer resitance [s/m]
	lgz0h = log(2.0/z0h);
	lgz0v = log(2.0/z0v);
	lgz02 = lgz0h*lgz0v;
	ra = lgz02 / (0.16 * wind);


	//saturation vapour pressure: es [kPa]:
	es = 0.6108*exp( (17.27*Tair)/(Tair+237.3));
	//actual vapour pressure ea  [mBar]
	ea = hum / 100.0 * (es*10.0);
	lgea = log10(ea);
	// DewPoint Temperature
	Tdew = (234.18*lgea - 184.2) / (8.204 - lgea);
	//AirHum absolute air humidity[kg/m3]:
	AirHum = 1.312 * exp(17.27*Tdew/(237.3+Tdew))/(Tair+273.16);


	// air humidity at soil surface [kg/m^3]
	Psi1 = MIN(0.0, Psi_Soil/1000.0);
	SoilHum0 = 1.323 * exp(17.27*Tsoil/(Tsoil+237.3))/(Tsoil+273.16);
	SoilHum = SoilHum0 * exp(Psi1/(46.97*(Tsoil+273.16)));



	// Evaporation Flux [m/s]:
	E = (SoilHum - AirHum) / (1000.0*ra);
	// latent heat of vaporization [J / m3]
	L = 2.49463e9 - 2.247e6 * Tsoil;
	eta = L * E;
	//fLH = ((fHS-fH)/fA/(double)1000.0) * ((double)2.49463e9 - (double)2.247e6 * fTS);
	xpn->pHe->pHEBalance->fHeatLatent = eta;
	return RET_SUCCESS;
}


//  CONVERT EVAP TERMS FROM KINEMATIC (KG M-2 S-1) TO ENERGY UNITS (W M-2)
#define LVH2O 2.501E+6


int daisy_calc_latent_heat2(daisy *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double eta,L;
	double T,T2,T3;

	T = xpn->pCl->pWeather->fTempAir_zlvl;
	T2=T*T;
	T3=T2*T;

	// Cubic fit to Table 2.1,p.16, Textbook: R.R.Rogers & M.K. Yau, A Short Course in Cloud Physics, 3e,(1989), Pergamon press
	// The latent heat of condensation of water in the temperature range from −40 °C to 40 °C is approximated by the following empirical cubic function:
	L = -(0.0000614342*T3 + 0.00158927*T2 - 2.36418*T + 2500.79)*1e-3*1e6; // für J /m2/day
	C_DEBUG(L);
	//eta = -L * xpn->pWa->fActET/86400.0;// / 2.45 /0.0864; // [mm]/[day] --> W m-2

	eta = (LVH2O * xpn->pWa->fActETR) /(24.0*60.0*60.0); //1/day --> 1/s

	xpn->pHe->pHEBalance->fHeatLatent = eta;
	C_DEBUG(xpn->pHe->pHEBalance->fHeatLatent);
	return RET_SUCCESS;
}

// ret [W/m2]
int daisy_calc_sensible_heat(daisy *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double Tair,Tsoil,wind,ra;
	double sheat,ch,z0h,z0v,lgz0h,lgz0v,lgz02;


	//Air Temp  [°C]
	Tair = xpn->pCl->pWeather->fTempAir;//xpn->pHe->fTempSurf;//xpn->pCl->pWeather->fTempAir;;//xpn->pCl->pWeather->fTempAir;	//xpn->pCl->pWeather->fTempAir_zlvl;
	// Soil Temp [°C]
	Tsoil = xpn->pHe->fTempSurf;//xpn->pHe->pHLayer->pNext->fSoilTemp;
	// Windspeed [m/s]
	wind = MAX(xpn->pCl->pWeather->fWindSpeed,1.E-4);
	/* roughness length */
	z0h = xpn->pPl->pCanopy->fRoughnessHeat;
	z0v = xpn->pPl->pCanopy->fRoughnessVapor;
	// ch: air heat capacity [J / (°C m3)]
	ch = 1154.8 + 303.16 /(Tair+273.16);
	// ra: areodynamic boundary layer resitance [s/m]
	lgz0h = log(2.0/z0h);
	lgz0v = log(2.0/z0v);
	lgz02 = lgz0h*lgz0v;
	ra = lgz02 / (0.16 * wind);
	sheat = -ch * (Tsoil - Tair) / ra;
	xpn->pHe->pHEBalance->fHeatSens = sheat;
	return RET_SUCCESS;
}


double daisy_get_energy_balance2(daisy *self,double fTS)
{
	expertn_modul_base *xpn = &(self->parent);

	PCLIMATE pCl = xpn->pCl;
	PHEAT pHe = xpn->pHe ;

	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;


	PWLAYER pWL;
	PHLAYER pHL;



	double DeltaT     = pTi->pTimeStep->fAct;


	PWEATHER pCW = pCl->pWeather;

	double DeltaZ       = pSo->fDeltaZ;
	//double DayTime      = pTi->pSimTime->fTimeDay;

	double fA, fC, fD, fH, fP, fT;
	double fRN, fSH, fHS, fLH, fGS, fHK, fHC, fBR;
	double z0v,z0h,lgz0v,lgz0h,lgz02;

	/* depth [mm]-->[m] */
	DeltaZ /= (double) 1000.0;


	pWL = pWa->pWLayer;
	pHL = pHe->pHLayer;

	fT = pHL->fSoilTemp;
//   fT = pCl->pWeather->fTempAve;

	/*average daily dew point temperature */
	fD = (pCW->fHumidity / (double)100.0)
	     * (double)6.11 * (double) exp((double)((double)17.269 * fT) / ((double)((double)237.3 + fT)));
	fD = ((double) 234.67 * (double) log10((double)fD)-(double)184.2)
	     / ((double) 8.233 - (double)log10((double)fD));

	fD = (DeltaT < (double)0.2) ? fD + fT : fD;;

	/*(DeltaT < (double)0.2) ?
	     fD + ((pCW->fTempMax - pCW->fTempMin) / (double)2) * (double)sin((double)2 * (double)PI * (DayTime-(double)0.25))
	   : fD;*/

	/* surface emissivity */
//	fE = (double)0.9 + (double)0.18 * pWL->pNext->fContAct;

	/* absolute air humidity [kg/m^3] */
	fH = (double) 1.323 * (double) exp((double)17.27 *(double)(fD/((double)237.3 + fD)))
	     / (fT + (double) 273.16);

	/* Stefan-Boltzmann Constant */
	//fS = (double) 5.67e-8;

	/* longwave sky irridiance */
//	fR = fS * abspower((fT + (double)273.16), (double)4.0)
//	     * ((double) 0.605 + (double) 1.777 * abspower(fH, (double)0.5));

	/* roughness length */
	//fL = (double)xpn->pPl->pCanopy->fRoughnessHeat;
	z0h = xpn->pPl->pCanopy->fRoughnessHeat;
	z0v = xpn->pPl->pCanopy->fRoughnessVapor;
	lgz0h = log(2.0/z0h);
	lgz0v = log(2.0/z0v);
	lgz02 = lgz0h*lgz0v;
	fA = lgz02 / (0.16 * pCW->fWindSpeed);

	/* aerodynamic boundary layer resistance */
	//fA = abspower((double)log((double)2.0/(double)fL),(double)2.0)/((double)0.16 * pCW->fWindSpeed);
	fA = MAX(fA,EPSILON);

	/* air heat capacity */
	fC = (double) 1154.8 + (double) 303.16 /(fT+(double)273.16);

	/* soil surface matrix potential [m] */
	fP = MIN((double)0, pWL->pNext->fMatPotAct/(double)1000.0);

	/* net radiation */
	//fRN = ((double)1.0 - pSo->fAlbedo) * pCW->fSolRad
	//      * (double) 11.57                                   /*SolRad [MJ/d/m^2] --> [J/s/m^2] */
	//      + fR - fE * fS * abspower((fTS + (double)273.16), (double)4.0);

	fRN = xpn->pHe->pHEBalance->fNetRad;

	/* sensible heat flux */
	fSH = fC * (fTS - fT) / fA;




	/* air humidity at soil surface [kg/m^3] */
	fHS = (double) 1.323 * (double) exp((double)17.27 * (double)(fTS/((double)237.3 + fTS)))
	      / (fTS + (double) 273.16);

	fHS = fHS *(double) exp((double)fP/((double)46.97 * (double)(fTS + (double) 273.16)));

	/*latent heat flux */
	fLH = ((fHS-fH)/fA/(double)1000.0) * ((double)2.49463e9 - (double)2.247e6 * fTS);


	/* soil heat flux density at soil surface */
	fHK = (double) 0.5 * (pHL->pNext->fConduct + pHL->pNext->pNext->fConduct)
	      * (double) 1000.0 /(double) 86400.0;

	fHC = (double) 0.5 * (pHL->pNext->fCapacity + pHL->pNext->pNext->fCapacity)
	      * (double) 1000.0;

	fGS = fHK * (fTS - pHL->pNext->pNext->fSoilTemp) / DeltaZ
	      + fHC * (fTS - pHL->pNext->fSoilTemp) * DeltaZ / (DeltaT * (double)86400.0) / (double)2.0;

	/* energy balance at soil surface [J/s/m^2 = W/m^2]*/
	fBR = fRN - fSH - fLH - fGS;    /* residual of */

//   fBR = fRN-fSH-fGS;              /* residual of */

	return fBR;
}

/*********************************************************************************************/
/*                                                                                           */
/*   Get_Horton_Surface_Energy_Balance(all_lp,fTS);                                          */
/*                                                                                           */
/*   Berechnung der Energiebilanz                                                            */
/*   am oberen Profilrand                                                                    */
/*   für Daisy-Temperaturmodell                                                              */
/*   Aufruf bei NewDay                                                                       */
/*                                                                                           */
/*   ep/gsf 25.9.96                                                                          */
/*********************************************************************************************/
double daisy_Get_Horton_Surface_Energy_Balance(daisy *self, double fTS)
{
	expertn_modul_base *xpn = &(self->parent);

	PCLIMATE pCl = xpn->pCl;
	PHEAT pHe = xpn->pHe ;

	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;


	PWLAYER pWL;
	PHLAYER pHL;



	double DeltaT     = pTi->pTimeStep->fAct;


	PWEATHER pCW = pCl->pWeather;

	double DeltaZ       = pSo->fDeltaZ;
	//double DayTime      = pTi->pSimTime->fTimeDay;

	double fA, fC, fD, fE, fH, fL, fP, fR, fS, fT;
	double fRN, fSH, fHS, fLH, fGS, fHK, fHC, fBR;

	/* depth [mm]-->[m] */
	DeltaZ /= (double) 1000.0;


	pWL = pWa->pWLayer;
	pHL = pHe->pHLayer;

//	fT = pHL->fSoilTemp;
   fT = xpn->pCl->pWeather->fTempAir;

	/*average daily dew point temperature */
	fD = (pCW->fHumidity / (double)100.0)
	     * (double)6.11 * (double) exp((double)((double)17.269 * fT) / ((double)((double)237.3 + fT)));
	fD = ((double) 234.67 * (double) log10((double)fD)-(double)184.2)
	     / ((double) 8.233 - (double)log10((double)fD));

	fD = (DeltaT < (double)0.2) ? fD + fT : fD;;

	/*(DeltaT < (double)0.2) ?
	     fD + ((pCW->fTempMax - pCW->fTempMin) / (double)2) * (double)sin((double)2 * (double)PI * (DayTime-(double)0.25))
	   : fD;*/

	/* surface emissivity */
	fE = (double)0.9 + (double)0.18 * pWL->pNext->fContAct;

	/* absolute air humidity [kg/m^3] */
	fH = (double) 1.323 * (double) exp((double)17.27 *(double)(fD/((double)237.3 + fD)))
	     / (fT + (double) 273.16);

	/* Stefan-Boltzmann Constant */
	fS = (double) 5.67e-8;

	/* longwave sky irridiance */
	fR = fS * abspower((fT + (double)273.16), (double)4.0)
	     * ((double) 0.605 + (double) 1.777 * abspower(fH, (double)0.5));

	/* roughness length */
	fL = (double)0.01;

	/* aerodynamic boundary layer resistance */
	fA = abspower((double)log((double)2.0/(double)fL),(double)2.0)/((double)0.16 * pCW->fWindSpeed);
	fA = MAX(fA,EPSILON);

	/* air heat capacity */
	fC = (double) 1154.8 + (double) 303.16 /(fT+(double)273.16);

	/* soil surface matrix potential [m] */
	fP = MIN((double)0, pWL->pNext->fMatPotAct/(double)1000.0);

	/* net radiation */
	fRN = ((double)1.0 - pSo->fAlbedo) * pCW->fSolRad
	      * (double) 11.57                                   /*SolRad [MJ/d/m^2] --> [J/s/m^2] */
	      + fR - fE * fS * abspower((fTS + (double)273.16), (double)4.0);

	/* sensible heat flux */
	fSH = fC * (fTS - fT) / fA;




	/* air humidity at soil surface [kg/m^3] */
	fHS = (double) 1.323 * (double) exp((double)17.27 * (double)(fTS/((double)237.3 + fTS)))
	      / (fTS + (double) 273.16);

	fHS = fHS *(double) exp((double)fP/((double)46.97 * (double)(fTS + (double) 273.16)));

	/*latent heat flux */
	fLH = ((fHS-fH)/fA/(double)1000.0) * ((double)2.49463e9 - (double)2.247e6 * fTS);


	/* soil heat flux density at soil surface */
	fHK = (double) 0.5 * (pHL->pNext->fConduct + pHL->pNext->pNext->fConduct)
	      * (double) 1000.0 /(double) 86400.0;

	fHC = (double) 0.5 * (pHL->pNext->fCapacity + pHL->pNext->pNext->fCapacity)
	      * (double) 1000.0;

	fGS = fHK * (fTS - pHL->pNext->pNext->fSoilTemp) / DeltaZ
	      + fHC * (fTS - pHL->pNext->fSoilTemp) * DeltaZ / (DeltaT * (double)86400.0) / (double)2.0;

	/* energy balance at soil surface [J/s/m^2 = W/m^2]*/
	fBR = fRN - fSH - fLH - fGS;    /* residual of */

//   fBR = fRN-fSH-fGS;              /* residual of */

	return fBR;
}

int daisy_st_first_soil_layer(daisy *self)
{
	expertn_modul_base *xpn = &(self->parent);
	xpn->pHe->fTempSurf = xpn->pHe->pHLayer->pNext->fSoilTemp;
	return RET_SUCCESS;
}

int daisy_st_horten_from_balance(daisy *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double fA,fB,f1,f2,fRA,fRB,fRM,fM,fT;
	int id;

	fT = xpn->pHe->pHLayer->pNext->fSoilTemp;
	fA = xpn->pHe->pHLayer->pNext->fSoilTemp;
	fB = xpn->pHe->pHLayer->pNext->fSoilTemp;
	f1 = (double) 1;
	f2 = (double) 1;

	//for (id=0; (f1>=(double)0)&&(id<=25); id++)
	for (id=0; (f1>=(double)0); id++)
		{
			fA = fA - (double) 0.5;
			fB = fB + (double) 0.5;
			fRA=daisy_Get_Horton_Surface_Energy_Balance(self,fA);
			fRB=daisy_Get_Horton_Surface_Energy_Balance(self,fB);
			f1 = fRA * fRB;
		}

	if (f1 < (double) 0)
		{
			fM=(fA+fB)/(double)2.0;
			//for (id=0; ((fM-fA)>(double)0.01)&&(id<=25); id++)
			for (id=0; ((fM-fA)>(double)0.01); id++)
				{
					fRM=daisy_Get_Horton_Surface_Energy_Balance(self,fM);
					f2=fRA*fRM;
					if (f2 >(double) 0)
						{
							fA=fM;
						}
					else
						{
							fB=fM;
						}
					fM=(fA+fB)/(double)2.0;
				}

			if ((fM-fA)>(double)0.01)
				{
					fM = fT;
//     fM = pCl->pWeather->fTempAve;
				}

		}
	else
		{
			fM = fT;
//   fM = pCl->pWeather->fTempAve;
		}

	C_DEBUG(f1)
	C_DEBUG(fRA)
	C_DEBUG(fRB)
	C_DEBUG(fRM)

	xpn->pHe->fTempSurf = fM;
	return RET_SUCCESS;
}
