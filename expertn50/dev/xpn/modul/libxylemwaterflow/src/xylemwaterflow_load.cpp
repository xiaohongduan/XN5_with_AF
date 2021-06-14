#include "xylemwaterflow.h"

//#include <expertn_modul_base.h>

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
	 

#define GET_INI_STRING(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_string (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}\

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

#define GET_INI_INT_OPTIONAL(var,groupname,key,std_value)\
{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (key_exists==FALSE) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (in file %s) is missing. Standard Value (%d) taken instead!\n",groupname,key,filename,std_value);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{\
				var = std_value;\
			} else\
			{\
				GET_INI_INT(var,groupname,key);\
			}\
	}\

/*RootGenerator RG(5,3,2,3,3,2,2,				//number
				 8.0,0.5,2.0,1.5,1.0,0.75,	//length  
				 0.93);						//gale and grigal beta*/

class readTLP;
int XWF_CLASS::loadconfig()//load values from config-file in input folder
{		
	//SB May 2011: some Hydrus solver specific values could be set in the 
	//config file before in the version of MJ, now it is hard-coded
	//As soon the Hydrus implementation is re-written, we can read it from file again
	//FH 2016/01/27: All variables in loadconfig() and in loadPhysiology() can now be loaded from one ini-file
	
	GKeyFile *		keyfile;
	GKeyFileFlags 	flags;
	GError *error = NULL;
	
	const char* 	filename=this->cfg_filename;
	
	char 			*species;
	
	double			dmax;
	
	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = (GKeyFileFlags) (G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS);

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, this->cfg_filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}
	
	GET_INI_DOUBLE(this->conf.delZ,"config","DeltaZ");
	GET_INI_DOUBLE(this->conf.dev,"config","Dev");
	GET_INI_DOUBLE(this->conf.internal_timestep_ini,"config","InternalTimeStep");
	GET_INI_INT(this->conf.iterbreak,"config","Iterbreak");
	//GET_INI_INT_OPTIONAL(this->conf.beep,"config","Beep",0);
	GET_INI_INT(this->conf.HydraulicLift,"config","HydraulicLift");
	GET_INI_INT(this->conf.pot_tra,"config","PotTra");
	GET_INI_INT(this->conf.maxStrahler,"config","MaxStrahler")
	GET_INI_STRING(this->conf.solver,"config","Solver");
	GET_INI_STRING(this->conf.xylemfractionCalculation,"config","SapwoodAreaFraction");
	GET_INI_DOUBLE(this->conf.CtoK,"config","CtoK");
	GET_INI_DOUBLE(this->conf.GtoPAR,"config","GtoPAR");
    GET_INI_DOUBLE(this->conf.WtoUMOL,"config","WtoUMOL");
    GET_INI_DOUBLE(this->conf.cCO2,"config","cCO2");
    GET_INI_DOUBLE(this->conf.R,"config","R");
    GET_INI_DOUBLE(this->conf.ci,"config","ci");
    GET_INI_DOUBLE(this->conf.Tref,"config","Tref");
    GET_INI_DOUBLE(this->conf.J_ref_max,"config","J_ref_max");
    GET_INI_DOUBLE(this->conf.V_ref_lmax,"config","V_ref_lmax");
    GET_INI_DOUBLE(this->conf.oi,"config","oi");
    GET_INI_DOUBLE(this->conf.Ko_ref,"config","Ko_ref");
    GET_INI_DOUBLE(this->conf.Kc_ref,"config","Kc_ref");
    GET_INI_DOUBLE(this->conf.Eo,"config","Eo");
    GET_INI_DOUBLE(this->conf.Ec,"config","Ec");
    GET_INI_DOUBLE(this->conf.EaJ,"config","EaJ");
    GET_INI_DOUBLE(this->conf.EaV,"config","EaV");
    GET_INI_DOUBLE(this->conf.EdJ,"config","EdJ");
    GET_INI_DOUBLE(this->conf.EdV,"config","EdV");
    GET_INI_DOUBLE(this->conf.S,"config","S");
    GET_INI_DOUBLE(this->conf.alpha,"config","alpha");
    GET_INI_DOUBLE(this->conf.Theta,"config","Theta");
    GET_INI_DOUBLE(this->conf.gamma0,"config","gamma0");
    GET_INI_DOUBLE(this->conf.gamma1,"config","gamma1");
    GET_INI_DOUBLE(this->conf.gamma2,"config","gamma2");
    GET_INI_DOUBLE(this->conf.Rd_f,"config","Rd_f");
    GET_INI_DOUBLE(this->conf.Ea_Rd,"config","Ea_Rd");
    GET_INI_DOUBLE(this->conf.Rd_T20,"config","Rd_T20");
	
	this->conf.contemp = 1.0;
	this->conf.delZ*=1000;//m -> mm

	this->T_m->stom_delay = 0;

	GET_INI_STRING(species,"plant","Type");
    xpn->pPl->pGenotype->acCropName = g_strdup_printf("%s",species);
    GET_INI_STRING(xpn->pPl->pGenotype->acCropCode,"plant","CropCode");
	// File specification now via configuration
	//GET_INI_STRING(this->T_m->canopy,"plant","CanopyFile");
	//GET_INI_STRING(this->T_m->root,"plant","RootFile");
	GET_INI_DOUBLE(this->T_m->HydraulicArea,"plant","HydraulicArea");
	GET_INI_DOUBLE(this->T_m->CrownProjArea,"plant","CrownProjArea");
	GET_INI_DOUBLE(dmax, "plant", "MaxStemDiameter");
	GET_INI_INT(this->T_m->LAIDistribution,"plant", "LAIdist");
	if(T_m->LAIDistribution == 1)
		GET_INI_STRING(this->T_m->LAIHisto,"plant","TFiles");
	if(T_m->LAIDistribution == 2)
		GET_INI_STRING(this->T_m->in_transpiration,"plant","TFiles");

        GET_INI_INT(this->T_m->RootSwitch, "plant", "RootSwitch");
        // Read variables for root generation
        if(this->T_m->RootSwitch == 2) {
            GET_INI_DOUBLE_OPTIONAL(this->T_m->RL1, "plant", "RL1", 0.0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->RL2, "plant", "RL2", 0.0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->RL3, "plant", "RL3", 0.0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->RL4, "plant", "RL4", 0.0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->RL5, "plant", "RL5", 0.0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->RL6, "plant", "RL6", 0.0);
            GET_INI_INT_OPTIONAL(this->T_m->RN1, "plant", "RN1", 0);
            GET_INI_INT_OPTIONAL(this->T_m->RN2, "plant", "RN2", 0);
            GET_INI_INT_OPTIONAL(this->T_m->RN3, "plant", "RN4", 0);
            GET_INI_INT_OPTIONAL(this->T_m->RN4, "plant", "RN5", 0);
            GET_INI_INT_OPTIONAL(this->T_m->RN5, "plant", "RN5", 0);
            GET_INI_INT_OPTIONAL(this->T_m->RN6, "plant", "RN6", 0);
            GET_INI_INT_OPTIONAL(this->T_m->RN7, "plant", "RN7", 0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->GG, "plant", "GG", 0.0);
            GET_INI_DOUBLE_OPTIONAL(this->T_m->dp1, "plant", "dp1", 0.0);
        }

        GET_INI_DOUBLE(this->T_m->stom_delay,species,"StomDelay");
	GET_INI_DOUBLE(this->T_m->stom_a,species,"StomCondA");
	GET_INI_DOUBLE(this->T_m->stom_b,species,"StomCondB");
	GET_INI_DOUBLE(this->T_m->elasticity,species,"ElastMod");
	GET_INI_DOUBLE(this->T_m->BC_a,species,"BrooksCoreyA");
	GET_INI_DOUBLE(this->T_m->BC_Lambda,species,"BrooksCoreyL");
	GET_INI_DOUBLE(this->T_m->kmax_branch,species,"KmaxBranch");
	GET_INI_DOUBLE(this->T_m->kmax_root,species,"KmaxRoot");
	GET_INI_DOUBLE(this->T_m->RadialRootKmax,species,"RadCond");
	GET_INI_DOUBLE(this->T_m->xylemfraction_stem,species,"XylemStem");
	GET_INI_DOUBLE(this->T_m->xylemfraction_root,species,"XylemRoot");
	GET_INI_DOUBLE(this->T_m->porosity,species,"XylemPor");

	double f = -this->T_m->BC_a/this->T_m->elasticity; // Water release from cell expansion with matric head
	this->T_m->Theta_aev = this->T_m->porosity - f; // Theta max is maximal Theta including cell volume extension, Theta_aev is Theta at air entry value (variables are set in ini-file).

	//compute xylem areas, total diameters and porousities 
	if(strcmp(this->conf.xylemfractionCalculation,"a")==0)
	{
		this->T_m->max_XylemArea_root = 100.0* 0.49 * pow((double)0.1*dmax,(double)2.13); //SB mm^2 //Aranda
		this->T_m->max_XylemArea_stem = 100.0* 0.49 * pow((double)0.1*dmax,(double)2.13); //SB mm^2 //Aranda
	}
	else if(strcmp(this->conf.xylemfractionCalculation,"c")==0)
	{
		this->T_m->max_XylemArea_root = pow(dmax*(double)0.5,(double)2.0)*PI *this->T_m->xylemfraction_root;//mm^2
		this->T_m->max_XylemArea_stem = pow(dmax*(double)0.5,(double)2.0)*PI *this->T_m->xylemfraction_stem;//mm^2
	}	
	else if(strcmp(this->conf.xylemfractionCalculation,"g")==0)
	{
		this->T_m->max_XylemArea_root   = 0.6546 * pow(dmax*(double)0.5,(double)2.0) + 0.5736*dmax - 40.069;//mm^2
		this->T_m->max_XylemArea_stem  = 0.6546 * pow(dmax*(double)0.5,(double)2.0) + 0.5736*dmax - 40.069;//mm^2
	}	
	//g_key_file_free(keyfile);
	
	return RET_SUCCESS;
};


