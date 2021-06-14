//      x_noah_util.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      
//      
//      

#ifndef __X_NOAH_UTIL__
#define __X_NOAH_UTIL__

#include <expertn_modul_base.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct struc_add_soil *PSTRUC_ADD_SOIL;
typedef struct struc_add_soil
      {
        double C_Soil;  // soil heat capacity [J m-3 K-1]
        double C_Soil_loc;// soil heat capacity [J m-3 K-1]
        double Heat_Cap; // J m-3 K-1 volumetric heat capacity
        double ZSoil; //m Soil Depth (negative sign, as it is below ground) (!! ueberpruefen)
        PSTRUC_ADD_SOIL pNext;
        PSTRUC_ADD_SOIL pBack;
      } STRUC_ADD_SOIL;

typedef struct struc_veg_param *PSTRUC_VEG_PARAM;
typedef struct struc_veg_param
      {
        int count;
        int *index;
        double *SHDTBL;
        double *NROTBL;
        double *RSTBL;
        double *RGLTBL;
        double *HSTBL;
        double *SNUPTBL;
        double *MAXALB;
        double *LAIMINTBL;
        double *LAIMAXTBL;
        double *EMISSMINTBL;
        double *EMISSMAXTBL;
        double *ALBEDOMINTBL;
        double *ALBEDOMAXTBL;
        double *Z0MINTBL;
        double *Z0MAXTBL;
        double TOPT_DATA;   // K Optimum transpiration air temperature
        double CMCMAX_DATA; // Maximal conopy water capacity 
        double CFACTR_DATA;
        double RSMAX_DATA;
        int BARE;
        int NATURAL;
      } STRUC_VEG_PARAM;

typedef struct struc_soil_param *PSTRUC_SOIL_PARAM;
typedef struct struc_soil_param
      {
        int count;
        int *index;
        double *BB; // pSWater->fCampB
        double *DRYSMC;
        double *F11;
        double *MAXSMC; // porosity, ie. saturated value of soil moisture = pSWater->fContSat
        double *REFSMC; // soil moisture threshold below which transpiration begins to stress (volumetrix)
        double *SATPSI; // m = pSWater->fCampA * mm_TO_m,  saturation soil suction/ saturated metric potential (Psi_s)
        double *SATDK;
        double *SATDW; 
        double *WLTSMC;
        double *QTZ; // Soil quartz content
      } STRUC_SOIL_PARAM;
      
typedef struct struc_gen_param *PSTRUC_GEN_PARAM;
typedef struct struc_gen_param
      {
        int count;
        double *SLOPE_DATA; // linear reservoir coefficient
        double SBETA_DATA; // paramter used to calculate soil heat
        double FXEXP_DATA; // soil evapuration exponent used in DEVAP
        double CSOIL_DATA; // soil heat capacity [J m-3 K-1]
        double SALP_DATA; // shape paramter of distribution function of snow cover
        double REFDK_DATA; // paramter of surface runoff parameteriz
        double REFKDT_DATA; // paramter of surface runoff parameteriz
        double FRZK_DATA; // frozen ground parameter
        double ZBOT_DATA; // depth[m] of lower boundary soil temperature
        double CZIL_DATA; // calculate roughness length heat
        double SMLOW_DATA; // soil moisture wilt, soilt moisture reference paramters
        double SMHIGH_DATA;// soil moisture wilt, soilt moisture reference paramters
        double LVCOEF_DATA;
      } STRUC_GEN_PARAM;

PSTRUC_VEG_PARAM xnoah_util_VEG_PARAM_new(int count);
void             xnoah_util_VEG_PARAM_delete(PSTRUC_VEG_PARAM veg_param);

PSTRUC_SOIL_PARAM xnoah_util_SOIL_PARAM_new(int count);
void              xnoah_util_SOIL_PARAM_delete(PSTRUC_SOIL_PARAM soil_param);

PSTRUC_GEN_PARAM xnoah_util_GEN_PARAM_new(int count);
void             xnoah_util_GEN_PARAM_delete(PSTRUC_GEN_PARAM gen_param);

int xnoah_util_load_veg_param(PSTRUC_VEG_PARAM *veg_param,const char *filename,const char *Landuse_Type,int DB_LEVEL);
// return veg_param
int xnoah_util_load_soil_param(PSTRUC_SOIL_PARAM *soil_param,const char *filename,const char *Soil_Type,int DB_LEVEL);
// return soil_param
int xnoah_util_load_gen_param(PSTRUC_GEN_PARAM *gen_param,const char *filename,int DB_LEVEL);
// return gen_param

