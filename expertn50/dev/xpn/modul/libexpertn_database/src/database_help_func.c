#include "database_help_func.h"
#include <math.h>

#include <xtime_reg.h>
#include <xclimate_reg.h>
#include <xheat_reg.h>
#include <xsoil_reg.h>
#include <xwater_reg.h>
#include <xchemist_reg.h>
#include <xplant_reg.h>
#include <expertn_noah_driver_reg.h>

//      Datenbankfuntkionene, die den Zugriff erleichtern und Runtime Debugging betreiben:

#ifndef XPN_WITHOUT_DATABASE
GdaDataModel *_db_select(GdaConnection *cnc, GdaSqlParser *parser, char* sql,int LineNr) {
	GError *error = NULL;
	GdaDataModel *data_model;
	GdaStatement *stmt;

	stmt = gda_sql_parser_parse_string (parser, sql, NULL, NULL);
	g_free(sql);
	data_model = gda_connection_statement_execute_select (cnc, stmt, NULL, &error);
	g_object_unref (stmt);
	if (!data_model) {
		char *error_mes;
		error_mes= g_strdup_printf( "Could not get the contents of the table: %s\n",
		                            error && error->message ? error->message : "No detail");

		db_PRINT_ERROR(error_mes,LineNr);
		g_free(error_mes);
		return NULL;
	}
	return data_model;
}

GDate* _db_get_date (GdaDataModel *data_model,int col, int LineNr) {
	return _db_get_date_row (data_model,col,0,LineNr);
}

GDate* _db_get_date_row (GdaDataModel *data_model,int col,int row, int LineNr) {
	GDate *g_date,*g_date2;
	const GValue *value;
	gchar *S;
	GError *error = NULL;
	value = gda_data_model_get_typed_value_at(data_model,col,row,G_TYPE_DATE,FALSE,&error);

	// OK Datum ist nicht wie in SQL Lite sondern hat z.B. Access Format: 1991-08-15 00:00:00+1
	if (error!=NULL) {
		error=NULL;
		value = gda_data_model_get_value_at (data_model,col,row,&error);

		if (error!=NULL) {
			char *error_mes;
			error_mes= g_strdup_printf( "%s\n\n\n",
			                            error && error->message ? error->message : "No detail");
			db_PRINT_ERROR(error_mes,LineNr);
			g_free(error_mes);
			gda_data_model_dump(data_model,stdout);
			return NULL;
		}
		// Jetzt muss wohl selbst geparst werden:
		S = g_strdup_value_contents  (value);
		//printf("%s\n",g_strdup_value_contents  (value));
		if (S==NULL) {
			db_PRINT_ERROR("Error with Date",LineNr);
		}
		g_date = convert_str_to_gdate(S);
		g_free(S);
		return g_date;
	} else {

		g_date=NULL;
		g_date2=NULL;
		g_date=g_value_get_boxed(value);

		if (g_date!=NULL) {
			g_date2 = (GDate*)g_malloc(sizeof(GDate));
			memcpy(g_date2,g_date,sizeof(GDate));
		}
		return g_date2;
	}
}




char* _db_get_string_row (GdaDataModel *data_model,int col,int row, int LineNr) {
	const GValue *value;
	GError *error = NULL;
	const char *S1;
	char *S2;
	value = gda_data_model_get_typed_value_at(data_model,col,row,G_TYPE_STRING,FALSE,&error);
	if (error!=NULL) {
		char *error_mes;
		error_mes= g_strdup_printf( "%s\n\n\n",
		                            error && error->message ? error->message : "No detail");
		db_PRINT_ERROR(error_mes,LineNr);
		g_free(error_mes);
		gda_data_model_dump(data_model,stdout);
		return NULL;
	}
	S1=NULL;
	S2=NULL;
	S1 = g_value_get_string(value);
	if (S1!=NULL) {
		S2 = (char*)g_malloc(strlen(S1)+1);
		strcpy(S2,S1);
	}
	return S2;
}

char* _db_get_string(GdaDataModel *data_model,int col,int LineNr) {
	return _db_get_string_row (data_model,col,0,LineNr);
}


double _db_get_double_row(GdaDataModel *data_model,int col,int row,int LineNr) {
	const GValue *value;
	GError *error = NULL;
	value = gda_data_model_get_typed_value_at(data_model,col,row,G_TYPE_DOUBLE,FALSE,&error);
	if (error!=NULL) {
		char *error_mes;
		error_mes= g_strdup_printf( "%s\n\n\n",
		                            error && error->message ? error->message : "No detail");
		db_PRINT_ERROR(error_mes,LineNr);
		g_free(error_mes);
		gda_data_model_dump(data_model,stdout);
		return 0.0;
	}
	return g_value_get_double(value);
}

double _db_get_double(GdaDataModel *data_model,int col,int LineNr) {
	return _db_get_double_row(data_model,col,0,LineNr);
}

int _db_get_int_row(GdaDataModel *data_model,int col,int row,int LineNr) {
	const GValue *value;
	GError *error = NULL;
	value = gda_data_model_get_typed_value_at(data_model,col,row,G_TYPE_INT,FALSE,&error);
	if (error!=NULL) {
		char *error_mes;
		error_mes= g_strdup_printf( "%s\n\n\n",
		                            error && error->message ? error->message : "No detail");
		db_PRINT_ERROR(error_mes,LineNr);
		g_free(error_mes);
		gda_data_model_dump(data_model,stdout);
		return 0;
	}
	return g_value_get_int(value);
}

int _db_get_int(GdaDataModel *data_model,int col,int LineNr) {
	return _db_get_int_row(data_model,col,0,LineNr);
}

#endif
// Weitere Funktionen:


// Tairmax=(double)pCl->pWeather->fTempMax;
// Tairmin=(double)pCl->pWeather->fTempMin;
// DayTime=double)Time;
// Latitude = (double) pLo->pFarm->fLatitude
// JulianDay =  pTi->pSimTime->iJulianDay

double calculateAirTemperature(double DayTime, double Tairmin, double Tairmax,double TairLastDayMax, double TairNextDayMin, double Latitude, int JulianDay) {
	double DayLength;
	double Tair=0.0; //ret
	double Tairmean ;
	double Tairvar;
	double tdawn;
	double tofday;
	double intpart;
	double sinf;

	

	Tairmean = 0.5*(Tairmax+Tairmin);

	DayLength = calculateDayLength(Latitude, JulianDay );

	Tairvar = 0.5*(Tairmax-Tairmin);

	tdawn = 0.5*(1.0 - DayLength);
	tofday = modf(DayTime,&intpart);


	if ((tofday >= 0.0) && (tofday <= tdawn)) {
		Tairvar = 0.5*( TairLastDayMax-Tairmin);
		Tairmean = 0.5*(TairLastDayMax+Tairmin);
		sinf = sin(2.0*M_PI*(tofday + 1.0 - 0.5 * (1.0+tdawn + 0.625))/ (2.0*(1.0+tdawn-0.625)));
		Tair = Tairmean - Tairvar*sinf;
	}
	if ((tofday > tdawn) && (tofday < 0.625)) {
		sinf = sin(2.0*M_PI*(tofday - 0.5 * (tdawn + 0.625))/ (2.0*(0.625-tdawn)));
		Tair = Tairmean + Tairvar*sinf;
	}
	if ((tofday >= 0.625) && (tofday < 1.0)) {
		Tairvar = 0.5*( Tairmax-TairNextDayMin);
		Tairmean = 0.5*(Tairmax+TairNextDayMin);
		sinf = sin(2.0*M_PI*(tofday - 0.5 * (1.0 + tdawn + 0.625))/ (2.0*(1.0+tdawn-0.625)));
		Tair = Tairmean - Tairvar*sinf;
	}


	return Tair;

}



////////////////////
// Name:    SoilCode(float fClay, float fSilt, float fSand, char acSoilCode)
//
// Methode: Weist anhand der Texturdaten die ADV-Kennung
//          des Bodens nach der Bodenkartieranleitung zu
//
// Author:  ep/gsf 150399
/////////////////////////////////////////

