//      expertn_database.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//

#ifndef XPN_WITHOUT_DATABASE
#include "expertn_database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <math.h>
#include "database_help_func.h"
#include "expertn_standard_ini_macros.h"



#ifdef USE_OPENMP
#include <omp.h>
#endif

G_DEFINE_TYPE(expertn_database, expertn_database, EXPERTN_MODUL_BASE_TYPE);
static void expertn_database_class_init(expertn_databaseClass *klass) {}
static void expertn_database_init(expertn_database *self) {
	self->Tairmax=0.0;
	self->Tairmin=0.0;
	self->meantemp=0.0;
	self->TairLastDayMax=0.0;
	self->TairNextDayMin=0.0;
	self->OldDate_day = 0;
	self->OldDate_month = 0;
	self->OldDate_year = 0;
	self->wetter_data_size=0;
	self->wetter_data=NULL;
}

typedef struct {
	GDate *Termin;
	//int Schichtnummer;
	double Schichtdicke;
	double NitratgehaltBoden;
	double AmmoniumgehaltBoden;
	double WassergehaltBoden;
	char* WassergehaltDimension;
	double Bodentemperatur;
	int Wurzelichteverteilung;
} STRUC_STARTVAlUE;


/*Calculate Net Radiation from Solar Radiation [MJ/m2*d]*/
double expertn_database_get_Rn(expertn_modul_base *xpn) {
	/* Abbreviations */

	/* Variables */
	double dt;
	double latitude;
	double soldec;
	int iJulianDay;
	double elevation;
	double t, b, Sc;
	double h, T, RH;
	double w, w1, w2, ws;
	double dr;
	double Ra_h;
	double Rs, Rs0, Rs0_noon, Ra_noon;
	double ea, es;
	double Rns, Rnl, Rn;
	double fcd, N;
	double ftime_day;

	dt = xpn->pTi->pTimeStep->fAct;
	elevation = xpn->pLo->pFarm->fAltitude;
	iJulianDay  = xpn->pTi->pSimTime->fTimeY;   // actual julian day
	ftime_day = xpn->pTi->pSimTime->fTimeY/(24.0*60.0*60.0);     // [s]->[day]
	h = (double)(((double)((double)ftime_day - (int)ftime_day))*24.0);


	T = xpn->pCl->pWeather->fTempAir;
	RH = xpn->pCl->pWeather->fHumidity;
	Rs = xpn->pCl->pWeather->fSolRad;

	//Extraterrestrial radiation for hourly periods (Ra )
	latitude = xpn->pLo->pFarm->fLatitude*M_PI/180.0;       // eq. 49, [rad]
	soldec = 0.409*sin(2.0*M_PI/365.0*iJulianDay-1.39);     // solar declination [radians]
	ws      = acos(-tan(soldec)*tan(latitude));             // sunset hour angle [radians]
	N   = 24.0/M_PI*ws;                                     // max. possible sunshine duration [hours]

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



	Ra_h = 12.0/M_PI * 4.92 *dr*((w2-w1)*sin(latitude)*sin(soldec)+cos(latitude)*cos(soldec)*(sin(w2)-sin(w1)));        // [MJ/m2*dt], orig eq. 48



	//self->Rs = (0.25+0.5*(self->n/self->N))*self->Ra;

	//Clear-sky solar radiation Rs0
	Rs0 = (0.75+2.0e-5*elevation)*Ra_h; // [MJ/m2*dt]

	//cloudiness factor
	if(Rs0>0.0) {
		fcd = 1.35*Rs/Rs0-0.35; //eqn 45
	} else {
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
		Rs0_noon = (0.75+2.0e-5*elevation)*Ra_noon;
		fcd = 1.35*Rs/Rs0_noon-0.35;
	}
	fcd = MAX(fcd,0.05);
	fcd = MIN(fcd,1.0);

	//Net Solar or Net Short-Wave Radiation (Rns )
	Rns = Rs - 0.23 * Rs; //eqn 43, [MJ/m2*d], with 0.23 = canopy reflection coefficient

	//Net Long-Wave Radiation (Rnl )
	es = 0.6108*exp(17.27*T/(T+237.3));         //eqn. 37, Saturation Vapor Pressure (es)
	ea = RH/100.0 * es;                 //Method nr. 3 in table 4, eqn 37, 41, Actual Vapor Pressure (ea), using rel. humidity and temperature at the hour

	Rnl = 2.042*1e-10*fcd*(0.34-0.14*sqrt(ea))*pow((T+273.16), 4.0); //eqn 44, [MJ/m2*dt]

	//Net Radiation (Rn )
	Rn = Rns - Rnl; //[MJ/m2*dt]
	if(Rn < 0.0) {
		Rn = 0.0;
	}

	return Rn;
}

void expertn_database_readStartValues(expertn_database *self) {
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *sql;
	GdaDataModel *data_model;
	int i,i2,i3;
	int countHoriz; // Anzahl der Horizonte
	double Schichtdicke;
	int *Anzahl_Schichten_pro_Horizont;
	STRUC_STARTVAlUE* start;

	PSLAYER pSLayer;
	PSWATER pSWater;
	PCLAYER pCLayer;
	PWLAYER pWLayer;
	PHLAYER pHLayer;



	//Anzahl der Horizonte:
	sql = g_strdup_printf("SELECT COUNT(*) FROM BewegungStartwerte WHERE AutoTeilschlag = %d  ORDER BY Schichtnummer",self->iAutoTeilSchlag);
	data_model = db_select(self->cnc,self->parser,sql);
	countHoriz=db_get_int(data_model,0);
	g_object_unref (data_model);

	if (countHoriz<1) {
		PRINT_ERROR("In Startvalue Table: No Soil Layers found");
		exit(1);
	}
	start = (STRUC_STARTVAlUE*)g_malloc(sizeof(STRUC_STARTVAlUE)*countHoriz);

	sql = g_strdup_printf("SELECT *  FROM BewegungStartwerte WHERE AutoTeilschlag = %d ORDER BY Schichtnummer",self->iAutoTeilSchlag);
	data_model = db_select(self->cnc,self->parser,sql);

	for (i=0; i<countHoriz; i++) {
		start[i].Termin = db_get_date_row(data_model,1,i);
		start[i].Schichtdicke = db_get_double_row(data_model,3,i); // Dicke des Horizontes
		start[i].NitratgehaltBoden = db_get_double_row(data_model,4,i);
		start[i].AmmoniumgehaltBoden = db_get_double_row(data_model,5,i);
		start[i].WassergehaltBoden = db_get_double_row(data_model,6,i)/100.0; // da dim = %
		start[i].WassergehaltDimension = db_get_string_row(data_model,7,i);
		start[i].Bodentemperatur = db_get_double_row(data_model,8,i);
		//start[i].Wurzelichteverteilung = db_get_int_row(data_model,9,i); //existiert zwar als eintrag, wird aber nie benutzt
	}

	g_object_unref (data_model);


	// Jetzt in die ExpertN strukturen schreiben

	/*  for (i=0; i<MAX_DEPTH_PER_MEASURE; i++)
	      {
	          if (i < countHoriz)
	              {
	                  xpn->pMa->Startvalue->lDate = conv_gdate_to_xpn_date(start[i].Termin);
	                  xpn->pMa->Startvalue->iDay = convert_date_to_days(xpn->pMa->Startvalue->lDate);
	                  xpn->pMa->Startvalue->afThickness[i] = start[i].Schichtdicke*10.0; // cm -> mm
	                  xpn->pMa->Startvalue->afNO3N[i] = start[i].NitratgehaltBoden;
	                  xpn->pMa->Startvalue->afNH4N[i] = start[i].AmmoniumgehaltBoden;
	                  xpn->pMa->Startvalue->afH2O[i] = start[i].WassergehaltBoden;
	                  xpn->pMa->Startvalue->afTemp[i] = start[i].Bodentemperatur;
	                  xpn->pMa->Startvalue->afRootDens[i] = 0.0;
	                  //printf ("Bodentemp:%f\n",start[i].Bodentemperatur);
	              }
	          else
	              {
	                  xpn->pMa->Startvalue->lDate = 0;
	                  xpn->pMa->Startvalue->iDay = 0;
	                  xpn->pMa->Startvalue->afThickness[i] = 0.0;
	                  xpn->pMa->Startvalue->afNO3N[i] = 0.0;
	                  xpn->pMa->Startvalue->afNH4N[i] = 0.0;
	                  xpn->pMa->Startvalue->afH2O[i] = 0.0;
	                  xpn->pMa->Startvalue->afTemp[i] = 0.0;
	                  xpn->pMa->Startvalue->afRootDens[i] = 0.0;
	              }
	      }*/

	i=0;
	Schichtdicke=0.0;
	i3 = 0;

	Anzahl_Schichten_pro_Horizont = g_malloc0_n(countHoriz,sizeof(int));
	// Anzahl der Schichten pro Horizont bestimmen:

	for (pSLayer=xpn->pSo->pSLayer,pHLayer = xpn->pHe->pHLayer,pWLayer=xpn->pWa->pWLayer,pSWater=xpn->pSo->pSWater,pCLayer=xpn->pCh->pCLayer,i2=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pCLayer=pCLayer->pNext,pWLayer=pWLayer->pNext,pHLayer=pHLayer->pNext,i2++) {
		if ((i2!=0) && (i2!=xpn->pSo->iLayers-1)) {

			i3++;
			Schichtdicke+=pSLayer->fThickness/10.0;
			if (Schichtdicke>=start[i].Schichtdicke) {
				Schichtdicke=0.0;
				Anzahl_Schichten_pro_Horizont[i] = i3;
				i3 = 0;
				i++;
			}
		}
	}



	i=0;
	Schichtdicke=0.0;
	for (pSLayer=xpn->pSo->pSLayer,pHLayer = xpn->pHe->pHLayer,pWLayer=xpn->pWa->pWLayer,pSWater=xpn->pSo->pSWater,pCLayer=xpn->pCh->pCLayer,i2=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pCLayer=pCLayer->pNext,pWLayer=pWLayer->pNext,pHLayer=pHLayer->pNext,i2++) {
		if ((i2!=0) && (i2!=xpn->pSo->iLayers-1)) {
			// Thermin wird ignoriert (das sollte der user einfach richtig einstellen)
			pCLayer->fNO3N = start[i].NitratgehaltBoden/(double)Anzahl_Schichten_pro_Horizont[i];
			pCLayer->fNH4N = start[i].AmmoniumgehaltBoden/(double)Anzahl_Schichten_pro_Horizont[i];
			pWLayer->fContAct = start[i].WassergehaltBoden;
			pHLayer->fSoilTemp = start[i].Bodentemperatur;
			pHLayer->fSoilTempOld = start[i].Bodentemperatur;
			//pSLayer->fThickness = start[i].Schichtdicke*10.0; // cm -> mm
			// alles in die Strukur pMa-Startvalue
			Schichtdicke+=pSLayer->fThickness/10.0;
			if (Schichtdicke>=start[i].Schichtdicke) {
				Schichtdicke=0.0;
				i++;
			}
		}
		if (i2==xpn->pSo->iLayers-1) {
			pCLayer->fNO3N = start[i-1].NitratgehaltBoden/(double)Anzahl_Schichten_pro_Horizont[i-1];
			pCLayer->fNH4N = start[i-1].AmmoniumgehaltBoden/(double)Anzahl_Schichten_pro_Horizont[i-1];
			pWLayer->fContAct = start[i-1].WassergehaltBoden;
			pHLayer->fSoilTemp = start[i-1].Bodentemperatur;
			pHLayer->fSoilTempOld = start[i-1].Bodentemperatur;
		}
		if (i2==0) {
			pCLayer->fNO3N = start[0].NitratgehaltBoden/(double)Anzahl_Schichten_pro_Horizont[0];
			pCLayer->fNH4N = start[0].AmmoniumgehaltBoden/(double)Anzahl_Schichten_pro_Horizont[0];
			pWLayer->fContAct = start[0].WassergehaltBoden;
			pHLayer->fSoilTemp = start[0].Bodentemperatur;
			pHLayer->fSoilTempOld = start[0].Bodentemperatur;
		}
	}

	for (i=0; i<countHoriz; i++) {
		if (start[i].Termin!=NULL) g_free(start[i].Termin);
		if (start[i].WassergehaltDimension!=NULL) g_free(start[i].WassergehaltDimension);
	}
	g_free(Anzahl_Schichten_pro_Horizont);
	g_free(start);
}

