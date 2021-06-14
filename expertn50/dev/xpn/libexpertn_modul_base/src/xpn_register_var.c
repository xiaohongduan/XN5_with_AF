//      xpn_register_var.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "xpn_register_var.h"
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <glib-object.h>
#include <gmodule.h>
#include <math.h>
#include "xinclexp.h"


static void xpn_register_var_finalize  			(GObject *object);

G_DEFINE_TYPE(xpn_register_var, xpn_register_var, G_TYPE_OBJECT);

#define PRINT_ERROR(Message) fprintf(stderr,"ERROR: %s\t%d\t%s\n",__FILE__,__LINE__,Message);fflush(stderr);



static char** xpn_register_var_add_filter(char **filterlist,int *filterlist_len,const char *_new_filter);
static void xpn_register_var_free_filter(char **filterlist, int filterlist_len);


static void xpn_register_var_class_init(xpn_register_varClass *klass)
{
	GObjectClass *g_object_class;

	g_object_class = G_OBJECT_CLASS(klass);

	g_object_class->finalize = xpn_register_var_finalize;

	// gda_init ();
}


static void xpn_register_var_finalize(GObject *object)
{
	xpn_register_var *self;

	self = XPN_REGISTER_VAR(object);

	xpn_register_var_clear(self);
#ifndef XPN_WITHOUT_DATABASE
	if (self->database_provider==NULL) free(self->database_provider);
	if (self->database_string==NULL) free(self->database_string);
#endif
	if (self->txtfile==NULL) free(self->txtfile);

	xpn_register_var_free_filter(self->str_filter,self->str_filter_len);
	self->str_filter = NULL;
	self->str_filter_len = 0;
	xpn_register_var_free_filter(self->str_filter_ig,self->str_filter_ig_len);
	self->str_filter_ig=NULL;
	self->str_filter_ig_len=0;

	g_return_if_fail(object != NULL);
	g_return_if_fail(IS_XPN_REGISTER_VAR(object));



	G_OBJECT_CLASS(xpn_register_var_parent_class)->finalize(object);
}


static void xpn_register_var_init(xpn_register_var *self)
{
	self->size=0;
	self->counter=0;
	self->counter_start=0;
	self->vars=NULL;
	self->str_filter=NULL;
	self->str_filter_len=0;
	self->str_filter_ig=NULL;
	self->str_filter_ig_len=0;
	self->create_xno_files=0;
	self->xno_time.year=0;
	self->xno_time.mon=0;
	self->xno_time.mday=0;
	self->xno_time.hour=0;
	self->xno_time.min=0;
	self->xno_time.sec=0;

}

static char** xpn_register_var_add_filter(char **filterlist,int *filterlist_len,const char *_new_filter)
{
	char *new_filter;
	if (_new_filter==NULL)
		{
			return filterlist;
		}
	new_filter=(char*)malloc(strlen(_new_filter)+1);
	strcpy(new_filter,_new_filter);
	deleteSpaceBegEnd(new_filter);
	if ((strcmp(new_filter,"")==0) || (new_filter[0]=='#'))
		{
			free(new_filter);
			return filterlist;
		}
	(*filterlist_len)++;
	if ((filterlist==NULL) || ((*filterlist_len) <= 1))
		{
			filterlist = (char**) malloc(sizeof(char*)*  (*filterlist_len) );
			filterlist[(*filterlist_len)-1]=(char*)malloc(strlen(new_filter)+1);
			strcpy(filterlist[(*filterlist_len)-1],new_filter);
			free(new_filter);
			return filterlist;
		}
	else
		{
			filterlist = (char**) realloc( filterlist, sizeof(char*)* (*filterlist_len) );
			filterlist[(*filterlist_len)-1]=(char*)malloc(strlen(new_filter)+1);
			strcpy(filterlist[(*filterlist_len)-1],new_filter);
			free(new_filter);
			return filterlist;
		}
}

static void xpn_register_var_free_filter(char **filterlist, int filterlist_len)
{
	int i;
	if ((filterlist==NULL) ||  (filterlist_len<=0)) return;
	for (i=0; i<filterlist_len; i++)
		{
			if (filterlist[i]!=NULL) free(filterlist[i]);
		}
	free(filterlist);
}

// return 0 if in list or 1 if not
int  xpn_register_var_check_if_in_list(char **filterlist, int filterlist_len,char* str)
{
	int i;
	for (i=0; i<filterlist_len; i++)
		{
			if (strcmp_extra(str,filterlist[i])==0)
				{
					return 0;
				}
		}
	return 1;
}


xpn_register_var *xpn_register_var_new(char* database_provider, char* database_string, char* txtfile)
{
	xpn_register_var * self;
	self = ( xpn_register_var *)g_object_new(XPN_REGISTER_VAR_TYPE, NULL);
#ifndef XPN_WITHOUT_DATABASE
	if (database_provider!=NULL)
		{
			self->database_provider=(char*)malloc(strlen(database_provider)+1);
			strcpy(self->database_provider,database_provider);
		}
	if (database_string!=NULL)
		{
			self->database_string=(char*)malloc(strlen(database_string)+1);
			strcpy(self->database_string,database_string);
		}
#endif
	if (txtfile!=NULL)
		{
			self->txtfile=(char*)malloc(strlen(txtfile)+1);
			strcpy(self->txtfile,txtfile);
		}
	self->fTime=0.0;
	return self;
}

static void xpn_register_var_add(xpn_register_var *self,int vartype,char *varname,int line, gboolean record, gboolean pointer,double d,double *pd,int i, int *pi,float *pf,void *none)
{
	struct_variable *var;
	int i2;

	if (varname==NULL) return;

	for (i2=0; i2<strlen(varname); i2++)
		{
			if ((varname[i2]==',') || (varname[i2]=='\n'))
				{
					char *S;
					S = g_strdup_printf("Var %s Line: %d has invalid character",varname,line);
					fprintf(stderr,"%s\n",S);
					fflush(stderr);
					g_free(S);
					return;
				}
		}

	if (pointer==TRUE)
		{
			// Wenn Variable nicht in der Liste --> wird nicht uebernommen:
			if (xpn_register_var_check_if_in_list(self->str_filter,self->str_filter_len,varname)==1)
				{
					//return; geht grad nicht, da sonst die counter falsch gezaehlt werden, daher nur deaktivieren
					record=FALSE;
				}
		}

	var = (struct_variable *) malloc(sizeof(struct_variable));
	var->vartype=vartype;
	var->line=line;
	var->varname = (char*)malloc(strlen(varname)+1);
	strcpy(var->varname,varname);
	var->flag_pointer=pointer;
	var->flag_record=record;
	switch(var->vartype)
		{
		case G_TYPE_INT:
			if(var->flag_pointer==TRUE) var->val.pi = pi;
			else var->val.i = i;
			break;
		case G_TYPE_DOUBLE:
			if(var->flag_pointer==TRUE) var->val.pd = pd;
			else var->val.d = d;
			break;
		case G_TYPE_FLOAT:
			if(var->flag_pointer==TRUE) var->val.pf = pf;
			else var->val.d = d;
			break;
		case  G_TYPE_NONE:
			var->val.n = none;
			var->flag_pointer=TRUE;
			break;
		default :
			fprintf(stderr,"%s\t%d\tERROR: Databasetype of %s is not known\n",__FILE__,__LINE__,var->varname);
		}

	// Add to list;
	self->size++;
	self->vars = (struct_variable**)realloc(self->vars,sizeof(struct_variable*)*self->size);
	self->vars[self->size-1]=var;
}

void xpn_register_var_add_int(xpn_register_var *self,char *varname,int value,int line, gboolean record)
{
	self->counter++;

	if (self->counter > self->size)
		{
			xpn_register_var_add (self,G_TYPE_INT,varname,line,record,FALSE,0.0,NULL,value,NULL,NULL,NULL);
		}
	else
		{
			self->vars[self->counter-1]->val.i=value;
		}
}

void xpn_register_var_add_pint(xpn_register_var *self,char *varname,int *value,int line, gboolean record,gboolean reg_once)
{
	if (reg_once==TRUE)
		{
			if (self->counter!=self->counter_start)
				{
					fprintf(stderr,"%s\t%d\tERROR: Database registration: %s\n",__FILE__,__LINE__,varname);
					exit(1);
				}
			self->counter_start++;
		}
	self->counter++;
	if (self->counter > self->size)
		{
			xpn_register_var_add (self,G_TYPE_INT,varname,line,record,TRUE,0.0,NULL,0,value,NULL,NULL);
		}/* else
		{
			self->vars[self->counter-1]->val.pi=value;
		}*/
}

