#ifndef ___XPN_REGISTER_MODUL_H___
#define ___XPN_REGISTER_MODUL_H___

#include <gmodule.h>



typedef struct 
        {
		  char *Name;  // Name des Modells bzw. Submodells
		  char *Modul; // Modulbezeichnung: water,heat,nitrogen,plant, management, ..
		  char *Submodul; // Submodulbezeichnung: Biomass Growth
		  
		  char*  expertn_modul_name; // Name der Klasse
		  char*  expertn_modul_load;   	
		  char*  expertn_modul_global_run;
		  char*  expertn_modul_mosaic_run;
		  char*  expertn_modul_run;
		  char*  expertn_modul_done;
		  
		  char *libname; // filename of the lib
		} struct_register_modul;

typedef struct 
        {
		   int 						length;
		   struct_register_modul   **modul_list;
		  
		} struct_register_modul_list;
		
		
G_MODULE_EXPORT struct_register_modul_list * register_modul_list_new(int length, ...);
G_MODULE_EXPORT struct_register_modul_list * register_modul_list_add(struct_register_modul_list *modul_list, struct_register_modul* modul);
G_MODULE_EXPORT void						 register_modul_list_delete(struct_register_modul_list *modul_list);


G_MODULE_EXPORT struct_register_modul * register_modul_run_new(char *Name, char *Modul, char *Submodul,
                                                                                   char *expertn_modul_name,
										   char *expertn_modul_run);

										   
G_MODULE_EXPORT struct_register_modul * register_modul_new(char *Name, char *Modul, char *Submodul,
										   char *expertn_modul_name,
										   char *expertn_modul_load,
										   char *expertn_modul_global_run,
										   char *expertn_modul_mosaic_run,
										   char *expertn_modul_run,
										   char *expertn_modul_done);
										   
G_MODULE_EXPORT struct_register_modul * register_modul_new_with_libname(char *Name, char *Modul, char *Submodul, 
                                                                                   char *expertn_modul_name,
										   char *expertn_modul_load,
										   char *expertn_modul_global_run,
										   char *expertn_modul_mosaic_run,
										   char *expertn_modul_run,
										   char *expertn_modul_done,
										   char *libname);
G_MODULE_EXPORT void						 register_modul_delete(struct_register_modul *modul);

// other functions:

// lädt aus einer Liste lib_liste(dll Liste) alle Module/Classen aus diesen
// return modul_list --> muss mit register_modul_list_delete(modul_list) deallociert werden
G_MODULE_EXPORT struct_register_modul_list *register_modul_load_modul_list(char **expertn_lib_list, int expertn_lib_list_len,const char* base_path);

#endif

