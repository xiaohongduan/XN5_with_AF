//      agroforestry.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//

#include "agroforestry.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


G_DEFINE_TYPE(agroforestry, agroforestry, EXPERTN_MODUL_BASE_TYPE);

static void agroforestry_class_init(agroforestryClass* klass)
{
}

static void agroforestry_init(agroforestry* self)
{
    //gchar* S;
    // hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
    // Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
    // Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

    // init example var:

    self->fAzimuthAngle = 0.0;
    self->fDiffAngle = 0.0;
    self->fMaxShadeLength = 0.0;
    self->fTreeHeight = 0.0;
    self->fViewAngle = 0.0;
    self->fZenithAngle = 0.0;
    self->__INIT_DONE = 0;

    // print Output Message if Modul is used from Expert N
    //S = g_strdup_printf("%s\t%d\tInit MODUL AGROFORESTRY!", __FILE__, __LINE__);
    //fprintf(stdout, "%s\n", S);
    //fflush(stdout);
    //g_free(S);
}

int agroforestry_done(agroforestry *self)
{
g_free(self->reg_string_tree);
g_free(self->reg_string_beside_tree);
g_free(self->fDistStripAll);
g_free(self->grid_id_beside_tree);
g_free(self->grid_i_beside_tree);
g_free(self->grid_j_beside_tree);
g_free(self->grid_sub_beside_tree);

return RET_SUCCESS;
}

/*#define CHECK_ZERO_ONE(var,inif) \
	if ((var < 0.0) || (var > 1.0)) \
		{ \
			gchar *S; \
			S = g_strdup_printf  ("%s is %f, but must be between 0.0 and 1.0, check %s!",#var,var,inif); \
			PRINT_ERROR(S); \
			g_free(S); \
		} \*/