void xpn_register_var_add_double(xpn_register_var *self,char *varname,double value,int line, gboolean record)
{
	int i;
	char *S;
	self->counter++;
	if (self->counter > self->size)
		{
			//self->counter=self->counter_start;
			for (i=self->counter_start; i<self->size; i++)
				{
					if ((strcmp(varname,self->vars[i]->varname)==0) && (line==self->vars[i]->line))
						{
							self->counter=i+1;
							self->vars[i]->val.d=value;
							/*S = g_strdup_printf("Problem with var: %s Line: %d",varname,line);
							fprintf(stderr,"%s\n",S);fflush(stderr);
							g_free(S);*/
							return;
						}

				}
			xpn_register_var_add (self,G_TYPE_DOUBLE,varname,line,record,FALSE,value,NULL,0,NULL,NULL,NULL);
		}
	else
		{
			if ((strcmp(varname,self->vars[self->counter-1]->varname)==0) && (line==self->vars[self->counter-1]->line))
				{
					self->vars[self->counter-1]->val.d=value;
					return;
				}
			S = g_strdup_printf("Problem with var: %s Line: %d",varname,line);
			fprintf(stderr,"%s\n",S);
			fflush(stderr);
			g_free(S);
			for (i=self->counter_start; i<self->size; i++)
				{
					if ((strcmp(varname,self->vars[i]->varname)==0) && (line==self->vars[i]->line))
						{
							self->counter=i+1;
							self->vars[i]->val.d=value;
							return;
						}
				}
			// Er sollte nie Hierher kommen:
			//fprintf(stderr,"%s\t%d\tERROR: Error in C_DEBUG Funćtion: %s\n",__FILE__,__LINE__,varname);
			xpn_register_var_add (self,G_TYPE_DOUBLE,varname,line,record,FALSE,value,NULL,0,NULL,NULL,NULL);
		}
}

void xpn_register_var_add_pdouble(xpn_register_var *self,char *varname,double *value,int line, gboolean record,gboolean reg_once)
{
	if (reg_once==TRUE)
		{
			if (self->counter!=self->counter_start)
				{
					fprintf(stderr,"%s\t%d\tERROR: Database registration: %s\n",__FILE__,__LINE__,varname);
					exit(1);
				}
			self->counter_start++;
		}
	self->counter++;
	if (self->counter > self->size)
		{
			xpn_register_var_add (self,G_TYPE_DOUBLE,varname,line,record,TRUE,0.0,value,0,NULL,NULL,NULL);
		} /*else
		{
			self->vars[self->counter-1]->val.pd=value;
		}*/
}


void xpn_register_var_add_float(xpn_register_var *self,char *varname,float value,int line, gboolean record)
{
	xpn_register_var_add_double(self,varname,(double)value,line,record);
}

void xpn_register_var_add_pfloat(xpn_register_var *self,char *varname,float *value,int line, gboolean record,gboolean reg_once)
{
	if (reg_once==TRUE)
		{
			if (self->counter!=self->counter_start)
				{
					fprintf(stderr,"%s\t%d\tERROR: Database registration: %s\n",__FILE__,__LINE__,varname);
					exit(1);
				}
			self->counter_start++;
		}
	self->counter++;
	if (self->counter > self->size)
		{
			xpn_register_var_add (self,G_TYPE_FLOAT,varname,line,record,TRUE,0.0,NULL,0,NULL,value,NULL);
		} /*else
		{
			self->vars[self->counter-1]->val.pd=value;
		}*/
}

void xpn_register_var_add_none(xpn_register_var *self,char *varname,void *value,int line,gboolean reg_once)
{
	if (reg_once==TRUE)
		{
			if (self->counter!=self->counter_start)
				{
					fprintf(stderr,"%s\t%d\tERROR: Database registration: %s\n",__FILE__,__LINE__,varname);
					exit(1);
				}
			self->counter_start++;
		}
	self->counter++;
	if (self->counter > self->size)
		{
			xpn_register_var_add (self,G_TYPE_NONE,varname,line,FALSE,TRUE,0.0,NULL,0,NULL,NULL,value);
		} /*else
		{
			self->vars[self->counter-1]->val.n=value;
		}*/
}

void xpn_register_var_clear_counter(xpn_register_var *self)
{
	self->counter=self->counter_start;
}

void xpn_register_var_clear(xpn_register_var *self)
{
//    struct_variable *var;
	int i;
	for (i=0; i!=self->size; i++)
		{
			//var = self->vars[i];
			if ( self->vars[i]->varname!=NULL) free( self->vars[i]->varname);
			free( self->vars[i]);
			self->vars[i]=NULL;
		}
	free(self->vars);
	self->size=0;
	self->counter=0;
	self->vars=NULL;

}

void xpn_register_var_write_header(xpn_register_var *self,FILE*f)
{
	int i;
	gboolean first;
	first = FALSE;
	if (self->create_xno_files==1)
		{
			//cdb
			fprintf(f,"Time and Date,");
		}
	else
		{
			//xno
			fprintf(f,"Time,");
		}
	for (i=0; i!=self->size; i++)
		{
			if (self->vars[i]->flag_record==TRUE)
				{
					if (first==TRUE)
						{
							fprintf(f,",");
						}
					else
						{
							first=TRUE;
						}
					if (self->vars[i]->line==-1)
						{
							fprintf(f,"%s",self->vars[i]->varname);
						}
					else
						{
							fprintf(f,"%s Line %d",self->vars[i]->varname,self->vars[i]->line);
						}
				}
		}
	fprintf(f,"\n");
}

void xpn_register_var_write_txt_row(xpn_register_var *self,FILE*f)
{
	int i;
	gboolean first;
	first = FALSE;




	for (i=0; i!=self->size; i++)
		{
			if (self->vars[i]->flag_record==TRUE)
				{
					if (first==TRUE)
						{
							fprintf(f,",");
						}
					else
						{
							if (self->create_xno_files==1)
								{
									// xno output:
									fprintf(f,"%04d-%02d-%02d %02d:%02d:%02d,",self->xno_time.year,self->xno_time.mon,self->xno_time.mday,self->xno_time.hour,self->xno_time.min,self->xno_time.sec);
								}
							else
								{
									// cdb output
									fprintf(f,"%12.12e,",self->fTime);
								}
							first=TRUE;
						}
					if (self->vars[i]->flag_pointer==TRUE)
						{
							switch(self->vars[i]->vartype)
								{
								case G_TYPE_INT:
									fprintf(f,"%d",*(self->vars[i]->val.pi));
									break;
								case G_TYPE_DOUBLE:
									fprintf(f,"%12.12e",*(self->vars[i]->val.pd));
									break;
								case G_TYPE_FLOAT:
									fprintf(f,"%12.12e",(double)*(self->vars[i]->val.pf));
									break;
								default :
									fprintf(stderr,"%s\t%d\tERROR: Writing Database in TXT File: %s!\n",__FILE__,__LINE__,self->vars[i]->varname);
								} //switch

						}
					else
						{
							switch(self->vars[i]->vartype)
								{
								case G_TYPE_INT:
									fprintf(f,"%d",self->vars[i]->val.i);
									break;
								case G_TYPE_DOUBLE:
									fprintf(f,"%12.12e",self->vars[i]->val.d);
									break;
								case G_TYPE_FLOAT:
									fprintf(f,"%12.12e",(double)self->vars[i]->val.d);
									break;
								default :
									fprintf(stderr,"%s\t%d\tERROR: Writing Database in TXT File: %s!\n",__FILE__,__LINE__,self->vars[i]->varname);
								} //switch
						}
				}
		}
	fprintf(f,"\n");
}

