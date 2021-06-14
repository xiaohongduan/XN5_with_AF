//      spass_root.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_root.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



int spass_root_length_growth(spass *self)
  {
	expertn_modul_base *xpn = &(self->parent);

	PPLANT pPl = xpn->pPl;

	if (PLANT_IS_GROWING)
	{
	spass_RootExtension(self);
	spass_RootLengthDensity(self);
	}

	return RET_SUCCESS;
	}


int spass_RootExtension(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);	

	PHEAT			pHe = xpn->pHe ; 
	PPLANT			pPl = xpn->pPl; 
	PSPROFILE		pSo = xpn->pSo; 
	PWATER			pWa = xpn->pWa;

	int 				i,L;
	double 				fMaxRootDepthSoil,fMaxRootDepthCrop,DEPMAX,fThickness;
	double 				fDepth,fRelWc,fMoistRed;
	double				fTempMin,fTempMax,fTempOpt,fTempFunc;	

	PSLAYER 			pSL		=pSo->pSLayer->pNext;
	PSWATER				pSWL	=pSo->pSWater->pNext;
	PHLAYER				pSLH	=pHe->pHLayer->pNext;
	PWLAYER	   			pSLW	=pWa->pWLayer->pNext; 
	PROOT				pRT		=pPl->pRoot;  
	PBIOMASS			pBm		=pPl->pBiomass;
	PTIMESTEP			pTi		=xpn->pTi->pTimeStep;

	//Constant Coefficient
	fMaxRootDepthSoil	= (double)200.0; 	//cm depends on soil property
	//fMaxRootDepthCrop   = (double)150.0; 	//cm depends on crop property
	//SG/20/04/99: 
	//	besser 
	fMaxRootDepthCrop = pPl->pGenotype->fMaxRootDepth;
	//	dann wird der Wert aus What.gtp verwendet!!!
	//	(entsprechend für fMaxRootDepthSoil?)

    DEPMAX=(double)0.0;
	pSL	  =pSo->pSLayer->pNext;
	for (L=1;L<=pSo->iLayers-2;L++)
		{
		DEPMAX += (double)0.1*pSL->fThickness;	//cm
		pSL=pSL->pNext;
		}
  

  	pRT->fMaxDepth=min(DEPMAX,min(fMaxRootDepthSoil,fMaxRootDepthCrop));

	//=========================================================================================
	//Root Depth Growth Rate
	//=========================================================================================
    fDepth 	= (double)0.0;
	pSL		=pSo->pSLayer->pNext;
	pSLH	=pHe->pHLayer->pNext;
	pSLW	=pWa->pWLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	for (i=1;i<=pSo->iLayers-2;i++)
		{ 
		fThickness = (double)0.1*pSL->fThickness;

		if ((pRT->fDepth>fDepth)&&(pRT->fDepth<=fDepth+fThickness))
			break;
		fDepth += fThickness; 
		
		if (i<pSo->iLayers-2)
			{

			//SG/09/04/99////////////////////////////////////////////////////////////
			//
			//	pSL bzw. pSL->pNext wird doch gar nicht gebraucht. Oder
			//	habe ich etwas übersehen?
			//	Aber: in PLAGEN wird immer der aktuelle Wassergehalt der tiefsten
			//	durchwurzelten Schicht verwendet. Deshalb muß hier pSLW auf pSLW->pNext 
			//	gesetzt werden!!!
			//////////////////////////////////////////////////////////////////////////

			pSLW = pSLW->pNext;
			pSWL = pSWL->pNext;

			pSL = pSL ->pNext;
			pSLH= pSLH->pNext;
		    }   
		}// for

