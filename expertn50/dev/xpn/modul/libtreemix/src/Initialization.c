#include "Initialization.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "treemix_reg.h"
//#include "cmath_functions.h"
#include <xpn_register_var.h>



int libtreemix_load(libtreemix *self)
{   
    char *parameter_filename, *ini_filename;
    int i;  // species counting variable
    char *S;
    expertn_modul_base *xpn = &(self->parent);
    xpn_register_var *var_list;         
    var_list = xpn->pXSys->var_list;
    
    
    
    
    // Wenn die Init bereits geladen ist
    if(self->__INIT_DONE==0)
    {
        self->__INIT_DONE=1;
    }
    else
    {
        return RET_SUCCESS;
    }
    
    // Vars:
    
    // Ouputs auf 0 setzen:
    //memset(&(self->output),0,sizeof(P_Output));
    
    // Zunächst: Registry:  
    register_var(self);
    
    
    /* Read from INI File: */   
    S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.treemix.ini_filename");
        
    
    if(S == NULL)
    {
        PRINT_ERROR("Missing entry 'ini_filename' in paragraph [treemix] in .cfg-file");
    }   
    else
    {
        char *S2;
        S2 = expertn_modul_base_replace_std_templates(xpn,S);
        ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);       
        if (load_config(self, ini_filename)!=0)
        {
            PRINT_ERROR("Error Reading TREEMIX INI File");
        }
        free(ini_filename);
        free(S2);
    }
    
    
    /* READ FROM PARAMETER FILE: */     
    /* These function(s) return the Model Options, which can be set under control, management, etc in the GUI */
    //parameter_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.treemix.parameter_filename"); 
    //parameter_filename = xpn_register_var_get_pointer_convert_to_array_string(self->parent.pXSys->var_list,"Config.treemix.parameter_filename", &(self->conf.Species_Count));
    //if (parameter_filename==NULL)
    //{
    //  PRINT_ERROR("Missing entry 'treemix.parameter_filename' in your configuration!");
    //} 
    //else
    //{
    
    /* Allocate Memory */
    self->plant     = (P_Plant *)calloc( self->conf.Species_Count, sizeof( P_Plant) );
    self->silv      = (P_Silv *)calloc( self->conf.Species_Count, sizeof( P_Silv) );
    //self->clim        = (P_Clim *)calloc( 1, sizeof( P_Clim) );
    //memset(&(self->plant),0,sizeof(P_Plant));
    
    /* set XN variables to zero */
    xpn->pPl->pCanopy->fPlantDensity = 0.0;     // set total plant density to zero
    xpn->pPl->pCanopy->fLAI = 0.0;              // set LAI zero
    xpn->pPl->pPltNitrogen->fNStressPhoto = 1.0;
    
    for(i=0; i<self->conf.Species_Count; i++)
    {
        //parameter_filename = xpn_register_var_get_pointer_convert_to_array_string(self->parent.pXSys->var_list,"Config.treemix.parameter_filename", &(self->conf.Species_Count));             
        parameter_filename = get_fullpath_from_relative(self->parent.pXSys->base_path,self->conf.Parameter_Files[i] );
        if (load_parameter(self,parameter_filename, i)!=0)
        {
            PRINT_ERROR("Error Read TREEMIX PARAMETER File");
        }   
    }
    free(parameter_filename);
    
    //}
    
    /* Set Start Values */
    set_climate(self);  
    
    for(i=0; i<self->conf.Species_Count; i++)
    {
        set_start_values_plant(self, i);        
    }
    
    libtreemix_set_litter_pools(self);
    
    /* Switches for the Growth Model */
    xpn->pPl->pDevelop->bMaturity = (int)0;
    xpn->pPl->pDevelop->bPlantGrowth = (int)1;
    for(i=0; i<self->conf.Species_Count; i++)
    {
        self->silv[i].PlantGrowth = 1;
        self->silv[i].Maturity = 0;
        self->silv[i].Dormancy = 0;
    }   
    
    /* calling the macro for the model output */
    //self->mean_temp = xpn_register_var_get_pointer(xpn->pXSys->var_list,"expertn_database.MeanTemp");
    
    // here: example for possible dynamic initialization for an array
    // Water Temporal Change
    //xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->water.water_content[i],varname,0.0),"output.water.Temporal Changes.Water Content",self->soil_numbers);
    
    TREEMIX_REG
        
    return RET_SUCCESS;
}

int load_config(libtreemix *self,const char* configfile)
{
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    const char* filename=configfile;
    gsize count;
    //char *parameter_files;
    
    //allocate memory
    //self->conf = (P_Conf *)calloc( 1, sizeof( P_Conf) );
    memset(&(self->conf),0,sizeof(P_Conf));

    /* Create a new GKeyFile object and a bitwise list of flags. */
    keyfile = g_key_file_new ();

    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    //printf("%s \n", configfile);

    /* Load the GKeyFile from keyfile.conf or return. */
    if (!g_key_file_load_from_file (keyfile, configfile, flags, &error))
    {
            PRINT_ERROR(error->message);
            return -1;
    }
    
    // get [Configuration Variables]
    GET_INI_DOUBLE(self->conf.Species_Count,"Configuration","species_count");
    GET_INI_STRING_LIST(self->conf.Parameter_Files, "Configuration", "parameter_files", &(count));
    GET_INI_DOUBLE(self->conf.Operating,"Configuration","operating");//Added by Hong on 20180621
	//self->conf.Species_Count = (int)count;
    GET_INI_DOUBLE(self->conf.Photosynthesis,"Configuration","photosynthesis");
    GET_INI_DOUBLE(self->conf.Light_Interception,"Configuration","light_interception");
    GET_INI_DOUBLE(self->conf.PhotoRad,"Configuration","photosynthesis_radiation");
    GET_INI_DOUBLE(self->conf.PhotoResp,"Configuration","photosynthesis_respiration");
    GET_INI_DOUBLE(self->conf.Tree_Water,"Configuration","tree_water");
    GET_INI_DOUBLE(self->conf.Phenology,"Configuration","phenology");
    GET_INI_DOUBLE(self->conf.Respiration,"Configuration","respiration");
    GET_INI_DOUBLE(self->conf.Tree_Geometry,"Configuration","tree_geometry");
	GET_INI_DOUBLE(self->conf.Tree_fast_growing,"Configuration","geo_incr_fast_growing_tree");//added by Hong on 20180627
    GET_INI_DOUBLE(self->conf.Weather_Input,"Configuration","weather_input");
    GET_INI_DOUBLE(self->conf.Radiation,"Configuration","radiation");
    GET_INI_DOUBLE(self->conf.AirPressure,"Configuration","air_pressure");
    GET_INI_DOUBLE(self->conf.Soil_Processes,"Configuration","soil_processes");
    GET_INI_DOUBLE(self->conf.Rainfall_Interception,"Configuration","rainfall_interception");
    GET_INI_DOUBLE(self->conf.Nitrogen_Deposition,"Configuration","nitrogen_deposition");
    
    g_key_file_free(keyfile);

    return RET_SUCCESS;
}

