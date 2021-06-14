//      database_help_func.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      



#ifndef __DATABASE_HELP_FUNC_H__
#define __DATABASE_HELP_FUNC_H__

// Wrapperfunktionen, die helfen Datenbank Fehler schnell zu finden:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#ifndef XPN_WITHOUT_DATABASE
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>
#endif
#include <xpn_register_var.h>
#include <expertn_modul_base.h>

#define db_select(cnc,parser,sql) _db_select(cnc,parser,sql,__LINE__)

#define db_get_string(data_model,col) _db_get_string(data_model,col,__LINE__)
#define db_get_double(data_model,col) _db_get_double(data_model,col,__LINE__)
#define db_get_int(data_model,col)    _db_get_int(data_model,col,__LINE__)
#define db_get_date(data_model,col) _db_get_date (data_model,col,__LINE__)

#define db_get_string_row(data_model,col,row) _db_get_string_row(data_model,col,row,__LINE__)
#define db_get_double_row(data_model,col,row) _db_get_double_row(data_model,col,row,__LINE__)
#define db_get_int_row(data_model,col,row)    _db_get_int_row(data_model,col,row,__LINE__)
#define db_get_date_row(data_model,col,row) _db_get_date_row (data_model,col,row,__LINE__)

#define db_PRINT_ERROR(Message,line) fprintf(stderr,"------ DB ERROR:  %s\t%d\t%s ----- \n",__FILE__,line,Message);

#ifndef XPN_WITHOUT_DATABASE
GdaDataModel *_db_select(GdaConnection *cnc, GdaSqlParser *parser, char* sql,int LineNr);

char* _db_get_string (GdaDataModel *data_model,int col,int LineNr);
char* _db_get_string_row (GdaDataModel *data_model,int col,int row,int LineNr);
double _db_get_double(GdaDataModel *data_model,int col,int LineNr);
double _db_get_double_row(GdaDataModel *data_model,int col,int row,int LineNr);
int _db_get_int      (GdaDataModel *data_model,int col,int LineNr);
int _db_get_int_row      (GdaDataModel *data_model,int col,int row,int LineNr);
GDate* _db_get_date_row (GdaDataModel *data_model,int col,int row, int LineNr);
GDate* _db_get_date (GdaDataModel *data_model,int col, int LineNr);
#endif

#define  EPSILON  1.0e-9

#define SET_IF_99(x1,y1) if (x1==-99.0) x1=(y1);
#define SET_IF_99_AND_PEDO_DEFINED(x1,y1,pedo) if (x1==-99.0) {\
if (pedo!=NULL)\
{\
x1=(y1);\
} else\
{\
PRINT_ERROR("No pedotransfer functions defined!");\
}\
}
//Added by Hong
 #define WATER_CONTENT(x) (*self->WCont)((double)x,\
        (double)pWL->fContAct,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2) 
//End of Hong
#define WATER_CONTENT_H(x) (*self->WCont)((double)x,\
                                        (double)pWL->fContAct,\
                                        (double)pSW->fCondSat,\
                                        (double)pSW->fContSat,\
                                        (double)pSW->fContRes,\
                                        (double)pSW->fVanGenA,\
                                        (double)pSW->fVanGenN,\
                                        (double)pSW->fVanGenM,\
                                        (double)pSW->fCampA,\
                                        (double)pSW->fCampB,\
                                        (double)pSW->fPotInflec,\
                                        (double)pSW->fContInflec,\
                                        (double)pSW->fMinPot,\
                                        (double)pWL->fMatPotOld,\
                                        (double)pWL->fContOld,\
                                        (double)pSW->fVanGenA2,\
                                        (double)pSW->fVanGenN2,\
                                        (double)pSW->fVanGenM2,\
                                        (double)pSW->fBiModWeight1,\
                                        (double)pSW->fBiModWeight2)

