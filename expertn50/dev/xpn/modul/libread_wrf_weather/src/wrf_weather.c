//      wrf_weather.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "wrf_weather.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(wrf_weather, wrf_weather, EXPERTN_MODUL_BASE_TYPE);


static void wrf_weather_class_init(wrf_weatherClass *klass) {}


static void wrf_weather_init(wrf_weather *self) {
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL WRF_WEATHER!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
        self->wrf = NULL;
        self->header = NULL;
        self->valuelist = NULL;
        self->size_of_values = 0;
        self->size_of_valuelist = 0;
        self->connection=NULL;
}



// Our Modul Functions:

static int wrf_weather_load_cdb(wrf_weather *self,char *cdb_filename) {
        char *S,*S2,*S3;
        FILE *f;
        int i,i2;
        double *values;
        int size;
        self->connection=NULL;
        f = fopen(cdb_filename,"r");
        i = 0;        
        if (f==NULL) return 1;
        while (1) {
                S = fgets_mem(f);
                if (S==NULL) break;
                size = getStringColumn_get_Size(S,",");
                if (size < 1) break;
                S2 = (char*)g_malloc0(strlen(S)+1);
                if (i==0) {
                        self->size_of_values = size;
                        self->size_of_valuelist = 0;
                        self->valuelist = NULL;
                        self->header = (char**)g_malloc0(sizeof(char*)*size);
                        for (i2=0; i2<size; i2++) {
                                getStringColumn(S2,S,",",i2);
                                S3 = (char*)g_malloc0(strlen(S2)+1);
                                strcpy(S3,S2);
                                self->header[i2] = S3;
                        }
                } else {
                        if (size!=self->size_of_values) break;
                        values =(double*)g_malloc0(sizeof(double)*size);
                        for (i2=0; i2<size; i2++) {
                                getStringColumn(S2,S,",",i2);
                                values[i2] = atof(S2);
                        }
                        self->size_of_valuelist++;
                        self->valuelist = (double**)g_realloc(self->valuelist,sizeof(double*)*self->size_of_valuelist);
                        self->valuelist[self->size_of_valuelist-1] = values;
                }



                g_free(S2);
                free(S);
                i++;
        }

        if (i < 2) return 1;

        return RET_SUCCESS;
}

static int wrf_weather_set_values(wrf_weather *self,int start) {
        expertn_modul_base *xpn = &(self->parent);
        int i,i2;
        double sim_time,sim_time2;

        if (self->valuelist!=NULL) {
                if (start==1) {
                        self->connection = (struct_connection*)g_malloc0(sizeof(struct_connection)*self->size_of_values);
                        for (i=0; i<self->size_of_values; i++) {
                                for (i2=0; i2<xpn->pXSys->var_list->size; i2++) {
                                        if (xpn->pXSys->var_list->vars[i2]->flag_pointer==TRUE) {
                                                if (strcmp(xpn->pXSys->var_list->vars[i2]->varname,self->header[i])==0) {
                                                        self->connection[i].vartype = xpn->pXSys->var_list->vars[i2]->vartype;
                                                        switch (self->connection[i].vartype) {
                                                        case G_TYPE_INT:
                                                                self->connection[i].pointer = (void*)(xpn->pXSys->var_list->vars[i2]->val.pi);
                                                                break;
                                                        case G_TYPE_DOUBLE:
                                                                self->connection[i].pointer = (void*)(xpn->pXSys->var_list->vars[i2]->val.pd);
                                                                break;
                                                        case G_TYPE_FLOAT:
                                                                self->connection[i].pointer = (void*)(xpn->pXSys->var_list->vars[i2]->val.pf);
                                                                break;
                                                        }
                                                        break;


                                                }
                                        }
                                }
                        }
                } // Simstart
                //for (i=0; i<self->size_of_valuelist; i++)
                {
                        i = self->counter;
                        sim_time = self->valuelist[i][0];
                        if ((i+1) >= self->size_of_valuelist) {
                                return 1;
                        }
                        sim_time2 = self->valuelist[i+1][0];

                        //if ((start==1) || (self->sim_start==1) || (((xpn->pTi->pSimTime->fTime) >= sim_time) && ((xpn->pTi->pSimTime->fTime) < sim_time2))) {
                        {
                                for (i2=0; i2<self->size_of_values; i2++) {
                                        if (self->connection[i2].pointer!=NULL) {
                                                /*      if (start!=1)
                                                                {
                                                                        if (strcmp_extra(self->header[i2],"pTi.*")==0)
                                                                        {
                                                                                continue;
                                                                        }
                                                                }*/
                                                if (strcmp_extra(self->header[i2],"Config.*")==0) {
                                                        continue;
                                                }

                                                switch (self->connection[i2].vartype) {
                                                case G_TYPE_INT:
                                                        (*(int*)(self->connection[i2].pointer)) = self->valuelist[i][i2];
                                                        break;
                                                case G_TYPE_DOUBLE:
                                                        (*(double*)(self->connection[i2].pointer)) = self->valuelist[i][i2];
                                                        break;
                                                case G_TYPE_FLOAT:
                                                        (*(float*)(self->connection[i2].pointer)) = self->valuelist[i][i2];
                                                        break;
                                                }

                                        }
                                }
                                //break;
                        }
                }
        }

        return RET_SUCCESS;
}


