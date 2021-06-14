//      wrfchem_coupling.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __WRFCHEM_COUPLING_H__
#define __WRFCHEM_COUPLING_H__

#include <expertn_modul_base.h>
#include <expertn_wrfchem_driver.h>

G_BEGIN_DECLS

#define WRFCHEM_COUPLING_TYPE				(wrfchem_coupling_get_type())
#define WRFCHEM_COUPLING(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			WRFCHEM_COUPLING_TYPE, wrfchem_coupling))
#define WRFCHEM_COUPLING_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			WRFCHEM_COUPLING_TYPE, wrfchem_couplingClass))
#define IS_WRFCHEM_COUPLING(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			WRFCHEM_COUPLING_TYPE))
#define IS_WRFCHEM_COUPLING_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			WRFCHEM_COUPLING_TYPE))

typedef struct _wrfchem_coupling			wrfchem_coupling;
typedef struct _wrfchem_couplingClass		wrfchem_couplingClass;

struct _wrfchem_coupling
{
	expertn_modul_base parent;
	/* add your public declarations here */	
	
	struct_WRF_CHEM *WRF_CHEM;
	struct_WRF_CHEM *own; // Expert-N Variablen, die übertragen werden würden, wenn es nur eine Instance pro Pixel geben würde
	
	struct_WRF_CHEM **own_link; // Zeiger der auf own strukturen der anderen Expert-Ns zeigt;
	int own_link_count;

	expertnclass** class_link;
	int class_link_count;
	
	int __ERROR;
};


// public class member function:
G_MODULE_EXPORT int wrfchem_coupling_load(wrfchem_coupling *self);
G_MODULE_EXPORT int wrfchem_coupling_mosaic_run(wrfchem_coupling *self);
G_MODULE_EXPORT int wrfchem_coupling_run(wrfchem_coupling *self);
G_MODULE_EXPORT int wrfchem_coupling_done(wrfchem_coupling *self);

void wrfchem_coupling_write_copy_wrfchem_struct_only_climate(struct_WRF_CHEM* dest, struct_WRF_CHEM* src);
void  wrfchem_coupling_write_own(wrfchem_coupling *self);


struct _wrfchem_couplingClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType wrfchem_coupling_get_type (void);

G_END_DECLS

#endif /* __WRFCHEM_COUPLING_H__ */

