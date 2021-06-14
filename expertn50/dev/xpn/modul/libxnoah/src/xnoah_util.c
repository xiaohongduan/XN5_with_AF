#include "xnoah_util.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <xpn_register_var.h>



#define READ_F S=fgets_mem(f); \
    if ((feof(f) != 0) && (S==NULL)) \
        { \
            break; \
        }

#define UTIL_PRINT_MESSAGE(DEBUG_LEVEL_,Message) if (DEBUG_LEVEL_ <= DB_LEVEL) fprintf(stderr,"%s\t%d\t%s\n",__FILE__,__LINE__,Message);

int xnoah_util_load_veg_param(PSTRUC_VEG_PARAM *veg_param,const char *filename,const char *Landuse_Type,int DB_LEVEL)
{
    FILE *f;
    char *S,*S2,*loadlusetype;
    int i;
    int land_use_categories; // Anzahl der LUCs
    int LT_OK;
    //filename = "/home/chrikle/software/WXPN/data/tables/wrf/VEGPARM.TBL";
    //printf("Load File: %s\n",filename);
    f = fopen(filename,"r");
    if (f==NULL)
        {
            char S[10000];
            sprintf(S,"load_veg_param(): %s  not found!",filename);
            setError(1,S);
            return RET_ERROR;
        }
    LT_OK=0;
    while (1)
        {
            READ_F // erste Zeile
            if (S!=NULL) free(S);
            READ_F //
            deleteSpaceBegEnd(S);
            loadlusetype=S;
            READ_F
            S2=(char*) malloc(strlen(S)+1);
            getStringColumn(S2,S,",",0);
            land_use_categories = atoi(S2);
            free(S);
            free(S2);
            if (strcmp(loadlusetype,Landuse_Type) == 0)
                {
                    /*S = g_strdup_printf("LANDUSE TYPE = %s \t FOUND %d CATEGORIES",Landuse_Type,land_use_categories);
                    UTIL_PRINT_MESSAGE(2,S);
                    g_free(S);*/
                    LT_OK=1;
                    break;
                }
            for (i=0; i!=land_use_categories+12; i++)
                {
                    READ_F
                    if (S!=NULL) free(S);
                }
        } // WHILE

    if (LT_OK == 0)
        {
            fclose(f);
            setError(1,"Could not load Landuse Type!");
            return RET_ERROR;
        }

    // Alle Parameter Lesen:
    (*veg_param) = xnoah_util_VEG_PARAM_new(land_use_categories);
    for (i=0; i!=land_use_categories; i++)
        {
            S=fgets_mem(f);
            \
            if ((feof(f) != 0) && (S==NULL)) \
                {
                    \
                    break;
                    \
                }
            S2 = (char*)malloc(strlen(S)+1);
            getStringColumn(S2,S,",",0); // Index
            deleteSpaceBegEnd(S2);
            (*veg_param)->index[i]= atoi(S2);
            getStringColumn(S2,S,",",1); // SHDFAC
            deleteSpaceBegEnd(S2);
            (*veg_param)->SHDTBL[i] = atof(S2);
            getStringColumn(S2,S,",",2); // NROOT
            deleteSpaceBegEnd(S2);
            (*veg_param)->NROTBL[i] = atof(S2);
            getStringColumn(S2,S,",",3); // RS
            deleteSpaceBegEnd(S2);
            (*veg_param)->RSTBL[i] = atof(S2);
            getStringColumn(S2,S,",",4); // RGL
            deleteSpaceBegEnd(S2);
            (*veg_param)->RGLTBL[i]= atof(S2);
            getStringColumn(S2,S,",",5); // HS
            deleteSpaceBegEnd(S2);
            (*veg_param)->HSTBL[i] = atof(S2);
            getStringColumn(S2,S,",",6); // SNUP
            deleteSpaceBegEnd(S2);
            (*veg_param)->SNUPTBL[i] = atof(S2);
            getStringColumn(S2,S,",",7); // MAXALB
            deleteSpaceBegEnd(S2);
            (*veg_param)->MAXALB[i] = atof(S2);
            getStringColumn(S2,S,",",8); // LAIMIN
            deleteSpaceBegEnd(S2);
            (*veg_param)->LAIMINTBL[i] = atof(S2);
            getStringColumn(S2,S,",",9); // LAIMAX
            deleteSpaceBegEnd(S2);
            (*veg_param)->LAIMAXTBL[i] = atof(S2);
            getStringColumn(S2,S,",",10); // EMISSMIN
            deleteSpaceBegEnd(S2);
            (*veg_param)->EMISSMINTBL[i] = atof(S2);
            getStringColumn(S2,S,",",11); // EMISSMAX
            deleteSpaceBegEnd(S2);
            (*veg_param)->EMISSMAXTBL[i] = atof(S2);
            getStringColumn(S2,S,",",12); // ALBEDOMIN
            deleteSpaceBegEnd(S2);
            (*veg_param)->ALBEDOMINTBL[i] = atof(S2);
            getStringColumn(S2,S,",",13); // ALBEDOMAX
            deleteSpaceBegEnd(S2);
            (*veg_param)->ALBEDOMAXTBL[i] = atof(S2);
            getStringColumn(S2,S,",",14); // Z0MIN
            deleteSpaceBegEnd(S2);
            (*veg_param)->Z0MINTBL[i] = atof(S2);
            getStringColumn(S2,S,",",15); // Z0MAX
            deleteSpaceBegEnd(S2);
            (*veg_param)->Z0MAXTBL[i] = atof(S2);
            if (S2!=NULL) free(S2);
            if (S!=NULL) free(S);
        }
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*veg_param)->TOPT_DATA=atof(S);
    if (S!=NULL) free(S); // TOPT_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*veg_param)->CMCMAX_DATA=atof(S);
    if (S!=NULL) free(S); // CMCMAX_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*veg_param)->CFACTR_DATA=atof(S);
    if (S!=NULL) free(S); // CFACTR_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*veg_param)->RSMAX_DATA=atof(S);
    if (S!=NULL) free(S); // RSMAX_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*veg_param)->BARE=atoi(S);
    if (S!=NULL) free(S); // BARE
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*veg_param)->NATURAL=atoi(S);
    if (S!=NULL) free(S); // NATURAL
    fclose(f);

    return RET_SUCCESS;
}

int xnoah_util_load_soil_param(PSTRUC_SOIL_PARAM *soil_param,const char *filename,const char *Soil_Type,int DB_LEVEL)
{
    FILE *f;
    char *S,*S2,*loadsoiltype;
    int i;
    int soil_use_categories; // Anzahl der SUCs
    int LT_OK;

    //filename = "/home/chrikle/software/WXPN/data/tables/wrf/SOILPARM.TBL";
    f = fopen(filename,"r");
    if (f==NULL)
        {
            setError(1,"load_soil_param(): SOILPARAM.TBL not found");
            return RET_ERROR;
        }
    LT_OK=0;
    while (1)
        {
            READ_F // erste Zeile
            if (S!=NULL) free(S);
            READ_F //
            deleteSpaceBegEnd(S);
            loadsoiltype=S;
            READ_F
            S2=(char*) malloc(strlen(S)+1);
            getStringColumn(S2,S,",",0);
            soil_use_categories = atoi(S2);
            free(S);
            free(S2);
            if (strcmp(loadsoiltype,Soil_Type) == 0)
                {
                    /*  S = g_strdup_printf("SOIL TEXTURE CLASSIFICATION = %s \t FOUND %d CATEGORIES",Soil_Type,soil_use_categories);
                        UTIL_PRINT_MESSAGE(2,S);
                        g_free(S);*/
                    LT_OK=1;
                    break;
                }
            for (i=0; i!=soil_use_categories; i++)
                {
                    READ_F
                    if (S!=NULL) free(S);
                }
        } // WHILE

    if (LT_OK == 0)
        {
            fclose(f);
            setError(1,"Could not load Soil texture type!");
            return RET_ERROR;
        }

    // Alle Parameter Lesen:
    (*soil_param) = xnoah_util_SOIL_PARAM_new(soil_use_categories);
    for (i=0; i!=(int)soil_use_categories; i++)
        {
            S=fgets_mem(f);
            \
            if ((feof(f) != 0) && (S==NULL)) \
                {
                    \
                    break;
                    \
                }
            S2 = (char*)malloc(strlen(S)+1);
            getStringColumn(S2,S,",",0); // Index
            deleteSpaceBegEnd(S2);
            (*soil_param)->index[i]= atoi(S2);
            getStringColumn(S2,S,",",1); // BB
            deleteSpaceBegEnd(S2);
            (*soil_param)->BB[i]= atof(S2);
            getStringColumn(S2,S,",",2); // DRYSMC
            deleteSpaceBegEnd(S2);
            (*soil_param)->DRYSMC[i]= atof(S2);
            getStringColumn(S2,S,",",3); // F11
            deleteSpaceBegEnd(S2);
            (*soil_param)->F11[i]= atof(S2);
            getStringColumn(S2,S,",",4); // MAXSMC
            deleteSpaceBegEnd(S2);
            (*soil_param)->MAXSMC[i]= atof(S2);
            getStringColumn(S2,S,",",5); // REFSMC
            deleteSpaceBegEnd(S2);
            (*soil_param)->REFSMC[i]= atof(S2);
            getStringColumn(S2,S,",",6); // SATPSI
            deleteSpaceBegEnd(S2);
            (*soil_param)->SATPSI[i]= atof(S2);
            getStringColumn(S2,S,",",7); // SATDK
            deleteSpaceBegEnd(S2);
            (*soil_param)->SATDK[i]= atof(S2);
            getStringColumn(S2,S,",",8); // SATDW
            deleteSpaceBegEnd(S2);
            (*soil_param)->SATDW[i]= atof(S2);
            getStringColumn(S2,S,",",9); // WLTSMC
            deleteSpaceBegEnd(S2);
            (*soil_param)->WLTSMC[i]= atof(S2);
            getStringColumn(S2,S,",",10); // QTZ
            deleteSpaceBegEnd(S2);
            (*soil_param)->QTZ[i]= atof(S2);
            if (S2!=NULL) free(S2);
            if (S!=NULL) free(S);
        }
    fclose(f);
    return RET_SUCCESS;
}

int xnoah_util_load_gen_param(PSTRUC_GEN_PARAM *gen_param,const char *filename,int DB_LEVEL)
{
    FILE *f;
    char *S;
    int i,count;

    f = fopen(filename,"r");
    if (f==NULL)
        {
            setError(1,"load_gen_param(): GENPARAM.TBL not found");
            return RET_ERROR;
        }
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    count=atoi(S);
    if (S!=NULL) free(S); // count of slope_data
    *gen_param = xnoah_util_GEN_PARAM_new(count);
    for (i=0; i!=count; i++)
        {
            S=fgets_mem(f);
            deleteSpaceBegEnd(S);
            (*gen_param)->SLOPE_DATA[i] = atof(S);
            if (S!=NULL) free(S); // slope_data
        }
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->SBETA_DATA = atof(S);
    if (S!=NULL) free(S); // SBETA_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->FXEXP_DATA = atof(S);
    if (S!=NULL) free(S); // FXEXP_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->CSOIL_DATA = atof(S);
    if (S!=NULL) free(S); // CSOIL_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->SALP_DATA = atof(S);
    if (S!=NULL) free(S); // SALP_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->REFDK_DATA = atof(S);
    if (S!=NULL) free(S); // REFDK_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->REFKDT_DATA = atof(S);
    if (S!=NULL) free(S); // REFKDT_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->FRZK_DATA = atof(S);
    if (S!=NULL) free(S); // FRZK_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->ZBOT_DATA = atof(S);
    if (S!=NULL) free(S); // ZBOT_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->CZIL_DATA = atof(S);
    if (S!=NULL) free(S); // CZIL_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->SMLOW_DATA = atof(S);
    if (S!=NULL) free(S); // SMLOW_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->SMHIGH_DATA = atof(S);
    if (S!=NULL) free(S); // SMHIGH_DATA
    S=fgets_mem(f);
    if (S!=NULL) free(S);
    S=fgets_mem(f);
    deleteSpaceBegEnd(S);
    (*gen_param)->LVCOEF_DATA = atof(S);
    if (S!=NULL) free(S); // LVCOEF_DATA

    fclose(f);
    return RET_SUCCESS;
}

