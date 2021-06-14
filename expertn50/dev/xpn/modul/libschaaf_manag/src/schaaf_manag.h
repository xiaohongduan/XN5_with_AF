//      schaaf_manag.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __SCHAAF_MANAG_H__
#define __SCHAAF_MANAG_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define SCHAAF_MANAG_TYPE				(schaaf_manag_get_type())
#define SCHAAF_MANAG(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			SCHAAF_MANAG_TYPE, schaaf_manag))
#define SCHAAF_MANAG_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			SCHAAF_MANAG_TYPE, schaaf_managClass))
#define IS_SCHAAF_MANAG(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			SCHAAF_MANAG_TYPE))
#define IS_SCHAAF_MANAG_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			SCHAAF_MANAG_TYPE))

typedef struct _schaaf_manag			schaaf_manag;
typedef struct _schaaf_managClass		schaaf_managClass;

struct _schaaf_manag
{
	expertn_modul_base parent;
	
	/* add your public declarations here */
//for INI
	char *ini_filename;
	int __LOAD_DONE;
	
	
//tillage
    int tillage_done;
    char **tillage_date, **tillage_implement, **tillage_code;	
	int tillage_date_len, tillage_implement_len, tillage_code_len;
	GDate **tillage_gdate;
	double *tillage_depth;
	int tillage_depth_len;
	
	PTILLAGE tillage_actual;
	
//irrigation
    int irrigation_done;
    char **irrigation_date, **irrigation_type, **irrigation_code;	
	int irrigation_date_len, irrigation_type_len, irrigation_code_len;
	GDate **irrigation_gdate;
	double *irrigation_amount, *irrigation_rate, *irrigation_no3n, *irrigation_nh4n, *irrigation_org_n, *irrigation_org_c;
	int irrigation_amount_len,irrigation_rate_len, irrigation_no3n_len, irrigation_nh4n_len, irrigation_org_n_len, irrigation_org_c_len;
	
	PMIRRIGATION irrigation_actual;
	
};


// public class member function:
/*********************************************************************************/
/**                    Externe Functionen                                       **/
/*********************************************************************************/
G_MODULE_EXPORT int TSSoilLoosening(schaaf_manag *self);
G_MODULE_EXPORT int TSSoilMixing(schaaf_manag *self);
G_MODULE_EXPORT int TSIrrigation(schaaf_manag *self);
G_MODULE_EXPORT int schaaf_manag_load(schaaf_manag *self);
//G_MODULE_EXPORT int schaaf_tillage_done(schaaf_manag *self);

/*********************************************************************************/
/**                 Interne Functionen                                          **/
/*********************************************************************************/
int InitBBGeraet(schaaf_manag *self);
int LagerungNeu(schaaf_manag *self);
int EinarbeitRueckstaende(schaaf_manag *self);
int DurchMisch(schaaf_manag *self);
	

struct _schaaf_managClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType schaaf_manag_get_type (void);

G_END_DECLS

#endif /* __SCHAAF_MANAG_H__ */