int XWF_CLASS::fill_soil_properties()
{
	PSWATER	pSW = xpn->pSo->pSWater;

	int i;

	while(pSW->pBack != NULL)
		pSW->pBack;
	
	this->boden = (struct soils*) g_malloc0_n((xpn->pSo->iLayers-2),sizeof(struct soils));
	
	for(i=0,pSW=pSW->pNext; i < xpn->pSo->iLayers-2; i++, pSW = pSW->pNext)
	{
		this->boden[i].alpha		= pSW->fVanGenA; // 1/mm
		this->boden[i].Ks			= pSW->fCondSat / (double)86400; // conductivity [mm/s]
		this->boden[i].n			= pSW->fVanGenN;
		this->boden[i].Theta_r		= pSW->fContRes;
		this->boden[i].Theta_max	= pSW->fContSat;
		this->boden[i].dz			= xpn->pSo->fDeltaZ;
	}
	
	return RET_SUCCESS;
};

int XWF_CLASS::load_architecture()
{
    PSLAYER     pSL;
    
    double fSoilLayerThickness;
    
	//char *cfile;
	//char *rfile;
	//char *ftype;
	char *ftype_c;                                                                                                 
	char *ftype_r;
	
	char *tlpoutputfile;
	char *S;
	
    int columns;
    
	this->canopy_filename  = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,(char*)"Config.xylemwaterflow.canopy_filename");
	this->root_filename  = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,(char*)"Config.xylemwaterflow.root_filename");

    

    ftype_c = (char*)g_malloc0(strlen(this->canopy_filename)+1);
    ftype_r = (char*)g_malloc0(strlen(this->root_filename)+1);

	this->tlp_output = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,(char*)"Config.xylemwaterflow.tlp_output",-1);

	if (this->canopy_filename==NULL) {
		PRINT_ERROR("Missing entry 'xylemwaterflow.canopy_filename' in your configuration!");
	}
	else{

			char *S2;
			S2 = expertn_modul_base_replace_std_templates(xpn,this->canopy_filename);
			if (S2!=NULL) {
				this->T_m->canopy=g_strdup_printf("%s",S2);
				//free(ini_filename);
				free(S2);
			}
	}

	if (this->root_filename==NULL) {
		PRINT_MESSAGE(xpn,3,"Missing entry 'xylemwaterflow.root_filename' in your configuration! Roots will be generated.");
		this->T_m->root=g_strdup_printf("none");
	}
	else{
			char *S2;
			S2 = expertn_modul_base_replace_std_templates(xpn,this->root_filename);
			if (S2!=NULL) {
				this->T_m->root=g_strdup_printf("%s",S2);
				//free(ini_filename);
				free(S2);
			}
	}


    columns = getStringColumn_get_Size(this->T_m->canopy,(char *) ".");
    if (columns != 0)
    {
        getStringColumn(ftype_c,this->T_m->canopy,(char *) ".",columns-1);
    }
   

    columns = getStringColumn_get_Size(this->T_m->root,(char *) ".");
    if (columns != 0)
    {
        getStringColumn(ftype_r,this->T_m->root,(char *) ".",columns-1);
    } 
    
    
/*		if(getStringColumn(ftype_c,this->T_m->canopy,(char*)".",i))
        {
			getStringColumn(ftype_c,this->T_m->canopy,(char*)".",i-1);
        }*/

	
    
