//      interception.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "interception.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "interception_reg.h"
#include <xpn_register_var.h>

G_DEFINE_TYPE(interception, interception, EXPERTN_MODUL_BASE_TYPE);


static void interception_class_init(interceptionClass *klass){}


static void interception_init(interception *self)
{
    gchar *S;
    // hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
    // Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
    // Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!
    
    // print Output Message if Modul is used from Expert N
    S  = g_strdup_printf("%s\t%d\tInit MODUL INTERCEPTION!",__FILE__,__LINE__);
    fprintf(stdout,"%s\n",S);
    fflush(stdout);
    g_free(S);
}

int interception_load(interception *self)
{
    char *ini_filename;
    char *S;
    expertn_modul_base *xpn = &(self->parent);
    xpn_register_var *var_list; 
    var_list = xpn->pXSys->var_list;
    
    //=========================================================================================
        
    // Wenn die Init bereits geladen ist
    if(self->__INIT_DONE==0)
    {
        self->__INIT_DONE=1;
    }
    else
    {
        return RET_SUCCESS;
    }   
    // Vars:
    
    // Ouputs auf 0 setzen:
    //memset(&(self->output),0,sizeof(P_Output));
    
    // Zunächst: Registry:  
    interception_register_var(self);
    
    /* Read from INI File: */   
    S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.interception.ini_filename");
    if(S == NULL)
    {
        PRINT_ERROR("Missing entry 'ini_filename' in paragraph [treemix] in .cfg-file");
    }   
    else
    {
        char *S2;
        S2 = expertn_modul_base_replace_std_templates(xpn,S);
        ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);       
        if (interception_load_config(self, ini_filename)!=0)
        {
            PRINT_ERROR("Error Reading INTERCEPTION INI File");
        }
        free(ini_filename);
        free(S2);
    }
    
    
    
    
    //===========================================================================================   
    
    /* RUTTER MODEL */
    /* Canopy Factors */
    //self->SaturationCap = 0.95;
    
    /* Constants, unit conversions see: "the ETo calculator" */
    self->lambda    = 590.0;        // [cal/cm3]            //2.45;         // [MJ/kg]; 0.0024662 [MJ/cm3]
    self->rho_c     = 2.9e-4;       // [cal/cm3*°C]     //1.2122e-9;    // [MJ/kg*Celsius]; 1.2122e-9 [MJ/cm3*Celsius]
    self->gamma     = 0.66;         // [mbar/°C]
    
    
    /* State Variables */
    self->CanopyWatR = 0.0;
    self->CanopyWatR_old = 0.0;
    self->DrainageR = 0.0;
    self->Drainage_dt = 0.0;
    self->Drainage_dt2 = 0.0;
    self->Epot_R = 0.0;
    self->InterceptionR = 0.0;
    
    self->Bal_I = 0.0;
    self->Bal_D = 0.0;
    self->Bal_E = 0.0;
    self->Bal_R = 0.0;
    self->Bal_T = 0.0;
    
    self->Epot_Yr = 0.0;
    self->Eact_Yr = 0.0;
    self->Throughfall_Yr = 0.0;
    self->FreeThroughfall_Yr = 0.0;
    
    self->C_S_dt = 0.0;
    
    
    /* regression model */
    self->SI_Yr = 0.0;
    self->SI_dt = 0.0;
    self->SImax = 0.0;
    
    
    INTERCEPTION_REG    //Makro aufrufen
    
    return RET_SUCCESS;
}

int interception_load_config(interception *self,const char* configfile)
{
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    const char* filename=configfile;
    //gsize count;
    //char *parameter_files;
    
    /* Create a new GKeyFile object and a bitwise list of flags. */
    keyfile = g_key_file_new ();

    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    /* Load the GKeyFile from keyfile.conf or return. */
    if (!g_key_file_load_from_file (keyfile, configfile, flags, &error))
    {
            PRINT_ERROR(error->message);
            return -1;
    }
    
    // get Parameter values for RUTTER model
    GET_INI_DOUBLE(self->SaturationCap,"Rutter","SaturationCap");
    GET_INI_DOUBLE(self->b,"Rutter","b");
    GET_INI_DOUBLE(self->a_help,"Rutter","a_help");
    GET_INI_DOUBLE(self->p,"Rutter","p");
    
    g_key_file_free(keyfile);

    return RET_SUCCESS;
}

