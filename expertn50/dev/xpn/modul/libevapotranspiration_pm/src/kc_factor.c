//      kc_factor.h
//
//      Copyright 2015 Christian Klein <christian.klein@helmholtz-muenchen.de>
//

#include "kc_factor.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(kc_factor, kc_factor, EXPERTN_MODUL_BASE_TYPE);

static void kc_factor_class_init(kc_factorClass* klass)
{
}

static void kc_factor_init(kc_factor* self)
{
    self->crop = NULL;
    self->crop_size = 0;
}

// Our Modul Functions:
// constant kc factor
int kc_factor_const(kc_factor* self)
{
    expertn_modul_base* xpn = &(self->parent);
    xpn->pWa->kc_factor =
        xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list, "Config.const.kc factor", 1.0);
    return RET_SUCCESS;
}

// Buchenwald ET0*0.75 < ET0 x LAI/3 < ET0*1.3 Basierend auf Haude-Werten //20151104  Christian Biernath <biernath.christian@gmail.com>
int kc_factor_LAI3rd_load(kc_factor* self) 
{
	expertn_modul_base* xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PEVAP  pEV = xpn->pWa->pEvap;
	
	double ET0_min;
	double ET0_max;
	double vLAI;
	
	ET0_min = xpn->pWa->fPotETdt *(double) 0.75;		//Minimalwert von ET0 basierend auf Haudefaktoren für Buchenwald
	ET0_max = xpn->pWa->fPotETdt * (double) 1.3;		//Maximalwert von ET0 basierend auf Haudefaktoren für Buchenwald
	vLAI = xpn->pPl->pCanopy->fLAI / (double) 3.0;	//Annahme EP zur Reduktion des Wassers im Bodenprofil
	xpn->pWa->kc_factor = max(min(xpn->pWa->fPotETdt*vLAI,ET0_max),ET0_min); //ET0 wird von ET0_min und ET0_max Begrenzt
	
	return RET_SUCCESS;
}

int kc_factor_LAI3rd_run(kc_factor* self)
{
	expertn_modul_base* xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PEVAP  pEV = xpn->pWa->pEvap;
	
	double ET0_min;
	double ET0_max;
	double vLAI;
	
	ET0_min = xpn->pWa->fPotETdt *(double) 0.75;		//Minimalwert von ET0 basierend auf Haudefaktoren für Buchenwald
	ET0_max = xpn->pWa->fPotETdt * (double) 1.3;		//Maximalwert von ET0 basierend auf Haudefaktoren für Buchenwald
	vLAI = xpn->pPl->pCanopy->fLAI / (double) 3.0;	//Annahme EP zur Reduktion des Wassers im Bodenprofil
	//xpn->pWa->kc_factor = max(min(xpn->pWa->fPotETdt*vLAI,ET0_max),ET0_min); //ET0 wird von ET0_min und ET0_max Begrenzt
	xpn->pWa->kc_factor = max(min(vLAI,1.3),0.75);
	
	return RET_SUCCESS;
}

// development stage dependent kc factor
int kc_factor_dev_stage_load(kc_factor* self)
{
    expertn_modul_base* xpn = &(self->parent);
    char* S;
    char* ini_filename;

    self->crop = NULL;
    self->crop_size = 0;

    S = xpn_register_var_get_pointer(xpn->pXSys->var_list, "Config.dev stage.kc_param_file");
    if(S == NULL) {
        S = xpn_register_var_get_pointer(xpn->pXSys->var_list, "Global_Config.options.kc_param_file");
        if(S == NULL) {
            S = g_strdup_printf("Entry 'Config.dev stage.kc_param_file' or 'Global_Config.options.kc_param_file' is "
                                "missing in your options!");
            PRINT_ERROR(S);
            g_free(S);
            return -1;
        }
    }

    if(S != NULL) {
        ini_filename = expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn, S);
        return kc_factor_read_dev_file(self, ini_filename);
        free(ini_filename);
    }

    return RET_SUCCESS;
}

