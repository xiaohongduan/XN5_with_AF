
// Diese Datei kann in Zukunft automatisch mit dem tool ctag erstellte werden

#define STRUCT_DRIVE_XPN_REG(wrf,wrf_str) \
xpn_register_var_add_pint(var_list,wrf_str".start_YY",&(wrf->start_YY),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_MM",&(wrf->start_MM),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_DD",&(wrf->start_DD),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_H",&(wrf->start_H),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_M",&(wrf->start_M),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_S",&(wrf->start_S),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_MS",&(wrf->start_MS),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".start_julianday",&(wrf->start_julianday),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_YY",&(wrf->stop_YY),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_MM",&(wrf->stop_MM),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_DD",&(wrf->stop_DD),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_H",&(wrf->stop_H),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_M",&(wrf->stop_M),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_S",&(wrf->stop_S),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".stop_MS",&(wrf->stop_MS),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,wrf_str".xlat",&(wrf->xlat),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,wrf_str".xlon",&(wrf->xlon),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,wrf_str".dx",&(wrf->dx),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,wrf_str".dy",&(wrf->dy),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".grid_id",&(wrf->grid_id),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".i",&(wrf->i),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".j",&(wrf->j),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".YY",&(wrf->YY),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".MM",&(wrf->MM),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".DD",&(wrf->DD),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".H",&(wrf->H),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".M",&(wrf->M),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".S",&(wrf->S),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".MS",&(wrf->MS),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".julianday",&(wrf->julianday),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,wrf_str".dt_d",&(wrf->dt_d),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".nsoil",&(wrf->nsoil),-1,TRUE,TRUE); \
{ \
             int i; \
			 char *varname;\
             for (i=0;i<wrf->nsoil;i++) \
               { \
			   varname = g_strdup_printf("%s[%d]",wrf_str".sldpth",i); \
                  xpn_register_var_add_pfloat(var_list,varname,&(wrf->sldpth[i]),-1,TRUE,TRUE); \
				  g_free(varname); \
               } \
} \
xpn_register_var_add_pint(var_list,wrf_str".coupling",&(wrf->coupling),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".lwdn",&(wrf->lwdn),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".soldn",&(wrf->soldn),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".solnet",&(wrf->solnet),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".solardirect",&(wrf->solardirect),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".cosz",&(wrf->cosz),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".q2",&(wrf->q2),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".q2k",&(wrf->q2k),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".T2",&(wrf->T2),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".Th2",&(wrf->Th2),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".sfctmp",&(wrf->sfctmp),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".psfc",&(wrf->psfc),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".sfcspd",&(wrf->sfcspd),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".prcp",&(wrf->prcp),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".prcprain",&(wrf->prcprain),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".snowh",&(wrf->snowh),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".sneqv",&(wrf->sneqv),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".zlvl",&(wrf->zlvl),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".vegtyp",&(wrf->vegtyp),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".soiltyp",&(wrf->soiltyp),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".slopetyp",&(wrf->slopetyp),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".shdfac",&(wrf->shdfac),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".shdfac_IO",&(wrf->shdfac_IO),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".shdmin",&(wrf->shdmin),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".shdmax",&(wrf->shdmax),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".alb",&(wrf->alb),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".alb_IO",&(wrf->alb_IO),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".snoalb",&(wrf->snoalb),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".tbot",&(wrf->tbot),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".z0brd",&(wrf->z0brd),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".z0brd_IO",&(wrf->z0brd_IO),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".z0",&(wrf->z0),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".embrd",&(wrf->embrd),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".emrd_O",&(wrf->emrd_O),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".emissi",&(wrf->emissi),-1,TRUE,TRUE); \
xpn_register_var_add_pint(var_list,wrf_str".emissi_IO",&(wrf->emissi_IO),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".cmc",&(wrf->cmc),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".T1",&(wrf->T1),-1,TRUE,TRUE); \
{ \
             int i; \
			 char *varname;\
             for (i=0;i<wrf->nsoil;i++) \
               { \
			   varname = g_strdup_printf("%s[%d]",wrf_str".stc",i); \
                  xpn_register_var_add_pfloat(var_list,varname,&(wrf->stc[i]),-1,TRUE,TRUE); \
				  g_free(varname); \
               } \
} \
{ \
             int i; \
			 char *varname;\
             for (i=0;i<wrf->nsoil;i++) \
               { \
			   varname = g_strdup_printf("%s[%d]",wrf_str".smc",i); \
                  xpn_register_var_add_pfloat(var_list,varname,&(wrf->smc[i]),-1,TRUE,TRUE); \
				  g_free(varname); \
               } \
} \
{ \
             int i; \
			 char *varname;\
             for (i=0;i<wrf->nsoil;i++) \
               { \
			   varname = g_strdup_printf("%s[%d]",wrf_str".sh2o",i); \
                  xpn_register_var_add_pfloat(var_list,varname,&(wrf->sh2o[i]),-1,TRUE,TRUE); \
				  g_free(varname); \
               } \
} \
xpn_register_var_add_pfloat(var_list,wrf_str".albedo",&(wrf->albedo),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".ch",&(wrf->ch),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".cm",&(wrf->cm),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".eta",&(wrf->eta),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".eta_kinematic",&(wrf->eta_kinematic),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".sheat",&(wrf->sheat),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".fdown",&(wrf->fdown),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".ec",&(wrf->ec),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".edir",&(wrf->edir),-1,TRUE,TRUE); \
{ \
             int i; \
			 char *varname;\
             for (i=0;i<wrf->nsoil;i++) \
               { \
			   varname = g_strdup_printf("%s[%d]",wrf_str".et",i); \
                  xpn_register_var_add_pfloat(var_list,varname,&(wrf->et[i]),-1,TRUE,TRUE); \
				  g_free(varname); \
               } \
} \
xpn_register_var_add_pfloat(var_list,wrf_str".ett",&(wrf->ett),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".esnow",&(wrf->esnow),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".drip",&(wrf->drip),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".dew",&(wrf->dew),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".beta",&(wrf->beta),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".etp",&(wrf->etp),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".ssoil",&(wrf->ssoil),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".flx1",&(wrf->flx1),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".flx2",&(wrf->flx2),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".flx3",&(wrf->flx3),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".snomlt",&(wrf->snomlt),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".sncovr",&(wrf->sncovr),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".runoff1",&(wrf->runoff1),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".runoff2",&(wrf->runoff2),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".runoff3",&(wrf->runoff3),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".rc",&(wrf->rc),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".pc",&(wrf->pc),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".xlai",&(wrf->xlai),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".rsmin",&(wrf->rsmin),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".rcs",&(wrf->rcs),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".rct",&(wrf->rct),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".rcq",&(wrf->rcq),-1,TRUE,TRUE); \
xpn_register_var_add_pfloat(var_list,wrf_str".rcsoil",&(wrf->rcsoil),-1,TRUE,TRUE); 