void expertn_database_readObjectProject(expertn_database *self,GDate *global_start_date,GDate *global_end_date) {
	enum Projekt {
		ProjektName=0,
		AutoTeilschlag,
		StartDate,
		StopDate,
		FarmNo,
		FarmName,
		Year,
		SchlagNo,
		SchlagName,
		TeilschlagNo,
		TeilschlagName,
	};
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *sql;
	GdaDataModel *data_model;
	GDate *act_date;
	//const GValue *val;

	sql = g_strdup_printf("SELECT * FROM Projekt WHERE Projektname = '%s'",xpn->pLo->pTile->ProjectName);

	data_model = db_select(self->cnc,self->parser,sql);

	//val = gda_data_model_get_typed_value_at(data_model,AutoTeilschlag,0,G_TYPE_LONG,FALSE,NULL);
	// ProjektName
	// AutoTeilschlag
	self->iAutoTeilSchlag = db_get_int(data_model,AutoTeilschlag);

	if (global_start_date==NULL) {
		act_date = db_get_date(data_model,2);
	} else {
		act_date = g_date_new_dmy(g_date_get_day(global_start_date),g_date_get_month(global_start_date),g_date_get_year(global_start_date) );
	}
	if (!xpn->pXSys->extern_time_management) {
	xpn_time_conv_gdate_to_xpn_time(act_date,&(xpn->pTi->pSimTime->iStart_year),&(xpn->pTi->pSimTime->fStart_TimeY));
	}
	G_FREE_DATE_IF_NOT_0(act_date);

	if (global_end_date==NULL) {
		act_date = db_get_date(data_model,3);
	} else {
		act_date = g_date_new_dmy(g_date_get_day(global_end_date),g_date_get_month(global_end_date),g_date_get_year(global_end_date) );
	}


	if (!xpn->pXSys->extern_time_management) {
		xpn_time_conv_gdate_to_xpn_time(act_date,&(xpn->pTi->pSimTime->iStop_year),&(xpn->pTi->pSimTime->fStop_TimeY));
		xpn->pTi->pSimTime->fSimDuration = xpn_time_get_number_of_days(xpn->pTi->pSimTime->iStart_year, xpn->pTi->pSimTime->fStart_TimeY,xpn->pTi->pSimTime->iStop_year, xpn->pTi->pSimTime->fStop_TimeY);		



		self->AnzahlderTage=expertn_database_help_func_Time_Message_and_get_number_of_days(xpn);
	}

	G_FREE_DATE_IF_NOT_0(act_date);


	// StartDate
	// StopDate
	// Year
	// FarmNo
	//self->dFarmNo
	xpn->pLo->pFarm->lFarmNumber = (int)db_get_double(data_model,FarmNo);
	// FarmName:
	//self->FarmName = g_value_get_string (gda_data_model_get_typed_value_at(data_model,FarmName,0,G_TYPE_STRING,FALSE,NULL));
	// SchlagNo
	self->SchlagNo = db_get_string(data_model,SchlagNo);
	// SchlagName
	self->SchlagName = db_get_string(data_model,SchlagName);
	// TeilschlagNo
	self->TeilschlagNo = db_get_string(data_model,TeilschlagNo);
	// TeilschlagName
	self->TeilschlagName = db_get_string(data_model,TeilschlagName);
	g_object_unref (data_model);

	if (!xpn->pXSys->extern_time_management) {
		sql = g_strdup_printf("SELECT * FROM Stammbetrieb WHERE Betriebsnummer = ("
		                      "SELECT Betriebsnummer FROM StammJahr WHERE AutoJahr = ("
		                      "SELECT AutoJahr FROM StammSchlag WHERE StammSchlag.AutoSchlag =("
		                      "SELECT AutoSchlag FrOM StammTeilschlag WHERE AutoTeilschlag = %d )))"
		                      ,self->iAutoTeilSchlag);

		data_model = db_select(self->cnc,self->parser,sql);



		xpn->pLo->pFarm->lFarmNumber = (int)g_value_get_double (gda_data_model_get_typed_value_at(data_model,0,0,G_TYPE_DOUBLE,FALSE,NULL));
		//xpn->pLo->pFarm->iFieldNumbers =
		//xpn->pLo->pFarm->acFamilyName = g_value_get_string (gda_data_model_get_typed_value_at(data_model,1,0,G_TYPE_STRING,FALSE,NULL));
		xpn->pLo->pFarm->fLongitude =   (double)g_value_get_double (gda_data_model_get_typed_value_at(data_model,2,0,G_TYPE_DOUBLE,FALSE,NULL));
		xpn->pLo->pFarm->fLatitude  =   (double)g_value_get_double (gda_data_model_get_typed_value_at(data_model,3,0,G_TYPE_DOUBLE,FALSE,NULL));
		//xpn->pLo->pFarm->acFamilyName = g_value_get_string (gda_data_model_get_typed_value_at(data_model,4,0,G_TYPE_STRING,FALSE,NULL));
		//xpn->pLo->pFarm->acName = g_value_get_string (gda_data_model_get_typed_value_at(data_model,5,0,G_TYPE_STRING,FALSE,NULL));

		g_object_unref (data_model);

	}

}

void expertn_database_readObjectLocation(expertn_database *self) {
	enum StammTeilschlag {
		AutoSchlag =0,
		AutoTeilschlag,
		Teilschlagnummer,
		Teilschlagname,
		Teilschlaggroesse,
		NameBodenprofil,
		NameWetterStation,
		NameKonfigurationsDatei,
		NameModellparamterDatei,
		Hoehenlage,
		Hangneigung,
		Exposition,
		Gebietsnummer,
	};
	//expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *sql;
	GdaDataModel *data_model;
	char *S;
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);

	sql = g_strdup_printf("SELECT * FROM StammTeilschlag WHERE StammTeilschlag.AutoTeilschlag = %d",self->iAutoTeilSchlag);
	data_model = db_select(self->cnc,self->parser,sql);
	// Die meisten Daten sind hier nicht wichtig

	self->NameBodenProfil = db_get_string(data_model,NameBodenprofil);

	S = g_strdup_printf("Bodenprofile: %s\n",self->NameBodenProfil);
	PRINT_MESSAGE(xpn,3,S);
	g_free(S);
	g_object_unref (data_model);
}

typedef struct {
	double Schichtmacht; // cm;
	int AnzSchichten;
	double Lagerungsdichte;
	char* Bodenart;
	double Tongehalt;
	double Schluffgehalt;
	double Sandgehalt;
	double Steingehalt;
	double GehaltOrganischeSubstanz;
	double GehaltOrganischerKohlenstoff;
	double GehaltOrganischerStickstoff;
	double Ph;
	double Austauschkapazitaet;
	double Gesamtporenvolumen;
	double Feldkapazitaet;
	double PermanenterWelkepunkt;
	double MaximalesPotential;
	double GesaettigteLeitfaehigkeit;
	double ModellgroesseWendepunktVol;
	double ModellgroesseWendepunkPa;
	double ModellgroesseVanGenuchtenAlpha;
	double ModellgroesseVanGenuchtenN;
	double ModellgroesseAev;
	double ModellgroesseCampellB;
	double ModellgroesseWasserRes;
} STRUC_SOIL;

int expertn_database_get_iLayers(expertn_database *self, char *ProjectName) {
	int iLayers;
	char *sql,*NameBodenProfil,*S;
	GdaDataModel *data_model;
	int i;
	int countHoriz; // Anzahl der Horizonte
	int GesamtSchichtzahl;
	int AnzSchichten;
	int iAutoTeilSchlag;

	sql = g_strdup_printf("SELECT * FROM Projekt WHERE Projektname = '%s'",ProjectName);

	data_model = db_select(self->cnc,self->parser,sql);

	iAutoTeilSchlag = db_get_int(data_model,1);

	g_object_unref (data_model);

	sql = g_strdup_printf("SELECT * FROM StammTeilschlag WHERE AutoTeilschlag = %d",iAutoTeilSchlag);
	data_model = db_select(self->cnc,self->parser,sql);
	// Die meisten Daten sind hier nicht wichtig

	NameBodenProfil = db_get_string(data_model,5);

	S = g_strdup_printf("Bodenprofile: %s\n",self->NameBodenProfil);
	g_free(S);
	g_object_unref (data_model);

// Anzahl der Horizonte
	sql = g_strdup_printf("SELECT COUNT(*) FROM StammBodenprofilSchichten WHERE NameBodenprofil = '%s'",NameBodenProfil);
	data_model = db_select(self->cnc,self->parser,sql);
	countHoriz=db_get_int(data_model,0);
	g_object_unref (data_model);

	if (countHoriz<1) {
		PRINT_ERROR("No Soil Layers found");
		exit(1);
	}
	// Einzelne Profile durchgehen
	sql = g_strdup_printf("SELECT * FROM StammBodenprofilSchichten WHERE NameBodenprofil = '%s' ORDER BY Schichtnummer",NameBodenProfil);
	data_model = db_select(self->cnc,self->parser,sql);
	GesamtSchichtzahl = 2; // Die oberste und unterste Schicht ist eine Virtuelle
	for (i=0; i<countHoriz; i++) {
		AnzSchichten = db_get_int_row(data_model,3,i); // Anzahl der Schichten
		GesamtSchichtzahl += AnzSchichten;
	}

	g_object_unref (data_model);
	iLayers = GesamtSchichtzahl;
	g_free(NameBodenProfil);
	return iLayers;
}

