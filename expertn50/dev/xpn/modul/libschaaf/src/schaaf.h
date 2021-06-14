//      schaaf.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __SCHAAF_H__
#define __SCHAAF_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define SCHAAF_TYPE				(schaaf_get_type())
#define SCHAAF(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			SCHAAF_TYPE, schaaf))
#define SCHAAF_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			SCHAAF_TYPE, schaafClass))
#define IS_SCHAAF(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			SCHAAF_TYPE))
#define IS_SCHAAF_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			SCHAAF_TYPE))

typedef struct _schaaf			schaaf;
typedef struct _schaafClass		schaafClass;

struct _schaaf
{
	expertn_modul_base parent;
	/* add your public declarations here */
	char *ini_filename;
	
	int first_fertil_done; //added by F.H. on 20171220 
	int fertil_done;
	
	// Mineral fertilization
	char **min_date, **min_fertilizer, **min_code;
	int min_date_len, min_fertilizer_len, min_code_len;
	double *min_n_tot, *min_no3n, *min_nh4n, *min_urea;
	int min_n_tot_len, min_no3n_len, min_nh4n_len, min_urea_len;
	
	// Organic fertilization
	char **org_date, **org_fertilizer, **org_code;
	int org_date_len, org_fertilizer_len, org_code_len;
	double *org_amount, *org_dry_matter, *org_subst, *org_n_tot, *org_nh4n;
	int org_amount_len, org_dry_matter_len, org_subst_len, org_n_tot_len, org_nh4n_len;

	// Mineral + Organic fertilization
	char **ges_date, **ges_fertilizer, **ges_code;
	double *ges_n_tot, *ges_n_org_tot, *ges_no3n, *ges_nh4n, *ges_urea;
	double *ges_amount, *ges_dry_matter, *ges_subst;
	
	//für Sortierung
	char *ges_date_zwischen, *ges_fertilizer_zwischen, *ges_code_zwischen;
	double ges_n_tot_zwischen, ges_n_org_tot_zwischen, ges_no3n_zwischen, ges_nh4n_zwischen, ges_urea_zwischen;
	double ges_amount_zwischen, ges_dry_matter_zwischen, ges_subst_zwischen;
	

	
	// irrigation:
	char **irr_date;
	GDate **irr_gdate;
	double *irr_amount;
	int irr_date_len;
	int irr_amount_len;
	
	PMIRRIGATION irr_actual;
};


// public class member function:
G_MODULE_EXPORT int schaaf_fertilizer(schaaf *self);
G_MODULE_EXPORT int read_fertilizers(schaaf *self);
G_MODULE_EXPORT int schaaf_irrigation(schaaf *self);
G_MODULE_EXPORT int read_irrigation(schaaf *self);

int schaaf_mineral_fertilization(schaaf *self);
int schaaf_organic_fertilization(schaaf *self);
int InitOrgDuenger(PNFERTILIZER fertil);
int InfiltrationOrgDuenger(schaaf *self);
int InfiltrationOrgDuengerRegen(schaaf *self);
int MinerOrgDuengHoff(schaaf *self);
int NitrOrgNH4Hoff(schaaf *self);



struct _schaafClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType schaaf_get_type (void);

#define  EPSILON  1e-9

#define GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_double_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	
#define GET_INI_STRING_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_string_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\

#define GET_INI_DOUBLE_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(double)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = std_value;\
					}\
			} else\
			{\
				GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key);\
			}\
	}\
	
	
#define GET_INI_STRING_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(char*)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = g_strdup_printf("%s",std_value);\
					}\
			} else\
			{\
				GET_INI_STRING_ARRAY(var,var_size,groupname,key)\
			}\
	}\

#define CHECK_LEN(var1,var2)\
	{\
		if (var1!=var2)\
			{\
				gchar *S;\
				S = g_strdup_printf  ("%s is not %s. Check your configuration in file: %s!",#var1,#var2,filename);\
				PRINT_ERROR(S);\
				g_free(S);\
				return -1;\
			}\
	}\



G_END_DECLS

#endif /* __SCHAAF_H__ */

