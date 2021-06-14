#include "hpm_ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "hpm_math.h"




int static file_exists(const char * filename)
{
	FILE * file;
	file = fopen(filename, "r");
	if (file != NULL) {
		fclose(file);
		return 1;
	}
	return 0;
}

int init_hpm_model(hpm *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *parameter_filename,*ini_filename;

	// Wenn die Init bereits geladen ist
	if (self->__INIT_DONE==0) {
		self->__INIT_DONE=1;
	} else {
		return RET_SUCCESS;
	}

	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL)) {
		PRINT_ERROR_ID(xpn,"Problem with hydraulic functions!");
	}

	// Vars:

	// Ouputs auf 0 setzen:
	memset(&(self->Output),0,sizeof(T_Output));

	// Zunächst: Registry:

	register_var(self);


	// Read from INI File:

	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.hpm.ini_filename");

	if (ini_filename==NULL) {
		PRINT_ERROR("Missing entry 'hpm.ini_filename' in your configuration!");
	} else {

		char *S2;
		S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
		if (S2!=NULL) {
			ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
			if (load_config(self,ini_filename)!=0) {
				PRINT_ERROR("Error Read HPM INI File");
			}
			self->ini_filename = g_strdup_printf("%s",ini_filename);
			free(ini_filename);
			free(S2);
		}

	}




	parameter_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.hpm.parameter_filename");

	if (parameter_filename==NULL) {
		PRINT_ERROR("Missing entry 'hpm.parameter_filename' in your configuration!");
	} else {
		char *S2;
		S2 = expertn_modul_base_replace_std_templates(xpn,parameter_filename);
		if (S2!=NULL) {
			parameter_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
			if (load_parameter(self,parameter_filename)!=0) {
				PRINT_ERROR("Error Read HPM PARAMETER File");
			}
			free(parameter_filename);
			free(S2);
		}



	}

	self->Control.Time_Zone = xpn->pTi->pSimTime->fTimeZone;

	if (self->Control.Time_Zone==0.0) {
		self->Control.Lz=0.0;
	} else if (self->Control.Time_Zone<0.0) {
		self->Control.Lz = -(self->Control.Time_Zone)/24.0 * 360.0;
	} else {
		self->Control.Lz = 360.0 - self->Control.Time_Zone/24.0 * 360.0;
	}

	self->Plant.MXLam = hpm_math_sum4(self->Plant.MLam4);
	self->Plant.MXss = hpm_math_sum4(self->Plant.Mss4);
	self->Plant.MXrt = hpm_math_sum4(self->Plant.Mrt4);
	xpn->pPl->pCanopy->fLAI = (float)hpm_math_sum4(self->Plant.LAI4);
	// '
	//Gilt nur am Anfang
	xpn->pPl->pRoot->fDepth=(float) (self->Plant.MXrt/ 1.0 * 1.0e3); //TODO: strange ergibt 2 statt 20(Plant.MXrt/ 1.0 * 1.0e-3);

	self->Plant.MCTrt=0.0;
	self->Plant.MCTsh=0.0;
	{
		//double Nph_ul=0.005;
		self->Plant.MNph_ul = 0.00133995;//= Nph_ul * (Plant.MXss+Plant.MXLam);
		self->Plant.rort = 0.809533;
	}


	self->Environment.CO2vpm = 383;//CO2;//700.0;//CO2; // constante in ExpertN xplant.h = 400 in HPM wird 350 angegeben

	// switch auto_ini_files on off:

	self->__CREATE_AUTO_INI_FILES = 0;

	{
		char *S;
		S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.hpm.create_auto_init_files");
		if (S==NULL) {
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.create_auto_init_files");
			if (S==NULL) {
				S = "0";
			}
		}

		self->__CREATE_AUTO_INI_FILES = atoi(S);
	}

	if (self->__CREATE_AUTO_INI_FILES==1) {
		PRINT_MESSAGE(xpn,4,"At the end of this simulation Start Values will be automatic saved in the ini file!");
	}

	self->__STATIC_ROOT_LENGTH=1;
	self->__USE_STATIC_ROOT_LENGTH=FALSE;

	{
		self->__USE_STATIC_ROOT_LENGTH = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.hpm.use_static_root_length", 0);
		if (self->__USE_STATIC_ROOT_LENGTH==TRUE) {
			self->__STATIC_ROOT_LENGTH = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.hpm.static_root_length", 1);
		}

	}

#ifdef TEST_HPM
	hpm_read_csv_file(self,"/home/chrikle/expertn50/built/cfg/test3/output/hpm_input/xpnout3.xno",2);
	self->test_count=0;


	{
		int i;
		for (i=0; i<self->test_data->size_of_values; i++) {
			printf("%s\n",self->test_data->header[i]);
		}
	}
#endif
	self->__DONE_DONE = 0;

	return RET_SUCCESS;
}

