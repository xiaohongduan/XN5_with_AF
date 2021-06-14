//      xnoah.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "xnoah.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(xnoah, xnoah, EXPERTN_MODUL_BASE_TYPE);

#define C_DEBUG(var)\
    xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i) \
    {\
        gchar *S;\
        S  = g_strdup_printf("%s[%d]",#var,i);\
        xpn_register_var_add_double(self->parent.pXSys->var_list,S,var[i],__LINE__,TRUE); \
        g_free(S);\
    }\
     

static void xnoah_class_init(xnoahClass *klass) {}


static void xnoah_init(xnoah *self)
{
    self->__ini_done=0;
    self->__done_done=0;
}




int xnoah_load_and_init(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    char *veg_filename,*soil_filename,*gen_filename;
    self->AI = NULL;
    self->BI = NULL;
    self->CI = NULL;
    self->CIin = NULL;
    self->RHSTS = NULL;
    self->RHSTSin = NULL;
    self->STCOUT = NULL;
    self->STCIN = NULL;
    self->first=1;
//  self->hcpct = 0.0;
    self->gen_param = NULL;
    self->soil_param = NULL;
    self->veg_param = NULL;

    veg_filename=NULL;
    soil_filename=NULL;
    gen_filename=NULL;

    veg_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.xnoah.veg_filename");
    if (veg_filename==NULL)
        {
            PRINT_ERROR("Missing entry 'xnoah.veg_filename' in your configuration!");
        }
    else
        {

            char *S2;
            S2 = expertn_modul_base_replace_std_templates(xpn,veg_filename);
            if (S2!=NULL)
                {
                    veg_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
                    free(S2);
                }
        }
    soil_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.xnoah.soil_filename");
    if (soil_filename==NULL)
        {
            PRINT_ERROR("Missing entry 'xnoah.soil_filename' in your configuration!");
        }
    else
        {
            char *S2;
            S2 = expertn_modul_base_replace_std_templates(xpn,soil_filename);
            if (S2!=NULL)
                {
                    soil_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
                    free(S2);
                }
        }
    gen_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.xnoah.gen_filename");
    if (gen_filename==NULL)
        {
            PRINT_ERROR("Missing entry 'xnoah.gen_filename' in your configuration!");
        }
    else
        {
            char *S2;
            S2 = expertn_modul_base_replace_std_templates(xpn,gen_filename);
            if (S2!=NULL)
                {
                    gen_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
                    free(S2);
                }
        }



    self->soiltype=0; //Sand
    self->vegtype=1; //Dryland Cropland and Pasture

    self->is_urban = 0; // kommt bei soiltype nicht vor
    self->itavg = TRUE;

    //PRINT_MESSAGE(xpn,3,"INIT VEGETATIONSPARAMS");
    if (xnoah_util_load_veg_param(&(self->veg_param),veg_filename,"USGS",xpn->DEBUG_LEVEL)!=RET_SUCCESS)
        {
            char *S;
            S=xnoah_util_getErrorString();
            PRINT_ERROR(S);
            free(S);
            return 1;
        }
    //PRINT_MESSAGE(xpn,3,"INIT SOILPARAMS");
    if (xnoah_util_load_soil_param(&(self->soil_param),soil_filename,"STAS",xpn->DEBUG_LEVEL)!=RET_SUCCESS)
        {
            char *S;
            S=xnoah_util_getErrorString();
            PRINT_ERROR(S);
            free(S);
            return 1;

        }
    //PRINT_MESSAGE(xpn,3,"INIT GENPARAMS");
    if (xnoah_util_load_gen_param(&(self->gen_param),gen_filename,xpn->DEBUG_LEVEL)!=RET_SUCCESS)
        {
            char *S;
            S=xnoah_util_getErrorString();
            PRINT_ERROR(S);
            free(S);
            return 1;
        }

    if (veg_filename!=NULL) free(veg_filename);
    if (soil_filename!=NULL) free(soil_filename);
    if (gen_filename!=NULL) free(gen_filename);
    return RET_SUCCESS;
}

int xnoah_heat_transfer_done(xnoah *self)
{
    if (self->veg_param!=NULL) xnoah_util_VEG_PARAM_delete(self->veg_param);
    if (self->soil_param!=NULL) xnoah_util_SOIL_PARAM_delete(self->soil_param);
    if (self->gen_param!=NULL) xnoah_util_GEN_PARAM_delete(self->gen_param);
    if (self->AI!=NULL) free(self->AI);
    if (self->BI!=NULL) free(self->BI);
    if (self->CI!=NULL) free(self->CI);
    if (self->CIin!=NULL) free(self->CIin);
    if (self->STCOUT!=NULL) free(self->STCOUT);
    if (self->STCIN!=NULL) free(self->STCIN);
    if (self->RHSTS!=NULL) free(self->RHSTS);
    if (self->RHSTSin!=NULL) free(self->RHSTSin);
    return RET_SUCCESS;
}

int xnoah_run_emissivity(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    double LAI;
    double e_v,e_s,sigma,e;
    double kcan = 0.5;
    LAI = xpn->pPl->pCanopy->fLAI;
    sigma = (1.0-exp(-kcan * LAI));
    e_v = self->veg_param->EMISSMINTBL[self->vegtype];
    e_s = self->veg_param->EMISSMAXTBL[self->vegtype];
    e = e_v-(e_v-e_s)*(1.0-sigma);
    self->emissi = e;

    C_DEBUG(e);
    C_DEBUG(e_v);
    C_DEBUG(e_s);
    xpn->pHe->fEmissi = e;
    return RET_SUCCESS;
}

int xnoah_run_load(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    int i;
    double depth;
    PSLAYER pSLayer;
    if (self->__ini_done==0)
        {
            self->nsoil = xpn->pSo->iLayers-2;
            self->smc = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->sh2o = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->smcdry = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->smcmax = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->bexp = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->smcwlt = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->dksat = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->refsmc = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->frzfact = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->satpsi = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->dwsat = (double*)g_malloc0(sizeof(double)*self->nsoil);

            self->ET = (double*)g_malloc0(sizeof(double)*self->nsoil);
            self->zsoil = (double*)g_malloc0(sizeof(double)*self->nsoil);

            self->NRoot = 10; // Plant Parameter
            self->rtdis = (double*)g_malloc0(sizeof(double)*self->NRoot);


            // ZSoil:
            self->ice=0;

            if (self->ice==1)
                {

                    // SEA-ICE LAYERS ARE EQUAL THICKNESS AND SUM TO 3 METERS
                    // stimmt das auch für Expert-N Layers?

                    /*self->zsoil[0] = - 3.0/2.0 * (xpn->pSo->pSLayer->pNext->fThickness * 1e-3);

                    for (i=1,pSLayer=xpn->pSo->pSLayer->pNext->pNext;i<self->NRoot;i++,pSLayer=pSLayer->pNext)
                        {
                            //self->zsoil[i] = -3.0 * ((double)(i+1))/((double)self->nsoil);
                            self->zsoil[i] = - 3.0/2.0 * (pSLayer->fThickness * 1e3) + self->zsoil[i-1];
                        }*/
                }
            else
                {
                    // CALCULATE DEPTH (NEGATIVE) BELOW GROUND FROM TOP SKIN SFC TO BOTTOM OF
                    // EACH SOIL LAYER.  NOTE:  SIGN OF ZSOIL IS NEGATIVE (DENOTING BELOW
                    // GROUND)

                    self->zsoil[0] = -(xpn->pSo->pSLayer->pNext->fThickness * 1e-3);

                    for (i=1,pSLayer=xpn->pSo->pSLayer->pNext->pNext; i<self->nsoil; i++,pSLayer=pSLayer->pNext)
                        {
                            self->zsoil[i] = - (pSLayer->fThickness * 1e-3) + self->zsoil[i-1];                         
                        }
                }


            // Distribution:

            depth = 0.0;
            // Tiefe bis Nroot berechnen
            for (i=0,pSLayer=xpn->pSo->pSLayer->pNext; i<self->NRoot; i++,pSLayer=pSLayer->pNext)
                {
                    depth += (-pSLayer->fThickness * 1e-3);
                }

            for (i=0,pSLayer=xpn->pSo->pSLayer->pNext; i<self->NRoot; i++,pSLayer=pSLayer->pNext)
                {
                    self->rtdis[i] =  -pSLayer->fThickness * 1e-3 / depth;
                }


            self->epsca=0.0;
            self->__ini_done=1;
            self->__done_done=0;
            self->ch = 1.e-4;
            self->cm = 1.e-4;
            xpn->pPl->pCanopy->fRoughnessHeat=0.125848;
            xpn->pPl->pCanopy->fRoughnessVapor=0.125848;

            self->weather_file_name=expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(xpn,"$PROJECT_PATH/noah.csv");

        /*  if (xnoah_load_weather_file(self)!=0)
                {
                    PRINT_ERROR("Problem with weather file!");
                }*/


        }

    return RET_SUCCESS;
}

int xnoah_run_done(xnoah *self)
{
    if (self->__done_done==0)
        {
            g_free(self->sh2o);
            g_free(self->smc);
            g_free(self->smcdry);
            g_free(self->smcmax);
            g_free(self->bexp);
            g_free(self->smcwlt);
            g_free(self->dksat);
            g_free(self->refsmc);
            g_free(self->rtdis);
            g_free(self->ET);
            g_free(self->zsoil);
            g_free(self->frzfact);
            g_free(self->dwsat);
            g_free(self->satpsi);
            //xnoah_free_weather_file(self);
            free(self->weather_file_name);
        }
    self->__ini_done=0;
    self->__done_done=1;
    return RET_SUCCESS;
}

// return th2
static double calc_pot_temp(double psfc,double sfcprs, double sfctmp)
{
    double capa;
    double R_D,CP;
    double apes,apelm,sfcth2;
    double th2;
    // Potentielle Temperatur Berechnen (aus noahdrive):
    R_D = 287.04;
    CP = 1004.6;
    capa =R_D/CP;
    apes=pow(1.0e5/psfc,capa);
    apelm =pow(1.0e5/sfcprs,capa);
    sfcth2 = sfctmp*apelm;
    th2 = sfcth2/apes;
    return th2;
}

// T = Temperature in Grad C
// return vapor pressure of water [hPa]
static double get_e(double T)
{
    double a0,a1,a2,a3,a4,a5,a6;
    a0 = 6.10779961;
    a1 = 4.436518521e-1;
    a2 = 1.428945805e-2;
    a3 = 2.650648471e-4;
    a4 = 3.031240396e-6;
    a5 = 2.034080948e-8;
    a6 = 6.136820929e-11;
    return a0 + T * (a1 + T*(a2 + T*(a3 + T*(a4 + T * (a5 + T * a6)))));
}

static double get_PH2O_from_rel_hum(double rel_hum, double temp_C)
{
    double e,PH2O;
    e=get_e(temp_C);
    PH2O = rel_hum * e / 100.0;
    return PH2O;
}

// in: rel_hum [%]
//     temp_C [° C]
//     pressure [hPa]
// return spec hum [kg / kg]
static double calc_spec_hum(double rel_hum,double temp_C, double pressure)
{
    double MH2O = 18.01534; // g/mol
    double Mdry = 28.9644; // g /mol
    double PH2O,xH2O;
    double q2;

    PH2O = get_PH2O_from_rel_hum(rel_hum,temp_C);
    xH2O = PH2O / pressure;
    q2 = xH2O * MH2O / (xH2O*MH2O + (1.0-xH2O)*Mdry);
    return q2;

}

// in:
//     temp_C [° C]
//     pressure [hPa]
static double calc_spec_hum_sat(double temp_C, double pressure)
{
    double MH2O = 18.01534; // g/mol
    double Mdry = 28.9644; // g /mol
    double q2sat,e,xH2O;

    e=get_e(temp_C);

    xH2O = e / pressure;
    q2sat = xH2O * MH2O / (xH2O*MH2O + (1.0-xH2O)*Mdry);

    return q2sat;

}

static double calc_dqsdt2(double q2sat, double sfctmp)
{
    double A2=17.67;
    double A3=273.15;
    double A4=29.65;
    double A23M4 = A2*(A3-A4);
    double dqsdt2;

    // ohne Schnee:

    dqsdt2 = q2sat * A23M4 / ((sfctmp-A4)*(sfctmp-A4));
    //dqsdt2= dqsdt2*(1.0-sncov) + Q2SATI*6174.0/(SFCTSNO**2)*sncovrM

    return dqsdt2;
}

static double calc_T1(double fdown, double emissi,double sfctmp,double ch,double q2, double th2, double sfcprs, double eta, double etp, double epsca, double zsoil1, double df1, double stc1)
{
    double sigma = 5.67e-8;
    double t24;
    double rch,rho,cp,rd,t2v;
    double yynum,yy,rr;
    double beta,zz1, T1;

    cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
    rd = 287.04; // R_spec for dry air [J / (kg*K)] -- Gas Konstante

    t2v = sfctmp * (1.0 + 0.61 * q2);
    t24 = sfctmp*sfctmp*sfctmp*sfctmp;

    if (t2v==0.0) return 273.15;
    if (sfcprs==0.0) return 273.15;
    if (etp==0.0) return 273.15;
    if (emissi==0.0) return 273.15;


    rho = sfcprs / (rd * t2v);

    rch = rho * cp * ch;

    rr = emissi*t24 * 6.48e-8 / (sfcprs * ch) + 1.0;

    yynum = fdown - emissi*sigma * t24;

    beta = eta*LVH2O/etp;
    
    //printf("%f\n",beta);

    yy = sfctmp + (yynum / rch + th2  - sfctmp - beta * epsca) / rr;
    

    zz1 = df1 / (-0.5 * zsoil1 * rch * rr) + 1.0;

    T1 = (yy + (zz1 - 1.0)  * stc1) / zz1;

    return T1;


}

static double calc_pressure_at_height(double old_pressure, double old_height, double new_height)
{
    double new_pressure;
    new_pressure = old_pressure * exp((old_height-new_height)/7990.0);
    return new_pressure;
}

void xnoah_calc_soil_vars(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PWLAYER pWLayer;
    PSLAYER pSLayer;
    PSWATER pSWater;
    int i;
    double soldn,lwdn,etp;
    double Z0; // rougthness length (m)
    double T1V; // Virtual skin temperature in K
    double Th2V; // Virual temperature at level zlvl
//  double act_date;
    int row;
    
//  act_date = self->parent.pTi->pSimTime->fTime;
    
/*  for (i=0; i<self->climate_values->size_of_valuelist; i++) {

            if (self->climate_values->valuelist[i][0] >= act_date) {
                break;
            }
        }
        if (i>=self->climate_values->size_of_valuelist) {
            PRINT_ERROR_ID((&(self->parent)),"No Weather data found for this date!");
        }
        
    row = i+1;*/




    if (xpn->pTi->pSimTime->bFirstRound==1)
    {
    for (i=0,pSWater=xpn->pSo->pSWater->pNext, pWLayer=xpn->pWa->pWLayer->pNext,pSLayer=xpn->pSo->pSLayer->pNext; i<self->nsoil; i++,pWLayer=pWLayer->pNext,pSLayer=pSLayer->pNext,pSWater=pSWater->pNext)
        {
            self->smc[i] = pWLayer->fContAct;
            self->sh2o[i] = pWLayer->fContAct;
            self->smcdry[i] = pSWater->fContRes;
            self->smcmax[i] = pSLayer->fPorosity;
            self->bexp[i] = pSWater->fCampB; // --> BB
            self->smcwlt[i] = pSWater->fContPWP;
            self->dksat[i] = pSWater->fCondSat * (1.0e-3/(24.0*60.0*60.0));//  mm/day --> m/s 3.38e-6;
            self->refsmc[i] = pSWater->fContFK;// *pSLayer->fThickness * 1e-3; // mm->m ;
            self->satpsi[i] = pSWater->fCampA * 1.0e-3; // mm --> m
            self->frzfact[i] = (self->smcmax[i] / self->refsmc[i]) * (0.412 / 0.468) * 0.15; // FRZK --> GENPARM = 0.15
            self->dwsat[i] =self->bexp[i] * self->dksat[i] * (self->satpsi[i] /  self->smcmax[i]) * 1.0e-2; // =BB*dksat*(satpsi/maxsmc)  0.143e-4
        }
    }
    self->sfctmp = xpn->pCl->pWeather->fTempAir_zlvl + 273.15;




    //self->shdfac=0.95999998; // Pflanzenanteil
    //self->shdfac=0.8;//self->climate_values->valuelist[0][6];
    self->shdfac=0.614142;
    self->fxexp=2.0; // const in GENPARM
    self->cfactr = 0.5; // const VEGPARM.tbl

    //self->ch = 1.29e-02; //??

    self->sbeta = -2.0; // --> GENPARM.TBL
    self->czil = 0.1;// --> GENPARM.TBL

    self->cmcmax = 0.5e-3;// VEGPARM.TBL




    Z0 = sqrt(xpn->pPl->pCanopy->fRoughnessHeat*xpn->pPl->pCanopy->fRoughnessVapor);
    T1V = self->T1 * (1.0+0.61*self->q2);
    Th2V = self->th2 * (1.0+0.61*self->q2);


    if ((T1V==0) || (Th2V==0))
        {
            //  self->ch = 1.0e-02; //??

        }
    else
        {
            //xnoah_util_sfcdif_off(xpn->pCl->fWindMeasHeight,Z0,T1V,Th2V,xpn->pCl->pWeather->fWindSpeed,self->czil,&(self->ch),&(self->cm));
        }


    self->sfcprs = xpn->pCl->pWeather->fAtmPressure;
        
    
    //calc_pressure_at_height(xpn->pCl->pWeather->fAtmPressure, xpn->pLo->pFarm->fAltitude,xpn->pLo->pFarm->fAltitude+xpn->pCl->fTempMeasHeight);


    //self->th2 = calc_pot_temp(xpn->pCl->pWeather->fAtmPressure,self->sfcprs,self->sfctmp);

    soldn = xpn->pCl->pWeather->fPAR / ( 1.0e-6*86400.0);  // Watt/m2 <-- MJ/(m2*day)
    lwdn = xpn->pCl->pWeather->fSolRad / (1.0e-6*86400.0) - soldn;  // Watt/m2 <-- MJ/(m2*day)
    self->fdown = soldn* (1.0 - xpn->pSo->fAlbedo) + lwdn;

    self->sheat = xpn->pHe->pHEBalance->fHeatSens;
//  self->eta = xpn->pHe->fHeatLatent;
    
    //SOLNET = SOLDN * (1.0-ALBEDO)
    self->solnet =soldn * (1.0 - xpn->pSo->fAlbedo);  // Watt/m2 <-- MJ/(m2*h)
    
    //self->ssoil = self->sheat + self->ETA - self->solnet; 
    
    
    //self->ssoil = self->climate_values->valuelist[row][2];
    

    self->q2 =  xnoah_util_convert_rel_hum_to_spec_hum(self->sfctmp, self->sfcprs,xpn->pCl->pWeather->fHumidity);//calc_spec_hum(xpn->pCl->pWeather->fHumidity,xpn->pCl->pWeather->fTempAir_zlvl ,self->sfcprs/100.0);
    //self->q2sat = calc_spec_hum_sat(xpn->pCl->pWeather->fTempAir_zlvl ,self->sfcprs/100.0);
    //self->dqsdt2 = calc_dqsdt2(self->q2sat,self->sfctmp);

    //self->T1 = calc_pot_temp(xpn->pCl->pWeather->fAtmPressure,self->sfcprs,xpn->pHe->pHLayer->pNext->fSoilTemp + 273.15);
    etp = xpn->pWa->pEvap->fPotR / (24.0*60.0*60.0) * LVH2O;


    self->df = xnoah_util_TDFCND ( self->smc[0],xpn->pSo->pSLayer->pNext->fSand/100.0, self->smcmax[0], self->sh2o[0]);
    self->df1 = self->df * exp (self->shdfac*self->sbeta);
    

    self->T1 = calc_T1(self->fdown, self->emissi,self->sfctmp,self->ch,self->q2,self->th2,self->sfcprs,self->ETA,self->etp, self->epsca,-(xpn->pSo->pSLayer->pNext->fThickness*1.0e-3), self->df1, xpn->pHe->pHLayer->pNext->fSoilTemp+273.15);
    //self->T1 = calc_T1(self->fdown, self->emissi,self->sfctmp,self->ch,self->q2,self->th2,self->sfcprs,self->ETA,self->etp, self->epsca,-(xpn->pSo->pSLayer->pNext->fThickness*1.0e-3), self->df1, self->climate_values->valuelist[row][11]);

    //self->T1 = self->climate_values->valuelist[row][1];
    
    /*C_DEBUG(self->df1);
    C_DEBUG(self->T1);
    C_DEBUG(xpn->pHe->pHLayer->pNext->fSoilTemp+273.15);
    C_DEBUG(0.5*xpn->pSo->pSLayer->pNext->fThickness*1.0e-3);*/
    
    self->ssoil = self->df1 * (self->T1 - (xpn->pHe->pHLayer->pNext->fSoilTemp+273.15)) / (0.5*xpn->pSo->pSLayer->pNext->fThickness*1.0e-3);

    xnoah_util_CALTMP(self->T1,self->sfctmp,self->sfcprs,xpn->pCl->fTempMeasHeight,self->q2,&(self->th2),&(self->t1v),&(self->th2v) );  
    xnoah_util_CALHUM(self->sfctmp,self->sfcprs,&(self->q2sat), &(self->dqsdt2));
    
    //self->pc = 0.0;// self->climate_values->valuelist[row][4];
    //self->cmc = self->climate_values->valuelist[row][3];
    //self->prcp1 = self->climate_values->valuelist[row][5];
    self->pc=(sin(xpn->pTi->pSimTime->fTimeDay*2.0*PI-0.7)/2+0.9)*0.8-0.2;
    
}

int xnoah_run_transpiration(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    double etp; // [kg / m2 s]
    double etp1;// [m / s]
    double edir;
    double Z0,T1V,Th2V;


    /*double ETT; // Transpiration (W M-2)
    double SNCOVR;
    double ETT1;
    SNCOVR = 0.0; // Kein Schnee



    ETT1 = ETT1*(1.-SNCOVR);
    ETT = ETT1*1000.0;
    //  CONVERT EVAP TERMS FROM KINEMATIC (KG M-2 S-1) TO ENERGY UNITS (W M-2)
    ETT = ETT * LVH2O;

    C_DEBUG (ETT);*/

    xnoah_calc_soil_vars(self);

    // etp --> penman mont
    etp = xpn->pWa->pEvap->fPotR / (24.0*60.0*60.0); // 1/day --> 1/s
    etp1 = etp* 0.001; // mm --> m, dh. etp1 = [m / m2*s]

    edir = xnoah_util_DEVAP(etp1,self->smc[0], self->shdfac, self->smcdry[0],self->smcmax[0],self->fxexp);
    edir = edir * 1.0e3; // [m / m2*s] --> [mm / m2*s] = [kg/m2*s]


    C_DEBUG(edir*LVH2O);
    C_DEBUG(etp*LVH2O)

    xpn->pPl->pPltWater->fActTranspR=edir;

    Z0 = sqrt(xpn->pPl->pCanopy->fRoughnessHeat*xpn->pPl->pCanopy->fRoughnessVapor);
    T1V = self->T1 * (1.0+0.61*self->q2);
    Th2V = self->th2 * (1.0+0.61*self->q2);

    C_DEBUG(xpn->pCl->fWindMeasHeight);
    C_DEBUG(Z0);
    C_DEBUG(T1V);
    C_DEBUG(Th2V);
    C_DEBUG(xpn->pCl->pWeather->fWindSpeed);
    C_DEBUG(self->czil);
    C_DEBUG(self->ch);
    C_DEBUG(self->cm);
    C_DEBUG(self->ssoil);

    //xnoah_util_sfcdif_off(xpn->pCl->fWindMeasHeight,Z0,T1V,Th2V,xpn->pCl->pWeather->fWindSpeed,self->czil,&(self->ch),&(self->cm));

    


    C_DEBUG(self->ch);
    C_DEBUG(self->cm);

    return RET_SUCCESS;
}

int xnoah_run_senible_heat(xnoah *self)
{
    double sheat,cp,r,t2v,Z0;
    expertn_modul_base *xpn = &(self->parent);

    cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
    r = 287.04;

    xnoah_calc_soil_vars(self);

    t2v = self->sfctmp * (1.0 + 0.61 * self->q2);

    if (t2v==0.0)
        {
            xpn->pHe->pHEBalance->fHeatSens=0.0;
            return RET_SUCCESS;
        }

    //DETERMINE SENSIBLE HEAT (H) IN ENERGY UNITS (W M-2)
    
    Z0 = sqrt(xpn->pPl->pCanopy->fRoughnessHeat*xpn->pPl->pCanopy->fRoughnessVapor);
    
    xnoah_util_sfcdif_off(xpn->pCl->fWindMeasHeight,Z0,self->t1v,self->th2v,0.0,self->czil,&(self->cm),&(self->ch));
    

    sheat = - (self->ch * cp * self->sfcprs)/ (r * t2v) * (self->th2- self->T1 );

    xpn->pHe->pHEBalance->fHeatSens = sheat;
    
    C_DEBUG(self->fdown)
    C_DEBUG(self->emissi);
    C_DEBUG(self->sfctmp);
    C_DEBUG(self->ch);
    C_DEBUG(self->q2);
    C_DEBUG(self->th2);
    C_DEBUG(self->sfcprs);
    C_DEBUG(self->ETA);
    C_DEBUG(self->etp);
    C_DEBUG(self->epsca);
    C_DEBUG(self->df1);
    C_DEBUG(xpn->pHe->pHLayer->pNext->fSoilTemp+273.15);

    C_DEBUG(self->th2);
    C_DEBUG(self->T1);

    C_DEBUG(self->sfcprs);

    C_DEBUG(t2v);

    C_DEBUG(xpn->pHe->pHEBalance->fHeatSens);

    return RET_SUCCESS;
}

int xnoah_run_potential_evaportranspiration(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    double etp,etp_noah,test,etp1,dt;
    double dew;

    //double cmc; // canopy water content [m]

    double edir,Z0,T1V,Th2V;
    int i,i2;
    double flx2,rch,rr,t24;
    double PRCP,PRCP1;
    double SLOPE,KDT;
    double runoff1,runoff2,runoff3;
    double drip; //THROUGH-FALL OF PRECIP AND/OR DEW IN EXCESS OF CANOPY WATER-HOLDING CAPACITY (M)
    double act_date;
    double ET_NAOH[4],SMC_NOAH[4],sum;
    
    PWLAYER pWLayer;
    PSLAYER pSLayer;
    PSWATER pSWater;

    

    xnoah_calc_soil_vars(self);
    
    
    
    //C_DEBUG(self->climate_values->valuelist[row][0]);
        
    
    
    
    

    C_DEBUG(self->sfctmp);
    C_DEBUG(self->sfcprs);
    C_DEBUG(xpn->pCl->pWeather->fAtmPressure);
    C_DEBUG(self->fdown);
    C_DEBUG(self->T1);
    C_DEBUG(self->th2);
    C_DEBUG(self->solnet);
    C_DEBUG(self->sheat);
    //C_DEBUG(self->eta);
    C_DEBUG(self->ssoil);
    C_DEBUG(self->q2);
    C_DEBUG(self->q2sat);
    C_DEBUG(self->dqsdt2);
    
    C_DEBUG(self->T1);
    C_DEBUG(self->T1_2);

    C_DEBUG(xpn->pHe->pHLayer->fSoilTemp + 273.15);
    C_DEBUG(xpn->pHe->pHLayer->pNext->fSoilTemp + 273.15);
    C_DEBUG(xpn->pCl->pWeather->fTempAir + 273.15);
    test = calc_pot_temp(xpn->pCl->pWeather->fAtmPressure,self->sfcprs,xpn->pCl->pWeather->fTempAir + 273.15);
    C_DEBUG(test);
    C_DEBUG(self->epsca);
    C_DEBUG(self->ch);

    self->emissi = 0.9693301;
    SLOPE = 0.1;
    drip = 0.0;
    //KDT = 2.0e-6; // --> GENPARM.TBL --> ist falsch
    KDT = 5.0700002;//3.0* self->dksat[0] / 2.0E-6;

    Z0 = sqrt(xpn->pPl->pCanopy->fRoughnessHeat*xpn->pPl->pCanopy->fRoughnessVapor);
    T1V = self->t1v; //self->T1 * (1.0+0.61*self->q2);
    Th2V = self->th2v;//self->th2 * (1.0+0.61*self->q2);

    //xnoah_util_sfcdif_off(xpn->pCl->fWindMeasHeight,Z0,T1V,Th2V,xpn->pCl->pWeather->fWindSpeed,self->czil,&(self->cm),&(self->ch));
    xnoah_util_sfcdif_off(xpn->pCl->fWindMeasHeight,Z0,T1V,Th2V,0.0,self->czil,&(self->cm),&(self->ch));

    //self->ch = MIN(self->ch,0.016);

    C_DEBUG(self->q2);
    C_DEBUG(self->th2);
    C_DEBUG(xpn->pCl->fWindMeasHeight);
    C_DEBUG(Z0);
    C_DEBUG(T1V);
    C_DEBUG(Th2V);
    C_DEBUG(xpn->pCl->pWeather->fWindSpeed);
    C_DEBUG(self->czil);
    C_DEBUG(self->cm);
    C_DEBUG(self->ch);


    PRCP = xpn->pCl->pWeather->fPreciRate / (60.0*60.0*24.0);  // kg / (m2 * day) = mm/(m2 day) --> kg / (m2*s)

    etp = xnoah_util_PENMAN(&(self->epsca),self->sfctmp,self->sfcprs,self->ch,self->th2, PRCP, self->fdown, self->ssoil,self->q2, self->q2sat, FALSE, FALSE,self->dqsdt2, self->emissi, self->T1,0, 0.0,&flx2,&rch,&rr,&t24);

    C_DEBUG(self->epsca);
    C_DEBUG(self->sfctmp);
    C_DEBUG(xpn->pCl->pWeather->fAtmPressure);
    C_DEBUG(self->ch);
    C_DEBUG(self->th2);
    C_DEBUG(PRCP);
    C_DEBUG(self->fdown);
    C_DEBUG(self->ssoil);
    C_DEBUG(self->q2);
    C_DEBUG(self->q2sat);
    C_DEBUG(self->dqsdt2);
    C_DEBUG(self->emissi);
    C_DEBUG(self->T1);
    C_DEBUG(flx2);
    C_DEBUG(rch);
    C_DEBUG(rr);
    C_DEBUG(t24);

    if (etp*LVH2O>500.0)
        {
//      printf("hallo");
        }

    etp1 = etp * 0.001;
    PRCP1 = PRCP * 0.001;



    //xpn->pWa->pEvap->fPotR = etp * (24.0*60.0*60.0); // 1/day <-- 1/s



    self->ETA = 0.0;
    self->EDIR = 0.0;
    self->EC = 0.0;


    for (i=0; i<self->nsoil; i++)
        {
            self->ET[i]=0.0;
        }

    self->ETT = 0.0;

    dt = xpn->pTi->pTimeStep->fAct * (24.0*60.0*60.0); // 1/day <-- 1/s
    
    C_DEBUG(self->ETA);
    C_DEBUG(self->smc[0]);
    C_DEBUG(self->nsoil);
    C_DEBUG(self->cmc);
    C_DEBUG(etp1);
    C_DEBUG(dt);
    C_DEBUG(self->sh2o[0]);
    C_DEBUG(self->smcmax[0])
    C_DEBUG(self->pc);
    C_DEBUG(self->smcwlt[0]);
    C_DEBUG(self->refsmc[0]);
    C_DEBUG(self->shdfac);
    C_DEBUG(self->cmcmax);  
    C_DEBUG(self->smcdry[0]);
    C_DEBUG(self->cfactr);
    
    //PRCP1 = self->prcp1;


    if (etp>0.0)
        {

            


            /*etp_noah = etp*LVH2O;

            C_DEBUG (etp_noah);*/


            /*  if (xpn->pWa->pEvap->fPotR==0.0)
                    {
                        pc = 0.0;
                    }
                else
                    {
                        pc = xpn->pWa->pEvap->fActR / xpn->pWa->pEvap->fPotR;  // pc*etp=actuel transp??? --> pc = actual transpir / potential evaporation
                    }*/
            //pc = 0.34;

            // smcwlt: wilting point (volumtric)
            self->ETA  = xnoah_util_evapo(self->smc,self->nsoil,self->cmc,etp1,dt,
                                          self->sh2o,
                                          self->smcmax,self->pc,self->smcwlt,
                                          self->refsmc,self->shdfac,self->cmcmax,
                                          self->smcdry,self->cfactr,
                                          &(self->EDIR),&(self->EC),&(self->ET),&(self->ETT),self->sfctmp,self->q2,self->NRoot,self->rtdis,self->fxexp);

            //PRCP1 = 0.0;
            //self->EC += 1.0e-8;
            //self->ET[0] = self->climate_values->valuelist[row][5];
            xnoah_util_SMFLX (&(self->smc),self->nsoil,&(self->cmc),dt,PRCP1,self->zsoil,
                              &(self->sh2o),SLOPE,KDT,self->frzfact,
                              self->smcmax,self->bexp,self->smcwlt,self->dksat,self->dwsat,
                              self->shdfac,self->cmcmax,
                              &runoff1,&runoff2,&runoff3,
                              self->EDIR,self->EC,self->ET,
                              &drip);
            /*  void xnoah_util_SMFLX( double **SMC,int NSOIL,double *CMC,double DT,double PRCP1,double *ZSOIL,
                            double **SH2O,double SLOPE,double KDT,double FRZFACT,
                            double *SMCMAX,double BEXP,double *SMCWLT,double *DKSAT,double *DWSAT,
                            double SHDFAC,double CMCMAX,
                            double *RUNOFF1,double *RUNOFF2,double *RUNOFF3,
                            double EDIR,double EC,double *ET,
                            double *DRIP);
            */
        }
    else
        {
            dew = -etp1;
            PRCP1 = PRCP1+ dew;
            xnoah_util_SMFLX (&(self->smc),self->nsoil,&(self->cmc),dt,PRCP1,self->zsoil,
                              &(self->sh2o),SLOPE,KDT,self->frzfact,
                              self->smcmax,self->bexp,self->smcwlt,self->dksat,self->dwsat,
                              self->shdfac,self->cmcmax,
                              &runoff1,&runoff2,&runoff3,
                              self->EDIR,self->EC,self->ET,
                              &drip);

        }
        

    i2 = 0;
    sum = 0.0;
    for (i=0; i<self->nsoil; i++)
        {
            sum += self->ET[i];
            ET_NAOH[i2] = sum;
            if (i==0) 
                { i2=1; sum = 0.0;
                }
            if (i==3) {i2=2; sum=0.0;}
            if (i==9) {i2=3; sum = 0.0;}
        }
    
    
        
    for (i=0; i<4; i++)
        {
            C_DEBUG_LOC_IN_ARRAY(ET_NAOH,i);
        }
        
    C_DEBUG(self->EDIR);
    C_DEBUG(self->EC);
    C_DEBUG(self->ETA);
    C_DEBUG(self->cmc);
    C_DEBUG(drip);
    //self->sfctmp,self->q2,self->NRoot,self->rtdis,self->fxexp

    // EVAPOTRANSPIRATION FROM  M S-1  TO  KG M-2 S-1.
    self->ETA = self->ETA * 1000.0;
    self->EDIR = self->EDIR * 1000.0;
    self->EC = self->EC * 1000.0;
    for (i=0; i<self->nsoil; i++)
        {
            self->ET[i]=self->ET[i]*1000.0;
        }

    self->ETT = self->ETT*1000.0;

    if (etp>0.0)
        {
            self->ETA = (self->EDIR + self->EC + self->ETT);
        }
    else
        {
            self->ETA = etp;
        }
        
    C_DEBUG(self->smc[0]);
    C_DEBUG(self->smc[3]);
    C_DEBUG(self->smc[7]);
    C_DEBUG(self->smc[16]);
    
    
    C_DEBUG(self->sh2o[0]);
    C_DEBUG(self->sh2o[1]);
    C_DEBUG(self->sh2o[4]);
    C_DEBUG(self->sh2o[9]);
    C_DEBUG(self->sh2o[19]);


    for (i=0,pSWater=xpn->pSo->pSWater->pNext, pWLayer=xpn->pWa->pWLayer->pNext,pSLayer=xpn->pSo->pSLayer->pNext; i<self->nsoil; i++,pWLayer=pWLayer->pNext,pSLayer=pSLayer->pNext,pSWater=pSWater->pNext)
        {
            pWLayer->fContAct = self->smc[i];//*pSLayer->fThickness * 1e-3; // mm->m

        }

//  xpn->pHe->fHeatLatent = self->eta;
    //xpn->pWa->pEvap->fPotR*(24.0*60.0*60.0)/LVH2O;
    xpn->pWa->pEvap->fPotR_old = xpn->pWa->pEvap->fPotR;
    xpn->pWa->pEvap->fPotR = etp * 1.0e3 *(24.0*60.0*60.0); // 1/day <-- 1/s 
    //xpn->pWa->pEvap->fActR
//  xpn->pPl->pPltWater->fPotTransp



    C_DEBUG(etp*LVH2O);

    C_DEBUG(self->ETA*LVH2O);
    C_DEBUG(self->EDIR*LVH2O);
    C_DEBUG(self->EC*LVH2O);
    C_DEBUG(self->ETT*LVH2O);
    
    for (i=0; i<self->nsoil; i++)
        {
            C_DEBUG_LOC_IN_ARRAY(LVH2O*self->ET,i);
        }

    //C_DEBUG(self->ETT*LVH2O);
    
    self->etp=etp*LVH2O;
    /*self->ETA = self->ETA*LVH2O;
    self->EDIR = self->EDIR*LVH2O;
    self->EC = self->EC*LVH2O;*/
    
    


    // --> actutal evporation, transpiration:
    
    
    
    xpn->pPl->pPltWater->fInterceptR = self->EC * (24.0*60.0*60.0); // m --> mm, 1/s --> 1/day
    xpn->pPl->pPltWater->fActTranspR = self->ETT * (24.0*60.0*60.0);// m --> mm
    xpn->pWa->pEvap->fActR = self->EDIR  * (24.0*60.0*60.0); // m --> mm; // DIRECT SOIL EVAPORATION
    

    xpn->pWa->fActETR = xpn->pPl->pPltWater->fActTranspR + xpn->pWa->pEvap->fActR + xpn->pPl->pPltWater->fInterceptR;   
    xpn->pWa->fPotETR = etp * (24.0*60.0*60.0);// m --> mm


    return RET_SUCCESS;
}






// out:
// eta1

// in:
// SMC[nsoil]: total soil moisture moisture content (volumentric fraction)
// cmc: canopy moisture content [m]
// etp1: etp = potential evaporation (W / m2)
// dt:  time step [s]
// zsoil[nsoil]: tiefe (neg)
// SH2O: unfrozen soil moisture content (volumetric fraction)
// smcmax: porosity , ie. saturated value of soil moisture
// bexp: ????
// pc plant coefficient (unitless fraction, 0-1) where pc*etp=actuel transp --> pc = actual transpir / potential evaporation
// smcwlt: wilting point (volumtric)
// dksat: sat soil conductivity --> soiltype table [m /s] = Ksat
// dwsat: SAT soil diffusivity --> aus tabelle = BB*dksat*(satpsi/maxsmc), mit satpsi = saturation soil suction
// smcref: soil moisture thresshold bolow which transpiration beginss to stress (volumetric)
// shdfac: green vegetation fraction --> soiltable
// cmcmax: maximum of canopy water capacity --> cmcmaxdata --> const in vegparam table
// smcdry: dry soil moisture thresshold where direct evap from top layer end (volumetric)--> soil table
// nsoil : number soillayers
/*void xnoah_EVAPO (double *eta1, double cmc,double etp,double bexp ,double pc, double smcwlt,double dksat,double dwsat, double smcref,double shdfac, double cmcmax,double smcdry,double *zsoil,double *smc, int nsoil,*/



int xnoah_load_weather_file(xnoah *self)
{
    expertn_modul_base *xpn = &(self->parent);
    int date_format;
    int year,month,day,hour,minute;
    char *seperator;
    char *S,*S2,*S3;
    char *date_str;
    FILE *f;
    int i,i2;
    double *values;
    int min_columns;
    GDate date;
    STRUCT_XNO_DATA* data;
    int size;

    self->climate_values=NULL;

    f = fopen(self->weather_file_name ,"r");
    i = 0;  
    if (f==NULL)
        {
            S = g_strdup_printf("Open Weather File %s!",self->weather_file_name);
            PRINT_ERROR_ID(xpn,S);
            g_free(S);
            return 1;
        }
    seperator = ",";
    min_columns = 2;
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
                                                    S_ERROR = g_strdup_printf("The weather file %s has the wrong number of columns %d (colon,semicolon,tabulator and space is alowed as seperator)!",self->weather_file_name,min_columns);
                                                    PRINT_ERROR_ID(xpn,S_ERROR);
                                                    g_free(S_ERROR);
                                                    return 1;
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
                            S_ERROR = g_strdup_printf("The weather file %s has the wrong number of rows %d in line %d",self->weather_file_name,min_columns,i+1);
                            PRINT_ERROR_ID(xpn,S_ERROR);
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
                                            if (getStringColumn_get_Size(S,"-") >= 3)
                                                {
                                                    date_format=csv_minus;
                                                }
                                            else if (getStringColumn_get_Size(S,"//") >= 3)
                                                {
                                                    date_format=csv_slash;
                                                }
                                            else if (getStringColumn_get_Size(S,".") >= 3)
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
                                        case csv_minus: // yyyy-mm--dd
                                            getStringColumn(date_str,S2,"-",0);
                                            year=atoi(date_str);
                                            getStringColumn(date_str,S2,"-",1);
                                            month=atoi(date_str);
                                            getStringColumn(date_str,S2,"-",2);
                                            day=atoi(date_str);
                                            g_date_set_day(&date,day);
                                            g_date_set_month(&date,month);
                                            g_date_set_year(&date,year);
                                            values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
                                            break;
                                        case csv_slash: // mm-dd-yyyy
                                            getStringColumn(date_str,S2,"//",0);
                                            month=atoi(date_str);
                                            getStringColumn(date_str,S2,"//",1);
                                            day=atoi(date_str);
                                            getStringColumn(date_str,S2,"//",2);
                                            year=atoi(date_str);
                                            g_date_set_day(&date,day);
                                            g_date_set_month(&date,month);
                                            g_date_set_year(&date,year);
                                            values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
                                            break;
                                        case csv_german:// dd.mm.yyyy.HH.MM
                                            getStringColumn(date_str,S2,".",0);
                                            day=atoi(date_str);
                                            getStringColumn(date_str,S2,".",1);
                                            month=atoi(date_str);
                                            getStringColumn(date_str,S2,".",2);
                                            year=atoi(date_str);
                                            if (year < 100)
                                                {
                                                    year+=1900;
                                                }
                                            getStringColumn(date_str,S2,".",3);
                                            hour=atoi(date_str)-2;
                                            getStringColumn(date_str,S2,".",4);
                                            minute=atoi(date_str);
                                            g_date_set_day(&date,day);
                                            g_date_set_month(&date,month);
                                            g_date_set_year(&date,year);
                                            values[i2] = (double)convert_date_to_days(conv_gdate_to_xpn_date(&date))*(60.0*60.0*24.0);
                                            values[i2] += (double)hour*(60.0*60.0) + (double)minute*(60.0);
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
                    data->valuelist[data->size_of_valuelist-1] = values;
                }



            g_free(S2);
            g_free(date_str);
            free(S);
            i++;
        }
    fclose(f);

    if (i < 2) return -1;

    self->climate_values=data;

    return RET_SUCCESS;
}

int xnoah_free_weather_file(xnoah *self)
{
    int i,i2;
    STRUCT_XNO_DATA* data;
    data = self->climate_values;
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
                }
            g_free(data);
        }
    self->climate_values = NULL;
    return RET_SUCCESS;
}
