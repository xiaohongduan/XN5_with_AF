//      century_n.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "century_n.h"


#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int century_n_Mineralisation_init(century_n *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	PCLAYER		pCL;
	PSLAYER		pSL;
	PWLAYER		pWL;
	int i;

	century_n__general_init(self);
	
	self->fCHumus_old = (double *)g_malloc0((pSo->iLayers-2) * sizeof (double));//Hong

	//set fixed parameters (values from Century file 'ffixed.100' for forests)
	self->fixed = century_n_getFixedParameters();
	//read site parameter from 'century.cfg' file

	//********************************************************************************************
	// Initialize pools
	//********************************************************************************************
	// A) Wood
	//wood 1: fine branch //aus *.cfg [kg/ha] gpm2TOkgpha
	//pCh->pCProfile->fCBranchLitterSurf = self->par.wood1C;
	//pCh->pCProfile->fNBranchLitterSurf = self->par.wood1C / self->par.cerfor[2][FBRCH];//calciv_dc line 216
	//wood 2: large wood  //aus *.cfg [kg/ha]
	//pCh->pCProfile->fCStemLitterSurf = self->par.wood2C;
	//pCh->pCProfile->fNStemLitterSurf = self->par.wood2C / self->par.cerfor[2][LWOOD];//calciv_dc line 216
	//wood 3: coarse roots //aus xnm file Marke 20003
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
	        pSL->pNext!=NULL; pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCGrossRootLitter = pCL->fCManure;
			pCL->fNGrossRootLitter = pCL->fNManure;
		}
	// B) Litter
	//Belowground = fine root litter //aus xnm file Marke 20003
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
	        pSL->pNext!=NULL; pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCFineRootLitter = pCL->fCLitter;

			pCL->fNFineRootLitter = pCL->fNLitter /(double)3.0; //Adjustment from workbook 2.b.

		}
	//Aboveground = leaves litter	//aus *.cfg
	pCh->pCProfile->fCLeafLitterSurf = pCh->pCProfile->fCLitterSurf;
	pCh->pCProfile->fNLeafLitterSurf = pCh->pCProfile->fNLitterSurf;

	//SOM soil //aus xnm file Marke 20003
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
	        pSL->pNext!=NULL; pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			self->fCHumus_old[i] = pSL->fCHumus;//Hong on 20190605
			
			pCL->fCHumusFast   = pSL->fCHumus * 0.03;	//percentages from century workbook Ch. II
			pCL->fNHumusFast   = pSL->fNHumus * 0.03 / 0.7;
			pCL->fCHumusSlow   = pSL->fCHumus * 0.65;
			pCL->fNHumusSlow   = pSL->fNHumus * 0.65 / 1.1615;//neu berechnet, so das alles N aufgeteilt ist
			pCL->fCHumusStable = pSL->fCHumus * 0.32;
			pCL->fNHumusStable = pSL->fNHumus * 0.32 / 0.7;

		}
	//SOM1 srfc //aus *.cfg
	pCh->pCProfile->fCMicLitterSurf = pCh->pCProfile->fCHumusSurf;
	pCh->pCProfile->fNMicLitterSurf = pCh->pCProfile->fNHumusSurf;

	self->INIT_DONE=1;
	
	return RET_SUCCESS;
}

int century_n_Mineralisation_done(century_n *self)
{
	if (self->config_filename!=NULL)
		free(self->config_filename);
	self->config_filename=NULL;
	return RET_SUCCESS;
}

