
#include "daisy_miner_load.h"
#include "stickstoff_macros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/*
G_DEFINE_TYPE(daisy_miner, daisy_miner, EXPERTN_MODUL_BASE_TYPE);


static void daisy_miner_class_init(daisy_minerClass *klass){}


static void daisy_miner_init(daisy_miner *self)
{
}
*/

int daisy_miner_load(daisy_miner *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self); 
	 // expertn_modul_base *xpn = &(self->parent);
	
	daisy_miner_load_ini(self);
	
	
	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR("Problem with hydraulic functions!");
		}
		

		
		
	PRINT_MESSAGE(xpn,4,"daisy_miner geladen!")
	return RET_SUCCESS;
}



int daisy_miner_load_ini(daisy_miner *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *ini_filename;
	
	self->fCDiffSum  = (double)0.0; // added by Hong 20170517
    self->fNDiffSum  = (double)0.0;
	
	if (self->ini_filename==NULL)
		{
			// Read from INI File:
			ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.daisy_miner.ini_filename");


			if (ini_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'daisy_miner.ini_filename' in your configuration!");
				}
			else
				{

					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
					if (S2!=NULL)
						{
							ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->ini_filename = g_strdup_printf("%s",ini_filename);
							daisy_miner_load_config(self);
							free(ini_filename);
							free(S2);
						}

				}

		}

	return RET_SUCCESS;
}


int daisy_miner_load_config(daisy_miner *self)
{
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	PCLAYER		pCL; //Test of Hong
	PSLAYER		pSL;
	
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int layers_len;
	int *layers;
	double *var;
	int var_len;
	int i;
	
	var = NULL;
	var_len=0;
	layers_len=0;
	layers=NULL;


	filename=g_strdup_printf("%s",self->ini_filename);

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}
		


G_FREE_IF_NOT_0(layers);
	layers_len=0;

//[start values daisy]
GET_INI_INT_ARRAY(layers,layers_len,"start values daisy","layers");
//20008
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCFOMSlow,"AOM1_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCFOMFast,"AOM2_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCFOMVeryFast,"AOM3_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCMicBiomSlow,"BOM1_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCMicBiomFast,"BOM2_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCMicBiomDenit,"BOMD_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCHumusStable,"SOM0_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCHumusSlow,"SOM1_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCHumusFast,"SOM2_C","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCsolC,"Csol","start values daisy",PCLAYER,xpn->pCh->pCLayer);

//START SOIL VALUES C-, N-POOLS
     //20003, 20008, 20009
     //[start values general]
//20003
GET_INI_INT_ARRAY(layers,layers_len,"start values general","layers");
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCLitter,"c_litter","start values general",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNLitter,"n_litter","start values general",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCManure,"c_manure","start values general",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNManure,"n_manure","start values general",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fCHumus,"c_humus","start values general",PSLAYER,xpn->pSo->pSLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNHumus,"n_humus","start values general",PSLAYER,xpn->pSo->pSLayer);

//Added by Hong on 20190703: if [start values general] = -99.0
	 for (pSL=xpn->pSo->pSLayer->pNext,pCL = xpn->pCh->pCLayer->pNext, i=0; pSL!=NULL,pCL!=NULL; pSL=pSL->pNext, pCL=pCL->pNext,i++)
		{
		if (pCL->fCLitter<0 ||pCL->fNLitter<0)
		   {
			   pCL->fCLitter = pCL->fCFOMSlow; 
               pCL->fNLitter = pCL->fNFOMSlow; 
		   }
	    if (pCL->fCManure <0 || pCL->fNManure <0)
           {
			   pCL->fCManure = pCL->fCFOMFast; 
               pCL->fNManure = pCL->fNFOMFast; 
		    }
	
		if (pSL->fCHumus <0 ||pSL->fNHumus<0)
            {
				pSL->fCHumus = pCL->fCMicBiomSlow + pCL->fCMicBiomFast			
                   + pCL->fCHumusSlow + pCL->fCHumusFast; 
				pSL->fNHumus = pCL->fNMicBiomSlow + pCL->fNMicBiomFast			
                   + pCL->fNHumusSlow + pCL->fNHumusFast;   
				   
				  }				   
	     }
//End of Hong

//20009
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNFOMSlow,"AOM1_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNFOMFast,"AOM2_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNFOMVeryFast,"AOM3_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNMicBiomSlow,"BOM1_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNMicBiomFast,"BOM2_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNMicBiomDenit,"BOMD_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNHumusStable,"SOM0_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNHumusSlow,"SOM1_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNHumusFast,"SOM2_N","start values daisy",PCLAYER,xpn->pCh->pCLayer);