int init_agroforestry(agroforestry *self)
{
	expertn_modul_base *xpn = &(self->parent);
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
	
	char *ini_filename;
	gchar *reg_string;
	gchar* S;
	int i, check;
	double Leaf_beside_frac_sum, Stem_beside_frac_sum, Branch_beside_frac_sum,FineRoot_beside_frac_sum,GrossRoot_beside_frac_sum;
	
//	PTIMESTEP		pTS = xpn->pTi->pTimeStep;
	//PPLANT			pPl = xpn->pPl;
	PCHEMISTRY pCh = xpn->pCh; //added by Hong
	//PNFERTILIZER fertil_first,fertil,fertil_act;

	// Wenn die Init bereits geladen ist
	if (self->__INIT_DONE==0) {
		self->__INIT_DONE=1;
	} else {
		return RET_SUCCESS;
	}
	
	
	// Read from INI File:
	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.agroforestry.filename");

	if (ini_filename==NULL) {
		PRINT_ERROR("Missing entry 'agroforestry.filename' in your configuration!");
	} else {

		char *S2;
		S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
		if (S2!=NULL) {
			ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
			if (agroforestry_read(self,ini_filename)!=0) {
				PRINT_ERROR("Error Read AGROFORESTRY INI File");
			}
			self->ini_filename = g_strdup_printf("%s",ini_filename);
			free(ini_filename);
			free(S2);
		}

	}
	
	self->reg_string_act = g_strdup_printf("%s",xpn->pXSys->reg_str);
	check = 0;
	
	Leaf_beside_frac_sum = 0.0;
    Stem_beside_frac_sum = 0.0;
	Branch_beside_frac_sum = 0.0;
	FineRoot_beside_frac_sum = 0.0;
	GrossRoot_beside_frac_sum = 0.0;
	//Allocate the variables to the individual grid points
	for(i = 0; i < self->grid_id_beside_tree_len; i++)
	{	
		self->reg_string_beside_tree = g_strdup_printf("%d_%d_%d_%d",self->grid_id_beside_tree[i],self->grid_i_beside_tree[i],self->grid_j_beside_tree[i],self->grid_sub_beside_tree[i]);
		Leaf_beside_frac_sum += self->DeadLeaf_beside_tree_array[i];
		Stem_beside_frac_sum += self->DeadStem_beside_tree_array[i];
		Branch_beside_frac_sum += self->DeadBranch_beside_tree_array[i];
		FineRoot_beside_frac_sum += self->DeadFineRoot_beside_tree_array[i];
		GrossRoot_beside_frac_sum += self->DeadGrossRoot_beside_tree_array[i];
		if (strcmp(self->reg_string_act,self->reg_string_beside_tree)==0)
		{
			self->fDistStrip = self->fDistStripAll[i];
			self->DeadLeaf_beside_tree_frac = self->DeadLeaf_beside_tree_array[i];
			self->DeadStem_beside_tree_frac = self->DeadStem_beside_tree_array[i];
			self->DeadFineRoot_beside_tree_frac = self->DeadFineRoot_beside_tree_array[i];
			self->DeadGrossRoot_beside_tree_frac = self->DeadGrossRoot_beside_tree_array[i];
/*			CHECK_ZERO_ONE(self->DeadLeaf_beside_tree_frac,self->ini_filename);
			CHECK_ZERO_ONE(self->DeadStem_beside_tree_frac,self->ini_filename);
			CHECK_ZERO_ONE(self->DeadRoot_beside_tree_frac,self->ini_filename);*/
			check = 1;
			break;
		}
	}	
	

	//printf("%f %f \n",self->DeadLeaf_beside_tree_frac, Leaf_beside_frac_sum);

	g_free(self->DeadLeaf_beside_tree_array);
	g_free(self->DeadStem_beside_tree_array);
	g_free(self->DeadBranch_beside_tree_array);
	g_free(self->DeadFineRoot_beside_tree_array);
	g_free(self->DeadGrossRoot_beside_tree_array);
	g_free(self->reg_string_act);	
	
	if(self->iTree == 1)
		{
		check = 1;
		self->DeadLeaf_tree_frac = 1 - Leaf_beside_frac_sum;
		self->DeadStem_tree_frac = 1 - Stem_beside_frac_sum;
		self->DeadBranch_tree_frac = 1 - Branch_beside_frac_sum;
		self->DeadFineRoot_tree_frac = 1 - FineRoot_beside_frac_sum;
		self->DeadGrossRoot_tree_frac = 1 - GrossRoot_beside_frac_sum;
		}
	

	
	// Check if grid point is included into agroforestry module
	if(check != 1)
	{
		S = g_strdup_printf("Actual grid point is not included into agroforestry configuration, check [grid_positions] in %s", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
		return -1;		
	}


	// Allocate internal Expert-N ID to the grid points and connect tree strip with the strips beside
	self->reg_string_tree = g_strdup_printf("%d_%d_%d_%d",self->grid_id_tree,self->grid_i_tree,self->grid_j_tree,self->grid_sub_tree);
	
	self->xn_id_tree = -99;
	self->xn_id_beside_tree = -99;



	for(i = 0; i < xpn->pXSys->id_N; i++)
	{	
		reg_string = g_strdup_printf(xpn_class[i]->pXSys->reg_str);
		//printf("%s %s \n", self->reg_string_beside_tree, reg_string);
		if (strcmp(reg_string,self->reg_string_tree)==0)
			{
			self->xn_id_tree = (int)xpn_class[i]->pXSys->id;
			S  = g_strdup_printf("Agroforestry: The internal Id of the tree strip is %d with reg string %s \n",i, reg_string);
			PRINT_MESSAGE(xpn,3,S);
			g_free(S);
			}
		if (strcmp(reg_string,self->reg_string_beside_tree)==0)
			{
			self->xn_id_beside_tree = (int)xpn_class[i]->pXSys->id;
			S  = g_strdup_printf("Agroforestry: The internal Id of the strip next to the trees is %d with reg string %s \n",i, reg_string);
			PRINT_MESSAGE(xpn,3,S);
			g_free(S);
			}
		g_free(reg_string);
		if ((self->xn_id_tree != -99) && (self->xn_id_beside_tree != -99))
			break;
	}
	
	
	if(self->xn_id_tree == -99)
	{
		S = g_strdup_printf("Error Grid position of tree strip is not correctly defined. Check %s at [grid_positions]", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
	}

	if(self->xn_id_beside_tree == -99)
	{
		S = g_strdup_printf("Error Grid position of strip next to trees is not correctly defined. Check %s at [grid_positions]", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
	}
	
	if (self->xn_id_beside_tree == self->xn_id_tree)
	{
		S = g_strdup_printf("Error Tree strip and the strip next to the trees must not have the same grid position. Check %s at [grid_positions]", self->ini_filename);
		PRINT_ERROR(S);
		g_free(S);
		return -1;
	}

	pCh->pCProfile->fUsedLeafWeight = 0.0;
	pCh->pCProfile->fUsedStemWeight = 0.0;
	pCh->pCProfile->fUsedRootWeight = 0.0;
	
	//printf("pCh->pCProfile->fUsedLeafWeight = 0.0 \n");


return RET_SUCCESS;
}


#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		} \

#define GET_INI_DOUBLE_OPTIONAL(var,groupname,key,std_value)\
{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (key_exists==FALSE) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (in file %s) is missing. Standard Value (%f) taken instead!\n",groupname,key,filename,std_value);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{\
				var = std_value;\
			} else\
			{\
				GET_INI_DOUBLE(var,groupname,key);\
			}\
	}\
    
 #define GET_INI_INT(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_integer (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}\   
    

 #define GET_INI_INT_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_integer_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	 
 #define GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_double_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\	
	

 #define GET_INI_DOUBLE_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(double)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = std_value;\
					}\
			} else\
			{\
				GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key);\
			}\
	}\	
	
 #define CHECK_LEN(var1,var2)\
	{\
		if (var1!=var2)\
			{\
				gchar *S;\
				S = g_strdup_printf  ("%s is not %s. Check your configuration in file: %s!",#var1,#var2,filename);\
				PRINT_ERROR(S);\
				g_free(S);\
				return -1;\
			}\
	}\	

agroforestry_read(agroforestry *self,const char* configfile)
{
expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
	
GKeyFile *keyfile;
GKeyFileFlags flags;
GError *error = NULL;
const char* filename=configfile;
keyfile = g_key_file_new ();
flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	/* Load the GKeyFile from keyfile.conf or return. */
if (!g_key_file_load_from_file (keyfile, configfile, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		};


 GET_INI_INT( self->iAverageShading,"grid_positions","AverageShading");
 GET_INI_DOUBLE( self->fViewAngle,"grid_positions","ViewAngle");  
 GET_INI_INT(self->iTreeHeightData,"agroforestry","TreeHeightData");
 GET_INI_DOUBLE( self->fTreeHeight,"agroforestry","TreeHeight");
 GET_INI_DOUBLE( self->fTreeWidth,"agroforestry","TreeStripWidth");
 GET_INI_INT( self->iCalcLightAttenuation,"agroforestry","LightAttenuationCalc");
 GET_INI_DOUBLE_OPTIONAL( self->fFixedLightAttenuationFactor,"agroforestry","LightAttenuationFactor",0.5);
 GET_INI_INT( self->iCalcExtinction,"agroforestry","ExtinctionCalculation");
 GET_INI_DOUBLE_OPTIONAL(self->fLAI,"agroforestry","LAI",3.0);
 GET_INI_DOUBLE_OPTIONAL( self->fExtinctionCoefficient,"agroforestry","ExtinctionCoefficient",1.0);
 GET_INI_INT( self->CN_calculate,"agroforestry","CN_calculate");
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadLeaf, "agroforestry","CN_fromDeadLeaf",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadBranch, "agroforestry","CN_fromDeadBranch",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadStem, "agroforestry","CN_fromDeadStem",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadFineRoot, "agroforestry","CN_fromDeadFineRoot",0.0);
 GET_INI_DOUBLE_OPTIONAL(self->CN_fromDeadGrossRoot, "agroforestry","CN_fromDeadGrossRoot",0.0);
 
 GET_INI_INT(self->iTree,"grid_positions","tree");
 GET_INI_INT(self->grid_id_tree,"grid_positions","grid_id_tree")
 GET_INI_INT(self->grid_i_tree,"grid_positions","grid_i_tree")
 GET_INI_INT(self->grid_j_tree,"grid_positions","grid_j_tree")
 GET_INI_INT(self->grid_sub_tree,"grid_positions","grid_sub_tree")
 GET_INI_INT_ARRAY(self->grid_id_beside_tree,self->grid_id_beside_tree_len,"grid_positions","grid_ids_beside_tree")
 GET_INI_INT_ARRAY(self->grid_i_beside_tree,self->grid_i_beside_tree_len,"grid_positions","grid_is_beside_tree")
 GET_INI_INT_ARRAY(self->grid_j_beside_tree,self->grid_j_beside_tree_len,"grid_positions","grid_js_beside_tree")
 GET_INI_INT_ARRAY(self->grid_sub_beside_tree,self->grid_sub_beside_tree_len,"grid_positions","grid_subs_beside_tree")
 GET_INI_DOUBLE_ARRAY_OPTIONAL(self->fDistStripAll,self->fDistStripAllLen,1,-99.9,"grid_positions","DistancesfromTreeStrip");  
 GET_INI_DOUBLE_ARRAY(self->DeadLeaf_beside_tree_array,self->DeadLeaf_beside_tree_len,"grid_positions","DeadLeaf_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadStem_beside_tree_array,self->DeadStem_beside_tree_len,"grid_positions","DeadStem_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadBranch_beside_tree_array,self->DeadBranch_beside_tree_len,"grid_positions","DeadBranch_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadFineRoot_beside_tree_array,self->DeadFineRoot_beside_tree_len,"grid_positions","DeadFineRoot_beside_tree_frac")
 GET_INI_DOUBLE_ARRAY(self->DeadGrossRoot_beside_tree_array,self->DeadGrossRoot_beside_tree_len,"grid_positions","DeadGrossRoot_beside_tree_frac")
 GET_INI_DOUBLE_OPTIONAL(self->fMaxShadeLength,"grid_positions","MaxShadeLength",10.0);  

 
 CHECK_LEN(self->grid_id_beside_tree_len,self->grid_i_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->grid_j_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->grid_sub_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->fDistStripAllLen);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadLeaf_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadStem_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadBranch_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadFineRoot_beside_tree_len);
 CHECK_LEN(self->grid_id_beside_tree_len,self->DeadGrossRoot_beside_tree_len);

g_key_file_free(keyfile);
  
return RET_SUCCESS; 
}

// Our Modul Functions:

int agroforestry_run(agroforestry* self)
{
    expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
    PCLIMATE pCl = xpn->pCl;
	PCPROFILE	pCP = xpn->pCh->pCProfile;
	PPLANT pPl = xpn->pPl;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20181126
	PSPROFILE pSo = xpn->pSo;//Added by Hong on 20181127
	PSLAYER         pSL     =pSo->pSLayer->pNext;//Added by Hong on 20181127
	PCLAYER pCL;
	PCHEMISTRY pCh = xpn->pCh;
	PLAYERROOT      pLR     =pPl->pRoot->pLayerRoot; //Added by Hong on 20190221
//	double DELT = (double)xpn->pTi->pTimeStep->fAct; //Hong on 20181127
	int i2;

    double fReductionFactor, fShadedFraction, fShadedLength, fAttenuationFactor;
    double fAverageLengthThroughTrees, fWidth0, fHeight0,fZenithTan;
	double fLAIeff;
	double fTreeDeadLeafWeight, fTreeDeadStemWeight, fTreeDeadBranchWeight, fTreeDeadRootWeight;
	double fCNTreeDeadLeaf, fCNTreeDeadStem, fCNTreeDeadBranch;
    double fTreeDeadFineRootWeight_Layer[200], fTreeDeadGrossRootWeight_Layer[200], fCNTreeDeadFineRoot_Layer[200], fCNTreeDeadGrossRoot_Layer[200]; 
	double fCumDepth;
	
	fTreeDeadLeafWeight=0.0;
    fTreeDeadStemWeight=0.0;
    fTreeDeadRootWeight=0.0;
	//memset(fTreeDeadFineRootWeight_Layer, 0.0, sizeof fTreeDeadFineRootWeight_Layer);
	memset(fTreeDeadGrossRootWeight_Layer, 0.0, sizeof fTreeDeadGrossRootWeight_Layer);
	memset(fCNTreeDeadFineRoot_Layer, 0.0, sizeof fCNTreeDeadFineRoot_Layer);
	memset(fCNTreeDeadGrossRoot_Layer, 0.0, sizeof fCNTreeDeadGrossRoot_Layer);
	
	fTreeDeadLeafWeight = max(xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadLeafWeight_dt, (double)0.0);
	
	fTreeDeadStemWeight = max(xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadStemWeight_dt, (double)0.0);
	
	fTreeDeadBranchWeight = max(xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadBranchWeight_dt, (double)0.0);
	
	if (self->CN_calculate == 1)
	    {
			fCNTreeDeadLeaf = xpn_class[self->xn_id_tree]->pCh->pCProfile->fCNLeafLitterSurf;
			fCNTreeDeadStem = xpn_class[self->xn_id_tree]->pCh->pCProfile->fCNStemLitterSurf;
			fCNTreeDeadBranch = xpn_class[self->xn_id_tree]->pCh->pCProfile->fCNBranchLitterSurf;
		}
	else
	   {
		   fCNTreeDeadLeaf =self->CN_fromDeadLeaf;
		   fCNTreeDeadStem =self->CN_fromDeadStem;
           fCNTreeDeadBranch= self->CN_fromDeadBranch;
	   }
	
	//Added by Hong
	
	pSo = xpn_class[self->xn_id_tree]->pSo;
	pLR = xpn_class[self->xn_id_tree]->pPl->pRoot->pLayerRoot;
	pCL = xpn_class[self->xn_id_tree]->pCh->pCLayer;
	for (pSL = pSo->pSLayer->pNext,
		     pLR = pLR->pNext, pCL= pCL->pNext, i2=0;
		     ((pSL->pNext != NULL)&&
		      (pLR->pNext != NULL));
		     pSL = pSL->pNext,
		     pLR = pLR->pNext,pCL= pCL->pNext,i2++)  {
		
			fTreeDeadFineRootWeight_Layer[i2] = pLR->fDeadFineRootWeight_dt;  
			fTreeDeadGrossRootWeight_Layer[i2] = pLR->fDeadGrossRootWeight_dt;
			if (self->CN_calculate == 1)
	          {
			   fCNTreeDeadFineRoot_Layer[i2] = pCL->fCNFineRootLitter;
			   fCNTreeDeadGrossRoot_Layer[i2] = pCL->fCNGrossRootLitter;
		       }
		    else
			  {
				fCNTreeDeadFineRoot_Layer[i2] = self->CN_fromDeadFineRoot;
				fCNTreeDeadGrossRoot_Layer[i2] = self->CN_fromDeadGrossRoot;
			  }
			  
			fTreeDeadRootWeight += fTreeDeadFineRootWeight_Layer[i2]+fTreeDeadGrossRootWeight_Layer[i2];
			
		}
		
	//pSo = xpn->pSo;
	//End of Hong
		
	if(self->iTree == 1) //Tree strip
	{
	fReductionFactor = 0.0;
	
/*	// Litter and Manure from trees to soil surface
	pCP->fCLitterSurf    += (fTreeDeadLeafWeight * self->C_fromDeadLeaf_frac * self->LeaftoLitter_frac * self->DeadLeaf_tree_frac
	                         +fTreeDeadStemWeight * self->C_fromDeadStem_frac * self->StemtoLitter_frac * self->DeadStem_tree_frac);
	pCP->fCManureSurf	 += (fTreeDeadLeafWeight * self->C_fromDeadLeaf_frac * self->LeaftoManure_frac * self->DeadLeaf_tree_frac
							 +fTreeDeadStemWeight * self->C_fromDeadStem_frac * self->StemtoManure_frac * self->DeadStem_tree_frac);
	pCP->fNLitterSurf    += (fTreeDeadLeafWeight * self->N_fromDeadLeaf_frac * self->LeaftoLitter_frac * self->DeadLeaf_tree_frac
	                         +fTreeDeadStemWeight * self->N_fromDeadStem_frac * self->StemtoLitter_frac * self->DeadStem_tree_frac);
	pCP->fNManureSurf	 += (fTreeDeadLeafWeight * self->N_fromDeadLeaf_frac * self->LeaftoManure_frac * self->DeadLeaf_tree_frac
							 +fTreeDeadStemWeight * self->N_fromDeadStem_frac * self->StemtoManure_frac * self->DeadStem_tree_frac);
							 
*/							 
	// Litter from trees reduced due to distribution to the field
		pCP->fCLitterSurf    -= fTreeDeadLeafWeight *  (1-self->DeadLeaf_tree_frac)
	                         +fTreeDeadStemWeight * (1-self->DeadStem_tree_frac)         +fTreeDeadBranchWeight * (1-self->DeadBranch_tree_frac);
	
	    pCP->fNLitterSurf    -= fTreeDeadLeafWeight /fCNTreeDeadLeaf * (1-self->DeadLeaf_tree_frac)
	                         + fTreeDeadStemWeight /fCNTreeDeadStem * (1-self->DeadStem_tree_frac) + fTreeDeadBranchWeight /fCNTreeDeadBranch * (1-self->DeadBranch_tree_frac);
			 
	
/*    pCB->dCInputCum -= fTreeDeadLeafWeight * (1-self->DeadLeaf_tree_frac)
	                         +fTreeDeadStemWeight * (1-self->DeadStem_tree_frac)         +fTreeDeadBranchWeight  * (1-self->DeadBranch_tree_frac); */
pCB->dCInputSurf -= fTreeDeadLeafWeight * (1-self->DeadLeaf_tree_frac)
	                         +fTreeDeadStemWeight * (1-self->DeadStem_tree_frac)         +fTreeDeadBranchWeight  * (1-self->DeadBranch_tree_frac);			 

	// Calculate "used" weights for each grid point and each plant compartment separately
	pCh->pCProfile->fUsedLeafWeight += fTreeDeadLeafWeight * (1-self->DeadLeaf_tree_frac);
	pCh->pCProfile->fUsedStemWeight += fTreeDeadStemWeight * (1-self->DeadStem_tree_frac);
	pCh->pCProfile->fUsedBranchWeight += fTreeDeadBranchWeight * (1-self->DeadBranch_tree_frac);
	
/*	gchar* S;
	S = g_strdup_printf(" pPl->pBiomass->fUsedLeafWeight %f \n", pPl->pBiomass->fUsedLeafWeight);
	PRINT_MESSAGE(xpn,3,S);
*/	
/*	if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,15) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, (1-self->DeadLeaf_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, (1-self->DeadLeaf_tree_frac));
	PRINT_MESSAGE(xpn,3,S);	
	
}
	
if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,16) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, (1-self->DeadLeaf_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, (1-self->DeadLeaf_tree_frac));

	PRINT_MESSAGE(xpn,3,S);	
	
}	
*/

	/* dead roots distributed to field*/
	pSo = xpn->pSo;
	//pLR = xpn->pPl->pRoot->pLayerRoot;
	fCumDepth= 0.0;
		for (pSL = pSo->pSLayer->pNext,
		     pCL = pCh->pCLayer->pNext,i2=0;
		     ((pSL->pNext != NULL)&&
		      (pCL->pNext != NULL));
		     pSL = pSL->pNext,
		     pCL = pCL->pNext,i2++) {
			//Check the whether there are roots in this layer:
			//if (pLR->fLengthDens==(double)0.0)           break;
			pCL->fCLitter -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                 fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
			  
			  pCL->fNLitter -= fTreeDeadFineRootWeight_Layer[i2] / fCNTreeDeadFineRoot_Layer[i2] * (1-self->DeadFineRoot_tree_frac) +              fTreeDeadGrossRootWeight_Layer[i2] / fCNTreeDeadGrossRoot_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
			  
			/*  pCB->dCInputCum -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                 fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac); */
							
			 pCB->dCInputProfile -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			                fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
			//Hong 20190507: balance for 0-30 cm profile:
			fCumDepth +=(double)0.1*pSL->fThickness; //cm
			if (fCumDepth <=30.0)
					{
						pCB->dCInputProfile_30 -= fTreeDeadFineRootWeight_Layer[i2] * (1-self->DeadFineRoot_tree_frac)+
			              fTreeDeadGrossRootWeight_Layer[i2] * (1-self->DeadGrossRoot_tree_frac);
					 }
			  
		}
	
	
	}
	else //Strip next to trees
	{

    fReductionFactor = 0.0;
    fShadedFraction = 0.0;
    fShadedLength = 0.0;
    fAttenuationFactor = 1.0;

    // Trees are always in the right of the viewer, the view angle fViewAngle is defined as the angle of the tree strip
    // turning around clockwise with 0°
    // in the North. Thus, when the tree row is from North to South and the viewer is in the West, the view angle is 0°
    // while it is
    // 180° when the viewer is in the East.
    // The azimuth angle fAzimuthAngle depends on the time of the day and is also counted clockwise with 0° in the North
    // fDiffAngle is the measure if the tree strip is completely sunlit or not.

    sunpos(self);

    //printf("fTimeDay %f SolRad at start %f \n", xpn->pTi->pSimTime->fTimeDay, pCl->pWeather->fSolRad);

    self->fDiffAngle = 360 + self->fAzimuthAngle - self->fViewAngle;

    while(self->fDiffAngle >= 360) {
        self->fDiffAngle -= 360;
    }
    
    if(self->fZenithAngle < 0) {
        self->fZenithAngle = -self->fZenithAngle;
        }

    fZenithTan = tan(self->fZenithAngle*PI/180.) ;

	if(self->iTreeHeightData == 1) //Tree height from other XN grid point is used, else the input value from project__agroforestry.ini
	{
		self->fTreeHeight = xpn_class[self->xn_id_tree]->pPl->pCanopy->fPlantHeight;
	}

    if((self->fDiffAngle >= 180) && (self->fDiffAngle <= 360)) {
        // pCl->pWeather->fSolRad *= 0.5;
        if(self->fZenithAngle < 90) {
            fShadedLength = fZenithTan * self->fTreeHeight;
			if (self->iAverageShading == 1){
				if(fShadedLength < self->fMaxShadeLength) {
					fShadedFraction = fShadedLength / self->fMaxShadeLength;
				} else {
					fShadedFraction = 1.0;
				}
			} else if (self->iAverageShading == 0) {
				if(fShadedLength < self->fDistStrip) {
					fShadedFraction = 0.0;
				} else {
					fShadedFraction = 1.0;
				}			
			}
        } else {
            fShadedFraction = 1.0;
        }
        //printf("fTimeDay %f Azimuth %f %f ShadedFrac %f Shaded Length %f \n", xpn->pTi->pSimTime->fTimeDay, self->fAzimuthAngle, self->fDiffAngle, fShadedFraction, fShadedLength);
        //printf("fTimeDay %f SolRad %f \n", xpn->pTi->pSimTime->fTimeDay, pCl->pWeather->fSolRad);
        if (self->iCalcLightAttenuation == 1)
        {
			fAverageLengthThroughTrees = 0.0;
			if (self->iAverageShading == 1){
				fWidth0 = fZenithTan * self->fTreeHeight;
				if ((fWidth0 <= self->fTreeWidth)&&(fWidth0 > 0.0))
					{
					fAverageLengthThroughTrees = sqrt(1 + fZenithTan * fZenithTan) * 0.5 * self->fTreeHeight;
					}
				else if (fWidth0 > self->fTreeWidth)
					{
					fHeight0 = self->fTreeWidth / fZenithTan;
					fAverageLengthThroughTrees = 
					(self->fTreeHeight - fHeight0) * sqrt(self->fTreeWidth * self->fTreeWidth + fHeight0 * fHeight0)/self->fTreeHeight 
					+ 0.5 * sqrt(1 + fZenithTan * fZenithTan) * fHeight0 * fHeight0 / self->fTreeHeight;
					}
				else
					{
					fAverageLengthThroughTrees = 0.0;
					}
			} else  {
				fHeight0 = (self->fTreeWidth + self->fDistStrip)/ fZenithTan;
				if ((fHeight0 <= self->fTreeHeight)&&(fHeight0 > 0.0))
					{
					fAverageLengthThroughTrees = self->fTreeWidth / sin(self->fZenithAngle*PI/180.);
					}
				else if (fHeight0 > self->fTreeHeight)
					{
					fAverageLengthThroughTrees = (self->fTreeHeight - self->fDistStrip/fZenithTan) / cos(self->fZenithAngle*PI/180.);
					}
				else
					{
					fAverageLengthThroughTrees = 0.0;
					}
				//if (fShadedFraction == 1)
				//	printf("%f \n", fAverageLengthThroughTrees);
			}
        
		if(self->iCalcExtinction == 1) //LAI from other XN grid point is used to calculate the extinction coefficient, else the input value from project__agroforestry.ini
		{
			self->fLAI = xpn_class[self->xn_id_tree]->pPl->pCanopy->fLAI;
		}		
		
		if(self->fTreeHeight <= 0.0)
		{
		fLAIeff = 0.0;
		}
		else
		{
		fLAIeff = self->fLAI * fAverageLengthThroughTrees / self->fTreeHeight;
		}
        
		fAttenuationFactor = exp(- self->fExtinctionCoefficient * fLAIeff );
		//printf("LAI %f Height %f Ext %f LAIeff %f \n",self->fLAI, self->fTreeHeight, self->fExtinctionCoefficient, fLAIeff);		
        } else { //end self->iCalcLightAttenuation == 1
        fAttenuationFactor = self->fFixedLightAttenuationFactor;
        }    
    
    
    
    } else {
        fShadedFraction  = 0.0;
    }

        
        fReductionFactor = fAttenuationFactor * fShadedFraction + 1.0 * (1 - fShadedFraction);
        //printf("Red %f ShadeF %f ShadeL %f \n", fReductionFactor, fShadedFraction, fShadedLength);
        pCl->pWeather->fSolRad *= fReductionFactor;
        pCl->pWeather->fPAR *= fReductionFactor;

	//Added by Hong on 20181127
	/* actualize the Soil Organic Matter Pools over soil depth
		 */

	// Litter from trees to soil surface
	pCP->fCLitterSurf    += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac
	                         +fTreeDeadStemWeight * self->DeadStem_beside_tree_frac 
							 + fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;
	
	pCP->fNLitterSurf    += fTreeDeadLeafWeight /fCNTreeDeadLeaf * self->DeadLeaf_beside_tree_frac
	                         +fTreeDeadStemWeight /fCNTreeDeadStem * self->DeadStem_beside_tree_frac
							 + fTreeDeadBranchWeight /fCNTreeDeadBranch * self->DeadBranch_beside_tree_frac;
							 
							 
	//Added by Hong on 20181126
	pCB->dCInputSurf += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac
						+fTreeDeadStemWeight * self->DeadStem_beside_tree_frac  
                         + fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;
	
/*	pCB->dCInputCum += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac
						+fTreeDeadStemWeight * self->DeadStem_beside_tree_frac  
                         + fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;*/
    //End of Hong						 
  
	// Calculate "used" weights for each grid point and each plant compartment separately
	pCh->pCProfile->fUsedLeafWeight += fTreeDeadLeafWeight * self->DeadLeaf_beside_tree_frac;
	pCh->pCProfile->fUsedStemWeight += fTreeDeadStemWeight * self->DeadStem_beside_tree_frac;
    pCh->pCProfile->fUsedBranchWeight += fTreeDeadBranchWeight * self->DeadBranch_beside_tree_frac;
	
/*	gchar* S;
	S = g_strdup_printf(" pPl->pBiomass->fUsedLeafWeight %f self->DeadLeaf_beside_tree_frac %f \n", pPl->pBiomass->fUsedLeafWeight, self->DeadLeaf_beside_tree_frac);
	PRINT_MESSAGE(xpn,3,S);
 */   
/*if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,15) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, beside_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, self->DeadLeaf_beside_tree_frac);
	PRINT_MESSAGE(xpn,3,S);	
	
}
	
if ((xpn_time_compare_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->mday,
		    2019,8,16) ==0 ))
		//if (xpn->pTi->pSimTime->fTimeDay>0.5&&xpn->pTi->pSimTime->fTimeDay<0.6)
 {		
    gchar* S;
	S = g_strdup_printf(" pCh->pCProfile->fUsedLeafWeight: %f, fTreeDeadLeafWeight:  %f, beside_tree_frac: %f \n", pCh->pCProfile->fUsedLeafWeight, fTreeDeadLeafWeight, self->DeadLeaf_beside_tree_frac);

	PRINT_MESSAGE(xpn,3,S);	
	
}	
*/

		/* root litter */
		fCumDepth =0.0;	
		for (pSL = pSo->pSLayer->pNext,
		     pCL = pCh->pCLayer->pNext,i2=0;
		     ((pSL->pNext != NULL)&&
		      (pCL->pNext != NULL));
		     pSL = pSL->pNext,
		     pCL = pCL->pNext,i2++) {
				 
			 pCL->fCLitter += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
			  
			  
			  pCL->fNLitter += fTreeDeadFineRootWeight_Layer[i2] /fCNTreeDeadFineRoot_Layer[i2]  * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] /fCNTreeDeadGrossRoot_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
					
			/*  pCB->dCInputCum += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;*/
			  
			  pCB->dCInputProfile += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
			  //Hong 20190507: balance for 0-30 cm profile:	
	  fCumDepth +=(double)0.1*pSL->fThickness; //cm
	  if (fCumDepth <=30.0)
	    {   
			pCB->dCInputProfile_30 += fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac +fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;
		}
              pCh->pCProfile->fUsedRootWeight +=	fTreeDeadFineRootWeight_Layer[i2] * self->DeadFineRoot_beside_tree_frac + fTreeDeadGrossRootWeight_Layer[i2] * self->DeadGrossRoot_beside_tree_frac;			

		}

	         
			
	} //end self->iTree (Strip next to trees)

//out-noted by Hong on 20190221, not necessary
/*
	self->UsedLeafWeight_Sum = 0.0;
	self->UsedStemWeight_Sum = 0.0;
	self->UsedRootWeight_Sum = 0.0;
	
  for(i = 0; i < xpn->pXSys->id_N; i++)
		{	
		self->UsedLeafWeight_Sum += xpn_class[i]->pPl->pBiomass->fUsedLeafWeight;
		self->UsedStemWeight_Sum += xpn_class[i]->pPl->pBiomass->fUsedStemWeight;
		self->UsedRootWeight_Sum += xpn_class[i]->pPl->pBiomass->fUsedRootWeight;
		}

	if(self->UsedLeafWeight_Sum > fTreeDeadLeafWeight)
		{
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadLeafWeight = 0.0; /
		}

   else 
		{
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadLeafWeight -= self->UsedLeafWeight_Sum;
		} 

	if(self->UsedStemWeight_Sum > fTreeDeadStemWeight)
		{			
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadStemWeight = 0.0;
		}
	if(self->UsedRootWeight_Sum > fTreeDeadRootWeight)
		{		
		xpn_class[self->xn_id_tree]->pPl->pBiomass->fDeadRootWeight = 0.0;
		}
*/
//End of Hong
     //g_free(fTreeDeadFineRootWeight_Layer);

    return RET_SUCCESS;
}

