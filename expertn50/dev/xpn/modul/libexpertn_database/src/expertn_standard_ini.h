//      expertn_standard_ini.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __EXPERTN_STANDARD_INI_H__
#define __EXPERTN_STANDARD_INI_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define EXPERTN_STANDARD_INI_TYPE				(expertn_standard_ini_get_type())
#define EXPERTN_STANDARD_INI(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			EXPERTN_STANDARD_INI_TYPE, expertn_standard_ini))
#define EXPERTN_STANDARD_INI_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			EXPERTN_STANDARD_INI_TYPE, expertn_standard_iniClass))
#define IS_EXPERTN_STANDARD_INI(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			EXPERTN_STANDARD_INI_TYPE))
#define IS_EXPERTN_STANDARD_INI_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			EXPERTN_STANDARD_INI_TYPE))

typedef struct _expertn_standard_ini			expertn_standard_ini;
typedef struct _expertn_standard_iniClass		expertn_standard_iniClass;

typedef struct
{	
	// simulation:
	GDate *start_date,*end_date;
	// location:
	double lat,lon,alt; // Prozent, hoehe in m
	char* exposition;
	double inclination; // Hang Neigung[%]
	double size; // [ha]
	double wind_measure_height;
	double temp_measure_height;
	
	//climate:
	double AveYearTemp;
	double MonthTempAmp;
	
	//preceding_crop //Added by Hong on 20190930
	double aboveResidualC;
	double aboveResidualN;
    //End of Hong
	// soil
	int layer_count;
	int layer_thickness;
	int *layers;
	int layers_len;
	double *clay;
	int clay_len;
	double *silt;
	int silt_len;
	double *sand;
	int sand_len;
	double *organic_matter;
	int organic_matter_len;
	double *bulk_desity;
	int bulk_density_len;
	double *rock_fraction;
	int rock_fraction_len;
	double *ph;
	int ph_len;
	
	// optional:
	double *organic_carbon;
	int	   organic_carbon_len;	
	double *organic_nitrogen;
	int 	organic_nitrogen_len;	
	double *porosity;
	int		porosity_len;	
	double *cont_sat;
	int		cont_sat_len;	
	double *wilting_point;
	int		wilting_point_len;	
	double *field_capacity;
	int		field_capacity_len;
	double *res_water_cont;
	int 	res_water_cont_len;
	double *camp_a;
	int 	camp_a_len;
	double *camp_b;
	int		camp_b_len;
	double *van_gen_a;
	int		van_gen_a_len;
	double *van_gen_n;
	int		van_gen_n_len;
	double *max_pot;
	int		max_pot_len;
	double *cond_sat;
	int		cond_sat_len;
	double *cont_inflec;
	int 	cont_inflec_len;
	double *pot_inflec_hPa;
	int		pot_inflec_hPa_len;
	char  **soil_type;
	int		soil_type_len;
	
	// start values;
	int *start_layers;
	int start_layers_len;
	double *water_content;
	int water_content_len;
	double *matrix_potential;
	int matrix_potential_len;
	double *soil_temperature;
	int soil_temperature_len;
	double *nh4_content;
	int nh4_content_len;
	double *no3_content;
	int no3_content_len;
	double *root_density;
	int root_density_len;
	
} STRUCT_CFG_FILE;


struct _expertn_standard_ini
{
	expertn_modul_base parent;
	/* add your public declarations here */
	char *cfg_filename;
	char *weather_file_name;
	char *weather_file_name_st;
	
	STRUCT_XNO_DATA *climate_values;
	STRUCT_XNO_DATA *climate_values_small_ts;
	
	int use_high_resolution_climate_data; // 1 or 0
	int interpolate_climate_data; // 1 or 0
	
	int number_of_days;

	int firstround;
	double timesave;
	int OldDate_day,OldDate_month,OldDate_year;
	double TairLastDayMax,TairNextDayMin;
	double global_radiation;	
	double precipitation;
	double sunshine_duration;
	double rel_himidity;
	double windspeed;
	double dewpoint;
	double kesselverdunstung;
	double saetigungsdefizit;
	double snowdepth;
	double par;
	double pressure;
	double Tair;
	double Tairmax;
	double Tairmin;
	double meantemp;
    
    //double SoilT02;
    //double SoilT05;
    //double SoilT10;
    //double SoilT20;
    //double SoilT50;
    double AtmCO2ppm; //SG20190211
    //double CnpyDrn;
    //double WatTbl;


	// run weather small time step:
	int weather_index;
	
	int measure_time;
    double start_time_intern;
    double end_time_intern;
    double sim_time_intern;
	
	int no_rain;
	
	//Added by Hong on 20181212
	// Hydraulic Functions:
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
	
	//End of Hong
	
	STRUCT_CFG_FILE *cfg;
};


// public class member function:
G_MODULE_EXPORT int expertn_standard_ini_load(expertn_standard_ini *self);
G_MODULE_EXPORT int expertn_standard_ini_run(expertn_standard_ini *self);
G_MODULE_EXPORT int expertn_standard_ini_done(expertn_standard_ini *self);


int expertn_standard_ini_load_config(expertn_standard_ini *self,GDate *global_start_date,GDate *global_end_date);
int expertn_standard_ini_load_weather_file(expertn_standard_ini *self);
int expertn_standard_ini_load_climate_ts_file(expertn_standard_ini *self);
int expertn_standard_ini_free_weather_file(expertn_standard_ini *self);
int expertn_standard_ini_free_climate_ts_file(expertn_standard_ini *self);
void expertn_standard_ini_free_cfg_file(expertn_standard_ini *self);

void expertn_standard_ini_set_location(expertn_standard_ini *self);
void expertn_standard_ini_set_climate_Ave(expertn_standard_ini *self);
void expertn_standard_ini_set_preceding_crop(expertn_standard_ini *self); //Added by Hong on 20190930
void expertn_standard_ini_set_soil(expertn_standard_ini *self);
void expertn_standard_ini_set_start_values(expertn_standard_ini *self);
void expertn_standard_ini_runWetterTageswerte(expertn_standard_ini *self);
void expertn_standard_ini_run_climate_high_res(expertn_standard_ini *self);
int expertn_standard_set_up_soil_temp(expertn_standard_ini *self);

struct _expertn_standard_iniClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType expertn_standard_ini_get_type (void);

G_END_DECLS

#endif /* __EXPERTN_STANDARD_INI_H__ */