PSTRUC_VEG_PARAM xnoah_util_VEG_PARAM_new(int count)
{
    PSTRUC_VEG_PARAM veg_param;

    veg_param = (PSTRUC_VEG_PARAM)malloc(sizeof(STRUC_VEG_PARAM));
    veg_param->count = count;
    veg_param->index = (int*)malloc(sizeof(int)*count);
    veg_param->SHDTBL = (double*)malloc(sizeof(double)*count);
    veg_param->NROTBL = (double*)malloc(sizeof(double)*count);
    veg_param->RSTBL = (double*)malloc(sizeof(double)*count);
    veg_param->RGLTBL = (double*)malloc(sizeof(double)*count);
    veg_param->HSTBL = (double*)malloc(sizeof(double)*count);
    veg_param->SNUPTBL = (double*)malloc(sizeof(double)*count);
    veg_param->MAXALB = (double*)malloc(sizeof(double)*count);
    veg_param->LAIMINTBL = (double*)malloc(sizeof(double)*count);
    veg_param->LAIMAXTBL = (double*)malloc(sizeof(double)*count);
    veg_param->EMISSMINTBL = (double*)malloc(sizeof(double)*count);
    veg_param->EMISSMAXTBL = (double*)malloc(sizeof(double)*count);
    veg_param->ALBEDOMINTBL = (double*)malloc(sizeof(double)*count);
    veg_param->ALBEDOMAXTBL = (double*)malloc(sizeof(double)*count);
    veg_param->Z0MINTBL = (double*)malloc(sizeof(double)*count);
    veg_param->Z0MAXTBL = (double*)malloc(sizeof(double)*count);

    return veg_param;
}

void xnoah_util_VEG_PARAM_delete(PSTRUC_VEG_PARAM veg_param)
{
    if (veg_param!=NULL)
        {
            free(veg_param->index);
            free(veg_param->SHDTBL);
            free(veg_param->NROTBL);
            free(veg_param->RSTBL);
            free(veg_param->RGLTBL);
            free(veg_param->HSTBL);
            free(veg_param->SNUPTBL);
            free(veg_param->MAXALB);
            free(veg_param->LAIMINTBL);
            free(veg_param->EMISSMINTBL);
            free(veg_param->EMISSMAXTBL);
            free(veg_param->ALBEDOMINTBL);
            free(veg_param->ALBEDOMAXTBL);
            free(veg_param->Z0MINTBL);
            free(veg_param->Z0MAXTBL);
            free(veg_param);
        }

}

PSTRUC_SOIL_PARAM xnoah_util_SOIL_PARAM_new(int count)
{
    PSTRUC_SOIL_PARAM soil_param;

    soil_param = (PSTRUC_SOIL_PARAM)malloc(sizeof(STRUC_SOIL_PARAM));
    soil_param->count = count;
    soil_param->index = (int*)malloc(sizeof(int)*count);
    soil_param->BB = (double*)malloc(sizeof(double)*count);
    soil_param->DRYSMC = (double*)malloc(sizeof(double)*count);
    soil_param->F11 = (double*)malloc(sizeof(double)*count);
    soil_param->MAXSMC = (double*)malloc(sizeof(double)*count);
    soil_param->REFSMC = (double*)malloc(sizeof(double)*count);
    soil_param->SATPSI = (double*)malloc(sizeof(double)*count);
    soil_param->SATDK = (double*)malloc(sizeof(double)*count);
    soil_param->SATDW = (double*)malloc(sizeof(double)*count);
    soil_param->WLTSMC = (double*)malloc(sizeof(double)*count);
    soil_param->QTZ = (double*)malloc(sizeof(double)*count);
    return soil_param;
}

void xnoah_util_SOIL_PARAM_delete(PSTRUC_SOIL_PARAM soil_param)
{
    if (soil_param!=NULL)
        {
            free(soil_param->index);
            free(soil_param->BB);
            free(soil_param->DRYSMC);
            free(soil_param->F11);
            free(soil_param->REFSMC);
            free(soil_param->MAXSMC);
            free(soil_param->SATPSI);
            free(soil_param->SATDK);
            free(soil_param->SATDW);
            free(soil_param->WLTSMC);
            free(soil_param->QTZ);
            free(soil_param);
        }

}

PSTRUC_GEN_PARAM xnoah_util_GEN_PARAM_new(int count)
{
    PSTRUC_GEN_PARAM gen_param;

    gen_param = (PSTRUC_GEN_PARAM)malloc(sizeof(STRUC_GEN_PARAM));
    gen_param->count = count;
    gen_param->SLOPE_DATA = (double*)malloc(sizeof(double)*count);
    return gen_param;
}

void xnoah_util_GEN_PARAM_delete(PSTRUC_GEN_PARAM gen_param)
{
    if (gen_param!=NULL)
        {
            free(gen_param->SLOPE_DATA);
            free(gen_param);
        }
}

double *xnoah_util_new_double_array(int count)
{
    double *da;
    int i;
    da = (double*)malloc(sizeof(double)*count);
    for (i=0; i<count; i++)
        {
            da[i]=0.0;
        }
    return da;
}



static int ERROR_NO=0;
static int ERROR_LINE=0;
static const char *ERROR_STRING=NULL;
void xnoah_util__setError(int No, const char* errorstring,int Line)
{
    if (ERROR_NO==0)
        {
            ERROR_NO=No;
            ERROR_LINE=Line;
            ERROR_STRING=errorstring;
        }
}

int  xnoah_util_getErrorNo()
{
    return ERROR_NO;
}
int  xnoah_util_GetErrorLine()
{
    return ERROR_LINE;
}

char *xnoah_util_getErrorString()
{
    char *S;
    S=NULL;
    if (ERROR_NO==0)
        return S;
    if (ERROR_STRING!=NULL)
        {
            S=(char*)malloc(128+strlen(ERROR_STRING)+1);
            sprintf(S,"Error %d at Line: %d \n Error Message: %s\n",ERROR_NO,ERROR_LINE,ERROR_STRING);
        }
    else
        {
            S=(char*)malloc(128);
            sprintf(S,"Error %d at Line: %d",ERROR_NO,ERROR_LINE);
        }

    return S;
}

// ----------------------------------------------------------------------
// LECH'S SURFACE FUNCTIONS
// ----------------------------------------------------------------------
static double sfcdif_off_PSLMU (double ZZ)
{
    return -0.96* log (1.0-4.5* ZZ);
}

double sfcdif_off_PSLMS (double ZZ,double RRIC)
{
    return (ZZ * RRIC -2.076* (1. -1./ (ZZ +1.)));
}

double sfcdif_off_PSLHU(double ZZ)
{
    return -0.96* log (1.0-4.5* ZZ);
}

// ----------------------------------------------------------------------
// PAULSON'S SURFACE FUNCTIONS
// ----------------------------------------------------------------------
static double sfcdif_off_PSLHS (double ZZ,double RFAC)
{
    return ZZ * RFAC -2.076* (1. -1./ (ZZ +1.));
}
static double sfcdif_off_PSPMU(double XX,double PIHF)
{
    return -2.* log ( (XX +1.)*0.5) - log ( (XX * XX +1.)*0.5)
           +2.* atan (XX)
           - PIHF;
}
static double sfcdif_off_PSPMS (double YY)
{
    return 5.* YY;
}
static double  sfcdif_off_PSPHU (double XX)
{
    return -2.* log ( (XX * XX +1.)*0.5);
}

// ----------------------------------------------------------------------
// THIS ROUTINE SFCDIF CAN HANDLE BOTH OVER OPEN WATER (SEA, OCEAN) AND
// OVER SOLID SURFACE (LAND, SEA-ICE).
// ----------------------------------------------------------------------
static double sfcdif_off_PSPHS (double YY)
{
    return  5.* YY;
}