//	if ((lpBm->fRootGrowRate>(double)0.0)&&(pRT->fDepth<fMaxRootDepthSoil))
	if ((pRT->fDepth<fMaxRootDepthSoil))
		{    
		//Temperature Effect

		fTempMin = pPl->pGenotype->fRootExtTempMin;
		fTempOpt = pPl->pGenotype->fRootExtTempOpt; 
		fTempMax = pPl->pGenotype->fRootExtTempMax;
		

		if (pPl->pGenotype->RootExtRateTemp[0].fInput==(double)-99.9)
			fTempFunc=(double)spass_RelativeTemperatureResponse(pSLH->fSoilTemp,fTempMin,fTempOpt,fTempMax);
		else
			fTempFunc=spass_AFGENERATOR(pSLH->fSoilTemp, pPl->pGenotype->RootExtRateTemp);

		//-------------------------------------------------------------------------------
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
        fRelWc =(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
        
		if (fRelWc<(double)0.25)
			fMoistRed =max((double)0.0,(double)4.0*fRelWc);
		else
			fMoistRed = (double)1.0;


		//SG/09/04/99//////////////////////////////////////////////////////////////////
		//
		//	In PLAGEN wird, wenn sich die Wurzeln ausschließlich in der obersten
		//	Schicht befinden, fMoistRed immer = 1.0 gesetzt!
		///////////////////////////////////////////////////////////////////////////////

		// if(i==1)
		//	fMoistRed = (double)1.0;			//SG/05/05/99: Streß auch bei i=1!
		
		pRT->fDepthGrowR = max((double)0.0, pPl->pGenotype->fMaxRootExtRate * fTempFunc * fMoistRed);
		}
	else
		pRT->fDepthGrowR = (double)0.0;		
		
	//=========================================================================================
	//Root Depth 
	//=========================================================================================
	
	
	//SG/12/04/99///////////////////////////////////////////////////////////////////////////////
	//
	//	In PLAGEN wird die Wurzeltiefe erst nach der Berechnung der Wurzelverteilung
	//	aktualisiert!!
	////////////////////////////////////////////////////////////////////////////////////////////

	
	pRT->fDepth += pRT->fDepthGrowR*pTi->fAct;

	pRT->fDepth = min(pRT->fDepth,pRT->fMaxDepth);	

		
	
	return RET_SUCCESS;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	RootGrowth_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


int spass_RootLengthDensity(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);	

	PCHEMISTRY			pCh = xpn->pCh;
	PHEAT				pHe = xpn->pHe ; 
	PPLANT				pPl = xpn->pPl; 
	PSPROFILE			pSo = xpn->pSo; 
	PWATER				pWa = xpn->pWa;

	PSLAYER 			pSL		=pSo->pSLayer;
	PSWATER				pSWL	=pSo->pSWater;
	PHLAYER				pSLH	=pHe->pHLayer;
	PWLAYER	   			pSLW	=pWa->pWLayer; 
	PCLAYER     		pSLN	=pCh->pCLayer;
	PROOT				pRT		=pPl->pRoot;  
	PLAYERROOT			pLR		=pPl->pRoot->pLayerRoot;
	PBIOMASS			pBm		=pPl->pBiomass;
	PTIMESTEP			pTS		=xpn->pTi->pTimeStep;

    int 	L,L1,NLAYR;
    double   fCumDepth,DEPMAX,TRLDF,RNLF,RLVF,fRelExtWater,fNewRootLength,fNewRootGrowFactor,fThickness;

	//SG 20120126
	double TRL,TRLLF, RLLR, RLLRLay, EPSILON;
	double fSWDF3;
	
	EPSILON = 1e-9;



	//if (pBm->fRootGrowR<=(double)0.0)

	////SG/12/04/99///////////////////////////////////////////////////////////////////////////////
	////
	////	In PLAGEN wird die Wurzeltiefe erst nach der Berechnung der Wurzelverteilung
	////	aktualisiert!!
	////	Die folgenden Zeilen wurden deshalb vom Ende der Funktion RootExtension_SPASS
	////	hierher verlegt.
	//////////////////////////////////////////////////////////////////////////////////////////////

	//{
	//	//	 pRT->fDepth += pRT->fDepthGrowR;

	//	//	 pRT->fDepth = min(pRT->fDepth,pRT->fMaxDepth);

	//	return 0;
	//}


    NLAYR=pSo->iLayers-2; //Anzahl Bodenschichten

    DEPMAX=(double)0.0;
	pSL	  =pSo->pSLayer->pNext;
	for (L=1;L<=NLAYR;L++)
	{
		DEPMAX += (double)0.1*pSL->fThickness;	//cm
		pSL=pSL->pNext;
	}
  
	//pRT->fMaxDepth=DEPMAX;	//SG/20/04/99: wird doch gar nicht gebraucht, oder?
								// Liefert nur falschen Wert am Ende bei Wurzellänge
  	
	//=========================================================================================
	//		New Root Length Calculation (fNewRootLength, RLNEW)


    //SG 20111024: Werte aus spez. Wurzellänge (*.gtp, Zeile 2000080)
    fNewRootLength=pBm->fRootGrowR*pPl->pGenotype->fRootLengthRatio*(double)1E-5;
	
	if(pPl->pGenotype->fRootLengthRatio == (double)0.0)
	{
		fNewRootLength=pBm->fRootGrowR*((double)1.00)*(double)1E-1;

		//      Werte aus E.Wang, Tabelle 3.5 [cm/cm^2]
		//=========================================================================================
		if (strcmp(pPl->pGenotype->acCropCode,"SG")==0)
			fNewRootLength=pBm->fRootGrowR*((double)1.00)*(double)1E-1; 
		if ((strcmp(pPl->pGenotype->acCropCode,"MI")==0)
			||(strcmp(pPl->pGenotype->acCropCode,"MZ")==0))
			fNewRootLength=pBm->fRootGrowR*((double)0.80)*(double)1E-1;
		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
			||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
			fNewRootLength=pBm->fRootGrowR*((double)1.05)*(double)1E-1;
		if (strcmp(pPl->pGenotype->acCropCode,"SF")==0)
			fNewRootLength=pBm->fRootGrowR*((double)1.70)*(double)1E-1; 
	}

	//=========================================================================================
	//		Factor Calculation for Root Growth
	//=========================================================================================
	fCumDepth	=(double)0.0;
	fSWDF3 		=(double)0.0;
	L = 0;   
	TRL = 0.0;
	
	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
 	while ((fCumDepth<=pRT->fDepth)&&(L<NLAYR))
	{
		L++;
		
		fThickness =(double)0.1*pSL->fThickness; //cm

		pSLN->fNO3Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(double)0.01*pSL->fBulkDens*pSL->fThickness;

		fCumDepth +=(double)0.1*pSL->fThickness; //cm
		//-------------------------------------------------------------------------------
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
        fRelExtWater=(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		if (fRelExtWater<(double)0.25)
			pLR->fWatStressRoot =max((double)0.0,(double)4.0*fRelExtWater);
		else
			pLR->fWatStressRoot = (double)1.0;

		//SG/12/04/99:	folgende Zeile aus PLAGEN:
		// if(L==1) pLR->fWatStressRoot = (double)1.0;	Streß auch bei L=1!

                                          
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		//-------------------------------------------------------------------------------
        //Mineral nitrogen availability effect factor on root growth (pLR->fNStressRoot)
		if (TRUE)
		{
			pLR->fNStressRoot =(double)(1.0-(1.17*exp(-0.15*(double)(pSLN->fNO3N/pSLN->fNO3Nmgkg
																   +pSLN->fNH4N/pSLN->fNH4Nmgkg))));

			if ((pLR->fNStressRoot<(double)0.01)&&
 				  ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
				||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)))
			   pLR->fNStressRoot=(double)0.01;
			else
				pLR->fNStressRoot=max((double)0.1,pLR->fNStressRoot);


			if((strcmp(pPl->pGenotype->acCropCode,"SF")==0)&&(pLR->fNStressRoot<(double)0.1))
				pLR->fNStressRoot = (double)0.1;
		}
        else
        	pLR->fNStressRoot=(double)1.0;

		//SG/12/04/99:	folgende Zeile aus PLAGEN:
		//if(L==1) pLR->fNStressRoot = (double)1.0;	Streß auch bei L=1!


		//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

		//Root length density factor to calculate new root growth distribution 
		fNewRootGrowFactor =(double)exp(-4.0*(double)((fCumDepth-(double)0.5*fThickness)/DEPMAX));
		//für Jörg (13.12.99):
		//fNewRootGrowFactor =(double)exp(-2.0*(double)((fCumDepth-(double)0.5*fThickness)/DEPMAX));
	    pLR->fLengthDensFac = fNewRootGrowFactor*fThickness*min(pLR->fWatStressRoot,pLR->fNStressRoot); //[cm]
  		if ((fCumDepth<pRT->fDepth)&&(L<NLAYR))
		fSWDF3 += (pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP)*fThickness; //[cm]

		TRL += pLR->fLengthDens*fThickness;

		if (L<NLAYR)
		{
			pSL =pSL ->pNext;
			pSWL=pSWL->pNext;
			pSLW=pSLW->pNext;
			pSLN=pSLN->pNext;
			pLR =pLR ->pNext;
		}
	}  // end while
		L1=L;
	//=========================================================================================
	//	Deepest root layer
	//=========================================================================================
	//if (L<=NLAYR)
	//{
	//    pSL=pSL->pBack;
	//	pLR=pLR->pBack;
	//}  	

	//auf Wunsch von Jörg Lippert geändert am 13.12.99
	if(fCumDepth>pRT->fDepth)
		pLR->fLengthDensFac=pLR->fLengthDensFac*((double)1.0-(fCumDepth-pRT->fDepth)/((double)0.1*pSL->fThickness));
		//pLR->fLengthDensFac=(double)0.0;


	//=========================================================================================
	//		Total Root Length Density Factor
	//=========================================================================================
	TRLDF =(double)0.0;
	pLR	  =pPl->pRoot->pLayerRoot;
	for (L=1;L<=L1;L++)
	{
		TRLDF += pLR->fLengthDensFac; //[cm]
        pLR    = pLR->pNext;
    }
	//=========================================================================================
	//		Root Length Density Calculation
	//=========================================================================================
	if ((TRLDF>(double)0)&&(TRLDF>=fNewRootLength*((double)0.00001)))
		RNLF = fNewRootLength/TRLDF; //[cm-2]
	else
		return RET_SUCCESS;

	//printf("SPASS %f %f %f\n", RNLF, fNewRootLength, TRLDF);

	//-------------------------------------------------------------------------------------
	//SG 20120126:
	// zu C) (siehe unten): Absterberate in der jeweiligen Schicht abhängig vom Wassergehalt (PLATHO)
