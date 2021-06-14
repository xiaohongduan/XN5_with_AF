//      nitrogen_deposition.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __NITROGEN_DEPOSITION_H__
#define __NITROGEN_DEPOSITION_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define NITROGEN_DEPOSITION_TYPE				(nitrogen_deposition_get_type())
#define NITROGEN_DEPOSITION(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			NITROGEN_DEPOSITION_TYPE, nitrogen_deposition))
#define NITROGEN_DEPOSITION_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			NITROGEN_DEPOSITION_TYPE, nitrogen_depositionClass))
#define IS_NITROGEN_DEPOSITION(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			NITROGEN_DEPOSITION_TYPE))
#define IS_NITROGEN_DEPOSITION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			NITROGEN_DEPOSITION_TYPE))

typedef struct _nitrogen_deposition			nitrogen_deposition;
typedef struct _nitrogen_depositionClass		nitrogen_depositionClass;

struct _nitrogen_deposition
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
};


// public class member function:
G_MODULE_EXPORT int nitrogen_deposition_run(nitrogen_deposition *self);
G_MODULE_EXPORT int nitrogen_deposition_load(nitrogen_deposition *self);

struct _nitrogen_depositionClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType nitrogen_deposition_get_type (void);

G_END_DECLS

#endif /* __NITROGEN_DEPOSITION_H__ */