int done_hpm_model(hpm *self)
{
	expertn_modul_base *xpn = &(self->parent);
	if (self->__DONE_DONE == 0) {
		self->__DONE_DONE=1;
	} else {
		return RET_SUCCESS;
	}

	if (self->__CREATE_AUTO_INI_FILES==1) {
		char *S;
		S = g_strdup_printf("Save Values to ini file: %s!",self->ini_filename);
		PRINT_MESSAGE(xpn,4,S);
		g_free(S);
		save_config(self,self->ini_filename);
	}

	if (self->ini_filename!=NULL) g_free(self->ini_filename);
	self->ini_filename=NULL;
	return RET_SUCCESS;
}

int load_config(hpm *self,const char* configfile)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	const char* filename=configfile;
	expertn_modul_base *xpn = &(self->parent);

	memset(&(self->Control),0,sizeof(T_Control));
	memset(&(self->Plant),0,sizeof(T_Plant));
	memset(&(self->Respiration),0,sizeof(T_Respiration));
	memset(&(self->Litter),0,sizeof(T_Litter));
	memset(&(self->Water),0,sizeof(T_Water));
	memset(&(self->Harvest),0,sizeof(T_Harvest));
	memset(&(self->Animal),0,sizeof(T_Animal));
	memset(&(self->Environment),0,sizeof(T_Environment));

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, configfile, flags, &error)) {
		PRINT_ERROR(error->message);
		return -1;
	}

	GET_INI_DOUBLE(self->Control.water,"Control","water");
	GET_INI_DOUBLE(self->Control.diurnal,"Control","diurnal");
	GET_INI_DOUBLE(self->Control.PAR_sin,"Control","PAR_sin");
//	GET_INI_DOUBLE(self->Control.sim_time,"Control","sim_time");

	GET_INI_DOUBLE(self->Environment.Pair,"Environment","Pair");

	GET_INI_DOUBLE(self->Water.Wrt,"Water","Wrt");
	GET_INI_DOUBLE(self->Water.Wsh,"Water","Wsh");


	GET_INI_DOUBLE(self->Plant.MLam4[0],"Plant","MLam4[0]");
	GET_INI_DOUBLE(self->Plant.MLam4[1],"Plant","MLam4[1]");
	GET_INI_DOUBLE(self->Plant.MLam4[2],"Plant","MLam4[2]");
	GET_INI_DOUBLE(self->Plant.MLam4[3],"Plant","MLam4[3]");

	GET_INI_DOUBLE(self->Plant.Mss4[0],"Plant","Mss4[0]");
	GET_INI_DOUBLE(self->Plant.Mss4[1],"Plant","Mss4[1]");
	GET_INI_DOUBLE(self->Plant.Mss4[2],"Plant","Mss4[2]");
	GET_INI_DOUBLE(self->Plant.Mss4[3],"Plant","Mss4[3]");

	GET_INI_DOUBLE(self->Plant.Mrt4[0],"Plant","Mrt4[0]");
	GET_INI_DOUBLE(self->Plant.Mrt4[1],"Plant","Mrt4[1]");
	GET_INI_DOUBLE(self->Plant.Mrt4[2],"Plant","Mrt4[2]");
	GET_INI_DOUBLE(self->Plant.Mrt4[3],"Plant","Mrt4[3]");

	GET_INI_DOUBLE(self->Plant.LAI4[0],"Plant","LAI4[0]");
	GET_INI_DOUBLE(self->Plant.LAI4[1],"Plant","LAI4[1]");
	GET_INI_DOUBLE(self->Plant.LAI4[2],"Plant","LAI4[2]");
	GET_INI_DOUBLE(self->Plant.LAI4[3],"Plant","LAI4[3]");

	GET_INI_DOUBLE(self->Plant.MCSrt,"Plant","MCSrt");
	GET_INI_DOUBLE(self->Plant.MCSsh,"Plant","MCSsh");
	GET_INI_DOUBLE(self->Plant.MNSrt,"Plant","MNSrt");
	GET_INI_DOUBLE(self->Plant.MNSsh,"Plant","MNSsh");

	GET_INI_DOUBLE(self->Animal.n,"Animal","n");
	GET_INI_DOUBLE(self->Animal.active,"Animal","active");

	//GET_INI_DOUBLE(self->Harvest.active,"Harvest","active");
	//GET_INI_DOUBLE(self->Harvest.Pulseharv,"Harvest","Pulseharv");

	g_key_file_free(keyfile);

	return RET_SUCCESS;
}

