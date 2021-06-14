//      wrf_coupling.h
//
//      Copyright 2010 Christian Klein <chrikle@berlios>
//



#ifndef __WRF_COUPLING_H__
#define __WRF_COUPLING_H__

#include <expertn_modul_base.h>
#include <expertn_noah_driver.h>

G_BEGIN_DECLS

#define WRF_COUPLING_TYPE				(wrf_coupling_get_type())
#define WRF_COUPLING(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
        WRF_COUPLING_TYPE, wrf_coupling))
#define WRF_COUPLING_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
        WRF_COUPLING_TYPE, wrf_couplingClass))
#define IS_WRF_COUPLING(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
        WRF_COUPLING_TYPE))
#define IS_WRF_COUPLING_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
        WRF_COUPLING_TYPE))

typedef struct _wrf_coupling			wrf_coupling;
typedef struct _wrf_couplingClass		wrf_couplingClass;

typedef struct
{
	int size_of_values;
	int size_of_valuelist;
	char **header;
	double **valuelist;
} struct_cdb_data;

typedef struct
{
	int cdb_var; // nummer in der cdb speicher struktur
	int linktype;  // Möglich sind: G_TYPE_INT,G_TYPE_DOUBLE, G_TYPE_FLOAT
	union
	{
		int   *pi;
		double *pd;
		float  *pf;
	} link;
} struct_cdb_link;

struct _wrf_coupling
{
	expertn_modul_base parent;
	/* add your public declarations here */


	struct_WRF *WRF;


	struct_WRF *own; // Expert-N Variablen, die übertragen werden würden, wenn es nur eine Instance pro Pixel geben würde


	struct_WRF **own_link; // Zeiger der auf own strukturen der anderen Expert-Ns zeigt;
	int own_link_count;

	expertnclass** class_link;
	int class_link_count;


	// CDB Input File:
	int use_cdb_input_file;
	gchar *cdb_input_file;
	struct_cdb_data *cdb_data;
	int free_wrf_struct;
	
	struct_cdb_link *cdblink;
	int cdblink_len;
	
	int cdb_sim_counter;
	
	int __ERROR;

};


// public class member function:
G_MODULE_EXPORT int wrf_coupling_load(wrf_coupling *self);
G_MODULE_EXPORT int wrf_coupling_mosaic_run(wrf_coupling *self);
G_MODULE_EXPORT int wrf_coupling_run(wrf_coupling *self);
G_MODULE_EXPORT int wrf_coupling_done(wrf_coupling *self);

int wrf_coupling_init_time_management(wrf_coupling *self);
void wrf_coupling_runWetter_WRF(wrf_coupling *self);
void  wrf_coupling_write_own(wrf_coupling *self);
void  wrf_coupling_write_copy_wrf_struct_only_climate(struct_WRF* dest, struct_WRF* src);
void wrf_coupling_check_for_infinity(wrf_coupling *self);

void  wrf_coupling_link_wrf_to_input_file(wrf_coupling *self);
int wrf_coupling_write_wrfstruct_from_data(wrf_coupling *self);

double lib_wrf_coupling_get_infinity();

struct _wrf_couplingClass
{
	expertn_modul_baseClass parent_class;
};




#define C_DEBUG(var) \
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);



G_MODULE_EXPORT GType wrf_coupling_get_type (void);

G_END_DECLS

#endif /* __WRF_COUPLING_H__ */
