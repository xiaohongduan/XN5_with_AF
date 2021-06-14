//      xpn_register_var.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>



#ifndef __XPN_REGISTER_VAR_H__
#define __XPN_REGISTER_VAR_H__

#include <glib-object.h>
#include <gmodule.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef XPN_WITHOUT_DATABASE
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>
#endif


G_BEGIN_DECLS


typedef struct {
		int vartype;  // Möglich sind: G_TYPE_INT,G_TYPE_DOUBLE, G_TYPE_NONE, (G_TYPE_STRING falls es gebraucht wird)
		int line;     // Zeile (-1 : Zeile wird nicht angezeigt)
		char* varname; // Variablenname
        gboolean flag_pointer;
		gboolean flag_record;

	    union {
				int    i;
				double d;
				// float f; --> convertet to double
				int   *pi;
				double *pd;
				float  *pf;
			    void *n;	    
			  } val;
		
		} struct_variable;
		
typedef struct
{
  double msec;			/* Milli Sec */
  int sec;			/* Seconds.	[0-60] (1 leap second) */
  int min;			/* Minutes.	[0-59] */
  int hour;			/* Hours.	[0-23] */
  int mday;			/* Day.		[1-31] */
  int mon;			/* Month.	[1-12] */
  int year;			/* Year	[YYYY]   */ 

  int  is_leap_year; /* 1 or 0 */ 
} STRUCT_XPN_TIME;

typedef struct
{
  int mday;			/* Day.		[1-31] */
  int mon;			/* Month.	[1-12] */
  int year;			/* Year	[YYYY]   */ 
} STRUCT_XPN_DATE;


typedef struct
{
	int size_of_values;
	int size_of_valuelist;
	char **header;
	double **valuelist;
	int *iyear;
	double *fTimeY;
} STRUCT_XNO_DATA;

enum WEATHER_FILE_DATE_FORMAT {
    cdb_format, csv_minus,csv_dot,csv_german,csv_slash
};

#define XPN_REGISTER_VAR_TYPE				(xpn_register_var_get_type())
#define XPN_REGISTER_VAR(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			XPN_REGISTER_VAR_TYPE, xpn_register_var))
#define XPN_REGISTER_VAR_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			XPN_REGISTER_VAR_TYPE, xpn_register_varClass))
#define IS_XPN_REGISTER_VAR(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			XPN_REGISTER_VAR_TYPE))
#define IS_XPN_REGISTER_VAR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			XPN_REGISTER_VAR_TYPE))

typedef struct _xpn_register_var			xpn_register_var;
typedef struct _xpn_register_varClass		xpn_register_varClass;

struct _xpn_register_var
{
	GObject parent;

#ifndef XPN_WITHOUT_DATABASE
    char* database_provider;
    char* database_string;
    GdaConnection *cnc;
    GdaSqlParser *parser;
#endif
	int create_xno_files; // use csv(1) instead of cdb(0) files
    
    char* txtfile;
    FILE *txtout;
    
    int size; // anzahl der Var;
    int counter;
    int counter_start; // alle Vars, die nur einmal registriert werden sind vor counter_start definiert
	
	
    struct_variable **vars;
	
    char **str_filter;
    int      str_filter_len;
    
    char **str_filter_ig;
    int      str_filter_ig_len;
	
	double fTime; // fTime für cdb Ausgabe
	STRUCT_XPN_TIME xno_time; // Time and Date for xno Ausgabe
    	
};

struct _xpn_register_varClass
{
	GObjectClass parent_class;
};

G_MODULE_EXPORT GType      xpn_register_var_get_type		(void);
G_MODULE_EXPORT xpn_register_var*  xpn_register_var_new			(char* database_provider, char* output_database_string, char* output_txtfile);