int save_config(hpm *self,const char* configfile)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	char *data;
	gsize length;
	FILE *f;
	int write_header;
	char *S;

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, configfile, flags, &error)) {
		PRINT_ERROR(error->message);
		return -1;
	}

	SET_INI_DOUBLE(self->Water.Wrt,"Water","Wrt");
	SET_INI_DOUBLE(self->Water.Wsh,"Water","Wsh");


	SET_INI_DOUBLE(self->Plant.MLam4[0],"Plant","MLam4[0]");
	SET_INI_DOUBLE(self->Plant.MLam4[1],"Plant","MLam4[1]");
	SET_INI_DOUBLE(self->Plant.MLam4[2],"Plant","MLam4[2]");
	SET_INI_DOUBLE(self->Plant.MLam4[3],"Plant","MLam4[3]");

	SET_INI_DOUBLE(self->Plant.Mss4[0],"Plant","Mss4[0]");
	SET_INI_DOUBLE(self->Plant.Mss4[1],"Plant","Mss4[1]");
	SET_INI_DOUBLE(self->Plant.Mss4[2],"Plant","Mss4[2]");
	SET_INI_DOUBLE(self->Plant.Mss4[3],"Plant","Mss4[3]");

	SET_INI_DOUBLE(self->Plant.Mrt4[0],"Plant","Mrt4[0]");
	SET_INI_DOUBLE(self->Plant.Mrt4[1],"Plant","Mrt4[1]");
	SET_INI_DOUBLE(self->Plant.Mrt4[2],"Plant","Mrt4[2]");
	SET_INI_DOUBLE(self->Plant.Mrt4[3],"Plant","Mrt4[3]");

	SET_INI_DOUBLE(self->Plant.LAI4[0],"Plant","LAI4[0]");
	SET_INI_DOUBLE(self->Plant.LAI4[1],"Plant","LAI4[1]");
	SET_INI_DOUBLE(self->Plant.LAI4[2],"Plant","LAI4[2]");
	SET_INI_DOUBLE(self->Plant.LAI4[3],"Plant","LAI4[3]");

	SET_INI_DOUBLE(self->Plant.MCSrt,"Plant","MCSrt");
	SET_INI_DOUBLE(self->Plant.MCSsh,"Plant","MCSsh");
	SET_INI_DOUBLE(self->Plant.MNSrt,"Plant","MNSrt");
	SET_INI_DOUBLE(self->Plant.MNSsh,"Plant","MNSsh");

	data =  g_key_file_to_data (keyfile,
	                            &length,
	                            &error);

	//g_key_file_unref(keyfile);
	g_key_file_free(keyfile);

	f = fopen(configfile,"w");
	fputs(data,f);
	fclose(f);

	S = g_strdup_printf("%s.change",configfile);

	if (file_exists(S)==0) {
		write_header=1;
	} else {
		write_header=0;
	}

	f = fopen(S,"a");
	if (write_header==1) {
		fprintf(f,"Water.Wrt,Water.Wsh,Plant.MLam4[0],Plant.MLam4[1],Plant.MLam4[2],Plant.MLam4[3],"
		        "Plant.Mss4[0],Plant.Mss4[1],Plant.Mss4[2],Plant.Mss4[3],"
		        "Plant.Mrt4[0],Plant.Mrt4[1],Plant.Mrt4[2],Plant.Mrt4[3],"
		        "Plant.LAI4[0],Plant.LAI4[1],Plant.LAI4[2],Plant.LAI4[3],"
		        "Plant.MCSrt,Plant.MCSsh,Plant.MNSrt,Plant.MNSsh\n");
	}

	fprintf(f,"%f,",self->Water.Wrt);
	fprintf(f,"%f,",self->Water.Wsh);
	fprintf(f,"%f,",self->Plant.MLam4[0]);
	fprintf(f,"%f,",self->Plant.MLam4[1]);
	fprintf(f,"%f,",self->Plant.MLam4[2]);
	fprintf(f,"%f,",self->Plant.MLam4[3]);
	fprintf(f,"%f,",self->Plant.Mss4[0]);
	fprintf(f,"%f,",self->Plant.Mss4[1]);
	fprintf(f,"%f,",self->Plant.Mss4[2]);
	fprintf(f,"%f,",self->Plant.Mss4[3]);
	fprintf(f,"%f,",self->Plant.Mrt4[0]);
	fprintf(f,"%f,",self->Plant.Mrt4[1]);
	fprintf(f,"%f,",self->Plant.Mrt4[2]);
	fprintf(f,"%f,",self->Plant.Mrt4[3]);
	fprintf(f,"%f,",self->Plant.LAI4[0]);
	fprintf(f,"%f,",self->Plant.LAI4[1]);
	fprintf(f,"%f,",self->Plant.LAI4[2]);
	fprintf(f,"%f,",self->Plant.LAI4[3]);
	fprintf(f,"%f,",self->Plant.MCSrt);
	fprintf(f,"%f,",self->Plant.MCSsh);
	fprintf(f,"%f,",self->Plant.MNSrt);
	fprintf(f,"%f",self->Plant.MNSsh);
	fprintf(f,"\n");

	fclose(f);


	return RET_SUCCESS;
}

