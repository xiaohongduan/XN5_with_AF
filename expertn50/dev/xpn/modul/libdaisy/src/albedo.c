//      albedo.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "albedo.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



G_DEFINE_TYPE(albedo, albedo, EXPERTN_MODUL_BASE_TYPE);


static void albedo_class_init(albedoClass *klass){}


static void albedo_init(albedo *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
/*self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
S  = g_strdup_printf("%s\t%d\tInit MODUL ALBEDO!",__FILE__,__LINE__);
fprintf(stdout,"%s\n",S);
fflush(stdout);
g_free(S);*/
}

typedef struct
{
	int number;
	double value;
} STRUCT_ALB;

STRUCT_ALB *ReadConfigFile(char* Name,int *list_size)
{
	FILE *f;
	int i,size;
	char *S,*S2;
	STRUCT_ALB *list;
	// Größe Ermitteln
	size=0;
	(*list_size)=0;
	f = fopen(Name,"r");
	if (f==NULL)
		{
			S = g_strdup_printf("File not found: %s",Name);
			PRINT_ERROR(S);
			g_free(S);
			return NULL;
		}
	while (1)
		{
			S = fgets_mem(f);
			if (S==NULL) break;
			deleteSpaceBegEnd(S);
			if ((strlen(S) < 3) || (S[0]=='#') || (getStringColumn_get_Size(S,",")<3))
				{
					free(S);
					continue;
				}
			size++;
			free(S);							
		}
	fclose(f);
	
	(*list_size) = size;
	list = (STRUCT_ALB*)calloc(1,size*sizeof(STRUCT_ALB));
	// Datei Auslesen:
	f = fopen(Name,"r");
	i=0;
	while (1)
		{
			S = fgets_mem(f);
			if (S==NULL) break;
			deleteSpaceBegEnd(S);
			if ((strlen(S) < 3) || (S[0]=='#') || (getStringColumn_get_Size(S,",")<3))
				{
					free(S);
					continue;
				}
			S2 = (char*)malloc(strlen(S)+1);
			getStringColumn(S2,S,",",0);
			list[i].number=atoi(S2);
			getStringColumn(S2,S,",",1);
			list[i].value=atof(S2);
			i++;
            free(S);
            free(S2);
		}
	fclose(f);
	return list;
}

double get_Value_From_List(STRUCT_ALB *list,int list_size,int number)
{
	int i;
	for (i=0;i<list_size;i++)
		{					
			if (list[i].number==number)
				{
					return list[i].value;
				}
		}
	PRINT_ERROR("No Albedo defined!");
	return 0.0;
}

int albedo_load(albedo *self)
{	
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *plant_albedo_filename ;
	STRUCT_ALB *plant_alb_list;
	int plant_alb_list_len;
	
	plant_albedo_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.Albedo.plant_cfg"); 
			
	if (plant_albedo_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'Config.Albedo.plant_cfg' in your configuration!");
					return 1;
				} else
				{
				
					plant_albedo_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, plant_albedo_filename);										
				}
	
	plant_alb_list = ReadConfigFile(plant_albedo_filename,&plant_alb_list_len);
	
	self->Plant_Type = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.Albedo.plant_type",0);
	
	xpn->pPl->pPltClimate->fPlantAlbedo = get_Value_From_List(plant_alb_list,plant_alb_list_len,self->Plant_Type);
	
	  // Albedo
	if (xpn->pWa->pWLayer->pNext->fContAct >= 0.25)
      {xpn->pSo->fSoilAlbedo =  0.1;}
      
   else if (xpn->pWa->pWLayer->pNext->fContAct <= 0.1)
      {xpn->pSo->fSoilAlbedo =  0.25;}
      
   else
      {xpn->pSo->fSoilAlbedo =  0.35 - xpn->pWa->pWLayer->pNext->fContAct;}
    
   // Gesamt-Albedo
  
	
	free(plant_alb_list);
	free(plant_albedo_filename);
	
	self->wrf = NULL;
	self->wrf=(struct_WRF*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_Noah");
	
	return RET_SUCCESS;
}

int albedo_run(albedo *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	double plant_alb,soil_alb,LAI;
	
		  // Albedo
	if (xpn->pWa->pWLayer->pNext->fContAct >= 0.25)
      {xpn->pSo->fSoilAlbedo =  0.1;}
      
   else if (xpn->pWa->pWLayer->pNext->fContAct <= 0.1)
      {xpn->pSo->fSoilAlbedo =  0.25;}
      
   else
      {xpn->pSo->fSoilAlbedo =  0.35 - xpn->pWa->pWLayer->pNext->fContAct;}
	
	plant_alb = xpn->pPl->pPltClimate->fPlantAlbedo;
	soil_alb = xpn->pSo->fSoilAlbedo;
	LAI = xpn->pPl->pCanopy->fLAI;
	
	// bei LAI = 2 ist die exp(-0.75*2)=0.2
	xpn->pSo->fAlbedo = plant_alb - (plant_alb - soil_alb) * exp(-0.75*LAI);
	C_DEBUG(LAI);
	
	return RET_SUCCESS;
}

int albedo_load_const(albedo *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	xpn->pSo->fAlbedo = xpn_register_var_get_pointer_convert_to_double(xpn->pXSys->var_list,"Config.Albedo const.albedo",0.25);
	xpn->pSo->fSoilAlbedo = 0.0;
	xpn->pPl->pPltClimate->fPlantAlbedo = xpn->pSo->fAlbedo;
	return RET_SUCCESS;
}

/*

	// Der Bodenalbedo der ersten Bodenschicht muss initialisiert
	//   werden. Dies geschieht in abhaengigkeit von der Bodenart und
	//   dem Humusgehalt der ersten Bodenschicht.


		if ((pSo->pSLayer->pNext->fHumus) <= (float)2.0)
		{
			if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"L\0"))
			{   
				pSo->fSoilAlbedo = (float)0.13;
			}
			else
			{   
				if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"S\0"))
				{   
					pSo->fSoilAlbedo = (float)0.13;
				}
				else
				{   
					if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"l\0"))
					{   
						pSo->fSoilAlbedo = (float)0.13;
					}
					else
					{   
						if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"l\0"))
						{   
							pSo->fSoilAlbedo = (float)0.13;
						}
						else
						{   
							pSo->fSoilAlbedo = (float)0.19;
						}	
					}	
				}		
			}
		}
		else
		{
			if ((pSo->pSLayer->pNext->fHumus) <= (float)5.0)		
				pSo->fSoilAlbedo = (float)0.13;		
			if (((pSo->pSLayer->pNext->fHumus) > (float)5.0)&&
				((pSo->pSLayer->pNext->fHumus) <= (float)10.0))
				pSo->fSoilAlbedo = (float)0.11;		
			if ((pSo->pSLayer->pNext->fHumus) > (float)10.0)		
				pSo->fSoilAlbedo = (float)0.08;		
		}	
*/