#ifndef XPN_WITHOUT_DATABASE
void xpn_register_var_write_db(xpn_register_var *self)
{
	int i;
	GValue *ref;
	GValue *value;
	gboolean res;
	GError *error = NULL;


	ref = gda_value_new (G_TYPE_DOUBLE);
	g_value_set_double (ref,self->vars[0]->val.d );


	for (i=0; i!=self->size; i++)
		{
			if (self->vars[i]->flag_record==TRUE)
				{


					value=NULL;
					if (self->vars[i]->flag_pointer==TRUE)
						{
							switch(self->vars[i]->vartype)
								{
								case G_TYPE_INT:
									value = gda_value_new (G_TYPE_INT);
									g_value_set_int (value,*(self->vars[i]->val.pi) );
									break;
								case G_TYPE_DOUBLE:
									value = gda_value_new (G_TYPE_DOUBLE);
									g_value_set_double (value,*(self->vars[i]->val.pd) );
									break;
								case G_TYPE_FLOAT:
									value = gda_value_new (G_TYPE_FLOAT);
									g_value_set_float (value,*(self->vars[i]->val.pf) );
									break;
								default :
									fprintf(stderr,"%s\t%d\tERROR: Writing Database in DB: %s!\n",__FILE__,__LINE__,self->vars[i]->varname);
								} //switch

						}
					else
						{

							switch(self->vars[i]->vartype)
								{
								case G_TYPE_INT:
									value = gda_value_new (G_TYPE_INT);
									g_value_set_int (value,self->vars[i]->val.i );
									break;
								case G_TYPE_DOUBLE:
									value = gda_value_new (G_TYPE_DOUBLE);
									g_value_set_double (value,self->vars[i]->val.d );
									break;
								case G_TYPE_FLOAT:
									value = gda_value_new (G_TYPE_FLOAT);
									g_value_set_float (value,(float)self->vars[i]->val.d );
									break;
								default :
									fprintf(stderr,"%s\t%d\tERROR: Writing Database in DB: %s!\n",__FILE__,__LINE__,self->vars[i]->varname);
								} //switch
						}
					if (value!=NULL)
						{
							// self->vars[i]->varname
							res = gda_insert_row_into_table (self->cnc, "expertn", &error, self->vars[0]->varname, ref,  self->vars[i]->varname, value, NULL);
							if (!res)
								{
									g_error ("Could not INSERT data into the table: %s\n",
									         error && error->message ? error->message : "No detail");
								}
							gda_value_free (value);
						}
				}
		}
	gda_value_free (ref);
}


void run_sql_non_select (GdaConnection *cnc, const gchar *sql)
{
	GdaStatement *stmt;
	GError *error = NULL;
	gint nrows;
	const gchar *remain;
	GdaSqlParser *parser;

	parser = (GdaSqlParser *)g_object_get_data (G_OBJECT (cnc), "parser");
	stmt = gda_sql_parser_parse_string (parser, sql, &remain, &error);
	if (remain)
		g_print ("REMAINS: %s\n", remain);

	nrows = gda_connection_statement_execute_non_select (cnc, stmt, NULL, NULL, &error);
	if (nrows == -1)
		g_error ("NON SELECT error: %s\n", error && error->message ? error->message : "no detail");
	g_object_unref (stmt);
}

#endif


void xpn_register_var_write_data(xpn_register_var *self, gboolean first, gboolean last)
{
	GError *error = NULL;

	xpn_register_var_clear_counter(self);
	// in TXT Datei speichern, falls akt.
	if (self->txtfile!=NULL)
		{
			if (first==TRUE)
				{
					self->txtout = fopen(self->txtfile,"w");
					xpn_register_var_write_header(self,self->txtout);
					xpn_register_var_write_txt_row(self,self->txtout);
					fflush(self->txtout);
					fprintf(stdout,"<TXTOUTPUT: '%s'>\n",self->txtfile);
					fflush(stdout);
				}
			else if (last==TRUE)
				{
					xpn_register_var_write_txt_row(self,self->txtout);
					fflush(self->txtout);
					fclose(self->txtout);
				}
			else
				{
					xpn_register_var_write_txt_row(self,self->txtout);
					fflush(self->txtout);
				}
		}
	// in Datenbank schreiben, falls akt:
#ifndef XPN_WITHOUT_DATABASE
	if ((self->database_provider!=NULL) && (self->database_string!=NULL))
		{
			if (first==TRUE)
				{
					self->cnc = gda_connection_open_from_string (self->database_provider,self->database_string , NULL,
					            GDA_CONNECTION_OPTIONS_NONE,
					            &error);
					if (!self->cnc)
						{
							g_print ("Could not open connection to Database: %s\n",
							         error && error->message ? error->message : "No detail");
							exit (1);
						}
					/* create an SQL parser */
					self->parser = gda_connection_create_parser (self->cnc);
					if (!self->parser) /* @cnc doe snot provide its own parser => use default one */
						self->parser = gda_sql_parser_new ();

					run_sql_non_select(self->cnc,"DROP table IF EXISTS expertn");
					run_sql_non_select(self->cnc,"CREATE table expertn");

					xpn_register_var_write_db(self);


				}
			else if (last==TRUE)
				{
					xpn_register_var_write_db(self);
				}
			else
				{
					xpn_register_var_write_db(self);
				}

		}
#endif
}
/*
void xpn_register_var_load_expertn_db(xpn_register_var *self,PTIME pTi, PCLIMATE pCl, PCHEMISTRY pCh, PGRAPHIC pGr, PHEAT pHe, PLOCATION pLo, PMANAGEMENT pMa, PPLANT pPl, PSPROFILE pSo, PWATER pWa)
{
   GdaConnection *cnc;
   GError *error = NULL;
   GdaSqlParser *parser;
   GdaStatement *stmt;
   gint nrows;
   const gchar *remain;
   //gchar *sql = "SELECT Projektname FROM Projekt";
   //gchar *sql = "SELECT Projektname, AutoTeilschlag, DatumStart, DatumEnde, Betriebsnummer, Betriebsname, Jahr, Schlagnummer, Schlagname, Teilschlagnummer, Teilschlagname, Fitting FROM Projekt";

   gchar *sql = "SELECT NameDuenger FROM Tabelledüngermineralisch";
   GdaDataModel *data_model;
   const GValue *val;;


   gda_init ();

	//cnc = gda_connection_open_from_string ("SQLite", "DB_NAME=expertn", NULL,
	//				       GDA_CONNECTION_OPTIONS_NONE,
	//				       &error);
 //  cnc = gda_connection_open_from_string ("MSAccess", "DB_DIR=.;DB_NAME=expertn", NULL,
	//				       GDA_CONNECTION_OPTIONS_NONE,
	//				       &error);

    if (!cnc) {
               g_print ("Could not open connection to SQLite database in ExpertNOutput.db file: %s\n",
                         error && error->message ? error->message : "No detail");
                exit (1);
        }*/
/* create an SQL parser */
//  parser = gda_connection_create_parser (cnc);
//   if (!parser) /* @cnc doe snot provide its own parser => use default one */
//		parser = gda_sql_parser_new ();
/* attach the parser object to the connection */
/*    g_object_set_data_full (G_OBJECT (cnc), "parser", parser, g_object_unref);

    stmt = gda_sql_parser_parse_string (parser, sql, NULL, NULL);
    data_model = gda_connection_statement_execute_select (cnc, stmt, NULL, &error);

    g_object_unref (stmt);
    if (!data_model)
                g_error ("Could not get the contents of the 'products' table: %s\n",
                         error && error->message ? error->message : "No detail");

    gda_data_model_dump (data_model, stdout);

    val = gda_data_model_get_typed_value_at(data_model,0,0,G_TYPE_STRING,FALSE,NULL);




	//printf("Test: %f\n",g_value_get_double(val));
	printf("Test: %s\n",g_value_get_string(val));

	g_object_unref (data_model);
}*/


void  xpn_register_var_add_filter_list_file(xpn_register_var *self,const char* filename)
{
	FILE *f;
	char *S;
	if (filename ==NULL)  return;
	f = fopen(filename,"r");
	if (f==NULL)
		{
			S = (char*)malloc(strlen(filename)+200);
			sprintf(S,"Error: xpn_register_var_add_filter_list_file, could not open file %s\n",filename);
			fprintf(stderr,"%s",S);
			free(S);
			return;
		}
	while (1)
		{
			S=fgets_mem(f);
			if ((feof(f) != 0) && (S==NULL))
				{
					break;
				}
			if (S!=NULL)
				{
					self->str_filter = xpn_register_var_add_filter(self->str_filter,&(self->str_filter_len),S);
					free(S);
				}

		}
	fclose(f);
}



