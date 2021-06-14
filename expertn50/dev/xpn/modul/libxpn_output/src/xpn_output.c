//      xpn_output.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "xpn_output.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "xpn_output_macros.h"

#ifndef S_IRWXU
#define S_IRWXU 1
#endif

G_DEFINE_TYPE(xpn_output, xpn_output, EXPERTN_MODUL_BASE_TYPE);


static void xpn_output_class_init(xpn_outputClass *klass) {}


static void xpn_output_init(xpn_output *self)
{
//	gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
// print Output Message if Modul is used from Expert N

	self->soil_numbers=0;

}

#define C_DEBUG(var) \
	xpn_register_var_add_double(self->parent.pXSys->var_list,var,var,__LINE__,TRUE);


static double divide_spec(double x,double y)
{
	if ((x==0.0) || (y==0.0)) return 0.0;
	return x/y;
}


// Our Modul Functions:

int xpn_output_reg_var(xpn_output *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PHLAYER pHLayer;
	PWLAYER pWLayer;
//	PCLAYER pCL;
	char *txtoutputfile;
	char *S;
	char *OutputPath;


	int i;

	// Timestep for output:

	self->parent.pXSys->var_list->txtfile=NULL;

	// Output Path:
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.output text path");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.output text path");
		}

	if (S==NULL)
		{
			OutputPath=g_strdup_printf("$PROJECT_PATH/output");
		}
	else
		{
			OutputPath=g_strdup_printf("%s",S);
		}





	// Output Time Step:
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.output time step");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.output time step");
			if (S==NULL)
				{
					S = "1.0";
					fprintf(stderr,"ERROR: Varname is not defined (%s or %s), take default value (%s)!\n","Config.XPN_OUTPUT.output time step","Global_Config.options.output time step",S);
				}
		}

	output_time_step_check_init(&(self->output_time_step),&(self->output_time_step_string),S);
	
	// time span of output:	
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.output time span");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.output time span");			
		}
	expertn_modul_base_init_timespan(&(self->timespan),S);
	
	//struct_timespan

	//  VarList:
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.varlist");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.varlist");
			if (S==NULL)
				{
					PRINT_ERROR("Config.XPN_OUTPUT.varlist or Global_Config.options.varlist is not configured: No OutputFile ist generated!");
					free(self->parent.pXSys->var_list->txtfile);
					self->parent.pXSys->var_list->txtfile=NULL;
					self->varlist=NULL;
				}
		}

	if (S!=NULL)
		{
			char *S2;
			S2 = expertn_modul_base_replace_std_templates(xpn,S);
			if (S2!=NULL)
				{
					self->varlist = get_fullpath_from_relative(self->parent.pXSys->base_path,S2);
					free(S2);
				}
		}

	self->bno_txt_output=0;



	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.no output txt file");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.no output txt file");
		}
	if (S!=NULL)
		{
			if ((atoi(S)==1) && (self->parent.pXSys->var_list->txtfile!=NULL))
				{
					free(self->parent.pXSys->var_list->txtfile);
					self->parent.pXSys->var_list->txtfile=NULL;
				}
		}

	if (S==NULL)
		{
			self->bno_txt_output=0;
		}
	else
		{
			self->bno_txt_output=atoi(S);
		}

	self->parent.pXSys->var_list->create_xno_files=1;
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.create xno files");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.create xno files");
		}
	if (S!=NULL)
		{
			if (atoi(S)==1)
				{
					self->parent.pXSys->var_list->create_xno_files=1;
				}
		}

// Pfad erzeugen falls nicht vorhanden
	S=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,OutputPath);
	g_mkdir_with_parents(S,S_IRWXU);
	free(S);

	if (self->parent.pXSys->var_list->create_xno_files==1)
		{
			txtoutputfile = g_strdup_printf("%s/%s_%s.xno",OutputPath,self->parent.pXSys->project_name,self->parent.pXSys->reg_str);
		}
	else
		{
			txtoutputfile = g_strdup_printf("%s/%s_%s.cdb",OutputPath,self->parent.pXSys->project_name,self->parent.pXSys->reg_str);
		}


	if (self->bno_txt_output==0)
		{
			self->parent.pXSys->var_list->txtfile=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,txtoutputfile);
		}
	else
		{
			self->parent.pXSys->var_list->txtfile=NULL;
		}
	g_free(OutputPath);
	g_free(txtoutputfile);







	/* ALLOCATE MEMORY */


	if (self->soil_numbers > self->parent.pSo->iLayers)
		{
			self->soil_numbers = self->parent.pSo->iLayers;
		}


	self->c_n_ratio_litter = g_malloc0_n(self->parent.pSo->iLayers,sizeof(double));
	self->c_n_ratio_manure = g_malloc0_n(self->parent.pSo->iLayers,sizeof(double));
	self->c_n_ratio_humus = g_malloc0_n(self->parent.pSo->iLayers,sizeof(double));
	self->wfps = g_malloc0_n(self->parent.pSo->iLayers,sizeof(double));
    self->RLD = g_malloc0_n(self->parent.pSo->iLayers,sizeof(double));


	// Heat:
	self->heat.soil_temp_mean = (double*)calloc(sizeof(double),self->soil_numbers);
	self->heat.soil_temp_calc = (double*)calloc(sizeof(double),self->soil_numbers);

	self->APHLayer = (PHLAYER *)calloc(sizeof(PHLAYER),self->parent.pSo->iLayers);
	for (i=0,pHLayer=self->parent.pHe->pHLayer; i<self->parent.pSo->iLayers; i++,pHLayer=pHLayer->pNext)
		{
			self->APHLayer[i] = pHLayer;
		}

	self->timesave=0;
	self->bWriteAverage=FALSE;


	// WATER:
	self->APWLayer = (PWLAYER *)calloc(sizeof(PWLAYER),self->parent.pSo->iLayers);
	for (i=0,pWLayer=self->parent.pWa->pWLayer; i<self->parent.pSo->iLayers; i++,pWLayer=pWLayer->pNext)
		{
			self->APWLayer[i] = pWLayer;
		}

	self->water.water_content =  (double*)calloc(sizeof(double),self->soil_numbers);
	self->water.matric_pot =  (double*)calloc(sizeof(double),self->soil_numbers);
	self->water.hydraulic_conduct =  (double*)calloc(sizeof(double),self->soil_numbers);
	self->water.hydraulic_conduct_calc =  (double*)calloc(sizeof(double),self->soil_numbers);
	self->water.FluxDens_Day = (double*)calloc(sizeof(double),self->soil_numbers);
	self->water.FluxDensZwischen_Day = (double*)calloc(sizeof(double),self->soil_numbers);

	// PLANT
	//self->RLD = (double*)calloc(sizeof(double),self->soil_numbers);

	// NITROGEN
	//self->no3_content = (double*)calloc(sizeof(double),self->soil_numbers);
	//self->nh4_content = (double*)calloc(sizeof(double),self->soil_numbers);

