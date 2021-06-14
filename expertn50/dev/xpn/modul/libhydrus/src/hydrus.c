//      hydrus.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "hydrus.h"
#include "hydrus_macros.h"
#include "math_functions.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


G_DEFINE_TYPE(hydrus, hydrus, EXPERTN_MODUL_BASE_TYPE);

double _hydrus_check_0(double x,double y,int line,expertn_modul_base *xpn)
{
	char *ERROR_TIME_STR;
	if (x==0)
		{
			ERROR_TIME_STR = xpn_time_get_formated_date_and_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);
			fprintf(stderr,"ERROR (%s): %s: %s\t%d\t%s\n",ERROR_TIME_STR,xpn->pXSys->reg_str,__FILE__,line,"max problem");
			fflush(stderr);
			g_free(ERROR_TIME_STR);
			return y;
		}
	return x;
}

#define hydrus_check_0(x,y) _hydrus_check_0(x,y,__LINE__,xpn)

static void hydrus_class_init(hydrusClass *klass) {}
static void hydrus_init(hydrus *self)
{
	self->iBotBC = 1;
	self->mob_imm = 0;
	self->use_infiltration_limit=TRUE;
	self->use_infiltration_limit_layer=TRUE;
	self->WCont=NULL;
	self->HCond=NULL;
	self->DWCap=NULL;
	self->MPotl=NULL;
	self->__ERROR=0;
}



// Our Modul Functions:


int hydrus_water_flow_init(hydrus *self)
{
	expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
	PTIME pTi = xpn->pTi;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;


	double DeltaT;
	int NSoil;
	PWLAYER pWL;
	PSLAYER pSL;
	PSWATER pSW;
	int i;

	//PWBALANCE   pWB;

	int iLayer;


	// Alle Wassvariablen auf 0. Schicht:
	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
	//pWB = pWa->pWBalance;

	iLayer = 0;


	// Boundary Condition aus der INI Datei lesen (falls undefiniert --> 1)
	self->iBotBC = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.hydrus.bottombc",1);
	// Mobil Immobil (Standartmäßig aus = 0)
	self->mob_imm = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.hydrus.mobil",0);

	self->use_infiltration_limit=xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.hydrus.infiltration_limit",TRUE);;
	self->use_infiltration_limit_layer=xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.hydrus.infiltration_layer_limit",TRUE);

	// Hydraulische Funktionen laden:
	self->WCont = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.WCont");
	self->HCond = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.HCond");
	self->DWCap = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.DWCap");
	self->MPotl = xpn_register_var_get_pointer(xpn->pXSys->var_list,"hydraulic_fuctions.MPotl");
	if ((self->WCont==NULL) || (self->HCond==NULL) || (self->DWCap==NULL) || (self->MPotl==NULL))
		{
			PRINT_ERROR_ID(xpn,"Problem with hydraulic functions!");
		}

	// Anfangswerte fpr Matrixpotential:
	for (pWL=xpn->pWa->pWLayer,pSL = xpn->pSo->pSLayer,pSW=xpn->pSo->pSWater; pWL!=NULL; pWL=pWL->pNext,pSL=pSL->pNext,pSW=pSW->pNext)
		{
			pWL->fMatPotAct = MATRIX_POTENTIAL(pWL->fContAct);
			pWL->fMatPotOld = pWL->fMatPotAct;
		}


	// Arrays (NSoil) mit Speicher belegen und auf 0 intitialisieren:

	NSoil = self->parent.pSo->iLayers;

	self->afTS = (double*)calloc (NSoil,sizeof(double));
	self->afTR = (double*)calloc (NSoil,sizeof(double));

	self->ThN = (double*)calloc (NSoil,sizeof(double));
	self->ThO = (double*)calloc (NSoil,sizeof(double));
	self->hNew = (double*)calloc (NSoil,sizeof(double));
	self->hOld = (double*)calloc (NSoil,sizeof(double));

	self->Sink = (double*)calloc (NSoil,sizeof(double));

	self->ThR = (double*)calloc (NSoil,sizeof(double));
	self->ThS = (double*)calloc (NSoil,sizeof(double));
	self->hSat = (double*)calloc (NSoil,sizeof(double));
	self->ConSat = (double*)calloc (NSoil,sizeof(double));
	self->Con = (double*)calloc (NSoil,sizeof(double));
	self->ConO = (double*)calloc (NSoil,sizeof(double));
	self->Cap = (double*)calloc (NSoil,sizeof(double));
	self->hTemp = (double*)calloc (NSoil,sizeof(double));

	self->v = (double*)calloc (NSoil,sizeof(double));
	
	self->vFlux = (double*)calloc (NSoil,sizeof(double));

	self->P = (double*)calloc (NSoil,sizeof(double));
	self->R = (double*)calloc (NSoil,sizeof(double));
	self->S = (double*)calloc (NSoil,sizeof(double));

	// NTabD Arrays:

	self->NTabD = 250;

	self->hTab = (double*)calloc (self->NTabD,sizeof(double));

	self->ConTab = (double**) calloc (self->NTabD,sizeof(double*));
	self->CapTab = (double**) calloc (self->NTabD,sizeof(double*));
	self->STab =   (double**) calloc (self->NTabD,sizeof(double*));

	for (i=0; i<self->NTabD; i++)
		{
			self->ConTab[i] = (double*)calloc (NSoil,sizeof(double));
			self->CapTab[i] = (double*)calloc (NSoil,sizeof(double));
			self->STab[i] =  (double*)calloc (NSoil,sizeof(double));
		}




	// Variablen Initialisieren:

	self->dCosAlf=(double)1;
	self->fCapillRiseR = (double)0;

	// dt:
	DeltaT=pTi->pTimeStep->fAct;


	//*********** top boundary condition ****************************************************************
	self->AtmBC =TRUE;//TRUE if variable upper boundary conditions given by atmospheric conditions

	self->KodTop=(double)-1;//positive for Dirichlet, negative for Neumann bc
	self->TopInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3
	self->WLayer=FALSE;//TRUE if water can accumulate at the soil surface without run off, then KodTop negative

	//*********  bottom boundary condition **************************************************************

	switch(self->iBotBC)
		{
		// free drainage
		case 1 :
			self->KodBot=(double)-1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(double)3;//-5;
			self->BotInF=FALSE;//TRUE if time dependent bc, then KodTop= +3 or -3
			self->FreeD=TRUE;  //TRUE if free drainage is considered as lower bc, then KodBot=-5
			self->SeepF=FALSE; //TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
			self->qGWLF=FALSE; //TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
			break;
		// constant ground water level
		case 2 :
			self->KodBot=(double)+1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(double)3;//-5;
			self->BotInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3
			self->FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
			self->SeepF=FALSE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
			self->qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
			break;
		// no flux
		case 3 :
			self->KodBot=(double)-1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(double)3;//-5;
			self->BotInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3
			self->FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
			self->SeepF=FALSE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
			self->qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
			break;
		// lysimeter
		case 4 :
			self->KodBot=(double)-1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(double)3;//-5;
			self->BotInF=FALSE;//TRUE if time dependent bc, then KodTop= +3 or -3
			self->FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
			self->SeepF=TRUE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
			self->qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
			break;
		// dynamic ground water table
		case 5 :
			self->KodBot=(double)+1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(double)3;//-5;
			self->BotInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3
			self->FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
			self->SeepF=FALSE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
			self->qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
			break;
		// sonst
		default: // sollte Nie vorkommen --> Error:
			PRINT_ERROR_ID(xpn,"No Boundary Condition");
			break;
		}


	//******************** input modification boundary conditions ***************************************
	if (self->TopInF) self->KodTop=hydrus_signum((double)3,self->KodTop);//positive for Dirichlet, negative for Neumann bc
	if (self->AtmBC)  self->KodTop=(double)-4;
	if (self->WLayer) self->KodTop=(double)fabs((double)self->KodTop);
	if (self->BotInF) self->KodBot=hydrus_signum((double)3,self->KodBot);//positive for Dirichlet, negative for Neumann bc
	if (self->qGWLF)  self->KodBot=(double)-7;
	if (self->FreeD)  self->KodBot=(double)-5;
	if (self->SeepF)  self->KodBot=(double)-2;
	self->KTOld=self->KodTop;
	self->KBOld=self->KodBot;

	//******************** input time control information ***************************************
	self->ItMin=3;
	self->ItMax=20;
	self->ItCum=0;
	self->dMul1=(double)1.2;
	self->dMul2=(double)0.8;
	self->dtMin=(double)1.e-6;
	self->dtMax=(double).5;
	self->dtMaxW=(double).5;
	self->dtMaxC=(double).5;
	self->dtInit=(double)5.e-2;
	self->dtOpt=(double)0.01;
	self->rMin=(double)1.e-32;
	self->rMax=(double)1.e+10;

	self->tOld=(double)0;//SimTime;
	self->tNew=DeltaT;//SimTime+DeltaT;
	self->DeltaTOld=DeltaT;
	self->dtOld=self->dt=(double)max((double)DeltaT/(double)3,(double)1.e-8);
	self->t=self->tOld+self->dt;//SimTime+dt;
	self->tMax=(double)1;//(double)(pTi->pSimTime->iSimDuration+(int)1);

	//******************* generate hydraulic function tables **********************************
	//GenMat(exp_p,self->ConTab,self->CapTab,self->STab);
	hydrus_water_flow_genMat(self, self->ConTab,self->CapTab,self->STab);

	//******************* initialize matric potentials and water contents *********************
	for (pWL=pWa->pWLayer->pNext,iLayer=1; pWL!=NULL; pWL=pWL->pNext,iLayer++)
		{
			self->hNew[iLayer] = pWL->fMatPotAct;
			self->hOld[iLayer] = pWL->fMatPotOld;

			self->ThN[iLayer]  = pWL->fContAct;
			self->ThO[iLayer]  = pWL->fContOld;
			
			CHECK_VALID(self->ThN[iLayer])
			CHECK_VALID(self->ThO[iLayer])
			CHECK_VALID(self->hNew[iLayer])
		}	// end for


	switch(self->mob_imm)
		{
		case 1:
			for (H2O_ALL_BUT_NOT_FIRST)
				{
					pWL->fContMobAct=pWL->fContAct*pSW->fContMobSat/self->afTS[iLayer];//pSW->fContSat;
					pWL->fContMobOld=pWL->fContMobAct;
					pWL->fContImmAct=pWL->fContAct*pSW->fContImmSat/self->afTS[iLayer];//pSW->fContSat;
					pWL->fContImmOld=pWL->fContImmAct;
					self->ThN[iLayer]= pWL->fContMobAct;
					self->ThO[iLayer]= pWL->fContMobOld;
				}
			break;
		default:
			for (H2O_ALL_BUT_NOT_FIRST)
				{
					pWL->fContMobAct=pWL->fContMobOld=pWL->fContImmAct=pWL->fContImmOld=(double)0;
				}
			break;

		}
	//hydrus_water_flow_run(self);

	self->cumRain=0.0;
	self->cumRainDay=0.0;
	self->cumRainDay_=0.0;

	xpn->pWa->FreeD = self->FreeD;
	xpn->pWa->iMobImm = self->mob_imm;
	xpn->pWa->iBotBC = self->iBotBC;



	return RET_SUCCESS;
}


