//      pollen.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//
// Birkenpollen: 25 µm diameter

#include "pollen.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

G_DEFINE_TYPE(pollen, pollen, EXPERTN_MODUL_BASE_TYPE);


static void pollen_class_init(pollenClass *klass) {}


static void pollen_init(pollen *self) {}



// Our Modul Functions:


int pollen_load(pollen *self)
{
	self->step=0;
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->pollination_amount,"pollen.pollination amount",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->pollen_storage,"pollen.pollen_storage",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->pollen_storage_amount,"pollen.pollen_storage_amount",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->pollen_release_trigger,"pollen.pollen_release_trigger",0.0);
	return RET_SUCCESS;
}



// pollen of 11um effective radius"     "ug/kg-dryair

//int pollen_BiomassGrowth1(pollen *self)
//{
//	expertn_modul_base *xpn = &(self->parent);
//	PATMOS pAtmos = xpn->pCh->pAtmos;
//	char S[128];
//	sprintf(S,"STEP: %d",self->step);
//	PRINT_MESSAGE(xpn,3,S);
//	if (self->step==400)
//		{
//			pAtmos->dustr1 = 5000.0/xpn->pTi->pTimeStep->fAct;
//		}
//	else
//		{
//			pAtmos->dustr1 = 0.0;
//		}
//	self->step++;
//	return RET_SUCCESS;
//}
//
//int pollen_BiomassGrowth2(pollen *self)
//{
//	expertn_modul_base *xpn = &(self->parent);
//	PATMOS pAtmos = xpn->pCh->pAtmos;
//
//	pAtmos->dustr1 = 0.0;
//	if (self->step==30)
//		{
//			self->step=0;
//			pAtmos->dustr1 = 1.0;
//		}
//
//	self->step++;
//	return RET_SUCCESS;
//}


int pollen_const_release(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PATMOS pAtmos = xpn->pCh->pAtmos;

	pAtmos->dustr1 = 0.0;
	if (self->pollination==1)
		{
			pAtmos->dustr1 = 200.0;
		}

	self->pollination_amount+=pAtmos->dustr1*xpn->pTi->pTimeStep->fAct;//

	return RET_SUCCESS;
}

int pollen_SILAM_Sofiev(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PATMOS pAtmos = xpn->pCh->pAtmos;
	double trigger,dustr;


	if (self->pollination==1)
		{
			dustr = 200.0 + self->pollen_storage;

			trigger = Trigger_relHum(self) * Trigger_Wind(self) * Trigger_NS(self); //Die Trigger sind ein Faktor zwischen 1 und 0 und reduzieren entsprechend die Freisetzungsrate von Pollen TODO: nicht freigesetzte Pollenmenge muss in einen Storage!

			pAtmos->dustr1       = min(500.0,(dustr * trigger));
			self->pollen_storage = dustr - pAtmos->dustr1;


		}
	else
		{
			trigger=0.0;
			pAtmos->dustr1 = 0.0;
			self->pollen_storage = 0.0;
		}

	self->pollen_release_trigger=trigger;
	self->pollen_storage_amount+=self->pollen_storage*xpn->pTi->pTimeStep->fAct;
	self->pollination_amount+=pAtmos->dustr1*xpn->pTi->pTimeStep->fAct;//

	return RET_SUCCESS;
}


int pollen_integrate_small_time_step_vars(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCLIMATE pCl = xpn->pCl;
	PWEATHER pWe = pCl->pWeather;
	PWATER pWa = xpn->pWa;
	double dt;

	//C_DEBUG(pWe->fTempAir);
	//C_DEBUG(self->weather.fTempAve);

	if ((pTi->pSimTime->fTimeY==self->weather.fTimeY_save) && (pTi->pSimTime->iyear == self->weather.iyear_save))
		{
			return RET_SUCCESS;
		}
	else
		{
			self->weather.fTimeY_save = pTi->pSimTime->fTimeY;
			self->weather.iyear_save = pTi->pSimTime->iyear;
		}

	dt = pTi->pTimeStep->fAct;



	if ((int)NewDay(pTi))
		{
			self->weather.fTempMax = self->__weather.fTempMax;
			self->weather.fTempMin = self->__weather.fTempMin;
			self->weather.fTempAve = self->__weather.fTempAve;
			//self->weather.fPotTraDay = self->__weather.fPotTraDay;
			//self->weather.fEvapPotDay = self->__weather.fEvapPotDay;
			//self->weather.fActTraDay = self->__weather.fActTraDay;
			self->weather.fDaylySolRad = self->__weather.fDaylySolRad;

			self->__weather.fTempMax=pWe->fTempAir;
			self->__weather.fTempMin=pWe->fTempAir;
			self->__weather.fTempAve=0.0;
			self->__weather.fEvapPotDay=0.0;
			self->__weather.fPotETDay = 0.0;
			self->__weather.fDaylySolRad=0.0;
		}
	else
		{
			if (pWe->fTempAir > self->__weather.fTempMax)
				{
					self->__weather.fTempMax = pWe->fTempAir;
				}
			if (pWe->fTempAir < self->__weather.fTempMin)
				{
					self->__weather.fTempMin = pWe->fTempAir;
				}

			self->__weather.fTempAve += pWe->fTempAir * dt;
			self->__weather.fEvapPotDay += pWa->pEvap->fPotR * dt;
			self->__weather.fPotETDay += pWa->fPotETR*dt;
			self->__weather.fDaylySolRad += pWe->fSolRad * dt;

		}

	/*self->weather.fTempAve = *(self->TairMean);
	self->weather.fTempMin = *(self->TairMin);
	self->weather.fTempMax = *(self->TairMax);
	self->weather.fDaylySolRad = *(self->fSolRad);*/
	return RET_SUCCESS;
}

// ------------------------Impact of Environment on Pollen release from male catkin--------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////
//SILAM model by Mikhail Sofiev, siehe auch
//////////////////////////////////////////////////////////////////////////////////////////////////////
//SILAM 1) nur RH affects pollen release...
int Pollen_Release_SILAM_Sofiev_load(pollen *self)	//_load ist Funktion die Variablen deklariert, und nur ein mal aufgerufen wird.
{
	//expertn_modul_base *xpn = &(self->parent);
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	//PATMOS pAtmos = xpn->pCh->pAtmos;


	//double pollen_emmision_rate = 0.0;

	return RET_SUCCESS;
}

int Pollen_Release_SILAM_Sofiev(pollen *self)	//_load ist Funktion die Variablen deklariert, und nur ein mal aufgerufen wird.
{
	self->pollination_amount = self->pollination_amount * Trigger_relHum(self) * Trigger_Wind(self) * Trigger_NS(self); //Die Trigger sind ein Faktor zwischen 1 und 0 und reduzieren entsprechend die Freisetzungsrate von Pollen TODO: nicht freigesetzte Pollenmenge muss in einen Storage!


	return RET_SUCCESS;
}

double Trigger_relHum(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);		//Deklaration der Variablen
	double ret;

	ret = 0.0;

	if (xpn->pCl->pWeather->fHumidity < 50.0)			//Komplette verfügbare Pollenmenge wird freigesetzt wenn Luftfeuchte kleiner 50%
		{
			ret = 1.0;
		}
	if (xpn->pCl->pWeather->fHumidity >= 50.0 || xpn->pCl->pWeather->fHumidity <= 80.0)
		{
			ret = ((80-(xpn->pCl->pWeather->fHumidity))/(80-50));
		}
	if (xpn->pCl->pWeather->fHumidity > 80.0)
		{
			ret = 0.0;
		}

	return min(1.0,max(0.0,ret));
}

double Trigger_Wind(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double ret;

	ret = 0.0;


	ret = (1.0-(exp(-(xpn->pCl->pWeather->fWindSpeed + xpn->pCl->pWeather->fWindSpeedConv)/1.5)));


	return min(1.0,max(0.0,ret));
}

double Trigger_NS(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double ret;

	ret = 0.0;


	ret = (1.0-(xpn->pCl->pWeather->fPreciRate/0.5));


	return min(1.0,max(0.0,ret));
}


// ------------------------- Phänologie-dt values-----------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
////////////////////////////////////////////////////////////////////////////////////////////
//pollen_TSM ist ein lineares Temperatursummen Modell (TSM)
//
//nach base Temperature nach Sofiev. Sofiev rechnet mit stündlichen Temperaturmittelwerten
//******************************************************************************************
//xpn->pPl->pRoot->fDepth	Durchwurzelungstiefe										[mm]
//self->GDD					grwoth degree days											[°C]
//self->start_GDD_int 		Anfangswert für grwoth degree days Integration				[°C]
//self->GDDc_start			kritischer Wert für GDD ab welchem Anthese beginnt			[°C]
//self->GDDc_end			kritischer Wert für GDD ab welchem Anthese endet			[°C]
//self->pollination			Freisetzung von Pollen (vor Anthese:0; nach Anthese: 1)		[-]
//self->T_base				Base temperature for integration of GDD						[°C]
//self->start_mday = 1		day of month												[-]
//self->start_mon = 1		month														[-]
//pTi = xpn->pTi			next time step												[-]
//S, S2						Strings for month day
//
////////////////////////////////////////////////////////////////////////////////////////////