int kc_factor_read_dev_file(kc_factor* self, char* filename)
{
    expertn_modul_base* xpn = &(self->parent);
    PPLANT pPl = xpn->pPl;
    PGENOTYPE pGe = pPl->pGenotype;

    GKeyFile* keyfile;
    GKeyFileFlags flags;
    GError* error = NULL;

    char** varietynames;
    char* S;
    int varietynum;
    int i;
    int col;
    int plant_nr;

    char** DevKC_arr, **KC_arr;
    char** else_arr;
    int DevKC_arr_size, KC_arr_size, else_arr_size;
    char* DevKC_str, *KC_str;
    char *else_str;

    keyfile = g_key_file_new();

    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Load the GKeyFile from keyfile.conf or return. */
    if(!g_key_file_load_from_file(keyfile, filename, flags, &error)) {
        PRINT_ERROR(error->message);
        return -1;
    }

    self->crop=NULL;
    self->crop_size = 0;
    
    //for(plant_nr = 0, pGe = pPl->pGenotype; pGe != NULL; pGe = pGe->pNext, plant_nr++)
    for(plant_nr = 0, pPl = xpn->pPl; pPl != NULL; pPl = pPl->pNext, plant_nr++)
    {                
        pGe = pPl->pGenotype;
        if(pGe->acCropName == NULL) {
            PRINT_ERROR_ID(xpn, "No Crop Code Name set!");
            return -1;
        }

        if(pGe->acVarietyName == NULL)
            pGe->acVarietyName = "Default";                
        
        
        self->crop_size++;
        self->crop = realloc_struct_kc_factor_crop_rot(self->crop,self->crop_size);
                
        
        self->crop[plant_nr].acCropName=g_strdup_printf("%s",pGe->acCropName);
        self->crop[plant_nr].acVarietyName=g_strdup_printf("%s",pGe->acVarietyName);        

        GET_INI_STRING_ARRAY(varietynames, varietynum, pGe->acCropName, "VarietyName");

        for(i = 0; i < varietynum; i++) {
            deleteSpaceBegEnd(varietynames[i]);
            if(strcmp(pGe->acVarietyName, varietynames[i]) == 0)
                break;
            if((i == varietynum - 1) && (strcmp(pGe->acVarietyName, varietynames[varietynum - 1]) != 0)) {
                char* S;
                i = 0;
                S = g_strdup_printf(
                    "Variety \"%s\" not found in %s: Default values are used.", pGe->acVarietyName, filename);
                PRINT_ERROR(S);
                g_free(S);
                break;
            }
        }
                
        col = i;
        g_strfreev(varietynames);

        GET_INI_STRING_ARRAY(DevKC_arr, DevKC_arr_size, pGe->acCropName, "DevKC");
        CHECK_LEN(DevKC_arr_size, varietynum)

        GET_INI_STRING_ARRAY(KC_arr, KC_arr_size, pGe->acCropName, "KC");
        CHECK_LEN(KC_arr_size, varietynum)

        GET_INI_STRING_ARRAY(else_arr, else_arr_size, pGe->acCropName, "else");
        CHECK_LEN(else_arr_size, varietynum)

        DevKC_str = DevKC_arr[col];
        KC_str = KC_arr[col];
        else_str = else_arr[col];

        self->crop[plant_nr].devkc_len = getStringColumn_get_Size(DevKC_str, ",");
        self->crop[plant_nr].devkc_arr = (double*)g_malloc0(self->crop[plant_nr].devkc_len * sizeof(double));

        S = (char*)g_malloc0(strlen(DevKC_str) + 1);
        for(i = 0; i < self->crop[plant_nr].devkc_len; i++) {
            getStringColumn(S, DevKC_str, ",", i);
            self->crop[plant_nr].devkc_arr[i] = atof(S);
        }
        g_free(S);

        self->crop[plant_nr].kc_len = getStringColumn_get_Size(KC_str, ",");
        self->crop[plant_nr].kc_arr = (double*)g_malloc0(self->crop[plant_nr].kc_len * sizeof(double));       

        S = (char*)g_malloc0(strlen(KC_str) + 1);
        for(i = 0; i < self->crop[plant_nr].kc_len; i++) {
            getStringColumn(S, KC_str, ",", i);
            self->crop[plant_nr].kc_arr[i] = atof(S);
        }
        g_free(S);
        
        if (getStringColumn_get_Size(else_str, ",")<2)
            {
                PRINT_ERROR_ID(xpn,"else value < 2");
                return 1;
            }
        self->crop[plant_nr].else_kc_arr = (double*)g_malloc0(2 * sizeof(double));
        S = (char*)g_malloc0(strlen(else_str) + 1);
        for(i = 0; i < 2; i++) {
            getStringColumn(S, else_str, ",", i);
            self->crop[plant_nr].else_kc_arr[i] = atof(S);
        }
        g_free(S);
                

        CHECK_LEN(self->crop[plant_nr].devkc_len, self->crop[plant_nr].kc_len);

        g_strfreev(DevKC_arr);
        g_strfreev(KC_arr);
        g_free(else_arr);      ;
    }

    g_key_file_free(keyfile);
    
    return RET_SUCCESS;
}