int	SoilCode(double fClay, double fSilt, double fSand, char* acSoilCode) {

	if ((fClay>=0)&&(fClay<5)&&(fSilt>=0)&&(fSilt<10)) {
		strcpy(acSoilCode,"S");
		return 1;
	}  else

		if ((fClay>=0)&&(fClay<5)&&(fSilt>=10)&&(fSilt<25)) {
			strcpy(acSoilCode,"Su2");
			return 2;
		}  else

			if ((fClay>=0)&&(fClay<8)&&(fSilt>=25)&&(fSilt<40)) {
				strcpy(acSoilCode,"Su3");
				return 3;
			}  else

				if ((fClay>=0)&&(fClay<8)&&(fSilt>=40)&&(fSilt<50)) {
					strcpy(acSoilCode,"Su4");
					return 4;
				}  else

					if ((fClay>=8)&&(fClay<15)&&(fSilt>=40)&&(fSilt<50)) {
						strcpy(acSoilCode,"Slu");
						return 5;
					}  else

						if ((fClay>=5)&&(fClay<8)&&(fSilt>=5)&&(fSilt<25)) {
							strcpy(acSoilCode,"Sl2");
							return 6;
						}  else

							if ((fClay>=8)&&(fClay<12)&&(fSilt>=7)&&(fSilt<40)) {
								strcpy(acSoilCode,"Sl3");
								return 7;
							}  else

								if (((fClay>=12)&&(fClay<15)&&(fSilt>=13)&&(fSilt<40))
								        ||((fClay>=15)&&(fClay<17)&&(fSilt>=13)&&(fSilt<35))) {
									strcpy(acSoilCode,"Sl4");
									return 8;
								}  else

									if (((fClay>=5)&&(fClay<8)&&(fSilt>=0)&&(fSilt<5))
									        ||((fClay>=8)&&(fClay<12)&&(fSilt>=0)&&(fSilt<7))
									        ||((fClay>=12)&&(fClay<15)&&(fSilt>=0)&&(fSilt<13))) {
										strcpy(acSoilCode,"St2");
										return 9;
									}  else

										if (((fClay>=15)&&(fClay<17)&&(fSilt>=0)&&(fSilt<13))
										        ||((fClay>=17)&&(fClay<25)&&(fSilt>=0)&&(fSilt<15))) {
											strcpy(acSoilCode,"St3");
											return 10;
										}  else

											if ((fClay>=0)&&(fClay<8)&&(fSilt>=80)&&(fSilt<=100)) {
												strcpy(acSoilCode,"U");
												return 11;
											}

	if ((fClay>=0)&&(fClay<8)&&(fSilt>=50)&&(fSilt<80)) {
		strcpy(acSoilCode,"Us");
		return 12;
	}  else

		if ((fClay>=8)&&(fClay<17)&&(fSilt>=50)&&(fSilt<65)) {
			strcpy(acSoilCode,"Uls");
			return 13;
		}  else

			if ((fClay>=8)&&(fClay<12)&&(fSilt>=65)&&(fSilt<92)) {
				strcpy(acSoilCode,"Ul2");
				return 14;
			}  else

				if ((fClay>=12)&&(fClay<17)&&(fSilt>=65)&&(fSilt<88)) {
					strcpy(acSoilCode,"Ul3");
					return 15;
				}  else

					if ((fClay>=17)&&(fClay<30)&&(fSilt>=70)&&(fSilt<83)) {
						strcpy(acSoilCode,"Ul4");
						return 16;
					}  else

						if ((fClay>=15)&&(fClay<25)&&(fSilt>=40)&&(fSilt<50)) {
							strcpy(acSoilCode,"Ls2");
							return 20;
						}  else

							if (((fClay>=15)&&(fClay<17)&&(fSilt>=35)&&(fSilt<40))
							        ||((fClay>=17)&&(fClay<25)&&(fSilt>=28)&&(fSilt<40))) {
								strcpy(acSoilCode,"Ls3");
								return 21;
							}  else

								if ((fClay>=17)&&(fClay<25)&&(fSilt>=15)&&(fSilt<28)) {
									strcpy(acSoilCode,"Ls4");
									return 22;
								}  else

									if ((fClay>=17)&&(fClay<30)&&(fSilt>=50)&&(fSilt<70)) {
										strcpy(acSoilCode,"Lu");
										return 23;
									}  else

										if ((fClay>=25)&&(fClay<35)&&(fSilt>=35)&&(fSilt<50)) {
											strcpy(acSoilCode,"Lt2");
											return 24;
										}  else

											if ((fClay>=35)&&(fClay<45)&&(fSilt>=30)&&(fSilt<50)) {
												strcpy(acSoilCode,"Lt3");
												return 25;
											}  else

												if ((fClay>=30)&&(fClay<45)&&(fSilt>=50)&&(fSilt<70)) {
													strcpy(acSoilCode,"Ltu");
													return 26;
												}  else

													if (((fClay>=25)&&(fClay<35)&&(fSilt>=18)&&(fSilt<35))
													        ||((fClay>=35)&&(fClay<45)&&(fSilt>=18)&&(fSilt<30))) {
														strcpy(acSoilCode,"Lts");
														return 27;
													}  else

														if ((fClay>=50)&&(fClay<65)&&(fSilt>=0)&&(fSilt<18)) {
															strcpy(acSoilCode,"Ts2");
															return 28;
														}  else

															if ((fClay>=35)&&(fClay<50)&&(fSilt>=0)&&(fSilt<18)) {
																strcpy(acSoilCode,"Ts3");
																return 29;
															}  else

																if ((fClay>=25)&&(fClay<35)&&(fSilt>=0)&&(fSilt<18)) {
																	strcpy(acSoilCode,"Ts4");
																	return 30;
																} else

																	if ((fClay>=45)&&(fClay<65)&&(fSilt>=18)&&(fSilt<55)) {
																		strcpy(acSoilCode,"Tl");
																		return 34;
																	}  else

																		if ((fClay>=65)&&(fClay<=100)&&(fSilt>=0)&&(fSilt<35)) {
																			strcpy(acSoilCode,"T");
																			return 35;
																		}

	return -1;
}

//////////////////////////////////////////////////////
// Name:    GPV_FK_PWP(float fClay, float fBulkDens,
//                     float fHumus, float fStone,
//      			   char acSoilCode, int iRet)
//
// Methode: Berechnet anhand der Texturdaten
//          und der Boden-ADV-Kennung
//
//          das GesamtPorenVolumen GPV
//          die FeldKapazitaet FK
//          den PermanentenWelkePunkt PWP
//
// Author: ep/gsf 250299
/////////////////////////////////////////////////////

double GPV_FK_PWP(double fClay,double fBulkDens,double fHumus, double fStone,
                  char* acSoilCode,int iRet) {
	double fRet;
//float fGPV,fFK,fPWP;
	int iRetValue1,iRetValue2,iFK,iFKK;
	int iBD,iClay;

	iBD = BD_Class(fBulkDens);
	iRetValue1=CapacityParam1(iBD,acSoilCode,iRet);

	iClay = Clay_Class(fClay);
	iRetValue2=CapacityParam2(iClay,fHumus,iRet);

	if (iRet < 3) fRet=((double)iRetValue1 + (double)iRetValue2)*((double)1 - fStone);
	else if (iRet==3) {
		iFK=CapacityParam1(iBD,acSoilCode,2);
		iFKK=CapacityParam2(iClay,fHumus,2);

		fRet=((double)iFK+(double)iFKK)*((double)1 - fStone)
		     -((double)iFK-(double)iRetValue1+(double)iRetValue2)*((double)1 - fStone);
	}

	/*
	if      (iRet==1) fRet=fGPV;
	else if (iRet==2) fRet=fFK;
	else if (iRet==3) fRet=fPWP;
	*/
	return fRet;
}

/**************************************************************
 * int BD_Class(float fVal)                                   *
 *                                                            *
 * Methode: Weist anhand der Lagerungsdichte                  *
 *          die Lagerungsdichteklasse LDi zu				  *
 *															  *
 * Author: mb/tum 93, angepasst ep/gsf 250299				  *
 **************************************************************/

int BD_Class(double fVal) {

	if(fVal > (double)0   && fVal <= (double)0.6)
		return 1;									/* Ld1		*/
	if(fVal > (double)0.6 && fVal <= (double)1.3)
		return 2;                       			/* Ld2		*/
	if(fVal > (double)1.3 && fVal <= (double)1.6)
		return 3;                       			/* Ld3		*/
	if(fVal > (double)1.6 && fVal <= (double)1.8)
		return 4;                       			/* Ld4		*/
	if(fVal > (double)1.8)
		return 5;                       			/* Ld5		*/

	return 0;

} /* End of BD_Class() */

/****************************************************************
* int CapacityParam1(int iLd, char acSoilCode, int iReturnType) *
*															    *
* Methode: Weist anhand der BodenKennung und Lagerungsdichte	*
*          noch zu korrigierende Werte zu fuer					*
*          														*
*          das GesamtPorenVolumen GPV							*
*          die FeldKapazitaet FK								*
*          den PermanentenWelkePunkt PWP						*
*																*
* Author: mb/tum 93, angepasst ep/gsf 250299				    *
*																*
****************************************************************/

