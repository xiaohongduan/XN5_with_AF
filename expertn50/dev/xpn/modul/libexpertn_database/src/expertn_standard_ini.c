//      expertn_standard_ini.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//
#include "expertn_standard_ini.h"
#include "expertn_standard_ini_macros.h"
#include "database_help_func.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef USE_OPENMP
#include <omp.h>
#endif
G_DEFINE_TYPE(expertn_standard_ini, expertn_standard_ini, EXPERTN_MODUL_BASE_TYPE);
static void expertn_standard_ini_class_init(expertn_standard_iniClass *klass) {}
static void expertn_standard_ini_init(expertn_standard_ini *self)
{
	self->cfg=NULL;
	self->cfg_filename = NULL;
	self->weather_file_name=NULL;
	self->weather_file_name_st=NULL;
	self->climate_values=NULL;
	self->climate_values_small_ts=NULL;
	self->interpolate_climate_data=0;
	self->use_high_resolution_climate_data=0;
	self->no_rain = 0;
}
// Our Modul Functions:
int expertn_standard_ini_load(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *S;
	int RET;    
	char *filename;
	GDate *global_start_date,*global_end_date;   

	
	self->measure_time=0;
	self->start_time_intern=0.0;
	self->end_time_intern=0.0;
	self->sim_time_intern=0.0;
	self->no_rain = 0.0;
    
	if (xpn->pXSys->id==0)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Database.measure time");
			if (S==NULL)
				{
					S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.measure time");
					if (S==NULL)
						{
							S = "0";
						}
				}
			self->measure_time = atoi(S);
			if (self->measure_time==1)
				{
#ifdef USE_OPENMP
					self->start_time_intern = omp_get_wtime();
#else
					self->start_time_intern = (double)clock()/(double)CLOCKS_PER_SEC;
#endif
				}
		}
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.expertn_database.no rain");
	if (S==NULL)
		{
			self->no_rain = 0;
		}
	else
		{
			self->no_rain = atoi (S);
		}
	self->cfg=NULL;
	self->cfg_filename = NULL;
	self->weather_file_name=NULL;
	self->weather_file_name_st=NULL;
	self->climate_values=NULL;
	self->climate_values_small_ts = NULL;
	RET = RET_SUCCESS;
	expertn_database_help_func_set_global_start_and_end_date(xpn,&(global_start_date),&(global_end_date));
	filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.filename");
	if (filename==NULL)
		{
			PRINT_ERROR("Missing entry 'ConfigExpert N Standard Read INI.filename' in your configuration!");
			return 1;
		}
	self->cfg_filename=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,filename);
	RET = expertn_standard_ini_load_config(self,global_start_date,global_end_date);
	// Wetterdaten aus Datenbank lesen falls nötig:
	if (!xpn->pXSys->extern_time_management)
		{
			self->use_high_resolution_climate_data=0;
			self->interpolate_climate_data=0;
			S = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.use high resolution climate data");
			if ((S==NULL) || (atoi(S)==0))
				{
					self->use_high_resolution_climate_data=0;
				}
			else
				{
					self->use_high_resolution_climate_data=1;
					S = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.interpolate climate data");
					if ((S==NULL) || (atoi(S)==1))
						{
							self->interpolate_climate_data=1;
						}
					else
						{
							self->interpolate_climate_data=0;
						}
				}
			if (self->use_high_resolution_climate_data==0)
				{
					filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.climate file");
					if (filename==NULL)
						{
							PRINT_ERROR("Missing entry 'ConfigExpert N Standard Read INI.climate file' in your configuration!");
							return 1;
						}
					self->weather_file_name=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,filename);
					RET += expertn_standard_ini_load_weather_file(self);
				}
			else
				{
					filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Read INI.high res climate file");
					if (filename==NULL)
						{
							PRINT_ERROR("Missing entry 'ConfigExpert N Standard Read INI.high res climate file' in your configuration!");
							return 1;
						}
					self->weather_file_name_st =expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,filename);
					RET += expertn_standard_ini_load_climate_ts_file(self);
				}
		}
	if (RET==RET_SUCCESS)
		{
			expertn_database_help_func_reservere_Memory(xpn,self->cfg->layer_count+2);
			expertn_database_help_func_set_time_step(xpn,"Config.Expert N Standard Read INI.time step");
			xpn->pTi->pSimTime->fTimeZone = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Expert N Standard Read INI.time zone",0.0);
			expertn_standard_ini_set_location(self);
			expertn_standard_ini_set_climate_Ave(self);
			expertn_standard_ini_set_preceding_crop(self); //Added by Hong on 20190930
			expertn_standard_ini_set_soil(self);
			// TODO: expertn_database_readTreatment(self);
			// TODO: expertn_database_readPlant(self);
			expertn_standard_ini_set_start_values(self);
			expertn_database_help_func_init_water(xpn);
			expertn_database_help_func_init_Chemistry(xpn);
		}
	expertn_standard_ini_free_cfg_file(self);
	G_FREE_DATE_IF_NOT_0(global_start_date);
	G_FREE_DATE_IF_NOT_0(global_end_date);
	self->firstround=0;
	self->timesave=0.0;
	self->OldDate_day = 0;
	self->OldDate_month = 0;
	self->OldDate_year = 0;
	self->TairLastDayMax=0.0;
	self->TairNextDayMin=0.0;
	self->global_radiation=0.0;
	self->precipitation=0.0;
	self->sunshine_duration=0.0;
	self->rel_himidity=0.0;
	self->windspeed=0.0;
	self->dewpoint=0.0;
	self->kesselverdunstung=0.0;
	self->saetigungsdefizit=0.0;
	self->snowdepth=0.0;
	self->par=0.0;
	self->Tairmax=0.0;
	self->Tairmin=0.0;
	self->meantemp=0.0;
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.MeanTemp",&(self->meantemp),-1,TRUE);
	self->weather_index=0;
    
    
    
    
	return RET;
}

int expertn_standard_set_up_soil_temp(expertn_standard_ini *self)
{
    expertn_modul_base *xpn = &(self->parent);	
    int iLayer;	
    PSLAYER pSL;
	PSWATER pSW;
	PWLAYER pWL;
	PHLAYER pHL;
    double lower_temp;
    double upper_temp;


// setze bodentemperatur:
    lower_temp = xpn->pCl->pAverage->fMonthTempAmp
	             + xpn->pCl->pAverage->fYearTemp	             
	             * cos((2.0 * PI / 365.0)*(xpn->pTi->pSimTime->fTimeY - 190.0));
    upper_temp = xpn->pCl->pWeather->fTempAir;
    
    for (pSL = xpn->pSo->pSLayer->pNext,
							pSW = xpn->pSo->pSWater->pNext,
							pWL = xpn->pWa->pWLayer->pNext,
							pHL = xpn->pHe->pHLayer->pNext,
							iLayer = 1; ((pSL->pNext != NULL)&&
									(pSW->pNext != NULL)&&
									(pWL->pNext != NULL)&&
									(pHL->pNext != NULL)); pSL = pSL->pNext,
									pSW = pSW->pNext,
									pWL = pWL->pNext,
									pHL = pHL->pNext,
									iLayer++)
							{					
                                if (pHL->fSoilTemp<= -98.0)
                                    {
									pHL->fSoilTemp = (((double)iLayer-1.0)/(double)(xpn->pSo->iLayers-2))*(lower_temp-upper_temp)+upper_temp;
                                    pHL->fSoilTempOld = pHL->fSoilTemp;
                                    }                                
							}
return RET_SUCCESS;
}