G_MODULE_EXPORT void  			xpn_register_var_clear_counter(xpn_register_var *self);
G_MODULE_EXPORT void                                xpn_register_var_clear(xpn_register_var *self);
G_MODULE_EXPORT void 			xpn_register_var_write_header(xpn_register_var *self,FILE*f);
G_MODULE_EXPORT void 			xpn_register_var_write_txt_row(xpn_register_var *self,FILE*f);
G_MODULE_EXPORT void 			xpn_register_var_write_data(xpn_register_var *self, gboolean first, gboolean last);

G_MODULE_EXPORT void                               xpn_register_var_add_filter_list_file(xpn_register_var *self,const char* filename);


G_MODULE_EXPORT void                                xpn_register_var_add_int(xpn_register_var *self,char *varname,int value,int line, gboolean record);
G_MODULE_EXPORT void 			xpn_register_var_add_double(xpn_register_var *self,char *varname,double value,int line, gboolean record);
G_MODULE_EXPORT void 			xpn_register_var_add_float(xpn_register_var *self,char *varname,float value,int line, gboolean record);

G_MODULE_EXPORT void 			xpn_register_var_add_pint(xpn_register_var *self,char *varname,int *value,int line, gboolean record,gboolean reg_once);
G_MODULE_EXPORT void 			xpn_register_var_add_pdouble(xpn_register_var *self,char *varname,double *value,int line, gboolean record,gboolean reg_once);
G_MODULE_EXPORT void 			xpn_register_var_add_pfloat(xpn_register_var *self,char *varname,float *value,int line, gboolean record,gboolean reg_once);
G_MODULE_EXPORT void 			xpn_register_var_add_none(xpn_register_var *self,char *varname,void *value,int line,gboolean reg_once);

/* gibt einen pointer auf einen identifikationsschlüssel in der registry zurück */
G_MODULE_EXPORT void* 			xpn_register_var_get_pointer(xpn_register_var *self,char *varname);

// Ein paar kleine Functionen, die für das Abrufen der Config interessant sein könnten:
G_MODULE_EXPORT int xpn_register_var_get_pointer_convert_to_int(xpn_register_var *self,char *varname, int standartvalue);
G_MODULE_EXPORT double xpn_register_var_get_pointer_convert_to_double(xpn_register_var *self,char *varname, double standartvalue);
G_MODULE_EXPORT float xpn_register_var_get_pointer_convert_to_float(xpn_register_var *self,char *varname, float standartvalue);

// Array aus z.B. aus config (Eintraege sind mit ';' getrennt):
// Das Array sollte dannach wieder freigegeben werden
G_MODULE_EXPORT char** xpn_register_var_get_pointer_convert_to_array_string(xpn_register_var *self,char *varname,int *size);
// String Array wieder frei geben
G_MODULE_EXPORT void  xpn_register_var_free_string_list(char **strlist,int size);

// same for int array:
G_MODULE_EXPORT int* xpn_register_var_get_pointer_convert_to_array_int(xpn_register_var *self,char *varname,int *size);
G_MODULE_EXPORT void  xpn_register_var_free_int_list(int *intlist);

// same for double array:
G_MODULE_EXPORT double* xpn_register_var_get_pointer_convert_to_array_double(xpn_register_var *self,char *varname,int *size);
G_MODULE_EXPORT void  xpn_register_var_free_double_list(double *list);



// return 0 if in list or 1 if not
G_MODULE_EXPORT int  xpn_register_var_check_if_in_list(char **filterlist, int filterlist_len,char* str);

//G_MODULE_EXPORT void 			xpn_register_var_load_expertn_db(xpn_register_var *self,PTIME pTi, PCLIMATE pCl, PCHEMISTRY pCh, PGRAPHIC pGr, PHEAT pHe, PLOCATION pLo, PMANAGEMENT pMa, PPLANT pPl, PSPROFILE pSo, PWATER pWa);

// ------------------------------  string functions  ------------------------------------------------------------- 