#ifdef TEST_HPM
int hpm_read_csv_file(hpm *self,char *weather_file_name_st,int min_columns)
{
	int size;
	int date_format;
	int year,month,day;
	char *seperator;
	char *S,*S2,*S3;
	char *date_str;
	FILE *f;
	int i,i2;
	double *values;
	GDate date;
	STRUCT_CDB_DATA* data;
	int iYear;
	double fTimeY;

	g_date_clear(&date,1);

	data=NULL;

	f = fopen(weather_file_name_st ,"r");
	i = 0;
	if (f==NULL) {
		S = g_strdup_printf("Open Weather File %s!",weather_file_name_st);
		PRINT_ERROR(S);
		g_free(S);
		return 1;
	}
	seperator = ",";
	date_format = cdb_format;
	size=0;

	while (1) {
		S = fgets_mem(f);
		if (S==NULL) break;
		if ((i==0) || (i==1)) {
			size = getStringColumn_get_Size(S,seperator);
			if (size < min_columns) {
				seperator=";";
				size = getStringColumn_get_Size(S,seperator);
				if (size < min_columns) {
					seperator="\t";
					size = getStringColumn_get_Size(S,seperator);
					if (size < min_columns) {
						seperator=" ";
						size = getStringColumn_get_Size(S,seperator);
						if (size < min_columns) {
							char *S_ERROR;
							S_ERROR = g_strdup_printf("The weather file %s has the wrong number of columns %d (colon,semicolon,tabulator and space is alowed as seperator)!",weather_file_name_st,min_columns);
							PRINT_ERROR(S_ERROR);
							g_free(S_ERROR);
							return 1;
						}
					}
				}
			}
		}
		S2 = (char*)g_malloc0(strlen(S)+1);
		date_str = (char*)g_malloc0(strlen(S)+1);
		if (i==0) {
			data = (STRUCT_CDB_DATA*)g_malloc0(sizeof(STRUCT_CDB_DATA));
			data->size_of_values = size;
			data->size_of_valuelist = 0;
			data->valuelist = NULL;
			data->iyear = NULL;
			data->fTimeY = NULL;
			data->header = (char**)g_malloc0(sizeof(char*)*size);
			for (i2=0; i2<size; i2++) {
				getStringColumn(S2,S,seperator,i2);
				S3 = (char*)g_malloc0(strlen(S2)+1);
				strcpy(S3,S2);
				data->header[i2] = S3;
			}
		} else {
			if (size!=data->size_of_values) {
				char *S_ERROR;
				S_ERROR = g_strdup_printf("The weather file %s has the wrong number of rows %d in line %d",weather_file_name_st,min_columns,i+1);
				PRINT_ERROR(S_ERROR);
				g_free(S_ERROR);
				break;
			}
			values =(double*)g_malloc0(sizeof(double)*size);
			for (i2=0; i2<size; i2++) {
				getStringColumn(S2,S,seperator,i2);
				if (i2==0) {
					if (i==1) {
						// Datums format checken:
						if (getStringColumn_get_Size(S2,"-") >= 3) {
							date_format=csv_minus;
						} else if (getStringColumn_get_Size(S2,"//") >= 3) {
							date_format=csv_slash;
						} else if (getStringColumn_get_Size(S2,".") >= 3) {
							date_format=csv_german;
						} else {
							date_format=cdb_format;
						}
					}
					switch(date_format) {
					case csv_minus: // yyyy-mm--dd  01.01.2000 == xno format
						S3 = (char*)g_malloc0(strlen(S2)+1);
						strcpy(S3,S2);
						getStringColumn(S2,S3," ",0);
						getStringColumn(date_str,S2,"-",0);
						year=atoi(date_str);
						getStringColumn(date_str,S2,"-",1);
						month=atoi(date_str);
						getStringColumn(date_str,S2,"-",2);
						day=atoi(date_str);
						g_date_set_day(&date,day);
						g_date_set_month(&date,month);
						g_date_set_year(&date,year);
						xpn_time_conv_gdate_to_xpn_time(&date,&iYear, &fTimeY);						
						values[i2] = fTimeY;
						if (getStringColumn(S2,S3," ",1)==0) {
							int hour,minute,second;
							double d;
							getStringColumn(date_str,S2,":",0);
							hour = atoi(date_str);
							getStringColumn(date_str,S2,":",1);
							minute = atoi(date_str);
							getStringColumn(date_str,S2,":",2);
							second = atoi(date_str);
							d = (double)hour/24.0 + (double)minute/(24.0*60.0) + (double)second/(24.0*60.0*60.0);
							fTimeY += d;
						}

						//values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
						values[i2] = fTimeY;
						g_free(S3);
						break;
					case csv_slash: // mm-dd-yyyy 01.01.2000
					    S3 = (char*)g_malloc0(strlen(S2)+1);
						strcpy(S3,S2);
						getStringColumn(S2,S3," ",0);
						getStringColumn(date_str,S2,"//",0);
						month=atoi(date_str);
						getStringColumn(date_str,S2,"//",1);
						day=atoi(date_str);
						getStringColumn(date_str,S2,"//",2);
						year=atoi(date_str);
						g_date_set_day(&date,day);
						g_date_set_month(&date,month);
						g_date_set_year(&date,year);
						xpn_time_conv_gdate_to_xpn_time(&date,&iYear, &fTimeY);
						values[i2] = fTimeY;
						if (getStringColumn(S2,S3," ",1)==0) {
							int hour,minute,second;
							double d;
							getStringColumn(date_str,S2,":",0);
							hour = atoi(date_str);
							getStringColumn(date_str,S2,":",1);
							minute = atoi(date_str);
							getStringColumn(date_str,S2,":",2);
							second = atoi(date_str);
							d = (double)hour/24.0 + (double)minute/(24.0*60.0) + (double)second/(24.0*60.0*60.0);
							fTimeY += d;
						}

						//values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
						values[i2] = fTimeY;
						g_free(S3);
						break;
					case csv_german:// dd.mm.yyyy 01.01.2001
						S3 = (char*)g_malloc0(strlen(S2)+1);
						strcpy(S3,S2);
						getStringColumn(S2,S3," ",0);
						getStringColumn(date_str,S2,".",0);
						day=atoi(date_str);
						getStringColumn(date_str,S2,".",1);
						month=atoi(date_str);
						getStringColumn(date_str,S2,".",2);
						year=atoi(date_str);
						g_date_set_day(&date,day);
						g_date_set_month(&date,month);
						g_date_set_year(&date,year);
						xpn_time_conv_gdate_to_xpn_time(&date,&iYear, &fTimeY);
						if (getStringColumn(S2,S3," ",1)==0) {
							int hour,minute,second;
							double d;
							getStringColumn(date_str,S2,":",0);
							hour = atoi(date_str);
							getStringColumn(date_str,S2,":",1);
							minute = atoi(date_str);
							getStringColumn(date_str,S2,":",2);
							second = atoi(date_str);
							d = (double)hour/24.0 + (double)minute/(24.0*60.0) + (double)second/(24.0*60.0*60.0);
							fTimeY += d;
						}

						//values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
						values[i2] = fTimeY;
						g_free(S3);
						break;
					default:
						values[i2] = atof(S2);
					}

				} else {
					values[i2] = atof(S2);
				}

			}
			data->size_of_valuelist++;
			data->valuelist = (double**)g_realloc(data->valuelist,sizeof(double*)*data->size_of_valuelist);
			data->iyear = (int*)g_realloc(data->iyear,sizeof(int)*data->size_of_valuelist);
			data->fTimeY = (double*)g_realloc(data->fTimeY,sizeof(double)*data->size_of_valuelist);
			data->valuelist[data->size_of_valuelist-1] = values;
			data->iyear[data->size_of_valuelist-1] = iYear;
			data->fTimeY[data->size_of_valuelist-1] = fTimeY;
		}



		g_free(S2);
		g_free(date_str);
		free(S);
		i++;
	}
	fclose(f);

	if (i < 2) return -1;

	self->test_data =data;

	return RET_SUCCESS;
}