//Phänologie wird EINMAL TÄGLICH aufgerufen und mit täglichen Durchschnittstemperaturen berechnet
int pollen_TSM_DT_load(pollen *self)							//_load ist Funktion die Variablen deklariert, und nur ein mal aufgerufen wird.
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	//Das sind die zusätzlichen Outputvariablen
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD,"pollen.GDD",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_BudDev,"pollen.GDD_BudDev",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_Anthesis,"pollen.GDD_Anthesis",0.0);


	self->GDD = 0.0; // [Grad C] Cumulative Temperature
	self->start_GDD_int = 0;
	self->pollination=0;


	// [Grad C] base temperature
	self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DT.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	// [Grad C] cirtical cum temp for bud burst
	self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DT.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	// [Grad C] critical cum temp for end of anthesis
	self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DT.gdd_end_crit",850.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850

//hier wird geprüft ob bereits lokale Variablen eingelesen werden oder nicht. Wenn nicht werden nun die globalen Variablenwerte verwendet.
	if (self->T_base == -99.0)
		// [Grad C] base temperature
		self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	if (self->GDDc_start == -99.0)
		// [Grad C] cirtical cum temp for bud burst
		self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	if (self->GDDc_end == -99.0)
		// [Grad C] critical cum temp for end of anthesis
		self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_end_crit",100.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850


	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN TSM DT.start_day_phenology");	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 01.01.
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM DT.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm
	return RET_SUCCESS;
}


/*int pollen_TSM_phenol_dev_dt(pollen *self)		//diese Funktion wird dann immer wieder in jedem Zeitschritt aufgerufen.
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	pollen_integrate_small_time_step_vars(self);
	dt = pTi->pTimeStep->fAct;

//	if ((int)NewDay(pTi))
//		{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1; 								//Entwicklungszustand 1
			self->GDD = 0.0;									//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		}


	if (self->start_GDD_int>0)
		{
			//self->GDD += MAX(self->weather.fTempAve - self->T_base,0.0);
			self->GDD += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;
			if (self->GDD >= self->GDDc_start)
				{
					self->start_GDD_int=2;						//Entwicklungszustand 2
					self->pollination=1;    					//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
				}
		}

	if (self->start_GDD_int==2)
		{
			if (self->GDD >= self->GDDc_end)
				{
					self->start_GDD_int=3;						//Entwicklungszustand 3
					self->pollination=0;    					//Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet
				}

		}


	//}

	return RET_SUCCESS;
}*/

//folgende Funktion nimmt ein definiertes Startdatum für Beginn der Knospenentwicklung und nicht eine bestimmte Tageslänge (wie Pollen TSM DLEN DT)
int pollen_TSM_phenol_dev_dt(pollen *self)		//diese Funktion wird dann immer wieder in jedem Zeitschritt aufgerufen.
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	pollen_integrate_small_time_step_vars(self);
	dt = pTi->pTimeStep->fAct;

//	if ((int)NewDay(pTi))
//		{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1; 								//Entwicklungszustand 1
			self->GDD = 0.0;									//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		}


	if (self->start_GDD_int>0 && self->start_GDD_int < 3)
		{
			if (self->start_GDD_int > 1)
				{	
					self->GDD_BudDev = self->GDD_BudDev; 
				}
				
			else
				{
			//self->GDD += MAX(self->weather.fTempAve - self->T_base,0.0);
			self->GDD_BudDev += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;
				}
				
			if (self->GDD_BudDev >= self->GDDc_start)
				{
					self->start_GDD_int=2;						//Entwicklungszustand 2
					self->pollination=1;    					//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
				
				self->z=self->z+1;
			
				if (self->z == 1)
					{
						self->GDD_Anthesis = self->GDD_BudDev - self->GDDc_start;	//weil ja der Grenzwert für Start der Anthese ja eventuell schon überschritten ist.
					}
				
				self->GDD_Anthesis += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;  //Temperatursummen für Anthesedauer werden aufintegriert
			
				}
		}

	if (self->start_GDD_int==2)
		{
			if (self->GDD_Anthesis >= self->GDDc_end)
				{
					self->start_GDD_int=3;						//Entwicklungszustand 3
					self->pollination=0;    					//Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet
				}
			
			if (self->start_GDD_int == 3)
				{
					self->GDD_Anthesis = 0.0;
				}

		}


	//}

	return RET_SUCCESS;
}

// ------------------------- Phänologie-dt values-----------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
////////////////////////////////////////////////////////////////////////////////////////////
//pollen_TSM_DLEN ist ein lineares Temperatursummen Modell (TSM) das die Tageslänge berücksichtigt
//
//nach base Temperature nach Sofiev. Sofiev rechnet mit stündlichen Temperaturmittelwerten
//******************************************************************************************
//xpn->pPl->pRoot->fDepth	Durchwurzelungstiefe										[mm]
//self->GDD					grwoth degree days											[°C]
//self->start_GDD_int 		Anfangswert für grwoth degree days Integration				[°C]
//self->GDDc_start			kritischer Wert für GDD ab welchem Anthese beginnt			[°C]
//self->GDDc_end			kritischer Wert für GDD ab welchem Anthese endet			[°C]
//self->pollination			Freisetzung von Pollen (vor Anthese:0; nach Anthese: 1)		[-]
//self->T_base				Base temperature for integration of GDD						[°C]
//self->start_mday = 1		day of month												[-]
//self->start_mon = 1		month														[-]
//pTi = xpn->pTi			next time step												[-]
//S, S2						Strings for month day
//
////////////////////////////////////////////////////////////////////////////////////////////

//Phänologie wird EINMAL TÄGLICH aufgerufen und mit täglichen Durchschnittstemperaturen berechnet
int pollen_TSM_DLEN_DT_load(pollen *self)							//_load ist Funktion die Variablen deklariert, und nur ein mal aufgerufen wird.
{
	expertn_modul_base *xpn = &(self->parent);
//	char *S,*S2;
	double DayLength;
	int i;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;
    self->start_julian_day = 0.0;

	//Das sind die zusätzlichen Outputvariablen
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD,"pollen.GDD",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_BudDev,"pollen.GDD_BudDev",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_Anthesis,"pollen.GDD_Anthesis",0.0);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->start_julian_day,"pollen.bud_develop_start",0.0);
    
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->anthesis_start_date,"pollen.anthesis_start_date",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->anthesis_stop_date,"pollen.anthesis_stop_date",0.0);
        
	


	self->GDD = 0.0; // [Grad C] Cumulative Temperature
	self->GDD_BudDev = 0.0; //[Grad C] Cummulative Temperature since start of Bud Development
	self->GDD_Anthesis = 0.0; //[Grad C] Cummulative Temperature since start of Anthesis
	self->start_GDD_int = 0;
	self->pollination=0;
	self->DayLengthCrit= 0.0; //Tageslänge in h
	self->z=0;
	


	// [Grad C] base temperature
	self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DLEN DT.t_base",-99.0);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	// [Grad C] cirtical cum temp for bud burst
	self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DLEN DT.gdd_start_crit",-99.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	// [Grad C] critical cum temp for end of anthesis
	self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DLEN DT.gdd_end_crit",-99.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850
	// Tageslänge in Stunden
	self->DayLengthCrit = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DLEN DT.start_dlen_phenology",-99.0);
///
///development---double calculateDayLength(double latitude, int JulianDay) in xpn_register_var.c
///

//hier wird geprüft ob bereits lokale Variablen eingelesen werden oder nicht. Wenn nicht werden nun die globalen Variablenwerte verwendet.
	if (self->T_base == -99.0)
		// [Grad C] base temperature
		self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	if (self->GDDc_start == -99.0)
		// [Grad C] cirtical cum temp for bud burst
		self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	if (self->GDDc_end == -99.0)
		// [Grad C] critical cum temp for end of anthesis
		self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_end_crit",100.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850
	if (self->DayLengthCrit == -99.0)
		// Tageslänge in Stunden
		self->DayLengthCrit = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.start_dlen_phenology",10.0);

	if (self->DayLengthCrit <= 0.0)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM DT.start_day_phenology not set? Check!");
			self->start_mday = 1;
			self->start_mon = 1;
		}

	else
		{
			self->start_mday = 1;
			self->start_mon = 1;

			for (i=0; i<366; i++)
				{
					DayLength = calculateDayLength(xpn->pLo->pFarm->fLatitude,i)*24.0;

					if (DayLength >= self->DayLengthCrit) //Vorsicht!: bedeutet, daß Phänolgoie beginnt sobald die kritische Tageslänge im Frühjahr überschritten wird. Aber es bedeutet auch, daß die Phängologische Entwicklung auch wieder aufhört sobald diese kritische Tageslänge im Herbst unterschritten wird.
						{
							STRUCT_XPN_TIME str_time;
							xpn_time_get_struc(&str_time,xpn->pTi->pSimTime->year,(double)i);
							self->start_mday = str_time.mday;
							self->start_mon = str_time.mon;
                            self->start_julian_day = (double)i;
							break;
						}
				}
            

		}
//DayLength = calculateDayLength(xpn->pLo->pFarm->fLatitude,(int)xpn->pTi->pSimTime->fTimeY);


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm
	return RET_SUCCESS;
}


/*int pollen_TSM_phenol_dev_DLEN_dt(pollen *self)		//diese Funktion wird dann immer wieder in jedem Zeitschritt aufgerufen.
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	pollen_integrate_small_time_step_vars(self);
	dt = pTi->pTimeStep->fAct;

//	if ((int)NewDay(pTi))
//		{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1; 								//Entwicklungszustand 1
			self->GDD = 0.0;									//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		}


	if (self->start_GDD_int>0)
		{
			//self->GDD += MAX(self->weather.fTempAve - self->T_base,0.0);
			self->GDD += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;
			if (self->GDD >= self->GDDc_start)
				{
					self->start_GDD_int=2;						//Entwicklungszustand 2
					self->pollination=1;    					//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
				}
		}

	if (self->start_GDD_int==2)
		{
			if (self->GDD >= self->GDDc_end)
				{
					self->start_GDD_int=3;						//Entwicklungszustand 3
					self->pollination=0;    					//Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet
				}

		}


	//}

	return RET_SUCCESS;
}*/