#define MATRIX_POTENTIAL_H(x) (float) (*MPotl)((double)pWL->fMatPotAct,\
        (double)x,\
        (double)pSW->fCondSat,\
        (double)pSW->fContSat,\
        (double)pSW->fContRes,\
        (double)pSW->fVanGenA,\
        (double)pSW->fVanGenN,\
        (double)pSW->fVanGenM,\
        (double)pSW->fCampA,\
        (double)pSW->fCampB,\
        (double)pSW->fPotInflec,\
        (double)pSW->fContInflec,\
        (double)pSW->fMinPot,\
        (double)pWL->fMatPotOld,\
        (double)pWL->fContOld,\
        (double)pSW->fVanGenA2,\
        (double)pSW->fVanGenN2,\
        (double)pSW->fVanGenM2,\
        (double)pSW->fBiModWeight1,\
        (double)pSW->fBiModWeight2)

#define C_DEBUG(var) \
	if CHECK_DOUBLE_INVALID(var) \
	{\
	  char *S;\
	  S = (char*)malloc(250+sizeof(#var));\
	  sprintf(S,"Variable %s hat den Wert: %f",#var,var);\
	  PRINT_ERROR(S);\
	  free(S);\
	  exit(1);\
	} else xpn_register_var_add_double(xpn->pXSys->var_list,#var,var,__LINE__,TRUE);




// T = Temperature in Grad C
// return vapor pressure of water [hPa]
double get_e(double T);
  
// q = spez. humidity [kg/kg]
// P = Pressure [hPa]
// return P_H20 [hPa]
double get_Ph20(double q,double P);
    
// q = spez. humidity [kg/kg]
// T = Temperature [Grad C]
// P = Pressure [hPa]
// return RH [%]
double calc_rel_hum(double q,double T,double P);

// distributes SolRad over the day, possible to do with three different methods:
// constant distribution, triangular approach and sine curve approach
// result:  // MJ / m2 / day Globalstrahlung = SOLDN + LWDN (langwelliger Lux + kurwelliger Lux)
double get_SolRad(double SolRad, double iJulianDay, double Latitude, double timeday, double dt);

double CalculateActiveRadiation(double DayTime, double SolRad , double fLatitude, int iJulianDay);
double cpulse(double t,double t_begin, double period,double width);


double calculateAirTemperature(double DayTime, double Tairmin, double Tairmax,double TairLastDayMax, double TairNextDayMin, double Latitude, int JulianDay);

#ifndef M_PI
# define M_PI		3.14159265358979323846	/* pi */
#endif

int	SoilCode(double fClay, double fSilt, double fSand, char* acSoilCode);
double GPV_FK_PWP(double fClay,double fBulkDens,double fHumus, double fStone,
				 char* acSoilCode,int iRet);
int BD_Class(double fVal);
int CapacityParam1(int iLd, char* acSoilCode, int iReturnType);
int Clay_Class(double fVal);
int CapacityParam2(int iTon, double fOs, int iReturnType);
/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/
double  abspowerDBL(double x, double y);







void expertn_database_help_func_reservere_Memory(expertn_modul_base *self, int iLayers);
void expertn_database_help_func_set_time_step(expertn_modul_base *self,const char* local_config_str);
int expertn_database_help_func_Time_Message_and_get_number_of_days(expertn_modul_base *self);
void expertn_database_help_func_init_Chemistry(expertn_modul_base *self);
int expertn_database_help_func_init_water(expertn_modul_base *self);
void expertn_database_help_func_init_Soil_Organic_Matter(expertn_modul_base *self);
void expertn_database_help_func_TSComparePreCropDate(expertn_modul_base *self);
double expertn_database_help_func_get_Rn(expertn_modul_base *self);
void expertn_database_help_func_set_standard_soil_paramter(expertn_modul_base *self);
void expertn_database_help_func_set_global_start_and_end_date(expertn_modul_base *self,GDate **start_date,GDate **end_date);


#endif // __DATABASE_HELP_FUNC_H__