void expertn_database_readObjectSoilProfile(expertn_database *self) {
	enum StammBodenprofilGesamt {
		NameBodenprofil =0,
		MittlererGrundwasserstand,
		Staunaeussestufe,
		Draenenabstand,
		Draentiefe,
		NutzungLdw,
	};
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *sql,*S;
	GdaDataModel *data_model;
	int i,i2,i3;
	int countHoriz; // Anzahl der Horizonte
	int GesamtSchichtzahl;
	double fDepth_all;
	STRUC_SOIL* Soil;
	PSPROFILE pSo;
	PSLAYER pSLayer;
	PSWATER pSWater;
	PSHEAT pSHeat;


	pSo = xpn->pSo;

	//const GValue *val;


	// Erst ueberpruefen, ob die Bodenschichtenanzahl mir der von Noah uebereinstimmt:

	// Anzahl der Horizonte
	sql = g_strdup_printf("SELECT COUNT(*) FROM StammBodenprofilSchichten WHERE NameBodenprofil = '%s'",self->NameBodenProfil);
	data_model = db_select(self->cnc,self->parser,sql);
	countHoriz=db_get_int(data_model,0);
	g_object_unref (data_model);

	if (countHoriz<1) {
		PRINT_ERROR("No Soil Layers found");
		exit(1);
	}
	Soil = (STRUC_SOIL*)g_malloc(sizeof(STRUC_SOIL)*countHoriz);
	// Einzelne Profile durchgehen
	sql = g_strdup_printf("SELECT * FROM StammBodenprofilSchichten WHERE NameBodenprofil = '%s'",self->NameBodenProfil);
	data_model = db_select(self->cnc,self->parser,sql);
	GesamtSchichtzahl = 2; // Die oberste und unterste Schicht ist eine Virtuelle
	for (i=0; i<countHoriz; i++) {
		Soil[i].Schichtmacht = db_get_double_row(data_model,2,i); // Dicke des Horizontes
		Soil[i].AnzSchichten = db_get_int_row(data_model,3,i); // Anzahl der Schichten
		Soil[i].Lagerungsdichte = db_get_double_row(data_model,4,i);
		Soil[i].Bodenart = db_get_string_row(data_model,5,i);
		Soil[i].Tongehalt = db_get_double_row(data_model,6,i);
		Soil[i].Schluffgehalt = db_get_double_row(data_model,7,i);
		Soil[i].Sandgehalt = db_get_double_row(data_model,8,i);
		Soil[i].Steingehalt = db_get_double_row(data_model,9,i);
		Soil[i].GehaltOrganischeSubstanz = db_get_double_row(data_model,10,i);
		Soil[i].GehaltOrganischerKohlenstoff = db_get_double_row(data_model,11,i);
		Soil[i].GehaltOrganischerStickstoff = db_get_double_row(data_model,12,i);
		Soil[i].Ph= db_get_double_row(data_model,13,i);
		Soil[i].Austauschkapazitaet = db_get_double_row(data_model,14,i);
		Soil[i].Gesamtporenvolumen = db_get_double_row(data_model,15,i);
		Soil[i].Feldkapazitaet = db_get_double_row(data_model,16,i);
		Soil[i].PermanenterWelkepunkt = db_get_double_row(data_model,17,i);
		Soil[i].MaximalesPotential = db_get_double_row(data_model,18,i);
		Soil[i].GesaettigteLeitfaehigkeit = db_get_double_row(data_model,19,i);
		Soil[i].ModellgroesseWendepunktVol = db_get_double_row(data_model,20,i);
		Soil[i].ModellgroesseWendepunkPa = db_get_double_row(data_model,21,i);
		Soil[i].ModellgroesseVanGenuchtenAlpha = db_get_double_row(data_model,22,i);
		Soil[i].ModellgroesseVanGenuchtenN = db_get_double_row(data_model,23,i);
		Soil[i].ModellgroesseAev = db_get_double_row(data_model,24,i);
		Soil[i].ModellgroesseCampellB = db_get_double_row(data_model,25,i);
		Soil[i].ModellgroesseWasserRes = db_get_double_row(data_model,26,i);
		GesamtSchichtzahl += Soil[i].AnzSchichten;
	}

	if (GesamtSchichtzahl!=pSo->iLayers) {
		S = g_strdup_printf("Soil layers don't fit:\n Noah iLayers: %d \nExpertn iLayers: %d\n",pSo->iLayers,GesamtSchichtzahl);
		PRINT_ERROR(S);
		g_free(S);
		exit(1);
	}

	i=0;
	i3=0;
	pSo->fDepth=0.0;
	fDepth_all=0.0;
	for (pSLayer=xpn->pSo->pSLayer,pSHeat = xpn->pSo->pSHeat,pSWater=xpn->pSo->pSWater,i2=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pSHeat = pSHeat->pNext,i2++) {
		//if ((i2!=xpn->pSo->iLayers-1))
		{


			// PSLAYER - Schicht:

			pSLayer->fClay = Soil[i].Tongehalt;
			pSLayer->fSand = Soil[i].Sandgehalt;
			pSLayer->fSilt = Soil[i].Schluffgehalt;
			pSLayer->fCHumus = Soil[i].GehaltOrganischerKohlenstoff;
			pSLayer->fHumus = Soil[i].GehaltOrganischeSubstanz;
			pSLayer->fNHumus = Soil[i].GehaltOrganischerStickstoff;


			pSLayer->fpH = Soil[i].Ph;
			pSLayer->fBulkDens = Soil[i].Lagerungsdichte;
			pSLayer->fBulkDensStart = Soil[i].Lagerungsdichte;

			if (Soil[i].Gesamtporenvolumen!=-99) {
				pSLayer->fPorosity = Soil[i].Gesamtporenvolumen/100.0;
			} else {
				pSLayer->fPorosity=-99.0;
			}
			pSLayer->fRockFrac = Soil[i].Steingehalt / 100.0;
			pSLayer->fRockFrac *= pSLayer->fBulkDens/2.65;//weight% to volume%


			// PSWATER Schicht:
			if (Soil[i].Gesamtporenvolumen!=-99.0) {
				pSWater->fContSat = Soil[i].Gesamtporenvolumen/100.0;
			} else {
				pSWater->fContSat = -99.0;
			}
			if (Soil[i].PermanenterWelkepunkt!=-99.0) pSWater->fContPWP = Soil[i].PermanenterWelkepunkt/100.0;
			else pSWater->fContPWP=-99.0;
			if (Soil[i].Feldkapazitaet!=-99.0)  pSWater->fContFK = Soil[i].Feldkapazitaet/100.0;
			else Soil[i].Feldkapazitaet=-99.0;
			if (Soil[i].ModellgroesseWasserRes!=-99.0) pSWater->fContRes = Soil[i].ModellgroesseWasserRes/100.0;
			else pSWater->fContRes=-99.0;
			//else pSWater->fContRes=0.0;

			if (Soil[i].ModellgroesseAev!=-99.0) pSWater->fCampA = Soil[i].ModellgroesseAev*(double) kPaTOmm;
			pSWater->fCampB = Soil[i].ModellgroesseCampellB;
			if (Soil[i].ModellgroesseVanGenuchtenAlpha!=-99) pSWater->fVanGenA =Soil[i].ModellgroesseVanGenuchtenAlpha*0.1 /*1/cm to 1/mm*/;
			else pSWater->fVanGenA=-99.0;
			pSWater->fVanGenN = Soil[i].ModellgroesseVanGenuchtenN;

			if (Soil[i].MaximalesPotential!=-99.0) pSWater->fMinPot = Soil[i].MaximalesPotential * (double)kPaTOmm;
			else pSWater->fMinPot=-99.0;
			pSWater->fCondSat=Soil[i].GesaettigteLeitfaehigkeit;
			pSWater->fContInflec = Soil[i].ModellgroesseWendepunktVol;
			if (Soil[i].ModellgroesseWendepunkPa!=-99.0) pSWater->fPotInflec=Soil[i].ModellgroesseWendepunkPa*(double)kPaTOmm;
			else pSWater->fPotInflec=-99.0;


			pSLayer->fThickness=Soil[i].Schichtmacht/(double)Soil[i].AnzSchichten * 10.0;
			if ((i2>0) && (i2<pSo->iLayers-1))
				{
					pSo->fDepth+=pSLayer->fThickness; // Gesamtprofiltiefe
				}
			fDepth_all+=pSLayer->fThickness; // Gesamtprofiltiefe

			if (Soil[i].Bodenart!=NULL) {
				pSLayer->acSoilID=(char*)g_malloc(strlen(Soil[i].Bodenart)+1);
				strcpy(pSLayer->acSoilID,Soil[i].Bodenart);
			} else {
				pSLayer->acSoilID=(char*)g_malloc(strlen("")+1);
				strcpy(pSLayer->acSoilID,"-99");
			}


			if ((i2>0) && (i2<xpn->pSo->iLayers-2)) i3++;
			if (i3>=Soil[i].AnzSchichten) {
				i++;
				i3=0;
			}

		}
		/*else
		if (i2==xpn->pSo->iLayers-1) // letzte Schicht
		{
		    pSLayer->acSoilID=(char*)g_malloc(strlen("")+1);
		    strcpy(pSLayer->acSoilID,"");
		}*/


	}

	pSo->fDeltaZ=fDepth_all/xpn->pSo->iLayers;

	expertn_database_help_func_set_standard_soil_paramter(xpn);


	g_object_unref (data_model);
	/*printf("i2 = %d ilayers = %d\n",i2,xpn->pSo->iLayers);
	exit(1);*/

	for (i=0; i<countHoriz; i++) {
		if (Soil[i].Bodenart!=NULL) g_free(Soil[i].Bodenart);
	}
	g_free(Soil);


	sql = g_strdup_printf("SELECT * FROM StammBodenprofilGesamt WHERE NameBodenprofil = '%s'",self->NameBodenProfil);

	data_model = db_select(self->cnc,self->parser,sql);
	//NameBodenprofil:
	//MittlererGrundwasserstand:
	//gda_data_model_dump(data_model,stdout);
	xpn->pLo->pFieldplot->fAveGroundWaterTable = db_get_double(data_model,1);
	//Staunaeussestufe:
	// Draenenabstand:
	xpn->pLo->pFieldplot->fDrainDistance = db_get_double(data_model,3);
	// Draentiefe:
	xpn->pLo->pFieldplot->fDrainDepth = db_get_double(data_model,4);
	//NutzungLdw:
	g_object_unref (data_model);
	//printf("%02d\n",(int)12);




}