// -------------------------------------------------------------- string functions -----------------------------------------------------------------------------------------


int getStringColumn(char *dest,char *source,char *seperator,int column)
{
	char *pos;
	char *oldpos;
	int i;
	pos=source-strlen(seperator);;
	oldpos=pos;

	for (i=0; i!=column+1; i++)
		{
			oldpos=pos+strlen(seperator);
			pos = strstr(oldpos, seperator);
			if (pos==NULL)
				{
					if (column==i)
						{
							strncpy(dest, oldpos,strlen(oldpos));
							dest[strlen(oldpos)]=0;
							return 0;
						}
					else
						{
							dest=NULL;
							return 1;
						}
				}

		}
	strncpy(dest,oldpos,(int)(pos-oldpos));
	dest[(int)(pos-oldpos)]=0;
	return 0;
}

int getStringColumn_get_Size(char *source,char *seperator)
{
	int i;
	char *S2;
	S2 = (char*)malloc(strlen(source)+1);
	for (i=0;; i++)
		{
			if (getStringColumn(S2,source,seperator,i)!=0)
				{
					free(S2);
					return i;
				}
		}
	free(S2);
	return 0;
}

char *fgets_mem(FILE *stream)
{
	char *S; //memBuffer
	char *S2;
	unsigned int lenS;//reserved Memory
	unsigned int stdlenS;//momemory allocation buffer size
	int ready;
	int i;
	char ch;

	stdlenS=128;
	lenS=stdlenS;
	S=(char*)malloc(lenS);
	//lenSCount=0;
	ready=0;
	i=0;
	ch=0;

	while (ready==0)
		{
			ch=fgetc(stream);
			if ((ch==EOF) && (i==0))
				{
					free(S);
					S=NULL;
					return NULL;
				}
			if (i>=(int)lenS-2)
				{
					lenS+=stdlenS;
					S=(char*)realloc(S,lenS);
					if (S==NULL)
						{
							return NULL;
						}
				}
			if (ch==13)
				{
					i--;
				}
			else if (ch==10)
				{
					ready=1;
				}
			else if (ch==EOF)
				{
					ready=1;
				}
			else
				{
					S[i]=ch;
				}
			i++;
		}
	S[i-1]=0;
	S2=(char*)malloc(strlen(S)+1);
	strcpy(S2,S);
	free(S);
	return S2;
}

void deleteSpaceBegEnd (char *str)
{
	char *str_temp;
	if ((str == NULL) || (strlen(str)==0)) return;
	str_temp = (char*)malloc(strlen(str)+1);
	while (str[0]==' ')
		{
			strcpy(str_temp,str);
			strcpy(str,&(str_temp[1]));
		}
	while (str[strlen(str)-1]==' ')
		{
			str[strlen(str)-1]=0;
		}
	free(str_temp);
}


static int strcmp_extra_nextstr(int *i,const char *str2, const char *str1)
{
	//int i;
	int i2;
	int str1_len_1;
	(*i) = 0;
	str1_len_1 = strlen(str1)-1;
	//printf ("%s,%s\n",str2,str1);
	for (i2=0; i2!=strlen(str2); i2++)
		{
			if (str1[*i] == '*') return i2;
			if (*i >= str1_len_1) return i2+1;
			if ((str2[i2] == str1[(*i)]) || (str1[(*i)] == '?'))
				{
					(*i)++;
				}
			else
				{
					(*i)=0;
				}
		}
	return -1;
}

// special strcmp func with asterisk and wildcards
// ret: 0  if str are equal
// ret: 1  if str are not equal
// ret: -1 any error
// example:
// ret = strcmp_extra("Der kleine Hund","*kl?ine*");
// ret is 0
int strcmp_extra(const char* srcstr,const char*cmpstr)
{
	int srcstr_len,cmpstr_len;
	int i,i2,i_add;
	int switch_ast;
	int next;

	if ((srcstr==NULL) || (cmpstr==NULL))
		{
			return -1;
		}

	srcstr_len = strlen(srcstr);
	cmpstr_len = strlen(cmpstr);

	if (srcstr_len < cmpstr_len)
		{
			return 1;
		}

	i2 = 0;
	switch_ast = 0;
//	switch_w   = 0;
	for (i = 0; i < srcstr_len-1; i++)
		{
			if (i2 > cmpstr_len-1)
				{
					return 1;
				}

			switch (cmpstr[i2])
				{
				case '*' :
					switch_ast = 1;
					i2++;
					i--;
					if (i2 > cmpstr_len-1)
						{
							return 0;
						}
					break;
				case '?' :  //switch_w = 1;
					i2 ++;
					break;
				default  :
					if (switch_ast==0)
						{
							if (cmpstr[i2] != srcstr[i]) return 1;
							i2 ++;
						}
					else
						{
							next = strcmp_extra_nextstr(&i_add,&(srcstr[i]),&(cmpstr[i2]));
							if (next==-1) return 1;
							i+=next-2;
							i2+=i_add;
							switch_ast=0;
						}
				}
		}

	return 0;
}

char* delete_last_part_of_path(const char *_path)
{
	int i;
	char *path;

	if (_path==NULL) return NULL;

	path = (char*) calloc(1,strlen(_path)+1);
	strcpy(path,_path);

	if (strlen(path) < 2) return path;
	for (i=strlen(path)-1; i > -1; i --)
		{
			if ((path[i]=='/') || (path[i]=='\\'))
				{
					path[i]=0;
					return path;
				}
		}
	return path;
}

char* delete_first_part_of_path(const char *_path)
{
	int i,pos;
	char *path;

	if (_path==NULL) return NULL;



	pos = 0;
	for (i=0; i<strlen(_path); i++)
		{
			if ((_path[i]=='/') || (_path[i]=='\\'))
				{
					pos = i+1;
					break;
				}
		}

	path = (char*) calloc(1,strlen(&(_path[pos]))+1);
	strcpy(path,&(_path[pos]));

	return path;
}

// return 0 --> is in path;  1 --> not at first place
int cmp_first_part_of_path(const char* path, const char* part)
{
	int i;
	int result;
	result = 0;

	if ((path==NULL) || (part==NULL) || (strlen(path) < strlen(part))) return 1;
	for (i=0; i<strlen(part); i++)
		{
			if (path[i]!=part[i])
				{
					result=1;
					break;
				}
		}
	return result;
}


// Erzeugt einen gültigen Pfad aus einen Basic und einem relativen Pfad
// Ist der relative Pfad schon ein direkter, so wird lediglich eine Kopie zurückgegeben
// z.B.: path = get_fullpath_from_relative("/windows/chrikle","../hallo");
// ergibt: "/windows/hallo"
// Ergnisstring muss mit free deallokiert werden
char *get_fullpath_from_relative(const char *_basepath, const char* _relativepath)
{
	char *result;
	char *relativepath,*basepath;
	int i;

	if (_relativepath==NULL) return NULL;

	relativepath = (char*)calloc(1,strlen(_relativepath)+2);
	strcpy(relativepath,_relativepath);

	// Alle '\' werden durch '/' ersetzt:
	for (i=0; i<strlen(relativepath); i++)
		{
			if (relativepath[i]=='\\')
				{
					relativepath[i]='/';
				}
		}


	result = NULL;
	if ((relativepath[1]==':') || (relativepath[0]=='/') || (_basepath==NULL) || (strlen(_basepath)==0))
		{
			result = (char*)malloc(strlen(relativepath)+1);
			strcpy(result,relativepath);
			free(relativepath);
			return result;
		}

	basepath = (char*)calloc(1,strlen(_basepath)+2);
	strcpy(basepath,_basepath);

	// Alle '\' werden durch '/' ersetzt:
	for (i=0; i<strlen(basepath); i++)
		{
			if (basepath[i]=='\\')
				{
					basepath[i]='/';
				}
		}
	if (basepath[strlen(basepath)-1]=='/') basepath[strlen(basepath)-1]=0;



	while(1)
		{
			char *nrelpath,*nbasepath;
			if (cmp_first_part_of_path(relativepath,"../")==1)
				{
					break;
				}

			nbasepath = delete_last_part_of_path(basepath);
			free(basepath);
			basepath=nbasepath;

			nrelpath = delete_first_part_of_path(relativepath);
			free(relativepath);
			relativepath=nrelpath;
		}

	result=(char*)calloc(1,strlen(relativepath)+strlen(basepath)+2);

	sprintf(result,"%s/%s",basepath,relativepath);

	free(relativepath);
	return result;
}