// CALCULATE SURFACE LAYER EXCHANGE COEFFICIENTS VIA ITERATIVE PROCESS.
// SEE CHEN ET AL (1997, BLM)
void xnoah_util_sfcdif_off(double ZLM,double Z0,double THZ0,double THLM,double SFCSPD,double CZIL,double *AKMS,double *AKHS)
{
    //double ZZ, PSLMU, PSLMS, PSLHU,SLHS;
//  double XX, PSPMU, YY, PSPMS, PSPHU, PSPHS;
    double ZILFC, ZU, ZT, RDZ, CXCH;
    double DTHV, DU2, BTGH, WSTAR2, USTAR, ZSLU, ZSLT, RLOGU, RLOGT;
    double RLMO, ZETALT, ZETALU, ZETAU, ZETAT, XLU4, XLT4, XU4, XT4;


    double XLU, XLT, XU, XT, PSMZ, SIMM, PSHZ, SIMH, USTARK, RLMN,RLMA;

    int ILECH, ITR;

    double WWST = 1.2;
    double WWST2 = WWST * WWST;
    double G = 9.8;
    double VKRM = 0.40;
    double EXCM = 0.001;
    double BETA = 1.0/270.0;
    double BTG = BETA * G;
    double ELFC = VKRM * BTG;
    double WOLD =.15;
    double WNEW = 1. - WOLD;
    int ITRMX = 5;
    double PIHF = 3.14159265/2.;

    double EPSU2 = 1.E-4;
    double EPSUST = 0.07;
    //double EPSIT = 1.E-4;
    //double EPSA = 1.E-8;
    double ZTMIN = -5.;
    double ZTMAX = 1.;
    double HPBL = 1000.0;
    double SQVISC = 258.2;

    double RIC = 0.183;
    double RRIC = 1.0/ RIC;
    double FHNEU = 0.8;
    double RFC = 0.191;
    double RFAC = RIC / (FHNEU * RFC * RFC);

// ----------------------------------------------------------------------
//     ZTFC: RATIO OF ZOH/ZOM  LESS OR EQUAL THAN 1
//     C......ZTFC=0.1
//     CZIL: CONSTANT C IN Zilitinkevich, S. S.1995,:NOTE ABOUT ZT
// ----------------------------------------------------------------------
    ILECH = 0;

// ----------------------------------------------------------------------
    ZILFC = - CZIL * VKRM * SQVISC;
//     C.......ZT=Z0*ZTFC
    ZU = Z0;


    RDZ = 1./ ZLM;
    CXCH = EXCM * RDZ;
    DTHV = THLM - THZ0;

// ----------------------------------------------------------------------
// BELJARS CORRECTION OF USTAR
// ----------------------------------------------------------------------
    DU2 = MAX (SFCSPD * SFCSPD,EPSU2);
//cc   If statements to avoid TANGENT LINEAR problems near zero
    BTGH = BTG * HPBL;
    if (BTGH * *AKHS * DTHV != 0.0)
        {
            WSTAR2 = WWST2* pow(fabs (BTGH * *AKHS * DTHV), (2./3.));
        }
    else
        {
            WSTAR2 = 0.0;
        }

// ----------------------------------------------------------------------
// ZILITINKEVITCH APPROACH FOR ZT
// ----------------------------------------------------------------------
    USTAR = MAX (sqrt (*AKMS * sqrt (DU2+ WSTAR2)),EPSUST);

/// ----------------------------------------------------------------------
    ZT = exp (ZILFC * sqrt (USTAR * Z0))* Z0;
    ZSLU = ZLM + ZU;
//     PRINT*,'ZSLT=',ZSLT
//     PRINT*,'ZLM=',ZLM
//     PRINT*,'ZT=',ZT

    ZSLT = ZLM + ZT;
    RLOGU = log (ZSLU / ZU);

    RLOGT = log (ZSLT / ZT);
//     PRINT*,'RLMO=',RLMO
//     PRINT*,'ELFC=',ELFC
//     PRINT*,'AKHS=',AKHS
//     PRINT*,'DTHV=',DTHV
//     PRINT*,'USTAR=',USTAR

    RLMO = ELFC * *AKHS * DTHV / pow(USTAR,3.0);
// ----------------------------------------------------------------------
// 1./MONIN-OBUKKHOV LENGTH-SCALE
// ----------------------------------------------------------------------



    for (ITR = 0; ITR < ITRMX; ITR++)
        {

            ZETALT = MAX (ZSLT * RLMO,ZTMIN);
            RLMO = ZETALT / ZSLT;
            ZETALU = ZSLU * RLMO;
            ZETAU = ZU * RLMO;

            ZETAT = ZT * RLMO;
            if (ILECH == 0)
                {
                    if (RLMO < 0.)
                        {
                            XLU4 = 1. -16.* ZETALU;
                            XLT4 = 1. -16.* ZETALT;
                            XU4 = 1. -16.* ZETAU;

                            XT4 = 1. -16.* ZETAT;
                            XLU = sqrt (sqrt (XLU4));
                            XLT = sqrt (sqrt (XLT4));
                            XU = sqrt (sqrt (XU4));

                            XT = sqrt (sqrt (XT4));
//     PRINT*,'-----------1------------'
//     PRINT*,'PSMZ=',PSMZ
//     PRINT*,'PSPMU(ZETAU)=',PSPMU(ZETAU)
//     PRINT*,'XU=',XU
//     PRINT*,'------------------------'
                            PSMZ = sfcdif_off_PSPMU (XU,PIHF);
                            SIMM = sfcdif_off_PSPMU (XLU,PIHF) - PSMZ + RLOGU;
                            PSHZ = sfcdif_off_PSPHU (XT);
                            SIMH = sfcdif_off_PSPHU (XLT) - PSHZ + RLOGT;
                        }
                    else
                        {
                            ZETALU = MIN (ZETALU,ZTMAX);
                            ZETALT = MIN (ZETALT,ZTMAX);
//     PRINT*,'-----------2------------'
//     PRINT*,'PSMZ=',PSMZ
//     PRINT*,'PSPMS(ZETAU)=',PSPMS(ZETAU)
//     PRINT*,'ZETAU=',ZETAU
//     PRINT*,'------------------------'
                            PSMZ = sfcdif_off_PSPMS (ZETAU);
                            SIMM = sfcdif_off_PSPMS (ZETALU) - PSMZ + RLOGU;
                            PSHZ = sfcdif_off_PSPHS (ZETAT);
                            SIMH = sfcdif_off_PSPHS (ZETALT) - PSHZ + RLOGT;
                        }
// ----------------------------------------------------------------------
// LECH'S FUNCTIONS
// ----------------------------------------------------------------------
                }
            else
                {

                    if (RLMO < 0.)
                        {
//     PRINT*,'-----------3------------'
//     PRINT*,'PSMZ=',PSMZ
//     PRINT*,'PSLMU(ZETAU)=',PSLMU(ZETAU)
//     PRINT*,'ZETAU=',ZETAU
//     PRINT*,'------------------------'
                            PSMZ = sfcdif_off_PSLMU (ZETAU);
                            SIMM = sfcdif_off_PSLMU (ZETALU) - PSMZ + RLOGU;
                            PSHZ = sfcdif_off_PSLHU (ZETAT);
                            SIMH = sfcdif_off_PSLHU (ZETALT) - PSHZ + RLOGT;
                        }
                    else
                        {
                            ZETALU = MIN (ZETALU,ZTMAX);

                            ZETALT = MIN (ZETALT,ZTMAX);
//     PRINT*,'-----------4------------'
//     PRINT*,'PSMZ=',PSMZ
//     PRINT*,'PSLMS(ZETAU)=',PSLMS(ZETAU)
//     PRINT*,'ZETAU=',ZETAU
//     PRINT*,'------------------------'
                            PSMZ = sfcdif_off_PSLMS (ZETAU,RRIC);
                            SIMM = sfcdif_off_PSLMS (ZETALU,RRIC) - PSMZ + RLOGU;
                            PSHZ = sfcdif_off_PSLHS (ZETAT,RFAC);
                            SIMH = sfcdif_off_PSLHS (ZETALT,RFAC) - PSHZ + RLOGT;
                        }
// ----------------------------------------------------------------------
// BELJAARS CORRECTION FOR USTAR
// ----------------------------------------------------------------------
                }

// ----------------------------------------------------------------------
// ZILITINKEVITCH FIX FOR ZT
// ----------------------------------------------------------------------
            USTAR = MAX (sqrt (*AKMS * sqrt (DU2+ WSTAR2)),EPSUST);

            ZT = exp (ZILFC * sqrt (USTAR * Z0))* Z0;
            ZSLT = ZLM + ZT;
//-----------------------------------------------------------------------
            RLOGT = log (ZSLT / ZT);
            USTARK = USTAR * VKRM;


            *AKMS = MAX (USTARK / SIMM,CXCH);
//-----------------------------------------------------------------------
// IF STATEMENTS TO AVOID TANGENT LINEAR PROBLEMS NEAR ZERO
//-----------------------------------------------------------------------
            *AKHS = MAX (USTARK / SIMH,CXCH);
            if (BTGH * *AKHS * DTHV != 0.0)
                {
                    WSTAR2 = WWST2*  pow(fabs(BTGH * *AKHS * DTHV), (2./3.));
                }
            else
                {
                    WSTAR2 = 0.0;
                }
//-----------------------------------------------------------------------
            RLMN = ELFC * *AKHS * DTHV / pow(USTAR,3.0);
//-----------------------------------------------------------------------
//     IF(ABS((RLMN-RLMO)/RLMA).LT.EPSIT)    GO TO 110
//-----------------------------------------------------------------------
            RLMA = RLMO * WOLD+ RLMN * WNEW;
//-----------------------------------------------------------------------
            RLMO = RLMA;
//     PRINT*,'----------------------------'
//     PRINT*,'SFCDIF OUTPUT !  ! ! ! ! ! ! ! !  !   !    !'

//     PRINT*,'ZLM=',ZLM
//     PRINT*,'Z0=',Z0
//     PRINT*,'THZ0=',THZ0
//     PRINT*,'THLM=',THLM
//     PRINT*,'SFCSPD=',SFCSPD
//     PRINT*,'CZIL=',CZIL
//     PRINT*,'AKMS=',AKMS
//     PRINT*,'AKHS=',AKHS
//     PRINT*,'----------------------------'

        }

}


//CALCULATE THERMAL DIFFUSIVITY AND CONDUCTIVITY OF THE SOIL FOR A GIVEN
// POINT AND TIME.
// ----------------------------------------------------------------------
// PETERS-LIDARD APPROACH (PETERS-LIDARD et al., 1998)
// June 2001 CHANGES: FROZEN SOIL CONDITION.
//return DF
//USE AS IN PETERS-LIDARD, 1998 (MODIF. FROM JOHANSEN, 1975).

//                                  PABLO GRUNMANN, 08/17/98
// REFS.:
//      FAROUKI, O.T.,1986: THERMAL PROPERTIES OF SOILS. SERIES ON ROCK
//              AND SOIL MECHANICS, VOL. 11, TRANS TECH, 136 PP.
//      JOHANSEN, O., 1975: THERMAL CONDUCTIVITY OF SOILS. PH.D. THESIS,
//              UNIVERSITY OF TRONDHEIM,
//      PETERS-LIDARD, C. D., ET AL., 1998: THE EFFECT OF SOIL THERMAL
//              CONDUCTIVITY PARAMETERIZATION ON SURFACE ENERGY FLUXES
//              AND TEMPERATURES. JOURNAL OF THE ATMOSPHERIC SCIENCES,
//              VOL. 55, PP. 1209-1224.
double xnoah_util_TDFCND ( double smc, double qz, double smcmax, double sh2o)
{
    double df;
    double poros,satratio,thkice,thkw,thko,thkqtz,thks,xunfroz,xu,thksat,gammd,thkdry,ake;
// NEEDS PARAMETERS
// POROSITY(SOIL TYPE):
    poros = smcmax;
// SATURATION RATIO:
// PARAMETERS  W/(M.K)
    satratio = smc/smcmax;
// ICE CONDUCTIVITY:
    thkice = 2.2;
// WATER CONDUCTIVITY:
    thkw = 0.57;
// THERMAL CONDUCTIVITY OF "OTHER" SOIL COMPONENTS
//      IF (QZ .LE. 0.2) THKO = 3.0
    thko = 2.0;
// QUARTZ' CONDUCTIVITY
    thkqtz = 7.7;
// SOLIDS' CONDUCTIVITY
    thks = pow(thkqtz,qz) * pow(thko,1.0-qz);
// UNFROZEN FRACTION (FROM 1., i.e., 100%LIQUID, TO 0. (100% FROZEN))
    xunfroz = sh2o / smc;
// UNFROZEN VOLUME FOR SATURATION (POROSITY*XUNFROZ)
    xu = xunfroz * smcmax;
// SATURATED THERMAL CONDUCTIVITY
    thksat = pow(thks,1.0-smcmax) * pow(thkice,smcmax - xu) * pow(thkw,xu);
// DRY DENSITY IN KG/M3
    gammd = (1.0-smcmax)*2700.0;
// DRY THERMAL CONDUCTIVITY IN W.M-1.K-1
    thkdry = (0.135*gammd +64.7) / (2700.0 - 0.947 * gammd);
// FROZEN
    if ((sh2o + 0.0005) < smc)
        {
            ake = satratio;
        }
    else
        {
            //UNFROZEN
            //RANGE OF VALIDITY FOR THE KERSTEN NUMBER (AKE)
            // KERSTEN NUMBER (USING "FINE" FORMULA, VALID FOR SOILS CONTAINING AT
            // LEAST 5% OF PARTICLES WITH DIAMETER LESS THAN 2.E-6 METERS.)
            //(FOR "COARSE" FORMULA, SEE PETERS-LIDARD ET AL., 1998).

            if (satratio > 0.1)
                {
                    ake = log10(satratio)+1.0;
                }
            else
                {
                    // USE K = KDRY
                    ake = 0.0;
                }
        }
    df = ake * (thksat - thkdry) + thkdry;
    return df;
}


// CALCULATE TRANSPIRATION FOR THE VEG CLASS.
// --> return ET
void xnoah_util_TRANSP (double **ET,int NSOIL,double ETP1,double *SMC,double CMC,double SHDFAC,double *SMCWLT,
                        double CMCMAX,double PC,double CFACTR,double *SMCREF,int NROOT,
                        double *RTDIS)