int century_n_Mineralisation_run(century_n *self) // returns total daily NNM [g/m2] = mineralisation - immobilisation
{



	//C_DEBUG(*(self->MeanTemp))
	//printf("Temp: %f %f\n",self->parent.pHe->pHLayer->pNext->fSoilTemp,self->parent.pTi->pSimTime->fTimeAct);
	//CENTminer(self); //actual call of Daycent's Decomposition submodel
	
	// used in loops over all layer
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PHEAT pHe = xpn->pHe ;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PCLAYER		pCL;
	PSLAYER		pSL;
	PWLAYER		pWL;
	PHLAYER		pHL;
	//int const TimeStepsPerDay=(int)(1.0/(pTi->pTimeStep->fAct));
	//double dtDecompDC=(double)1.0/365./TimeStepsPerDay;	//fraction of year per timestep
	double dtDecompDC=(double)1.0/365.0*(pTi->pTimeStep->fAct);	//fraction of year per timestep
	
	double C=0.0, N=0.0, lig=0.0, Navail=0.0, oldlig=0.0, oldsC, sC=0.0, sN=0.0, newlig=0.0, mC=0.0, mN=0.0, ret; //partition litter into struc/metab
	double tcflow; //total c flow
	double defacSRFC, anerb;
	int i;
	//iday;
	double *pdecf;
	double rneww1[2],								//ratio C/N fine branches to: [0]SOM1 [1]SOM2
	       rneww2[2],								//ratio C/N large wood    to: [0]SOM1 [1]SOM2
	       rneww3[2],								//ratio C/N coarse roots  to: [0]SOM1 [1]SOM2
	       rnewas[2],								//ratio C/N above ground structural  to: [0]SOM1 [1]SOM2
	       rnewbs[2];								//ratio C/N below ground structural  to: [0]SOM1 [1]SOM2
	double radds1;
	double CtoSOM2, CtoSOM1, co2loss;
	double orgNflow=0.0, minNflow=0.0;
	double mineralFlow;// Mineralization associated with respiration
	double cfmes1,rcemet,cfsfs2,rceto2,rceto3,rceto1,cfs1s3,cfs1s2,cfs2s1,cfs2s3,cfs3s1;
	double microbeC; //C flow out of surface microbe pool
	double efftext;
	double frNH4, frNO3;
	double dummy;
	double MinByWood,MinByStructLitter,MinByMetabLitter,MinBySOM; //Mineralisation by pools [kg/ha]
	double ImmByWood,ImmByStructLitter,ImmByMetabLitter,ImmByInput,ImmBySOM; //Immobilisation by pools [kg/ha]
	

//	FILE *outf;

frNH4=0.0;	
frNO3=0.0;
	//if (NewDay(pTi))
		{
//********************************************************************************************
// 0) Initialize
//********************************************************************************************
			for(pCL=pCh->pCLayer; pCL->pNext!=NULL; pCL=pCL->pNext)
				{
					pCL->fMinerR = 0.0;
					pCL->fNImmobR = 0.0;
					pCL->fCO2ProdR = 0.0;
					pCL->fCO2C_dt=0.0; //Hong added on 20190604
					pCL->fHumusMinerR = 0.0;
					pCL->fLitterMinerR = 0.0;
					pCL->fNLitterImmobR = 0.0;
				}

			//pCh->pCProfile->fNHumusImmobDay = 0.0;
			MinByWood=MinByStructLitter=MinByMetabLitter=MinBySOM=0.0;
			ImmByWood=ImmByStructLitter=ImmByMetabLitter=ImmByInput=ImmBySOM=0.0;


//********************************************************************************************
// I) Calculate temperature and water factor for decomposition, do this daily
//********************************************************************************************
//	a) Surface pools: defacSRFC, anerb
			century_n_decf20cm(self,&defacSRFC, &anerb, self->fixed, self->fdrain);
//	b) Soil layers:   pdecf[i]
			pdecf =(double *)malloc((pSo->iLayers-2) * sizeof (double));
			for(i=0,pSL=pSo->pSLayer->pNext,pHL=pHe->pHLayer->pNext, pWL=pWa->pWLayer->pNext;
			        pSL->pNext!=NULL; pSL=pSL->pNext, pHL=pHL->pNext, pWL=pWL->pNext, i++)
				{
					pdecf[i]=century_n_decfSL(self,pSL->fSand,(pWL->fContAct/pSL->fPorosity),pHL->fSoilTemp,self->fixed);

				}

//********************************************************************************************


//********************************************************************************************
// II) Input litter (leaves to surface, fine roots to soil)
//********************************************************************************************
// a) surface litter
			C=pCh->pCProfile->fCLeafLitterSurf * kgphaTOgpm2;
			N=pCh->pCProfile->fNLeafLitterSurf * kgphaTOgpm2;
			//lig=self->par.wdlig[LEAF];
			lig=pCh->pCProfile->fLigFracLeaf;
			Navail=0.0; //no direct adsorbtion in surface litter
			oldlig = pCh->pCProfile->fLignFracStrLitSurf;
			oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
			if(C>0.0 && N>0.0)
				{
					ret = century_n_PartitionResidue(self,C,N/C,lig,SRFC,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,self->fixed);
					pCh->pCProfile->fCLeafLitterSurf = 0.0; //all litter is now in struc/metab pools
					pCh->pCProfile->fNLeafLitterSurf = 0.0;
					pCh->pCProfile->fCStrLitterSurf += sC * gpm2TOkgpha;
					pCh->pCProfile->fNStrLitterSurf += sN * gpm2TOkgpha;
					pCh->pCProfile->fLignFracStrLitSurf = newlig;
					pCh->pCProfile->fCMtbLitterSurf += mC * gpm2TOkgpha;
					pCh->pCProfile->fNMtbLitterSurf += mN * gpm2TOkgpha;
				}

// b) soil litter (fine roots)
			for(i=0,pCL=pCh->pCLayer->pNext; pCL->pNext!=NULL; pCL=pCL->pNext,i++)
				{
					C=pCL->fCFineRootLitter * kgphaTOgpm2;
					N=pCL->fNFineRootLitter * kgphaTOgpm2;
					lig=pCh->pCProfile->fLigFracFineRoot;
					//lig=self->par.wdlig[LEAF];
					Navail=(pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //direct adsorbtion from first 20 cm
					oldlig = pCh->pCProfile->fLignFracStrLitSurf;
					oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
					if(C>0.0 && N>0.0)
						{
							ret = century_n_PartitionResidue(self,C,N/C,lig,SOIL,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,self->fixed);
							//direct adsorbtion of mineral N
							if(ret>0.0)
								{
									ret = MIN (ret, (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
									frNH4 = pCL->fNH4N / (pCL->fNO3N + pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
									frNO3 = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N);
									pCL->fNH4N -= frNH4
									              * ret * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
									pCL->fNO3N -= frNO3
									              * ret * gpm2TOkgpha;
									pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
									pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
									pCL->fNImmobR += ret * gpm2TOkgpha;

									ImmByInput += ret * gpm2TOkgpha;
									if(pCL->fNO3N <0.0 ) PRINT_ERROR_ID((&(self->parent)),"Warning: NO3 @ input negative!");
									if(pCL->fNH4N <0.0 ) PRINT_ERROR_ID((&(self->parent)),"Warning: NH4N @ input negative!");

								}
							//all litter is now in struc/metab pools
							pCL->fCFineRootLitter = 0.0;
							pCL->fNFineRootLitter = 0.0;
							pCL->fCStrcLitter += sC * gpm2TOkgpha;
							pCL->fNStrcLitter += sN * gpm2TOkgpha;
							pCL->fLignFracStrcLit  = newlig;
							pCL->fCMtbLitter += mC * gpm2TOkgpha;
							pCL->fNMtbLitter += mN * gpm2TOkgpha;
						}

				}//loop over soil layers

//************************************************************************************************
// IV)  Calculates the decomposition ratio.
// 	Determine C/N of new material entering 'Box B'.
// 	The C/N ratios for structural and wood are computed once per day here;
//************************************************************************************************

			
			
			if(self->iForest == 1) //forest
				{
// Fine branch
// Ratio for new SOM1 from decomposition of Fine Branches
					C=pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
					N=pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
					rneww1[0]= century_n_AboveGroundDecompRatio(self,C,N,2.0,self->fixed);
// Ratio for new SOM2 from decomposition of Fine Branches
					C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
					N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
					radds1=self->fixed.rad1p[0] + self->fixed.rad1p[1] *
					       ( century_n_AboveGroundDecompRatio (self,C,N,2.5,self->fixed)- self->fixed.pcemic[1]);
					rneww1[1]= rneww1[0]+ radds1;
// Large Wood
// Ratio for new SOM1 from decomposition of Large Wood
					C=pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
					N=pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
					rneww2[0] =century_n_AboveGroundDecompRatio(self,C,N,2.0,self->fixed);
// Ratio for new SOM2 from decomposition of Large Wood
					C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
					N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
					rneww2[1] = rneww2[0] + self->fixed.rad1p[0] + self->fixed.rad1p[1] *
					            ( century_n_AboveGroundDecompRatio (self,C,N,2.0,self->fixed)- self->fixed.pcemic[1]);
					rneww2[1] =  MAX (rneww2[1],self->fixed.rad1p[2]);
// Coarse Roots
// Ratio for new SOM1 from decomposition of Coarse Roots
					rneww3[0] = self->fixed.varat1[0];
// Ratio for new SOM2 from decomposition of Coarse Roots
					rneww3[1] = self->fixed.varat2[1];
				}//forest
				
				
				
// Surface Structural
// Ratios for new som1 from decomposition of AG structural
			C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
			N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
			rnewas[0] = century_n_AboveGroundDecompRatio(self,C,N,2.5,self->fixed);
// Ratio for new SOM2 from decomposition of AG strutural
			radds1=self->fixed.rad1p[0] + self->fixed.rad1p[1] *
			       ( century_n_AboveGroundDecompRatio (self,C,N,2.5,self->fixed)- self->fixed.pcemic[1]);
			rnewas[1] =MAX( self->fixed.rad1p[2], rnewas[0] + radds1);
// Soil Structural
// Ratios for new som1 from decomposition of BG structural
			rnewbs[0] = self->fixed.varat1[0];
// Ratio for new SOM2 from decomposition of BG strutural
			rnewbs[1] = self->fixed.varat2[0];


//   ***************************************************************************************
//		Decomposition Routines (CALL: simsom_dc.cpp line 280), do 4 times a day
//		 IV) DecomposeLitter
//		  V) DecomposeWood
//		 VI) DecomposeSOM
//				Steps:
//				1. Surface SOM1 decomposes to SOM2 with CO2 loss.
//				2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss and leaching.
//				3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss
//				4. SOM3 decomposes to soil SOM1 with CO2 loss.
//   ***************************************************************************************

			//for(iday=1; iday<=TimeStepsPerDay; iday++) //4 times a day
				{
//************************************************************************************************
// IV) Decompose Litter
//************************************************************************************************

//SURFACE STRUCTURAL Material to SOM1[SRFC] and SOM2
					C		= pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
					N		= pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
					lig		= pCh->pCProfile->fLignFracStrLitSurf;
					Navail  = century_n_getNtoDepth(self,20.0);  //top 20cm
					if (C >= 10e-7)
						{
							tcflow=MIN(C , self->fixed.strmax[SRFC]* defacSRFC * self->fixed.dec1[SRFC] //C flow from A to B
							           * (double)exp(-self->fixed.pligst[SRFC]*lig) ) * dtDecompDC;
							// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
							// See if Box A can decompose to SOM1.
							// If it can go to som1, it will also go to som2.
							// If it can't go to som1, it can't decompose at all.
							if (century_n_CanDecompose (self,C,N,rnewas[0],Navail))
								{
									// Decompose Box A to SOM2
									// -----------------------
									// C Flow
									CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
									co2loss = CtoSOM2 * self->fixed.rsplig;
									CtoSOM2 -= co2loss;//net C flow to SOM2

									pCh->pCProfile->fCStrLitterSurf -= tcflow * gpm2TOkgpha;
									pCh->pCProfile->fCLeafLitterSurf -= tcflow * gpm2TOkgpha; //Hong added for C-Balance on 20190605, fCLeafLitterSurf was distributed to fCStrLitterSurf and fCMtbLitterSurf
									pCh->pCProfile->fCLitterSurf -= tcflow * gpm2TOkgpha; //Hong added for C-Balance on 20190605, fCLeafLitterSurf is fCLitterSurf
									century_n_CO2intoSOIL(self,co2loss,20.0);//CO2 distributed in soil till depth of 20 cm
									//pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;//Hong: Fehler? CO2 distributed in soil till depth of 20 cm
									//pCh->pCProfile->dCO2SurfEmisCum += co2loss * gpm2TOkgpha; //added by Hong for C balance on 20190605
									
									century_n_FlowCintoSOM2(self,CtoSOM2,20.0);

									// Mineralization associated with respiration
									mineralFlow=co2loss * N/C;
									mineralFlow=century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
									pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha;
									MinByStructLitter += mineralFlow * gpm2TOkgpha; //Hong: sum of fMinerR in 0-20 profile

									// Compute and schedule N flow from A to B.
									century_n_ScheduleNFlow(self,CtoSOM2,rnewas[1],C,N,Navail,&orgNflow, &minNflow);
									pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
									century_n_FlowNintoSOM2(self,orgNflow,  20.0);			 //B += orgflow
									if( minNflow >= 0.0)//Mineralisation
										{
											pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
											century_n_FlowNintoMineralSoil(self,minNflow, 20.0,0.0);	//Mineral += minflow
											MinByStructLitter += minNflow * gpm2TOkgpha; 
										}
									if ( minNflow < 0.0)//Immobilisation
										{
											//Flow can be limited by available N, this is done in FlowNfromMineralSoil
											//the return value gives the restricted flow of mineral N
											ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);	//Mineral -= minflow
											century_n_FlowNintoSOM2(self, (double)ret, 20.0);			//B += minflow
											ImmByStructLitter +=  ret * gpm2TOkgpha; 
										}

									// Decompose Box A to SOM1
									// -----------------------
									// C Flow
									CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
									if(CtoSOM1 > 10e-7)
										{
											co2loss = CtoSOM1 * self->fixed.p1co2a[SRFC];
											CtoSOM1 -= co2loss;							//net  C flow to SOM1

											//pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;//Hong: Fehler? write to layer 0
									        //pCh->pCProfile->dCO2SurfEmisCum += co2loss * gpm2TOkgpha; //added by Hong for C balance on 20190605 CO2 distributed to soil
											
											century_n_CO2intoSOIL(self,co2loss,20.0);//CO2 distributed to soil
											pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
											pCh->pCProfile->fCHumusSurf += CtoSOM1 * gpm2TOkgpha; //Hong added for C-Balance on 20190605

											// Mineralization associated with respiration
											mineralFlow=co2loss * N/C;
											mineralFlow=century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
											pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha;
											MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
											// Compute and schedule N flow from A to B.
											century_n_ScheduleNFlow(self,CtoSOM1,rnewas[0],C,N,Navail,&orgNflow, &minNflow);
											pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
											pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
													century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
													MinByStructLitter += minNflow * gpm2TOkgpha; ;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													//Flow can be limited by available N, this is done in FlowNfromMineralSoil
													//the return value gives the restricted flow of mineral N
													ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);//Mineral -= minflow
													pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
													ImmByStructLitter +=  ret * gpm2TOkgpha; ;
												}
										}//if(CtoSOM1 > 10e-7)
								}//if Candecompose
						}//C >= 10e-7

//SOIL STRUCTURAL Material to SOM1[SOIL] and SOM2
					for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
					        pSL->pNext!=NULL; i++,pSL=pSL->pNext,pCL=pCL->pNext)
						{
							C		= pCL->fCStrcLitter * kgphaTOgpm2;
							N		= pCL->fNStrcLitter * kgphaTOgpm2;
							lig		= pCL->fLignFracStrcLit;
							Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
							if (C >= 10e-7)
								{
									tcflow=MIN(C , self->fixed.strmax[SOIL]* pdecf[i] *  anerb  * self->fixed.dec1[SOIL] //C flow from A to B
									           * (double)exp(-self->fixed.pligst[SOIL]*lig) ) * dtDecompDC;
									// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
									// See if Box A can decompose to SOM1.
									// If it can go to som1, it will also go to som2.
									// If it can't go to som1, it can't decompose at all.
									if (century_n_CanDecompose (self,C,N,rnewbs[0],Navail))
										{
											// Decompose Box A to SOM2
											// -----------------------
											// C Flow
											CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
											co2loss = CtoSOM2 * self->fixed.rsplig;
											CtoSOM2 -= co2loss;//net C flow to SOM2

											pCL->fCStrcLitter -= tcflow * gpm2TOkgpha;
											pCL->fCLitter -= tcflow * gpm2TOkgpha;// Hong added for C balance on 20190605 
											pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
											pCL->fCO2C_dt		+= co2loss * gpm2TOkgpha;
											pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
											pSL->fCHumus  += CtoSOM2 * gpm2TOkgpha; //Hong for C balance

											// Mineralization associated with respiration
											mineralFlow = co2loss * N/C;
											//mineralFlow = min (mineralFlow,Navail );
											pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
											pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha;
											pCL->fMinerR += mineralFlow * gpm2TOkgpha;
											MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
											// Compute and schedule N flow from A to B.
											century_n_ScheduleNFlow(self,CtoSOM2,rnewbs[1],C,N,Navail,&orgNflow, &minNflow);
											pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											//Hong: falsch pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
											pCL->fNHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
													pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
													pCL->fMinerR += minNflow * gpm2TOkgpha;
													MinByStructLitter += minNflow * gpm2TOkgpha; ;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
													if (minNflow > 1e-7)
														{
															//Hong: falsch? pCL->fCHumusSlow  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															pCL->fNHumusSlow  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
															frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
															pCL->fNH4N -= frNH4 *  (double)fabs(minNflow) * gpm2TOkgpha; //Mineral -= minflow
															pCL->fNO3N -= frNO3 *  (double)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate
															pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
															pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
															pCL->fNImmobR  +=  (double)fabs(minNflow) * gpm2TOkgpha;

															ImmByStructLitter +=  (double)fabs(minNflow) * gpm2TOkgpha;
														}
												}

											// Decompose Box A to SOM1
											// -----------------------
											// C Flow
											CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
											if(CtoSOM1 > 10e-7)
												{
													co2loss = CtoSOM1 * self->fixed.p1co2a[SOIL];
													CtoSOM1 -= co2loss;							//net  C flow to SOM1

													pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
													pCL->fCO2C_dt		+= co2loss * gpm2TOkgpha;
													pCL->fCHumusFast += CtoSOM1 * gpm2TOkgpha;
													pSL->fCHumus += CtoSOM1 * gpm2TOkgpha; //Hong for C balance

													// Mineralization associated with respiration
													mineralFlow=co2loss * N/C;
													//mineralFlow = min (mineralFlow,Navail );
													pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
													pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha;
													MinByStructLitter += mineralFlow * gpm2TOkgpha;
													pCL->fMinerR += mineralFlow * gpm2TOkgpha;
													// Compute and schedule N flow from A to B.
													century_n_ScheduleNFlow(self,CtoSOM1,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
													pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
													//Hong: falsch pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
													pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
													if( minNflow >= 0.0)//Mineralisation
														{
															pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
															pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
															MinByStructLitter += minNflow * gpm2TOkgpha;
															pCL->fMinerR += minNflow * gpm2TOkgpha;
														}
													if ( minNflow < 0.0)//Immobilisation
														{
															minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
															if(minNflow > 1e-7)
																{
																	//Hong: falsch? pCL->fCHumusFast  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
																	pCL->fNHumusFast  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
																	frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
																	frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
																	pCL->fNH4N -= frNH4 *  (double)fabs(minNflow) * gpm2TOkgpha;//Mineral -= minflow
																	pCL->fNO3N -= frNO3 *  (double)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate
																	pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
																	pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
																	ImmByStructLitter +=  (double)fabs(minNflow) * gpm2TOkgpha;
																	pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

																}
														}
												}//if(CtoSOM1 > 10e-7)
										}//if Candecompose
								}//C >= 10e-7
						}//loop over all layer


//METABOLIC surface to SOM1
					C = pCh->pCProfile->fCMtbLitterSurf * kgphaTOgpm2;
					N = pCh->pCProfile->fNMtbLitterSurf * kgphaTOgpm2;
					Navail  = century_n_getNtoDepth(self,20.0);
					if (C >= 10e-6)
						{
							// Determine C/E ratios for flows to SOM1
							rcemet = century_n_AboveGroundDecompRatio(self,C,N,2.5,self->fixed);

							// If decomposition can occur
							if (century_n_CanDecompose(self,C,N,rcemet,Navail) )
								{
									// C Flow
									// Compute total C flow out of metabolic into SOM1[layer]
									tcflow = C * defacSRFC * self->fixed.dec2[SRFC] * anerb * dtDecompDC;
									// Make sure metab does not go negative.
									tcflow = MIN ( tcflow, C );
									co2loss = tcflow * self->fixed.pmco2[SRFC];

									cfmes1 = tcflow - co2loss;
									//Hong: falsch? pCh->pCProfile->fCMtbLitterSurf -= cfmes1 * gpm2TOkgpha;//Hong: Frage? cfmes1 or tcflow
									//Hong: falsch? pCh->pCProfile->fCLeafLitterSurf -= cfmes1 * gpm2TOkgpha;//Hong added for C-Balance on 20190605
									pCh->pCProfile->fCMtbLitterSurf -= tcflow * gpm2TOkgpha;//Hong: Frage? cfmes1 or tcflow
									pCh->pCProfile->fCLeafLitterSurf -= tcflow * gpm2TOkgpha;//Hong added for C-Balance on 20190605
									pCh->pCProfile->fCLitterSurf -= tcflow * gpm2TOkgpha;//Hong added for C-Balance on 20190605
									century_n_CO2intoSOIL(self,co2loss,20.0);
									century_n_FlowCintoSOM1(self,cfmes1,20.0);

									// Mineralization associated with respiration
									mineralFlow=co2loss * N/C;
									mineralFlow=century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
									pCh->pCProfile->fNMtbLitterSurf -= mineralFlow * gpm2TOkgpha;
									MinByMetabLitter += mineralFlow * gpm2TOkgpha;
									// Compute and schedule N flow from A to B.
									century_n_ScheduleNFlow(self,cfmes1,rcemet,C,N,Navail,&orgNflow, &minNflow);
									pCh->pCProfile->fNMtbLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
									century_n_FlowNintoSOM1(self,orgNflow,20.0);			 //B += orgflow
									if( minNflow >= 0.0)//Mineralisation
										{
											pCh->pCProfile->fNMtbLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
											century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
											MinByMetabLitter += minNflow * gpm2TOkgpha;
										}
									if ( minNflow < 0.0)//Immobilisation
										{
											//Flow can be limited by available N, this is done in FlowNfromMineralSoil
											//the return value gives the restricted flow of mineral N
											ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow), 20.0,-1);	//Mineral -= minflow
											century_n_FlowNintoSOM1(self, ret, 20.0);			//B += minflow
											ImmByMetabLitter += ret * gpm2TOkgpha;
										}
								}// if candecomp
						} //if (pCh->pCProfile->fCMtbLitterSurf >= 10e-6)