//=================================================================================================

	self->dt_sum_count= 0.0;

	self->water.precip_mean_calc = 0.0;

	// climate
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.DayMeanTemp,"output.Climate.Climate.Air Mean Temperature [degC]",0.0);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Temperature and Humidity.Air Temperature [deg C]",&(xpn->pCl->pWeather->fTempAir),-1,TRUE,TRUE);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.relHum,"output.Climate.Temperature and Humidity.Relative Humidity [Percent]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.precip_day,"output.Climate.Precipitation.Precipitation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.canopyprecip_day,"output.Climate.Precipitation.Canopy Precipitation [mm]",0.0);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Precipitation.Snow [mm]",&(xpn->pCl->pWeather->fSnow),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Radiation.Solar Radiation [MJ/(m2 day)]",&(xpn->pCl->pWeather->fSolRad),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Radiation.PAR [MJ/(m2 day)]",&(xpn->pCl->pWeather->fPAR),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Wind Speed.Wind Speed [m/s]",&(xpn->pCl->pWeather->fWindSpeed),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Radiation Day.Solar Radiation [MJ/m2]",&(self->fSolRadDay),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Radiation Day.PAR [MJ/m2]",&(self->fPARDay),-1,TRUE,TRUE);
    //xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Radiation.DTR [J m-2 s-1]",&(xpn->pCl->pWeather->fDTR),-1,TRUE,TRUE);//Hong for GECROS
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Climate.Radiation.DTR2 [J m-2 s-1]",&(xpn->pCl->pWeather->fDTR2),-1,TRUE,TRUE);//Hong for GECROS
	// --------------------------------------------------------------------------------------------------------
	// definition of special outputs, which fits to the measurement values:

	self->special_output_def_file=NULL;
	self->spec_vars_count=0;
	self->spec_vars=NULL;

	self->spec_vars_profile_count=0;
	self->spec_vars_profile=NULL;

	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.XPN_OUTPUT.special_output_def");
	if (S==NULL)
		{
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.special_output_def");
		}
	if (S!=NULL)
		{
			self->special_output_def_file = expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,S);
		}

	if (self->special_output_def_file!=NULL)
		{
			xpn_output_load_special_output_def_file(self);
		}

	// ... und registrieren:
	if (self->spec_vars!=NULL)
		{
			for (i=0; i<self->spec_vars_count; i++)
				{
					if (self->spec_vars[i].name!=NULL)
						{
							xpn_register_var_add_pdouble(self->parent.pXSys->var_list,self->spec_vars[i].name,self->spec_vars[i].link_to_var,-1,TRUE,TRUE);
						}
				}
		}
	if (self->spec_vars_profile!=NULL)
		{
			for (i=0; i<self->spec_vars_profile_count; i++)
				{
					if (self->spec_vars_profile[i].name!=NULL)
						{
							xpn_register_var_add_pdouble(self->parent.pXSys->var_list,self->spec_vars_profile[i].name,&(self->spec_vars_profile[i].var),-1,TRUE,TRUE);
						}
				}
		}

	// --------------------------------------------------------------------------------------------------------
	// temporal changes
	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.water_content[i],varname,0.0),"output.Water.Temporal Changes.Water_Content [Vol-Percent]",self->soil_numbers);

	// matric potential
	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.matric_pot[i],varname,0.0),"output.Water.Matric Potential.Matric_Potential [kPa]",self->soil_numbers);

    //FH 2019-07-11: moved from balance.c
	self->cum_dust1=0.0;
	self->cum_dust2=0.0;
	self->cum_dust3=0.0;
	self->cum_dust4=0.0;
    self->cum_dust5=0.0;


	// Evapotranspiration (ET)
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potET_Day,"output.Water.Evapotranspiration Day.Potential Evapotranspiration  [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actET_Day,"output.Water.Evapotranspiration Day.Actual Evapotranspiration [mm]",0.0);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potE_Day,"output.Water.Evaporation and Transpiration Day.Potential Evaporation [mm]",0.0);	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actE_Day,"output.Water.Evaporation and Transpiration Day.Actual Evaporation [mm]",0.0);	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potT_Day,"output.Water.Evaporation and Transpiration Day.Potential Transpiration [mm]",0.0);	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actT_Day,"output.Water.Evaporation and Transpiration Day.Actual Transpiration [mm]",0.0);


	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Water.Evapotranspiration Rate.Potential Evapotranspiration  [mm/day]",&(xpn->pWa->fPotETR),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Water.Evapotranspiration Rate.Actual Evapotranspiration  [mm/day]",&(xpn->pWa->fActETR),-1,TRUE,TRUE);

	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Water.Evaporation and Transpiration Rate.Potential Evaporation  [mm/day]",&(xpn->pWa->pEvap->fPotR),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Water.Evaporation and Transpiration Rate.Actual Evaporation  [mm/day]",&(xpn->pWa->pEvap->fActR),-1,TRUE,TRUE);

	self->fPotTranspR = 0.0;
    self->fActTranspR = 0.0;
    xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Water.Evaporation and Transpiration Rate.Potential Transpiration  [mm/day]",&(self->fPotTranspR),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Water.Evaporation and Transpiration Rate.Actual Transpiration [mm/day]",&(self->fActTranspR),-1,TRUE,TRUE);
    
    

	//Water Flux Density
	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.FluxDens_Day[i],varname,0.0),"output.Water.Water_Flux.Water_Flux_Day [mm]",self->soil_numbers);

	// rates
	/*xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.precip_mean,"output.XWater.Rates.Precipitation [mm d-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potETR,"output.XWater.Rates.pot Evapotransp [mm d-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potER,"output.XWater.Rates.pot Evap [mm d-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potTR,"output.XWater.Rates.pot Transp [mm d-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.EIR,"output.XWater.Rates.Interception [mmd-1]",0.0);
	*/

	// dt
	/*
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.precip_dt,"output.XWater.Precipitation.P_dt [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potET_dt,"output.XWater.Evapotranspiration.pot ET_dt [mmdt-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potE_dt,"output.XWater.Evaporation.pot E_dt [mmdt-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potT_dt,"output.XWater.Transpiration.pot T_dt [mmdt-1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.EI_dt,"output.XWater.Interception.EI_dt [mmdt-1]",0.0);
	*/

	/* Subdirectory XHeat */
	// Air Temperature
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.DayMeanTemp,"output.Heat.Air Temperature.T_air [degC]",0.0);
	// Soil Temperature
	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.soil_temp_mean[i],varname,0.0),"output.Heat.Soil Temperature.T_soil [degC]",self->soil_numbers);
	/*
	// latent heat
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.latentHeat,"output.Heat.Heat Flux.Latent_Heat [Wm-2]",0.0);
	// sensible heat
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.sensibleHeat,"output.Heat.Heat Flux.Sensible_Heat [Wm-2]",0.0);
	// net radiation
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.netRadiation,"output.Heat.Heat Flux.Net_Radiation [Wm-2]",0.0);
	// net radiation
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.solRadiation,"output.Heat.Heat Flux.Sol_Radiation [Wm-2]",0.0);
	 */


	/* Subdirectory nitrogen */



	// Cum. N Transformation:
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNMinerCum,"output.Nitrogen.N Transformations Cumulative.Mineralization [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNO3DenitCum,"output.Nitrogen.N Transformations Cumulative.Denitrifcation [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNH4NitrCum,"output.Nitrogen.N Transformations Cumulative.Nitrification [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNImmobCum,"output.Nitrogen.N Transformations Cumulative.Immobilization [kg/ha]",0.0);
    
	// N Contents in Soil:
	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->no3_content[i],varname,0.0),"output.Nitrogen.NO3-N-Layer.NO3-N [kg/ha]",self->soil_numbers);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->no3profile,"output.Nitrogen.NO3-N-Profile.Soil Profile [kgNO3-N/ha]",0.0);

	// Temporal Changes
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->no3profile,"output.Nitrogen.N Temporal Changes Day.NO3 Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->no3lower23proflile,"output.Nitrogen.N Temporal Changes Day.NO3 lower 2/3 Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->no3lower13profile,"output.Nitrogen.N Temporal Changes Day.NO3 lower 1/3 Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->nh4profile,"output.Nitrogen.N Temporal Changes Day.NH4 Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->urealprofil,"output.Nitrogen.N Temporal Changes Day.Urea Profile [kg/ha]",0.0);

	// N Transformations
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fUreaHydroDay,"output.Nitrogen.N Transformations Day.Urea Hydrolysis [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NReleaseFrehOrgMatter,"output.Nitrogen.N Transformations Day.Mineralized N from Litter and Manure [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNHumusMinerDay,"output.Nitrogen.N Transformations Day.Mineralized N from Humus [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNH4NitrDay,"output.Nitrogen.N Transformations Day.Nitrified N [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNO3DenitDay,"output.Nitrogen.N Transformations Day.Denitrified N [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNImmobDay,"output.Nitrogen.N Transformations Day.Immobilized N [kg/ha]",0.0);

	// Mineralization/ Immobilization
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNHumusImmobDay,"output.Nitrogen.Mineralization/Immobilization Day.N Immobilization in Humus [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNManureImmobDay,"output.Nitrogen.Mineralization/Immobilization Day.N Immobilization in Manure [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNLitterImmobDay,"output.Nitrogen.Mineralization/Immobilization Day.N Immobilization in Litter [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNHumusMinerDay,"output.Nitrogen.Mineralization/Immobilization Day.N Release from Humus [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNManureMinerDay,"output.Nitrogen.Mineralization/Immobilization Day.N Release from Manure [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->fNLitterMinerDay,"output.Nitrogen.Mineralization/Immobilization Day.N Release from Litter [kg/ha]",0.0);
	
	//Test of Hong
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Daily Mineralization/Immobilization.fHumusMinerR",&(xpn->pCh->pCLayer->fHumusMinerR),-1,TRUE,TRUE);
	//End of Test
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Mineral/Immob.N fCFOMFast",&(xpn->pCh->pCLayer->fCFOMFast),-1,TRUE,TRUE);//Hong for NIMMOBR
	
	/* N Pools */
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NLitterProf,"output.Nitrogen.N Pools in Profile.N Litter in entire Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NManureProf,"output.Nitrogen.N Pools in Profile.N Manure in entire Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NHumusProf,"output.Nitrogen.N Pools in Profile.N Humus in entire Profile [kg/ha]",0.0);


/*	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NLitter,"output.Nitrogen.N Pools in Profile.N Litter [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NManure,"output.Nitrogen.N Pools in Profile.N Manure [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NHumus,"output.Nitrogen.N Pools in Profile.N Humus [kg/ha]",0.0);

	//C
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CLitter,"output.Carbon.C Pools in Profile.C Litter [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CManure,"output.Carbon.C Pools in Profile.C Manure [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CHumus,"output.Carbon.C Pools in Profile.C Humus [kg/ha]",0.0);*/
    
/*    // 0-30cm
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CLitter30,"output.Carbon.0 to 30 cm.C Litter 0-30 cm",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CManure30,"output.Carbon.0 to 30 cm.C Manure 0-30 cm",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CHumus30,"output.Carbon.0 to 30 cm.C Humus 0-30 cm",0.0);
	// 30-60cm
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CLitter60,"output.Carbon.30 to 60 cm.C Litter 30-60 cm kg/ha",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CManure60,"output.Carbon.30 to 60 cm.C Manure 30-60 cm kg/ha",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CHumus60,"output.Carbon.30 to 60 cm.C Humus 30-60 cm kg/ha",0.0);
	// 60-90cm
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CLitter90,"output.Carbon.60 to 90 cm.C Litter 60-90 cm kg/ha",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CManure90,"output.Carbon.60 to 90 cm.C Manure 60-90 cm kg/ha",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CHumus90,"output.Carbon.60 to 90 cm.C Humus 60-90 cm kg/ha",0.0);*/
	// profile
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CLitterProf,"output.Carbon.C Pools in Profile.C Litter in entire Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CManureProf,"output.Carbon.C Pools in Profile.C Manure in entire Profile [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CHumusProf,"output.Carbon.C Pools in Profile.C Humus in entire Profile [kg/ha]",0.0);

	// C - N Ratio:
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CNRatioLitter,"output.Carbon.C/N Ratio in Top Soil.C/N Ratio Litter 0-30 cm [-]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CNRatioManure,"output.Carbon.C/N Ratio in Top Soil.C/N Ratio Manure 0-30 cm [-]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->CNRatioHumus,"output.Carbon.C/N Ratio in Top Soil.C/N Ratio Humus 0-30 cm [-]",0.0);

	/* Subdirectory XWATER */




	/*
		// latent heat
		xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.cum_lat_heat,"output.Heat.Cum Heat Flux.Latent_Heat [Wm-2]",0.0);
		// sensible heat
		xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.cum_sen_heat,"output.Heat.Cum Heat Flux.Sensible_Heat [Wm-2]",0.0);
		// net heat
		xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.cum_net_rad,"output.Heat.Cum Heat Flux.Net_Radiation [Wm-2]",0.0);
		// sol rad
		xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.cum_sol_rad,"output.Heat.Cum Heat Flux.Sol_Radiation [Wm-2]",0.0);
		 */

	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Net Radiation.Net Shortwave Radiation [W/m2]",&(xpn->pHe->pHEBalance->fShortNetRad),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Net Radiation.Net Longwave Radiation [W/m2]",&(xpn->pHe->pHEBalance->fLongNetRad),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Net Radiation.Net Radiation [W/m2]",&(xpn->pHe->pHEBalance->fNetRad),-1,TRUE,TRUE);

//Alternative
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Net Radiation.Net Shortwave Radiation [W/m$^2\\!$]",&(xpn->pHe->pHEBalance->fShortNetRad),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Net Radiation.Net Longwave Radiation [W/m$^2\\!$]",&(xpn->pHe->pHEBalance->fLongNetRad),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Net Radiation.Net Radiation [W/m$^2\\!$]",&(xpn->pHe->pHEBalance->fNetRad),-1,TRUE,TRUE);


	// emissivity
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.emissivity,"output.Heat.Surface Properties.Emissivity [-]",0.0);
	// albedo
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->heat.Albedo,"output.Heat.Surface Properties.Albedo [-]",0.0);

	// Surface Temperature
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Surface Temperature.Air Temperature [deg C]",&(xpn->pCl->pWeather->fTempAir),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Surface Temperature.Surface Temperature [deg C]",&(xpn->pHe->fTempSurf),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Surface Temperature.Upper Soil Temperature [deg C]",&(xpn->pHe->pHLayer->pNext->fSoilTemp),-1,TRUE,TRUE);

    //Alternative
    //xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Surface Temperature.Air Temperature [$\\degree$C]",&(xpn->pCl->pWeather->fTempAir),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Surface Temperature.Surface Temperature [$\\degree$C]",&(xpn->pHe->fTempSurf),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Heat.Surface Temperature.Upper Soil Temperature [$\\degree$C]",&(xpn->pHe->pHLayer->pNext->fSoilTemp),-1,TRUE,TRUE);


	/* Subdirectory XPLANT */
	//Development Stage (Crops)
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->DevStage,"output.Plant.Development Stage.BBCH Stage [-]",0.0);

	// N and H2O uptake
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Nuptake,"output.Plant.N and Water Uptake.N Uptake [kgN/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->H2Ouptake,"output.Plant.N and Water Uptake.Water Uptake [mm]",0.0);

	//Masses
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RootMass,"output.Plant.Biomass.Roots [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->StemMass,"output.Plant.Biomass.Stem [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->BranchMass,"output.Plant.Biomass.Branches [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->LeafMass,"output.Plant.Biomass.Leaves [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->FruitMass,"output.Plant.Biomass.Fruits [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->StrawMass,"output.Plant.Biomass.Straw [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->AboveWoodMass,"output.Plant.Biomass.Wood above Ground [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->WoodMass,"output.Plant.Biomass.Wood including Gross Root [kg/ha]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->AboveMass,"output.Plant.Total Biomass.Above Ground [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RootMass,"output.Plant.Total Biomass.Roots [kg/ha]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->TotalMass,"output.Plant.Total Biomass.Total [kg/ha]",0.0);

	// Assimilate Pool
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->AssimilatePool,"output.Plant.Biomass.Assimilate Pool [kg/ha]",0.0);

	//LAI
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->LAI,"output.Plant.Leaf Area Index.LAI [m2/m2]",0.0);

	//Transpiration
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potT_Day,"output.Plant.Transpiration Day.Potential Transpiration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actT_Day,"output.Plant.Transpiration Day.Actual Transpiration [mm]",0.0);

	// Total Root Length Density
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->TotRLD,"output.Plant.Root Length Density in Profile.Total RLD [cm/cm3]",0.0);
	// Root Length Density in soil layers
    // FH 2019-07-18 Moved to special_output_dev
	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RLD[i],varname,0.0),"output.Plant.Root Length Density.RLD [cm/cm3]",self->soil_numbers);
	
	
	// Rooting depth
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->rooting_depth,"output.Plant.Rooting Depth.Depth [mm]",0.0);
	

	// N concentration
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NGrain,"output.Plant.N Concentration.N in Fruits [-]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NLeaves,"output.Plant.N Concentration.N in Leaves [-]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NStem,"output.Plant.N Concentration.N in Stem [-]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->NRoot,"output.Plant.N Concentration.N in Root [-]",0.0);

	//Height, Diameter, Density
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Height,"output.Plant.Height.Height [m]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Height,"output.XPlant.Diameter.diameter [m]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->PlantDens,"output.Plant.Plant Density.Plant_Density [Plants/ha]",0.0);



	//================================================================
	//intfSolRad

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->intfSolRad,"OUTPUT.Intfsolrad",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->intfPar,"OUTPUT.intfPar",0.0);

	// cumulative Evapotranspiration
	//Out-noted by hong on 20181220
	/*
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potET_Sum,"output.System Balance.Evapotranspiration Cum.Potential ET [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actET_Sum,"output.System Balance.Evapotranspiration Cum.Actual ET [mm]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potE_Sum,"output.System Balance.Evapotranspiration Cum.Potential Evaporation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actE_Sum,"output.System Balance.Evapotranspiration Cum.Actual Evaporation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potT_Sum,"output.System Balance.Evapotranspiration Cum.Potential Transpiration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actT_Sum,"output.System Balance.Evapotranspiration Cum.Actual Transpiration [mm]",0.0);
*/
	// cumulative water flows
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.precip_sum,"output.Water.Water Compartments Cumulative.Precipitation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potE_Sum,"output.Water.Water Compartments Cumulative.Potential Evaporation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actE_Sum,"output.Water.Water Compartments Cumulative.Actual Evaporation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actT_Sum,"output.Water.Water Compartments Cumulative.Actual Transpiration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.potT_Sum,"output.Water.Water Compartments Cumulative.Potential Transpiration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.EI_Sum,"output.Water.Water Compartments Cumulative.Interception Loss [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Runoff_Sum,"output.Water.Water Compartments Cumulative.Runoff [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Infiltration_Sum,"output.Water.Water Compartments Cumulative.Infiltration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Drain_Sum,"output.Water.Water Compartments Cumulative.Drainage [mm]",0.0);

	// cumulative water flows (day)
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.precip_day,"output.Water.Water Compartments Day.Precipitation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actE_Day,"output.Water.Water Compartments Day.Actual Evaporation [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.actT_Day,"output.Water.Water Compartments Day.Actual Transpiration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.EI_Day,"output.Water.Water Compartments Day.Interception Loss [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Runoff_Day,"output.Water.Water Compartments Day.Runoff [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Infiltration_Day,"output.Water.Water Compartments Day.Infiltration [mm]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Drain_Day,"output.Water.Water Compartments Day.Drainage [mm]",0.0);

	// soil water balance
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Balance,"output.Water.Water Storage.Water Storage in Profile [mm]",0.0); //reloacted to output.Water. by Hong on 20181218
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Input,"output.Water.Water Storage.Input [mm]",0.0);//reloacted to output.Water. by Hong on 20181218
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.Output,"output.Water.Water Storage.Output [mm]",0.0);//reloacted to output.Water. by Hong on 20181218
	
	      //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.WaterIce_Profile,"output.Water.Soil_Water_Balance.Water_Ice_in_Profile [mm]",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.BalanceXN,"output.Water.Soil_Water_Balance.Balance_XN [mm]",0.0);

