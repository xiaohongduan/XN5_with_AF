//      expertn_modul_base.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      

#ifndef __EXPERTN_MODUL_BASE_H__
#define __EXPERTN_MODUL_BASE_H__

#include <glib-object.h>
#include <gmodule.h>
#include "xinclexp.h"



G_BEGIN_DECLS

#define EXPERTN_MODUL_BASE_TYPE				(expertn_modul_base_get_type())
#define EXPERTN_MODUL_BASE(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			EXPERTN_MODUL_BASE_TYPE, expertn_modul_base))
#define EXPERTN_MODUL_BASE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			EXPERTN_MODUL_BASE_TYPE, expertn_modul_baseClass))
#define IS_EXPERTN_MODUL_BASE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			EXPERTN_MODUL_BASE_TYPE))
#define IS_EXPERTN_MODUL_BASE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			EXPERTN_MODUL_BASE_TYPE))

typedef struct _expertn_modul_base			expertn_modul_base;
typedef struct _expertn_modul_baseClass		expertn_modul_baseClass;

struct _expertn_modul_base
{
	GObject parent;

	PXSYSTEM pXSys;
		
	PLOCATION pLo; // Informationen ueber die Position und Projektname, cfg Datei ...
	
	PTIME pTi; // Uhrzeit, Zeitschrit, ...
	PCLIMATE pCl; // Klimatische Daten, wie Sonneneinstrahlung ...
	
	PCHEMISTRY pCh;
	
	//PGRAPHIC pGr;
	PHEAT pHe;
	
	PMANAGEMENT pMa;
	PPLANT pPl;
	PSPROFILE pSo;
	
	PWATER pWa;

	// einige System Variablen:
	int DEBUG_LEVEL;

};

struct _expertn_modul_baseClass
{
	GObjectClass parent_class;
};

G_MODULE_EXPORT GType       expertn_modul_base_get_type		(void);
G_MODULE_EXPORT expertn_modul_base*	expertn_modul_base_new			(void);

// return char* str --> has do be freed with free(str);
G_MODULE_EXPORT char *expertn_modul_base_replace_std_templates(expertn_modul_base *self,char *source);
G_MODULE_EXPORT char *expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(expertn_modul_base *self,char *source);

// Einige Macros:
#define PRINT_ERROR(Message) fprintf(stderr,"ERROR: %s\t%d\t%s\n",__FILE__,__LINE__,Message);fflush(stderr);
#define PRINT_ERROR_ID(self,Message)\
									{\
									char *ERROR_TIME_STR;\
									ERROR_TIME_STR = xpn_time_get_formated_date_and_time(self->pTi->pSimTime->iyear,self->pTi->pSimTime->fTimeY);\
									fprintf(stderr,"ERROR (%s): %s: %s\t%d\t%s\n",ERROR_TIME_STR,self->pXSys->reg_str,__FILE__,__LINE__,Message);fflush(stderr);\
									g_free(ERROR_TIME_STR);\
									}
#define PRINT_WARNING(self,Message) PRINT_WARNING_ID(self,Message)
                                                                                       
#define PRINT_WARNING_ID(self,Message)\
									{\
									if (self->DEBUG_LEVEL > 2)\
									{\
									char *ERROR_TIME_STR;\
									ERROR_TIME_STR = xpn_time_get_formated_date_and_time(self->pTi->pSimTime->iyear,self->pTi->pSimTime->fTimeY);\
									fprintf(stderr,"WARNING (%s): %s: %s\t%d\t%s\n",ERROR_TIME_STR,self->pXSys->reg_str,__FILE__,__LINE__,Message);fflush(stderr);\
									g_free(ERROR_TIME_STR);\
									}\
									}

#define PRINT_MESSAGE(self,DEBUG_LEVEL_,Message)\
                                                                                 if (DEBUG_LEVEL_ <= self->DEBUG_LEVEL)\
                                                                                  { \
                                                                                       if (self->DEBUG_LEVEL > 3) \
                                                                                        { \
																								fprintf(stdout,"%s: %s\t%d\t%s\n",self->pXSys->reg_str,__FILE__,__LINE__,Message);\
                                                                                                 fflush(stdout);\
                                                                                        } else \
                                                                                        {\
                                                                                                 fprintf(stdout,"%s: %s\n",self->pXSys->reg_str,Message);\
                                                                                                 fflush(stdout);\
                                                                                        }\
                                                                                   }


#define allocPlantList(DATA_STRUCT,item,Count) \
										{ \
										   if (Count==0) \
										       { \
												item->pNext=NULL; \
												item->pBack=NULL; \
											   } \
												item->pNext=(DATA_STRUCT*)g_malloc0_n(1,sizeof(DATA_STRUCT)); \
												item->pNext->pBack = item; \
												item->pNext->pNext=NULL; \
												item=item->pNext; \
										}

