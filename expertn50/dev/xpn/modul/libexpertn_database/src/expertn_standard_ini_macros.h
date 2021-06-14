#ifndef __XPN_STANDARD_INI_MACROS_H__
#define __XPN_STANDARD_INI_MACROS_H__


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
	 
#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}\
	 
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
	 
#define GET_INI_STRING(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_string (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
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
				PRINT_ERROR(S);\
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
	
#define GET_INI_DOUBLE_OPTIONAL(var,groupname,key,std_value)\
{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (key_exists==FALSE) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (in file %s) is missing. Standard Value (%f) taken instead!\n",groupname,key,filename,std_value);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{\
				var = std_value;\
			} else\
			{\
				GET_INI_DOUBLE(var,groupname,key);\
			}\
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
	 
#define G_FREE_IF_NOT_0(var)\
	{\
		if (var!=NULL)\
			{\
				g_free(var);\
				var=NULL;\
			}\
	}\

#define FREE_IF_NOT_0(var)\
	{\
		if (var!=NULL)\
			{\
				free(var);\
				var=NULL;\
			}\
	}\
	
#define G_FREE_DATE_IF_NOT_0(var)\
	{\
		if (var!=NULL)\
			{\
				g_date_free(var);\
				var=NULL;\
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
	
#define calc_if_n_value(var,var_len,operation,val)\
	{\
		int i;\
		for (i=0;i<var_len;i++)\
			{\
				if (var[i]!=val)\
					{\
						operation;\
					}\
			}\
	}\

#endif //__XPN_STANDARD_INI_MACROS_H__


