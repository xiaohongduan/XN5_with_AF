//      expertnclass.h
//
//      Copyright 2010 Christian Klein <chrikle@chrikle-Helmholtz>
//



#ifndef __EXPERTNCLASS_H__
#define __EXPERTNCLASS_H__

#include <glib-object.h>
#include <gmodule.h>
#include "xinclexp.h"
#include "xpn_register_var.h"
#include "xpn_register_modul.h"
#include "expertn_modul_base.h"
#include "general_config.h"



G_BEGIN_DECLS

#define EXPERTNCLASS_TYPE				(expertnclass_get_type())
#define EXPERTNCLASS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
        EXPERTNCLASS_TYPE, expertnclass))
#define EXPERTNCLASS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
        EXPERTNCLASS_TYPE, expertnclassClass))
#define IS_EXPERTNCLASS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
        EXPERTNCLASS_TYPE))
#define IS_EXPERTNCLASS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
        EXPERTNCLASS_TYPE))

typedef struct _expertnclass			expertnclass;
typedef struct _expertnclassClass		expertnclassClass;

typedef struct
{
	GType (*get_type)();
	char *libName;
	char *ModulName; // eindeutiger Name des Moduls, z.B. Hurley Pasture Plant Growth
	char *className; // Name der Klasse in der das Modul befindet (abgeleitet von ExpertNBaseClase
	char *Modul; // Modulname z.B. Plant
	char *SubModul; // SubModulname z.B. Biomass_Growth
	char *load_name;
	char *global_run_name;
	char *mosaic_run_name;
	char *run_name;
	char *done_name;
	int (*load) (expertn_modul_base *self); // load Function
	int (*global_run) (expertn_modul_base *self); // global run Function
	int (*mosaic_run) (expertn_modul_base *self); // mosaic run Function
	int (*run) (expertn_modul_base *self); // run Funktion
	int (*done) (expertn_modul_base *self); // done Function
	GModule *ModulLib;		/*  Handle to shared lib file	*/
	expertn_modul_base *xpn; // struct von der Basic Clas (entspricht self)
} struct_module;



struct _expertnclass
{
	GObject parent;

	xpn_register_var *var_list;
	struct_register_modul_list *modul_list;



	PXSYSTEM pXSys;

	PTIME pTi;
	PCLIMATE pCl;

	PCHEMISTRY pCh;

	//PGRAPHIC pGr;
	PHEAT pHe;
	PLOCATION pLo;
	PMANAGEMENT pMa;
	PPLANT pPl;
	PSPROFILE pSo;

	PWATER pWa;

	int config_values_len;
	STRUCT_Config_Value* config_values;

	int DEBUG_LEVEL;

	char *ini_file_name;
	double timesave;

	// Module:

	struct_module **XPN_Moduls;
	int            XPN_Moduls_len;
	double output_time_step;
};

struct _expertnclassClass
{
	GObjectClass parent_class;
};


G_MODULE_EXPORT GType		  expertnclass_get_type		(void);


G_MODULE_EXPORT expertnclass *expertnclass_new(int DEBUG_LEVEL, struct_register_modul_list *modul_list, char* ini_file_name,const char*base_path, char* project_name, char* project_path,char* reg_str, int id,int id_N,int config_values_len,STRUCT_Config_Value* config_values);
G_MODULE_EXPORT void 	     expertnclass_initialize(expertnclass *self);
G_MODULE_EXPORT void expertnclass_done(expertnclass *self);
G_MODULE_EXPORT xpn_register_var *expertnclass_get_varlist(expertnclass *self);
G_MODULE_EXPORT int  expertnclass_run(expertnclass *self);
G_MODULE_EXPORT int  expertnclass_run_global(expertnclass *self);
G_MODULE_EXPORT int  expertnclass_run_mosaic(expertnclass *self);

G_MODULE_EXPORT void expertnclass_load_INI(expertnclass *self);   // INI Datei wird geladen, in der die Modulauswahl gemacht wird
G_MODULE_EXPORT void expertnclass_loadModuls(expertnclass *self); // Module werden geladen und und Klassen initialisiert


G_MODULE_EXPORT void expertnclass_InitModuls(expertnclass *self);     // Load Funktion der Module wird aufgerufen, falls vorhanden
G_MODULE_EXPORT int expertnclass_RunGlobalModuls(expertnclass *self);// Run Gobal Funktion der Module wird aufgerufen, falls vorhanden 
G_MODULE_EXPORT int expertnclass_RunMosaicModuls(expertnclass *self);// Run Mosaic Funktion der Module wird aufgerufen, falls vorhanden
G_MODULE_EXPORT int expertnclass_RunModuls(expertnclass *self);      // Run Funktion der Module wird aufgerufen, falls vorhanden
G_MODULE_EXPORT void expertnclass_FinalizeModuls(expertnclass *self); // Done Funktion der Module wird aufgerufen, falls vorhanden



G_END_DECLS

#endif /* __EXPERTNCLASS_H__ */