G_MODULE_EXPORT int getStringColumn(char *dest,char *source,char *seperator,int column);
// result:  0 if everything ok
// 			1 column does not exist
// example: 
// getStringColumn(S,"520::200::234","::",2);
// printf("Number %s",S); // Number 234
G_MODULE_EXPORT int getStringColumn_get_Size(char *source,char *seperator);
// liefert die Anzahl der Spalten zurück


G_MODULE_EXPORT char *fgets_mem(FILE *stream);
// similar as fgets (without endl charakter)
// for windows and linux txt files
// result: string which must be freed with free(..)
//         NULL if error

G_MODULE_EXPORT void deleteSpaceBegEnd (char *str);
// deletes spaces at beginn and end of string


G_MODULE_EXPORT int strcmp_extra(const char* srcstr,const char*cmpstr);
// special strcmp func with asterisk and wildcards
// ret: 0  if str are equal
// ret: 1  if str are not equal
// ret: -1 any error
// example: 
// ret = strcmp_extra("Der kleine Hund","*kl?ine*");
// ret is 0


// Replace a string with a given template
// Example:
// char *res = eplace_str_with_template("Der kleine $Tier ging in den Wald und der $Tier war dort!","$Tier","Hund");
// free(res);
G_MODULE_EXPORT char* replace_str_with_template(char *source,char* _template,char* expr);

// Replace a string with a given templates
// Example:
// char *S = "Der kleine $test geht in den $Platz, dabei passiert $test folgendes!";
// char *Templates[2] = {"$test","$Platz"};
// char *expr[2] = {"Christian","Garten"};
// char *res = replace_str_with_templates(S,Templates,expr,2);
// free(res);
G_MODULE_EXPORT char* replace_str_with_templates(char *source,char** _templates,char** exprs,int templ_len);


// Erzeugt einen gültigen Pfad aus einen Basic und einem relativen Pfad
// Ist der relative Pfad schon ein direkter, so wird lediglich eine Kopie zurückgegeben
// z.B.: path = get_fullpath_from_relative("/windows/chrikle","../hallo"); 
// ergibt: "/windows/hallo"
// Ergnisstring muss mit free deallokiert werden
G_MODULE_EXPORT char *get_fullpath_from_relative(const char *_basepath, const char* _relativepath);

G_MODULE_EXPORT char* delete_last_part_of_path(const char *_path);
G_MODULE_EXPORT char* delete_first_part_of_path(const char *_path);

G_MODULE_EXPORT char* get_filename_without_path(const char *filename);
G_MODULE_EXPORT char* get_path_from_filename(const char*filename);
G_MODULE_EXPORT char *get_filename_without_extension(const char *filename);

// return 0 --> is in path;  1 --> not at first place
G_MODULE_EXPORT int cmp_first_part_of_path(const char* path, const char* part);

// return C string (muss mit g_free wieder frei gemacht werden)
G_MODULE_EXPORT gchar *convert_fortran_to_c_string(char* fortran_str,int str_len);

/*
in:
 * latitude: Breitengrad in [Grad]
 * JulianDay: aktueller Tag im Jahr
*/
G_MODULE_EXPORT double calculateDayLength(double latitude, int JulianDay);

//  ---------------------------------------------------------------------------------------------------------------------

// Macros:
#define xpn_register_var_init_pint(dtb,var,varname,value) \
	var=value; \
	xpn_register_var_add_pint(dtb,varname,&(var),-1,TRUE,TRUE);

// long wird wie int behandelt!
#define xpn_register_var_init_plong(dtb,var,varname,value) \
	if (sizeof(int)!=sizeof(long)) \
		{ \
		    fprintf(stderr,"%s\t%d\tERROR: Database registration: Integer has different size as long:  %s\n",__FILE__,__LINE__,varname);\
		    fprintf(stderr,"Int Size: %d\t Long Size: %d\n",(int)sizeof(int),(int)sizeof(long));\
		    exit(1);\
		} \
	var=value; \
	xpn_register_var_add_pint(dtb,varname,(int*)(&(var)),-1,TRUE,TRUE);