int expertn_standard_ini_run(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PWLAYER pWLayer;
	char *S;
	STRUCT_XPN_TIME xpn_time;
	int iyear;
	double fTimeY;
	xpn->pWa->fActETR = xpn->pPl->pPltWater->fActTranspR + xpn->pWa->pEvap->fActR + xpn->pPl->pPltWater->fInterceptR;
	// Wetterdaten aus Datenbank lesen falls nötig:
	if (!xpn->pXSys->extern_time_management)
		{
			double intpart;
			if (self->firstround==1)
				{
					xpn->pTi->pTimeStep->fOld = xpn->pTi->pTimeStep->fAct;
					xpn->pTi->pSimTime->iyear_old = xpn->pTi->pSimTime->iyear;
					xpn->pTi->pSimTime->fTimeY_old = xpn->pTi->pSimTime->fTimeY;
					xpn_time_add_dt(&(xpn->pTi->pSimTime->iyear),&(xpn->pTi->pSimTime->fTimeY),xpn->pTi->pTimeStep->fAct);
					xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
					xpn->pTi->pSimTime->is_leap_year = xpn_time.is_leap_year; //  FLAG: 1 or 0
					xpn->pTi->pSimTime->msec = xpn_time.msec;
					xpn->pTi->pSimTime->sec = xpn_time.sec;
					xpn->pTi->pSimTime->min = xpn_time.min;
					xpn->pTi->pSimTime->hour = xpn_time.hour;
					xpn->pTi->pSimTime->mday = xpn_time.mday;
					xpn->pTi->pSimTime->mon = xpn_time.mon;
					xpn->pTi->pSimTime->year = xpn_time.year;
					xpn->pTi->pSimTime->fTimeDay = modf(xpn->pTi->pSimTime->fTimeY,&intpart);
					//expertn_database_init_water(self);
				}
			else
				{
					xpn->pTi->pSimTime->iyear_old = xpn->pTi->pSimTime->iyear;
					xpn->pTi->pSimTime->fTimeY_old = xpn->pTi->pSimTime->fTimeY;
					xpn->pTi->pTimeStep->fOld = xpn->pTi->pTimeStep->fAct;
					xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
					xpn->pTi->pSimTime->is_leap_year = xpn_time.is_leap_year; //  FLAG: 1 or 0
					xpn->pTi->pSimTime->msec = xpn_time.msec;
					xpn->pTi->pSimTime->sec = xpn_time.sec;
					xpn->pTi->pSimTime->min = xpn_time.min;
					xpn->pTi->pSimTime->hour = xpn_time.hour;
					xpn->pTi->pSimTime->mday = xpn_time.mday;
					xpn->pTi->pSimTime->mon = xpn_time.mon;
					xpn->pTi->pSimTime->year = xpn_time.year;
					xpn->pTi->pSimTime->fTimeDay = modf(xpn->pTi->pSimTime->fTimeY,&intpart);
					//expertn_database_init_water(self);
				}
			if (self->use_high_resolution_climate_data==0)
				{
					expertn_standard_ini_runWetterTageswerte(self);
				}
			else
				{
					expertn_standard_ini_run_climate_high_res(self);
				}
			/*printf("Simtime: %f\n",xpn->pTi->pSimTime->fTime);
			printf("TimeStep: %f\n",xpn->pTi->pTimeStep->fAct);
			printf("Anzahl Tage: %d\n",(int)self->AnzahlderTage);
			printf("Tag Nr: %d\n",(int)(xpn->pTi->pSimTime->fTimeAct));*/
			// Ausgabe der Zeit soll nur einmal erfolgen:
			if (xpn->pXSys->id==0)
				{
					if (xpn->pTi->pSimTime->mday != self->timesave)
						{
							//int minute,hour;
							//hour = (int)(xpn->pTi->pSimTime->fTimeDay*24.0);
							//minute = (int)((xpn->pTi->pSimTime->fTimeDay*24.0-hour)*60.0);
							S =xpn_time_get_formated_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,FALSE);
							fprintf(stdout,"Simulation Day : %s\n",S);
							fflush(stdout);
							g_free(S);
							self->timesave=xpn->pTi->pSimTime->mday;
						}
				}
			//PRINT_ERROR_ID((&(self->parent)),"Test");
			if (self->firstround==0)
				{
					xpn->pTi->pSimTime->bFirstRound=TRUE;
				}
			else
				{
					xpn->pTi->pSimTime->bFirstRound=FALSE;
				}
			self->firstround=1;
		}
	else     // externes Zeitmanagement:
		{
			// Ausgabe der Zeit soll nur einmal erfolgen:
			if (xpn->pXSys->id==0)
				{
					//if (xpn->pTi->pSimTime->fTime >= self->timesave+(1.0*60.0*60.0*24.0))
					{
						char str_h[5],str_m[5],str_s[5];
						if (xpn->pTi->pSimTime->hour < 10) sprintf(str_h,"0%d",xpn->pTi->pSimTime->hour);
						else sprintf(str_h,"%d",xpn->pTi->pSimTime->hour);
						if (xpn->pTi->pSimTime->min< 10) sprintf(str_m,"0%d",xpn->pTi->pSimTime->min);
						else sprintf(str_m,"%d",xpn->pTi->pSimTime->min);
						if (xpn->pTi->pSimTime->sec < 10) sprintf(str_s,"0%d",xpn->pTi->pSimTime->sec);
						else sprintf(str_s,"%d",xpn->pTi->pSimTime->sec);
						S = xpn_time_get_formated_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,FALSE);
						//fprintf(stdout,"Simulation Day : %s %s:%s:%s %d %f\n",S,str_h,str_m,str_s,self->WRF->M,xpn->pTi->pSimTime->fTime);
						fprintf(stdout,"Simulation Day : %s %s:%s:%s\n",S,str_h,str_m,str_s);
						fflush(stdout);
						g_free(S);
						self->timesave=xpn->pTi->pSimTime->mday;
					}
				}
			//PRINT_ERROR_ID((&(self->parent)),"Test");
			if (self->firstround==0)
				{
					xpn->pTi->pSimTime->bFirstRound=TRUE;
					self->number_of_days=xpn_time_get_number_of_days(xpn->pTi->pSimTime->iStart_year, xpn->pTi->pSimTime->fStart_TimeY,xpn->pTi->pSimTime->iStop_year, xpn->pTi->pSimTime->fStop_TimeY);
				}
			else
				{
					xpn->pTi->pSimTime->bFirstRound=FALSE;
				}
			self->firstround=1;
		}
	//self->durchgang++;
	/*if (self->durchgang == 20)
	    {
	        PRINT_MESSAGE(xpn,3,"Expert N Abbruch\n");
	        exit(1);
	    }*/
	for (pWLayer = xpn->pWa->pWLayer; pWLayer!=NULL; pWLayer=pWLayer->pNext)
		{
			pWLayer->fContOld =  pWLayer->fContAct;
		}
	iyear = xpn->pTi->pSimTime->iStop_year;
	fTimeY = xpn->pTi->pSimTime->fStop_TimeY;
	xpn_time_add_dt(&iyear,&fTimeY,-xpn->pTi->pTimeStep->fAct);
	if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,iyear,fTimeY)>=0)
		{
			xpn->pTi->pSimTime->bLastRound=TRUE;
		}
	else
		{
			xpn->pTi->pSimTime->bLastRound=FALSE;
		}
	if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,xpn->pTi->pSimTime->iStop_year,xpn->pTi->pSimTime->fStop_TimeY)>=0)
		{
			return 1;
		}
	self->sim_time_intern += xpn->pTi->pTimeStep->fAct;
	return RET_SUCCESS;
}
int expertn_standard_ini_done(expertn_standard_ini *self)
{
	char *S;
	expertn_standard_ini_free_weather_file(self);
	expertn_standard_ini_free_climate_ts_file(self);
	G_FREE_IF_NOT_0(self->weather_file_name);
	FREE_IF_NOT_0(self->cfg_filename);
	if (self->parent.pXSys->id==0)
		{
			if (self->measure_time==1)
				{
#ifdef USE_OPENMP
					self->end_time_intern = omp_get_wtime();
#else
					self->end_time_intern = (double)clock()/(double)CLOCKS_PER_SEC;
#endif
					S = g_strdup_printf("Expert-N execution time : %f s\n",self->end_time_intern-self->start_time_intern);
					PRINT_MESSAGE((&(self->parent)),3,S);
					g_free(S);
					S = g_strdup_printf("Expert-N simulation time: %f s\n",self->sim_time_intern*(24.0*60.0*60.0));
					PRINT_MESSAGE((&(self->parent)),3,S);
					g_free(S);
				}
		}
	return RET_SUCCESS;
}
int expertn_standard_ini_load_config(expertn_standard_ini *self,GDate *global_start_date,GDate *global_end_date)
{
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int i;
	double sum;
	filename=g_strdup_printf("%s",self->cfg_filename);
	self->cfg = (STRUCT_CFG_FILE*)g_malloc0(sizeof(STRUCT_CFG_FILE));
	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}
	// Simulation:
	if (global_start_date==NULL)
		{
			GET_INI_DATE(self->cfg->start_date,"simulation","start");
		}
	else
		{
			self->cfg->start_date = g_date_new_dmy(g_date_get_day(global_start_date),g_date_get_month(global_start_date),g_date_get_year(global_start_date) );
		}
	if (global_end_date==NULL)
		{
			GET_INI_DATE(self->cfg->end_date,"simulation","end");
		}
	else
		{
			self->cfg->end_date = g_date_new_dmy(g_date_get_day(global_end_date),g_date_get_month(global_end_date),g_date_get_year(global_end_date) );
		}
	// location:
	GET_INI_DOUBLE(self->cfg->lat,"location","lat");
	GET_INI_DOUBLE(self->cfg->lon,"location","lon");
	GET_INI_DOUBLE(self->cfg->alt,"location","alt");
	GET_INI_STRING(self->cfg->exposition,"location","exposition");
	GET_INI_DOUBLE(self->cfg->inclination,"location","inclination");
	GET_INI_DOUBLE(self->cfg->size,"location","size");
	GET_INI_DOUBLE_OPTIONAL(self->cfg->wind_measure_height,"location","wind_measure_height",2.0);
	GET_INI_DOUBLE_OPTIONAL(self->cfg->temp_measure_height,"location","temp_measure_height",2.0);
	//GET_INI_DOUBLE(self->cfg->wind_measure_height,"location","wind_measure_height");
	//GET_INI_DOUBLE(self->cfg->temp_measure_height,"location","temp_measure_height");
	// climate:
	GET_INI_DOUBLE_OPTIONAL(self->cfg->AveYearTemp,"climate","AveYearTemp",7.4);
	GET_INI_DOUBLE_OPTIONAL(self->cfg->MonthTempAmp,"climate","MonthTempAmp",6.0);
	
	//Added by Hong on 20190930
	//preceding_crop
	GET_INI_DOUBLE_OPTIONAL(self->cfg->aboveResidualC,"preceding_crop","above_residual_c",0.0);
	GET_INI_DOUBLE_OPTIONAL(self->cfg->aboveResidualN,"preceding_crop","above_residual_n",0.0);
	
	//End of hong
	
	
	// soil properties:
	GET_INI_INT(self->cfg->layer_count,"soil","layer_count");
	GET_INI_DOUBLE(self->cfg->layer_thickness,"soil","layer_thickness");
	GET_INI_INT_ARRAY(self->cfg->layers,self->cfg->layers_len,"soil","layers");
	GET_INI_DOUBLE_ARRAY(self->cfg->clay,self->cfg->clay_len,"soil","clay");
	GET_INI_DOUBLE_ARRAY(self->cfg->silt,self->cfg->silt_len,"soil","silt");
	GET_INI_DOUBLE_ARRAY(self->cfg->sand,self->cfg->sand_len,"soil","sand");
	GET_INI_DOUBLE_ARRAY(self->cfg->organic_matter,self->cfg->organic_matter_len,"soil","organic_matter");
	GET_INI_DOUBLE_ARRAY(self->cfg->bulk_desity,self->cfg->bulk_density_len,"soil","bulk_density");
	GET_INI_DOUBLE_ARRAY(self->cfg->rock_fraction,self->cfg->rock_fraction_len,"soil","rock_fraction");
	GET_INI_DOUBLE_ARRAY(self->cfg->ph,self->cfg->ph_len,"soil","ph");
	calc_if_n_value(self->cfg->rock_fraction,self->cfg->rock_fraction_len, {self->cfg->rock_fraction[i]/=100.0;},0.0);
	// Array Groessen muessen alle gleich sein, sonst: Fehlermeldung und abbrechen
	CHECK_LEN(self->cfg->layers_len,self->cfg->clay_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->silt_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->sand_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->organic_matter_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->bulk_density_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->rock_fraction_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->ph_len);
	// Optionale soil Properties:
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->organic_carbon,self->cfg->organic_carbon_len,self->cfg->layers_len,-99.0,"soil","organic_carbon");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->organic_nitrogen,self->cfg->organic_nitrogen_len,self->cfg->layers_len,-99.0,"soil","organic_nitrogen");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->porosity,self->cfg->porosity_len,self->cfg->layers_len,-99.0,"soil","porosity");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->cont_sat,self->cfg->cont_sat_len,self->cfg->layers_len,-99.0,"soil","cont_sat");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->wilting_point,self->cfg->wilting_point_len,self->cfg->layers_len,-99.0,"soil","wilting_point");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->field_capacity,self->cfg->field_capacity_len,self->cfg->layers_len,-99.0,"soil","field_capacity");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->res_water_cont,self->cfg->res_water_cont_len,self->cfg->layers_len,-99.0,"soil","res_water_cont");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->camp_a,self->cfg->camp_a_len,self->cfg->layers_len,-99.0,"soil","camp_a");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->camp_b,self->cfg->camp_b_len,self->cfg->layers_len,-99.0,"soil","camp_b");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->van_gen_a,self->cfg->van_gen_a_len,self->cfg->layers_len,-99.0,"soil","van_gen_a");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->van_gen_n,self->cfg->van_gen_n_len,self->cfg->layers_len,-99.0,"soil","van_gen_n");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->max_pot,self->cfg->max_pot_len,self->cfg->layers_len,-99.0,"soil","max_pot");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->cond_sat,self->cfg->cond_sat_len,self->cfg->layers_len,-99.0,"soil","cond_sat");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->cont_inflec,self->cfg->cont_inflec_len,self->cfg->layers_len,-99.0,"soil","cont_inflec");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(self->cfg->pot_inflec_hPa,self->cfg->pot_inflec_hPa_len,self->cfg->layers_len,-99.0,"soil","pot_inflec_hPa");
	GET_INI_STRING_ARRAY_OPTIONAL(self->cfg->soil_type,self->cfg->soil_type_len,self->cfg->layers_len,"-99.0","soil","soil_type");
	// Array Groessen muessen alle gleich sein, sonst: Fehlermeldung und abbrechen
	CHECK_LEN(self->cfg->layers_len,self->cfg->organic_carbon_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->organic_nitrogen_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->porosity_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->cont_sat_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->wilting_point_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->field_capacity_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->res_water_cont_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->camp_a_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->camp_b_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->van_gen_a_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->van_gen_n_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->max_pot_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->cond_sat_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->cont_inflec_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->pot_inflec_hPa_len);
	CHECK_LEN(self->cfg->layers_len,self->cfg->soil_type_len);
	for (i=0; i<self->cfg->soil_type_len; i++)
		{
			sum=self->cfg->clay[i]+self->cfg->sand[i]+self->cfg->silt[i];
			if ((sum-EPSILON > 100.0) || (sum+EPSILON < 100.0))
				{
					gchar *S;
					S = g_strdup_printf  ("Clay + silt + sand is not 100 %%, it is %f. Check your configuration in file (col %d): %s!",sum,i+1,filename);
					PRINT_ERROR(S);
					g_free(S);
					return -1;
				}
		}
	calc_if_n_value(self->cfg->porosity,self->cfg->porosity_len, {self->cfg->porosity[i]/=100.0;},-99.0);
	calc_if_n_value(self->cfg->cont_sat,self->cfg->cont_sat_len, {self->cfg->cont_sat[i]/=100.0;},-99.0);
	calc_if_n_value(self->cfg->wilting_point,self->cfg->wilting_point_len, {self->cfg->wilting_point[i]/=100.0;},-99.0);
	calc_if_n_value(self->cfg->field_capacity,self->cfg->field_capacity_len, {self->cfg->field_capacity[i]/=100.0;},-99.0);
	calc_if_n_value(self->cfg->res_water_cont,self->cfg->res_water_cont_len, {self->cfg->res_water_cont[i]/=100.0;},-99.0);
	calc_if_n_value(self->cfg->camp_a,self->cfg->camp_a_len, {self->cfg->camp_a[i]*=kPaTOmm;},-99.0);
	calc_if_n_value(self->cfg->van_gen_a,self->cfg->van_gen_a_len, {self->cfg->van_gen_a[i]*=0.1;},-99.0); /*1/cm to 1/mm*/
	calc_if_n_value(self->cfg->max_pot ,self->cfg->max_pot_len, {self->cfg->max_pot[i]*=kPaTOmm;},-99.0);
	calc_if_n_value(self->cfg->pot_inflec_hPa ,self->cfg->pot_inflec_hPa_len, {self->cfg->pot_inflec_hPa[i]*=kPaTOmm;},-99.0);
	for (i=0; i<self->cfg->soil_type_len; i++)
		{
			deleteSpaceBegEnd(self->cfg->soil_type[i]);
		}
	// start values:
	GET_INI_INT_ARRAY(self->cfg->start_layers,self->cfg->start_layers_len,"start values","layers");
	GET_INI_DOUBLE_ARRAY(self->cfg->water_content,self->cfg->water_content_len,"start values","water_content");
	GET_INI_DOUBLE_ARRAY(self->cfg->matrix_potential,self->cfg->matrix_potential_len,"start values","matrix_potential");
	GET_INI_DOUBLE_ARRAY(self->cfg->soil_temperature,self->cfg->soil_temperature_len,"start values","soil_temperature");
	GET_INI_DOUBLE_ARRAY(self->cfg->nh4_content,self->cfg->nh4_content_len,"start values","nh4_content");
	GET_INI_DOUBLE_ARRAY(self->cfg->no3_content,self->cfg->no3_content_len,"start values","no3_content");
	GET_INI_DOUBLE_ARRAY(self->cfg->root_density,self->cfg->root_density_len,"start values","root_density");
	// Array Groessen muessen alle gleich sein, sonst: Fehlermeldung und abbrechen
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->start_layers_len);
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->water_content_len);
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->matrix_potential_len);
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->soil_temperature_len);
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->nh4_content_len);
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->no3_content_len);
	CHECK_LEN(self->cfg->start_layers_len,self->cfg->root_density_len);
	calc_if_n_value(self->cfg->water_content,self->cfg->water_content_len, {self->cfg->water_content[i]/=100.0;},-99.0);
	g_key_file_free(keyfile);
	g_free(filename);
	return RET_SUCCESS;
}
void expertn_standard_ini_free_cfg_file(expertn_standard_ini *self)
{
	int i;
	if (self->cfg!=NULL)
		{
			G_FREE_DATE_IF_NOT_0(self->cfg->start_date);
			G_FREE_DATE_IF_NOT_0(self->cfg->end_date);
			G_FREE_IF_NOT_0(self->cfg->exposition);
			G_FREE_IF_NOT_0(self->cfg->layers);
			G_FREE_IF_NOT_0(self->cfg->clay);
			G_FREE_IF_NOT_0(self->cfg->silt);
			G_FREE_IF_NOT_0(self->cfg->sand);
			G_FREE_IF_NOT_0(self->cfg->organic_matter);
			G_FREE_IF_NOT_0(self->cfg->bulk_desity);
			G_FREE_IF_NOT_0(self->cfg->rock_fraction);
			G_FREE_IF_NOT_0(self->cfg->ph);
			G_FREE_IF_NOT_0(self->cfg->start_layers);
			G_FREE_IF_NOT_0(self->cfg->water_content);
			G_FREE_IF_NOT_0(self->cfg->matrix_potential);
			G_FREE_IF_NOT_0(self->cfg->soil_temperature);
			G_FREE_IF_NOT_0(self->cfg->nh4_content);
			G_FREE_IF_NOT_0(self->cfg->no3_content);
			G_FREE_IF_NOT_0(self->cfg->root_density);
			G_FREE_IF_NOT_0(self->cfg->organic_carbon);
			G_FREE_IF_NOT_0(self->cfg->organic_nitrogen);
			G_FREE_IF_NOT_0(self->cfg->porosity);
			G_FREE_IF_NOT_0(self->cfg->cont_sat);
			G_FREE_IF_NOT_0(self->cfg->wilting_point);
			G_FREE_IF_NOT_0(self->cfg->field_capacity);
			G_FREE_IF_NOT_0(self->cfg->res_water_cont);
			G_FREE_IF_NOT_0(self->cfg->camp_a);
			G_FREE_IF_NOT_0(self->cfg->camp_b);
			G_FREE_IF_NOT_0(self->cfg->van_gen_a);
			G_FREE_IF_NOT_0(self->cfg->van_gen_n);
			G_FREE_IF_NOT_0(self->cfg->max_pot);
			G_FREE_IF_NOT_0(self->cfg->cond_sat);
			G_FREE_IF_NOT_0(self->cfg->cont_inflec);
			G_FREE_IF_NOT_0(self->cfg->pot_inflec_hPa);
			if (self->cfg->soil_type!=NULL)
				{
					for (i=0; i<self->cfg->soil_type_len; i++)
						{
							G_FREE_IF_NOT_0(self->cfg->soil_type[i]);
						}
					G_FREE_IF_NOT_0(self->cfg->soil_type);
				}
			g_free(self->cfg);
			self->cfg=NULL;
		}
}
int expertn_standard_ini_load_weather_file(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	int size;
	int date_format;
	int year,month,day;
	char *seperator;
	char *S,*S2,*S3;
	char *date_str;
	FILE *f;
	int i,i2;
	double *values;
	int min_columns;
	GDate date;
	STRUCT_XNO_DATA* data;
	int iYear;
	double fTimeY;
	g_date_clear(&date,1);
	self->climate_values=NULL;
	f = fopen(self->weather_file_name ,"r");
	i = 0;
	if (f==NULL)
		{
			S = g_strdup_printf("Open Weather File %s!",self->weather_file_name);
			PRINT_ERROR(S);
			g_free(S);
			return 1;
		}
	seperator = ",";
	min_columns = 18;
	date_format = cdb_format;
	size=0;
	while (1)
		{
			S = fgets_mem(f);
			if (S==NULL) break;
			if ((i==0) || (i==1))
				{
					size = getStringColumn_get_Size(S,seperator);
					if (size < min_columns)
						{
							seperator=";";
							size = getStringColumn_get_Size(S,seperator);
							if (size < min_columns)
								{
									seperator="\t";
									size = getStringColumn_get_Size(S,seperator);
									if (size < min_columns)
										{
											seperator=" ";
											size = getStringColumn_get_Size(S,seperator);
											if (size < min_columns)
												{
													char *S_ERROR;
													S_ERROR = g_strdup_printf("The weather file %s has the wrong number of columns %d (colon,semicolon,tabulator and space is alowed as seperator)!",self->weather_file_name,min_columns);
													PRINT_ERROR(S_ERROR);
													g_free(S_ERROR);
													return 1;
												}
										}
								}
						}
				}
			S2 = (char*)g_malloc0(strlen(S)+1);
			date_str = (char*)g_malloc0(strlen(S)+1);
			if (i==0)
				{
					data = (STRUCT_XNO_DATA*)g_malloc0(sizeof(STRUCT_XNO_DATA));
					data->size_of_values = size;
					data->size_of_valuelist = 0;
					data->valuelist = NULL;
					data->iyear = NULL;
					data->fTimeY = NULL;
					data->header = (char**)g_malloc0(sizeof(char*)*size);
					for (i2=0; i2<size; i2++)
						{
							getStringColumn(S2,S,seperator,i2);
							S3 = (char*)g_malloc0(strlen(S2)+1);
							strcpy(S3,S2);
							data->header[i2] = S3;
						}
				}
			else
				{
					if (size!=data->size_of_values)
						{
							char *S_ERROR;
							S_ERROR = g_strdup_printf("The weather file %s has the wrong number of rows %d in line %d",self->weather_file_name,min_columns,i+1);
							PRINT_ERROR(S_ERROR);
							g_free(S_ERROR);
							break;
						}
					values =(double*)g_malloc0(sizeof(double)*size);
					for (i2=0; i2<size; i2++)
						{
							getStringColumn(S2,S,seperator,i2);
							if (i2==0)
								{
									if (i==1)
										{
											// Datums format checken:
											if (getStringColumn_get_Size(S,"-") >= 3)
												{
													date_format=csv_minus;
												}
											else if (getStringColumn_get_Size(S,"//") >= 3)
												{
													date_format=csv_slash;
												}
											else if (getStringColumn_get_Size(S,".") >= 3)
												{
													date_format=csv_german;
												}
											else
												{
													date_format=cdb_format;
												}
										}
									switch(date_format)
										{
										case csv_minus: // yyyy-mm--dd
											getStringColumn(date_str,S2,"-",0);
											year=atoi(date_str);
											getStringColumn(date_str,S2,"-",1);
											month=atoi(date_str);
											getStringColumn(date_str,S2,"-",2);
											day=atoi(date_str);
											g_date_set_day(&date,day);
											g_date_set_month(&date,month);
											g_date_set_year(&date,year);
											xpn_time_conv_gdate_to_xpn_time(&date,&iYear, &fTimeY);
											//values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
											values[i2] = fTimeY;
											break;
										case csv_slash: // mm-dd-yyyy
											getStringColumn(date_str,S2,"//",0);
											month=atoi(date_str);
											getStringColumn(date_str,S2,"//",1);
											day=atoi(date_str);
											getStringColumn(date_str,S2,"//",2);
											year=atoi(date_str);
											g_date_set_day(&date,day);
											g_date_set_month(&date,month);
											g_date_set_year(&date,year);
											xpn_time_conv_gdate_to_xpn_time(&date,&iYear, &fTimeY);
											//values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
											values[i2] = fTimeY;
											break;
										case csv_german:// dd.mm.yyyy
											getStringColumn(date_str,S2,".",0);
											day=atoi(date_str);
											getStringColumn(date_str,S2,".",1);
											month=atoi(date_str);
											getStringColumn(date_str,S2,".",2);
											year=atoi(date_str);
											g_date_set_day(&date,day);
											g_date_set_month(&date,month);
											g_date_set_year(&date,year);
											xpn_time_conv_gdate_to_xpn_time(&date,&iYear, &fTimeY);
											//values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
											values[i2] = fTimeY;
											break;
										default:
											values[i2] = atof(S2);
										}
								}
							else
								{
									values[i2] = atof(S2);
								}
						}
					data->size_of_valuelist++;
					data->valuelist = (double**)g_realloc(data->valuelist,sizeof(double*)*data->size_of_valuelist);
					data->iyear = (int*)g_realloc(data->iyear,sizeof(int)*data->size_of_valuelist);
					data->fTimeY = (double*)g_realloc(data->fTimeY,sizeof(double)*data->size_of_valuelist);
					data->valuelist[data->size_of_valuelist-1] = values;
					data->iyear[data->size_of_valuelist-1] = iYear;
					data->fTimeY[data->size_of_valuelist-1] = fTimeY;
				}
			g_free(S2);
			g_free(date_str);
			free(S);
			i++;
		}
	fclose(f);
	if (i < 2) return -1;
	self->climate_values=data;
	return RET_SUCCESS;
}
int expertn_standard_ini_load_climate_ts_file(expertn_standard_ini *self)
{
	self->climate_values_small_ts=xpn_load_climate_xno_file(self->weather_file_name_st,6);
	if (self->climate_values_small_ts==NULL)
		{
			return -1;
		}
	else
		{
			return RET_SUCCESS;
		}
}
int expertn_standard_ini_free_weather_file(expertn_standard_ini *self)
{
	self->climate_values = xpn_free_XNO_DATA(self->climate_values);
	return RET_SUCCESS;
}
int expertn_standard_ini_free_climate_ts_file(expertn_standard_ini *self)
{
	int i,i2;
	STRUCT_XNO_DATA* data;
	data = self->climate_values_small_ts;
	if (data!=NULL)
		{
			if (data->header!=NULL)
				{
					for (i=0; i<data->size_of_values; i++)
						{
							if (data->header[i]!=NULL)
								{
									g_free(data->header[i]);
									data->header[i]=NULL;
								}
						}
					g_free(data->header);
					data->header=NULL;
				}
			if (data->valuelist!=NULL)
				{
					for (i2=0; i2<data->size_of_valuelist; i2++)
						{
							if (data->valuelist[i2] !=NULL)
								{
									g_free(data->valuelist[i2]);
									data->valuelist[i2] = NULL;
								}
						}
					if (data->iyear!=NULL) g_free(data->iyear);
					if (data->fTimeY!=NULL) g_free(data->fTimeY);
				}
			g_free(data);
		}
	self->climate_values_small_ts = NULL;
	return RET_SUCCESS;
}
void expertn_standard_ini_set_location(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	if (!xpn->pXSys->extern_time_management)
		{
			xpn_time_conv_gdate_to_xpn_time(self->cfg->start_date,&(xpn->pTi->pSimTime->iStart_year),&(xpn->pTi->pSimTime->fStart_TimeY));
			xpn_time_conv_gdate_to_xpn_time(self->cfg->end_date,&(xpn->pTi->pSimTime->iStop_year),&(xpn->pTi->pSimTime->fStop_TimeY));
			xpn->pTi->pSimTime->fSimDuration = xpn_time_get_number_of_days(xpn->pTi->pSimTime->iStart_year, xpn->pTi->pSimTime->fStart_TimeY,xpn->pTi->pSimTime->iStop_year, xpn->pTi->pSimTime->fStop_TimeY);
			self->number_of_days = expertn_database_help_func_Time_Message_and_get_number_of_days(xpn);
		}
	xpn->pLo->pFarm->fLongitude=self->cfg->lon;
	xpn->pLo->pFarm->fLatitude=self->cfg->lat;
	xpn->pLo->pFarm->fAltitude=self->cfg->alt;
	xpn->pCl->fTempMeasHeight=self->cfg->temp_measure_height;
	xpn->pCl->fWindMeasHeight=self->cfg->wind_measure_height;
	/*char* exposition;
	double inclination; // Hang Neigung[%]
	double size; // [ha]*/
}
void expertn_standard_ini_set_climate_Ave(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	xpn->pCl->pAverage->fYearTemp = self->cfg->AveYearTemp;
	xpn->pCl->pAverage->fMonthTempAmp = self->cfg->MonthTempAmp;
}