void sunpos(agroforestry* self)
{
    expertn_modul_base* xpn = EXPERTN_MODUL_BASE(self);
    PSIMTIME pST = xpn->pTi->pSimTime;
    PLOCATION pLo = xpn->pLo;

    // Time variables
    int year, month, day, hour, minutes, seconds;

    year = pST->year;
    month = pST->mon;
    day = pST->mday;
    hour = pST->hour;
    minutes = pST->min;
    seconds = pST->sec;

    // Main variables
    double dElapsedJulianDays;
    double dDecimalHours;
    double dEclipticLongitude;
    double dEclipticObliquity;
    double dRightAscension;
    double dDeclination;

    // Auxiliary variables
    double dY;
    double dX;

    // Declaration of some constants
    double pi = 3.14159265358979323846;
    double twopi = (2 * pi);
    double rad = (pi / 180);
    double dEarthMeanRadius = 6371.01;    // In km
    double dAstronomicalUnit = 149597890; // In km

    // Calculate difference in days between the current Julian Day
    // and JD 2451545.0, which is noon 1 January 2000 Universal Time
    {
        double dJulianDate;
        long int liAux1;
        long int liAux2;
        // Calculate time of the day in UT decimal hours
        dDecimalHours = hour + (minutes + seconds / 60.0) / 60.0;
        // Calculate current Julian Day
        liAux1 = (month - 14) / 12;
        liAux2 = (1461 * (year + 4800 + liAux1)) / 4 + (367 * (month - 2 - 12 * liAux1)) / 12 -
                 (3 * ((year + 4900 + liAux1) / 100)) / 4 + day - 32075;
        dJulianDate = (double)(liAux2) - 0.5 + dDecimalHours / 24.0;
        // Calculate difference between current Julian Day and JD 2451545.0
        dElapsedJulianDays = dJulianDate - 2451545.0;
    }

    // Calculate ecliptic coordinates (ecliptic longitude and obliquity of the
    // ecliptic in radians but without limiting the angle to be less than 2*Pi
    // (i.e., the result may be greater than 2*Pi)
    {
        double dMeanLongitude;
        double dMeanAnomaly;
        double dOmega;
        dOmega = 2.1429 - 0.0010394594 * dElapsedJulianDays;
        dMeanLongitude = 4.8950630 + 0.017202791698 * dElapsedJulianDays; // Radians
        dMeanAnomaly = 6.2400600 + 0.0172019699 * dElapsedJulianDays;
        dEclipticLongitude = dMeanLongitude + 0.03341607 * sin(dMeanAnomaly) + 0.00034894 * sin(2 * dMeanAnomaly) -
                             0.0001134 - 0.0000203 * sin(dOmega);
        dEclipticObliquity = 0.4090928 - 6.2140e-9 * dElapsedJulianDays + 0.0000396 * cos(dOmega);
    }

    // Calculate celestial coordinates ( right ascension and declination ) in radians
    // but without limiting the angle to be less than 2*Pi (i.e., the result may be
    // greater than 2*Pi)
    {
        double dSin_EclipticLongitude;
        dSin_EclipticLongitude = sin(dEclipticLongitude);
        dY = cos(dEclipticObliquity) * dSin_EclipticLongitude;
        dX = cos(dEclipticLongitude);
        dRightAscension = atan2(dY, dX);
        if(dRightAscension < 0.0) {
            dRightAscension = dRightAscension + twopi;
        }
        dDeclination = asin(sin(dEclipticObliquity) * dSin_EclipticLongitude);
    }

    // Calculate local coordinates ( azimuth and zenith angle ) in degrees
    {
        double dGreenwichMeanSiderealTime;
        double dLocalMeanSiderealTime;
        double dLatitudeInRadians;
        double dHourAngle;
        double dCos_Latitude;
        double dSin_Latitude;
        double dCos_HourAngle;
        double dParallax;
        dGreenwichMeanSiderealTime = 6.6974243242 + 0.0657098283 * dElapsedJulianDays + dDecimalHours;
        dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15 + pLo->pFarm->fLongitude) * rad;
        dHourAngle = dLocalMeanSiderealTime - dRightAscension;
        dLatitudeInRadians = pLo->pFarm->fLatitude * rad;
        dCos_Latitude = cos(dLatitudeInRadians);
        dSin_Latitude = sin(dLatitudeInRadians);
        dCos_HourAngle = cos(dHourAngle);
        self->fZenithAngle =
            (acos(dCos_Latitude * dCos_HourAngle * cos(dDeclination) + sin(dDeclination) * dSin_Latitude));
        dY = -sin(dHourAngle);
        dX = tan(dDeclination) * dCos_Latitude - dSin_Latitude * dCos_HourAngle;
        self->fAzimuthAngle = atan2(dY, dX);
        if(self->fAzimuthAngle < 0.0) {
            self->fAzimuthAngle = self->fAzimuthAngle + twopi;
        }
        self->fAzimuthAngle = self->fAzimuthAngle / rad;
        // Parallax Correction
        dParallax = (dEarthMeanRadius / dAstronomicalUnit) * sin(self->fZenithAngle);
        self->fZenithAngle = (self->fZenithAngle + dParallax) / rad;
    }
}