char* get_filename_without_path(const char *filename)
{
	char *filename2,*filename3;
	char *S;
	int i;
	int len;
	filename2 = (char*)calloc(1,strlen(filename)+2);
	strcpy(filename2,filename);

	// Alle '\' werden durch '/' ersetzt:
	for (i=0; i<strlen(filename2); i++)
		{
			if (filename2[i]=='\\')
				{
					filename2[i]='/';
				}
		}
	len = getStringColumn_get_Size(filename2,"/");
	S = (char*)calloc(1,strlen(filename2)+1);
	getStringColumn(S,filename2,"/",len-1);
	filename3 = (char*)calloc(1,strlen(S)+1);
	strcpy(filename3,S);
	free(filename2);
	free(S);
	return filename3;
}

char* get_path_from_filename(const char*filename)
{
	char *filename2,*filename3,*S,*S2;
	int i,len;
	filename2 = (char*)calloc(1,strlen(filename)+2);
	strcpy(filename2,filename);

	// Alle '\' werden durch '/' ersetzt:
	for (i=0; i<strlen(filename2); i++)
		{
			if (filename2[i]=='\\')
				{
					filename2[i]='/';
				}
		}
	filename3 = (char*)calloc(1,strlen(filename)+2);
	S = (char*)calloc(1,strlen(filename)+2);
	S2 = (char*)calloc(1,strlen(filename)+2);
	len = getStringColumn_get_Size(filename2,"/");
	for (i=0; i<len-1; i++)
		{
			getStringColumn(S,filename2,"/",i);
			if (i==0)
				{
					sprintf(S2,"%s",S);
				}
			else
				{
					sprintf(S2,"/%s",S);
				}
			strcat (filename3,S2);
		}
	free(S);
	free(S2);
	free(filename2);
	return filename3;
}

char *get_filename_without_extension(const char *filename)
{
	int len;
	char *S,*S2;
	len = getStringColumn_get_Size((char*)filename,".");
	S = (char*)calloc(1,strlen(filename)+1);
	getStringColumn(S,(char*)filename,".",len-2);
	S2 = (char*)calloc(1,strlen(S)+1);
	strcpy(S2,S);
	free(S);
	return S2;
}


void* xpn_register_var_get_pointer(xpn_register_var *self,char *varname)
{
	int i;
	for (i=0; i<self->size; i++)
		{
			if (strcmp(self->vars[i]->varname,varname)==0)
				{
					if  ((self->vars[i]->flag_pointer==TRUE) && (self->vars[i]->vartype==G_TYPE_NONE))
						{
							return self->vars[i]->val.n;
						}
				}
		}
	return NULL;
}


int xpn_register_var_get_pointer_convert_to_int(xpn_register_var *self,char *varname, int standartvalue)
{
	char *S;
	S = (char*)xpn_register_var_get_pointer(self,varname);
	if (S==NULL)
		{
			fprintf(stderr,"ERROR: Varname is not defined (%s), take default value (%d)!\n",varname,standartvalue);
			return  standartvalue;
		}
	else
		{
			return atoi(S);
		}

}

double xpn_register_var_get_pointer_convert_to_double(xpn_register_var *self,char *varname, double standartvalue)
{
	char *S;
	S = (char*)xpn_register_var_get_pointer(self,varname);
	if (S==NULL)
		{
			fprintf(stderr,"ERROR: Varname is not defined (%s), take default value (%f)!\n",varname,standartvalue);
			return  standartvalue;
		}
	else
		{
			return atof(S);
		}

}

float xpn_register_var_get_pointer_convert_to_float(xpn_register_var *self,char *varname, float standartvalue)
{
	char *S;
	S = (char*)xpn_register_var_get_pointer(self,varname);
	if (S==NULL)
		{
			fprintf(stderr,"ERROR: Varname is not defined (%s), take default value (%f)!\n",varname,(double)standartvalue);
			return  standartvalue;
		}
	else
		{
			return (float)atof(S);
		}

}

char** xpn_register_var_get_pointer_convert_to_array_string(xpn_register_var *self,char *varname,int *size)
{
	char *S,*S2;
	char **SArray;
	int i;
	S = (char*)xpn_register_var_get_pointer(self,varname);
	if (S==NULL)
		{
			fprintf(stderr,"ERROR: Varname is not defined (%s)!\n",varname);
			*size = 0;
			return  NULL;
		}
	else
		{
			*size  = getStringColumn_get_Size(S,";");
			S2 = (char*) malloc (strlen(S)+1);
			SArray = (char**)calloc(*size,sizeof(char*));
			for (i=0; i<(*size); i++)
				{
					getStringColumn(S2,S,";",i);
					SArray[i] = (char*) malloc(strlen(S2)+1);
					strcpy(SArray[i],S2);
				}
			free(S2);
		}
	return SArray;
}

void  xpn_register_var_free_string_list(char **strlist,int size)
{
	int i;
	if (strlist==NULL) return;
	for (i=0; i<size; i++)
		{
			free(strlist[i]);
		}
	free(strlist);
}

int* xpn_register_var_get_pointer_convert_to_array_int(xpn_register_var *self,char *varname,int *size)
{
	char **StrArray;
	int *intarray;
	int i;
	StrArray = xpn_register_var_get_pointer_convert_to_array_string(self,varname,size);
	if (StrArray==NULL) return NULL;
	{
		intarray = (int*) malloc(sizeof(int)*(*size));
		for (i=0; i< (*size); i++)
			{
				intarray[i] = atoi(StrArray[i]);
			}
		xpn_register_var_free_string_list(StrArray,*size);
	}
	return intarray;
}

G_MODULE_EXPORT void  xpn_register_var_free_int_list(int *intlist)
{
	if (intlist==NULL) return;
	free(intlist);
}

double* xpn_register_var_get_pointer_convert_to_array_double(xpn_register_var *self,char *varname,int *size)
{
	char **StrArray;
	double *array;
	int i;
	StrArray = xpn_register_var_get_pointer_convert_to_array_string(self,varname,size);
	if (StrArray==NULL) return NULL;
	{
		array = (double*) malloc(sizeof(double)*(*size));
		for (i=0; i< (*size); i++)
			{
				array[i] = atof(StrArray[i]);
			}
		xpn_register_var_free_string_list(StrArray,*size);
	}
	return array;
}


void  xpn_register_var_free_double_list(double *list)
{
	if (list==NULL) return;
	free(list);
}

gchar *convert_fortran_to_c_string(char* fortran_str,int str_len)
{
	char *S,*S2;
	int i;
	S = (char*)g_malloc0(str_len*sizeof(char));
	for (i=0; i<str_len; i++)
		{
			S[i] = fortran_str[i];
		}
	S[str_len-1]=0;
	while (S[strlen(S)-1]==' ')
		{
			S[strlen(S)-1]=0;
		}

	S2 = (char*)g_malloc0(strlen(S)+1);
	strcpy(S2,S);
	g_free(S);
	return S2;
}

// Replace a string with a given template
// Example:
// char *res = eplace_str_with_template("Der kleine $Tier ging in den Wald und der $Tier war dort!","$Tier","Hund");
// free(res);
char* replace_str_with_template(char *source,char* _template,char* expr)
{
	int i,len;

	char *S2;
	char *resstr;
	if ((source==NULL) || (_template==NULL)) return NULL;
	resstr = NULL;

	len = getStringColumn_get_Size(source,_template);

	S2 = (char*)malloc(strlen(source)+1);
	resstr = (char*)malloc(strlen(source)+1);

	getStringColumn(resstr,source,_template,0);

	for (i=1; i<len; i++)
		{
			getStringColumn(S2,source,_template,i);
			resstr = (char*)realloc(resstr,strlen(resstr)+strlen(S2)+strlen(expr)+1);
			strcat(resstr,expr);
			strcat(resstr,S2);
		}

	if (S2!=NULL)
		{
			free(S2);
		}

	return resstr;
}