int load_parameter(libtreemix *self,const char* paramfile, int i) 
{
    expertn_modul_base *xpn = &(self->parent);
    
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    const char* filename=paramfile;
    

    /* Create a new GKeyFile object and a bitwise list of flags. */
    keyfile = g_key_file_new ();

    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Load the GKeyFile from keyfile.conf or return. */
    if (!g_key_file_load_from_file (keyfile, paramfile, flags, &error))
    {
        PRINT_ERROR(error->message);
        return -1;
    }
    
    /* READ PARAMETERS*/
    // [Plant]
    GET_INI_STRING(self->plant[i].code,"Plant","code");
    //GET_INI_STRING(self->plant[i].name,"Plant","name");
    GET_INI_STRING(self->plant[i].type,"Plant","type"); 
    GET_INI_DOUBLE(self->plant[i].density,"Plant","density");
        xpn->pPl->pCanopy->fPlantDensity += self->plant[i].density; // summing up plant density     
    GET_INI_DOUBLE(self->plant[i].H,"Plant","height");
    
    /* for different calculations, the diameter at breast height is needed 
     * here the equation of Chojnacky (1999, technical note)
     * western journal of applied forestry, vol 14, no 1, january 1999 */
    GET_INI_DOUBLE(self->plant[i].D,"Plant","stem_diameter");
    //for pinyon pine (used for spruce)
    self->plant[i].DBH = -6.818+(1.022*(self->plant[i].D*100.0))+1.8879+1.8971+(-0.0339*(self->plant[i].D*100.0));  //[cm]
    self->plant[i].DBH = self->plant[i].DBH/100.0;
    if(self->plant[i].DBH <= 0.0)
    {
        self->plant[i].DBH = 0.01;
    }   
    
    GET_INI_DOUBLE(self->plant[i].CWdMass,"Plant","wood_mass");
    GET_INI_DOUBLE(self->plant[i].CLfMass,"Plant","leaf_mass");
    GET_INI_DOUBLE(self->plant[i].CFRtMass,"Plant","fine_root_mass");
    GET_INI_DOUBLE(self->plant[i].CFrMass,"Plant","fruit_mass");    
    GET_INI_DOUBLE(self->plant[i].CAss,"Plant","assimilates_wood");
    GET_INI_DOUBLE(self->plant[i].NWd,"Plant","nitrogen_wood");
    
    // [Morphology]
    
    /* for spruce: form factor is being calculated from HD-ratio 
     * according to Forschungszentrum Waldökosysteme, 1995 */
    GET_INI_INTEGER(self->plant[i].Form_var,"Morphology","form_fac_var");    
    //if(!strcmp(self->plant[i].code, "SP"))
    if(self->plant[i].Form_var == 1)
    {
        self->plant[i].Form = (self->plant[i].H/(self->plant[i].D*600.0)) + 0.32;
    }
    else
    {
        GET_INI_DOUBLE(self->plant[i].Form,"Morphology","form_factor");
    }
    
    /* different height and diameter increment calculations 
     * according toJansen et al. 1995 (Forschungszentrum Waldökosysteme, 1995) */
    if(self->conf.Tree_Geometry == 2)
    {
        GET_INI_DOUBLE(self->plant[i].Incr_p,"Morphology","Incr_p");
    }
    
    
    GET_INI_DOUBLE(self->plant[i].Dmax,"Morphology","Dmax");
    GET_INI_DOUBLE(self->plant[i].BrWdFrCoeff,"Morphology","branchwood_fraction_coeff");
    GET_INI_DOUBLE(self->plant[i].FiBrWdFr,"Morphology","final_branchwood_fraction");
    GET_INI_DOUBLE(self->plant[i].CWdDens,"Morphology","CWdDens");
    GET_INI_DOUBLE(self->plant[i].CLfDens,"Morphology","CLfDens");
    GET_INI_DOUBLE(self->plant[i].LightExt,"Morphology","light_extinction");
    GET_INI_DOUBLE(self->plant[i].LightFrac,"Morphology","min_light_fraction");
    GET_INI_DOUBLE(self->plant[i].RespLay,"Morphology","thickness_resp_wood_layer");
    GET_INI_DOUBLE(self->plant[i].CD,"Morphology","CD_ratio");
    
    GET_INI_INTEGER(self->plant[i].HDvar,"Morphology","HD_var");
    GET_INI_DOUBLE(self->plant[i].HDmin,"Morphology","HD_min"); 
    GET_INI_DOUBLE(self->plant[i].HDmax,"Morphology","HD_max"); 
    
    GET_INI_DOUBLE(self->plant[i].UgWdFr,"Morphology","below_ground_wood_fraction");
    GET_INI_DOUBLE(self->plant[i].CReloc,"Morphology","assimilate_relocation");
    GET_INI_DOUBLE(self->plant[i].NRelocLit,"Morphology","nitrogen_relocation");
    GET_INI_DOUBLE(self->plant[i].SaturationCap,"Morphology","saturation_capacity");
	{
		char S[128];
		sprintf(S,"%f\n", self->plant[i].SaturationCap);
		PRINT_MESSAGE(xpn,5,S);
	}
    GET_INI_DOUBLE(self->plant[i].CanopyReflection,"Morphology","canopy_reflection");
    GET_INI_DOUBLE(self->plant[i].b_Drain,"Morphology","b_drainage");
    GET_INI_DOUBLE(self->plant[i].ThroughfallCoeff,"Morphology","throughfall_coefficient");
    GET_INI_DOUBLE(self->plant[i].CAssReserve,"Morphology","assimilate_reserve");
    GET_INI_DOUBLE(self->plant[i].LAImaxH,"Morphology","LAImaxH");
    GET_INI_DOUBLE(self->plant[i].LfFRt,"Morphology","Leaf_to_FineRoot_ratio");
    GET_INI_DOUBLE(self->plant[i].FiFrLf,"Morphology","final_fruit_to_leaf_ratio");
    
    // [Morphology]
    GET_INI_DOUBLE(self->plant[i].LfFall,"Phenology","leaf_fall");
    GET_INI_DOUBLE(self->plant[i].LfFlush,"Phenology","leaf_flush");
    GET_INI_DOUBLE(self->plant[i].LfBudBurst,"Phenology","bud_burst_const");
    GET_INI_DOUBLE(self->plant[i].LfFallStart,"Phenology","leaf_fall_start");   
    GET_INI_DOUBLE(self->plant[i].LfFallEnd,"Phenology","leaf_fall_end");   
    
    // [Root]
    GET_INI_DOUBLE(self->plant[i].RtLengthSpec,"Root","specific_root_length");
    GET_INI_DOUBLE(self->plant[i].WaterUpMax,"Root","max_water_uptake");
    GET_INI_DOUBLE(self->plant[i].NUpMax,"Root","max_nitrogen_uptake");
    GET_INI_DOUBLE(self->plant[i].TRootZero,"Root","TRootZero");
    
    // [Damage]
    GET_INI_DOUBLE(self->plant[i].DamageLf,"Damage","leaf");
    GET_INI_DOUBLE(self->plant[i].DamagePh,"Damage","photosynthesis");
    GET_INI_DOUBLE(self->plant[i].DamageFRt,"Damage","fine_root");  
    
    // [Nitrogen_Contents]
    GET_INI_DOUBLE(self->plant[i].NWdResp,"Nitrogen_Contents","NWdResp");
    GET_INI_DOUBLE(self->plant[i].NWdDead,"Nitrogen_Contents","NWdDead");
    GET_INI_DOUBLE(self->plant[i].NLf,"Nitrogen_Contents","NLf");
    GET_INI_DOUBLE(self->plant[i].NLfDead,"Nitrogen_Contents","NLfDead");
    GET_INI_DOUBLE(self->plant[i].NFr,"Nitrogen_Contents","NFr");
    GET_INI_DOUBLE(self->plant[i].NFRt,"Nitrogen_Contents","NFRt");
	
	GET_INI_DOUBLE(self->plant[i].NO3_DepIncr,"Nitrogen_Contents","NO3_DepIncr");
	GET_INI_INTEGER(self->plant[i].NO3_DepConstYr,"Nitrogen_Contents","NO3_DepConstYr");
	GET_INI_INTEGER(self->plant[i].N_DepStartYr,"Nitrogen_Contents","N_DepStartYr");
	GET_INI_DOUBLE(self->plant[i].N_DepStart,"Nitrogen_Contents","N_DepStart");
	
    
    // [Soil_Litter]
    GET_INI_DOUBLE(self->plant[i].CLit,"Soil_Litter","C_litter");
    GET_INI_DOUBLE(self->plant[i].CSOM,"Soil_Litter","C_SOM");
    GET_INI_DOUBLE(self->plant[i].NLit,"Soil_Litter","N_litter");
    GET_INI_DOUBLE(self->plant[i].NSOM,"Soil_Litter","N_SOM");
    GET_INI_DOUBLE(self->plant[i].NSoil,"Soil_Litter","N_soil");
    
    // [Climate]
    GET_INI_DOUBLE(self->clim.AtmAbs,"Climate","AtmAbs");
    GET_INI_DOUBLE(self->clim.PARfactor,"Climate","PARfactor");
    GET_INI_DOUBLE(self->clim.SolarConstant,"Climate","SolarConstant");
    GET_INI_DOUBLE(self->clim.CloudFraction,"Climate","CloudFraction");
    GET_INI_DOUBLE(self->clim.CloudyDaysFrac,"Climate","CloudyDaysFrac");
    
    // [Soil]
    GET_INI_DOUBLE(self->plant[i].TSoilRef,"Soil","T_ref");
    GET_INI_DOUBLE(self->plant[i].TSoilZero,"Soil","T_zero");
    GET_INI_DOUBLE(self->plant[i].T_veg,"Soil","T_veg");
    GET_INI_DOUBLE(self->plant[i].damp_T,"Soil","damp_T");
    
    // [Silviculture]
    GET_INI_INTEGER(self->silv[i].Harvesting,"Silviculture","harvesting");
    if(self->silv[i].Harvesting == 1)
    {
        GET_INI_DOUBLE(self->silv[i].ThinningEvents,"Silviculture","thinning_events");
        GET_INI_DOUBLE_LIST(self->silv[i].ThinningInterval, "Silviculture", "thinning_interval", &(self->silv[i].ThinningEvents))
        GET_INI_DOUBLE_LIST(self->silv[i].ThinningFraction, "Silviculture", "thinning_fraction", &(self->silv[i].ThinningEvents))
        {
            int i2;
            char S[128];
			for (i2=0;i2<self->silv[i].ThinningEvents;i2++)
            {
                sprintf(S,"%d. thinning: in %.1f years \n",i2+1,self->silv[i].ThinningInterval[i2]);
                PRINT_MESSAGE(xpn,3,S);
            }
        }
        GET_INI_DOUBLE(self->silv[i].LitterRemoval,"Silviculture","litter_removal");
        GET_INI_DOUBLE(self->silv[i].ThinningMethod,"Silviculture","thinning_method");
		//Added by Hong on 20180716:
		GET_INI_DOUBLE(self->silv[i].heightAfterCut,"Silviculture","height_after_cut");
		GET_INI_DOUBLE(self->silv[i].diameterAfterCut,"Silviculture","diameter_after_cut");
    }
    
    // [Photosynthesis]
    GET_INI_DOUBLE(self->plant[i].DPhotoTime,"Photosynthesis","time_step");
    GET_INI_INTEGER(self->plant[i].DPhotoLayers,"Photosynthesis","intermediate_layers");
    GET_INI_DOUBLE(self->plant[i].PhProdMax,"Photosynthesis","max_photoproduction");
    GET_INI_DOUBLE(self->plant[i].PhInitSlope,"Photosynthesis","init_slp_photo_sens");
    GET_INI_DOUBLE(self->plant[i].PhTopt,"Photosynthesis","T_opt");
    GET_INI_DOUBLE(self->plant[i].PhTzero,"Photosynthesis","T_zero");
    GET_INI_DOUBLE(self->plant[i].PhCO2Fac,"Photosynthesis","CO2_factor");
	GET_INI_DOUBLE(self->clim.CO2_incr_fac,"Photosynthesis","CO2_incr_fac");
	
	GET_INI_DOUBLE(self->plant[i].CO2conc,"Photosynthesis","CO2_conc");
	xpn->pCl->pWeather->fAtmCO2ppm = self->plant[i].CO2conc;
	
	GET_INI_DOUBLE(self->plant[i].CO2incr,"Photosynthesis","CO2_incr");
	GET_INI_INT_OPTIONAL(self->clim.CO2Yr,"Photosynthesis","CO2_yr",xpn->pTi->pSimTime->iStart_year+1);
        // Falge Model is used
        if(self->conf.Photosynthesis != 1)
        {
            GET_INI_DOUBLE(self->plant[i].gfacbase,"Photosynthesis","gfacbase");
            GET_INI_DOUBLE(self->plant[i].alpha,"Photosynthesis","init_slp_photo_sens_FALGE");
            GET_INI_DOUBLE(self->plant[i].WidthOfLeaf,"Photosynthesis","width_of_leaf");
            GET_INI_DOUBLE(self->plant[i].SigmaLeaf,"Photosynthesis","coniferous_leaf_coefficient");
            
            GET_INI_DOUBLE(self->plant[i].vcmax298,"Photosynthesis","vcmax298");
            GET_INI_DOUBLE(self->plant[i].darkresp298,"Photosynthesis","darkresp298");
            GET_INI_DOUBLE(self->plant[i].fac,"Photosynthesis","fac");
            GET_INI_DOUBLE(self->plant[i].tau298,"Photosynthesis","tau298");
            GET_INI_DOUBLE(self->plant[i].pml298,"Photosynthesis","pml298");
            GET_INI_DOUBLE(self->plant[i].ko298,"Photosynthesis","ko298");
            GET_INI_DOUBLE(self->plant[i].kc298,"Photosynthesis","kc298");
            
			GET_INI_DOUBLE(self->plant[i].f_Ko,"Photosynthesis","f_Ko");
			GET_INI_DOUBLE(self->plant[i].f_Kc,"Photosynthesis","f_Kc");
			GET_INI_DOUBLE(self->plant[i].f_tau,"Photosynthesis","f_tau");
			GET_INI_DOUBLE(self->plant[i].f_resp,"Photosynthesis","f_resp");
			
            GET_INI_DOUBLE(self->plant[i].DeltaHaKo,"Photosynthesis","DeltaHaKo");
            GET_INI_DOUBLE(self->plant[i].DeltaHaKc,"Photosynthesis","DeltaHaKc");
            GET_INI_DOUBLE(self->plant[i].DeltaHatau,"Photosynthesis","DeltaHatau");
            GET_INI_DOUBLE(self->plant[i].DeltaHaresp,"Photosynthesis","DeltaHaresp");
            
            GET_INI_DOUBLE(self->plant[i].DeltaHaVcMax,"Photosynthesis","DeltaHaVcMax");
            GET_INI_DOUBLE(self->plant[i].DeltaHdVcMax,"Photosynthesis","DeltaHdVcMax");
            GET_INI_DOUBLE(self->plant[i].DeltaSVcMax,"Photosynthesis","DeltaSVcMax");
            
            GET_INI_DOUBLE(self->plant[i].DeltaHaPml,"Photosynthesis","DeltaHaPml");
            GET_INI_DOUBLE(self->plant[i].DeltaHdPml,"Photosynthesis","DeltaHdPml");
            GET_INI_DOUBLE(self->plant[i].DeltaSPml,"Photosynthesis","DeltaSPml");  
            
            GET_INI_DOUBLE(self->plant[i].StomCondMax,"Photosynthesis","StomCondMax");
            GET_INI_DOUBLE(self->plant[i].StomCondMin,"Photosynthesis","StomCondMin");
            GET_INI_DOUBLE(self->plant[i].totaltoprojm2factor,"Photosynthesis","totaltoprojm2factor");      
        }
    
    // [Respiration]
    GET_INI_DOUBLE(self->plant[i].DRespTime,"Respiration","time_step_respiration");
        // respiration rates [1/yr]
    GET_INI_DOUBLE(self->plant[i].RespGr,"Respiration","resp_growth");
    GET_INI_DOUBLE(self->plant[i].RespLf,"Respiration","resp_leaves");
    GET_INI_DOUBLE(self->plant[i].RespRt,"Respiration","resp_fine_roots");
    GET_INI_DOUBLE(self->plant[i].RespWd,"Respiration","resp_wood");
    GET_INI_DOUBLE(self->plant[i].RespFr,"Respiration","resp_fruit");
        // temperature limits
    GET_INI_DOUBLE(self->plant[i].TRespZero,"Respiration","T_zero");
    GET_INI_DOUBLE(self->plant[i].TRespRef,"Respiration","T_ref");
        // respiration period
    GET_INI_DOUBLE(self->plant[i].RespPeriodFr,"Respiration","resp_fruit"); 
    
    // [TREEDYN]    
    GET_INI_DOUBLE(self->plant[i].Decomp,"TREEDYN","norm_decomp");
    GET_INI_DOUBLE(self->plant[i].Humif,"TREEDYN","humification");
    GET_INI_DOUBLE(self->plant[i].HuMiner,"TREEDYN","humus_mineralization");
    GET_INI_DOUBLE(self->plant[i].Leach,"TREEDYN","nitrogen_leaching");
    GET_INI_DOUBLE(self->plant[i].NFixMax,"TREEDYN","max_nitrogen_fixation");
    GET_INI_DOUBLE(self->plant[i].NDepos,"TREEDYN","nitrogen_deposition");
    
    // [Fagus]  
    GET_INI_DOUBLE(self->plant[i].SoilMinLit,"Fagus","litter_mineralization");
    GET_INI_DOUBLE(self->plant[i].SoilMinRt,"Fagus","fine_root_mineralization");
    GET_INI_DOUBLE(self->plant[i].SoilMinHum,"Fagus","humus_mineralization");
    GET_INI_DOUBLE(self->plant[i].LeafMinLig,"Fagus","lignin_mineralization");
    GET_INI_DOUBLE(self->plant[i].LeafMinCel,"Fagus","cellulose_mineralization");
    GET_INI_DOUBLE(self->plant[i].LeafMinCHy,"Fagus","carbohydrate_mineralization");
    
    // [Senescence]
    GET_INI_DOUBLE(self->plant[i].TOFr,"Senescence","fruit_turnover");
    GET_INI_DOUBLE(self->plant[i].TOLf,"Senescence","leaf_turnover");
    GET_INI_DOUBLE(self->plant[i].TORt,"Senescence","fine_root_turnover");
    GET_INI_INTEGER(self->plant[i].MortInitYr,"Senescence","mortality_init_yr");
    GET_INI_DOUBLE(self->plant[i].MortNormInit,"Senescence","mortality_norm_init");
    GET_INI_DOUBLE(self->plant[i].MortCrowdInit,"Senescence","mortality_crwd_init");
    GET_INI_DOUBLE(self->plant[i].MortNorm,"Senescence","mortality_norm");
    GET_INI_DOUBLE(self->plant[i].MortCrowd,"Senescence","mortality_crwd");
    GET_INI_DOUBLE(self->plant[i].DeadWdLoss,"Senescence","deadwood_loss"); 
    
    // [Miscellaneous]
    GET_INI_DOUBLE(self->plant[i].CUseSpec,"Miscellaneous","spec_assimilate_use");
    GET_INI_DOUBLE(self->plant[i].NUpSpecR,"Miscellaneous","spec_nitrogen_uptake");
    //GET_INI_DOUBLE(self->plant[i].C,"Miscellaneous","spec_nitrogen_uptake");
    
	//added by Hong on 26032019 for agroforestry:
	//xpn->pCh->pCProfile->fCNLeafLitterSurf = 1.0 /self->plant[j].NLfDead;
	//xpn->pCh->pCProfile->fCNBranchLitterSurf =1.0 /self->plant[j].NWdDead;
	//xpn->pCh->pCProfile->fCNStemLitterSurf = 1.0 /self->plant[j].NWdDead;	
    //GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNCFineRootLitter,"NFRt","Nitrogen_Contents",PCLAYER,xpn->pCh->pCLayer);
	
	//GET_INI_DOUBLE(self->plant[i].NFRt,"Nitrogen_Contents","NFRt");
	//End of Hong
	
    g_key_file_free(keyfile);

    return RET_SUCCESS;
}