//	TRL = pPl->pBiomass->fRootWeight*pPl->pGenotype->fRootLengthRatio*(double)1E-5; //total root length [cm/cm^2]
	RLLR = pPl->pBiomass->fRootDeathRate/pPl->pBiomass->fRootWeight*TRL; //[cm/cm^2]
//	RLLR = pPl->pBiomass->fRootDeathRate*pPl->pGenotype->fRootLengthRatio*(double)1E-5; //[cm/cm^2]
	TRLLF = 0.0; //total root length loss factor [-]
            
    pLR		= pPl->pRoot->pLayerRoot;
	for (L=1;L<=L1;L++)
	{
		//==================================
		//  Total Root Length Loss Factor
		//==================================
//        TRLLF += pLR->fLengthDens*fThickness*max(1.0-pLR->fWatStressRoot,1.0-pLR->fNStressRoot); //[cm/cm^2]
        TRLLF += pLR->fLengthDens*fThickness*(1.0-pLR->fWatStressRoot); //[cm/cm^2]
		pLR    = pLR->pNext;
    }
	//--------------------------------------------------------------------------------------

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
	fCumDepth=(double)0.0;
	pRT->fTotalLength = (double)0.0;
	for (L=1;L<=L1;L++)
	{//Fow Wheat and Barley
		fThickness =(double)0.1*pSL->fThickness; //cm

		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
		{
			fCumDepth +=fThickness;
          

			////////////////////////////////////////////////////////////////////////////////////////
			//SG 20120126
			// Beim Absterben der Wurzeln geht auch Wurzellänge verloren!
			//  
			// Original-SPASS:
			// pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-((double)0.01)*pLR->fLengthDens; //[cm/cm^3]

			//bzw.
			// RLLRLay = ((double)0.01)*pLR->fLengthDens;

			//SG 20120126:
			// zunehmendes Absterben der Wurzeln (hier: Wurzellängendichte) nach der Blüte (Seneszenz)
			// --> muestte eigentlich aus Absterberate der Wurzeln errechnet werden, am besten unter Berücksichtigung der 
			// Ressourcenverfügbarkeit in den einzelnen Schichten wie in PLATHO

			// A) tägliche Absterberate von 0.01 d-1 (1%) nimmt nach der Blüte zu bis auf 0.05 d-1 (5%) zum Zeitpunkt der Abreife
			//    RLLRLay = ((double)0.01+(double)0.04*max(0.0,(pPl->pDevelop->fStageSUCROS-(double)1.0))/(double)1.0)*pLR->fLengthDens;

			// B) Absterberate in allen Schichten entsprechend des prozentualen gesamten Verlusts an Wurzelbiomasse 
			//    RLLRLay = (double)max(0.0,pLR->fLengthDens*pBiom->fRootDeathRate/(pBiom->fRootWeight+EPSILON));
				
			// C) Absterberate in der jeweiligen Schicht abhängig von der Ressourcenverfügbarkeit (PLATHO)
			// In ungünstigen Schichten sterben mehr Wurzeln ab
			if(TRLLF>0.0)
				RLLRLay = RLLR*pLR->fLengthDens*(1.0-pLR->fWatStressRoot)/TRLLF; //[cm/cm^3]
//				RLLRLay = RLLR*pLR->fLengthDens*max(1.0-pLR->fWatStressRoot,1.0-pLR->fNStressRoot)/TRLLF; //[cm/cm^3]
			else
				RLLRLay = max(0.0,pLR->fLengthDens*pPl->pBiomass->fRootDeathRate/(pPl->pBiomass->fRootWeight+EPSILON));

			RLLRLay = max((double)0.01*pLR->fLengthDens,RLLRLay);
			// 
			////////////////////////////////////////////////////////////////////////////////////////

			//  pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(double)0.01*pLR->fLengthDens;

			//SG 20120126
			pLR->fLengthDens += (pLR->fLengthDensFac*RNLF/fThickness-(double)RLLRLay)*pTS->fAct;  //[m]

			if (fCumDepth<(double)115.0)
				pLR->fLengthDens=max((double)0.0,pLR->fLengthDens);
			else
			{
			if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)
				RLVF=(double)0.377-((double)0.0015)*fCumDepth;
			else
				RLVF=((double)0.00004)*(fCumDepth-(double)220.0)*(fCumDepth-(double)220.0);
			pLR->fLengthDens=min(RLVF,pLR->fLengthDens);
			}
	    }
		
		//SG/17/05/99: Potato (ausPLAGEN)
		else if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
       	{
			pLR->fLengthDens += (pLR->fLengthDensFac*RNLF/fThickness-(double)0.005*pLR->fLengthDens)*pTS->fAct; 
			pLR->fLengthDens  = min((double)8.0, max((double)0.0,pLR->fLengthDens));
		}

		else if(strcmp(pPl->pGenotype->acCropCode,"SF")==0)
		{
			if(pPl->pDevelop->iStageCERES < 4)
				pLR->fLengthDens += pLR->fLengthDensFac*RNLF*pTS->fAct/fThickness;
			else
				pLR->fLengthDens += (pLR->fLengthDensFac*RNLF/fThickness-(double)0.005*pLR->fLengthDens)*pTS->fAct;

			pLR->fLengthDens = (double)min(10.0,max(0.0,pLR->fLengthDens));
		}
		else //For Other crops
        {
			pLR->fLengthDens += (pLR->fLengthDensFac*RNLF/fThickness
				-((double)0.005)*pLR->fLengthDens)*pTS->fAct;
			pLR->fLengthDens=min((double)5.0, max((double)0.0,pLR->fLengthDens));
        }
		
		//printf("SPASS %d %f \n", L, pLR->fLengthDens);

		pLR->fLengthDens=max(0.0,pLR->fLengthDens);
		pRT->fTotalLength += pLR->fLengthDens * fThickness; //FH 09/12 Test

		pSL =pSL->pNext;
		pLR =pLR->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext; 

	}
	
	//pRT->fTotalLength = pRT->fTotalLength*(double)10.0; //FH 09/12 Test

	return RET_SUCCESS;
	}

/*******************************************************************************
** EOF */