int pollen_TSM_phenol_dev_DLEN_dt(pollen *self)		//diese Funktion wird dann immer wieder in jedem Zeitschritt aufgerufen.
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	
	pollen_integrate_small_time_step_vars(self);
	dt = pTi->pTimeStep->fAct;

//	if ((int)NewDay(pTi))
//		{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1; 			//Entwicklungszustand 1
		//	self->GDD_BudDev = 0.0;			//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		//	self->GDD_Anthesis = 0.0;		//Blütenentwicklung ist deaktiviert
		}


	if (self->start_GDD_int > 0 && self->start_GDD_int < 3)
		{
			if (self->start_GDD_int > 1)
				{	
					self->GDD_BudDev = self->GDD_BudDev; 
				}
				
			else
				{
						self->GDD_BudDev += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;
				}
				
			if (self->GDD_BudDev >= self->GDDc_start)
				{
					self->start_GDD_int=2;	//Entwicklungszustand 2
					self->pollination=1;    	//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
					
					if (self->anthesis_start_date<=0.0)
						{
							self->anthesis_start_date=xpn->pTi->pSimTime->fTimeY;
						}
				
								
				self->z=self->z+1;
			
				if (self->z == 1)
					{
						self->GDD_Anthesis = self->GDD_BudDev - self->GDDc_start;	//weil ja der Grenzwert für Start der Anthese ja eventuell schon überschritten ist.
					}
				
				self->GDD_Anthesis += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;  //Temperatursummen für Anthesedauer werden aufintegriert
								
				}
		}

	if (self->start_GDD_int==2)
		{
			if (self->GDD_Anthesis >= self->GDDc_end)
				{
					self->start_GDD_int=3;	//Entwicklungszustand 3
					self->pollination=0;    //Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet-keine Pollenfreisetzung mehr
					//self->GDD_Anthesis=0.0; //Anthese beendet
					
					if (self->anthesis_stop_date<=0.0)
						{
							self->anthesis_stop_date=xpn->pTi->pSimTime->fTimeY;
						}
				}
	if (self->start_GDD_int == 3)
				{
					self->GDD_Anthesis = 0.0;
				}

		}


	//}

	return RET_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//pollen_TSM_WC ist ein lineares Temperatursummen Modell (TSM) wobei zusätzlich der
//Bodenwassergehalt berücksichtigt wird.
//Problem: für die Variablen alpha und beta sind in der Literatur keine art- bzw.
//sortenspezifischen Werte hinterlegt... (fatchfaktoren...)
//
//nach Yuan W. et al.Ecol.Res. 2007 "Simulating phenolotical characteristics of two dominant
//grass species in a semi-arid steppe ecosystem"-> Model_2
//*******************************************************************************************************
//xpn->pPl->pRoot->fDepth 		Durchwurzelungstiefe										[mm]
//fRootDepth					== xpn->pPl->pRoot->fDepth
//self->GDD						grwoth degree days											[°C]
//self->start_GDD_int 			Anfangswert für grwoth degree days Integration				[°C]
//self->GDDc_start				kritischer Wert für GDD ab welchem Anthese beginnt			[°C]
//self->GDDc_end				kritischer Wert für GDD ab welchem Anthese endet			[°C]
//self->pollination				Freisetzung von Pollen (vor Anthese:0; nach Anthese: 1)		[-]
//self->T_base					Base temperature for integration of GDD						[°C]
//self->start_mday				day of month												[-]
//self->start_mon				month														[-]
//self->alpha					Art-/Sortenspezifischer Parameter für GDDberechnung mit WC	[°C mm mm-1]
//self->beta					Art-/Sortenspezifischer Parameter für GDDberechnung mit WC	[°C]
//iRootLayer					ite Wurzelschicht startend mit der 1.  Schicht...			[-]
//xpn->pSo->pSLayer->pNext		go to next deeper soil layer								[-]
//pSL 							== xpn->pSo->pSLayer->pNext
//rsm							relative soil moisture	(fraction)							[mm mm-1]
//pWL->fContAct					actual soil water content (fraction)						[mm mm-1]
//pSW->fContPWP					permanent wilting point (fraction)							[mm mm-1]
//pSW->fContFK					field capacity (fraction)									[mm mm-1]
//PTIME pTi = xpn->pTi			next time step												[-]
//S, S2							Strings for month day
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Phänologie wird EINMAL TÄGLICH aufgerufen und mit täglichen Durchschnittstemperaturen berechnet
int pollen_TSM_WC_DT_load(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	//Das sind die zusätzlichen Outputvariablen
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD,"pollen.GDD",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_BudDev,"pollen.GDD_BudDev",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_Anthesis,"pollen.GDD_Anthesis",0.0);


	self->GDD = 0.0; // [Grad C] Cumulative Temperature
	self->start_GDD_int = 0;
	self->pollination=0;

	// [Grad C] base temperature
	self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.t_base",-5.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	// [Grad C] cirtical cum temp for bud burst
	self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.gdd_start_crit",750.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 300
	// [Grad C] critical cum temp for end of anthesis
	self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.gdd_end_crit",850.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 600.0

//hier wird geprüft ob bereits lokale Variablen eingelesen werden oder nicht. Wenn nicht werden nun die globalen Variablenwerte verwendet.
	if (self->T_base == -99.0)
		// [Grad C] base temperature
		self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	if (self->GDDc_start == -99.0)
		// [Grad C] cirtical cum temp for bud burst
		self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	if (self->GDDc_end == -99.0)
		// [Grad C] critical cum temp for end of anthesis
		self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_end_crit",100.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850
	
	

	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.start_day_phenology");	//no calibration values were provided by Yuan et al. 2007 Ecol Res. CB Scheyern; 01.01.
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM WC DT.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}

	// [Grad C * mm/mm] variety specific parameter
	self->alpha = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.alpha",10.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res
	// [Grad C] variety specific parameter
	self->beta = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.beta",10.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm
	return RET_SUCCESS;
}



/*int pollen_TSM_WC_phenol_dev_dt(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	dt = pTi->pTimeStep->fAct;
	pollen_integrate_small_time_step_vars(self);

	double rsm; // relative soil moisture
	double fDepth; //mm
	double fRootDepth; // actual rooting depth
	PSLAYER pSL;
	PWLAYER pWL;
	PSWATER pSW;
	int iRootLayer; // actueal deepest rooted soil layer
	int i;

	//if ((int)NewDay(pTi))
	//{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1;											//Entwicklungszustand 1
			self->GDD = 0.0;												//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		}


	if (self->start_GDD_int>0 && self->start_GDD_int < 3)
		{
			self->GDD += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;

			fDepth = 0.0;
			iRootLayer=0;
			fRootDepth = xpn->pPl->pRoot->fDepth;
			pSL = xpn->pSo->pSLayer->pNext;
			while(fDepth<=fRootDepth)
				{
					if (iRootLayer >= xpn->pSo->iLayers-1) break;
					fDepth += pSL->fThickness; //fThickness: mm
					iRootLayer++;
					pSL = pSL->pNext;
				}

			rsm=0.0;
			for (i=1,pWL = xpn->pWa->pWLayer->pNext,pSW=xpn->pSo->pSWater->pNext; i<iRootLayer; i++,pWL=pWL->pNext,pSW=pSW->pNext)
				{
					rsm += (pWL->fContAct - pSW->fContPWP) / (pSW->fContFK-pSW->fContPWP);
				}
			rsm /= iRootLayer-1;

			if (self->GDD >= self->GDDc_start - (self->alpha * rsm + self->beta))
				{
					self->start_GDD_int=2;									//Entwicklungszustand 2
					self->pollination=1;									//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
				}

		}
	if (self->start_GDD_int>0)
		{
			if (self->GDD >= self->GDDc_end)
				{
					self->start_GDD_int=3;						//Entwicklungszustand 3
					self->pollination=0;    					//Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet
				}
		}


	//}

	return RET_SUCCESS;
}*/