void expertn_database_readTreatment(expertn_database *self) {
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *sql,*startdate,*stopdate;
	GdaDataModel *data_model;
	PTILLAGE tillage;
	int i,TreatmentNo;
	GDate *act_date;
	char *S;

	// Treatment No:
	startdate = xpn_time_get_formated_date(xpn->pTi->pSimTime->iStart_year,xpn->pTi->pSimTime->fStart_TimeY,self->MSAccess);
	stopdate = xpn_time_get_formated_date(xpn->pTi->pSimTime->iStop_year,xpn->pTi->pSimTime->fStop_TimeY,self->MSAccess);	
	sql = g_strdup_printf("SELECT COUNT(*) FROM BewegungBodenbearbeitung WHERE AutoTeilschlag = %d "
	                      "AND Bearbeitungstermin >= '%s' "
	                      "AND Bearbeitungstermin < '%s'",
	                      self->iAutoTeilSchlag,startdate,stopdate);
	g_free(startdate);
	g_free(stopdate);
	data_model = db_select(self->cnc,self->parser,sql);
	TreatmentNo = db_get_int(data_model,0);
	g_object_unref (data_model);
	S = g_strdup_printf("Treatment No: %d\tAutoteilschlag: %d\n",TreatmentNo,self->iAutoTeilSchlag);
	PRINT_MESSAGE(xpn,3,S);
	g_free(S);

	// Tillage Structur Allocieren:
	if (TreatmentNo < 1) {
		xpn->pMa->pTillage=NULL;
		return;
	}

	// Speicher Reservieren und Datenbank nach Datum sortieren:
	allocItemList(STTILLAGE,xpn->pMa->pTillage,TreatmentNo);
	startdate = xpn_time_get_formated_date(xpn->pTi->pSimTime->iStart_year,xpn->pTi->pSimTime->fStart_TimeY,self->MSAccess);
	stopdate = xpn_time_get_formated_date(xpn->pTi->pSimTime->iStop_year,xpn->pTi->pSimTime->fStop_TimeY,self->MSAccess);		
	sql = g_strdup_printf("SELECT * FROM BewegungBodenbearbeitung WHERE AutoTeilschlag = %d "
	                      "AND Bearbeitungstermin >= '%s' "
	                      "AND Bearbeitungstermin < '%s' ORDER BY Bearbeitungstermin",
	                      self->iAutoTeilSchlag,startdate,stopdate);
	g_free(startdate);
	g_free(stopdate);
	data_model = db_select(self->cnc,self->parser,sql);


	tillage = xpn->pMa->pTillage;
	for (i=0; i<TreatmentNo; i++) {
		GdaDataModel *data_model2;
		act_date = db_get_date_row(data_model,1,i);
		
		tillage->Day = g_date_get_day(act_date);
		tillage->Month = g_date_get_month(act_date);
		tillage->Year = g_date_get_year(act_date);		
		g_free(act_date);
		tillage->fDepth = db_get_double_row(data_model,2,i) * 10.0; // cm in mm;
		tillage->acName = db_get_string_row(data_model,3,i);
		

		sql = g_strdup_printf("SELECT Code FROM TabelleGeräteBodenbearbeitung  WHERE NameGeraet = '%s'",
		                      tillage->acName);
		data_model2 = db_select(self->cnc,self->parser,sql);
		tillage->acCode=db_get_string(data_model2,0);
		g_object_unref (data_model2);
		tillage->iEquipment=0; //?
		tillage->fEffIncorp=0.0; //?
		tillage->fEffLoose=0.0; //?
		tillage->fEffMix=0.0; //?
		tillage=tillage->pNext;
	}
	g_object_unref (data_model);

}

void expertn_database_readPlant(expertn_database *self) {
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	char *sql,*startdate,*stopdate;
	GdaDataModel *data_model,*data_model2;
	GDate *seed_date,*EmergenceDate,*HarvestDate,*MaxBioMasse,*MaxDurchwurzelung;
	int PlantNo,i;
	char *S;
	PGENOTYPE Genotype;
	PMSOWINFO SowInfo;
	PMODELPARAM ModelParam;

	// PlantNo:
	
	startdate = xpn_time_get_formated_date(xpn->pTi->pSimTime->iStart_year,xpn->pTi->pSimTime->fStart_TimeY,self->MSAccess);
	stopdate = xpn_time_get_formated_date(xpn->pTi->pSimTime->iStop_year,xpn->pTi->pSimTime->fStop_TimeY,self->MSAccess);		
		
	sql = g_strdup_printf("SELECT COUNT(*) FROM BewegungPflanzendaten WHERE AutoTeilschlag = %d "
	                      "AND Saattermin >= '%s' "
	                      "AND Saattermin < '%s'",
	                      self->iAutoTeilSchlag,startdate,stopdate);
	g_free(startdate);
	g_free(stopdate);
	data_model = db_select(self->cnc,self->parser,sql);
	PlantNo = db_get_int(data_model,0);
	g_object_unref (data_model);
	S = g_strdup_printf("Plant No: %d\tAutoteilschlag: %d\n",PlantNo,self->iAutoTeilSchlag);
	PRINT_MESSAGE(xpn,3,S);
	g_free(S);
	
	if (PlantNo < 1) {
		//xpn->pPl=NULL;
		// Das ist wohl keine Crop Plfanze, daher return
		return;
	}
	startdate =  xpn_time_get_formated_date(xpn->pTi->pSimTime->iStart_year,xpn->pTi->pSimTime->fStart_TimeY,self->MSAccess);
	stopdate =  xpn_time_get_formated_date(xpn->pTi->pSimTime->iStop_year,xpn->pTi->pSimTime->fStop_TimeY,self->MSAccess);	
	sql = g_strdup_printf("SELECT * FROM BewegungPflanzendaten WHERE AutoTeilschlag = %d "
	                      "AND Saattermin >= '%s' "
	                      "AND Saattermin < '%s'"
	                      "ORDER BY Saattermin,Reihenabstand",
	                      self->iAutoTeilSchlag,startdate,stopdate);
	g_free(startdate);
	g_free(stopdate);
	data_model = db_select(self->cnc,self->parser,sql);

	// Hier muesste eigentlich eine for schleife Stehen (scheinbar gibt es immer nur ein Element in der Liste)
	
	PlantNo=1;

	Genotype = xpn->pPl->pGenotype;
	SowInfo = xpn->pMa->pSowInfo;
	ModelParam = xpn->pPl->pModelParam;
	for (i=0; i<PlantNo; i++) {
		Genotype->acCropName = db_get_string_row(data_model,1,i); // Fruchtart
		SowInfo->fPlantDens = (double)db_get_int_row(data_model,2,i); // Saatstaerke
		SowInfo->fRowWidth = db_get_double_row(data_model,3,i); // Reihenabstand
		SowInfo->fSowDepth = db_get_double_row(data_model,4,i); // Saattiefe
		seed_date = db_get_date_row(data_model,5,i);    // Saattermin
		SowInfo->Day = g_date_get_day(seed_date);
		SowInfo->Month = g_date_get_month(seed_date);
		SowInfo->Year = g_date_get_year(seed_date);
		g_free(seed_date);		
		xpn->pPl->pRoot->fMaxDepth = db_get_int_row(data_model,6,i); // MaxDurchwurzelungstiefe
		EmergenceDate = db_get_date_row(data_model,7,i);//TerminAuflaufen			
		ModelParam->EmergenceDay=g_date_get_day(EmergenceDate);
		ModelParam->EmergenceMonth=g_date_get_month(EmergenceDate);
		ModelParam->EmergenceYear=g_date_get_year(EmergenceDate); 
		g_free(EmergenceDate);
		HarvestDate = db_get_date_row(data_model,8,i);//TerminErntenutzung
		ModelParam->HarvestDay=g_date_get_day(HarvestDate);
		ModelParam->HarvestMonth=g_date_get_month(HarvestDate);
		ModelParam->HarvestYear=g_date_get_year(HarvestDate);		
		g_free(HarvestDate);
		MaxBioMasse = db_get_date_row(data_model,9,i); //TerminMaxBioMasse
		ModelParam->MaxAboveBiomassDay=g_date_get_day(MaxBioMasse);
		ModelParam->MaxAboveBiomassMonth=g_date_get_day(MaxBioMasse);
		ModelParam->MaxAboveBiomassYear=g_date_get_day(MaxBioMasse);		
		g_free(MaxBioMasse);
		MaxDurchwurzelung = db_get_date_row(data_model,10,i); //terminMaxDurchwurzelung
		ModelParam->MaxRootBiomassDay=g_date_get_day(MaxDurchwurzelung);
		ModelParam->MaxRootBiomassMonth=g_date_get_month(MaxDurchwurzelung);
		ModelParam->MaxRootBiomassYear=g_date_get_year(MaxDurchwurzelung);		
		g_free(MaxDurchwurzelung);

		ModelParam->cResidueCarryOff=db_get_int_row(data_model,11,i);  //AbuhrBiomasse
		Genotype->acVarietyName= db_get_string_row(data_model,12,i);//Sorte

		S = g_strdup_printf("Crop Name: %s\n",xpn->pPl->pGenotype->acCropName);
		PRINT_MESSAGE(xpn,3,S);
		g_free(S);

		// CropCode Auslesen:

		sql = g_strdup_printf("SELECT Code FROM TabelleFruchtarten WHERE tFruchtart = '%s'",
		                      xpn->pPl->pGenotype->acCropName);
		data_model2 = db_select(self->cnc,self->parser,sql);
		Genotype->acCropCode = db_get_string_row(data_model2,0,i);
		g_object_unref (data_model2);
		
		// Sorteneigenschaften auslesen:
		/*sql = g_strdup_printf("SELECT * FROM TabelleSorteneigenschaften WHERE Fruchtart = '%s' AND Sortenname = '%s'",
		                      xpn->pPl->pGenotype->acCropName,Genotype->acVarietyName);
		
		data_model2 = db_select(self->cnc,self->parser,sql);
		
		g_object_unref (data_model2);*/
		
		// Vorfrucht auslesen ?
	}




	g_object_unref (data_model);

}