double *xnoah_util_new_double_array(int count);
// return double array intit mit 0.0




// Functions to Read and Write Errors, Kann von jeder beliebigen Funktion verwendet werden, normalerweise ist der Error=0;
// SetError ändert nur dann den Wert, wenn der Error==0 ist, daher enthät man mit GetError immer den Fehler,
// der als erstes aufgetreten ist
#define setError(No,Message) xnoah_util__setError(No,Message,__LINE__);
void xnoah_util__setError(int No, const char* errorstring,int Line);
int    xnoah_util_getErrorNo();
int    xnoah_util_getErrorLine();
char*  xnoah_util_getErrorString(); //Should be freed with free(..)

// CALCULATE SURFACE LAYER EXCHANGE COEFFICIENTS VIA ITERATIVE PROCESS.
// SEE CHEN ET AL (1997, BLM)
void xnoah_util_sfcdif_off(double ZLM,double Z0,double THZ0,double THLM,double SFCSPD,double CZIL,double *AKMS,double *AKHS);
double xnoah_util_TDFCND ( double smc, double qz, double smcmax, double sh2o);
// return EDIR Direct soil evaporation [W / m2]
double xnoah_util_DEVAP(double etp1,double smc, double shdfac, double smcdry,double smcmax,double fxexp);
double xnoah_util_evapo(double *SMC,int NSOIL,double CMC,double ETP1,double DT,               
                         double *SH2O,
                         double *SMCMAX,double PC,double *SMCWLT,             
                         double *SMCREF,double SHDFAC,double CMCMAX,                          
                         double *SMCDRY,double CFACTR,                                 
                         double *EDIR,double *EC,double **ET,double *ETT,double SFCTMP,double Q2,int NROOT,double *RTDIS,double FXEXP);


double xnoah_util_PENMAN(
/*out*/ double *o_epsca,
/*in*/  double sfctmp,double sfcprs,double ch,double th2, double prcp, double fdown, double ssoil,double q2, double q2sat, gboolean snowing, gboolean frzgra,
        double dqsdt2, double emissi_in, double T1, int ice, double sncovr,double *flx2,double *rch,double *rr,double *t24);
        
void xnoah_util_SMFLX( double **SMC,int NSOIL,double *CMC,double DT,double PRCP1,double *ZSOIL,
                        double **SH2O,double SLOPE,double KDT,double *FRZFACT,
                        double *SMCMAX,double *BEXP,double *SMCWLT,double *DKSAT,double *DWSAT,
                        double SHDFAC,double CMCMAX,
                        double *RUNOFF1,double *RUNOFF2,double *RUNOFF3,                       
                        double EDIR,double EC,double *ET,
                        double *DRIP);
                        
// out: double *TH2, double *T1V, double *TH2V
void xnoah_util_CALTMP(double T1, double SFCTMP, double SFCPRS, double ZLVL, double Q2, double *TH2, double *T1V, double *TH2V );

double xnoah_util_convert_rel_hum_to_spec_hum(double sfctmp, double sfcprs, double rhf);
void xnoah_util_CALHUM(double SFCTMP, double SFCPRS, double *Q2SAT, double *DQSDT2);

//double xnoah_util_calc_T1(double fdown, double emissi,double sfctmp,double ch,double q2, double th2, double sfcprs, double eta, double etp, double epsca, double zsoil1, double df1, double stc1);
double xnoah_util_calc_T1(double fdown, double emissi,double sfctmp,double ch,double q2, double th2, double sfcprs, double eta, double etp, double epsca, double zsoil1, double df1, double stc1, double ssoil);

double xnoah_util_calc_epsca(int ice,double sncovr,double sfcprs, double t2v,double T1,double ch, double etp);


#define QUAD(x) ( x * x )
#define mm_TO_m 1.0e-3
#define m_TO_mm 1.0e+3
#define d_TO_s (24.0*60.0*60.0)

#define EPSILON 1.0e-34

#define RET_ERROR 1

#define PRINT_ARRAY(var)
/*{\
int i;\
for (i=0;i<NSOIL;i++)\
{\
if (i==0) printf("%s ",#var);\
printf("%e   ",var[i]);\
}\
printf("\n");\
}*/

//  CONVERT EVAP TERMS FROM KINEMATIC (KG M-2 S-1) TO ENERGY UNITS (W M-2)
#define LVH2O 2.501E+6

#endif //__X_NOAH_UTIL__
