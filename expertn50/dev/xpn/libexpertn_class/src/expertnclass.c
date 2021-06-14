//      expertnclass.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include "expertnclass.h"

#include <xtime_reg.h>
#include <xclimate_reg.h>
#include <xheat_reg.h>
#include <xsoil_reg.h>
#include <xwater_reg.h>
#include <xchemist_reg.h>
#include <xplant_reg.h>




//define PRINT_ERROR_ID(self,Message) fprintf(stderr,"%s\t%d\t%s\n",__FILE__,__LINE__,Message);
//define PRINT_MESSAGE(self,DEBUG_LEVEL_,Message) if (DEBUG_LEVEL_ <= self->DEBUG_LEVEL) fprintf(stderr,"%s\t%d\t%s\n",__FILE__,__LINE__,Message);



static void expertnclass_finalize           (GObject *object);

G_DEFINE_TYPE(expertnclass, expertnclass, G_TYPE_OBJECT);


static void expertnclass_class_init(expertnclassClass *klass)
{
    GObjectClass *g_object_class;

    g_object_class = G_OBJECT_CLASS(klass);

    g_object_class->finalize = expertnclass_finalize;	

}


static void expertnclass_finalize(GObject *object)
{
    expertnclass *self;
    int i3;

    self = EXPERTNCLASS(object);

    PRINT_MESSAGE(self,2,"Finalize!"); 


    expertnclass_FinalizeModuls(self);

    // Module entladen:

    for (i3=0; i3<self->XPN_Moduls_len; i3++)
        {
            if (self->XPN_Moduls[i3]->className!=NULL)
                {
                    if (!g_module_close (self->XPN_Moduls[i3]->ModulLib))
                        PRINT_ERROR_ID(self,g_module_error ());
                }
        }

    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            free(self->XPN_Moduls[i3]);
        }

    free(self->XPN_Moduls);

    g_object_unref(self->var_list);

    // pXSys
    free(self->pXSys);

    // pLo init:
    free(self->pLo);

    // pTi init:
    free(self->pTi);

    // pCl init:
    free(self->pCl);

    // pCh init:
    free(self->pCh);

    // pSo init:
    free(self->pSo);

    // pHe init:
    free(self->pHe);

    // pPl init:
    free(self->pPl);

    // PWa init:
    free(self->pWa);

    // pMa
    free(self->pMa);

    g_return_if_fail(object != NULL);
    g_return_if_fail(IS_EXPERTNCLASS(object));



    G_OBJECT_CLASS(expertnclass_parent_class)->finalize(object);
}


static void expertnclass_init(expertnclass *self)  {}

static char* create_string_from_source(char* src)
{
    char *S;
    if (src==NULL) return NULL;
    S = (char*) malloc(strlen(src)+1);
    strcpy(S,src);
    return S;

}