/*	for(int i = 0; i < 1000; i++)
	{
		if(getStringColumn(ftype_r,this->T_m->root,(char*)".",i))
			getStringColumn(ftype_r,this->T_m->root,(char*)".",i-1);
	}	
	*/
	//count nodes and allocate memory for T
	this->countAndAllocate(this->T_m);
	
   
    
    //for(int i; i < this->T_m->iCnodes + this->T_m->iRnodes; i++)
    //    this->T_m->node[i].parent = -99;
	//printf("%d %d \n", this->T_m->iCnodes, T_m->iRnodes);
    pSL = xpn->pSo->pSLayer->pNext;
    fSoilLayerThickness = pSL->fThickness;
	 	//-------------------------read-------------------------------------
	if(!strcmp(ftype_c,"csv"))
		readCSV(this->T_m,1);    
	else if(!strcmp(ftype_c,"tlp"))
		readTLP(this->T_m,1,fSoilLayerThickness);   
    
    if (this->T_m->RootSwitch == 0)
        {
        if(!strcmp(ftype_r,"csv"))
            readCSV(this->T_m,0);
        else if(!strcmp(ftype_r,"tlp"))
            readTLP(this->T_m,0,fSoilLayerThickness);
        }
    else if ((this->T_m->RootSwitch == 1) ||(this->T_m->RootSwitch == 2))
         GenerateRoots(this->T_m);

	this->OutputPath=g_strdup_printf("%s/output", xpn->pXSys->project_path);
	
	// Output Path erzeugen falls nicht vorhanden
	S=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,this->OutputPath);
	g_mkdir_with_parents(S,1);
	free(S);
	
	if (this->tlp_output == 0)
		this->OutputCompartment = g_strdup_printf("root");
	else if (this->tlp_output == 1)
		this->OutputCompartment = g_strdup_printf("canopy");
	else if (this->tlp_output == 2)
		this->OutputCompartment = g_strdup_printf("root_and_canopy");
	else
		this->OutputCompartment = g_strdup_printf("no_tlp");
	tlpoutputfile = g_strdup_printf("%s/%s_%s_%s.tlp",this->OutputPath,xpn->pXSys->project_name,xpn->pXSys->reg_str,this->OutputCompartment);
    
	if((this->tlp_output == 0) || (this->tlp_output == 1) || (this->tlp_output == 2))
		writeTLP(*this->T_m, tlpoutputfile, this->tlp_output);
	else
		PRINT_MESSAGE(xpn,3,"No tlp output");
		
	g_free(tlpoutputfile);
    
	//calculate l, cosine, vz
	for(int i=0; i<this->T_m->iCnodes+this->T_m->iRnodes; i++)                                                                             
	{                                                                                                                  
		int parentID = this->T_m->node[i].parent;      
		if(i==0) 
        {
            parentID=this->T_m->iCnodes;
            //this->T_m->node[i].parent = parentID;                                                              
		}
        if(i==this->T_m->iCnodes)
        {
            parentID=0;
             //this->T_m->node[i].parent = parentID; 
        }
       // printf("%d %d \n",i, parentID);

		//distance from parent                                                                                            
		this->T_m->node[i].l =(double)sqrt( (double)pow(this->T_m->node[i].X - this->T_m->node[parentID].X,2)                                    
									+(double)pow(this->T_m->node[i].Y - this->T_m->node[parentID].Y,2)                                                          
									+(double)pow(this->T_m->node[i].Z - this->T_m->node[parentID].Z,2));                                                        
		this->T_m->node[i].l = max(this->T_m->node[i].l,(double)0.0001);                                                              
		//cosine to parent                                                                                                
		this->T_m->node[i].cosine =(this->T_m->node[i].Z - this->T_m->node[parentID].Z) / this->T_m->node[i].l;                                        
		if(this->T_m->node[i].X - this->T_m->node[parentID].X < 0)                                                                        
			this->T_m->node[i].vz_delZ=-1;                                                                                           
		else                                                                                                              
			this->T_m->node[i].vz_delZ=1;                                                                                            
	}	                                                                                                                 
	this->T_m->node[0].cosine=this->T_m->node[1].cosine;                                                                               
                                                                                                                                                                                                                                       
    
    // 3)                                                                                                              
	//computes childs                                                                                                  
	this->T_m->maxchilds = 0;    
    for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)                                                                              
	{              
		//printf("%d %d %d \n", i, this->T_m->node[i].parent, this->T_m->node[i].child.size());																									
		//T_m->node[i].child = T_m->node[i].child2 = T_m->node[i].child3 = -1; //init                                           
		//this->T_m->node[i].child = new std::vector<int>;
		for(int j=0;j<this->T_m->iCnodes+this->T_m->iRnodes;j++)                                                                          
		{                           
            //if(i == 1)
             //  printf("%d parent %d \n",j, this->T_m->node[j].parent);
			if(this->T_m->node[j].parent==i)                                                                                         
			{			                                                                                                             
				if(!((i==0&&j==this->T_m->iCnodes) || (j==0&&i==this->T_m->iCnodes))) //exclude special case: first branch and root
				{
				this->T_m->node[i].child.push_back(j);
                  //for(int k = 0; k < this->T_m->node[i].child.size(); k++)
                    // printf("%d %d %d %d %d \n",i, j,this->T_m->node[i].child.size(), this->T_m->node[i].parent, this->T_m->node[i].child.at(k));     
				}   
			}                                                           
		}                                                                                                                 
		this->T_m->maxchilds = max(this->T_m->maxchilds, (int)this->T_m->node[i].child.size());                                                       

		//printf("childsize %d %d \n",this->T_m->node[i].child.size(), this->T_m->node[i].child.at(i));
	}                                                                                                                
             
    //for(int i=0; i<this->T_m->iCnodes; i++)
       //printf("child size %d \n", T->node[i].child.size());
       //for(int j =0; j < (int)this->T_m->node[i].child.size() ;j++)
         //  printf("%d  %d \t%f %f %f \t %d %d \n",i,this->T_m->node[i].parent ,this->T_m->node[i].X,this->T_m->node[i].Y,this->T_m->node[i].Z,this->T_m->node[i].child.size(),this->T_m->node[i].child.at(j));                                                                                                       
                                                                                                                    
	//connect root to stem                                                                                             
	if(this->T_m->node[this->T_m->iCnodes].Z == 0.0)                                                                                   
		this->T_m->node[this->T_m->iCnodes].Z-=(double)0.001;                                                                              
	this->T_m->node[this->T_m->iCnodes].parent= 0;                                                                                     
	this->T_m->node[0].parent = this->T_m->iCnodes;                                                                                    
    
   // for(int i=0; i<this->T_m->iCnodes+this->T_m->iRnodes; i++) 
     //   printf("%d %d \n",i, this->T_m->node[i].parent);
                                                                                                                    
	for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)                                                                           
	{   
        double distance = abstand(this->T_m->node[i],this->T_m->node[this->T_m->node[i].parent]);
        if (distance == 0)
            {
            // FH 02/17: when parent and child have the same height Z, g equals 0, we need this also to account for node duplicates
            this->T_m->node[i].g = 0.0; 
            }
        else
            {
            this->T_m->node[i].g = ( this->T_m->node[this->T_m->node[i].parent].Z - this->T_m->node[i].Z ) / distance;
            }
        //vorzeichen: Direction from parent to i                                                                          

	}                                                                                                                  
       
	//calculate Strahler index
	this->T_m->getStrahler();
    
  g_free(ftype_c);
  g_free(ftype_r);
    
	return RET_SUCCESS;
};

