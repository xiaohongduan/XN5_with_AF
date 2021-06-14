#define TREEMIX_REG \
\
\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.tree.Distr [-]",&(self->plant[0].TreeDistr),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.tree.H [m]",&(self->plant[0].H),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.tree.D [m]",&(self->plant[0].D),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.tree.DBH [m]",&(self->plant[0].DBH),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.tree.LAI [m2/m2]",&(self->plant[0].LAI),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.tree.LAImax [m2/m2]",&(self->plant[0].LAImax),-1,TRUE,TRUE); \
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_HD.HDmin [-]",&(self->plant[0].HDmin),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_HD.HDmax [-]",&(self->plant[0].HDmax),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_HD.HD [-]",&(self->plant[0].HD),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_HD.CanopyFormation [-]",&(self->plant[0].CanopyFormation),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_HD.Form [-]",&(self->plant[0].Form),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_Crown.Crowding [bool]",&(self->silv[0].Crowding),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_Crown.CrArPot [-]",&(self->plant[0].CrArPot),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.geo_Crown.CrArea [m2]",&(self->plant[0].CrArea),-1,TRUE,TRUE); \
\
xpn_register_var_add_pdouble(var_list,"output.treemix.density.density [plants ha-1]",&(self->plant[0].density),-1,TRUE,TRUE); \
\
xpn_register_var_add_pdouble(var_list,"output.treemix.masses.CWdWeight [kgC ha-1]",&(self->plant[0].CWdWeight),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.masses.CLfMass [kgC ha-1]",&(self->plant[0].CLfMass),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.masses.CLfLim [kgC ha-1]",&(self->plant[0].CLfLim_kg),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.masses.CStMass [kgC ha-1]",&(self->plant[0].CStMass),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.masses.CFRtMass [kgC ha-1]",&(self->plant[0].CFRtMass),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.masses.CFrMass [kgC ha-1]",&(self->plant[0].CFrMass),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.Precip [mm]",&(self->plant[0].PreciRate_Yr),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.CanPrecip [mm]",&(self->plant[0].CanPreci_Yr),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.FreeThroughfall [mm]",&(self->plant[0].FreeThroughfall_Yr),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.Throughfall [mm]",&(self->plant[0].Throughfall_Yr),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.Interception [mm]",&(self->plant[0].Interception_Year),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.Drainage [mm]",&(self->plant[0].Drainage_Yr),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_TotSum.Balance [mm]",&(self->plant[0].Bal_P),-1,TRUE,TRUE); \
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_DaySum.Precipitation [mm]",&(self->plant[0].PreciRate_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_DaySum.Throughfall [mm]",&(self->plant[0].Throughfall_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_DaySum.InterceptionLoss [mm]",&(self->plant[0].Interception_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Water_DaySum.IEfracP [-]",&(self->plant[0].IE_P),-1,TRUE,TRUE); \
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.N_Content.NLeafContent [tN ha-1]",&(self->plant[0].LeafContent),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.N_Content.NRootContent [tN ha-1]",&(self->plant[0].RootContent),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.N_Content.NTotalContent [tN ha-1]",&(self->plant[0].TotalContent),-1,TRUE,TRUE); \
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Photosynthesis_DaySum.NPP [tCha-1d-1]",&(self->plant[0].PhCanopyNoN_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Photosynthesis_DaySum.NPPnlim [tCha-1d-1]",&(self->plant[0].PhCanopyNlim_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Photosynthesis_DaySum.GPPFalge [tCha-1d-1]",&(self->plant[0].GrossCanopyPrd_Day),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Photosynthesis_TotSum.NPP [tCha-1]",&(self->plant[0].PhCan_Tot ),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Photosynthesis_TotSum.NPPnlim [tCha-1]",&(self->plant[0].PhCanopyNlim_Yr ),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Photosynthesis_TotSum.GPPFalge [tCha-1]",&(self->plant[0].GrossCanopyPrd_Sum ),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_DaySum.MaintRespLf [tCha-1d-1]",&(self->plant[0].MainRespLf_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_DaySum.MaintRespSt [tCha-1d-1]",&(self->plant[0].MainRespSt_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_DaySum.MaintRespRt [tCha-1d-1]",&(self->plant[0].MainRespRt_Day),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_DaySum.MaintResp [tCha-1d-1]",&(self->plant[0].MaintResp__Day),-1,TRUE,TRUE); \
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_TotSum.MaintRespLf [tC ha-1]",&(self->plant[0].MaintRespLf_Sum),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_TotSum.MaintRespRt [tC ha-1]",&(self->plant[0].MaintRespRt_Sum),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_TotSum.MaintRespSt [tC ha-1]",&(self->plant[0].MaintRespSt_Sum),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Resp_TotSum.MaintResp [tC ha-1]",&(self->plant[0].MaintResp_Sum),-1,TRUE,TRUE); \
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.C_Demand.CLeafDemand [tC ha-1]",&(self->plant[0].CLfDemand),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.C_Demand.CRootDemand [tC ha-1]",&(self->plant[0].CRtDemand),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Nutrients_C.CSurplus [tC ha-1]",&(self->plant[0].CSurplus),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Nutrients_C.CAss [kgC ha-1]",&(self->plant[0].CAss),-1,TRUE,TRUE); \
xpn_register_var_add_pdouble(var_list,"output.treemix.Nutrients_C.TOLfMass [tC ha-1]",&(self->plant[0].TOLfMass),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Nutrients_C.TORtMass [tC ha-1]",&(self->plant[0].TORtMass),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Nutrients_C.CLfRtConstr [tC ha-1]",&(self->plant[0].CLfRtConstr),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Nutrients_N.NSurplus [tN ha-1]",&(self->plant[0].NSurplus),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Incr.IncrSt [tC tree-1 yr-1]",&(self->plant[0].IncrSt),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Incr.IncrD_Sum [m]",&(self->plant[0].IncrD_Sum),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Incr.IncrH_Sum [m]",&(self->plant[0].IncrH_Sum),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Incr.Increment [tC ha-1]",&(self->plant[0].Increment),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Incr.IncrLimitation [bool]",&(self->plant[0].IncrLimitation),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Roots.TRLD",&(self->plant[0].TRLD),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_TotSum.NUpAct [kgNha-1]",&(self->plant[0].NUpAct_Sum),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_TotSum.ActNO3Up [kgNha-1]",&(self->plant[0].ActNO3Up_Sum),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_TotSum.ActNH4Up [kgNha-1]",&(self->plant[0].ActNH4Up_Sum),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.NUpLfRt [tN ha-1]",&(self->plant[0].NUpLfRt_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.NUpAct_1 [kgNha-1]",&(self->plant[0].NUpAct_1_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.NUpAct_2 [kgNha-1]",&(self->plant[0].NUpAct_2_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.NUpAct_3 [kgNha-1]",&(self->plant[0].NUpAct_3_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.ActNO3Up [kgNha-1]",&(self->plant[0].ActNO3Up_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.ActNH4Up [kgNha-1]",&(self->plant[0].ActNH4Up_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptake_DaySum.NUpPot [tNha-1]",&(self->plant[0].NUpPot_Day),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.NUptakeFac.NUpNormFac [-]",&(self->plant[0].NUpNormFac),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Senescence.TotDeadWdLoss [kgC ha-1 yr-1]",&(self->plant[0].TotDeadWdLoss),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Senescence.DeadTree [kgC ha-1 yr-1]",&(self->plant[0].DeadTree),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Senescence.CTrMass [kgC ha-1 yr-1]",&(self->plant[0].CTrMass),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.Litter.Lf [kgC ha-1 yr-1]",&(self->plant[0].LitLf),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Litter.Rt [kgC ha-1 yr-1]",&(self->plant[0].LitRt),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Litter.GrRt [kgC ha-1 yr-1]",&(self->plant[0].LitGrRt),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Litter.Br [kgC ha-1 yr-1]",&(self->plant[0].LitBr),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.Litter.St [kgC ha-1 yr-1]",&(self->plant[0].LitSt),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.LitterInput.CLf_Sum [tC ha-1]",&(self->plant[0].CLitLf_Sum),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.LitterInput.CBr_Sum [tC ha-1]",&(self->plant[0].CLitBr_Sum),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.LitterInput.CSt_Sum [tC ha-1]",&(self->plant[0].CLitSt_Sum),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.TranspCanDay.Day [mm d-1]",&(self->plant[0].TransCan_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.TranspCanTot.Tot [mm]",&(self->plant[0].TransCan_Tot),-1,TRUE,TRUE);\
\
xpn_register_var_add_pdouble(var_list,"output.treemix2.TransCanDay.Day [mm d-1]",&(self->plant[1].TransCan_Day),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix2.TransCanTot.Tot [mm]",&(self->plant[1].TransCan_Tot),-1,TRUE,TRUE);\
\
\
xpn_register_var_add_pdouble(var_list,"output.treemix.NDepos.Tot [kgN ha-1 d-1]",&(self->plant[0].NDeposTotal),-1,TRUE,TRUE);\
xpn_register_var_add_pdouble(var_list,"output.treemix.CO2.CO2_air [ppm]",&(self->clim.CO2_air),-1,TRUE,TRUE);\