int CapacityParam1(int iLd, char* acSoilCode, int iReturnType) {
	int	iReturnVal;
	int	iNfk;
	int	iFk;
	int    iSwitch;
// char acSoilCode[6];

	iReturnVal = 0;

	if      (iLd < 3) iSwitch=1;
	else if (iLd == 3) iSwitch=2;
	else if (iLd > 3) iSwitch=3;

	switch(iSwitch) {
	case 1:									// Ld1 & Ld2
		if (!strcmp(acSoilCode,"S\0")) {
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 14;
			if(iReturnType == 3) {
				iFk  = 14;
				iNfk = 10;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Su2\0"))||
		        (!strcmp(acSoilCode,"Su3\0"))||
		        (!strcmp(acSoilCode,"Su4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3) {
				iFk  = 31;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl2\0")) {
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3) {
				iFk  = 27;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Slu\0")) {
			if(iReturnType == 1)
				iReturnVal = 49;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3) {
				iFk  = 34;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl3\0")) {
			if(iReturnType == 1)
				iReturnVal = 51;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3) {
				iFk  = 32;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl4\0")) {
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3) {
				iFk  = 34;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"St2\0")) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 29;
			if(iReturnType == 3) {
				iFk  = 29;
				iNfk = 18;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"St3\0")) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3) {
				iFk  = 32;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"U\0")) {
			if(iReturnType == 1)
				iReturnVal = 51;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 28;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Us\0")) {
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 35;
			if(iReturnType == 3) {
				iFk  = 35;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ul2\0")) {
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3) {
				iFk  = 38;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ul3\0")) {
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3) {
				iFk  = 40;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ul4\0")) {
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3) {
				iFk  = 40;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Uls\0")) {
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ut2\0")) {
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3) {
				iFk  = 38;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ut3\0")) {
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3) {
				iFk  = 40;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ut4\0")) {
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3) {
				iFk  = 40;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls2\0")) {
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3) {
				iFk  = 38;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls3\0")) {
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3) {
				iFk  = 38;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls4\0")) {
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lu\0")) {
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3) {
				iFk  = 40;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lt2\0")) {
			if(iReturnType == 1)
				iReturnVal = 56;
			if(iReturnType == 2)
				iReturnVal = 46;
			if(iReturnType == 3) {
				iFk  = 46;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lt3\0")) {
			if(iReturnType == 1)
				iReturnVal = 56;
			if(iReturnType == 2)
				iReturnVal = 46;
			if(iReturnType == 3) {
				iFk  = 46;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ltu\0")) {
			if(iReturnType == 1)
				iReturnVal = 57;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3) {
				iFk  = 47;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lts\0")) {
			if(iReturnType == 1)
				iReturnVal = 58;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3) {
				iFk  = 47;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		/* Ts2, Ts3, Ts4 nicht in Tab. 43, AG Bodenkunde 1982 enthalten ! */
		/* Werte übernommen von Lts ! */
		if ((!strcmp(acSoilCode,"Ts2\0"))||
		        (!strcmp(acSoilCode,"Ts3\0"))||
		        (!strcmp(acSoilCode,"Ts4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 58;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3) {
				iFk  = 47;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!strcmp(acSoilCode,"Tu4\0")) {
			if(iReturnType == 1)
				iReturnVal = 57;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3) {
				iFk  = 47;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!strcmp(acSoilCode,"Tu3\0")) {
			if(iReturnType == 1)
				iReturnVal = 56;
			if(iReturnType == 2)
				iReturnVal = 46;
			if(iReturnType == 3) {
				iFk  = 46;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Tu2\0"))||
		        (!strcmp(acSoilCode,"Tl\0"))) {
			if(iReturnType == 1)
				iReturnVal = 63;
			if(iReturnType == 2)
				iReturnVal = 55;
			if(iReturnType == 3) {
				iFk  = 55;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"T\0")) {
			if(iReturnType == 1)
				iReturnVal = 66;
			if(iReturnType == 2)
				iReturnVal = 59;
			if(iReturnType == 3) {
				iFk  = 59;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		break;



	case 2:                                 // Ld3

		if (!strcmp(acSoilCode,"S\0")) {
			if(iReturnType == 1)
				iReturnVal = 36;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3) {
				iFk  = 12;
				iNfk = 9;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Su2\0"))||
		        (!strcmp(acSoilCode,"Su3\0"))||
		        (!strcmp(acSoilCode,"Su4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 24;
			if(iReturnType == 3) {
				iFk  = 24;
				iNfk = 18;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl2\0")) {
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 22;
			if(iReturnType == 3) {
				iFk  = 22;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Slu\0")) {
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 30;
			if(iReturnType == 3) {
				iFk  = 30;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl3\0")) {
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3) {
				iFk  = 27;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl4\0")) {
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 28;
			if(iReturnType == 3) {
				iFk  = 28;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"St2\0")) {
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 22;
			if(iReturnType == 3) {
				iFk  = 22;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"St3\0")) {
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3) {
				iFk  = 27;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"U\0")) {
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3) {
				iFk  = 34;
				iNfk = 25;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Us\0")) {
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3) {
				iFk  = 33;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ul2\0")) {
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3) {
				iFk  = 36;
				iNfk = 25;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Uls\0")) {
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3) {
				iFk  = 33;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ul3\0")) {
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ul4\0")) {
			if(iReturnType == 1)
				iReturnVal = 45;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ut2\0")) {
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3) {
				iFk  = 36;
				iNfk = 25;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ut3\0")) {
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ut4\0")) {
			if(iReturnType == 1)
				iReturnVal = 45;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls2\0")) {
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3) {
				iFk  = 33;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls3\0")) {
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3) {
				iFk  = 33;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls4\0")) {
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3) {
				iFk  = 32;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lu\0")) {
			if(iReturnType == 1)
				iReturnVal = 45;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3) {
				iFk  = 36;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lt2\0")) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3) {
				iFk  = 41;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lt3\0")) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3) {
				iFk  = 41;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ltu\0")) {
			if(iReturnType == 1)
				iReturnVal = 49;
			if(iReturnType == 2)
				iReturnVal = 42;
			if(iReturnType == 3) {
				iFk  = 42;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lts\0")) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3) {
				iFk  = 41;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		/* Ts2, Ts3, Ts4 nicht in Tab. 43, AG Bodenkunde 1982 enthalten ! */
		/* Werte übernommen von Lts ! */
		if ((!strcmp(acSoilCode,"Ts2\0"))||
		        (!strcmp(acSoilCode,"Ts3\0"))||
		        (!strcmp(acSoilCode,"Ts4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3) {
				iFk  = 41;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!strcmp(acSoilCode,"Tu4\0")) {
			if(iReturnType == 1)
				iReturnVal = 49;
			if(iReturnType == 2)
				iReturnVal = 42;
			if(iReturnType == 3) {
				iFk  = 42;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Tu3\0")) {
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3) {
				iFk  = 41;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Tu2\0"))||
		        (!strcmp(acSoilCode,"Tl\0"))) {
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 49;
			if(iReturnType == 3) {
				iFk  = 49;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"T\0")) {
			if(iReturnType == 1)
				iReturnVal = 58;
			if(iReturnType == 2)
				iReturnVal = 54;
			if(iReturnType == 3) {
				iFk  = 54;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		break;


	case 3:                                 // Ld4 & Ld5

		if (!strcmp(acSoilCode,"S\0")) {
			if(iReturnType == 1)
				iReturnVal = 31;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3) {
				iFk  = 12;
				iNfk = 9;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if ((!strcmp(acSoilCode,"Su2\0"))||
		        (!strcmp(acSoilCode,"Su3\0"))||
		        (!strcmp(acSoilCode,"Su4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 33;
			if(iReturnType == 2)
				iReturnVal = 24;
			if(iReturnType == 3) {
				iFk  = 24;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl2\0")) {
			if(iReturnType == 1)
				iReturnVal = 32;
			if(iReturnType == 2)
				iReturnVal = 21;
			if(iReturnType == 3) {
				iFk  = 21;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Slu\0")) {
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3) {
				iFk  = 27;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl3\0")) {
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 26;
			if(iReturnType == 3) {
				iFk  = 26;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Sl4\0")) {
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3) {
				iFk  = 27;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"St2\0")) {
			if(iReturnType == 1)
				iReturnVal = 33;
			if(iReturnType == 2)
				iReturnVal = 20;
			if(iReturnType == 3) {
				iFk  = 20;
				iNfk = 13;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"St3\0")) {
			if(iReturnType == 1)
				iReturnVal = 30;
			if(iReturnType == 2)
				iReturnVal = 22;
			if(iReturnType == 3) {
				iFk  = 22;
				iNfk = 13;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"U\0")) {
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3) {
				iFk  = 31;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Us\0")) {
			if(iReturnType == 1)
				iReturnVal = 36;
			if(iReturnType == 2)
				iReturnVal = 29;
			if(iReturnType == 3) {
				iFk  = 29;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Ul2\0"))||
		        (!strcmp(acSoilCode,"Ut2\0"))) {
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3) {
				iFk  = 32;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Uls\0")) {
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 30;
			if(iReturnType == 3) {
				iFk  = 30;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Ul3\0"))||
		        (!strcmp(acSoilCode,"Ut3\0"))) {
			if(iReturnType == 1)
				iReturnVal = 39;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3) {
				iFk  = 34;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Ul4\0"))||
		        (!strcmp(acSoilCode,"Ut4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 35;
			if(iReturnType == 3) {
				iFk  = 35;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls2\0")) {
			if(iReturnType == 1)
				iReturnVal = 36;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3) {
				iFk  = 31;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls3\0")) {
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3) {
				iFk  = 31;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ls4\0")) {
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 30;
			if(iReturnType == 3) {
				iFk  = 30;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lu\0")) {
			if(iReturnType == 1)
				iReturnVal = 38;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3) {
				iFk  = 33;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lt2\0")) {
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3) {
				iFk  = 36;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lt3\0")) {
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3) {
				iFk  = 36;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Ltu\0")) {
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3) {
				iFk  = 38;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Lts\0")) {
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		/* Ts2, Ts3, Ts4 nicht in Tab. 43, AG Bodenkunde 1982 enthalten ! */
		/* Werte übernommen von Lts ! */
		if ((!strcmp(acSoilCode,"Ts2\0"))||
		        (!strcmp(acSoilCode,"Ts3\0"))||
		        (!strcmp(acSoilCode,"Ts4\0"))) {
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3) {
				iFk  = 37;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!strcmp(acSoilCode,"Tu4\0")) {
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3) {
				iFk  = 38;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"Tu3\0")) {
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3) {
				iFk  = 36;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!strcmp(acSoilCode,"Tu2\0"))||
		        (!strcmp(acSoilCode,"Tl\0"))) {
			if(iReturnType == 1)
				iReturnVal = 47;
			if(iReturnType == 2)
				iReturnVal = 45;
			if(iReturnType == 3) {
				iFk  = 45;
				iNfk = 11;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!strcmp(acSoilCode,"T\0")) {
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 49;
			if(iReturnType == 3) {
				iFk  = 49;
				iNfk = 11;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		break;


	}

	return iReturnVal;
} /* End of CapacityParam1() */

/**************************************************************
* int Clay_Class(double fVal)                                  *
*						                                      *
* Methode: Weist anhand des Tongehalts 						  *
*          die Tongehaltsklasse zu							  *
*															  *
* Author:  mb/tum 93, angepasst ep/gsf 250299      			  *
*															  *
**************************************************************/

int Clay_Class(double fVal) {
	fVal=fVal*(double)100;

	if(fVal > (double)0   && fVal < (double)5)
		return 1;									// Tongehalt < 5 %
	if(fVal >= (double)5  && fVal < (double)12)
		return 2;                       			// Tongehalt 5-12%
	if(fVal >= (double)12 && fVal < (double)17)
		return 3;                       			// Tongehalt 12-17%
	if(fVal >= (double)17 && fVal < (double)35)
		return 4;                       			// Tongehalt 17-35%
	if(fVal >= (double)35 && fVal < (double)65)
		return 5;                       			// Tongehalt 35-65%
	if(fVal >= (double)65)
		return 6;                       			// Tongehalt > 65%

	return 0;

} /* End of Clay_Class() */

/************************************************************/
/* int CapacityParam2(int		iTon,      					*/
/*					  float		fOs,						*/
/*					  int		iReturnType)				*/
/*															*/
/* Methode: Berechnet die Zu-/Ab-Schlaege fuer  			*/
/*          das GesamtPorenVolumen GPV						*/
/*          die FeldKapazitaet FK							*/
/*          die nutzbare FeldKapazitaet nFK  				*/
/*															*/
/* Author:  mb/tum 93, angepasst ep/gsf 250299 	   			*/
/************************************************************/

int CapacityParam2(int iTon, double fOs, int iReturnType) {
	int	iReturnVal;

	fOs=fOs*(double)100;
	iReturnVal = 0;

	switch(iTon) {
	case 1:									// Tongehalt < 5 %
		if(fOs >= 2 && fOs < 4) {			// 2% OS
			if(iReturnType == 1)
				iReturnVal = 1;				// Zu-/Abschlag Gpv
			if(iReturnType == 2)
				iReturnVal = 1;             // Zu-/Abschlag Fk
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 4 && fOs < 6) {			// 4% OS
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 3;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8) {			// 6% OS
			if(iReturnType == 1)
				iReturnVal = 5;
			if(iReturnType == 2)
				iReturnVal = 7;
			if(iReturnType == 3)
				iReturnVal = 7;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10) {			// 8% OS
			if(iReturnType == 1)
				iReturnVal = 9;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12) {		// 10% OS
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 15;
			if(iReturnType == 3)
				iReturnVal = 12;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14) {		// 12% OS
			if(iReturnType == 1)
				iReturnVal = 14;
			if(iReturnType == 2)
				iReturnVal = 19;
			if(iReturnType == 3)
				iReturnVal = 14;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14) {					// 14% OS
			if(iReturnType == 1)
				iReturnVal = 17;
			if(iReturnType == 2)
				iReturnVal = 23;
			if(iReturnType == 3)
				iReturnVal = 16;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 2:									// Tongehalt 5-12%
		if(fOs >= 2 && fOs < 4) {			// 2% OS
			if(iReturnType == 1)
				iReturnVal = 1;
			if(iReturnType == 2)
				iReturnVal = 1;
			if(iReturnType == 3)
				iReturnVal = 1;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 4 && fOs < 6) {			// 4% OS
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 3;
			if(iReturnType == 3)
				iReturnVal = 3;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8) {			// 6% OS
			if(iReturnType == 1)
				iReturnVal = 4;
			if(iReturnType == 2)
				iReturnVal = 6;
			if(iReturnType == 3)
				iReturnVal = 5;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10) {		// 8% OS
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 10;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12) {		// 10% OS
			if(iReturnType == 1)
				iReturnVal = 10;
			if(iReturnType == 2)
				iReturnVal = 13;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14) {		// 12% OS
			if(iReturnType == 1)
				iReturnVal = 13;
			if(iReturnType == 2)
				iReturnVal = 17;
			if(iReturnType == 3)
				iReturnVal = 13;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14) {					// 14% OS
			if(iReturnType == 1)
				iReturnVal = 16;
			if(iReturnType == 2)
				iReturnVal = 21;
			if(iReturnType == 3)
				iReturnVal = 15;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 3:                                 // Tongehalt 12-17%
		if(fOs >= 4 && fOs < 6) {			// 4% OS
			if(iReturnType == 1)
				iReturnVal = 1;
			if(iReturnType == 2)
				iReturnVal = 2;
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8) {			// 6% OS
			if(iReturnType == 1)
				iReturnVal = 4;
			if(iReturnType == 2)
				iReturnVal = 6;
			if(iReturnType == 3)
				iReturnVal = 5;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10) {		// 8% OS
			if(iReturnType == 1)
				iReturnVal = 7;
			if(iReturnType == 2)
				iReturnVal = 9;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12) {		// 10% OS
			if(iReturnType == 1)
				iReturnVal = 9;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14) {		// 12% OS
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 15;
			if(iReturnType == 3)
				iReturnVal = 12;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14) {					// 14% OS
			if(iReturnType == 1)
				iReturnVal = 15;
			if(iReturnType == 2)
				iReturnVal = 20;
			if(iReturnType == 3)
				iReturnVal = 15;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 4:                                 // Tongehalt 17-35%
		if(fOs >= 4 && fOs < 6) {			// 4% OS
			if(iReturnType == 1)
				iReturnVal = 0;
			if(iReturnType == 2)
				iReturnVal = 1;
			if(iReturnType == 3)
				iReturnVal = 1;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8) {			// 6% OS
			if(iReturnType == 1)
				iReturnVal = 3;
			if(iReturnType == 2)
				iReturnVal = 4;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10) {		// 8% OS
			if(iReturnType == 1)
				iReturnVal = 6;
			if(iReturnType == 2)
				iReturnVal = 7;
			if(iReturnType == 3)
				iReturnVal = 6;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12) {		// 10% OS
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 9;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14) {		// 12% OS
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14) {					// 14% OS
			if(iReturnType == 1)
				iReturnVal = 14;
			if(iReturnType == 2)
				iReturnVal = 15;
			if(iReturnType == 3)
				iReturnVal = 12;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 5:                                 // Tongehalt 35-65%
		if(fOs >= 6 && fOs < 8) {			// 6% OS
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 2;
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10) {		// 8% OS
			if(iReturnType == 1)
				iReturnVal = 5;
			if(iReturnType == 2)
				iReturnVal = 4;
			if(iReturnType == 3)
				iReturnVal = 3;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12) {		// 10% OS
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 5;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14) {		// 12% OS
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 7;
			if(iReturnType == 3)
				iReturnVal = 6;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14) {					// 14% OS
			if(iReturnType == 1)
				iReturnVal = 13;
			if(iReturnType == 2)
				iReturnVal = 9;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 6:                                 // Tongehalt >65%
		if(fOs >= 6 && fOs < 8) {			// 6% OS
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 2;
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10) {		// 8% OS
			if(iReturnType == 1)
				iReturnVal = 5;
			if(iReturnType == 2)
				iReturnVal = 3;
			if(iReturnType == 3)
				iReturnVal = 3;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12) {		// 10% OS
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 4;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14) {		// 12% OS
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 5;
			if(iReturnType == 3)
				iReturnVal = 5;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14) {					// 14% OS
			if(iReturnType == 1)
				iReturnVal = 13;
			if(iReturnType == 2)
				iReturnVal = 6;
			if(iReturnType == 3)
				iReturnVal = 6;              // Zu-/Abschlag nFk
			break;
		}
		break;
	}

	return iReturnVal;

} /* End of CapacityParam2() */

/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double  abspowerDBL(double x, double y) {

	double sign,z;
	if (x < 0.0) {
		sign = -1.0;
		x  *= -1.0;
	}

	if (y < 0.0) {
		y *= -1.0;
		if (x != (double) 0.0)  x = 1.0/x;
	}
	z = (double)pow((double)x,(double)y);
	return z;
}


// T = Temperature in Grad C
// return vapor pressure of water [hPa]
double get_e(double T) {
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
double get_Ph20(double q,double P) {
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
double calc_rel_hum(double q,double T,double P) {
	double PH20;
	PH20 = get_Ph20(q,P);
	return PH20 / get_e(T) * 100.0;
}

// distributes SolRad over the day, possible to do with three different methods:
// constant distribution, triangular approach and sine curve approach
// result:  // MJ / m2 / day Globalstrahlung = SOLDN + LWDN (langwelliger Lux + kurwelliger Lux)
double get_SolRad(double SolRad, double iJulianDay, double Latitude, double timeday, double dt) {
	double soldec_ss;	// declination at summer solstice [rad]
	double hr_ss, hr_ws, hr_sunrise, hr_sunset;		// hour of sunrise at summer solstice
	double m, SinFun;			// intermediate variables
	double hrMax, hrMin, hAmp;	// maximal, minimal day length and day length amplitude [hour]
	double HrsPerDay;
	double daylength;

	double hr_act = 0;	// time after sunrise, 0...daylength [hr]

	double Rs, Rs_mean;
	double seas_day;	// seasonal day
	double soldec;	//solar declination, elevation, elevation at noon [rad]
	double hr;


	double lat;			// latitude [rad]

	hr = timeday*24.0;

	// general parameters
	Rs = 0.0;
	Rs_mean = SolRad;

	lat = Latitude* M_PI/180.0;
	HrsPerDay = 24.0;
	seas_day = ((double)iJulianDay+10.0)/365.0;
	soldec = (-23.4*cos(2.0*M_PI*seas_day))*M_PI/180.0;

	soldec_ss = 0.40841;
	m = sin(lat)*sin(soldec_ss)/(cos(lat)*cos(soldec_ss));	// eq. 45 --> Bossel96 TREEDYN 3 (forest simulation model)
	if((m*m) < 0.0001) {
		hr_ss = 6.0;
	} else {
		// hr_ss = (12.0/M_PI)*atan(sqrt(1.0-(m*m))/m);			// eq. 44
		hr_ss = abs((12.0/M_PI)*atan(sqrt(1.0-(m*m))/m)); //Correted by EP and FH on 20190121 for southern hemisphere
	}

	// calculating day length
	hrMax = HrsPerDay - (2.0*hr_ss);					// eq. 46
	hrMin = 2.0*hr_ss;									// eq. 47
	hr_ws = (HrsPerDay/2.0)-hr_ss;
	hAmp = HrsPerDay-(4.0*hr_ss);						// eq. 48

	SinFun = sin(2*M_PI*seas_day-(M_PI/2.0));				// pt of eq. 49, 54
	daylength = 12.0+(hAmp/2.0)*SinFun;					// eq. 49

	hr_sunrise = 12.0-(daylength/2.0);
	hr_sunset = 12.0+(daylength/2.0);
	hr_act = hr-hr_sunrise;

	if((hr >= hr_sunrise) && (hr < (hr_sunrise+daylength))) {
		Rs = ((M_PI/2)*(Rs_mean)/daylength)*sin(M_PI*hr_act/daylength)*24.0;

		//Rs = Rs_mean * PI*PI * cos(PI*hr_act/daylength) / 2.0 / (daylength*daylength);
		//Rs = Rs_mean/2.0 * cos(PI*hr_act/daylength);
	} else {
		Rs = 0.0;
	}



	return Rs;
}

double cpulse(double t,double t_begin, double period,double width) {
	double t2;
	t2 = t - period*(double)((int)(t/period));
	return ((t2 >= t_begin) && (t2 <= (t_begin+width)));
}

/*
 * Berechnet die Photosynthetische aktive Strahlung [J (PAR) m^-2 s^-1]
 *
in:
 * DayTime: Kann entweder die Tageszeit in [days] sein oder auch Simulationszeit (mit Datum, Zeit) in [days]: --> daraus wird Tageszeit berechnet
 * SunDuration: Sonnenscheindauer [h]
 * SolRad: Globalstrahlung [MJ / (m^2 * day)]
 *
 Steuerung:
 * diurnal: bewertet die Strahlung entweder mit einer Sinus funktion oder einer Schrittfunktion
 * PAR_sin: step funktion oder Sinusfunktion (==1.0)
 * light_on: Tag oder Nacht (nur fall diurnal = 1.0); wird automatisch geregelt
 *
ret:
 * Par: Photosynthetisch aktive Strahlung [ J (PAR) m^2-2 s^-1]
*/
double CalculateActiveRadiation(double DayTime, double SolRad , double fLatitude, int iJulianDay) { // 7.5a
#define twopi (M_PI * 2.0)
	double par; //return value
	double y,daylsc;
	double tofday;
	double dayldy;
	double intpart;
	double light_on;
	double DayLength;
	double SunDuration;


	SolRad  = SolRad * 1.0e6; // MJ/m2day --> J/m2day

	DayLength = calculateDayLength(fLatitude, iJulianDay) * 24.0;

	SunDuration = DayLength;

	light_on = cpulse(DayTime, 0.5 * (1.0 - DayLength/24.0) - 0.000001,1.0,DayLength/24.0);

	par = 0.0;
	daylsc = 3600.0 * SunDuration;// Daylength (secs day-1).
	if (daylsc==0.0) {
		par=0.0;

	} else {


		y = SolRad / daylsc;
		//_M("y",y);



		tofday = modf(DayTime,&intpart);
		dayldy  =  SunDuration / 24.0;
		par= light_on * y *(1.0+cos(twopi * (tofday - 0.5)/ dayldy));

	}


	return par* 1.0e-6 * 86400.0 ;
}



void expertn_database_help_func_reservere_Memory(expertn_modul_base *self, int iLayers)
{
	//int iLayers;

	PLOCATION pLo = self->pLo;
	PTIME pTi = self->pTi;
	PCHEMISTRY pCh = self->pCh;
	PCLIMATE pCl = self->pCl;
	PHEAT pHe = self->pHe;
	PMANAGEMENT pMa = self->pMa;
	PPLANT pPl = self->pPl;
	PSPROFILE pSo = self->pSo;
	PWATER pWa = self->pWa;
	xpn_register_var *var_list = self->pXSys->var_list;

	// Anzahl der Bodenschichten aus Datenbank ermitteln:
	//iLayers = expertn_database_get_iLayers(self,ProjectName);




	// Time Vars:
	if (!(self->pXSys->extern_time_management))
	{
	pTi->pSimTime= (PSIMTIME)g_malloc0_n(1,sizeof(SIMTIME));
	pTi->pTimeStep = (PTIMESTEP)g_malloc0_n(1,sizeof(TIMESTEP));
	}

	// pLo:
	if (!(self->pXSys->extern_time_management))
	{
	pLo->pFarm = (PFARM)g_malloc0_n(1,sizeof(FARM));
	pLo->pFieldplot = (PFIELDPLOT)g_malloc0_n(1,sizeof(FIELDPLOT));
	pLo->pTile = (PTILE)g_malloc0_n(1,sizeof(TILE));
	}

	// PCl:
	pCl->pWeather = (PWEATHER) g_malloc0_n(1,sizeof(WEATHER));
	pCl->pAverage = (PAVERAGE) g_malloc0_n(1,sizeof(AVERAGE));
	memset (pCl->pWeather,0,sizeof(WEATHER));

	//PCh:
	allocItemList(CLAYER,pCh->pCLayer,iLayers);
	pCh->pNMeasure = (PNMEASURE) g_malloc0_n(1,sizeof(NMEASURE));
	pCh->pCBalance = (PCBALANCE) g_malloc0_n(1,sizeof(CBALANCE));
	pCh->pCProfile = (PCPROFILE) g_malloc0_n(1,sizeof(CPROFILE));
	pCh->pAtmos = (PATMOS) g_malloc0_n(1,sizeof(ATMOS));
	allocItemList(CPARAM,pCh->pCParam,iLayers);

	// pSo:
	allocItemList(SLAYER,pSo->pSLayer,iLayers);
	allocItemList(SHEAT,pSo->pSHeat,iLayers);
	allocItemList(SWATER,pSo->pSWater,iLayers);

	// pHe:
	allocItemList(HLAYER,pHe->pHLayer,iLayers);
	pHe->pHBalance = (PHBALANCE) g_malloc0_n(1,sizeof(HBALANCE));
	pHe->pHMeasure = (PHMEASURE) g_malloc0_n(1,sizeof(HMEASURE));
	pHe->pHEBalance = (PHEBALANCE) g_malloc0_n(1,sizeof(HEBALANCE));

	// pPl:
	pPl->pOutput = (POUTPUT) g_malloc0_n(1,sizeof(OUTPUT));
	pPl->pGenotype = (PGENOTYPE) g_malloc0_n(1,sizeof(STGENOTYPE));
	pPl->pGenotype->pStageParam = (PSTAGEPARAM)g_malloc0_n(1,sizeof(STSTAGEPARAM));
	pPl->pGenotype->pVarietyInfo = (PVARIETYINFO)g_malloc0_n(1,sizeof(STVARIETYINFO));
	
	pPl->pDevelop = (PDEVELOP) g_malloc0_n(1,sizeof(STDEVELOP));
	pPl->pBiomass = (PBIOMASS) g_malloc0_n(1,sizeof(STBIOMASS));
	pPl->pCanopy = (PCANOPY) g_malloc0_n(1,sizeof(STCANOPY));
	pPl->pRoot = (PROOT) g_malloc0_n(1,sizeof(STROOT));
	allocItemList(STLAYERROOT,pPl->pRoot->pLayerRoot,iLayers);
	pPl->pPltCarbon = (PPLTCARBON) g_malloc0_n(1,sizeof(STPLTCARBON));
	pPl->pPltWater = (PPLTWATER) g_malloc0_n(1,sizeof(STPLTWATER));
	pPl->pPltNitrogen = (PPLTNITROGEN) g_malloc0_n(1,sizeof(STPLTNITROGEN));
	pPl->pPltClimate = (PPLTCLIMATE) g_malloc0_n(1,sizeof(STPLTCLIMATE));
	pPl->pModelParam = (PMODELPARAM) g_malloc0_n(1,sizeof(STMODELPARAM));
	pPl->pPMeasure = NULL;//(PPLTMEASURE) g_malloc0_n(1,sizeof(STPLTMEASURE));
	
	
	

	// pWa:
	allocItemList(WLAYER,pWa->pWLayer,iLayers);
	pWa->pWMeasure = (PWMEASURE) g_malloc0_n(1,sizeof(WMEASURE));
	pWa->pEvap = (PEVAP) g_malloc0_n(1,sizeof(EVAP));
	pWa->pWBalance = (PWBALANCE) g_malloc0_n(1,sizeof(WBALANCE));

	// pMa:
	pMa->pSowInfo=(PMSOWINFO)g_malloc0_n(1,sizeof(STLITTER));
	pMa->pLitter=(PMLITTER)g_malloc0_n(1,sizeof(STLITTER));
	pMa->pIrrigation=(PMIRRIGATION)g_malloc0_n(1,sizeof(STIRRIGATION));
	pMa->pNFertilizer=NULL;//(PNFERTILIZER)g_malloc0_n(1,sizeof(STNFERTILIZER));
	pMa->pTillage=(PTILLAGE)g_malloc0_n(1,sizeof(STTILLAGE));
//	pMa->Startvalue=(PSTARTVALUE)g_malloc0_n(1,sizeof(STSTARTVALUE));

	// Alle Variablen der Struct initialisieren und mit der Datenbank verknuepfen!
	
	if (!(self->pXSys->extern_time_management))
	{
	XTIME_REG
	}
	XCLIMATE_REG
	
	XSOIL_REG
	XHEAT_REG
	XWATER_REG
	XCHEMIST_REG
	XPLANT_REG
	pSo->iLayers = iLayers;
}

void expertn_database_help_func_set_time_step(expertn_modul_base *self,const char* local_config_str)
{
	char *S;
	S = (char*)xpn_register_var_get_pointer(self->pXSys->var_list,(char*)local_config_str);
        if (S==NULL)
            {
                S = (char*)xpn_register_var_get_pointer(self->pXSys->var_list,"Global_Config.options.time step");
                if (S==NULL)
                    {
                        char *ERROR_STR;
						S = "0.1";
						ERROR_STR = g_strdup_printf("Varname is not defined (%s or %s), take default value (%s)!\n",local_config_str,"Global_Config.options.time step",S);
                        PRINT_ERROR_ID(self,ERROR_STR);
						g_free(ERROR_STR);
                    }
            }
        self->pTi->pTimeStep->fAct = atof(S);
}

int expertn_database_help_func_Time_Message_and_get_number_of_days(expertn_modul_base *self)
{
	int number_of_days;
	char *S,*S2;
	STRUCT_XPN_TIME xpn_time;
	number_of_days=0;	
	if (self->pXSys->extern_time_management==FALSE)
		{
			self->pTi->pSimTime->fTimeY = self->pTi->pSimTime->fStart_TimeY;
			self->pTi->pSimTime->iyear =  self->pTi->pSimTime->iStart_year;
			
			xpn_time_get_struc(&xpn_time,self->pTi->pSimTime->iyear,self->pTi->pSimTime->fTimeY);

			self->pTi->pSimTime->is_leap_year = xpn_time.is_leap_year; //  FLAG: 1 or 0
			self->pTi->pSimTime->msec = xpn_time.msec;
			self->pTi->pSimTime->sec = xpn_time.sec;
			self->pTi->pSimTime->min = xpn_time.min;
			self->pTi->pSimTime->hour = xpn_time.hour;
			self->pTi->pSimTime->mday = xpn_time.mday;
			self->pTi->pSimTime->mon = xpn_time.mon;
			self->pTi->pSimTime->year = xpn_time.year;

			//self->start_time = self->pTi->pSimTime->fTime;
			S2 =  xpn_time_get_formated_date(self->pTi->pSimTime->iStart_year, self->pTi->pSimTime->fStart_TimeY,FALSE);			
			S =  g_strdup_printf("Startdate: %s",S2);
			PRINT_MESSAGE(self,4,S);
			g_free(S);
			g_free(S2);

			S2 =  xpn_time_get_formated_date(self->pTi->pSimTime->iStop_year, self->pTi->pSimTime->fStop_TimeY,FALSE);
			S =  g_strdup_printf("Stopdate: %s",S2);
			PRINT_MESSAGE(self,4,S);
			g_free(S);
			g_free(S2);

			
			

			number_of_days=xpn_time_get_number_of_days(self->pTi->pSimTime->iStart_year, self->pTi->pSimTime->fStart_TimeY,self->pTi->pSimTime->iStop_year, self->pTi->pSimTime->fStop_TimeY);		

		}
	else
		{
			//self->start_time = self->pTi->pSimTime->fTime;

			S2 =xpn_time_get_formated_date(self->pTi->pSimTime->iStart_year, self->pTi->pSimTime->fStart_TimeY,FALSE);						
			S =  g_strdup_printf("Startdate: %s",S2);
			PRINT_MESSAGE(self,3,S);
			g_free(S);
			g_free(S2);

			S2 = xpn_time_get_formated_date(self->pTi->pSimTime->iStop_year, self->pTi->pSimTime->fStop_TimeY,FALSE);
			S =  g_strdup_printf("Stopdate: %s",S2);
			PRINT_MESSAGE(self,3,S);
			g_free(S);
			g_free(S2);

			number_of_days=xpn_time_get_number_of_days(self->pTi->pSimTime->iStart_year, self->pTi->pSimTime->fStart_TimeY,self->pTi->pSimTime->iStop_year, self->pTi->pSimTime->fStop_TimeY);					
			// Verwaltung der Zeit wird von einem anderen Modul übernommen
		}
	return number_of_days;
}

int expertn_database_help_func_init_water(expertn_modul_base *self)
{
    double (*WCont)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*HCond)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*DWCap)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
    double (*MPotl)(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                    double Alpha, double N, double M, double Ca, double Cb,
                    double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                    double Alpha2, double N2, double M2, double W1, double W2);
	double (*PedoTransfer)(double dClayF, double dSiltF, double dSandF, 
                            double dBD, double dCarbonF, double dPS, int i);


    PCLIMATE pCl = self->pCl;


    PSPROFILE pSo = self->pSo;

    PWATER pWa = self->pWa;
	
	
	
    int    iSoilNr;
	 int iLayer;
    double  f1,f2,f3;
    double dClayF, dSiltF, dSandF, dBD, dSP, dCarbonF;
//   int *lpFuncSel;

    // Hydraulic Functions:


    PWLAYER pWL;
    PSLAYER pSL;
    PSWATER pSW;


    if (pCl->pWeather == NULL)
        {
            PRINT_ERROR_ID(self,"ERROR_WEATHER_DATA");
            return -1;
        }

    /* set the pool of water that may infiltrate, runoff or stay on the surface. */
    //  pWa->pWBalance->fReservoir = pCl->pWeather->fRainAmount;
    //  SET_IF_99(pWa->pWBalance->fSnowStorage,(double)0);
    //  SET_IF_99(pWa->pWBalance->fSnowWaterContent,(double)0);

    pWa->fPondWater     = (double)0.0;
    pWa->fPotInfiltration   = (double)0.0;

    pWa->fRunOffR       = (double)0.0;
    pWa->fPercolR       = (double)0.0;


    // Hydraulische Funktionen laden:
    WCont = xpn_register_var_get_pointer(self->pXSys->var_list,"hydraulic_fuctions.WCont");
    HCond = xpn_register_var_get_pointer(self->pXSys->var_list,"hydraulic_fuctions.HCond");
    DWCap = xpn_register_var_get_pointer(self->pXSys->var_list,"hydraulic_fuctions.DWCap");
    MPotl = xpn_register_var_get_pointer(self->pXSys->var_list,"hydraulic_fuctions.MPotl");
    if ((WCont==NULL) || (HCond==NULL) || (DWCap==NULL) || (MPotl==NULL))
        {
            PRINT_ERROR_ID(self,"Problem with hydraulic functions!");
        }

    PedoTransfer = xpn_register_var_get_pointer(self->pXSys->var_list,"xpn_pedotransfer.function");

    /*if (self->PedoTransfer==NULL)
    {
        PRINT_ERROR_ID((&(self->parent)),"No pedotransfer functions defined!");
    }*/


    /* Initialisieren der hydraulischen Funktionen */
    // lpFuncSel = CfgFuncSelectAdress();


    // initHydFunctions(lpFuncSel[30]);   // 0 = Hutson & Cass
    // 1 = VanGenuchten
    // 2 = VanGenuchten Bimodal type




    for (pSW=pSo->pSWater,
            pSL=pSo->pSLayer,
            pWL= pWa->pWLayer; ((pSW!= NULL)&&
                                (pSL!= NULL)&&
                                (pWL!= NULL));
            pSL = pSL->pNext,
            pWL = pWL->pNext,
            pSW = pSW->pNext)
        {
            // ep 150397
            SET_IF_99(pSL->fPorosity,((double)1 - pSL->fBulkDens/(double)2.65));
            SET_IF_99(pWL->fSoilAirVol,(pSL->fPorosity - pWL->fContAct));
            SET_IF_99(pSW->fMinPot,(double)-2);

            // ep 120500
            if ((strcmp(pSL->acSoilID,"-99.0")==0)|| (strcmp(pSL->acSoilID,"-99")==0) || (!strcmp(pSL->acSoilID,"")))
				{
                iSoilNr=SoilCode(pSL->fClay,pSL->fSilt,pSL->fSand,pSL->acSoilID);
				}

            // ep 180500 transform fRockfrac from weight% to volume% ?
            //f0=
            // ep 180299
            f1=GPV_FK_PWP(pSL->fClay,pSL->fBulkDens,pSL->fCHumus,
                          pSL->fRockFrac,pSL->acSoilID,1);
            f2=GPV_FK_PWP(pSL->fClay,pSL->fBulkDens,pSL->fCHumus,
                          pSL->fRockFrac,pSL->acSoilID,2);
            f3=GPV_FK_PWP(pSL->fClay,pSL->fBulkDens,pSL->fCHumus,
                          pSL->fRockFrac,pSL->acSoilID,3);
            /*
            if ((pSW->fContFK==(double)-99)||(pSW->fContPWP==(double)-99))
              pSL->fPorosity=f1/(double)100;
            */
            SET_IF_99(pSW->fContFK,  f2/(double)100.0);
            SET_IF_99(pSW->fContPWP, f3/(double)100.0);


            dClayF= (double)pSL->fClay;
            dSiltF= (double)pSL->fSilt;
            dSandF= (double)pSL->fSand;
            dBD= (double)pSL->fBulkDens;
            dSP= (double)pSL->fPorosity;
            /*        dCarbonF= (double)pSL->fCHumus /((double)pSo->fDeltaZ * dBD *(double)100);   */
            dCarbonF= (double)pSL->fCHumus;



            /* Da derzeit keine PTF2 aufgerufen wird, wird fContRes nach
               Initialisierungsliste mit null belegt. */

            SET_IF_99_AND_PEDO_DEFINED(pSW->fContSat,(*PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,1),PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fContRes,(*PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,2),PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fCampA,  (*PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,3),PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fCampB,  (*PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,4),PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fVanGenA,((PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,5))*0.1,PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fVanGenN,(*PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,6),PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fVanGenM,(*PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,7),PedoTransfer);
            SET_IF_99_AND_PEDO_DEFINED(pSW->fCondSat,((PedoTransfer)(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP,8))*10.0,PedoTransfer);




//        pSL->fPorosity = pSW->fContSat;


//     lpFuncSel = CfgFuncSelectAdress();

            // saturated volumetric water content [1]

//TODO: Pedotransferfunktionen:
            /*
             if (pSW->fContSat == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
                            break;
                        }
                    pSW->fContSat = f1;
                    pSW->fContSat = pSL->fPorosity;//in water1.c : pSW->fContSat = pSL->fPorosity - pWL->fIce !!
                                                   // this leads to a water balance error at simulation start
                    }

                    // residual volumetric water content [1]
                    if (pSW->fContRes == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)2);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)2);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)2);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF, dSiltF, dSandF, dBD, dCarbonF, (double)1,(int)2);
                            break;
                        }
                    pSW->fContRes = f1;
                    }

                    // Campbell A parameter or "air entry value" [?]
                    if (pSW->fCampA == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
                            break;
                        }
                    pSW->fCampA = f1;
                    }

                    //Campbell B parameter [1]
                    if (pSW->fCampB == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
                            break;Autoteilschlag: 175
                        }
                    pSW->fCampB = f1;
                    }

                    // van Genuchten alpha parameter [1/cm]
                    if (pSW->fVanGenA == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
                            break;
                        }
                    pSW->fVanGenA = f1*(double)0.1;// [1/cm] to [1/mm]
                    }

                    // van Genuchten n parameter [1]
                    if (pSW->fVanGenN == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
                            break;
                        }
                    pSW->fVanGenN = f1;
                    }

                    // van Genuchten m parameter [1]
                    if (pSW->fVanGenM == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
                            break;
                        }
                    pSW->fVanGenM = f1;
                    }

                    // saturated hydraulic conductivity [mm/day]
                    if (pSW->fCondSat == (double)-99)
                    {
                        switch(lpFuncSel[43])
                        {
                        case 0:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                            break;
                        case 1:
                            f1= (double)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                            break;
                        case 2:
                            f1= (double)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                            break;
                        default:
                            f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                            break;
                        }

                    //only Campbell estimates sat. cond.
                    f1= (double)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                    pSW->fCondSat = f1*(double)10;// [cm/d] to [mm/d]
                    }
            */
            //// watercontent at the inflection point  ===============
            // ch if (pSW->fContInflec == (double)-99)

            // This variable must be > 0 !
            if (pSW->fContInflec < EPSILON)
                {
                    pSW->fContInflec = (double)2.0 * pSW->fCampB * pSL->fPorosity;
                    pSW->fContInflec /= ((double)1.0+(double)2.0*pSW->fCampB);
                }


            // This variable must be < 0 !
            if ((pSW->fPotInflec + EPSILON > 0) || (pSW->fPotInflec == (double)-99))
                {
                    f1 = (double)2.0 * pSW->fCampB;
                    f1 /= ((double)1.0+(double)2.0*pSW->fCampB);

                    pSW->fPotInflec = pSW->fCampA * abspowerDBL(f1,((double)-1.0*(pSW->fCampB)));
                }

            if (pSW->fMinPot > pSW->fPotInflec)
                {

                    ////=========  max water content per layer ===
                    pWL->fContMax = (double)1.0 - (pSW->fContInflec /pSL->fPorosity);
                    pWL->fContMax *= abspowerDBL(pSW->fMinPot,(double)2.0);
                    pWL->fContMax *= pSL->fPorosity;
                    f1 =    (double)-2.0*pSW->fCampB;
                    f2 = pSW->fContInflec/pSL->fPorosity;
                    f3 = (double)(abspowerDBL(pSW->fCampA,(double)2.0));
                    f3 *= (abspowerDBL(f2,f1));
                    pWL->fContMax /= f3;
                    pWL->fContMax = pSL->fPorosity - pWL->fContMax;
                } // Ende: if Min. potential
            else
                {
                    f1 = ((double)-1.0 /pSW->fCampB);
                    f2 = (pSW->fMinPot/pSW->fCampA);
                    pWL->fContMax   = (abspowerDBL(f2,f1)* pSL->fPorosity);
                } //// Ende: else Min. potential

        }  // Ende: schichtweise Berechnung

    /* Fuer das Kapazitaetsmodell nach Ritchie wird die Modellvariable
       fContSatCER benoetigt. Sie berechnet sich aus der Feldkapazitaet
       und dem Gesamtporenvolumen */

    for(pSW = pSo->pSWater,
            pSL = pSo->pSLayer; ((pSW != NULL)&&
                                 (pSL != NULL));
            pSW = pSW->pNext,
            pSL = pSL->pNext)
        {
            SET_IF_99(pSW->fContSatCER,(pSW->fContFK + ((pSL->fPorosity - pSW->fContFK)  * (double)0.5)));
        }  // Ende schichtweise Berechnung von fContSatCER


    /* Der aus den Startwerten eingelesene Wassergehalt zu
    Beginn der Simulation wird auf die aktuellen Wassergehalte
    initialisiert. Die Fluesse aller Simulationsschichten werden
    mit Null initialisiert.  */

    /*for (pSL = pSo->pSLayer->pNext,
            pSW = pSo->pSWater->pNext,
            pWL = pWa->pWLayer->pNext,
            iLayer = 1; ((pSL->pNext != NULL)&&
                         (pSW->pNext != NULL)&&
                         (pWL->pNext != NULL)); pSL = pSL->pNext,
            pSW = pSW->pNext,
            pWL = pWL->pNext,
            iLayer++) {
        pWL->fFluxDens = (double)0.0;
        pWL->fFlux      = (double)0.0;

        if (pWL->fContInit < (double)0.0) {
            pWL->fContInit = WATER_CONTENT(pWL->fMatPotAct);
        }

        pWL->fContAct  = pWL->fContInit;
        pWL->fContOld  = pWL->fContInit;
    }

    // Die virtuellen Schichten werden mit den
    //  physikalischen Randschichten initialisiert.
    pWa->pWLayer->fContAct   = pWa->pWLayer->pNext->fContAct;
    pWa->pWLayer->fContOld   = pWa->pWLayer->pNext->fContOld;
    pWa->pWLayer->fFluxDens  = (double)0.0;
    pWa->pWLayer->fFlux      = (double)0.0;

    pWL->fContAct   = pWL->pBack->fContAct;
    pWL->fContOld   = pWL->pBack->fContOld;
    pWL->fFluxDens  = (double)0.0;
    pWL->fFlux      = (double)0.0;
    */
    /* Initialisieren der Potentialgroessen des Bodens fuer die
       Loeser der Richardsgleichung.  */
    // initSoilPotential(exp_p);  // newly calculation of soil water potential
	
for(pSL = pSo->pSLayer->pNext,
            pSW = pSo->pSWater->pNext,
            pWL = pWa->pWLayer->pNext,
            iLayer = 1; ((pSL->pNext != NULL)&&
                         (pSW->pNext != NULL)&&
                         (pWL->pNext != NULL)); pSL = pSL->pNext,
            pSW = pSW->pNext,
            pWL = pWL->pNext,
            iLayer++)
        {
            pWL->fMatPotAct = MATRIX_POTENTIAL_H(pWL->fContAct);
            pWL->fMatPotOld = pWL->fMatPotAct;
        }

    /* Auf die virtuelle erste Simulationsschicht werden
       Werte der zweiten Simulationsschicht geschrieben  */

    pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
    pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


    /* Auf die virtuelle letzte Simulationsschicht werden
       Werte der vorletzten Simulationsschicht geschrieben  */

    pWL->fMatPotOld = pWL->pBack->fMatPotOld;
    pWL->fMatPotAct = pWL->pBack->fMatPotAct;
	
	
	


    return RET_SUCCESS;
}



void expertn_database_help_func_init_Chemistry(expertn_modul_base *self)
{
    PCHEMISTRY pCh = self->pCh;
    PSPROFILE pSo=self->pSo;


    PSLAYER         pSL;
    PCLAYER         pCL;
    PCPARAM         pPA;

    double tiefe;
    int iLayer;


    /* Initialisieren der Pools fuer die fr. org. Substanz
        und die Humuspools der Simulationsschichten.  */

    for( pSL=pSo->pSLayer,
            pCL=pCh->pCLayer; ((pSL->pNext!=NULL)&&
                               (pCL->pNext!=NULL));
            pSL=pSL->pNext,
            pCL=pCL->pNext)
        {
            pCL->fPpmTOkg = pSL->fBulkDens * pSL->fThickness * (float)0.01;  //  factor 10 cm/mm
        }

    /* Initialisieren der Pools fuer die fr. org. Substanz
       und die Humuspools der Bodenschichten.  */

	expertn_database_help_func_init_Soil_Organic_Matter(self);

    /* Werden fuer bestimmte konstante Zerfallsraten oder Potentiale keine
        Werte eingelesen, muessen fuer einen ordnungsgemaessen Ablauf der
        Simulation diese Werte standardmaessig initialisiert werden.  */

    for (pSL = pSo->pSLayer->pNext,pCL = pCh->pCLayer->pNext; ((pSL->pNext != NULL)&&(pCL->pNext != NULL)); pSL = pSL->pNext,pCL = pCL->pNext)
        {
            pCL->fN2ONGasConc = 2.07e-4;
            pCL->fN2ONGasConcOld = 2.07e-4;
            pCL->fHumusMinerMaxR = 1.0e-04;
            pCL->fN2ONGasConc = 2.07e-4;
            pCL->fN2ONGasConcOld = 2.07e-4;
            pCL->fHumusMinerMaxR = 1.0e-04;
            pCL->fNH4NitrMaxR = 1.0;
            pCL->fHumusDecR = 7.0e-05;
            pCL->afFOMDecR[0] = 0.2;
            pCL->afFOMDecR[1] = 0.05;
            pCL->afFOMDecR[2] = 0.0095;
            pCL->fNMicBiomDenit = pCL->fCLitter * 0.5 * 0.02 / 8.0;
            pCL->fCMicBiomDenit = pCL->fCLitter * 0.5 * 0.02;
            pCL->fLitterMinerMaxR = 0.02;
            pCL->fManureMinerMaxR = 0.01;
            pCL->fMicBiomCN = 8.0;
            pCL->fFOMFastDecMaxR = 0.07;
            pCL->fFOMSlowDecMaxR = 0.02;
            pCL->fMicBiomFastDecMaxR = 0.33;
            pCL->fMicBiomSlowDecMaxR = 0.04;
            pCL->fHumusFastMaxDecMaxR = 0.0001;
            pCL->fHumusSlowMaxDecMaxR = 0.00007;
            pCL->fUreaHydroMaxR = 1.36;
        }

    /* Tiefenabhaengige Initialisierung. Bis 30 cm
       Bodentiefe wird mit anderem Wert gerechnet.  */

    for( tiefe = 0.0, pSL=pSo->pSLayer, pCL=pCh->pCLayer; ((pSL->pNext!=NULL)&&(pCL->pNext!=NULL)); tiefe += pSL->fThickness, pSL=pSL->pNext,pCL=pCL->pNext)
        {
            if (tiefe <= 300.0)
                {
                    /* als Standard wird hier als max. Denitrifikationsrate 1kg/ha
                       auf die Schichten von 0-30cm aufgeteilt. */
                    pCL->fNO3DenitMaxR  = (1.0 * pSL->fThickness /300.0);

                }
            else
                {
                    pCL->fNO3DenitMaxR = 0.0;
                }
        }


    /* Initialisierung von Modellparametern */

    for (SOIL_LAYERS0(pPA,pCh->pCParam))
        {
            pPA->fMolDiffCoef = 120.0;
            pPA->fBioMassFastCN = 20.0;
            pPA->fBioMassSlowCN = 20.0;
            pPA->fBioMassDenitCN= 20.0;
            pPA->fDispersivity= 100.0;
            pPA->fNitrifPot = 0.1;
            pPA->fMinerThetaMin= 0.08;
            pPA->fMinerEffFac=0.4;
            pPA->fMinerHumFac=0.2;
            pPA->fMinerTempB=20.0;
            pPA->fMinerQ10=2.0;
            pPA->fMinerSatActiv=0.6;
            pPA->fMaintCoeff=0.1824;
            pPA->fYieldCoeff=0.503;
            pPA->fBioFastMaxGrowR=3.4;
            pPA->fN2ORedMaxR=7.0;
            pPA->fDenitThetaMin=0.6;
            pPA->fDenitKsNO3=(0.1 * pSo->pSLayer->pNext->fThickness);

            pPA->fNitrifNO3NH4Ratio = 25.0;
            pPA->fDenitMaxGrowR = 3.4;
            pPA->fDenitKsCsol = 1.7;
            pPA->fDenitMaintCoeff = 0.2;
            pPA->fDenitYieldCoeff = 0.5;

            pPA->afKd[0] = 1.0;
            pPA->afKd[1] = 0.92;
            pPA->afKd[2] = 0.0;
            pPA->afKd[3] = 0.0;

            pPA->afKh[0] = 0.0;
            pPA->afKh[1] = 0.0;
            pPA->afKh[2] = 0.0;
            pPA->afKh[3] = 0.0;
        }
		
	// air pressure barometrische Hoehenformel:
	{
		double p0 = 101325; // Pa
		self->pCl->pWeather->fAtmPressure = p0 * exp(-self->pLo->pFarm->fAltitude / 7990.0);
	}
	
	

}


void expertn_database_help_func_init_Soil_Organic_Matter(expertn_modul_base *self)
{    
    PTIME pTi = self->pTi;
    PCHEMISTRY pCh = self->pCh;
    PMANAGEMENT pMa=self->pMa;
    PSPROFILE pSo=self->pSo;


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

    for (pSL= pSo->pSLayer->pNext; (pSL->pNext!=NULL); pSL=pSL->pNext)
        {
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


    if ((pTi->pSimTime->mday == pMa->pLitter->Day) && (pTi->pSimTime->mon == pMa->pLitter->Month) && (pTi->pSimTime->year == (int)pMa->pLitter->Year))
        {            
			expertn_database_help_func_TSComparePreCropDate(self);
        }
    else
        {

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

            if (pCh->pCProfile->fCLitterSurf< 0)
            pCh->pCProfile->fCLitterSurf = 0.0;
			if (pCh->pCProfile->fNLitterSurf <0)
            pCh->pCProfile->fNLitterSurf = 0.0;

            if(pCh->pCProfile->fCManureSurf<0)
			pCh->pCProfile->fCManureSurf = 0.0;
			if(pCh->pCProfile->fNManureSurf<0)
            pCh->pCProfile->fNManureSurf = 0.0;

            if(pCh->pCProfile->fCHumusSurf<0)
			pCh->pCProfile->fCHumusSurf = 0.0;
			if(pCh->pCProfile->fNHumusSurf<0)
            pCh->pCProfile->fNHumusSurf = 0.0;

            if(pCh->pCProfile->fCStandCropRes < 0)
            pCh->pCProfile->fCStandCropRes = 0.0;
			if(pCh->pCProfile->fNStandCropRes< 0)
            pCh->pCProfile->fNStandCropRes = 0.0;

            /* Berechnen schichtmaessiger Anteil. Zur Vereinfachung
              Variable pCL->fCLitter benutzt um Anteile zu uebergeben. */
            for (pSL = pSo->pSLayer->pNext,
                    pCL = pCh->pCLayer->pNext;
                    ((pSL->pNext != NULL)&&
                     (pCL->pNext != NULL));
                    pSL = pSL->pNext,
                    pCL = pCL->pNext)
                {
                    pCL->fCLitter = 0.0;
                }

            /* Anteilsmaessige Verteilung auf die Bodenschichten. */
            for (pSL = pSo->pSLayer->pNext,
                    pCL = pCh->pCLayer->pNext;
                    ((pSL->pNext != NULL)&&
                     (pCL->pNext != NULL));
                    pSL = pSL->pNext,
                    pCL = pCL->pNext)
                {
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
    for (pSL = pSo->pSLayer->pNext,pCL = pCh->pCLayer->pNext; ((pSL->pNext != NULL)&&(pCL->pNext != NULL)); pSL = pSL->pNext,pCL = pCL->pNext)
        {

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

            if(AddCN >= fCN_AOM3 && AddCN < fCN_AOM2)
                {
                    fCFOMVF = (fCFast * (1.0/AddCN - 1.0/fCN_AOM3)) / (1.0/fCN_AOM2 - 1.0/fCN_AOM3);
                    fCFOMF = fCFast - fCFOMVF;
                    fCFOMS = 0.0;
                }

            if(AddCN >= fCN_AOM2 && AddCN < fCN_AOM1)
                {
                    fCFOMF = (fCFast * (1.0/AddCN - 1.0/fCN_AOM2)) / (1.0/fCN_AOM1 - 1.0/fCN_AOM2);
                    fCFOMS = fCFast - fCFOMF;
                    fCFOMVF = 0.0;
                }

            if ((AddCN>=0.0) && (AddCN<=5.0))
                {
                    fCFOMF = 0.0;
                    fCFOMS = 0.0;
                    fCFOMVF = 0.0;
                }

            if (AddCN > fCN_AOM1)
                {
                    gchar *S;
                    S  = g_strdup_printf("AddCN > fCN_AOM1");
                    PRINT_ERROR_ID(self,S);
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

            if ((fCTotal)&&(fNTotal))
                {
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
void expertn_database_help_func_TSComparePreCropDate(expertn_modul_base *self)
{
    PTIME pTi = self->pTi;
    PCHEMISTRY pCh = self->pCh;
    PMANAGEMENT pMa=self->pMa;
    PSPROFILE pSo=self->pSo;
    double          actDepth,RootProp,amount,factor,RootSum;

    PSLAYER       pSL;
    PCLAYER       pCL;

    RootProp = 0.0;
    RootSum  = 0.0;
    actDepth = 0.0;

    if ((pTi->pSimTime->mday== pMa->pLitter->Day) && (pTi->pSimTime->mon == pMa->pLitter->Month) && (pTi->pSimTime->year == pMa->pLitter->Year))
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
                    pCL = pCL->pNext)
                {
                    actDepth = actDepth + (pSL->fThickness / 10.0);
                    RootProp = exp(-3.0 * actDepth / (pSo->fDepth / 10.0));
                    RootSum += RootProp;
                    pCL->fCFreeFOM = RootProp;
                }

            for (pCL = pCh->pCLayer->pNext;
                    pCL->pNext != NULL;
                    pCL = pCL->pNext)
                {
                    factor = pCL->fCFreeFOM/ RootSum;
                    amount = pMa->pLitter->fRootC * factor;

                    if(amount>0.0)
                        {
                            pCL->fCLitter = amount;
                            pCL->fNLitter = amount/pMa->pLitter->fRootCN;
                        }

                    else
                        {
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




/*Calculate Net Radiation from Solar Radiation [MJ/m2*d]*/
double expertn_database_help_func_get_Rn(expertn_modul_base *self)
{
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

    dt = self->pTi->pTimeStep->fAct;
    elevation = self->pLo->pFarm->fAltitude;
    iJulianDay  = (int)self->pTi->pSimTime->fTimeY;   // actual julian day
    ftime_day = self->pTi->pSimTime->fTimeY;     // [s]->[day]
    h = (double)(((double)((double)ftime_day - (int)ftime_day))*24.0);


    T = self->pCl->pWeather->fTempAir;
    RH = self->pCl->pWeather->fHumidity;
    Rs = self->pCl->pWeather->fSolRad;

    //Extraterrestrial radiation for hourly periods (Ra )
    latitude = self->pLo->pFarm->fLatitude*M_PI/180.0;       // eq. 49, [rad]
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
    if(Rs0>0.0)
        {
            fcd = 1.35*Rs/Rs0-0.35; //eqn 45
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
    if(Rn < 0.0)
        {
            Rn = 0.0;
        }

    return Rn;
}

void expertn_database_help_func_set_standard_soil_paramter(expertn_modul_base *self)
{
	int i;

	PSPROFILE pSo;
	PSLAYER pSLayer;
	PSWATER pSWater;
	PSHEAT pSHeat;
	pSo = self->pSo;





	for (pSLayer=self->pSo->pSLayer,pSHeat = self->pSo->pSHeat,pSWater=self->pSo->pSWater,i=0; pSLayer!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pSHeat = pSHeat->pNext,i++)
		{
			pSHeat->afPartGeom[0][0] = 0.125;
			pSHeat->afPartGeom[0][1] = 0.125;
			pSHeat->afPartGeom[0][2] = 0.5;

			pSHeat->afPartGeom[1][0] = 0.125;
			pSHeat->afPartGeom[1][1] = 0.125;
			pSHeat->afPartGeom[1][2] = 0.5;

			pSHeat->afPartGeom[2][0] = 0.75;
			pSHeat->afPartGeom[2][1] = 0.75;
			pSHeat->afPartGeom[2][2] = 0.0;

			pSHeat->afPartDens[0] = 2.65;
			pSHeat->afPartDens[1] = 2.65;
			pSHeat->afPartDens[2] = 1.1;

			/* Spezifische Waermekonduktivitaeten: [W / (K m)] nach: De Vries, 1963 */
			pSHeat->afPartHeatCond[0] = 8.8;     /* Quarz */
			pSHeat->afPartHeatCond[1] = 2.9;     /* Mineral */
			pSHeat->afPartHeatCond[2] = 0.25;    /* org. Substanz */
			pSHeat->afPartHeatCond[3] = 0.57;    /* Wasser */
			pSHeat->afPartHeatCond[4] = 0.025;   /* Luft */
			pSHeat->afPartHeatCond[5] = 2.2;     /* Eis  */

			pSLayer->fHumusCN = 10.0; // aus xfile.c (warum auch immer)

			// muss es nicht fHumus * 1.7 heissen?
			pSLayer->afPartVolFrac[0] = ((100.0 - pSLayer->fClay - pSLayer->fHumus)/(100.0 * pSHeat->afPartDens[0])); // Quarz Gehalt
			pSLayer->afPartVolFrac[1] = (pSLayer->fClay * pSLayer->fBulkDens)/(100.0 * pSHeat->afPartDens[1]); //
			pSLayer->afPartVolFrac[2] = pSLayer->fHumus/(100.0 * pSHeat->afPartDens[2]);
			pSLayer->afPartVolFrac[3] = 0.0;

			pSLayer->fCompress = 0.0;

			pSLayer->fImpedLiqA = 0.0;
			pSLayer->fImpedLiqB = 0.0;
			pSLayer->fImpedGasA = 0.0;
			pSLayer->fImpedGasB = 0.0;

			if (pSLayer->fCHumus==-99.0) pSLayer->fCHumus = pSLayer->fHumus/1.724;
			if (pSLayer->fNHumus==-99.0) pSLayer->fNHumus = pSLayer->fCHumus/10.0;

			pSWater->fContSatCER = 0.0;

			pSWater->fContMobSat = 0.0; // ?
			pSWater->fContImmSat = 0.0; // ?
			pSWater->fMobImmEC = 0.0; // ?

			pSWater->fBiModWeight1 = 0.0; // ?
			pSWater->fBiModWeight2 = 0.0; // ?

			// jetzt noch die Variablen setzen, die noch nicht gesetzt sind:
			// Planzen und Boden Albedo
			pSo->fAlbedo = 0.14; // 0.2 .. 0.25
			pSo->fSoilAlbedo = 0.14; // 0.2 .. 0.25
			self->pPl->pPltClimate->fPlantAlbedo = 0.23;

			// KurvebNummern
			pSo->fCurvNumNorm = 79.0;

		}
}


void expertn_database_help_func_set_global_start_and_end_date(expertn_modul_base *self,GDate **start_date,GDate **end_date)
{
	char *S;
	S = (char*)xpn_register_var_get_pointer(self->pXSys->var_list,"Global_Config.options.simulation start");
	if (S!=NULL)
		{
				*start_date = convert_str_to_gdate(S);
		} else
		{
				*start_date = NULL;
		}
	S = (char*)xpn_register_var_get_pointer(self->pXSys->var_list,"Global_Config.options.simulation end");
	if (S!=NULL)
		{
				*end_date = convert_str_to_gdate(S);
		} else
		{
				*end_date = NULL;
		}
          
}
