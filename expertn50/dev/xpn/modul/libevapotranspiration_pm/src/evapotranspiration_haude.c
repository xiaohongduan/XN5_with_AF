//      evapotranspiration_fao.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//  


#include "evapotranspiration.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "evapotranspiration_reg.h"

int evapotranspiration_haude_load(evapotranspiration *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	self->ETpot_Day = 0.0;
	self->ETpot_Day_=0.0;
	self->ETpot_Year = 0.0;

	self->Epot_Day = 0.0;
	self->Epot_Year = 0.0;	
	self->fcumtrad = 0.0;
	self->fcumaero = 0.0;
	
	self->iHaudeFalse = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.Haude.false Haude",1);
	self->iHaudePan = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.Haude.pan evapotranspiration",0);
	
	return RET_SUCCESS;
}


int evapotranspiration_haude_run(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PWEATHER  		pWE = xpn->pCl->pWeather;
	PWATER			pWa = xpn->pWa;
	PEVAP     		pEV = xpn->pWa->pEvap;
	PTIME			pTi = xpn->pTi;
	PPLANT			pPl = xpn->pPl;

	double   EvTeiler, fHaudeFac;
	double   Pi=(float)3.141592654;
	double DeltaT;
	
	double   fSD1,fSD2,fSatDef;

	DeltaT = pTi->pTimeStep->fAct;

	if(self->iHaudeFalse == 1)
		evapotranspiration_integrate_small_time_step_vars(self); //can be found in evapotranspiration_fao.c
	else
		evapotranspiration_get_Haude_weather(self);

 if(NewDay(pTi))
  {

//  Berechnung der potentiellen Verdunstung nach Haude (Entwurf VDI 3786 Blatt 13 S.27)

    fSD1 = (double)(6.11 * exp(17.269*(double)self->weather.fTempMax
         /(237.3+(double)self->weather.fTempMax)));

//    fSD1 = (double)(6.11 * exp(17.269*(double)pWE->fTempAve
//         /(237.3+(double)pWE->fTempAve)));

//    fSD1 = (double)(6.11 * exp(17.62*(double)pWE->fTempAve
//         /(243.12+(double)pWE->fTempAve)));

	fSD2 =((double)1.0 - self->weather.fDailyHumidity/(double) 100.0);

    fSatDef=fSD1*fSD2;

//  Verdunstung aus der Wasserschale (Kesselverdunstung)
	pWE->fPanEvap  = (double) 0.5 * fSatDef;//0.5 fuer Verdunstung aus bedeckter Wasserschale
	
    
	if (pPl != NULL)
	{
    // plant pan factor or Haude factor	

	 fHaudeFac = (double)0;

        if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) //Winterweizen
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.17;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.24;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.33;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.41;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.37;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.28;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.15;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.11;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) //Wintergerste
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.17;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.24;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.37;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.38;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.32;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.22;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.15;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.11;
		}
		
		if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) //Mais
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.17;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.21;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.24;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.25;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.26;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.21;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.18;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.11;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"OA")==0) //Hafer
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.15;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.34;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.44;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.45;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.30;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.19;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.11;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"RY")==0) //Roggen
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.17;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.23;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.30;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.36;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.36;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.27;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.15;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.11;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"SB")==0) //Zuckerruebe
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.15;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.23;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.30;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.37;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.33;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.26;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.20;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.11;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.11;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"FW")==0) //Fichte
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.08;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.04;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.14;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.35;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.39;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.34;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.31;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.25;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.20;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.13;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.07;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.05;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"BW")==0) //Buche
		{
		    if((pTi->pSimTime->fTimeY > 0)&&(pTi->pSimTime->fTimeY <= 31))
				fHaudeFac = (double)0.01;
		    if((pTi->pSimTime->fTimeY > 31)&&(pTi->pSimTime->fTimeY <= 59))
				fHaudeFac = (double)0.00;
		    if((pTi->pSimTime->fTimeY > 59)&&(pTi->pSimTime->fTimeY <= 90))
				fHaudeFac = (double)0.04;
		    if((pTi->pSimTime->fTimeY > 90)&&(pTi->pSimTime->fTimeY <= 120))
				fHaudeFac = (double)0.10;
		    if((pTi->pSimTime->fTimeY > 120)&&(pTi->pSimTime->fTimeY <= 151))
				fHaudeFac = (double)0.23;
		    if((pTi->pSimTime->fTimeY > 151)&&(pTi->pSimTime->fTimeY <= 181))
				fHaudeFac = (double)0.28;
		    if((pTi->pSimTime->fTimeY > 181)&&(pTi->pSimTime->fTimeY <= 212))
				fHaudeFac = (double)0.32;
		    if((pTi->pSimTime->fTimeY > 212)&&(pTi->pSimTime->fTimeY <= 243))
				fHaudeFac = (double)0.26;
		    if((pTi->pSimTime->fTimeY > 243)&&(pTi->pSimTime->fTimeY <= 273))
				fHaudeFac = (double)0.17;
		    if((pTi->pSimTime->fTimeY > 273)&&(pTi->pSimTime->fTimeY <= 304))
				fHaudeFac = (double)0.10;
		    if((pTi->pSimTime->fTimeY > 304)&&(pTi->pSimTime->fTimeY <= 334))
				fHaudeFac = (double)0.01;
		    if((pTi->pSimTime->fTimeY > 334)&&(pTi->pSimTime->fTimeY <= 366))
				fHaudeFac = (double)0.00;
		}

      }
	  else
	  {
	   fHaudeFac = (double) 0.11; // Brache
	   
	  }

     pWa->fPotETDay = fHaudeFac * fSatDef; 
	 
	

   }

	// SolPartTime(pTi) can be found in evapotranspiration_fao.c
	EvTeiler = (DeltaT < (double)0.11)?
	           SolPartTime(pTi)
	           : (double)1.0;

	if (self->iHaudePan == 1)
		{
		xpn->pWa->fPotETdt = EvTeiler * pWE->fPanEvap;
		}	
	else
		{
		xpn->pWa->fPotETdt = EvTeiler * pWa->fPotETDay;
		}
		
	xpn->pWa->fPotETR = xpn->pWa->fPotETdt *pWa->kc_factor / DeltaT;

	/***DAILY VARIABLES********************************************************************/
	if(NewDay(self->parent.pTi)) {
		self->Rs_Day = 0.0;
		self->ETpot_Day = self->ETpot_Day_;
		self->ETpot_Day_=0.0;
	}

	if((int)pTi->pSimTime->fTimeY == 1) {
		self->ETpot_Year = 0.0;
	}
	self->Rs_Day += self->weather.fDaylySolRad*DeltaT;
	self->ETpot_Day_ += xpn->pWa->fPotETdt;
	self->ETpot_Year += self->ETpot_dt;
	self->ETpot_Cum += self->ETpot_dt;
	return  RET_SUCCESS;
}

