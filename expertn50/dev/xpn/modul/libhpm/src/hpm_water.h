//      hpm_water.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM__WATER_H__
#define __HPM__WATER_H__

#include "hpm.h"

G_BEGIN_DECLS



double calculateDayLength(double latitude, int JulianDay);
// Calculates the Root Pool 6.3a
/*
 ret: DWrt: [kg Water / (m^2 * day)] Rates from the pools
**/
//theta_so_max = fContMax
double calculateWaterRootPool(hpm *self,double *IWso_rt,double *OWrt_sh,double Psi_sh,double Psi_rt, double theta_so, double theta_so_max);
double calculateWaterRootPool_feddes(hpm *self,double *IWso_rt,double *OWrt_sh, double Psi_sh,double Psi_rt, double theta_so);

double hpm_water_FAlfa(hpm *self,double rRoot,double h);

// Calculates the Shoot Pool 6.4a
/*
 ret: DWsh: [kg Water / (m^2 * day)] Rates from the pools
**/
double calculateWaterShootPool(hpm *self,double IWrt_sh, double Psirt, double Psish);



// 6.6c  [m s^-1] boundary layer conductance
double get_canopy_conductance(hpm *self,double LAI);

/*
Rechnet den Blatt stomata Fluss (vom CO2 aus) aus
Er ist hängt u.a. von der Luft CO2 konzentration ab

in:

out:
 * Plant.gsmax;
ret: gstom [m s-1 (m2 leaf)-1]
*/
double calculateLeafStomatalConductance(hpm *self,double theta_sh);


// water_uptake_model: 0: original hpm, 1: feddes model
int hpm_water_uptake(hpm *self,int water_uptake_model);


G_END_DECLS

#endif /* __HPM_H__ */