//METABOLIC soil to SOM1
					// Determine C/E ratios for flows to SOM1
					// This is done by analysing the first 20cm of soil, but used in all XN layers.
					if(century_n_getNtoDepth(self,20.0) <= 0.0)			rcemet=self->fixed.varat1[0]; //set ratio to max. allowed
					else if(century_n_getNtoDepth(self,20.0) <= self->fixed.varat1[2])	rcemet=self->fixed.varat1[1]; //set ratio to min. allowed
					else	//  aminrl > 0 and <= varat(2,element)
						rcemet=(1.0 - century_n_getNtoDepth(self,20.0)/self->fixed.varat1[2])*(self->fixed.varat1[0]-self->fixed.varat1[1])+self->fixed.varat1[1];

					//loop over all layer
					for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
					        pSL->pNext!=NULL; i++,pSL=pSL->pNext,pCL=pCL->pNext)
						{
							C		= pCL->fCMtbLitter * kgphaTOgpm2;
							N		= pCL->fNMtbLitter * kgphaTOgpm2;
							Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
							if (C >= 10e-7)
								{
									// If decomposition can occur
									if (century_n_CanDecompose(self,C,N,rcemet,Navail) )
										{
											// C Flow
											// Compute total C flow out of metabolic into SOM1[layer]
											tcflow = C * pdecf[i] * self->fixed.dec2[SOIL] * anerb * dtDecompDC;
											// Make sure metab does not go negative.
											tcflow = MIN ( tcflow, C );	    //gross  C flow to SOM1
											if(tcflow > 10e-7)
												{
													pCL->fCMtbLitter  -= tcflow * gpm2TOkgpha;// relocated by Hong
													pCL->fCLitter  -= tcflow * gpm2TOkgpha;//Hong added for C Balance on 20190605
													
													co2loss=tcflow * self->fixed.pmco2[SOIL];
													tcflow -= co2loss;					//net  C flow to SOM1

													pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
													pCL->fCO2C_dt		+= co2loss * gpm2TOkgpha;
													pCL->fCHumusFast  += tcflow * gpm2TOkgpha;
													pSL->fCHumus += tcflow * gpm2TOkgpha; //Hong for C balance
													//Hong: Fehler, soll oben sein pCL->fCMtbLitter  -= tcflow * gpm2TOkgpha;
													
													// Mineralization associated with respiration
													mineralFlow=co2loss * N/C;
													//mineralFlow = min (mineralFlow,Navail );
													pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
													pCL->fNMtbLitter -= mineralFlow * gpm2TOkgpha;
													MinByMetabLitter += mineralFlow * gpm2TOkgpha;
													pCL->fMinerR += mineralFlow * gpm2TOkgpha;
													// Compute and schedule N flow from A to B.
													century_n_ScheduleNFlow(self,tcflow,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
													pCL->fNMtbLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
													pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
													if( minNflow >= 0.0)//Mineralisation
														{
															pCL->fNMtbLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
															pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
															MinByMetabLitter += minNflow * gpm2TOkgpha;
															pCL->fMinerR += minNflow * gpm2TOkgpha;
														}
													if ( minNflow < 0.0)//Immobilisation
														{
															minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
															if(minNflow > 1e-7)
																{
																	pCL->fNHumusFast  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
																	frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
																	frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
																	pCL->fNH4N -= frNH4  //Mineral -= minflow
																	              *  (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
																	pCL->fNO3N -= frNO3
																	              *  (double)fabs(minNflow) * gpm2TOkgpha;
																	pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
																	pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
																	ImmByMetabLitter +=  (double)fabs(minNflow) * gpm2TOkgpha;
																	pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

																}
														}
												}//if(CtoSOM1 > 10e-7)
										} // if candecomp
								} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)
						} // loop over all layer

//_______________________________________________________________________________________

//		 V) DecomposeWood
//_______________________________________________________________________________________

					if(self->iForest == 1) //forest
						{
// FINE BRANCHES
							C		= pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
							N		= pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
							//lig		= self->par.wdlig[1];
							lig=pCh->pCProfile->fLigFracBranch;
							Navail  = century_n_getNtoDepth(self,20.0);
							if (C >= 10e-7)
								{
									tcflow=C * defacSRFC * pCh->pCProfile->fFineBranchDecR * (double)exp(-self->fixed.pligst[SRFC]*lig) * dtDecompDC;
									// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
									// See if Box A can decompose to SOM1.
									// If it can go to som1, it will also go to som2.
									// If it can't go to som1, it can't decompose at all.
									if (century_n_CanDecompose (self,C,N,rneww1[0],Navail))
										{
											// Decompose Box A to SOM2
											// -----------------------
											// C Flow
											CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
											co2loss = CtoSOM2 * self->fixed.rsplig;
											CtoSOM2 -= co2loss;//net C flow to SOM2

											pCh->pCProfile->fCBranchLitterSurf -= tcflow * gpm2TOkgpha;
											century_n_CO2intoSOIL(self,co2loss,20.0);
											century_n_FlowCintoSOM2(self,CtoSOM2,20.0);

											// Mineralization associated with respiration
											mineralFlow=co2loss * N/C;
											mineralFlow=century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
											pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha;
											MinByWood += mineralFlow * gpm2TOkgpha;

											// N flow from A to B.
											century_n_ScheduleNFlow(self,CtoSOM2,rneww1[1],C,N,Navail,&orgNflow, &minNflow);
											pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											century_n_FlowNintoSOM2(self,orgNflow,20.0);			 //B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
													century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
													MinByWood += minNflow * gpm2TOkgpha;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													//Flow can be limited by available N, this is done in FlowNfromMineralSoil
													//the return value gives the restricted flow of mineral N
													ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);	//Mineral -= minflow
													century_n_FlowNintoSOM2(self, ret,20.0);			//B += minflow
													ImmByWood +=  ret * gpm2TOkgpha;
												}

											// Decompose Box A to SOM1
											// -----------------------
											// C Flow
											CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
											if(CtoSOM1 > 10e-7)
												{
													co2loss = CtoSOM1 * self->fixed.p1co2a[SRFC];
													CtoSOM1 -= co2loss;							//net  C flow to SOM1

													century_n_CO2intoSOIL(self,co2loss,20.0);
													pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
													pCh->pCProfile->fCHumusSurf += CtoSOM1 * gpm2TOkgpha; //Hong added for C-Balance on 20190605

													// Mineralization associated with respiration
													mineralFlow=co2loss * N/C;
													mineralFlow=century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
													pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha;
													MinByWood += mineralFlow * gpm2TOkgpha;

													// Compute and schedule N flow from A to B.
													century_n_ScheduleNFlow(self,CtoSOM1,rneww1[0],C,N,Navail,&orgNflow, &minNflow);
													pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
													pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
													if( minNflow >= 0.0)//Mineralisation
														{
															pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
															century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
															MinByWood += minNflow * gpm2TOkgpha;
														}
													if ( minNflow < 0.0)//Immobilisation
														{
															//Flow can be limited by available N, this is done in FlowNfromMineralSoil
															//the return value gives the restricted flow of mineral N
															ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);//Mineral -= minflow
															pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
															ImmByWood +=  ret * gpm2TOkgpha;
														}
												}//if(CtoSOM1 > 10e-7)
										}//if Candecompose
								}//C >= 10e-7

// LARGE WOOD
							C		= pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
							N		= pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
							//lig		= self->par.wdlig[2];
							lig=pCh->pCProfile->fLigFracStem;
							Navail  = century_n_getNtoDepth(self,20.0);
							if (C >= 10e-7)
								{
									tcflow= C * defacSRFC * pCh->pCProfile->fLargeWoodDecR * (double)exp(-self->fixed.pligst[SRFC]*lig) * dtDecompDC;
									// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
									// See if Box A can decompose to SOM1.
									// If it can go to som1, it will also go to som2.
									// If it can't go to som1, it can't decompose at all.
									if (century_n_CanDecompose (self,C,N,rneww2[0],Navail))
										{
											// Decompose Box A to SOM2
											// -----------------------
											// C Flow
											CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
											co2loss = CtoSOM2 * self->fixed.rsplig;
											CtoSOM2 -= co2loss;//net C flow to SOM2

											pCh->pCProfile->fCStemLitterSurf -= tcflow * gpm2TOkgpha;
											century_n_CO2intoSOIL(self,co2loss,20.0);
											century_n_FlowCintoSOM2(self,CtoSOM2,20.0);

											// Mineralization associated with respiration
											mineralFlow = co2loss * N/C;
											mineralFlow = century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
											pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha;
											MinByWood += mineralFlow * gpm2TOkgpha;

											// Compute and schedule N flow from A to B.
											century_n_ScheduleNFlow(self,CtoSOM2,rneww2[1],C,N,Navail,&orgNflow, &minNflow);
											pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											century_n_FlowNintoSOM2(self,orgNflow,20.0);			 //B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
													century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
													MinByWood += minNflow * gpm2TOkgpha;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													//Flow can be limited by available N, this is done in FlowNfromMineralSoil
													//the return value gives the restricted flow of mineral N
													ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);	//Mineral -= minflow
													century_n_FlowNintoSOM2(self, ret, 20.0);			//B += minflow
													ImmByWood +=  ret * gpm2TOkgpha;
												}

											// Decompose Box A to SOM1
											// -----------------------
											// C Flow
											CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
											if(CtoSOM1 > 10e-7)
												{
													co2loss = CtoSOM1 * self->fixed.p1co2a[SRFC];
													CtoSOM1 -= co2loss;							//net  C flow to SOM1

													// C flow
													century_n_CO2intoSOIL(self,co2loss,20.0);
													pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
													pCh->pCProfile->fCHumusSurf += CtoSOM1 * gpm2TOkgpha; //Hong added for C-Balance on 20190605

													// Mineralization associated with respiration
													mineralFlow=co2loss * N/C;
													mineralFlow=century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
													pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha;
													MinByWood += mineralFlow * gpm2TOkgpha;

													// N flow from A to B.
													century_n_ScheduleNFlow(self,CtoSOM1,rneww2[0],C,N,Navail,&orgNflow, &minNflow);
													pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
													pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
													if( minNflow >= 0.0)//Mineralisation
														{
															pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
															century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
															MinByWood += minNflow * gpm2TOkgpha;
														}
													if ( minNflow < 0.0)//Immobilisation
														{
															//Flow can be limited by available N, this is done in FlowNfromMineralSoil
															//the return value gives the restricted flow of mineral N
															ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);//Mineral -= minflow
															pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
															ImmByWood +=  ret * gpm2TOkgpha;
														}
												}//if(CtoSOM1 > 10e-7)
										}//if Candecompose
								}//C >= 10e-7