//Added by Hong on 20181220
//N Balance
    //cumulative
/*    xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Nmin Input [kg N/ha]",&(xpn->pCh->pCBalance->dNInputCum),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Nmin from mineralization [kg N/ha]",&(xpn->pCh->pCProfile->dNMinerCum),-1,TRUE,TRUE);//added by Hong on 20190516
    xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.net Nmin (mineral_N-immob_N) [kg N/ha]",&(xpn->pCh->pCProfile->dNetNMinerCum),-1,TRUE,TRUE);//added by Hong on 20190606	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Nmin in Profile [kg N/ha]",&(xpn->pCh->pCBalance->dNProfile),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Nmin Uptake [kg N/ha]",&(xpn->pCh->pCProfile->dNUptakeCum),-1,TRUE,TRUE);
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.N2O Emission [kg N/ha]",&(xpn->pCh->pCProfile->dN2OEmisCum),-1,TRUE,TRUE);
    xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.NO Emission [kg N/ha]",&(xpn->pCh->pCProfile->dNOEmisCum),-1,TRUE,TRUE);//Added by Hong on 20190109
xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.N2 Emission [kg N/ha]",&(xpn->pCh->pCProfile->dN2EmisCum),-1,TRUE,TRUE);//Added by Hong on 20190109		
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.N Leaching [kg N/ha]",&(xpn->pCh->pCProfile->dNTotalLeachCum),-1,TRUE,TRUE);	
	xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Nmin Balance [kg N/ha]",&(xpn->pCh->pCBalance->dNBalance),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Cum NO3 Leaching [kg/ha]",&(xpn->pCh->pCProfile->fNO3LeachDay),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Cum NH4 Leaching [kg/ha]",&(xpn->pCh->pCProfile->fNH4LeachDay),-1,TRUE,TRUE);
	//xpn_register_var_add_pdouble(self->parent.pXSys->var_list,"output.Nitrogen.Nitrogen Cum.Cum Urea Leaching [kg/ha]",&(xpn->pCh->pCProfile->fUreaLeachDay),-1,TRUE,TRUE);
	*/
	//daily
	// xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.precip_day,"output.System Balance.Nitrogen Daily.N Leaching [kg N/ha]",0.0);
			
//End of Hong

    // FH 2019-07-11: moved from balance.c
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust1,"output.Atmospheric Transport.Dust Distribution.Dust 1 [ug/(kg dryair)]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust2,"output.Atmospheric Transport.Dust Distribution.Dust 2 [ug/(kg dryair)]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust3,"output.Atmospheric Transport.Dust Distribution.Dust 3 [ug/(kg dryair)]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust4,"output.Atmospheric Transport.Dust Distribution.Dust 4 [ug/(kg dryair)]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust5,"output.Atmospheric Transport.Dust Distribution.Dust 5 [ug/(kg dryair)]",0.0);

    //Alternative
   /* xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust1,"output.Atmospheric Transport.Dust Distribution.Dust 1 [$\\mu g$/(kg dryair)]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust2,"output.Atmospheric Transport.Dust Distribution.Dust 2 [$\\mu g$/(kg dryair)]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust3,"output.Atmospheric Transport.Dust Distribution.Dust 3 [$\\mu g$/(kg dryair)]",0.0);		
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust4,"output.Atmospheric Transport.Dust Distribution.Dust 4 [$\\mu g$/(kg dryair)]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_dust5,"output.Atmospheric Transport.Dust Distribution.Dust 5 [$\\mu g$/(kg dryair)]",0.0);*/


//	self->timesave_output_Year = 0;
//	self->timesave_output_TimeY = 0.0;
	self->output_first=1;

	self->xpn_time_old.msec = 0.0;
	self->xpn_time_old.sec = 0;
	self->xpn_time_old.min = 0;
	self->xpn_time_old.hour = 0;
	self->xpn_time_old.mday = 1;
	self->xpn_time_old.mon = 1;
	self->xpn_time_old.year = 0;
	self->xpn_time_old.is_leap_year = 0;




	return RET_SUCCESS;
}


int xpn_output_load_special_output_def_file(xpn_output *self)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	int i;
	double depth;
	double depth_from,depth_to;
	double act_depth;
	PSLAYER  pSL;
	PWLAYER  pWL;
	PHLAYER pHL;
	PCLAYER pCL;
    PLAYERROOT pLR;
	int iLayer;
	char *S;

	double *darray=NULL;
	int darray_len=0;
	double *darray_from=NULL;
	double *darray_to=NULL;
	int layer_from, layer_to;
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	PSWATER pSW = pSo->pSWater;
	int first;
	int count;


	char *filename=self->special_output_def_file;
	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}


	// water content:
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"water","water_content");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;

					for (pSL = pSo->pSLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext, pSW = pSW->pNext, pWL = pWL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pWL->fContAct);
									//self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Water.WC at Depth.$\\theta$ at %d cm [cm$^3\\!$/cm$^3\\!$]",(int)(depth*100.0+0.5));
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Water.Water Content at Depth.theta at %d cm [cm3/cm3]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}
	
		
		
	// water potential
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"water","matric_potential");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;

					for (pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pWL->fMatPotAct);
									//self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Water.Matric Potential at Depth.$\\psi$ at %d cm [mm]",(int)(depth*100.0+0.5));
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Water.Matric Potential at Depth.psi at %d cm [mm]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}
	
	// water filled pore space
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"water","wfps");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;

					for (pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(self->wfps[iLayer]);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Water.Water filled Pore Space at Depth.WFPS at %d cm [-]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}
	
	// soil temperature
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"heat","soil_temperature");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pHL->fSoilTemp);
									//self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Heat.Soil Temperature at Depth.T at %d cm [$\\degree$C]",(int)(depth*100.0+0.5));
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Heat.Soil Temperature at Depth.T at %d cm [deg C]",(int)(depth*100.0+0.5));
                                   if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	// Nitrogen
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","no3");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fNO3N);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Content.NO3 at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","nh4");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fNH4N);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Content.NH4 at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","urea");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fUreaN);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Content.Urea at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","n_litter");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fNLitter);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Pools at Depth.N Litter at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","n_manure");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fNManure);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Pools at Depth.N Manure at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","n_humus");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pSL->fNHumus);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Pools at Depth.N Humus at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

//Test of Hong
//fMinerR
GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","miner_rate");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fMinerR);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Mineralization Rate.Total at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}
//fHumusMinerR
GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","humus_miner_rate");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fHumusMinerR);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Mineralization Rate.In Humus at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}		

//fLitterMinerR
GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","litter_miner_rate");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fLitterMinerR);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Mineralization Rate.In Litter %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}	

