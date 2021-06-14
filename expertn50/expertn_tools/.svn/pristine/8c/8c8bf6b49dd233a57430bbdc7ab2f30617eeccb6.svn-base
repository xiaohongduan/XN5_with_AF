//      ${template}.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __${templ_uppercase}_H__
#define __${templ_uppercase}_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define ${templ_uppercase}_TYPE				(${template}_get_type())
#define ${templ_uppercase}(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			${templ_uppercase}_TYPE, ${template}))
#define ${templ_uppercase}_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			${templ_uppercase}_TYPE, ${template}Class))
#define IS_${templ_uppercase}(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			${templ_uppercase}_TYPE))
#define IS_${templ_uppercase}_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			${templ_uppercase}_TYPE))

typedef struct _${template}			${template};
typedef struct _${template}Class		${template}Class;

struct _${template}
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
};


// public class member function:
G_MODULE_EXPORT int ${template}_BiomassGrowth(${template} *self);
G_MODULE_EXPORT int ${template}_Photosynthesis(${template} *self);

struct _${template}Class
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType ${template}_get_type (void);

G_END_DECLS

#endif /* __${templ_uppercase}_H__ */