//Added by Hong on 20190930
void expertn_standard_ini_set_preceding_crop(expertn_standard_ini *self) 
{
	expertn_modul_base *xpn = &(self->parent);
	xpn->pCh->pCProfile->fCStandCropRes = self->cfg->aboveResidualC;
	xpn->pCh->pCProfile->fNStandCropRes = self->cfg->aboveResidualN;
}
//End of Hong
void expertn_standard_ini_set_soil(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	int i,i2,i3;
	double fDepth_all;
	PSPROFILE pSo;
	PSLAYER pSLayer;
	PSWATER pSWater;
	PSHEAT pSHeat;
	//Added by Hong
	PWLAYER pWL;
	PSWATER      pSW;
    pWL= xpn->pWa->pWLayer; //Added by Hong
	pSW = xpn->pSo->pSWater;
	//End of Hong
	pSo = xpn->pSo;
	pSo->fDepth=0.0;
	fDepth_all =0.0;
	i3 = 0;
	i2 = 0;
	
	//Added by Hong
	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR("Problem with hydraulic functions!");
		}
	//End of Hong
	
	
	for (pSLayer=xpn->pSo->pSLayer,pSHeat = xpn->pSo->pSHeat,pSWater=xpn->pSo->pSWater,i=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pSHeat = pSHeat->pNext,i++)
		{
			pSLayer->fClay = self->cfg->clay[i2];
			pSLayer->fSilt = self->cfg->silt[i2];
			pSLayer->fSand = self->cfg->sand[i2];
			pSLayer->fNHumus = self->cfg->organic_nitrogen[i2];
			pSLayer->fCHumus = self->cfg->organic_carbon[i2];
			pSLayer->fHumus = self->cfg->organic_matter[i2];
			pSLayer->fpH = self->cfg->ph[i2];
			pSLayer->fBulkDens = self->cfg->bulk_desity[i2];
			pSLayer->fBulkDensStart = self->cfg->bulk_desity[i2];
			pSLayer->fPorosity = self->cfg->porosity[i2];
			//pSLayer->fRockFrac = self->cfg->rock_fraction[i2] * pSLayer->fBulkDens/2.65; //weight% to volume%
			pSLayer->fRockFrac = self->cfg->rock_fraction[i2] * pSLayer->fBulkDens/2.06; //weight% to volume%
			pSWater->fContSat = pSLayer->fPorosity;//self->cfg->cont_sat[i2];
			pSWater->fContPWP = self->cfg->wilting_point[i2];
			pSWater->fContFK = self->cfg->field_capacity[i2];
			pSWater->fContRes = self->cfg->res_water_cont[i2];
			pSWater->fCampA = self->cfg->camp_a[i2];
			pSWater->fCampB = self->cfg->camp_b[i2];
			pSWater->fVanGenA = self->cfg->van_gen_a[i2];
			pSWater->fVanGenN = self->cfg->van_gen_n[i2];
			//Added by Hong Dec.2018, test of Hong
			int f1, f2;
			f1= (double)-150000;
			f2=(double)-3300;
			SET_IF_99(pSWater->fContPWP,  WATER_CONTENT(f1));
			SET_IF_99(pSWater->fContFK, WATER_CONTENT(f2));
			//End of Hong
			pSWater->fMinPot = self->cfg->max_pot[i2];
			pSWater->fCondSat = self->cfg->cond_sat[i2];
			pSWater->fContInflec = self->cfg->cont_inflec[i2];
			pSWater->fPotInflec =  self->cfg->pot_inflec_hPa[i2];
			pSLayer->acSoilID=g_strdup_printf("%s",self->cfg->soil_type[i2]);
			pSLayer->fThickness =self->cfg->layer_thickness * 10.0; // cm --> mm
			if ((i>0) && (i<pSo->iLayers-1))
				{
					pSo->fDepth+=pSLayer->fThickness; // Gesamtprofiltiefe
				}
			fDepth_all+=pSLayer->fThickness; // Gesamtprofiltiefe
			if ((i>0) && (i<xpn->pSo->iLayers-2))
				{
					i3++;
					if (i3 >= self->cfg->layers[i2])
						{
							i3=0;
							i2++;
						}
				}
		}
	pSo->fDeltaZ=fDepth_all/xpn->pSo->iLayers;
	expertn_database_help_func_set_standard_soil_paramter(xpn);
}
void expertn_standard_ini_set_start_values(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
	int i,i2,i3;
	PSLAYER pSLayer;
	PSWATER pSWater;
	PSHEAT pSHeat;
	PCLAYER pCLayer;
	PWLAYER pWLayer;
	PHLAYER pHLayer;    
	i3 = 0;
	i2 = 0;
	for (pHLayer=xpn->pHe->pHLayer, pCLayer=xpn->pCh->pCLayer,pWLayer=xpn->pWa->pWLayer, pSLayer=xpn->pSo->pSLayer,pSHeat = xpn->pSo->pSHeat,pSWater=xpn->pSo->pSWater,i=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pSHeat = pSHeat->pNext,pCLayer=pCLayer->pNext,pWLayer=pWLayer->pNext,pHLayer=pHLayer->pNext,i++)
		{
			pCLayer->fNO3N = self->cfg->no3_content[i2] / self->cfg->start_layers[i2];
			pCLayer->fNH4N = self->cfg->nh4_content[i2] / self->cfg->start_layers[i2];
			pWLayer->fContAct = self->cfg->water_content[i2];
			
            pHLayer->fSoilTemp = self->cfg->soil_temperature[i2];
			pHLayer->fSoilTempOld = self->cfg->soil_temperature[i2];
            
           
            
			if ((i>0) && (i<xpn->pSo->iLayers-2))
				{
					i3++;
					if (i3 >= self->cfg->start_layers[i2])
						{
							i3=0;
							i2++;
						}
				}
		}
}
#define expertn_standard_ini_run_climate_high_res_interpol(value_result,value_number)\
	if (i<=0)\
		{\
			value_result = self->climate_values_small_ts->valuelist[i][value_number];\
		} else\
		{\
			value_result = xpn_time_interpolate_linear(self->climate_values_small_ts->iyear[i-1],self->climate_values_small_ts->fTimeY[i-1],self->climate_values_small_ts->valuelist[i-1][value_number] ,self->climate_values_small_ts->iyear[i],self->climate_values_small_ts->fTimeY[i],self->climate_values_small_ts->valuelist[i][value_number], self->parent.pTi->pSimTime->iyear,self->parent.pTi->pSimTime->fTimeY);\
		}
