//      xpn_output_netcdf.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __XPN_OUTPUT_NETCDF_H__
#define __XPN_OUTPUT_NETCDF_H__

#include <glib-object.h>
#include <expertnclass.h>

G_BEGIN_DECLS

#define TYPE_XPN_OUTPUT_NETCDF                (xpn_output_netcdf_get_type())
#define XPN_OUTPUT_NETCDF(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            TYPE_XPN_OUTPUT_NETCDF, xpn_output_netcdf))
#define XPN_OUTPUT_NETCDF_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass),\
            TYPE_XPN_OUTPUT_NETCDF, xpn_output_netcdfClass))
#define IS_XPN_OUTPUT_NETCDF(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            TYPE_XPN_OUTPUT_NETCDF))
#define IS_XPN_OUTPUT_NETCDF_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),\
            TYPE_XPN_OUTPUT_NETCDF))
#define XPN_OUTPUT_NETCDF_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj),\
            TYPE_XPN_OUTPUT_NETCDF, xpn_output_netcdfClass))

typedef struct _xpn_output_netcdf            xpn_output_netcdf;
typedef struct _xpn_output_netcdfClass        xpn_output_netcdfClass;


typedef struct
{
	int xpn_class;
	int var_number;
	int i;
	int j;
	int k;
} STRUCT_REG_VAR;

typedef struct
{
	char* name;
	int netcdf_id;
	STRUCT_REG_VAR *reg_var;
	int reg_var_len;	
} STRUCT_XPN_VARLIST;

typedef struct
{
  STRUCT_XPN_VARLIST *var_list;
  int var_list_len;
} STRUCT_OTHER_RANK_VARS;

typedef struct
{
	// interne Datten
	STRUCT_XPN_VARLIST *var_list;
	int var_list_len;
	
	STRUCT_OTHER_RANK_VARS *other_ranks_vars;
	int other_ranks_vars_len;
	
	char *netcdf_filename;
	int time_dim;
	int TimeStrLen_dim;
	
	int i_dim,j_dim,k_dim;
	int time_var_id;
	int Times_var_id;

	STRUCT_XPN_VARLIST XLAT,XLON;
	
	int netf;// netcdf File ID;
	
	// XPN Daten
	int i1; // min i
	int i2; // max i
	int j1; // min j
	int j2; // max j
	int instance_size; // max size of instances
	int nest_id; // nest id
	int first_xpn_instance_for_domain;
	
	int time_dims[1];
	int Times_dims[2];
	int var_dims[4];
	
	int count; // Zeitschritt
	double ftime; // Expert-N Zeit der Domain
	double output_time_step; // output_time_step in tagen
	char *output_time_step_string; // utput_time_step als string
	
	struct_timespan timespan;
	
	STRUCT_XPN_TIME xno_time;
	STRUCT_XPN_TIME xpn_time_old;
		
	
} STRUCT_NESTS;


struct _xpn_output_netcdf
{
    GObject parent;
	
    // von Expert-N uebergebene Daten:
	expertnclass **xpn_class;
	int xpn_class_len;	
	general_config* cfg;
	
	
	STRUCT_NESTS *nests;
	int nests_len;
	
	// interne Daten:
	int NETCDF_FORMAT;
	char *netcdf_path;

	int activated; // [0 or 1] netcdfoutput activeted or not
	int always_dump_nc_file; // [0 or 1] 1 is standart. Dump NC File every output time step (is slower 
	
	// MPI Variablen:
	
	int loop_min,loop_max;
	
	#ifdef USE_MPI
	int numtasks;
	int rank;	
	#endif
	
};



struct _xpn_output_netcdfClass
{
    GObjectClass parent_class;
};


G_MODULE_EXPORT GType       xpn_output_netcdf_get_type        (void);
G_MODULE_EXPORT xpn_output_netcdf *xpn_output_netcdf_new(expertnclass ** classes,int classes_len,STRUCT_Config_Value* config_values,int config_values_len,general_config* cfg);
G_MODULE_EXPORT xpn_output_netcdf * xpn_output_netcdf_Done(xpn_output_netcdf *self);

G_MODULE_EXPORT void xpn_output_netcdf_write_data_matrix(xpn_output_netcdf *self, int grid_id);

G_END_DECLS

#endif /* __XPN_OUTPUT_H__ */