#define xpn_register_var_init_pdouble(dtb,var,varname,value) \
	var=value; \
	xpn_register_var_add_pdouble(dtb,varname,&(var),-1,TRUE,TRUE);

#define xpn_register_var_init_pfloat(dtb,var,varname,value) \
	var=value; \
	xpn_register_var_add_pfloat(dtb,varname,&(var),-1,TRUE,TRUE);

#define xpn_register_var_init_none(dtb,var,varname,value) \
	var=value; \
	xpn_register_var_add_none(dtb,varname,&(var),-1,TRUE);



// zu benutzen: xpn_register_var_init_array(dtb_init_exp(..,varname, ..) ,myvarname,count)



#define xpn_register_var_init_array(dtb_init_exp,myvarname_2,count) \
        { \
             char *varname; \
             int i; \
             for (i=0;i<count;i++) \
               { \
                  varname = g_strdup_printf("%s[%d]",myvarname_2,i); \
                  dtb_init_exp; \
		  g_free(varname); \
               } \
        }

#define xpn_register_var_init_array2d(dtb_init_exp,myvarname,count1,count2) \
        { \
             char *varname; \
             int i,i2; \
             for (i2=0;i2<count2;i2++) \
               { \
                    for (i=0;i<count1;i++) \
                       { \
                           varname = g_strdup_printf("%s[%d][%d]",myvarname,i2,i); \
                           dtb_init_exp; \
		           g_free(varname); \
                       } \
               } \
        } 


#define xpn_register_var_init_rec_struct(dtb,structure,first,dtb_init_exp,var,_varname,value) \
        { \
             structure *mystruc; \
             int _i; \
             char *myvarname; \
             for (mystruc=first, _i = 0;mystruc!=NULL;mystruc=mystruc->pNext, _i++) \
                 { \
                    myvarname = g_strdup_printf("%s{%d}",_varname,_i); \
                    dtb_init_exp(dtb,mystruc->var,myvarname,value); \
                    g_free(myvarname); \
                 } \
	} 

#ifndef INFINITY
#define INFINITY get_infinity()
#endif
	
#define CHECK_DOUBLE_INVALID(val) ((val==-INFINITY)||(val==+INFINITY)||(val!=val))

#define NewDay(pz) ((pz->pSimTime->bFirstRound==TRUE)||(pz->pSimTime->fTimeDay==0.0)||((int)(pz->pSimTime->fTimeY) != (int)(pz->pSimTime->fTimeY_old)))? 1 : 0

// function used from the macro INFINITY
G_MODULE_EXPORT double get_infinity();





// Time Functions:
G_MODULE_EXPORT void xpn_time_get_struc(STRUCT_XPN_TIME *str_time, int iYear, double fTimeY);
G_MODULE_EXPORT void xpn_time_set_struc(STRUCT_XPN_TIME *str_time, int *iYear, double *fTimeY);
G_MODULE_EXPORT void xpn_time_add_dt(int *iYear, double *fTimeY,double dt);
G_MODULE_EXPORT void xpn_time_sub_dt(int *iYear, double *fTimeY,double dt);
G_MODULE_EXPORT char* xpn_time_get_formated_date(int iYear, double fTimeY,gboolean MS_ACCESS_COMP);
G_MODULE_EXPORT char* xpn_time_get_formated_date_and_time(int iYear, double fTimeY);

// number = days2-days1
G_MODULE_EXPORT double xpn_time_get_number_of_days(int iYear1, double fTimeY1,int iYear2, double fTimeY2);
G_MODULE_EXPORT double xpn_time_get_number_of_dates(int day1, int month1, int year1, int day2, int month2, int year2);
G_MODULE_EXPORT void xpn_time_conv_gdate_to_xpn_time(GDate *_date,int *iYear, double *fTimeY);
G_MODULE_EXPORT void xpn_time_conv_xpn_time_to_gdate(GDate *_date,int iYear, double fTimeY);

