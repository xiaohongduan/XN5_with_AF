#include "century_n.h"


#include <glib.h>d
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int  century_n_Nitrification_init(century_n *self)
{
	int i;

	century_n__general_init(self);

	for (i=0; i<14; i++)
		{
			self->ppt2lastweeks[i] = 0.0;
		}

	self->fPreciRate=0.0;
	self->nitri_daycount = 14;

	for (i=0; i<12; i++)
		{
			self->NOxPulse[i]=1.0;
		}

	return RET_SUCCESS;
}

int  century_n_Nitrification_done(century_n *self)
{
	if (self->config_filename!=NULL)
		free(self->config_filename);
	self->config_filename=NULL;
	return RET_SUCCESS;
}

int century_n_Nitrification_run(century_n *self) //daily loss of NH4 due to nitrification [gN/m2] //NH4 goes to NH3, N2O and N0
{

	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PCLIMATE pCl = xpn->pCl;
	PHEAT pHe = xpn->pHe ;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
	PWLAYER  pWL=pWa->pWLayer->pNext;	// used to iterate over the linked list
	PCLAYER  pCL=pCh->pCLayer->pNext;	// used to iterate over the linked list
	PHLAYER  pHL=pHe->pHLayer->pNext;	// used to iterate over the linked list
	PSWATER  pSW=pSo->pSWater->pNext;
	enum  TSoilTextureIndex textureindex=UNDEFINED;

	double sumppt;//rain amount of the last 2 weeks [cm]

	//index 0 is tomorrow
	double NH4_to_NO3 = 0.0,ret=0.0;			// amount of NH4 converted to NO3 [kg/ha]
	double newNO3 = 0.0;			// new NO3 left after Trace gas production(gN/m^2/day)
	double const MaxNitrifRate = 0.10;	// max fraction of ammonium that goes to NO3 during nitrification
	double const min_ammonium = 0.1;	// min. total ammonium in soil layer (kgN/ha)
	//double const min_ammonium = 0.15;	// min. total ammonium in soil (gN/m^2)
	int i,j;
	double a, b, c, d, base1, base2, e1,e2;
//	double avgWFPS = getWFPStoDepth(20.0, exp_p); //water filled pore space to 20cm depth
	double  WFPS;
	double fNwfps, fNsoilt, fNph;
	double base_flux,
	       diffusivity;				//	normalized diffusivity in aggregate soil media, units 0-1:
	//	ratio of gas diffusivity through soil to gas diffusivity through air
	//	at optimum water content.
	double  A[4];						// parameters to parton-innis functions
	//  fraction  N2O to NO of new NO3 that is converted
	//double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
	double NO_N2O_ratio = 0.0;  		// NO/N2O ratio <= 1.0
	double fluxN2Onit = 0.0;  			// nitrification N2O flux (kgN/ha/day)
	double fluxNOnit = 0.0;				// flux nitrified NO (kgN/ha/day)
	double const fractionN03toN2O = 0.02;  // frac of new NO3 that goes to N2O
	double potentialFluxNOnit;			// maximum possible nitrification NO flux (gN/m^2/day)
	double NH4_to_NO = 0.0; 			// ammonium converted to NO to reach
	//    potential NO flux (kgN/ha/day)
	double NNM;
	double NH4NbeforeMiner;


	if (NewDay(pTi)) //Trace gas modell is called daily
		{


//***********************************************************************************
// 0) Initialize
//***********************************************************************************
			pCh->pCProfile->fN2OEmisR = 0.0;
			pCh->pCProfile->fNOEmisR = 0.0;
			pCh->pCProfile->fN2EmisR = 0.0;

//***********************************************************************************
//  multiplier NOx flux based on recent rain events and snowpack
// increase of NO due to moisture and rain >= 1.0
//***********************************************************************************
//shift the daily values and add current day's ppt [cm]
			for(j=0; j<13; j++) //shift the daily values and add current day's ppt [cm]
				{
					self->ppt2lastweeks[j]=self->ppt2lastweeks[j+1];
				}
//ppt2lastweeks[13] = pCl->pWeather->fRainAmount*0.1;
//ck:
			self->ppt2lastweeks[13] = self->fPreciRate*0.1;

// Da das Regenarray erst nach 2 Wochen gefüllt ist, darf darf das Modell erst nach 2 Wochen Starten:
			if (self->nitri_daycount<14)
				{
					self->nitri_daycount++;
				}
			else
				{

//sum of rain during last 2 weeks:
					sumppt=0.0;
					for(j=0; j<14; j++) //shift the daily values and add current day's ppt [cm]
						{
							sumppt += self->ppt2lastweeks[j];
						}
//get pulse factor for today
					if((int)(pTi->pSimTime->fTimeY) > 14)
						{
							self->kPrecipNO = self->NOxPulse[0];

							for(j=0; j<11; j++) //shift the daily values and add factor 1.0 for the last day
								{
									self->NOxPulse[j]=self->NOxPulse[j+1];
								}
							self->NOxPulse[11]=1.0;

//calculate future pulses
//new pulse, if sum is lower than 1cm and no snow
							if (sumppt <= 1.0 && (pCl->pWeather->fSnow<=0.0) )
								{
									/*	if(pCl->pWeather->fRainAmount*0.1 <= 0.5//low rain event
											&& pCl->pWeather->fRainAmount*0.1 > 0.1)*/
//ck;
									if(self->fPreciRate*0.1 <= 0.5//low rain event
									        && self->fPreciRate*0.1 > 0.1)
										{
											for(j=0; j<1; j++) //low rain event pulses last 1 day
												{
													self->NOxPulse[j] = MAX(self->NOxPulse[j], 11.19 *  (double)exp(-0.805 * (j+2)) );
												}
										}
//ck:
									/*	if(pCl->pWeather->fRainAmount*0.1 > 0.5//medium rain event
										&& pCl->pWeather->fRainAmount*0.1 <= 1.5)*/
									if(self->fPreciRate*0.1 > 0.5//medium rain event
									        && self->fPreciRate*0.1 <= 1.5)
										{
											for(j=0; j<5; j++) //medium rain event pulses last 5 days
												{
													self->NOxPulse[j] = MAX(self->NOxPulse[j], 14.68 * (double)exp(-0.384 * (j+2)) );
												}
										}
//	if(pCl->pWeather->fRainAmount*0.1 > 1.5)//high rain event
//ck:
									if(self->fPreciRate*0.1 > 1.5)//high rain event
										{
											for(j=0; j<12; j++) //large rain event pulses last 12 days
												{
													self->NOxPulse[j] = MAX(self->NOxPulse[j], 18.46 * (double)exp(-0.208 * (j+1)) );
												}
										}
								}
						}//simulated days >14
					else//at the first 14 days
						{
							self->kPrecipNO = 1.0;
						}
					self->fPreciRate=0.0;

//***********************************************************************************
// I) total NH4 loss
//***********************************************************************************

//Loop over all soil layer
					for(i=0,pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pHL=pHe->pHLayer->pNext,pSW=pSo->pSWater->pNext;
					        pSL->pNext!=NULL; pCL=pCL->pNext,pSL=pSL->pNext,pWL=pWL->pNext,pHL=pHL->pNext,pSW=pSW->pNext,i++)
						{
							NH4_to_NO3 = 0.0;
							pCL->fNH4NitrR = 0.0;
							// Determine texture index (COARSE,MEDIUM, FINE, VERY FINE);
							if (pSL->fSand > 70.0 ) textureindex=COARSE;
							if (pSL->fSand <= 70.0 && pSL->fSand >= 30.0  ) textureindex=MEDIUM;
							if (pSL->fSand < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"

							if (pCL->fNH4N >= min_ammonium)
								{
									//  Compute the effect of WFPS on Nitrification (0-1)
									WFPS = pWL->fContAct / pSL->fPorosity;
									switch (textureindex)
										{
										case COARSE:
											a = 0.5;
											b = 0.0;
											c = 1.5;
											d = 4.5;
											break;
										case FINE:
										case VERYFINE:
											a = 0.65;
											b = 0.0;
											c = 1.2;
											d = 2.5;
											break;
										case MEDIUM:
										default:
											a = 0.65;
											b = 0.0;
											c = 1.2;
											d = 2.5;
											break;
										}
									//
									base1 =((WFPS - b) / (a - b));
									base2 =((WFPS - c) / (a - c));
									e1 = d * ((b - a) / (a - c));
									e2 = d;
									fNwfps = pow (base1, e1) * pow (base2, e2);
									if(fNwfps < 0.0 || fNwfps>1.001)
										{
											PRINT_ERROR_ID((&(self->parent)),"Nitrification error: fNwfps out of range");
										}

									//  Soil temperature effect on Nitrification (0-1)
									//A[0] = (&(self->parent))->pCl->pAverage->fMonthTempAmp;
									//A[0] = self->par.maxt;	// Long term avg max monthly air temp. of hottest month
									A[0] = self->monthAvTempAmpl; // Long term avg max monthly air temp. of hottest month
									A[1] = -5.0;
									A[2] = 4.5;
									A[3] = 7.0;
									fNsoilt = century_n_f_gen_poisson_density (self,pHL->fSoilTemp, A);
									if(fNsoilt < 0.0 || fNsoilt > 1.001)
										{
											PRINT_ERROR_ID((&(self->parent)),"Nitrification error: fNsoilt out of range");
										}
									//  Compute pH effect on nitrification
									A[0] = 5.0;
									A[1] = 0.56;
									A[2] = 1.0;
									A[3] = 0.45;
									fNph = century_n_f_arctangent (self,pSL->fpH,A); //pH Wert
									if(fNph < 0.0 || fNph>1.001)
										{
											PRINT_ERROR_ID((&(self->parent)),"Nitrification error: ffNph out of range");
										}
									// Ammonium that goes to nitrate during nitrification.
									// base flux in Daycent program code:
									//base_flux = 0.1 * Ha_to_SqM;	// 0.1 gN/ha/day
									//base flux from Parton et al. (2001): 20% of NNM-Ammonium
									NH4NbeforeMiner = pCL->fNH4NSoilConc / 100.0 * (pSL->fThickness*( pWL->fContAct + pSL->fBulkDens*pCh->pCParam->afKd[1]));
									NNM = pCL->fNH4N - NH4NbeforeMiner; // fNAnorg: NH4 content before decomposition
									base_flux=  MAX(0.0, NNM * 0.2);
									NH4_to_NO3 = (NH4NbeforeMiner) * MaxNitrifRate * fNph * fNwfps * fNsoilt + base_flux;	//[kg/ha]
									NH4_to_NO3 = MAX( pCL->fNH4N, NH4_to_NO3 );
									NH4_to_NO3 = MAX( 0.0, NH4_to_NO3 );

									if(pCL->fNH4N < NH4_to_NO3)
										{
											PRINT_ERROR_ID((&(self->parent)),"Nitrification error: more nitrified than available");
										}
									ret +=  NH4_to_NO3;	//[kg/ha]
								}//if (ammonium >= min_ammonium)

//***********************************************************************************
							newNO3 = NH4_to_NO3; //[kg/ha]
							if (newNO3 > 1.0E-10)
								{

//***********************************************************************************
// II) Partition to Nitrate, N2, NO
//***********************************************************************************
									//Diffusivity
									//	Returns the normalized diffusivity in aggregate soil media, units 0-1:
									//	ratio of gas diffusivity through soil to gas diffusivity through air
									//	at optimum water content.

									diffusivity=century_n_Diffusivity(self,pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,WFPS);
									//  [1]        , [kg/dm3=g/cm3]´, [1],        ,[1]
									// ratio NO:N2O
									NO_N2O_ratio = 15.23 + ( 35.45 *
									                         atan ( 0.676 * PI * (10.0 * diffusivity - 1.86) ) ) / PI;
									if (self->iForest == 0)// true if agricultural system or grassland
										{
											NO_N2O_ratio *= 0.20;
										}
									// N2O flux
									fluxN2Onit = newNO3 * fractionN03toN2O;
									newNO3 -= fluxN2Onit;
									// maximum possible nitrification NO flux (kg/ha/day)
									potentialFluxNOnit = fluxN2Onit * NO_N2O_ratio * (self->kPrecipNO*pTi->pTimeStep->fAct);

									if (potentialFluxNOnit <= newNO3) //NO is taken from newNO3
										{
											fluxNOnit = potentialFluxNOnit;
											newNO3 -= fluxNOnit;
										}
									else
										{
											// take N out of ammonimum to get max fluxNOtotal possible
											NH4_to_NO =  potentialFluxNOnit - newNO3 ;
											NH4_to_NO = MIN(NH4_to_NO,pCL->fNH4N); //restrain  flow
											//pCL->fNH4N -= (double)NH4_to_NO; //perform flow below
											ret += NH4_to_NO;
											fluxNOnit = newNO3 + NH4_to_NO;
											newNO3 = 0.0;
										}
//*********************************************************************
// Write fluxes to XN variables
//		fluxNOnit and fluxN2Onit  -  [kgN/ha/day]
//*********************************************************************
									// 1. Subtract Ammonium:
									pCL->fNH4N -= NH4_to_NO3;
									pCL->fNH4N -= NH4_to_NO;
									// 2. Add NO3
									pCL->fNO3N += newNO3;
									pCL->fNH4NitrR = newNO3 ;//[kg/ha/day]
									//pCL->fNH4NitrR /= pTi->pTimeStep->fAct;
									//* (double)1e5 / (double)24.0 ;//[ug/m2*h] ??
									// 3. Gaseous Emissions
									//this does work, but use is without setting fN2OEmisR in Profile
									pCL->fN2ON  += fluxN2Onit;  //[kg/ha]
									pCL->fNON  += fluxNOnit;  //[kg/ha]

									//XN: NxOx Emission has to be balanced somehow for the graphical output.
									//XN: just do it the same way as for N2O
									//till it is fixed, write the NOx emission to N2O emission
//		pCh->pCProfile->fNOEmisR  += (double)(fluxNOnit) * 1e5 / 24.0 /(double)pTi->pTimeStep->fAct;  //[ug/m2*h] per timestep
									C_DEBUG_LOC_IN_ARRAY(NH4_to_NO3,i);

									pCh->pCProfile->fN2OEmisR += (double)(fluxN2Onit + fluxNOnit )* (double)1e5 / (double)24.0 /(double)pTi->pTimeStep->fAct;    //[ug/m2*h] per timestep
									//pCh->pCProfile->fN2OEmisR += (double)(fluxN2Onit + fluxNOnit )* (double)1e5 / (double)24.0 /(double)pTi->pTimeStep->fAct;    //[ug/m2*h] per timestep


								}//if (newNO3 > 1.0E-10)

							if (pCL->fNO3N < 0 || pCL->fNH4N < 0)
								{
									PRINT_ERROR_ID((&(self->parent)),"Error Nitrify: neg. Mineral N");
								}
						}//loop over layer

				}
		}//daily

	self->fPreciRate += pCl->pWeather->fPreciRate*pTi->pTimeStep->fAct;

	return RET_SUCCESS;
}
//************ end of function Nitrify****************

double century_n_Diffusivity (century_n *self,
                              double const A,		// fraction of soil bed volume occupied by
                              //   field capacity
                              //   (intra-aggregate pore space, 0-1)
                              double const bulkDen,	// bulk density of soil (g/cm^3)
                              double const porosity,	// porosity fraction (0-1)
                              double const wfps)	// Water Filled Pore Space fraction
// (volumetric swc/porosity, 0-1)
{
	double  pfc,vfrac,theta_V,theta_P,theta_A,s_wat,sw_p; //explained below
	double tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8;  // intermediate variables
	// normalized diffusivity in aggregate soil media (0-1)
	double dDO = 0.0; //return value
	// volumetric air content fraction
	double const vac = MIN (1.0, MAX (0.0, porosity - A) );
	if ( vac > 0.0 )	// undersaturated?
		{
			// soil water content as % of field capacity (%)
			// (this value can be > 100%)
			pfc = wfps * 100.0  / (A / porosity);

			// volumetric fraction
			vfrac = ( pfc >= 100.0 ? (wfps * porosity - A) / vac : 0.0 );

			// volumetric water content of the soil bed volume
			// double my_theta_V = wfps * (1.0 - bulkden/PARTDENS);
			// double my_theta_V = wfps * porosity;
			theta_V = ( pfc < 100.0 ? (pfc / 100.0) * A : A + MIN (vfrac, 1.0) * vac );

			// volume H2O per unit bed volume in inter-aggregate pore space
			theta_P = (theta_V < A) ? 0.0 : theta_V - A;

			// volume H2O per unit bed volume in intra-aggregate pore space
			theta_A = (theta_V > A) ? A : theta_V;

			// fractional liquid saturation of the A component of total pore volume
			s_wat = MIN ( 1.0, theta_V / A );

			// fractional liquid saturation of the P component of total pore volume
			sw_p =  MIN ( 1.0, theta_P / vac );

			if (1.0 - s_wat > 0.0)
				tp1 = (double)pow( (1.0 - s_wat), 2.0 );
			else
				tp1 = 0.0;
			tp2 = (A - theta_A) / (A + (1.0 - porosity));
			if ( tp2 > 0.0 )
				tp3 = (double)pow( tp2, (0.5 * tp2 + 1.16) );
			else
				tp3 = 0.0;
			tp4 = 1.0 - (double)pow( vac, (0.5 * vac + 1.16) );
			tp5 = vac - theta_P;
			if (tp5 > 0.0)
				tp6 = (double)pow(tp5, (0.5 * tp5 + 1.16));
			else
				tp6 = 0.0;
			tp7 = (double)pow((1.0 - sw_p), 2.0);
			tp8 = MAX( 0.0,
			           ( (tp1 * tp3 * tp4 * (tp5 - tp6)) /
			             (1.0E-6 + (tp1 * tp3 * tp4) + tp5 - tp6) * 1.0E7) );

			// normalized diffusivity in aggregate soil media (0-1)
			dDO = MAX(0.0, (tp8 / 1.0E7 + tp7 * tp6) );
		}

	if(dDO < 0 || dDO > 1.0 ) PRINT_ERROR_ID((&(self->parent)),"Warning - diffusivity out of range");

	return dDO;
}




double century_n_f_arctangent(century_n *self,double x, double A[])
{
	return(A[1] + (A[2] / (double)PI) *
	       (double)atan(PI * (double)A[3] * (double)(x - A[0])));
}

double century_n_f_gen_poisson_density(century_n *self,double x, double A[])
{
	double tmp1, tmp2, tmp3;

	if (A[1] == A[0])
		{
			return(0.0);
		}

	tmp1 = (double)((A[1] - x) / (A[1] - A[0]));
	if (tmp1 <= 0.0)
		{
			return 0.0;
		}

	tmp2 = 1.0 - pow(tmp1, (double)A[3]);
	tmp3 = pow(tmp1, (double)A[2]);
	return (double)(exp((double)A[2] * tmp2 / (double)A[3]) * tmp3);

}