int register_var(libtreemix *self)
{
    
    //Plant:
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->plant.density,"treemix.plant.density",0.0);

	//xpn_register_var_init_rec_struct( var_list,CLAYER, pCh->pCLayer,xpn_register_var_init_pdouble,fCNFineRootLitter,"pCh.pCLayer.fCNFineRootLitter",NFRt);
	
    return RET_SUCCESS;
}

int set_start_values_plant(libtreemix *self, int i)
{
    /* Abbreviations */
    expertn_modul_base *xpn = &(self->parent);
    PPLTNITROGEN    pPlN    = xpn->pPl->pPltNitrogen;
    
    self->silv[i].StandAlive = 1;
    //self->silv[i].Harvesting = 1;
    
    self->silv[i].CWdHarv = 0.0;
    self->silv[i].CStHarv = 0.0;
    
    
    self->plant[i].kN = pPlN->fNStressPhoto;    // nitrogen availability
    self->plant[i].CLfOld = self->plant[i].CLfMass/1000.0;  // [kg] -> [t]
    
    /* total biomass summed up, but depends on species distribution */
    self->plant[i].TreeDistr = self->plant[i].density/xpn->pPl->pCanopy->fPlantDensity;
    //xpn->pPl->pBiomass->fWoodWeight += self->plant[i].TreeDistr * self->plant[i].CWdMass;     // Wood Mass [kgC/ha]
    xpn->pPl->pBiomass->fLeafWeight += self->plant[i].TreeDistr * self->plant[i].CLfMass;       // Leaf Mass [kgC/ha]
    xpn->pPl->pBiomass->fRootWeight += self->plant[i].TreeDistr * self->plant[i].CFRtMass;      // Fine Root Mass [kgC/ha]
    xpn->pPl->pBiomass->fFruitWeight += self->plant[i].TreeDistr * self->plant[i].CFrMass;      // Fruit Mass [kgC/ha]
    
    /* Set Morphology Parameters and Geometric Relations*/
    libtreemix_set_tree_morphology(self, i);
    libtreemix_set_tree_geometry(self, i);
    libtreemix_set_rootlength_density(self, i);
    
    return RET_SUCCESS;
}

