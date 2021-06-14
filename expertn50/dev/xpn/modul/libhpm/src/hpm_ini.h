//      hpm_ini.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM__INI_H__
#define __HPM__INI_H__

#include "hpm.h"

G_BEGIN_DECLS


G_MODULE_EXPORT int init_hpm_model(hpm *self);
G_MODULE_EXPORT int done_hpm_model(hpm *self);
G_MODULE_EXPORT int load_config(hpm *self,const char* configfile);
G_MODULE_EXPORT int save_config(hpm *self,const char* configfile);
G_MODULE_EXPORT int load_parameter(hpm *self,const char* paramfile);
G_MODULE_EXPORT int register_var(hpm *self);

#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
	{ \
		gchar *S;\
		S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
		PRINT_ERROR_ID(xpn,S);\
		g_free(S);\
	} \
	
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
	} 

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
	}
	

#define SET_INI_DOUBLE(var,groupname,key) \
	g_key_file_set_double (keyfile,groupname,key,var);

G_END_DECLS

#endif /* __HPM_H__ */
