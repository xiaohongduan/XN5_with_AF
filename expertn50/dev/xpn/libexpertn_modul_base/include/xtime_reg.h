
// Diese Datei kann in Zukunft automatisch mit dem tool ctag erstellte werden

#define XTIME_REG \
\
xpn_register_var_init_pdouble( var_list, pTi->pSimTime->fTimeY,"pTi.pSimTime.fTimeY",0.0);\
xpn_register_var_init_pint( var_list, pTi->pSimTime->iyear,"pTi.pSimTime.iyear",0.0);\
xpn_register_var_init_pdouble( var_list, pTi->pSimTime->fTimeZone,"pTi.pSimTime.fTimeZone",0.0);\
xpn_register_var_init_pdouble( var_list, pTi->pSimTime->fSimDuration,"pTi.pSimTime.fSimDuration",0.0);\
\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fAct,"pTi.pTimeStep.fAct",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fOld,"pTi.pTimeStep.fOld",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fMax,"pTi.pTimeStep.fMax",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fOpt,"pTi.pTimeStep.fOpt",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fMin,"pTi.pTimeStep.fMin",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fWaterOpt,"pTi.pTimeStep.fWaterOpt",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fChemOpt,"pTi.pTimeStep.fChemOpt",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fWaterDist,"pTi.pTimeStep.fWaterDist",0.0f);\
xpn_register_var_init_pdouble( var_list, pTi->pTimeStep->fChemDist,"pTi.pTimeStep.fChemDist",0.0f);


