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
 * $History: xsoil.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:09
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 19.1.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XSOIL_H
#define _INC_EXPERTN_XSOIL_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct sLayer    *PSLAYER;
typedef struct sLayer  
	{
	char	*acSoilID; // Bodenart nach Bodenkdl. Kartier-anleitung
	double	fThickness; //mm Dicke der Bodenschicht
	double	fClay; // % Ton - Anteil
	double	fSand;// % Sand - Anteil
	double	fSilt;// % Schluff - Anteil
	double	fCHumus; // kg/ha Kohlenstoffanteil im Humus
	double	fHumus;// kg/ha Menge an Humus
	double	fNHumus; // kg/ha Stickstoffanteil im Humus
	double	fHumusCN; // ?  scheint immer 10.0 zu sein (warum auch immer)
	double	afPartVolFrac[4];//  rel. Volumenanteile (Quarz, Ton-Mineralien, Organische Substanz, Luft )
	double	fpH; // ph Wert
	double	fBulkDens; // kg /dm3 Lagerungsdichte der Bodenschicht
	double	fBulkDensStart; // kg /dm3 Lagerungsdichte der Bodenschicht vor Beginn der Simulation
	double	fPorosity; // Porosität
	double	fCompress; // Faktor für die natürliche Rückverfest- igung einer Bodenschicht nach einer Lockerung
	double	fRockFrac;// kg/kg Gravimetrischer Steingehalt
	double   fImpedLiqA; // Konstante zur Berechnung des Tortuositätsfaktors
	double   fImpedLiqB; // Konstante zur Berechnung des Tortuositätsfaktors
	double   fImpedGasA;// Konstante zur Berechnung des Tortuositätsfaktors
	double   fImpedGasB; // Konstante zur Berechnung des Tortuositätsfaktors
	PSLAYER   pNext;
	PSLAYER   pBack;
	}
	SLAYER;

typedef struct sHeat    *PSHEAT;  // Wird diese Structur überhaupt mal irgendwo in Expert N gesetzt, abgesehen der ersten drei vars, wird der Rest nicht benutzt
typedef struct sHeat  
	{
	double	afPartDens[3]; // kg/l spez. Dichte (Ton, Sand, org. Substanz)
	double	afPartGeom[3][3]; // Geometriefaktoren
	double	afPartHeatCond[6]; // kJ/( m K day) spezifische Waermeleitfaehigkeit (Quarz, Mineral, org. Substanz, Wasser, Luft, Eis)
	double	fCapAir; // J / kg / Grad C spez. Waermekapazitaet der Bodenluft
	double	fCapWater; // J / kg / Grad C spez. Waermekapazitaet des Bodenwassers
	double	fCapIce; // J / kg / Grad C spez. Waermekapazitaet des Eis
	double	fCapOrg; // J / kg / Grad C spez. Waermekapazitaet der org. festen Bodensubstanz
	double	fCapAnOrg; // J / kg / Grad C spez. Waermekapazitaet der anorg. festen Bodensubstanz
	double	fCondAir; // (J * m2) / (Grad Celsius * s) spezifische Waermeleitfaehigkeit der Bodenluft
	double	fCondWater; // (J * m2) / (Grad Celsius * s) spezifische Waermeleitfaehigkeit des Bodenwassser
	double	fCondIce; // (J * m2) / (Grad Celsius * s) spezifische Waermeleitfaehigkeit des Eis
	double	fCondOrg; // (J * m2) / (Grad Celsius * s) spezifische Waermeleitfaehigkeit der org. Bodensubsanz
	double	fCondAnOrg; // (J * m2) / (Grad Celsius * s) spezifische Waermeleitfaehigkeit anorg. Bodensubsanz
	PSHEAT   pNext;
	PSHEAT 	 pBack;
	}
	SHEAT;

typedef struct swater    *PSWATER;
typedef struct swater
	{
	double	fContSat; // Vol. Wassergehalt bei Sättigung
	double	fContPWP; // Vol. Wassergehalt bei Permanentem Welkepunkt
	double	fContFK; // Vol. Wassergehalt bei Feldkapazität
	double	fContSatCER; // Vol. Wassergehalt bei Sättigung für Modell CERES
	double   fContRes; // Vol. Restwassergehalt
	double   fContMobSat; // ?
	double   fContImmSat; // ?
	double   fMobImmEC; // ?
	double	fCampA; // mm Campbel A Konstante
	double	fCampB; // 1 Campbel B Konstante
	double	fVanGenA;  // 1/mm Van Genuchten Alpha Konstante
	double	fVanGenA2;//  1/mm Van Genuchten Alpha Konstante
	double	fVanGenM; // Van Genuchten m Konstante
	double	fVanGenM2; // Van Genuchten m Konstante
	double	fVanGenN; // Van Genuchten m Konstante
	double	fVanGenN2; // Van Genuchten m Konstante
	double   fMualemP; // ?
	double   fMualemQ; // ?
	double   fMualemR; // ?
	double	fMinPot; // mm Betragsmäßig kleinst mögliches Potential (=< 0)
	double	fCondSat; // mm/day  Wasserleitfähigkeit bei Wasser-sättigung
	double	fContInflec; // Vol % Wassergehalt am Wendepunkt parabolisch / exponentieller Kennlinie hydraulischer Funktionen
	double	fPotInflec; // mm Potential am Wendepunkt parabolisch / exponentieller Kennlinie hydraulischer Funktionen
	double   fBiModWeight1; // Anteil des ersten Porenraumes
	double   fBiModWeight2;// Anteil des zweiten Porenraumes
	PSWATER   pNext;
	PSWATER   pBack;
	}
	SWATER;

/**********************************************************************************************
*		SammelVariable Boden										     *
**********************************************************************************************/

typedef struct sProfile   *PSPROFILE;
typedef struct sProfile
  	{
	int	iLayers;  // Anzahl der Bodenschichten
	double	fDepth; // mm, Profiltiefe
	double	fDeltaZ;  // mm, Konstante Mächtigkeit der Profilschichten
	double	fPotPWP; // mm, Wasserpotential beim permanten Welkepunkt  -------------------------------- wird nicht benutzt
	double	fSoilAlbedo; // Albedo (nicht absorbierte Strahlung) des Bodens
	double	fAlbedo;// [0..1] Aktuelle Gesamt Albedo (plant, soil) anteilsmaeßig
	double   fSoilCover; //  ?                                                                                    -------------------------------- wird nicht benutzt
	double	fCurvNumNorm; // SCS-Kurvennummer bei normal feuchtem Boden von 1 bis 99
	double	fCurvNumDry; // SCS-Kurvennummer bei trockenem Boden von 1 bis 99  -------------------------------- wird nicht benutzt
	double	fCurvNumWet; // SCS-Kurvennummer bei feuchtem Boden von 1 bis 99  -------------------------------- wird nicht benutzt
	double	fCurvNumAct; // Aktuelle SCS-Kurvennummer von 1 bis 99  -------------------------------- wird nicht benutzt
	PSLAYER	pSLayer;
	PSHEAT	pSHeat;
	PSWATER	pSWater;
	}
	SPROFILE;

#endif // #ifndef _INC_EXPERTN_XSOIL_H

/*******************************************************************************
** EOF */