// compares (iYear1,fTimeY1) with (iYear2,fTimeY2)+dt2
// result:
// 1: > 
// -1: <
// 0: ==
G_MODULE_EXPORT int xpn_compare_to_added_date(int iYear1, double fTimeY1, int iYear2, double fTimeY2,double dt2);

// 1: > 
// -1: <
// 0: ==
G_MODULE_EXPORT int xpn_time_compare_time(int iYear1, double fTimeY1,int iYear2, double fTimeY2);


// return dt [day]
G_MODULE_EXPORT double xpn_time_sub_time(int iYear1, double fTimeY1,int iYear2, double fTimeY2);

G_MODULE_EXPORT double xpn_time_interpolate_linear(double iYear1,double fTimeY1,double fvalue1 ,double iYear2,double fTimeY2,double fvalue2, int iYear,double fTimeY);
// return interpolated value

// dt [day]
G_MODULE_EXPORT void xpn_time_date_add_dt(int *year1,int *month1,int *day1, int dt);

// 1: > 
// -1: <
// 0: ==
G_MODULE_EXPORT int xpn_time_compare_date(int year1,int month1,int day1, int year2,int month2,int day2);


G_MODULE_EXPORT double xpn_time_conv_time_systime(STRUCT_XPN_TIME *t);

// Weitere Funktionen:
G_MODULE_EXPORT char* get_formated_date(int _date,gboolean MS_ACCESS_COMP); //_date:  DDMMYY
// return:  02-03-1999

G_MODULE_EXPORT int get_expertn_date(char *_date); //_date:  02-03-1999
// return: DDMMYY

G_MODULE_EXPORT struct tm convert_date_to_unix_time(int _date); // _date: DDMMYY

G_MODULE_EXPORT  int convert_date_to_days(int _date); // _date: DDMMYY
// return Anzahl der Tage seit 1900;

G_MODULE_EXPORT  int conv_gdate_to_xpn_date(GDate *_date);
//return DDMMYY

G_MODULE_EXPORT  int get_expertn_date_from_unix(int unixtime); //der sec seit 1970;
//return DDMMYY

G_MODULE_EXPORT int get_expertn_JulianDay(int _date); //date DDMMYY
// rerunt julian day

G_MODULE_EXPORT int get_date_before_day(int _date);

G_MODULE_EXPORT int get_date_after_day(int _date);

G_MODULE_EXPORT int get_numb_of_days(int year);// Anzahl der Tage im Jahr:


G_MODULE_EXPORT long double c_mktime(struct tm *timeptr);

// input: 1992-08-13
G_MODULE_EXPORT GDate *convert_str_to_gdate(char* S);
// input: 1992-08-13
G_MODULE_EXPORT void convert_str_to_xpn_date(STRUCT_XPN_DATE *xpn_date,char* S);

G_MODULE_EXPORT int is_leap_year(int year);




// Reads weather files
// seprerators: ",", ";", "\t", " "
// date formats: cdbformat (days in seconds)
// or            "yyyy-mm-dd  hh:mm:ss", "mm/dd/yyyy hh:mm:ss", "dd.mm.yyyy hh:mm:ss"
// or            "yyyy-mm-dd",           "mm/dd/yyyy",          "dd.mm.yyyy"
G_MODULE_EXPORT STRUCT_XNO_DATA *xpn_load_climate_xno_file(char *filename,int min_columns);

// frees data structure
// returns NULL
G_MODULE_EXPORT STRUCT_XNO_DATA *xpn_free_XNO_DATA(STRUCT_XNO_DATA* data);


#if !GLIB_CHECK_VERSION(2,24,0)
G_MODULE_EXPORT gpointer g_malloc0_n (gsize n_blocks, gsize n_block_bytes);
#endif



G_END_DECLS

#endif /* __XPN_REGISTER_VAR_H__ */