#define expertn_standard_ini_run_climate_high_res_interpol_daily_models(value_result,value_number)\
	if (i<=0)\
		{\
			value_result = self->climate_values_small_ts->valuelist[i][value_number];\
		} else\
		{\
			value_result = xpn_time_interpolate_linear(self->climate_values_small_ts->iyear[i-1],self->climate_values_small_ts->fTimeY[i-1],self->climate_values_small_ts->valuelist[i-1][value_number] ,self->climate_values_small_ts->iyear[i],self->climate_values_small_ts->fTimeY[i],self->climate_values_small_ts->valuelist[i][value_number], add_year,add_fTimeY);\
		}
		
		
void expertn_standard_ini_run_climate_high_res(expertn_standard_ini *self)
{
	expertn_modul_base *xpn = &(self->parent);
//	double dt;
	int i;
	int leap_year;
	int add_year;
	double add_fTimeY;
//	STRUCT_XPN_TIME t1;
//	int day,month,year;
//	dt = xpn->pTi->pTimeStep->fAct;
	for (i=self->weather_index; i<self->climate_values_small_ts->size_of_valuelist; i++)
		{
			if (xpn_time_compare_time(self->climate_values_small_ts->iyear[i],self->climate_values_small_ts->fTimeY[i],self->parent.pTi->pSimTime->iyear,self->parent.pTi->pSimTime->fTimeY)>=0)
				{
					break;
				}
		}
	if (i>=self->climate_values_small_ts->size_of_valuelist)
		{
			PRINT_ERROR_ID((&(self->parent)),"No Weather data found for this date!");
			return;
		}
	self->weather_index=i;
	self->par = -9999.0;
	self->pressure = -9999.0;
	if (self->interpolate_climate_data==1)
		{
			expertn_standard_ini_run_climate_high_res_interpol(self->global_radiation,1)
			expertn_standard_ini_run_climate_high_res_interpol(self->precipitation,2)
			expertn_standard_ini_run_climate_high_res_interpol(self->rel_himidity,3)
			expertn_standard_ini_run_climate_high_res_interpol(self->windspeed,4)
			expertn_standard_ini_run_climate_high_res_interpol(self->Tair,5)
			if (self->climate_values_small_ts->size_of_values>6)
				{
					expertn_standard_ini_run_climate_high_res_interpol(self->par,6)
					if (self->climate_values_small_ts->size_of_values>6)
						{
							expertn_standard_ini_run_climate_high_res_interpol(self->pressure,7);
						}
				}
		}
	else
		{
			self->global_radiation = self->climate_values_small_ts->valuelist[i][1];
			self->precipitation = self->climate_values_small_ts->valuelist[i][2];
			self->rel_himidity = self->climate_values_small_ts->valuelist[i][3];
			self->windspeed = self->climate_values_small_ts->valuelist[i][4];
			self->Tair = self->climate_values_small_ts->valuelist[i][5];
			if (self->climate_values_small_ts->size_of_values>5)
				{
					self->par = self->climate_values_small_ts->valuelist[i][6];
					if (self->climate_values_small_ts->size_of_values>6)
						{
							self->pressure =  self->climate_values_small_ts->valuelist[i][7];
						}
				}
		}
	if (self->no_rain==1)
		{
			self->precipitation = 0.0;
		}
	// vllt muss man später noch schneien berücksichtigen
	//self->precipitation=0.0;
	xpn->pCl->pWeather->fPreciRate = self->precipitation;		// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fLiquPreciRate = self->precipitation;	// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fBulkPreciRate = self->precipitation;	// open land precip.
	xpn->pCl->pWeather->fHumidity = self->rel_himidity;
	xpn->pCl->pWeather->fWindSpeed = self->windspeed;
	xpn->pCl->pWeather->fSolRad = (60.0*60.0*24.0) * self->global_radiation * 1.0e-6;
	if (self->par < -9998.0)
		{
			xpn->pCl->pWeather->fPAR = xpn->pCl->pWeather->fSolRad / 2.0;//CalculateActiveRadiation(xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay, self->global_radiation ,xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->iJulianDay);
		}
	else
		{
			xpn->pCl->pWeather->fPAR = (60.0*60.0*24.0) * self->par * 1.0e-6;
		}
	if (self->pressure >= -9998.0)
		{
			xpn->pCl->pWeather->fAtmPressure = self->pressure;
		}
	//xpn->pCl->pWeather->fTempAir=db_get_double(data_model,9); // mittlere Lufttemperatur
	xpn->pCl->pWeather->fTempAir = self->Tair;
	xpn->pCl->pWeather->fTempAir_zlvl = self->Tair;
	if (xpn->pCl->fTempMeasHeight==0.0)
		{
			xpn->pCl->fTempMeasHeight=2.0;
		}
	if (xpn->pCl->fWindMeasHeight==0.0)
		{
			xpn->pCl->fWindMeasHeight=2.0;
		}
	//xpn->pCl->pWeather->fSolNet = expertn_database_help_func_get_Rn(xpn);
	/*laenge = calculateDayLength(xpn->pLo->pFarm->fLatitude,xpn->pTi->pSimTime->iJulianDay );
	C_DEBUG(laenge);*/
	
	// Get values for daily models for one day later
	if(is_leap_year(self->parent.pTi->pSimTime->iyear) == 0)
		{
			leap_year = 365;
		}
	else
		{
			leap_year = 366;
		}
	if(self->parent.pTi->pSimTime->fTimeY + 1 > leap_year)
		{
			add_year = self->parent.pTi->pSimTime->iyear + 1;
			add_fTimeY =  self->parent.pTi->pSimTime->fTimeY + 1 - leap_year;
		}
	else
		{
			add_year = self->parent.pTi->pSimTime->iyear;
			add_fTimeY = self->parent.pTi->pSimTime->fTimeY + 1;
		}
	for (i=self->weather_index; i<self->climate_values_small_ts->size_of_valuelist; i++)
		{
			if (xpn_time_compare_time(self->climate_values_small_ts->iyear[i],self->climate_values_small_ts->fTimeY[i],add_year,add_fTimeY)>=0)
				{
					break;
				}
		}
	//i += (int)(24);
	if (self->interpolate_climate_data==1)
		{
			expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->global_radiation,1)
			expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->precipitation,2)
			expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->rel_himidity,3)
			expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->windspeed,4)
			expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->Tair,5)
			if (self->climate_values_small_ts->size_of_values>6)
				{
					expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->par,6)
					if (self->climate_values_small_ts->size_of_values>6)
						{
							expertn_standard_ini_run_climate_high_res_interpol_daily_models(self->pressure,7);
						}
				}
		}
	else
		{
			self->global_radiation = self->climate_values_small_ts->valuelist[i][1];
			self->precipitation = self->climate_values_small_ts->valuelist[i][2];
			self->rel_himidity = self->climate_values_small_ts->valuelist[i][3];
			self->windspeed = self->climate_values_small_ts->valuelist[i][4];
			self->Tair = self->climate_values_small_ts->valuelist[i][5];
			if (self->climate_values_small_ts->size_of_values>5)
				{
					self->par = self->climate_values_small_ts->valuelist[i][6];
					if (self->climate_values_small_ts->size_of_values>6)
						{
							self->pressure =  self->climate_values_small_ts->valuelist[i][7];
						}
				}
		}
	if (self->no_rain==1)
		{
			self->precipitation = 0.0;
		}
	xpn->pCl->pWeather->fPreciRate_daily_models = self->precipitation;		// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fHumidity_daily_models = self->rel_himidity;
	xpn->pCl->pWeather->fWindSpeed_daily_models = self->windspeed;
	xpn->pCl->pWeather->fSolRad_daily_models = (60.0*60.0*24.0) * self->global_radiation * 1.0e-6;
	if (self->pressure >= -9998.0)
		{
			xpn->pCl->pWeather->fAtmPressure_daily_models = self->pressure;
		}
	//xpn->pCl->pWeather->fTempAir=db_get_double(data_model,9); // mittlere Lufttemperatur
	xpn->pCl->pWeather->fTempAir_daily_models = self->Tair;
}
void expertn_standard_ini_runWetterTageswerte(expertn_standard_ini *self)
{
    double Solar_radiation(expertn_modul_base *xpn, double); //SG20190212
	expertn_modul_base *xpn = &(self->parent);
//	double dt;
	int i;
	STRUCT_XPN_TIME t1;
	int day,month,year;
//	dt = xpn->pTi->pTimeStep->fAct;
	if ((self->OldDate_day !=xpn->pTi->pSimTime->mday) || (self->OldDate_month !=xpn->pTi->pSimTime->mon) || (self->OldDate_year !=xpn->pTi->pSimTime->year))
		{
			self->OldDate_day = xpn->pTi->pSimTime->mday;
			self->OldDate_month = xpn->pTi->pSimTime->mon;
			self->OldDate_year = xpn->pTi->pSimTime->year;
			day = self->parent.pTi->pSimTime->mday;
			month = self->parent.pTi->pSimTime->mon;
			year = self->parent.pTi->pSimTime->year;
			for (i=0; i<self->climate_values->size_of_valuelist; i++)
				{
					xpn_time_get_struc(&t1, self->climate_values->iyear[i],self->climate_values->fTimeY[i]);
					//if (xpn_time_compare_time(self->climate_values->iyear[i],self->climate_values->fTimeY[i],iyear,fTimeY)>=0)
					if (xpn_time_compare_date(t1.year,t1.mon,t1.mday, year,month,day)>=0)
						{
							break;
						}
				}
			if (i>=self->climate_values->size_of_valuelist)
				{
					PRINT_ERROR_ID((&(self->parent)),"No Weather data found for this date!");
					return;
				}
			self->global_radiation = self->climate_values->valuelist[i][1];
			self->Tairmax = self->climate_values->valuelist[i][2];
			self->Tairmin = self->climate_values->valuelist[i][3];
			self->precipitation = self->climate_values->valuelist[i][4];
			self->sunshine_duration = self->climate_values->valuelist[i][5];
			self->rel_himidity = self->climate_values->valuelist[i][6];
			self->windspeed = self->climate_values->valuelist[i][7];
			self->meantemp = self->climate_values->valuelist[i][8];
			self->dewpoint = self->climate_values->valuelist[i][9];
			self->kesselverdunstung = self->climate_values->valuelist[i][10];
			self->saetigungsdefizit = self->climate_values->valuelist[i][11];
			self->snowdepth = self->climate_values->valuelist[i][12];
			self->par = self->climate_values->valuelist[i][13];
            
            //SG20190211: 
            
            //fehlende Spalten im Wetter-File (Tageswerte) ergänzen:
            
            //self->SoilT02 = self->climate_values->valuelist[i][14];
            //self->SoilT05 = self->climate_values->valuelist[i][15];
            //self->SoilT10 = self->climate_values->valuelist[i][16];
            //self->SoilT20 = self->climate_values->valuelist[i][17];
            //self->SoilT50 = self->climate_values->valuelist[i][18];
        
             if(self->climate_values->size_of_values>19) //wenn Spalte CO2 vorhanden vorhanden
                 self->AtmCO2ppm =   self->climate_values->valuelist[i][19];//SG20190211 - Provisorium für Rajina! Später "Throughfall" [19]; Groundwater [20]; "Atm. CO2" [21];
             
            //self->CnpyDrn = self->climate_values->valuelist[i][20];
            //self->WatTbl= self->climate_values->valuelist[i][21];
            
            
            //Berechnung fehlender Globalstrahlung, falls Sonnenscheindauer gegeben ist:
            if ((self->global_radiation < -9)||(self->global_radiation>=99))
            {
                if ((self->sunshine_duration>=0)&&(self->sunshine_duration<=99))
                {
                      self->global_radiation = Solar_radiation(xpn, self->sunshine_duration);
                }
                else
                {
                    //Fehlermeldung: weder Sonnenscheindauer noch Globalstrahlung gegeben
                    PRINT_ERROR_ID((&(self->parent)),"Neither  global radiation nor sunshine duration given at this date!");
                    return;
                }
            }
     //end of SG
	           
        
  
			// Gestrige Temperatur
			if (i>0)
				{
					self->TairLastDayMax = self->climate_values->valuelist[i-1][2];
				}
			else
				{
					self->TairLastDayMax = self->climate_values->valuelist[i][2];
				}
			// Morgen:
			if (i<self->climate_values->size_of_valuelist-1)
				{
					self->TairNextDayMin =  self->climate_values->valuelist[i+1][3];
				}
			else
				{
					self->TairNextDayMin = self->climate_values->valuelist[i][3];
				}
			xpn->pCl->pWeather->fHumidity = self->rel_himidity; // ist nur Tageswert --> vlt. sollte man auch eine Sinusartige Funktion durchlegen
			xpn->pCl->pWeather->fWindSpeed = self->windspeed;
		}
	if (self->no_rain==1)
		{
			self->precipitation = 0.0;
		}
	// vllt muss man später noch schneien berücksichtigen
	xpn->pCl->pWeather->fPreciRate = self->precipitation;		// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fLiquPreciRate = self->precipitation;	// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fBulkPreciRate = self->precipitation;	// open land precip.!!!
	xpn->pCl->pWeather->fSolRad = get_SolRad(self->global_radiation,xpn->pTi->pSimTime->fTimeY, xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->fTimeDay, xpn->pTi->pTimeStep->fAct);
	if ((self->par<-9.0) || (self->par >= 98.0))
		{
			xpn->pCl->pWeather->fPAR = xpn->pCl->pWeather->fSolRad / 2.0;//CalculateActiveRadiation(xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay, self->global_radiation ,xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->iJulianDay);
		}
	else
		{
			xpn->pCl->pWeather->fPAR = get_SolRad(self->par,xpn->pTi->pSimTime->fTimeY, xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->fTimeDay, xpn->pTi->pTimeStep->fAct);
		}
	//xpn->pCl->pWeather->fTempAir=db_get_double(data_model,9); // mittlere Lufttemperatur
	xpn->pCl->pWeather->fTempAir = calculateAirTemperature(xpn->pTi->pSimTime->fTimeDay,self->Tairmin,self->Tairmax,self->TairLastDayMax,self->TairNextDayMin,(double)xpn->pLo->pFarm->fLatitude,xpn->pTi->pSimTime->fTimeY);
	
	
//Added by Hong on 20180724: in order to use the daily input from weather data for SurfaceMiner()
	xpn->pCl->pWeather->fTempMax =self->Tairmax;
	xpn->pCl->pWeather->fTempMin =self->Tairmin;
	xpn->pCl->pWeather->fTempAve =self->meantemp;
	xpn->pCl->pWeather->fGlobalStrahlung= self->global_radiation;
	//End of Hong
    
    //SG20190211:
   if((self->AtmCO2ppm>199.999)&&(self->AtmCO2ppm<2000.01))
        xpn->pCl->pWeather->fAtmCO2ppm =     self->AtmCO2ppm;
    //end of SG
	
	xpn->pCl->pWeather->fTempAir_zlvl = xpn->pCl->pWeather->fTempAir;
	if (xpn->pCl->fTempMeasHeight==0.0)
		{
			xpn->pCl->fTempMeasHeight=2.0;
		}
	if (xpn->pCl->fWindMeasHeight==0.0)
		{
			xpn->pCl->fWindMeasHeight=2.0;
		}
	//xpn->pCl->pWeather->fSolNet = expertn_database_help_func_get_Rn(xpn);
	/*laenge = calculateDayLength(xpn->pLo->pFarm->fLatitude,xpn->pTi->pSimTime->iJulianDay );
	C_DEBUG(laenge);*/
	// Write values for daily models without interpolation
	xpn->pCl->pWeather->fPreciRate_daily_models = self->precipitation;		// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fHumidity_daily_models = self->rel_himidity;
	xpn->pCl->pWeather->fWindSpeed_daily_models = self->windspeed;
	xpn->pCl->pWeather->fSolRad_daily_models = xpn->pCl->pWeather->fSolRad;
	if (self->pressure >= -9998.0)
		{
			xpn->pCl->pWeather->fAtmPressure_daily_models = self->pressure;
		}
	//xpn->pCl->pWeather->fTempAir=db_get_double(data_model,9); // mittlere Lufttemperatur
	xpn->pCl->pWeather->fTempAir_daily_models = xpn->pCl->pWeather->fTempAir;
}

