#include "century_n.h"


#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int  century_n_Denitrification_init(century_n *self)
{
	int i;
	
	
	century_n__general_init(self);	

	for (i=0; i<14; i++)
		{
			self->dppt2lastweeks[i] = 0.0;
		}

	self->dfPreciRate=0.0;
	self->denit_daycount = 14;
	
	for (i=0;i<12;i++)
	{
		self->dNOxPulse[i]=1.0;
	}

	return RET_SUCCESS;
}

int  century_n_Denitrification_done(century_n *self)
{
	if (self->config_filename!=NULL)
		free(self->config_filename);
	self->config_filename=NULL;
	return RET_SUCCESS;
}

int century_n_EmitAllN2O(century_n *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PTIME        pTi = xpn->pTi;
    PSPROFILE    pSo = xpn->pSo;
    PCHEMISTRY   pCh = xpn->pCh;

    int   iLayer;
    double fN2O_In_Profile = (double)0;
    double fNO_In_Profile  = (double)0;
    double fN2_In_Profile  = (double)0;

    PCLAYER      pCL= pCh->pCLayer;

    pCL->fN2ONGasConc               =
        pCL->fN2ONGasConcOld            = (double)0;
    /* Athmosphaerische N2O-Konzentration nicht beachten */


    for(SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
        {
            if (pCL->fN2ON > 1.e-10)
                {
                    fN2O_In_Profile += pCL->fN2ON;
                    fNO_In_Profile  += pCL->fNON;
                    fN2_In_Profile  += pCL->fN2N;
                }

            pCL->fN2ONGasConc    =
                pCL->fN2ONGasConcOld =
                    pCL->fN2ON           =
                        pCL->fNON            =
                            pCL->fN2N            = (double)0;
        }

    pCh->pCProfile->fN2OEmisR = fN2O_In_Profile/((double)24e-5 * pTi->pTimeStep->fAct);
    /*  [ mu g N /m^2 /h] */
    pCh->pCProfile->fNOEmisR  = fNO_In_Profile/((double)24e-5 * pTi->pTimeStep->fAct);
    /*  [ mu g N /m^2 /h] */
    pCh->pCProfile->fN2EmisR  = fN2_In_Profile/((double)24e-5 * pTi->pTimeStep->fAct);
    /*  [ mu g N /m^2 /h] */

    return RET_SUCCESS;
}      /* EmitAllN2O */





int century_n_Denitrification_run(century_n *self) //daily loss of NH4 due to nitrification [gN/m2] //NH4 goes to NH3, N2O and N0
{

	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PCLIMATE pCl = xpn->pCl;
	int i,j;
	PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
	PCLAYER	 pCL=pCh->pCLayer->pNext;
	PWLAYER  pWL=pWa->pWLayer->pNext;
	PSWATER  pSW=pSo->pSWater->pNext;



	double co2PPM[1000]; //newCO2 [ppm]
	double grams_soil[1000]; //grams of soil in a layer in a 1m x 1m section
	double  nitratePPM[1000];//soil nitrate (NO3-) concentration (ppm)
	double fluxNTotal[1000];// soil (N2+N2O) denitrif. flux by lyr ( gN/m^2 )
	double fluxTotalDenitPPM; // total (N2+N2O) denitrif. flux (ppm N/day)
	double gramsSoil;   		// soil mass in layer (g m-2)
	double fluxN2Odenit[1000];	// N2O flux from denitrification (gN/m2/day)
	double fluxN2denit[1000];	// N2 flux from denitrification (gN/m2/day)
	double fluxNOdenit[1000];	// NO flux from denitrification (gN/m2/day)
	double  porosity;
	double  wfps_fc,dD0_fc;	// water filled pore space at field capacity (0-1)
	double WFPS_threshold;//  water filled pore space threshold (0-1)
	double WFPS;
	double co2PPM_correction;
	double a;
	double  fDno3;		// Nitrate effect on denitrification
	double fDco2;		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
	double fDwfps, M, x_inflection;		// wfps effect on denitrification (fDwfps, 0-1?)
	double fRno3_co2, k1;//  Nitrate effect on the ratio of N2 to N2O
	double fRwfps;		// WFPS effect on the N2/N2O Ratio
	double ratioN2N2O;	// N2:N2O Ratio
	double fluxN2O ;
	double excess = 0.0;	// amount of N flux in excess of what
	//   can actually be removed from soil (gN/m^2)
	double totalFlux;
	double n2oFrac ; // N2O fraction of flux
	double n2rac  ; // N2 fraction of flux
	double minNitrate_final;
	double fluxOut;
	//double dkPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
	double potentialFluxDenitrifNO;	//   maximum possible denitrification NO flux
	//   based on NO/N2O and dkPrecipNO (gN/m^2/day)
	double NO_N2O_ratio;//ratio NO:N2O
//constants
	double const ug_per_gram = 1.0E6;		// micrograms per gram (ppm)
	double const grams_per_ug = 1.0E-6;		// grams per microgram
	// min. nitrate concentration required in a lyr for trace gas calc. (ppm N)
	double const minNitratePPM = 0.1;
	// min. allowable nitrate per lyr at end of day (ppm N)
	double const minNitratePPM_final = 0.05;
	double const cm2m2 = 10000.0;	// cm^2 per m^2
	double A[4] =	        // parameters to parton-innis functions
	{ 9.23, 1.556, 76.91, 0.00222 };
	double const tolerance = 1.0E-30;
	double sumppt;




	if (NewDay(pTi))
		{


			//  multiplier NOx flux based on recent rain events and snowpack
			// increase of NO due to moisture and rain >= 1.0
			//shift the daily values and add current day's ppt [cm]
			for(j=0; j<13; j++) //shift the daily values and add current day's ppt [cm]
				{
					self->dppt2lastweeks[j]=self->dppt2lastweeks[j+1];
				}
			//dppt2lastweeks[13] = pCl->pWeather->fRainAmount*0.1;
//ck:
			self->dppt2lastweeks[13] = self->dfPreciRate*0.1;

			// Da das Regenarray erst nach 2 Wochen gefüllt ist, darf darf das Modell erst nach 2 Wochen Starten:
			if (self->denit_daycount<14)
				{
					self->denit_daycount++;
				}
			else
				{


					//sum of rain during last 2 weeks:
					sumppt=0.0;
					for(j=0; j<14; j++) //shift the daily values and add current day's ppt [cm]
						{
							sumppt += self->dppt2lastweeks[j];
						}
					//get pulse factor for today
					self->dkPrecipNO = self->dNOxPulse[0];
					for(j=0; j<11; j++) //shift the daily values and add factor 1.0 for the last day
						{
							self->dNOxPulse[j]=self->dNOxPulse[j+1];
						}
					self->dNOxPulse[11]=1.0;

					//calculate future pulses
					//new pulse, if sum is lower than 1cm and no snow
					if (sumppt <= 1.0 && (pCl->pWeather->fSnow<=0.0) )
						{
//		if(pCl->pWeather->fRainAmount*0.1 <= 0.5)//low rain event
//ck:
							if(self->dfPreciRate*0.1 <= 0.5)//low rain event
								{
									for(j=0; j<1; j++) //low rain event pulses last 1 day
										{
											self->dNOxPulse[j] = MAX(self->dNOxPulse[j], 11.19 *  (double)exp(-0.805 * (j+2)) );
										}
								}
//		 if(pCl->pWeather->fRainAmount*0.1 > 0.5//medium rain event
//		&& pCl->pWeather->fRainAmount*0.1 <= 1.5)
//ck:
							if(self->dfPreciRate*0.1 > 0.5//medium rain event
							        && self->dfPreciRate*0.1 <= 1.5)

								{
									for(j=0; j<5; j++) //medium rain event pulses last 5 days
										{
											self->dNOxPulse[j] = MAX(self->dNOxPulse[j], 14.68 * (double)exp(-0.384 * (j+2)) );
										}
								}

//		if(pCl->pWeather->fRainAmount*0.1 > 1.5)//high rain event
//ck:
							if(self->dfPreciRate*0.1 > 1.5)//high rain event
								{
									for(j=0; j<12; j++) //large rain event pulses last 12 days
										{
											self->dNOxPulse[j] = MAX(self->dNOxPulse[j], 18.46 * (double)exp(-0.208 * (j+1)) );
										}
								}
						}



					//*********************************************************************
					// Convert newCO2 (g/m2) to co2PPM[] (ppm) and distrbute it
					// through the soil profile
					//*********************************************************************
					for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext,pCL=pCL->pNext)
						{
							gramsSoil = pSL->fBulkDens  * pSL->fThickness * 0.1   * 1.0e4;//soil mass in layer (g m-2)
							if(gramsSoil <= 0.0) PRINT_ERROR_ID((&(self->parent)),"Denitrify Error: Devison by 0");
							co2PPM[i] =  (pCL->fCO2C_dt*kgphaTOgpm2 ) / gramsSoil * 1.0e6;
							if(co2PPM[i]<1e-30) co2PPM[i]=0.0;
						}

					//*********************************************************************
					// grams of soil in a layer in a 1m x 1m section
					//*********************************************************************
					for(pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext)
						{
							grams_soil[i] = pSL->fBulkDens  * pSL->fThickness * 0.1 * cm2m2;
						}

					//*********************************************************************
					// soil nitrate (NO3-) concentration (ppm)
					//*********************************************************************
					for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2; i++,pCL=pCL->pNext)
						{
							nitratePPM[i] = pCL->fNO3N * kgphaTOgpm2 / (double)grams_soil[i] * ug_per_gram;
						}

					//*********************************************************************
					// denitrification loop
					//  Dentrification occurs over all layers
					//*********************************************************************
					//  Convert nitrate (gN/m2) to nitratePPM (ppm N)
					for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater->pNext,i=0;
					        i<pSo->iLayers-2;
					        i++, pSL= pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,pSW=pSW->pNext)
						{
							fluxN2Odenit[i]=0.0;
							fluxN2denit[i]=0.0;
							
							if (nitratePPM[i] < minNitratePPM) continue;

							// normalized diffusivity in aggregate soil
							//   media, at a standard field capacity (0-1)
							//dD0_fc = diffusiv(&stdfieldc, &stdbulkd, &wfps_fc);
							//dD0 calc changed 6/20/00 -mdh
							// water filled pore space at field capacity (0-1)
							porosity = 1.0 - pSL->fBulkDens / (double)2.65;
							wfps_fc = pSW->fContFK / porosity;
							wfps_fc = MAX ( 0.0, wfps_fc );
							wfps_fc = MIN ( wfps_fc, 1.0 );
							dD0_fc = century_n_Diffusivity(self,pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,wfps_fc);
							if(dD0_fc < 0 || dD0_fc > 1.0 ) PRINT_ERROR_ID((&(self->parent)),"Warning - Denitrification: dD0_fc out of range");

							// water filled pore space threshold (0-1)
							WFPS_threshold =
							    (dD0_fc >= 0.15) ? 0.80 : (dD0_fc * 250.0 + 43.0) / 100.0;
							if(WFPS_threshold < 0 || WFPS_threshold > 1.0 )
								PRINT_ERROR_ID((&(self->parent)),"Warning - Denitrification: WFPS_threshold out of range");

							WFPS = pWL->fContAct / pSL->fPorosity;


							// CO2 correction factor when WFPS has reached threshold
							if (WFPS <= WFPS_threshold)
								{
									co2PPM_correction = co2PPM[i];
								}
							else
								{
									a = (dD0_fc >= 0.15) ? 0.004 : (-0.1 * dD0_fc + 0.019);
									co2PPM_correction = co2PPM[i] * (1.0 + a * (WFPS - WFPS_threshold) * 100.0);
								}
							if(co2PPM_correction < 0.0)
								{
									PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: co2PPM_correction < 0.0");
								}

							// Nitrate effect on denitrification
							// denitrification flux due to soil nitrate (ppm N/day)
							// Changed NO3 effect on denitrification based on
							// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
							// fDno3 = 1.15 * std::pow(nitratePPM(layer), 0.57);
							fDno3 = MAX( 0.0, century_n_f_arctangent(self,nitratePPM[i], A) );

							//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
							//  Changed CO2 effect on denitrification based on
							//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
							fDco2 = MAX(0.0,(0.1 * pow(co2PPM_correction, 1.3) - minNitratePPM) );

							// wfps effect on denitrification (fDwfps, 0-1?)
							// Changed wfps effect on denitrification based on
							// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
							M =  MIN(0.113, dD0_fc) * (-1.25) + 0.145;
							x_inflection = 9.0 - M * co2PPM_correction;
							fDwfps = MAX ( 0.0, 0.45 +
							               (atan(0.6 *  3.14159265358979  * (10.0 * WFPS - x_inflection))) / 3.14159265358979  );

							//  N fluxes (N2 + N2O) for the current layer, ppm N
							fluxTotalDenitPPM = // total (N2+N2O) denitrif. flux (ppm N/day)
							    (fDno3 < fDco2) ? fDno3 : fDco2;
							if(fluxTotalDenitPPM < 0.0) PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxTotalDenitPPM < 0.0");
							// Minimum value for potential denitrification in simulation layers.
							fluxTotalDenitPPM = MAX(0.066, fluxTotalDenitPPM);
							fluxTotalDenitPPM *= fDwfps;

							//  Nitrate effect on the ratio of N2 to N2O
							//  Maximum N2/N2O ratio soil respiration function
							//  Changed the NO3 and CO2 effect on the N2/N2O ratio based on
							//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
							k1 = MAX (1.5, 38.4 - 350 * dD0_fc);
							if(k1 < 0.0) PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: k1 < 0.0");
							fRno3_co2 = MAX (0.16 * k1,
							                 k1 * exp(-0.8 * nitratePPM[i] / co2PPM[i]) );
							if(fRno3_co2 < 0.0) PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fRno3_co2 < 0.0");

							// WFPS effect on the N2/N2O Ratio
							// Changed wfps effect on the N2/N2O ratio based on
							// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
							fRwfps = MAX (0.1, 0.015 * WFPS * 100.0 - 0.32);

							// N2:N2O Ratio
							// ratioN2N2O = (fRno3 < fRco2) ? fRno3 : fRco2;
							// ratioN2N2O *= fRwfps;
							ratioN2N2O = MAX(0.1, fRno3_co2 * fRwfps);

							// Compute N2 and N2O flux by layer (fluxNTotal(layer))
							// convert ppm N to gN/m^2
							fluxNTotal[i] = fluxTotalDenitPPM * grams_soil[i] * grams_per_ug;
							if(fluxNTotal[i] > 10.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxNTotal > 10.0");
							fluxN2O = fluxNTotal[i] / (ratioN2N2O + 1.0);
							fluxN2Odenit[i] = fluxN2O;
							fluxN2denit[i]	= MAX(0.0, fluxNTotal[i] - fluxN2O);
							if(fluxN2Odenit[i] < 0.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxN2Odenit < 0.0");
							if(fluxN2denit[i] < 0.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxN2denit < 0.0");
						} // denitrification loop

					//*********************************************************************
					//  Reduce nitrate in soil by the amount of N2-N N2O-N that is lost
					//  Do not let nitrate in any layer go below minNitratePPM_final
					//*********************************************************************
					for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2; pCL=pCL->pNext, i++)
						{
							totalFlux = fluxN2Odenit[i] + fluxN2denit[i];
							if (totalFlux > 1.0E-30)	// have any flux?
								{
									n2oFrac = fluxN2Odenit[i] / totalFlux; // N2O fraction of flux
									n2rac  = fluxN2denit[i]  / totalFlux; // N2 fraction of flux

									minNitrate_final = minNitratePPM_final * grams_soil[i] * grams_per_ug;
									if ( nitratePPM[i] < minNitratePPM )	// No flux from layer?
										{
											excess += fluxNTotal[i];
										}
									else if (pCL->fNO3N*kgphaTOgpm2 - fluxNTotal[i] > minNitrate_final )
										{
											// remove N in calculated trace gas flux from the layer
											pCL->fNO3N -= (double)fluxNTotal[i] * gpm2TOkgpha;
										}
									else
										{
											// reduce trace gas flux in layer so soil N won't fall below min
											// actual total trace gas flux (gN/m^2)
											fluxOut =
											    (nitratePPM[i] - minNitratePPM_final) *
											    grams_soil[i] * grams_per_ug;
											excess += fluxNTotal[i] - fluxOut;
											pCL->fNO3N = (double)minNitrate_final * gpm2TOkgpha;
										}
									fluxN2Odenit[i] -= n2oFrac * excess;
									fluxN2denit[i]  -= n2rac * excess;
									fluxN2denit[i]=MAX(0.0, fluxN2denit[i]);
									fluxN2Odenit[i]=MAX(0.0, fluxN2Odenit[i]);
								}
							else //no Flux: totalFlux < 1.0E-30
								{
									fluxN2Odenit[i] = 0.0;
									fluxN2denit[i] = 0.0;
								}

//*********************************************************************
// --- Partition denitratrification fluxes ---
//*********************************************************************
							// For denitrification, dkPrecipNO is >= 1.0 -mdh 6/22/00
							// potentialFluxDenitrifNO =
							//   maximum possible denitrification NO flux
							//   based on NO/N2O and dkPrecipNO (gN/m^2/day)

							//*********************************************************************
							// ratio NO:N2O and NO flux
							//*********************************************************************
							WFPS = pWL->fContAct / pSL->fPorosity;
							NO_N2O_ratio = 15.23 + ( 35.45 *
							                         atan ( 0.676 * 3.141592 *
							                                (10.0 * century_n_Diffusivity(self,pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,(double)WFPS) - 1.86) )
							                       ) / 3.141592;
							if(NO_N2O_ratio < 0.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denitrification: NO_N2O_ratio < 0");
							if (self->iForest == 0)// true if agricultural system or grassland
								{
									NO_N2O_ratio *= 0.20;
								}
							potentialFluxDenitrifNO = fluxN2Odenit[i] * NO_N2O_ratio *  MIN (1.0, self->dkPrecipNO);
							if(potentialFluxDenitrifNO < 0.0) PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: potentialFluxDenitrifNO < 0");

							if (potentialFluxDenitrifNO <= pCL->fNH4N * kgphaTOgpm2)//  Take all N out of NH4
								{
									fluxNOdenit[i] = potentialFluxDenitrifNO;
									pCL->fNH4N -= (double)potentialFluxDenitrifNO * gpm2TOkgpha;
								}
							else// NH4 limits; some N from N2O
								{
									// Convert all ammonium to NO
									fluxNOdenit[i] = pCL->fNH4N * kgphaTOgpm2;
									potentialFluxDenitrifNO -= pCL->fNH4N * kgphaTOgpm2;
									pCL->fNH4N = 0.0;
									// convert some N2O to NO
									if (potentialFluxDenitrifNO <= fluxN2Odenit[i])
										{
											fluxNOdenit[i] += potentialFluxDenitrifNO;
											fluxN2Odenit[i] -= potentialFluxDenitrifNO;
										}
								}// NH4 limits; some N from N2O

							// Check for very small values
							if(fluxNOdenit[i] < 0.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxNOdenit < 0");;
							if (fluxNOdenit[i] < tolerance)
								fluxNOdenit[i] = 0.0;
							if(fluxN2Odenit[i] < 0.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxN2Odenit < 0");;
							if (fluxN2Odenit[i] < tolerance)
								fluxN2Odenit[i] = 0.0;
							if(fluxN2denit[i] < 0.0)PRINT_ERROR_ID((&(self->parent)),"Warning - Denit.: fluxN2denit < 0");;
							if (fluxN2denit[i] < tolerance)
								fluxN2denit[i] = 0.0;

//*********************************************************************
// Write fluxes to XN variables
//	fluxN2denit[i]	-	[g/m2/day]
//	fluxN2Odenit[i]	-	[g/m2/day]
//	fluxNOdenit[i]	-	[g/m2/day]
//*********************************************************************
							pCL->fNO3DenitR = (double)(fluxN2Odenit[i] + fluxNOdenit[i] +fluxN2denit[i])
							                  * gpm2TOkgpha; // pTi->pTimeStep->fAct;; //[kg/ha/day]
							//* (double)1e6 / (double)24.0 /(double)pTi->pTimeStep->fAct;//[ug/m2*h] per timestep

							//pCh->pCProfile->fN2OEmisR += (double)(fluxN2Odenit[i] + fluxNOdenit[i] +fluxN2denit[i])
							//* (double)1e6 / (double)24.0;    //[ug/m2*h] per timestep
							//XN: NxOx Emission has to be balanced somehow for the graphical output.
							//XN: just do it the same way as for N2O
							//till it is fixed, write the NOx emission to N2O emission
//		pCh->pCProfile->fNOEmisR  += (double)(fluxNOnit) * 1e5 / 24.0 /(double)pTi->pTimeStep->fAct;  //[ug/m2*h] per timestep

//also fN2EmisR is not summed up!

							pCL->fN2ON += fluxN2Odenit[i]* gpm2TOkgpha;//[kg/ha]
							pCL->fNON  += fluxNOdenit[i] * gpm2TOkgpha;//[kg/ha]
							pCL->fN2N  += fluxN2denit[i] * gpm2TOkgpha;//[kg/ha]

						}  // for layer
				}
			self->dfPreciRate=0.0;
		} //new Day

	// Da die Regenrate sich ständig änderen kann

	self->dfPreciRate += pCl->pWeather->fPreciRate*pTi->pTimeStep->fAct;
	
	century_n_EmitAllN2O(self);

	return RET_SUCCESS;
}
