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
 * $History: xheat.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:07
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XHEAT_H
#define _INC_EXPERTN_XHEAT_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct hlayer   *PHLAYER;
typedef struct hlayer
	{
	double	 fSoilTemp; // Grad Celsius, Bodentemperatur
	double   fSoilTempOld; // Grad Celsius, Bodentemperatur im vorherigen Zeitschritt
	double   fHeat;	// J m-2, Waerme 
	double	 fHeatflow;// J m s-1 Bodenwaermefluss
	double	 fHeatSens;// J m-2 sensible Waerme
	double	 fHeatLatent; // J m-3 latente Waerme
	double	 fMeltPoint; // Grad Celsius, Schmelzpunkt der Bodenloesung
	double   fConduct; // W/mK Waermeleitfaehigkeit
	double   fConductOld; // W/mK Waermeleitfaehigkeit vorheriger Zeitschritt
	double   fCapacity; // kJ/m3K Vol-Waermekapazitaet des Bodens
	double   fCapacityOld; // kJ/m3K Vol-Waermekapazitaet des Bodens vorheriger Zeitschritt
//	double	 fSoilTempAve; // Grad Celsius durchschnittliche Tagestemp
	PHLAYER	pNext;
	PHLAYER	pBack;
	}
	HLAYER;


typedef struct hbalance   *PHBALANCE;
typedef struct hbalance
	{
	double	 fInput;
	double	 fOutput;
	double	 fProfil;
	double	 fProfilStart;
	double	 fBalance;
	}
	HBALANCE;


typedef struct hmeasure   *PHMEASURE;
typedef struct hmeasure
	{
	int	 lDate;
	int	 iDay;	
	double	 afDepth[MAX_DEPTH_PER_MEASURE];
	double	 afTemp[MAX_DEPTH_PER_MEASURE];
	PHMEASURE	pNext;
	PHMEASURE	pBack;
	}
	HMEASURE;
	
// Energy Balance
typedef struct hebalance   *PHEBALANCE;
typedef struct hebalance
	{
	double	 fHeatSens;// W m-2 day-1 sensible Waerme (An der Oberflaeche)
	double	 fHeatLatent; // W m-2 day-1 latente Waerme (An der Oberflaeche)
	double fGroundHeat; // // W m-2 day-1 ground heat flux	
	double fShortNetRad; // W m-2 day-1 Short Wave Net Radiation
	double fLongNetRad; // W m-2 day-1 Long Wave Net Radiation
	double fNetRad; // W m-2 day-1 Net Radiation fShortNetRad+fLongNetRad
	double fRes; // W m-2 day-1 Residuum of the Energy Ballance
	}
	HEBALANCE;

/****************************************************************************************
*                      SammelVariable  HEAT						*
*****************************************************************************************/

typedef struct heat   * PHEAT;
typedef struct heat
	{ 
	double   fFreezingDepth;
	double fEmissi; // Surface emissivity (between 0 and 1)
	double fTempSurf; // Surface Temperature
	PHLAYER		pHLayer;
	PHBALANCE	pHBalance;
	PHMEASURE	pHMeasure;
	PHEBALANCE pHEBalance;
	}
	HEAT;

#endif // #ifndef _INC_EXPERTN_XHEAT_H

/*******************************************************************************
** EOF */