void  XWF_CLASS::countAndAllocate(tree *T)
{
    FILE *infile;
    
	char *line;                                                                                                    
	char *entry;                                                                                                   
	char *ftype_c;                                                                                                 
	char *ftype_r;
    
    int columns;

	//char *read_filename;
	//char *canopy_filename;
	//char *root_filename;
	
	//char *S, *S2;
						
    ftype_c  = (char*)g_malloc0(strlen(T->canopy)+1);
    ftype_r  = (char*)g_malloc0(strlen(T->root)+1);
											
    columns = getStringColumn_get_Size(this->T_m->canopy,(char *) ".");
    if (columns != 0)
    {
        getStringColumn(ftype_c,this->T_m->canopy,(char *) ".",columns-1);
    }
   

    columns = getStringColumn_get_Size(this->T_m->root,(char *) ".");
    if (columns != 0)
    {
        getStringColumn(ftype_r,this->T_m->root,(char *) ".",columns-1);
    } 

	
/*	for(int i =0;i<1000;i++)
	{
		if(getStringColumn(ftype_c,T->canopy,(char *)".",i))
			getStringColumn(ftype_c,T->canopy,(char *)".",i-1);
	}
	for(int i =0;i<1000;i++)
	{
		if(getStringColumn(ftype_r,T->root,(char *)".",i))
			getStringColumn(ftype_r,T->root,(char *)".",i-1);
	}           */                                                                                                                                                                                                                                                                                                   
	                                                                   
	                                                                                                                                                                             
	int n_r =0;		//number of roots
	int n_c =0;		//number of branches

	//-------------------------- *.csv ----------------------------------------------
	if(!strcmp(ftype_c,"csv"))
	{
        infile = fopen(T->canopy ,"r");
        line = fgets_mem(infile);
		//std::ifstream c_in(T->canopy,std::ifstream::in);  
		//c_in.getline(line,500);  
		//while(!c_in.eof())     
        free(line);
		while(1)                                                                                                 
		{             
           line = fgets_mem(infile);
           if (line==NULL) break;
            //entry  = (char*)g_malloc0(strlen(line)+1);
			//c_in.getline(line,500);                                                                                           
			n_c++;  
            free(line);
		} 
        fclose(infile);
		//c_in.close(); 
	}

	if(!strcmp(ftype_r,"csv"))
	{
        infile = fopen(T->root ,"r");
        line = (char*)fgets_mem(infile);
		//std::ifstream r_in(T->root,std::ifstream::in);  
		//r_in.getline(line,500);  
		//while(!r_in.eof()) 
        free(line);
		while(1)                                                                                                 
		{             
           line = fgets_mem(infile);
           if (line==NULL) break;
			//r_in.getline(line,500);                                                                                           
			n_r++;  
            free(line);
		} 
        fclose(infile);
		//r_in.close(); 
	}

	//-------------------------- *.tlp ----------------------------------------------

	if(!strcmp(ftype_c,"tlp"))
	{
        infile = fopen(T->canopy ,"r");
		//std::ifstream c_in(T->canopy,std::ifstream::in);  
		//while(c_in.good())
		while(1)
		{
            line = (char*)fgets_mem(infile);
             if (line==NULL) break;
			//c_in.getline(line,1000000);
            entry  = (char*)g_malloc0(strlen(line)+1);
			getStringColumn(entry,line,(char *)" ",0);		 
			if(!strcmp("(node",entry))
				n_c++;
            free(line);
            g_free(entry);
		}
         fclose(infile);
		//c_in.close();
	}
    
	if(!strcmp(ftype_r,"tlp"))
	{
        infile = fopen(T->root ,"r");
		//std::ifstream r_in(T->root,std::ifstream::in);   
		//while(r_in.good())
		while(1)
		{
            line = (char*)fgets_mem(infile);
            if (line==NULL) break;
            entry  = (char*)g_malloc0(strlen(line)+1);
			//r_in.getline(line,1000000);
			getStringColumn(entry,line,(char *)" ",0);		 
			if(!strcmp("(node",entry))
				n_r++;
            free(line);
            g_free(entry);
		}
        fclose(infile);
		//r_in.close(); 
	}

    g_free(ftype_c);
    g_free(ftype_r);

	//-------------------------- Root Generator----------------------------------------------
	if((T->RootSwitch == 1)|| (T->RootSwitch == 2)) //no root file
	{
		//the number of root nodes the generator will generate
		n_r = 1 + T->RN1*T->RN2 
			  + T->RN1*T->RN2*T->RN3 
			  + T->RN1*T->RN2*T->RN3*T->RN4 
			  + T->RN1*T->RN2*T->RN3*T->RN4*T->RN5 
			  + T->RN1*T->RN2*T->RN3*T->RN4*T->RN5*T->RN6
			  + T->RN1*T->RN2*T->RN3*T->RN4*T->RN5*T->RN6*T->RN7;
	}
    
    if((n_r == 1) || (T->GG == 0.0) || (T->dp1== 0))
        {
        PRINT_ERROR_ID(xpn, "Xylemwaterflow input error, check plant.RNx, plant.GG and plant.dp1! Standard values are used for all root generator variables.");
        T->RL1 = 8.0;
        T->RL2 = 0.5;
        T->RL3 = 2.0;
        T->RL4 = 1.5;
        T->RL5 = 1.0;
        T->RL6 = 0.75;
        T->RN1  = 5;
        T->RN2 = 3;
        T->RN3 = 2;
        T->RN4 = 3;
        T->RN5 = 3;
        T->RN6 = 2;
        T->RN7 = 2;
        T->GG = 0.93;
        T->dp1 = 360.0 / T->RN1;
        n_r = 1 + T->RN1*T->RN2 
			  + T->RN1*T->RN2*T->RN3 
			  + T->RN1*T->RN2*T->RN3*T->RN4 
			  + T->RN1*T->RN2*T->RN3*T->RN4*T->RN5 
			  + T->RN1*T->RN2*T->RN3*T->RN4*T->RN5*T->RN6
			  + T->RN1*T->RN2*T->RN3*T->RN4*T->RN5*T->RN6*T->RN7;
        }
	//--------------------------allocate nodes--------------------------	
	T->iCnodes = n_c;                                                                                                  
	T->iRnodes = n_r;
    //printf("%d \n", n_c + n_r);
	T->node = (nodes*)g_malloc0_n((n_r+n_c ),sizeof(nodes));
	//double enough =1;
}

                                                                                                                                                                                                                               
void readCSV(tree *T, bool canopy)                                                                                  
{     	
	int i;
	char *line;                                                                                                    
	char *entry;  
	//std::ifstream in;
    FILE *infile;


	if(canopy)
        infile = fopen(T->canopy ,"r");
		//in.open(T->canopy,std::ifstream::in);       
	else
        infile = fopen(T->root ,"r");
//		in.open(T->root,std::ifstream::in);                                                                              
	
    line = fgets_mem(infile);
    //in.getline(line,500);//header       
    free(line);
	

	if(canopy)
		i=0;     
	else
		i=T->iCnodes;
    
    
    free(line);
	while(1)                                                                                                
	{               
        line =  fgets_mem(infile);
         if (line==NULL) break;
        entry  = (char*)g_malloc0(strlen(line)+1);
		//in.getline(line,500);
        getStringColumn(entry,line,(char *)";",0);                                                       
		T->node[i].X = (double)atof(entry)*1000;//m->mm;                                                                   
                                                                                                                    
		getStringColumn(entry,line,(char *)";",1);                                                                                
		T->node[i].Y = (double)atof(entry)*1000;//m->mm;                                                                   
                                                                                                                    
		getStringColumn(entry,line,(char *)";",2);                                                                                
		T->node[i].Z = (double)atof(entry)*1000;//m->mm;                                                                   
                                                                                                                    
		getStringColumn(entry,line,(char *)";",3); 

		if(canopy)
			T->node[i].parent = atoi(entry);
		else
            T->node[i].parent = atoi(entry)+T->iCnodes-1; //the "-1" is special for the root files from michael (csv)                                                                                                                                                                          
                                                                                                                    
		T->node[i].canopy = canopy;                                                                                            
		i++;             
        free(line);
        g_free(entry);
	}       
	//connect stem
	if(canopy)
		T->node[0].parent=T->iCnodes;
	if(!canopy)
		T->node[T->iCnodes].parent=0;
	fclose(infile);
    //in.close();                                                                                                                    
}     