int pollen_TSM_WC_phenol_dev_dt(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	dt = pTi->pTimeStep->fAct;
	pollen_integrate_small_time_step_vars(self);

	double rsm; // relative soil moisture
	double fDepth; //mm
	double fRootDepth; // actual rooting depth
	PSLAYER pSL;
	PWLAYER pWL;
	PSWATER pSW;
	int iRootLayer; // actueal deepest rooted soil layer
	int i;

	//if ((int)NewDay(pTi))
	//{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1;											//Entwicklungszustand 1
			self->GDD = 0.0;												//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		}


	if (self->start_GDD_int>0 && self->start_GDD_int < 3)
		{
			if (self->start_GDD_int > 1)
				{	
					self->GDD_BudDev = self->GDD_BudDev; 
				}
				
			else
				{
			self->GDD_BudDev += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;
				}

			fDepth = 0.0;
			iRootLayer=0;
			fRootDepth = xpn->pPl->pRoot->fDepth;
			pSL = xpn->pSo->pSLayer->pNext;
			while(fDepth<=fRootDepth)
				{
					if (iRootLayer >= xpn->pSo->iLayers-1) break;
					fDepth += pSL->fThickness; //fThickness: mm
					iRootLayer++;
					pSL = pSL->pNext;
				}

			rsm=0.0;
			for (i=1,pWL = xpn->pWa->pWLayer->pNext,pSW=xpn->pSo->pSWater->pNext; i<iRootLayer; i++,pWL=pWL->pNext,pSW=pSW->pNext)
				{
					rsm += (pWL->fContAct - pSW->fContPWP) / (pSW->fContFK-pSW->fContPWP);
				}
			rsm /= iRootLayer-1;

			if (self->GDD_BudDev >= self->GDDc_start - (self->alpha * rsm + self->beta))
				{
					self->start_GDD_int=2;									//Entwicklungszustand 2
					self->pollination=1;									//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
				
					self->z=self->z+1;
				
					if (self->z == 1)
						{
							self->GDD_Anthesis = max((self->GDD_BudDev - self->GDDc_start), 0.0);	//weil ja der Grenzwert für Start der Anthese ja eventuell schon überschritten ist.
						}
					
					self->GDD_Anthesis += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;  //Temperatursummen für Anthesedauer werden aufintegriert
				
				}

		}
	
	if (self->start_GDD_int==2)
		{
			if (self->GDD_Anthesis >= self->GDDc_end)
				{
					self->start_GDD_int=3;	//Entwicklungszustand 3
					self->pollination=0;    //Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet-keine Pollenfreisetzung mehr
					//self->GDD_Anthesis=0.0; //Anthese beendet
				}
	if (self->start_GDD_int == 3)
				{
					self->GDD_Anthesis = 0.0;
				}

		}


	//}

	return RET_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//pollen_TSM_WC_DLEN ist ein lineares Temperatursummen Modell (TSM) wobei zusätzlich der
//Bodenwassergehalt (WC) berücksichtigt wird und die Tageslänge die den Beginn der Knospenentwicklung determiniert (DLEN).
//Problem: für die Variablen alpha und beta sind in der Literatur keine art- bzw.
//sortenspezifischen Werte hinterlegt... (fatchfaktoren...)
//
//nach Yuan W. et al.Ecol.Res. 2007 "Simulating phenolotical characteristics of two dominant
//grass species in a semi-arid steppe ecosystem"-> Model_2
//*******************************************************************************************************
//xpn->pPl->pRoot->fDepth 		Durchwurzelungstiefe										[mm]
//fRootDepth					== xpn->pPl->pRoot->fDepth
//self->GDD						grwoth degree days											[°C]
//self->start_GDD_int 			Anfangswert für grwoth degree days Integration				[°C]
//self->GDDc_start				kritischer Wert für GDD ab welchem Anthese beginnt			[°C]
//self->GDDc_end				kritischer Wert für GDD ab welchem Anthese endet			[°C]
//self->pollination				Freisetzung von Pollen (vor Anthese:0; nach Anthese: 1)		[-]
//self->T_base					Base temperature for integration of GDD						[°C]
//self->start_mday				day of month												[-]
//self->start_mon				month														[-]
//self->alpha					Art-/Sortenspezifischer Parameter für GDDberechnung mit WC	[°C mm mm-1]
//self->beta					Art-/Sortenspezifischer Parameter für GDDberechnung mit WC	[°C]
//iRootLayer					ite Wurzelschicht startend mit der 1.  Schicht...			[-]
//xpn->pSo->pSLayer->pNext		go to next deeper soil layer								[-]
//pSL 							== xpn->pSo->pSLayer->pNext
//rsm							relative soil moisture	(fraction)							[mm mm-1]
//pWL->fContAct					actual soil water content (fraction)						[mm mm-1]
//pSW->fContPWP					permanent wilting point (fraction)							[mm mm-1]
//pSW->fContFK					field capacity (fraction)									[mm mm-1]
//PTIME pTi = xpn->pTi			next time step												[-]
//S, S2							Strings for month day
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Phänologie wird EINMAL TÄGLICH aufgerufen und mit täglichen Durchschnittstemperaturen berechnet
int pollen_TSM_WC_DLEN_DT_load(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	//char *S,*S2;
	double DayLength;
	int i;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	//Das sind die zusätzlichen Outputvariablen
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD,"pollen.GDD",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_BudDev,"pollen.GDD_BudDev",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD_Anthesis,"pollen.GDD_Anthesis",0.0);


	self->GDD = 0.0; // [Grad C] Cumulative Temperature
	self->start_GDD_int = 0;
	self->pollination=0;
	self->DayLengthCrit= 0.0; //Tageslänge in h
	self->z=0;

	// [Grad C] base temperature
	self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DLEN DT.t_base",-5.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	// [Grad C] cirtical cum temp for bud burst
	self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DLEN DT.gdd_start_crit",750.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 300
	// [Grad C] critical cum temp for end of anthesis
	self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DLEN DT.gdd_end_crit",850.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 600.0
	// Tageslänge in Stunden
	self->DayLengthCrit = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DLEN DT.start_dlen_phenology",10.0);


//hier wird geprüft ob bereits lokale Variablen eingelesen werden oder nicht. Wenn nicht werden nun die globalen Variablenwerte verwendet.
	if (self->T_base == -99.0)
		// [Grad C] base temperature
		self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	if (self->GDDc_start == -99.0)
		// [Grad C] cirtical cum temp for bud burst
		self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	if (self->GDDc_end == -99.0)
		// [Grad C] critical cum temp for end of anthesis
		self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_end_crit",100.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850
	if (self->DayLengthCrit == -99.0)
		// Tageslänge in Stunden
		self->DayLengthCrit = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.start_dlen_phenology",10.0);



	if (self->DayLengthCrit <= 0.0)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM WC DLEN DT.start_day_phenology not set? Check!");
			self->start_mday = 1;
			self->start_mon = 1;
		}

	else
		{
			self->start_mday = 1;
			self->start_mon = 1;

			for (i=0; i<366; i++)
				{
					DayLength = calculateDayLength(xpn->pLo->pFarm->fLatitude,i)*24.0;

					if (DayLength >= self->DayLengthCrit) //Vorsicht!: bedeutet, daß Phänolgoie beginnt sobald die kritische Tageslänge im Frühjahr überschritten wird. Aber es bedeutet auch, daß die Phängologische Entwicklung auch wieder aufhört sobald diese kritische Tageslänge im Herbst unterschritten wird.
						{
							STRUCT_XPN_TIME str_time;
							xpn_time_get_struc(&str_time,xpn->pTi->pSimTime->year,(double)i);
							self->start_mday = str_time.mday;
							self->start_mon = str_time.mon;
							break;
						}
				}

		}

/*	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DT.start_day_phenology");	//no calibration values were provided by Yuan et al. 2007 Ecol Res. CB Scheyern; 01.01.
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM WC DT.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}*/

	// [Grad C * mm/mm] variety specific parameter
	self->alpha = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DLEN DT.alpha",10.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res
	// [Grad C] variety specific parameter
	self->beta = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DLEN DT.beta",10.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm
	return RET_SUCCESS;
}



int pollen_TSM_WC_DLEN_dt(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double dt;
	dt = pTi->pTimeStep->fAct;
	pollen_integrate_small_time_step_vars(self);

	double rsm; // relative soil moisture
	double fDepth; //mm
	double fRootDepth; // actual rooting depth
	PSLAYER pSL;
	PWLAYER pWL;
	PSWATER pSW;
	int iRootLayer; // actueal deepest rooted soil layer
	int i;

	//if ((int)NewDay(pTi))
	//{
	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_GDD_int=1;											//Entwicklungszustand 1
			self->GDD = 0.0;												//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
		}


	if (self->start_GDD_int>0 && self->start_GDD_int < 3)
		{
			if (self->start_GDD_int > 1)
				{	
					self->GDD_BudDev = self->GDD_BudDev; 		//Knospenentwicklung endet
				}
				
			else
				{
			self->GDD_BudDev += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt; //Knospenentwicklung beginnt
				}

			
			//Berechnung des Wassergehalts des durchwurzelten Bodens
			fDepth = 0.0;
			iRootLayer=0;
			fRootDepth = xpn->pPl->pRoot->fDepth;
			pSL = xpn->pSo->pSLayer->pNext;
			while(fDepth<=fRootDepth)
				{
					if (iRootLayer >= xpn->pSo->iLayers-1) break;
					fDepth += pSL->fThickness; //fThickness: mm
					iRootLayer++;
					pSL = pSL->pNext;
				}

			//Berechnung der relative soil moisture des rooted soils (rsm)
			rsm=0.0;	
			for (i=1,pWL = xpn->pWa->pWLayer->pNext,pSW=xpn->pSo->pSWater->pNext; i<iRootLayer; i++,pWL=pWL->pNext,pSW=pSW->pNext)
				{
					rsm += (pWL->fContAct - pSW->fContPWP) / (pSW->fContFK-pSW->fContPWP);
				}
			rsm /= iRootLayer-1;

			if (self->GDD_BudDev >= self->GDDc_start - (self->alpha * rsm + self->beta))	//Wenn Knospenentwicklung die StartTSM für die Blütenentwicklung minus eines Terms für den Bodenwassergehalt überschreitet, dann beginnt die Anthese...
				{
					self->start_GDD_int=2;									//Entwicklungszustand 2
					self->pollination=1;									//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
				
					self->z=self->z+1;
				
					if (self->z == 1)
						{
							self->GDD_Anthesis = max((self->GDD_BudDev - self->GDDc_start - (self->alpha * rsm + self->beta)), 0.0);	//weil ja der Grenzwert für Start der Anthese ja eventuell schon überschritten ist.
						}
					
					self->GDD_Anthesis += MAX(xpn->pCl->pWeather->fTempAir - self->T_base,0.0)*dt;  //Temperatursummen für Anthesedauer werden aufintegriert
				
				}

		}
	
	if (self->start_GDD_int==2)
		{
			if (self->GDD_Anthesis >= self->GDDc_end)
				{
					self->start_GDD_int=3;	//Entwicklungszustand 3
					self->pollination=0;    //Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet-keine Pollenfreisetzung mehr
					//self->GDD_Anthesis=0.0; //Anthese beendet
				}
	if (self->start_GDD_int == 3)
				{
					self->GDD_Anthesis = 0.0;
				}

		}


	//}

	return RET_SUCCESS;
}