// Replace a string with a given templates
// Example:
// char *S = "Der kleine $test geht in den $Platz, dabei passiert $test folgendes!";
// char *Templates[2] = {"$test","$Platz"};
// char *expr[2] = {"Christian","Garten"};
// char *res = replace_str_with_templates(S,Templates,expr,2);
// free(res);
char* replace_str_with_templates(char *source,char** _templates,char** exprs,int templ_len)
{
	int i;
	char *resstr,*resstr2;
	if (source==NULL) return NULL;
	resstr=(char*)malloc(strlen(source)+1);
	strcpy(resstr,source);
	for (i=0; i<templ_len; i++)
		{
			resstr2 = replace_str_with_template(resstr,_templates[i],exprs[i]);
			free(resstr);
			resstr = resstr2;
		}

	return resstr;
}

char* get_formated_date(int _date,gboolean MS_ACCESS_COMP)   //_date:  DDMMYY
{
	int days, month, year;
	days = _date/10000;
	_date-=days*10000;
	month = _date/100;
	_date-=month*100;
	if (_date > 70)
		{
			year = _date+1900;
		}
	else
		{
			year = _date+2000;
		}
	if (MS_ACCESS_COMP==TRUE)
		{
			return g_strdup_printf("%04d-%02d-%02d 00:00:00+0",year,month,days);
		}
	else
		{
			return g_strdup_printf("%04d-%02d-%02d",year,month,days);
		}


}

int get_expertn_date(char *_date)
{
	int date;
	char cday[3],cmonth[3],cyear[3];
	if (strlen(_date)<10) return 0;
	cday[0]=_date[0];
	cday[1]=_date[1];
	cday[2]=0;
	cmonth[0]=_date[3];
	cmonth[1]=_date[4];
	cmonth[2]=0;
	cyear[0]=_date[8];
	cyear[1]=_date[9];
	cyear[2]=0;
	date = atoi (cday)*10000 + atoi (cmonth) * 100 + atoi (cyear);
	return date;
}

struct tm convert_date_to_unix_time(int _date)   // _date: DDMMYY
{
	struct tm ti;
	int days, month, year;
	days = _date/10000;
	_date-=days*10000;
	month = _date/100;
	_date-=month*100;
	if (_date > 70)
		{
			year = _date+1900;
		}
	else
		{
			year = _date+2000;
		}
	//ti.tm_zone=0;
	//ti.tm_gmtoff=0;
	ti.tm_wday=0;
	ti.tm_yday=0;
	ti.tm_isdst=0;
	ti.tm_sec=0;
	ti.tm_min=0;
	ti.tm_hour=0;
	ti.tm_mday = days+1;
	ti.tm_mon = month-1;
	ti.tm_year = year-1900;
	return ti;
}

int convert_date_to_days(int _date)   // _date: DDMMYY
{
	struct tm ti;
	ti = convert_date_to_unix_time(_date);
	return (int) mktime(&ti)/(60*60*24); // von sek in Tage
}

int conv_gdate_to_xpn_date(GDate *_date)
{
	int date,year;
	date = g_date_get_day(_date)*10000 + (g_date_get_month(_date)) * 100;
	year = g_date_get_year(_date);
	if (year < 2000)
		{
			date+=year-1900;
		}
	else
		{
			date+=year-2000;
		}
	return date;
}
//return DDMMYY

int get_expertn_date_from_unix(int unixtime) //der sec seit 1900;
//return DDMMYY
{
	GDate date;
	g_date_set_time_t (&date,(time_t)unixtime);
	return conv_gdate_to_xpn_date(&date);
}

int get_date_before_day(int _date)
{
	struct tm ti;
	int sec;
	ti = convert_date_to_unix_time(_date);
	sec = (int) mktime(&ti);
	sec-=(24*60*60)*2;
	return get_expertn_date_from_unix(sec);
}

int get_date_after_day(int _date)
{
	struct tm ti;
	int sec;
	ti = convert_date_to_unix_time(_date);
	sec = (int) mktime(&ti);
	//sec+=24*60*60;
	return get_expertn_date_from_unix(sec);
}

int is_leap_year(int year)
{
	// Die Regel lautet: Alles, was durch 4 teilbar ist, ist ein Schaltjahr.
	// Es sei denn, das Jahr ist durch 100 teilbar, dann ist es keins.
	// Aber wenn es durch 400 teilbar ist, ist es doch wieder eins.

	if ((year % 400) == 0)
		return TRUE;
	else if ((year % 100) == 0)
		return FALSE;
	else if ((year % 4) == 0)
		return TRUE;

	return FALSE;
}