//fManureMinerR			
GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"nitrogen","manure_miner_rate");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fManureMinerR);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Nitrogen.N Mineralization Rate.In Manure %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}			
//End of TEST



	// Carbon
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"carbon","c_litter");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fCLitter);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Carbon.C Pools at Depth.C Litter at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"carbon","c_manure");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pCL->fCManure);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Carbon.C Pools at Depth.C Manure at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"carbon","c_humus");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pSL->fCHumus);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Carbon.C Pools at Depth.C Humus at %d cm [kg/ha]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"carbon","c_n_ratio_litter");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (iLayer=1,pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(self->c_n_ratio_litter[iLayer]);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Carbon.C/N Ratio at Depth.C/N Ratio Litter at %d cm [-]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"carbon","c_n_ratio_manure");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (iLayer=1,pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(self->c_n_ratio_manure[iLayer]);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Carbon.C/N Ratio at Depth.C/N Ratio Manure at %d cm [-]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"carbon","c_n_ratio_humus");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (iLayer=1,pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(self->c_n_ratio_humus[iLayer]);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Carbon.C/N Ratio at Depth.C/N Ratio Humus at %d cm [-]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}
	
	//Plant
	GET_INI_DOUBLE_ARRAY_OPTIONAL(darray,darray_len,"plant","root_length_density");
	if (darray!=NULL)
		{
			for (i=0; i<darray_len; i++)
				{
					depth = darray[i];
					act_depth = 0.0;
					for (iLayer=1,pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext,pLR=xpn->pPl->pRoot->pLayerRoot->pNext, iLayer = 1;	//start
					        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL)&&(pLR->pNext != NULL));						//end
					        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, pLR=pLR->pNext, iLayer++)								//step
						{
							act_depth+=pSL->fThickness*1.0e-3; // mm -> m
							if (act_depth >= depth)
								{
									self->spec_vars_count++;
									self->spec_vars = g_realloc(self->spec_vars,self->spec_vars_count*sizeof(struct_special_var));
									self->spec_vars[self->spec_vars_count-1].link_to_var=&(pLR->fLengthDens);
									self->spec_vars[self->spec_vars_count-1].name=g_strdup_printf ("output.Plant.Root Length Density at Depth.RLD at %d cm [cm/cm3]",(int)(depth*100.0+0.5));
									if (5 <= xpn->DEBUG_LEVEL)
										{
											S=g_strdup_printf("%s is at layer %d",self->spec_vars[self->spec_vars_count-1].name,iLayer);
											PRINT_MESSAGE(xpn,5,S);
											g_free(S);
										}
									break;
								}
						}
				}
			g_free(darray);
		}

	//xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RLD[i],varname,0.0),"output.Plant.Root Length Density.RLD [cm/cm3]",self->soil_numbers);


	// Profiles:
	// water content profile:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"water","water_content_profile");
	WRITE_TO_PROFILE_VAR(pWL->fContAct,"output.Water.Water Content in Profile.theta %d-%d cm [cm3/cm3]",count);
    //WRITE_TO_PROFILE_VAR(pWL->fContAct,"output.Water.Water Content in Profile.$\\theta$ %d-%d cm [cm$^3\\!$/cm$^3\\!$]",count);
	
	// water potential profile:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"water","matric_potential_profile");
	WRITE_TO_PROFILE_VAR(pWL->fMatPotAct,"output.Water.Matric Potential in Profile.psi %d-%d cm [mm]",count);
	//WRITE_TO_PROFILE_VAR(pWL->fMatPotAct,"output.Water.Matric Potential in Profile.$\\psi$ %d-%d cm [mm]",count);
    
	// water filled pore space profile:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"water","wfps_profile");
	WRITE_TO_PROFILE_VAR(self->wfps[iLayer],"output.Water.Water filled Pore Space in Profile.WFPS %d-%d cm [-]",count);
	
	// soil temperature profile:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"heat","soil_temperature_profile");
	WRITE_TO_PROFILE_VAR(pHL->fSoilTemp,"output.Heat.Soil Temperature in Profile.T %d-%d cm [deg C]",count);
    //WRITE_TO_PROFILE_VAR(pHL->fSoilTemp,"output.Heat.Soil Temperature Profile.T %d-%d cm [$\\degree$C]",count);
	
//############################# NITROGEN ######################################	

	// Nitrogen profile no3:	
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"nitrogen","no3_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fNO3N,"output.Nitrogen.Nmin Content in Profile.NO3 %d-%d cm [kg/ha]",1.0);
	WRITE_TO_PROFILE_VAR(pCL->fNO3N,"output.Nitrogen.Nmin Content in Profile.NO3 %d-%d cm [kg/ha]",1.0);
    
	// Nitrogen profile nh4:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"nitrogen","nh4_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fNH4N,"output.Nitrogen.Nmin Content in Profile.NH4 %d-%d cm [kg/ha]",1.0);
	WRITE_TO_PROFILE_VAR(pCL->fNH4N,"output.Nitrogen.Nmin Content in Profile.NH4 %d-%d cm [kg/ha]",1.0);
    
	// Nitrogen profile urea:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"nitrogen","urea_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fUreaN,"output.Nitrogen.Nmin Content in Profile.Urea %d-%d cm [kg/ha]",1.0);
	WRITE_TO_PROFILE_VAR(pCL->fUreaN,"output.Nitrogen.Nmin Content in Profile.Urea %d-%d cm [kg/ha]",1.0);
    
	// litter and humus:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"nitrogen","n_litter_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fNLitter,"output.Nitrogen.N Pools in Profile.N Litter %d-%d cm [kg/ha]",1.0);
    WRITE_TO_PROFILE_VAR(pCL->fNLitter,"output.Nitrogen.N Pools in Profile.N Litter %d-%d cm [kg/ha]",1.0);
	
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"nitrogen","n_manure_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fNManure,"output.Nitrogen.N Pools in Profile.N Manure %d-%d cm [kg/ha]",1.0);
    WRITE_TO_PROFILE_VAR(pCL->fNManure,"output.Nitrogen.N Pools in Profile.N Manure %d-%d cm [kg/ha]",1.0);
	
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"nitrogen","n_humus_profile");
	//WRITE_TO_PROFILE_VAR(pSL->fNHumus,"output.Nitrogen.N Pools in Profile.N Humus  %d-%d cm [kg/ha]",1.0);
    WRITE_TO_PROFILE_VAR(pSL->fNHumus,"output.Nitrogen.N Pools in Profile.N Humus  %d-%d cm [kg/ha]",1.0);
//###################################### CARBON ######################################	
	// carbon:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"carbon","c_litter_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fCLitter,"output.Carbon.C Pools in Profile.C Litter %d-%d cm [kg/ha]",1.0);
	WRITE_TO_PROFILE_VAR(pCL->fCLitter,"output.Carbon.C Pools in Profile.C Litter %d-%d cm [kg/ha]",1.0);
    
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"carbon","c_manure_profile");
	//WRITE_TO_PROFILE_VAR(pCL->fCManure,"output.Carbon.C Pools in Profile.C Manure %d-%d cm [kg/ha]",1.0);
    WRITE_TO_PROFILE_VAR(pCL->fCManure,"output.Carbon.C Pools in Profile.C Manure %d-%d cm [kg/ha]",1.0);
	
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"carbon","c_humus_profile");
	//WRITE_TO_PROFILE_VAR(pSL->fCHumus,"output.Carbon.C Pools in Profile.C Humus %d-%d cm [kg/ha]",1.0);
    WRITE_TO_PROFILE_VAR(pSL->fCHumus,"output.Carbon.C Pools in Profile.C Humus %d-%d cm [kg/ha]",1.0);
	
	// r_c ratio:
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"carbon","c_n_ratio_litter_profile");
	//WRITE_TO_PROFILE_VAR(self->c_n_ratio_litter[iLayer],"output.Carbon.C N Ratio in Profile.Litter %d-%d cm [kg/ha]",count);
    WRITE_TO_PROFILE_VAR(self->c_n_ratio_litter[iLayer],"output.Carbon.C/N Ratio in Profile.C/N Ratio Litter %d-%d cm [-]",count);

	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"carbon","c_n_ratio_manure_profile");
	//WRITE_TO_PROFILE_VAR(self->c_n_ratio_manure[iLayer],"output.Carbon.C N Ratio in Profile.Manure %d-%d cm [kg/ha]",count);
    WRITE_TO_PROFILE_VAR(self->c_n_ratio_manure[iLayer],"output.Carbon.C/N Ratio in Profile.C/N Ratio Manure %d-%d cm [-]",count);
	
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"carbon","c_n_ratio_humus_profile");
	//WRITE_TO_PROFILE_VAR(self->c_n_ratio_humus[iLayer],"output.Carbon.C N Ratio in Profile.Humus %d-%d cm [kg/ha]",count);
    WRITE_TO_PROFILE_VAR(self->c_n_ratio_humus[iLayer],"output.Carbon.C/N Ratio in Profile.C/N Ratio Humus %d-%d cm [-]",count);

    // Hong: added for Scott Demyan:
	//[DAISY mineralisation]
	//C in Fresh organic matter Slow (AOM1) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_AOM1_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCFOMSlow,"output.DAISY mineralisation.C in Fresh organic matter Slow.C_AOM1_profile %d-%d cm [kg/ha]",1.0);

    //C in Fresh organic matter Fast (AOM2) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_AOM2_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCFOMFast,"output.DAISY mineralisation.C in Fresh organic matter Fast.C_AOM2_profile %d-%d cm [kg/ha]",1.0);
	
	//C in MicBiomSlow (BOM1) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_BOM1_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCMicBiomSlow,"output.DAISY mineralisation.C in MicBiomSlow.C_BOM1_profile %d-%d cm [kg/ha]",1.0);
	
	//C in MicBiomFast (BOM2) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_BOM2_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCMicBiomFast,"output.DAISY mineralisation.C in MicBiomFast.C_BOM2_profile %d-%d cm [kg/ha]",1.0);
	
	//C in MicBiomDenit (BOMD) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_BOMD_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCMicBiomDenit,"output.DAISY mineralisation.C in MicBiomDenit.C_BOMD_profile %d-%d cm [kg/ha]",1.0);
	
	//C in HumusSlow (SOM1) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_SOM1_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCHumusSlow,"output.DAISY mineralisation.C in HumusSlow.C_SOM1_profile %d-%d cm [kg/ha]",1.0);
	
	//C in HumusFast (SOM2) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_SOM2_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCHumusFast,"output.DAISY mineralisation.C in HumusFast.C_SOM2_profile %d-%d cm [kg/ha]",1.0);
	
	//C in HumusStable (SOM0) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","C_SOM0_profile");
	WRITE_TO_PROFILE_VAR(pCL->fCHumusStable,"output.DAISY mineralisation.C in HumusStable.C_SOM0_profile %d-%d cm [kg/ha]",1.0);
	
	
    //N in Fresh organic matter Slow (AOM1) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_AOM1_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNFOMSlow,"output.DAISY mineralisation.N in Fresh organic matter Slow.N_AOM1_profile %d-%d cm [kg/ha]",1.0);
	
	//N in Fresh organic matter Fast (AOM2) [kg/ha]
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_AOM2_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNFOMFast,"output.DAISY mineralisation.N in Fresh organic matter Fast.N_AOM2_profile %d-%d cm [kg/ha]",1.0);
	
	//N in MicBiomSlow (BOM1) (AOM2) [kg/ha]
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_BOM1_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNMicBiomSlow,"output.DAISY mineralisation.N in MicBiomSlow.N_BOM1_profile %d-%d cm [kg/ha]",1.0);
	
    //N in MicBiomFast (BOM2) (AOM2) [kg/ha]
	GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_BOM2_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNMicBiomFast,"output.DAISY mineralisation.N in MicBiomFast.N_BOM2_profile %d-%d cm [kg/ha]",1.0);
  
    //N in MicBiomDenit (BOMD) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_BOMD_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNMicBiomDenit,"output.DAISY mineralisation.N in MicBiomDenit.N_BOMD_profile %d-%d cm [kg/ha]",1.0); 
    
    //N in HumusSlow (SOM1) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_SOM1_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNHumusSlow,"output.DAISY mineralisation.N in HumusSlow.N_SOM1_profile %d-%d cm [kg/ha]",1.0);
	
	//N in HumusFast (SOM2) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_SOM2_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNHumusFast,"output.DAISY mineralisation.N in HumusFast.N_SOM2_profile %d-%d cm [kg/ha]",1.0);
	
    //N in HumusStable (SOM0) [kg/ha]
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"DAISY mineralisation","N_SOM0_profile");
	WRITE_TO_PROFILE_VAR(pCL->fNHumusStable,"output.DAISY mineralisation.N in HumusStable.N_SOM0_profile %d-%d cm [kg/ha]",1.0);

	//End of Hong

    //FH 2019-07-18 Added Root Length Density
    GET_INI_PROFILE_ARRAY_OPTIONAL(darray_from,darray_to,darray_len,"plant","root_length_density_profile");
    // 3rd varibale in WRITE_TO_PROFILE_VAR must be "count" for averaging over the layers, otherwise 1.0
    WRITE_TO_PROFILE_VAR(self->RLD[iLayer],"output.Plant.Root Length Density in Profile.RLD %d-%d cm [cm/cm3]",count);
    //printf("%d %d \n", count, pSo->iLayers);
	
	g_key_file_free(keyfile);
	return RET_SUCCESS;
}