// COARSE ROOTS
							for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
							        pSL->pNext!=NULL; i++,pSL=pSL->pNext,pCL=pCL->pNext)
								{
									C		= pCL->fCGrossRootLitter * kgphaTOgpm2;
									N		= pCL->fNGrossRootLitter * kgphaTOgpm2;
									dummy=C/N;

									//lig		= self->par.wdlig[3] ;
									lig=pCh->pCProfile->fLigFracCoarseRoot;
									Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
									if (C >= 10e-7)
										{
											tcflow= C * pdecf[i] * pCh->pCProfile->fCoarseRootDecR * (double)exp(-self->fixed.pligst[SOIL]* lig) * dtDecompDC * anerb;
											// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
											// See if Box A can decompose to SOM1.
											// If it can go to som1, it will also go to som2.
											// If it can't go to som1, it can't decompose at all.
											if (century_n_CanDecompose (self,C,N,rneww3[0],Navail))
												{
													// Decompose Box A to SOM2
													// -----------------------
													// C Flow
													CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
													co2loss = CtoSOM2 * self->fixed.rsplig;
													CtoSOM2 -= co2loss;//net C flow to SOM2

													// C flow
													pCL->fCGrossRootLitter -= tcflow * gpm2TOkgpha;
													pCL->fCManure -= tcflow * gpm2TOkgpha;//Hong added for C balance on 20190605
													pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
													pCL->fCO2C_dt		+= co2loss * gpm2TOkgpha;
													pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
													pSL->fCHumus  += CtoSOM2 * gpm2TOkgpha; //Hong for C balance

													// Mineralization associated with respiration
													mineralFlow = co2loss * N/C;
													pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
													pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha;
													MinByWood += mineralFlow * gpm2TOkgpha;
													pCL->fMinerR += mineralFlow * gpm2TOkgpha;
													// N flow from A to B.
													century_n_ScheduleNFlow(self,CtoSOM2,rneww3[1],C,N,Navail,&orgNflow, &minNflow);
													pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
													//falsch pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
													pCL->fNHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
													if( minNflow >= 0.0)//Mineralisation
														{
															pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
															pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
															MinByWood += minNflow * gpm2TOkgpha;
															pCL->fMinerR += minNflow * gpm2TOkgpha;
														}
													if ( minNflow < 0.0)//Immobilisation
														{
															minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
															if(minNflow > 1e-7)
																{
																	pCL->fCHumusSlow  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
																	pSL->fCHumus  +=  (double)fabs(minNflow) * gpm2TOkgpha;//Hong for C balance
																	frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
																	frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
																	pCL->fNH4N -= frNH4  //Mineral -= minflow
																	              *  (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
																	pCL->fNO3N -= frNO3
																	              *  (double)fabs(minNflow) * gpm2TOkgpha;
																	pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
																	pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
																	ImmByWood +=  (double)fabs(minNflow) * gpm2TOkgpha;
																	pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

																}
														}

													// Decompose Box A to SOM1
													// -----------------------
													// C Flow
													CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
													if(CtoSOM1 > 10e-7)
														{
															co2loss = CtoSOM1 * self->fixed.p1co2a[SOIL];
															CtoSOM1 -= co2loss;							//net  C flow to SOM1

															// C flow
															pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
															pCL->fCO2C_dt		+= co2loss * gpm2TOkgpha;
															pCL->fCHumusFast  += CtoSOM1 * gpm2TOkgpha;
															pSL->fCHumus += CtoSOM1 * gpm2TOkgpha; //Hong for C balance

															// Mineralization associated with respiration
															mineralFlow=co2loss * N/C;
															pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
															pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha;
															MinByWood += mineralFlow * gpm2TOkgpha;
															pCL->fMinerR += mineralFlow * gpm2TOkgpha;
															// N flow from A to B.
															century_n_ScheduleNFlow(self,CtoSOM1,rneww3[0],C,N,Navail,&orgNflow, &minNflow);
															pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
															//Hong: falsch pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
															pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
															if( minNflow >= 0.0)//Mineralisation
																{
																	pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
																	pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
																	MinByWood += minNflow * gpm2TOkgpha;
																	pCL->fMinerR += minNflow * gpm2TOkgpha;
																}
															if ( minNflow < 0.0)//Immobilisation
																{
																	minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
																	if(minNflow > 1e-7)
																		{
																			pCL->fCHumusFast  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
																			pSL->fCHumus += (double)fabs(minNflow) * gpm2TOkgpha;		//Hong for C balance
																			
																			pCL->fNH4N -= frNH4  //Mineral -= minflow
																			              *  (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
																			pCL->fNO3N -= frNO3
																			              *  (double)fabs(minNflow) * gpm2TOkgpha;
																			pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
																			pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
																			ImmByWood +=  (double)fabs(minNflow) * gpm2TOkgpha;
																			pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

																		}
																}
														}//if(CtoSOM1 > 10e-7)
												}//if Candecompose
										}//C >= 10e-7
								}//loop over all layer
						}//if site is a forest
//_______________________________________________________________________________________
//		 VI) DecomposeSOM (dtDecomp, anerb, defac);
//_______________________________________________________________________________________

//	VI) 1. Surface SOM1 decomposes to SOM2 with CO2 loss.
					C		= pCh->pCProfile->fCMicLitterSurf * kgphaTOgpm2;
					N		= pCh->pCProfile->fNMicLitterSurf * kgphaTOgpm2;
					Navail  = century_n_getNtoDepth(self,20.0);
					if( C >= 10e-7 )
						{
//	determine C/N ratios for flows to SOM2
							rceto2=C/N + self->fixed.rad1p[0] +  self->fixed.rad1p[1]*(C/N - self->fixed.pcemic[1]);
							rceto2=MAX(rceto2,self->fixed.rad1p[2]);
//  If decomposition can occur, schedule flows associated with /
//  respiration and decomposition
							if (century_n_CanDecompose (self,C, N , rceto2, Navail) )
								{
									// Decompose Box A to SOM2
									// -----------------------
									// Calculate C Flow
									// total C flow out of surface microbes.
									microbeC=C * defacSRFC * self->fixed.dec3[SRFC] * dtDecompDC;
									co2loss = microbeC * self->fixed.p1co2a[SRFC];
									// cfsfs2 is C Flow from SurFace som1 to Som2
									cfsfs2 = microbeC - co2loss;

									//C flow
									pCh->pCProfile->fCMicLitterSurf -= microbeC * gpm2TOkgpha;
									pCh->pCProfile->fCHumusSurf -= microbeC * gpm2TOkgpha;//Hong added for C balance on 20190605
									century_n_CO2intoSOIL(self,co2loss,20.0);
									century_n_FlowCintoSOM2(self,cfsfs2,20.0);

									//Mineralization associated with respiration
									mineralFlow = co2loss * N/C;
									mineralFlow = century_n_FlowNintoMineralSoil(self,mineralFlow,20.0,0.0);
									pCh->pCProfile->fNMicLitterSurf -= mineralFlow * gpm2TOkgpha;
									MinBySOM += mineralFlow * gpm2TOkgpha;
									//N flow from A to B.
									century_n_ScheduleNFlow(self,cfsfs2,rceto2,C,N,Navail,&orgNflow, &minNflow);
									pCh->pCProfile->fNMicLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
									century_n_FlowNintoSOM2(self,orgNflow,20.0);	 //B += orgflow
									if( minNflow >= 0.0)//Mineralisation
										{
											pCh->pCProfile->fNMicLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
											century_n_FlowNintoMineralSoil(self,minNflow,20.0,0.0);	//Mineral += minflow
											MinBySOM += minNflow * gpm2TOkgpha;
										}
									if ( minNflow < 0.0)//Immobilisation
										{
											//Flow can be limited by available N, this is done in FlowNfromMineralSoil
											//the return value gives the restricted flow of mineral N
											ret = century_n_FlowNfromMineralSoil(self, (double)fabs(minNflow),20.0,-1);	//Mineral -= minflow
											century_n_FlowNintoSOM2(self, ret, 20.0);			//B += minflow
											ImmBySOM +=  ret * gpm2TOkgpha;
										}
								}//if canDecompose
						} // if amount is significant



//	VI) 2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss

					// Determine C/E ratios for flows to SOM2 and SOM3
					// This is done by analysing the first 20cm of soil, but used in all XN layers.
					rceto2=rceto3=0.0;
					if(century_n_getNtoDepth(self,20.0) <=0.0 )
						{
							rceto2 = self->fixed.varat2[0];	   //set to maximum allowed ??<=??
							rceto3 = self->fixed.varat3[0];
						}
					else if (century_n_getNtoDepth(self,20.0) > self->fixed.varat2[2])
						{
							rceto2 = self->fixed.varat2[1];
							rceto3 = self->fixed.varat3[1];
						} //set to minimal allowed
					else // >0 and <= varat2_2
						{
							rceto2 = (1.0-century_n_getNtoDepth(self,20.0)/self->fixed.varat2[2])*(self->fixed.varat2[0]-self->fixed.varat2[1])+self->fixed.varat2[1];
							rceto3 = (1.0-century_n_getNtoDepth(self,20.0)/self->fixed.varat3[2])*(self->fixed.varat3[0]-self->fixed.varat3[1])+self->fixed.varat3[1];
						}
					//loop over all layer
					for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
					        pSL->pNext!=NULL; i++,pSL=pSL->pNext,pCL=pCL->pNext)
						{
							C		= pCL->fCHumusFast * kgphaTOgpm2;
							N		= pCL->fNHumusFast * kgphaTOgpm2;
							Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2;
							if (C >= 10e-7)
								{
									// If soil som1 can decompose to som2, it will also go to som3.
									// If it can't go to som2, it can't decompose at all.
									// If decomposition can occur
									if (century_n_CanDecompose(self,C,N,rceto2,Navail) )
										{
											//C Flow
											efftext = self->fixed.peftxa + self->fixed.peftxb * pSL->fSand*0.01;
											microbeC =C * pdecf[i] * self->fixed.dec3[SOIL] * dtDecompDC * anerb * efftext;//cltfac=1 for no cultivation
											co2loss = microbeC * self->fixed.p1co2a[SOIL];
											cfs1s3 = microbeC * (self->fixed.ps1s3[0]+self->fixed.ps1s3[1]*pSL->fClay*0.01) *
											         (self->fixed.animp * (1.0 - anerb) + 1.0); // C to SOM3
											// SOM2 gets what's left of microbeC.
											cfs1s2 = microbeC - co2loss - cfs1s3;
											//C flow
											//pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha; //Hong: Felher?
											//pCh->pCLayer->fCO2C_dt += co2loss * gpm2TOkgpha;//Hong: Fehler!
											pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
											pCL->fCO2C_dt += co2loss * gpm2TOkgpha;//changed by Hong
											pCL->fCHumusStable  += cfs1s3 * gpm2TOkgpha;
											pCL->fCHumusSlow  += cfs1s2 * gpm2TOkgpha;
											pCL->fCHumusFast   -= microbeC * gpm2TOkgpha;
											pSL->fCHumus -= co2loss * gpm2TOkgpha; //Hong for C balance
											// Mineralization associated with respiration
											mineralFlow=co2loss * N/C;
											pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
											pCL->fNHumusFast -= mineralFlow * gpm2TOkgpha;
											MinBySOM += mineralFlow * gpm2TOkgpha;
											pCL->fMinerR += mineralFlow * gpm2TOkgpha;
											// to SOM3
											// N flow from A to B.
											century_n_ScheduleNFlow(self,cfs1s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
											pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
													pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
													MinBySOM += minNflow * gpm2TOkgpha;
													pCL->fMinerR += minNflow * gpm2TOkgpha;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
													if(minNflow > 1e-7)
														{
															pCL->fNHumusStable  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
															frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
															pCL->fNH4N -= frNH4  //Mineral -= minflow
															              *  (double)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate
															pCL->fNO3N -= frNO3
															              *  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
															pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
															ImmBySOM +=  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

														}
												}

											//to SOM2
											//N flow from A to B.
											century_n_ScheduleNFlow(self,cfs1s2,rceto2,C,N,Navail,&orgNflow, &minNflow);
											pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											pCL->fNHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
													pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
													MinBySOM += minNflow * gpm2TOkgpha;
													pCL->fMinerR += minNflow * gpm2TOkgpha;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
													if(minNflow > 1e-7)
														{
															pCL->fNHumusSlow  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															pCL->fNH4N -= frNH4  //Mineral -= minflow
															              *  (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
															pCL->fNO3N -= frNO3
															              *  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
															pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
															ImmBySOM +=  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

														}
												}

										} // if candecomp
								} //if (pCh->pCLayer->fCHumusFast  >= 10e-7)
						} // loop over all layer