int kc_factor_dev_stage_run(kc_factor* self)
{
    int plant_nr;
    int i,devk_nr;
    double fStage;
    expertn_modul_base* xpn = &(self->parent);
    PPLANT pPl; 
    PGENOTYPE pGe;
    PMSOWINFO pSI;
    
    if ((self->crop==NULL) || (self->crop_size==0))
        {
            return RET_SUCCESS;
        }
        
    pPl = xpn->pPl;
    pGe = pPl->pGenotype;
    pSI	= xpn->pMa->pSowInfo;
    
    plant_nr = -1;
    for (i=0;i<self->crop_size;i++)
        {
            if ((strcmp(pGe->acCropName,self->crop[i].acCropName)==0) && (strcmp(pGe->acVarietyName,self->crop[i].acVarietyName)==0))
                {
                    plant_nr = i;
                    break;
                }
        }
    if (plant_nr==-1)
        {
            char *S;
            S = g_strdup_printf("Error in kc factor ...? No Cropname found for: %s,%s",pGe->acCropName,pGe->acVarietyName);
            PRINT_ERROR_ID(xpn,S);
            g_free(S);
            return -1;
        }
        
    fStage = pPl->pDevelop->fStageSUCROS;
    
    
    devk_nr = -1;
    for (i=0;i<self->crop[plant_nr].devkc_len-1;i++)
        {
            if ((fStage >= self->crop[plant_nr].devkc_arr[i]) && (fStage < self->crop[plant_nr].devkc_arr[i+1]))
            {
                devk_nr=i;
                break;
            }                
        }
    
    
   // if (xpn_time_compare_date(xpn->pTi->pSimTime->year,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,pSI->Year,pSI->Month,pSI->Day)<0)
   //SG 20190626: use bare soil value (else_kc_arr[0]) until field emergence (including phase from sowing (fStageSUCROS= -1.0) until emergence (fStageSUCROS= 0.0) 
    if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,pSI->Year,pSI->Month,pSI->Day)<0)||(pPl->pDevelop->fStageSUCROS<= 0.0))
        {
            xpn->pWa->kc_factor=self->crop[plant_nr].else_kc_arr[0];            
        }
    else
    if ((xpn_time_compare_date(xpn->pTi->pSimTime->year,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,xpn->pPl->pModelParam->HarvestYear,xpn->pPl->pModelParam->HarvestMonth,xpn->pPl->pModelParam->HarvestDay)>=0)||(pPl->pDevelop->fStageSUCROS>= 2.0))
        {
            xpn->pWa->kc_factor=self->crop[plant_nr].else_kc_arr[1];
        }
    else
        {
            double m,t;
            double x1,x2,y1,y2;
            y2 = self->crop[plant_nr].kc_arr[devk_nr+1];
            y1 = self->crop[plant_nr].kc_arr[devk_nr];
            x2 = self->crop[plant_nr].devkc_arr[devk_nr+1];
            x1 = self->crop[plant_nr].devkc_arr[devk_nr];
            
            m = (y2-y1)/(x2-x1);
            t = y1-m*x1;
            xpn->pWa->kc_factor= m*fStage+t;
            
            //printf("%f %f \n",  xpn->pWa->kc_factor,  fStage);
            
            //m = (self->crop[plant_nr].devkc_arr[devk_nr+1]-self->crop[plant_nr].devkc_arr[devk_nr])/(self->crop[plant_nr].kc_arr[devk_nr+1]-self->crop[plant_nr].kc_arr[devk_nr]);
            //t = self->crop[plant_nr].devkc_arr[devk_nr+1]-m*self->crop[plant_nr].kc_arr[devk_nr+1];
            
            
            
            //printf("%f,%f,%f,%f\n",self->crop[plant_nr].kc_arr[devk_nr+1],self->crop[plant_nr].kc_arr[devk_nr],self->crop[plant_nr].devkc_arr[devk_nr+1],self->crop[plant_nr].devkc_arr[devk_nr]);
            //xpn->pWa->kc_factor= (self->crop[plant_nr].kc_arr[devk_nr+1] - self->crop[plant_nr].kc_arr[devk_nr])*fStage/(self->crop[plant_nr].devkc_arr[devk_nr+1] - self->crop[plant_nr].devkc_arr[devk_nr]);
        }
    
	//if (NewDay(xpn->pTi))
	//	printf("%f \n", xpn->pWa->kc_factor);
	
    return RET_SUCCESS;
}

int kc_factor_dev_stage_done(kc_factor* self)
{
    self->crop = free_struct_kc_factor_crop_rot(self->crop,&(self->crop_size));
    return RET_SUCCESS;
}

struct_kc_factor_crop_rot *realloc_struct_kc_factor_crop_rot(struct_kc_factor_crop_rot* crop, int size)
{
    crop = (struct_kc_factor_crop_rot*)g_realloc(crop, size*sizeof(struct_kc_factor_crop_rot));
    crop[size - 1].acCropName = NULL;
    crop[size - 1].acVarietyName = NULL;
    crop[size - 1].devkc_arr = NULL;
    crop[size - 1].kc_arr = NULL;
    crop[size - 1].devkc_len = 0;
    crop[size - 1].kc_len = 0;
    crop[size - 1].else_kc_arr = NULL;
    return crop;
}



struct_kc_factor_crop_rot* free_struct_kc_factor_crop_rot(struct_kc_factor_crop_rot* crop, int* len)
{
    int i;
    if(crop == NULL) {
        *len = 0;
        return NULL;
    }
    for(i = 0; i < (*len); i++) {
        if(crop[i].devkc_arr != NULL)
            g_free(crop[i].devkc_arr);
        if(crop[i].kc_arr != NULL)
            g_free(crop[i].kc_arr);
        if(crop[i].else_kc_arr != NULL)
            g_free(crop[i].else_kc_arr);
        if(crop[i].acCropName != NULL)
            g_free(crop[i].acCropName);
        if(crop[i].acVarietyName != NULL)
            g_free(crop[i].acVarietyName);
    }
    g_free(crop);
    *len = 0;
    return NULL;
}