{
    int K,I;
    double ETP1A,SGX,DENOM,RTX;
    double *GX; // GX[NROOT]



    // INITIALIZE PLANT TRANSP TO ZERO FOR ALL SOIL LAYERS.

    GX = g_malloc0(sizeof(double)*NROOT);

    for (K=0; K<NSOIL; K++)
        {
            (*ET)[K] = 0.0;
        }



    // CALCULATE AN 'ADJUSTED' POTENTIAL TRANSPIRATION
    // IF STATEMENT BELOW TO AVOID TANGENT LINEAR PROBLEMS NEAR ZERO
    // NOTE: GX AND OTHER TERMS BELOW REDISTRIBUTE TRANSPIRATION BY LAYER,
    // ET(K), AS A FUNCTION OF SOIL MOISTURE AVAILABILITY, WHILE PRESERVING
    // TOTAL ETP1A.

    if (CMC != 0.0)
        {
            ETP1A = SHDFAC * PC * ETP1 * (1.0- pow(CMC / CMCMAX, CFACTR));
        }
    else
        {
            ETP1A = SHDFAC * PC * ETP1;
        }

    SGX = 0.0;
    for (I=0; I<NROOT; I++)
        {
            GX[I] = (SMC[I] - SMCWLT[I]) / (SMCREF[I] - SMCWLT[I]);
            GX[I] = MAX ( MIN ( GX [I], 1.0 ), 0.0 );
            SGX = SGX + GX [I];
        }

    SGX = SGX / NROOT;

    DENOM = 0.0;
    for (I=0; I<NROOT; I++)
        {
            RTX = RTDIS[I] + GX[I] - SGX;
            GX[I] = GX[I] * MAX ( RTX, 0.0 );
            DENOM = DENOM + GX[I];
        }

    if (DENOM <= 0.0)
        {
            DENOM = 1.0;
        }

    for (I=0; I<NROOT; I++)
        {
            (*ET)[I] = ETP1A * GX[I] / DENOM;


            // ABOVE CODE ASSUMES A VERTICALLY UNIFORM ROOT DISTRIBUTION
            //CODE BELOW TESTS A VARIABLE ROOT DISTRIBUTION


//
//      ET(1) = ( ZSOIL(1) / ZSOIL(NROOT) ) * GX * ETP1A
//      ET(1) = ( ZSOIL(1) / ZSOIL(NROOT) ) * ETP1A
// ----------------------------------------------------------------------
// USING ROOT DISTRIBUTION AS WEIGHTING FACTOR
// ----------------------------------------------------------------------
//      ET(1) = RTDIS(1) * ETP1A
//      ET(1) = ETP1A * PART(1)
// ----------------------------------------------------------------------
// LOOP DOWN THRU THE SOIL LAYERS REPEATING THE OPERATION ABOVE,
// BUT USING THE THICKNESS OF THE SOIL LAYER (RATHER THAN THE
// ABSOLUTE DEPTH OF EACH LAYER) IN THE FINAL CALCULATION.
// ----------------------------------------------------------------------
//      DO K = 2,NROOT
//        GX = ( SMC(K) - SMCWLT ) / ( SMCREF - SMCWLT )
//        GX = MAX ( MIN ( GX, 1. ), 0. )
// TEST CANOPY RESISTANCE
//        GX = 1.0
//        ET(K) = ((ZSOIL(K)-ZSOIL(K-1))/ZSOIL(NROOT))*GX*ETP1A
//        ET(K) = ((ZSOIL(K)-ZSOIL(K-1))/ZSOIL(NROOT))*ETP1A
// ----------------------------------------------------------------------
// USING ROOT DISTRIBUTION AS WEIGHTING FACTOR
// ----------------------------------------------------------------------
//        ET(K) = RTDIS(K) * ETP1A
//        ET(K) = ETP1A*PART(K)
//      END DO
        }


    if (GX!=NULL) g_free(GX);

}


// return EDIR Direct soil evaporation [W / m2]
double xnoah_util_DEVAP(double etp1,double smc, double shdfac, double smcdry,double smcmax,double fxexp)
{
    double edir;
    double sratio, fx;

    sratio = (smc - smcdry)/(smcmax-smcdry);
    if (sratio >0.0)
        {
            fx = pow(sratio,fxexp);
            fx = max(min(fx,1.0),0.0);
        }
    else
        {
            fx = 0.0;
        }
    edir = fx * (1.0-shdfac) * etp1;
    return edir;

}

// CALCULATE SOIL MOISTURE FLUX.  THE SOIL MOISTURE CONTENT (SMC - A PER
// UNIT VOLUME MEASUREMENT) IS A DEPENDENT VARIABLE THAT IS UPDATED WITH
// PROGNOSTIC EQNS. THE CANOPY MOISTURE CONTENT (CMC) IS ALSO UPDATED.
// FROZEN GROUND VERSION:  NEW STATES ADDED: SH2O, AND FROZEN GROUND
// CORRECTION FACTOR, FRZFACT AND PARAMETER SLOPE.
double xnoah_util_evapo(double *SMC,int NSOIL,double CMC,double ETP1,double DT,
                        double *SH2O,
                        double *SMCMAX,double PC,double *SMCWLT,
                        double *SMCREF,double SHDFAC,double CMCMAX,
                        double *SMCDRY,double CFACTR,
                        double *EDIR,double *EC,double **ET,double *ETT,double SFCTMP,double Q2,int NROOT,double *RTDIS,double FXEXP)
{
    double ETA1; //ret
    int K;
    double CMC2MS;

    // EXECUTABLE CODE BEGINS HERE IF THE POTENTIAL EVAPOTRANSPIRATION IS
    // GREATER THAN ZERO.
    // ----------------------------------------------------------------------
    *EDIR = 0.0;
    *EC = 0.0;
    *ETT = 0.0;
    for (K=0; K<NSOIL; K++)
        {
            (*ET)[K] = 0.0;
        }

// RETRIEVE DIRECT EVAPORATION FROM SOIL SURFACE.  CALL THIS FUNCTION
// ONLY IF VEG COVER NOT COMPLETE.
// FROZEN GROUND VERSION:  SH2O STATES REPLACE SMC STATES.

    if (ETP1 > 0.0)
        {
            if (SHDFAC < 1.0)
                {
                    *EDIR = xnoah_util_DEVAP(ETP1,SMC[0],SHDFAC,SMCDRY[0],SMCMAX[0],FXEXP);
                }
            // INITIALIZE PLANT TOTAL TRANSPIRATION, RETRIEVE PLANT TRANSPIRATION,
            // AND ACCUMULATE IT FOR ALL SOIL LAYERS.

            if (SHDFAC > 0.0)
                {
                    xnoah_util_TRANSP (ET,NSOIL,ETP1,SMC,CMC,SHDFAC,SMCWLT,
                                       CMCMAX,PC,CFACTR,SMCREF,NROOT,
                                       RTDIS);
                    for (K=0; K<NSOIL; K++)
                        {
                            *ETT = *ETT + (*ET)[K];
                        }
                    // CALCULATE CANOPY EVAPORATION.
                    //IF STATEMENTS TO AVOID TANGENT LINEAR PROBLEMS NEAR CMC=0.0.

                    if (CMC > 0.0)
                        {
                            *EC = SHDFAC * ( pow(( CMC / CMCMAX ), CFACTR) ) * ETP1;
                        }
                    else
                        {
                            *EC = 0.0;
                        }

                    // EC SHOULD BE LIMITED BY THE TOTAL AMOUNT OF AVAILABLE WATER ON THE
                    // CANOPY.  -F.CHEN, 18-OCT-1994

                    CMC2MS = CMC / DT;
                    *EC = MIN ( CMC2MS, *EC );

                }
        }

// TOTAL UP EVAP AND TRANSP TYPES TO OBTAIN ACTUAL EVAPOTRANSP

    ETA1 = *EDIR + *ETT + *EC;

    return ETA1;
}





// return etp
double xnoah_util_PENMAN(
    /*out*/ double *o_epsca,
    /*in*/  double sfctmp,double sfcprs,double ch,double th2, double prcp, double fdown, double ssoil,double q2, double q2sat, gboolean snowing, gboolean frzgra,
    double dqsdt2, double emissi_in, double T1, int ice, double sncovr,double *flx2,double *rch,double *rr,double *t24)
{

    double emissi;
    double elcp1,lvs,delta,rho,fnet,a,rad;
    double t2v;
    // out
    // t24; // == sfctmp^4
    double epsca;
    double etp;



    // const:
    double elcp,lsubc,cp,lsubs,rd,cph20,cpice,sigma,lsubf;

    elcp = 2.4888e+3;
    lsubc = 2.501000e+6;
    cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
    lsubs = 2.83e+6;
    rd = 287.04; // R_spec for dry air [J / (kg*K)] -- Gas Konstante
    cph20 = 4.218e+3;
    cpice = 2.106e+3;
    sigma = 5.67e-8;
    lsubf = 3.335e+5;

    t2v = sfctmp * (1.0 + 0.61 * q2);

    if (t2v==0.0) return 0.0;
    if (sfcprs == 0.0) return 0.0;

    emissi=emissi_in;
    if (ice==0)
        {
            elcp1 = (1.0-sncovr)*elcp + sncovr*elcp*lsubs/lsubc;
            lvs = (1.0-sncovr)*lsubc + sncovr*lsubs;
        }
    else
        {
            if (T1> 273.15)
                {
                    elcp1 = elcp;
                    lvs = lsubc;
                }
            else
                {
                    elcp1 = elcp * lsubs/lsubc;
                    lvs = lsubs;
                }
        }

    *flx2 = 0.0;
//      DELTA = ELCP * DQSDT2
    delta = elcp1 * dqsdt2;
    *t24 = sfctmp*sfctmp*sfctmp*sfctmp;
//      RR = T24 * 6.48E-8 / (SFCPRS * CH) + 1.0
    *rr = emissi* (*t24) * 6.48e-8 / (sfcprs * ch) + 1.0;
    rho = sfcprs / (rd * t2v);

    // ----------------------------------------------------------------------
    // ADJUST THE PARTIAL SUMS / PRODUCTS WITH THE LATENT HEAT
    // EFFECTS CAUSED BY FALLING PRECIPITATION.
    //----------------------------------------------------------------------
    *rch = rho * cp * ch;

    if (snowing == FALSE)
        {
            if (prcp > 0.0) (*rr) += cph20 * prcp / *rch;
        }
    else
        {
            (*rr)+= cpice * prcp / *rch;
        }

    // ----------------------------------------------------------------------
    // INCLUDE THE LATENT HEAT EFFECTS OF FRZNG RAIN CONVERTING TO ICE ON
    // IMPACT IN THE CALCULATION OF FLX2 AND FNET.
    // ----------------------------------------------------------------------
    //      FNET = FDOWN - SIGMA * T24- SSOIL


    fnet = fdown - emissi*sigma * (*t24) - ssoil;
    if (frzgra==TRUE)
        {
            *flx2 = - lsubf * prcp;
            fnet -= (*flx2);
        }


// ----------------------------------------------------------------------
// FINISH PENMAN EQUATION CALCULATIONS.
// ----------------------------------------------------------------------

    rad = fnet / *rch + th2 - sfctmp;
    //      A = ELCP * (Q2SAT - Q2)
    a = elcp1 * (q2sat - q2);
    epsca = (a * *rr + rad * delta) / (delta + *rr);
    //ETP = EPSCA * RCH / LSUBC
    etp = epsca * *rch / lvs;
    *o_epsca = epsca;
    return etp;
}

double xnoah_util_calc_epsca(int ice,double sncovr,double sfcprs, double t2v,double T1,double ch, double etp)
{
    
    double lvs,rho;
	double epsca;



    // const:
    double lsubc,lsubs,sigma,rch,rd,cp;

    
    lsubc = 2.501000e+6;
    sigma = 5.67e-8;
	lsubs = 2.83e+6;
   
	rd = 287.04; // R_spec for dry air [J / (kg*K)] -- Gas Konstante
	cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
	
	if (ice==0)
        {            
            lvs = (1.0-sncovr)*lsubc + sncovr*lsubs;
        }
    else
        {
            if (T1> 273.15)
                {                    
                    lvs = lsubc;
                }
            else
                {                    
                    lvs = lsubs;
                }
        }
		
    rho = sfcprs / (rd * t2v);		
	
	rch = rho * cp * ch;
	
	etp = etp / LVH2O;
	
	epsca = etp / rch * lvs;
        
    return epsca;
}



// return FLIMIT
double xnoah_util_FAC2MIT(double SMCMAX)
{
    double FLIMIT;

    FLIMIT = 0.90;

    if ( SMCMAX == 0.395 )
        {
            FLIMIT = 0.59;
        }
    else if  ( ( SMCMAX == 0.434 ) || ( SMCMAX == 0.404 ) )
        {
            FLIMIT = 0.85;
        }
    else if ( ( SMCMAX == 0.465 ) || ( SMCMAX == 0.406 ) )
        {
            FLIMIT = 0.86;
        }
    else if ( ( SMCMAX == 0.476 ) || ( SMCMAX == 0.439 ) )
        {
            FLIMIT = 0.74;
        }
    else if ( ( SMCMAX == 0.200 ) || ( SMCMAX == 0.464 ) )
        {
            FLIMIT = 0.80;
        }

// TODO: (Estimate FLIMIT)
    FLIMIT = 0.74;
    return FLIMIT;
}


