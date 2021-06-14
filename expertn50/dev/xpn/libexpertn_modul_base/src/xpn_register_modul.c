#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <gmodule.h>
#include <glib.h>
#include "xpn_register_var.h"

#define PRINT_ERROR(Message) fprintf(stderr,"ERROR: %s\t%d\t%s\n",__FILE__,__LINE__,Message);fflush(stderr);

#define varfree(var) \
        if (var!=NULL) \
            {\
              free(var);\
            }

static char* create_string_from_source(char* src)
{
  char *S;
  if (src==NULL) return NULL;
  S = (char*) malloc(strlen(src)+1);
  strcpy(S,src);
  return S;

}



struct_register_modul_list * register_modul_list_new(int length, ...)
{
	va_list vl;
	struct_register_modul_list * list;
	struct_register_modul *modul;
	int i;
	
    
	if (length<1) return NULL;
	va_start(vl,length);
	
	list = (struct_register_modul_list*)malloc(sizeof(struct_register_modul_list));
	list->length=length;
	list->modul_list=(struct_register_modul**)malloc(sizeof(struct_register_modul*)*length);
	
	
	
	for (i=0;i<length;i++)
                {
	               modul=va_arg(vl,struct_register_modul *);
	               list->modul_list[i] = modul;
                }
   return list;

}

struct_register_modul_list * register_modul_list_add(struct_register_modul_list *modul_list, struct_register_modul* modul)
{
	if (modul_list==NULL)
		{
		  modul_list = (struct_register_modul_list*)malloc(sizeof(struct_register_modul_list));
		  modul_list->length=1;
		  modul_list->modul_list=(struct_register_modul**)malloc(sizeof(struct_register_modul*)*modul_list->length);
		  modul_list->modul_list[0] = modul;
		} else
		{
		  modul_list->length++;
		  modul_list->modul_list = (struct_register_modul**)realloc ( modul_list->modul_list, sizeof(struct_register_modul*)*modul_list->length );
		  modul_list->modul_list[modul_list->length-1] = modul;
		}
	return modul_list;
}

void register_modul_list_delete(struct_register_modul_list *modul_list)
{
   int i;
   for (i=0;i!=modul_list->length;i++)
	{
		register_modul_delete(modul_list->modul_list[i]);
	}
	free(modul_list->modul_list);
	free(modul_list);
}


struct_register_modul * register_modul_run_new(char *Name, char *Modul, char *Submodul,
                                                                                   char *expertn_modul_name,
										   char *expertn_modul_run)
{
   return  register_modul_new(Name,Modul,Submodul,expertn_modul_name,NULL,NULL,NULL,expertn_modul_run,NULL);
}										  
									

struct_register_modul * register_modul_new(char *Name, char *Modul, char *Submodul,
										   char *expertn_modul_name,
										   char *expertn_modul_load,
										   char *expertn_modul_global_run,
										   char *expertn_modul_mosaic_run,
										   char *expertn_modul_run,
										   char *expertn_modul_done)
{
	struct_register_modul *modul;
	
	// Schauen, ob alles richtig angegeben ist:
	if ((Name==NULL) || (Modul==NULL) || (Submodul==NULL) || (expertn_modul_name==NULL))
	   {
	
             PRINT_ERROR("ERROR in register_modul_new:  Name, Modul, Submodul or expertn_modul_name is not defined!\n");
             exit(1);             
            }

	
	modul = (struct_register_modul*)malloc(sizeof(struct_register_modul));

        // Strings müssen in neu allkokierten Speicher übergeben werden, da die Strings sonst nach entladen der LIBS nicht mehr vorhanden sind
	modul->Name =  create_string_from_source(Name);
	modul->Modul = create_string_from_source(Modul);
	modul->Submodul = create_string_from_source(Submodul);
	modul->expertn_modul_name = create_string_from_source(expertn_modul_name);
	modul->expertn_modul_load = create_string_from_source(expertn_modul_load);
	modul->expertn_modul_global_run = create_string_from_source(expertn_modul_global_run);
	modul->expertn_modul_mosaic_run = create_string_from_source(expertn_modul_mosaic_run);
	modul->expertn_modul_run = create_string_from_source(expertn_modul_run);
	modul->expertn_modul_done = create_string_from_source(expertn_modul_done);
	modul->libname = NULL;
	return modul;
}