// ------------------------- Phänologie-daily values-----------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
////////////////////////////////////////////////////////////////////////////////////////////
//pollen_TSM ist ein lineares Temperatursummen Modell (TSM)
//
//nach Yuan W. et al.Ecol.Res. 2007 "Simulating phenolotical characteristics of two dominant
//grass species in a semi-arid steppe ecosystem" -> Model_1
//******************************************************************************************
//xpn->pPl->pRoot->fDepth	Durchwurzelungstiefe										[mm]
//self->GDD					grwoth degree days											[°C]
//self->start_GDD_int 		Anfangswert für grwoth degree days Integration				[°C]
//self->GDDc_start			kritischer Wert für GDD ab welchem Anthese beginnt			[°C]
//self->GDDc_end			kritischer Wert für GDD ab welchem Anthese endet			[°C]
//self->pollination			Freisetzung von Pollen (vor Anthese:0; nach Anthese: 1)		[-]
//self->T_base				Base temperature for integration of GDD						[°C]
//self->start_mday = 1		day of month												[-]
//self->start_mon = 1		month														[-]
//pTi = xpn->pTi			next time step												[-]
//S, S2						Strings for month day
//
////////////////////////////////////////////////////////////////////////////////////////////

//Phänologie wird EINMAL TÄGLICH aufgerufen und mit täglichen Durchschnittstemperaturen berechnet
int pollen_TSM_DAILY_load(pollen *self)							//_load ist Funktion die Variablen deklariert, und nur ein mal aufgerufen wird.
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD,"pollen.GDD",0.0);


	self->GDD = 0.0; // [Grad C] Cumulative Temperature
	self->start_GDD_int = 0;
	self->pollination=0;


	// [Grad C] base temperature
	self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DAILY.t_base",-5.0);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	// [Grad C] cirtical cum temp for bud burst
	self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DAILY.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	// [Grad C] critical cum temp for end of anthesis
	self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM DAILY.gdd_end_crit",850.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850


	//hier wird geprüft ob bereits lokale Variablen eingelesen werden oder nicht. Wenn nicht werden nun die globalen Variablenwerte verwendet.
	if (self->T_base == -99.0)
		// [Grad C] base temperature
		self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	if (self->GDDc_start == -99.0)
		// [Grad C] cirtical cum temp for bud burst
		self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	if (self->GDDc_end == -99.0)
		// [Grad C] critical cum temp for end of anthesis
		self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_end_crit",100.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850
	

	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN TSM DAILY.start_day_phenology");	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 01.01.
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM DAILY.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm
	return RET_SUCCESS;
}


int pollen_TSM_phenol_dev_daily(pollen *self)		//diese Funktion wird dann immer wieder in jedem Zeitschritt aufgerufen.
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	pollen_integrate_small_time_step_vars(self);

	if ((int)NewDay(pTi))
		{
			if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
				{
					self->start_GDD_int=1; 								//Entwicklungszustand 1
					self->GDD = 0.0;									//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
				}


			if (self->start_GDD_int>0)
				{
					self->GDD += MAX(self->weather.fTempAve - self->T_base,0.0);
					if (self->GDD >= self->GDDc_start)
						{
							self->start_GDD_int=2;						//Entwicklungszustand 2
							self->pollination=1;    					//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
						}
				}

			if (self->start_GDD_int==2)
				{
					if (self->GDD >= self->GDDc_end)
						{
							self->start_GDD_int=3;						//Entwicklungszustand 3
							self->pollination=0;    					//Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet
						}

				}


		}

	return RET_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//pollen_TSM_WC ist ein lineares Temperatursummen Modell (TSM) wobei zusätzlich der
//Bodenwassergehalt berücksichtigt wird.
//Problem: für die Variablen alpha und beta sind in der Literatur keine art- bzw.
//sortenspezifischen Werte hinterlegt... (fatchfaktoren...)
//
//nach Yuan W. et al.Ecol.Res. 2007 "Simulating phenolotical characteristics of two dominant
//grass species in a semi-arid steppe ecosystem"-> Model_2
//*******************************************************************************************************
//xpn->pPl->pRoot->fDepth 		Durchwurzelungstiefe										[mm]
//fRootDepth					== xpn->pPl->pRoot->fDepth
//self->GDD						grwoth degree days											[°C]
//self->start_GDD_int 			Anfangswert für grwoth degree days Integration				[°C]
//self->GDDc_start				kritischer Wert für GDD ab welchem Anthese beginnt			[°C]
//self->GDDc_end				kritischer Wert für GDD ab welchem Anthese endet			[°C]
//self->pollination				Freisetzung von Pollen (vor Anthese:0; nach Anthese: 1)		[-]
//self->T_base					Base temperature for integration of GDD						[°C]
//self->start_mday				day of month												[-]
//self->start_mon				month														[-]
//self->alpha					Art-/Sortenspezifischer Parameter für GDDberechnung mit WC	[°C mm mm-1]
//self->beta					Art-/Sortenspezifischer Parameter für GDDberechnung mit WC	[°C]
//iRootLayer					ite Wurzelschicht startend mit der 1.  Schicht...			[-]
//xpn->pSo->pSLayer->pNext		go to next deeper soil layer								[-]
//pSL 							== xpn->pSo->pSLayer->pNext
//rsm							relative soil moisture	(fraction)							[mm mm-1]
//pWL->fContAct					actual soil water content (fraction)						[mm mm-1]
//pSW->fContPWP					permanent wilting point (fraction)							[mm mm-1]
//pSW->fContFK					field capacity (fraction)									[mm mm-1]
//PTIME pTi = xpn->pTi			next time step												[-]
//S, S2							Strings for month day
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Phänologie wird EINMAL TÄGLICH aufgerufen und mit täglichen Durchschnittstemperaturen berechnet
int pollen_TSM_WC_DAILY_load(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->GDD,"pollen.GDD",0.0);


	self->GDD = 0.0; // [Grad C] Cumulative Temperature
	self->start_GDD_int = 0;
	self->pollination=0;

	// [Grad C] base temperature
	self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DAILY.t_base",-5.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	// [Grad C] cirtical cum temp for bud burst
	self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DAILY.gdd_start_crit",750.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 300
	// [Grad C] critical cum temp for end of anthesis
	self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DAILY.gdd_end_crit",850.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 600.0


	//hier wird geprüft ob bereits lokale Variablen eingelesen werden oder nicht. Wenn nicht werden nun die globalen Variablenwerte verwendet.
	if (self->T_base == -99.0)
		// [Grad C] base temperature
		self->T_base = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.t_base",3.5);  //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: -5.0
	if (self->GDDc_start == -99.0)
		// [Grad C] cirtical cum temp for bud burst
		self->GDDc_start = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_start_crit",750.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 750
	if (self->GDDc_end == -99.0)
		// [Grad C] critical cum temp for end of anthesis
		self->GDDc_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Global_Config.options.gdd_end_crit",100.0); //no calibration values were provided by Yuan et al. 2007 Ecol Res; CB Scheyern: 850
	

	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DAILY.start_day_phenology");	//no calibration values were provided by Yuan et al. 2007 Ecol Res. CB Scheyern; 01.01.
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN TSM WC DAILY.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}

	// [Grad C * mm/mm] variety specific parameter
	self->alpha = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DAILY.alpha",10.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res
	// [Grad C] variety specific parameter
	self->beta = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC DAILY.beta",10.0);	//no calibration values were provided by Yuan et al. 2007 Ecol Res


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm
	return RET_SUCCESS;
}