int set_climate(libtreemix *self)
{
    expertn_modul_base *xpn = &(self->parent);
    
    /* variables */
    double m = 0.0;         // help variable
    double FirstSunSolYr, LastSunSolYr;
    double HrSrW, HrSrS;    // time of sunrise in winter and summer
    double hMax, hMin;      //minimal and maximal daylength [h]
    double IMinClear = 0.0, IMinCloud = 0.0;
    double IMaxClear = 0.0, IMaxCloud = 0.0;
    
    /* functions */
    void libtreemix_CalcPAR(libtreemix *self, double Sunrise, double Daylength, double SeasonalDay, double *ISumClear, double *ISumCloud);
    
    /* seasonal time */
    self->clim.SeasTime = (double)(xpn->pTi->pSimTime->fTimeY + 10.0)/365.25;   //eq. 33
    
    self->clim.Lat = xpn->pLo->pFarm->fLatitude/90.0 * M_PI/2.0;    
    if(self->clim.Lat > PI/2.0)
    {
        self->clim.Lat = PI/2.0;
    }
    
    /* latitude parameters, polar, tropical or seasonal? */
    if(self->clim.Lat > (66.5*(PI/180.0)))
    {
        self->clim.Polar = 1;
    }
    else if((xpn->pLo->pFarm->fLatitude < 23.5)&&(xpn->pLo->pFarm->fLatitude > -23.5)) // nördlicher und südlicher wendekreis)
    {
        self->clim.Polar = 0;
        self->clim.Tropical = 1;
    }
    else
    {
        self->clim.Tropical = 0;
        self->clim.Seasonal = 1;
    }
    
    /* sun appearance in polar regions */
    if(self->clim.Polar == 1)
    {
        m = -(self->clim.Lat-(PI/2.0)/0.40841);
        
        if(m < 0.001)
        {
            FirstSunSolYr = 1.0/4.0;
        }
        else
        {
            FirstSunSolYr = atan(sqrt(1.0-(m*m))/m)/(2.0*PI);
        }
        LastSunSolYr = 1.0-FirstSunSolYr;
        
        self->clim.DayFirstSun = FirstSunSolYr*365.25-10;
        self->clim.DayLastSun = LastSunSolYr*365.25-10;
        
        HrSrS = 0.0;
        
    }
    else
    {
        m = sin(self->clim.Lat)*sin(0.40841)/(cos(self->clim.Lat)*cos(0.40841));
        
        if((m*m) < 0.0001)
        {
            HrSrS = 6.0;
        }
        else
        {
            HrSrS = (12/PI)*atan(sqrt(1.0-(m*m))/m);    //eq. 44, sunrise at summer solstice (sommersonnenwende)
        }
    }
    
    HrSrW = (24.0/2.0)-HrSrS;
    
    hMax = 24-(2.0*HrSrS);      //eq. 46
    hMin = 2.0*HrSrS;           //eq. 47
    self->clim.hAmp = 24.0-(4.0*HrSrS); //eq. 48
    
    /* computation of PAR receipt on longest and shortest day of year */
    //ATTN: Note that the PARFACTOR = 0.47 (orig treedyn = 0.5)
    libtreemix_CalcPAR(self, HrSrS, hMax, 0.5, &IMaxClear, &IMaxCloud); //eq. 50, 0.5 -> seasonal date 21.06.
    libtreemix_CalcPAR(self, HrSrW, hMin, 0.0, &IMinClear, &IMinCloud); //eq. 51, 0.0 -> seasonal date 22.12.
    
    self->clim.IsumclearMax = IMaxClear;
    self->clim.IsumclearMin = IMinClear;
    
    self->clim.IsumcloudMax = IMaxCloud;
    self->clim.IsumcloudMin = IMinCloud;
    
    
    /* Computation of PAR amplitudes, eq. 52 */
    self->clim.IAmpClear = IMaxClear - IMinClear;
    self->clim.IAmpCloud = IMaxCloud - IMinCloud;

    /* Computation of PAR average, eq. 53 */
    self->clim.IAvgClear = IMinClear + self->clim.IAmpClear/2.0;
    self->clim.IAvgCloud = IMinCloud + self->clim.IAmpCloud/2.0;    
    
    return RET_SUCCESS;
}