//	VI)	3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss

					// Determine C/E ratios for flows to SOM1 and SOM3
					// This is done by analysing the first 20cm of soil, but used in all XN layers.
					rceto1= 0.0;
					rceto3= 0.0;
					if(century_n_getNtoDepth(self,20.0) <=0.0 )
						{
							rceto1 = self->fixed.varat1[0];    //set to maximum allowed
							rceto3 = self->fixed.varat3[0];
						}
					else if (century_n_getNtoDepth(self,20.0) > self->fixed.varat1[2])
						{
							rceto1 = self->fixed.varat1[1];    //set to minimal allowed
							rceto3 = self->fixed.varat3[1];
						}
					else // >0 and <= varat1_2
						{
							rceto1 = (1.0-century_n_getNtoDepth(self,20.0)/self->fixed.varat1[2])*(self->fixed.varat1[0]-self->fixed.varat1[1])+self->fixed.varat1[1];
							rceto3 = (1.0-century_n_getNtoDepth(self,20.0)/self->fixed.varat3[2])*(self->fixed.varat3[0]-self->fixed.varat3[1])+self->fixed.varat3[1];
						}

					//loop over all layer
					for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
					        pSL->pNext!=NULL; i++,pSL=pSL->pNext,pCL=pCL->pNext)
						{
							C		= pCL->fCHumusSlow * kgphaTOgpm2;
							N		= pCL->fNHumusSlow * kgphaTOgpm2;
							Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2;
							if (C >= 10e-7)
								{
									// If soil som2 can decompose to som1, it will also go to som3.
									// If it can't go to som1, it can't decompose at all.
									// If decomposition can occur
									if (century_n_CanDecompose(self,C,N,rceto1,Navail) )
										{
											// C Flow
											tcflow=C * pdecf[i] * self->fixed.dec5 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
											co2loss = tcflow * self->fixed.p2co2;
											cfs2s3 = tcflow * (self->fixed.ps2s3[0]+self->fixed.ps2s3[1]*pSL->fClay*0.01) *
											         (self->fixed.animp * (1.0 - anerb) + 1.0);
											cfs2s1 = tcflow - co2loss - cfs2s3;//SOM1 gets what's left of tcflow.
											// C flow
											//pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;//Hong: Felher?
											//pCh->pCLayer->fCO2C_dt += co2loss * gpm2TOkgpha;//Hong: Fehler!
											pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
											pCL->fCO2C_dt += co2loss * gpm2TOkgpha;//Changed by Hong
											pCL->fCHumusFast    += cfs2s1 * gpm2TOkgpha;
											pCL->fCHumusStable  += cfs2s3 * gpm2TOkgpha;
											pCL->fCHumusSlow    -= tcflow * gpm2TOkgpha;
											pSL->fCHumus -= co2loss * gpm2TOkgpha; //Hong for C balance
											// Mineralization associated with respiration
											mineralFlow=co2loss * N/C;
											mineralFlow = MIN(mineralFlow, pCL->fNHumusSlow * kgphaTOgpm2);
											pCL->fNH4N			+= mineralFlow * gpm2TOkgpha; // all to ammonium
											pCL->fNHumusSlow	-= mineralFlow * gpm2TOkgpha;
											MinBySOM			+= mineralFlow * gpm2TOkgpha;
											pCL->fMinerR		+= mineralFlow * gpm2TOkgpha ;
											//SB! Fehler in N Bilanz im Logfile, nicht in der Grafischen Ausgabe,
											//nur hier... funktioniert, wenn oberen 4 Zeilen auskommentiert sind.
											//Fehler erscheint sogar täglich, wenn die unteren beiden Zeilen auskommentiert werden ???
											//Numerik ? double -> double?
											//pCL->fNH4N		+= 0.1;
											// pCL->fMinerR	+= 0.1;

											if (pCL->fNO3N < 0)		PRINT_ERROR_ID((&(self->parent)),"neg. Mineral NO3N 3a");
											if (pCL->fNH4N < 0)		PRINT_ERROR_ID((&(self->parent)),"neg. Mineral NH4N 3a");
											//to SOM3
											// N flow from A to B.
											century_n_ScheduleNFlow(self,cfs2s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
											pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
													pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
													MinBySOM += minNflow * gpm2TOkgpha;
													pCL->fMinerR += minNflow * gpm2TOkgpha;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
													if(minNflow > 1e-7)
														{
															pCL->fNHumusStable  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
															frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
															pCL->fNH4N -= frNH4  //Mineral -= minflow
															              *  (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
															pCL->fNO3N -= frNO3
															              *  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
															pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
															ImmBySOM +=  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

														}
												}

											//to SOM1
											// N flow from A to B.
											century_n_ScheduleNFlow(self,cfs2s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
											pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
													pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
													MinBySOM += minNflow * gpm2TOkgpha;
													pCL->fMinerR += minNflow * gpm2TOkgpha;
												}
											

											if ( minNflow < 0.0)//Immobilisation
												{
													minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
													if(minNflow > 1e-7)
														{
															pCL->fNHumusFast  +=  (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
															frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
															pCL->fNH4N -= frNH4   //Mineral -= minflow
															              *  (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
															pCL->fNO3N -= frNO3
															              *  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
															pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
															ImmBySOM +=  (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNImmobR +=  (double)fabs(minNflow) * gpm2TOkgpha;

														}
												}
										} // if candecomp
								} //if (pCh->pCLayer->fCHumusSlow  >= 10e-7)

						} // loop over all layer


//	IV)	4. SOM3 decomposes to soil SOM1 with CO2 loss.

					// Determine C/E ratios for flows to SOM1 : already done
					//loop
					for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
					        pSL->pNext!=NULL; i++,pSL=pSL->pNext,pCL=pCL->pNext)
						{
							C		= pCL->fCHumusStable * kgphaTOgpm2;
							N		= pCL->fNHumusStable * kgphaTOgpm2;
							Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //

							if (C >= 10e-7)
								{
									// If decomposition can occur
									if (century_n_CanDecompose(self,C,N,rceto1,Navail) )
										{
											// Calculate C Flow
											// C flow out of metabolic into SOM1[layer]
											tcflow = C * pdecf[i] * self->fixed.dec4 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
											co2loss=tcflow * self->fixed.p3co2;
											cfs3s1 = tcflow - co2loss;

											// C flow
											//pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;//Hong: Felher?
											//pCh->pCLayer->fCO2C_dt += co2loss * gpm2TOkgpha;//Hong: Fehler!
											pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
											pCL->fCO2C_dt += co2loss * gpm2TOkgpha;//Changed by Hong
											pCL->fCHumusFast   += cfs3s1 * gpm2TOkgpha;
											pCL->fCHumusStable  -= tcflow * gpm2TOkgpha;
											pSL->fCHumus -= co2loss * gpm2TOkgpha; //Hong for C balance
											
											// Mineralization associated with respiration
											mineralFlow=co2loss * N/C;
											mineralFlow = MIN (mineralFlow,pCL->fNHumusStable * kgphaTOgpm2 );
											pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
											pCL->fNHumusStable -= mineralFlow * gpm2TOkgpha;
											MinBySOM += mineralFlow * gpm2TOkgpha;
											pCL->fMinerR += mineralFlow * gpm2TOkgpha;
											// N flow from A to B.
											century_n_ScheduleNFlow(self,cfs3s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
											pCL->fNHumusStable -= orgNflow * gpm2TOkgpha;	//A -= orgflow
											//Hong: falsch pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
											pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
											if( minNflow >= 0.0)//Mineralisation
												{
													pCL->fNHumusStable -= minNflow * gpm2TOkgpha;	//A -= minflow
													pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
													MinBySOM += minNflow * gpm2TOkgpha;
													pCL->fMinerR += minNflow * gpm2TOkgpha;
												}
											if ( minNflow < 0.0)//Immobilisation
												{
													minNflow = (double)MIN (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
													if(minNflow > 1e-7)
														{
															//Hong: falsch? pCL->fCHumusFast  += (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															pCL->fNHumusFast  += (double)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
															frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
															frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
															pCL->fNH4N -= frNH4  //Mineral -= minflow
															              * (double)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate
															pCL->fNO3N -= frNO3
															              * (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNO3N = (pCL->fNO3N < 0.0) ? (double)0.0 : pCL->fNO3N; //numerical issue
															pCL->fNH4N = (pCL->fNH4N < 0.0) ? (double)0.0 : pCL->fNH4N; //numerical issue
															ImmBySOM += (double)fabs(minNflow) * gpm2TOkgpha;
															pCL->fNImmobR += (double)fabs(minNflow) * gpm2TOkgpha;

														}
												}
										} // if candecomp
								} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)

							if (pCL->fNO3N < 0)		PRINT_ERROR_ID((&(self->parent)),"neg. Mineral NO3N ");
							if (pCL->fNH4N < 0)		PRINT_ERROR_ID((&(self->parent)),"neg. Mineral NH4N ");

						} // loop over all layer

				} //4 times a day

//*******************************************************************************
//CEAN UP HEAP
			free(pdecf);
//*******************************************************************************


//*******************************************************************************
//	write ExpertN variables
//	for result files and Denitrification modul
//*******************************************************************************
//write mineralisation/immobilisation by pools into first real XN layer
//values could be calculated per layer, but thats not implemented yet.
			pCh->pCLayer->pNext->fHumusMinerR   = MinBySOM; // pTi->pTimeStep->fAct;
			pCh->pCLayer->pNext->fLitterMinerR  = (MinByStructLitter + MinByMetabLitter+ MinByWood); // pTi->pTimeStep->fAct;
// fNHumusImmobR is not summed up in XN, use Day instead!!
			//pCh->pCProfile->fNHumusImmobDay    = ImmBySOM ;
			//unter Linux machen wir das richtig:
			pCh->pCLayer->pNext->fNHumusImmobR = ImmBySOM;///pTi->pTimeStep->fAct;
			pCh->pCLayer->pNext->fNLitterImmobR = (ImmByInput + ImmByStructLitter
			                                      + ImmByMetabLitter+ ImmByWood); // pTi->pTimeStep->fAct; // - DailyCorrection;wird wohl nie verwendet
							
//write humus and FOS für Ceres-N Denitrifikation
			for(i=0, pCL=pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext ; pSL->pNext != NULL ;
			        pSL=pSL->pNext,pCL=pCL->pNext, i++)
				{
					//pSL->fCHumus = pCL->fCHumusSlow + pCL->fCHumusFast + pCL->fCHumusStable;
					//diff = pSL->fCHumus - self->fCHumus_old[i];
					//Hong: update of humus, litter and manure:
					//pSL->fCHumus += (pCL->fCHumusSlow + pCL->fCHumusFast + pCL->fCHumusStable) -self->fCHumus_old[i];;
					
					
					//pCL->fCLitter = pCL->fCStrcLitter +pCL->fCMtbLitter;//added by Hong on 20190604
					//pCL->fCManure = pCL->fCGrossRootLitter; 
					//End of Hong 
					
					//like Mineralisation_Fagus: just the fine root litter is written to FOM[i], no coarse roots
					
					pCL->afCOrgFOMFrac[1]= pCL->fCMtbLitter + pCL->fCStrcLitter;
					pCL->fNImmobR/=pTi->pTimeStep->fAct;
					
					pCL->fMinerR/=pTi->pTimeStep->fAct;
					pCL->fHumusMinerR/=pTi->pTimeStep->fAct;
					pCL->fCO2ProdR/=pTi->pTimeStep->fAct;
					//pCL->fCO2C_dt/=pTi->pTimeStep->fAct; //Fehler?
					pCL->fLitterMinerR/=pTi->pTimeStep->fAct;
					pCL->fNLitterImmobR/=pTi->pTimeStep->fAct;
					pCL->fNHumusImmobR/=pTi->pTimeStep->fAct;
					
					pCL->fCO2C	+= pCL->fCO2C_dt;//Hong added for C balance
					//Test of Hong
                    //pSL->fNHumus  = pCL->fNHumusFast  + pCL->fNHumusSlow +pCL->fNHumusStable;
					//pCL->fNLitter = pCL->fNStrcLitter + pCL->fNMtbLitter;
					//End of TESt
				}

// Hong on 20190604: update of litterSurf and HumusSurf 
//pCh->pCProfile->fCLitterSurf = pCh->pCProfile->fCStrLitterSurf +pCh->pCProfile->fCMtbLitterSurf; //falsch!!!
//pCh->pCProfile->fCHumusSurf = pCh->pCProfile->fCMicLitterSurf;
//End of Hong

//*******************************************************************************
// write resultfile: "century.out"
//*******************************************************************************
//open file
//if(outf==NULL) ... exception handling
			/*
			if (pTi->pSimTime->fTimeAct == 0)
				outf = fopen("/home/chrikle/projects/newmatrix/expertn_modul/century_n/src/century.out", "w");	//Create file
			else outf = fopen("/home/chrikle/projects/newmatrix/expertn_modul/century_n/src/century.out", "a+");	//append

			{
				static int first=0;
				if (first==0)
					{
						first = 1;

						fprintf(outf, "pTi->pSimTime->fTimeAct,");

						fprintf(outf, "pCh->pCLayer->pNext->fNImmobR," );
						fprintf(outf, "ImmByInput," );
						fprintf(outf, "ImmBySOM," );
						fprintf(outf, "ImmByStructLitter," );
						fprintf(outf, "ImmByMetabLitter," );
						fprintf(outf, "ImmByWood," );
						//8
						fprintf(outf, "pCh->pCLayer->pNext->fMinerR," );
						fprintf(outf, "MinBySOM," );
						fprintf(outf, "MinByStructLitter," );
						fprintf(outf, "MinByMetabLitter," );
						fprintf(outf, "MinByWood," );
						//13f. NNM
						fprintf(outf, "pCh->pCLayer->pNext->fMinerR - pCh->pCLayer->pNext->fNImmobR," );
						fprintf(outf, "MinBySOM - ImmBySOM,");
						fprintf(outf, "MinByStructLitter -ImmByStructLitter,");
						fprintf(outf, "MinByMetabLitter -ImmByMetabLitter,");
						fprintf(outf, "MinByWood -ImmByWood,");
						//18: CumCorrection
						fprintf(outf, "CumCorrection");
						//new line
						fprintf(outf, "\n");
					}
			}

			fprintf(outf, "%f,",pTi->pSimTime->fTimeAct);
//Variables
//Profile
			fprintf(outf, "%f,",pCh->pCLayer->pNext->fNImmobR );
			fprintf(outf, "%f,",ImmByInput );
			fprintf(outf, "%f,",ImmBySOM );
			fprintf(outf, "%f,",ImmByStructLitter );
			fprintf(outf, "%f,",ImmByMetabLitter );
			fprintf(outf, "%f,",ImmByWood );
			//8
			fprintf(outf, "%f,",pCh->pCLayer->pNext->fMinerR );
			fprintf(outf, "%f,",MinBySOM );
			fprintf(outf, "%f,",MinByStructLitter );
			fprintf(outf, "%f,",MinByMetabLitter );
			fprintf(outf, "%f,",MinByWood );
			//13f. NNM
			fprintf(outf, "%f,",pCh->pCLayer->pNext->fMinerR - pCh->pCLayer->pNext->fNImmobR );
			fprintf(outf, "%f,",MinBySOM - ImmBySOM);
			fprintf(outf, "%f,",MinByStructLitter -ImmByStructLitter);
			fprintf(outf, "%f,",MinByMetabLitter -ImmByMetabLitter);
			fprintf(outf, "%f,",MinByWood -ImmByWood);
			//18: CumCorrection
			fprintf(outf, "%f.",self->CumCorrection);
			//new line
			fprintf(outf, "\n");
			//close file
			fclose(outf);
*/
		}// if new day
	
	
	
	return RET_SUCCESS;
}

void century_n_decf20cm(century_n *self, double * defac, double * anerb, struct_fixedparameter fixed,double drain)
{
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	expertn_modul_base *xpn = &(self->parent);
	PCLIMATE pCl = xpn->pCl;
	PHEAT pHe = xpn->pHe ;
	PWATER pWa = xpn->pWa;
	double avgWFPS,a,b,c,d,e1,e2,base1,base2,wfunc,tfunc;
	double DailyPET,xh20,ratioH2OtoPET;
// a)Determine texture index (COARSE,MEDIUM, FINE, VERY FINE);
	if (century_n_getSandtoDepth(self,20.0) > 70.0 ) textureindex=COARSE;
	if (century_n_getSandtoDepth(self,20.0) <= 70.0 && century_n_getSandtoDepth(self,20.0) >= 30.0  ) textureindex=MEDIUM;
	if (century_n_getSandtoDepth(self,20.0) < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"

// b)Determine water factor
	avgWFPS=century_n_getWFPStoDepth(self,20.0); //water filled pore space to 20cm depth
	switch(textureindex)
		{
		case COARSE:
			a=0.55;
			b=1.70;
			c=-0.007;
			d=3.22;
			break;
		case MEDIUM:
			a=0.60;
			b=1.27;
			c=0.012;
			d=2.84;
			break;
		case FINE:
			a=0.60;
			b=1.27;
			c=0.012;
			d=2.84;
			break;//Daycent: no values here. ??
		case VERYFINE:
			a=0.60;
			b=1.27;
			c=0.012;
			d=2.84;
			break; //Where can it be set to VERYFINE?
		 default:
			PRINT_ERROR_ID((&(self->parent)),"");
		    break;
		}
	//exponent bases
	base1=(avgWFPS - b)/(a-b);
	base2=(avgWFPS - c)/(a-c);
	//exponents
	e1=d*(b-a)/(a-c);
	e2=d;
	//water factor
	wfunc= (double)pow(base1,e1)*(double)pow(base2,e2);
	if(wfunc > 1.0) wfunc=1.0;

// c)Determine temperature factor
//	 Century uses temperature at 5cm depth
	tfunc=( fixed.teff[1] + (fixed.teff[2]/
	                         (double)PI)*(double)atan((double)PI*fixed.teff[3]*(pHe->pHLayer->pNext->fSoilTemp - fixed.teff[0])) )
	      /( fixed.teff[1] + (fixed.teff[2]/
	                          (double)PI)*(double)atan((double)PI*fixed.teff[3]*(30.0 - fixed.teff[0])) ); // normalized to 30°C
	tfunc=MIN(1.2,MAX(0.01,tfunc));
//printf("C Temp: %f %f\n",self->parent.pHe->pHLayer->pNext->fSoilTemp,self->parent.pTi->pSimTime->fTimeAct);
// d)Compute Decomposition factor
	*defac=MAX(0.0, wfunc*tfunc);
	*defac=wfunc*tfunc;

// e)Determine factor of anaerobic conditions
	//drain: the fraction of excess water lost by drainage.
	//Anaerobic conditions (high soil water content) cause decomposition to decrease.
	//The soil drainage factor (site parameter DRAIN) allows a soil to have differing degrees of wetness
	//(e.g., DRAIN=1 for well drained sandy soils and DRAIN=0 for a poorly drained clay soil).
	DailyPET=0.1* pWa->fPotETDay;
	if(DailyPET<=0.01) DailyPET=0.01;
	/*ratioH2OtoPET = (getH2OtoDepth(-1.0,exp_p) + 0.1*pCl->pWeather->fRainAmount) //+ pMa->pIrrigation->fAmount causes fault
		                  /DailyPET;  */
//ck:
	ratioH2OtoPET = (century_n_getH2OtoDepth(self,-1.0) + 0.1*pCl->pWeather->fPreciRate) //+ pMa->pIrrigation->fAmount causes fault
	                /DailyPET;

	*anerb=1.0;
//ck:
	/*if(pCl->pWeather->fTempAve > 2.0 && ratioH2OtoPET > fixed.aneref[0]){
			xh20=(ratioH2OtoPET-fixed.aneref[0])*0.1*pWa->fPotETDay*(1.0-drain);
			if(xh20>0.0){
			*anerb=max(fixed.aneref[2],
			( ((1.0-fixed.aneref[2])/(fixed.aneref[0]-fixed.aneref[1])) * xh20/(0.1*pWa->fPotETDay) + 1.0 ));
			}*/
	//if(*(self->MeanTemp) > 2.0 && ratioH2OtoPET > fixed.aneref[0])
	if(pCl->pWeather->fTempAir > 2.0 && ratioH2OtoPET > fixed.aneref[0])
		{
			xh20=(ratioH2OtoPET-fixed.aneref[0])*0.1*pWa->fPotETR*(1.0-drain);
			if(xh20>0.0)
				{
					*anerb=MAX(fixed.aneref[2],
					           ( ((1.0-fixed.aneref[2])/(fixed.aneref[0]-fixed.aneref[1])) * xh20/(0.1*pWa->fPotETDay) + 1.0 ));
				}
		}
	C_DEBUG(pWa->fPotETDay);
}//decf20cm

// decomposition factor of soil layer
double century_n_decfSL(century_n *self,double sand, double WFPS, double SoilTemp, struct_fixedparameter fixed)
{
	double defac;  //return value
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	double a,b,c,d,e1,e2,base1,base2,wfunc,tfunc;

	// change: ck --> ansonsten kann es sein, dass textureindex nicht gestetzt ist
	textureindex=MEDIUM;

// a)Determine texture index (COARSE,MEDIUM, FINE, VERY FINE);
	if (sand > 70.0 ) textureindex=COARSE;
	if (sand <= 70.0 && century_n_getSandtoDepth(self,20.0) >= 30.0  ) textureindex=MEDIUM;
	if (sand < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"

// b)Determine water factor
	switch(textureindex)
		{
		case COARSE:
			a=0.55;
			b=1.70;
			c=-0.007;
			d=3.22;
			break;
		case MEDIUM:
			a=0.60;
			b=1.27;
			c=0.012;
			d=2.84;
			break;
		case FINE:
			a=0.60;
			b=1.27;
			c=0.012;
			d=2.84;
			break; //Daycent: no values here. ??
		case VERYFINE:
			a=0.60;
			b=1.27;
			c=0.012;
			d=2.84;
			break; //Where can it be set to VERYFINE?
		default:
			PRINT_ERROR_ID((&(self->parent)),"");
		    break;
		}
	//exponent bases
	base1=(WFPS - b)/(a-b);
	base2=(WFPS - c)/(a-c);
	//exponents
	e1=d*(b-a)/(a-c);
	e2=d;
	//water factor
	wfunc= (double)pow(base1,e1)*(double)pow(base2,e2);
	if(wfunc > 1.0) wfunc=1.0;

// c)Determine temperature factor
//	 Century uses temperature at 5cm depth
	tfunc=( fixed.teff[1] + (fixed.teff[2]/(double)PI)*(double)((double)PI*fixed.teff[3]*(SoilTemp - fixed.teff[0])) )
	      /( fixed.teff[1] + (fixed.teff[2]/(double)PI)*(double)atan((double)PI*fixed.teff[3]*(30.0     - fixed.teff[0])) ); // normalized to 30°C
	tfunc=MIN(1.2,MAX(0.01,tfunc));

// d)Compute Decomposition factor
	defac=MAX(0.0, wfunc*tfunc);
	defac=wfunc*tfunc;
	return defac;
}//decf soil layer

/********************************************************************
Partition Residue: Input: source pool of residue, C amount, ratio residue: N/C, lignin fraction
Partions the residue into structual and metabolic and schedules flows.

returns direct adsorbed mineral N [g/m2]

NB: no direct adsorption of Surface litter
    arguments: C/N/lig of TMP struc/metab layer
*********************************************************************/

double century_n_PartitionResidue(century_n *self,
    double C,			// C Content g/m2 of litter
    double recres,		//  residue: ratio N/C
    double frlig,		// // fraction of incoming material = lignin.
    int layer,			// SRFC or SOIL
    double Navailable,   //available N, either in first 20 or in XN soil layer
    //for direct adsorbtion
    double *newmetabC,      //return values: new C,N of struc and metab pool,
    double *newmetabN,
    double oldstrucC,
    double *newstrucC,
    double *newstrucN,
    double oldlig,
    double *newlig,		//               adjusted lignin content of structural pool
    struct_fixedparameter fixed)
{
	double ratioCtotalN; // C/N ratio after direct adsorption
	double fractionN; //  fraction of nitrogen in residue after direct adsorption.
	double rlnres;	//Lignin/nitrogen ratio of residue
	double frmet;  //fractio of C that goes to metabolic
	double caddm, cadds; //total C added to metabolic/structural
	double  amtadded;
	double fligst;		// fligst is the lig. fraction of incoming structural residue
	double N=C*recres; //N content in residue
	double directadsorbedN=0.0; //amount of Mineral N from mineral pool that is adsorbed by the residue
	//return value
	double dummy; //debugger
	if (C >= 10e-7)
		{
			// Direct absorption of mineral element by residue
			// (mineral will be transferred to donor compartment and then
			// partitioned into structural and metabolic using flow routines.)
			// If minerl(SRFC,e) is negative then directAbsorbE = zero.

			if (Navailable < 0.0) directadsorbedN=0.0;
			else
				{
					directadsorbedN=fixed.damr[layer]*Navailable*(double)MIN(1.0, C/fixed.pabres); //Change  it to min?? In Daycent: max makes no sense
				}

			// If C/E ratio is too low, transfer just enough to make
			// C/E of residue = damrmn
			if (N+directadsorbedN <= 0.0) ratioCtotalN=0.0;
			else ratioCtotalN=C/(N+directadsorbedN);
			if(ratioCtotalN < fixed.damrmn)
				{
					directadsorbedN = C/fixed.damrmn - N;
				}
			directadsorbedN= MAX(directadsorbedN,0.0);

			//Partition carbon into structural and metabolic fraction of residue
			fractionN= (N+directadsorbedN) / (C*2.5);

			//Carbon added to metabolic
			rlnres=frlig/fractionN;		// Lignin/nitrogen ratio of residue
			frmet= fixed.spl[0] + fixed.spl[1]*rlnres;//fractio of C that goes to metabolic
			// Make sure the fraction of residue which is lignin isn't
			// greater than the fraction which goes to structural.
			frmet=MIN(frmet, 1.0-frlig);
			frmet=MAX(frmet, 0.2); // at least 20% goes to metabolic
			//Compute amounts to flow
			caddm=MAX(0.0,C*frmet); // C added to metabolic
			cadds= C - caddm; //C added to structural
			// Adjust lignin content of structural.
			// fligst is the fraction of incoming structural residue
			// which is lignin;
			// Changed maximum fraction from .6 to 1.0  -lh 1/93
			fligst=(double)MIN(frlig*C/cadds, 1.0);
			//ligninFraction = (oldlig + newlig) / (oldC + addC);
			*newlig= (oldlig*oldstrucC + fligst*cadds ) /
			         (oldstrucC + cadds);
			dummy=*newlig;

			// C flow to structural / metabolic [layer]
			*newstrucC = cadds;
			*newmetabC = caddm;

			//N flow to struc/metab
			//structural
			// 14.10.2008 SB: insert min() to avoid amtadded>N
			//amtadded=cadds/fixed.rcestr;
			amtadded=MIN( (N+directadsorbedN), cadds/fixed.rcestr);
			*newstrucN =  amtadded;
			//metabolical
			amtadded=N + directadsorbedN - amtadded ;
			*newmetabN =  amtadded;
		}// if C is significant

	return directadsorbedN;
}

//sum all available mineral N=Nitrate+Ammonium to depth
// return g/m2

//	AboveGroundDecompRatio
// 	Calculates the aboveground decomposition ratio.
// 	Determine C/N of new material entering 'Box B'.
//
// 	The C/N ratios for structural and wood can be computed once;
// 	they then remain fixed throughout the run.  The ratios for
// 	metabolic and som1 may vary and must be recomputed each time step.
// 	Returns the C/N ratio of new N;  = function of the N content of the
// 	material being decomposed.
double century_n_AboveGroundDecompRatio (century_n *self,double totalCinA ,double totalNinA,double biofac,//amount of C and N in box A
                               //biomass factor: 2.0 for wood, 2.5 else
                               struct_fixedparameter fixed)
{
	double ratio;
	double cemicb;
	double const biomass = totalCinA * biofac;	// biomass factor=2.0 for wood
	double const econt =				// E/C ratio in box A
	    biomass <= 1.0e-10 ?	0.0 : totalNinA / biomass;
	//		TDecomposition.cpp line 173f:
// cemicb = slope of the regression line for C/E of som1
//    where pcemicMax = maximum C/E of new som1
//      pcemicMin = minimum C/E of new som1
//      pcemic3 = minimum E content of decomposing
//                material that gives minimum C/E   //    pcemic is a fixed parameter
	cemicb =( fixed.pcemic[1] - fixed.pcemic[0]) / fixed.pcemic[2];
	ratio  = ( econt > fixed.pcemic[2] ? fixed.pcemic[1] : fixed.pcemic[0] + econt * cemicb);
	return ratio;
}

double century_n_FlowCintoSOM2(century_n *self,double Ctoadd,			// amount of C to add (gN/m^2)
                     double depth				// soil depth to receive mineral E (cm)
                     )
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20190717

	double d=0.0,toadd=0.0;
	int i;

	for(i=0 ; pSL->pNext != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext)
		{
			d += pSL->fThickness *0.1;
			if (d <= depth)
				{
					toadd = Ctoadd *  (pSL->fThickness*0.1)/depth;
					pCL->fCHumusSlow += toadd * gpm2TOkgpha; //SOM2
					pSL->fCHumus += toadd * gpm2TOkgpha; //Hong for C balance
					
					pCB->dCInputProfile += toadd * gpm2TOkgpha; //Hong on 20190717: new input from surface pools into profile pools
					if (d <= 30.0)
						pCB->dCInputProfile_30 += toadd * gpm2TOkgpha; 
					
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < (depth-10e-7)))  //fractional layer
				{
					toadd=Ctoadd  * ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fCHumusSlow += toadd * gpm2TOkgpha;
					pSL->fCHumus += toadd * gpm2TOkgpha; //Hong for C balance
					
					pCB->dCInputProfile += toadd * gpm2TOkgpha; //Hong on 20190717: new input from surface pools into profile pools
					if (d <= 30.0)
						pCB->dCInputProfile_30 += toadd * gpm2TOkgpha; 
				}
		}
	return Ctoadd;
}

double century_n_FlowNintoSOM2(century_n *self,double Ntoadd,			// amount of organic N to add (gN/m^2)
                     double depth				// soil depth to receive mineral E (cm)
                     )
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
	double d=0.0,toadd=0.0;
	int i;

	for(i=0 ; pSL->pNext != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext )
		{
			d += pSL->fThickness *0.1;
			if (d <= depth)
				{
					toadd = Ntoadd *  (pSL->fThickness*0.1)/depth;
					pCL->fNHumusSlow += toadd * gpm2TOkgpha; //SOM2
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < (depth-10e-7)))  //fractional layer
				{
					toadd=Ntoadd *  ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fNHumusSlow += toadd * gpm2TOkgpha;
				}
		}
	return Ntoadd;
}
double century_n_FlowCintoSOM1(century_n *self,double Ctoadd,			// amount of C to add (gN/m^2)
                     double depth				// soil depth to receive mineral E (cm)
                     )
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20190717
	
	double d=0.0,toadd=0.0;
	int i;

	for(i=0 ; pSL->pNext != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext)
		{
			d += pSL->fThickness *0.1;
			if (d <= depth)
				{
					toadd = Ctoadd *  (pSL->fThickness*0.1)/depth;
					pCL->fCHumusFast += toadd * gpm2TOkgpha; //SOM2
					pSL->fCHumus += toadd * gpm2TOkgpha;  //Hong for C balance
					
					pCB->dCInputProfile += toadd * gpm2TOkgpha; //Hong on 20190717: new input from surface pools into profile pools
					if (d <= 30.0)
						pCB->dCInputProfile_30 += toadd * gpm2TOkgpha; 
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < (depth-10e-7)))  //fractional layer
				{
					toadd=Ctoadd  * ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fCHumusFast += toadd * gpm2TOkgpha;
					pSL->fCHumus += toadd * gpm2TOkgpha;  //Hong for C balance
					
					pCB->dCInputProfile += toadd * gpm2TOkgpha; //Hong on 20190717: new input from surface pools into profile pools
					if (d <= 30.0)
						pCB->dCInputProfile_30 += toadd * gpm2TOkgpha; 
				}
		}
	return Ctoadd;
}

