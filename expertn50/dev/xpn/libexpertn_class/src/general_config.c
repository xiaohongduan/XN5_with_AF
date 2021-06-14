//      general_config.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "general_config.h"
#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <stdlib.h>
#include <string.h>
#include <xpn_register_var.h>

#define PRINT_ERROR(Message) fprintf(stderr,"ERROR:  %s\t%d\t%s\n",__FILE__,__LINE__,Message);fflush(stderr);


G_DEFINE_TYPE(general_config, general_config, G_TYPE_OBJECT);


static void general_config_class_init(general_configClass *klass) {}



static void general_config_init(general_config *self) 
{
	self->DEBUG_LEVEL=3;       	
	self->num_proc=0;
	self->expertn_lib_list=NULL;
	self->expertn_lib_list_len=0;
	self->number_of_xpn=0;
	self->grid=NULL;
	self->grid_len=0;
	self->number_of_xpn=0;
	self->config_values_len=0;
	self->config_values=NULL;	
}


general_config* general_config_new(void)
{
	return g_object_new(GENERAL_CONFIG_TYPE, NULL);
}

void general_config_delete(general_config *self)
{	
	int i,i2;
	g_free(self->expertn_lib_list);		
	
	for (i=0;i<self->grid_len;i++)
		{
			for (i2=0;i2<self->grid[i].instance_len;i2++)
				{
					g_free(self->grid[i].instance[i2].config_file);
				}
			g_free(self->grid[i].instance);
		}
	g_free(self->grid);
	
	for (i=0;i<self->config_values_len;i++)
		{
			g_free(self->config_values[i].key);
			g_free(self->config_values[i].value);
		}
	g_free(self->config_values);
	
	self->DEBUG_LEVEL=3;       	
	self->num_proc=0;
	self->expertn_lib_list=NULL;
	self->expertn_lib_list_len=0;
	self->number_of_xpn=0;
	self->grid=NULL;
	self->grid_len=0;
	self->number_of_xpn=0;
	self->config_values_len=0;
	self->config_values=NULL;
	g_object_unref(self);
	
}	


int general_config_load_config_file(general_config *self, const char *cfgfile)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	gchar  *modul_path;
	gchar  **expertn_lib_list;
	
	gchar **grid;
	gsize grid_length;
	
	int grid_index;
	int id_index;
	int i,i2,i3;
	char *S,*S2;
	
	gsize number_of_groups,number_of_keys;
	gchar ** groups, ** keys;

	// Standart Werte für die Config setzen:
	self->DEBUG_LEVEL=3;       	
	self->num_proc=0;
	self->expertn_lib_list=NULL;
	self->expertn_lib_list_len=0;
	self->number_of_xpn=0;
	self->grid=NULL;
	self->grid_len=0;
	self->number_of_xpn=0;
	self->config_values_len=0;
	self->config_values=NULL;
	
	modul_path = "models";
	
	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, cfgfile, flags, &error))
		{
			if (cfgfile!=NULL)
				{
					S = g_strdup_printf("Error with cfgfile: %s",cfgfile);
					PRINT_ERROR(S);
					g_free(S);
				}
			PRINT_ERROR(error->message);					
			return -1;
		}

	/* Create a new Settings object. If you are using GTK+ 2.8 or below, you should
	 * use g_new() or g_malloc() instead! */
	//xdr->self = g_slice_new (struct_expertn_driver_config);

	self->DEBUG_LEVEL = g_key_file_get_integer (keyfile,"system","debug_level",NULL);
	self->num_proc = g_key_file_get_integer (keyfile, "system",  "num_proc", NULL);
	
	grid = g_key_file_get_string_list( keyfile, "model","grid",&(grid_length),NULL);
	
	self->number_of_xpn=0;
	self->grid_len=0;
	
	for (i=0;i<grid_length;i++)
		{
			S = (char*)g_malloc0(strlen(grid[i])+1);
			if (getStringColumn(S,grid[i],",",0)!=0) {PRINT_ERROR("problem with grid in INI File");break;}
			if (atoi(S)==1)
				{
					self->grid_len++;
				}
			g_free(S);
		}
	if (self->grid_len>0)
		{
			self->grid = (STRUCT_Grid*)g_malloc0(sizeof(STRUCT_Grid)*self->grid_len);
		}
		
	grid_index=0;
	id_index=0;
	for (i=0;i<grid_length;i++)
		{
			S = (char*)g_malloc0(strlen(grid[i])+1);
			if (getStringColumn(S,grid[i],",",0)!=0) {PRINT_ERROR("problem with grid in INI File");break;}
				if (atoi(S)==1)
				{
					int arraylen;
					if (getStringColumn(S,grid[i],",",1)!=0) {PRINT_ERROR("problem with grid in INI File");break;}
					self->grid[grid_index].id = atoi(S);
					if (getStringColumn(S,grid[i],",",2)!=0) {PRINT_ERROR("problem with grid in INI File");break;}
					self->grid[grid_index].i = atoi(S);
					if (getStringColumn(S,grid[i],",",3)!=0) {PRINT_ERROR("problem with grid in INI File");break;}
					self->grid[grid_index].j = atoi(S);
					arraylen = getStringColumn_get_Size(grid[i],",");
					self->grid[grid_index].instance_len=(arraylen-4)/2;
					if (self->grid[grid_index].instance_len <= 0)
						{
							PRINT_ERROR("problem with grid in INI File");break;
						}
					self->grid[grid_index].instance= (STRUCT_Instance*) g_malloc0(sizeof(STRUCT_Instance)*self->grid[grid_index].instance_len);
					for (i2=0;i2<self->grid[grid_index].instance_len;i2++)
						{
							if (getStringColumn(S,grid[i],",",4+(i2*2))!=0) {PRINT_ERROR("problem with grid in INI File");break;}
							self->grid[grid_index].instance[i2].config_file = (char*)g_malloc0(strlen(S)+1);
							strcpy(self->grid[grid_index].instance[i2].config_file,S);
							if (getStringColumn(S,grid[i],",",4+(i2*2)+1)!=0) {PRINT_ERROR("problem with grid in INI File");break;}
							self->grid[grid_index].instance[i2].cover = atof(S);
							self->grid[grid_index].instance[i2].xpn_id_number=id_index;
							id_index++;
						}
					grid_index++;
				}								
			g_free(S);
			//S = g_strdup_printf("",grid[i]
		}
	self->number_of_xpn= id_index;
	expertn_lib_list = g_key_file_get_string_list( keyfile, "modul","libs",&(self->expertn_lib_list_len),NULL);	
	
	self->expertn_lib_list = (gchar  **)g_malloc0(self->expertn_lib_list_len*sizeof(gchar*));
	for (i=0;i<(int)self->expertn_lib_list_len;i++)
		{
			
			if ((strstr(expertn_lib_list[i],"/") == NULL) && (strstr(expertn_lib_list[i],"\\")==NULL) )
				{						
					self->expertn_lib_list[i] = g_module_build_path (modul_path,expertn_lib_list[i]);
					//g_strdup_printf("%s/lib%s.%s",)
				} else
				{
					self->expertn_lib_list[i] = g_module_build_path (NULL,expertn_lib_list[i]);
				}
		}
		
	
	// Alle Gruppen und keys werden in die Registry geladen:
	//number_of_groups,number_of_keys

	// Anzahl der Einstellungen ermitteln
	self->config_values_len=0;
	groups = g_key_file_get_groups (keyfile,&number_of_groups);
	for (i=0; i<(int)number_of_groups; i++)
		{
			keys = g_key_file_get_keys(keyfile, groups[i], &number_of_keys,NULL);
			if (keys!=NULL)
				{
					for (i2=0; i2<(int)number_of_keys; i2++)
						{
							self->config_values_len++;							
						}
					g_strfreev(keys);
				}

		}
	g_strfreev(groups);
	
	// Speicher reservieren:
	self->config_values = (STRUCT_Config_Value*)g_malloc0(sizeof(STRUCT_Config_Value)*self->config_values_len);
	
	// Werte auslesen:
	i3=0;
	groups = g_key_file_get_groups (keyfile,&number_of_groups);
	for (i=0; i<(int)number_of_groups; i++)
		{
			keys = g_key_file_get_keys(keyfile, groups[i], &number_of_keys,NULL);
			if (keys!=NULL)
				{
					for (i2=0; i2<(int)number_of_keys; i2++)
						{													
							S = g_strdup_printf("Global_Config.%s.%s",groups[i],keys[i2]);
							S2 = g_key_file_get_value (keyfile,groups[i],keys[i2],NULL);
							self->config_values[i3].key=S;
							self->config_values[i3].value=S2;						
							i3++;
						}
					g_strfreev(keys);
				}

		}
	g_strfreev(groups);
		
		
	g_key_file_free(keyfile);

	return 0;
}


