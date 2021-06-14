
#ifdef USE_MPI
#define MPI_INCLUDED
#include <mpi.h>
#include <pnetcdf.h>
#endif

#ifndef USE_MPI
#include <netcdf.h>
#endif


#include "xpn_output_netcdf.h"



#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#ifndef S_IRWXU
#define S_IRWXU 1
#endif



G_DEFINE_TYPE(xpn_output_netcdf, xpn_output_netcdf, G_TYPE_OBJECT)

static void xpn_output_netcdf_class_init(xpn_output_netcdfClass *klass) {}
static void xpn_output_netcdf_init(xpn_output_netcdf *self) {}

#ifdef USE_MPI
#define wrap_nc_def_dim(ncid,name,size_t,dimidp) ncmpi_def_dim(ncid,name,size_t,dimidp)
#define wrap_nc_def_var(ncid,name,xtype,ndims,dimids,varidp) ncmpi_def_var(ncid,name,xtype,ndims,dimids,varidp)
#define wrap_nc_enddef(ncid) ncmpi_enddef(ncid)
#define wrap_nc_put_vara_text(ncid,varid,start,count,tp) ncmpi_put_vara_text(ncid,varid,start,count,tp)
#define wrap_nc_put_var1_double(ncid,varid,index,dp) ncmpi_put_vara_double_all(ncid,varid,index,var_count,dp)
#define wrap_nc_put_var1_int(ncid,varid,index,dp) ncmpi_put_vara_int_all(ncid,varid,index,var_count,dp)
#define wrap_nc_put_var1_float(ncid,varid,index,dp) ncmpi_put_vara_float_all(ncid,varid,index,var_count,dp)
#define wrap_nc_sync(ncid) ncmpi_sync(ncid)
#else
#define wrap_nc_def_dim(ncid,name,size_t,dimidp) nc_def_dim(ncid,name,size_t,dimidp)
#define wrap_nc_def_var(ncid,name,xtype,ndims,dimids,varidp) nc_def_var(ncid,name,xtype,ndims,dimids,varidp)
#define wrap_nc_enddef(ncid) nc_enddef(ncid)
#define wrap_nc_put_vara_text(ncid,varid,start,count,tp) nc_put_vara_text(ncid,varid,start,count,tp)
#define wrap_nc_put_var1_double(ncid,varid,index,dp) nc_put_var1_double(ncid,varid,index,dp)
#define wrap_nc_put_var1_int(ncid,varid,index,dp) nc_put_var1_int(ncid,varid,index,dp)
#define wrap_nc_put_var1_float(ncid,varid,index,dp) nc_put_var1_float(ncid,varid,index,dp)
#define wrap_nc_sync(ncid) nc_sync(ncid)
#endif