void readTLP(tree *T, bool canopy, double fSoilLayerThickness)                                                                                  
{   
	//set x,y,z, parentID, first node is the stem/root
	char *line; //tlp 2.0 contains one big line
	char *dest;
	char *dest2;
	char *dest3;
    FILE *infile;
    FILE *infile2;
	//std::ifstream in, in2;	
	edge dummyedge;
	int i;


	if(canopy)	
	{
        infile = fopen(T->canopy ,"r");
		//in.open(T->canopy);
		i=0;
	}
	else		
	{
        infile = fopen(T->root ,"r");
		//in.open(T->root);
		i=T->iCnodes;
	}


	//--------------------READ THE NODES----------------------------
	double minz_c  =1e6;
    double minz_r =1e6;  //the stem will be shifted to canopy: 0,0,0,0.10m
								//		  root: 0,0,-0.005m
	int  minID_c = 0;
    int minID_r = 0;

	//while(in.good())
    while(1)
	{
        line = fgets_mem(infile);
        if (line==NULL) break;
        dest = (char*)g_malloc0(strlen(line)+1);
        dest2 = (char*)g_malloc0(strlen(line)+1);
        dest3 = (char*)g_malloc0(strlen(line)+1);
		//in.getline(line,1000000);
		getStringColumn(dest,line,(char*)" ",0);		 
		if(!strcmp("(node",dest))
		{
			getStringColumn(dest,line,(char *)" ",2);
			//x
			getStringColumn(dest2,dest,(char *)",",0);
			getStringColumn(dest3,dest2,(char *)"(",1);
			T->node[i].X=atof(dest3)*1000;//m->mm 
			//y
			getStringColumn(dest2,dest,(char *)",",1);
			T->node[i].Y=atof(dest2)*1000;//m->mm 
			//z
			getStringColumn(dest2,dest,(char *)",",2);
			getStringColumn(dest3,dest2,(char *)")",0);
			T->node[i].Z=atof(dest3)*1000;//m->mm 
			if(canopy)
			{
				if(fabs(T->node[i].Z)<minz_c)
				{
					minz_c=T->node[i].Z;
                    minID_c=i;		
				}	
			}
			else
			{
                // for testing, file wrong with a factor 10
                //T->node[i].X /= 10.0;
                //T->node[i].Y /= 10.0;
                //T->node[i].Z /= 10.0;
                 //if ((int)T->node[i].Z % (int)(fSoilLayerThickness) == 0)
                //{
                //   T->node[i].Z -= 1.0;
                  // printf("%f %f \n",T->node[i].Z, fSoilLayerThickness);
                //}
				if(fabs(T->node[i].Z)<minz_r)
				{
					minz_r=T->node[i].Z;
                    minID_r=i;
                 //   printf("%d %d %f  %f\n",i, minID_r, minz_r, T->node[i].Z);
				}
			}
			T->node[i].parent = -99; //init
			T->node[i].canopy = canopy;
			//if(T->node[i].canopy == 1)
			//	printf("node %d %d \t%f %f %f \n",i,T->node[i].parent ,T->node[i].X,T->node[i].Y,T->node[i].Z);
            
            //printf("%d %d \n", i, T->node[i].parent);
			i++;


		}
    free(line);
    g_free(dest);
    g_free(dest2);
    g_free(dest3);
	}
    
    //T->node[i].parent = -99;
    //T->node[i].canopy = canopy;
    
    fclose(infile);
    
	//in.close();
	//--------------------Shift the coordinates to (0/0/0) ----------------------------
	double sx;  
	double sy;  
	if(canopy)
	{
		minz_c -= 50.0; //5cm hight
		sx = T->node[minID_c].X;
		sy = T->node[minID_c].Y;
		for(int i=0;i<T->iCnodes;i++)
		{
			T->node[i].Z -= minz_c;
			T->node[i].X -= sx;
			T->node[i].Y -= sy;
		}
		 
	}
	else
	{
		minz_r += 10.0; //1cm depth
		sx = T->node[minID_r].X;
		sy = T->node[minID_r].Y;
		for(int i=T->iCnodes;i<T->iCnodes+T->iRnodes;i++)
		{
			T->node[i].Z -= minz_r;
			T->node[i].X -= T->node[minID_r].X;
			T->node[i].Y -= T->node[minID_r].Y;
            //printf("%d %f %f %f \n",i,  T->node[i].X, T->node[i].Y,T->node[i].Z);
		}
	 
	}
	//--------------------READ THE EDGES----------------------------
	std::vector<edge> E(0);
	int offset=0;
	if(!canopy) offset = T->iCnodes;
/*	if(canopy)
		in2.open(T->canopy);
	else
		in2.open(T->root);*/

	if(canopy)	
	{
        infile2 = fopen(T->canopy ,"r");
		//in.open(T->canopy);
		i=0;
	}
	else		
	{
        infile2 = fopen(T->root ,"r");
		//in.open(T->root);
		i=T->iCnodes;
	}

	int iedges=0;
	int a,b;
    
	//while(in2.good())
    while(1)
	{
        line = fgets_mem(infile2);
        if (line==NULL) break;
        dest = (char*)g_malloc0(strlen(line)+1);
        dest2 = (char*)g_malloc0(strlen(line)+1);
        dest3 = (char*)g_malloc0(strlen(line)+1);
		//in2.getline(line,1000000);
		getStringColumn(dest,line,(char *)" ",0);		 
		if(!strcmp("(edge",dest))
		{	//a
			getStringColumn(dest,line,(char *)" ",2);
			a=atoi(dest) + offset;
			//b
			getStringColumn(dest,line,(char *)" ",3);
			getStringColumn(dest2,dest,(char *)")",0);
			b=atoi(dest) + offset;
            
			dummyedge.set(a,b);
			E.insert(E.end(),dummyedge );
			iedges++;

		}
    free(line);
    g_free(dest);
    g_free(dest2);
    g_free(dest3);
	}
    fclose(infile2);
	//in2.close();
    
	//--------------------FIND PARENTS----------------------------
	//Define node with the lowest |z| as the stem node
	if(canopy)
		T->node[minID_c].parent=T->iCnodes;//all other have -99 at this point
	else
		T->node[minID_r].parent=0;

	//loop over all points: find connected one
	int partner, iparents=0, minIDbr;
	int startindex, endindex;
	if(canopy)
	{
		startindex = 0;
		endindex=T->iCnodes;
		minIDbr=minID_c;
	}
	else
	{
		startindex = T->iCnodes;
		endindex= T->iCnodes+T->iRnodes;
		minIDbr=minID_r;
	}
  

for(int i=startindex;i<endindex;i++)
	{
    for(int j=iedges-1;j>=0;j--)//loop over all edges  
    {

		if(i!=minIDbr)
        {
           //printf("%d %d \n",i,  j);
			if(E[j].a == i) 
            {
				partner=E[j].b;
                //printf("a = i %d %d %d %d \n",i,  j, partner, T->node[partner].parent);
			}
            else if(E[j].b == i) 
            {
				partner=E[j].a;
                //printf("b = i %d %d %d %d \n",i,  j, partner, T->node[partner].parent);
			}
            else 
				continue;
           // printf("i %d j %d partner %d parent %d E.a %d E.b %d\n", i, j, partner, T->node[partner].parent, E[j].a, E[j].b);
			//if the partner has already a parent, then the partner is my parent
			//pp = T->node[partner].parent;
             if(T->node[partner].parent >= 0) 
			{           
				T->node[i].parent = partner;
				//if(T->node[i].parent == i)
					//int bug=1;
				//if(T->node[i].canopy == 1)
				//	printf("edges %d %d %d \t%f %f %f \n",i,j,T->node[i].parent ,T->node[i].X,T->node[i].Y,T->node[i].Z);
				iparents++; //has to equal inodes-1 at the end of the day
				break;
			}
            if(T->node[i].parent == -99 && i!=minIDbr) // not found
            {
                int exception =1;
            }
        }
    }
 }



    

    
	
    //connect stem
	if(!canopy)
    {
        T->node[0].parent=minID_r;
		T->node[minID_r].parent=0;
        //printf("Test \n");
    }  
    
        //for(int i=startindex;i<endindex;i++)
        // printf("%d %d \n", i, T->node[i].parent);
    
    //printf("%d %d %d \n", minID_r, T->node[0].parent, T->node[minID_r].parent);    
        
}

void GenerateRoots(class tree *T)
{
	for(int i=T->iCnodes;i<T->iCnodes+T->iRnodes;i++)
		T->node[i].canopy =0;

	//first we are doing everything in [m], at the end its converted to mm
	int inodes=T->iCnodes;
	//first element
	T->node[inodes].X = 0.0; 
	T->node[inodes].Y = 0.0;
	T->node[inodes].Z = -0.01;//1cm depth
	T->node[inodes].parent = 0; 
	T->node[inodes].sub = 1;
	inodes++;

	struct direction dir;

	//make second order nodes // -> radial root density distribution
	for(int i=0;i<T->RN1;i++)
	{
			dir.x = cos(T->dp1*i/360*2.0*PI);
			dir.y = sin(T->dp1*i/360*2.0*PI);
			dir.z = 0.0;
			//int check = T->node[T->iCnodes].parent;
			makeSubBranch(T,&inodes,T->node[T->iCnodes],T->iCnodes,dir,T->RL1,T->RN2,LINEAR_DECREASE);	//horizontal
			 
	}
	
	//make third order nodes -> vertical root density distribution
	int sofar =inodes;
	for(int i=0;i<sofar;i++) //loop over all nodes existing yet and check if second order branch should be attached
	{
		if(T->node[i].sub==2)
		{
			dir.x = 0.0;
			dir.y = 0.0;
			dir.z = -1.0;
			makeSubBranch(T,&inodes,T->node[i],i,dir,T->RL2,T->RN3,GALE_GRIGAL);		
		}
	}

	//make 4th order nodes  
	sofar =inodes;
	for(int i=0;i<sofar;i++) //loop over all nodes existing yet and check if second order branch should be attached
	{
		if(T->node[i].sub==3)
		{
			dir.x = rand()-RAND_MAX*0.5;
			dir.y = rand()-RAND_MAX*0.5;
			dir.z = 0.0;
			makeSubBranch(T,&inodes,T->node[i],i,dir,T->RL3,T->RN4,HOMOGEN);		
		}
	}

	//make 5th order nodes
	sofar =inodes;
	for(int i=0;i<sofar;i++) //loop over all nodes existing yet and check if second order branch should be attached
	{
		if(T->node[i].sub==4)
		{
			dir.x = rand()-RAND_MAX*0.5;
			dir.y = rand()-RAND_MAX*0.5;
			dir.z = 0.0;
			makeSubBranch(T,&inodes,T->node[i],i,dir,T->RL4,T->RN5,HOMOGEN);		
		}
	}
	//make 6th order nodes
	sofar =inodes;
	for(int i=0;i<sofar;i++) //loop over all nodes existing yet and check if second order branch should be attached
	{
		if(T->node[i].sub==5)
		{
			dir.x = rand()-RAND_MAX*0.5;
			dir.y = rand()-RAND_MAX*0.5;
			dir.z = 0.0;
			makeSubBranch(T,&inodes,T->node[i],i,dir,T->RL5,T->RN6,HOMOGEN);		
		}
	}

	//make 7th order nodes
	sofar =inodes;
	for(int i=0;i<sofar;i++) //loop over all nodes existing yet and check if second order branch should be attached
	{
		if(T->node[i].sub==6)
		{
			dir.x = rand()-RAND_MAX*0.5;
			dir.y = rand()-RAND_MAX*0.5;
			dir.z = 0.0;
			makeSubBranch(T,&inodes,T->node[i],i,dir,T->RL6,T->RN7,HOMOGEN);		
		}
	}
 
	//convert to mm
	for(int i=T->iCnodes;i<T->iCnodes+T->iRnodes;i++)
	{
		T->node[i].X *= 1000;
		T->node[i].Y *= 1000;
		T->node[i].Z *= 1000;
	}
	//int end=1;
    
    	//connect stem

	T->node[0].parent=T->iCnodes;
    T->node[T->iCnodes].parent=0;
    
}