int pollen_TSM_WC_phenol_dev_daily(pollen *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	pollen_integrate_small_time_step_vars(self);

	double rsm; // relative soil moisture
	double fDepth; //mm
	double fRootDepth; // actual rooting depth
	PSLAYER pSL;
	PWLAYER pWL;
	PSWATER pSW;
	int iRootLayer; // actueal deepest rooted soil layer
	int i;

	if ((int)NewDay(pTi))
		{
			if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
				{
					self->start_GDD_int=1;											//Entwicklungszustand 1
					self->GDD = 0.0;												//Pollenfreisetzungsmodell ist deaktiviert, Knospen noch nicht geöfnnet
				}


			if (self->start_GDD_int>0)
				{
					self->GDD += MAX(self->weather.fTempAve-self->T_base,0.0);

					fDepth = 0.0;
					iRootLayer=0;
					fRootDepth = xpn->pPl->pRoot->fDepth;
					pSL = xpn->pSo->pSLayer->pNext;
					while(fDepth<=fRootDepth)
						{
							if (iRootLayer >= xpn->pSo->iLayers-1) break;
							fDepth += pSL->fThickness; //fThickness: mm
							iRootLayer++;
							pSL = pSL->pNext;
						}

					rsm=0.0;
					for (i=1,pWL = xpn->pWa->pWLayer->pNext,pSW=xpn->pSo->pSWater->pNext; i<iRootLayer; i++,pWL=pWL->pNext,pSW=pSW->pNext)
						{
							rsm += (pWL->fContAct - pSW->fContPWP) / (pSW->fContFK-pSW->fContPWP);
						}
					rsm /= iRootLayer-1;

					if (self->GDD >= self->GDDc_start - (self->alpha * rsm + self->beta))
						{
							self->start_GDD_int=2;									//Entwicklungszustand 2
							self->pollination=1;									//Pollenfreisetzungsmodell wird aktiviert, Knospen geöfnnet
						}

				}
			if (self->start_GDD_int>0)
				{
					if (self->GDD >= self->GDDc_end)
						{
							self->start_GDD_int=3;						//Entwicklungszustand 3
							self->pollination=0;    					//Pollenfreisetzungsmodell wird deaktiviert, Anthese beendet
						}
				}


		}

	return RET_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
////////////////////////////////////////////////////////////////////////////////////////////
//multiplikatives Modell für Betula pendula (mMBp)
//
//nach Häkkinen R. Tree Physiology 18, 707-712 "Effects of dormancy and environmental factors on
//timing of bud burst in Betula pendula" -> Hypothesis_1
//+++++++++++++"bud ontogenesis begins after bud dormancy"+++++++++++++++++++++++++++++++++++++++
//******************************************************************************************
//anthesis						Zeitraum der Anthese
//bud ontogenesis				Zeitraum der Knospenentwicklung
//bud dormancy					Zeitraum der Ruhe der Knospenentwicklung
//
//xpn->pPl->pRoot->fDepth	Durchwurzelungstiefe										[mm]
////////////////////////////////////////////////////////////////////////////////////////////

int pollen_MultiplikationModel_H1_load(pollen *self)  	//Alle Variablen werden deklariert, wenn die Funktion das erste Mal aufgerufen wird...
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//diese Variablen werden in der Expert-N Ausgabe im Terminal zusätzlich angezeigt...
	/////////////////////////////////////////////////////////////////////////////////////////
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU,"pollen.RU",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->SD,"pollen.SD",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU,"pollen.cum_RU",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_SD_dormancy,"pollen.cum_SD_dormancy",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_dormancy,"pollen.cum_RU_dormancy",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_ontogenesis,"pollen.cum_RU_ontogenesis",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_anthesis,"pollen.cum_RU_anthesis",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU_Dormancy,"pollen.RU_Dormancy",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU_Ontogenesis,"pollen.RU_Ontogenesis",0.0);

	self->RU_Dormancy = 0.0; // [ - ] relative units for bud dormancy stage
	self->RU_Ontogenesis = 0.0; // [ - ] relative units to compute bud ontogenesis
	self->start_RU_Dormancy_check = 0; // [ - ] integration of RU to compute bud dormancy started
	self->start_RU_Ontogenesis_int = 0; // [ - ] integration of RU to compute bud ontogenesis started
	self->start_RU_Anthesis_check = 0;
	self->pollination=0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//diese Variablen werden modellspezifisch eingelesen
	/////////////////////////////////////////////////////////////////////////////////////////
	// [Grad C] cirtical relative unit for end of bud dormancy
	self->SD_Dormancy_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H1.sd_dormancy_end_crit",740.0); //Scheyern CB: 1500.0//740.0 ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	// [Grad C] cirtical relative unit for end of anthesis
	self->RU_Ontogenesis_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H1.ru_ontogenesis_end_crit",3250.0); //Scheyern CB: 3250.0//3250.0 ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	// [Grad C] cirtical relative unit for end of anthesis -> pollination end
	self->RU_Anthesis_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H1.ru_anthesis_end_crit",1050.0); //Scheyern CB: 1050.0// gefittet für Scheyern mit Zeilgröße Pollenflugbeginn 17.4. und Ende 29.4.(laut DWD Phänologischer Kalender für Bayern)

	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN MM H1.start_day_phenology");	////Scheyern CB: 12.9.//12.9. ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN MM H1.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}

	/*// [Grad C * mm/mm] variety specific parameter
	self->alpha = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC.alpha",10.0);	//fiktive Werte --> Todo.
	// [Grad C] variety specific parameter
	self->beta = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC.beta",10.0);		//fiktive Werte --> Todo.


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm*/


	return RET_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
////////////////////////////////////////////////////////////////////////////////////////////
//multiplikatives Modell für Betula pendula (mMBp)
//
//nach Häkkinen R. Tree Physiology 18, 707-712 "Effects of dormancy and environmental factors on
//timing of bud burst in Betula pendula" -> Hypothesis_2
//+++++++++++++"bud ontogenesis already starts before bud dormancy has finnished"++++++++++
//******************************************************************************************
//anthesis						Zeitraum der Anthese
//bud ontogenesis				Zeitraum der Knospenentwicklung
//bud dormancy					Zeitraum der Ruhe der Knospenentwicklung
//
//xpn->pPl->pRoot->fDepth	Durchwurzelungstiefe										[mm]
////////////////////////////////////////////////////////////////////////////////////////////

int pollen_MultiplikationModel_H2_load(pollen *self)  	//Alle Variablen werden deklariert, wenn die Funktion das erste Mal aufgerufen wird...
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//diese Variablen werden in der Expert-N Ausgabe im Terminal zusätzlich angezeigt...
	/////////////////////////////////////////////////////////////////////////////////////////
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU,"pollen.RU",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->SD,"pollen.SD",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU,"pollen.cum_RU",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_SD_dormancy,"pollen.cum_SD_dormancy",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_dormancy,"pollen.cum_RU_dormancy",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_ontogenesis,"pollen.cum_RU_ontogenesis",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_anthesis,"pollen.cum_RU_anthesis",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU_Dormancy,"pollen.RU_Dormancy",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU_Ontogenesis,"pollen.RU_Ontogenesis",0.0);

	self->RU_Dormancy = 0.0; // [ - ] relative units for bud dormancy stage
	self->RU_Ontogenesis = 0.0; // [ - ] relative units to compute bud ontogenesis
	self->start_RU_Dormancy_check = 0; // [ - ] integration of RU to compute bud dormancy started
	self->start_RU_Ontogenesis_int = 0; // [ - ] integration of RU to compute bud ontogenesis started
	self->start_RU_Anthesis_check = 0;
	self->pollination=0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//diese Variablen werden modellspezifisch eingelesen
	/////////////////////////////////////////////////////////////////////////////////////////
	// [Grad C] cirtical relative unit for end of bud dormancy
	self->SD_Dormancy_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H2.sd_dormancy_end_crit",900.0); //Scheyern CB: 1920.0//900.0 ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	// [Grad C] cirtical relative unit for end of anthesis
	self->RU_Ontogenesis_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H2.ru_ontogenesis_end_crit",3380.0); //Scheyern CB: 3380.0//3380.0 ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	// [Grad C] cirtical relative unit for end of anthesis -> pollination end
	self->RU_Anthesis_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H2.ru_anthesis_end_crit",1050.0); //Scheyern CB: 1050.0// gefittet für Scheyern mit Zeilgröße Pollenflugbeginn 17.4. und Ende 29.4.(laut DWD Phänologischer Kalender für Bayern)

	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN MM H2.start_day_phenology");	////Scheyern CB: 11.9.//11.9. ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN MM H1.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}

	/*// [Grad C * mm/mm] variety specific parameter
	self->alpha = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC.alpha",10.0);	//fiktive Werte --> Todo.
	// [Grad C] variety specific parameter
	self->beta = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC.beta",10.0);		//fiktive Werte --> Todo.


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm*/


	return RET_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////
