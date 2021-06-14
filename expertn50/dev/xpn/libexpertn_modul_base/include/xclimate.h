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
 * $History: xclimate.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:07
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 01.12.96
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XCLIMATE_H
#define _INC_EXPERTN_XCLIMATE_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct weather   * PWEATHER;
typedef struct weather  
    {
    //long   lDate;  // DDMMYY Termin der Wetterdaten (==aktuelles Datum? )
    //float  fNumDay; // day Anzahl der Tage seit Simulationsbeginn
/*?*/   double   fSolRad; // MJ / m2 / day Globalstrahlung = Globalstrahlung (etwa 2*fPAR)
    //float  fSunDuration; // Sonnenscheindauer vlt. Werte vom Vortag?
    double   fPAR;     // MJ / m2 / day Photosynthetisch aktive Strahlung = SOLDN
/*neu*/ double   fSolNet;  // MJ / m2 / day netto shortwave flux 
/*neu*/ double   fSolarDirect; // MJ / m2 direct component of downward solar radiation
/*neu*/ double   cosz; //  Cosine of the Solar Zenith Angle
/*  float    fTempAve;
    float    fTempMax;      // Hoechnstens Temps vom Vortag
    float    fTempMin;*/
	//Begin of Hong, for GECROS
	double fTempMax;
	double fTempMin;
	double fTempAve;
	double fGlobalStrahlung;
	//End of hong
/*neu*/ double   fTempAir;      // Grad Celsius Lufttemperatur bei der Hoehe ZLVL m = SFCTMP
    double fTempAir_zlvl;// Grad Cellsius Lufttemperatur bei der Hoehe ZLVL m = SFCTMP
    double   fHumidity; // % --> wrf&RH wahrscheinlich
/*?*/   double   fPanEvap;  // mm/day Kesselverdunstung
    double   fWindSpeed; // m/s SFCSPD Windspeed at speed at height ZLVL m AGL
	double   fWindSpeedConv; // m/s convective wind speed
    double   fDewPoint;  // Grad Celsius Taupunkt --> Wikipedia/Taupunkt Formel 2.3 aus rel. Luftfeuchtikeit und Temperatur
    double   fSatDef;  // % Sättigungsdefizit der Luft  -> RCQ * 100% ???
/*  float    fSoilTemp5;
    float    fSoilTemp10;  ist dies nicht im SOIL struct?, falls nicht gehört es auf jeden Fall nicht hierher
    float    fSoilTemp20;*/ 
/*  float    fRainBegin;
    float    fRainEnd;*/     //  machen auch meiner Meinung nach auch keinen Sinn mehr, sattdessen 
/*new*/ double   fPreciRate; // kg / (m^2) precipitation rate --> PRCP
/*new*/ double   fLiquPreciRate;  // kg / (m^2) liquid precipitation rate --> PRCPRAIN
    double fWaterPond; // mm / m2 Regenwasser Storage
    
//  float    fDayFrac;
//  double   fRainRate; // mm/day   Rate in der Niederschlag erfolgt  --> falls gebraucht, kann aus fLiquPreciRate ermittelt werden
//  float    fRainAmount;// mm Niederschlagsmenge
    double   fSnow; // mm Schneeauflage  -> SNOWH
/*new*/ double   fSnowEqu;//mm Water equivalent snow depth --> SNEQV
/*new*/ double	 fBulkPreciRate;	// Freilandniederschlag = Bestandesniederschlag + Interzeptionsverlust
/*interception*/    double   fCanopyDrain; // keine Doku!!!!  = 0.0
/*interception*/    double   fThroughfall;
/*interception*/    double   fStemflow;
/*interception*/    double   fCanopyStorage;

/*?*/   double   fWaterTable;  // keine Doku!!!!  = 0.0
    double   fUreaNRain;  // mg /dm3 Harnstoff N im Niederschlag 
    double   fNH4NRain;   // mg /dm3 NH4 N im Niederschlag       
    double   fNO3NRain;   // mg /dm3 NH3 N im Niederschlag       
    double   fChemRain;   // mg /dm3 chemischer Stoffim Niederschlag --> keine Ahnung, ob sowas in WRF drin ist --> = 0
    
/*new*/ double   fAtmPressure;  // Pa Atmospheric pressure at height ZLVL m --> SFCPRS
/*new*/ double   fAtmCO2ppm;    // atmospheric CO2 concentration [ppm]
/*new*/ double   fAtmO2ppm;     // atmospheric CO2 concentration [ppm]

// Wettervariablen um einen Tag verschoben für die Tagesmodelle CERES, ET_FAO und ET_Haude
/*new*/ double	fTempAir_daily_models;
/*new*/ double	fHumidity_daily_models;
/*new*/ double	fWindSpeed_daily_models;
/*new*/ double	fSolRad_daily_models;
/*new*/ double	fPreciRate_daily_models;
/*new*/ double	fAtmPressure_daily_models;	

    PWEATHER       pNext; // = NULL --> nur aktuelle Wetterdaten vorhanden (ich schätze morgen ist das Wetter genauso wie heute)
    PWEATHER       pBack; // = NULL --> nur aktuelle Wetterdaten vorhanden (vllt irgendwann mal, falls Zeit ist)
    }
    WEATHER;




typedef struct average   *  PAVERAGE;
typedef struct average 
    {
    double   afMonthTemp[12];
    double   fYearTemp;
    double   fMonthTempAmp;
    double   fYearSunDuration;
    double   fYearSolRad;
    double   fYearPAR;
    double   fYearRain;
    double   fYearSnow;
    double   fYearHumidity;
    double   fYearPanEvap;
    double   fYearSatDef;
    }   
    AVERAGE;

/**********************************************************************************************
*           SammelVariable   Climate                                              *                                              *
***********************************************************************************************/

typedef struct climate    * PCLIMATE;
typedef struct climate
    {
/*?*/   double   fCumRain;     //mm  Kummulative Niederschlagsmenge heisst das ich addiere alle Niederschläge von SimBeginn an auf (wozu?)
/*?*/   double   afTempLast5Days[5]; // Grad Celsius Tagesdurchschnitttemp der letzten fünf Tage (wie wird das initialisiert?)
/*?*/   double   fCumTempL5Days;  // Grad Celsius gemittelte Temperatur über fünf Tage (INIT?)
    double   fTempMeasHeight; // m  Höhe des Temperaturfühlers -->  ZLVL
    double   fWindMeasHeight; // m  Höhe der Windmessung --> ZLVL

    PWEATHER    pWeather;
    PAVERAGE    pAverage;
    }
    CLIMATE;

#endif // #ifndef _INC_EXPERTN_XCLIMATE_H

/*******************************************************************************
** EOF */