int evapotranspiration_get_Haude_weather(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCLIMATE pCl = xpn->pCl;
	PWEATHER pWe = pCl->pWeather;
	
	if ((pTi->pSimTime->fTimeY==self->weather.fTimeY_save) && (pTi->pSimTime->iyear == self->weather.iyear_save)) {
		return RET_SUCCESS;
	} else {
		self->weather.fTimeY_save = pTi->pSimTime->fTimeY;
		self->weather.iyear_save = pTi->pSimTime->iyear;
	}
	
	if ((int)NewDay(pTi)) 
		{
		self->iHaudeTime = 0;
		self->weather.fTempMax = self->__weather.fTempMax;
		self->weather.fDailyHumidity = self->__weather.fDailyHumidity;
		}
	else if((pTi->pSimTime->fTimeDay > 0.6)&&(self->iHaudeTime == 0))
		{
		self->iHaudeTime = 1;
		self->__weather.fTempMax = pWe->fTempAir_daily_models;
		self->__weather.fDailyHumidity = pWe->fHumidity_daily_models;
		}
	
	return  RET_SUCCESS;
}
/*int evapotranspiration_integrate_small_time_step_vars(evapotranspiration *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCLIMATE pCl = xpn->pCl;
	PWEATHER pWe = pCl->pWeather;
	PWATER pWa = xpn->pWa;
	double dt;

	if ((pTi->pSimTime->fTimeY==self->weather.fTimeY_save) && (pTi->pSimTime->iyear == self->weather.iyear_save)) {
		return RET_SUCCESS;
	} else {
		self->weather.fTimeY_save = pTi->pSimTime->fTimeY;
		self->weather.iyear_save = pTi->pSimTime->iyear;
	}

	dt = pTi->pTimeStep->fAct;



	if ((int)NewDay(pTi)) {
		self->weather.fTempMax = self->__weather.fTempMax;
		self->weather.fTempMin = self->__weather.fTempMin;
		self->weather.fTempAve = 0.5*(self->weather.fTempMax + self->weather.fTempMin);	
		self->weather.fWindSpeed = self->__weather.fWindSpeed;
		self->weather.fDaylySolRad = self->__weather.fDaylySolRad;
		self->weather.fDailyHumidity = self->__weather.fDailyHumidity;

		self->__weather.fTempMax=pWe->fTempAir;
		self->__weather.fTempMin=pWe->fTempAir;
		self->__weather.fTempMax=-99.0;
		self->__weather.fTempMin= 99.0;		
		self->__weather.fTempAve=0.0;
		self->__weather.fDaylySolRad=0.0;
		self->__weather.fWindSpeed = 0.0;
		self->__weather.fDailyHumidity = 0.0;
	} else {
		if ((pWe->fTempAir > self->__weather.fTempMax)&&(pTi->pSimTime->fTimeDay > 0.5)) {
			self->__weather.fTempMax = pWe->fTempAir;
		}
		if ((pWe->fTempAir < self->__weather.fTempMin)&&(pTi->pSimTime->fTimeDay < 0.5)) {
			self->__weather.fTempMin = pWe->fTempAir;
		}

		self->__weather.fWindSpeed += pWe->fWindSpeed * dt;
		self->__weather.fTempAve += pWe->fTempAir * dt;
		self->__weather.fDaylySolRad += pWe->fSolRad * dt;
		self->__weather.fDailyHumidity += pWe->fHumidity * dt;

	}

	/*self->weather.fTempAve = *(self->TairMean);
	self->weather.fTempMin = *(self->TairMin);
	self->weather.fTempMax = *(self->TairMax);
	self->weather.fDaylySolRad = *(self->fSolRad);
	return RET_SUCCESS;
}*/