//Phänologie für Bestimmung des Blühbeginns und somit der Pollenflugvorhersage
////////////////////////////////////////////////////////////////////////////////////////////
//multiplikatives Modell für Betula pendula (mMBp)
//
//nach Häkkinen R. Tree Physiology 18, 707-712 "Effects of dormancy and environmental factors on
//timing of bud burst in Betula pendula" -> Hypothesis_3
//+++++++++++++"beginning of bud ontogenesis is fixed to a calender date"++++++++++
//******************************************************************************************
//anthesis						Zeitraum der Anthese
//bud ontogenesis				Zeitraum der Knospenentwicklung
//bud dormancy					Zeitraum der Ruhe der Knospenentwicklung
//
//xpn->pPl->pRoot->fDepth	Durchwurzelungstiefe										[mm]
////////////////////////////////////////////////////////////////////////////////////////////
int pollen_MultiplikationModel_H3_load(pollen *self)  	//Alle Variablen werden deklariert, wenn die Funktion das erste Mal aufgerufen wird...
{
	expertn_modul_base *xpn = &(self->parent);
	char *S,*S2,*T,*T2;
	self->weather.fTempMax=0.0;
	self->weather.fTempMin=0.0;
	self->weather.fTempAve=0.0;
	self->weather.fPotTraDay=0.0;
	self->weather.fEvapPotDay=0.0;
	self->weather.fPotETDay=0.0;
	self->weather.fActTraDay=0.0;
	self->weather.fDaylySolRad=0.0;
	self->weather.fTimeY_save=0.0;
	self->weather.iyear_save=0;
	self->__weather.fTempMax=0.0;
	self->__weather.fTempMin=0.0;
	self->__weather.fTempAve=0.0;
	self->__weather.fPotTraDay=0.0;
	self->__weather.fEvapPotDay=0.0;
	self->__weather.fPotETDay=0.0;
	self->__weather.fActTraDay=0.0;
	self->__weather.fDaylySolRad=0.0;
	self->__weather.fTimeY_save=0.0;
	self->__weather.iyear_save=0;

	////////////////////////////////////////////////////////////////////////////////////////
	//diese Variablen werden in der Expert-N Ausgabe im Terminal zusätzlich angezeigt...
	////////////////////////////////////////////////////////////////////////////////////////
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU,"pollen.RU",0.0);
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU,"pollen.cum_RU",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_ontogenesis,"pollen.cum_RU_ontogenesis",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->cum_RU_anthesis,"pollen.cum_RU_anthesis",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->RU_Ontogenesis,"pollen.RU_Ontogenesis",0.0);

	self->RU_Dormancy = 0.0; // [ - ] relative units for bud dormancy stage
	self->RU_Ontogenesis = 0.0; // [ - ] relative units to compute bud ontogenesis
	self->start_RU_Dormancy_check = 0; // [ - ] integration of RU to compute bud dormancy started
	self->start_RU_Ontogenesis_int = 0; // [ - ] integration of RU to compute bud ontogenesis started
	self->start_RU_Anthesis_check = 0; //
	self->pollination = 0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//diese Variablen werden modellspezifisch eingelesen
	/////////////////////////////////////////////////////////////////////////////////////////
	self->RU_Ontogenesis_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H3.ru_ontogenesis_end_crit",2610.0); //Scheyern CB: 2610.0//2610.0 ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	self->RU_Anthesis_c_end = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN MM H3.ru_anthesis_end_crit",1050.0);	//Scheyern CB: 1050.0// gefittet für Scheyern mit Zeilgröße Pollenflugbeginn 17.4. und Ende 29.4.(laut DWD Phänologischer Kalender für Bayern)

	S = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN MM H3.start_day_phenology");	////Scheyern CB: 11.9.//11.9. ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	if (S==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN MM H3.start_day_phenology not set!");
			self->start_mday = 1;
			self->start_mon = 1;
		}
	else
		{
			S2 = malloc(strlen(S)+1);
			getStringColumn(S2,S,".",0);
			self->start_mday=atoi(S2);
			getStringColumn(S2,S,".",1);
			self->start_mon=atoi(S2);
			free(S2);
		}

	T = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.POLLEN MM H3.start_day_ontogenesis");	////Scheyern CB: 21.3.//21.3. ist originalwert aus Häkkinen R. Tree Physiology 18, 707-712
	if (T==NULL)
		{
			PRINT_ERROR_ID(xpn,"Config.POLLEN MM H1.start_day_ontogenesis not set!");
			self->start_ont_mday = 1;
			self->start_ont_mon = 1;
		}
	else
		{
			T2 = malloc(strlen(T)+1);
			getStringColumn(T2,T,".",0);
			self->start_ont_mday=atoi(T2);
			getStringColumn(T2,T,".",1);
			self->start_ont_mon=atoi(T2);
			free(T2);
		}

	/*// [Grad C * mm/mm] variety specific parameter
	self->alpha = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC.alpha",10.0);	//fiktive Werte
	// [Grad C] variety specific parameter
	self->beta = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.POLLEN TSM WC.beta",10.0);		//fiktive Werte


	//ToDo: root depth anders bestimmen:

	xpn->pPl->pRoot->fDepth = 200.0; // mm*/


	return RET_SUCCESS;
}

/////////////////////////////////
//Hypothese 1:nach Häkkinen R. Tree Physiology 18, 707-712 "Effects of dormancy and environmental factors on
//timing of bud burst in Betula pendula" -> Hypothesis_1 +++++++++++++"bud ontogenesis begins after bud dormancy"++++++++++
///////////////////////////////
int pollen_MultiplikationModel_H1(pollen *self)  				//das ist dann die Funktion, die in jedem Zeitschritt ausgeführt wird
{
	//double bud_ontogenesis_begins(pollen *self);
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double w_D;
	double w_O;
	double w_A;
	double w_L;
	double g_0;
	double g_d;
	double dt;

	pollen_integrate_small_time_step_vars(self);

	if ((int)NewDay(pTi))

		{
			dt = 1.0; // weil Funktion in jedem Zeitschritt aufgerufen wird
			w_D = check_bud_dormancy_H1(self); //Funktionsaufruf zur Dormanz vor der Knospenentwicklung
			w_L = check_fixed_ontogenesis_date_H1_H2(self); //Funktionsaufruf bezüglich eines fixen Startdatums für Knospenontogenese
			g_0 = Temp_dependence_of_ontogenesis(self,xpn->pCl->pWeather->fTempAir); //Funktionsaufruf bezüglich der Temperaturabhängigkeit der Ontogenese (conversion in relative units (RU)).
			g_d = Temp_dependence_of_dormancy(self,xpn->pCl->pWeather->fTempAir); //Funktionsaufruf bezüglich der Temperaturabhängigkeit der Dormanz der Knospe

			w_O = check_bud_ontogenesis(self);
			w_A = check_anthesis(self);
			if(w_A == 0.0)
				{
					self->pollination = 0.0;
				}
			if(w_A == 1.0)
				{
					self->pollination = 1.0;
				}

			self->cum_RU += g_0*dt;							//RU seit Datierung des Beginns der Phänologie (Datum)
			self->cum_SD_dormancy += w_D *g_d*dt;							//cummulative SD für Budddormancy
			self->cum_RU_dormancy += (w_D * w_L * g_0)*dt;  //Relative Units der Dormanz
			self->cum_RU_ontogenesis += (w_O*g_0)*dt;		//Relative Units der Ontogenese
			self->cum_RU_anthesis += (w_A*g_0)*dt;  			//Relative Units der Anthese
		}
	return RET_SUCCESS;
}

///////////////////////////////
//Hypothese 2:nach Häkkinen R. Tree Physiology 18, 707-712 "Effects of dormancy and environmental factors on
//timing of bud burst in Betula pendula" -> Hypothesis_2 +++++++++++++"bud ontogenesis already starts before bud dormancy has finnished"++++++++++
///////////////////////////////
int pollen_MultiplikationModel_H2(pollen *self)  				//das ist dann die Funktion, die in jedem Zeitschritt ausgeführt wird
{
	//double bud_ontogenesis_begins(pollen *self);
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double w_D;
	double w_O;
	double w_A;
	double w_L;
	double g_d;
	double g_0;
	double dt;

	pollen_integrate_small_time_step_vars(self);

	if ((int)NewDay(pTi))

		{
			dt = 1.0; // weil Funktion in jedem Zeitschritt aufgerufen wird
			w_D = check_bud_dormancy_H2(self); //Funktionsaufruf zur Dormanz vor der Knospenentwicklung
			w_L = check_fixed_ontogenesis_date_H1_H2(self); //Funktionsaufruf bezüglich eines fixen Startdatums für Knospenontogenese
			g_0 = Temp_dependence_of_ontogenesis(self,xpn->pCl->pWeather->fTempAir); //Funktionsaufruf bezüglich der Temperaturabhängigkeit der Ontogenese (conversion in relative units (RU)).
			g_d = Temp_dependence_of_dormancy(self,xpn->pCl->pWeather->fTempAir); //Funktionsaufruf bezüglich der Temperaturabhängigkeit der Dormanz der Knospe

			w_O = check_bud_ontogenesis_H2(self);
			w_A = check_anthesis(self);


			if(w_A == 0.0)
				{
					self->pollination = 0.0;
				}
			if(w_A == 1.0)
				{
					self->pollination = 1.0;
				}

			self->cum_RU += g_0*dt;							//RU seit Datierung des Beginns der Phänologie (Datum)
			self->cum_SD_dormancy += w_D*g_d*dt;				//cummulative SD für Budddormancy
			self->cum_RU_dormancy += (w_D * w_L * g_d)*dt;  //Relative Units der Dormanz
			self->cum_RU_ontogenesis += (w_O*g_0)*dt;		//Relative Units der Ontogenese
			self->cum_RU_anthesis += (w_A*g_0)*dt;  		//Relative Units der Anthese
		}

	return RET_SUCCESS;
}

//////////////////////////////////////////
//Hypothese 3:nach Häkkinen R. Tree Physiology 18, 707-712 "Effects of dormancy and environmental factors on
//timing of bud burst in Betula pendula" -> Hypothesis_3 +++++++++++++"beginning of bud ontogenesis is fixed to a calender date"++++++++++
//////////////////////////////////////////
int pollen_MultiplikationModel_H3(pollen *self)  				//das ist dann die Funktion, die in jedem Zeitschritt ausgeführt wird
{
	//double bud_ontogenesis_begins(pollen *self);
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double w_D;
	double w_O;
	double w_A;
	double w_L;
	double g_0;
	double g_d;
	double dt;

	pollen_integrate_small_time_step_vars(self);

	if ((int)NewDay(pTi))

		{
			dt = 1.0; // weil Funktion in jedem Zeitschritt aufgerufen wird
			w_D = check_bud_dormancy_H3(self); //Funktionsaufruf zur Dormanz vor der Knospenentwicklung
			w_L = check_fixed_ontogenesis_date_H3(self); //Funktionsaufruf bezüglich des Tages des Starts der Knospenentwicklung
			g_0 = Temp_dependence_of_ontogenesis(self,xpn->pCl->pWeather->fTempAir); //Funktionsaufruf bezüglich der Temperaturabhängigkeit der Ontogenese (conversion in relative units (RU)).
			g_d = Temp_dependence_of_dormancy(self,xpn->pCl->pWeather->fTempAir); //Funktionsaufruf bezüglich der Temperaturabhängigkeit der Dormanz der Knospe

			w_O = check_bud_ontogenesis_H3(self);			//1.0 wird zurückgegeben, denn wenn w_L 1.0 ist geht die Knospenentwicklung ja los...
			w_A = check_anthesis(self);

			if(w_A == 0.0)
				{
					self->pollination = 0.0;
				}
			if(w_A == 1.0)
				{
					self->pollination = 1.0;
				}

			self->cum_RU+=g_0*dt;							//RU seit Datierung des Beginns der Phänologie (Datum)
			self->cum_SD_dormancy+=w_D*g_d*dt;							//cummulative SD für Budddormancy
			//self->cum_RU_dormancy += (w_D * w_L * g_0)*dt;  //Relative Units der Dormanz
			self->cum_RU_ontogenesis += (w_D*w_O*w_L*g_0)*dt;		//Relative Units der Ontogenese
			self->cum_RU_anthesis += (w_A*g_0)*dt;  			//Relative Units der Anthese
		}
	return RET_SUCCESS;
}