double get_var(hpm *self,char *name)
{
	double d;
	int i,i2,name_found;
	char *S_error;
	name_found=-1;
	d=0.0;
	for (i=0; i<self->test_data->size_of_valuelist; i++) {
		if (xpn_time_compare_time(self->test_data->iyear[i],self->test_data->fTimeY[i],self->parent.pTi->pSimTime->iyear,self->parent.pTi->pSimTime->fTimeY)>=0) {
			break;
		}
	}
	
	//printf("i: %d\n",i);

	for (i2=0; i2<self->test_data->size_of_values; i2++) {
		if (strcmp(self->test_data->header[i2],name)==0) {
			name_found=i2;
			break;
		}
	}
	if (name_found==-1) {
		S_error=g_strdup_printf("Name %s not found!",name);
		PRINT_ERROR_ID((&(self->parent)),S_error);
		g_free(S_error);
	} else {
		d = self->test_data->valuelist[i][name_found];
	}
	return d;
}
#endif

int load_parameter(hpm *self,const char* paramfile)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	const char* filename=paramfile;
	expertn_modul_base *xpn = &(self->parent);

	memset(&(self->parameter),0,sizeof(STRUCT_PARAM));

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, paramfile, flags, &error)) {
		PRINT_ERROR(error->message);
		return -1;
	}

	// Photosynthesis
	GET_INI_DOUBLE(self->parameter.photosynthesis.kcan,"photosynthesis","kcan");	
	GET_INI_DOUBLE(self->parameter.photosynthesis.XIleaf,"photosynthesis","xileaf");
	GET_INI_DOUBLE(self->parameter.photosynthesis.ksi_ph,"photosynthesis","ksi_ph");
	GET_INI_DOUBLE(self->parameter.photosynthesis.KCO2_Pmax,"photosynthesis","kco2_pmax");
	GET_INI_DOUBLE(self->parameter.photosynthesis.mu_W,"photosynthesis","mu_w");
	GET_INI_DOUBLE(self->parameter.photosynthesis.Rgas,"photosynthesis","rgas");
	GET_INI_DOUBLE(self->parameter.photosynthesis.Pmax20,"photosynthesis","pmax20");
	GET_INI_DOUBLE(self->parameter.photosynthesis.rmmCS,"photosynthesis","rmmcs");
	GET_INI_DOUBLE(self->parameter.photosynthesis.rmmNS,"photosynthesis","rmmns");
	GET_INI_DOUBLE(self->parameter.photosynthesis.alpham15,"photosynthesis","alpham15");
	GET_INI_DOUBLE(self->parameter.photosynthesis.beta,"photosynthesis","beta");
	GET_INI_DOUBLE(self->parameter.photosynthesis.tau,"photosynthesis","tau");
	GET_INI_DOUBLE(self->parameter.photosynthesis.cTalpha,"photosynthesis","ctalpha");
	GET_INI_DOUBLE(self->parameter.photosynthesis.crleaf,"photosynthesis","crleaf");