void libtreemix_CalcPAR(libtreemix *self, double Sunrise, double Daylength, double SeasonalDay, double *ISumClear, double *ISumCloud)
{
    /* Abbreviations */
    expertn_modul_base *xpn = &(self->parent);
    
    /* variables */
    double SolarElevation, SolarDeclination;
    double IClear, ICloud;
    double Step;
    double Hour;
    double dt = xpn->pTi->pTimeStep->fAct;
    /****************************************************************************************************************/
    
    Step = dt;
    Hour = Sunrise-Step;

    do
    {
        Hour = Hour+Step;

        /* calculate solar elevation */
        SolarDeclination = -23.4*PI/180.0*cos(2*PI*SeasonalDay);                                                            //eq. 41

        SolarElevation = sin(self->clim.Lat)*sin(SolarDeclination) + cos(self->clim.Lat)*cos(SolarDeclination)*
                         cos((2*PI/24.0)*(Hour+(24.0/2.0)));                                                                //eq. 40
        

        if(SolarElevation > 0.01){
            IClear = self->clim.SolarConstant*self->clim.PARfactor*SolarElevation*exp(-(self->clim.AtmAbs)/SolarElevation); //eq. 39
            ICloud = self->clim.CloudFraction*IClear;                                                                       //eq. 42
        }
        else{
            IClear = 0.0;
            ICloud = 0.0;
        }

        *ISumClear += (IClear*Step);
        *ISumCloud += (ICloud*Step);

    }while(!((SolarElevation<0.0)&&(Hour>(Sunrise+Daylength))) || (Hour>24.0));
}


