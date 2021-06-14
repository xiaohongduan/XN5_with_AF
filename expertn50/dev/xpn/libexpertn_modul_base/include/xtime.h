/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      C.Sperr
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Prieack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Include file for Expert-N library development.
 *   Simulation time objects.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: xtime.h $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:33
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Avoid compile error "Simtime already defined".
 * 
 *   27.11.95
 *   letzte Aenderung 2010 (geändert für WRF von Christian Klein (chrikle@berlios.de)
 *  Der Datentyp long wird jetzt ausnahmslos durch int ersetzt
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XTIME_H
#define _INC_EXPERTN_XTIME_H


typedef struct
{

// Daten werden während der Simulation geändert:

  //double  fTime; //         day       Aktuelle Zeit (UNIX) seit 1900 (für Datenbank)
  int     iyear; // year YYYY z.B. 1981
  double  fTimeY; // Days from Jan.
  
  // derived vars:
  
  int  is_leap_year; //  FLAG: 1 or 0
  double msec;	
  int sec;		
  int min;		
  int hour;		
  int mday;		
  int mon;		
  int year;		
  
  double  fTimeDay;  // 		day	[0..1]	Vergangene Zeit eines Tages
  
  int     iyear_old; // year YYYY z.B. 1981
  double  fTimeY_old ; // Days from Jan.
  

// Diese Daten stehen nach der Init zur Verfügung:
	
  double fTimeZone;	// Time Zone: 0 = GMT, +1 = Germany
  double  fSimDuration; //	day		Simulationsdauer in Tagen, wozu wird die denn gebraucht?
  int   iStart_year;     // Datum des Simulationsstartes
  double fStart_TimeY;
  int   iStop_year;      // Datum des Simulationsendes
  double fStop_TimeY;

// other FLags:

  gboolean bLastRound; //  TRUE (falls letzte Simulationsrunde)
  gboolean bFirstRound;//  TRUE (falls erste Simulationsrunde)

} SIMTIME, *PSIMTIME;

typedef struct
{
  // Daten werden während (vllt) der Simulation geändert:
	
  double  fAct;			// day		Aktueller Zeitschritt der Simulation (dt)
  double  fOld;			// day 		Letzter Zeitschritt der Simulation (dt)

 // Diese Daten stehen nach der Init zur Verfügung:	
  double  fMax;			// day		Maximaler Zeitschritt
  double  fOpt;			// day		Optimaler Zeitschritt
  double  fMin;			// day		Minimale Dauer eines Zeitschritts
  double  fWaterOpt;		// day		Optimale Dauer eines Zeitschritts fürs Wassermodell
  double  fChemOpt;		// day		Optimale Dauer eines Zeitschritts für den Stofftransport
  double  fWaterDist;	// ?		Abstandsnorm für Wassertransport	
  double  fChemDist;		// ?		Abstandsnorm für Stofftransport
} TIMESTEP, *PTIMESTEP;

typedef struct
{
  PSIMTIME   pSimTime;
  PTIMESTEP  pTimeStep;
} TIME, *PTIME;


#endif // #ifndef _INC_EXPERTN_XTIME_H

/******************************************************************************
** EOF */