void distribute(double *dist, int n, double L, int distribution, double GG)
{
	/*
			n(x): numerical cum. distribution function
			f(x): analytical function, f.e cdf_linear
	*/
	//n++; //numerical issue: the last one is not made
	double nx=0.0, fx;
	int maxstep =1000;//make 5000 steps between 0 and L
	double dx = L/(double)maxstep;

	double ystep = 1.0/(double)n; //stepwith y of the numerical cum dist function
	int c=-1; 

	for(int step=1;step<maxstep;step++)
	{	 
		
		double xi = step*dx;
		switch(distribution)
		{
		case HOMOGEN:
			fx = cdf_homogen(xi,L); //const. distr. function, linear cum. dist. function
			break;
		case INVERSE_SQUARE:
			fx = cdf_inverseSquareLaw(xi,L);
			break;
		case LINEAR_DECREASE: //as measured by Meinen for radial distribution
			fx = cdf_linearDecrease(xi,L);
			break;
		case GALE_GRIGAL:
			fx = cdf_GaleGrigal(xi,GG);//as measured by Meinen for vertical distribution
			break;
		}
	//	if(order==2)
	//		fx = cdf_GaleGrigal(xi,beta);
	//	if(order>2)
	//		fx = cdf_linearDecrease(xi,L);
		//fx = cdf_linear(xi,L); //const. distr. function, linear cum. dist. function
		//  
		//fx = cdf_inverseSquareLaw(xi,L);
		//if n(x+1) < f(x) add node here
		if(nx + ystep < fx  )  
		{
			c++;
			dist[c] = xi;		
			nx += ystep;
		}
		dist[n-1]=L;
	}
	//if(c+1 > n)
	//	int doh =1;
}

//collection of cumulative distribution functions
//strictly increasing: 0...1 
//parameter: x, xmax

//const. distr. function, linear cum. dist. function
double cdf_homogen(double x, double xmax)
{
	return 1.0/xmax * x;
}
double cdf_inverseSquareLaw(double x, double xmax)
{
	return pow(x,2.0) / pow(xmax,2.0);
}
//Meinen: linear decrease of radial biomass density -> cum. Function Mx - 0.5M/Lx^2
double cdf_linearDecrease(double x, double xmax)
{
	return (x-0.5/xmax*x*x)/(0.5*xmax);
}	

double cdf_GaleGrigal(double x, double GGbeta)// input x in m
{
	return 1.0 - pow(GGbeta,x*100);
}


void makeSubBranch(class tree *T, int *inodes, class nodes start, int startID, struct direction dir , double length, int n, int distribution)
{
	int pID;
	//normalize direction
	double norm = sqrt( pow(dir.x,2)+ pow(dir.y,2) +pow(dir.z,2));
	//set distances on branch
	double * dl= new double[n]; //contains the distances from the stem
    
	distribute(dl,n,length,distribution, T->GG);
	//make n new nodes
	for(int i=0;i<n;i++)
	{
		if(i==0) pID = startID;
		else	pID = *inodes-1;
		T->node[*inodes].X = start.X + dir.x*dl[i]/norm;
		T->node[*inodes].Y = start.Y + dir.y*dl[i]/norm;
		T->node[*inodes].Z = start.Z + dir.z*dl[i]/norm;
		T->node[*inodes].parent = pID; 
		T->node[*inodes].sub = start.sub +1;
		*inodes+=1;
	}
	delete [] dl;
}

void writeTLP(class tree T,char * outname, int what)//what: 1=canopy, 0=root, 2=both
{
	int sindex, eindex;
	switch(what)
	{
	case 2: 
		eindex= T.iCnodes+T.iRnodes;
		sindex=0;
		break;
	case 1: 
		sindex = 0;
		eindex = T.iCnodes;
		break;
	case 0:
		eindex = T.iCnodes+T.iRnodes;
		sindex = T.iCnodes;
	}
		
	if(what>0)
	{
		std::ofstream of(outname);
		//header
		of << "(tlp \"2.0\"\n";
		of << "(nodes";
		for(int i=sindex;i<eindex;i++)
			of <<" "<<i;
		of <<")\n";

		//edges
		for(int i=sindex+1;i<eindex;i++) 
		{
			 //if(N[i].sub <= 5)
			of << "(edge " << i << " "<< i << " " << T.node[i].parent<<")\n";
		}
		//properties
		of << "\n(property 0 color \"viewColor\"\n(default \"(0,0,0,255)\" \"(0,0,0,255)\")\n)\n";
		of << "\n(property 0 size \"viewSize\"\n(default \"(0.005,0.005,0.05)\" \"(10,10,10)\")\n)\n";
		of << "\n(property 0 layout \"viewLayout\"\n(default \"(0,0,0)\" \"()\")\n";
		//nodes
		
		for(int i=sindex;i<eindex;i++) 
		{
			of << "(node " << i << " \"(" << T.node[i].X*0.001+i*1e-5<<","<< T.node[i].Y*0.001<<","<< T.node[i].Z*0.001<<")\")\n";
		}
		of<<")\n)";
		of.close();
	}

	if(what==0)
	{
		std::ofstream of(outname);
		//header
		of << "(tlp \"2.0\"\n";
		of << "(nodes";
		for(int i=T.iCnodes; i<T.iCnodes+T.iRnodes; i++)
			of << " " << i-T.iCnodes;
		of <<")\n";

		//edges
		//first edge
		for(int i=T.iCnodes+1; i<T.iCnodes+T.iRnodes; i++) 
		{
			 //if(N[i].sub <= 5)
			of << "(edge " << i-T.iCnodes << " "<< i-T.iCnodes << " " << T.node[i].parent-T.iCnodes<<")\n";
		}
		//properties
		of << "\n(property 0 color \"viewColor\"\n(default \"(0,0,0,255)\" \"(0,0,0,255)\")\n)\n";
		of << "\n(property 0 size \"viewSize\"\n(default \"(0.005,0.005,0.05)\" \"(10,10,10)\")\n)\n";
		of << "\n(property 0 layout \"viewLayout\"\n(default \"(0,0,0)\" \"()\")\n";
		//nodes
		
		for(int i=T.iCnodes; i<T.iCnodes+T.iRnodes; i++)
		{
			of << "(node " << i-T.iCnodes << " \"(" << T.node[i].X*0.001+i*1e-5<<","<< T.node[i].Y*0.001<<","<< T.node[i].Z*0.001<<")\")\n";
		}
		of<<")\n)";
		of.close();
	}
}

void writeOUTD(class tree T,char * outname, int what)//what: 1=canopy, 0=root, 2=both
{
	int offset;
	int sindex, eindex;
	switch(what)
	{
	case 2: 
		eindex= T.iCnodes+T.iRnodes;
		sindex=0 ;
		offset=0;
		break;
	case 1: 
		sindex = 0 ;
		eindex = T.iCnodes;
		offset = 0;
		break;
	case 0:
		eindex = T.iCnodes+T.iRnodes;
		sindex = T.iCnodes ;
		offset= T.iCnodes;
	}
	std::ofstream of(outname);
    of << "id;X;Y;Z;Total Diameter;H_total;parentID\n";
	for(int i=sindex;i<eindex;i++)//dont write the first one, thats a dummy, second one also has no edge
	{
		//int pa=T.node[i].parent;
		of << i-offset << " ;" <<  T.node[i].X*0.001 << " ;" <<  T.node[i].Y*0.001 << " ;" <<  T.node[i].Z*0.001 << ";" 
			<<  T.node[i].TotalDiameter << " ;" <<  T.node[i].H_total/101971.621298 << " ;" <<  T.node[i].parent-offset << "\n";
		/*of << T.node[i].X*0.001 << " " <<  T.node[i].Y*0.001 << " " <<  T.node[i].Z*0.001 << " "
			<<  T.node[pa].X*0.001 << " " <<  T.node[pa].Y*0.001 << " " <<  T.node[pa].Z*0.001<< " "
			<<  T.node[i].TotalDiameter*0.001 << " \n";*/
	}

	of.close();
}