#define NC_CHECK_ERROR(command)\
	{\
		char *S_ERROR;\
		int err;\
		err=command;\
		if (err != NC_NOERR)\
			{\
				S_ERROR = g_strdup_printf("command %s, Error ID: %d",#command,err);\
				PRINT_ERROR(S_ERROR);\
				g_free(S_ERROR);\
			}\
	}\
	 
#define NC_CHECK_ERROR2(command,output)\
	{\
		char *S_ERROR;\
		int err;\
		err=command;\
		if (err != NC_NOERR)\
			{\
				S_ERROR = g_strdup_printf("command %s, Error ID: %d, String: %s",#command,err,output);\
				PRINT_ERROR(S_ERROR);\
				g_free(S_ERROR);\
			}\
	}\
	 


static char* create_string_from_source(char* src)
{
	char *S;
	if (src==NULL) return NULL;
	S = (char*) g_malloc0(strlen(src)+1);
	strcpy(S,src);
	return S;

}


// char TimeStr[20]
static void write_TimeStr(char TimeStr[],STRUCT_XPN_TIME *xno_time)
{
	// year
	sprintf(&((TimeStr)[0]),"%d",xno_time->year);
	(TimeStr)[4] = '-';
	if (xno_time->mon<10)
		{
			sprintf(&((TimeStr)[5]),"0%d",xno_time->mon);
		}
	else
		{
			sprintf(&((TimeStr)[5]),"%d",xno_time->mon);
		}
	(TimeStr)[7] = '-';
	if (xno_time->mday <10)
		{
			sprintf(&((TimeStr)[8]),"0%d",xno_time->mday);
		}
	else
		{
			sprintf(&((TimeStr)[8]),"%d",xno_time->mday);
		}
	(TimeStr)[10] = '_';
	if (xno_time->hour <10)
		{
			sprintf(&((TimeStr)[11]),"0%d",xno_time->hour);
		}
	else
		{
			sprintf(&((TimeStr)[11]),"%d",xno_time->hour);
		}
	(TimeStr)[13] = ':';
	if (xno_time->min <10)
		{
			sprintf(&((TimeStr)[14]),"0%d",xno_time->min);
		}
	else
		{
			sprintf(&((TimeStr)[14]),"%d",xno_time->min);
		}
	(TimeStr)[16] = ':';
	if (xno_time->sec <10)
		{
			sprintf(&((TimeStr)[17]),"0%d",xno_time->sec);
		}
	else
		{
			sprintf(&((TimeStr)[17]),"%d",xno_time->sec);
		}
	(TimeStr[19]) = 0;
}



xpn_output_netcdf *xpn_output_netcdf_new(expertnclass ** classes,int classes_len,STRUCT_Config_Value* config_values,int config_values_len,general_config* cfg)
{

	xpn_output_netcdf *self;
	int i,i2;
	int activated;
	int *id_list;
	int id_list_size;
	char *S;






	// reserve Memory:
	self = g_object_new(TYPE_XPN_OUTPUT_NETCDF, NULL);

	// Init MPI Vars:
	self->loop_min=0;
	self->loop_max=classes_len;

#ifdef USE_MPI
	self->numtasks=1;
	self->rank=0;
#endif

	self->xpn_class = classes;
	self->xpn_class_len = classes_len;
	self->cfg = cfg;


#ifdef USE_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &(self->rank));
	MPI_Comm_size(MPI_COMM_WORLD, &(self->numtasks));
	if (self->numtasks>classes_len)
		{
			self->numtasks=classes_len;
		}


	self->loop_min    =(int)((long)(self->rank + 0) *
	                         (long)(classes_len)/(long)self->numtasks);
	self->loop_max    =(int)((long)(self->rank + 1) *
	                         (long)(classes_len)/(long)self->numtasks);

	/*
	printf("---------------------------------------------------------------------------------------------\n");
	printf("rank: %d\n",self->rank);
	printf("numtasks: %d\n",self->numtasks);

	printf("loop_min: %d\n",self->loop_min);
	printf("loop_max: %d\n",self->loop_max);
	printf("---------------------------------------------------------------------------------------------\n");
	*/
#endif



	for (i=0; i<config_values_len; i++)
		{
			if (strcmp(config_values[i].key,"Global_Config.options.output netcdf path")==0)
				{
					self->netcdf_path=g_strdup_printf("%s",config_values[i].value);
					break;
				}
		}
	if (self->netcdf_path==NULL)
		{
			self->netcdf_path=g_strdup_printf("$PROJECT_PATH/output");
		}


	// Check if NETCDF Output activated (standard: not activated):
	activated=0;
	for (i=0; i<config_values_len; i++)
		{
			if (strcmp(config_values[i].key,"Global_Config.options.no output netcdf file")==0)
				{
					if (atoi(config_values[i].value)==0)
						{
							activated=1;
						}
					break;
				}
		}


	// das auskommentieren sollte nichts bewirken:
	/*
	if (activated==0)
		{
			if(self->netcdf_path!=NULL)
				{
					g_free(self->netcdf_path);
				}
			g_object_unref(self);
			return NULL;
		}
	*/

	self->activated = activated;

	self->always_dump_nc_file = 1;
	for (i=0; i<config_values_len; i++)
		{
			if (strcmp(config_values[i].key,"Global_Config.options.always dump nc file")==0)
				{
					self->always_dump_nc_file=atoi(config_values[i].value);
				}
		}



#ifdef USE_MPI
	self->NETCDF_FORMAT=NC_CLOBBER|NC_64BIT_OFFSET;//NC_NETCDF4|NC_MPIIO;//NC_64BIT_OFFSET|NC_MPIIO;
#else
	self->NETCDF_FORMAT=NC_64BIT_OFFSET;
#endif
	//self->NETCDF_FORMAT=NC_NETCDF4;

	self->nests = 0;
	id_list = general_config_get_grid_id_list(cfg,&(id_list_size));// liste der nest ids

	self->nests_len = id_list_size;
	self->nests = (STRUCT_NESTS*) g_malloc0(sizeof(STRUCT_NESTS)*self->nests_len);


	for (i=0; i<self->nests_len; i++)
		{
			self->nests[i].output_time_step=0.0;
			S=NULL;
			for (i2=0; i2<config_values_len; i2++)
				{
					if (strcmp(config_values[i2].key,"Global_Config.options.output time step")==0)
						{
							//self->nests[i].output_time_step = atof(config_values[i2].value);
							S = config_values[i2].value;
							break;
						}
				}
			if (S==NULL)
				{
					S = "1.0";
				}
			output_time_step_check_init(&(self->nests[i].output_time_step),&(self->nests[i].output_time_step_string),S);
			
			S=NULL;
			for (i2=0; i2<config_values_len; i2++)
				{
					if (strcmp(config_values[i2].key,"Global_Config.options.output time span")==0)
						{
							//self->nests[i].output_time_step = atof(config_values[i2].value);
							S = config_values[i2].value;
							break;
						}
				}
				
			expertn_modul_base_init_timespan(&(self->nests[i].timespan),S);
			

			self->nests[i].xpn_time_old.msec = 0.0;
			self->nests[i].xpn_time_old.sec = 0;
			self->nests[i].xpn_time_old.min = 0;
			self->nests[i].xpn_time_old.hour = 0;
			self->nests[i].xpn_time_old.mday = 1;
			self->nests[i].xpn_time_old.mon = 1;
			self->nests[i].xpn_time_old.year = 0;
			self->nests[i].xpn_time_old.is_leap_year = 0;

			self->nests[i].xno_time.msec = 0.0;
			self->nests[i].xno_time.sec = 0;
			self->nests[i].xno_time.min = 0;
			self->nests[i].xno_time.hour = 0;
			self->nests[i].xno_time.mday = 1;
			self->nests[i].xno_time.mon = 1;
			self->nests[i].xno_time.year = 0;
			self->nests[i].xno_time.is_leap_year = 0;



			self->nests[i].count=-1;


			self->nests[i].nest_id= id_list[i];
			general_config_get_grid_size_ij(cfg,&(self->nests[i].i1),&(self->nests[i].j1),&(self->nests[i].i2),&(self->nests[i].j2),self->nests[i].nest_id);
			self->nests[i].instance_size = general_config_get_instance_max_size(cfg,id_list[i]);
			self->nests[i].netf=-1;
			self->nests[i].var_list=NULL;
			self->nests[i].var_list_len=0;
			self->nests[i].netcdf_filename=NULL;
		}
	g_free(id_list);


	return self;
}

xpn_output_netcdf * xpn_output_netcdf_Done(xpn_output_netcdf *self)
{
	int i,i2;
	if (self==NULL) return NULL;
	for (i=0; i<self->nests_len; i++)
		{
			if (self->nests[i].netf > -1)
				{
#ifdef USE_MPI
					NC_CHECK_ERROR(ncmpi_close(self->nests[i].netf));
#else
					NC_CHECK_ERROR(nc_close(self->nests[i].netf));
#endif
					self->nests[i].netf=-1;
				}
			if (self->nests[i].netcdf_filename!=NULL)
				{
					g_free(self->nests[i].netcdf_filename);
					self->nests[i].netcdf_filename=NULL;
				}
			if (self->nests[i].var_list!=NULL)
				{
					for (i2=0; i2<self->nests[i].var_list_len; i2++)
						{
							if (self->nests[i].var_list[i2].reg_var!=NULL)
								g_free(self->nests[i].var_list[i2].reg_var);
							g_free(self->nests[i].var_list[i2].name);
						}
					g_free(self->nests[i].var_list);
				}

			if (self->nests[i].output_time_step_string!=NULL) g_free(self->nests[i].output_time_step_string);
			self->nests[i].output_time_step_string=NULL;
            
            expertn_modul_base_time_span_free(&(self->nests[i].timespan));
		}


	if (self->netcdf_path!=NULL) g_free(self->netcdf_path);
	if (self->nests!=0) g_free(self->nests);
	self->nests=NULL;
	self->nests_len=0;

	g_object_unref(self);
	return NULL;
}



// creates: self->nests[nest].var_list and self->nests[nest].var_list_len
static void xpn_output_netcdf_create_var_list(xpn_output_netcdf *self,int nest)
{
	int i,i2,i3,i4,i5;
	int not_in_list;
	int time_pointer_written;
	expertnclass *xpn_class;
	xpn_register_var *var_list;
	STRUCT_XPN_TIME xpn_time;


	self->nests[nest].var_list=NULL;
	self->nests[nest].var_list_len=0;
	time_pointer_written=0;
	i3=0;
	for (i=0; i<self->cfg->grid_len; i++)
		{
			for (i2=0; i2<self->cfg->grid[i].instance_len; i2++)
				{
					/*if (!((i3 >= self->loop_min) && (i3 < self->loop_max)))
						{
							i3++;
							continue;
						}*/
					xpn_class = self->xpn_class[i3];
					i3++;
					if (xpn_class==NULL)
						{
							continue;
						}
					if (self->cfg->grid[i].id!=self->nests[nest].nest_id) continue;
					if (time_pointer_written==0)
						{
							xpn_time_get_struc(&xpn_time,self->xpn_class[i3-1]->pTi->pSimTime->iyear,self->xpn_class[i3-1]->pTi->pSimTime->fTimeY);
							self->nests[nest].ftime = xpn_time_conv_time_systime(&xpn_time);//&(self->xpn_class[i3-1]->pTi->pSimTime->fTime);
							self->nests[nest].first_xpn_instance_for_domain = i3-1;
							time_pointer_written=1;
						}
					var_list = xpn_class->pXSys->var_list;
					for (i4=0; i4!=var_list->size; i4++)
						{
							if ((var_list->vars[i4]->flag_record==TRUE) && (var_list->vars[i4]->flag_pointer==TRUE))
								{
									not_in_list=1;
									for (i5=0; i5<self->nests[nest].var_list_len; i5++)
										{
											if (strcmp(var_list->vars[i4]->varname,self->nests[nest].var_list[i5].name)==0)
												{
													self->nests[nest].var_list[i5].reg_var_len++;
													self->nests[nest].var_list[i5].reg_var=(STRUCT_REG_VAR*)g_realloc(self->nests[nest].var_list[i5].reg_var,sizeof(STRUCT_REG_VAR)*self->nests[nest].var_list[i5].reg_var_len);
													self->nests[nest].var_list[i5].reg_var[self->nests[nest].var_list[i5].reg_var_len-1].xpn_class=i3-1;
													self->nests[nest].var_list[i5].reg_var[self->nests[nest].var_list[i5].reg_var_len-1].var_number=i4;
													self->nests[nest].var_list[i5].reg_var[self->nests[nest].var_list[i5].reg_var_len-1].i=self->cfg->grid[i].i;
													self->nests[nest].var_list[i5].reg_var[self->nests[nest].var_list[i5].reg_var_len-1].j=self->cfg->grid[i].j;
													self->nests[nest].var_list[i5].reg_var[self->nests[nest].var_list[i5].reg_var_len-1].k=i2;
													not_in_list=0;
													break;
												}
										}
									if (not_in_list==1)
										{
											self->nests[nest].var_list_len++;
											self->nests[nest].var_list = (STRUCT_XPN_VARLIST*)g_realloc (self->nests[nest].var_list,sizeof(STRUCT_XPN_VARLIST)*self->nests[nest].var_list_len);
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].name = create_string_from_source(var_list->vars[i4]->varname);
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].netcdf_id=0;
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var_len=1;
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var=(STRUCT_REG_VAR*)g_malloc0(sizeof(STRUCT_REG_VAR)*self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var_len);
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var[0].xpn_class=i3-1;
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var[0].var_number=i4;
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var[0].i=self->cfg->grid[i].i;
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var[0].j=self->cfg->grid[i].j;
											self->nests[nest].var_list[self->nests[nest].var_list_len-1].reg_var[0].k=i2;
										}

								}
						}


				}

		}
}

