//      wrf_coupling.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "wrf_coupling.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <expertn_noah_driver_reg.h>
#include <xtime_reg.h>
#include "wrf_coupling_wrf_var_def.h"

// T = Temperature in Grad C
// return vapor pressure of water [hPa]
static double get_e(double T);

// q = spez. humidity [kg/kg]
// P = Pressure [hPa]
// return P_H20 [hPa]
static double get_Ph20(double q,double P);

// q = spez. humidity [kg/kg]
// T = Temperature [Grad C]
// P = Pressure [hPa]
// return RH [%]
static double calc_rel_hum(double q,double T,double P);

static struct_cdb_data* load_cdb(char *cdb_filename);
static void free_cdb(struct_cdb_data *self);


G_DEFINE_TYPE(wrf_coupling, wrf_coupling, EXPERTN_MODUL_BASE_TYPE);


static void wrf_coupling_class_init(wrf_couplingClass *klass) {}


static void wrf_coupling_init(wrf_coupling *self)
{
    self->WRF=NULL;
    self->__ERROR=RET_SUCCESS;
}





// Our Modul Functions:


int wrf_coupling_load(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PXSYSTEM pXSys = xpn->pXSys;
    PTIME pTi = self->parent.pTi;
    xpn_register_var *var_list = self->parent.pXSys->var_list;
    char *S;
    int i;


    self->WRF=NULL;
    self->own=NULL;
    self->own_link=NULL;
    self->own_link_count=0;
    self->class_link=NULL;
    self->class_link_count=0;
    self->cdb_input_file=NULL;
    self->cdb_data=NULL;
    self->free_wrf_struct=0;
    self->cdblink=NULL;
    self->cdblink_len=0;
    self->cdb_sim_counter=0;


    // checken, ob ExpertN an Noah hängt, je nachdem werden später Wetterdaten geladen, falls ExpertN als standalone arbeitet
    self->WRF=(struct_WRF*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_Noah");


    S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.WRF Coupling.use input file");
    if (S==NULL) {
        S = "0";
    }
    self->use_cdb_input_file = atoi(S);

    if (self->use_cdb_input_file==1) {
        char *ini_filename;
        ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.WRF Coupling.cdb filename");

        if (ini_filename==NULL) {
            PRINT_ERROR("Missing entry 'WRF Coupling.cdb filename' in your configuration!");
        } else {

            char *S2;
            S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
            if (S2!=NULL) {
                self->cdb_input_file = g_strdup_printf("%s",S2);
                free(ini_filename);
                free(S2);
            }

        }

        // Falls Dateiename vorhanden: Datei laden:
        if ((self->cdb_input_file==NULL) || ((self->cdb_data = load_cdb(self->cdb_input_file))==NULL)) {
            self->use_cdb_input_file=0;
        }
    }



    if ((self->WRF==NULL) && (self->use_cdb_input_file>0)) {
        self->free_wrf_struct=1;
        self->WRF = (struct_WRF*)g_malloc0(sizeof(struct_WRF));

        self->WRF->nsoil=4;

        self->WRF->sldpth = (float*) g_malloc0(sizeof(float)*((int)(self->WRF->nsoil)));        
        self->WRF->sldpth[0] = 0.1;
        self->WRF->sldpth[1] = 0.30;
        self->WRF->sldpth[2] = 0.60;
        self->WRF->sldpth[3] = 1.00;
        self->WRF->et = (float*) g_malloc0(sizeof(float)*((int)(self->WRF->nsoil)));
        self->WRF->stc = (float*) g_malloc0(sizeof(float)*((int)(self->WRF->nsoil)));
        self->WRF->smc = (float*) g_malloc0(sizeof(float)*((int)(self->WRF->nsoil)));
        self->WRF->sh2o = (float*) g_malloc0(sizeof(float)*((int)(self->WRF->nsoil)));
    }


    if (self->WRF!=NULL) {
        struct_WRF *wrf;
        xpn_register_var *var_list;
        wrf = self->WRF;
        S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.WRF Coupling.coupling");
        if (S==NULL) {
            S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.coupling");
            if (S==NULL) {
                S = "0";
                fprintf(stderr,"ERROR: Varname is not defined (%s or %s), take default value (%s)!\n","Config.Expert N Standard Database.coupling","Global_Config.options.coupling",S);
            }
        }
        wrf->coupling = atof(S);

        var_list = xpn->pXSys->var_list;

        if (self->use_cdb_input_file!=2) {

            pXSys->extern_climate_data_source = TRUE;
            pXSys->extern_time_management = TRUE;

            // Time Vars:
            xpn->pTi->pSimTime= (PSIMTIME)g_malloc0_n(1,sizeof(SIMTIME));
            xpn->pTi->pTimeStep = (PTIMESTEP)g_malloc0_n(1,sizeof(TIMESTEP));


            // pLo:
            xpn->pLo->pFarm = (PFARM)g_malloc0_n(1,sizeof(FARM));
            xpn->pLo->pFieldplot = (PFIELDPLOT)g_malloc0_n(1,sizeof(FIELDPLOT));
            xpn->pLo->pTile = (PTILE)g_malloc0_n(1,sizeof(TILE));


            

        }
        //XTIME_REG

         STRUCT_DRIVE_XPN_REG(wrf,"WRF");

        if (self->use_cdb_input_file==1) {
            wrf_coupling_link_wrf_to_input_file(self);
        }
        if (self->use_cdb_input_file!=2) {
            wrf_coupling_init_time_management(self);
        }

        self->own = (struct_WRF*)g_malloc0(sizeof(struct_WRF));
        self->own->nsoil = wrf->nsoil;

        self->own->sldpth = (float*) g_malloc0(sizeof(float)*((int)(self->own->nsoil)));
        if (self->own->nsoil >= 4)
            {
                self->own->sldpth[0] = 0.1;
                self->own->sldpth[1] = 0.30;
                self->own->sldpth[2] = 0.60;
                self->own->sldpth[3] = 1.00;
            }
        
        self->own->et = (float*) g_malloc0(sizeof(float)*((int)(self->own->nsoil)));
        self->own->stc = (float*) g_malloc0(sizeof(float)*((int)(self->own->nsoil)));
        self->own->smc = (float*) g_malloc0(sizeof(float)*((int)(self->own->nsoil)));
        self->own->sh2o = (float*) g_malloc0(sizeof(float)*((int)(self->own->nsoil)));

        STRUCT_DRIVE_XPN_REG(self->own,"own");

        xpn_register_var_add_none(self->parent.pXSys->var_list,"WRF Coupling.own",self->own,-1,TRUE);


        self->class_link_count =pXSys->id_M;
        self->class_link = (expertnclass**)g_malloc0(sizeof(expertnclass*)*self->class_link_count);
        for (i=0; i<self->class_link_count; i++) {
            self->class_link[i] = pXSys->xpn_classes[pXSys->id_Mosaic[i]];
        }





    }


    return RET_SUCCESS;
}

int wrf_coupling_mosaic_run(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PXSYSTEM pXSys = xpn->pXSys;
    struct_WRF* wrf = self->WRF;
    int o,i;
    expertnclass *xpnclass;


    if (wrf==NULL) return RET_SUCCESS;

    self->__ERROR = RET_SUCCESS;

    if (wrf->coupling > -1) {

        if ((pXSys->id_M>0) && (pXSys->id==pXSys->id_Mosaic[0])) { // Nur Master macht das!!!!
            if (xpn->pTi->pSimTime->bFirstRound==TRUE) {
                self->own_link_count=pXSys->id_M;
                self->own_link = (struct_WRF **)g_malloc0(sizeof(struct_WRF *)*self->own_link_count);
                for (i=0; i<self->own_link_count; i++) {
                    xpnclass = pXSys->xpn_classes[pXSys->id_Mosaic[i]];
                    self->own_link[i] = xpn_register_var_get_pointer(xpnclass->pXSys->var_list,"WRF Coupling.own");
                }

            }

            // History State:
            wrf->cmc = 0.0;
            wrf->T1 = 0.0;
            for (i=0; i<wrf->nsoil; i++) {
                wrf->stc[i] = 0.0;
                wrf->smc[i] = 0.0;
                wrf->sh2o[i] = 0.0;
            }
            wrf->albedo=0.0;
            wrf->ch = 0.0;
            wrf->cm = 0.0;

            // Output:
            wrf->eta=0.0;
            wrf->eta_kinematic=0.0;
            wrf->sheat=0.0;
            wrf->fdown=0.0;
            wrf->ec = 0.0;
            wrf->edir = 0.0;

            for (i=0; i<wrf->nsoil; i++) {
                wrf->et[i] = 0.0;
            }
            wrf->ett = 0.0;

            wrf->esnow = 0.0;
            wrf->drip = 0.0;

            wrf->dew = 0.0;

            wrf->beta = 0.0;
            wrf->etp = 0.0;
            wrf->ssoil = 0.0;

            wrf->flx1 = 0.0;
            wrf->flx2 = 0.0;
            wrf->flx3 = 0.0;

            wrf->snomlt = 0.0;
            wrf->sncovr = 0.0;


            wrf->runoff1 = 0.0;
            wrf->runoff2 = 0.0;
            wrf->runoff3 = 0.0;


            wrf->rc = 0.0;
            wrf->pc = 0.0;
            wrf->xlai = 0.0;
            wrf->rsmin = 0.0;
            wrf->rcs = 0.0;
            wrf->rct = 0.0;
            wrf->rcq = 0.0;
            wrf->rcsoil = 0.0;

            for (o=0; o<self->own_link_count; o++) {
                struct_WRF *src;
                double frac;
                src = self->own_link[o];
                frac = self->class_link[o]->pXSys->landuse_cover_fraction;



                // History State:
                wrf->cmc += src->cmc*frac; // moister content (m)
                wrf->T1 += src->T1*frac;
                for (i=0; i<wrf->nsoil; i++) {
                    wrf->stc[i] += src->stc[i]*frac; //SoilTemp K
                    wrf->smc[i] += src->smc[i]*frac; // Soil moister content (volumetric)
                    wrf->sh2o[i] += src->sh2o[i]*frac; // unfrozen soil moisture content
                }
                wrf->albedo+= src->albedo*frac;
                wrf->ch += src->ch*frac;
                wrf->cm += src->cm*frac;


                // Output:
                wrf->eta+=src->eta*frac;
                wrf->eta_kinematic+=src->eta_kinematic*frac;
                wrf->sheat+=src->sheat*frac;
                wrf->fdown+=src->fdown*frac;
                wrf->ec+=src->ec*frac;
                wrf->edir+=src->edir*frac;

                for (i=0; i<wrf->nsoil; i++) {
                    wrf->et[i] = src->et[i]*frac;
                }
                wrf->ett += src->ett*frac;

                wrf->esnow += src->esnow*frac;
                wrf->drip += src->drip*frac;

                wrf->dew += src->dew*frac;

                wrf->beta += src->beta*frac;
                wrf->etp += src->etp*frac;
                wrf->ssoil += src->ssoil;

                wrf->flx1 += src->flx1*frac;
                wrf->flx2 += src->flx2*frac;
                wrf->flx3 += src->flx3*frac;

                wrf->snomlt += src->snomlt * frac;
                wrf->sncovr +=src->sncovr *frac ;


                wrf->runoff1 += src->runoff1*frac;
                wrf->runoff2 += src->runoff2*frac;
                wrf->runoff3 += src->runoff3*frac;


                wrf->rc += src->rc*frac;
                wrf->pc += src->pc*frac;
                wrf->xlai += src->xlai*frac;
                wrf->rsmin += src->rsmin*frac;
                wrf->rcs += src->rcs*frac;
                wrf->rct += src->rct*frac;
                wrf->rcq += src->rcq*frac;
                wrf->rcsoil += src->rcsoil;

            }
        }


    }

    if ((xpn->DEBUG_LEVEL>3) && (xpn->pTi->pSimTime->bFirstRound==FALSE)) {
        wrf_coupling_check_for_infinity(self);
    }

    return self->__ERROR;
}

void wrf_coupling_check_for_infinity(wrf_coupling *self)
{
    int i;
    struct_WRF* wrf = self->WRF;
    CHECK_VALID(wrf->T1);
    for (i=0; i<wrf->nsoil; i++) {
        CHECK_VALID(wrf->stc[i]);
        CHECK_VALID(wrf->smc[i]);
        CHECK_VALID(wrf->sh2o[i]);
    }
    CHECK_VALID(wrf->albedo);
    CHECK_VALID(wrf->ch);
    CHECK_VALID(wrf->cm);
    CHECK_VALID(wrf->eta);
    CHECK_VALID(wrf->eta_kinematic);
    CHECK_VALID(wrf->sheat);
    CHECK_VALID(wrf->fdown);
    CHECK_VALID(wrf->ec);
    CHECK_VALID(wrf->edir);
    for (i=0; i<wrf->nsoil; i++) {
        CHECK_VALID(wrf->et[i]);
    }
    CHECK_VALID(wrf->ett);
    CHECK_VALID(wrf->esnow);
    CHECK_VALID(wrf->dew);
    CHECK_VALID(wrf->beta);
    CHECK_VALID(wrf->etp);
    CHECK_VALID(wrf->ssoil);
    CHECK_VALID(wrf->flx1);
    CHECK_VALID(wrf->flx2);
    CHECK_VALID(wrf->flx3);
    CHECK_VALID(wrf->snomlt);
    CHECK_VALID(wrf->sncovr);
    CHECK_VALID(wrf->runoff1);
    CHECK_VALID(wrf->runoff2);
    CHECK_VALID(wrf->runoff3);
    CHECK_VALID(wrf->rc);
    CHECK_VALID(wrf->pc);
    CHECK_VALID(wrf->xlai);
    CHECK_VALID(wrf->rsmin);
    CHECK_VALID(wrf->rcs);
    CHECK_VALID(wrf->rct);
    CHECK_VALID(wrf->rcq);
    CHECK_VALID(wrf->rcsoil);
}

int wrf_coupling_run(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    double intpart;
    STRUCT_XPN_TIME timeptr;



    if (self->WRF==NULL) {
        return RET_SUCCESS;
    }

    self->__ERROR = RET_SUCCESS;

    if (self->use_cdb_input_file==1) {
        self->__ERROR = wrf_coupling_write_wrfstruct_from_data(self);
    }

    if (self->use_cdb_input_file!=2) {

        xpn->pTi->pSimTime->iyear_old = xpn->pTi->pSimTime->iyear;
        xpn->pTi->pSimTime->fTimeY_old = xpn->pTi->pSimTime->fTimeY;
        xpn->pTi->pTimeStep->fOld = xpn->pTi->pTimeStep->fAct;

        timeptr.year = self->WRF->YY;
        timeptr.mon = self->WRF->MM;
        timeptr.mday = self->WRF->DD;
        timeptr.hour = self->WRF->H;
        timeptr.min = self->WRF->M;
        timeptr.sec = self->WRF->S;
        timeptr.msec = (double)self->WRF->MS;

        xpn->pTi->pSimTime->is_leap_year = is_leap_year(timeptr.year); //  FLAG: 1 or 0
        xpn->pTi->pSimTime->msec = timeptr.msec;
        xpn->pTi->pSimTime->sec = timeptr.sec;
        xpn->pTi->pSimTime->min = timeptr.min;
        xpn->pTi->pSimTime->hour = timeptr.hour;
        xpn->pTi->pSimTime->mday = timeptr.mday;
        xpn->pTi->pSimTime->mon = timeptr.mon;
        xpn->pTi->pSimTime->year = timeptr.year;

        xpn_time_set_struc(&timeptr, &(xpn->pTi->pSimTime->iyear), &(xpn->pTi->pSimTime->fTimeY));
        xpn->pTi->pSimTime->fTimeDay = modf(xpn->pTi->pSimTime->fTimeY,&intpart);
        //expertn_database_init_water(self);



        xpn->pTi->pTimeStep->fAct = self->WRF->dt_d;






        wrf_coupling_runWetter_WRF(self);
    }

    wrf_coupling_write_copy_wrf_struct_only_climate(self->own,self->WRF);

    wrf_coupling_write_own(self);

    return self->__ERROR;
}

int wrf_coupling_done(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    if (self->WRF==NULL) {
        return RET_SUCCESS;
    }
    if (self->own!=NULL) {

        if (self->own->nsoil>0) {
            //if (self->own->sldpth!=NULL) g_free(self->own->sldpth);
            if (self->own->et!=NULL) g_free(self->own->et);
            if (self->own->stc!=NULL) g_free(self->own->stc);
            if (self->own->smc!=NULL) g_free(self->own->smc);
            if (self->own->sh2o!=NULL) g_free(self->own->sh2o);
        }

        g_free(self->own);
        self->own=NULL;
    }
    if ((self->WRF!=NULL) && (self->free_wrf_struct==1)) {
        if (self->WRF->nsoil>0) {
            //if (self->WRF->sldpth!=NULL) g_free(self->WRF->sldpth);
            if (self->WRF->et!=NULL) g_free(self->WRF->et);
            if (self->WRF->stc!=NULL) g_free(self->WRF->stc);
            if (self->WRF->smc!=NULL) g_free(self->WRF->smc);
            if (self->WRF->sh2o!=NULL) g_free(self->WRF->sh2o);
        }
        g_free(self->WRF);
        self->WRF=NULL;
    }

    if (self->use_cdb_input_file!=2) {

        g_free(xpn->pTi->pSimTime);
        g_free(xpn->pTi->pTimeStep);
        g_free(xpn->pLo->pFarm);
        g_free(xpn->pLo->pFieldplot);
        g_free(xpn->pLo->pTile);

    }

    if (self->cdb_data!=NULL) {
        free_cdb(self->cdb_data);
    }
    if (self->cdb_input_file!=NULL) {
        g_free(self->cdb_input_file);
    }

    return RET_SUCCESS;
}

int wrf_coupling_init_time_management(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    STRUCT_XPN_TIME timeptr;






    //start_YY,start_MM,start_DD,start_H,start_M,start_S,start_MS,start_julianday
    memset(&timeptr,sizeof(STRUCT_XPN_TIME),0);
    timeptr.year = self->WRF->start_YY;
    timeptr.mon = self->WRF->start_MM;
    timeptr.mday = self->WRF->start_DD;
    timeptr.hour = self->WRF->start_H;
    timeptr.min = self->WRF->start_M;
    timeptr.sec = self->WRF->start_S;
    timeptr.msec = (double)self->WRF->MS;

    xpn->pTi->pSimTime->is_leap_year = is_leap_year(timeptr.year); //  FLAG: 1 or 0
    xpn->pTi->pSimTime->msec = timeptr.msec;
    xpn->pTi->pSimTime->sec = timeptr.sec;
    xpn->pTi->pSimTime->min = timeptr.min;
    xpn->pTi->pSimTime->hour = timeptr.hour;
    xpn->pTi->pSimTime->mday = timeptr.mday;
    xpn->pTi->pSimTime->mon = timeptr.mon;
    xpn->pTi->pSimTime->year = timeptr.year;

    xpn_time_set_struc(&timeptr, &(xpn->pTi->pSimTime->iyear), &(xpn->pTi->pSimTime->fTimeY));
    xpn_time_set_struc(&timeptr, &(xpn->pTi->pSimTime->iStart_year), &(xpn->pTi->pSimTime->fStart_TimeY));



    timeptr.year = self->WRF->stop_YY;
    timeptr.mon = self->WRF->stop_MM;
    timeptr.mday = self->WRF->stop_DD;
    timeptr.hour = self->WRF->stop_H;
    timeptr.min = self->WRF->stop_M;
    timeptr.sec = self->WRF->stop_S;
    timeptr.msec = self->WRF->MS;
    xpn->pLo->pFarm->fLatitude = self->WRF->xlat;
    xpn->pLo->pFarm->fLongitude = self->WRF->xlon;

    xpn_time_set_struc(&timeptr, &(xpn->pTi->pSimTime->iStop_year), &(xpn->pTi->pSimTime->fStop_TimeY));


    xpn->pTi->pSimTime->fTimeDay=0;
    //xpn->pTi->pTimeStep->fAct=0.05;//2600.0/24.0/60.0/60.0; //1.0/10.0;
    return RET_SUCCESS;
}

void wrf_coupling_runWetter_WRF(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PCLIMATE pCl = xpn->pCl;
    PWEATHER pWeather = pCl->pWeather;
    struct_WRF* wrf = self->WRF;
    double dt;
    dt = xpn->pTi->pTimeStep->fAct;

    pWeather->fSolRad = ((double)(wrf->lwdn + wrf->soldn))*1.0e-6*86400.0;  // Watt/m2 --> MJ/(m2*day)
    pWeather->fPAR = (double)wrf->soldn * 1.0e-6*86400.0;  // Watt/m2 --> MJ/(m2*h)
    pWeather->fSolNet = (double)wrf->solnet * 1.0e-6*86400.0;  // Watt/m2 --> MJ/(m2*h)
    pWeather->fSolarDirect = (double)wrf->solardirect * 1.0e-6*86400.0;  // Watt/m2 --> MJ/(m2*h)
    pWeather->cosz = (double)wrf->cosz;
    pWeather->fTempAir = (double)wrf->T2 - 273.15; // K --> °C
    pWeather->fTempAir_zlvl = (double)wrf->sfctmp - 273.15;
    pWeather->fHumidity = calc_rel_hum((double)wrf->q2,(double)wrf->sfctmp- 273.15,(double)wrf->psfc / 100.0);
    pWeather->fWindSpeed = wrf->sfcspd;//1.0e-4;//(double)wrf->sfcspd; // Todo wird im Noah drive nicht gesetzt --> vllt in neuerster WRF Version
    /*if (wrf->sfcspd==0.0)
    {
        pWeather->fWindSpeed = EPSILON;
    }*/
    pWeather->fPreciRate = (double)wrf->prcp*(60.0*60.0*24.0);  // mm/s --> mm/day
    pWeather->fLiquPreciRate = (double)wrf->prcp*(60.0*60.0*24.0); //wrf->prcprain wird wohl irgndenwie nicht gesetzt
    pWeather->fBulkPreciRate = pWeather->fLiquPreciRate;
	pWeather->fSnow = (double)wrf->snowh* 1.0e3; // m --> mm
    pWeather->fSnowEqu = (double)wrf->sneqv* 1.0e3; // m --> mm
    pWeather->fAtmPressure = (double)wrf->psfc;

    pCl->fTempMeasHeight = (double)wrf->zlvl;
    pCl->fWindMeasHeight = (double)wrf->zlvl; // thickness of layers Was genau ist die Ausage der Variable?

    // ueberprüfe den Wertebereich bestimmter Vars:
    if (xpn->DEBUG_LEVEL>3) {
        if (((double)wrf->T2 - 273.15) < -50.0) {
            char *S;
            S = g_strdup_printf("Air Temperature is lower than -50°C --> Air Humidty can not be calculated Temperature = %f!",(double)wrf->T2 - 273.15);
            PRINT_ERROR_ID((&(self->parent)),S);
            g_free(S);
            SET_ERROR
        }

        if (pWeather->fHumidity<0.0) {

            PRINT_ERROR_ID((&(self->parent)),"Negative value: pWeather->fHumidity");
            SET_ERROR

        }

    }


}

void  wrf_coupling_write_own(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PCLIMATE pCl = xpn->pCl;
    //PWEATHER pWeather = pCl->pWeather;
    double dt;
    double T3,T2,T,L;//T1,dz,T2V,T1V,TH2V,Z0,CZIL;
    PHLAYER pHLayer;
    PSLAYER pSLayer;
    PWLAYER pWLayer;
    int i;
    double depthXPN,depthWRF;
    double thickness;
//  double L;
    dt = xpn->pTi->pTimeStep->fAct;




    if ((self->own->coupling<0) || (self->own->coupling>1)) return;




    self->own->T1 = 0.0; // effective skin temp

    for (i=0;i<self->own->nsoil;i++)
        {
            self->own->stc[i] = 0.0;
            self->own->smc[i] = 0.0;
            self->own->sh2o[i] = 0.0;
        }
        
        
    pHLayer = xpn->pHe->pHLayer;
    pSLayer = xpn->pSo->pSLayer;
    pWLayer = xpn->pWa->pWLayer;
    depthXPN = 0.0;
    depthWRF = self->own->sldpth[0];
    i = 0;
    
    
    thickness = 0.0;

    while (1) {
        pHLayer = pHLayer->pNext;
        pSLayer = pSLayer->pNext;
        pWLayer = pWLayer->pNext;
        if (pWLayer == NULL) {
            self->own->stc[i] /= thickness;
            self->own->smc[i] /= thickness;
            self->own->sh2o[i] /= thickness;
            thickness= 0.0;
            break;
        }
        self->own->stc[i] += (pHLayer->fSoilTemp + 273.15)*pSLayer->fThickness*1.0e-3;
        self->own->smc[i] += pWLayer->fContAct*pSLayer->fThickness*1.0e-3;
        self->own->sh2o[i] += pWLayer->fContAct*pSLayer->fThickness*1.0e-3;
        thickness += pSLayer->fThickness*1.0e-3;
        depthXPN += pSLayer->fThickness*1.0e-3;
        if (depthXPN > depthWRF) {  
            self->own->stc[i] /= thickness;
            self->own->smc[i] /= thickness;
            self->own->sh2o[i] /= thickness;
            thickness= 0.0;
			depthWRF +=self->own->sldpth[i];
            i++;
            if (i>self->own->nsoil-1) {
                break;
            }
        }       
    }
    
/*  for (i=0;i<self->own->nsoil;i++)
        {
            self->own->stc[i] /= self->own->sldpth[i];
            self->own->smc[i] /= self->own->sldpth[i];
            self->own->sh2o[i] /= self->own->sldpth[i];
        }*/

    self->own->albedo = xpn->pSo->fAlbedo; // Albedo mit Snow Effekt
    self->own->alb = xpn->pSo->fAlbedo; // Albedo mit Snow Effekt
    self->own->ch = 0.0; // Surface Exchange Coeeficent
    self->own->cm = 0.0; // Surface Exchange Coeeficent
    self->own->eta = xpn->pHe->pHEBalance->fHeatLatent;
    T = pCl->pWeather->fTempAir;
    T2=T*T;
    T3=T2*T;

    L = -(0.0000614342*T3 + 0.00158927*T2 - 2.36418*T + 2500.79)*1e-3*1e6; // für J /m2/day
    self->own->eta_kinematic = -self->own->eta/L; // [mm]/[day] --> W m-2

    //L = 2.49463e9 - 2.247e6 * xpn->pHe->pHLayer->pNext->fSoilTemp;
    //self->own->eta_kinematic = self->own->eta/L;
    self->own->sheat = xpn->pHe->pHEBalance->fHeatSens;
    self->own->fdown = self->own->soldn* (1.0 - xpn->pSo->fAlbedo) + self->own->lwdn;
    self->own->ec =  -L *xpn->pPl->pPltWater->fInterceptR/86400.0; // interception (xpn->pWa->fActET - xpn->pWa->pEvap->fAct) / 86400.0;
    self->own->edir = -L * xpn->pWa->pEvap->fActR/86400.0; // 2.45 /0.0864;// / 86400.0;  // DIRECT SOIL EVAPORATION (W m-2) // bodenverdunstung
    for (i=0; i<self->own->nsoil; i++) {
        self->own->et[i]=0.0;
    }
    self->own->ett = -(xpn->pPl->pPltWater->fActTranspR) * L/86400.0 ;// / 2.45 /0.0864;
    self->own->esnow = 0.0;
    self->own->drip = 0.0; // interception
    self->own->dew = 0.0; //
    if (xpn->pWa->pEvap->fPotR_old!=0.0) {
        self->own->beta = (xpn->pWa->pEvap->fActR+xpn->pPl->pPltWater->fActTranspR) / (xpn->pWa->pEvap->fPotR+xpn->pPl->pPltWater->fPotTranspR) ;
    } else {
        self->own->beta = 1.0;
    }
    self->own->beta = MIN(MAX(self->own->beta,1.0),0.0);
    self->own->etp = -xpn->pWa->fPotETR * L/86400.0 ;;
    self->own->ssoil = xpn->pHe->pHEBalance->fGroundHeat;//self->own->sheat + self->own->eta - self->own->solnet;
    self->own->flx1=0.0;
    self->own->flx2=0.0;
    self->own->flx3=0.0;
    self->own->snomlt = 0.0;
    self->own->sncovr = 0.0;
    self->own->runoff1 = xpn->pWa->fRunOffR;
    self->own->runoff2 = 0.0; // ??
    self->own->runoff3 = 0.0; // ??
    self->own->rc = 0.0; // wozu ist das gut?
    if ((xpn->pPl->pPltWater->fPotTranspR)!=0) {
        self->own->pc = (xpn->pPl->pPltWater->fActTranspR) / (xpn->pPl->pPltWater->fPotTranspR);
    } else {
        self->own->pc=1.0;
    }
    self->own->xlai = xpn->pPl->pCanopy->fLAI;
    self->own->rsmin = 0.0; //?
    self->own->rcs = 0.0; //?
    self->own->rct = 0.0; //
    self->own->rcq = 0.0; // ?
    self->own->rcsoil = 0.0; // ?

}

void  wrf_coupling_write_copy_wrf_struct_only_climate(struct_WRF* dest, struct_WRF* src)
{
    int i;

    dest->start_YY=src->start_YY;
    dest->start_MM=src->start_MM;
    dest->start_DD=src->start_DD;
    dest->start_H=src->start_H;
    dest->start_M=src->start_M;
    dest->start_S=src->start_S;
    dest->start_MS=src->start_MS;
    dest->start_julianday=src->start_julianday;
    dest->stop_YY=src->stop_YY;
    dest->stop_MM=src->stop_MM;
    dest->stop_DD=src->stop_DD;
    dest->stop_H=src->stop_H;
    dest->stop_M=src->stop_M;
    dest->stop_S=src->stop_S;
    dest->stop_MS=src->stop_MS;
    dest->xlat=src->xlat;
    dest->xlon=src->xlon;
    dest->dx=src->dx;
    dest->dy=src->dy;
    dest->grid_id=src->grid_id;
    dest->i=src->i;
    dest->j=src->j;
    dest->YY=src->YY;
    dest->MM=src->MM;
    dest->DD=src->DD;
    dest->H=src->H;
    dest->M=src->M;
    dest->S=src->S;
    dest->MS=src->MS;
    dest->julianday=src->julianday;
    dest->dt_d=src->dt_d;
    dest->nsoil=src->nsoil;
    dest->coupling=src->coupling;
    dest->lwdn=src->lwdn;
    dest->soldn=src->soldn;
    dest->solnet=src->solnet;
    dest->solardirect=src->solardirect;
    dest->cosz=src->cosz;
    dest->q2=src->q2;
    dest->q2k=src->q2k;
    dest->T2=src->T2;
    dest->Th2=src->Th2;
    dest->sfctmp=src->sfctmp;
    dest->psfc=src->psfc;
    dest->sfcspd=src->sfcspd;
    dest->prcp=src->prcp;
    dest->prcprain=src->prcprain;
    dest->snowh=src->snowh;
    dest->sneqv=src->sneqv;
    dest->zlvl=src->zlvl;
    dest->vegtyp=src->vegtyp;
    dest->soiltyp=src->soiltyp;
    dest->slopetyp=src->slopetyp;
    dest->shdfac=src->shdfac;
    dest->shdfac_IO=src->shdfac_IO;
    dest->shdmin=src->shdmin;
    dest->shdmax=src->shdmax;
    dest->alb=src->alb;
    dest->alb_IO=src->alb_IO;
    dest->snoalb=src->snoalb;
    dest->tbot=src->tbot;
    dest->z0brd=src->z0brd;
    dest->z0brd_IO=src->z0brd_IO;
    dest->z0=src->z0;
    dest->embrd=src->embrd;
    dest->emrd_O=src->emrd_O;
    dest->emissi=src->emissi;
    dest->emissi_IO=src->emissi_IO;
    dest->sldpth=src->sldpth;
    for (i=0; i<src->nsoil; i++) {
        dest->sldpth[i] = src->sldpth[i];
    }
}

void  wrf_coupling_link_wrf_to_input_file(wrf_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    char *S;
    struct_cdb_data *d;
    struct_WRF *w;
    int i,i2,i3;
    // Read Climate File:

    d = self->cdb_data;
    w = self->WRF;

    self->cdblink_len = value_input_len;
    self->cdblink = g_malloc0(sizeof(struct_cdb_link)*self->cdblink_len);

    for (i=0; i<self->cdblink_len; i++) {
        for (i2=0; i2<xpn->pXSys->var_list->size; i2++) {
            if (xpn->pXSys->var_list->vars[i2]->flag_pointer==TRUE) {
                S = g_strdup_printf("WRF.%s",value_input[i]);
                if (strcmp(xpn->pXSys->var_list->vars[i2]->varname,S)==0) {
                    for (i3=0; i3<d->size_of_values; i3++) {
                        if (strcmp(d->header[i3],S)==0) {
                            switch (xpn->pXSys->var_list->vars[i2]->vartype) {
                            case G_TYPE_INT:
                                self->cdblink[i].link.pi = xpn->pXSys->var_list->vars[i2]->val.pi;
                                self->cdblink[i].linktype = G_TYPE_INT;
                                self->cdblink[i].cdb_var = i3;
                                break;
                            case G_TYPE_DOUBLE:
                                self->cdblink[i].link.pd = xpn->pXSys->var_list->vars[i2]->val.pd;
                                self->cdblink[i].linktype = G_TYPE_DOUBLE;
                                self->cdblink[i].cdb_var = i3;
                                break;
                            case G_TYPE_FLOAT:
                                self->cdblink[i].link.pf = xpn->pXSys->var_list->vars[i2]->val.pf;
                                self->cdblink[i].linktype = G_TYPE_FLOAT;
                                self->cdblink[i].cdb_var = i3;
                                break;
                            default:
                                self->cdblink[i].cdb_var = i3;
                                self->cdblink[i].linktype = -1;
                                self->cdblink[i].link.pd = NULL;
                            }
                            break;
                        }
                    }
                }
                g_free(S);
            }
        }
    }



}

int wrf_coupling_write_wrfstruct_from_data(wrf_coupling *self)
{
    int i;
    struct_cdb_link *l;
    for (i=0; i<self->cdblink_len; i++) {
        l = &(self->cdblink[i]);
        switch(l->linktype) {
        case G_TYPE_INT:
            *(l->link.pi) = (int)self->cdb_data->valuelist[self->cdb_sim_counter][l->cdb_var];
            break;
        case G_TYPE_DOUBLE:
            *(l->link.pd) = (double)self->cdb_data->valuelist[self->cdb_sim_counter][l->cdb_var];
            break;
        case G_TYPE_FLOAT:
            *(l->link.pf) = (float)self->cdb_data->valuelist[self->cdb_sim_counter][l->cdb_var];
            break;
        }
    }

    self->cdb_sim_counter++;
    if (self->cdb_sim_counter>=self->cdb_data->size_of_valuelist) {
        PRINT_ERROR_ID((&(self->parent)),"self->cdb_sim_counter>=self->cdb_data->size_of_valuelist");
        return 1; // beenden
    }
    return RET_SUCCESS;
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



// q = spez. humidity [kg/kg]
// P = Pressure [hPa]
// return P_H20 [hPa]
static double get_Ph20(double q,double P)
{
    double M_dry,M_h20,xh20;
    M_dry = 28.9644;
    M_h20 = 18.01534;
    xh20 = - M_dry * q / ((q-1.0) * M_h20 - M_dry*q);
    return xh20 * P;
}


// q = spez. humidity [kg/kg]
// T = Temperature [Grad C]
// P = Pressure [hPa]
// return RH [%]
static double calc_rel_hum(double q,double T,double P)
{
    double PH20;
    PH20 = get_Ph20(q,P);
    return PH20 / get_e(T) * 100.0;
}

static struct_cdb_data* load_cdb(char *cdb_filename)
{
    char *S,*S2,*S3;
    FILE *f;
    int i,i2;
    double *values;
    int size;
    struct_cdb_data* self;
    self = NULL;
    f = fopen(cdb_filename,"r");
    i = 0;  
    if (f==NULL) return NULL;
    while (1) {
        S = fgets_mem(f);
        if (S==NULL) break;
        size = getStringColumn_get_Size(S,",");
        if (size < 1) break;
        S2 = (char*)g_malloc0(strlen(S)+1);
        if (i==0) {
            self= g_malloc0(sizeof(struct_cdb_data));
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

    fclose(f);
    
    if (i < 2) return NULL;

    return self;
}

static void free_cdb(struct_cdb_data *self)
{
    int i,i2;
    if (self!=NULL) {
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
        g_free(self);
    }
}