int interception_register_var(interception *self)
{
    
    //Plant:
    //xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->plant.density,"treemix.plant.density",0.0);

    return RET_SUCCESS;
}


// Our Modul Functions:
/* RUTTER model, Rutter et al. 1971, Agricultural Meteorology */    
int interception_Rutter_Model(interception *self)
{
    /* Abbreviations */
    expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
    PWEATHER pWe = xpn->pCl->pWeather;
    //PPLANT pPl = xpn->pPl;
    
    /* model funtions */
    double get_Rn(interception *self);
    
    /* variables */ 
    double Rn, Rn2;
    //double alpha = 0.15;  // reflection coefficient of the canopy for short wave reflection, affecting Rn
    double res_a;           // aerodynamic resistance [s/m]
    double h, z, z_null, d, k, u, u_z, K;   // height parameters [cm], von Karman Constant
    double a, b, p;         // coefficients for calculating DrainageR, throughfall coefficient [mm]
    double measurementheight;
    //double P, gamma;      // [kPa/Celsius]
    double Delta;           //eqn. 36, Slope of the Saturation Vapor Pressure-Temperature Curve (Δ) 
    double dt;
    double Term1, Term2, Term3;
    double result1, result2, result3;
    
    /* Get Weather and Location Values */
    self->T = pWe->fTempAir;
    self->RH = pWe->fHumidity;
    self->Rs = xpn->pCl->pWeather->fSolRad;             // [MJ/m2*day]
    //self->Rn = xpn->pCl->pWeather->fSolNeto;          // [MJ/m2*day]
    self->wind = xpn->pCl->pWeather->fWindSpeed;    
    measurementheight = xpn->pCl->fWindMeasHeight;
    self->elevation = xpn->pLo->pFarm->fAltitude;
    dt = xpn->pTi->pTimeStep->fAct;
    
    // Bulk Precipitation as input
    self->PreciRate_dt = pWe->fBulkPreciRate * dt;
    
    /* Height Dependent Parameters */
    h = xpn->pPl->pCanopy->fPlantHeight;
    z = h + measurementheight*100.0;        
    z_null = 0.1*h;
    d = 0.75*h;
    k = 0.41;
    u = pWe->fWindSpeed*100;    // [m/s] -> [cm/s]
    u_z = (u/4.87)*log(67.8*z-5.42); // [cm/s]
    
    /* Drainage Coefficients */ 
    b = self->b;
    a = log(self->a_help)-(b*self->SaturationCap);  // 0.002 mm/min = 2.88 mm/day
    K = exp(a);

    /* Calculating Throughfall */
    p = self->p;    // plant cover?
    self->CanopyInputR = (1.0-p)*pWe->fBulkPreciRate;
    self->CanopyWatR = self->CanopyInputR + self->CanopyWatR_old;
    self->FreeThroughfallR = p * pWe->fBulkPreciRate;
    
    /* Actual Canopy Input [mm/dt] */
    self->CanopyInput_dt = (1.0-p)*self->PreciRate_dt;
    self->CanopyWat_dt = self->CanopyInput_dt + self->CanopyWat_dt_old;
    self->FreeThroughfall_dt = p * self->PreciRate_dt;
    
    self->C_S_dt = self->CanopyWat_dt/self->SaturationCap;  
    
    /* Calculating Atmospheric Parameters */    
    Delta = (2503.0*exp( 17.27*self->T/(self->T+237.3))/pow(self->T+237.3,2.0))/10.0;   // [kPa/°C]/10.0 -> [mbar/°C]
    self->es = (0.6108*exp(17.27*self->T/(self->T+237.3)))/10.0;                        // [kPa/°C]/10.0 -> [mbar/°C]
    self->ea = (self->RH/100.0 * self->es)/10.0;                                        // [kPa/°C]/10.0 -> [mbar/°C], Method nr. 3 in table 4, using rel. humidity and temperature at the hour
    //P = 101.3*pow((293.0-0.0065*self->elevation)/293.0, 5.26);    //atmospheric pressure eqn34, Psychrometric Constant (γ)
    //gamma = 0.000665*P;                               // eqn 35
    
    res_a = (9.2/u);    // approximation [s/cm]
    //res_a = (1.0/(k*k*u)*pow((log((z-d)/z_null)), 2.0))/100.0;     // Aerodynamic Resistance, [s/cm] = [s/m]/100.0
    
    /* calculate net Radiation */
    Rn = get_Rn(self);  // [MJ/m2]
    Rn2 = pWe->fSolNet;
    C_DEBUG(Rn2);
    
    self->Rn = Rn/0.041868/86400.0;          // [MJ/m2*day] -> [cal/cm2*sec]
    self->Rn_dt = self->Rn*dt;
    
    /* Calculating Evaporation from Canopy */   
    Term1 = Delta*self->Rn;
    Term2 = self->rho_c*(self->es-self->ea)/res_a;
    Term3 = self->lambda*(Delta+self->gamma);
    
    result1 = Term1 + Term2;            // result: [cal*mbar/°C*cm2*s]
    result2 = result1/Term3;            // result: [cm/s]
    result3 = result2*10.0;             // conversion: [cm/s]->[mm/s]
    self->Epot_R = result3*86400.0;             // [mm/s] -> [mm/d]
    self->Epot_dt = self->Epot_R * dt;  
    
    /* Getting DrainageR (as a rate) */ 
    if(self->CanopyWat_dt >= self->SaturationCap)   // for C>=S
    {       
        self->Drainage_dt = log(dt*b*K + exp(-b*self->CanopyWat_dt_old))/-b;    // eq. 11
        self->DrainageR = 2.88*exp(b*(self->CanopyWat_dt-self->SaturationCap)); // eq. 10, geht gegen unendlich je größer das gespeicherte wasser in der canopy     
        self->Eact_R = self->Epot_R;
    }
    if(self->CanopyWat_dt < self->SaturationCap)    // for C<S
    {
        self->Drainage_dt = 0.0;
        self->DrainageR = 0.0;
        
        if(self->CanopyWat_dt == 0.0)   // Valente et al. 1995, Journal of Hydrology
        {
            self->Eact_R = 0.0;
        }
        else
        {
            self->Eact_R = self->Epot_R * (self->CanopyWat_dt/self->SaturationCap);     // Epot is diminished by (C/S)
        }
    }
    
    
    self->Interception_dt = self->Eact_R*dt;
    self->Eact_dt = self->Eact_R*dt;
    self->Throughfall_dt = self->Drainage_dt + self->FreeThroughfall_dt;
    self->CanopyWat_dt_old = self->CanopyWat_dt - self->Drainage_dt - (self->Eact_R*dt);    // eq. 2
    
    
    // actualize the precipitation rate:
    // throughfall_dt is everything that reaches the ground so:
    xpn->pCl->pWeather->fPreciRate = self->Throughfall_dt/dt;
    xpn->pCl->pWeather->fLiquPreciRate = self->Throughfall_dt/dt;
    if(xpn->pCl->pWeather->fPreciRate < 0.0)
    {
        xpn->pCl->pWeather->fPreciRate = 0.0;
        xpn->pCl->pWeather->fLiquPreciRate = 0.0;
    }
    
    //============================================================
    if(NewDay(xpn->pTi))
    {
        self->RnDay             = 0.0;      
        self->PreciRate_Day     = 0.0;
        self->Interception_Day  = 0.0;
        self->Epot_Day          = 0.0;
        self->Eact_Day          = 0.0;
        self->Throughfall_Day   = 0.0;      
        self->Drainage_Day      = 0.0;
    }
    self->RnDay             += (self->Rn*dt);           // [MJ/m2*dt]   
    self->PreciRate_Day     += (self->PreciRate_dt);    // [mm/d]
    self->Interception_Day  += (self->Interception_dt); // [mm/d]   
    self->Epot_Day          += (self->Epot_dt);         // [mm/d]
    self->Eact_Day          += (self->Eact_dt);     // [mm/d]   
    self->Throughfall_Day   += (self->Throughfall_dt);  // [mm/d]   
    self->Drainage_Day      += (self->Drainage_dt);     // [mm/d]
    
    self->Epot_Yr += self->Epot_dt;
    self->Eact_Yr += self->Eact_dt;
    self->Throughfall_Yr += self->Throughfall_dt;
    self->PreciRate_Yr += self->PreciRate_dt;
    self->FreeThroughfall_Yr += self->FreeThroughfall_dt;
    
    
    self->Bal_I += (self->PreciRate_dt - self->Throughfall_dt);
    self->Bal_E += (self->PreciRate_dt - self->Throughfall_dt);
    self->Bal_T += ((p*self->PreciRate_dt) + self->Drainage_dt);
    self->Bal_R += (self->PreciRate_dt);
    
    // give to Expert-N structs
    xpn->pPl->pPltWater->fInterceptR = self->Eact_R;
    xpn->pPl->pPltWater->fInterceptdt = self->Interception_dt;
    xpn->pPl->pPltWater->fInterceptDay = self->Interception_Day;
    
    return RET_SUCCESS;
  }

