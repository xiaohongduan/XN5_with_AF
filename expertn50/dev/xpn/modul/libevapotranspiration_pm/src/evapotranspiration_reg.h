#define EVAPOTRANSPIRATION_REG \
xpn_register_var_add_pdouble(var_list,"evapotranspiration.EpotR",&(xpn->pWa->pEvap->fPotR),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"evapotranspiration.EactR",&(xpn->pWa->pEvap->fActR),-1,TRUE,TRUE); \
\
xpn_register_var_add_pdouble(var_list,"evapotranspiration.TpotR",&(xpn->pPl->pPltWater->fPotTranspR),-1,TRUE,TRUE); \
\
xpn_register_var_add_pdouble(var_list,"evapotranspiration.ETpotR",&(xpn->pWa->fPotETR),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"evapotranspiration.ETpot_dt_XN",&(xpn->pWa->fPotETdt),-1,TRUE,TRUE); \
\
\
xpn_register_var_add_pdouble(var_list,"evapotranspiration.ETpot_dt",&(self->ETpot_dt),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"evapotranspiration.ETpot_Day",&(self->ETpot_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"evapotranspiration.ETpot_Year",&(self->ETpot_Year),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"evapotranspiration.SEB_ETpot_Year",&(self->SEB_ETpot_Year),-1,TRUE,TRUE);
