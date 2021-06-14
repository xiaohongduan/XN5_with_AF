//      general_config.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>


#ifndef __GENERAL_CONFIG_H__
#define __GENERAL_CONFIG_H__

#include <glib-object.h>
#include <gmodule.h>

G_BEGIN_DECLS

#define GENERAL_CONFIG_TYPE				(general_config_get_type())
#define GENERAL_CONFIG(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			GENERAL_CONFIG_TYPE, general_config))
#define GENERAL_CONFIG_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			GENERAL_CONFIG_TYPE, general_configClass))
#define IS_GENERAL_CONFIG(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			GENERAL_CONFIG_TYPE))
#define IS_GENERAL_CONFIG_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			GENERAL_CONFIG_TYPE))

typedef struct _general_config			general_config;
typedef struct _general_configClass		general_configClass;


typedef struct
{
	char *config_file;
	double cover;
    int xpn_id_number;
} STRUCT_Instance;

typedef struct
{
	//int act; nicht nötig, da deaktivierte instancen erst gar nicht geladen werden
	int id;
	int i;
	int j;
	int instance_len;
	STRUCT_Instance *instance;
	
} STRUCT_Grid;


typedef struct
{
  gchar* key;
  gchar* value;
	
} STRUCT_Config_Value;


struct _general_config
{
	GObject parent;
	gint      DEBUG_LEVEL;   // 0: nur Ausgaben, die immer Ausgegeben werden, werden ausgegeben (z.B. Modell wurde erfolgreich initiiert)
					     // 1: Warnungen ausgeben (könnten Fehler im Programm sein), Beispiel 1/0 oder sqrt(-1) ...
					     // 2: 
					     // 3: 
					     // 4: Wichtige if oder Case Verzweigungnen werden ausgegen
			       		 // 5: Jede Funktion gibt aus, dass sie aufgerufen wurde

    	
	gint	   num_proc;	
	
	gchar      **expertn_lib_list;
	gsize		 expertn_lib_list_len;
	
	int 		 number_of_xpn;
	
	
	int			 grid_len;
	STRUCT_Grid* grid;
	
	int			config_values_len;
	STRUCT_Config_Value *config_values;		

};

struct _general_configClass
{
	GObjectClass parent_class;
};

G_MODULE_EXPORT GType		    general_config_get_type		(void);
G_MODULE_EXPORT general_config*	general_config_new			(void);
G_MODULE_EXPORT void			general_config_delete(general_config *self);

// Functions:
G_MODULE_EXPORT int general_config_load_config_file(general_config *self, const char *cfgfile);

G_MODULE_EXPORT int general_config_get_instance_max_size(general_config *self,int id);
// return: list of nest ids:
G_MODULE_EXPORT int* general_config_get_grid_id_list(general_config *self,int *_size);
G_MODULE_EXPORT void general_config_get_grid_size_ij(general_config *self,int *_i1, int *_j1, int *_i2, int *_j2,int id);


#define PRINT_M(DEBUG_LEVEL,DEBUG_LEVEL_,Message)\
                                                                                 if (DEBUG_LEVEL_ <= DEBUG_LEVEL)\
                                                                                  { \
                                                                                       if (DEBUG_LEVEL > 3) \
                                                                                        { \
                                                                                                fprintf(stdout,"%s\t%d\t%s\n",__FILE__,__LINE__,Message);\
                                                                                                 fflush(stdout);\
                                                                                        } else \
                                                                                        {\
                                                                                                 fprintf(stdout,"%s\n",Message);\
                                                                                                 fflush(stdout);\
                                                                                        }\
                                                                                   }

G_END_DECLS

#endif /* __GENERAL_CONFIG_H__ */