/*INTEGER, INTENT(IN)   :: NSOIL
      INTEGER               :: K, KK

      REAL, DIMENSION(1:NSOIL), INTENT(IN):: A, B, D
      REAL, DIMENSION(1:NSOIL),INTENT(INOUT):: C,P,DELTA*/

//INVERT (SOLVE) THE TRI-DIAGONAL MATRIX PROBLEM SHOWN BELOW:
// ###                                            ### ###  ###   ###  ###
// #B(1), C(1),  0  ,  0  ,  0  ,   . . .  ,    0   # #      #   #      #
// #A(2), B(2), C(2),  0  ,  0  ,   . . .  ,    0   # #      #   #      #
// # 0  , A(3), B(3), C(3),  0  ,   . . .  ,    0   # #      #   # D(3) #
// # 0  ,  0  , A(4), B(4), C(4),   . . .  ,    0   # # P(4) #   # D(4) #
// # 0  ,  0  ,  0  , A(5), B(5),   . . .  ,    0   # # P(5) #   # D(5) #
// # .                                          .   # #  .   # = #   .  #
// # .                                          .   # #  .   #   #   .  #
// # .                                          .   # #  .   #   #   .  #
// # 0  , . . . , 0 , A(M-2), B(M-2), C(M-2),   0   # #P(M-2)#   #D(M-2)#
// # 0  , . . . , 0 ,   0   , A(M-1), B(M-1), C(M-1)# #P(M-1)#   #D(M-1)#
// # 0  , . . . , 0 ,   0   ,   0   ,  A(M) ,  B(M) # # P(M) #   # D(M) #
// ###                                            ### ###  ###   ###  ###

void xnoah_util_ROSR12 (double **P,double *A,double *B,double **C,double *D,double **DELTA,int NSOIL)
{
    int K;
    int KK;
    // INITIALIZE EQN COEF C FOR THE LOWEST SOIL LAYER
    (*C)[NSOIL-1] = 0.0;
    //SOLVE THE COEFS FOR THE 1ST SOIL LAYER
    (*P)[0] = - (*C)[0]  / B[0];

    PRINT_ARRAY((*P));
    
    //SOLVE THE COEFS FOR SOIL LAYERS 2 THRU NSOIL
    (*DELTA)[0] = D[0] / B[0];
    for (K=1; K<NSOIL; K++)
        {
            (*P)[K] = - (*C)[K] * ( 1.0 / (B[K] + A[K] * (*P)[K-1]) );
            (*DELTA)[K] = (D[K] - A[K]* (*DELTA)[K -1])* (1.0/ (B[K] + A[K] * (*P)[K -1]));
        }
    PRINT_ARRAY((*P));

    // SET P TO DELTA FOR LOWEST SOIL LAYER
    (*P)[NSOIL-1] = (*DELTA)[NSOIL-1];
    // ADJUST P FOR SOIL LAYERS 2 THRU NSOIL
    PRINT_ARRAY((*P));
    for (K=1; K<NSOIL; K++)
        {
            KK = NSOIL - K - 1;
            (*P) [KK] = (*P)[KK] * (*P)[KK +1] + (*DELTA)[KK];
        }
    PRINT_ARRAY((*P));

}

double xnoah_util_calc_T1(double fdown, double emissi,double sfctmp,double ch,double q2, double th2, double sfcprs, double eta, double etp, double epsca, double zsoil1, double df1, double stc1, double ssoil)
{
	double sigma = 5.67e-8;
	double t24;
	double rch,rho,cp,rd,t2v;
	double yynum,yy,rr;
	double beta,zz1, T1;
	char S[1000];

	cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
	rd = 287.04; // R_spec for dry air [J / (kg*K)] -- Gas Konstante

	t2v = sfctmp * (1.0 + 0.61 * q2);
	t24 = sfctmp*sfctmp*sfctmp*sfctmp;
/*
	if (t2v==0.0) 
		{	
			PRINT_ERROR("t2v==0.0");
			return 273.15;
		}
	if (sfcprs==0.0) 
		{
			PRINT_ERROR("sfcprs==0.0");
			return 273.15;
		}
	
	if (emissi==0.0) 
		{
			PRINT_ERROR("emissi==0.0");
			return 273.15;
		}*/


	rho = sfcprs / (rd * t2v);

	rch = rho * cp * ch;

	rr = emissi*t24 * 6.48e-8 / (sfcprs * ch) + 1.0;

	yynum = fdown - emissi*sigma * t24;

	if (etp<=0.0)		
		{
		  if (eta<=0)
			{
				beta = 0.0;
			} else
			{
				beta = 1.0;
			}
		} else
		{
		  beta = eta*LVH2O/etp;
		}
	
	//printf("%f\n",beta);

	//yy = sfctmp + (yynum / rch + th2  - sfctmp - beta * epsca) / rr;
	

	zz1 = 1.0;// zz1 = df1 / (-0.5 * zsoil1 * rch * rr) + 1.0;
	
	yy = stc1 - 0.5 * ssoil * zsoil1 * zz1  / df1;

	T1 = (yy + (zz1 - 1.0)  * stc1) / zz1;
	
	sprintf(S,"%f %f %f %f %f",T1,stc1,ssoil,zsoil1,df1);
	
	printf("%s\n",S);

	return T1;


}


/*INTEGER, INTENT(IN)       :: NSOIL
      INTEGER                   :: I, K, KK11

      REAL, INTENT(IN)          :: CMCMAX, DT, SMCMAX
      REAL, INTENT(OUT)         :: RUNOFF3
      REAL, INTENT(INOUT)       :: CMC
      REAL, DIMENSION(1:NSOIL), INTENT(IN)     :: SH2OIN, SICE, ZSOIL
      REAL, DIMENSION(1:NSOIL), INTENT(OUT)    :: SH2OOUT
      REAL, DIMENSION(1:NSOIL), INTENT(INOUT)  :: RHSTT, SMC
      REAL, DIMENSION(1:NSOIL), INTENT(INOUT)  :: AI, BI, CI
      REAL, DIMENSION(1:NSOIL)  :: RHSTTin
      REAL, DIMENSION(1:NSOIL)  :: CIin
      REAL                      :: DDZ, RHSCT, STOT, WPLUS*/

//CALCULATE/UPDATE SOIL MOISTURE CONTENT VALUES AND CANOPY MOISTURE
// CONTENT VALUES.
void xnoah_util_SSTEP (double **SH2OOUT,double *SH2OIN,double *CMC,double **RHSTT,double RHSCT,double DT,
                       int NSOIL,double *SMCMAX,double CMCMAX,double *RUNOFF3,double *ZSOIL,double **SMC,double *SICE,
                       double **AI,double **BI,double **CI)
{
    int K,KK11;
    double *RHSTTin,*CIin;
    double WPLUS,DDZ,STOT;

    RHSTTin = (double*) g_malloc0(sizeof(double)*NSOIL);
    CIin = (double*) g_malloc0(sizeof(double)*NSOIL);

// CREATE 'AMOUNT' VALUES OF VARIABLES TO BE INPUT TO THE
// TRI-DIAGONAL MATRIX ROUTINE.

    for (K=0; K<NSOIL; K++)
        {
            (*RHSTT)[K] = (*RHSTT)[K] * DT;
            (*AI)[K] = (*AI)[K] * DT;
            (*BI)[K] = 1.0 + (*BI)[K] * DT;
            (*CI)[K] = (*CI)[K] * DT;
        }

// COPY VALUES FOR INPUT VARIABLES BEFORE CALL TO ROSR12

    for (K=0; K<NSOIL; K++)
        {
            RHSTTin[K] = (*RHSTT)[K];
            CIin[K] = (*CI) [K];
        }

// CALL ROSR12 TO SOLVE THE TRI-DIAGONAL MATRIX

    PRINT_ARRAY((*CI));
    PRINT_ARRAY(CIin);
    PRINT_ARRAY((*RHSTT));

    xnoah_util_ROSR12 (CI,*AI,*BI,&CIin,RHSTTin,RHSTT,NSOIL);


    PRINT_ARRAY((*CI));
    PRINT_ARRAY(CIin);
    PRINT_ARRAY((*RHSTT));

// SUM THE PREVIOUS SMC VALUE AND THE MATRIX SOLUTION TO GET A
// NEW VALUE.  MIN ALLOWABLE VALUE OF SMC WILL BE 0.02.
// RUNOFF3: RUNOFF WITHIN SOIL LAYERS

    WPLUS = 0.0;
    *RUNOFF3 = 0.0;

    DDZ = - ZSOIL[0];

    for (K=0; K<NSOIL; K++)
        {
            if (K != 0) DDZ = ZSOIL[K - 1] - ZSOIL[K];
            (*SH2OOUT)[K] = SH2OIN[K] + (*CI)[K] + WPLUS / DDZ;
            STOT = (*SH2OOUT)[K] + SICE[K];
            if (STOT > SMCMAX[K])
                {
                    if (K == 0)
                        {
                            DDZ = - ZSOIL[1];
                        }
                    else
                        {
                            KK11 = K - 1;
                            DDZ = - ZSOIL[K] + ZSOIL[KK11];
                        }
                    WPLUS = (STOT - SMCMAX[K]) * DDZ;
                }
            else
                {
                    WPLUS=0.0;
                }

            (*SMC) [K] = MAX ( MIN (STOT,SMCMAX[K]),0.02 );
            (*SH2OOUT)[K] = MAX ( ((*SMC)[K] - SICE[K]),0.0);
        }

    // UPDATE CANOPY WATER CONTENT/INTERCEPTION (CMC).  CONVERT RHSCT TO
    // AN 'AMOUNT' VALUE AND ADD TO PREVIOUS CMC VALUE TO GET NEW CMC.

    (*RUNOFF3) = WPLUS;
    (*CMC) = *CMC + DT * RHSCT;
    if (*CMC < 1.E-20) (*CMC) = 0.0;
    *CMC = MIN (*CMC,CMCMAX);

    g_free(RHSTTin);
    g_free(CIin);
}



// CALCULATE SOIL WATER DIFFUSIVITY AND SOIL HYDRAULIC CONDUCTIVITY.
void xnoah_util_WDFCND(double *WDF,double *WCND,double SMC,double SMCMAX,double BEXP,double DKSAT,double DWSAT,double SICEMAX)
{
    double FACTR1,FACTR2,EXPON,VKWGT;
//  CALC THE RATIO OF THE ACTUAL TO THE MAX PSBL SOIL H2O CONTENT

    FACTR1 = 0.05 / SMCMAX;
    //PREP AN EXPNTL COEF AND CALC THE SOIL WATER DIFFUSIVITY

    FACTR2 = SMC / SMCMAX;
    FACTR1 = MIN(FACTR1,FACTR2);
    EXPON = BEXP + 2.0;

    // FROZEN SOIL HYDRAULIC DIFFUSIVITY.  VERY SENSITIVE TO THE VERTICAL
    // GRADIENT OF UNFROZEN WATER. THE LATTER GRADIENT CAN BECOME VERY
    // EXTREME IN FREEZING/THAWING SITUATIONS, AND GIVEN THE RELATIVELY
    // FEW AND THICK SOIL LAYERS, THIS GRADIENT SUFFERES SERIOUS
    // TRUNCTION ERRORS YIELDING ERRONEOUSLY HIGH VERTICAL TRANSPORTS OF
    // UNFROZEN WATER IN BOTH DIRECTIONS FROM HUGE HYDRAULIC DIFFUSIVITY.
    // THEREFORE, WE FOUND WE HAD TO ARBITRARILY CONSTRAIN WDF

    //   VERSION D_10CM: ........  FACTR1 = 0.2/SMCMAX
    // WEIGHTED APPROACH...................... PABLO GRUNMANN, 28_SEP_1999.

    *WDF = DWSAT * pow(FACTR2,EXPON);
    if (SICEMAX > 0.0)
        {
            VKWGT = 1.0/ (1.0 + pow(500.0 * SICEMAX,3.0));
            *WDF = VKWGT * *WDF + (1.0 - VKWGT)* DWSAT * pow(FACTR1, EXPON);
        }
    // RESET THE EXPNTL COEF AND CALC THE HYDRAULIC CONDUCTIVITY
    EXPON = (2.0 * BEXP) + 3.0;
    *WCND = DKSAT * pow(FACTR2,EXPON);

}



