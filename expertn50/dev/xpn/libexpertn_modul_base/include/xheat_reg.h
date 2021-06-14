
// Diese Datei kann in Zukunft automatisch mit dem tool ctag erstellte werden

#define XHEAT_REG \
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fHeatSens,"pHe.pHEBalance.fHeatSens",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fHeatLatent,"pHe.pHEBalance.fHeatLatent",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fGroundHeat,"pHe.pHEBalance.fGroundHeat",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fShortNetRad,"pHe.pHEBalance.fShortNetRad",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fLongNetRad,"pHe.pHEBalance.fLongNetRad",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fNetRad,"pHe.pHEBalance.fNetRad",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->pHEBalance->fRes,"pHe.pHEBalance.fRes",0.0);\
xpn_register_var_init_pdouble( var_list, pHe->fEmissi,"pHe.fEmissi",0.9693301);\
xpn_register_var_init_pdouble( var_list, pHe->fTempSurf,"pHe.fTempSurf",0.0);\
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fSoilTemp,"pHe.pHLayer.fSoilTemp",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fSoilTempOld,"pHe.pHLayer.fSoilTempOld",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fHeat,"pHe.pHLayer.fHeat",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fHeatflow,"pHe.pHLayer.fHeatflow",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fHeatSens,"pHe.pHLayer.fHeatSens",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fHeatLatent,"pHe.pHLayer.fHeatLatent",0.0);  \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fMeltPoint,"pHe.pHLayer.fMeltPoint",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fConduct,"pHe.pHLayer.fConduct",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fConductOld,"pHe.pHLayer.fConductOld",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fCapacity,"pHe.pHLayer.fCapacity",0.0); \
xpn_register_var_init_rec_struct( var_list,HLAYER, pHe->pHLayer,xpn_register_var_init_pdouble,fCapacityOld,"pHe.pHLayer.fCapacityOld",0.0);