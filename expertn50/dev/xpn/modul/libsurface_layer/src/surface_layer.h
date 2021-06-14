//      surface_layer.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __SURFACE_LAYER_H__
#define __SURFACE_LAYER_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define SURFACE_LAYER_TYPE				(surface_layer_get_type())
#define SURFACE_LAYER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			SURFACE_LAYER_TYPE, surface_layer))
#define SURFACE_LAYER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			SURFACE_LAYER_TYPE, surface_layerClass))
#define IS_SURFACE_LAYER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			SURFACE_LAYER_TYPE))
#define IS_SURFACE_LAYER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			SURFACE_LAYER_TYPE))

typedef struct _surface_layer			surface_layer;
typedef struct _surface_layerClass		surface_layerClass;

struct _surface_layer
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
};


// public class member function:
G_MODULE_EXPORT int surface_layer_setup_stability(surface_layer *self);

struct _surface_layerClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType surface_layer_get_type (void);

#define C_DEBUG(var)\
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

G_END_DECLS

#endif /* __SURFACE_LAYER_H__ */