int hydrus_water_flow_done(hydrus *self)
{
	int i;

	free_ifn0(self->afTS);
	free_ifn0(self->afTR);

	free_ifn0(self->ThN);
	free_ifn0(self->ThO);
	free_ifn0(self->hNew);
	free_ifn0(self->hOld);

	free_ifn0(self->Sink);

	free_ifn0(self->ThR);
	free_ifn0(self->ThS);
	free_ifn0(self->hSat);
	free_ifn0(self->ConSat);
	free_ifn0(self->Con);
	free_ifn0(self->ConO);
	free_ifn0(self->Cap);
	free_ifn0(self->hTemp);

	free_ifn0(self->v);
	free_ifn0(self->vFlux);

	free_ifn0(self->P);
	free_ifn0(self->R);
	free_ifn0(self->S);

	for (i=0; i<self->NTabD; i++)
		{
			free_ifn0(self->ConTab[i]);
			free_ifn0(self->CapTab[i]);
			free_ifn0(self->STab[i]);
		}

	free_ifn0(self->ConTab);
	free_ifn0(self->CapTab);
	free_ifn0(self->STab);

	free_ifn0(self->hTab);


	self->NTabD = 0;

	return RET_SUCCESS;
}


int hydrus_water_flow_run(hydrus *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	PPLANT pPl = xpn->pPl;


	int iLayer        = 0;
	double DeltaT     = pTi->pTimeStep->fAct;

	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;
//	PWBALANCE   pWB;




	int   N=pSo->iLayers-1;
	double f1;

	double fTheta,fSink,fECDT;
	double fPotIm;//,fEpsZD;
	double DeltaZ;

	BOOL ItCrF; // True: Konvergiert, False: Konvergiert nicht


	PEVAP pEV = pWa->pEvap;

	// Alle Wassvariablen auf 0. Schicht:
	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
	//pWB = pWa->pWBalance;

	iLayer = 0;

	if (self->use_infiltration_limit_layer==TRUE)
		{
			hydrus_rain_limit_infiltration_layer(self);
		}



	if (self->use_infiltration_limit>0)
		{
			hydrus_rain_limit_infiltration(self);
		}





	{
		double fLiquPreciRate;
		double fAbsfCont1;
		//	double smcmax;




		fLiquPreciRate = xpn->pCl->pWeather->fLiquPreciRate;
		fAbsfCont1=xpn->pWa->pWLayer->pNext->fContAct*xpn->pSo->pSLayer->pNext->fThickness;

		//C_DEBUG(fLiquPreciRate);
		//C_DEBUG(fAbsfCont1);
		//C_DEBUG(xpn->pWa->fInfiltR);
//		smcmax = xpn->pSo->pSLayer->pNext->fPorosity;








		self->cumRain += xpn->pCl->pWeather->fLiquPreciRate*xpn->pTi->pTimeStep->fAct;
		self->cumRainDay_ += xpn->pCl->pWeather->fLiquPreciRate*xpn->pTi->pTimeStep->fAct;
		if (NewDay(xpn->pTi))
			{
				self->cumRainDay=self->cumRainDay_;
				self->cumRainDay_=0.0;
			}

		//C_DEBUG(self->cumRain);
		//C_DEBUG(self->cumRainDay);

		//C_DEBUG(xpn->pCl->pWeather->fWaterPond);

	}


	// dt:
	DeltaT=pTi->pTimeStep->fAct;

	//TODO: Mächtigkeit nur für konstante Bodenschichtendicke!
	DeltaZ=pSo->fDeltaZ; // Konstante Mächtigkeit der Profilschichten


	switch (self->mob_imm)
		{
		case 1 :
			for (H2O_ALL_BUT_NOT_FIRST)
				{
					self->afTS[iLayer]=pSW->fContSat;
					self->afTR[iLayer]=pSW->fContRes;
					//pSW->fContMobSat=(double)0.25*afTS[iLayer];
					//pSW->fContImmSat=(double)0.75*afTS[iLayer];
					//pSW->fMobImmEC=(double)0.142857;//24;//1440;//24;//0.00275;
					pSW->fContSat=pSW->fContMobSat;
					pSW->fContRes=(double)0;
				}

			break;
		default:
			break;
		}



	self->tOld=(double)0;//SimTime;
	self->tNew=DeltaT;//SimTime+DeltaT;
	//dtOld=
	self->dt=(double)max((double)DeltaT/(double)3,(double)1.e-6);
	self->t=self->tOld+self->dt;//SimTime+dt;




	//***************************** defining sink term (includes root water uptake) **********************
	for (H2O_ALL_BUT_NOT_FIRST)
		{
			if (pWL->fContAct > pSL->fPorosity )
				{
					PRINT_WARNING_ID(xpn,"fContAct>fPorosity");
					pWL->fContAct = pSL->fPorosity-EPSILON;
				}
			///*
			switch(self->mob_imm)
				{
				case 0://Without mobile-immobile distinction
					// this is the sink term in Richards Equation:
					self->Sink[iLayer] =-(pWL->fContAct-WATER_CONTENT(pWL->fMatPotAct))/DeltaT;//[1/day]
					CHECK_VALID(self->Sink[iLayer])					
					break;

				case 1://mobile-immobile first order exchange

					//Sink[iLayer] =(pWL->fContAct-pWL->fContMobAct-pWL->fContImmOld)/DeltaT;//[1/day]
					fTheta = pWL->fContMobAct/pSW->fContMobSat*(pSW->fContImmSat-self->afTR[iLayer])
					         -(pWL->fContImmAct-self->afTR[iLayer]);
					fSink = pWL->fContMobAct/pWL->fContAct*
					        (pWL->fContAct-pWL->fContMobAct-pWL->fContImmAct);
					self->Sink[iLayer] =pSW->fMobImmEC*fTheta-fSink/DeltaT;
					CHECK_VALID(self->Sink[iLayer])
					//
					break;

				case 2://mobile-immobile bimodal hydraulic functions
					// this is the sink term in Richards Equation:
					self->Sink[iLayer] =-(pWL->fContAct-WATER_CONTENT(pWL->fMatPotAct))/DeltaT; // [1/day]
					CHECK_VALID(self->Sink[iLayer])
					break;
					// Kommt nicht vor (hoffentlich)
				default:
					PRINT_ERROR_ID(xpn,"mob_imm > 2");
					break;
				}//switch */
			//Sink[iLayer] =-(pWL->fContAct-WATER_CONTENT(pWL->fMatPotAct))/DeltaT;//[1/day]
		}//for




	
	// ck:
	self->vTopFlux=0.0;
	self->vBotFlux=0.0;
	{
		int i;
		for (i=0;i<self->parent.pSo->iLayers;i++)
			{
				self->vFlux[i] = 0.0;
			}
	}
    
    for (pWL=pWa->pWLayer,iLayer=0; pWL!=NULL; pWL=pWL->pNext,iLayer++)
        {
            pWL->fFluxDensOld = pWL->fFluxDens;            
        }
	
	self->Iter=0;
	self->IGoTo=0;

	while(1) //! beginning of iteration loop ---------------------------
		{
			if (self->IGoTo<2)
				{
					//Water_BC_In(exp_p);
					//SetBC(exp_p);
					hydrus_water_flow_setBC(self);
					self->Iter=0;
					/*
					if ((NewDay(pTi))&&(IGoTo<1)&&
						((double)fabs((double)rTopOld-(double)rTop)>(double)1.e-8))
						{
							MinStep=TRUE;
							dt=dtMin;
							dtMax=dtMin;(double)1.e+2;
							t=tOld+dt;
						}
					*/
				}

			if (self->IGoTo<1)
				{
					for (pWL=pWa->pWLayer->pNext,iLayer=1; pWL!=NULL; pWL=pWL->pNext,iLayer++)
						{
							self->hTemp[iLayer]=self->hNew[iLayer]+(self->hNew[iLayer]-self->hOld[iLayer])*self->dt/self->dtOld;
							self->hOld[iLayer]=self->hNew[iLayer];
							self->hNew[iLayer]=self->hTemp[iLayer];
							self->ThO[iLayer]=self->ThN[iLayer];
							CHECK_VALID(self->hNew[iLayer])
							CHECK_VALID(self->ThO[iLayer])
						}	// end for
				} //end IGoTo<1


//******* Generate terms of matrix equation and solve by Gauss elimination **********************

			//SetMat(exp_p,self->ConTab,self->CapTab,self->STab);
			hydrus_water_flow_setMat(self,self->ConTab,self->CapTab,self->STab);

			//SetEq(exp_p);
			hydrus_water_flow_setEq(self);

			//ChangeBC(exp_p);
			hydrus_water_flow_changeBC(self);

//*************************** begin iterative solution ***************************************************
			for (H2O_ALL_BUT_NOT_FIRST) //i=iLayer
				{
					self->hTemp[iLayer]=self->hNew[iLayer];
				}

			//SolveEq(exp_p);
			hydrus_water_flow_solveEq(self);

			self->Iter++;
			self->ItCum++;


//Convergency criterion
			//TestCvg(exp_p);
			ItCrF = hydrus_water_flow_testCvg(self);

//Continue iteration or redo with smaller time step
			if ((self->IGoTo<3)&&(ItCrF==FALSE)) continue;


//Next dt until t=tNew where tNew=SimTime+DeltaT
			// ck:
			self->vTopFlux+=(-(self->Con[2]+self->Con[1])/(double)2*((self->hNew[2]-self->hNew[1])/DeltaZ/(double)2-(double)self->dCosAlf)
	           +((self->ThN[1]-self->ThO[1])/self->dt+self->Sink[1])*DeltaZ)*self->dt;
			
			// orig. Hydrus:
			//self->vBotFlux+=(-(self->Con[N]+self->Con[N-1])/(double)2*((self->hNew[N]-self->hNew[N-1])/DeltaZ/(double)2-(double)self->dCosAlf)
	        //   -((self->ThN[N]-self->ThO[N])/self->dt+self->Sink[N])*DeltaZ)*self->dt;
			   
			// Eckart Priesack (verbessert Wasserbilanz)
			self->vBotFlux+=(-(self->Con[N]+self->Con[N-1])/(double)2*((self->hNew[N]-self->hNew[N-1])/DeltaZ/(double)2-(double)self->dCosAlf))*self->dt;
			
			//Veloc(pSo->iLayers-1,self->dCosAlf);
			hydrus_water_flow_veloc(self,pSo->iLayers-1,self->dCosAlf);
			
			{
				int i;
				for (i=0;i<self->parent.pSo->iLayers;i++)
					{
						self->vFlux[i] += self->v[i]*self->dt;
					}
			} 


			if (((double)fabs((double)self->t-(double)self->tNew)>(double)0.001*self->dt)&&(self->IGoTo<3))
				{
					self->tOld=self->t;
					self->dtOld=self->dt;
					self->KTOld=self->KodTop;
					self->KBOld=self->KodBot;

					//TmCont(exp_p);
					hydrus_water_flow_tmCont(self);
					self->t=self->t+self->dt;

					self->Iter=0;
					self->IGoTo=0;

					continue;
				}
			break;
		}
//**************************** evaluation based on calculated new matric potentials **********************

// root water uptake
	if (pPl != NULL)
		{
			PLAYERROOT  pLR = pPl->pRoot->pLayerRoot;

			pPl->pRoot->fUptakeR = (double)0;

			for (
			    pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,
			    pWL = pWa->pWLayer->pNext,pLR = pPl->pRoot->pLayerRoot->pNext,
			    iLayer = 1;

			    ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL)&&
			     (pLR->pNext != NULL));

			    pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,pLR = pLR->pNext,
			    iLayer++
			)

				{
					pLR->fActLayWatUpt = self->Sink[iLayer];
					///*
					switch (self->mob_imm)
						{
						case 1:
							pLR->fActLayWatUpt = -(pWL->fContAct-pWL->fContMobAct-pWL->fContImmAct)/DeltaT;
							break;
						default:
							break;
						}


					//It is assumed that H2O uptake only takes place, when soil is not freezing or thawig
					if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
						{
							pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * DeltaZ;  // [mm/day]
						}
				}
		}// end of root water uptake

	for (H2O_ALL_BUT_NOT_FIRST)
		{
			///*
			switch(self->mob_imm)
				{
				case 0:
					pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);
					pWL->fMatPotAct=self->hNew[iLayer];
					pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
					CHECK_VALID(pWL->fContAct)
					break;

				case 1:
					pWL->fContMobOld = WATER_CONTENT(pWL->fMatPotAct);
					pWL->fContOld = pWL->fContMobOld+pWL->fContImmAct;

					pWL->fMatPotAct=self->hNew[iLayer];
					pWL->fContMobAct = WATER_CONTENT(pWL->fMatPotAct);

					fTheta = pWL->fContMobAct/pSW->fContMobSat*
					         (pSW->fContImmSat-self->afTR[iLayer])+self->afTR[iLayer];
					fECDT = pSW->fMobImmEC*DeltaT/((double)1+pSW->fMobImmEC*DeltaT);
					fSink = pWL->fContImmAct/pWL->fContAct*(pWL->fContAct-pWL->fContOld);
					pWL->fContImmOld = pWL->fContImmAct;
					pWL->fContImmAct = fECDT*fTheta+(fSink+pWL->fContImmOld)
					                   /((double)1+pSW->fMobImmEC*DeltaT);

					pWL->fContAct = pWL->fContMobAct+pWL->fContImmAct;
					CHECK_VALID(pWL->fContAct)
					break;

				case 2:
					pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);
					pWL->fMatPotAct=self->hNew[iLayer];
					pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
					CHECK_VALID(pWL->fContAct)

					pWL->fContImmOld = pWL->fContImmAct;
					//fEpsZD=(double)0.0005;
					fPotIm=0.0;
					// TODO: FunKtion  fehlt:
					/*(double)ZD_PotIm((double)pWL->fMatPotAct,(double)pWL->fContAct,(double)pSW->fCondSat,
					 (double)pSW->fContSat,(double)pSW->fContRes,(double)pSW->fVanGenA,
					 (double)pSW->fVanGenN,(double)pSW->fVanGenM,(double)pSW->fCampA,
					 (double)pSW->fCampB,(double)pWL->fMatPotAct,(double)fEpsZD,(double)pSW->fMinPot,
					 (double)pWL->fMatPotOld,(double)pWL->fContOld,(double)pSW->fVanGenA2,
					 (double)pSW->fVanGenN2,(double)pSW->fVanGenM2,(double)pSW->fBiModWeight1,
					 (double)pSW->fBiModWeight2);*/

					if(fPotIm==pWL->fMatPotAct)
						{
							PRINT_ERROR_ID(xpn,"Wrong value for immobile matric potential");
						}

					pWL->fContImmAct = WATER_CONTENT(fPotIm);

					pWL->fContMobOld = pWL->fContMobAct;
					pWL->fContMobAct = pWL->fContAct-pWL->fContImmAct;

					break;
				}//switch */
			/*
			pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);
			pWL->fMatPotAct=hNew[iLayer];
			pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
			*/
			
			
		}//for
	

	//self->vTop=-(self->Con[2]+self->Con[1])/(double)2*((self->hNew[2]-self->hNew[1])/DeltaZ/(double)2-(double)self->dCosAlf)
	//           +((self->ThN[1]-self->ThO[1])/self->dt+self->Sink[1])*DeltaZ;
	
			   
	CHECK_VALID(self->vTop);
	
	/*if (self->vTop*DeltaT>1.0)
		{
			PRINT_ERROR_ID(xpn,"fdsf");
		}*/
	
	
	//self->vBot=-(self->Con[N]+self->Con[N-1])/(double)2*((self->hNew[N]-self->hNew[N-1])/DeltaZ/(double)2-(double)self->dCosAlf)
	//           -((self->ThN[N]-self->ThO[N])/self->dt+self->Sink[N])*DeltaZ;
	