int general_config_get_instance_max_size(general_config *self,int id)
{
	int size,i;
	size = 0;
	for (i=0; i<self->grid_len; i++)
		{
			if (self->grid[i].id==id)
			{
			if (self->grid[i].instance_len > size)
				{
					size = self->grid[i].instance_len;
				}
			}
		}
	return size;
}

int* general_config_get_grid_id_list(general_config *self,int *_size)
{
	int size,i,i2;
	int *ids;
	int not_id;
	size = 1;
	ids = (int*)g_malloc0(sizeof(int)*size);
	ids[0] = self->grid[0].id;
	for (i=0; i<self->grid_len; i++)
		{		
			not_id=1;
			for (i2=0;i2<size;i2++)
				{
					if (ids[i2] == self->grid[i].id)
						{
							not_id=0;
							break;
						}					
				}
			if (not_id==1)
			{
			size++;
			ids = (int*)g_realloc(ids,sizeof(int)*size);
			ids[size-1] = self->grid[i].id;
			}
		}
	*_size=size;
	return ids;
}



void general_config_get_grid_size_ij(general_config *self,int *_i1, int *_j1, int *_i2, int *_j2,int id)
{
	int i;
	int i1,i2,j1,j2;
	char *S;
	i1=-1;
	i2=-1;
	j1=-1;
	j2=-1;
	// ersten Wert festlegen:
	for (i=0; i<self->grid_len; i++)
		{
			if (self->grid[i].id==id)
				{
					i1 = self->grid[i].i;
					i2 = self->grid[i].i;
					j1 = self->grid[i].j;
					j2 = self->grid[i].j;
					break;
				}
		}
	
	for (i=0; i<self->grid_len; i++)
		{
			if (self->grid[i].id==id)
				{
					if (self->grid[i].i > i2) i2 = self->grid[i].i;
					if (self->grid[i].i < i1) i1 = self->grid[i].i;
					if (self->grid[i].j > j2) j2 = self->grid[i].j;
					if (self->grid[i].j < j1) j1 = self->grid[i].j;
				}
		}
	if (i1==-1)
		{
			S = g_strdup_printf("Grid %d does not exist!",id);
			PRINT_ERROR(S);
			g_free(S);
			return;
		}
	*_i1 = i1;
	*_i2 = i2;
	*_j1 = j1;
	*_j2 = j2;	
}