//SG20190212
double Solar_radiation(expertn_modul_base *xpn, double sunshine_duration)
{
    //Calculation of global ration from sun shine duration
    //(following FAO56 guideline)
    
//	expertn_modul_base *xpn = &(self->parent);

//	PWEATHER  		pWE = xpn->pCl->pWeather;
	PTIME			pTi = xpn->pTi;


//	double	fP, flamda, , fra, frc, ;

	double   fJ, fdr, fdelta, fphi, ftemp, fomegas;
	double	fRa, fN, fn;
	double   fas=(double)0.25, fbs=(double)0.5;
	
    double DailyGlobalRadiation;

	if(NewDay(pTi)) {

		fJ = (double)pTi->pSimTime->fTimeY;
	
		fdr = (double)1 + (double)0.033*(double)cos((double)0.0172*fJ);	       //equation (21)

		fdelta = (double)0.409*(double)sin((double)0.0172*fJ - (double)1.39);  //equation (22)
		fphi   = xpn->pLo->pFarm->fLatitude*(double)PI/(double)180;

		ftemp = -(double)tan(fphi)*(double)tan(fdelta);					   //equation (20)
		if(     ftemp <= (double)-1)	fomegas = (double)PI;
		else if(ftemp >= (double)1 )	fomegas = (double)0;
		else						fomegas = (double)acos(ftemp);

		fRa = (double)37.6*fdr*(fomegas*(double)sin(fphi)*(double)sin(fdelta) //equation (19)
		                        + (double)cos(fphi)*(double)cos(fdelta)*(double)sin(fomegas));
		fN  = (double)24/(double)PI * fomegas;							   //equation (25)


			if (sunshine_duration >= (double)0) {
				// Berechnung der aktuellen Globalstrahlung //equation (52)
				fn = sunshine_duration;
				DailyGlobalRadiation = fRa*(fn/fN * fbs + fas);
			}
    }
return DailyGlobalRadiation;
}

    //end of SG
	