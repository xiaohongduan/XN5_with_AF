
// Diese Datei kann in Zukunft automatisch mit dem tool ctag erstellte werden

#define XSOIL_REG \
\
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_none,acSoilID,"pSo.pSLayer.acSoilID",NULL); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fThickness,"pSo.pSLayer.fThickness",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fClay,"pSo.pSLayer.fClay",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fSand,"pSo.pSLayer.fSand",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fSilt,"pSo.pSLayer.fSilt",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fCHumus,"pSo.pSLayer.fCHumus",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fHumus,"pSo.pSLayer.fHumus",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fNHumus,"pSo.pSLayer.fNHumus",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fHumusCN,"pSo.pSLayer.fHumusCN",0.0); \
xpn_register_var_init_array(xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,afPartVolFrac[i],varname,0.0),"pSo.pSLayer.afPartVolFrac",4);\
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fpH,"pSo.pSLayer.fpH",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fBulkDens,"pSo.pSLayer.fBulkDens",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fBulkDensStart,"pSo.pSLayer.fBulkDensStart",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fPorosity,"pSo.pSLayer.fPorosity",-99.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fCompress,"pSo.pSLayer.fCompress",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fRockFrac,"pSo.pSLayer.fRockFrac",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble, fImpedLiqA,"pSo.pSLayer. fImpedLiqA",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fImpedLiqB,"pSo.pSLayer.fImpedLiqB",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fImpedGasA,"pSo.pSLayer.fImpedGasA",0.0); \
xpn_register_var_init_rec_struct( var_list,SLAYER, pSo->pSLayer,xpn_register_var_init_pdouble,fImpedGasB,"pSo.pSLayer.fImpedGasB",0.0); \
\
xpn_register_var_init_array(xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,afPartDens[i],varname,0.0),"pSo.pSHeat.afPartDens",3) \
xpn_register_var_init_array2d(xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,afPartGeom[i2][i],varname,0.0),"pSo.pSHeat.afPartGeom",3,3) \
xpn_register_var_init_array(xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,afPartHeatCond[i],varname,0.0),"pSo.pSHeat.afPartHeatCond",6) \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCapAir,"pSo.pSHeat.fCapAir",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCapWater,"pSo.pSHeat.fCapWater",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCapIce,"pSo.pSHeat.fCapIce",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCapOrg,"pSo.pSHeat.fCapOrg",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCapAnOrg,"pSo.pSHeat.fCapAnOrg",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCondAir,"pSo.pSHeat.fCondAir",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCondWater,"pSo.pSHeat.fCondWater",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCondIce,"pSo.pSHeat.fCondIce",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCondOrg,"pSo.pSHeat.fCondOrg",0.0); \
xpn_register_var_init_rec_struct( var_list,SHEAT, pSo->pSHeat,xpn_register_var_init_pdouble,fCondAnOrg,"pSo.pSHeat.fCondAnOrg",0.0); \
\
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContSat,"pSo.pSWater.fContSat",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContPWP,"pSo.pSWater.fContPWP",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContFK,"pSo.pSWater.fContFK",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContSatCER,"pSo.pSWater.fContSatCER",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContRes,"pSo.pSWater.fContRes",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContMobSat,"pSo.pSWater.fContMobSat",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContImmSat,"pSo.pSWater.fContImmSat",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fMobImmEC,"pSo.pSWater.fMobImmEC",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fCampA,"pSo.pSWater.fCampA",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fCampB,"pSo.pSWater.fCampB",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fVanGenA,"pSo.pSWater.fVanGenA",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fVanGenA2,"pSo.pSWater.fVanGenA2",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fVanGenM,"pSo.pSWater.fVanGenM",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fVanGenM2,"pSo.pSWater.fVanGenM2",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fVanGenN,"pSo.pSWater.fVanGenN",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fVanGenN2,"pSo.pSWater.fVanGenN2",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fMualemP,"pSo.pSWater.fMualemP",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fMualemQ,"pSo.pSWater.fMualemQ",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fMualemR,"pSo.pSWater.fMualemR",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fMinPot,"pSo.pSWater.fMinPot",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fCondSat,"pSo.pSWater.fCondSat",-99.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fContInflec,"pSo.pSWater.fContInflec",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fPotInflec,"pSo.pSWater.fPotInflec",0.0); \
xpn_register_var_init_rec_struct( var_list,SWATER, pSo->pSWater,xpn_register_var_init_pdouble,fBiModWeight1,"pSo.pSWater.fBiModWeight1",0.0); \
\
xpn_register_var_init_pint( var_list, pSo->iLayers,"pSo.iLayers",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fDepth,"pSo.fDepth",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fDeltaZ,"pSo.fDeltaZ",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fPotPWP,"pSo.fPotPWP",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fSoilAlbedo,"pSo.fSoilAlbedo",0.23); \
xpn_register_var_init_pdouble( var_list, pSo->fAlbedo,"pSo.fAlbedo",0.23); \
xpn_register_var_init_pdouble( var_list, pSo->fSoilCover,"pSo. fSoilCover",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fCurvNumNorm,"pSo.fCurvNumNorm",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fCurvNumDry,"pSo.fCurvNumDry",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fCurvNumWet,"pSo. fCurvNumWet",0.0); \
xpn_register_var_init_pdouble( var_list, pSo->fCurvNumAct,"pSo. fCurvNumAct",0.0);