expertnclass *expertnclass_new(int DEBUG_LEVEL, struct_register_modul_list *modul_list, char* ini_file_name,const char*base_path, char* project_name,char* project_path ,char* reg_str, int id,int id_N,int config_values_len,STRUCT_Config_Value* config_values)
{
    expertnclass *self;
    expertn_modul_base *base;
//  char *write_timestep;
//  char *S;
    int i;
    self = g_object_new(EXPERTNCLASS_TYPE, NULL);
    self->DEBUG_LEVEL = DEBUG_LEVEL;
    self->modul_list = modul_list;
    
    self->timesave=0.0;
    

    //S =get_fullpath_from_relative(base_path,modul_path);

    // Registry wird angelegen
    /*S = get_fullpath_from_relative(base_path,output_txtfile);
    self->var_list = xpn_register_var_new (var_list_provider,output_var_list_string,S);
    if (S!=NULL) free(S);*/
    
    self->var_list = xpn_register_var_new (NULL,NULL,NULL);

    // Auswahlfilter fürs "Mittschneiden" wird übergeben (dh. die Datei liste.var wird geladen)
    /*S = get_fullpath_from_relative(base_path,var_list_file);
    if (S!=NULL) xpn_register_var_add_filter_list_file(self->var_list,S);
    if (S!=NULL) free(S);*/



    // pXSys anlegen und Registry, id, ... den verschiedenen Modulen zur Verfügen stellen:
    self->pXSys=(PXSYSTEM)malloc(sizeof(XSYSTEM));
    memset (self->pXSys,0,sizeof(XSYSTEM));
    self->pXSys->var_list = self->var_list;
    self->pXSys->id=id;
    self->pXSys->id_N = id_N;
    self->pXSys->MPI_rank=0;
    self->pXSys->MPI_size=1;
    self->pXSys->OMP_rank=0;
    self->pXSys->OMP_size=1;
    if (reg_str==NULL)
        {
            self->pXSys->reg_str=NULL;
        }
    else
        {
            self->pXSys->reg_str=(char*)malloc(strlen(reg_str)+1);
            strcpy(self->pXSys->reg_str,reg_str);
        }


    if (base_path==NULL)
        {
            self->pXSys->base_path=NULL;
        }
    else
        {
            self->pXSys->base_path=(char*)malloc(strlen(base_path)+1);
            strcpy(self->pXSys->base_path,base_path);
        }
        
    if (project_name==NULL)
        {
            self->pXSys->project_name = NULL;
        }
    else
        {           
            self->pXSys->project_name = (char*)malloc(strlen(project_name)+1);
            strcpy(self->pXSys->project_name,project_name);
        }
        
    if (project_path==NULL)
        {
            self->pXSys->project_path = NULL;           
        }
    else
        {
            self->pXSys->project_path = (char*)malloc(strlen(project_path)+1);
            strcpy(self->pXSys->project_path,project_path);
        }

    // pLo init:
    self->pLo = (PLOCATION)malloc(sizeof(LOCATION));
    memset (self->pLo,0,sizeof(LOCATION));

    // pTi init:

    self->pTi = (PTIME)malloc(sizeof(TIME));
    memset (self->pTi,0,sizeof(TIME));


    // pCl init:
    self->pCl = (PCLIMATE) malloc(sizeof(CLIMATE));
    memset (self->pCl,0,sizeof(CLIMATE));


    // pCh init:
    self->pCh = (PCHEMISTRY) malloc (sizeof(CHEMISTRY));
    memset (self->pCh ,0,sizeof(CHEMISTRY));

    // pSo init:
    self->pSo = (PSPROFILE) malloc(sizeof(SPROFILE));
    memset (self->pSo ,0,sizeof(SPROFILE));

    // pHe init:
    self->pHe = (PHEAT) malloc(sizeof(HEAT));
    memset (self->pHe ,0,sizeof(HEAT));

    // pPl init:
    self->pPl = (PPLANT) malloc(sizeof(STPLANT));
    memset (self->pPl ,0,sizeof(STPLANT));

    // PWa init:
    self->pWa = (PWATER) malloc(sizeof(WATER));
    memset (self->pWa ,0,sizeof(WATER));

    // pMa
    self->pMa=(PMANAGEMENT)malloc(sizeof(STMANAGEMENT));
    memset (self->pMa ,0,sizeof(STMANAGEMENT));

    self->config_values_len=0;
    self->config_values=NULL;
    
    self->config_values_len = config_values_len;
    self->config_values = g_malloc0(sizeof(STRUCT_Config_Value)*self->config_values_len);
    
    for (i=0;i<config_values_len;i++)
        {
            self->config_values[i].key = g_strdup_printf("%s",config_values[i].key);
            self->config_values[i].value =  g_strdup_printf("%s",config_values[i].value);
            xpn_register_var_add_none(self->var_list,self->config_values[i].key,self->config_values[i].value,-1,TRUE);
        }


    // Interne Struktur self->XPN_Moduls mit Werten belegen (Modellenamen/Klassennamen usw. werden einetragen):
    
    base = expertn_modul_base_new();
    base->pXSys = self->pXSys;
    
    //expertn_modul_base_replace_std_templates(
    
    self->ini_file_name = expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(base,ini_file_name);  
    g_object_unref(base);
	
	PRINT_MESSAGE(self,2,"Init!");
	
    expertnclass_load_INI(self);
    
    expertnclass_loadModuls(self);
    
	return self;
}

void expertnclass_initialize(expertnclass *self)
{
    expertnclass_InitModuls(self);
}

