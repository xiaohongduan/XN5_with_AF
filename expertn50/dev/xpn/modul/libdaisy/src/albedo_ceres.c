//      albedo_ceres.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "albedo_ceres.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(albedo_ceres, albedo_ceres, EXPERTN_MODUL_BASE_TYPE);


static void albedo_ceres_class_init(albedo_ceresClass *klass){}


static void albedo_ceres_init(albedo_ceres *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL ALBEDO_CERES!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
}



// Our Modul Functions:

int albedo_ceres_Water(albedo_ceres *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	xpn->pPl->pPltClimate->fPlantAlbedo = 0.0;
	xpn->pSo->fSoilAlbedo =  0.05;
	xpn->pSo->fAlbedo = xpn->pSo->fSoilAlbedo + xpn->pPl->pPltClimate->fPlantAlbedo;
	return RET_SUCCESS;
}

int albedo_ceres_Vegetation(albedo_ceres *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	xpn->pSo->fSoilAlbedo =  0.0;
	xpn->pPl->pPltClimate->fPlantAlbedo = 0.25;
	xpn->pSo->fAlbedo = xpn->pSo->fSoilAlbedo + xpn->pPl->pPltClimate->fPlantAlbedo;
	return RET_SUCCESS;
}


int albedo_ceres_pasture(albedo_ceres *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	double theta;
	xpn->pPl->pPltClimate->fPlantAlbedo = 0.23;
	theta = xpn->pWa->pWLayer->pNext->fContAct;
	if (theta <= 0.10)
		{
			xpn->pSo->fSoilAlbedo =  0.25;
		} 
	else if ((theta > 0.10) && (theta <= 0.25))
		{
			xpn->pSo->fSoilAlbedo =  0.35 - theta;
		}
	else // theta > 0.25
		{
			xpn->pSo->fSoilAlbedo =  0.10;
		}
			
	xpn->pSo->fAlbedo = xpn->pPl->pPltClimate->fPlantAlbedo - (xpn->pPl->pPltClimate->fPlantAlbedo - xpn->pSo->fSoilAlbedo) * exp(-0.75*xpn->pPl->pCanopy->fLAI);
	return RET_SUCCESS;
}

int albedo_ceres_bare_soil(albedo_ceres *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	double theta;
	xpn->pPl->pPltClimate->fPlantAlbedo = 0.0;
	theta = xpn->pWa->pWLayer->pNext->fContAct;
	if (theta <= 0.10)
		{
			xpn->pSo->fSoilAlbedo =  0.25;
		} 
	else if ((theta > 0.10) && (theta <= 0.25))
		{
			xpn->pSo->fSoilAlbedo =  0.35 - theta;
		}
	else // theta > 0.25
		{
			xpn->pSo->fSoilAlbedo =  0.10;
		}
			
	xpn->pSo->fAlbedo = xpn->pSo->fSoilAlbedo;
	return RET_SUCCESS;
}