//vTop=-(Con[2]+Con[1])/(double)2*((hNew[2]-hNew[1])/DeltaZ/(double)2-(double)dCosAlf);
	//self->vBot=-(self->Con[N]+self->Con[N-1])/(double)2*((self->hNew[N]-self->hNew[N-1])/DeltaZ/(double)2-(double)self->dCosAlf);
//vBot=-(Con[N-1]+Con[N-2])/(double)2*((hNew[N-1]-hNew[N-2])/DeltaZ/(double)2-(double)dCosAlf);


// ck:
			if (self->ConvgF==TRUE)
            {
                self->vTop=self->vTopFlux/DeltaT;
                self->vBot=self->vBotFlux/DeltaT;
                {
                    int i;
                    for (i=0;i<self->parent.pSo->iLayers;i++)
                        {
                            self->v[i] = self->vFlux[i]/DeltaT;
                        }
                }                
            } else
            {
                self->vTop=0.0;
                self->vBot=0.0;
                {
                    int i;
                    for (i=0;i<self->parent.pSo->iLayers;i++)
                        {
                            self->v[i] = 0.0;
                        }
                }
            }

// Water flow into/from top layer means infiltration/ evaporation (depending on sign)

	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
	iLayer = 0;

//f1=max(pCl->pWeather->fRainAmount-max(vTop,(double)0),(double)0);
//f1=max(pCl->pWeather->fRainAmount-pWa->fRunOffR-max(vTop,(double)0),(double)0);
	//f1=max(pWa->pWBalance->fReservoir-pWa->fRunOffR-max(self->vTop,(double)0),(double)0);

	f1 = max(xpn->pCl->pWeather->fLiquPreciRate-pWa->fRunOffR-max(self->vTop,(double)0),(double)0);
	pWa->pEvap->fActR = min(-min(self->vTop,(double)0)+f1,pWa->pEvap->fPotR);