struct_register_modul * register_modul_new_with_libname(char *Name, char *Modul, char *Submodul, 
                                                                                   char *expertn_modul_name,
										   char *expertn_modul_load,
										   char *expertn_modul_global_run,
										   char *expertn_modul_mosaic_run,
										   char *expertn_modul_run,
										   char *expertn_modul_done,
										   char *libname)
{
	struct_register_modul *modul;
	modul = register_modul_new(Name,Modul,Submodul,expertn_modul_name,expertn_modul_load,expertn_modul_global_run,expertn_modul_mosaic_run,expertn_modul_run,expertn_modul_done);	
	modul->libname = create_string_from_source(libname);
	return modul;
}

void register_modul_delete(struct_register_modul *modul)
{
        varfree(modul->Name);
        varfree(modul->Modul);
        varfree(modul->Submodul);
        varfree(modul->expertn_modul_name);
        varfree(modul->expertn_modul_load);
        varfree(modul->expertn_modul_global_run);
        varfree(modul->expertn_modul_run);
        varfree(modul->expertn_modul_done);
        varfree(modul->libname);
	free(modul);
}

struct_register_modul_list *register_modul_load_modul_list(char **expertn_lib_list, int expertn_lib_list_len,const char *base_path)
{
  int count_libs,i;
 GModule  *ModulLib;		/*  Handle to shared lib file	*/ 
  // const char *dlError;		/*  Pointer to error string		*/
  struct_register_modul_list* (*ExpertN_Lib_Register)(); 
  struct_register_modul_list *list;
 
  struct_register_modul_list *modul_list;
  char *act_module_list_entry;
  

  
  modul_list = NULL;
  
  if (expertn_lib_list_len > 0)
  {
     for (count_libs=0;count_libs!=expertn_lib_list_len;count_libs++)
     	{
     	   /*ModulLib = dlopen(expertn_lib_list[count_libs],RTLD_NOW);
    	   dlError = dlerror();
		        if( dlError ) 
        		    {
        		        PRINT_ERROR(dlError);
		                exit(1);
             		}*/
            act_module_list_entry = get_fullpath_from_relative(base_path,expertn_lib_list[count_libs]);
			ModulLib = g_module_open(act_module_list_entry,G_MODULE_BIND_LOCAL);

	if (!ModulLib)
		{
			PRINT_ERROR(g_module_error ());
			exit(1);
		}
		
          /*ExpertN_Lib_Register = dlsym(ModulLib, "ExpertN_Lib_Register");
           dlError = dlerror();
		        if( dlError ) 
        		    {
        		        PRINT_ERROR(dlError);
		                exit(1);
             		}*/
         if (!g_module_symbol ( ModulLib, "ExpertN_Lib_Register",(gpointer *)&(ExpertN_Lib_Register)))
	        {
								
			PRINT_ERROR(g_module_error ());
			exit(1);
		}
             		
         list = (*ExpertN_Lib_Register)();
         
         for (i=0;i!=list->length;i++)
         	{
         	   modul_list = register_modul_list_add(modul_list, 
         	   										register_modul_new_with_libname(list->modul_list[i]->Name,list->modul_list[i]->Modul,list->modul_list[i]->Submodul,
         	   																		list->modul_list[i]->expertn_modul_name,list->modul_list[i]->expertn_modul_load,list->modul_list[i]->expertn_modul_global_run,
         	   																		list->modul_list[i]->expertn_modul_mosaic_run,list->modul_list[i]->expertn_modul_run,list->modul_list[i]->expertn_modul_done,
         	   																		act_module_list_entry));
         	}
         register_modul_list_delete(list);
         
         
                   
         
         list = NULL;
		 
		 free(act_module_list_entry);
        
         /*dlclose(ModulLib);
		 dlError = dlerror();
         if( dlError ) 
            {
                PRINT_ERROR(dlError);
                exit(1);
             }*/
           if (!g_module_close (ModulLib))
                        {
        			PRINT_ERROR(g_module_error ());
				exit(1);
			}
     	}
  }
  
return modul_list;   
 
}

