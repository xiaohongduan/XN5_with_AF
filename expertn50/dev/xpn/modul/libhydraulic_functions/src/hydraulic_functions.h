//      hydraulic_functions.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HYDRAULIC_FUNCTIONS_H__
#define __HYDRAULIC_FUNCTIONS_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define HYDRAULIC_FUNCTIONS_TYPE				(hydraulic_functions_get_type())
#define HYDRAULIC_FUNCTIONS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			HYDRAULIC_FUNCTIONS_TYPE, hydraulic_functions))
#define HYDRAULIC_FUNCTIONS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			HYDRAULIC_FUNCTIONS_TYPE, hydraulic_functionsClass))
#define IS_HYDRAULIC_FUNCTIONS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			HYDRAULIC_FUNCTIONS_TYPE))
#define IS_HYDRAULIC_FUNCTIONS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			HYDRAULIC_FUNCTIONS_TYPE))

typedef struct _hydraulic_functions			hydraulic_functions;
typedef struct _hydraulic_functionsClass		hydraulic_functionsClass;

struct _hydraulic_functions
{
	expertn_modul_base parent;
	/* add your public declarations here */
        int hydraulic_function_choice;
};


// public class member function:
G_MODULE_EXPORT int hydraulic_functions_set(hydraulic_functions *self,void *WCont,void *HCond, void *DWCap, void *MPotl);
G_MODULE_EXPORT int hydraulic_functions_set_Hutson_and_Cass(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_van_Genuchten_and_Mualem(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Brooks_and_Corey(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Brutsaert_and_Gardner(hydraulic_functions *self);
G_MODULE_EXPORT int hydraulic_functions_set_Bimodal_van_Genuchten(hydraulic_functions *self);

struct _hydraulic_functionsClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType hydraulic_functions_get_type		(void);

G_END_DECLS

#endif /* __HYDRAULIC_FUNCTIONS_H__ */