int get_DaysPerMonth(int month, int year)
{
	//                     ungült,Jan,Feb,Mrz,Apr,Mai,Jun,Jul,Aug,Sep,Okt,Nov,Dez
	int arrTageImMonat[13] = {  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (month == 2)
		{
			// Februar: Schaltjahr unterscheiden
			if (is_leap_year(year))
				return 29;
			else
				return 28;
		}

	if ((month >= 1) && (month <= 12))
		return arrTageImMonat[month];
	return 0;
}

int get_julianday(int day, int month, int year)
{
// Der wievielte Tag des Jahres ist dieser Tag
	int uLokalTag = day;
	int uLokalMonat = month;
	if ((month == 0) || (month > 12))
		{
			return -1;
		}

	while (uLokalMonat > 1)
		{
			uLokalMonat--;
			uLokalTag += get_DaysPerMonth(uLokalMonat, year);
		}

	return uLokalTag;

}

int get_expertn_JulianDay(int _date)   //date
{
	int days, month, year;
	days = _date/10000;
	_date-=days*10000;
	month = _date/100;
	_date-=month*100;
	if (_date > 70)
		{
			year = _date+1900;
		}
	else
		{
			year = _date+2000;
		}


	return get_julianday(days, month, year);
}


GDate *convert_str_to_gdate(char* S)
{
	char yearstr[5],monthstr[3],daystr[3];
	int i;
	int day,month,year;
	//printf("%s\n",S);
	for (i=0; i<4; i++)
		{
			yearstr[i] = S[i];
		}
	yearstr[4]=0;
	monthstr[0]=S[5];
	monthstr[1]=S[6];
	monthstr[2]=0;
	daystr[0]=S[8];
	daystr[1]=S[9];
	daystr[2]=0;

	//printf("%s	%s	%s\n",yearstr,monthstr,daystr);
	day = atoi(daystr);
	month = atoi(monthstr);
	year = atoi(yearstr);
	//printf ("%i,%i,%i\n",day,month,year);
	return((GDate *)g_date_new_dmy((GDateMonth)day,(GDateMonth)month,(GDateMonth)year)) ;

}

void convert_str_to_xpn_date(STRUCT_XPN_DATE *xpn_date,char* S)
{
	char yearstr[5],monthstr[3],daystr[3];
	int i;
	//printf("%s\n",S);
	for (i=0; i<4; i++)
		{
			yearstr[i] = S[i];
		}
	yearstr[4]=0;
	monthstr[0]=S[5];
	monthstr[1]=S[6];
	monthstr[2]=0;
	daystr[0]=S[8];
	daystr[1]=S[9];
	daystr[2]=0;

	//printf("%s	%s	%s\n",yearstr,monthstr,daystr);
	xpn_date->mday = atoi(daystr);
	xpn_date->mon = atoi(monthstr);
	xpn_date->year = atoi(yearstr);
}

/*
in:
 * latitude: Breitengrad in [Grad]
 * JulianDay: aktueller Tag im Jahr
*/
double calculateDayLength(double latitude, int JulianDay)   // kap. 7.3
{

	double DayLength,latitude_RAD;
	double theta_year; // Jahreswinkel [RAD]
	double theta_sol_dec; //  Solar Declination [rad]
	double c1ty,c2ty,c3ty,c4ty,c5ty,c6ty;
	double s1ty,s2ty,s3ty,s4ty,s5ty;



	latitude_RAD = latitude * (M_PI / 180.0);
	theta_year = ((double)JulianDay - 80.0) * (2.0 * M_PI / 365); // 7.2a Jahreswinkel ist 0 am 21 März
	c1ty = cos(theta_year);
	c2ty = cos(2.0*theta_year);
	c3ty = cos(3.0*theta_year);
	c4ty = cos(4.0*theta_year);
	c5ty = cos(5.0*theta_year);
	c6ty = cos(6.0*theta_year);
	s1ty = sin(theta_year);
	s2ty = sin(2.0*theta_year);
	s3ty = sin(3.0*theta_year);
	s4ty = sin(4.0*theta_year);
	s5ty = sin(5.0*theta_year);

	theta_sol_dec = (M_PI / 180.0) * (0.38092 - 0.76996*c1ty + 23.26500*s1ty
	                                  + 0.36958*c2ty + 0.10868*s2ty + 0.01834*c3ty
	                                  - 0.16650*s3ty - 0.00392*c4ty + 0.00072*s4ty
	                                  - 0.00051*c5ty + 0.00250*s5ty + 0.00442*c6ty);  // 7.2b

	DayLength = (1.0/M_PI)*acos(-tan(latitude_RAD)*tan(theta_sol_dec));

	return DayLength;
}



int get_numb_of_days(int year)   // Anzahl der Tage im Jahr:
{
	if (is_leap_year(year)==TRUE)
		{
			return 366;
		}
	else
		{
			return 365;
		}
}

long double c_mktime(struct tm *timeptr)
{

//	return (long double)mktime(timeptr);

	long double d;
	int year,i;
	d = 0.0;
	d += timeptr->tm_sec;
	d += timeptr->tm_min*60.0;
	d += (timeptr->tm_hour-1)*(60.0*60.0);
	d += (get_julianday(timeptr->tm_mday,timeptr->tm_mon+1,timeptr->tm_year)-1)*(60.0*60.0*24.0);
	year = timeptr->tm_year-70;
	if (year < 0)
		{
			for (i=0; i>year; i--)
				{
					d -= get_numb_of_days(1971+i)*(60.0*60.0*24.0);
				}
		}
	else
		{
			for (i=0; i<year; i++)
				{
					d += get_numb_of_days(1971+i)*(60.0*60.0*24.0);
				}
		}

	return d;

}


double get_infinity()
{
	double d1,d2;
	d1=1.0;
	d2=0.0;
	return d1 / d2;
}

double xpn_time_conv_time_systime(STRUCT_XPN_TIME *t)
{
	struct tm timeptr;

	timeptr.tm_sec = t->sec;
	timeptr.tm_min = t->min;
	timeptr.tm_hour = t->hour;
	timeptr.tm_mday = t->mday;
	timeptr.tm_mon = t->mon-1;
	timeptr.tm_year = t->year- 1900;
	return c_mktime(&timeptr);
}

void xpn_time_conv_gdate_to_xpn_time(GDate *_date,int *iYear, double *fTimeY)
{
	STRUCT_XPN_TIME t;
	t.msec = 0.0;
	t.sec = 0;
	t.min = 0;
	t.hour = 0;
	t.mday = g_date_get_day(_date);
	t.mon = g_date_get_month(_date);
	t.year = g_date_get_year(_date);
	xpn_time_set_struc(&t,iYear,fTimeY);
}

void xpn_time_conv_xpn_time_to_gdate(GDate *_date,int iYear, double fTimeY)
{
	STRUCT_XPN_TIME t;
	xpn_time_get_struc(&t,iYear,fTimeY);
	g_date_clear(_date,1);
	g_date_set_dmy (_date,(GDateDay)t.mday,(GDateMonth)t.mon,(GDateYear)t.year);
}

// 1: >
// -1: <
// 0: ==
int xpn_time_compare_date(int year1,int month1,int day1, int year2,int month2,int day2)
{

	if (year1>year2) return 1;
	if (year1<year2) return -1;
	if (month1>month2) return 1;
	if (month1<month2) return -1;
	if (day1 >day2) return 1;
	if (day1 < day2) return -1;
	return 0;
}



// 1: >
// -1: <
// 0: ==
int xpn_time_compare_time(int iYear1, double fTimeY1,int iYear2, double fTimeY2)
{
	if (iYear1>iYear2) return 1;
	if (iYear1<iYear2) return -1;
	if (fTimeY1>fTimeY2) return 1;
	if (fTimeY1<fTimeY2) return -1;
	return 0;
}




double xpn_time_sub_time(int iYear1, double fTimeY1,int iYear2, double fTimeY2)
{
	double dt;
	int cmcase;
	int year;

	if (iYear1==iYear2)
		{
			return fTimeY1-fTimeY2;
		}

	cmcase=0;
	if ((iYear1 > iYear2) || ((iYear1 == iYear2) && (fTimeY1 > fTimeY2)))
		{
			cmcase=0;
		}
	else
		{
			int i;
			double f;
			cmcase=1;
			i = iYear2;
			iYear2 = iYear1;
			iYear1 = i;
			f = fTimeY2;
			fTimeY2 = fTimeY1;
			fTimeY1 = f;
		}

	dt	= 0.0;
	for (year=iYear2; year != iYear1+1; year++)
		{
			if (year == iYear2)
				{
					dt += (get_numb_of_days(year)-fTimeY2);
				}
			else if (year == iYear1)
				{
					dt += fTimeY1;
				}
			else
				{
					dt += get_numb_of_days(year);
				}
		}

	if (cmcase==0)
		{
			return dt;
		}
	else
		{
			return -dt;
		}
	return 0.0;
}

double xpn_time_interpolate_linear(double iYear1,double fTimeY1,double fvalue1 ,double iYear2,double fTimeY2,double fvalue2, int iYear,double fTimeY)
{
	double x2;
	double x;
	double m;
	x2 = xpn_time_sub_time(iYear2,fTimeY2,iYear1,fTimeY1);
	x = xpn_time_sub_time(iYear,fTimeY,iYear1,fTimeY1);
	m = (fvalue2 - fvalue1) / x2;
	return m * x + fvalue1;
}

// dt [day]
void xpn_time_date_add_dt(int *year1,int *month1,int *day1, int dt)
{
	int days_per_month;
	if (dt > 0)
		{
			(*day1) += dt;
			while (1)
				{
					days_per_month = get_DaysPerMonth(*month1,*year1);
					if ((*day1) > days_per_month)
						{
							(*day1) -= days_per_month;
							(*month1) ++;
							if ((*month1) > 12)
								{
									(*month1) -= 12;
									(*year1) ++;
								}

						}
					else
						{
							break;
						}
				}
		}
	else if (dt < 0)
		{
			(*day1) += dt;
			while (1)
				{
					if ((*day1) < 1)
						{
							(*month1) --;
							if ((*month1) < 1)
								{
									(*year1)--;
									(*month1) = 12;
								}
							days_per_month = get_DaysPerMonth(*month1,*year1);
							(*day1) = days_per_month + (*day1);
						}
					else
						{
							break;
						}
				}
		}
}

int xpn_compare_to_added_date(int iYear1, double fTimeY1, int iYear2, double fTimeY2,double dt2)
{
	xpn_time_add_dt(&iYear2,&fTimeY2,dt2);
	return xpn_time_compare_time(iYear1,fTimeY1,iYear2,fTimeY2);
}



void xpn_time_get_struc(STRUCT_XPN_TIME *str_time, int iYear, double fTimeY)
{
	int month;
	double number_per_month,ftimeday,intpart;
	double days;
	str_time->year = iYear;
	month = 1;
	days = 0.0;
	while (1)
		{
			number_per_month = get_DaysPerMonth(month,iYear);
			days += number_per_month;
			if (days>fTimeY) break;
			month++;
		}
	days -= number_per_month;
	str_time->mon = month;
	fTimeY -= days;
	str_time->mday = (int) fTimeY+1;
	ftimeday = modf(fTimeY,&intpart);
	str_time->hour = (int)(ftimeday*24.0);
	ftimeday -= (double) str_time->hour/ 24.0;
	str_time->min = (int)(ftimeday*24.0*60);
	ftimeday -= (double)str_time->min/(24.0*60.0);
	str_time->sec = (int)(ftimeday*24.0*60.0*60.0);
	ftimeday -= str_time->sec/(24.0*60.0*60.0);
	str_time->msec = ftimeday*24.0*60.0*60.0*1000.0;
	str_time->is_leap_year = is_leap_year(iYear);

}

void xpn_time_set_struc(STRUCT_XPN_TIME *str_time, int *iYear, double *fTimeY)
{
	double ftime;
	int i;

	*iYear = str_time->year;
	ftime = 0.0;

	// days:
	for (i=1; i< str_time->mon; i++)
		{
			ftime += get_DaysPerMonth(i,*iYear);
		}

	ftime += (str_time->mday-1) + str_time->hour/24.0 + str_time->min/(24.0*60.0) + str_time->sec/(24.0*60.0*60.0) + str_time->msec/(24.0*60.0*60.0*1000.0);

	*fTimeY = ftime;
}

void xpn_time_add_dt(int *iYear, double *fTimeY,double dt)
{
	double number_of_days;
	if (dt > 0.0)
		{
			*fTimeY += dt;
			while (1)
				{
					number_of_days = (double) get_numb_of_days(*iYear);
					if (*fTimeY >= number_of_days)
						{
							(*iYear) ++;
							*fTimeY -= number_of_days;
						}
					else
						{
							break;
						}

				}
		}
	else if (dt < 0.0)
		{
			*fTimeY += dt;
			while (1)
				{
					if (*fTimeY < 0.0)
						{
							number_of_days = (double) get_numb_of_days(*iYear);
							(*iYear) --;
							*fTimeY += number_of_days;
						}
					else
						{
							break;
						}

				}
		}

}

void xpn_time_sub_dt(int *iYear, double *fTimeY,double dt)
{
	xpn_time_add_dt(iYear, fTimeY,-dt);
}

double xpn_time_get_number_of_days(int iYear1, double fTimeY1,int iYear2, double fTimeY2)
{
	int schaltjahre1, schaltjahre2;
	double days1, days2;
	double days;

	schaltjahre1 = (int)(iYear1/4) - (int)(iYear1/100) + (int)(iYear1/400);
	schaltjahre2 = (int)(iYear2/4) - (int)(iYear2/100) + (int)(iYear2/400);
	days1 = (double)(iYear1 - schaltjahre1)*365.0 + (double)(schaltjahre1*366) + (double)fTimeY1;
	days2 = (double)(iYear2 - schaltjahre2)*365.0 + (double)(schaltjahre2*366) + (double)fTimeY2;
	days = (double)(days2 - days1);

	return days;

}

double xpn_time_get_number_of_dates(int day1, int month1, int year1, int day2, int month2, int year2)
{
	STRUCT_XPN_TIME str_time;
	int iYear1;
	double fTimeY1;
	int iYear2;
	double fTimeY2;
	

	str_time.is_leap_year=0;
	str_time.msec = 0.0;
	str_time.sec=0;
	str_time.min=0;
	str_time.hour=0;
	str_time.mday=day1;
	str_time.mon=month1;
	str_time.year=year1;
	xpn_time_set_struc(&str_time, &iYear1, &fTimeY1);
	str_time.mday=day2;
	str_time.mon=month2;
	str_time.year=year2;
	xpn_time_set_struc(&str_time, &iYear2, &fTimeY2);
	return xpn_time_get_number_of_days(iYear1,fTimeY1,iYear2,fTimeY2);
}

char* xpn_time_get_formated_date(int iYear, double fTimeY,gboolean MS_ACCESS_COMP)
{
	STRUCT_XPN_TIME t;
	xpn_time_get_struc(&t, iYear, fTimeY);

	if (MS_ACCESS_COMP==TRUE)
		{
			return g_strdup_printf("%04d-%02d-%02d 00:00:00+0",t.year,t.mon,t.mday);
		}
	else
		{
			return g_strdup_printf("%04d-%02d-%02d",t.year,t.mon,t.mday);
		}


}

char* xpn_time_get_formated_date_and_time(int iYear, double fTimeY)
{
	STRUCT_XPN_TIME t;
	xpn_time_get_struc(&t, iYear, fTimeY);
	return g_strdup_printf("%04d-%02d-%02d %02d:%02d:%02d",t.year,t.mon,t.mday,t.hour,t.min,t.sec);
}


STRUCT_XNO_DATA *xpn_load_climate_xno_file(char *filename,int min_columns)
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
	STRUCT_XNO_DATA* data;
	int iYear;
	double fTimeY;

	g_date_clear(&date,1);


	f = fopen(filename ,"r");
	i = 0;
	if (f==NULL)
		{
			S = g_strdup_printf("Open Weather File %s!",filename);
			PRINT_ERROR(S);
			g_free(S);
			return NULL;
		}
	seperator = ",";
	date_format = cdb_format;
	size=0;

	while (1)
		{
			S = fgets_mem(f);
			if (S==NULL) break;
			if ((i==0) || (i==1))
				{
					size = getStringColumn_get_Size(S,seperator);
					if (size < min_columns)
						{
							seperator=";";
							size = getStringColumn_get_Size(S,seperator);
							if (size < min_columns)
								{
									seperator="\t";
									size = getStringColumn_get_Size(S,seperator);
									if (size < min_columns)
										{
											seperator=" ";
											size = getStringColumn_get_Size(S,seperator);
											if (size < min_columns)
												{
													char *S_ERROR;
													S_ERROR = g_strdup_printf("The weather file %s has the wrong number of columns %d (colon,semicolon,tabulator and space is alowed as seperator)!",filename,min_columns);
													PRINT_ERROR(S_ERROR);
													g_free(S_ERROR);
													return NULL;
												}
										}
								}
						}
				}
			S2 = (char*)g_malloc0(strlen(S)+1);
			date_str = (char*)g_malloc0(strlen(S)+1);
			if (i==0)
				{
					data = (STRUCT_XNO_DATA*)g_malloc0(sizeof(STRUCT_XNO_DATA));
					data->size_of_values = size;
					data->size_of_valuelist = 0;
					data->valuelist = NULL;
					data->iyear = NULL;
					data->fTimeY = NULL;
					data->header = (char**)g_malloc0(sizeof(char*)*size);
					for (i2=0; i2<size; i2++)
						{
							getStringColumn(S2,S,seperator,i2);
							S3 = (char*)g_malloc0(strlen(S2)+1);
							strcpy(S3,S2);
							data->header[i2] = S3;
						}
				}
			else
				{
					if (size!=data->size_of_values)
						{
							char *S_ERROR;
							S_ERROR = g_strdup_printf("The weather file %s has the wrong number of rows %d in line %d",filename,min_columns,i+1);
							PRINT_ERROR(S_ERROR);
							g_free(S_ERROR);
							break;
						}
					values =(double*)g_malloc0(sizeof(double)*size);
					for (i2=0; i2<size; i2++)
						{
							getStringColumn(S2,S,seperator,i2);
							if (i2==0)
								{
									if (i==1)
										{
											// Datums format checken:
											if (getStringColumn_get_Size(S2,"-") >= 3)
												{
													date_format=csv_minus;
												}
											else if (getStringColumn_get_Size(S2,"//") >= 3)
												{
													date_format=csv_slash;
												}
											else if (getStringColumn_get_Size(S2,".") >= 3)
												{
													date_format=csv_german;
												}
											else
												{
													date_format=cdb_format;
												}
										}
									switch(date_format)
										{
											// xno format:
										case csv_minus: // yyyy-mm-dd  hh:mm:ss
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
											if (getStringColumn(S2,S3," ",1)==0)
												{
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
										case csv_slash: // mm-dd-yyyy hh:mm:ss
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
											if (getStringColumn(S2,S3," ",1)==0)
												{
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
										case csv_german:// dd.mm.yyyy hh:mm:ss
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
											if (getStringColumn(S2,S3," ",1)==0)
												{
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

								}
							else
								{
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

	if (i < 2) return NULL;

	return data;
}

STRUCT_XNO_DATA *xpn_free_XNO_DATA(STRUCT_XNO_DATA* data)
{
	int i,i2;
	if (data!=NULL)
		{
			if (data->header!=NULL)
				{
					for (i=0; i<data->size_of_values; i++)
						{
							if (data->header[i]!=NULL)
								{
									g_free(data->header[i]);
									data->header[i]=NULL;
								}

						}
					g_free(data->header);
					data->header=NULL;
				}
			if (data->valuelist!=NULL)
				{
					for (i2=0; i2<data->size_of_valuelist; i2++)
						{
							if (data->valuelist[i2] !=NULL)
								{
									g_free(data->valuelist[i2]);
									data->valuelist[i2] = NULL;
								}
						}
					if (data->iyear!=NULL) g_free(data->iyear);
					if (data->fTimeY!=NULL) g_free(data->fTimeY);
				}
			g_free(data);
		}
	data = NULL;
	return data;
}



#if !GLIB_CHECK_VERSION(2,24,0)
gpointer g_malloc0_n (gsize n_blocks, gsize n_block_bytes)
{
	return g_malloc0( n_block_bytes*n_blocks);
}
#endif