double century_n_FlowNintoSOM1(century_n *self,double Ntoadd,			// amount of organic N to add (gN/m^2)
                     double depth				// soil depth to receive mineral E (cm)
                     )
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
	double d=0.0,toadd=0.0;
	int i;

	for(i=0 ; pSL->pNext != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext)
		{
			d += pSL->fThickness *0.1;
			if (d <= depth)
				{
					toadd = Ntoadd *  (pSL->fThickness*0.1)/depth;
					pCL->fNHumusFast += toadd * gpm2TOkgpha; //SOM2
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < (depth-10e-7)))  //fractional layer
				{
					toadd=Ntoadd  * ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fNHumusFast += toadd * gpm2TOkgpha;
				}
		}
	return Ntoadd;
}

double century_n_CO2intoSOIL(century_n *self,double CO2toadd,			// amount of organic N to add (gN/m^2)
                   double depth				// soil depth to receive mineral E (cm)
                   )
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
	PCBALANCE	    pCB = xpn->pCh->pCBalance; //Added by Hong on 20190717
	PCPROFILE	pCP = xpn->pCh->pCProfile;
	double d=0.0,toadd=0.0;
	int i;

	for(i=0 ; pSL->pNext != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext )
		{
			d += pSL->fThickness *0.1; //cm
			if (d <= depth)
				{
					toadd = CO2toadd *  (pSL->fThickness*0.1)/depth;
					pCL->fCO2C_dt		+= toadd * gpm2TOkgpha; // [kg/ha]
					pCL->fCO2ProdR	+= toadd * gpm2TOkgpha; // [kg/ha]
					//Added by Hong on 20190717 for C balance
					pCP->dCO2SurfEmisCum += toadd * gpm2TOkgpha;
					pCB->dCInputProfile += toadd * gpm2TOkgpha; //treat CO2 from surface pools as input
					if (d <= 30.0)
						pCB->dCInputProfile_30 += toadd * gpm2TOkgpha; 
					//End of Hong	
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < (depth-10e-7)))  //fractional layer
				{
					toadd=CO2toadd  * ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fCO2C_dt		+= toadd * gpm2TOkgpha;// [kg/ha]
					pCL->fCO2ProdR	+= toadd * gpm2TOkgpha; // [kg/ha]
					
					pCP->dCO2SurfEmisCum += toadd * gpm2TOkgpha;//Added by Hong on 20190717 for C balance					
					pCB->dCInputProfile += toadd * gpm2TOkgpha; 
					if (d <= 30.0)
						pCB->dCInputProfile_30 += toadd * gpm2TOkgpha; 
				}
		}
	return CO2toadd;
}