//	GET_INI_DOUBLE(self->parameter.photosynthesis.rNetR_PAR,"photosynthesis","rnetr_par");

	GET_INI_DOUBLE(self->parameter.photosynthesis.cpsiPr,"photosynthesis","cpsipr");
	GET_INI_DOUBLE(self->parameter.photosynthesis.epsilon,"photosynthesis","epsilon");
	GET_INI_DOUBLE(self->parameter.photosynthesis.fSos_ac,"photosynthesis","fsos_ac");
	GET_INI_DOUBLE(self->parameter.photosynthesis.rmm_osm,"photosynthesis","rmm_osm");

	// Temperature
	GET_INI_DOUBLE(self->parameter.temperature.TmaxPmax350,"temperature","tmaxpmax350");
	GET_INI_DOUBLE(self->parameter.temperature.TmaxPmax700,"temperature","tmaxpmax700");
	GET_INI_DOUBLE(self->parameter.temperature.qfT,"temperature","qft");
	GET_INI_DOUBLE(self->parameter.temperature.Tref,"temperature","tref");
	GET_INI_DOUBLE(self->parameter.temperature.mfT,"temperature","mft");
	GET_INI_DOUBLE(self->parameter.temperature.T0,"temperature","t0");
	GET_INI_DOUBLE(self->parameter.temperature.T0p,"temperature","t0p");
	GET_INI_DOUBLE(self->parameter.temperature.Tref,"temperature","tref");
	GET_INI_DOUBLE(self->parameter.temperature.qfT,"temperature","qft");
	GET_INI_DOUBLE(self->parameter.temperature.mfT,"temperature","mft");

	// Water
	GET_INI_DOUBLE(self->parameter.water.gsmaxA,"water","gsmaxa");
	GET_INI_DOUBLE(self->parameter.water.gsminA,"water","gsmina");
	GET_INI_DOUBLE(self->parameter.water.cgsCO2,"water","cgsco2");
	GET_INI_DOUBLE(self->parameter.water.theta_gsmin,"water","theta_gsmin");
	GET_INI_DOUBLE(self->parameter.water.theta_gsmax,"water","theta_gsmax");
	GET_INI_DOUBLE(self->parameter.water.thetasoil_max,"water","thetasoil_max");

	GET_INI_DOUBLE(self->parameter.water.cWTpl,"water","cwtpl");
	GET_INI_DOUBLE(self->parameter.water.cWrs_rt,"water","cwrs_rt");
	GET_INI_DOUBLE(self->parameter.water.qpsi_soW,"water","qpsi_sow");