int xpn_output_calc_var(xpn_output *self)
{
	double zwischen, zwischen2;	// zwischen3, zwischen4;
	double ContAct, Thickness;	// Matric, Cond,
	int startv,endv;
	int i,i2,count, count2;

	int iLayer;
	double TotSoilWater = 0.0;
	//double TotRLD;

	PCLAYER pCL;
	PSLAYER  pSL;
	PWLAYER  pWL;
	PLAYERROOT pLR;

	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
//	PCLIMATE pCl = xpn->pCl;
//	PHEAT pHe = xpn->pHe ;
//	PLOCATION pLo = xpn->pLo;
//	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PSWATER pSW = pSo->pSWater;

	double dt = pTi->pTimeStep->fAct;

	self->intfSolRad+=dt*xpn->pCl->pWeather->fSolRad;
	self->intfPar+=dt*xpn->pCl->pWeather->fPAR;

	for (i=0; i<self->soil_numbers; i++)
		{
			zwischen = 0.0;
			count = 0;
			startv= (int)(((double)self->parent.pSo->iLayers-2.0)/(double)self->soil_numbers*(double)i+1.0);
			endv= (int)(((double)self->parent.pSo->iLayers-2.0)/(double)self->soil_numbers*(double)(i+1)+1.0);


			for (i2=startv; i2<endv; i2++)
				{
					zwischen+=self->APHLayer[i2]->fSoilTemp;
					count++;
				}
			self->heat.soil_temp_mean[i] = zwischen/(double)count;
		}

	/* set daily variables to zero */
	if(NewDay(pTi))
		{

			self->water.precip_day = self->water.precip_zwischen;	// Precipitation
			self->water.canopyprecip_day = self->water.canopyprecip_zwischen;	// Canopy Precipitation
			self->water.actET_Day = self->water.evapotransp_zwischen;	// act. Evapotransp.
			self->water.actE_Day = self->water.evap_zwischen;		// act. Evaporation
			self->water.actT_Day = self->water.transp_zwischen;		// act. Transpiration
			self->water.EI_Day = self->water.intercept_zwischen;		// Interception Evaporation
			self->water.Infiltration_Day = self->water.infiltration_zwischen;		// Infiltration
			self->water.Runoff_Day = self->water.runoff_zwischen;			// Runoff
			self->water.Drain_Day = self->water.drain_zwischen;			// Drainage/Ground Water Leaching

			self->water.potET_Day = self->water.pot_evapotransp_zwischen;	// pot. Evapotransp.
			self->water.potE_Day = self->water.pot_evapo_zwischen;		// pot. Evaporation
			self->water.potT_Day = self->water.pot_transp_zwischen;		// pot. Transpiration

            self->fSolRadDay = self->fSolRadDay_zwischen;
            self->fPARDay = self->fPARDay_zwischen;


			// set zwischen values to zero
			self->water.precip_zwischen = 0.0;
			self->water.canopyprecip_zwischen = 0.0;
			self->water.intercept_zwischen = 0.0;
			self->water.evap_zwischen = 0.0;
			self->water.transp_zwischen = 0.0;
			self->water.evapotransp_zwischen = 0.0;
			self->water.infiltration_zwischen = 0.0;
			self->water.runoff_zwischen = 0.0;
			self->water.drain_zwischen = 0.0;

			self->water.pot_transp_zwischen = 0.0;
			self->water.pot_evapotransp_zwischen = 0.0;
			self->water.pot_evapo_zwischen = 0.0;

            self->fSolRadDay_zwischen = 0.0;
            self->fPARDay_zwischen = 0.0;

		}
	//===========================================================

	// Air Temperature
	self->heat.DayMeanTemp = self->parent.pCl->pWeather->fTempAir;

	// Humidity
	self->water.relHum = self->parent.pCl->pWeather->fHumidity;
	//ATTN: todo
	// http://www.cru.uea.ac.uk/cru/pubs/thesis/2007-willett/2INTRO.pdf page 34
	//self->water.specHum =

	// Precipitation
	self->water.precip_zwischen += (self->parent.pCl->pWeather->fBulkPreciRate*dt);				// zwischen value for visual reasons in cdb plot
	self->water.precip_sum += (self->parent.pCl->pWeather->fBulkPreciRate*dt);

	self->water.canopyprecip_zwischen += (self->parent.pCl->pWeather->fPreciRate*dt);
	self->water.canopyprecip_sum += (self->parent.pCl->pWeather->fPreciRate*dt);

     // FH 2019-07-11 moved from balance.c
	// Atmosphere dust:
	self->cum_dust1+=xpn->pCh->pAtmos->dustr1*dt;
	self->cum_dust2+=xpn->pCh->pAtmos->dustr2*dt;
	self->cum_dust3+=xpn->pCh->pAtmos->dustr3*dt;	
	self->cum_dust4+=xpn->pCh->pAtmos->dustr4*dt;
	self->cum_dust5+=xpn->pCh->pAtmos->dustr5*dt;


	// Evapotranspiration
	self->water.pot_evapotransp_zwischen += (self->parent.pWa->fPotETR*dt);
	self->water.potET_Sum += (self->parent.pWa->fPotETR*dt);

	self->water.evapotransp_zwischen += (self->parent.pWa->fActETR*dt);		// zwischen value for visual reasons in cdb plot
	self->water.actET_Sum += (self->parent.pWa->fActETR*dt);

	// Evaporation
	self->water.pot_evapo_zwischen += (self->parent.pWa->pEvap->fPotR*dt);
	self->water.potE_Sum += (self->parent.pWa->pEvap->fPotR*dt);

	self->water.evap_zwischen += (self->parent.pWa->pEvap->fActR*dt);		// zwischen value for visual reasons in cdb plot
	self->water.actE_Sum += (self->parent.pWa->pEvap->fActR*dt);

	// Transpiration
	self->water.pot_transp_zwischen += (self->parent.pPl->pPltWater->fPotTranspR*dt);
	self->water.potT_Sum += (self->parent.pPl->pPltWater->fPotTranspR*dt);

	self->water.transp_zwischen += (self->parent.pPl->pPltWater->fActTranspR*dt);
	self->water.actT_Sum += (self->parent.pPl->pPltWater->fActTranspR*dt);

	// Interception
	self->water.intercept_zwischen += (self->parent.pPl->pPltWater->fInterceptR*dt);
	self->water.EI_Sum += (self->parent.pPl->pPltWater->fInterceptR*dt);

	// Runoff
	self->water.runoff_zwischen += (self->parent.pWa->fRunOffR*dt);
	self->water.Runoff_Sum += (self->parent.pWa->fRunOffR*dt);

	// Infiltration
	self->water.infiltration_zwischen += (self->parent.pWa->fInfiltR*dt);
	self->water.Infiltration_Sum += (self->parent.pWa->fInfiltR*dt);

	// Drainage
	self->water.drain_zwischen += (self->parent.pWa->fPercolR*dt);
	self->water.Drain_Sum += (self->parent.pWa->fPercolR*dt);

    // Radiation
    self->fSolRadDay_zwischen += xpn->pCl->pWeather->fSolRad *dt;
    self->fPARDay_zwischen += xpn->pCl->pWeather->fPAR * dt;

	// Balance
	self->water.BalanceXN = self->parent.pWa->pWBalance->fBalance;

	self->water.Output = self->water.Drain_Sum + self->water.Runoff_Sum + self->water.actT_Sum + self->water.actE_Sum;
	self->water.Input = self->water.precip_sum;	//ATTN: interception is calculated but not yet considered as loss if it evaporates from the canopy!!!
	self->water.Balance = self->water.Input - self->water.Output;

	//----------------------------------------------
	// rates (only for debugging)
	/*self->water.precip_mean = self->parent.pCl->pWeather->fPreciRate;
	self->water.potETR = self->parent.pWa->fPotETR;
	self->water.actETR = self->parent.pWa->fActETR;
	self->water.potER = self->parent.pWa->pEvap->fPotR;
	self->water.actER = self->parent.pWa->pEvap->fActR;
	self->water.potTR = self->parent.pPl->pPltWater->fPotTranspR;
	self->water.actTR = self->parent.pPl->pPltWater->fActTranspR;
	self->water.EIR = self->parent.pPl->pPltWater->fInterceptR;
	self->water.RunoffR = self->parent.pWa->fRunOffR;
	self->water.InfiltrationR = self->parent.pWa->fInfiltR;
	self->water.DrainR = self->parent.pWa->fPercolR;	*/

	// dt variables (only for debugging)
	/*self->water.precip_dt = self->parent.pCl->pWeather->fPreciRate*dt;
	self->water.actE_dt = self->parent.pWa->pEvap->fActR*dt;
	self->water.actT_dt = self->parent.pPl->pPltWater->fActTranspR*dt;
	self->water.EI_dt = (self->parent.pPl->pPltWater->fInterceptR*dt);
	self->water.Runoff_dt = self->parent.pWa->fRunOffR*dt;
	self->water.Infiltration_dt = self->parent.pWa->fInfiltR*dt;
	self->water.Drain_dt = self->parent.pWa->fPercolR*dt; */


	//=========================================================================================

	//if (self->parent.pTi->pSimTime->iJulianDay!=self->timesave) // neuer Tag:
	if ((int)(self->parent.pTi->pSimTime->fTimeY)!=self->timesave)
		{
			//self->timesave=self->parent.pTi->pSimTime->iJulianDay;
			self->timesave=(int)(self->parent.pTi->pSimTime->fTimeY);

			// Heat:

			/*for (i=0; i<self->soil_numbers; i++) {
				self->heat.soil_temp_mean[i] = self->heat.soil_temp_calc[i];
				self->heat.soil_temp_calc[i]=0.0;
			}
			self->heat.DayMeanTemp = self->heat.DayMeanTempcalc;
			self->heat.DayMeanTempcalc=0.0;*/

			// Water:
			for (i=0; i<self->soil_numbers; i++)
				{

					self->water.hydraulic_conduct[i] = self->water.hydraulic_conduct_calc[i];
					self->water.hydraulic_conduct_calc[i] = 0.0;
				}
			//self->water.precip_mean = self->water.precip_mean_calc;
			self->water.precip_mean_calc = 0.0;
			self->dt_sum = self->dt_sum_count;
			self->dt_sum_count = 0.0;

			// Nitrogen:
			self->no3profile=0.0;
			self->nh4profile=0.0;
			self->urealprofil=0.0;
			for (i = 1,pCL=pCh->pCLayer->pNext; (pCL->pNext!=NULL); pCL=pCL->pNext,i++)
				{
					self->no3profile 	+= pCL->fNO3N;
					self->nh4profile 	+= pCL->fNH4N;
					/*pCB->fN2ONProfile 	+= pCL->fN2ON;
					pCB->fN2NProfile  	+= pCL->fN2N;*/
					self->urealprofil    += pCL->fUreaN;
					//pCB->fNxOyNProfile  	+= pCL->fNxOyN;

					//self->no3_content[i] = pCL->fNO3N;
				}

			/*no3profile= pCh->pCBalance->fNO3NProfile;
			 nh4profile=pCh->pCBalance->fNH4NProfile;
			 urealprofil= pCh->pCBalance->fUreaNProfile;*/
			for(i = 1,pCL=pCh->pCLayer->pNext; ((pCL->pNext!=NULL)&&(i < ceil(((((double)pSo->iLayers-2.0)/3.0))))); pCL=pCL->pNext,i++);
			for( self->no3lower23proflile    = 0.0; (pCL->pNext!=NULL); pCL=pCL->pNext)
				{
					self->no3lower23proflile    += pCL->fNO3N;
				}

			for(i = 1,pCL=pCh->pCLayer->pNext; ((pCL->pNext!=NULL)&&(i < ceil((2.0*((double)pSo->iLayers-2.0)/3.0)))); pCL=pCL->pNext,i++);
			for(self->no3lower13profile    = 0.0; (pCL->pNext!=NULL); pCL=pCL->pNext)
				{
					self->no3lower13profile    += pCL->fNO3N;
				} // for idummy




			// Cum. N Transformation:

			self->fNO3LeachCum=0.0;
			//fNMinerCum=0.0;
			self->fNO3DenitCum=0.0;
			self->fNH4NitrCum=0.0;
			self->fNImmobCum=0.0;

			for(i = 1,pCL=pCh->pCLayer->pNext; pCL->pNext!=NULL; pCL=pCL->pNext,i++)
				{
					//self->fNO3LeachCum+= pCL->fNO3LeachDay;
					//self->fNMinerCum+=(pCL->fHumusMinerR+pCL->fLitterMinerR+pCL->fManureMinerR) *pTi->pTimeStep->fAct;
					self->fNO3DenitCum+=pCL->fNO3DenitCum;
					self->fNH4NitrCum+=pCL->fNH4NitrCum;
					self->fNImmobCum+=pCL->fNImmobCum;
			
				}							
				
			//self->fNO3LeachCum = pCh->pCProfile->fNO3LeachCum;

			// N Transformations:
			self->fUreaHydroDay = pCh->pCProfile->fUreaHydroDay;
			self->fNHumusMinerDay  = pCh->pCProfile->fNHumusMinerDay;
			self->fNH4NitrDay = pCh->pCProfile->fNH4NitrDay;
			self->fNO3DenitDay = pCh->pCProfile->fNO3DenitDay;
			self->fNImmobDay = pCh->pCProfile->fNImmobDay;
			self->fNLitterMinerDay = pCh->pCProfile->fNLitterMinerDay;
			self->fNManureMinerDay = pCh->pCProfile->fNManureMinerDay;
			self->fNLitterImmobDay = pCh->pCProfile->fNLitterImmobDay;
			self->fNManureImmobDay = pCh->pCProfile->fNManureImmobDay;
			self->fNHumusImmobDay = pCh->pCProfile->fNHumusImmobDay;
			self->NReleaseFrehOrgMatter = pCh->pCProfile->fNLitterMinerDay + pCh->pCProfile->fNManureMinerDay;

			pCh->pCProfile->fUreaHydroDay = 0.0;
			pCh->pCProfile->fNHumusMinerDay  = 0.0;
			pCh->pCProfile->fNH4NitrDay = 0.0;
			pCh->pCProfile->fNO3DenitDay = 0.0;
			pCh->pCProfile->fNImmobDay = 0.0;
			pCh->pCProfile->fNLitterMinerDay = 0.0;
			pCh->pCProfile->fNManureMinerDay = 0.0;
			pCh->pCProfile->fNLitterImmobDay = 0.0;
			pCh->pCProfile->fNManureImmobDay = 0.0;
			pCh->pCProfile->fNHumusImmobDay = 0.0;
		}

	//C_DEBUG(self->dt_sum);
	// über den tag mitteln:
	for (i=0; i<self->soil_numbers; i++)
		{
			zwischen = 0.0;
			count = 0;
			startv= (int)(((double)self->parent.pSo->iLayers-2.0)/(double)self->soil_numbers*(double)i+1.0);
			endv= (int)(((double)self->parent.pSo->iLayers-2.0)/(double)self->soil_numbers*(double)(i+1)+1.0);


			for (i2=startv; i2<endv; i2++)
				{
					zwischen+=self->APHLayer[i2]->fSoilTemp;
					count++;
				}
			self->heat.soil_temp_calc[i]+=zwischen/(double)count * self->parent.pTi->pTimeStep->fAct;
		}

	self->heat.DayMeanTempcalc += self->parent.pCl->pWeather->fTempAir * self->parent.pTi->pTimeStep->fAct;


// Water:
	for (i=0; i<self->soil_numbers; i++)
		{
			startv= (int)(((double)self->parent.pSo->iLayers-2.0)/(double)self->soil_numbers*(double)i+1.0);
			endv= (int)(((double)self->parent.pSo->iLayers-2.0)/(double)self->soil_numbers*(double)(i+1)+1.0);

			// calculation of water content:
			zwischen = 0.0;
			zwischen2 = 0.0;
			ContAct = 0.0;
			Thickness = 0.0;

			count = 0;
			count2 = 1;

			/* old method: simple mean over the layers */
			//for (i2=startv; i2<endv; i2++) {
			//	zwischen += self->APWLayer[i2]->fContAct;
			//	count++;
			//}
			//self->water.water_content[i] = zwischen/(double)count;


			/* new method: for every soil coloum fraction (see self->soil_numbers), the water content is calculated */
			//
			// get to soil layer in right depth
			pSL = pSo->pSLayer->pNext;
			pLR = pPl->pRoot->pLayerRoot->pNext;
			while(count2 < startv)
				{
					pSL = pSL->pNext;
					pLR = pLR->pNext;
					count2++;
				}

			// calculate water content, matrice potential and hydraulic conductivity
			for (i2=startv; ((pSL->pNext != NULL)&&(i2<endv)); i2++)		// pointer mustn't become NULL!
				{
					//	zwischen += self->APWLayer[i2]->fContAct;

					zwischen2 += (self->APWLayer[i2]->fContMax*pSL->fThickness);	// sum max air filled porosity [mm]
					ContAct += (self->APWLayer[i2]->fContAct*pSL->fThickness);		// sum act water content [mm]
					Thickness += pSL->fThickness;									// sum thickness [mm]

					pSL = pSL->pNext;
					count++;
				}
			self->water.water_content[i] = (ContAct/Thickness)*100.0;

			// root length density
			zwischen = 0.0;
			count = 0.0;
			for (i2=startv; ((pLR->pNext != NULL)&&(i2<endv)); i2++)		// pointer mustn't become NULL!
				{
					zwischen += pLR->fLengthDens;		// Root length density

					pLR = pLR->pNext;
					count++;
				}
			//self->RLD[i] = zwischen;	// root length density depending on soil number

			// matrice potential:
			zwischen = 0.0;
			count = 0;
			for (i2=startv; i2<endv; i2++)
				{
					zwischen+=self->APWLayer[i2]->fMatPotAct;
					count++;
				}
			self->water.matric_pot[i] =zwischen/(double)count;

			// hydraulic conductivity:
			zwischen = 0.0;
			count = 0;
			for (i2=startv; i2<endv; i2++)
				{
					zwischen+=self->APWLayer[i2]->fHydrCond;
					count++;
				}
			self->water.hydraulic_conduct_calc[i]+=zwischen/(double)count * self->parent.pTi->pTimeStep->fAct;

			// flux density
			count = 0;
			for (i2=startv; i2<endv; i2++)
				{

					// is the same:
					//self->water.FluxDensZwischen_Day[i] += (self->APWLayer[i2]->fFluxDens*dt);
					self->water.FluxDensZwischen_Day[i] += (self->APWLayer[i2]->fFlux);

					count++;
				}
			if(NewDay(pTi))
				{
					self->water.FluxDens_Day[i] = self->water.FluxDensZwischen_Day[i];
					self->water.FluxDensZwischen_Day[i] = 0.0;
				}
		}

	// Total Soil Water and Total Root Length Density
	for (pSL = pSo->pSLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;	//start
	        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));						//end
	        pSL = pSL->pNext, pSW = pSW->pNext, pWL = pWL->pNext, iLayer++)								//step
		{
			//double fProfile;
			//double fProfileIce;
			//double fFluxDay;

			//fProfile    += pWL->fContAct * pSL->fThickness;
			//fProfileIce += pWL->fIce * pSL->fThickness / 1.1;
			//fFluxDay += pWL->fFluxDens  * dt;

			TotSoilWater += ((pWL->fContAct + (pWL->fIce/1.1))*pSL->fThickness);
		}
	self->water.WaterIce_Profile = TotSoilWater;


	self->water.precip_mean_calc += self->parent.pCl->pWeather->fPreciRate * self->parent.pTi->pTimeStep->fAct;

	self->dt_sum_count+=self->parent.pTi->pTimeStep->fAct;



