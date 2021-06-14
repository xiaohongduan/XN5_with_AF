#ifndef __XPN_OUTPUT_MACROS_H__
#define __XPN_OUTPUT_MACROS_H__

#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}


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
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	
#define GET_INI_DOUBLE_ARRAY_OPTIONAL(var,var_size,groupname,key) \
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
				var=NULL;\
				var_size=0;\
			} else\
			{\
				GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key);\
			}\
	}\
	



#define GET_INI_PROFILE_ARRAY(var_from,var_to,var_size,groupname,key) \
	{\
		gsize _var_size;\
		char **_var_temp;\
		char *_S_temp;\
		error = NULL;\
		int _temp_i;\
		_var_temp = g_key_file_get_string_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
		var_from = g_malloc0_n(var_size,sizeof(double));\
		var_to = g_malloc0_n(var_size,sizeof(double));\
		for (_temp_i=0;_temp_i<var_size;_temp_i++)\
			{\
				if (getStringColumn_get_Size(_var_temp[_temp_i],"-")!=2)\
				{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,"Profile seperator is wrong");\
				PRINT_ERROR(S);\
				g_free(S);\
				}\
				_S_temp = g_malloc0(strlen(_var_temp[_temp_i])+1);\
				getStringColumn(_S_temp,_var_temp[_temp_i],"-",0);\
				var_from[_temp_i]=atof(_S_temp);\
				getStringColumn(_S_temp,_var_temp[_temp_i],"-",1);\
				var_to[_temp_i]=atof(_S_temp);\
				g_free(_var_temp[_temp_i]);\
				g_free(_S_temp);\
			}\
		g_free(_var_temp);\
	}\
	
	
#define GET_INI_PROFILE_ARRAY_OPTIONAL(var_from,var_to,var_size,groupname,key) \
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
			{\
				var_from=NULL;\
				var_to=NULL;\
				var_size=0;\
			} else\
			{\
				GET_INI_PROFILE_ARRAY(var_from,var_to,var_size,groupname,key);\
			}\
	}\
	

#define WRITE_TO_PROFILE_VAR(VARIABLE,VAR_NAME,MUL_FACT)\
  if (darray_from!=NULL)\
		{\
			if (darray_to!=NULL)\
				{\
					for (i=0; i<darray_len; i++)\
						{\
							depth_from = darray_from[i];\
							depth_to = darray_to[i];\
							act_depth = 0.0;\
							first=0;\
							layer_from=0;\
							layer_to =0;\
							for (iLayer=1,pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext,pHL=xpn->pHe->pHLayer->pNext, pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext, iLayer = 1;\
							        ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL));\
							        pSL = pSL->pNext,pCL=pCL->pNext, pSW = pSW->pNext, pWL = pWL->pNext,pHL=pHL->pNext, iLayer++)\
								{\
									act_depth+=pSL->fThickness*1.0e-3;\
									if ((act_depth >= depth_from) && (act_depth < depth_to))\
										{\
											if (first==0)\
												{\
													self->spec_vars_profile_count++;\
													self->spec_vars_profile = g_realloc(self->spec_vars_profile,self->spec_vars_profile_count*sizeof(struct_special_var_profile));\
													self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars_count=0;\
													self->spec_vars_profile[self->spec_vars_profile_count-1].name=g_strdup_printf (VAR_NAME,(int)(depth_from*100.0+0.5),(int)(depth_to*100.0+0.5));\
													self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars=NULL;\
													layer_from=iLayer;\
													first++;\
												}\
											self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars_count++;\
											count=self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars_count;\
											self->spec_vars_profile[self->spec_vars_profile_count-1].mul_fact=MUL_FACT;\
											self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars = g_realloc(self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars,self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars_count*sizeof(double *));\
											self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars[self->spec_vars_profile[self->spec_vars_profile_count-1].link_to_vars_count-1]=&(VARIABLE);\
											layer_to=iLayer;\
										}\
								}\
							if (5 <= xpn->DEBUG_LEVEL)\
								{\
									S=g_strdup_printf("%s is from layer %d to %d",self->spec_vars_profile[self->spec_vars_profile_count-1].name,layer_from,layer_to);\
									PRINT_MESSAGE(xpn,5,S);\
									g_free(S);\
								}\
						}\
					g_free(darray_to);\
					darray_to=NULL;\
				}\
			g_free(darray_from);\
			darray_from=NULL;\
		}
    
#endif