//pWa->pEvap->fActR = -min(vTop,(double)0)+f1;
	pWa->fInfiltR     =  max(self->vTop,(double)0)+f1;
	
	pWa->fPercolR     =  max(self->vBot,(double)0);
	self->fCapillRiseR      = -min(self->vBot,(double)0);
//Maximal actual evaporation rate
	pWL->pNext->fHydrCond = NEXT_CONDUCTIVITY(pWL->pNext->fMatPotAct); // first layer
	pWL->fHydrCond        = pWL->pNext->fHydrCond;  // zero layer same value as first layer
	pEV->fMaxR	= pWL->fHydrCond*((double)300000+pWL->pNext->fMatPotAct)/DeltaZ;//pSo->fDeltaZ;

//Water flux densities and water flow
	pWL->fFluxDens=self->vTop;
	CHECK_VALID(pWL->fFluxDens);
	pWL->fFlux = pWL->fFluxDens * DeltaT;

	

	for (pWL=pWa->pWLayer->pNext,iLayer=1; pWL->pNext!=NULL; pWL=pWL->pNext,iLayer++)
		{
			pWL->fFluxDens=self->v[iLayer];
			pWL->fFlux = pWL->fFluxDens * DeltaT;
		}
	pWL->fFluxDens=self->vBot;
	pWL->fFlux = pWL->fFluxDens * DeltaT;
	pWL->pBack->fFluxDens=self->vBot;
	pWL->pBack->fFlux = pWL->pBack->fFluxDens * DeltaT;

// update hydraulic conductivities for root water uptake (Nimah and Hanks 1973)
	for (H2O_SOIL_LAYERS) //  Schicht i = 1 bis  n-1
		{
			/*     unsaturated hydraulic conductivities */
			pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
			                 ? ((CONDUCTIVITY(pWL->fMatPotOld)
			                     + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld))/ (double)2.0)
			                 : CONDUCTIVITY(pWL->fMatPotOld);                     // [mm/day]

		} // for all soil layers