// Cum. N Transformation:
	//for(i = 0,pCL=pCh->pCLayer; pCL->pNext!=NULL; pCL=pCL->pNext,i++)//Test of hong
		for(i = 0,pCL=pCh->pCLayer->pNext; pCL->pNext!=NULL; pCL=pCL->pNext,i++)
		{
			//self->fNMinerCum+=(pCL->fHumusMinerR+pCL->fLitterMinerR+pCL->fManureMinerR) *pTi->pTimeStep->fAct;
			self->fNMinerCum += (pCL->fMinerR)*pTi->pTimeStep->fAct;//Changed by Hong on 20190521
			
			pCL->fNO3DenitCum += pCL->fNO3DenitR * pTi->pTimeStep->fAct;
			pCL->fN2ODenitCum += pCL->fN2ODenitR * pTi->pTimeStep->fAct;
			pCL->fNH4NitrDay  += pCL->fNH4NitrR  * pTi->pTimeStep->fAct;
			pCL->fNH4NitrCum  += pCL->fNH4NitrR  * pTi->pTimeStep->fAct;
			pCL->fNH4ToN2OCum += pCL->fNH4ToN2OR * pTi->pTimeStep->fAct;
			pCL->fNImmobCum  += pCL->fNImmobR * pTi->pTimeStep->fAct;
									
		}
		