//	GET_INI_DOUBLE(self->parameter.water.Ksomax,"water","ksomax");
	GET_INI_DOUBLE(self->parameter.water.KWrs_rt,"water","kwrs_rt");
	GET_INI_DOUBLE(self->parameter.water.cWso_rs,"water","cwso_rs");
	GET_INI_DOUBLE(self->parameter.water.roWpl20,"water","roWpl20");

	GET_INI_DOUBLE(self->parameter.water.knuK,"water","knuk");
	GET_INI_DOUBLE(self->parameter.water.chcanLAI,"water","chcanlai");

	GET_INI_DOUBLE(self->parameter.water.rho_Wa,"water","rho_wa");

	// Nitrogen:
	GET_INI_DOUBLE(self->parameter.nitrogen.fTuN20,"nitrogen","ftun20");
	GET_INI_DOUBLE(self->parameter.nitrogen.fTuN10,"nitrogen","ftun10");
	GET_INI_DOUBLE(self->parameter.nitrogen.c_uN4[0],"nitrogen","c_un4[0]");
	GET_INI_DOUBLE(self->parameter.nitrogen.c_uN4[1],"nitrogen","c_un4[1]");
	GET_INI_DOUBLE(self->parameter.nitrogen.c_uN4[2],"nitrogen","c_un4[2]");
	GET_INI_DOUBLE(self->parameter.nitrogen.c_uN4[3],"nitrogen","c_un4[3]");
	GET_INI_DOUBLE(self->parameter.nitrogen.KMXrtuN,"nitrogen","kmxrtun");
	GET_INI_DOUBLE(self->parameter.nitrogen.KNeffuN,"nitrogen","kneffun");
	GET_INI_DOUBLE(self->parameter.nitrogen.KCuN,"nitrogen","kcun");
	GET_INI_DOUBLE(self->parameter.nitrogen.JNuN,"nitrogen","jnun");
	GET_INI_DOUBLE(self->parameter.nitrogen.sig_uN20,"nitrogen","sig_un20");
	GET_INI_DOUBLE(self->parameter.nitrogen.qWuN,"nitrogen","qwun");
	GET_INI_DOUBLE(self->parameter.nitrogen.cuNamm,"nitrogen","cunamm");
	GET_INI_DOUBLE(self->parameter.nitrogen.cuNnit,"nitrogen","cunnit");

	GET_INI_DOUBLE(self->parameter.nitrogen.fNshli_re_max,"nitrogen","fnshli_re_max");
	GET_INI_DOUBLE(self->parameter.nitrogen.KNrec,"nitrogen","knrec");
	GET_INI_DOUBLE(self->parameter.nitrogen.fNSshli_min,"nitrogen","fnsshli_min");
	GET_INI_DOUBLE(self->parameter.nitrogen.KNSshli,"nitrogen","knsshli");

	GET_INI_DOUBLE(self->parameter.nitrogen.fNrtli_re_max,"nitrogen","fnrtli_re_max");
	GET_INI_DOUBLE(self->parameter.nitrogen.kNSrt_ex20,"nitrogen","knsrt_ex20");
	GET_INI_DOUBLE(self->parameter.nitrogen.fNSrtli_min,"nitrogen","fnsrtli_min");
	GET_INI_DOUBLE(self->parameter.nitrogen.KNSrtli,"nitrogen","knsrtli");


	// Plant
	GET_INI_DOUBLE(self->parameter.plant.roC20,"plant","roc20");
	GET_INI_DOUBLE(self->parameter.plant.roN20,"plant","ron20");

	GET_INI_DOUBLE(self->parameter.plant.KCmai,"plant","kcmai");

	GET_INI_DOUBLE(self->parameter.plant.kmai_sh120,"plant","kmai_sh120");
	GET_INI_DOUBLE(self->parameter.plant.kmai_sh220,"plant","kmai_sh220");
	GET_INI_DOUBLE(self->parameter.plant.kmai_sh320,"plant","kmai_sh320");
	GET_INI_DOUBLE(self->parameter.plant.kmai_sh420,"plant","kmai_sh420");

	GET_INI_DOUBLE(self->parameter.plant.kmai_rt120,"plant","kmai_rt120");
	GET_INI_DOUBLE(self->parameter.plant.kmai_rt220,"plant","kmai_rt220");
	GET_INI_DOUBLE(self->parameter.plant.kmai_rt320,"plant","kmai_rt320");
	GET_INI_DOUBLE(self->parameter.plant.kmai_rt420,"plant","kmai_rt420");

	GET_INI_DOUBLE(self->parameter.plant.fCplX,"plant","fcplx");
	GET_INI_DOUBLE(self->parameter.plant.fNplX,"plant","fnplx");
	GET_INI_DOUBLE(self->parameter.plant.YGpl,"plant","ygpl");
	GET_INI_DOUBLE(self->parameter.plant.fCSshli_min,"plant","fcsshli_min");
	GET_INI_DOUBLE(self->parameter.plant.KCSshli,"plant","kcsshli");
	GET_INI_DOUBLE(self->parameter.plant.rCNrec_li,"plant","rcnrec_li");

	GET_INI_DOUBLE(self->parameter.plant.kCSrt_ex20,"plant","kcsrt_ex20");
	GET_INI_DOUBLE(self->parameter.plant.fCSrtli_min,"plant","fcsrtli_min");
	GET_INI_DOUBLE(self->parameter.plant.KCSrtli,"plant","kcsrtli");

	GET_INI_DOUBLE(self->parameter.plant.cSLAmax,"plant","cslamax");
	GET_INI_DOUBLE(self->parameter.plant.cSLA_C,"plant","csla_c");
	GET_INI_DOUBLE(self->parameter.plant.cSLA_W,"plant","csla_w");
	GET_INI_DOUBLE(self->parameter.plant.flam,"plant","flam");

	GET_INI_DOUBLE(self->parameter.plant.qWpl,"plant","qwpl");
	GET_INI_DOUBLE(self->parameter.plant.mu20,"plant","mu20");
	GET_INI_DOUBLE(self->parameter.plant.kdegsh20,"plant","kdegsh20");
	GET_INI_DOUBLE(self->parameter.plant.kturnss20,"plant","kturnss20");
	GET_INI_DOUBLE(self->parameter.plant.kturnlam20,"plant","kturnlam20");
	GET_INI_DOUBLE(self->parameter.plant.kturnrt20,"plant","kturnrt20");
	GET_INI_DOUBLE(self->parameter.plant.kdegrt20,"plant","kdegrt20");

	GET_INI_DOUBLE(self->parameter.plant.depthrtmin,"plant","depthrtmin");
	GET_INI_DOUBLE(self->parameter.plant.rortmax,"plant","rortmax");
	GET_INI_DOUBLE(self->parameter.plant.rortmin,"plant","rortmin");


	xpn->pPl->pCanopy->fkcan=self->parameter.photosynthesis.kcan;

	g_key_file_free(keyfile);

	return RET_SUCCESS;
}