/*INTEGER, INTENT(IN)       :: NSOIL
      INTEGER                   :: IALP1, IOHINF, J, JJ,  K, KS
      REAL, INTENT(IN)          :: BEXP, DKSAT, DT, DWSAT, EDIR, FRZX,  &
                                   KDT, PCPDRP, SLOPE, SMCMAX, SMCWLT
      REAL, INTENT(OUT)         :: RUNOFF1, RUNOFF2
      REAL, DIMENSION(1:NSOIL), INTENT(IN)   :: ET, SH2O, SH2OA, SICE,  &
                                                ZSOIL
      REAL, DIMENSION(1:NSOIL), INTENT(OUT)  :: RHSTT
      REAL, DIMENSION(1:NSOIL), INTENT(OUT)  :: AI, BI, CI
      REAL, DIMENSION(1:NSOIL)  :: DMAX
      REAL                      :: ACRT, DD, DDT, DDZ, DDZ2, DENOM,     &
                                   DENOM2,DICE, DSMDZ, DSMDZ2, DT1,     &
                                   FCR,INFMAX,MXSMC,MXSMC2,NUMER,PDDUM, &
                                   PX, SICEMAX,SLOPX, SMCAV, SSTT,      &
                                   SUM, VAL, WCND, WCND2, WDF, WDF2
      INTEGER, PARAMETER        :: CVFRZ = 3*/

// CALCULATE THE RIGHT HAND SIDE OF THE TIME TENDENCY TERM OF THE SOIL
// WATER DIFFUSION EQUATION.  ALSO TO COMPUTE ( PREPARE ) THE MATRIX
// COEFFICIENTS FOR THE TRI-DIAGONAL MATRIX OF THE IMPLICIT TIME SCHEME.
void xnoah_util_SRT(double **RHSTT,double EDIR,double *ET,double *SH2O,double *SH2OA,int NSOIL,double PCPDRP,
                    double *ZSOIL,double *DWSAT,double *DKSAT,double *SMCMAX,double *BEXP,double *RUNOFF1,
                    double *RUNOFF2,double DT,double *SMCWLT,double SLOPE,double KDT,double *FRZX,double *SICE,double **AI,double **BI,double **CI)
{
    //int CVFRZ = 3;
    int IOHINF,KS,K;
    double SICEMAX,PDDUM,DT1,SMCAV,DICE,DD,VAL,DDT,PX,INFMAX,FCR,MXSMC,WDF,WCND,DENOM2,DDZ2,SSTT,DSMDZ,DDZ;
    double SLOPX,MXSMC2,WDF2,WCND2,DENOM,DSMDZ2,NUMER;
    //double SUM,ACRT;
    //int IALP1,J,JJ;
    double *DMAX;

    DMAX = (double*) g_malloc0(sizeof(double)*NSOIL);

//
// FROZEN GROUND VERSION:
// REFERENCE FROZEN GROUND PARAMETER, CVFRZ, IS A SHAPE PARAMETER OF
// AREAL DISTRIBUTION FUNCTION OF SOIL ICE CONTENT WHICH EQUALS 1/CV.
// CV IS A COEFFICIENT OF SPATIAL VARIATION OF SOIL ICE CONTENT.  BASED
// ON FIELD DATA CV DEPENDS ON AREAL MEAN OF FROZEN DEPTH, AND IT CLOSE
// TO CONSTANT = 0.6 IF AREAL MEAN FROZEN DEPTH IS ABOVE 20 CM.  THAT IS
// WHY PARAMETER CVFRZ = 3 (INT{1/0.6*0.6}).
// CURRENT LOGIC DOESN'T ALLOW CVFRZ BE BIGGER THAN 3
// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
// DETERMINE RAINFALL INFILTRATION RATE AND RUNOFF.  INCLUDE THE
// INFILTRATION FORMULE FROM SCHAAKE AND KOREN MODEL.
// MODIFIED BY Q DUAN
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// LET SICEMAX BE THE GREATEST, IF ANY, FROZEN WATER CONTENT WITHIN SOIL
// LAYERS.


    IOHINF = 1;
    SICEMAX = 0.0;
    for (KS = 0; KS < NSOIL; KS++)
        {
            if (SICE[KS] >  SICEMAX) SICEMAX = SICE [KS];
            //DETERMINE RAINFALL INFILTRATION RATE AND RUNOFF
        }

    PDDUM = PCPDRP;
    *RUNOFF1 = 0.0;


    //MODIFIED BY Q. DUAN, 5/16/94
    // IF (IOHINF == 1) THEN

    if (PCPDRP !=  0.0)
        {
            DT1 = DT /86400.0;
            SMCAV = SMCMAX[0] - SMCWLT[0];
            //FROZEN GROUND VERSION:
            DMAX[0] =  - ZSOIL[0]* SMCAV;

            DICE = - ZSOIL [0] * SICE [0];
            DMAX[0]= DMAX [0]* (1.0- (SH2OA [0] + SICE [0] - SMCWLT[0])/ SMCAV);

            DD = DMAX [0];

            //FROZEN GROUND VERSION:

            for (KS=1; KS<NSOIL; KS++)
                {
                    DICE = DICE+ ( ZSOIL [KS-1] - ZSOIL[KS] ) * SICE[KS];
                    DMAX[KS] = (ZSOIL [KS -1] - ZSOIL [KS])* SMCAV;
                    DMAX[KS] = DMAX[KS]* (1.0- (SH2OA [KS] + SICE [KS]- SMCWLT[KS])/ SMCAV);
                    DD = DD+ DMAX[KS];
                }

            // VAL = (1.-EXP(-KDT*SQRT(DT1)))
            // IN BELOW, REMOVE THE SQRT IN ABOVE



            VAL = (1.0 - exp ( - KDT * DT1));
            DDT = DD * VAL;
            PX = PCPDRP * DT;
            if (PX <  0.0) PX = 0.0;


            // FROZEN GROUND VERSION:
            // REDUCTION OF INFILTRATION BASED ON FROZEN GROUND PARAMETERS

            INFMAX = (PX * (DDT / (PX + DDT)))/ DT;
            FCR = 1.0;
            //TODO: keine Ahnung, wie dieser ICE Teil funktioniert:
            /*
            if (DICE >  1.E-2)
            {
               ACRT = CVFRZ * FRZX / DICE;
               SUM = 1.0;
               IALP1 = CVFRZ - 1;
               for (J=0;J<IALP1;J++)
                {
                    for (JJ=J+1;JJ<IALLP1;;JJ++)
                        {

                        }
                }


            DO J = 1,IALP1
                  K = 1
                  DO JJ = J +1,IALP1
                     K = K * JJ
                  END DO
                  SUM = SUM + (ACRT ** ( CVFRZ - J)) / FLOAT (K)
               END DO
               FCR = 1. - EXP ( - ACRT) * SUM
            }
             */

            // CORRECTION OF INFILTRATION LIMITATION:
            // IF INFMAX .LE. HYDROLIC CONDUCTIVITY ASSIGN INFMAX THE VALUE OF
            // HYDROLIC CONDUCTIVITY
            INFMAX = INFMAX * FCR;

            MXSMC = SH2OA [0];

            xnoah_util_WDFCND (&WDF,&WCND,MXSMC,SMCMAX[0],BEXP[0],DKSAT[0],DWSAT[0],SICEMAX);
            INFMAX = MAX (INFMAX,WCND);

            INFMAX = MIN (INFMAX,PX/DT);
            if (PCPDRP >  INFMAX)
                {
                    *RUNOFF1 = PCPDRP - INFMAX;
                    PDDUM = INFMAX;
                }
        }

    // TO AVOID SPURIOUS DRAINAGE BEHAVIOR, 'UPSTREAM DIFFERENCING' IN LINE
    // BELOW REPLACED WITH NEW APPROACH IN 2ND LINE:
    // 'MXSMC = MAX(SH2OA(1), SH2OA(2))'

    MXSMC = SH2OA[0];

    xnoah_util_WDFCND (&WDF,&WCND,MXSMC,SMCMAX[0],BEXP[0],DKSAT[0],DWSAT[0],
                       SICEMAX);

    // CALC THE MATRIX COEFFICIENTS AI, BI, AND CI FOR THE TOP LAYER

    DDZ = 1.0 / ( - 0.5 * ZSOIL [1] );
    (*AI) [0] = 0.0;
    (*BI) [0] = WDF * DDZ / ( - ZSOIL [0] );

    // CALC RHSTT FOR THE TOP LAYER AFTER CALC'NG THE VERTICAL SOIL MOISTURE
    // GRADIENT BTWN THE TOP AND NEXT TO TOP LAYERS.

    (*CI) [0] = - (*BI) [0];
    DSMDZ = ( SH2O [0] - SH2O [1] ) / ( - 0.5 * ZSOIL [1] );
    (*RHSTT) [0] = (WDF * DSMDZ + WCND- PDDUM + EDIR + ET [0])/ ZSOIL [0];

    // INITIALIZE DDZ2
    SSTT = WDF * DSMDZ + WCND+ EDIR + ET [0];

    // LOOP THRU THE REMAINING SOIL LAYERS, REPEATING THE ABV PROCESS
    DDZ2 = 0.0;
    for (K=1; K<NSOIL; K++)
        {

            DENOM2 = (ZSOIL [K -1] - ZSOIL [K]);
            if (K != NSOIL-1)
                {

                    // AGAIN, TO AVOID SPURIOUS DRAINAGE BEHAVIOR, 'UPSTREAM DIFFERENCING' IN
                    // LINE BELOW REPLACED WITH NEW APPROACH IN 2ND LINE:
                    // 'MXSMC2 = MAX (SH2OA(K), SH2OA(K+1))'

                    SLOPX = 1.0;

                    MXSMC2 = SH2OA [K];
                    xnoah_util_WDFCND (&WDF2,&WCND2,MXSMC2,SMCMAX[K],BEXP[K],DKSAT[K],DWSAT[K], SICEMAX);

                    // CALC SOME PARTIAL PRODUCTS FOR LATER USE IN CALC'NG RHSTT
                    DENOM = (ZSOIL [K -1] - ZSOIL [K +1]);


                    // CALC THE MATRIX COEF, CI, AFTER CALC'NG ITS PARTIAL PRODUCT

                    DSMDZ2 = (SH2O [K] - SH2O [K +1]) / (DENOM * 0.5);
                    DDZ2 = 2.0 / DENOM;
                    (*CI) [K] = - WDF2 * DDZ2 / DENOM2;

                }
            else
                {

                    // SLOPE OF BOTTOM LAYER IS INTRODUCED

                    // RETRIEVE THE SOIL WATER DIFFUSIVITY AND HYDRAULIC CONDUCTIVITY FOR
                    // THIS LAYER

                    SLOPX = SLOPE;
                    xnoah_util_WDFCND (&WDF2,&WCND2,SH2OA [NSOIL-1],SMCMAX[NSOIL-1],BEXP[NSOIL-1],DKSAT[NSOIL-1],DWSAT[NSOIL-1],SICEMAX);

                    // CALC A PARTIAL PRODUCT FOR LATER USE IN CALC'NG RHSTT


                    // SET MATRIX COEF CI TO ZERO

                    DSMDZ2 = 0.0;
                    (*CI) [K] = 0.0;

                    // CALC RHSTT FOR THIS LAYER AFTER CALC'NG ITS NUMERATOR

                }
            NUMER = (WDF2 * DSMDZ2) + SLOPX * WCND2- (WDF * DSMDZ)
                    - WCND+ ET [K];


            // CALC MATRIX COEFS, AI, AND BI FOR THIS LAYER

            (*RHSTT) [K] = NUMER / ( - DENOM2);
			
			
            (*AI) [K] = - WDF * DDZ / DENOM2;


            // RESET VALUES OF WDF, WCND, DSMDZ, AND DDZ FOR LOOP TO NEXT LYR
            // RUNOFF2:  SUB-SURFACE OR BASEFLOW RUNOFF

            (*BI) [K] = - ( (*AI) [K] + (*CI) [K] );
            if (K == NSOIL-1)
                {
                    *RUNOFF2 = SLOPX * WCND2;
                }
            else
                {
                    WDF = WDF2;
                    WCND = WCND2;
                    DSMDZ = DSMDZ2;
                    DDZ = DDZ2;
                }
        }

    g_free(DMAX);

}