int interception_regression(interception *self)
{
    /* Description 
     * 
     * entnommen aus Maniak, U. Hydrologie und Wasserwirtschaft, 1997
     * 
    */
    
    /* Abbreviations */
    expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
    PWEATHER pWe = xpn->pCl->pWeather;
    
    /* Variables */
    double dt;
    //double LAI;
    
    double Term1;
    double Term2;
    double Term3;
    double Term4;
    double Term5;
    
    dt = xpn->pTi->pTimeStep->fAct;
    //LAI = 4.0;
    //xpn->pPl->pCanopy->fLAI = LAI;
    
    if(pWe->fPreciRate != 0.0)
    {
        self->SImax = 0.935+(0.498*xpn->pPl->pCanopy->fLAI)-(0.0057*pow(xpn->pPl->pCanopy->fLAI, 2.0));
    
        Term1 = 0.254*(pWe->fPreciRate*dt);
        Term2 = 0.2*xpn->pPl->pCanopy->fLAI;
        Term3 = 0.0111*pow((pWe->fPreciRate*dt), 2.0);
        Term4 = 0.0271*(pWe->fPreciRate*dt)*xpn->pPl->pCanopy->fLAI;
        Term5 = 0.0109*pow(xpn->pPl->pCanopy->fLAI, 2.0);
    
        self->SI_dt = -0.42+Term1+Term2-Term3+Term4-Term5;  // [mm/dt]
        
        if (self->SI_dt > self->SImax)
        {
            self->SI_dt = self->SImax;
        }
    }
    else
    {
        self->SI_dt = 0.0;
    }
    
    self->SI_R = self->SI_dt/dt;    //[mm/d]
    
    // actualize Precipitation Rate [mm/d]
    xpn->pCl->pWeather->fPreciRate = xpn->pCl->pWeather->fBulkPreciRate - self->SI_R;
    if(xpn->pCl->pWeather->fPreciRate < 0.0)
    {
        xpn->pCl->pWeather->fPreciRate = 0.0;
    }
    
    
    if(NewDay(xpn->pTi))
    {
        self->SI_Day = 0.0;
    }
    self->SI_Day += self->SI_dt;
    self->SI_Yr += self->SI_dt;
    
    return RET_SUCCESS;
}
  
