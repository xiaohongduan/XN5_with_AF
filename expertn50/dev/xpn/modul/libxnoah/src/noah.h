//      noah.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __NOAH_H__
#define __NOAH_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define NOAH_TYPE				(noah_get_type())
#define NOAH(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			NOAH_TYPE, noah))
#define NOAH_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			NOAH_TYPE, noahClass))
#define IS_NOAH(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			NOAH_TYPE))
#define IS_NOAH_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			NOAH_TYPE))

typedef struct _noah			noah;
typedef struct _noahClass		noahClass;

struct _noah
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double ch,cm;
	double T1;
	
	// const:
	double sbeta;
	double czil;
};


// public class member function:

G_MODULE_EXPORT int noah_run_load(noah *self);
G_MODULE_EXPORT int noah_run_senible_heat(noah *self);
G_MODULE_EXPORT int noah_run_done(noah *self);

struct _noahClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType noah_get_type (void);

G_END_DECLS

#endif /* __NOAH_H__ */