void expertn_database_readWetterTageswerte(expertn_database *self) {
	char *sql,*S;
	char *NameWetterstation;
	GdaDataModel *data_model;
	int i;
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);

	// Wetterstation rausfinden:
	NameWetterstation=NULL;
	sql = g_strdup_printf("SELECT * FROM StammTeilschlag WHERE AutoTeilschlag = %d",self->iAutoTeilSchlag);
	data_model = db_select(self->cnc,self->parser,sql);
	NameWetterstation=db_get_string(data_model,6);
	g_object_unref (data_model);



	S = g_strdup_printf("Weatherstation '%s' is taken!",NameWetterstation);
	PRINT_MESSAGE((&(self->parent)),3,S);
	g_free(S);
	
	
	// Durchschnittswerte lesen:
	sql = g_strdup_printf("SELECT * FROM StammWetterStation WHERE NameWetterstation = '%s'",NameWetterstation);
	data_model = db_select(self->cnc,self->parser,sql);
	
	xpn->pCl->pAverage->fYearTemp = db_get_double(data_model,6);
	xpn->pCl->pAverage->fMonthTempAmp = db_get_double(data_model,7);
	
	g_object_unref (data_model);
	
	// Anzahl der Wetterdaten rausfinden:
	sql = g_strdup_printf("SELECT COUNT(*) FROM StammWetterTageswerte WHERE NameWetterstation = '%s'",NameWetterstation);
	data_model = db_select(self->cnc,self->parser,sql);
	self->wetter_data_size = db_get_int(data_model,0);
	g_object_unref (data_model);
	if (self->wetter_data_size==0) {
		PRINT_ERROR("No weather data in database!");
		return;
	}
	self->wetter_data = g_malloc0_n(self->wetter_data_size,sizeof(struct_wetter_data));

	sql = g_strdup_printf("SELECT * FROM StammWetterTageswerte WHERE NameWetterstation = '%s' ORDER BY DatumTageswerte",NameWetterstation);
	data_model = db_select(self->cnc,self->parser,sql);
	for (i=0; i<self->wetter_data_size; i++) {
		self->wetter_data[i].Datum = db_get_date_row(data_model,1,i);
		self->wetter_data[i].MaximaleLufttemperatur = db_get_double_row(data_model,3,i);
		self->wetter_data[i].MinimaleLufttemperatur = db_get_double_row(data_model,4,i);
		self->wetter_data[i].MittlereLufttemperatur = db_get_double_row(data_model,9,i); // mittlere Lufttemperatur

		self->wetter_data[i].Globalstrahlung = db_get_double_row(data_model,2,i);
		self->wetter_data[i].Niederschlag = db_get_double_row(data_model,5,i);
		self->wetter_data[i].Sonnenscheindauer = db_get_double_row(data_model,6,i);
		self->wetter_data[i].RelativeLuftfeuchte = db_get_double_row(data_model,7,i);
		self->wetter_data[i].Windgeschwindigkeit = db_get_double_row(data_model,8,i);
		self->wetter_data[i].Taupunkt = db_get_double_row(data_model,10,i);
		self->wetter_data[i].Kesselverdunstung = db_get_double_row(data_model,11,i);
		self->wetter_data[i].SaettigungsdefizitLuft = db_get_double_row(data_model,12,i);
		self->wetter_data[i].Schneehoehe = db_get_double_row(data_model,13,i);
		self->wetter_data[i].PhotosynthischAktiveStrahlung = db_get_double_row(data_model,14,i);
	}
	g_object_unref (data_model);
	g_free(NameWetterstation);
}


void expertn_database_runWetterTageswerte(expertn_database *self) {
	expertn_modul_base *xpn = &(self->parent);
	//char *sql;
	char *NameWetterstation;//,*formdate;
//  GdaDataModel *data_model;
	GDate akt_date;
	double dt;
	int i;

	dt = xpn->pTi->pTimeStep->fAct;

	/*int i,i2,i3;
	int countHoriz; // Anzahl der Horizonte
	int GesamtSchichtzahl;
	STRUC_SOIL* Soil;
	PSPROFILE pSo;
	PSLAYER pSLayer;
	PSWATER pSWater;
	PSHEAT pSHeat;*/

//  pSo = xpn->pSo;

	// Name der Wetterstation:

	NameWetterstation=NULL;

	if ((self->OldDate_day !=xpn->pTi->pSimTime->mday) || (self->OldDate_month !=xpn->pTi->pSimTime->mon) || (self->OldDate_year !=xpn->pTi->pSimTime->year))
		{
		self->OldDate_day = xpn->pTi->pSimTime->mday;
		self->OldDate_month = xpn->pTi->pSimTime->mon;
		self->OldDate_year = xpn->pTi->pSimTime->year;


		
		xpn_time_conv_xpn_time_to_gdate(&akt_date,xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY);
		for (i=0; i<self->wetter_data_size; i++) {

			if (g_date_compare(self->wetter_data[i].Datum,&akt_date)==0) {
				break;
			}
		}
		if (i>=self->wetter_data_size) {
			PRINT_ERROR("No Weather data found for this date!");
			return;
		}


		// Gestrige Temperatur
		if (i>0) {
			self->TairLastDayMax = self->wetter_data[i-1].MaximaleLufttemperatur;
		} else {
			self->TairLastDayMax = self->wetter_data[i].MaximaleLufttemperatur;
		}

		// Heute:
		self->Tairmax = self->wetter_data[i].MaximaleLufttemperatur;
		self->Tairmin = self->wetter_data[i].MinimaleLufttemperatur;
		self->meantemp = self->wetter_data[i].MittlereLufttemperatur;

		// Morgen:
		if (i<self->wetter_data_size-1) {
			self->TairNextDayMin = self->wetter_data[i+1].MinimaleLufttemperatur;
		} else {
			self->TairNextDayMin = self->wetter_data[i].MinimaleLufttemperatur;
		}

		self->global_radiation = self->wetter_data[i].Globalstrahlung;
		self->precipitation = self->wetter_data[i].Niederschlag;
		self->sunshine_duration = self->wetter_data[i].Sonnenscheindauer;
		self->rel_himidity = self->wetter_data[i].RelativeLuftfeuchte;
		self->windspeed = self->wetter_data[i].Windgeschwindigkeit;
		self->dewpoint = self->wetter_data[i].Taupunkt;
		self->kesselverdunstung = self->wetter_data[i].Kesselverdunstung;
		self->saetigungsdefizit = self->wetter_data[i].SaettigungsdefizitLuft;
		self->snowdepth = self->wetter_data[i].Schneehoehe;
		self->par = self->wetter_data[i].PhotosynthischAktiveStrahlung;


		xpn->pCl->pWeather->fHumidity = self->rel_himidity; // ist nur Tageswert --> vlt. sollte man auch eine Sinusartige Funktion durchlegen
		xpn->pCl->pWeather->fWindSpeed = self->windspeed;
	}
	
	if (self->no_rain==1)
		{
			self->precipitation = 0.0;
		}
	
	// vllt muss man später noch schneien berücksichtigen
	xpn->pCl->pWeather->fPreciRate = self->precipitation;		// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fLiquPreciRate = self->precipitation;	// open land precip. until interception model is executed!
	xpn->pCl->pWeather->fBulkPreciRate = self->precipitation;	// open land precip.

	xpn->pCl->pWeather->fSolRad = get_SolRad(self->global_radiation,xpn->pTi->pSimTime->fTimeY , xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->fTimeDay, xpn->pTi->pTimeStep->fAct);



	xpn->pCl->pWeather->fPAR = xpn->pCl->pWeather->fSolRad / 2.0;//CalculateActiveRadiation(xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay, self->global_radiation ,xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->iJulianDay);


	//xpn->pCl->pWeather->fTempAir=db_get_double(data_model,9); // mittlere Lufttemperatur

	xpn->pCl->pWeather->fTempAir = calculateAirTemperature(xpn->pTi->pSimTime->fTimeDay,self->Tairmin,self->Tairmax,self->TairLastDayMax,self->TairNextDayMin,(double)xpn->pLo->pFarm->fLatitude,xpn->pTi->pSimTime->fTimeY);
	
	xpn->pCl->pWeather->fTempAir_zlvl = xpn->pCl->pWeather->fTempAir;

	if (xpn->pCl->fTempMeasHeight==0.0) {
		xpn->pCl->fTempMeasHeight=2.0;
	}
	if (xpn->pCl->fWindMeasHeight==0.0) {
		xpn->pCl->fWindMeasHeight=2.0;
	}

	//xpn->pCl->pWeather->fSolNet = expertn_database_get_Rn(xpn);

	/*laenge = calculateDayLength(xpn->pLo->pFarm->fLatitude,xpn->pTi->pSimTime->iJulianDay );

	C_DEBUG(laenge);*/



/*	C_DEBUG(self->Tairmax);
	C_DEBUG(self->Tairmin);





	C_DEBUG(self->meantemp);*/
	//C_DEBUG((double)xpn->pLo->pFarm->fLatitude)
	//C_DEBUG((double)xpn->pTi->pSimTime->iJulianDay)

	if (NameWetterstation!=NULL) g_free(NameWetterstation);
}