void expertnclass_done(expertnclass *self)
{
    int i;
    for (i=0;i<self->config_values_len;i++)
        {
            g_free(self->config_values[i].key);
            g_free(self->config_values[i].value);
        }
    g_free(self->config_values);
    self->config_values_len=0;
    self->config_values=NULL;
    g_free(self->ini_file_name);
    g_object_unref(self);
}

xpn_register_var *expertnclass_get_varlist(expertnclass *self)
{
    return self->var_list;
}



int  expertnclass_run(expertnclass *self)
{
    int ret;
    ret = expertnclass_RunModuls(self);
    return ret;
}

int  expertnclass_run_global(expertnclass *self)
{
    return expertnclass_RunGlobalModuls(self);
}

int  expertnclass_run_mosaic(expertnclass *self)
{
    return expertnclass_RunMosaicModuls(self);
}






void expertnclass_InitModuls(expertnclass *self)
{
    int i3;
// Expert N Module werden alle nach der Reihe aufgerufen:
    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            if (self->XPN_Moduls[i3]->load!=NULL)
                {                   
                    (*(self->XPN_Moduls[i3]->load))(self->XPN_Moduls[i3]->xpn);                 
                }
        }
}

int expertnclass_RunGlobalModuls(expertnclass *self)
{
    int i3;
    int ret;
// Expert N Module werden alle nach der Reihe aufgerufen:
    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            if (self->XPN_Moduls[i3]->global_run!=NULL)
                {                                       
                    ret = (*(self->XPN_Moduls[i3]->global_run))(self->XPN_Moduls[i3]->xpn);
                    if (ret!=RET_SUCCESS) return ret;
                }
        }
    return RET_SUCCESS;
}

int expertnclass_RunMosaicModuls(expertnclass *self)
{
    int i3;
    int ret;
// Expert N Module werden alle nach der Reihe aufgerufen:
    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            if (self->XPN_Moduls[i3]->mosaic_run!=NULL)
                {                   
                    ret = (*(self->XPN_Moduls[i3]->mosaic_run))(self->XPN_Moduls[i3]->xpn);
                    if (ret!=RET_SUCCESS) return ret;
                }
        }
    return RET_SUCCESS;
}

int expertnclass_RunModuls(expertnclass *self)
{
    int i3;
    int ret;
// Expert N Module werden alle nach der Reihe aufgerufen:
    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            if (self->XPN_Moduls[i3]->run!=NULL)
                {
                    ret = (*(self->XPN_Moduls[i3]->run))(self->XPN_Moduls[i3]->xpn);
                    if (ret!=RET_SUCCESS) return ret;
                }
        }
    return RET_SUCCESS;
}

void expertnclass_FinalizeModuls(expertnclass *self)
{
    int i3;
// Expert N Module werden alle in umgekehreter Reihe aufgerufen:
    for (i3 = self->XPN_Moduls_len-1; i3 > -1; i3--)
        {
            if (self->XPN_Moduls[i3]->done!=NULL)
                {
                    (*(self->XPN_Moduls[i3]->done))(self->XPN_Moduls[i3]->xpn);
                }
        }

}

void expertnclass_free_def_list(char *** deflist,int list_len)
{
    int i;
    char **entry;
    char *module,*submodule;
    if (deflist==NULL) return;
    for (i=0;i<list_len;i++)
        {
            entry = deflist[i];
            if (entry!=NULL)
            {
                module = entry[0];
                submodule = entry[1];
                if (module!=NULL)
                {
                    free(module);
                }
                if (submodule!=NULL)
                {
                    free(submodule);
                }
                free(entry);
            }
        }
    free(deflist);
}