//Following funktions are for debugging purpose
//it returns the total mineral N in TMP Clayers [kg/ha]
double century_n_getN(century_n *self,PCLAYER  pCL)
{
	double N=0;
	for(pCL=pCL->pNext; pCL->pNext != NULL ; pCL=pCL->pNext)
		{
			N += pCL->fNO3N + pCL->fNH4N;
		}
	return N;
}

double century_n_getNH4(century_n *self,PCLAYER  pCL)
{
	double N=0;
	for(pCL=pCL->pNext; pCL->pNext != NULL ; pCL=pCL->pNext)
		{
			N +=  pCL->fNH4N;
		}
	return N;
}

double century_n_getNO3(century_n *self,PCLAYER  pCL)
{
	double N=0;
	for(pCL=pCL->pNext; pCL->pNext != NULL ; pCL=pCL->pNext)
		{
			N += pCL->fNO3N  ;
		}
	return N;
}

double century_n_getNtoDepth (century_n *self,double depth)
{
// Calculate N amount to depth [g/m2]
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer;// used to iterate over the original list
	PCLAYER  pCH=pCh->pCLayer;
	double N=0.0; //return value
	double d=0.0;
	for(pSL=pSL->pNext,pCH=pCH->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext,pCH=pCH->pNext) // exclude virtual layer at top and bottom
		{
			d+= pSL->fThickness*0.1;
			if (d <= depth)
				{
					N += pCH->fNO3N
					     + pCH->fNH4N;
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < depth))
				{
					N += pCH->fNO3N   *(pSL->fThickness * 0.1-(d-depth))/(pSL->fThickness*0.1)
					     + pCH->fNH4N *(pSL->fThickness * 0.1-(d-depth))/(pSL->fThickness*0.1);
				}
		}
	N *= kgphaTOgpm2;
	return N;
}