double abstand(class nodes n1, class nodes n2)
{
	return sqrt( (n1.X-n2.X)*(n1.X-n2.X) + (n1.Y-n2.Y)*(n1.Y-n2.Y) + (n1.Z-n2.Z)*(n1.Z-n2.Z) );
}

void tree::getStrahler()
{
	for(int i=0; i< iCnodes+iRnodes;i++)
	{
		if(node[i].child.size() ==0)//terminal
			node[i].Strahler=1;
		else
			node[i].Strahler=-99; //init
	}
	//canopy
	for(int i=0; i< iCnodes+iRnodes;i++)
	{	
		if(node[i].child.size() ==0)//terminal
		{
		int current = i;
		int pa = node[current].parent;
		while(current!=iCnodes && current!=0)	//including node 0
		{
			if(node[pa].Strahler != node[current].Strahler)
				node[pa].Strahler = max(node[pa].Strahler, node[current].Strahler);
			else if(node[pa].Strahler == node[current].Strahler)
				node[pa].Strahler++;
			current = node[current].parent;//iterate
			pa = node[current].parent;//iterate			
		}
		}
	}
}

//Calculates the xylem area of every node
void  XWF_CLASS::XylemArea(class tree *T) 
{
	//start with stem
	double *totallength = new double;
	double *childlength = new double;
	double anteil;
	double anteilsum;
	T->node[0].XylemArea = T->max_XylemArea_stem;
    
	for(int i=0; i<T->iCnodes; i++)
	{
       //printf("child size %d \n", T->node[i].child.size());
       //for(int j =0; j < (int)T->node[i].child.size() ;j++)
        //        printf("%d  %d \t%f %f %f \t %d %d %f \n",i,T->node[i].parent ,T->node[i].X,T->node[i].Y,T->node[i].Z,T->node[i].child.size(), T->node[i].child.at(j), T->node[i].XylemArea);
		double actualArea = T->node[i].XylemArea;
		if((int)T->node[i].child.size() == 1) //single child as fat as parent
		{
			int childID = T->node[i].child.at(0);
			T->node[childID].XylemArea = actualArea; 		
		}
		if((int)T->node[i].child.size() > 1)  //more childs have to share according to pipe model
		{		
			*totallength =0.0;
			anteilsum=0;
			T->node[i].collectAdjacentLength(totallength,T);
                        //printf("Test \n");
			for(int j=0; j< (int)T->node[i].child.size();j++)//loop over all childs
			{
				int childID = T->node[i].child.at(j);		
				*childlength =0.0;
				T->node[childID].collectAdjacentLength(childlength,T);//add the adjectent length to childlength
				*childlength += T->node[childID].l; //add the length of the child to childlength
				anteil = *childlength / *totallength;
				T->node[childID].XylemArea = actualArea * anteil;
				anteilsum += anteil;
			}
			//int ready=1;
		}
	}

   //printf("Test \n");

	//start with root
	T->node[T->iCnodes].XylemArea = T->max_XylemArea_root;
	for(int i=T->iCnodes; i<T->iCnodes+T->iRnodes; i++)
	{
		double actualArea = T->node[i].XylemArea;
		if((int)T->node[i].child.size() == 1) //single child as fat as parent
		{
			int childID = T->node[i].child.at(0);
			T->node[childID].XylemArea = actualArea; 		
		}
		if((int)T->node[i].child.size() > 1) //more childs have to share according to pipe model
		{		
			*totallength =0.0;
			anteilsum=0;
			T->node[i].collectAdjacentLength(totallength,T);
			for(int j=0; j< (int)T->node[i].child.size();j++)//loop over all childs
			{
				int childID = T->node[i].child.at(j);		
				*childlength =0.0;
				T->node[childID].collectAdjacentLength(childlength,T);//add the adjectent length to childlength
				*childlength += T->node[childID].l; //add the length of the child to childlength
				anteil = *childlength / *totallength;
				T->node[childID].XylemArea = actualArea * anteil;
				anteilsum += anteil;
			}
			//int ready=1;
		}
    //for(int j =0; j < (int)T->node[i].child.size() ;j++)
    //        printf("%d  %d \t%f %f %f \t %d %d %f \n",i,T->node[i].parent ,T->node[i].X,T->node[i].Y,T->node[i].Z,T->node[i].child.size(), T->node[i].child.at(j), T->node[i].XylemArea);
	 
         
         
	}
	delete totallength;
	delete childlength;
	//int halt=2;
}

//calculates or reads from file the total diameters
void XWF_CLASS::totalDiameter(class tree *T)
{
	double area;
	for(int i=0;i<T->iCnodes+T->iRnodes;i++)
	{	//Aranda 2005
		if(strcmp(this->conf.xylemfractionCalculation,"a")==0)
		{		 
			T->node[i].TotalDiameter = 10.0 * pow(T->node[i].XylemArea*(double)(0.01/0.49),(double)(1.0/2.13));//mm
		}
		if((strcmp(this->conf.xylemfractionCalculation,"c")==0) || (strcmp(this->conf.xylemfractionCalculation,"g")==0))
		{		 
			if(T->node[i].canopy)
				area= T->node[i].XylemArea / T->xylemfraction_stem;
			if(!T->node[i].canopy)
				area= T->node[i].XylemArea / T->xylemfraction_root;
			T->node[i].TotalDiameter = 2.0*sqrt(area/PI);
		}
		//printf("Diameter %f %f %f %f %f \n", T->node[i].TotalDiameter, area, T->node[i].XylemArea, T->xylemfraction_root, T->xylemfraction_stem);
	}
}

double nodes::collectAdjacentLength(double *sum, class tree *T)
{
	int ii= this->child.size();
    //printf("ii %d %d %d \n", ii, this->child.size(), this->parent);
	this->gloii=ii;
	if(ii>0)
	{
	for(int i=0;i<ii;i++)
	{	
		this->gloi=i;
       //printf("sum %f %f  %d %d %d\n", *sum,T->node[this->child.at(i)].l, this->child.at(i), i), this->child.at(1);
		*sum+= T->node[this->child.at(i)].l;
        //if(this->child.at(i)==i)
         //  break;
		T->node[this->child.at(i)].collectAdjacentLength(sum,T);
	}	 
	}
	return *sum;
}

// sets T->node[i].LA
// sum over all nodes = 1.0.
// distribution has to be proportional to the lenght of the cylinder