char *** expertnclass_read_modules_def_file(int *list_len,char* filename)
{
    FILE *f;
    char *S,*S2,*S3;
    char ***def_list;
    char **entry;
    char *module,*submodule;
    def_list = NULL;
    *list_len = 0;
    if (filename ==NULL)  return NULL;
    f = fopen(filename,"r");
    if (f==NULL)
        {
            return NULL;
        }
     while (1)
      {
        S=fgets_mem(f);
          if ((feof(f) != 0) && (S==NULL))
            {
                break;
            }
        if (S!=NULL)
            {
                deleteSpaceBegEnd(S);
                if ((strlen(S) >= 3) && (S[0] != '#'))
                    {
                        S2 = (char*)malloc(strlen(S)+1);
                        S3 = (char*)malloc(strlen(S)+1);
                        
                        if (getStringColumn_get_Size(S,".")>=2)
                            {
                                getStringColumn(S2,S,".",0);
                                getStringColumn(S3,S,".",1);
                                module = (char*)malloc(strlen(S2)+1);
                                strcpy(module,S2);
                                submodule = (char*)malloc(strlen(S3)+1);
                                strcpy(submodule,S3);
                                entry = (char**)malloc(sizeof(char*)*2);
                                entry[0] = module;
                                entry[1] = submodule;
                                (*list_len)++;
                                def_list = realloc(def_list,sizeof(char**)* (*list_len));                               
                                def_list[(*list_len)-1] = entry;
                            }
                        
                        if (S2!=NULL) free(S2);
                        if (S3!=NULL) free(S3);
                    }
                free(S);
            }
                    
      }
      
    fclose(f);
    
    return def_list;
}


char*** expertnclass_load_possible_modules(expertnclass *self,int *list_len)
{   
    char *Def_File;
    char *S;
    int module_count;
    char ***module_list;
    //if (S!=NULL) self->varlist = get_fullpath_from_relative(self->parent.pXSys->base_path,S);
    S = (char*)xpn_register_var_get_pointer(self->pXSys->var_list,"Global_Config.system.module_def_file");
    if (S==NULL)
        {
            PRINT_ERROR_ID(self,"Missing Module Definition File 'system.module_def_file! If this happens Expert-N might not work!");
            return NULL;
        }
    Def_File = get_fullpath_from_relative(self->pXSys->base_path,S);
    
    module_count=0;
    module_list = expertnclass_read_modules_def_file(&module_count,Def_File);
    (*list_len) = module_count;
    free(Def_File); 
    if (module_list==NULL)
        {
            PRINT_ERROR_ID(self,"Loading Module Definition File 'system.module_def_file! If this happens Expert-N might not work!");
            return NULL;
        }
    return module_list;
}


