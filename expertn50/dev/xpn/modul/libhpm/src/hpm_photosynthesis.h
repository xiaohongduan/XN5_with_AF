//      hpm_photosynthesis.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM__PHOTOSYNTHESIS_H__
#define __HPM__PHOTOSYNTHESIS_H__

#include "hpm.h"

G_BEGIN_DECLS

// TODO: Muss später entfernt werden
double CalculateActiveRadiation(hpm *self,double DayTime,double SunDuration, double SolRad );
double calulateMaxGrossphotosynthesisRate_HPM(hpm *self);
double calculateCanopyPhotosynthesis_getAlpha(hpm *self,double Tair, double CO2air, double aWsh);

/*
Berechnet Temperatur funktion (maximum) ist von der CO2 concentration der Luft abhängig
in:
 * Plant.TmaxPmax700
 * Plant.TmaxPmax350
 * Plant.CO2vpm
out:
 * pPl->pGenotype->fTempOptPs // Optimale Photsynthesetemperatur
 * pPl->pGenotype->fTempMaxPs // Maximale Photosynthestemp
 * pPl->pGenotype->fTempMinPs // Minimale Photosynthestemp = 0.0
ret:
 * fTMax // Temperature function
*/

double calculatePlantTemperatureProcesses_getfTPMax_HPM(hpm *self, double T);

G_END_DECLS

#endif /* __HPM_H__ */
