//      century_n.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "century_n.h"


#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <xpn_register_var.h>

// Die Variable wird nur übergeben, falls der Key Vorhanden ist
#define GET_INI_DOUBLE(var,groupname,key) \
						{\
						double oldvar;\
						oldvar=var;\
						error = NULL; \
						var = g_key_file_get_double (keyfile,groupname,key,&error); \
						if (error!=NULL) \
						{ \
							var = oldvar;\
						} \
						}


		

// Die Variable wird nur übergeben, falls der Key Vorhanden ist
#define GET_INI_GET_DOUBLE_LIST(var2,list_len,groupname,key) \
	{\
	char* var;\
	error = NULL; \
	var2=NULL;\
	list_len=0;\
	var = g_key_file_get_string (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			if (var!=NULL) g_free(var);\
			var2=NULL;\
			list_len=0;\
		} else if (var!=NULL)\
		{\
			int i;\
			char *S2;\
			list_len = getStringColumn_get_Size(var,";");\
			var2=(double*)g_malloc0(sizeof(double)*list_len);\
			S2 = (char*)g_malloc(strlen(var)+1);\
			for (i=0;i<list_len;i++)\
			{\
				if (getStringColumn(S2,var,";",i)==0)\
				{\
					var2[i] = atof(S2);\
				}\
			}\
			g_free(S2);\
			g_free(var);\
		}\
	}
	 
	 

G_DEFINE_TYPE(century_n, century_n, EXPERTN_MODUL_BASE_TYPE);


static void century_n_class_init(century_nClass *klass) {}


static void century_n_init(century_n *self)
{
	int i;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
// print Output Message if Modul is used from Expert N


	self->CumCorrection=0.0;
	memset (&(self->fixed),0,sizeof(struct_fixedparameter));


	for (i=0; i<12; i++)
		{
			self->NOxPulse[i]=1.0;
		}
	self->kPrecipNO = self->NOxPulse[0];
	self->fPreciRate=0.0;

	for (i=0; i<12; i++)
		{
			self->dNOxPulse[i]=1.0;
		}
	self->dkPrecipNO = self->dNOxPulse[0];
	self->INIT_DONE=0;
	self->dfPreciRate=0.0;
	self->config_filename=NULL;

	self->iForest=0;
	self->fdrain=1.0;

	self->INIT_DONE=0;
}

void century_n__general_init(century_n *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *ini_filename;

	if (self->INIT_DONE!=0)
		{
			return;
		}
	else
		{
			self->INIT_DONE=1;
		}

	self->iForest = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.century_n.iforest",0);
	self->fdrain = xpn_register_var_get_pointer_convert_to_double(xpn->pXSys->var_list,"Config.century_n.fdrain",1.0);

	// Read from INI File:

	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.century_n.ini_filename");

	if (ini_filename==NULL)
		{
			PRINT_ERROR_ID((&(self->parent)),"Missing entry 'hpm.ini_filename' in your configuration!");
		}
	else
		{

			char *S2;
			S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
			if (S2!=NULL)
				{
					ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
					if (century_n_load_config(self,ini_filename)!=0)
						{
							PRINT_ERROR_ID((&(self->parent)),"Error Read CENTURY INI File");
						}
					free(ini_filename);
					free(S2);
				}

		}

}



