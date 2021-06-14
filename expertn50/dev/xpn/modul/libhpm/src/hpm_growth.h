//      hpm_growth.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM__GROWTH_H__
#define __HPM__GROWTH_H__

#include "hpm.h"

G_BEGIN_DECLS

void clear_all_fluxes(hpm *self);

/*
Berechnet Temperatur funktion (für verschiedene Prozesse als Gewichtungsfaktor): //3.11a
in:
 * T: temperatur
ret:
 * f // Temperature function
*/

double calculatePlantTemperatureProcesses_getf_HPM(hpm *self, double T);

void calculateLaminaAreaPools_HPM(hpm *self,double Gsh, double thetash, double kturnlam,double kdegsh );
double calculateBiomassStructuralLamina(hpm *self, double Gsh, double kturnlam, double kdegsh, double fTsh, double fWsh, double flam);
double calculateBiomassStructuralSheathAndStem(hpm *self, double Gsh, double kturnss, double kdegsh ,double fTsh, double fWsh, double flam);
double calculateBiomassStructuralRoot(hpm *self,double Grt, double fTrt, double fWrt,double kturnrt, double kdegrt, double qWPl, double mu20);
double calculateBiomassSubstrateCShootFromShootToRoot(hpm *self,double MCSsh, double MCSrt,double MXsh, double MXrt, double fTsh, double fTrt, double fWsh, double fWrt);
double calculateBiomassSubstrateCShootOuput_for_sh_maintenace(hpm *self,double *Msh4, double MCSsh,double MXsh,double fTsh, double fWsh, double fCplX);
double calculateBiomassSubstrateCRootOuput_for_rt_maintenace(hpm *self,double *Mrt4, double MCSrt,double MXrt,double fTrt, double fWrt, double fCplX);
void calculateBiomassSubstrateCShoot(hpm *self,double *OCSsh_rt, double ICSsh_re ,double *Msh4, double Gsh, double fTsh, double fTrt, double fWsh, double fWrt ,double kturnlam, double kturnss, double kdegsh);
void calculatePhotoynthetic_N(hpm *self,double *ONSsh_Nph,double *ONph_NSsh,double *INph_li_NSsh,double OXsh_li,double fTsh,double fWsh,double MXsh);
double calculateBiomassSubstrateNShoot(hpm *self,double *INSsh_rt,double ONSsh_Nph,double INSsh_Nph,double INph_li_NSsh,double Gsh, double fTsh,double fTrt,double fWsh, double fWrt,double kturnlam, double kturnss, double kdegsh);
double calculateBiomassSubstrateNRoot(hpm *self,double ONSsh_rt,double Namm, double Nnit,double Grt,double  Tsoil, double fTrt, double fWrt,double thetasoil, double kturnrt, double kdegrt);
void calculateBiomassSubstrateCRoot(hpm *self, double Namm, double Nnit, double Tsoil, double thetasoil, double ICSsh_rt, double ICSrt_re,double Gsh,  double Grt, double fTrt, double fWrt ,double kturnrt , double kdegrt);
double calculateBiomassSubstrateNShoot_TransportSubstateNfromRootToShoot(hpm *self,double MXsh, double MXrt, double fTsh, double fTrt, double fWsh, double fWrt, double Nsh, double Nrt);
double calculateNitrogenUptakeSoil(hpm *self,double Namm, double Nnit, double Tsoil, double thetasoil,double  MCSrt, double MNSrt, double MXrt, double *Mrt4, double fTrt, double fWrt);
double calculateBiomassSubstrateCRootNUptake(hpm *self,double Namm, double Nnit, double Tsoil, double thetasoil, double  MCSrt, double MNSrt, double MXrt, double *Mrt4, double fTrt, double fWrt);

G_END_DECLS

#endif /* __HPM_H__ */
