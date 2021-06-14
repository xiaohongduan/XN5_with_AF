//      module_example.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __MODULE_EXAMPLE_H__
#define __MODULE_EXAMPLE_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define MODULE_EXAMPLE_TYPE				(module_example_get_type())
#define MODULE_EXAMPLE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			MODULE_EXAMPLE_TYPE, module_example))
#define MODULE_EXAMPLE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			MODULE_EXAMPLE_TYPE, module_exampleClass))
#define IS_MODULE_EXAMPLE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			MODULE_EXAMPLE_TYPE))
#define IS_MODULE_EXAMPLE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			MODULE_EXAMPLE_TYPE))

typedef struct _module_example			module_example;
typedef struct _module_exampleClass		module_exampleClass;

struct _module_example
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
};


// public class member function:
G_MODULE_EXPORT int module_example_BiomassGrowth(module_example *self);
G_MODULE_EXPORT int module_example_Photosynthesis(module_example *self);

struct _module_exampleClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType module_example_get_type (void);

G_END_DECLS

#endif /* __MODULE_EXAMPLE_H__ */

