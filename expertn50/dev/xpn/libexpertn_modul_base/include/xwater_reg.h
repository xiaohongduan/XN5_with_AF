
// Diese Datei kann in Zukunft automatisch mit dem tool ctag erstellte werden

#define XWATER_REG \
xpn_register_var_init_pint( var_list, pWa->iTopBC,"pWa.iTopBC",1);\
xpn_register_var_init_pint( var_list, pWa->iBotBC,"pWa.iBotBC",1);\
xpn_register_var_init_pint( var_list, pWa->FreeD,"pWa.FreeD",1);\
xpn_register_var_init_pint( var_list, pWa->iMobImm,"iMobImm",0);\
xpn_register_var_init_pdouble( var_list, pWa->fRunOffR,"pWa.fRunOffR",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fLeaching,"pWa.fLeaching",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fCumLeaching,"pWa.fCumLeaching",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fPotInfiltration,"pWa.fPotInfiltration",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fInfiltR,"pWa.fInfiltR",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fInfiltDay,"pWa.fInfiltDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fCumInfiltration,"pWa.fCumInfiltration",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fPondWater,"pWa.fPondWater",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fPercolR,"pWa.fPercolR",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fPotETdt,"pWa.fPotETdt",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fActETdt,"pWa.fActETdt",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fActETR,"pWa.fActET",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fRunOffCum,"pWa.fRunOffCum",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fRunOffDay,"pWa.fRunOffDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fActTranspDay,"pWa.fActTranspDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fActTranspCum,"pWa.fActTranspCum",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fLatFlowCum,"pWa.fLatFlowCum",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->fGrdWatLevel,"pWa.fGrdWatLevel",2000.0);\
xpn_register_var_init_pdouble( var_list, pWa->fGrdWatLvlPot,"pWa.fGrdWatLvlPot",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->kc_factor,"pWa.kc_factor",1.0);\
\
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContInit,"pWa.pWLayer.fContInit",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble, fContMax,"pWa.pWLayer. fContMax",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContAct,"pWa.pWLayer.fContAct",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContOld,"pWa.pWLayer.fContOld",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fMatPotAct,"pWa.pWLayer.fMatPotAct",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fMatPotOld,"pWa.pWLayer.fMatPotOld",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContMobAct,"pWa.pWLayer.fContMobAct",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContMobOld,"pWa.pWLayer.fContMobOld",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContImmAct,"pWa.pWLayer.fContImmAct",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fContImmOld,"pWa.pWLayer.fContImmOld",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fOsmPot,"pWa.pWLayer.fOsmPot",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fFluxDens,"pWa.pWLayer.fFluxDens",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fFluxDensOld,"pWa.pWLayer.fFluxDensOld",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fFlux,"pWa.pWLayer.fFlux",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fHydrCond,"pWa.pWLayer.fHydrCond",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fSoilAirVol,"pWa.pWLayer.fSoilAirVol",-99.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fDiffCap,"pWa.pWLayer.fDiffCap",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fMaxEvapFD,"pWa.pWLayer. fMaxEvapFD",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fIce,"pWa.pWLayer.fIce",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fIceOld,"pWa.pWLayer.fIceOld",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fWaterFreezeR,"pWa.pWLayer.fWaterFreezeR",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fWaterFreezeDay,"pWa.pWLayer.fWaterFreezeDay",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fFluxDay,"pWa.pWLayer.fFluxDay",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fFluxCum,"pWa.pWLayer.fFluxCum",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fLatFlowR,"pWa.pWLayer.fLatFlowR",0.0); \
xpn_register_var_init_rec_struct( var_list,WLAYER, pWa->pWLayer,xpn_register_var_init_pdouble,fLatFlowDay,"pWa.pWLayer.fLatFlowDay",0.0);\
\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCumStage1,"pWa.pEvap.fCumStage",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCumStage2,"pWa.pEvap.fCumStage2",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fTimeStage2,"pWa.pEvap.fTimeStage2",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fPotR,"pWa.pEvap.fPotR",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fPotR_old,"pWa.pEvap.fPotR_old",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fActR,"pWa.pEvap.fActR",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fMax,"pWa.pEvap.fMax",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fMaxR,"pWa.pEvap.fMaxR",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropBasalIni,"pWa.pEvap.fCropBasalIni",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropBasalMid,"pWa.pEvap.fCropBasalMid",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropBasalEnd,"pWa.pEvap.fCropBasalEnd",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropTimeIni,"pWa.pEvap.fCropTimeIni",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropTimeMid,"pWa.pEvap.fCropTimeMid",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropTimeEnd,"pWa.pEvap.fCropTimeEnd",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pEvap->fCropTimeDev,"pWa.pEvap.fCropTimeDev",0.0);\
\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fInput,"pWa.pWBalance.fInput",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fOutput,"pWa.pWBalance.fOutput",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fProfil,"pWa.pWBalance.fProfil",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fProfileIce,"pWa.pWBalance.fProfileIce",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fProfilStart,"pWa.pWBalance.fProfilStart",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fBalance,"pWa.pWBalance.fBalance",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fPotETDay,"pWa.pWBalance.fPotETDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fActEVDay,"pWa.pWBalance.fActEVDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fPotCumEvap,"pWa.pWBalance.fPotCumEvap",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fActCumEvap,"pWa.pWBalance.fActCumEvap",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fPotTranspDay,"pWa.pWBalance.fPotTranspDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fActTranspDay,"pWa.pWBalance.fActTranspDay",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fPotCumTransp,"pWa.pWBalance.fPotCumTransp",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fActCumTransp,"pWa.pWBalance.fActCumTransp",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fCumInterception,"pWa.pWBalance.fCumInterception",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fSnowStorage,"pWa.pWBalance.fSnowStorage",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fSnowWaterContent,"pWa.pWBalance.fSnowWaterContent",0.0);\
xpn_register_var_init_pdouble( var_list, pWa->pWBalance->fLatFlow,"pWa.pWBalance.fLatFlow",0.0);