double get_Rn(interception *self)
{
    /* Abbreviations */
    expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self); 
    
    /* Variables */ 
    double dt;
    double latitude;
    double soldec;
    int iJulianDay;
    double t, b, Sc;
    double h;
    double w, w1, w2, ws;
    double dr;
    double Rs0_noon, Ra_noon, Rn;
    double fcd;
    double ftime_day;
    
    dt = xpn->pTi->pTimeStep->fAct; 
    //elevation = xpn->pLo->pFarm->fAltitude;
    iJulianDay  = xpn->pTi->pSimTime->fTimeY;   // actual julian day
    ftime_day = xpn->pTi->pSimTime->fTimeDay;   // /(24.0*60.0*60.0);       // [s]->[day]   
    h = xpn->pTi->pSimTime->hour;               //(double)(((double)((double)ftime_day - (int)ftime_day))*24.0);
    
    //self->T = xpn->pCl->pWeather->fTempAir;
    //self->RH = xpn->pCl->pWeather->fHumidity; 
    //self->Rs = xpn->pCl->pWeather->fSolRad;   
    
    //Extraterrestrial radiation for hourly periods (Ra )
    latitude = xpn->pLo->pFarm->fLatitude*M_PI/180.0;       // eq. 49, [rad]
    soldec = 0.409*sin(2.0*M_PI/365.0*iJulianDay-1.39);     // solar declination [radians]  
    self->omega = acos(-tan(soldec)*tan(latitude));         // sunset hour angle [radians]
    ws          = acos(-tan(soldec)*tan(latitude)); 
    self->N = 24.0/M_PI*self->omega;                            // max. possible sunshine duration [hours]
    
    b = 2.0*M_PI*(iJulianDay-81.0)/364.0;                   //eq. 58
    Sc = 0.1645*sin(2.0*b)-0.1255*cos(b)-0.025*sin(b);      //eq. 57, seasonal correction factor    
    t = h+0.5;
    
    w  = M_PI/12.0 *(t + Sc-12.0);      //eqn 55, solar time angle  
    w1 = w - M_PI*dt;       //(dt); //*24.0)/24.0;              //eqn 53
    w2 = w + M_PI*dt;       //(dt); //*24.0)/24.0;              //eqn 54
    
    /* conditions for stability in eq. 48, (eqn 56) */
    if(w1 < -ws) w1 = -ws;  
    if(w2 < -ws) w2 = -ws;
    if(w1 > ws) w1 = ws;
    if(w2 > ws) w2 = ws;
    if(w1 > w2) w1 = w2;
    
    dr = (1.0+0.033*cos(2.0*M_PI/365.0*iJulianDay));        //eqn 50, inverse relative distance factor (squared) for earth-sun
    
    if( dt < 1.0)
    {
        self->Ra_h = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));      // [MJ/m2*dt], orig eq. 48
    }
    else
    {
        self->Ra_d = 24.0/M_PI * 4.92 *dr*(self->omega*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*sin(self->omega));   // [MJ/m2*day]  
    }
    
    //self->Rs = (0.25+0.5*(self->n/self->N))*self->Ra;

    //Clear-sky solar radiation Rs0
    self->Rs0 = (0.75+2.0e-5*self->elevation)*self->Ra_h;   // [MJ/m2*dt]

    //cloudiness factor
    if(self->Rs0>0.0)
    {
        fcd = 1.35*self->Rs/self->Rs0-0.35; //eqn 45
    }
    else
    {
        t = 12.5;                               // noon
        w  = M_PI/12 *(t +Sc-12.0);
        w1 = w - M_PI*dt;                       // /24.0; //eqn 53
        w2 = w + M_PI*dt;                       // /24.0; //eqn 54
        ws = acos(-tan(soldec)*tan(latitude));
        
        if(w1 < -ws) w1 = -ws;                  //conditions eqn 56
        if(w2 < -ws) w2 = -ws;
        if(w1 > ws) w1 = ws;
        if(w2 > ws) w2 = ws;
        if(w1 > w2) w1 = w2;
        
        //b = 2.0*M_PI*(iJulianDay-81.0)/364.0;
        //Sc = 0.1645*sin(2.0*b)-0.1255*cos(b)-0.025*sin(b);//seasonal correction factor
        //dr = (1.0+0.033*cos(2.0*M_PI/365.0*iJulianDay)); //eqn 50
        Ra_noon = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));     // [MJ/m2*dt]
        Rs0_noon = (0.75+2.0e-5*self->elevation)*Ra_noon;
        fcd = 1.35*self->Rs/Rs0_noon-0.35;
    }   
    fcd = MAX(fcd,0.05);
    fcd = MIN(fcd,1.0);
    
    //Net Solar or Net Short-Wave Radiation (Rns )
    self->Rns = self->Rs - 0.23 * self->Rs; //eqn 43, [MJ/m2*d], with 0.23 = canopy reflection coefficient
    
    //Net Long-Wave Radiation (Rnl )
    //self->es = 0.6108*exp(17.27*self->T/(self->T+237.3));         //eqn. 37, Saturation Vapor Pressure (es)
    //self->ea = self->RH/100.0 * self->es;                 //Method nr. 3 in table 4, eqn 37, 41, Actual Vapor Pressure (ea), using rel. humidity and temperature at the hour

    self->Rnl = 2.042*1e-10*fcd*(0.34-0.14*sqrt(self->ea))*pow((self->T+273.16), 4.0); //eqn 44, [MJ/m2*dt]
    
    //Net Radiation (Rn )
    Rn = self->Rns - self->Rnl; //[MJ/m2*dt]    
    if(Rn < 0.0)
    {
        Rn = 0.0;
    }
    
    
    /* integrating over day */
    if(NewDay(xpn->pTi))
    {
        self->RsDay = 0.0;
        self->Ra_hDay = 0.0;
    }
    self->RsDay += (xpn->pCl->pWeather->fSolRad*(dt));
    self->Ra_hDay += (self->Ra_h);

    return Rn;
}
