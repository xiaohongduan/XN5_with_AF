#ifndef __STICKSTOFF_MACROS_H__
#define __STICKSTOFF_MACROS_H__


#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR_ID(xpn,S);\
			g_free(S);\
		}\
	 
#define GET_INI_INT(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_integer (keyfile,groupname,key,&error); \
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

#define G_FREE_IF_NOT_0(var)\
	{\
		if (var!=NULL)\
			{\
				g_free(var);\
				var=NULL;\
			}\
	}\
	
#define SET_VAR_TO_STRUC(xpn_type,first_element,var_name)\
	{\
	int i,i2,i3;\
	i = 0;\
	i3 = 0;\
	i2 = 0;\
	xpn_type l;\
	for (l = first_element,i=0;(l!=NULL);l=l->pNext,i++)\
		{\
			if (i2 >= var_len)\
            l->var_name = var[var_len-1];\
            else\
            l->var_name = var[i2];\
			if ((i>0) && (i<xpn->pSo->iLayers-2))\
				{\
					i3++;\
					if (i2 < var_len)\
                    if (i3 >= layers[i2])\
						{\
							i3=0;\
							i2++;\
						}\
				}\
		}\
	}\
	
#define GET_INI_DOUBLE_ARRAY_AND_SET_TO_STRUC(var_name,name,group,xpn_type,first_element)\
	{\
	GET_INI_DOUBLE_ARRAY(var,var_len,group,name);\
	CHECK_LEN(layers_len,var_len);\
	SET_VAR_TO_STRUC(xpn_type,first_element,var_name);\
	G_FREE_IF_NOT_0(var);\
	var_len=0;\
	}
	
#endif //__STICKSTOFF_MACROS_H__