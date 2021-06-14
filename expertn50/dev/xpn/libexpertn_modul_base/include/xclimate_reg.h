// Diese Datei kann in Zukunft automatisch mit dem tool ctag erstellte werden (falls ich dazukomme ein entsprechendes
// Skript zu schreiben)

#define XCLIMATE_REG \
\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSolRad,"pCl.pWeather.fSolRad",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSolRad_daily_models,"pCl.pWeather.fSolRad_daily_models",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fPAR,"pCl.pWeather.fPAR",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fTempMax,"pCl.pWeather.fTempMax",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fTempMin,"pCl.pWeather.fTempMin",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fTempAve,"pCl.pWeather.fTempAve",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fGlobalStrahlung,"pCl.pWeather.fGlobalStrahlung",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSolNet,"pCl.pWeather.fSolNet",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSolarDirect,"pCl.pWeather.fSolarDirect",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->cosz,"pCl.pWeather.cosz",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fTempAir,"pCl.pWeather.fTempAir",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fTempAir_daily_models,"pCl.pWeather.fTempAir_daily_models",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fTempAir_zlvl,"pCl.pWeather.fTempAir_zlvl",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fHumidity,"pCl.pWeather.fHumidity",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fHumidity_daily_models,"pCl.pWeather.fHumidity_daily_models",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fPanEvap,"pCl.pWeather.fPanEvap",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fWindSpeed,"pCl.pWeather.fWindSpeed",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fWindSpeed_daily_models,"pCl.pWeather.fWindSpeed_daily_models",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fWindSpeedConv,"pCl.pWeather.fWindSpeedConv",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fDewPoint,"pCl.pWeather.fDewPoint",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSatDef,"pCl.pWeather.fSatDef",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fPreciRate,"pCl.pWeather.fPreciRate",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fPreciRate_daily_models,"pCl.pWeather.fPreciRate_daily_models",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fLiquPreciRate,"pCl.pWeather.fLiquPreciRate",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fWaterPond,"pCl.pWeather.fWaterPond",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSnowEqu,"pCl.pWeather.fSnowEqu",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fBulkPreciRate,"pCl.pWeather.fBulkPreciRate",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fCanopyDrain,"pCl.pWeather.fCanopyDrain",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fCanopyStorage,"pCl.pWeather.fCanopyStorage",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fThroughfall,"pCl.pWeather.fThroughfall",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fStemflow,"pCl.pWeather.fStemflow",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fSnow,"pCl.pWeather.fSnow",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fWaterTable,"pCl.pWeather.fWaterTable",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fUreaNRain,"pCl.pWeather.fUreaNRain",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fNH4NRain,"pCl.pWeather.fNH4NRain",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fNO3NRain,"pCl.pWeather.fNO3NRain",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fChemRain,"pCl.pWeather.fChemRain",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fAtmPressure,"pCl.pWeather.fAtmPressure",101325.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fAtmPressure_daily_models,"pCl.pWeather.fAtmPressure_daily_models",101325.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fAtmCO2ppm,"pCl.pWeather.fAtmCO2ppm",400.0);\
xpn_register_var_init_pdouble( var_list, pCl->pWeather->fAtmO2ppm,"pCl.pWeather.fAtmO2ppm",210.0);\
\
xpn_register_var_init_pdouble( var_list, pCl->fCumRain,"pCl.fCumRain",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->afTempLast5Days[0],"pCl.afTempLast5Days[0]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->afTempLast5Days[1],"pCl.afTempLast5Days[1]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->afTempLast5Days[2],"pCl.afTempLast5Days[2]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->afTempLast5Days[3],"pCl.afTempLast5Days[3]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->afTempLast5Days[4],"pCl.afTempLast5Days[4]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->fCumTempL5Days,"pCl.fCumTempL5Days",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->fTempMeasHeight,"pCl.fTempMeasHeight",2.0);\
xpn_register_var_init_pdouble( var_list, pCl->fWindMeasHeight,"pCl.fWindMeasHeight",2.0);\
\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[0],"pCl.pAverage.afMonthTemp[0]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[1],"pCl.pAverage.afMonthTemp[1]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[2],"pCl.pAverage.afMonthTemp[2]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[3],"pCl.pAverage.afMonthTemp[3]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[4],"pCl.pAverage.afMonthTemp[4]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[5],"pCl.pAverage.afMonthTemp[5]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[6],"pCl.pAverage.afMonthTemp[6]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[7],"pCl.pAverage.afMonthTemp[7]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[8],"pCl.pAverage.afMonthTemp[8]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[9],"pCl.pAverage.afMonthTemp[9]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[10],"pCl.pAverage.afMonthTemp[10]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->afMonthTemp[11],"pCl.pAverage.afMonthTemp[11]",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearTemp,"pCl.pAverage.fYearTemp",7.4);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fMonthTempAmp,"pCl.pAverage.fMonthTempAmp",6.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearSunDuration,"pCl.pAverage.fYearSunDuration",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearSolRad,"pCl.pAverage.fYearSolRad",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearPAR,"pCl.pAverage.fYearPAR",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearSnow,"pCl.pAverage.fYearSnow",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearHumidity,"pCl.pAverage.fYearHumidity",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearPanEvap,"pCl.pAverage.fYearPanEvap",0.0);\
xpn_register_var_init_pdouble( var_list, pCl->pAverage->fYearSatDef,"pCl.pAverage.fYearSatDef",0.0);