///*
	switch (self->mob_imm)
		{
		case 1:
			for (H2O_ALL_BUT_NOT_FIRST)
				{
					pSW->fContSat=self->afTS[iLayer];
					pSW->fContRes=self->afTR[iLayer];
				}
			break;
		default:
			break;
		}


//*/
	if (self->use_infiltration_limit_layer==TRUE)
		{
			hydrus_rain_limit_infiltration_layer(self);
		}
		
	/*for (H2O_SOIL_LAYERS)
		{	
	if (WATER_CONTENT(pWL->fMatPotAct) > pSL->fPorosity)
				{
					pWL->fMatPotAct = MATRIX_POTENTIAL(pSL->fPorosity-EPSILON);
					pWL->fMatPotOld = pWL->fMatPotAct;
					pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);					
					pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
					PRINT_WARNING_ID(xpn,"WATER_CONTENT(pWL->fMatPotAct) > pSL->fPorosity")
				}
		}*/

	return RET_SUCCESS;
}



// ------------------------------------------------------- HYDRUS SUB FUNCTIONS: ----------------------------------------------

void hydrus_rain_limit_infiltration(hydrus *self)
{
	expertn_modul_base *xpn = &(self->parent);
	/*	int i;
		PWLAYER pWLayer;
		PSLAYER pSLayer;
		PSWATER pSWater;*/
	double max_infiltration,fPrecdt,fWaterStorageRate;

	fWaterStorageRate = xpn->pCl->pWeather->fWaterPond/xpn->pTi->pTimeStep->fAct;

	xpn->pCl->pWeather->fLiquPreciRate+=fWaterStorageRate;
	fWaterStorageRate=0.0;

	max_infiltration = (xpn->pSo->pSLayer->pNext->fPorosity-xpn->pWa->pWLayer->pNext->fContAct)*xpn->pSo->pSLayer->pNext->fThickness;
	/*	for (i=0,pSWater=xpn->pSo->pSWater->pNext, pWLayer=xpn->pWa->pWLayer->pNext,pSLayer=xpn->pSo->pSLayer->pNext;i<xpn->pSo->iLayers-1; i++,pWLayer=pWLayer->pNext,pSLayer=pSLayer->pNext,pSWater=pSWater->pNext)
		{
			max_infiltration = min(max_infiltration,(pSLayer->fPorosity-pWLayer->fContAct)*pSLayer->fThickness);
		}*/
	fPrecdt = xpn->pCl->pWeather->fLiquPreciRate*xpn->pTi->pTimeStep->fAct;

	if (fPrecdt>max_infiltration)
		{
			fWaterStorageRate=(fPrecdt-max_infiltration)/xpn->pTi->pTimeStep->fAct;
			xpn->pCl->pWeather->fLiquPreciRate-=fWaterStorageRate;
			
		}
	if (self->use_infiltration_limit==1)
		{
			xpn->pCl->pWeather->fWaterPond = fWaterStorageRate*xpn->pTi->pTimeStep->fAct;
		}
	else
		{
			xpn->pWa->fRunOffR = fWaterStorageRate;
		}
		
}

void hydrus_rain_limit_infiltration_layer(hydrus *self)
{
	expertn_modul_base *xpn = &(self->parent);
	int i;
	double water_pond;
	PWLAYER pWLayer;
	PSLAYER pSLayer;
	PSWATER pSWater;

	for (i=1,pSLayer=xpn->pSo->pSLayer->pNext,pWLayer=xpn->pWa->pWLayer->pNext,pSWater=xpn->pSo->pSWater->pNext; pSLayer->pNext!=NULL; pSLayer=pSLayer->pNext,pSWater=pSWater->pNext,pWLayer=pWLayer->pNext,i++)
		{
			//Nothing
		}

	water_pond = 0.0;

	for	(; pWLayer->pBack!=NULL; pWLayer=pWLayer->pBack,pSLayer=pSLayer->pBack,pSWater=pSWater->pBack,i--)
		{
			//if (pWLayer->fContOld!=pWLayer->fContAct) PRINT_ERROR_ID(xpn,"fContAct != fContActOld");

			//C_DEBUG_LOC_IN_ARRAY(water_pond,i);

			if (water_pond > 0.0)
				{
					if (i>1)
						{
							pWLayer->fContAct += 	water_pond/pSLayer->fThickness;
							//pWLayer->fContAct = pWLayer->fContOld;
						}
					else
						{
							xpn->pCl->pWeather->fWaterPond += water_pond *xpn->pTi->pTimeStep->fAct;
						}

				}

			water_pond = 0.0;

			if (pWLayer->fContAct > pSLayer->fPorosity)
				{
					water_pond = (pWLayer->fContAct - pSLayer->fPorosity)*20.0;
					pWLayer->fContAct -= water_pond;
					water_pond *= pSLayer->fThickness;
					//pWLayer->fContAct = pWLayer->fContOld;
				}
			else
				{
					water_pond = 0.0;
				}
			water_pond = 0.0;
		}



}

/************************************************************************/
int hydrus_water_flow_genMat(hydrus *self, Tableau ConTab, Tableau CapTab, Tableau STab)
{

	expertn_modul_base *xpn = &(self->parent);
	PSPROFILE pSo = self->parent.pSo;
	PWATER pWa = self->parent.pWa;

	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;
//	PWBALANCE   pWB;

	int iLayer        = 0;



	double x0,xN,dx;
	double fh;
	int i,iM;


	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
	iLayer = 0;

	self->hTab[0]=-(double)1e-8;
	self->hTab[self->NTabD-1]=-(double)1e+8;

	x0=(double)log10(-(double)self->hTab[0]);
	xN=(double)log10(-(double)self->hTab[self->NTabD-1]);
	dx=(xN-x0)/(double)(self->NTabD-1);

	for (i=0; i<=self->NTabD-1; i++)
		{
			self->hTab[i]=-hydrus_abspower((double)10,(x0+i*dx));
		}

	for (H2O_ALL_BUT_NOT_FIRST) //iM --> iLayer
		{
			//Material M=MatNum[i]
			iM=iLayer;
			self->ConSat[iM]= pSW->fCondSat;
			self->ThS[iM]= pSW->fContSat;
			self->ThR[iM]=pSW->fContRes;
			//fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(double)1;
			fh=self->ThR[iM]+(self->ThS[iM]-self->ThR[iM])*(double)1;
			self->hSat[iM]=MATRIX_POTENTIAL(fh);


			for (i=0; i<=self->NTabD-1; i++)
				{
					self->ConTab[i][iM]=CONDUCTIVITY(self->hTab[i]);
					self->CapTab[i][iM]=CAPACITY(self->hTab[i]);
					self->STab[i][iM]=(WATER_CONTENT(self->hTab[i])-self->ThR[iM])/hydrus_check_0(self->ThS[iM]-self->ThR[iM],EPSILON);
					// -pSW->fContRes)/(pSW->fContSat-pSW->fContRes);
				}
		} /* for: soil layers */
	/*
	  for (H2O_ALL_BUT_NOT_FIRST) // M=1,NMat
	  {
	    ConSat[iLayer]=pSW->fCondSat;
	    ThS[iLayer]=pSW->fContSat;
	    ThR[iLayer]=pSW->fContRes;
	    fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(double)1;
	    hSat[iLayer]=MATRIX_POTENTIAL(fh);
	  }
	*/
	return RET_SUCCESS;
} /* end GenMat */