// N Transformations:
	for(i = 0,pCL=pCh->pCLayer,pWL=pWa->pWLayer; pCL->pNext!=NULL; pCL=pCL->pNext,pWL=pWL->pNext,i++)
		//for(i = 0,pCL=pCh->pCLayer->pNext,pWL=pWa->pWLayer->pNext; pCL->pNext!=NULL; pCL=pCL->pNext,pWL=pWL->pNext,i++)//Test of hongfor (SOIL_LAYERS1(pCL,pCL->pNext))
		{
			pCh->pCProfile->fUreaHydroDay +=  pCL->fUreaHydroR *pTi->pTimeStep->fAct;
			pCh->pCProfile->fNHumusMinerDay  += pCL->fHumusMinerR * pTi->pTimeStep->fAct;
			pCh->pCProfile->fNH4NitrDay += pCL->fNH4NitrR * pTi->pTimeStep->fAct;
			pCh->pCProfile->fNO3DenitDay += pCL->fNO3DenitR * pTi->pTimeStep->fAct;
			pCh->pCProfile->fNImmobDay += pCL->fNImmobR * pTi->pTimeStep->fAct;						
			pCh->pCProfile->fNLitterMinerDay +=  pCL->fLitterMinerR * pTi->pTimeStep->fAct;
			pCh->pCProfile->fNManureMinerDay += pCL->fManureMinerR * pTi->pTimeStep->fAct;
			pCh->pCProfile->fNLitterImmobDay +=  pCL->fNLitterImmobR * pTi->pTimeStep->fAct;
			//pCh->pCProfile->fNManureImmobDay  += pCL->fNLitterImmobR * pTi->pTimeStep->fAct; //Hong: incorrect!
			pCh->pCProfile->fNManureImmobDay  += pCL->fNManureImmobR * pTi->pTimeStep->fAct;
			pCh->pCProfile->fNHumusImmobDay +=pCL->fNHumusImmobR * pTi->pTimeStep->fAct;			

			/*		pCP->fUreaLeachDay += pWL->fFlux *
			                                   * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
			                                   / (float)2.0 / kgPhaTomgPsqm;
			    pCP->fNH4LeachDay  += pWL->fFlux
			                                   * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
			                                   / (float)2.0 / kgPhaTomgPsqm;
			    pCP->fNO3LeachDay  += pWL->fFlux
			                                  * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
			                                  / (float)2.0 / kgPhaTomgPsqm;*/
			// Variable noch nicht vorhanden
			//pCP->fN2ODrainDay  += pWL->fFlux
			//                               * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc)
			//                               * pCL->fN2OKh
			//                               / (float)2.0 / kgPhaTomgPsqm;
			/*
			 pCP->fUreaLeachCum += pWL->fFlux
			                                * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
			                                / (float)2.0 / kgPhaTomgPsqm;
			 pCP->fNH4LeachCum  += pWL->fFlux
			                                * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
			                                / (float)2.0 / kgPhaTomgPsqm;
			 pCP->fNO3LeachCum  += pWL->fFlux
			                               * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
			                               / (float)2.0 / kgPhaTomgPsqm;
			 pCP->fN2ODrainCum  += pWL->fFlux
			                                * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc)
			                                * pCL->fN2OKh
			                                / (float)2.0 / kgPhaTomgPsqm;*/

		}
		
//		pCh->pCProfile->fNO3LeachDay  += pWL->fFlux* (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)/ (double)2.0 / kgPhaTomgPsqm;
//		          self->fNO3LeachCum  += pWL->fFlux* (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)/ (double)2.0 / kgPhaTomgPsqm;
//        pCh->pCProfile->fNO3LeachDay  += pWL->fFlux* (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)/ (double)2.0 / 100;
//		          self->fNO3LeachCum  += pWL->fFlux* (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)/ (double)2.0 / 100;
        
		
		self->fNO3LeachCum  =pCh->pCProfile->dNO3LeachCum;//Hong added on 20171027
//======================================C - N Pools =============================================================
	self->CLitter = 0.0;
	self->CManure = 0.0;
	self->CHumus = 0.0;
	self->NLitter = 0.0;
	self->NManure = 0.0;
	self->NHumus = 0.0;

	self->CLitterProf = 0.0;
	self->CManureProf = 0.0;
	self->CHumusProf = 0.0;
	self->NLitterProf = 0.0;
	self->NManureProf = 0.0;
	self->NHumusProf = 0.0;

	self->CLitter30 = 0.0;
	self->CManure30 = 0.0;
	self->CHumus30 = 0.0;
	self->NLitter30 = 0.0;
	self->NManure30 = 0.0;
	self->NHumus30 = 0.0;

	self->CLitter60 = 0.0;
	self->CManure60 = 0.0;
	self->CHumus60 = 0.0;
	self->NLitter60 = 0.0;
	self->NManure60 = 0.0;
	self->NHumus60 = 0.0;

	self->CLitter90 = 0.0;
	self->CManure90 = 0.0;
	self->CHumus90 = 0.0;
	self->NLitter90 = 0.0;
	self->NManure90 = 0.0;
	self->NHumus90 = 0.0;

//old formulation
	for(i= 1,pCL = pCh->pCLayer->pNext,pSL = pSo->pSLayer->pNext;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)&&((double)pSL->fThickness*(double)i <= 300.0));
	        pSL=pSL->pNext,pCL=pCL->pNext,i++)
		{
			self->CLitter	  += pCL->fCLitter;
			self->CManure	  += pCL->fCManure;
			self->CHumus	  += pSL->fCHumus;
			self->NLitter	  += pCL->fNLitter;
			self->NManure	  += pCL->fNManure;
			self->NHumus	  += pSL->fNHumus;
		}    /* 0-300mm */


	for(i= 1,pCL = pCh->pCLayer->pNext,pSL = pSo->pSLayer->pNext;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL));
	        pSL=pSL->pNext,pCL=pCL->pNext,i++)
		{
			self->CLitterProf	  += pCL->fCLitter;
			self->CManureProf	  += pCL->fCManure;
			self->CHumusProf	  += pSL->fCHumus;
			self->NLitterProf	  += pCL->fNLitter;
			self->NManureProf	  += pCL->fNManure;
			self->NHumusProf	  += pSL->fNHumus;
		}    /* Profile */


// the following for loops are depending on the first loop!
// this loop makes the initializations, the others use this
	for(i= 1,pCL = pCh->pCLayer->pNext,pSL = pSo->pSLayer->pNext;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)&&((double)pSL->fThickness*(double)i <= 300.0));
	        pSL=pSL->pNext,pCL=pCL->pNext,i++)
		{
			self->CLitter30	  += pCL->fCLitter;
			self->CManure30	  += pCL->fCManure;
			self->CHumus30	  += pSL->fCHumus;
			self->NLitter30	  += pCL->fNLitter;
			self->NManure30	  += pCL->fNManure;
			self->NHumus30	  += pSL->fNHumus;
		}
	/* 0-300mm */

	for(;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)&&((double)pSL->fThickness*(double)i <= 600.0));
	        pSL=pSL->pNext,pCL=pCL->pNext,i++)
		{
			self->CLitter60	  += pCL->fCLitter;
			self->CManure60	  += pCL->fCManure;
			self->CHumus60	  += pSL->fCHumus;
			self->NLitter60	  += pCL->fNLitter;
			self->NManure60	  += pCL->fNManure;
			self->NHumus60	  += pSL->fNHumus;
		}
	/* 300-600mm */

	for(;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)&&((double)pSL->fThickness*(double)i <= 900.0));
	        pSL=pSL->pNext,pCL=pCL->pNext,i++)
		{
			self->CLitter90	  += pCL->fCLitter;
			self->CManure90	  += pCL->fCManure;
			self->CHumus90	  += pSL->fCHumus;
			self->NLitter90	  += pCL->fNLitter;
			self->NManure90	  += pCL->fNManure;
			self->NHumus90	  += pSL->fNHumus;
		}
	/* 600-900mm */
//=========================================================================================================




	/*self->CLitter  /= (double)(i);
	self->CManure  /= (double)(i);
	self->CHumus  /= (double)(i);
	self->NLitter  /= (double)(i);
	self->NManure  /= (double)(i);
	self->NHumus  /= (double)(i);*/