#ifndef INFINITY
#define INFINITY expertn_modul_base_get_infinity()
#endif

#define SET_ERROR self->__ERROR=-1;

#define CHECK_DOUBLE_INVALID(val) ((val==-INFINITY)||(val==+INFINITY)||(val!=val))
#define CHECK_0(x)  if ((x == 0.0)||CHECK_DOUBLE_INVALID (x)) {PRINT_ERROR_ID((&(self->parent)),"Value "#x" is 0");SET_ERROR}
#define CHECK_VALID(var) if ((&(self->parent))->DEBUG_LEVEL > 2) {if (CHECK_DOUBLE_INVALID (var)) {PRINT_ERROR_ID((&(self->parent)),"Invalid value: "#var);SET_ERROR}}


#define CONF_m_IN_mm 1.0e+3
#define CONF_mm_IN_m 1.0e-3

#define allocItemList(DATA_STRUCT,first_item,Count) \
										{ \
										   int i; \
										   DATA_STRUCT *item; \
										   if (Count>0) \
										       { \
										        item =  (DATA_STRUCT*)g_malloc0_n(1,sizeof(DATA_STRUCT)); \
												item->pNext=NULL; \
												item->pBack=NULL; \
												first_item=item; \
												  for (i=1;i<Count;i++) \
												    { \
														item->pNext=(DATA_STRUCT*)g_malloc0_n(1,sizeof(DATA_STRUCT)); \
														item->pNext->pBack = item; \
														item->pNext->pNext=NULL; \
														item=item->pNext; \
													} \
											   } else \
											   { \
												  first_item=NULL; \
											   } \
										}


// PLANT FUNCTIONS:

G_MODULE_EXPORT double 	expertn_modul_base_AFGENERATOR(double fInput, RESPONSE* pResp);
G_MODULE_EXPORT int	  	expertn_modul_base_NitrogenConcentrationLimits(PPLANT pPl);
G_MODULE_EXPORT int    	expertn_modul_base_PlantVariableInitiation(PPLANT pPl, PSPROFILE pSo, PMSOWINFO pSI);
G_MODULE_EXPORT int    	expertn_modul_base_GenotypeRead(expertn_modul_base *self,PPLANT pPl,const char* configfile);
G_MODULE_EXPORT int    	expertn_modul_base_PlantManagementRead(expertn_modul_base *self,const char* configfile, char* model);
G_MODULE_EXPORT int	  	expertn_modul_base_DevelopmentCheckAndPostHarvestManagement(expertn_modul_base *self);
G_MODULE_EXPORT void   	expertn_modul_base_SetAllPlantRateToZero(expertn_modul_base *self);
G_MODULE_EXPORT int    	expertn_modul_base_plant_next(expertn_modul_base *self, PPLANT pPl);

// Timestep Functions:
G_MODULE_EXPORT void output_time_step_check_init(double *output_time_step,char **output_time_step_string,char *reg_str);
G_MODULE_EXPORT int output_time_step_compare_time_step(PTIME pTi,double output_time_step,char *output_time_step_string,STRUCT_XPN_TIME *xpn_time_old,STRUCT_XPN_TIME *xno_time);

G_MODULE_EXPORT void get_grid_info(char**_grid_id,char**_grid_i,char**_grid_j, char* reg_str);
G_MODULE_EXPORT void get_full_grid_info(int *_grid_id,int *_grid_i,int *_grid_j,int *_grid_k, char* reg_str);


typedef struct
{
    int iYear_from;
	double fTimeY_from;
	int iYear_to;
	double fTimeY_to;
} struct_timeperiod;

typedef struct
{
	gboolean active;
    struct_timeperiod *span;
    int span_len;	
} struct_timespan;

G_MODULE_EXPORT void expertn_modul_base_init_timespan(struct_timespan *timespan,char *Str);
G_MODULE_EXPORT void expertn_modul_base_init_timespan_parse(char *Str,struct_timeperiod *timespan);
G_MODULE_EXPORT int expertn_modul_base_check_time_span(struct_timespan *timespan,PTIME pTi);
G_MODULE_EXPORT void expertn_modul_base_time_span_free(struct_timespan *timespan);

#define G_FREE_IF_NOT_0(var)\
	{\
		if (var!=NULL)\
			{\
				g_free(var);\
				var=NULL;\
			}\
	}\


G_END_DECLS

#endif /* __EXPERTN_MODUL_BASE_H__ */
