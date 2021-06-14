//      kc_factor.h
//      
//      Copyright 2015 Christian Klein <christian.klein@helmholtz-muenchen.de>
//      



#ifndef __KC_FACTOR_H__
#define __KC_FACTOR_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define KC_FACTOR_TYPE				(kc_factor_get_type())
#define KC_FACTOR(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			KC_FACTOR_TYPE, kc_factor))
#define KC_FACTOR_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			KC_FACTOR_TYPE, kc_factorClass))
#define IS_KC_FACTOR(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			KC_FACTOR_TYPE))
#define IS_KC_FACTOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			KC_FACTOR_TYPE))

typedef struct _kc_factor			kc_factor;
typedef struct _kc_factorClass		kc_factorClass;

typedef struct
{
  char *acCropName;
  char *acVarietyName;
  double *devkc_arr,*kc_arr;
  int devkc_len,kc_len;
  double *else_kc_arr;
} struct_kc_factor_crop_rot;


struct _kc_factor
{
	expertn_modul_base parent;
	/* add your public declarations here */
	struct_kc_factor_crop_rot *crop;
    int crop_size;
    
};


// public class member function:
G_MODULE_EXPORT int kc_factor_const(kc_factor *self);

  
G_MODULE_EXPORT int kc_factor_dev_stage_load(kc_factor *self);
G_MODULE_EXPORT int kc_factor_dev_stage_run(kc_factor *self);
G_MODULE_EXPORT int kc_factor_dev_stage_done(kc_factor *self);

G_MODULE_EXPORT int kc_factor_LAI3rd_load(kc_factor *self);
G_MODULE_EXPORT int kc_factor_LAI3rd_run(kc_factor *self);

int kc_factor_read_dev_file(kc_factor *self,char* filename);

struct_kc_factor_crop_rot* free_struct_kc_factor_crop_rot(struct_kc_factor_crop_rot* crop, int* len);
struct_kc_factor_crop_rot *realloc_struct_kc_factor_crop_rot(struct_kc_factor_crop_rot* crop, int size);

struct _kc_factorClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType kc_factor_get_type (void);


#define GET_INI_INT(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_integer (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}\

#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		} \
	 

#define GET_INI_STRING(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_string (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR_ID(xpn,S);\
			g_free(S);\
		}\
	 
#define GET_INI_INT_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_integer_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	 


#define GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_double_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(xpn,S);\
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
				PRINT_ERROR_ID(xpn,S);\
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
				PRINT_ERROR_ID(xpn,S);\
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
				PRINT_ERROR_ID(xpn,S);\
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

#define GET_INI_DATE(_date,groupname,key) \
	{\
		char* var;\
		error = NULL; \
		var = g_key_file_get_string (keyfile,groupname,key,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
				_date=NULL;\
			} else\
			{\
				_date = convert_str_to_gdate(var);\
				g_free(var);\
			}\
	}\
	 
#define CHECK_LEN(var1,var2)\
	{\
		if (var1!=var2)\
			{\
				gchar *S;\
				S = g_strdup_printf  ("%s is not %s. Check your configuration in file: %s!",#var1,#var2,filename);\
				PRINT_ERROR_ID(xpn,S);\
				g_free(S);\
			}\
	}\
	 

G_END_DECLS

#endif /* __KC_FACTOR_H__ */
