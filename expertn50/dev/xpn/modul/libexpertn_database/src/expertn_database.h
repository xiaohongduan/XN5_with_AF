//      expertn_database.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#ifndef XPN_WITHOUT_DATABASE

#ifndef __EXPERTN_DATABASE_H__
#define __EXPERTN_DATABASE_H__

#include <expertn_modul_base.h>
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>
#include <expertn_noah_driver.h>

G_BEGIN_DECLS

#define EXPERTN_DATABASE_TYPE				(expertn_database_get_type())
#define EXPERTN_DATABASE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			EXPERTN_DATABASE_TYPE, expertn_database))
#define EXPERTN_DATABASE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			EXPERTN_DATABASE_TYPE, expertn_databaseClass))
#define IS_EXPERTN_DATABASE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			EXPERTN_DATABASE_TYPE))
#define IS_EXPERTN_DATABASE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			EXPERTN_DATABASE_TYPE))

typedef struct _expertn_database			expertn_database;
typedef struct _expertn_databaseClass		expertn_databaseClass;


typedef struct 
{
	GDate *Datum;
	double Globalstrahlung;
	double MaximaleLufttemperatur;
	double MinimaleLufttemperatur;
	double Niederschlag;
	double Sonnenscheindauer;
	double RelativeLuftfeuchte;
	double Windgeschwindigkeit;
	double MittlereLufttemperatur;
	double Taupunkt;
	double Kesselverdunstung;
	double SaettigungsdefizitLuft;
	double Schneehoehe;
	double PhotosynthischAktiveStrahlung;	
}
struct_wetter_data;



struct _expertn_database
{
	expertn_modul_base parent;
	
	GdaConnection *cnc;
    GdaSqlParser *parser;
	
//	gboolean StandAlone; // TRUE falls ExpertN als Standaloneprogramm läuft
	gboolean MSAccess; // Kompatibilitätsmodus fuer MSAccess
	double AnzahlderTage;
	
	
	int iAutoTeilSchlag;
	double dFarmNo;
	const char *FarmName;
	const char *SchlagNo;
	const char *SchlagName;
	const char *TeilschlagNo;
	const char *TeilschlagName;
	
	// Stammteilschlag:
	const char *NameBodenProfil;
	int timesave;
	
	// test
	int durchgang;
	int firstround;
	
	// Wetter (dayly):
	double Tairmax;
	double Tairmin;
	double meantemp;
	double TairLastDayMax;
	double TairNextDayMin;
	int OldDate_day,OldDate_month,OldDate_year;
	
	double global_radiation;
	double precipitation; //[mm];
	double sunshine_duration;
	double rel_himidity;
	double windspeed;
	double dewpoint;
	double kesselverdunstung;
	double saetigungsdefizit;
	double snowdepth;
	double par;
	
	double CM,CH;
	
	double sim_time_intern;
	double start_time_intern,end_time_intern;

	
	struct_wetter_data *wetter_data;
	int wetter_data_size;
	
	int measure_time;	
	/*double start_timef,stop_timef;
	double start_time_intern,end_time_intern;*/
	
	int no_crop;
	
	double (*WCont)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*HCond)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*DWCap)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
    double (*MPotl)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
					
	double (*PedoTransfer)(double dClayF, double dSiltF, double dSandF, 
                            double dBD, double dCarbonF, double dPS, int i);
					
//	struct_WRF *WRF;
	
	double start_time;
	int no_rain;
};

struct _expertn_databaseClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType expertn_database_get_type		(void);

G_MODULE_EXPORT int  expertn_database_initialize(expertn_database *self);
G_MODULE_EXPORT int  expertn_database_run(expertn_database *self);
G_MODULE_EXPORT int  expertn_datanase_finalize(expertn_database *self);


G_MODULE_EXPORT void expertn_database_readStartValues(expertn_database *self);

G_MODULE_EXPORT void expertn_database_readObjectProject(expertn_database *self,GDate *global_start_date,GDate *global_end_date);
G_MODULE_EXPORT void expertn_database_readObjectLocation(expertn_database *self);
G_MODULE_EXPORT void expertn_database_readObjectSoilProfile(expertn_database *self);

G_MODULE_EXPORT void expertn_database_readTreatment(expertn_database *self);
G_MODULE_EXPORT void expertn_database_readPlant(expertn_database *self);

G_MODULE_EXPORT void expertn_database_readWetterTageswerte(expertn_database *self);

G_MODULE_EXPORT int expertn_database_get_iLayers(expertn_database *self, char *ProjectName);


G_MODULE_EXPORT void expertn_database_TSComparePreCropDate(expertn_database *self);

G_MODULE_EXPORT void expertn_database_runWetter_WRF(expertn_database *self);
G_MODULE_EXPORT void expertn_database_write_WRF(expertn_database *self);

		

G_END_DECLS

#endif /* __EXPERTN_DATABASE_H__ */
#endif