/************************************************************************/
int hydrus_water_flow_setBC(hydrus *self)
{

	PSPROFILE pSo = self->parent.pSo;
	PWATER pWa = self->parent.pWa;

//	PSLAYER     pSL;
//	PSWATER     pSW;
//	PWLAYER     pWL;
//	PWBALANCE   pWB;

//	int iLayer        = 0;

	//double hCritA,fh;



//	pSL = pSo->pSLayer;
//	pSW = pSo->pSWater;
//	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
//	iLayer = 0;

//	fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(double)0.0002;
//	hCritA=MATRIX_POTENTIAL(fh);

	if (self->TopInF==TRUE)
		{
			self->rTopOld= self->rTop;
			self->rTop=(double)fabs((double)self->parent.pCl->pWeather->fLiquPreciRate)
			           -(double)fabs((double)pWa->fRunOffR)
			           -(double)fabs((double)pWa->pEvap->fActR);
			//if((double)fabs((double)rTopOld-(double)rTop)>(double)1.e-8) MinStep=TRUE;

			//hTopOld=;
			//hTop=;
			//if(abs(hTopOld-hTop)>(double)1.e-8) MinStep=TRUE;
			//rRoot=;
		}

	if (self->BotInF==TRUE)
		{
			self->rBotOld=self->rBot;
			self->rBot=(double)0;
			if (self->iBotBC == (int)3) self->rBot=(double)-0.0;//no flux
			//if(abs(rBotOld-rBot)>(double)1.e-8) MinStep=TRUE;
			self->hBotOld=self->hBot;
			self->hBot=(double)99;

			pWa->fGrdWatLvlPot = pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(double)2;
			if (self->iBotBC == (int)2) self->hBot=pWa->fGrdWatLvlPot;
			//if(abs(hBotOld-hBot)>(double)1.e-8) MinStep=TRUE;
		}

	return RET_SUCCESS;
}  /*   end  */

/************************************************************************/


/************************************************************************/
int hydrus_water_flow_setMat(hydrus *self, Tableau ConTab, Tableau CapTab, Tableau STab)
{

	expertn_modul_base *xpn = &(self->parent);
	int iLayer        = 0;
	PSPROFILE pSo = self->parent.pSo;
	PWATER pWa = self->parent.pWa;

	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;
//PWBALANCE   pWB;


	double hi1,hi2;
	double x0,xN,dx,dh,hiM,hTabM;
	double Coni,Capi,SSSi;
	int   M,iT;
//  int   NumNP = pSo->iLayers-1;

	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
	iLayer = 0;

	hTabM=self->hTab[self->NTabD-1];
	x0=(double)log10((double)(-self->hTab[0]));
	xN=(double)log10((double)(-hTabM));
	dx=(xN-x0)/(double)(self->NTabD-1);

	for (H2O_ALL_BUT_NOT_FIRST) //i=iLayer
		{
			// Material M=MatNum(i)
			M=iLayer;
			// Hysterese
			/*       if(Kappa(i).eq.-1)
				   {
					hi1=min(hTemp(i)/Ah(i),hSat(M))
					hi2=min(hNew(i)/Ah(i),hSat(M))
				   }
			       if(Kappa(i).eq.+1) hiM=min(hNew(i)/Ah(i)/AhW(M),hSat(M))
			*/

			hi1=(double)min((double)self->hTemp[iLayer],(double)self->hSat[M]);
			hi2=(double)min((double)self->hNew[iLayer],(double)self->hSat[M]);
			hiM=(double)0.1*hi1+(double)0.9*hi2;
			//hiM=min(hNew[iLayer],hSat[M]);
			if((hi1>=self->hSat[M])&&(hi2>=self->hSat[M]))
				{
					Coni=self->ConSat[M];
				}
			else
				{
					if ((hiM>self->hTab[self->NTabD-1])&&(hiM<=self->hTab[0]))
						{
							iT=(int)(((double)log10((double)(-hiM))-x0)/dx);
							dh=(hiM-self->hTab[iT])/(self->hTab[iT+1]-self->hTab[iT]);
							Coni=ConTab[iT][M]+(ConTab[iT+1][M]-ConTab[iT][M])*dh;
						}
					else
						{
							Coni=CONDUCTIVITY(hiM);
						}
				}

			if(hiM>=self->hSat[M])
				{
					Capi=(double)0;
					SSSi=(double)1;
				}
			else
				{
					if((hiM>=self->hTab[self->NTabD-1])&&(hiM<=self->hTab[0]))
						{
							iT=(int)(((double)log10((double)(-hiM))-x0)/dx);
							dh=(hiM-self->hTab[iT])/(self->hTab[iT+1]-self->hTab[iT]);
							Capi=CapTab[iT][M]+(CapTab[iT+1][M]-CapTab[iT][M])*dh;
							SSSi=STab[iT][M]+(STab[iT+1][M]-STab[iT][M])*dh;
						}
					else
						{
							Capi=CAPACITY(hiM);
							SSSi=(WATER_CONTENT(hiM)-self->ThR[M])/(hydrus_check_0(self->ThS[M]-self->ThR[M],EPSILON));
						}
				}

			Coni=CONDUCTIVITY(hiM);
			Capi=CAPACITY(hiM);
			SSSi=(WATER_CONTENT(hiM)-self->ThR[M])/(hydrus_check_0(self->ThS[M]-self->ThR[M],EPSILON));

			self->Con[iLayer]=Coni;
			self->Cap[iLayer]=Capi;

			if(self->Iter==0) self->ConO[iLayer]=self->Con[iLayer];

			self->ThN[iLayer]=self->ThR[M]+SSSi*(self->ThS[M]-self->ThR[M]);
			CHECK_VALID(self->ThN[iLayer])

		}// soil layers

	return RET_SUCCESS;
}  //   end



/************************************************************************/
int hydrus_water_flow_changeBC(hydrus *self)
{

	PSPROFILE pSo = self->parent.pSo;
	PWATER pWa = self->parent.pWa;

//	PSLAYER     pSL;
	PSWATER     pSW;
	PWLAYER     pWL;
//	PWBALANCE   pWB;
	double hCritS,hCritA,fh;
	int   N=pSo->iLayers-1;
	double DeltaZ;

//	pSL = pSo->pSLayer;
	pSW = pSo->pSWater;
	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
	DeltaZ = pSo->fDeltaZ;
//	iLayer = 0;

//critical parameters
	fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(double)0.0002;
	hCritA=MATRIX_POTENTIAL(fh);
	hCritS=(double)0;

//   Seepage face at the bottom
	if(self->SeepF==TRUE)
		{
			if(self->KodBot>=(double)0)
				{
					//vBot=(Con[N-1]+Con[N])/(double)2*((hNew[N-1]-hNew[N])/DeltaZ+(double)dCosAlf)
					//     -((ThN[N]-ThO[N])/dt+Sink[N])*DeltaZ/(double)2;

					self->vBot=-(self->Con[N]+self->Con[N-1])/(double)2*((self->hNew[N]-self->hNew[N-1])/DeltaZ/(double)2-(double)self->dCosAlf);

					if(self->vBot<(double)0)
						{
							self->KodBot=-(double)2;
							self->rBot=(double)0;
						}
				}
			else
				{
					if(self->hNew[N]>=(double)0)
						{
							self->KodBot=(double)2;
							self->hBot=(double)0;
						}
				} /* if KodBot >=0 */
		} /* if SeepF */

//   Atmospheric boundary condition
// CK: ERROR
	if(self->TopInF==TRUE)
		{
			if(self->KodTop>=(double)0)
				{
					self->vTop=-(self->Con[2]+self->Con[1])/(double)2*((self->hNew[2]-self->hNew[1])/DeltaZ-(double)self->dCosAlf)
					           +((self->ThN[1]-self->ThO[1])/self->dt+self->Sink[1])*DeltaZ/(double)2;

					self->vTop=-(self->Con[2]+self->Con[1])/(double)2*((self->hNew[2]-self->hNew[1])/DeltaZ/(double)2-(double)self->dCosAlf)
					           +((self->ThN[1]-self->ThO[1])/self->dt+self->Sink[1])*DeltaZ;


					if((fabs((double)self->vTop)>fabs((double)self->rTop)) || (self->vTop*self->rTop<=(double)0))
						{
							self->KodTop=-(double)4;
						}
				}
			else
				{
					if((self->hNew[1]>hCritS) && (self->WLayer==FALSE))
						{
							self->KodTop=(double)4;
							self->hTop=(double)hCritS;
						}

					if (self->hNew[1]<=hCritA)
						{
							self->KodTop=(double)4;
							self->hTop=hCritA;
						}
				} /* if KodTop >=0 */
		} /* if TopInF */



	return RET_SUCCESS;
}  //   end