void expertn_database_init_Soil_Organic_Matter(expertn_database *self) {
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCHEMISTRY pCh = xpn->pCh;
	PMANAGEMENT pMa=xpn->pMa;
	PSPROFILE pSo=xpn->pSo;


	double f1,actDepth,RootProp,RootSum;

	double fCFOMVF, fCFOMF, fCFOMS,fNFOMVF, fNFOMF, fNFOMS;
	double fCMBF, fCMBS, fNMBF, fNMBS;
	double fCHF, fCHS, fCHR, fNHF, fNHS, fNHR;

	double fCN_AOM1,fCN_AOM2,fCN_AOM3,fCN_BOM1,fCN_BOM2;

	double AddCN,fCTotal,fNTotal;
	//float AnteilCFOMSlow,AnteilCFOMFast,AnteilCFOMVeryFast;
	//float AnteilCMicBiomSlow,AnteilCMicBiomFast;
	//float AnteilCHumusFast,AnteilCHumusSlow,AnteilCHumusStable;

	//float fCSlow,fNSlow;
	double fCFast,fNFast;
	//float CAnteilLitter,CAnteilManure,NAnteilLitter,NAnteilManure;


	PSLAYER       pSL;
	PCLAYER       pCL;

	RootProp = 0.0;
	RootSum  = 0.0;
	actDepth = 0.0;

	fCN_AOM1 = 200.0;
	fCN_AOM2 = 50.0;
	fCN_AOM3 = 5.0;

	fCN_BOM1 = 8.0;
	fCN_BOM2 = 8.0;

	/* Humusgehalte aus den Bodenbasisdaten zum Gehalt organischer Substanz

	In der Datenbank werden der Humusgehalt und der Gehalt an Kohlenstoff
	und Stickstoff im Humus als Gewichts-Prozent gespeichert. Bis zu dieser Funktion
	wurde mit diesen Prozentangaben gerechnet. Fuer die verwendeten
	Modelle ist es notwendig die Humusfraktionen in Mengen zu ueberfuehren.
	Hierzu muss neben der Lagerungsdichte auch der Flaechenbezug berueck-
	sichtigt werden. (10000 m2; Schichtdicke in mm; Lagerungsdichte in t/m3).
	Als Umrechnungsfaktor bleibt dann 100 erhalten. Weiterhin muss der Steingehalt
	bei der Berechnung des Bodengewichtes beuecksichtigt werden. Der Gehalt
	an organischer Substanz bezieht sich auf den Feinbodenanteil. */

	for (pSL= pSo->pSLayer->pNext; (pSL->pNext!=NULL); pSL=pSL->pNext) {
		/* Masse des Feinbodens pro ha */
		f1 = pSL->fBulkDens * pSL->fThickness * 100.0 * (1.0 -  pSL->fRockFrac);

		pSL->fHumus  *= f1;
		pSL->fCHumus *= f1;
		pSL->fNHumus *= f1;

	}

	/* Pflanzenrueckstaende (Litter-Pools) aus der Vorfrucht

	    Am Starttag der Simulation wird geprueft, ob die Ernte der
	    Vorfrucht bereits erreicht ist. Wenn ja erfolgt in der Funktion
	    TSComparePreCropDate die Initialisierung. Falls nein    erfolgt die
	    Beruecksichtigung der Pflanzenrueckstaende durch Aufruf von
	    TSComparePreCropDate zu spaeteren Termin aus time1.c heraus

	    */


	if ((pTi->pSimTime->mday == pMa->pLitter->Day) && (pTi->pSimTime->mon == pMa->pLitter->Month)  && (pTi->pSimTime->year == pMa->pLitter->Year) )
		{
		expertn_database_TSComparePreCropDate(self);
	} else {

		/* Wenn Angaben zu den Ernterueckstaenden in der Datenbank fehlen kommt es
		   in dieser Funktion zum Programmabsturz aufgrund Division by zero. Daher
		   findet eine letzte Ueberpruefung statt und es werden Werte ungleich Null
		   initialisiert. */

		if (pMa->pLitter->fTopCN <= 0.0)
			pMa->pLitter->fTopCN = 0.1;
		if (pMa->pLitter->fRootCN <= 0.0)
			pMa->pLitter->fRootCN = 0.1;

		pMa->pLitter->fTotalAmount = 0.0;
		pMa->pLitter->fTopC = 0.0;
		pMa->pLitter->fRootC = 0.0;
		pMa->pLitter->fTotalC = 0.0;
		pMa->pLitter->fTotalCN = 0.1;

		pCh->pCProfile->fCLitterSurf = 0.0;
		pCh->pCProfile->fNLitterSurf = 0.0;

		pCh->pCProfile->fCManureSurf = 0.0;
		pCh->pCProfile->fNManureSurf = 0.0;

		pCh->pCProfile->fCHumusSurf = 0.0;
		pCh->pCProfile->fNHumusSurf = 0.0;

		pCh->pCProfile->fCStandCropRes = 0.0;
		pCh->pCProfile->fNStandCropRes = 0.0;

		/* Berechnen schichtmaessiger Anteil. Zur Vereinfachung
		  Variable pCL->fCLitter benutzt um Anteile zu uebergeben. */
		for (pSL = pSo->pSLayer->pNext,
		        pCL = pCh->pCLayer->pNext;
		        ((pSL->pNext != NULL)&&
		         (pCL->pNext != NULL));
		        pSL = pSL->pNext,
		        pCL = pCL->pNext) {
			pCL->fCLitter = 0.0;
		}

		/* Anteilsmaessige Verteilung auf die Bodenschichten. */
		for (pSL = pSo->pSLayer->pNext,
		        pCL = pCh->pCLayer->pNext;
		        ((pSL->pNext != NULL)&&
		         (pCL->pNext != NULL));
		        pSL = pSL->pNext,
		        pCL = pCL->pNext) {
			/* wurde auf -99 initialisiert; wenn nein in Modellparameter-
			   File eingelesen und sollte nicht ueberschrieben werden   */

			pCL->fCLitter = 0.0;
			pCL->fNLitter = 0.1;
			pCL->fLitterCN = 0.1;

		}
	}


	/***********************************************************************/
	/* Initialisierung der Mengen in den spezifischen DNDC-Pools           */
	/***********************************************************************/

	//schichtweise Berechnung
	for (pSL = pSo->pSLayer->pNext,pCL = pCh->pCLayer->pNext; ((pSL->pNext != NULL)&&(pCL->pNext != NULL)); pSL = pSL->pNext,pCL = pCL->pNext) {

		/* Summe der zu verteilenden Mengen */
		fCTotal =  pCL->fCLitter +  pSL->fCHumus;
		fCFast  = 0.15 * fCTotal;

		fNTotal =  pCL->fNLitter +  pSL->fNHumus;
		fNFast  = 0.15 * fNTotal;

		/* Initialisierung der Residue-Pools Resistant, Labile, VeryLabile*/
		if(fNFast)
			AddCN = fCFast/fNFast;
		else
			AddCN = 0.0;

		if(AddCN >= fCN_AOM3 && AddCN < fCN_AOM2) {
			fCFOMVF = (fCFast * (1.0/AddCN - 1.0/fCN_AOM3)) / (1.0/fCN_AOM2 - 1.0/fCN_AOM3);
			fCFOMF = fCFast - fCFOMVF;
			fCFOMS = 0.0;
		}

		if(AddCN >= fCN_AOM2 && AddCN < fCN_AOM1) {
			fCFOMF = (fCFast * (1.0/AddCN - 1.0/fCN_AOM2)) / (1.0/fCN_AOM1 - 1.0/fCN_AOM2);
			fCFOMS = fCFast - fCFOMF;
			fCFOMVF = 0.0;
		}

		if ((AddCN>=0.0) && (AddCN<=5.0)) {
			fCFOMF = 0.0;
			fCFOMS = 0.0;
			fCFOMVF = 0.0;
		}

		if (AddCN > fCN_AOM1) {
			gchar *S;
			S  = g_strdup_printf("AddCN > fCN_AOM1");
			PRINT_ERROR(S);
			g_free(S);
		}

		fNFOMVF = fCFOMVF / fCN_AOM3;
		fNFOMF  = fCFOMF  / fCN_AOM2;
		fNFOMS  = fCFOMS  / fCN_AOM1;


		//initialisieren der globalen Variablen
		pCL->fCFOMVeryFast = fCFOMVF;
		pCL->fCFOMFast = fCFOMF;
		pCL->fCFOMSlow = fCFOMS;

		pCL->fNFOMVeryFast = fNFOMVF;
		pCL->fNFOMFast = fNFOMF;
		pCL->fNFOMSlow = fNFOMS;


		/* Initialisieren der Biomasse Pools */
		fCMBF = 0.15 * 0.02 * 0.9 * fCTotal;
		fCMBS = 0.15 * 0.02 * 0.1 * fCTotal;

		fNMBF = 1.0/fCN_BOM2 * fCMBF;
		fNMBS = 1.0/fCN_BOM1 * fCMBS;

		pCL->fCMicBiomFast = fCMBF;
		pCL->fCMicBiomSlow = fCMBS;
		pCL->fNMicBiomFast = fNMBF;
		pCL->fNMicBiomSlow = fNMBS;


		/* Restmengen berechnen */
		fCTotal -= (pCL->fCMicBiomSlow + pCL->fCMicBiomFast +
		            pCL->fCFOMFast + pCL->fCFOMSlow + pCL->fCFOMVeryFast);
		fNTotal -= (pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
		            pCL->fNFOMFast + pCL->fNFOMSlow + pCL->fNFOMVeryFast);

		/*
		Initialisieren der "Humads" und Humus Pools
		bezogen auf den gesamten org. C (Humus + Litter)
		fCTotal ist der noch nicht auf Pools
		verteilte Rest des gesamten org. C
		*/

		if ((fCTotal)&&(fNTotal)) {
			pSL->fHumusCN = fCTotal/fNTotal;
			fCHS   = (float)0.146 * fCTotal;//0.15 * 0.98 * 0.84 des gesamten org. C
			fCHF   = (float)0.028 * fCTotal;//0.15 * 0.98 * 0.16 des gesamten org C
			fCHR = (float)0.826 * fCTotal;  //0.7 des gesamten org. C
			fNHS = fCHS / pSL->fHumusCN;
			fNHF = fCHF / pSL->fHumusCN;
			fNHR = fCHR / pSL->fHumusCN;
		}


		pCL->fCHumusFast = fCHF;
		pCL->fCHumusSlow = fCHS;
		pCL->fCHumusStable = fCHR;
		pCL->fNHumusFast = fNHF;
		pCL->fNHumusSlow = fNHS;
		pCL->fNHumusStable= fNHR;

	}// end for zur schichtweisen Berechnung

}