int libtreemix_set_tree_morphology(libtreemix *self, int i)
{
    /*
        Determines:
        - Carbon Mass of Wood a. Stems
        - Root a. Branch Fraction
        - Maximum LAI
        - N content in Wood
        - Respiring and non respiring Wood Mass, etc.
    */
    
    expertn_modul_base *xpn = &(self->parent);
    
    if((self->conf.Tree_Geometry == 1)||(self->conf.Tree_Geometry == 2))
    {
        double V;   // stem volume per tree [m3/tree]
        double G;   // carbon mass per tree stem [tC/tree]
        
        /* note: the expon. factor 'brexp' (NOW: BrWdFrCoeff) in the orig. TREEDYN source code is = 100.0, why? It is 10.0 in BOSSEL!!!! */
        self->plant[i].BrWdFr = self->plant[i].FiBrWdFr + (1.0-self->plant[i].FiBrWdFr)*exp(-self->plant[i].BrWdFrCoeff*self->plant[i].D/self->plant[i].Dmax); //eq. 20, pt.1
        
        /* determining specific wood masses by morphologic relations and fractions */
        V = (M_PI/4.0)*self->plant[i].Form * pow(self->plant[i].D, 2)*self->plant[i].H; // eq. 15, [m3], Stem Volume of an individual tree
        G = V*(self->plant[i].CWdDens * 1000.0);                                        // eq. 16, [kgC/tree], (Carbon) mass of an individual stem
        
        /* whole stand [kgC/ha] */
        self->plant[i].CStMass = G*self->plant[i].density;  // Stem carbon mass, [kgC/ha]
        self->plant[i].CWdWeight = G*self->plant[i].density/((1.0-self->plant[i].UgWdFr)*(1.0-self->plant[i].BrWdFr));
        
        
        /* summing up for stand */
        xpn->pPl->pBiomass->fStemWeight += self->plant[i].TreeDistr*(self->plant[i].CStMass);
        xpn->pPl->pBiomass->fWoodWeight += self->plant[i].TreeDistr*(self->plant[i].CWdWeight); // [kgC/ha]
        
        self->plant[i].CTrMass = self->plant[i].CWdWeight/1000.0/self->plant[i].density; // [tC/tree], wood mass per tree incl. branches, coarse roots
        
        //xpn->pPl->pBiomass->fWoodWeight += self->plant[i].TreeDistr*(self->plant[i].CWdMass); // [kgC/ha]     
        //self->plant[i].CTrMass = self->plant[i].CWdMass/(1000.0*self->plant[i].density); // [tC/tree]

        // calculating ratio of respiring to total wood
        self->plant[i].b = 1.0;     // eq. 26       
        if(self->plant[i].D > (2.0*self->plant[i].RespLay))
        {
            double Hc;
            double Hi;
            double Vc;
            double Vo;

            Hc = (12.0/(PI*self->plant[i].CWdDens))*(self->plant[i].CTrMass/pow(self->plant[i].D, 2.0));// eq. 22
            Hi = (self->plant[i].D-(2.0*self->plant[i].RespLay))*(Hc/self->plant[i].D);                 // eq. 23

            Vc = (PI/12.0)*pow(self->plant[i].D, 2.0)*Hc;                                               // eq. 24
            Vo = (PI/12.0)*(pow(self->plant[i].D, 2.0)*Hc - pow((self->plant[i].D-(2.0*self->plant[i].RespLay)), 2.0)*Hi);  // eq. 25

            self->plant[i].b = Vo/Vc;                                                                   // eq. 26
        }
        self->plant[i].CWdResp = (self->plant[i].b*self->plant[i].CWdWeight)/1000.0;                        // resp. wood mass, [tC/ha]

        self->plant[i].LAImax = -log(self->plant[i].LightFrac)/self->plant[i].LightExt;                 // eq. 32
        self->plant[i].NWd = self->plant[i].b*self->plant[i].NWdResp*self->plant[i].CWdWeight +
            (1.0-self->plant[i].b)*self->plant[i].NWdDead*self->plant[i].CWdWeight;                     // part of eq. 9
            
    }
    
    return RET_SUCCESS;
}

int libtreemix_set_tree_geometry(libtreemix *self, int i)
{
    //expertn_modul_base *xpn = &(self->parent);
    
    if((self->conf.Tree_Geometry == 1)||(self->conf.Tree_Geometry == 2))
    {
        /* for spruce: min and max HD-ratio is being calculated 
         * according to Forschungsberichte Waldökosysteme, 1995 */
        if(self->plant[i].HDvar == 1)
        {
            if(!strcmp(self->plant[i].code, "SP"))
            {               
                self->plant[i].HDmin = 160.0/(pow(self->plant[i].H, 0.5));
                self->plant[i].HDmax = self->plant[i].HDmin + 80.0;
            
                if(self->plant[i].H < 5.0)
                {
                    self->plant[i].HDmin = 70.0;
                }
            }       
        }
        
        // HD ratio
        self->plant[i].HD = self->plant[i].H/self->plant[i].D;
        
        //current crown projection area, [sqm]  
        if(self->plant[i].D < self->plant[i].Dmax)
        {
            self->plant[i].CrArea = pow((self->plant[i].D*self->plant[i].CD), 2.0)*M_PI/4.0;        // eq. 27           
        }
        else
        {
            self->plant[i].CrArea = pow((self->plant[i].Dmax*self->plant[i].CD), 2.0)*M_PI/4.0;     // eq. 28           
        }
        
        
        if(((self->plant[i].density*self->plant[i].CrArea)/10000.0) < 1.0)
        {
            self->plant[i].CrArPot = (self->plant[i].density*self->plant[i].CrArea)/10000.0;            // eq. 29
            self->silv[i].Crowding = 0.0;
        }
        else
        {
            self->silv[i].Crowding = 1.0;

            /*
                CrArPot could take values > 1.0,
                and DRed is NOT being used in the original BOSSEL code
                DRed is only used for diminishing D in the calculation of CrArea
            */
            self->plant[i].CrArPot = 1.0;
            self->plant[i].Dred = self->plant[i].D - 200.0 /(sqrt(PI*self->plant[i].density)*self->plant[i].CD); 
        }
        
        self->plant[i].LAI = (self->plant[i].CLfMass/1000.0)*(100.0/(self->plant[i].CLfDens*self->plant[i].CrArPot)); // eq. 30
        self->plant[i].LAItemp = self->plant[i].LAI * self->plant[i].TreeDistr;
    
        //ATTN: sums up the LAI, and leads to constantly rising values...see in Senescence!
        //xpn->pPl->pCanopy->fLAI += self->plant[i].LAItemp;
        self->plant[i].LAItemp = 0.0;
        
        
    }
	
    else
    {
        // Here is space left for another tree geometry module (e.g.the PGM module)
    }   
    return RET_SUCCESS;
}