// C/N Ratio
	self->CNRatioLitter=0.0;
	if (self->NLitter > EPSILON)
		{
			self->CNRatioLitter = self->CLitter/self->NLitter;
		}

	self->CNRatioManure =0.0;
	if (self->NManure > EPSILON)
		{
			self->CNRatioManure = self->CManure/self->NManure;
		}

	self->CNRatioHumus=0.0;
	if (self->NHumus > EPSILON)
		{
			self->CNRatioHumus = self->CHumus/self->NHumus;
		}

	for(i= 1,pCL = pCh->pCLayer->pNext,pSL = pSo->pSLayer->pNext,pWL = pWa->pWLayer->pNext, pLR = xpn->pPl->pRoot->pLayerRoot->pNext;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)&&(pLR->pNext!= NULL)&&((double)pSL->fThickness*(double)i <= 300.0));
	        pSL=pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext, pLR = pLR->pNext,i++)
	/*for(i= 1,pCL = pCh->pCLayer->pNext,pSL = pSo->pSLayer->pNext,pWL = pWa->pWLayer->pNext;
	        ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)&&((double)pSL->fThickness*(double)i <= 300.0));
	        pSL=pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,i++)			*/
		{
			self->c_n_ratio_litter[i] = divide_spec(pCL->fCLitter,pCL->fNLitter);
			self->c_n_ratio_manure[i] = divide_spec(pCL->fCManure,pCL->fNManure);
			self->c_n_ratio_humus[i] = divide_spec(pSL->fCHumus,pSL->fNHumus);
			self->wfps[i] = divide_spec(pWL->fContAct,pSL->fPorosity);
            //FH 2019-07-18 for correct profile calculation
            self->RLD[i] = pLR->fLengthDens;
		}

	/* XHeat */
	self->heat.Albedo = xpn->pSo->fAlbedo;	// [-]
	self->heat.emissivity = xpn->pHe->fEmissi;		// []

	self->heat.latentHeat = xpn->pHe->pHEBalance->fHeatLatent;	// [W/m2]
	self->heat.sensibleHeat = xpn->pHe->pHEBalance->fHeatSens;	// [W/m2]

	self->heat.netRadiation = xpn->pCl->pWeather->fSolNet/86400.0*1.0e6; // [W/m2]
	self->heat.solRadiation = xpn->pCl->pWeather->fSolRad/86400.0*1.0e6; // [W/m2]

	self->heat.cum_lat_heat += xpn->pHe->pHEBalance->fHeatLatent*dt;
	self->heat.cum_sen_heat += xpn->pHe->pHEBalance->fHeatSens*dt;
	self->heat.cum_net_rad += xpn->pCl->pWeather->fSolNet/86400.0*1.0e6*dt;
	self->heat.cum_sol_rad += xpn->pCl->pWeather->fSolRad/86400.0*1.0e6*dt;

	/* XPlant */
	self->DevStage = xpn->pPl->pDevelop->fDevStage;

	self->Nuptake = (self->parent.pPl->pPltNitrogen->fCumActNUpt);		// [kgN/ha]
	self->H2Ouptake += (self->parent.pPl->pPltWater->fActTranspR*dt);

	self->RootMass = xpn->pPl->pBiomass->fRootWeight;
	self->LeafMass = xpn->pPl->pBiomass->fLeafWeight;
	self->StemMass = xpn->pPl->pBiomass->fStemWeight;
	self->BranchMass = xpn->pPl->pBiomass->fBranchWeight;
	self->FruitMass = xpn->pPl->pBiomass->fFruitWeight;
	self->WoodMass = xpn->pPl->pBiomass->fWoodWeight;
	self->AssimilatePool = xpn->pPl->pPltCarbon->fCAssimilatePool;
    
	self->StrawMass = self->LeafMass + self->StemMass; //Added by Hong on 20190206
	
	//self->AboveMass = self->LeafMass + self->StemMass + self->BranchMass + 
	//self->FruitMass + self->WoodMass;
	self->AboveMass = self->LeafMass + self->StemMass + self->BranchMass + 
	self->FruitMass; //Changed by EP and Hong on 20180705, self->WoodMass = self->StemMass + self->BranchMass + GroßßRoot;
	self->AboveWoodMass = self->StemMass + self->BranchMass; //Added by hong on 20190520 
	
	self->TotalMass = self->AboveMass + self->RootMass;

	self->LAI = xpn->pPl->pCanopy->fLAI;

    // FH 20190705 Added division by the soil layers because we want to have the root length density
	self->TotRLD = xpn->pPl->pRoot->fTotalLength /(pSo->iLayers-1);

	self->NGrain = xpn->pPl->pPltNitrogen->fFruitActConc;
	self->NLeaves = xpn->pPl->pPltNitrogen->fLeafActConc;
	self->NStem = xpn->pPl->pPltNitrogen->fStemActConc;
	self->NRoot = xpn->pPl->pPltNitrogen->fRootActConc;

	self->PlantDens = xpn->pPl->pCanopy->fPlantDensity;
	self->Height	= xpn->pPl->pCanopy->fPlantHeight;
    
    self->rooting_depth = xpn->pPl->pRoot->fDepth;
    
    self->fActTranspR = xpn->pPl->pPltWater->fActTranspR;
    self->fPotTranspR = xpn->pPl->pPltWater->fPotTranspR;


// special output Profiles:
	for (i=0; i<self->spec_vars_profile_count; i++)
		{
			struct_special_var_profile *profile;
			profile = &(self->spec_vars_profile[i]);
			profile->var = 0.0;
			for (i2 = 0; i2 < profile->link_to_vars_count; i2++)
				{
					profile->var += *(profile->link_to_vars[i2]);
				}
			profile->var /= (double)profile->mul_fact;

		}



//====================================================================================================================================
// Varliste:

	if (self->varlist!=NULL)
		{
			xpn_register_var_add_filter_list_file(self->parent.pXSys->var_list,self->varlist);
			free(self->varlist);
			self->varlist=NULL;

			for (i=0; i<xpn->pXSys->var_list->size; i++)
				{
					if (xpn->pXSys->var_list->vars[i]->flag_pointer==TRUE)
						{
							// Wenn Variable nicht in der Liste --> wird nicht uebernommen:
							if ((( xpn->pXSys->var_list->vars[i]->vartype ==G_TYPE_INT) ||
							        ( xpn->pXSys->var_list->vars[i]->vartype ==G_TYPE_DOUBLE) ||
							        ( xpn->pXSys->var_list->vars[i]->vartype ==G_TYPE_FLOAT)) &&
							        (xpn_register_var_check_if_in_list(xpn->pXSys->var_list->str_filter,xpn->pXSys->var_list->str_filter_len,xpn->pXSys->var_list->vars[i]->varname)==0))
								{
									xpn->pXSys->var_list->vars[i]->flag_record=TRUE;
								}
							else
								{
									// C_DEBUG funktioniert sonst nicht
									xpn->pXSys->var_list->vars[i]->flag_record=FALSE;
								}
						}
				}
			xpn_register_var_clear_counter(xpn->pXSys->var_list);
		}
	else
		{

// OutputFile Schreiben:

			if ((expertn_modul_base_check_time_span(&(self->timespan),xpn->pTi)==0) && (output_time_step_compare_time_step(xpn->pTi,self->output_time_step,self->output_time_step_string,&(self->xpn_time_old),&(xpn->pXSys->var_list->xno_time))==0))
				{
					if (self->parent.pXSys->var_list->create_xno_files==0)
						{
							xpn->pXSys->var_list->fTime = xpn_time_conv_time_systime(&(xpn->pXSys->var_list->xno_time));
						}
					xpn_register_var_write_data(xpn->pXSys->var_list,self->output_first,xpn->pTi->pSimTime->bLastRound);
					self->output_first=0;
				}


			/*if (self->output_time_step==-1)
			{
				STRUCT_XPN_TIME xpn_time;
				xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
				if (self->xpn_time_old.hour != xpn_time.hour)
				{
					self->xpn_time_old = xpn_time;
					self->xpn_time_old.msec=0;
					self->xpn_time_old.sec=0;
				if (self->parent.pXSys->var_list->create_xno_files==1)
					{
						xpn->pXSys->var_list->xno_time = self->xpn_time_old;
					} else
					{
						xpn->pXSys->var_list->fTime = xpn_time_conv_time_systime(&self->xpn_time_old);
					}
				xpn_register_var_write_data(xpn->pXSys->var_list,self->output_first,xpn->pTi->pSimTime->bLastRound);
				self->timesave_output_Year = xpn->pTi->pSimTime->iyear;
				self->timesave_output_TimeY	= xpn->pTi->pSimTime->fTimeY;
				self->output_first=0;
				}
			} else
			if (self->output_time_step==-2)
			{
				STRUCT_XPN_TIME xpn_time;
				xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
				if (self->xpn_time_old.min/15 != xpn_time.min/15)
				{
					self->xpn_time_old = xpn_time;
					self->xpn_time_old.msec=0;
					self->xpn_time_old.sec=0;
				if (self->parent.pXSys->var_list->create_xno_files==1)
					{
						xpn->pXSys->var_list->xno_time = self->xpn_time_old;
					} else
					{
						xpn->pXSys->var_list->fTime = xpn_time_conv_time_systime(&self->xpn_time_old);
					}
				xpn_register_var_write_data(xpn->pXSys->var_list,self->output_first,xpn->pTi->pSimTime->bLastRound);
				self->timesave_output_Year = xpn->pTi->pSimTime->iyear;
				self->timesave_output_TimeY	= xpn->pTi->pSimTime->fTimeY;
				self->output_first=0;
				}
			} else
			{
			if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,self->timesave_output_Year,self->timesave_output_TimeY)>=0)
			 {
				STRUCT_XPN_TIME xpn_time;
				xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
				if (self->parent.pXSys->var_list->create_xno_files==1)
					{
						xpn->pXSys->var_list->xno_time = xpn_time;
					} else
					{
						xpn->pXSys->var_list->fTime = xpn_time_conv_time_systime(&xpn_time);
					}
				xpn_register_var_write_data(xpn->pXSys->var_list,self->output_first,xpn->pTi->pSimTime->bLastRound);
				self->timesave_output_Year = xpn->pTi->pSimTime->iyear;
				self->timesave_output_TimeY	= xpn->pTi->pSimTime->fTimeY;
				if (self->output_first==TRUE)
					{
						xpn_time_add_dt(&(self->timesave_output_Year),&(self->timesave_output_TimeY),self->output_time_step-EPSILON-xpn->pTi->pTimeStep->fAct);
					} else
					{
						xpn_time_add_dt(&(self->timesave_output_Year),&(self->timesave_output_TimeY),self->output_time_step-EPSILON);
					}
			} else {
				//xpn_register_var_clear_counter(xpn->pXSys->var_list);
			}
			self->output_first=0;
			}*/


		}
	return RET_SUCCESS;
}

int xpn_output_done(xpn_output *self)
{
	int i;
	// Heat:
	free(self->heat.soil_temp_mean);
	free(self->heat.soil_temp_calc);
	free(self->APHLayer);

	// Water:
	free(self->water.water_content);
	free(self->water.matric_pot);
	free(self->water.FluxDens_Day);
	free(self->water.FluxDensZwischen_Day);
	free(self->water.hydraulic_conduct);
	free(self->water.hydraulic_conduct_calc);
	free(self->APWLayer);

	//Nitrogen
	//free(self->no3_content);
	//free(self->nh4_content);

	//Plant
	//free(self->RLD);

	if (self->output_time_step_string!=NULL) g_free(self->output_time_step_string);
	self->output_time_step_string=NULL;

	if (self->special_output_def_file!=NULL)
		{
			free(self->special_output_def_file);
			self->special_output_def_file=NULL;
		}
	if (self->spec_vars!=NULL)
		{
			for (i=0; i<self->spec_vars_count; i++)
				{
					if (self->spec_vars[i].name!=NULL)
						{
							g_free(self->spec_vars[i].name);
							self->spec_vars[i].name=NULL;
						}
					self->spec_vars[i].link_to_var=NULL;
				}
			g_free(self->spec_vars);
			self->spec_vars=NULL;
			self->spec_vars_count=0;
		}

	if (self->spec_vars_profile!=NULL)
		{
			for (i=0; i<self->spec_vars_profile_count; i++)
				{
					if (self->spec_vars_profile[i].name!=NULL)
						{
							g_free(self->spec_vars_profile[i].name);
							self->spec_vars_profile[i].name=NULL;
						}
					if (self->spec_vars_profile[i].link_to_vars!=NULL)
						{
							g_free(self->spec_vars_profile[i].link_to_vars);
							self->spec_vars_profile[i].link_to_vars=NULL;
						}
				}
			g_free(self->spec_vars_profile);
			self->spec_vars_profile=NULL;
			self->spec_vars_profile_count=0;
		}

	if (self->c_n_ratio_litter!=NULL)
		{
			g_free(self->c_n_ratio_litter);
			self->c_n_ratio_litter=NULL;
		}
	if (self->c_n_ratio_manure!=NULL)
		{
			g_free(self->c_n_ratio_manure);
			self->c_n_ratio_manure=NULL;
		}
	if (self->c_n_ratio_humus!=NULL)
		{
			g_free(self->c_n_ratio_humus);
			self->c_n_ratio_humus=NULL;
		}
		
	if (self->wfps!=NULL)
		{
			g_free(self->wfps);
			self->wfps=NULL;
		}
        
	if (self->RLD!=NULL)
		{
			g_free(self->RLD);
			self->RLD=NULL;
		}
        
    expertn_modul_base_time_span_free(&(self->timespan));

	return RET_SUCCESS;
}
