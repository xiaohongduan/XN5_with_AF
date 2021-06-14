//      spass_ini.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//   

#include "spass_ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SET_IF_99(x,y) 		if (x == (double)-99)   x = (double)y;

//////////////////////////////////////////////////////////////////////////////////////////////
// INITALISATION FUNCTIONS (called from manage.c)		
//////////////////////////////////////////////////////////////////////////////////////////////

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Initialization of plant variables
------------------------------------------------------------------------------------------------------------
*/
int spass_PlantVariableInitiation(spass *self)
	{

	expertn_modul_base *xpn = &(self->parent);
	  
	PMANAGEMENT		pMa = xpn->pMa; 
	PPLANT			pPl = xpn->pPl; 
	PSPROFILE		pSo = xpn->pSo; 


	double 		CUMDEP,fThickness;
	int   		L,L1;
	char		*pCropCode;
	gchar *S;


 	PGENOTYPE 		pGT		=pPl->pGenotype;
	PDEVELOP		pDev	=pPl->pDevelop;
	PBIOMASS		pGrw	=pPl->pBiomass;
	PMODELPARAM 	pGrwP	=pPl->pModelParam;
	PCANOPY			pCan	=pPl->pCanopy;
	PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
	PMSOWINFO		pSI		=pMa->pSowInfo;
	PSLAYER 		pSL		=pSo->pSLayer;

	double fSowDepth;

	pCropCode = (char*)malloc(sizeof(pPl->pGenotype->acCropCode));   
	strcpy(pCropCode,pPl->pGenotype->acCropCode);

	if(pSI->fSowDepth>(double)0.0)
		fSowDepth = pSI->fSowDepth;
	else
		fSowDepth = (double)3.0;

	//======================================================================================
	//Initialize all variables
	//======================================================================================

	pPl->pDevelop->iStageCERES = (int)-99;
	pPl->pDevelop->fDevStage = (int)-99;
	pPl->pDevelop->fStageXCERES = (int)-99;
	pPl->pDevelop->fStageWang = (int)-99;
	pPl->pDevelop->fStageSUCROS = (int)-99;
	pPl->pDevelop->iDayAftEmerg = (int)0;
	
	pPl->pCanopy->fExpandLeafNum = (double)0.0;
	pPl->pCanopy->fTillerNum = (double)0.0;
	pSI->fPlantDens = (double)0.0;
	pCan->fLeafTipNum = (double)0.0;
	pDev->fDaylengthEff = (double)0.0;
	pPl->pOutput->fValue1 = (double)0.0;
	pPl->pOutput->fValue2 = (double)0.0;

	//======================================================================================
	//Development Stage
	//======================================================================================
	if (pPl->pDevelop->iStageCERES == (int)-99) pPl->pDevelop->iStageCERES	= (int)0;
	SET_IF_99(pPl->pDevelop->fDevStage,(double)0.0);
	SET_IF_99(pPl->pDevelop->fStageXCERES,(double)0.0);
	SET_IF_99(pPl->pDevelop->fStageWang,(double)0.0);
	SET_IF_99(pPl->pDevelop->fStageSUCROS,(double)0.0);

	
    pDev->iStageCERES= (int)0;
    //pDev->fDevStage=(double)0;
	//pDev->fStageXCERES=(double)0;
	//pDev->fStageWang=(double)0;
	pDev->fStageSUCROS=(double)-1.0;

   	if (pSI->fPlantDens == (double)0.0) pSI->fPlantDens = (double)400;
    if (pSI->fRowWidth  == (double)0.0) pSI->fRowWidth  = (double)10;
    if (pSI->fSowDepth  == (double)0.0) pSI->fSowDepth  = (double)3;

	pCan->fLAI=pSI->fPlantDens*((double)5.0E-4);

	pPl->pCanopy->fCropCoverFrac    = (double) 0.0;
	pPl->pModelParam->fMaxRootDepth = (double) 0.0;


	pCan->fPlantLA					=(double)0.16	*pSI->fPlantDens; 
	pGrw->fLeafWeight				=(double)0.00136	*(double)10.0*pSI->fPlantDens; 
	pGrw->fSeedReserv				=(double)0.048	*(double)10.0*pSI->fPlantDens; 					
	pGrw->fStemWeight				=(double)0.0		*(double)10.0*pSI->fPlantDens; 					
	pGrw->fStovWeight				= pGrw->fLeafWeight;
	pGrw->fRootWeight				=(double)2.0		*pGrw->fSeedReserv;	
	pGrw->fTotalBiomass				=(double)0.0;		

    //für CERES-Modelle
	self->iStageOld=self->iTLNO=self->iSlowFillDay=(int)0;
    self->fPHINT= self->fXDevStage= self->fP5= self->fP9= self->fO1= self->fNFAC=(double)0.0;

    //Maize:
	self->fEarWeight= self->fEarGrowR= self->fEarFraction=(double)0.0;
    self->fTasselLeafNum= self->fMaxPlantLA=(double)0.0;

    //Sunflower:
    self->fStemWeightMin= self->fStemWeightMax=(double)0.0;
    self->fHeadWeight= self->fHeadGrowR= self->fPotHeadWeight= self->fHeadWeightMin= self->fHeadWeightMax=(double)0.0;
    self->fPericarpWeight= self->fPericarpGrowR= self->fPotPericarpGrowR=(double)0.0;
    self->fEmbryoWeight= self->fEmbryoGrowR=(double)0.0;
    self->fIGrainWeight= self->fOil= self->fOilPercent=(double)0.0;
    self->fNHeadActConc= self->fNHeadMinConc= self->fNHeadOptConc= self->fNHeadDemand= self->fNHeadCont=(double)0.0;
    self->fNPericarpActConc= self->fNPericarpOptConc= self->fNPericarpDemand= self->fNPericarpCont=(double)0.0;
    self->fNEmbryoActConc= self->fNEmbryoOptConc= self->fNEmbryoDemand= self->fNEmbryoCont=(double)0.0;
    self->fNGreenLeafActConc= self->fNGreenLeafCont= self->fNSenesLeafCont=(double)0.0;
    self->fSumPS= self->fRI1= self->fRM= self->fRFR= self->fGrowFactor= self->ppp=(double)0.0;
    self->fCumSenesLA= self->fSpecLeafArea= self->fAPLA=(double)0.0;
    self->fSenesLAN2= self->fSLOPEPE= self->fXPEPE= self->fYRAT=(double)0.0;

	//======================================================================================
	//Plant nitrogen content   
	//======================================================================================
	pPltN->fRootActConc	=(double)0.02;
	pPltN->fGrainConc	=(double)0.0;
	pPltN->fTopsActConc	=(double)0.050;
	pPltN->fGrainCont	=(double)0.0;

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPltN->fLeafActConc	=(double)0.05;
	pPltN->fStemActConc	=(double)0.05;

	pPltN->fLeafRelNc	=(double)1.0;
	pPltN->fStemRelNc	=(double)1.0;
	pPltN->fRootRelNc	=(double)1.0;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    pPltN->fDeadLeafNw = (double)0.0; 
    pPltN->fDeadRootNw = (double)0.0; 

	//======================================================================================
	//Dry matter initialization
	//======================================================================================
	if ((strcmp(pCropCode,"WH")==0)
		||((strcmp(pCropCode,"BA")==0)||(strcmp(pCropCode,"S")==0))
		||((strcmp(pCropCode,"RY")==0)))
		{
	   	if (pSI->fPlantDens<(double)100)
	   		pSI->fPlantDens=(double)390;
	
		pDev->fDaylengthEff	=(double)0.01; 	

	//	pSI->fPlantDens=(double)390;

		//SG 10.11.2004 //////////////////////////////////////////////////////
		//
		//	Tausendkorngewicht (TKG)
		//	WW: 45 g
		//	BA:	45 g
		//	S:	50 g
		//	RY:	35 g
		//	TC:	45 g
		//
		// ca. 50% available carbohydrates

		if (strcmp(pCropCode,"WH")==0)
			pGrw->fSeedReserv = (double)(0.045/2.0*10.0)*pSI->fPlantDens; // kg/ha				
		if (strcmp(pCropCode,"BA")==0)
			pGrw->fSeedReserv = (double)(0.045/2.0*10.0)*pSI->fPlantDens; // kg/ha 				
		if (strcmp(pCropCode,"S")==0)
			pGrw->fSeedReserv = (double)(0.05/2.0*10.0)*pSI->fPlantDens; // kg/ha 				
		if (strcmp(pCropCode,"RY")==0)
			pGrw->fSeedReserv = (double)(0.035/2.0*10.0)*pSI->fPlantDens; // kg/ha 				

		//Biomass
		pGrw->fLeafWeight				= (double)0.5*pGrw->fSeedReserv; 
		pCan->fPlantLA					= (double)1E4*pGrw->fLeafWeight/pGT->fSpecLfWeight; 
		pGrw->fStemWeight				= (double)0.0*pGrw->fSeedReserv; 				
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				= (double)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				= (double)0.0;		
		pDev->fCumPhyll					= (double)0.0;				
		pGrwP->fSingleTillerStemWeight	= (double)0.01;		

		pCan->fLAI=pCan->fPlantLA*(double)1E-4;

/*
		// alte Initialisierung:

		//Biomass
		pCan->fPlantLA					=(double)0.04*pSI->fPlantDens; 
		pGrw->fLeafWeight				=(double)0.00034*(double)10.0*pSI->fPlantDens; 
		pGrw->fSeedReserv				=(double)0.012*(double)10.0*pSI->fPlantDens; 					
		pGrw->fStemWeight				=(double)0.0*(double)10.0*pSI->fPlantDens; 					
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				=(double)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				=(double)0.0;		
		pDev->fCumPhyll					=(double)0.0;				
		pGrwP->fSingleTillerStemWeight	=(double)0.01;		

		if (strcmp(pCropCode,"WH"))
			pCan->fLAI=pSI->fPlantDens*((double)4.0E-6);
		else
			pCan->fLAI=pSI->fPlantDens*((double)4.0E-4);

		pCan->fLAI=pSI->fPlantDens*((double)5.0E-4); //(double)0.010;//0.005;            		
*/

		pCan->fTillerNum=(double)1.0;                   
		pCan->fTillerNumSq=pSI->fPlantDens;				
	   				
	
		if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0))
		{
			pDev->fCumPhyll		=(double)0.0;
		}
	
		if ((strcmp(pCropCode,"WH")==0)||((strcmp(pCropCode,"RY")==0)))
			{
				pPltN->fLeafActConc		=(double)0.044;
				pPltN->fStemActConc		=(double)0.044;			

				pPltN->fRootActConc=(double)0.02;//0.022; CERES
				pPltN->fTopsActConc=(double)0.044;
			}

		if ((strcmp(pCropCode,"BA")==0)||((strcmp(pCropCode,"S")==0)))
			{
				pPltN->fLeafActConc		=(double)0.045;
				pPltN->fStemActConc		=(double)0.045;

				pPltN->fRootActConc=(double)0.02; //0.045; CERES
				pPltN->fTopsActConc=(double)0.045;
			}
		}

  	//==========================================================================
	//POTATO
	//==========================================================================
	if (strcmp(pCropCode,"PT")==0)
		{
		//SIMPOTATO
		pCan->fLAI			=pSI->fPlantDens*((double)4.0E-6);

		pGrw->fSeedReserv	=(double)20.0*(double)10.0*pSI->fPlantDens; //20g/plt					
		pGrw->fLeafWeight	=(double)0.1125*(double)10.0*pMa->pSowInfo->fPlantDens; //0.1125g/plt 
		pGrw->fStemWeight	=(double)0.1125*(double)10.0*pMa->pSowInfo->fPlantDens; 
		pGrw->fGrainWeight	=(double)0; 

		pPltN->fLeafActConc		=(double)0.06;
		pPltN->fStemActConc		=(double)0.05;

		pPltN->fRootActConc=(double)0.0215;
		pPltN->fTopsActConc=(double)0.08;
		}   

  	//==========================================================================
	//MAIZE
	//==========================================================================
	if (strcmp(pCropCode,"MZ")==0)
		{

		//SG 10.11.2004 //////////////////////////////////////////////////////
		//
		//	Tausendkorngewicht (TKG)
		//	MZ: 300 g
		//
		// ca. 50% available carbohydrates

		/* CERES
		pCan->fPlantLA		=(double)1.0;
		pCan->fLAI			=(double)0.0001*pSI->fPlantDens*pCan->fPlantLA;
		*/

		pDev->fCumPhyll		=(double)0.514;

		//pGrw->fSeedReserv	=(double)0.2;	
		pGrw->fSeedReserv = (double)(0.3/2.0*10.0)*pSI->fPlantDens; // kg/ha				

		pGrw->fLeafWeight				= (double)0.5*pGrw->fSeedReserv; 
		pCan->fPlantLA					= (double)1E4*pGrw->fLeafWeight/pGT->fSpecLfWeight; 
		pCan->fLAI						= pCan->fPlantLA*(double)1E-4;
		pGrw->fStemWeight				= (double)0.0*pGrw->fSeedReserv; 				
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				= (double)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				= pGrw->fSeedReserv;	

        //Biomass
		pGrw->fLeafWeight				= (double)0.5*pGrw->fSeedReserv; 
		pCan->fPlantLA					= (double)1E4*pGrw->fLeafWeight/pGT->fSpecLfWeight; 
		pGrw->fStemWeight				= (double)0.0*pGrw->fSeedReserv; 				
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				= (double)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				= (double)0.0;		
		pDev->fCumPhyll					= (double)0.0;				
		pGrwP->fSingleTillerStemWeight	= (double)0.01;		

		pCan->fLAI=pCan->fPlantLA*(double)1E-4;		/* alt
		pGrw->fLeafWeight	=(double)0.2;
		pGrw->fStemWeight	=(double)0.2;
		pGrw->fGrainWeight	=(double)0; 
		pGrw->fStovWeight	=(double)0.4;
		pGrw->fRootWeight	=(double)0.2;
		pGrw->fTotalBiomass	=pGrw->fStovWeight;	
		*/
		pPltN->fLeafActConc		=(double)0.044;
		pPltN->fStemActConc		=(double)0.044;
		
		pPltN->fRootActConc=(double)0.022;
		pPltN->fTopsActConc=(double)0.044;

		}   

  	//==========================================================================
	//Sunflower
	//==========================================================================
	if (strcmp(pCropCode,"SF")==0)
		{
		pGrw->fSeedReserv	=(double)0.2;
		self->fHeadWeight = (double)0.0;

		//OILCROP-SUN, pp.5
		pPltN->fRootActConc	=(double)0.0;
		pPltN->fLeafActConc	=(double)0.0;
		pPltN->fStemActConc	=(double)0.0;
		pPltN->fTopsActConc	=(double)0.0;

		pDev->fSumDTT = pDev->fCumDTT = pDev->fDTT = (double)0.0;

		//leaf number (= LN)
		pCan->fLeafTipNum = (double)0.0;	
		pCan->fLAI=(double)0.0;
		
		pPl->pCanopy->fExpandLeafNum = (double)0.0;

		}   

  	//==========================================================================
	//Sugarbeet
	//==========================================================================
	if (strcmp(pCropCode,"SB")==0)
		{
	   	if (pSI->fPlantDens<(double)50)
	   		pSI->fPlantDens=(double)90;
	
		//Biomass
		pCan->fPlantLA					=(double)0.16    *pSI->fPlantDens; 
		pGrw->fLeafWeight				=(double)0.00136	*(double)10.0*pSI->fPlantDens; 
		pGrw->fSeedReserv				=(double)0.05	*(double)10.0*pSI->fPlantDens; 					
		pGrw->fStemWeight				=(double)0.0	*(double)10.0*pSI->fPlantDens; 					
		pGrw->fGrainWeight	            =(double)0; 
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				=(double)0.5		*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				=(double)0.0;		

		pCan->fLAI			=pSI->fPlantDens*((double)5.0E-4);


		pPltN->fLeafActConc		=(double)0.06;
		pPltN->fStemActConc		=(double)0.0;
		pPltN->fRootActConc=(double)0.0215;
		}

	//======================================================================================
	//Dry matter initialization "pasture"
	//======================================================================================
	if (strcmp(pCropCode,"IR")==0)
	{
   		if (pSI->fPlantDens==(double)0.0) pSI->fPlantDens=(double)1000;
		pCan->fLAI=pSI->fPlantDens*((double)5.0E-4);

		pPl->pCanopy->fCropCoverFrac    = (double) 0.0;
		pPl->pModelParam->fMaxRootDepth = (double) 0.0;


		pCan->fPlantLA			= (double)1E4*pCan->fLAI; 
		pGrw->fLeafWeight		= (double)1E-4 * pCan->fPlantLA * pGT->fSpecLfWeight; 
		pGrw->fSeedReserv		= (double)0.048	*(double)10.0*pSI->fPlantDens; 					
		pGrw->fStemWeight		= pGrw->fLeafWeight; 					
		pGrw->fStovWeight		= pGrw->fLeafWeight + pGrw->fStemWeight;
		pGrw->fRootWeight		= pGrw->fStovWeight;	
		pGrw->fTotalBiomass		= pGrw->fRootWeight + pGrw->fLeafWeight + pGrw->fStemWeight;		


		//======================================================================================
		//Plant nitrogen content   (CERES, NWHEAT)
		//======================================================================================
		pPltN->fRootActConc	=(double)0.01;
		pPltN->fGrainConc	=(double)0.0;
		pPltN->fTopsActConc	=(double)0.050;
		pPltN->fGrainCont	=(double)0.0;

		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		pPltN->fLeafActConc	=(double)0.02;
		pPltN->fStemActConc	=(double)0.02;

		pPltN->fLeafRelNc	=(double)1.0;
		pPltN->fStemRelNc	=(double)1.0;
		pPltN->fRootRelNc	=(double)1.0;
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX    
		pDev->fStageSUCROS = (double)0.0;
		strcpy(pPl->pGenotype->acCropCode,"WH\0");
	} // end "pasture (IR)"

	//SG 2005/12/14: (wegen neuer N-Stress-Berechnung in Photosynthese)
	spass_NitrogenConcentrationLimits(self);

   //======================================================================================
	//Root depth and length density
	//======================================================================================
	
	pPl->pRoot->fDepth= fSowDepth;

	pLR	  =pPl->pRoot->pLayerRoot;
	pSL	  =pSo->pSLayer->pNext;
	CUMDEP=(double)0.0;

	for (L=1;L<=pSo->iLayers-2;L++)
		{
		fThickness =(double)0.1*pSL->fThickness; //cm

		CUMDEP=CUMDEP+fThickness;

	  //IF_CROP_IS_UNKNOWN
			pLR->fLengthDens=(double)0.20*pSI->fPlantDens/fThickness;
	
		if ((strcmp(pCropCode,"WH")==0)||(strcmp(pCropCode,"BA")==0))
			pLR->fLengthDens=(double)0.0024*pSI->fPlantDens/fThickness;

		if (strcmp(pCropCode,"PT")==0)
			pLR->fLengthDens=(double)0.2*pSI->fPlantDens/fThickness;

		if (CUMDEP>pPl->pRoot->fDepth)
			break;

		pSL=pSL->pNext;
		pLR=pLR->pNext;
		}

	pLR->fLengthDens = pLR->fLengthDens*
							 ((double)1.0-(CUMDEP-pPl->pRoot->fDepth)/((double)0.1*pSL->fThickness));
	
	L1=L+1;
	pLR=pLR->pNext;
	if (L1<pSo->iLayers)
		{
		for (L=L1;L<=pSo->iLayers;L++)
        	{
        	pLR->fLengthDens=(double)0.0;
        	pLR=pLR->pNext;
        	}
        }                       
      
	
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPltN->fRootCont = pPltN->fRootActConc*pGrw->fRootWeight;
    pPltN->fStovCont = pPltN->fTopsActConc*pGrw->fStovWeight;
	
    pPltN->fLeafCont = pPltN->fLeafActConc*pGrw->fLeafWeight;
    pPltN->fStemCont = pPltN->fStemActConc*pGrw->fStemWeight;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPl->pPltWater->fStressFacPartition	= (double)1;
	pPl->pPltWater->fStressFacPhoto		= (double)1;
	pPl->pPltWater->fStressFacLeaf		= (double)1;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

	//SG 2005 11 03: kein N-Mangel bei Feldaufgang (CERES)
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPl->pPltNitrogen->fNStressPhoto=(double)1.0;
	pPl->pPltNitrogen->fNStressLeaf=(double)1.0;
	pPl->pPltNitrogen->fNStressTiller=(double)1.0;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    
	pPl->pCanopy->fPlantHeight = (double)0.0;

	pCan->fLAISpass = pCan->fLAI;

// print Output Message if Modul is used from Expert N
S  = g_strdup_printf("Init MODUL SPASS");
PRINT_MESSAGE(xpn,3,S);
g_free(S);
free(pCropCode);



return RET_SUCCESS;

	}