int libtreemix_set_rootlength_density(libtreemix *self, int i)
{   
    /* Description */
    /*
     * Calculate Root Length Density according to CERES 
     * (Jones & Kiniry, 1986; Ritchie & Godwin, 2000)
    */
    
    /* Abbreviations */
    expertn_modul_base *xpn = &(self->parent);
    PROOT pRt = xpn->pPl->pRoot;    
    PSLAYER pSL = xpn->pSo->pSLayer;
    PSWATER pSWL = xpn->pSo->pSWater;   
    PWLAYER pSLW = xpn->pWa->pWLayer;
    PCLAYER pSLN = xpn->pCh->pCLayer;
    PLAYERROOT pLR = xpn->pPl->pRoot->pLayerRoot;
    //PBIOMASS pBio = xpn->pPl->pBiomass;
    //PHLAYER   pSLH = xpn->pHe->pHLayer;
    
    /* variables */
    int z, L, L1, NLAYER;
    double CumDepth;
    
    double RtFac;
    double RtLgthDensFacTot = 0.0;
    double Thickness;
    
    /* Functions */
    
    /****************************************************************************************************************/
    
    NLAYER = xpn->pSo->iLayers-2;

    pRt->fDepth = xpn->pSo->fDepth*0.1;                     // roots reach through complete soil profile [cm]=[mm]*0.1
    pRt->fMaxDepth = pRt->fDepth = 0.1*xpn->pSo->fDepth;    // [cm]
    
    //==================================================================================================================
    /* Calc Total Root Length out of Fine Root Mass using the specific root length */
    // 1 [kg/ha]    = 0.000 000 01  [kg/cm2]
    // 1 [cm/g]     = 1000          [cm/kg]
    // => conversion factor: 0.00000001*1000.0 = 0.00001
    //==================================================================================================================
	// [cm/cm²] 			= 			[kg/ha] 	  * 			[cm/g]			* conversion
    self->plant[i].RtLength = self->plant[i].CFRtMass * self->plant[i].RtLengthSpec * 0.00001;
    
    //==================================================================================================================
    /* calc. Root Length Density Factor for each Layer */
    //==================================================================================================================
    CumDepth = 0.0;
    z = 1;
    L = 0;  
    pSL = xpn->pSo->pSLayer->pNext;
    pSWL = xpn->pSo->pSWater->pNext;
    pSLW = xpn->pWa->pWLayer->pNext; 
    pSLN = xpn->pCh->pCLayer->pNext;
    pLR = pLR->pNext;   
    while((CumDepth <= pRt->fDepth)&&(L<NLAYER))
    {
        L++;

        Thickness = pSL->fThickness*0.1;    // [cm]=[mm]*0.1
        CumDepth += pSL->fThickness*0.1;    // [cm]=[mm]*0.1
        
        // [-] {0...1}, mimic mechanical impedance for root expansion (CERES)
        RtFac = exp(-4.0*((CumDepth - 0.5*Thickness)/pRt->fMaxDepth));          // [-]
        
        self->plant[i].RtLengthDensFac[z-1] = RtFac * self->plant[i].RtLength * Thickness; // [-] = [-]*[cm/cm²]*[cm]

        //ATTN: Root Length Density Factor will not be calculated in the last layer, due to the if statement, WHY!!!!
        // see also old treedyn3 code in XN3.0
        if(L<NLAYER-1)
        {
            pSL = pSL->pNext;
            pSWL = pSWL->pNext;
            pSLW = pSLW->pNext;
            pSLN = pSLN->pNext;
            pLR = pLR->pNext;
            z++;
        }
    }   
    L1 = L;
    
    // sum up to total Root length density factor
    for(L=1; L<=L1; L++)
    {       
        RtLgthDensFacTot += self->plant[i].RtLengthDensFac[L-1];    // [-]
    }
    self->plant[i].RtLengthDensFacTot = RtLgthDensFacTot;
    
    //==================================================================================================================
    /* Sum Up Root Length Density Factor => Total Root Length Density Factor */
    //==================================================================================================================    
    //self->plant[i].RtLengthDensFacTot = 0.0;
    //pLR = xpn->pPl->pRoot->pLayerRoot->pNext;
    //for(L=1; L<=L1; L++)
    //{
    //  self->plant[i].RtLengthDensFacTot += self->plant[i].RtLengthDensFac[L-1];   // [-]
    //  pLR = pLR->pNext;
    //}
    if(self->plant[i].RtLengthDensFacTot >= (self->plant[i].RtLength*0.00001))
    {
        self->plant[i].RNLF = self->plant[i].RtLength/self->plant[i].RtLengthDensFacTot;    // [cm/cm²] = [cm/cm²]/[-]
    }
    else
    {
        PRINT_ERROR("Initialization.c: set_rootlength_density: RNLF");
        return self->__ERROR;
    }
    
    //==================================================================================================================
    /* Calc. Root Length Density */
    //==================================================================================================================
    pSL     = xpn->pSo->pSLayer->pNext;
    pSWL    = xpn->pSo->pSWater->pNext;
    pSLW    = xpn->pWa->pWLayer->pNext; 
    pSLN    = xpn->pCh->pCLayer->pNext;
    pLR     = xpn->pPl->pRoot->pLayerRoot->pNext;
    CumDepth = 0.0;
    
    for(L=1; L<=L1; L++)
    {
        self->plant[i].RtLengthDens[L-1] = self->plant[i].RtLengthDensFac[L-1]*self->plant[i].RNLF; // [cm ROOT/cm2 SOIL]
        
        // Length Density is summed up to a total (if there are more than one one species)
        pLR->fLengthDens += MAX(0.0, self->plant[i].RtLengthDens[L-1]);

        pSL     = pSL->pNext;
        pLR     = pLR->pNext;
        pSWL    = pSWL->pNext;
        pSLW    = pSLW->pNext;
        pSLN    = pSLN->pNext;
    }   
    return RET_SUCCESS;
}