// creates: self->nests[nest].netcdf_filename
static void xpn_output_netcdf_create_var_name(xpn_output_netcdf *self,int nest)
{
	char *path;
	expertn_modul_base *base;
	// Pfad erzeugen, falls er nicht existiert
	base = self->xpn_class[self->nests[nest].var_list[0].reg_var[0].xpn_class]->XPN_Moduls[0] ->xpn;
	path=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(base,self->netcdf_path);
	g_mkdir_with_parents(path,S_IRWXU);
#ifdef USE_MPI
	self->nests[nest].netcdf_filename = g_strdup_printf("%s/%s_%d_output_%d.nc",path,base->pXSys->project_name,self->nests[nest].nest_id,self->rank);
#else
	self->nests[nest].netcdf_filename = g_strdup_printf("%s/%s_%d_output.nc",path,base->pXSys->project_name,self->nests[nest].nest_id);
#endif
	free(path);
}

#ifdef USE_MPI

/*
typedef struct
{
	char* name;
	int netcdf_id;
	STRUCT_REG_VAR *reg_var;
	int reg_var_len;
} STRUCT_XPN_VARLIST;
 */
enum MPI_IDS {MPI_ID_VAR_LIST_LEN=__LINE__, MPI_ID_VARLIST};

static void xpn_output_MPI_SEND_STRUCT_XPN_VARLIST(STRUCT_XPN_VARLIST* var,MPI_Comm comm)
{
	int len_name;
	int i;
	//printf("%s\n",var->name);
	len_name = strlen(var->name)+1;
	// strlänge senden:
	MPI_Send(&len_name,1, MPI_INT, 0, MPI_ID_VARLIST,comm);
	// str senden
	MPI_Send(var->name,len_name, MPI_CHAR, 0, MPI_ID_VARLIST+1,comm);
	// reg_var_len:
	MPI_Send(&(var->reg_var_len),1, MPI_INT, 0, MPI_ID_VARLIST+2,comm);
	// reg_var:
	for (i=0; i<var->reg_var_len; i++)
		{
			MPI_Send(&(var->reg_var[i]),sizeof(STRUCT_REG_VAR), MPI_BYTE, 0, MPI_ID_VARLIST+3+i,comm);
		}
}