double check_bud_ontogenesis(pollen *self)
{
	//expertn_modul_base *xpn = &(self->parent);	//Deklaration der Variablen
	double ret;

	ret = 0.0;

	if (self->start_RU_Dormancy_check==4)			//Wenn Entwicklungsstadium 3 der Dormanz erreicht ist (Dormanz beendet), dann startet Knospenontogenese.
		{
			ret = 1.0;
		}
	if (self->cum_RU_ontogenesis >= self->RU_Ontogenesis_c_end)
		{
			ret = 0.0;
			self->start_RU_Anthesis_check = 1;		//Pollination starts
		}

	return ret;
}

double check_bud_ontogenesis_H2(pollen *self)
{
	//expertn_modul_base *xpn = &(self->parent);	//Deklaration der Variablen
	double ret;

	ret = 0.0;

	if (self->cum_SD_dormancy <= (0.5*self->SD_Dormancy_c_end))			//Wenn Entwicklungsstadium 3 der Dormanz erreicht ist (Dormanz beendet), dann startet Knospenontogenese.
		{
			ret = 0.0;
		}
	else if(self->cum_SD_dormancy >= self->SD_Dormancy_c_end)
		{
			ret = 1.0;
		}
	else if ((self->cum_SD_dormancy > 0.5*self->SD_Dormancy_c_end) && (self->cum_SD_dormancy < self->SD_Dormancy_c_end))
		{
			ret = (self->cum_SD_dormancy-(0.5*self->SD_Dormancy_c_end))/(0.5*self->SD_Dormancy_c_end);
		}

	if (self->cum_RU_ontogenesis >= self->RU_Ontogenesis_c_end)
		{
			ret = 0.0;
			self->start_RU_Anthesis_check = 1;		//Pollination starts
		}

	return ret;
}

double check_bud_ontogenesis_H3(pollen *self)
{
	//expertn_modul_base *xpn = &(self->parent);	//Deklaration der Variablen
	double ret;

	ret = 1.0;

	if (self->cum_RU_ontogenesis >= self->RU_Ontogenesis_c_end)
		{
			ret = 0.0;
			self->start_RU_Anthesis_check = 1;		//Pollination starts
		}

	return ret;
}

double check_anthesis(pollen *self)
{
	double ant;

	ant = 0.0;

	if(self->start_RU_Anthesis_check == 1)
		{
			ant = 1.0;
		}

	if(self->start_RU_Anthesis_check == 1 && self->cum_RU_anthesis > self->RU_Anthesis_c_end)
		{
			ant = 0.0;
			self->start_RU_Anthesis_check = 0;
		}

	return ant;
}


double check_bud_dormancy_H1(pollen *self)  				//Berechnung der Temperatursummen als Subfunktion
{
	expertn_modul_base *xpn = &(self->parent);		//Deklaration der Variablen
	PTIME pTi = xpn->pTi;
	double Dormance_in_progress;


	Dormance_in_progress = 0.0;



	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_RU_Dormancy_check=1; 								//Dormanz beginnt: Zustand 1
			//Development_in_progress = 1.0;
			Dormance_in_progress = 1.0;
		}


	if (self->start_RU_Dormancy_check>0)							// solange start_RU_Dormancy=1 wird die Temperatursumme aufaddiert
		{

			if (self->cum_SD_dormancy < self->RU_Dormancy_c_start)
				{
					Dormance_in_progress = 0.0;
				}

			if (self->cum_SD_dormancy >= self->RU_Dormancy_c_start)
				{
					Dormance_in_progress = 1.0;
					self->start_RU_Dormancy_check=3;				//Dormanz endet: Zustand 2
				}
		}

	if (self->start_RU_Dormancy_check==3)
		{
			if (self->cum_SD_dormancy >= self->SD_Dormancy_c_end)
				{
					self->start_RU_Dormancy_check=4;						//RUsums für Dormanz werden nicht weiter berechnet: Zustand 3
					self->RU_Dormancy = 0.0;
					Dormance_in_progress = 0.0;
				}

		}

	if (self->start_RU_Dormancy_check > 4)
		{
			Dormance_in_progress = 0.0;
		}


	return Dormance_in_progress;

}

double check_bud_dormancy_H2(pollen *self)  				//Berechnung der Temperatursummen als Subfunktion
{
	expertn_modul_base *xpn = &(self->parent);		//Deklaration der Variablen
	PTIME pTi = xpn->pTi;
	double Dormance_in_progress;


	Dormance_in_progress = 0.0;


	if (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_mon,self->start_mday) == 0)
		{
			self->start_RU_Dormancy_check=1; 								//Dormanz beginnt: Zustand 1
			//Development_in_progress = 1.0;
			Dormance_in_progress = 1.0;
		}

	if (self->start_RU_Dormancy_check>0)							// solange start_RU_Dormancy=1 wird die Temperatursumme aufaddiert
		{

			if (self->cum_SD_dormancy < self->RU_Dormancy_c_start)
				{
					Dormance_in_progress = 0.0;
				}

			if (self->cum_SD_dormancy >= self->RU_Dormancy_c_start)
				{
					Dormance_in_progress = 1.0;
					self->start_RU_Dormancy_check=3;				//Dormanz endet: Zustand 2
				}
		}

	if (self->start_RU_Dormancy_check==3)
		{
			if (self->cum_SD_dormancy >= self->SD_Dormancy_c_end)
				{
					self->start_RU_Dormancy_check=4;						//RUsums für Dormanz werden nicht weiter berechnet: Zustand 3
					self->RU_Dormancy = 0.0;
					Dormance_in_progress = 0.0;
				}

		}

	if (self->start_RU_Dormancy_check > 4)
		{
			Dormance_in_progress = 0.0;
		}

	return Dormance_in_progress;

}

double check_bud_dormancy_H3(pollen *self)  				//Keine Dormanz, da Knospenentwicklung ab bestimmtem Tag (z.B. 21. März)
{
	return 1.0;
}



double check_fixed_ontogenesis_date_H1_H2(pollen* self)  				//Prüfung der Lichtqualität zum Zeitpunkt der potentiellen Pollenfreisetzung
{
	return 1.0;
}

double check_fixed_ontogenesis_date_H3(pollen* self)  				//Prüfung der Lichtqualität zum Zeitpunkt der potentiellen Pollenfreisetzung
{

	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;

	double bud_ontogenesis_in_progress;

	bud_ontogenesis_in_progress = 0.0;



	if (self->start_RU_Ontogenesis_int == 1 || xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pTi->pSimTime->year,self->start_ont_mon,self->start_ont_mday) == 0)
		{
			self->start_RU_Ontogenesis_int=1; 								//Knospenontogenese beginnt: Zustand 1
			//Development_in_progress = 1.0;
			bud_ontogenesis_in_progress = 1.0;
		}

	return bud_ontogenesis_in_progress;

}

double Temp_dependence_of_ontogenesis(pollen* self,double temperature)
{
	//double RU;//relative units [h-1] relatives Wachstum pro Stunde
	//expertn_modul_base *xpn = &(self->parent);

	{
		self->RU = 27.245/(1.0+exp(-(temperature-18.298)/5.301));		//ToDo: Average Temperature oder aktuelle Temperatur???
		//Funktion abgeleitet aus Häkkinen et al.1998 Figure 3 und gefittet mit
		//SigmaPlot_12.0 (a=27.245; b=5.301; x0=18.298) vgl: Sarvas 1972:Investigations on annual cycle development
		//of forest trees. Active period. Commun. Inst. For, Fenn. 76:1-110.
		self->RU *= 24.0;															//*24 ist pro Tag und *dt ist dann pro Zeitschritt
	}
	return self->RU;
}

double Temp_dependence_of_dormancy(pollen* self,double temperature)
{
	//double RU;//relative units [h-1] relatives Wachstum pro Stunde
	//expertn_modul_base *xpn = &(self->parent);

	{
		if(self->cum_SD_dormancy <= self->SD_Dormancy_c_end)
			{
				if(temperature<=-3.5||temperature>=10.5)
					{
						self->SD = 0.0;
					}

				if(temperature>-3.5 && temperature<=3.5)
					{
						self->SD=1/7*temperature+0.5;
					}

				if(temperature>3.5 && temperature<10.5)
					{
						self->SD=-1/7*temperature+1.5;
					}

				self->SD *= 24.0;
			}
		if(self->cum_SD_dormancy > self->SD_Dormancy_c_end)
			{
				self->SD = 0.0;
			}															//*24 ist pro Tag und *dt ist dann pro Zeitschritt
	}
	return self->SD;
}
