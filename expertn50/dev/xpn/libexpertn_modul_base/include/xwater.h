/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      Sperr Chr.
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Priesack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Simulation time objects
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: xwater.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:09
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XWATER_H
#define _INC_EXPERTN_XWATER_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct wLayer    *PWLAYER;
typedef struct wLayer
	{
	double	 fContInit; // Vol. Wassergehalt zu Beginn der Simulation
	double	 fContMax; // Vol. Wassergehalt beim Potentialwert      von -2 Pa
	double	 fContAct; // Aktueller vol. Wassergehalt
	double	 fContOld; // Vol. Wassergehalt im vorange-gangenen Simulationszeitpunkt
	double	 fMatPotAct; // mm Aktueller Potentialwert
	double	 fMatPotOld; // mm Potentialwert des vorangegangenen Simulationszeitpunkts
	double	 fContMobAct; // ?
	double	 fContMobOld; // ?
	double	 fContImmAct;// ?
	double	 fContImmOld;// ?
	double	 fOsmPot; // mm Osmotische Potential
	double	 fFluxDens; // mm / day Flußdichte in der Schicht
	double    fFluxDensOld;  // mm / day Flußdichte in der Schicht im vorigen Zeitschritt
	double	 fFlux;  // mm Fluß in der Schicht
	double	 fHydrCond;// mm/day  Hydraulische Leitfähigkeit
	double	 fSoilAirVol; // Anteil der luftgefüllten Poren
	double	 fDiffCap; // 1/mm Differenzielle Wasserkapazität
	double	 fMaxEvapFD; // mm/dm3 Maximale Evapotranspirations- Flußdichte in der aktuellen Schicht
	double    fIce;//  Vol. Eisgehalt
	double    fIceOld; //  Vol. Eisgehalt vor. Zeitschritt
	double    fWaterFreezeR; // day-1 Zunahme des vol. Eisgehalts
	double    fWaterFreezeDay; // Kumulative tägliche Zunahme des vol. Eisgehalts
	double	 fFluxDay; // mm Täglicher Fluß in der Schicht
	double	 fFluxCum; // mm Kumulierter Fluß in der Schicht
	double    fLatFlowR; // ?
	double	 fLatFlowDay; // ?
	PWLAYER	pNext;
	PWLAYER	pBack;
	}
	WLAYER;


typedef struct wMeasure   *PWMEASURE;
typedef struct wMeasure
	{ 
	long	 lDate;     
	int		 iDay;
	double	 afDepth[MAX_DEPTH_PER_MEASURE];
	double	 afContent[MAX_DEPTH_PER_MEASURE];
	double	 afFluxDens[MAX_DEPTH_PER_MEASURE];
	PWMEASURE pNext;
	PWMEASURE pBack;
	}
	WMEASURE;


typedef struct Evap   *PEVAP;
typedef struct Evap
	{ 
	double	 fCumStage1;
	double	 fCumStage2;
	double	 fTimeStage2;
	double    fPot; // [mm] Potentielle Evaporation
	double    fPotR; // [mm/day] Potentielle Evaporationsrate
	double   fPotR_old; // [mm/day] old Potentielle Evaporationsrate
	double	 fAct;// [mm] Actuelle Evaporation
	double	 fActR;// [mm/day] Aktuelle Evaporationsrate
	double	 fMax; //[mm] Maximale Evaporation
	double	 fMaxR;// [mm/day] Maximale Evaporationsrate
	double    fCropBasalIni;
	double    fCropBasalMid;
	double    fCropBasalEnd;
    double    fCropTimeIni;
    double    fCropTimeMid;
    double    fCropTimeEnd;
    double    fCropTimeDev;
	}
	EVAP;


typedef struct wBalance   *PWBALANCE;
typedef struct wBalance
	{ 
	double	 fInput;            
	double	 fOutput;           
	double	 fProfil;           
	double    fProfileIce;
	double	 fProfilStart;
	double	 fBalance;
    double    fPotETDay;
    double    fPotEVDay;
    double    fActEVDay;
	double	 fPotCumEvap;
	double	 fActCumEvap;
	double	 fPotTranspDay;
	double	 fActTranspDay;
	double	 fPotCumTransp;
	double	 fActCumTransp;
	double    fCumInterception;
	//double    fReservoir; --> nehmt mal bitte die Regenrate: entweder: pCl->pWeather->fPreciRate oder pCl->pWeather->fLiquPreciRate
	double    fSnowStorage;
	double    fSnowWaterContent;
	double	 fLatFlow;
	}
	WBALANCE;


/********************************************************************************************
 *	    SammelVariable   Water                   									        *
 ********************************************************************************************/
typedef   struct xwater * PWATER;
typedef   struct xwater
	{
    int      iTopBC;// wird vom Wassermodell gestzt
    int      iBotBC;// wird vom Wassermodell gesetzt
	int 	 FreeD; // Free Drainage 1 oder 0 --> muss im Wassermodell gesetzt werden (standart: 1)
	int		 iMobImm; // wird vom Wassermodell gesetzt
	double	 fRunOffR;
	double	 fLeaching; 
	double	 fCumLeaching;
	double	 fPotInfiltration;
	double    fInfiltR;
	double    fInfiltDay;
	double	 fCumInfiltration;
	double	 fPondWater;
	double	 fPercolR;
	double	 fPotETDay;// Diese Variablen sollten, wenn dann lokal in der Bilanz oder zur Ausgabe erzeugt werden
//	double	 fActETDay;
//	double	 fPotETCum;
//	double	 fActETCum;
	double   fPotETR; // [mm]/[day] Potentielle Evaporationsrate
	double	 fPotETdt;// [mm] Potentielle Evaporation
	double	 fActETdt;
/*new*/	double   fActETR;  // [mm]/[day] Actual Evapotranspiration
	double    fRunOffCum;
	double	 fRunOffDay;
	double	 fActTranspDay;
	double	 fActTranspCum;
	double	 fLatFlowCum;
	double    fGrdWatLevel;
	double    fGrdWatLvlPot;
	double    kc_factor; // kc factor for pm: 1 without model
	PWLAYER	   pWLayer;
	PWMEASURE  pWMeasure;
	PEVAP	   pEvap;
	PWBALANCE  pWBalance;
 	}WATER;


#endif // #ifndef _INC_EXPERTN_XWATER_H

/*******************************************************************************
** EOF */