/*********************************************************************************/
/*  Name     : TSComparePreCropDate                                              */
/*                                                                               */
/*  Funktion : Vergleicht das Stardatum der Simulation mit dem Termin fuer       */
/*             Ernte der Vorfrucht. Liegt dieser Termine nach dem Beginn der     */
/*             Simulation werden die entsprechenden Pools aktualisiert und die   */
/*             Bilanzgroessen auf null gesetzt.                                  */
/*  Autor    :                                                                   */
/*  Datum    :                                                                   */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
void expertn_database_TSComparePreCropDate(expertn_database *self) {
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PCHEMISTRY pCh = xpn->pCh;
	PMANAGEMENT pMa=xpn->pMa;
	PSPROFILE pSo=xpn->pSo;
	double          actDepth,RootProp,amount,factor,RootSum;

	PSLAYER       pSL;
	PCLAYER       pCL;

	RootProp = 0.0;
	RootSum  = 0.0;
	actDepth = 0.0;

	if ((pTi->pSimTime->mday == pMa->pLitter->Day) && (pTi->pSimTime->mon == pMa->pLitter->Month) && (pTi->pSimTime->year == pMa->pLitter->Year))
		 {
		/* Wenn Angaben zu den Ernterueckstaenden in der Datenbank fehlen kommt es
		   in dieser Funktion zum Programmabsturz aufgrund Division by zero. Daher
		   findet eine letzte Ueberpruefung statt und es werden Werte ungleich Null
		   initialisiert. */

		if (pMa->pLitter->fTopCN <= 0.0)
			pMa->pLitter->fTopCN = 0.1;
		if (pMa->pLitter->fRootCN <= 0.0)
			pMa->pLitter->fRootCN = 0.1;



		/* Gesamtmenge der ober- und unterirdischen Ernterueckstaende. */
		if (pMa->pLitter->fTotalAmount == -99.0)
			pMa->pLitter->fTotalAmount = pMa->pLitter->fRootAmount;
		else
			pMa->pLitter->fTotalAmount += pMa->pLitter->fRootAmount;

		if(pMa->pLitter->iCarryOff==0)
			pMa->pLitter->fTotalAmount += pMa->pLitter->fTopAmount;


		/* Zu Beginn der Simulation werden die oberirdischen Ernterueckstaende auf
		   eine Oberflaechenschicht initialisiert. Dabei bestehen 40% der Rueck-
		   staende aus Kohlenstoff und werden daher auf die Pools geschrieben */

		if(pMa->pLitter->iCarryOff==0)
			pMa->pLitter->fTopC = pMa->pLitter->fTopAmount  * 0.4;

		else
			pMa->pLitter->fTopC = 0.0;

		pCh->pCProfile->fCLitterSurf = pMa->pLitter->fTopC;
		pCh->pCProfile->fNLitterSurf = pMa->pLitter->fTopC / pMa->pLitter->fTopCN;

		pCh->pCProfile->fCManureSurf = 0.0;
		pCh->pCProfile->fNManureSurf = 0.0;

		pCh->pCProfile->fCHumusSurf = 0.0;
		pCh->pCProfile->fNHumusSurf = 0.0;

		/* Die Wurzelmasse, wird auf die Bodenschichten verteilt. Zuerst wird die Gesamtsumme ermittelt,
		   der Schichtanteil berechnet und dann die schichtmaessige Verteilung vorgenommen.
		   Zur Vereinfachung Variable pCL->fCg_freeFOM benutzt um Anteile zu uebergeben */

		pMa->pLitter->fRootC = pMa->pLitter->fRootAmount * 0.40;

		for (pSL = pSo->pSLayer->pNext,
		        pCL = pCh->pCLayer->pNext;
		        ((pSL->pNext != NULL)&&
		         (pCL->pNext != NULL));
		        pSL = pSL->pNext,
		        pCL = pCL->pNext) {
			actDepth = actDepth + (pSL->fThickness / 10.0);
			RootProp = exp(-3.0 * actDepth / (pSo->fDepth / 10.0));
			RootSum += RootProp;
			pCL->fCFreeFOM = RootProp;
		}

		for (pCL = pCh->pCLayer->pNext;
		        pCL->pNext != NULL;
		        pCL = pCL->pNext) {
			factor = pCL->fCFreeFOM/ RootSum;
			amount = pMa->pLitter->fRootC * factor;

			if(amount>0.0) {
				pCL->fCLitter = amount;
				pCL->fNLitter = amount/pMa->pLitter->fRootCN;
			}

			else {
				pCL->fCLitter =  0.0;
				pCL->fNLitter =  0.1;
			}

			if(pCL->fNLitter>EPSILON)
				pCL->fLitterCN = pCL->fCLitter / pCL->fNLitter;

			else
				pCL->fLitterCN = 0.1;
		}
	}


}     /*===== Ende der Ueberpreufung der Startwerte =============*/


int expertn_database_readBewegungMineralduengung(expertn_database *self) {
	expertn_modul_base *xpn = &(self->parent);
	GdaDataModel *data_model, *data_model2;
	int ferilizer_count;
	PNFERTILIZER fertil_first,fertil,fertil_act;
	gchar *sql,*sql2;
	GDate* date;
	int i;


	sql = g_strdup_printf("SELECT COUNT(*) FROM BewegungMineraldüngung WHERE AutoTeilschlag = %d ORDER BY Ausbringungstermin",
	                      self->iAutoTeilSchlag);

	data_model = db_select(self->cnc,self->parser,sql);
	ferilizer_count = db_get_int(data_model,0);
	g_object_unref (data_model);

	sql = g_strdup_printf("SELECT * FROM BewegungMineraldüngung WHERE AutoTeilschlag = %d ORDER BY Ausbringungstermin",
	                      self->iAutoTeilSchlag);

	data_model = db_select(self->cnc,self->parser,sql);

	fertil_first=NULL;
	fertil_act=NULL;

	for (i=0; i<ferilizer_count; i++) {
		// Struktur mit 0 Initialisieren
		fertil = g_malloc0_n(1,sizeof(STNFERTILIZER));

		fertil->acName = db_get_string_row(data_model,1,i);
		date = db_get_date_row(data_model,2,i);
		fertil->Day = g_date_get_day(date);
		fertil->Month = g_date_get_month(date);
		fertil->Year = g_date_get_year(date);
		g_free(date);
		fertil->fTotalN = (double)db_get_int_row(data_model,3,i);
		fertil->fNH4N = db_get_double_row(data_model,5,i);
		fertil->fNO3N = db_get_double_row(data_model,6,i);
		fertil->fUreaN = db_get_double_row(data_model,7,i);

		sql2 = g_strdup_printf("SELECT Code FROM  TabelleDüngerMineralisch  WHERE NameDuenger = '%s'",
		                       fertil->acName);
		data_model2 = db_select(self->cnc,self->parser,sql2);
		fertil->acCode = db_get_string(data_model2,0);

		g_object_unref (data_model2);
		if (i==0) {
			fertil_first = fertil;
			fertil_act = fertil;
		} else {
			fertil_act->pNext = fertil;
			fertil->pBack = fertil_act;
			fertil_act = fertil_act->pNext;
		}

	}

	if (ferilizer_count!=0) {
		// Letzter hat -1 als Wert
		fertil = g_malloc0_n(1,sizeof(STNFERTILIZER));
		fertil->Day = 0;
		fertil->Month = 0;
		fertil->Year = 0;
		fertil_act->pNext = fertil;
		fertil->pBack = fertil_act;
		fertil_act = fertil_act->pNext;
	}

	xpn->pMa->pNFertilizer = fertil_first;


	g_object_unref (data_model);
	return RET_SUCCESS;
}



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

#define MATRIX_POTENTIAL(x) (float) (*self->MPotl)((double)pWL->fMatPotAct,\
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







int expertn_database_initialize(expertn_database *self) {
	expertn_modul_base *xpn = &(self->parent);
	GError *error = NULL;
	char *Provider_Name,*cnc_String, *ProjectName;
	char *S,*S_reg;
	GDate *global_start_date,*global_end_date;

	cnc_String=NULL;	

	gda_init ();
		


	self->measure_time=0;
	self->start_time_intern=0.0;
	self->end_time_intern=0.0;
	self->sim_time_intern=0.0;
	if (xpn->pXSys->id==0) {
		S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Database.measure time");
		if (S==NULL) {
			S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.measure time");
			if (S==NULL) {
				S = "0";
			}
		}
		self->measure_time = atoi(S);
		
	self->no_rain = 0;
	
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.expertn_database.no rain");
	
	if (S==NULL)
		{
			self->no_rain = 0;
		} else
		{
			self->no_rain = atoi (S);
		}
		
	self->no_crop=0;
	S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Database.no crop");
	if (S!=NULL)
		{
			self->no_crop = atoi(S);
		}

if (self->measure_time==1)
{

#ifdef USE_OPENMP
		self->start_time_intern = omp_get_wtime();
#else
		self->start_time_intern = (double)clock()/(double)CLOCKS_PER_SEC;
#endif


}
	}

	expertn_database_help_func_set_global_start_and_end_date(xpn,&(global_start_date),&(global_end_date));


	Provider_Name = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Database.provider name");
	ProjectName = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Database.project name");

	// cnc_String:
	S_reg = xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.Expert N Standard Database.cnc string");

	if (S_reg!=NULL) {
		S =expertn_modul_base_replace_std_templates(xpn,S_reg);
	} else {
		S = NULL;
	}


	if (S!=NULL) {
		int res;
		char *S2,*S3,*S4,*S5;
		S3 = NULL;
		S4 = NULL;
		S5 = NULL;
		S2 = (char*)g_malloc0(sizeof(char)*(strlen(S)+1));
		res = getStringColumn(S2,S,"DB_DIR=",1);
		if (res != 0) {
			cnc_String = (char*)calloc(1,sizeof(char)*(strlen(S)+1));
			strcpy(cnc_String,S);
		} else {
			S3 = (char*)g_malloc0(sizeof(char)*(strlen(S2)+1));
			res = getStringColumn(S3,S2,";",0);
			if (res != 0) {
				cnc_String = (char*)calloc(1,sizeof(char)*(strlen(S)+1));
				strcpy(cnc_String,S);
			} else {



				S4 = get_fullpath_from_relative(self->parent.pXSys->base_path, S3);


				res = getStringColumn(S2,S,";",1);
				if (res==0) {
					S5 = g_strdup_printf("DB_DIR=%s;%s",S4,S2);
				} else {
					S5 = g_strdup_printf("DB_DIR=%s;",S4);
				}
				cnc_String = (char*)calloc(1,sizeof(char)*(strlen(S5)+1));
				strcpy(cnc_String,S5);
			}
		}

		if (S2 != NULL) g_free(S2);
		if (S3 != NULL) g_free(S3);
		if (S4 != NULL) free(S4);
		if (S5 != NULL) g_free(S5);
	}



	if (ProjectName==NULL) {
		char *S;
		S = g_strdup_printf  ("No Project defined!");
		PRINT_ERROR(S);
		g_free(S);
		exit (1);
	}


	if ((Provider_Name==NULL) || (cnc_String==NULL)) {
		char *S;
		S = g_strdup_printf  ("Could not open connection to Database: Problem with Provider Name or cnc String");
		PRINT_ERROR(S);
		g_free(S);
		exit (1);
		self->cnc=NULL;
	} else {	
		self->cnc = gda_connection_open_from_string (Provider_Name,cnc_String, NULL, GDA_CONNECTION_OPTIONS_READ_ONLY, &error);
	}

	if (cnc_String!=NULL) {
		free(cnc_String);
		cnc_String=NULL;
	}

	if (!self->cnc) {
		char *S;
		S= g_strdup_printf  ("Could not open connection to Database: %s\n",
		                     error && error->message ? error->message : "No detail");
		PRINT_ERROR(S);
		g_free(S);
		exit (1);
	}

	if (strcmp(Provider_Name,"MSAccess")==0) {
		self->MSAccess=TRUE;
	} else {
		self->MSAccess=FALSE;
	}


	/* create an SQL parser */
	self->parser = gda_connection_create_parser (self->cnc);
	if (!self->parser) { /* @cnc doe snot provide its own parser => use default one */
		self->parser = gda_sql_parser_new ();
	}
	/* attach the parser object to the connection */
	g_object_set_data_full (G_OBJECT (self->cnc), "parser", self->parser, g_object_unref);

	//if (xpn->pLo->pTile->ProjectName!=NULL)
	{
//        char *S;
		//xpn->pLo->pTile->ProjectName="Neu";
		//printf("Anzahl der Schichten:     %d\n",expertn_database_get_iLayers(self));
		//xpn->pTi->pSimTime->lStartDate=10190;



		expertn_database_help_func_reservere_Memory(xpn,expertn_database_get_iLayers(self,ProjectName));
		xpn->pLo->pTile->ProjectName = (char*)g_malloc(strlen(ProjectName)+1);
		strcpy(xpn->pLo->pTile->ProjectName,ProjectName);

		expertn_database_help_func_set_time_step(xpn,"Config.Expert N Standard Database.time step");


		xpn->pTi->pSimTime->fTimeZone = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Expert N Standard Database.time zone",0.0);


		expertn_database_readObjectProject(self,global_start_date,global_end_date);
		expertn_database_readObjectLocation(self);
		expertn_database_readObjectSoilProfile(self);
		
		if (self->no_crop==0)
			{
				expertn_database_readTreatment(self);
				expertn_database_readPlant(self);
			}
		
		expertn_database_readStartValues(self);
		expertn_database_help_func_init_water(xpn);
		expertn_database_help_func_init_Chemistry(xpn);

		expertn_database_readBewegungMineralduengung(self);
		expertn_database_readWetterTageswerte(self);
	}

	self->durchgang=0;
	self->timesave=0.0;
	self->firstround=0;
	//xpn_register_var_init_pdouble(self->parent.pXSys->var_list,"expertn_database.MeanTemp", self->meantemp,0.0);
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.fSolRad",&(self->global_radiation),-1,TRUE);	
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.fTempMin",&(self->Tairmin),-1,TRUE);
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.fTempMax",&(self->Tairmax),-1,TRUE);
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.fTempMean",&(self->meantemp),-1,TRUE);
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.global_radiation",&(self->global_radiation),-1,TRUE);
	xpn_register_var_add_none(self->parent.pXSys->var_list,"expertn_database.par",&(self->par),-1,TRUE);

	g_free(ProjectName);

	// ToDo:




	self->CM = 1.E-4; // keine Ahnung, was ein guter Startwert ist
	self->CH = 1.E-4;


	/*  if (self->StandAlone==FALSE) {
	        struct_WRF *wrf;
	        xpn_register_var *var_list;
	        wrf = self->WRF;
	        var_list = xpn->pXSys->var_list;
	        STRUCT_DRIVE_XPN_REG
	    }*/


	gda_connection_close (self->cnc);

	

	G_FREE_DATE_IF_NOT_0(global_start_date);
	G_FREE_DATE_IF_NOT_0(global_end_date);

	return 0;
}