/************************************************************************/
int hydrus_water_flow_setEq(hydrus *self)
{

//	int iLayer        = 0;
	PSPROFILE pSo = self->parent.pSo;
//	PWATER pWa = self->parent.pWa;

//	PSLAYER     pSL;
//	PSWATER     pSW;
//	PWLAYER     pWL;
//	PWBALANCE   pWB;
	double DeltaZ;

	double dx,ddt;
	double dCosAlf,dxA,dxB,ConA,ConB,ST,RT,PT;
	int   N=pSo->iLayers-1;
	int   i;
	double fAqh=(double)1,fBqh=(double)1;//Input from '*.xnm' file
	double fGWL=(double)1;//Input from database: ground-water table

//	pSL = pSo->pSLayer;
//	pSW = pSo->pSWater;
//	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
	DeltaZ = pSo->fDeltaZ;
//	iLayer = 0;

	ddt=(double)self->dt;

	for (i=0; i<=N; i++)
		{
			self->P[i]=(double)0;
			self->R[i]=(double)0;
			self->S[i]=(double)0;
		}


	dCosAlf=(double)1;
//    dxA=x(2)-x(1)
	dxA=(double)DeltaZ;//pSo->fDeltaZ;
	dxA=(double)2*(double)DeltaZ;//pSo->fDeltaZ;
	dxB=dxA;
	dx=dxA/(double)2;

//upper boundary
	ConB=((double)self->Con[1]+(double)self->Con[2])/(double)2;
	RT=ConB/dxB+(double)self->Cap[1]*dx/ddt;
	ST=-ConB/dxB;

	PT=(double)self->Cap[1]*dx*(double)self->hNew[1]/ddt
	   -((double)self->ThN[1]-(double)self->ThO[1])*dx/ddt-(double)self->Sink[1]*dx-ConB*dCosAlf;
	PT+=(double)self->rTop;

	if(self->WLayer==TRUE) // then ! surface reservoir BC
		{
			if(self->hNew[1]>(double)0)
				{
					RT=RT+(double)1/ddt;
					PT=PT+max((double)self->hOld[1],(double)0)/ddt;
				}
			else
				{
					PT=PT+max((double)self->hOld[1],(double)0)/ddt;
				}
		}

	self->P[1]=PT;
	CHECK_VALID(self->P[1])
	self->S[1]=ST;
	self->R[1]=RT;


	//profile
	for (i=2; i<=N-1; i++)
		{
//         dxA=x(i)-x(i-1)
//         dxB=x(i+1)-x(i)
//         dx=(dxA+dxB)/(double)2;
			ConA=((double)self->Con[i]+(double)self->Con[i-1])/(double)2;
			ConB=((double)self->Con[i]+(double)self->Con[i+1])/(double)2;
			self->P[i]=(ConA-ConB)*dCosAlf-(double)self->Sink[i]*dx;
			self->P[i]+=((double)self->Cap[i]*(double)self->hNew[i]
			             -((double)self->ThN[i]-(double)self->ThO[i]))*dx/ddt;
			self->R[i]=(double)self->Cap[i]*dx/ddt+ConA/dxA+ConB/dxB;
			self->S[i]=-ConB/dxB;			
			CHECK_VALID(self->P[i])
			
		}/* for */

//lower boundary
//      dxB=x(N)-x(N-1)
//      dx=dxB/(double)2;
	ConA=((double)self->Con[N]+(double)self->Con[N-1])/(double)2;

	if(self->FreeD==TRUE) // free drainage
		{
			self->R[N]=ConA/dxA;
			self->S[N]=-self->R[N];
			self->P[N]=(double)0;
		}
	else
		{
			self->R[N]=(double)self->Cap[N]*dx/ddt+ConA/dxA;
			self->S[N]=-ConA/dxA;
			if(self->qGWLF) self->rBot=Fqh(fGWL,fAqh,fBqh);// outflow function BC
			self->P[N]=-ConA*dCosAlf-(double)self->Sink[N]*dx;//dxB instead of dx
			self->P[N]=ConA*dCosAlf+(double)self->Sink[N]*dx;//dxB instead of dx
			self->P[N]+=(double)self->Cap[N]*dx*(double)self->hNew[N]/ddt
			            -((double)self->ThN[N]-(double)self->ThO[N])*dx/ddt;
			self->P[N]-=self->rBot; // flux BC			
			CHECK_VALID(self->P[N])			
		}


	return RET_SUCCESS;
}  //   end

/************************************************************************/
//|||||||||||||||||||||||Solves Matrix Equation


int hydrus_water_flow_solveEq(hydrus *self)
{
	PSPROFILE pSo = self->parent.pSo;

	int   N=pSo->iLayers-1;
	int   i;


//forward
	if(self->KodTop>(double)0)
		{
			self->P[2]-=self->S[1]*(double)self->hTop;
			CHECK_VALID(self->P[2])
		}
	else
		{
			if(fabs(self->R[1])<self->rMin) self->R[1]=self->rMin;
			self->P[2]-=self->S[1]/self->R[1]*self->P[1];
			self->R[2]-=self->S[1]/self->R[1]*self->S[1];
			CHECK_VALID(self->P[2])
			CHECK_VALID(self->R[2])
		}

	for(i=3; i<=N-1; i++)
		{
			if(fabs(self->R[i-1])<self->rMin) self->R[i-1]=self->rMin;
			self->P[i]-=self->S[i-1]/self->R[i-1]*self->P[i-1];
			self->R[i]-=self->S[i-1]/self->R[i-1]*self->S[i-1];
			CHECK_VALID(self->P[i])
			CHECK_VALID(self->R[i])
			CHECK_VALID(self->R[i-1])
		}

	if (self->KodBot>(double)0)
		{
			self->P[N-1]-=self->S[N-1]*(double)self->hBot;
			CHECK_VALID(self->P[N-1])
		}
	else
		{
			if(fabs(self->R[N-1])<self->rMin) self->R[N-1]=self->rMin;
			self->P[N]-=self->S[N-1]/self->R[N-1]*self->P[N-1];
			self->R[N]-=self->S[N-1]/self->R[N-1]*self->S[N-1];
			CHECK_VALID(self->P[N])
			CHECK_VALID(self->R[N])
		    CHECK_VALID(self->R[N-1])		
		}


//backward
	if(fabs(self->R[N-1])<self->rMin) self->R[N-1]=self->rMin;

	if (self->KodBot>(double)0)
		{
			self->hNew[N]=self->hBot;
			CHECK_VALID(self->hNew[N])
			self->hNew[N-1]=(double)(self->P[N-1]/self->R[N-1]);
			CHECK_VALID(self->hNew[N-1])
		}
	else
		{
			self->hNew[N]=(double)(self->P[N]/self->R[N]);
			CHECK_VALID(self->hNew[N])
			self->hNew[N-1]=(double)((self->P[N-1]-self->S[N-1]*(double)self->hNew[N])/self->R[N-1]);
			CHECK_VALID(self->hNew[N-1])
		}
	
	

	for (i=N-2; i>1; i--)
		{
			if(fabs(self->R[i])<self->rMin) self->R[i]=self->rMin;
			self->hNew[i]=(double)((self->P[i]-self->S[i]*self->hNew[i+1])/self->R[i]);
			CHECK_VALID(self->hNew[i])
		}


	if (self->KodTop>(double)0)
		{
			self->hNew[1]=self->hTop;
			CHECK_VALID(self->hNew[1])
		}
	else
		{
			if(fabs(self->R[1])<self->rMin) self->R[1]=self->rMin;
			self->hNew[1]=(double)((self->P[1]-self->S[1]*self->hNew[2])/self->R[1]);
			CHECK_VALID(self->hNew[1])
		}

	for (i=1; i<N; i++)
		{
			if(fabs(self->hNew[i])>self->rMax)
				{
					if(self->hNew[i]>=(double)0)
						{
							self->hNew[i]=(double)fabs(self->rMax);
							CHECK_VALID(self->hNew[i])
						}
					else
						{
							self->hNew[i]=-(double)fabs(self->rMax);
							CHECK_VALID(self->hNew[i])
						}
				}
		}

	return RET_SUCCESS;
}  //   end