/*INTEGER, INTENT(IN)   :: NSOIL
     INTEGER               :: I,K

     REAL, INTENT(IN)      :: BEXP, CMCMAX, DKSAT,DWSAT, DT, EC, EDIR,  &
                              KDT, PRCP1, SHDFAC, SLOPE, SMCMAX, SMCWLT
     REAL, INTENT(OUT)                      :: DRIP, RUNOFF1, RUNOFF2, RUNOFF3
     REAL, INTENT(INOUT)   :: CMC
     REAL, DIMENSION(1:NSOIL), INTENT(IN)   :: ET,ZSOIL
     REAL, DIMENSION(1:NSOIL), INTENT(INOUT):: SMC, SH2O
     REAL, DIMENSION(1:NSOIL)             :: AI, BI, CI, STCF,RHSTS, RHSTT, &
                                             SICE, SH2OA, SH2OFG
     REAL                  :: DUMMY, EXCESS,FRZFACT,PCPDRP,RHSCT,TRHSCT
     REAL :: FAC2
     REAL :: FLIMIT */

//CALCULATE SOIL MOISTURE FLUX.  THE SOIL MOISTURE CONTENT (SMC - A PER
// UNIT VOLUME MEASUREMENT) IS A DEPENDENT VARIABLE THAT IS UPDATED WITH
// PROGNOSTIC EQNS. THE CANOPY MOISTURE CONTENT (CMC) IS ALSO UPDATED.
// FROZEN GROUND VERSION:  NEW STATES ADDED: SH2O, AND FROZEN GROUND
// CORRECTION FACTOR, FRZFACT AND PARAMETER SLOPE.
void xnoah_util_SMFLX( double **SMC,int NSOIL,double *CMC,double DT,double PRCP1,double *ZSOIL,
                       double **SH2O,double SLOPE,double KDT,double *FRZFACT,
                       double *SMCMAX,double *BEXP,double *SMCWLT,double *DKSAT,double *DWSAT,
                       double SHDFAC,double CMCMAX,
                       double *RUNOFF1,double *RUNOFF2,double *RUNOFF3,
                       double EDIR,double EC,double *ET,
                       double *DRIP)
{
    double DUMMY,RHSCT,TRHSCT,EXCESS,PCPDRP,FAC2;
    double *FLIMIT,*RHSTT,*SICE,*AI,*BI,*CI,*SH2OFG,*SH2OA;
    int I,K;

    
    FLIMIT = (double*) g_malloc0(sizeof(double)*NSOIL);
    RHSTT = (double*) g_malloc0(sizeof(double)*NSOIL);
    SICE = (double*) g_malloc0(sizeof(double)*NSOIL);
    AI = (double*) g_malloc0(sizeof(double)*NSOIL);
    BI = (double*) g_malloc0(sizeof(double)*NSOIL);
    CI = (double*) g_malloc0(sizeof(double)*NSOIL);
    SH2OFG = (double*) g_malloc0(sizeof(double)*NSOIL);
    SH2OA = (double*) g_malloc0(sizeof(double)*NSOIL);

    //COMPUTE THE RIGHT HAND SIDE OF THE CANOPY EQN TERM ( RHSCT )

    DUMMY = 0.0;

// CONVERT RHSCT (A RATE) TO TRHSCT (AN AMOUNT) AND ADD IT TO EXISTING
// CMC.  IF RESULTING AMT EXCEEDS MAX CAPACITY, IT BECOMES DRIP AND WILL
// FALL TO THE GRND.

    RHSCT = SHDFAC * PRCP1- EC;
    *DRIP = 0.0;
    TRHSCT = DT * RHSCT;
    EXCESS = *CMC + TRHSCT;

// ----------------------------------------------------------------------
// PCPDRP IS THE COMBINED PRCP1 AND DRIP (FROM CMC) THAT GOES INTO THE
// SOIL

    if (EXCESS > CMCMAX)
        {
            *DRIP = EXCESS - CMCMAX;
        }
    PCPDRP = (1.0 - SHDFAC) * PRCP1+ *DRIP / DT;

    // STORE ICE CONTENT AT EACH SOIL LAYER BEFORE CALLING SRT and SSTEP

    for (I=0; I<NSOIL; I++)
        {
            SICE[I] = (*SMC)[I] - (*SH2O)[I];
        }

// CALL SUBROUTINES SRT AND SSTEP TO SOLVE THE SOIL MOISTURE
// TENDENCY EQUATIONS.
// IF THE INFILTRATING PRECIP RATE IS NONTRIVIAL,
//   (WE CONSIDER NONTRIVIAL TO BE A PRECIP TOTAL OVER THE TIME STEP
//    EXCEEDING ONE ONE-THOUSANDTH OF THE WATER HOLDING CAPACITY OF
//    THE FIRST SOIL LAYER)
// THEN CALL THE SRT/SSTEP SUBROUTINE PAIR TWICE IN THE MANNER OF
//   TIME SCHEME "F" (IMPLICIT STATE, AVERAGED COEFFICIENT)
//   OF SECTION 2 OF KALNAY AND KANAMITSU (1988, MWR, VOL 116,
//   PAGES 1945-1958)TO MINIMIZE 2-DELTA-T OSCILLATIONS IN THE
//   SOIL MOISTURE VALUE OF THE TOP SOIL LAYER THAT CAN ARISE BECAUSE
//   OF THE EXTREME NONLINEAR DEPENDENCE OF THE SOIL HYDRAULIC
//   DIFFUSIVITY COEFFICIENT AND THE HYDRAULIC CONDUCTIVITY ON THE
//   SOIL MOISTURE STATE
// OTHERWISE CALL THE SRT/SSTEP SUBROUTINE PAIR ONCE IN THE MANNER OF
//   TIME SCHEME "D" (IMPLICIT STATE, EXPLICIT COEFFICIENT)
//   OF SECTION 2 OF KALNAY AND KANAMITSU
// PCPDRP IS UNITS OF KG/M**2/S OR MM/S, ZSOIL IS NEGATIVE DEPTH IN M
// ----------------------------------------------------------------------
//  According to Dr. Ken Mitchell's suggestion, add the second contraint
//  to remove numerical instability of runoff and soil moisture
//  FLIMIT is a limit value for FAC2

    FAC2=0.0;
    for (I=0; I<NSOIL; I++)
        {
            FAC2=MAX(FAC2,(*SH2O)[I]/SMCMAX[I]);
        }

    for (I=0; I<NSOIL; I++)
        {
            FLIMIT[I] = xnoah_util_FAC2MIT(SMCMAX[I]);
        }

//FROZEN GROUND VERSION:
// SMC STATES REPLACED BY SH2O STATES IN SRT SUBR.  SH2O & SICE STATES
// INC&UDED IN SSTEP SUBR.  FROZEN GROUND CORRECTION FACTOR, FRZFACT
// ADDED.  ALL WATER BALANCE CALCULATIONS USING UNFROZEN WATER

//TODO: Funktioniert das auch wenn SMCMAX unterschiedlich ist für jede Schicht?
    if ( ( (PCPDRP * DT) > (0.0001*1000.0* (- ZSOIL[0])* SMCMAX[0]) )
            || (FAC2 > FLIMIT[0]) )
        {
            xnoah_util_SRT (&RHSTT,EDIR,ET,*SH2O,*SH2O,NSOIL,PCPDRP,ZSOIL,
                            DWSAT,DKSAT,SMCMAX,BEXP,RUNOFF1,
                            RUNOFF2,DT,SMCWLT,SLOPE,KDT,FRZFACT,SICE,&AI,&BI,&CI);
            
                          
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);

            xnoah_util_SSTEP (&SH2OFG,*SH2O,&DUMMY,&RHSTT,RHSCT,DT,NSOIL,SMCMAX,
                              CMCMAX,RUNOFF3,ZSOIL,SMC,SICE,&AI,&BI,&CI);

            PRINT_ARRAY(SH2OFG);
            PRINT_ARRAY(CMC);
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(RUNOFF3);
            PRINT_ARRAY((*SMC));
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);
                                           
            for (K=0; K<NSOIL; K++)
                {
                    SH2OA[K] = ((*SH2O) [K] + SH2OFG [K]) * 0.5;
                }
            xnoah_util_SRT (&RHSTT,EDIR,ET,*SH2O,SH2OA,NSOIL,PCPDRP,ZSOIL,
                            DWSAT,DKSAT,SMCMAX,BEXP,RUNOFF1,
                            RUNOFF2,DT,SMCWLT,SLOPE,KDT,FRZFACT,SICE,&AI,&BI,&CI);
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);
          
                            
            xnoah_util_SSTEP (SH2O,*SH2O,CMC,&RHSTT,RHSCT,DT,NSOIL,SMCMAX,
                              CMCMAX,RUNOFF3,ZSOIL,SMC,SICE,&AI,&BI,&CI);

             PRINT_ARRAY(SH2OFG);
            PRINT_ARRAY(CMC);
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(RUNOFF3);
            PRINT_ARRAY((*SMC));
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);
                             
        }
    else
        {
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);
            
            xnoah_util_SRT (&RHSTT,EDIR,ET,*SH2O,*SH2O,NSOIL,PCPDRP,ZSOIL,
                            DWSAT,DKSAT,SMCMAX,BEXP,RUNOFF1,
                            RUNOFF2,DT,SMCWLT,SLOPE,KDT,FRZFACT,SICE,&AI,&BI,&CI);
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);
          
            xnoah_util_SSTEP (SH2O,*SH2O,CMC,&RHSTT,RHSCT,DT,NSOIL,SMCMAX,
                              CMCMAX,RUNOFF3,ZSOIL,SMC,SICE,&AI,&BI,&CI);

             PRINT_ARRAY((*SH2O));
            PRINT_ARRAY(CMC);
            PRINT_ARRAY(RHSTT);
            PRINT_ARRAY(RUNOFF3);
            PRINT_ARRAY((*SMC));
            PRINT_ARRAY(AI);
            PRINT_ARRAY(BI);
            PRINT_ARRAY(CI);                            
        }

    g_free(RHSTT);
    g_free(FLIMIT);
    g_free(SICE);
    g_free(AI);
    g_free(BI);
    g_free(CI);
    g_free(SH2OFG);
    g_free(SH2OA);
}

// out: double *TH2, double *T1V, double *TH2V
void xnoah_util_CALTMP(double T1, double SFCTMP, double SFCPRS, double ZLVL, double Q2, double *TH2, double *T1V, double *TH2V )
{

    *TH2 = SFCTMP + ( 0.0098 * ZLVL);
    *T1V= T1 * (1.0+ 0.61 * Q2);
    *TH2V = *TH2 * (1.0+ 0.61 * Q2);
    //T2V = SFCTMP * ( 1.0 + 0.61 * Q2 );
    //RHO = SFCPRS/(RD * T2V)

}

//PURPOSE:  TO CALCULATE VALUES OF SAT. VAPOR PRESSURE E [ Pa ]
//            FORMULAS AND CONSTANTS FROM ROGERS AND YAU, 1989.

//                         ADDED BY PABLO J. GRUNMANN, 7/9/97.