int register_var(hpm *self)
{

	//Plant:
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MCSrt,"hpm.plant.MCSrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MNSrt,"hpm.plant.MNSrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MCSsh,"hpm.plant.MCSsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMCSsh,"hpm.plant.DMCSsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMNSsh,"hpm.plant.DMNSsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMCSrt,"hpm.plant.DMCSrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMNSrt,"hpm.plant.DMNSrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.gsmax,"hpm.plant.gsmax",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MLam4[0],"hpm.plant.MLam4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MLam4[1],"hpm.plant.MLam4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MLam4[2],"hpm.plant.MLam4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MLam4[3],"hpm.plant.MLam4[3]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MXLam,"hpm.plant.MXLam",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMLam4[0],"hpm.plant.DMLam4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMLam4[1],"hpm.plant.DMLam4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMLam4[2],"hpm.plant.DMLam4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMLam4[3],"hpm.plant.DMLam4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mss4[0],"hpm.plant.Mss4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mss4[1],"hpm.plant.Mss4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mss4[2],"hpm.plant.Mss4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mss4[3],"hpm.plant.Mss4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMss4[0],"hpm.plant.DMss4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMss4[1],"hpm.plant.DMss4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMss4[2],"hpm.plant.DMss4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMss4[3],"hpm.plant.DMss4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MXss,"hpm.plant.MXss",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mrt4[0],"hpm.plant.Mrt4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mrt4[1],"hpm.plant.Mrt4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mrt4[2],"hpm.plant.Mrt4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.Mrt4[3],"hpm.plant.Mrt4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MXrt,"hpm.plant.MXrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMrt4[0],"hpm.plant.DMrt4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMrt4[1],"hpm.plant.DMrt4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMrt4[2],"hpm.plant.DMrt4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMrt4[3],"hpm.plant.DMrt4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[0],"hpm.plant.LAI4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[1],"hpm.plant.LAI4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[2],"hpm.plant.LAI4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[3],"hpm.plant.LAI4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DLAI4[0],"hpm.plant.DLAI4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DLAI4[1],"hpm.plant.DLAI4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DLAI4[2],"hpm.plant.DLAI4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DLAI4[3],"hpm.plant.DLAI4[3]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MNph_ul,"hpm.plant.MNph_ul",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMNph_ul,"hpm.plant.DMNph_ul",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MCTsh,"hpm.plant.MCTsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MCTrt,"hpm.plant.MCTrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMCTsh,"hpm.plant.DMCTsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.DMCTrt,"hpm.plant.DMCTrt",0.0);


	//Water:
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.aWrt,"hpm.water.aWrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.aWsh,"hpm.water.aWsh",0.0);


	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.RH,"hpm.water.RH",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.ICSsh_ph,"hpm.water.ICSsh_ph",0.0);

	//Soil:
	//vars->Nnit
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant->MNSsh ICSsh_ph,"hpm.water.ICSsh_ph",0.0);

	//Output:
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.MPl,"output.hpm.complete mass.MPl",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.MSPl,"output.hpm.substrate mass.MSPl",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.MSrt,"output.hpm.substrate mass.MSrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.MSsh,"output.hpm.substrate mass.MSsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.MXPl,"output.hpm.structural mass.MXPl",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.MXrt,"output.hpm.structural mass.MXrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.MXsh,"output.hpm.structural mass.MXsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Mrt,"output.hpm.complete mass.Mrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Msh,"output.hpm.complete mass.Msh",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Namm,"output.hpm.nitrogen.Namm",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Nnit,"output.hpm.nitrogen.Nnit",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.theta_soil,"output.hpm.water content.theta_soil",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.theta_sh,"output.hpm.water content.theta_sh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.theta_rt,"output.hpm.water content.theta_rt",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.psi_sh,"output.hpm.water potential.psi_sh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.psi_rt,"output.hpm.water potential.psi_rt",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.Wrt,"output.hpm.water mass.Wrt [kg water m-2 ground]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Water.Wsh,"output.hpm.water mass.Wsh [kg water m-2 ground]",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Grt,"output.hpm.growth func.Grt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Gsh,"output.hpm.growth func.Gsh",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.fTsh,"output.hpm.growth func.fTsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.fWsh,"output.hpm.growth func.fWsh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Csh,"output.hpm.growth func.Csh",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Nsh,"output.hpm.growth func.Nsh",0.0);

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.fTrt,"output.hpm.growth func.fTrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.fWrt,"output.hpm.growth func.fWrt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Crt,"output.hpm.growth func.Crt",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.Nrt,"output.hpm.growth func.Nrt",0.0);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[0],"output.hpm.lai.LAI4[0]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[1],"output.hpm.lai.LAI4[1]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[2],"output.hpm.lai.LAI4[2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Plant.LAI4[3],"output.hpm.lai.LAI4[3]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.LAI,"output.hpm.lai.LAI",0.0);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.hcan,"output.hpm.height.hcan [m]",0.0);

	return RET_SUCCESS;
}