static void xpn_output_MPI_REC_STRUCT_XPN_VARLIST(STRUCT_XPN_VARLIST* var,int source_rank,MPI_Comm comm)
{
	int len_name;
	int i;
	MPI_Status stat;



	// strlänge:
	MPI_Recv(&len_name, 1, MPI_INT, source_rank, MPI_ID_VARLIST, comm,&stat);
	// str
	var->name = g_malloc0(len_name);
	//var->name = g_malloc0(200);

	MPI_Recv(var->name,len_name, MPI_CHAR, source_rank, MPI_ID_VARLIST+1,comm,&stat);
	//printf("rec %d %s\n",source_rank,var->name);
	//reg_var_len:
	MPI_Recv(&(var->reg_var_len),1, MPI_INT, source_rank, MPI_ID_VARLIST+2,comm,&stat);
	// reg_var:
	var->reg_var = g_malloc0(sizeof(STRUCT_REG_VAR)*var->reg_var_len);
	for (i=0; i<var->reg_var_len; i++)
		{
			MPI_Recv(&(var->reg_var[i]),sizeof(STRUCT_REG_VAR), MPI_BYTE, source_rank, MPI_ID_VARLIST+3+i,comm,&stat);
		}

}
#endif

void xpn_output_netcdf_write_data_matrix(xpn_output_netcdf *self,int grid_id)
{
	STRUCT_XPN_TIME xpn_time;
	int nest;
	expertnclass *xpn_class;
	xpn_register_var *var_list;
	struct_variable *var;
	char *S_ERROR,*S;
#ifdef USE_MPI
	MPI_Offset times_index[1];
	MPI_Offset times_count[1];
	MPI_Offset var_index[4];
	MPI_Offset var_count[4];
#else
	size_t times_index[1];
	size_t var_index[4];
#endif
	double time_value;
	int i,i2,i3;
	char TimeStr[20];
#ifdef USE_MPI
	MPI_Offset Times_Start[2];
	MPI_Offset Times_Count[2];
#else
	size_t Times_Start[2];
	size_t Times_Count[2];
#endif


	if (self==NULL) return;




	// Herausfinden, ob Expert-N für die Domain grid_id definiert ist:
	nest = -1;
	for (i=0; i<self->nests_len; i++)
		{
			if (grid_id == self->nests[i].nest_id)
				{
					nest = i;
					break;
				}
		}

	// Falls kein Expert-N für die Domain konfiguriert ist--> Exit
	if (nest==-1)
		{
			return;
		}

	self->nests[nest].other_ranks_vars=NULL;
	self->nests[nest].other_ranks_vars_len=0;

	// Initialisieren:
	if (self->nests[nest].count==-1)
		{
			// Liste der Vars erstellen (self->nests[nest].var_list and self->nests[nest].var_list_len):
			xpn_output_netcdf_create_var_list(self,nest);

			// MPI VarListe austauschen und evtl. ergänzen:
#ifdef USE_MPI



			/*if (self->rank==0)
				{
					STRUCT_XPN_VARLIST var;
					MPI_Status stat;
					self->nests[nest].other_ranks_vars_len = self->numtasks;
					self->nests[nest].other_ranks_vars = g_malloc0(sizeof(STRUCT_OTHER_RANK_VARS)*self->nests[nest].other_ranks_vars_len);
					self->nests[nest].other_ranks_vars[0].var_list = self->nests[nest].var_list;
					self->nests[nest].other_ranks_vars[0].var_list_len = self->nests[nest].var_list_len;
					for (i=1;i<self->numtasks;i++)
						{
							// länge:
							MPI_Recv(&(self->nests[nest].other_ranks_vars[i].var_list_len), 1, MPI_INT, i, MPI_ID_VAR_LIST_LEN, MPI_COMM_WORLD,&stat);
							self->nests[nest].other_ranks_vars[i].var_list = g_malloc0(sizeof(STRUCT_XPN_VARLIST)*self->nests[nest].other_ranks_vars[i].var_list_len);
							for (i2=0;i2<self->nests[nest].other_ranks_vars[i].var_list_len;i2++)
							//for (i2=0;i2<1;i2++)
								{
									xpn_output_MPI_REC_STRUCT_XPN_VARLIST(&(self->nests[nest].other_ranks_vars[i].var_list[i2]),i,MPI_COMM_WORLD);
									//xpn_output_MPI_REC_STRUCT_XPN_VARLIST(&var,i,MPI_COMM_WORLD);
								}

						}
					for (i=0;i<self->numtasks;i++)
					{
						printf("Länge: %d %d\n",i,self->nests[nest].other_ranks_vars[i].var_list_len);
					}
				} else
				{
					// länge:
					printf("Send: Länge: %d %d\n",self->rank,self->nests[nest].var_list_len);
					MPI_Send(&(self->nests[nest].var_list_len), 1, MPI_INT, 0, MPI_ID_VAR_LIST_LEN, MPI_COMM_WORLD);
					for (i2=0;i2<self->nests[nest].var_list_len;i2++)
					//for (i2=0;i2<1;i2++)
								{
									xpn_output_MPI_SEND_STRUCT_XPN_VARLIST(&(self->nests[nest].var_list[i2]),MPI_COMM_WORLD);
								}

				}*/


			//self->nests[nest].count++;
#endif


			if (self->activated==1)
				{
					// Filename var erzeugen
					xpn_output_netcdf_create_var_name(self,nest);


					// Datei anlegen:
#ifdef USE_MPI
					NC_CHECK_ERROR(ncmpi_create(MPI_COMM_SELF,self->nests[nest].netcdf_filename,self->NETCDF_FORMAT,MPI_INFO_NULL,&(self->nests[nest].netf)));
#else
					NC_CHECK_ERROR(nc_create(self->nests[nest].netcdf_filename,self->NETCDF_FORMAT,&(self->nests[nest].netf)));
#endif

					// Zeit Dimension erzeugen (unlimited):
					NC_CHECK_ERROR(wrap_nc_def_dim(self->nests[nest].netf,"time", NC_UNLIMITED, &(self->nests[nest].time_dim)));

					// Dimension für Times output string
					NC_CHECK_ERROR(wrap_nc_def_dim(self->nests[nest].netf,"TimeStrLen", 19, &(self->nests[nest].TimeStrLen_dim)));



					// Id Dimension erzeugen
					//NC_CHECK_ERROR(wrap_nc_def_dim(self->netf,"ID", self->nests_size, &(self->nested_grid_dim)));

					// i,j Dimension erzeugen
					NC_CHECK_ERROR(wrap_nc_def_dim(self->nests[nest].netf,"i", self->nests[nest].i2+1, &(self->nests[nest].i_dim)));
					NC_CHECK_ERROR(wrap_nc_def_dim(self->nests[nest].netf,"j", self->nests[nest].j2+1, &(self->nests[nest].j_dim)));

					// k Dimension erzeugen:
					NC_CHECK_ERROR(wrap_nc_def_dim(self->nests[nest].netf,"k", self->nests[nest].instance_size, &(self->nests[nest].k_dim)));


					// Variable Times (string) erzeugen:
					self->nests[nest].Times_dims[0] = self->nests[nest].time_dim;
					self->nests[nest].Times_dims[1] = self->nests[nest].TimeStrLen_dim;

					NC_CHECK_ERROR(wrap_nc_def_var (self->nests[nest].netf, "Times", NC_CHAR,
					                                2, self->nests[nest].Times_dims, &(self->nests[nest].Times_var_id)));


					//NC_CHECK_ERROR(nc_var_par_access(self->nests[nest].netf, self->nests[nest].Times_var_id, NC_COLLECTIVE));
					// Variable time erzeugen:
					self->nests[nest].time_dims[0] = self->nests[nest].time_dim;

					NC_CHECK_ERROR(wrap_nc_def_var (self->nests[nest].netf, "time", NC_DOUBLE,
					                                1, self->nests[nest].time_dims, &(self->nests[nest].time_var_id)));

					self->nests[nest].var_dims[0] = self->nests[nest].time_dim;
					self->nests[nest].var_dims[1] = self->nests[nest].k_dim;
					self->nests[nest].var_dims[2] = self->nests[nest].j_dim;
					self->nests[nest].var_dims[3] = self->nests[nest].i_dim;


					// alle anderen Variablen erzeugen:

					// xlat,xlon anlegen:
					NC_CHECK_ERROR2(wrap_nc_def_var (self->nests[nest].netf,"XLAT", NC_DOUBLE,4, self->nests[nest].var_dims, &(self->nests[nest].XLAT.netcdf_id)),"XLAT");
					NC_CHECK_ERROR2(wrap_nc_def_var (self->nests[nest].netf,"XLONG", NC_DOUBLE,4, self->nests[nest].var_dims, &(self->nests[nest].XLON.netcdf_id)),"XLAT");



					for (i=0; i<self->nests[nest].var_list_len; i++)
						{
							// Zuerst ungueltige zeichen ersetzen:
							S = replace_str_with_template(self->nests[nest].var_list[i].name,"/"," ");
							xpn_class = self->xpn_class[self->nests[nest].var_list[i].reg_var[0].xpn_class];
							var_list = xpn_class->pXSys->var_list;
							var = var_list->vars[self->nests[nest].var_list[i].reg_var[0].var_number];
							switch(var->vartype)
								{
								case G_TYPE_INT:
									NC_CHECK_ERROR2(wrap_nc_def_var (self->nests[nest].netf, S, NC_INT,
									                                 4, self->nests[nest].var_dims, &(self->nests[nest].var_list[i].netcdf_id)),S);
									break;
								case G_TYPE_DOUBLE:
									NC_CHECK_ERROR2(wrap_nc_def_var (self->nests[nest].netf, S, NC_DOUBLE,
									                                 4, self->nests[nest].var_dims, &(self->nests[nest].var_list[i].netcdf_id)),S);

									break;
								case G_TYPE_FLOAT:
									NC_CHECK_ERROR2(wrap_nc_def_var (self->nests[nest].netf, S, NC_FLOAT,
									                                 4, self->nests[nest].var_dims, &(self->nests[nest].var_list[i].netcdf_id)),S);

									break;
								default :
									S_ERROR = g_strdup_printf("Writing Variable in NET_CDF File: %s",var->varname);
									PRINT_ERROR(S_ERROR);
									g_free(S_ERROR);

								}



							free(S);
						} // for i
					NC_CHECK_ERROR(wrap_nc_enddef(self->nests[nest].netf));
#ifdef USE_MPI
					if (self->rank==0)
						{
#endif
							fprintf(stdout,"<NETCDFOUTPUT: '%s'>\n",self->nests[nest].netcdf_filename);
#ifdef USE_MPI
						}
#endif
					fflush(stdout);

				}
			self->nests[nest].count++;
		}

//} // if (self->count==0) (init)


// global modell wird ausgeführt

	expertnclass_run_global(self->xpn_class[self->nests[nest].first_xpn_instance_for_domain]);


// i3 auf die 0. Instance jedes Nests gesetzt werden
	i3 = self->nests[nest].first_xpn_instance_for_domain+1;
	xpn_time_get_struc(&xpn_time,self->xpn_class[i3-1]->pTi->pSimTime->iyear,self->xpn_class[i3-1]->pTi->pSimTime->fTimeY);
	self->nests[nest].ftime = xpn_time_conv_time_systime(&xpn_time);//&(self->xpn_class[i3-1]->pTi->pSimTime->fTime);


	if (self->activated==0) return;

	if ((expertn_modul_base_check_time_span(&(self->nests[nest].timespan),self->xpn_class[i3-1]->pTi)==0) && (output_time_step_compare_time_step(self->xpn_class[i3-1]->pTi,self->nests[nest].output_time_step,self->nests[nest].output_time_step_string,&(self->nests[nest].xpn_time_old),&(self->nests[nest].xno_time))==0))
		{

// Variablen schreiben:
			times_index[0]=self->nests[nest].count;
			time_value = self->nests[nest].ftime;//self->xpn_class[self->nests[nest].var_list[0].reg_var[0].xpn_class]->XPN_Moduls[0]->xpn->pTi->pSimTime->fTime;

// SimTime in folgendes Format schreiben: "1991-12-09_00:14:23"
			write_TimeStr(TimeStr,&(self->nests[nest].xno_time));

// In TimeStr in netcdf schreiben:
			Times_Start[0]=self->nests[nest].count;
			Times_Start[1]=0;
			Times_Count[0]=1;
			Times_Count[1]=19;




#ifdef USE_MPI
			NC_CHECK_ERROR2(ncmpi_put_vara_text_all(self->nests[nest].netf,self->nests[nest].Times_var_id,Times_Start,Times_Count,(const char*)TimeStr),"TimeStr");
#else
			NC_CHECK_ERROR2(nc_put_vara_text(self->nests[nest].netf,self->nests[nest].Times_var_id,Times_Start,Times_Count,(const char*)TimeStr),"TimeStr");
#endif




#ifdef USE_MPI
			times_count[0]=1;
			NC_CHECK_ERROR2(ncmpi_put_vara_double_all(self->nests[nest].netf,self->nests[nest].time_var_id,times_index,times_count,&time_value),"time");
#else
			NC_CHECK_ERROR2(wrap_nc_put_var1_double(self->nests[nest].netf,self->nests[nest].time_var_id,times_index,&time_value),"time");
#endif




// xlat,xlong schreiben:
			for (i=0; i<self->nests[nest].var_list_len; i++)
				{
					for (i2=0; i2<self->nests[nest].var_list[i].reg_var_len; i2++)
						{
							xpn_class = self->xpn_class[self->nests[nest].var_list[i].reg_var[i2].xpn_class];
							if (xpn_class==NULL) continue;
							//if (self->cfg->grid[i].id!=self->nests[nest].nest_id) continue;
							var_index[0]=self->nests[nest].count;
							var_index[1]=self->nests[nest].var_list[i].reg_var[i2].k;
							var_index[2]=self->nests[nest].var_list[i].reg_var[i2].j;
							var_index[3]=self->nests[nest].var_list[i].reg_var[i2].i;
#ifdef USE_MPI
							var_count[0]=1;
							var_count[1]=1;
							var_count[2]=1;
							var_count[3]=1;
#endif
							NC_CHECK_ERROR2(wrap_nc_put_var1_double(self->nests[nest].netf,self->nests[nest].XLAT.netcdf_id,var_index,&(xpn_class->pLo->pFarm->fLatitude)),"XLAT");
							NC_CHECK_ERROR2(wrap_nc_put_var1_double(self->nests[nest].netf,self->nests[nest].XLON.netcdf_id,var_index,&(xpn_class->pLo->pFarm->fLongitude)),"XLONG");
						}
				}



			for (i=0; i<self->nests[nest].var_list_len; i++)
				{
					for (i2=0; i2<self->nests[nest].var_list[i].reg_var_len; i2++)
						{
							xpn_class = self->xpn_class[self->nests[nest].var_list[i].reg_var[i2].xpn_class];
							if (xpn_class==NULL) continue;
							var_list = xpn_class->pXSys->var_list;
							var = var_list->vars[self->nests[nest].var_list[i].reg_var[i2].var_number];
							var_index[0]=self->nests[nest].count;
							var_index[1]=self->nests[nest].var_list[i].reg_var[i2].k;
							var_index[2]=self->nests[nest].var_list[i].reg_var[i2].j;
							var_index[3]=self->nests[nest].var_list[i].reg_var[i2].i;
#ifdef USE_MPI
							var_count[0]=1;
							var_count[1]=1;
							var_count[2]=1;
							var_count[3]=1;
#endif
							switch(var->vartype)
								{
								case G_TYPE_INT:
									NC_CHECK_ERROR2(wrap_nc_put_var1_int(self->nests[nest].netf,self->nests[nest].var_list[i].netcdf_id,var_index,var->val.pi),var->varname);
									break;
								case G_TYPE_DOUBLE:
									NC_CHECK_ERROR2(wrap_nc_put_var1_double(self->nests[nest].netf,self->nests[nest].var_list[i].netcdf_id,var_index,var->val.pd),var->varname);
									break;
								case G_TYPE_FLOAT:
									NC_CHECK_ERROR2(wrap_nc_put_var1_float(self->nests[nest].netf,self->nests[nest].var_list[i].netcdf_id,var_index,var->val.pf),var->varname);
									break;
								default :
									S_ERROR = g_strdup_printf("Writing Variable in NET_CDF File: %s",var->varname);
									PRINT_ERROR(S_ERROR);
									g_free(S_ERROR);
								}

						}
				}

// Daten im speicher auf Festlpatte flushen
			if (self->always_dump_nc_file==1)
				{
					NC_CHECK_ERROR(wrap_nc_sync(self->nests[nest].netf));
				}


			self->nests[nest].count++;
		}

}