int libtreemix_set_litter_pools(libtreemix *self)
{
    /* Abbreviations */
    expertn_modul_base *xpn = &(self->parent);
    //PPLTNITROGEN  pPlN    = xpn->pPl->pPltNitrogen;
    PCLAYER         pSLN    = xpn->pCh->pCLayer;
    PSLAYER         pSL     = xpn->pSo->pSLayer;
    PLAYERROOT      pLR     = xpn->pPl->pRoot->pLayerRoot;
	PCBALANCE	    pCB     = xpn->pCh->pCBalance; //Added by Hong on 20180731

    /* variables */
    int i;
    
    for(i=0; i<self->conf.Species_Count; i++)
    {
        self->plant[i].LitGrRt  = self->plant[i].CWdWeight*self->plant[i].UgWdFr*self->plant[i].DeadWdLoss;
        self->plant[i].LitRt    = self->plant[i].CFRtMass*self->plant[i].TORt;
        self->plant[i].LitBr    = (1.0-self->plant[i].UgWdFr)*self->plant[i].CWdWeight*self->plant[i].BrWdFr*self->plant[i].DeadWdLoss;
        self->plant[i].LitSt    = (1.0-self->plant[i].UgWdFr)*self->plant[i].CWdWeight*(1.0-self->plant[i].BrWdFr)*self->plant[i].DeadWdLoss;
        
        if(self->plant[i].LitSt < 0.0)
        {
            self->plant[i].LitSt = 0.0;
        }
        
        // Total Wood loss
        self->plant[i].DeadWdLossTot = self->plant[i].LitGrRt + self->plant[i].LitBr + self->plant[i].LitSt;
    }
    
    if(self->conf.Soil_Processes == 2)
    {
        /* Variables */
        int i;  // soil layer looping variable
        int j;  // species looping variable

        // Temporal Variables for summing up the Litter Pools
        double TempCFineRootLitter, TempNFineRootLitter;
        double TempCGrossRootLitter, TempNGrossRootLitter;
        double TempCLitter, TempNLitter;
		
        double Biom, LfFrac, LfNorm;
        
		//added by EP on 20160629. Local varaibles have to be initialized!!!
		TempCFineRootLitter =0.0;
        TempNFineRootLitter=0.0;
        TempCGrossRootLitter=0.0;
        TempNGrossRootLitter=0.0;
        TempCLitter=0.0;
        TempNLitter=0.0;
		
        //==================================================================================================================
        /* Distribution of Roots Litter to Soil Layers */
        //==================================================================================================================
        pSL     = xpn->pSo->pSLayer->pNext;
        pLR     = xpn->pPl->pRoot->pLayerRoot->pNext;
        pSLN    = xpn->pCh->pCLayer->pNext;

        for(i=1; i<=xpn->pSo->iLayers-2; i++)
        {
			
            /* Setting Up Fresh Organic Matter Pools for each Species and summing them up at the end */
            for(j=0; j<self->conf.Species_Count; j++)
            {
                if(self->plant[j].RtLengthDens[i-1] > 0.0)
                {
                    // LyFc starts with '1'
                    self->plant[j].LyFc[i] = self->plant[j].RtLengthDens[i-1]/self->plant[j].RtLength;  
                }
                else
                {
                    self->plant[j].LyFc[i] = 0.0;
                }

                /* Gross Roots Litter [kg/ha] */
                if((pSLN->fCGrossRootLitter == 0.0)&&(pSLN->fNGrossRootLitter == 0.0))
                {
                    self->plant[j].FOCGrRt[i] = self->plant[j].LitGrRt*(1.0-self->plant[j].CReloc)*self->plant[j].LyFc[i];
                    self->plant[j].FONGrRt[i] = self->plant[j].LitGrRt*(1.0-self->plant[j].NRelocLit)*self->plant[j].NWdResp*self->plant[j].LyFc[i];        
                }
                else
                {
                    self->plant[j].FOCGrRt[i] = pSLN->fCGrossRootLitter;
                    self->plant[j].FONGrRt[i] = pSLN->fNGrossRootLitter;
                }

                /* Fine Roots Litter [kg/ha] */
                if((pSLN->fCFineRootLitter == 0.0)&&(pSLN->fNFineRootLitter == 0.0))
                {
                    self->plant[j].FOCFiRt[i] = self->plant[j].LitRt * (1.0-self->plant[j].CReloc)*self->plant[j].LyFc[i];
                    self->plant[j].FONFiRt[i] = self->plant[j].FOCFiRt[i]*self->plant[j].NFRt;
                }
                else
                {
                    self->plant[j].FOCFiRt[i] = pSLN->fCFineRootLitter;
                    self->plant[j].FONFiRt[i] = pSLN->fNFineRootLitter;
                }

                // summing up the species pools per soil layer, depending on species distribution
                TempCFineRootLitter += self->plant[j].FOCFiRt[i]*self->plant[j].TreeDistr;
                TempNFineRootLitter += self->plant[j].FONFiRt[i]*self->plant[j].TreeDistr;

                TempCGrossRootLitter += self->plant[j].FOCGrRt[i]*self->plant[j].TreeDistr;
                TempNGrossRootLitter += self->plant[j].FONGrRt[i]*self->plant[j].TreeDistr;

                TempCLitter += TempCFineRootLitter + TempCGrossRootLitter;
                TempNLitter += TempNFineRootLitter + TempNGrossRootLitter;

                // handing over to Expert-N variables
                pSLN->fCFineRootLitter += TempCFineRootLitter;
                pSLN->fNFineRootLitter += TempNFineRootLitter;

                pSLN->fCGrossRootLitter += TempCGrossRootLitter;
                pSLN->fNGrossRootLitter += TempNGrossRootLitter;

                pSLN->fCLitter += TempCLitter;
                pSLN->fNLitter += TempNLitter;
            }           

            // setting temporal variables back to zero
            TempCFineRootLitter = 0;
            TempNFineRootLitter = 0;
            
            TempCGrossRootLitter = 0;
            TempNGrossRootLitter = 0;

            TempCLitter = 0;
            TempNLitter = 0;

            /* Proceed to next Layer */
            pSL     = pSL->pNext;
            pLR     = pLR->pNext;
            pSLN    = pSLN->pNext;
        }
        
        //==================================================================================================================
        pSLN = xpn->pCh->pCLayer->pNext;

        for(j=0; j<self->conf.Species_Count; j++)
        {
            /* Branches, Twigs and Stem [kg/ha] */
            self->plant[j].FONBrSt = (self->plant[j].LitBr*self->plant[j].NWdResp)+
                (self->plant[j].LitSt*self->plant[j].NWdDead)*(1.0-self->plant[j].CReloc);
            
            self->plant[j].FOCBrSt = (self->plant[j].LitBr+self->plant[j].LitSt)*(1.0-self->plant[j].CReloc);
        
            /* Leaf and Fruit from FAGUS */
                //normally there is a distinction made between species, 
                // but there is no sufficient data!
            Biom = self->plant[j].CWdWeight/1000.0*self->plant[j].CWdDens;
            LfFrac = 0.117*pow(Biom, -0.4219);
            LfNorm = self->plant[j].CWdWeight/1000.0*LfFrac;

            self->plant[j].initFOCLfS = LfNorm*(1.0-self->plant[j].CReloc);
            self->plant[j].initFONLfS = self->plant[j].initFOCLfS*self->plant[j].NLf;

            self->plant[j].FOCLfFr = 0.8*LfNorm*(1.0-self->plant[j].CReloc);
            self->plant[j].FONLfFr = self->plant[j].FOCLfFr*self->plant[j].NLf;

            // sum up the species litter parts to XN-struct => total Leaf, Branch, Stem Litter
            xpn->pCh->pCProfile->fCLeafLitterSurf += self->plant[j].FOCLfFr*self->plant[j].TreeDistr;
            xpn->pCh->pCProfile->fNLeafLitterSurf += self->plant[j].FONLfFr*self->plant[j].TreeDistr;

            xpn->pCh->pCProfile->fCBranchLitterSurf += ((self->plant[j].FOCBrSt*self->plant[j].FiBrWdFr)*self->plant[j].TreeDistr);
            xpn->pCh->pCProfile->fNBranchLitterSurf += ((self->plant[j].FONBrSt*self->plant[j].FiBrWdFr)*self->plant[j].TreeDistr);

            xpn->pCh->pCProfile->fCStemLitterSurf += ((self->plant[j].FOCBrSt*(1.0-self->plant[j].FiBrWdFr))*self->plant[j].TreeDistr);
            xpn->pCh->pCProfile->fNStemLitterSurf += (float)((self->plant[j].FONBrSt*(1.0-self->plant[j].FiBrWdFr))*self->plant[j].TreeDistr);
            
			//Hong added on 20180807 for C-balance
			pCB->dCInputSurf += self->plant[j].FOCLfFr*self->plant[j].TreeDistr+
							((self->plant[j].FOCBrSt*self->plant[j].FiBrWdFr)*self->plant[j].TreeDistr)+ 
							((self->plant[j].FOCBrSt*(1.0-self->plant[j].FiBrWdFr))*self->plant[j].TreeDistr); 
			
/*			pCB->dCInputCum += self->plant[j].FOCLfFr*self->plant[j].TreeDistr+
							((self->plant[j].FOCBrSt*self->plant[j].FiBrWdFr)*self->plant[j].TreeDistr)+ 
							((self->plant[j].FOCBrSt*(1.0-self->plant[j].FiBrWdFr))*self->plant[j].TreeDistr);*/
							
			xpn->pCh->pCProfile->fCLitterSurf =xpn->pCh->pCProfile->fCLeafLitterSurf+ xpn->pCh->pCProfile->fCBranchLitterSurf + xpn->pCh->pCProfile->fCStemLitterSurf;//20181016				
			//End of Hong
			
			
            // The litter fractions of branches, leaves and stems are given to the surface litter layer
            pSLN->fCLitter += ((self->plant[j].FOCLfFr + self->plant[j].FOCBrSt)*self->plant[j].TreeDistr);
            pSLN->fNLitter += ((self->plant[j].FONLfFr + self->plant[j].FONBrSt)*self->plant[j].TreeDistr);
        }
    }
    
    return RET_SUCCESS;
}

int libtreemix_done(libtreemix *self)
{
    int i;
    
    for(i=0; i<self->conf.Species_Count; i++)
    {
        free(&self->plant[i]);      
    }
    
    free(&self->conf);
    
    free(&self->silv);
    free(&self->clim);
    
    
    return RET_SUCCESS;
}