int wrf_weather_load(wrf_weather *self) {
        expertn_modul_base *xpn = &(self->parent);
        xpn_register_var *reg;
        char *cdb_filename0,*cdb_filename;
        self->sim_start=0;
        self->wrf_is_driver=0;
        reg = xpn->pXSys->var_list;
        cdb_filename0 = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.use wrf weather file.filename");
        if (cdb_filename0==NULL) {
                PRINT_ERROR("Missing entry 'use wrf weather file.filename' in your configuration!");
        } else
                //if (xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_Noah")!=NULL)
                //{
                //      PRINT_ERROR("You can't use the module together with the WRF Driver");
                //}else
        {
                cdb_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, cdb_filename0);
                if (wrf_weather_load_cdb(self,cdb_filename)!=0) {
                        PRINT_ERROR("Error Read WRF Weather File");
                }
                free(cdb_filename);
                self->wrf = xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_Noah");
                if (self->wrf!=NULL) {
                        self->wrf = (struct_WRF*)g_malloc0(sizeof(struct_WRF));
                        xpn_register_var_add_none(reg,"pWRF_Noah",self->wrf,-1,TRUE);
                        self->wrf_is_driver=0;
                } else {
                        self->wrf_is_driver=1;
                }
        }

        self->counter = -1;
        return RET_SUCCESS;//wrf_weather_set_values(self,1);
}

int wrf_weather_run(wrf_weather *self) {
//      expertn_modul_base *xpn = &(self->parent);
        if (self->sim_start==0) {
                self->sim_start=1;
        } else {
                self->sim_start=-1;
        }
        self->counter++;
        return wrf_weather_set_values(self,self->sim_start);
}

int wrf_weather_done(wrf_weather *self) {
        int i,i2;
        if (self->wrf_is_driver==0) {
                if (self->wrf !=NULL) {
                        g_free(self->wrf);
                        self->wrf=NULL;
                }

        }

        if (self->header!=NULL) {
                for (i=0; i<self->size_of_values; i++) {
                        if (self->header[i]!=NULL) {
                                g_free(self->header[i]);
                                self->header[i]=NULL;
                        }

                }
                g_free(self->header);
                self->header=NULL;
        }
        if (self->valuelist!=NULL) {
                for (i2=0; i2<self->size_of_valuelist; i2++) {
                        if (self->valuelist[i2] !=NULL) {


                                g_free(self->valuelist[i2]);
                                self->valuelist[i2] = NULL;




                        }
                }
        }
        if (self->connection != NULL) {
                g_free(self->connection);
                self->connection = NULL;
        }
        return RET_SUCCESS;
}


int wrf_weather_coupling_load(wrf_weather *self) {
        expertn_modul_base *xpn = &(self->parent);
        xpn_register_var *reg;
        char *cdb_filename0,*cdb_filename;
        self->sim_start=0;
        self->wrf_is_driver=0;
        reg = xpn->pXSys->var_list;
        cdb_filename0 = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.test coupling.filename");
        if (cdb_filename0==NULL) {
                PRINT_ERROR("Missing entry 'test coupling.filename' in your configuration!");
        } else
                //if (xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_Noah")!=NULL)
                //{
                //      PRINT_ERROR("You can't use the module together with the WRF Driver");
                //}else
        {
                cdb_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, cdb_filename0);
                if (wrf_weather_load_cdb(self,cdb_filename)!=0) {
                        PRINT_ERROR("Error Read WRF Weather File");
                }
                free(cdb_filename);
                self->wrf = xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_Noah");
                if (self->wrf==NULL) {
                        PRINT_ERROR("Input Error: This is only for use with WRF!")
                        return 1;
                } else {
                        self->wrf_is_driver=1;
                }
        }

        self->counter = -1;
        return RET_SUCCESS;//wrf_weather_set_values(self,1);
}


// ret 1 if value is in list else 0
static int value_in_list(char** valuelist,int list_len,char* value) {
        int i;
        for (i=0; i<list_len; i++) {
                if (strcmp(valuelist[i],value)==0) {
                        return 1;
                }
        }
        return 0;
}