void expertnclass_load_INI(expertnclass *self)
{
//define modul_count 48
    // Alle Module und Submodule werden hier definiert und in der gleichen Reihenfolge später aufgerufen
    // Falls es nötig sein sollte, weitere Module zu definieren, so können diese hier einfach ergänzt werden (modul_count muss dann auch geändert werden)
/*  const char moduls[modul_count][2][128] =
    {
        {"control","input"},
        {"control","database"},
        {"control","time step"},
        {"control","balance"},
        {"control","output"},
        {"control","initialisation"},
        {"control","pedotransfer"},
        {"water","hydraulic functions"},

        {"water","snow"},    // Schnee
        {"water","interception"},
        {"water","surface runoff"},  // Oberflächenabfluss
        {"water","potential evapotranspiration"}, // Potentielle Evaporation
        {"water","actual evaporation"}, // Aktuelle Evaporation
        {"plant","potential transpiration"}, // Potentielle Transpiration
        {"plant","actual transpiration"}, // aktuelle Transpiration / Wasseraufnahme
        {"plant","phenological development"}, // Phänologie
        {"plant","canopy gross photosynthesis"}, // Photosynthese
        {"plant","biomass growth"}, // Assimilatverteilung / Biomassewachstum
        {"plant","canopy formation"}, // Blattflächenentwicklung
        {"plant","root length growth"}, // Wurzelwachstum
        {"water","lower boundary condition"}, // Grundwasser
        {"water","flow module"}, // Wassertransport
        {"heat","heat transfer"}, // Bodentempertur
        {"heat","latent heat"}, // Bodentempertur
        {"heat","sensible heat"}, // Bodentempertur
        {"heat","albedo"}, // Bodentempertur
        {"nitrogen","deposition"}, // Deposition
        {"nitrogen","nitrogen transport"}, // N-Transport
        {"nitrogen","urea hydolysis"}, // Hydrolyse
        {"nitrogen","mineralisation"}, // Mineralisation / Immobilisation
        {"nitrogen","nitrification"}, // Nitrifikation
        {"plant","nitrogen demand"}, // Stickstoffbedarf
        {"plant","nitrogen uptake"}, // Stickstoffaufnahme
        {"plant","crop senescence"}, // Senescence
        {"nitrogen","denitrification"},  // Denitrifikation
        {"nitrogen","volatilization"}, // Volatilization
        {"nitrogen","ch4 imission emission"},   // CH4 - Emission/Imission


        {"water","potential evaporation"},
        {"water","pde solver"},

        {"heat","upper boundary"},
        {"heat","lower boundary"},
        {"nitrogen","pde Solver"},

        {"management","mixing incorporation"},
        {"management","soil loosing compaction"},
        {"management","application fertilizers"},
        {"management","loss volatilization"},
        {"management","application type"},
        {"management","loss efficiency"},
    };*/
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;

    int i,i2,i3;
    char *S,*S2;
    int modul_count;
    char *** moduls;
    gsize number_of_groups,number_of_keys;
    gchar ** groups, ** keys;
    
    modul_count=0;
    moduls=NULL;
    moduls = expertnclass_load_possible_modules(self,&modul_count);
    
    if (moduls==NULL)
        {
            return;
        }

    /* Create a new GKeyFile object and a bitwise list of flags. */
    keyfile = g_key_file_new ();

    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Load the GKeyFile from keyfile.conf or return. */
    if (!g_key_file_load_from_file (keyfile, self->ini_file_name, flags, &error))
        {
         //   PRINT_ERROR_ID(self,error->message);
	fprintf(stderr,"ERROR  %s\t%d\t%s\n",__FILE__,__LINE__,error->message);fflush(stderr);

            exit(1);
        }
        
    
    
    //cfg->output_var_list_provider = g_key_file_get_string  (keyfile, "system",  "output_var_list_provider", NULL);
    self->XPN_Moduls_len = modul_count;

    self->XPN_Moduls = (struct_module **)malloc(sizeof(struct_module *)*self->XPN_Moduls_len);


    PRINT_MESSAGE(self,4,"---------------- Configured Models: -------------------------\n");


    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {

            self->XPN_Moduls[i3] = (struct_module *)malloc(sizeof(struct_module));
            self->XPN_Moduls[i3]->get_type=NULL;
            self->XPN_Moduls[i3]->className=NULL;
            self->XPN_Moduls[i3]->libName=NULL;
            self->XPN_Moduls[i3]->load=NULL;
            self->XPN_Moduls[i3]->global_run=NULL;
            self->XPN_Moduls[i3]->mosaic_run=NULL;
            self->XPN_Moduls[i3]->run=NULL;
            self->XPN_Moduls[i3]->done=NULL;
            self->XPN_Moduls[i3]->xpn=NULL;
            self->XPN_Moduls[i3]->load_name=NULL;
            self->XPN_Moduls[i3]->global_run_name=NULL;
            self->XPN_Moduls[i3]->mosaic_run_name=NULL;
            self->XPN_Moduls[i3]->run_name=NULL;
            self->XPN_Moduls[i3]->done_name=NULL;
            self->XPN_Moduls[i3]->ModulName= g_key_file_get_string  (keyfile,moduls[i3][0],moduls[i3][1], NULL);
            self->XPN_Moduls[i3]->Modul=create_string_from_source((char*)moduls[i3][0]);
            self->XPN_Moduls[i3]->SubModul=create_string_from_source((char*)moduls[i3][1]);
            if  (self->XPN_Moduls[i3]->ModulName!=NULL)
                {
                    S = (char*)malloc(strlen(moduls[i3][0])+strlen(moduls[i3][1])+strlen(self->XPN_Moduls[i3]->ModulName)+128);
                    sprintf(S,"For model type (%s,%s): Model \"%s\" is used.",moduls[i3][0],moduls[i3][1], self->XPN_Moduls[i3]->ModulName);
                    PRINT_MESSAGE(self,4,S);
                    free(S);
                }
        }

// if (3 <= self->DEBUG_LEVEL) fprintf(stderr,"\n\n-----------------------------------------------------------------\n\n\n");

// Alle Gruppen und keys werden in die Registry geladen:
//number_of_groups,number_of_keys

    groups = g_key_file_get_groups (keyfile,&number_of_groups);
    for (i=0; i<(int)number_of_groups; i++)
        {
            keys = g_key_file_get_keys(keyfile, groups[i], &number_of_keys,NULL);
            if (keys!=NULL)
                {
                    for (i2=0; i2<(int)number_of_keys; i2++)
                        {

                            S = g_strdup_printf("Config.%s.%s",groups[i],keys[i2]);
                            S2 = g_key_file_get_value (keyfile,groups[i],keys[i2],NULL);
                            xpn_register_var_add_none(self->var_list,S,S2,-1,TRUE);
                            //Todo: string wird im moment nicht freigegeben (das ist ein Memory Leak)
                            g_free(S);
                        }
                    g_strfreev(keys);
                }

        }
        
    g_strfreev(groups);
    g_key_file_free(keyfile);
    
    expertnclass_free_def_list(moduls,modul_count);
}