static double EsFuncT (double T)
{
    //double LVH2O = 2.501e6; // Latent Heat of Vaporization   [ J kg{-1} ]
    double ESO   = 611.2;   // Sat. vapor pres. at T = T0    [ Pa ]
    double CW    = 4187.0; //  Specific heat of liquid water [ J kg{-1} K{-1} ]
    double RV    = 461.5; // Water vapor gas constant      [ J kg{-1} K{-1} ]
    double CPV   = 1870.0;  // Specific heat of water vapor  [ J kg{-1} K{-1} ]
    double TO    = 273.15;
    double LW;
    double E; // ret;
    LW = LVH2O - ( CW - CPV ) * ( T - TO );
    E = ESO*exp (LW*(1.0/TO - 1.0/T)/RV);
    return E;
}

// rhf: relative humidity %
double xnoah_util_convert_rel_hum_to_spec_hum(double sfctmp, double sfcprs, double rhf)
{   
    double eps   = 0.622;//Parameter used to compute Specific Humidity from Pressure and Saturation Vapor Pressure.
    double svp,QS,E,spechumd;
    
    // Im orginal wird noch zwischen den Messwerten interpoliert
    
    rhf = rhf * 1.E-2;
    
    svp = EsFuncT(sfctmp);
    QS = eps * svp / (sfcprs - (1.-eps) * svp);
    E = (sfcprs*svp*rhf)/(sfcprs - svp*(1. - rhf));
    spechumd = (eps*E)/(sfcprs-(1.0-eps)*E);
    if (spechumd < 0.1E-5) spechumd = 0.1e-5;
    if (spechumd >  QS) spechumd = QS*0.99;
    return spechumd;
}

void xnoah_util_CALHUM(double SFCTMP, double SFCPRS, double *Q2SAT, double *DQSDT2)
{
    double A2=17.67;
    double A3=273.15;
    double A4=29.65;
    double ELWV=2.501e6;
    double A23M4=A2*(A3-A4);
    double E0=611.0; 
    double RV=461.0;
    double EPSILON1=0.622;
    double ES;
    
    ES = E0 * exp ( ELWV/RV*(1.0/A3 - 1.0/SFCTMP) );
    *Q2SAT = EPSILON1 * ES / (SFCPRS-(1.0-EPSILON1)*ES);
    *DQSDT2= *Q2SAT * A23M4/pow(SFCTMP-A4,2.0);
    
}
   
   
    
    
/*!CALCULATE CANOPY RESISTANCE WHICH DEPENDS ON INCOMING SOLAR RADIATION,
! AIR TEMPERATURE, ATMOSPHERIC WATER VAPOR PRESSURE DEFICIT AT THE
! LOWEST MODEL LEVEL, AND SOIL MOISTURE (PREFERABLY UNFROZEN SOIL
! MOISTURE RATHER THAN TOTAL)
! ----------------------------------------------------------------------
! SOURCE:  JARVIS (1976), NOILHAN AND PLANTON (1989, MWR), JACQUEMIN AND
! NOILHAN (1990, BLM)
! SEE ALSO:  CHEN ET AL (1996, JGR, VOL 101(D3), 7251-7268), EQNS 12-14
! AND TABLE 2 OF SEC. 3.1.2
! ----------------------------------------------------------------------
! INPUT:
!   SOLAR   INCOMING SOLAR RADIATION
!   CH      SURFACE EXCHANGE COEFFICIENT FOR HEAT AND MOISTURE
!   SFCTMP  AIR TEMPERATURE AT 1ST LEVEL ABOVE GROUND
!   Q2      AIR HUMIDITY AT 1ST LEVEL ABOVE GROUND
!   Q2SAT   SATURATION AIR HUMIDITY AT 1ST LEVEL ABOVE GROUND
!   DQSDT2  SLOPE OF SATURATION HUMIDITY FUNCTION WRT TEMP
!   SFCPRS  SURFACE PRESSURE
!   SMC     VOLUMETRIC SOIL MOISTURE
!   ZSOIL   SOIL DEPTH (NEGATIVE SIGN, AS IT IS BELOW GROUND)
!   NSOIL   NO. OF SOIL LAYERS
!   NROOT   NO. OF SOIL LAYERS IN ROOT ZONE (1.LE.NROOT.LE.NSOIL)
!   XLAI    LEAF AREA INDEX
!   SMCWLT  WILTING POINT
!   SMCREF  REFERENCE SOIL MOISTURE (WHERE SOIL WATER DEFICIT STRESS
!             SETS IN)
! RSMIN, RSMAX, TOPT, RGL, HS ARE CANOPY STRESS PARAMETERS SET IN
!   SURBOUTINE REDPRM
! OUTPUT:
!   PC  PLANT COEFFICIENT
!   RC  CANOPY RESISTANCE*/

/*void xnoah_util_CANRES (double SOLAR,double CH,double SFCTMP,double Q2,double SFCPRS,double *SMC,double *ZSOIL,int NSOIL,
                         double *SMCWLT,double *SMCREF,double RSMIN,RC,PC,NROOT,Q2SAT,DQSDT2,    
                         TOPT,RSMAX,RGL,HS,XLAI,                          
                         RCS,RCT,RCQ,RCSOIL,EMISSI)



      IMPLICIT NONE
      INTEGER, INTENT(IN) :: NROOT,NSOIL
      INTEGER  K
      REAL,    INTENT(IN) :: CH,DQSDT2,HS,Q2,Q2SAT,RSMIN,RGL,RSMAX,        &
                             SFCPRS,SFCTMP,SMCREF,SMCWLT, SOLAR,TOPT,XLAI, &
                             EMISSI
      REAL,DIMENSION(1:NSOIL), INTENT(IN) :: SMC,ZSOIL
      REAL,    INTENT(OUT):: PC,RC,RCQ,RCS,RCSOIL,RCT
      REAL                :: DELTA,FF,GX,P,RR
      REAL, DIMENSION(1:NSOIL) ::  PART
      REAL, PARAMETER     :: SLV = 2.501000E6


! ----------------------------------------------------------------------
! INITIALIZE CANOPY RESISTANCE MULTIPLIER TERMS.
! ----------------------------------------------------------------------
      RCS = 0.0
      RCT = 0.0
      RCQ = 0.0
      RCSOIL = 0.0

! ----------------------------------------------------------------------
! CONTRIBUTION DUE TO INCOMING SOLAR RADIATION
! ----------------------------------------------------------------------
      RC = 0.0
      FF = 0.55*2.0* SOLAR / (RGL * XLAI)
      RCS = (FF + RSMIN / RSMAX) / (1.0+ FF)

! ----------------------------------------------------------------------
! CONTRIBUTION DUE TO AIR TEMPERATURE AT FIRST MODEL LEVEL ABOVE GROUND
! RCT EXPRESSION FROM NOILHAN AND PLANTON (1989, MWR).
! ----------------------------------------------------------------------
      RCS = MAX (RCS,0.0001)
      RCT = 1.0- 0.0016* ( (TOPT - SFCTMP)**2.0)

! ----------------------------------------------------------------------
! CONTRIBUTION DUE TO VAPOR PRESSURE DEFICIT AT FIRST MODEL LEVEL.
! RCQ EXPRESSION FROM SSIB
! ----------------------------------------------------------------------
      RCT = MAX (RCT,0.0001)
      RCQ = 1.0/ (1.0+ HS * (Q2SAT - Q2))

! ----------------------------------------------------------------------
! CONTRIBUTION DUE TO SOIL MOISTURE AVAILABILITY.
! DETERMINE CONTRIBUTION FROM EACH SOIL LAYER, THEN ADD THEM UP.
! ----------------------------------------------------------------------
      RCQ = MAX (RCQ,0.01)
      GX = (SMC (1) - SMCWLT) / (SMCREF - SMCWLT)
      IF (GX  >  1.) GX = 1.
      IF (GX  <  0.) GX = 0.

! ----------------------------------------------------------------------
! USE SOIL DEPTH AS WEIGHTING FACTOR
! ----------------------------------------------------------------------
! ----------------------------------------------------------------------
! USE ROOT DISTRIBUTION AS WEIGHTING FACTOR
!      PART(1) = RTDIS(1) * GX
! ----------------------------------------------------------------------
      PART (1) = (ZSOIL (1)/ ZSOIL (NROOT)) * GX
      DO K = 2,NROOT
         GX = (SMC (K) - SMCWLT) / (SMCREF - SMCWLT)
         IF (GX >  1.) GX = 1.
         IF (GX <  0.) GX = 0.
! ----------------------------------------------------------------------
! USE SOIL DEPTH AS WEIGHTING FACTOR
! ----------------------------------------------------------------------
! ----------------------------------------------------------------------
! USE ROOT DISTRIBUTION AS WEIGHTING FACTOR
!        PART(K) = RTDIS(K) * GX
! ----------------------------------------------------------------------
         PART (K) = ( (ZSOIL (K) - ZSOIL (K -1))/ ZSOIL (NROOT)) * GX
      END DO
      DO K = 1,NROOT
         RCSOIL = RCSOIL + PART (K)
      END DO

! ----------------------------------------------------------------------
! DETERMINE CANOPY RESISTANCE DUE TO ALL FACTORS.  CONVERT CANOPY
! RESISTANCE (RC) TO PLANT COEFFICIENT (PC) TO BE USED WITH POTENTIAL
! EVAP IN DETERMINING ACTUAL EVAP.  PC IS DETERMINED BY:
!   PC * LINERIZED PENMAN POTENTIAL EVAP =
!   PENMAN-MONTEITH ACTUAL EVAPORATION (CONTAINING RC TERM).
! ----------------------------------------------------------------------
      RCSOIL = MAX (RCSOIL,0.0001)

      RC = RSMIN / (XLAI * RCS * RCT * RCQ * RCSOIL)
!      RR = (4.* SIGMA * RD / CP)* (SFCTMP **4.)/ (SFCPRS * CH) + 1.0
      RR = (4.* EMISSI *SIGMA * RD / CP)* (SFCTMP **4.)/ (SFCPRS * CH) &
             + 1.0

      DELTA = (SLV / CP)* DQSDT2

      PC = (RR + DELTA)/ (RR * (1. + RC * CH) + DELTA)

! ----------------------------------------------------------------------
  END SUBROUTINE CANRES
*/

/*
double xnoah_util_month_d(double *a12, double nowdate)
  
  // Given a set of 12 values, taken to be valid on the fifteenth of each month (Jan through Dec)
// and a date in the form <YYYYMMDD[HHmmss]> ....
   
 //   Return a value valid for the day given in <nowdate>, as an interpolation from the 12
 //   monthly values.
 
 int nowy,nowm,nowd;
 
 {
	 if (nowd == 15)
		{		
		return a12[nowm];
		}
 } 
 
  real, dimension(12), intent(in) :: a12 ! 12 monthly values, taken to be valid on the 15th of
  !                                      ! the month
  character(len=12), intent(in) :: nowdate ! Date, in the form <YYYYMMDD[HHmmss]>
  integer :: nowy, nowm, nowd
  integer :: prevm, postm
  real    :: factor
  integer, dimension(12) :: ndays = (/ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 /)

  !
  ! Handle leap year by setting the number of days in February for the year in question.
  !
  read(nowdate(1:8),'(I4,I2,I2)') nowy, nowm, nowd
  ndays(2) = nfeb(nowy)

  !
  ! Do interpolation between the fifteenth of two successive months.
  !
  if (nowd == 15) then
     nowval = a12(nowm)
     return
  else if (nowd < 15) then
     postm = nowm
     prevm = nowm - 1
     if (prevm == 0) prevm = 12
     factor = real(ndays(prevm)-15+nowd)/real(ndays(prevm))
  else if (nowd > 15) then
     prevm = nowm
     postm = nowm + 1
     if (postm == 13) postm = 1
     factor = real(nowd-15)/real(ndays(prevm))
  endif

  nowval = a12(prevm)*(1.0-factor) + a12(postm)*factor

end function month_d

*/
