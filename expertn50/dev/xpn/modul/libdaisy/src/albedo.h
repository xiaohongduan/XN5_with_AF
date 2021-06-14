//      albedo.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __ALBEDO_H__
#define __ALBEDO_H__

#include <expertn_modul_base.h>
#include <expertn_noah_driver.h>

G_BEGIN_DECLS

#define ALBEDO_TYPE				(albedo_get_type())
#define ALBEDO(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			ALBEDO_TYPE, albedo))
#define ALBEDO_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			ALBEDO_TYPE, albedoClass))
#define IS_ALBEDO(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			ALBEDO_TYPE))
#define IS_ALBEDO_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			ALBEDO_TYPE))

typedef struct _albedo			albedo;
typedef struct _albedoClass		albedoClass;

struct _albedo
{
	expertn_modul_base parent;
	/* add your public declarations here */
	int Soil_Type;
	int Plant_Type;
	struct_WRF *wrf;
};


// public class member function:
G_MODULE_EXPORT int albedo_load(albedo *self);
G_MODULE_EXPORT int albedo_run(albedo *self);
G_MODULE_EXPORT int albedo_load_const(albedo *self);

struct _albedoClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType albedo_get_type (void);

#define C_DEBUG(var)\
	xpn_register_var_add_double(xpn->pXSys->var_list,#var,var,__LINE__,TRUE);

G_END_DECLS

#endif /* __ALBEDO_H__ */