static void wrf_weather_coupling_set_values(wrf_weather *self,char **value_list,int list_len) {
        expertn_modul_base *xpn = &(self->parent);
        int i,i2;
        double sim_time,sim_time2;

        if (self->valuelist!=NULL) {
                if (self->counter==0) {
                        self->connection = (struct_connection*)g_malloc0(sizeof(struct_connection)*self->size_of_values);
                        for (i=0; i<self->size_of_values; i++) {
                                for (i2=0; i2<xpn->pXSys->var_list->size; i2++) {
                                        if (xpn->pXSys->var_list->vars[i2]->flag_pointer==TRUE) {
                                                if (strcmp(xpn->pXSys->var_list->vars[i2]->varname,self->header[i])==0) {
                                                        self->connection[i].vartype = xpn->pXSys->var_list->vars[i2]->vartype;
                                                        switch (self->connection[i].vartype) {
                                                        case G_TYPE_INT:
                                                                self->connection[i].pointer = (void*)(xpn->pXSys->var_list->vars[i2]->val.pi);
                                                                break;
                                                        case G_TYPE_DOUBLE:
                                                                self->connection[i].pointer = (void*)(xpn->pXSys->var_list->vars[i2]->val.pd);
                                                                break;
                                                        case G_TYPE_FLOAT:
                                                                self->connection[i].pointer = (void*)(xpn->pXSys->var_list->vars[i2]->val.pf);
                                                                break;
                                                        }
                                                        break;


                                                }
                                        }
                                }
                        }

                }
                
                self->counter++;
                if (self->counter==0) return;
                
                for (i=0; i<self->size_of_valuelist; i++) {
                        sim_time = self->valuelist[i][0];
                        if (i>=self->size_of_valuelist-2) {
                                i= self->size_of_valuelist-1;
                                break;
                        }
                        sim_time2 =self->valuelist[i+1][0];
                        //ToDo: Funktioniert jetzt so nicht mehr:
                        /*if ((xpn->pTi->pSimTime->fTime>=sim_time) && (xpn->pTi->pSimTime->fTime<sim_time2)) {
                                break;
                        }*/
                }

                for (i2=0; i2<self->size_of_values; i2++) {
                        if (self->connection[i2].pointer!=NULL) {                               
                                if (value_in_list(value_list,list_len,self->header[i2])==0) {
                                        continue;
                                }                               
                                switch (self->connection[i2].vartype) {
                                case G_TYPE_INT:
                                        (*(int*)(self->connection[i2].pointer)) = self->valuelist[i][i2];
                                        break;
                                case G_TYPE_DOUBLE:
                                        (*(double*)(self->connection[i2].pointer)) = self->valuelist[i][i2];                                    
                                        break;
                                case G_TYPE_FLOAT:
                                        (*(float*)(self->connection[i2].pointer)) = self->valuelist[i][i2];                                     
                                        break;
                                }

                        }
                }

        }
}


int wrf_weather_coupling_run(wrf_weather *self) {
#define _value_list_len 47
        char* value_list[_value_list_len] = {
                "WRF.eta",
                "WRF.eta_kinematic",
                "WRF.sheat",
                "WRF.fdown",
                "WRF.edir",
                "WRF.et[0]",
                "WRF.et[1]",
                "WRF.et[2]",
                "WRF.et[3]",
                "WRF.ett",
                "WRF.esnow",
                "WRF.dew",
                "WRF.beta",
                "WRF.etp",
                "WRF.ssoil",
                "WRF.flx1",
                "WRF.flx2",
                "WRF.flx3",
                "WRF.snomlt",
                "WRF.sncovr",
                "WRF.runoff1",
                "WRF.runoff2",
                "WRF.runoff3",
                "WRF.rc",
                "WRF.pc",
                "WRF.xlai",
                "WRF.rsmin",
                "WRF.rcs",
                "WRF.rct",
                "WRF.rcq",
                "WRF.rcsoil",
                "WRF.cmc",
                "WRF.T1",
                "WRF.stc[0]",
                "WRF.stc[1]",
                "WRF.stc[2]",
                "WRF.stc[3]",
                "WRF.smc[1]",
                "WRF.smc[2]",
                "WRF.smc[3]",
                "WRF.sh2o[0]",
                "WRF.sh2o[1]",
                "WRF.sh2o[2]",
                "WRF.sh2o[3]",
                "WRF.albedo",
                "WRF.ch",
                "WRF.cm"
        };
        wrf_weather_coupling_set_values(self,value_list,_value_list_len);
        return RET_SUCCESS;
}

int wrf_weather_coupling_done(wrf_weather *self) {
        int i,i2;

        if (self->header!=NULL) {
                for (i=0; i<self->size_of_values; i++) {
                        if (self->header[i]!=NULL) {
                                g_free(self->header[i]);
                                self->header[i]=NULL;
                        }

                }
                g_free(self->header);
                self->header=NULL;
        }
        if (self->valuelist!=NULL) {
                for (i2=0; i2<self->size_of_valuelist; i2++) {
                        if (self->valuelist[i2] !=NULL) {
                                g_free(self->valuelist[i2]);
                                self->valuelist[i2] = NULL;
                        }
                }
        }
        if (self->connection != NULL) {
                g_free(self->connection);
                self->connection = NULL;
        }
        return RET_SUCCESS;
}