double century_n_FlowNfromMineralSoil(century_n *self,double Ndemand,		// amount of mineral N wanted (gE/m^2)
                            double depth,			// soil depth to give mineral E (cm)
                            double useNitrateFrac	// max fraction removed from NOx pools range = (0-1),
                            // -1 = remove in proportion to nitrate/ammonium available
                            )

{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext;// XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;

	double d=0.0,removed=0.0,totalNremoved=0.0;
	int i;
	double nitrateFraction, ammoniumFraction;

	//	FlowNFromSoil
	//	Schedule a flow for a mineral element from the top simDepth of soil.
	//	Demand is limited by availability in the total soil.
	//	so no layer is completely depleted.
	//	Return the total amount of flow scheduled.
	if( century_n_getNtoDepth(self,depth)< 0.0) return 0;

	Ndemand= MIN(Ndemand, century_n_getNtoDepth (self,depth));
	for(i= 0; pSL != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext )
		{
			// nitrate and ammonium fractions
			if(pCL->fNO3N <= 0.0 && pCL->fNH4N <= 0.0)
				{
					totalNremoved = 0.0;
					break;
				}
			else
				{
					if (useNitrateFrac < 0.0)
						{
							nitrateFraction = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N);
							ammoniumFraction =1.0 - nitrateFraction;
						}
					else   // 0.0 <= useNitrateFrac <= 1.0;
						{
							nitrateFraction = MIN( useNitrateFrac,
							                       pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N) );
							ammoniumFraction = 1.0 - nitrateFraction;
						}

					if(pCL->fNO3N <=0.0)
						{
							nitrateFraction=0.0;
							ammoniumFraction=1.0;
						}
					if(pCL->fNH4N <=0.0)
						{
							nitrateFraction=1.0;
							ammoniumFraction=0.0;
						}
					//Flow from Soil
					d+= pSL->fThickness*0.1;
					if (d <= depth)	//full layer
						{
							//nitrate
							if(pCL->fNO3N > 0.0)
								{
									removed=Ndemand * nitrateFraction * (pSL->fThickness*0.1)/depth;
									removed=MIN(removed, (pCL->fNO3N*kgphaTOgpm2));
									pCL->fNO3N -= removed * gpm2TOkgpha;
									pCL->fNImmobR += removed * gpm2TOkgpha;

									totalNremoved +=removed;
								}
							//ammonium
							if(pCL->fNH4N > 0.0)
								{
									removed=Ndemand * ammoniumFraction * (pSL->fThickness*0.1)/depth;
									removed=MIN(removed, (pCL->fNH4N*kgphaTOgpm2));
									pCL->fNH4N -= removed * gpm2TOkgpha;
									pCL->fNImmobR += removed * gpm2TOkgpha;

									totalNremoved +=removed;
								}
						}
					if (d > depth && ((d-pSL->fThickness*0.1) < depth))  //fractional layer
						{
							//nitrate
							if(pCL->fNO3N > 0.0)
								{
									removed=Ndemand * nitrateFraction * ((pSL->fThickness*0.1)-(d-depth))/depth;
									//removed=min(removed, (pCL->fNO3N*kgphaTOgpm2));
									pCL->fNO3N -= removed * gpm2TOkgpha;
									pCL->fNImmobR += removed * gpm2TOkgpha;

									totalNremoved +=removed;
								}
							//ammonium
							if(pCL->fNH4N > 0.0)
								{
									removed=Ndemand * ammoniumFraction * ((pSL->fThickness*0.1)-(d-depth))/depth;
									removed=MIN(removed, (pCL->fNH4N*kgphaTOgpm2));
									pCL->fNH4N -= removed * gpm2TOkgpha;
									pCL->fNImmobR += removed * gpm2TOkgpha;

									totalNremoved +=removed;
								}
						}
					if(pCL->fNO3N <0.0) pCL->fNO3N = 0.0;//numerical issue
					if(pCL->fNH4N <0.0) pCL->fNH4N = 0.0;//numerical issue
				}//there is some mineral N at all
		}//loop over layer

	return totalNremoved;
}
double century_n_FlowNintoMineralSoil(century_n *self,double Ntoadd,			// amount of mineral N to add (gN/m^2)
                            double depth,				// soil depth to receive mineral E (cm)
                            double fractionToNitrate	// fraction of N to go to Nitrate range = (0-1)
                            )

{
expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;

	double d=0.0,toadd=0.0,debug;
	int i;

	for(i=0 ; pSL->pNext != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext )
		{
			d += pSL->fThickness *0.1;
			if (d <= depth)
				{
					//nitrate
					toadd=Ntoadd * fractionToNitrate * (pSL->fThickness*0.1)/depth;
					pCL->fNO3N += toadd * gpm2TOkgpha;
					pCL->fMinerR += toadd * gpm2TOkgpha;
					//ammonium
					toadd=Ntoadd * (1.0-fractionToNitrate) * (pSL->fThickness*0.1)/depth;
					pCL->fNH4N += toadd * gpm2TOkgpha;
					pCL->fMinerR += toadd * gpm2TOkgpha;
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < (depth-10e-7)))  //fractional layer
				{
					//nitrate
					debug=4;
					toadd=Ntoadd * fractionToNitrate * ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fNO3N += toadd * gpm2TOkgpha;
					pCL->fMinerR += toadd * gpm2TOkgpha;
					//ammonium
					toadd=Ntoadd * (1.0-fractionToNitrate) * ((pSL->fThickness*0.1)-(d-depth))/depth;
					pCL->fNH4N += toadd * gpm2TOkgpha;
					pCL->fMinerR += toadd * gpm2TOkgpha;
				}
		}

	return Ntoadd;
}


/* Calculates if Immobilization or Mineralization occurs and calculates flows
  from A to B and from/to the mineral pool.

*/
void century_n_ScheduleNFlow(century_n *self,
    double CFlowAtoB,
    double FlowRatioCtoN,
    double AC,double AN,	//C and N content of Box A (source)
    double availMinN,	//available mineral N for possible Immobilisation
    //return values
    double *orgNflow,
    double *minNflow		//negative value for immobilisation
)
{
	double const NFlowFromA=CFlowAtoB * AN/AC; //proportional zum C Fluß, abhängig vom
	//C/N Verhältniss in A																	//!!i.A.: nicht alles kommt in B an
//Immobilization
	if(CFlowAtoB/NFlowFromA > FlowRatioCtoN)
		{
			// organic N flow from A to B
			*orgNflow = NFlowFromA;
			//calculate Immobilization
			*minNflow = -(double)MIN( availMinN, CFlowAtoB/FlowRatioCtoN - NFlowFromA);//neg value for immobilization
			//Schedule flow from mineral pool to pool B (immobilization flow)
		}
//Mineralization
	if(CFlowAtoB/NFlowFromA <= FlowRatioCtoN)
		{
			// N flow from A to B
			*orgNflow = CFlowAtoB/FlowRatioCtoN;
			//Schedule flow from pool A to mineral pool
			*minNflow = NFlowFromA - CFlowAtoB/FlowRatioCtoN; //pos. value for mineralization
		}

}

int century_n_CanDecompose (century_n *self,
    double const C,		// total C in Box A
    double const N,		//   N in Box A
    double const ratio, //   C/N,  ratios of new material being added to Box B
    double availMinN)
{
	int canDo = 1;	// flag for N = true
	// If there is no available mineral E
	if(N>=0)
		{
			if ( availMinN <=10e-5 )
				{
					// if C/E of Box A > C/E of new material
					if (C / N > ratio)
						{
							// Immobilization is necessary therefore
							// Box A cannot decompose to Box B.
							canDo = 0; //false
						}
				}
		}
	if (N<=0) canDo=0; //false

	return ( canDo );
}

//returns percentage of sand content
double century_n_getSandtoDepth(century_n *self,double depth)
{
	expertn_modul_base *xpn = &(self->parent);
	PSPROFILE pSo = xpn->pSo;
	
	double sand=0.0,weightingFactorFullLayer=0.0; //return value
	double d=0.0;
	PSLAYER  pSL=pSo->pSLayer; // used to iterate over the original list

	for(pSL=pSL->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext)   // exclude virtual layer at top and bottom
		{
			d+= pSL->fThickness*0.1;
			weightingFactorFullLayer=pSL->fThickness * 0.1 /depth;
			if (d <= depth)
				{
					sand += pSL->fSand * weightingFactorFullLayer;
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < depth))
				{
					sand +=  pSL->fSand *
					         (pSL->fThickness * 0.1-(d-depth))/depth ;
				}
		}
	return sand;
}

double century_n_getWFPStoDepth(century_n *self,double depth)
{
	expertn_modul_base *xpn = &(self->parent);
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	double WFPS=0.0,weightingFactorFullLayer=0.0;
	double d=0.0;
	PSLAYER  pSL=pSo->pSLayer; // used to iterate over the original list
	PWLAYER  pWA=pWa->pWLayer;
	for(pSL=pSL->pNext,pWA=pWA->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext,pWA=pWA->pNext)   // exclude virtual layer at top and bottom
		{
			weightingFactorFullLayer=pSL->fThickness * 0.1 /depth;

			d += pSL->fThickness*0.1; //[cm]
			if (d <= depth)
				{
					WFPS += pWA->fContAct / pSo->pSLayer->fPorosity  * weightingFactorFullLayer;
					//	use direct input value: pSo->pSLayer->fPorosity.
					//Daycent: porosity=1-bulk density/2.65
				}
			if (d > depth && ((d-pSL->fThickness*0.1) < depth)) //Fractional Layer
				{
					WFPS +=  pWA->fContAct / pSo->pSLayer->fPorosity *
					         (pSL->fThickness * 0.1-(d-depth))/depth ;
				}
		}
// if (WFPS <0.0 || WFPS >1.0) ExpertN Warnung ausgeben
	return WFPS; //0..1
}

// getH2OtoDepth
// depth= -1 : return water content of all layers
double century_n_getH2OtoDepth (century_n *self,double depth)
{
// Calculate H2O amount to root depth
	expertn_modul_base *xpn = &(self->parent);
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	PWLAYER  pWL=pWa->pWLayer;// used to iterate over the original list
	PSLAYER  pSL=pSo->pSLayer;// used to iterate over the original list
	double h2o=0.0; //return value
	double d=0.0;
	for( pWL = pWL->pNext, pSL=pSL->pNext;
	        pWL->pNext != NULL && pSL->pNext != NULL ;  pWL = pWL->pNext, pSL=pSL->pNext)  // exclude virtual layer at top and bottom
		{
			if (depth <0.0)
				{
					h2o += pWL->fContAct * pSL->fThickness * 0.1;

				}
			else //depth > 0
				{
					d+= pSL->fThickness*0.1;
					if (d <= depth) //full layer
						{
							h2o += pWL->fContAct * pSL->fThickness * 0.1; // H20 [cm]
						}
					if (d > depth && ((d-pSL->fThickness*0.1) < depth)) //fractional layer
						{
							h2o += pWL->fContAct *  (pSL->fThickness * 0.1-(d-depth))/depth ; // H20 [cm];
						}
				}
		}
	return h2o; //[cm]
}
