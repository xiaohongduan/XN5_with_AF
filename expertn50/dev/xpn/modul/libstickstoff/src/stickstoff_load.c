#include "stickstoff.h"
#include "stickstoff_macros.h"


int stickstoff_load_ini(stickstoff *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *ini_filename;
	
	if (self->ini_filename==NULL)
		{
			// Read from INI File:
			ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.stickstoff.ini_filename");

			if (ini_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'stickstoff.ini_filename' in your configuration!");
				}
			else
				{

					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
					if (S2!=NULL)
						{
							ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->ini_filename = g_strdup_printf("%s",ini_filename);
							stickstoff_load_config(self);
							free(ini_filename);
							free(S2);
						}

				}

		}

	return RET_SUCCESS;
}


int stickstoff_load_config(stickstoff *self)
{
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	int layers_len;
	int *layers;
	double *var;
	int var_len;
	
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


	//TRANSFORMATION RATES MODEL PARAMETERS
	//20002
	//[transformation]
	GET_INI_INT_ARRAY(layers,layers_len,"transformation","layers");
	
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fUreaHydroMaxR,"urea_hydrolysis","transformation",PCLAYER,xpn->pCh->pCLayer);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNH4NitrMaxR,"nitrification","transformation",PCLAYER,xpn->pCh->pCLayer);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNO3DenitMaxR,"denitrification","transformation",PCLAYER,xpn->pCh->pCLayer);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fLitterMinerMaxR,"mineralisation_lit","transformation",PCLAYER,xpn->pCh->pCLayer);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fManureMinerMaxR,"mineralisation_man","transformation",PCLAYER,xpn->pCh->pCLayer);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fHumusMinerMaxR,"mineralisation_hum","transformation",PCLAYER,xpn->pCh->pCLayer);
	
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
	
	
	G_FREE_IF_NOT_0(layers);
	layers_len=0;
	
	
	//Nitrification MODEL PARAMETERS
	//?????
	//[nitrification]
	GET_INI_INT_ARRAY(layers,layers_len,"nitrification","layers");
	
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fNitrifNH4ToN2O,"nh4_to_n20","nitrification",PCPARAM,xpn->pCh->pCParam);
	
	G_FREE_IF_NOT_0(layers);
	layers_len=0;
	
	
	
	//DENITRIFICATION MODEL PARAMETER
	//marker: 20011
	//[denitrification]
	
	GET_INI_DOUBLE(self->fN2Fraction,"denitrification","fn2Fraction")
	GET_INI_DOUBLE(self->fN2OReduction,"denitrification","fn2oeduction")
	GET_INI_INT(self->iRewet,"denitrification","irewet")
	GET_INI_INT(self->iNO3Kin,"denitrification","ino3kin")

	
	
	GET_INI_INT_ARRAY(layers,layers_len,"denitrification","layers");
	
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitKsNO3,"ksno3","denitrification",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitKsCsol,"ksc","denitrification",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitThetaMin,"theta0","denitrification",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitMaxGrowR,"biogrowth","denitrification",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fMaintCoeff,"bio_mc","denitrification",PCPARAM,xpn->pCh->pCParam);
	
	G_FREE_IF_NOT_0(layers);
	layers_len=0;
	
	
	
	//EVENTS MODEL PARAMETERS
	//marker: 20012
	//[denitrification events]
	GET_INI_INT_ARRAY(layers,layers_len,"denitrification events","layers");
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fN2ORedMaxR,"n2o_n2","denitrification events",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitFreeze,"freezing","denitrification events",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitThaw,"thawing","denitrification events",PCPARAM,xpn->pCh->pCParam);
	GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(fDenitReWet,"rewet","denitrification events",PCPARAM,xpn->pCh->pCParam);
	
	
	G_FREE_IF_NOT_0(layers);
	layers_len=0;


	g_key_file_free(keyfile);
	g_free(filename);

	return RET_SUCCESS;
}