void expertnclass_loadModuls(expertnclass *self)
{

//  const char *dlError;    /*  Pointer to error string */
    char *S;
    int i2,i3;
    GType modul_type;




    // Alle Module durchgehen
    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            // Schauen ob diese Module in irgendeiner LIB definiert sind und Klassen und Funktionsnamen in XPN_Moduls eintragen:
            if  (self->XPN_Moduls[i3]->ModulName!=NULL)
                {
                    for (i2=0; i2<self->modul_list->length; i2++)
                        {
                            //char S[128];
                            //sprintf(S,"%d %d \n",i2,i3);
                            //PRINT_ERROR_ID(self,S);

                            if (strcmp(self->XPN_Moduls[i3]->ModulName,self->modul_list->modul_list[i2]->Name)==0)
                                {
                                    //if ((self->XPN_Moduls[i3]->Modul!=NULL) && (self->modul_list->modul_list[i2]->Modul!=NULL) && (self->XPN_Moduls[i3]->SubModul!=NULL) && (self->modul_list->modul_list[i2]->Submodul!=NULL))
                                    /*printf("%d %d \n",i2,i3);
                                                           printf("%s\n",self->XPN_Moduls[i3]->ModulName);
                                                           printf("%s\n",self->XPN_Moduls[i3]->Modul);
                                                           printf("%s\n",self->modul_list->modul_list[i2]->Modul);
                                                           printf("%s\n",self->XPN_Moduls[i3]->SubModul);
                                                           printf("%s\n",self->modul_list->modul_list[i2]->Submodul);*/
                                    if ((strcmp(self->XPN_Moduls[i3]->Modul,self->modul_list->modul_list[i2]->Modul)==0) && (strcmp(self->XPN_Moduls[i3]->SubModul,self->modul_list->modul_list[i2]->Submodul)==0))
                                        {
                                            self->XPN_Moduls[i3]->libName=(char*)self->modul_list->modul_list[i2]->libname;
                                            self->XPN_Moduls[i3]->className=(char*)self->modul_list->modul_list[i2]->expertn_modul_name;
                                            self->XPN_Moduls[i3]->load_name=(char*)self->modul_list->modul_list[i2]->expertn_modul_load;
                                            self->XPN_Moduls[i3]->global_run_name=(char*)self->modul_list->modul_list[i2]->expertn_modul_global_run;
                                            self->XPN_Moduls[i3]->mosaic_run_name=(char*)self->modul_list->modul_list[i2]->expertn_modul_mosaic_run;
                                            self->XPN_Moduls[i3]->run_name=(char*)self->modul_list->modul_list[i2]->expertn_modul_run;
                                            self->XPN_Moduls[i3]->done_name=(char*)self->modul_list->modul_list[i2]->expertn_modul_done;
                                            break;
                                        }
                                }
                        }

                }
        }

    // Checken, ob auch die konfigurierten Module auch wirklich in den LIBS waren, ansonsten Fehler
    for  (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            if  ((self->XPN_Moduls[i3]->ModulName!=NULL) && (self->XPN_Moduls[i3]->className==NULL))
                {
                    char *S;

                    PRINT_MESSAGE(self,3,"\n--------------------------------------------\nFollowing Moduls exists:\n\n");
                    for (i2=0; i2<self->modul_list->length; i2++)
                        {
                            S = (char*)malloc(strlen(self->modul_list->modul_list[i2]->Modul)+strlen(self->modul_list->modul_list[i2]->Submodul)+strlen(self->modul_list->modul_list[i2]->Name)+200);
                            sprintf(S,"Modul: \"%s\"   Submodul: \"%s\"   Name: \"%s\"",self->modul_list->modul_list[i2]->Modul,self->modul_list->modul_list[i2]->Submodul,self->modul_list->modul_list[i2]->Name);
                            PRINT_MESSAGE(self,3,S);
                            free(S);
                        }

                    printf("\n\n");
                    S = (char*) malloc(strlen(self->XPN_Moduls[i3]->ModulName)+200);
                    sprintf(S,"ERROR: Configured Modul \"%s\" could not be found in any LIB!",self->XPN_Moduls[i3]->ModulName);
                    PRINT_ERROR_ID(self,S);
                    free(S);
                    exit(1);
                }
        }


    // Jetzt wo hoffentlich alle Module erkannt worden sind, werden die Funktionen registriert und die Klassen Initialisiert:
    for (i3 = 0; i3 < self->XPN_Moduls_len; i3++)
        {
            // Falls es für das Modul einen Funktionsnamen gibt, existiert auch eine Klasse
            if (self->XPN_Moduls[i3]->className!=NULL)
                {
                    int bool_init;
                    bool_init = 0;
                    //self->XPN_Moduls[i3]->ModulLib = dlopen(self->XPN_Moduls[i3]->libName,RTLD_NOW);

                    /* dlError = dlerror();
                     if( dlError )
                                            {
                          PRINT_ERROR_ID(self,dlError);
                          exit(1);
                      }*/

                    self->XPN_Moduls[i3]->ModulLib = g_module_open(self->XPN_Moduls[i3]->libName,G_MODULE_BIND_LOCAL);

                    if (!self->XPN_Moduls[i3]->ModulLib)
                        {
                            PRINT_ERROR_ID(self,g_module_error ());
                            exit(1);
                        }

                    // Jetzt muss noch überprüft werden, ob die Klasse schon vorher initialisiert wurde, da es mögliche
                    // ist, dass mehrere Funktionen in ein und der selben Klasse sind
                    for (i2 = 0; i2 < i3; i2 ++)
                        {
                            if (self->XPN_Moduls[i2]->className!=NULL)
                                {
                                    if (strcmp(self->XPN_Moduls[i2]->className,self->XPN_Moduls[i3]->className)==0)
                                        {
                                            // OK Klasse existiert
                                            bool_init=1;
                                            self->XPN_Moduls[i3]->xpn = self->XPN_Moduls[i2]->xpn;
                                            self->XPN_Moduls[i3]->get_type=self->XPN_Moduls[i2]->get_type;
                                            break;
                                        }
                                }
                        }
                    // falls Klasse noch nicht initialisiert, dann muss das jetzt gemacht werden
                    if (bool_init==0)
                        {
                            S = (char*)malloc(strlen(self->XPN_Moduls[i3]->className)+100);
                            sprintf(S,"%s_get_type",self->XPN_Moduls[i3]->className);
                            //self->XPN_Moduls[i3]->get_type = dlsym(self->XPN_Moduls[i3]->ModulLib,S);
                            if (!g_module_symbol ( self->XPN_Moduls[i3]->ModulLib, S,(gpointer *)&(self->XPN_Moduls[i3]->get_type)))
                                {
                                    free(S);
                                    PRINT_ERROR_ID(self,g_module_error ());
                                    exit(1);
                                }
                            free(S);
                            /*dlError = dlerror();
                            if( dlError )
                                                             {
                              PRINT_ERROR_ID(self,dlError);
                              exit(1);
                             }*/




                            modul_type= (*(self->XPN_Moduls[i3]->get_type)) ();
                            self->XPN_Moduls[i3]->xpn = g_object_new(modul_type,NULL);
                            // Jetzt noch alle Variablen in der ExpertNBase Class mit Werten füllen:
                            self->XPN_Moduls[i3]->xpn->pLo = self->pLo;
                            self->XPN_Moduls[i3]->xpn->pTi = self->pTi;
                            self->XPN_Moduls[i3]->xpn->pCl = self->pCl;
                            self->XPN_Moduls[i3]->xpn->pCh = self->pCh;
                            self->XPN_Moduls[i3]->xpn->pHe = self->pHe;
                            self->XPN_Moduls[i3]->xpn->pMa = self->pMa;
                            self->XPN_Moduls[i3]->xpn->pPl = self->pPl;
                            self->XPN_Moduls[i3]->xpn->pSo = self->pSo;
                            self->XPN_Moduls[i3]->xpn->pWa = self->pWa;
                            self->XPN_Moduls[i3]->xpn->DEBUG_LEVEL = self->DEBUG_LEVEL;
                            self->XPN_Moduls[i3]->xpn->pXSys = self->pXSys;
                        }

                    // Die Pointer der Klasse wird jetzt noch zur Registry hinzugefügt:
                    S = g_strdup_printf("Class.%s.%s",self->XPN_Moduls[i3]->Modul,self->XPN_Moduls[i3]->SubModul);
                    xpn_register_var_add_none(self->var_list,S,self->XPN_Moduls[i3]->xpn,-1,TRUE);
                    g_free(S);

                }
            // jetzt müssen noch die Funktionspointer aus der LIB geladen werden
            if  (self->XPN_Moduls[i3]->load_name!=NULL)
                {
                    if (!g_module_symbol ( self->XPN_Moduls[i3]->ModulLib, self->XPN_Moduls[i3]->load_name,(gpointer *)&(self->XPN_Moduls[i3]->load)))
                        {
                            PRINT_ERROR_ID(self,g_module_error ());
                            exit(1);
                        }

                    /*self->XPN_Moduls[i3]->load = dlsym(self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->load_name);
                    dlError = dlerror();
                    if( dlError )
                                                       {
                     PRINT_ERROR_ID(self,dlError);
                     exit(1);
                     }*/
                }
            if  (self->XPN_Moduls[i3]->global_run_name!=NULL)
                {
                    if (!g_module_symbol ( self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->global_run_name,(gpointer *)&(self->XPN_Moduls[i3]->global_run)))
                        {
                            PRINT_ERROR_ID(self,g_module_error ());
                            exit(1);
                        }
                    /*self->XPN_Moduls[i3]->global_run = dlsym(self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->global_run_name);
                    dlError = dlerror();
                    if( dlError )
                                                       {
                     PRINT_ERROR_ID(self,dlError);
                     exit(1);
                     }*/
                }
            if  (self->XPN_Moduls[i3]->mosaic_run_name!=NULL)
                {
                    if (!g_module_symbol ( self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->mosaic_run_name,(gpointer *)&(self->XPN_Moduls[i3]->mosaic_run)))
                        {
                            PRINT_ERROR_ID(self,g_module_error ());
                            exit(1);
                        }
                    /*self->XPN_Moduls[i3]->global_run = dlsym(self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->global_run_name);
                    dlError = dlerror();
                    if( dlError )
                                                       {
                     PRINT_ERROR_ID(self,dlError);
                     exit(1);
                     }*/
                }
            if  (self->XPN_Moduls[i3]->run_name!=NULL)
                {
                    if (!g_module_symbol ( self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->run_name,(gpointer *)&(self->XPN_Moduls[i3]->run)))
                        {
                            PRINT_ERROR(g_module_error ());
                            //PRINT_ERROR_ID(self,g_module_error ());
                            exit(1);
                        }

                    /* self->XPN_Moduls[i3]->run = dlsym(self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->run_name);

                     dlError = dlerror();
                     if( dlError )
                                                       {
                      PRINT_ERROR_ID(self,dlError);
                      exit(1);
                      }*/
                }
            if  (self->XPN_Moduls[i3]->done_name!=NULL)
                {
                    if (!g_module_symbol ( self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->done_name,(gpointer *)&(self->XPN_Moduls[i3]->done)))
                        {
                            PRINT_ERROR_ID(self,g_module_error ());
                            exit(1);
                        }
                    /*self->XPN_Moduls[i3]->done = dlsym(self->XPN_Moduls[i3]->ModulLib,self->XPN_Moduls[i3]->done_name);
                    dlError = dlerror();
                    if( dlError )
                                                       {
                     PRINT_ERROR_ID(self,dlError);
                     exit(1);
                     }*/
                }
        }
}