int century_n_load_config(century_n *self,const char* configfile)
{
	expertn_modul_base *xpn = &(self->parent);
	PCPROFILE pCProfile = xpn->pCh->pCProfile;	
	GError *error = NULL;
	PCLAYER pCLayer;
	PSLAYER pSLayer;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	double *fCLitter,*fNLitter,*fCManure,*fNManure,*fCHumus,*fNHumus;
	int fCLitter_len,fNLitter_len,fCManure_len,fNManure_len,fCHumus_len,fNHumus_len;
	int i;


	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, configfile, flags, &error))
		{
			PRINT_ERROR_ID((&(self->parent)),error->message);
			return -1;
		}


	//GET_INI_DOUBLE(self->Control.water,"Control","water");


	//This Label is needed for forest systems. Grassland/Crop simulations don’t need this. (xnm file Marke 20000)
	if (self->iForest==1)
		{
			GET_INI_DOUBLE(pCProfile->fCBranchLitterSurf,"forest","fCBranchLitterSurf");
			GET_INI_DOUBLE(pCProfile->fNBranchLitterSurf,"forest","fNBranchLitterSurf");

			GET_INI_DOUBLE(pCProfile->fCStemLitterSurf,"forest","fCStemLitterSurf");
			GET_INI_DOUBLE(pCProfile->fNStemLitterSurf,"forest","fNStemLitterSurf");

			GET_INI_DOUBLE(pCProfile->fLigFracLeaf,"forest","fLigFracLeaf");
			GET_INI_DOUBLE(pCProfile->fLigFracBranch,"forest","fLigFracBranch");
			GET_INI_DOUBLE(pCProfile->fLigFracStem,"forest","fLigFracStem");
			GET_INI_DOUBLE(pCProfile->fLigFracFineRoot,"forest","fLigFracFineRoot");
			GET_INI_DOUBLE(pCProfile->fLigFracCoarseRoot,"forest","fLigFracCoarseRoot");

			GET_INI_DOUBLE(pCProfile->fFineBranchDecR,"forest","fFineBranchDecR");
			GET_INI_DOUBLE(pCProfile->fLargeWoodDecR,"forest","fLargeWoodDecR");
			GET_INI_DOUBLE(pCProfile->fCoarseRootDecR,"forest","fCoarseRootDecR");
		}
		
		
	// C and N content of Soil Layers (aus xnm file Marke 20003)
	GET_INI_GET_DOUBLE_LIST(fCLitter,fCLitter_len,"litter","fCLitter");
	GET_INI_GET_DOUBLE_LIST(fNLitter,fNLitter_len,"litter","fNLitter");
	
	GET_INI_GET_DOUBLE_LIST(fCManure,fCManure_len,"litter","fCManure");
	GET_INI_GET_DOUBLE_LIST(fNManure,fNManure_len,"litter","fNManure");
	
	GET_INI_GET_DOUBLE_LIST(fCHumus,fCHumus_len,"litter","fCHumus");
	GET_INI_GET_DOUBLE_LIST(fNHumus,fNHumus_len,"litter","fNHumus");
	
	
	for (i=0,pCLayer=xpn->pCh->pCLayer->pNext,pSLayer=xpn->pSo->pSLayer->pNext;pCLayer->pNext!=NULL;pCLayer=pCLayer->pNext,pSLayer=pSLayer->pNext,i++)
	{
		if (i<=(fCLitter_len-1)) pCLayer->fCLitter = fCLitter[i];
		if (i<=(fNLitter_len-1)) pCLayer->fNLitter = fNLitter[i];
		
		if (i<=(fCManure_len-1)) pCLayer->fCManure = fCManure[i];
		if (i<=(fNManure_len-1)) pCLayer->fNManure = fNManure[i];
		
		if (i<=(fCHumus_len-1)) pSLayer->fCHumus = fCHumus[i];
		if (i<=(fNHumus_len-1)) pSLayer->fNHumus = fNHumus[i];
	}
	
	if (fCLitter!=NULL) g_free(fCLitter);
	if (fNLitter!=NULL) g_free(fNLitter);
	if (fCManure!=NULL) g_free(fCManure);
	if (fNManure!=NULL) g_free(fNManure);
	if (fCHumus!=NULL) g_free(fCHumus);
	if (fNHumus!=NULL) g_free(fNHumus);
	
	// For the estimation of Humus C and N, see the ’Century Workbook’ Chap. 2. Label ’20004’ in *.xnm file
	GET_INI_DOUBLE(pCProfile->fCLitterSurf,"surface","fCLitterSurf");
	GET_INI_DOUBLE(pCProfile->fNLitterSurf,"surface","fNLitterSurf");
	
	GET_INI_DOUBLE(pCProfile->fNManureSurf,"surface","fNManureSurf");
	GET_INI_DOUBLE(pCProfile->fCManureSurf,"surface","fCManureSurf");
	
	GET_INI_DOUBLE(pCProfile->fCHumusSurf,"surface","fCHumusSurf");
	GET_INI_DOUBLE(pCProfile->fNHumusSurf,"surface","fNHumusSurf");
	
	g_key_file_free(keyfile);

	return RET_SUCCESS;
}