/************************************************************************/
BOOL hydrus_water_flow_testCvg(hydrus *self)
{

expertn_modul_base *xpn = &(self->parent);
//	int iLayer        = 0;
	PSPROFILE pSo = self->parent.pSo;
//	PWATER pWa = self->parent.pWa;

//	PSLAYER     pSL;
//	PSWATER     pSW;
//	PWLAYER     pWL;
//	PWBALANCE   pWB;
	BOOL ItCrF;


	double Eps=(double)0;
	double Eph=(double)0;

//double Tol=(double)1.0e-6;
	double Tol=(double)1.0e-5;
//double Tol=(double)1.0e-4;
//double Tol=(double)0.001;//orig.: Tol=0.001;
//double Tol=(double)0.01;

//double Tolh=(double)0.01;
	double Tolh=(double)0.001;//orig.: Tolh=0.1;
//double Tolh=(double)1;
//double Tolh=(double)10;

	int m,i;
	int NumNP=pSo->iLayers-1;

//	pSL = pSo->pSLayer;
//	pSW = pSo->pSWater;
//	pWL = pWa->pWLayer;
//	pWB = pWa->pWBalance;
//	iLayer = 0;

	ItCrF=TRUE;
	self->ConvgF=TRUE;

	for (i=NumNP; (i>0)&&(ItCrF==TRUE); i--)
		{
			//m=MatNum(i)
			m=i;
			if((self->hTemp[i]<self->hSat[m])&&(self->hNew[i]<self->hSat[m]))
				{
					//Eps=abs(Cap(i)*(hNew(i)-hTemp(i))/(ThS(m)-ThR(m))/ATh(i))
					Eps=(double)fabs((double)(self->Cap[i]*(self->hNew[i]-self->hTemp[i])/hydrus_check_0(self->ThS[i]-self->ThR[i],EPSILON)));
				}
			else
				{
					//Eph=abs(hNew(i)-hTemp(i))/Ah(i)
					Eph=(double)fabs((double)(self->hNew[i]-self->hTemp[i]));
				}

			if((Eps>Tol) || (Eph>Tolh))
				{
					ItCrF=FALSE;
				}

		} /* for */

	if(ItCrF==TRUE)
		{
			for (i=1; i<=NumNP; i++)
				{
					self->ThN[i]=self->ThN[i]+self->Cap[i]*(self->hNew[i]-self->hTemp[i]);
					CHECK_VALID(self->ThN[i])
				}
		}
	else
		{
			if (self->Iter<self->ItMax)
				{
					self->IGoTo=(int)2;
				}
			else
				{
					if (self->dt<=self->dtMin)
						{
							self->ConvgF=FALSE;
							self->IGoTo=(int)3; //!give up
							//pTi->pSimTime->fTimeAct=(double)(pTi->pSimTime->iSimDuration+(int)5); //!give up
							//pTi->pTimeStep->fAct=(double)1;
							PRINT_WARNING_ID((&(self->parent)),"Water Flow PDE Solver Did Not Converge.");

						}
					else
						{
							for (i=1; i<=NumNP; i++)
								{
									//Hysterese  if(IHyst.gt.0) Kappa(i)=KappaO(i)
									self->hNew[i]=self->hOld[i];
									self->hTemp[i]=self->hOld[i];
									self->ThN[i]=self->ThO[i];
									CHECK_VALID(self->ThN[i])
								}

							self->KodTop=self->KTOld;
							self->KodBot=self->KBOld;
							self->dt=(double)max((double)self->dt/(double)3,(double)self->dtMin);
							self->dtOpt=self->dt;
							self->t=self->tOld+self->dt;
							self->IGoTo=(int)1; // !try smaller time step

						} /* else dt<dtMin */
				} /* else Iter<MaxIt */
		} /* else ItCrF */

	return ItCrF;
}  //   end hydrus_water_flow_testCvg

/************************************************************************/
int hydrus_water_flow_tmCont(hydrus *self)
{

	PTIME pTi = self->parent.pTi;
	double DeltaT=pTi->pTimeStep->fAct;

	if (self->MinStep==TRUE)
		{
			self->dtMax=(double)min((double)self->dtMax,min((double)self->dtMaxC,(double)self->dtInit));
			self->dtOpt=self->dtMax;
			self->MinStep=FALSE;
		}
	else
		{
			//dtMax=(double)min((double)dtMaxW,(double)dtMaxC);
			self->dtMax=DeltaT;
		}

//tFix=(double)min((double)tNew,min((double)tDay,(double)tMax));
	self->tFix=(double)min((double)self->tNew,(double)self->tMax);

	if((self->Iter<=self->ItMin)&&((self->tFix-self->t)>self->dMul1*self->dtOpt))
		self->dtOpt=(double)min((double)self->dtMax,(double)self->dMul1*(double)self->dtOpt);


	if (self->Iter>=self->ItMax) self->dtOpt=(double)max((double)self->dtMin,(double)self->dMul2*(double)self->dtOpt);

	self->dt=(double)min((double)self->dtOpt,(double)self->tFix-(double)self->t);
	self->dt=(double)min((double)((self->tFix-self->t)/(int)((self->tFix-self->t)/self->dt)),(double)self->dtMax);
	if(((self->tFix-self->t)!=self->dt)&&(self->dt>(self->tFix-self->t)/(double)2)) self->dt=(self->tFix-self->t)/(double)2;

	return RET_SUCCESS;
}  //   end




int hydrus_water_flow_veloc(hydrus *self, int N,double dCosAlf)
{

	PSPROFILE pSo = self->parent.pSo;
	double DeltaZ = pSo->fDeltaZ;

//      real*8 Con,ConO
//      dimension hNew(*),x(*),Con(*),ConO(*),v(*)
	double dxA,dxB,ConA,ConB,vA,vB;
	int i;
//      dxA=x(2)-x(1)
	dxA=dxB=DeltaZ;
	ConA=(self->Con[1]+self->Con[2])/2;
	self->v[1]=-ConA*(self->hNew[2]-self->hNew[1])/dxA+ConA*(double)dCosAlf;
	for(i=2; i<=N-1; i++)
		{
//        dxB=x(i+1)-x(i)
//        dxA=x(i)-x(i-1)
			ConB=(self->Con[i]+self->Con[i+1])/2;
			ConA=(self->Con[i]+self->Con[i-1])/2;
			vB=-ConB*((self->hNew[i+1]-self->hNew[i])/dxB-(double)dCosAlf);
			vA=-ConA*((self->hNew[i]-self->hNew[i-1])/dxA-(double)dCosAlf);
			self->v[i]=(vA*dxA+vB*dxB)/(dxA+dxB);
		}
//      dxB=x(N)-x(N-1)
	ConB=(self->Con[N]+self->Con[N-1])/2;
	self->v[N]=-ConB*(self->hNew[N]-self->hNew[N-1])/dxB+ConB*(double)dCosAlf;

	return RET_SUCCESS;
} /*  end  Veloc */

/************************************************************************/
double Fqh(double z, double a, double b)
{
	//flux q at lower boundary in relation to ground-water table z
	double q;
	q = a * (double)exp((double)b * fabs((double)z));

	return q;
}

/************************************************************************/