int expertn_database_run(expertn_database *self) {
	expertn_modul_base *xpn = &(self->parent);
	PWLAYER pWLayer;
	char *S;
	STRUCT_XPN_TIME xpn_time;
	int iyear;
	double fTimeY;



	xpn->pWa->fActETR = xpn->pPl->pPltWater->fActTranspR + xpn->pWa->pEvap->fActR + xpn->pPl->pPltWater->fInterceptR;

	// Wetterdaten aus Datenbank lesen falls nötig:
	if (!xpn->pXSys->extern_time_management) {
		double intpart;

		if (self->firstround==1) {
			xpn->pTi->pTimeStep->fOld = xpn->pTi->pTimeStep->fAct;
			
			xpn->pTi->pSimTime->iyear_old = xpn->pTi->pSimTime->iyear;
			xpn->pTi->pSimTime->fTimeY_old = xpn->pTi->pSimTime->fTimeY;
			
			xpn_time_add_dt(&(xpn->pTi->pSimTime->iyear),&(xpn->pTi->pSimTime->fTimeY),xpn->pTi->pTimeStep->fAct);

			xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
			
			xpn->pTi->pSimTime->is_leap_year = xpn_time.is_leap_year; //  FLAG: 1 or 0
			xpn->pTi->pSimTime->msec = xpn_time.msec;
			xpn->pTi->pSimTime->sec = xpn_time.sec;
			xpn->pTi->pSimTime->min = xpn_time.min;
			xpn->pTi->pSimTime->hour = xpn_time.hour;
			xpn->pTi->pSimTime->mday = xpn_time.mday;
			xpn->pTi->pSimTime->mon = xpn_time.mon;
			xpn->pTi->pSimTime->year = xpn_time.year;
			
			xpn->pTi->pSimTime->fTimeDay = modf(xpn->pTi->pSimTime->fTimeY,&intpart);
			//expertn_database_init_water(self);

		} else {
			
			xpn->pTi->pSimTime->iyear_old = xpn->pTi->pSimTime->iyear;
			xpn->pTi->pSimTime->fTimeY_old = xpn->pTi->pSimTime->fTimeY;			
			xpn->pTi->pTimeStep->fOld = xpn->pTi->pTimeStep->fAct;
			
			xpn_time_get_struc(&xpn_time,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
			
			xpn->pTi->pSimTime->is_leap_year = xpn_time.is_leap_year; //  FLAG: 1 or 0
			xpn->pTi->pSimTime->msec = xpn_time.msec;
			xpn->pTi->pSimTime->sec = xpn_time.sec;
			xpn->pTi->pSimTime->min = xpn_time.min;
			xpn->pTi->pSimTime->hour = xpn_time.hour;
			xpn->pTi->pSimTime->mday = xpn_time.mday;
			xpn->pTi->pSimTime->mon = xpn_time.mon;
			xpn->pTi->pSimTime->year = xpn_time.year;
			
			xpn->pTi->pSimTime->fTimeDay = modf(xpn->pTi->pSimTime->fTimeY,&intpart);
			//expertn_database_init_water(self);
		}

		expertn_database_runWetterTageswerte(self);

		/*printf("Simtime: %f\n",xpn->pTi->pSimTime->fTime);
		printf("TimeStep: %f\n",xpn->pTi->pTimeStep->fAct);

		printf("Anzahl Tage: %d\n",(int)self->AnzahlderTage);
		printf("Tag Nr: %d\n",(int)(xpn->pTi->pSimTime->fTimeAct));*/


		// Ausgabe der Zeit soll nur einmal erfolgen:
		if (xpn->pXSys->id==0) {
			if (xpn->pTi->pSimTime->mday != self->timesave) {
				//int minute,hour;

				//hour = (int)(xpn->pTi->pSimTime->fTimeDay*24.0);
				//minute = (int)((xpn->pTi->pSimTime->fTimeDay*24.0-hour)*60.0);

				S =xpn_time_get_formated_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,FALSE);

				fprintf(stdout,"Simulation Day : %s\n",S);
				fflush(stdout);
				g_free(S);
				self->timesave=xpn->pTi->pSimTime->mday;
			}
		}

		//PRINT_ERROR("Test");

		if (self->firstround==0) {
			xpn->pTi->pSimTime->bFirstRound=TRUE;
		} else {
			xpn->pTi->pSimTime->bFirstRound=FALSE;
		}
		self->firstround=1;

	} else { // externes Zeitmanagement:


		// Ausgabe der Zeit soll nur einmal erfolgen:
		if (xpn->pXSys->id==0) {
			//if (xpn->pTi->pSimTime->fTime >= self->timesave+(1.0*60.0*60.0*24.0))
			{
				char str_h[5],str_m[5],str_s[5];
				

				
				if (xpn->pTi->pSimTime->hour < 10) sprintf(str_h,"0%d",xpn->pTi->pSimTime->hour);
				else sprintf(str_h,"%d",xpn->pTi->pSimTime->hour);
				if (xpn->pTi->pSimTime->min< 10) sprintf(str_m,"0%d",xpn->pTi->pSimTime->min);
				else sprintf(str_m,"%d",xpn->pTi->pSimTime->min);
				if (xpn->pTi->pSimTime->sec < 10) sprintf(str_s,"0%d",xpn->pTi->pSimTime->sec);
				else sprintf(str_s,"%d",xpn->pTi->pSimTime->sec);

				S = xpn_time_get_formated_date(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,FALSE);

				//fprintf(stdout,"Simulation Day : %s %s:%s:%s %d %f\n",S,str_h,str_m,str_s,self->WRF->M,xpn->pTi->pSimTime->fTime);
				fprintf(stdout,"Simulation Day : %s %s:%s:%s\n",S,str_h,str_m,str_s);
				fflush(stdout);
				g_free(S);
				self->timesave=xpn->pTi->pSimTime->mday;
			}
		}

		//PRINT_ERROR("Test");

		if (self->firstround==0) {
			xpn->pTi->pSimTime->bFirstRound=TRUE;
			self->AnzahlderTage=xpn_time_get_number_of_days(xpn->pTi->pSimTime->iStart_year, xpn->pTi->pSimTime->fStart_TimeY,xpn->pTi->pSimTime->iStop_year, xpn->pTi->pSimTime->fStop_TimeY);					
		} else {
			xpn->pTi->pSimTime->bFirstRound=FALSE;
		}
		self->firstround=1;
	}

	//self->durchgang++;
	/*if (self->durchgang == 20)
	    {
	        PRINT_MESSAGE(xpn,3,"Expert N Abbruch\n");
	        exit(1);
	    }*/
	for (pWLayer = xpn->pWa->pWLayer; pWLayer!=NULL; pWLayer=pWLayer->pNext) {
		pWLayer->fContOld =  pWLayer->fContAct;
	}
	
	iyear = xpn->pTi->pSimTime->iStop_year;
	fTimeY = xpn->pTi->pSimTime->fStop_TimeY;
	
	xpn_time_add_dt(&iyear,&fTimeY,-xpn->pTi->pTimeStep->fAct);

	if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,iyear,fTimeY)>=0)
	{
		xpn->pTi->pSimTime->bLastRound=TRUE;
	} else {
		xpn->pTi->pSimTime->bLastRound=FALSE;
	}

	if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,xpn->pTi->pSimTime->iStop_year,xpn->pTi->pSimTime->fStop_TimeY)>=0) {
		return 1;
	}
	
	self->sim_time_intern += xpn->pTi->pTimeStep->fAct;
    
    if (xpn->pTi->pSimTime->bFirstRound==TRUE)
        {
            expertn_standard_set_up_soil_temp(self);
        }

	return RET_SUCCESS;
}



int expertn_datanase_finalize(expertn_database *self) {
	int i;
	char *S;


	for (i=0; i<self->wetter_data_size; i++) {
		g_free(self->wetter_data[i].Datum);
	}
	g_free(self->wetter_data);
	self->wetter_data=NULL;
	self->wetter_data_size=0;

	if (self->parent.pXSys->id==0) {
		if (self->measure_time==1) {

#ifdef USE_OPENMP
			self->end_time_intern = omp_get_wtime();
#else
			self->end_time_intern = (double)clock()/(double)CLOCKS_PER_SEC;
#endif			
			S = g_strdup_printf("Expert-N execution time : %f s\n",self->end_time_intern-self->start_time_intern);
			PRINT_MESSAGE((&(self->parent)),3,S);
			g_free(S);
			S = g_strdup_printf("Expert-N simulation time: %f s\n",self->sim_time_intern*(24.0*60.0*60.0));
			PRINT_MESSAGE((&(self->parent)),3,S);
			g_free(S);
		}
	}
	return 0;
}
#endif