void XWF_CLASS::genLA(class tree *T)
{	
    //xpn->pPl->pGenotype->acCropCode = "FA";
	T->iLeafNodes =0;
	double sum=0, sum2 = 0; //to normalize
    double leaflength, maxwidth, position;
	for(int i=T->iCnodes;i<T->iCnodes+T->iRnodes ;i++)
	{
		T->node[i].LA =0.0;
        T->node[i].width = 0.0;
	}
	//---------------------------------------------------
	//hompogen distribution
	//---------------------------------------------------
	if(T->LAIDistribution==0) 
	{
        if(strcmp(xpn->pPl->pGenotype->acCropCode,"MZ")==0)
        {
            for(int i=0;i<T->iCnodes ;i++)
                {
                if((int)T->node[i].Strahler <= this->conf.maxStrahler)
                    {
                    int j=i;
                    leaflength = 0.0;
                    maxwidth = 0.0;
                    position = 0.0;
                    while(T->node[j].child.size() <=1)
                        {
                        T->node[j].LA = T->node[j].l; //proportional to length of the element
                        T->iLeafNodes++;
                        leaflength +=  T->node[j].l;
                        j = T->node[j].parent;
                        }
                        // empirical function, maxwidth depending on leaflength
                        position = leaflength;
                        maxwidth = 0.106*leaflength;
                        j = i;
                     while(T->node[j].child.size() <= 1)
                        {
                        position -= T->node[j].l/2.0;
                        T->node[j].width=(-2.5*(position*position/(leaflength*leaflength))+1.84*position/leaflength+0.66)*maxwidth;
                        T->node[j].LA *= T->node[j].width;
                        position -= T->node[j].l/2.0;
                         j = T->node[j].parent;
                        }
                    }
                }
           for(int i=0;i<T->iCnodes ;i++)
           {
               sum+=T->node[i].LA ;
           }
/*               char *S;
               S = g_strdup_printf("The calculated leaf area amounts to %f mm.", sum);
               PRINT_MESSAGE(xpn,3,S);
               g_free(S);*/
            //normalize
            for(int i=0;i<T->iCnodes;i++)
            {
                T->node[i].LA/=sum;
                sum2 += T->node[i].LA;
            }
        }
        else
        {
		for(int i=0;i<T->iCnodes ;i++)
		{
			if((int)T->node[i].Strahler <= this->conf.maxStrahler) // && (T->node[i].child.size()==0))
            {
			T->node[i].LA = T->node[i].l; //proportional to length of the element
			T->iLeafNodes++;
            }
            else
                T->node[i].LA =0;
            sum+=T->node[i].LA ;
            }
		//normalize
		for(int i=0;i<T->iCnodes;i++)
		{
			T->node[i].LA/=sum;
            sum2 += T->node[i].LA;
		}	
        }
		     //  printf("%.10f \n", sum2);
	}

	//---------------------------------------------------
	//Distribute from histogram file
	//---------------------------------------------------
	if(T->LAIDistribution==1) 
	{
		//load file
		std::vector<double> h(0);
		std::vector<double> n(0);
		double dummyh;
		double dummyn, totaln=0;
		std::ifstream in(T->LAIHisto);
		while(in.good())
		{
			in >> dummyh >>dummyn;
			h.insert(h.end(),dummyh);
			n.insert(n.end(),dummyn);
			totaln += dummyn;
		}
		h.erase(--h.end());n.erase(--n.end()); //no entries in last line
		
		in.close();
		//int a = n.size();

		//count the elements in a histogramm layer that have leaves
		double *inodes = new double[n.size()];
		//init
		for(int i=0;i<(int)n.size();i++) 
			inodes[i]=0;
		for(int i=0;i<T->iCnodes;i++)
		{
			double height = 1e-3*(T->node[i].Z+T->node[T->node[i].parent].Z ) *0.5;//[m]
			if((int)T->node[i].Strahler <= this->conf.maxStrahler)
			{
				for(int j=0; j<(int)h.size();j++) //loop over histogram
				{
					if(height<h.at(j))//
					{
						inodes[j]++; 
	
						break;
					}
				}//loop over histogram
				if (height>h.at(h.size()-1))//highest layer
				{
					inodes[h.size()-1]++;
				
				}
			}
		}
		//distribute
		for(int i=0;i<T->iCnodes;i++)
		{
			double height = 1e-3*(T->node[i].Z+T->node[T->node[i].parent].Z ) *0.5;//[m]
			if((int)T->node[i].Strahler <= this->conf.maxStrahler)
			{
				for(int j=0; j<(int)h.size();j++) //loop over histogram
				{
					if(height<h.at(j))//
					{
						T->node[i].LA = (double)n.at(j)	//proportional to n, the number of voxles of the layer
										//* T->node[i].l		//proportional to length of the element 
										/ inodes[j];		//reziprocal prop to the total elements in the hight of the histogramm layer
						T->iLeafNodes++;
						break;
					}
				}//loop over histogram
				//double hh = inodes[n.size()-1];
				if (height>h.at(h.size()-1))//highest layer
				{
					T->node[i].LA = (double)n.at(n.size()-1) / inodes[n.size()-1];//* T->node[i].l; //proportional to length of the element and to n
				}
			}
			else
				T->node[i].LA =0;

			sum+=T->node[i].LA ;
		}
		//normalize
		//double LAI[30];
		for(int i=0;i<T->iCnodes;i++)
		{
			T->node[i].LA /= sum;
			//LAI[i] = T->node[i].LA;
		}
		//int stioo =5;
		delete [] inodes;
	}
}

void XWF_CLASS::RootSurfADistrSoil(class tree *T)//Computes the distribution of root surface areas over the root elements
{
	//double sum = 0.0;
	
	int layers;
	
	layers = xpn->pSo->iLayers-2;
	//calculate depth of layers, init rootSALayer
	this->depth=(double*)malloc((layers+1)*sizeof(double));
	this->depth[0]=0;
	for(int j=1;j<layers;j++)
		this->depth[j]=this->depth[j-1]+(double)this->boden[j].dz;

	this->depth[layers]=this->depth[layers-1]+this->boden[layers-1].dz;//lower border of soil profile


	for(int nodin=T->iCnodes; nodin<T->iCnodes+T->iRnodes; nodin++)
	{
		//jedes Nodium ist an Internodien angechlossen. Ein Wurzelnodium zieht Wasser aus allen Bodenschichten, die von den
		//anliegenden Nodien geschnitten werden, wichtig vor allem für GBF_sub
		if(fabs(T->node[T->node[nodin].parent].Z) > fabs(T->node[nodin].Z)) //parent is deeper
		{
			T->node[nodin].unteresWurzelelement =(double)fabs(T->node[T->node[nodin].parent].Z);
			T->node[nodin].oberesWurzelelement  =(double)fabs(T->node[nodin].Z);
		}
		else if(fabs(T->node[T->node[nodin].parent].Z)<fabs(T->node[nodin].Z))//parent is upper
		{
			T->node[nodin].oberesWurzelelement=(double)fabs(T->node[T->node[nodin].parent].Z);
			T->node[nodin].unteresWurzelelement=(double)fabs(T->node[nodin].Z);
		}
		else
		{
			T->node[nodin].oberesWurzelelement=(double)fabs(T->node[T->node[nodin].parent].Z);
			T->node[nodin].unteresWurzelelement=(double)fabs(T->node[nodin].Z)+(double)0.01;
		}
	
	}

}


int intersects(double oB, double uB, int j, class tree *T)//does root node ly in soil layer?
{
	int mode=0;
	double oW;
	double uW;
	oW= T->node[j].oberesWurzelelement;
	uW=	T->node[j].unteresWurzelelement;
	if(oW<=uB&&uW>=oB)
		mode=1;
	return mode;
}

//function by Michael Janott
void XWF_CLASS::RootPartitioningToSoilLayers(class tree *T)
{
		int layers;
		layers = xpn->pSo->iLayers-2;
	
		double obereBodenschicht, untereBodenschicht;
		for(int i=0;i<T->iCnodes+T->iRnodes;i++)
			T->node[i].frac=(double*)g_malloc0_n(layers,sizeof(double));
		for(int nodin=1;nodin<T->iCnodes+T->iRnodes;nodin++)
		{
			if(!T->node[nodin].canopy)//root
			{
				double nodedZ=T->node[nodin].oberesWurzelelement - T->node[nodin].unteresWurzelelement;
				for(int soildepth=0;soildepth<layers;soildepth++)
				{//wann liegt Wurzel in Bodenschicht? mode=1
					obereBodenschicht=fabs(this->depth[soildepth]);//bigger soildepth is deeper
					untereBodenschicht=fabs(this->depth[soildepth+1]);
					
					T->node[nodin].frac[soildepth]=0;
					if(intersects(obereBodenschicht, untereBodenschicht, nodin,T))//wenn Bodenschicht zwischen node und arch.parent[node] liegt
					{
						T->node[nodin].frac[soildepth]=(min(untereBodenschicht,T->node[nodin].unteresWurzelelement )
							-max(obereBodenschicht,T->node[nodin].oberesWurzelelement ))/-nodedZ;
						
					}
				}
			}
		}
	
}