G_FREE_IF_NOT_0(layers);
	layers_len=0;

//Hong
     //TRANSFORMATION RATES DAISY MODEL PARAMETERS
     //20007
     //[transformation daisy]
GET_INI_INT_ARRAY(layers,layers_len,"transformation daisy","layers");
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fFOMSlowDecMaxR,"fom_slow","transformation daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fFOMFastDecMaxR,"fom_fast","transformation daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fFOMVeryFastDecMaxR,"fom_veryfast","transformation daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMicBiomSlowDecMaxR,"micbioms_slow","transformation daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMicBiomFastDecMaxR,"micbioms_fast","transformation daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fHumusSlowMaxDecMaxR,"humus_slow","transformation daisy",PCLAYER,xpn->pCh->pCLayer);
GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fHumusFastMaxDecMaxR,"humus_fast","transformation daisy",PCLAYER,xpn->pCh->pCLayer);

G_FREE_IF_NOT_0(layers);
	layers_len=0;

	
	//MINERALIZATION MODEL PARAMETERS
	//20001
	//[mineralisation]
	GET_INI_INT_ARRAY(layers,layers_len,"mineralisation","layers");
	
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMinerEffFac,"effic","mineralisation",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMinerHumFac,"humf","mineralisation",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMicBiomCN,"min_cn","mineralisation",PCLAYER,xpn->pCh->pCLayer);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMinerTempB,"temp0","mineralisation",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMinerQ10,"miner_q10","mineralisation",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMinerThetaMin,"theta0","mineralisation",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMinerSatActiv,"MinerSatActiv","mineralisation",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNitrifNO3NH4Ratio,"NitrifNO3NH4Ratio","mineralisation",PCPARAM,xpn->pCh->pCParam);
	
	//Hong: Temperaturabhängigkeit für DAISY Abbauraten (für Scott Demyan)
	GET_INI_INT(self->iTempCorr,"mineralisation","iTempCorr");	
	GET_INI_DOUBLE(self->fParCAOM1Q10,"mineralisation","CAOM1Q10");
	GET_INI_DOUBLE(self->fParCAOM2Q10,"mineralisation","CAOM2Q10");
	GET_INI_DOUBLE(self->fParCBOM1Q10,"mineralisation","CBOM1Q10");
	GET_INI_DOUBLE(self->fParCBOM2Q10,"mineralisation","CBOM2Q10");
	GET_INI_DOUBLE(self->fParCSOM1Q10,"mineralisation","CSOM1Q10");
	GET_INI_DOUBLE(self->fParCSOM2Q10,"mineralisation","CSOM2Q10");
	GET_INI_DOUBLE(self->fParDBOM1Q10,"mineralisation","DBOM1Q10");
	GET_INI_DOUBLE(self->fParDBOM2Q10,"mineralisation","DBOM2Q10");
	GET_INI_DOUBLE(self->fParMBOM1Q10,"mineralisation","MBOM1Q10");
	GET_INI_DOUBLE(self->fParMBOM2Q10,"mineralisation","MBOM2Q10");
	
	GET_INI_DOUBLE_OPTIONAL(self->fParBOM1,"mineralisation","fBOM1",0.5);
	GET_INI_DOUBLE_OPTIONAL(self->fParSOM1,"mineralisation","fSOM1",0.1);
	GET_INI_DOUBLE_OPTIONAL(self->fParSOM2,"mineralisation","fSOM2",0.4);
	GET_INI_DOUBLE_OPTIONAL(self->fParEff,"mineralisation","fEFF",0.6);
	
	GET_INI_DOUBLE(self->fEff_AOM1,"mineralisation","fEff_AOM1");
	GET_INI_DOUBLE(self->fEff_AOM2,"mineralisation","fEff_AOM2");
	GET_INI_DOUBLE(self->fEff_BOM1,"mineralisation","fEff_BOM1");
	GET_INI_DOUBLE(self->fEff_BOM2,"mineralisation","fEff_BOM2");
	GET_INI_DOUBLE(self->fEff_SOM1,"mineralisation","fEff_SOM1");
	GET_INI_DOUBLE(self->fEff_SOM2,"mineralisation","fEff_SOM2");
	
	//PRINT_MESSAGE(xpn,1,"neue Pameter geladen!")
	//End of Hong
	
	G_FREE_IF_NOT_0(layers);
	layers_len=0;
	

	g_key_file_free(keyfile);
	g_free(filename);

	return RET_SUCCESS;
}

int daisy_miner_done(daisy_miner *self)
{
	G_FREE_IF_NOT_0(self->ini_filename);
	return RET_SUCCESS;
}
