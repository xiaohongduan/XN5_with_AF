//      leachn_plant.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "plant.h"
#include "stickstoff_util.h"
#include "stickstoff_macros.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#define MIN_ROOT_POT (double)-150000.0
#define TOLERANCE (double)0.0001

G_DEFINE_TYPE(leachn_plant, leachn_plant, EXPERTN_MODUL_BASE_TYPE);


static void leachn_plant_class_init(leachn_plantClass *klass) {}


static void leachn_plant_init(leachn_plant *self)
{
	self->plant_measurement_filename=NULL;
	self->plant_iAlreadyInitialised=0;
}

// Our Modul Functions:


static void calc_noah_soil_dens(leachn_plant *self,double nroot)
{
	expertn_modul_base *xpn = &(self->parent);
	double sldpthN;
	int i;
	PSLAYER pSLayer;
	PLAYERROOT pLayerRoot;

	sldpthN = 0.0;
	for (i=1,pSLayer = xpn->pSo->pSLayer->pNext; i!=nroot; i++,pSLayer=pSLayer->pNext)
		{
			sldpthN+=pSLayer->fThickness;
			if (i>=nroot) break;
		}

	for (i=1,pSLayer = xpn->pSo->pSLayer->pNext,pLayerRoot = xpn->pPl->pRoot->pLayerRoot->pNext; i!=nroot; i++,pSLayer=pSLayer->pNext,pLayerRoot=pLayerRoot->pNext)
		{
			pLayerRoot->fLengthDens = pSLayer->fThickness / sldpthN;
		}
}

static void calc_cos_soil_dens(leachn_plant *self,double nroot)
{
	expertn_modul_base *xpn = &(self->parent);
	//double sldpthN;
	int i;
	PSLAYER pSLayer;
	PLAYERROOT pLayerRoot;
	double sum1;

	sum1 = 0.0;
	for (i=1,pSLayer = xpn->pSo->pSLayer->pNext,pLayerRoot = xpn->pPl->pRoot->pLayerRoot->pNext; i!=nroot; i++,pSLayer=pSLayer->pNext,pLayerRoot=pLayerRoot->pNext)
		{
			pLayerRoot->fLengthDens = cos((double) i / (double)(nroot+1) * M_PI/2.0);
			sum1 += pLayerRoot->fLengthDens;
		}

	// Normieren:
	for (i=1,pSLayer = xpn->pSo->pSLayer->pNext,pLayerRoot = xpn->pPl->pRoot->pLayerRoot->pNext; i!=nroot; i++,pSLayer=pSLayer->pNext,pLayerRoot=pLayerRoot->pNext)
		{
			pLayerRoot->fLengthDens/=sum1;
		}

}

int leachn_plant_load(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double   fMinimalRootDepth = (double)50; //mm
	char *plant_measurement_filename;
	char *waterUptakePara_filename;
	PPLTMEASURE  pPM;

	self->fRootPot = 0.0;
	xpn->pPl->pDevelop->fDevStage = 0.0;
	
    //FH 20190701 for correctly setting pPM to pPM->pNext this variable is needed
    self->iRootNTrue = 0;
    
    //added by Hong 
    if (self->waterUptakePara_filename==NULL)
		{
			// Read from INI File:
			waterUptakePara_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.leachn.plant_waterUptakePara_filename");

			if (waterUptakePara_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'plant_waterUptakePara_filename' in your configuration!");
				}
			else
				{

					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,waterUptakePara_filename);
					if (S2!=NULL)
						{
							waterUptakePara_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->waterUptakePara_filename = g_strdup_printf("%s",waterUptakePara_filename);
							waterUptakePara_load_config(self);
							free(waterUptakePara_filename);
							free(S2);
						}

				}

		}
    //End of Hong   

	if (self->plant_measurement_filename==NULL)
		{
			// Read from INI File:
			plant_measurement_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.leachn.plant_measurement_filename");

			if (plant_measurement_filename==NULL)
				{
					PRINT_ERROR("Missing entry 'leachn.plant_measurement_filename' in your configuration!");
				}
			else
				{

					char *S2;
					S2 = expertn_modul_base_replace_std_templates(xpn,plant_measurement_filename);
					if (S2!=NULL)
						{
							plant_measurement_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
							self->plant_measurement_filename = g_strdup_printf("%s",plant_measurement_filename);
							load_plant_measurement_data(self,self->plant_measurement_filename);
							free(plant_measurement_filename);
							free(S2);
						}

				}

		}


	self->IsConstRootDens = 0;
	self->iCropCover = 0;

	xpn->pPl->pModelParam->MaxRootBiomassDay = xpn->pTi->pSimTime->mday;
	xpn->pPl->pModelParam->MaxRootBiomassMonth = xpn->pTi->pSimTime->mon;
	xpn->pPl->pModelParam->MaxRootBiomassYear = xpn->pTi->pSimTime->year;
	//xpn->pMa->pSowInfo->Day = 17;
	//xpn->pMa->pSowInfo->Month = 8;
	//xpn->pMa->pSowInfo->Year= 1992;

	//xpn->pMa->pSowInfo->Day = 23;
	//xpn->pMa->pSowInfo->Month = 4;
	//xpn->pMa->pSowInfo->Year= 1993;



	for (pPM = xpn->pPl->pPMeasure;
	        (pPM->pNext != NULL) && ((xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0));
	        pPM = pPM->pNext);

	//PRINT_MESSAGE(xpn,3,"Start of Growing Season");

	xpn->pPl->pDevelop->fDevStage = 0;//pPM->pNext->fDevStage;

	//xpn->pPl->pModelParam->iEmergenceDay = pPM->iDay;
	{
		STRUCT_XPN_TIME str_time;
		xpn_time_get_struc(&str_time, pPM->iyear,pPM->fTimeY);
		xpn->pPl->pModelParam->EmergenceDay=str_time.mday;
		xpn->pPl->pModelParam->EmergenceMonth=str_time.mon;
		xpn->pPl->pModelParam->EmergenceYear=str_time.year;
	}

	if (pPM->fRootDepth < fMinimalRootDepth)
		{
			pPM->fRootDepth = fMinimalRootDepth;
		}


	while ((pPM->pNext != NULL) && (pPM->pNext->fDevStage > pPM->fDevStage))
		{
			pPM = pPM->pNext;
		}

	//pPl->pModelParam->iHarvestDay = pPM->iDay;
	{
		STRUCT_XPN_TIME str_time;
		xpn_time_get_struc(&str_time, pPM->iyear,pPM->fTimeY);
		xpn->pPl->pModelParam->HarvestDay=str_time.mday;
		xpn->pPl->pModelParam->HarvestMonth=str_time.mon;
		xpn->pPl->pModelParam->HarvestYear=str_time.year;
	}

	xpn->pPl->pRoot->fMaxDepth = pPM->fRootDepth;

	return RET_SUCCESS;
}

int leachn_plant_done(leachn_plant *self)
{
	if (self->plant_measurement_filename!=NULL)
		{
			g_free(self->plant_measurement_filename);
			self->plant_measurement_filename=NULL;
		}
	return RET_SUCCESS;
}

int load_plant_measurement_data(leachn_plant *self,char *filename)
{
	expertn_modul_base *xpn = &(self->parent);

	STRUCT_XNO_DATA *data;
	PPLTMEASURE pPltM;
	double *values;
	double fNConcCrop,fNConcDryRoot;
//	double fNConcLeaf,fNConcStem;

	int i2;

	data = xpn_load_climate_xno_file(filename,18);
	if (data==NULL) return 1;
	for (i2=0; i2<data->size_of_valuelist; i2++)
		{
			if (i2 == 0)
				{
					pPltM = (PPLTMEASURE) g_malloc0_n(1,sizeof(STPLTMEASURE));
					xpn->pPl->pPMeasure = pPltM;
					pPltM->pBack=NULL;
					pPltM->pNext=NULL;
				}
			else
				{
					pPltM->pNext= (PPLTMEASURE) g_malloc0_n(1,sizeof(STPLTMEASURE));
					pPltM->pNext->pBack=pPltM;
					pPltM=pPltM->pNext;
					pPltM->pNext=NULL;
				}

			values = data->valuelist[i2];
			pPltM->iyear = data->iyear[i2];
			pPltM->fTimeY = data->fTimeY[i2];
			//fNConcLeaf=0.0;
			//fNConcStem=0.0;
			fNConcCrop=0.0;
			fNConcDryRoot=0.0;

			pPltM->fDevStage=values[1];
			pPltM->fHeight=values[2]*10.0; // cm to mm
			pPltM->fLAI=values[3];
			pPltM->iLeafNum=(int)values[4];
			pPltM->fTillers=values[5];
			pPltM->fLeafWeight=values[6];
			pPltM->fStemWeight=values[7];
			pPltM->fGenBiomass=values[8];
			pPltM->fGrainWeight=values[8];
			pPltM->fAboveBiomass=values[9];
			//fNConcLeaf=values[10];
			//fNConcStem=values[11];							;
			fNConcCrop=values[12];
			pPltM->fNConcAbvBiomass=values[13];
			pPltM->fRootDepth = values[14]*10.0; // cm to mm
			pPltM->fRootWeight=values[15];
			fNConcDryRoot=values[16];
			pPltM->fCropCoverFrac = values[17];

			pPltM->fVegBiomass = pPltM->fLeafWeight+pPltM->fStemWeight;
			pPltM->fNAboveBiomass	= (pPltM->fAboveBiomass* pPltM->fNConcAbvBiomass)/100.0;
			pPltM->fNGenBiomass	= pPltM->fGrainWeight*fNConcCrop/100.0;
			pPltM->fNGrain		= pPltM->fGrainWeight*fNConcCrop/100.0;
			pPltM->fNRoot 		= pPltM->fRootWeight*fNConcDryRoot/100.0;
			pPltM->fTotalN		= pPltM->fNAboveBiomass+pPltM->fNRoot;
		}



	data = xpn_free_XNO_DATA(data);
	return RET_SUCCESS;
}


int waterUptakePara_load_config(leachn_plant *self)
{
	GError *error = NULL;
	expertn_modul_base *xpn = &(self->parent);
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	char *filename;
	//double *h_Feddes; 
	int afRSPar_Feddes_len;
	
	self->afRSPar_Feddes = NULL;
	afRSPar_Feddes_len=0;


	filename=g_strdup_printf("%s",self->waterUptakePara_filename);

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}

	//WATER UPTAKE MODEL PARAMETERS
	//20020
	//[Water_uptake_model_parameters]
	GET_INI_DOUBLE_ARRAY(self->afRSPar_Feddes,afRSPar_Feddes_len,"Water_uptake_model_parameters","afRSPar_Feddes");
	GET_INI_DOUBLE(self->fh50Global,"Water_uptake_model_parameters","fh50Global_vanGen");
	GET_INI_DOUBLE(self->fp1Global,"Water_uptake_model_parameters","fp1Global_vanGen");
	
    //printf("%f/n", self->fh50Global);

	g_key_file_free(keyfile);
	g_free(filename);

	return RET_SUCCESS;
}



int leachn_plant_InitiateLeachModel(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PPLTMEASURE  pPM;
//	PMODELPARAM  pMP = xpn->pPl->pModelParam;
	char *S;
	double root_max_depth;

	double   fMinimalRootDepth = (double)50;
	// Dieser Minimalwert ermöglicht Aufnahme in anderen Modulen.

	if (xpn->pPl == NULL)
		{
			return RET_SUCCESS;
		}

	if (xpn->pPl->pPMeasure == NULL)
		{
			return RET_SUCCESS;
		}
	if (self->plant_iAlreadyInitialised==0)
		{
			for (pPM = xpn->pPl->pPMeasure; pPM != NULL; pPM = pPM->pNext)
				{

					if (pPM->fAboveBiomass < EPSILON)
						{

							pPM->fAboveBiomass = (pPM->fStemWeight + pPM->fLeafWeight  + pPM->fGrainWeight > EPSILON)?
							                     pPM->fStemWeight + pPM->fLeafWeight  + pPM->fGrainWeight
							                     :
							                     (double)0;
						}
				}
			self->plant_iAlreadyInitialised=1;
		}


	/*for (xpn->pPM = pPl->pPMeasure;
	        (pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= pTi->pSimTime->fTimeAct);
	        pPM = pPM->pNext);*/

	for (pPM = xpn->pPl->pPMeasure;
	        (pPM->pNext != NULL) && ((xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0));
	        pPM = pPM->pNext);

	PRINT_MESSAGE(xpn,3,"Start of Growing Season");

	// ck: meiner Meinung nach muss es so sein:
	xpn->pPl->pDevelop->fDevStage = pPM->fDevStage;
	//xpn->pPl->pDevelop->fDevStage = pPM->pNext->fDevStage;

	//xpn->pPl->pModelParam->iEmergenceDay = pPM->iDay;
	{
		STRUCT_XPN_TIME str_time;
		xpn_time_get_struc(&str_time, pPM->iyear,pPM->fTimeY);
		xpn->pPl->pModelParam->EmergenceDay=str_time.mday;
		xpn->pPl->pModelParam->EmergenceMonth=str_time.mon;
		xpn->pPl->pModelParam->EmergenceYear=str_time.year;
	}

	//if (pPM->fRootDepth < fMinimalRootDepth)
	//	{
	//		pPM->fRootDepth = fMinimalRootDepth;
	//	}
	
	root_max_depth = fMinimalRootDepth;


	while ((pPM->pNext != NULL) && (pPM->pNext->fDevStage > pPM->fDevStage))
		{
			if (pPM->fRootDepth > root_max_depth)
				{
					root_max_depth=pPM->fRootDepth;
				}
			pPM = pPM->pNext;
		}
		
	xpn->pPl->pRoot->fMaxDepth = root_max_depth;

	//pPl->pModelParam->iHarvestDay = pPM->iDay;
	{
		STRUCT_XPN_TIME str_time;
		xpn_time_get_struc(&str_time, pPM->iyear,pPM->fTimeY);
		xpn->pPl->pModelParam->HarvestDay=str_time.mday;
		xpn->pPl->pModelParam->HarvestMonth=str_time.mon;
		xpn->pPl->pModelParam->HarvestYear=str_time.year;
	}

	

	S = g_strdup_printf("potential plant values:\n%d  mm max. root depth\n%d kg N demand\n%d LAI",(int)pPM->fRootDepth,(int)pPM->fTotalN,(int)pPM->fLAI);
	PRINT_MESSAGE(xpn,3,S);
	g_free(S);


	return RET_SUCCESS;
}

/*########### potential and actual treanspiration from XN3 ##############*///Added by Hong on 20190409
/**************************************************************/
/*  Name     : PotentialTranspiration()                       */
/*                                                            */
/*  Ein      : Zeiger auf Sammelvariable Evaporation,Zeit     */
/*             steuerung, Pflanze                             */
/*                                                            */
/*  Funktion : Belegung der Systemvariablen zur Transpiration */
/*             mit neu errechneten potentiellen               */
/*             Simulationswerten                              */
/*                                                            */
/*  Autor    : ep/gsf, ch/gsf, ep/gsf                         */
/*                                                            */
/*  Datum    : 19.08.92, 12.03.97, 02.08.00                   */
/*                                                            */
/**************************************************************/
/*  veränd. Var.:		pPW->fPotTranspdt                     */
/*						pPW->fPotTranspDay                    */
/**************************************************************/

/*************************************************************************/
/*  Name: LAI2CropCover()   */
/*  Autor    : gsf/ch     */
/*  Datum    : 17.07.97         */
/*  Purpose   : Sehr simpler Ansatz zur Umrechnung LAI -> CropCoverFrac  */
/* Da die Pflanzenmodelle LAI modellieren muss CropCoverFrac aus diesem geschätztwerden.*/
/**++++++++++++++++++**************************************************/
double LAI2CropCover(double fLAI)
{
       double fCropCoverFrac;

       fCropCoverFrac = min((double)1, fLAI / (double)3);

       return fCropCoverFrac;
       
}

int leachn_plant_pot_transpiration(leachn_plant *self)
{
    expertn_modul_base *xpn = &(self->parent);
	PWATER       pWa = xpn->pWa; 
	PPLTWATER    pPW = xpn->pPl->pPltWater;         
    PCANOPY      pPC = xpn->pPl->pCanopy;         
	PPLANT       pPl = xpn->pPl;
	PTIME        pTi = xpn->pTi;
	PMANAGEMENT   pMa= xpn->pMa;

    double  DeltaT  = pTi->pTimeStep->fAct;
    double  fLAIFactor;
	             
    if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return -1;     
  
    pPW->fPotTranspdt = (double)0;
 
    if ((pPC != NULL)&&(pWa->fPotETdt > EPSILON))
       {	 
        if ((pPC->fCropCoverFrac > EPSILON)||(pPC->fLAI > EPSILON))
	       {
             //potentielle Transpiration ohne Interzeptionsverdunstung	
	         pPW->fPotTranspdt =  MAX(0.0, pWa->fPotETdt - pWa->pEvap->fPotR*DeltaT);
	         //self->fPotTraDay =  max((double)0,pWa->fPotETDay - pEV->fPotDay);//removed by Hong
			 self->fPotTraDay  = MAX(0.0, pWa->fPotETR - pWa->pEvap->fPotR); //Added by Hong on 20190410, there is no pEV->fPotDay

			// transpiration is increased by the evaporation deficit
			 if((pWa->pEvap->fActR < pWa->pEvap->fPotR)&&(self->fPotTraDay >(double)0))
                {
                  pPW->fPotTranspdt = max((double)0,pWa->fPotETdt - pWa->pEvap->fActR*DeltaT);
	             }
    
    //potentielle Transpiration eingeschränkt bei niedrigem LAI <3 :
     fLAIFactor = LAI2CropCover(pPl->pCanopy->fLAI);
     pPW->fPotTranspdt*=fLAIFactor;
     self->fPotTraDay*=fLAIFactor;

	 }//pPC->fLAI>EPSILON
    }//pPC!=Null und fPotTranspdt > 0
    
 pPW->fPotTranspR = pPW->fPotTranspdt/DeltaT; //Added by Hong
  
 return RET_SUCCESS;
}  /* Ende potentielle Transpirations - Berechnung    */
 

/*******************************************************************/
/*  Name     : WaterUptakeNimahHanks()                             */
/*                                                                 */
/*  Eingabe  : Wurzelverteilung, Pot. Transpiration im Zeitschritt */
/*  Ausgabe  : Akt. Transpiration                                  */
/*             Veraendeung des Wassergehalts durch                 */
/*             Wurzelwasseraufnahme ---                            */
/*  Funktion : Berechnung Wasseraufnahme der Pflanze               */
/*             Veränderung des Bodenwassergehalts                  */
/*             Methode Leachn                                      */
/*  Autor    : Ch. Haberbosch                                      */
/*  Datum    : 7.1.97                                              */
/*******************************************************************/
int leachn_plant_WaterUptakeNimahHanks(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PWATER pWa = xpn->pWa;
	PMANAGEMENT   pMa= xpn->pMa;
	PSPROFILE pSo = xpn->pSo;
    PSWATER       pSWL    =xpn->pSo->pSWater; 
	PSLAYER       pSL;
	pSL		=pSo->pSLayer;
    double fContAct; //Added by Hong
	
	double DeltaT     = xpn->pTi->pTimeStep->fAct;	
	signed short int  iterat;
	
	/* ch, fRootPot is static for better performance */
	//static double  fRootPot = (double)0;		-> self	              	/*   [mm]   */
	double         relWuVert = (double)0;			              	/*   [1]    */
	double         fEffectPot[MAXSCHICHT],fPotMax,fPotMin;       /*   [mm]   */
	double         fRootDensTotal = (double)0;        			/*   [mm]   */
	double         fUptake;				        	/*   [mm]   */
	double         fUptakeLay;                        /*   [mm]   */
	double		   fd;
	
	PWLAYER       pWL;
	PLAYERROOT    pLR;
	
    int iLayer        = 0; 
	
	if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return -1; 
	
   	fUptake = 0.0;
	
    /* calculate only if Transpiration occurs */
	if ((pPl->pDevelop->fDevStage > EPSILON || self->iIsConstRootDens) && (pPl->pPltWater->fPotTranspdt > EPSILON))      
	{		
		for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))
		{
			/* comment in Leach:    lpPot->osmoticPot *= ((double)(-1.0 * 360.0));  */
			
			/* effective water potential = matric + osmotic - root resistance trerm */
			fEffectPot[iLayer] = (double)min((double)0.0,pWL->fMatPotOld)
				+ pWL->fOsmPot
				- (double)1.05 			/*  in Leach: * root flow resistance term */
				* pSo->fDeltaZ
				* (double)iLayer;
			
			fRootDensTotal += pLR->fLengthDens;
		}  /* for */
		
		
        //printf("%f \n", fRootDensTotal);
        
		fPotMax = (double)0.0;
		
		fPotMin = MIN_ROOT_POT;
		
		self->fRootPot = (double)max((fPotMin + fPotMax) / (double)2.0,(double)2.0 * self->fRootPot);
		
		if (fRootDensTotal > EPSILON)
		{
			/*   Iteration    1.. 40  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
			for (iterat=0; iterat< 50; iterat++)
//			for (iterat=0; iterat< 250; iterat++)
			{                         
				fUptake = (double)0.0;
				
				for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))
				{          
					relWuVert = pLR->fLengthDens / fRootDensTotal;
					
					/* For Uptake there must exist roots,
					root potential must be lower than effective water potential and
					effective water potential must be higher than welting point. */
					if ((relWuVert>EPSILON) && (self->fRootPot < fEffectPot[iLayer])
						&& (fEffectPot[iLayer] > MIN_ROOT_POT))
					{         
						fUptake += (pWL->fHydrCond * relWuVert * DeltaT *(fEffectPot[iLayer] - self->fRootPot)
							/ ((double)10.0));     /* [ mm/d * 1 * d * mm / mm = mm ] */
					}/* if  */
					
					//if(fUptake > pPl->pPltWater->fPotTranspdt + TOLERANCE)
                    if(fUptake > pPl->pPltWater->fPotTranspR *DeltaT + TOLERANCE) //changed by Hong 
					{
						break;
					}
				} /* for */
				
				/* if Uptake too high increase root potential */
				//if (fUptake > pPl->pPltWater->fPotTranspdt + TOLERANCE)
				if (fUptake > pPl->pPltWater->fPotTranspR * DeltaT + TOLERANCE)
				{
					fPotMin  = self->fRootPot;
					self->fRootPot = (fPotMin + fPotMax) / (double)2.0;
					
				}  /* if */
				else 
					/* if Uptake too low decrease root potential */
					//if (fUptake < pPl->pPltWater->fPotTranspdt - TOLERANCE)
					if (fUptake < pPl->pPltWater->fPotTranspR * DeltaT - TOLERANCE)              
				{                                            
					fPotMax  = self->fRootPot;
					self->fRootPot = ((fPotMin + fPotMax)/(double)2.0);
					
				}  /* else if */                         
				else
					/* Uptake is ok */
				{
					break;
				}                 
			}  /*  Iteration   */
			
			fUptake = (double)0;  
			
			for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))
			{          
				relWuVert = pLR->fLengthDens / fRootDensTotal;
				
				/* For Uptake there must exist roots,
				root potential must be lower than effective water potential and
				effective water potential must be higher than welting point. */
				if ((relWuVert>EPSILON) && (self->fRootPot < fEffectPot[iLayer])
					&& (fEffectPot[iLayer] > MIN_ROOT_POT))
				{         
					fUptakeLay = (pWL->fHydrCond * relWuVert * DeltaT *(fEffectPot[iLayer] - self->fRootPot)
						/ ((double)10.0));     /* [ mm/d * 1 * d * mm / mm = mm ] */
					
					
					// wateruptake in each layer is realized by decreasing
					// the actual watercontent in this layer:
					fd = fUptakeLay / pSo->fDeltaZ;
					
					//Added by Hong after SPASS on 20190411
/*					fContAct  = pWL->fContAct; 
					fContAct -= fd; 
					if (fContAct<=1.1*pSWL->fContPWP)
						{
						fd *= fContAct/pSWL->fContPWP*xpn->pTi->pTimeStep->fAct/pSL->fThickness;
						fd = 0;
						} */
					//End of Hong
					
					pWL->fContAct -= fd;  /* [1] */
					
					pLR->fPotLayWatUpt          = fUptakeLay / DeltaT;   /* [mm/day] */					
					fUptake += fUptakeLay;
				}/* if  */
				
				
			} /* for */
			
			//if (fUptake > pPl->pPltWater->fPotTranspdt + (double)0.1)
			if (fUptake > pPl->pPltWater->fPotTranspR * DeltaT + (double)0.1)
			{
				PRINT_ERROR_ID(xpn,"Water updake too high");
				
			}    
		
		} /* Wasserentzug möglich */
		else
		{
			PRINT_ERROR_ID(xpn,"No Roots");
        }
				
	}   /*  if Transpiration */

    xpn->pPl->pPltWater->fActTranspR = fUptake/DeltaT; //Added by Hong
	
	return RET_SUCCESS;
} /*  ende   WaterUptake    */



/*******************************************************************/
/*  Name     : WaterUptakeFeddes()                                 */
/*                                                                 */
/*  Eingabe  : Wurzelverteilung, Pot. Transpiration im Zeitschritt */
/*  Ausgabe  : Akt. Transpiration                                  */
/*             Veraendeung des Wassergehalts durch                 */
/*             Wurzelwasseraufnahme ---                            */
/*  Funktion : Berechnung Wasseraufnahme der Pflanze               */
/*             Veränderung des Bodenwassergehalts                  */
/*  Autor    : St.Achatz                                           */
/*  Datum    : 19.02.98                                            */
/*******************************************************************/

//--------------- Constants and Subroutines for WaterUptakeFeddes ---------------
/*double afRSPar[8] = {		(double)9999.0, 

							(double)-120000.0,		//Par1 --- h3
							(double)-8000.0,			//Par2 \__ h2
							(double)-2000.0,			//Par3 /
							(double)-250.0,			//Par4 --- h1
							(double)-100.0,			//Par5 --- h0
							(double)0.1,				//Par6
							(double)0.5}	;			//Par7

*/ //Removed by Hong,parameters are load from ini and saved to self->afRSPar_Feddes						
double FAlfa(leachn_plant *self, double rRoot,double h)
{
	double res;
	double p0,p1,p2,p3;
	
	p0 = self->afRSPar_Feddes[5];
	p1 = self->afRSPar_Feddes[4];
	p3 = self->afRSPar_Feddes[1];
	
	if      (rRoot < self->afRSPar_Feddes[6]) p2 = self->afRSPar_Feddes[2];
	else if (rRoot > self->afRSPar_Feddes[7]) p2 = self->afRSPar_Feddes[3];
	else                     
		p2 = (self->afRSPar_Feddes[2]*(self->afRSPar_Feddes[7] - rRoot) + 
		self->afRSPar_Feddes[3]*(rRoot - self->afRSPar_Feddes[6]))  /  (self->afRSPar_Feddes[7] - self->afRSPar_Feddes[6]);
	res = (double)0.0;
	
	if      (h >  p3  &&  h < p2) res = (h-p3)/(p2-p3);
	else if	(h >= p2  &&  h < p1) res = (double)1.0;
	else if (h >= p1  &&  h < p0) res = (h-p0)/(p1-p0);
	
	return res;
}


/*****************************/
/* FEDDES  UPTAKE  ROUTINE   */
/*****************************/

int leachn_plant_WaterUptakeFeddes(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PWATER pWa = xpn->pWa;
	PSPROFILE pSo = xpn->pSo;
	PMANAGEMENT   pMa= xpn->pMa;
	PWLAYER       pWL;
	PLAYERROOT    pLR;
	PSWATER       pSWL    =xpn->pSo->pSWater; 
	PSLAYER       pSL;
	pSL		=pSo->pSLayer;
	double DeltaT     = xpn->pTi->pTimeStep->fAct;
		
	int				L;
	double			rRoot;
	double			dxM;
	double			Alfa;
	double			fEffectPot[MAXSCHICHT];
	double			fRootDensTotal;
	double			Sink;
	double          fUptake; //Added by Hong
	double          fContAct;
	int iLayer        = 0; 
	
	fUptake = 0.0;	
	
	if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return -1; //?
	
//	if ((pPl->pDevelop->fDevStage < EPSILON && !self->iIsConstRootDens) || 
//		(pPl->pPltWater->fPotTranspdt < EPSILON)) return -1; //removed by Hong
	
	fRootDensTotal = (double)0.0;
	/* calculate only if Transpiration occurs */
	if ((pPl->pDevelop->fDevStage > EPSILON || self->iIsConstRootDens) && (pPl->pPltWater->fPotTranspdt > EPSILON))   //Added by Hong   
	{
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))  
	{
		fEffectPot[iLayer] = (double)min((double)0.0,pWL->fMatPotOld);
		fRootDensTotal += pLR->fLengthDens;
	}  /* for */
	
//	if (fRootDensTotal <= (double)0.0) return -1; //removed by hong

	rRoot = pPl->pPltWater->fPotTranspdt;
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))  
	{
		//if (pLR->fLengthDens > (double)0.0) //removed by Hong
        if (fRootDensTotal > EPSILON) //Added by Hong
		{
			dxM  = pSo->fDeltaZ;
			//SG20150930: Für Irenbe Witte - Feddes-alpha kann optional = 1 (const.) gesetzt werden
			if (self->afRSPar_Feddes[0] ==0)
				Alfa = (double)1.0;
			else
				Alfa = FAlfa(self, rRoot,fEffectPot[iLayer]);	

			Sink = Alfa * pLR->fLengthDens / fRootDensTotal * rRoot/dxM;
			
			//Added by Hong after SPASS on 20190411
		    fContAct  = pWL->fContAct; 
			fContAct -= Sink; 
		    if (fContAct<=1.01*pSWL->fContPWP)
			   {
			    Sink *= fContAct/pSWL->fContPWP*xpn->pTi->pTimeStep->fAct/pSL->fThickness;
			   }
		    //End of Hong
						
			pWL->fContAct       -= Sink;
									
			pLR->fPotLayWatUpt   = Sink * dxM / DeltaT; 
			
			fUptake += Sink * dxM; //Added by Hong 
			
		} //if
		else pLR->fPotLayWatUpt = (double)0.0;
	} //for

	// hp 221002: fPotUptakedt wird in CERES zur Berechnung der Stressfaktoren
    //            benoetigt und wird danach gleich Null gesetzt (einmal pro Tag!).

	L 	  = 1;
	pLR	=pPl->pRoot->pLayerRoot; 
    pPl->pPltWater->fPotUptakedt = (double)0.0;

    while (((pLR->fLengthDens!=(double)0.0)||(pLR->pNext->fLengthDens !=(double)0.0))
		  &&(L<pSo->iLayers-2))
		{
			pPl->pPltWater->fPotUptakedt += pLR->fPotLayWatUpt*DeltaT;
			L 	 ++;
			pLR =pLR ->pNext;
		}

}/* End if Transpiration */

	xpn->pPl->pPltWater->fActTranspR = fUptake/DeltaT; //Added by Hong
	
	return RET_SUCCESS;
}


/*******************************************************************/
/*  Name     : WaterUptakeVanGenuchten()                           */
/*                                                                 */
/*  Eingabe  : Wurzelverteilung, Pot. Transpiration im Zeitschritt */
/*  Ausgabe  : Akt. Transpiration                                  */
/*             Veraendeung des Wassergehalts durch                 */
/*             Wurzelwasseraufnahme ---                            */
/*  Funktion : Berechnung Wasseraufnahme der Pflanze               */
/*             Veränderung des Bodenwassergehalts                  */
/*  Autor    : St.Achatz                                           */
/*  Datum    : 19.02.98                                            */
/*******************************************************************/

//--------------- Constants and Subroutines for WaterUptakeVanGnuchten ---------------



double FWStrs(double h, double h50, double p1)
{
	return (double)(1.0/(1.0 + abspowerDBL( min(h,(double)-1.0e-20)/h50 , p1 )));
}


/***********************************/
/* VanGenuchten   UPTAKE  ROUTINE   */
/***********************************/

int leachn_plant_WaterUptakeVanGenuchten(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;
	PWATER pWa = xpn->pWa;
	PSPROFILE pSo = xpn->pSo;
	PWLAYER       pWL;
	PLAYERROOT    pLR;	
	PSWATER       pSWL    =xpn->pSo->pSWater; 
	PSLAYER       pSL;
	pSL		=pSo->pSLayer;
	double DeltaT     = xpn->pTi->pTimeStep->fAct;
	
	double			rRoot;
	double			dxM;
	double			Alfa;
	double			fEffectPot[MAXSCHICHT];
	double			fRootDensTotal;
	double			Sink;
	double          fUptake;	//Added by Hong	        				/*   [mm]   */
	double          fContAct;
	int iLayer        = 0;
	fUptake = 0.0;
	
	if (pPl == NULL) return -1;
	/*if ((pPl->pDevelop->fDevStage < EPSILON  && !self->iIsConstRootDens) || 
		(pPl->pPltWater->fPotTranspdt < EPSILON)) return -1;*/ //removed by Hong
	
	fRootDensTotal = (double)0.0;
	/* calculate only if Transpiration occurs */
	if ((pPl->pDevelop->fDevStage > EPSILON || self->iIsConstRootDens) && (pPl->pPltWater->fPotTranspdt > EPSILON))   //Added by Hong   
	{
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))  
	{
		fEffectPot[iLayer] = (double)min((double)0.0,pWL->fMatPotOld);
		fRootDensTotal += pLR->fLengthDens;
	}  /* for */
	
	//if (fRootDensTotal <= (double)0.0) return -1;//Removed by Hong

	rRoot = pPl->pPltWater->fPotTranspdt;
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))  
	{
		//if (pLR->fLengthDens > (double)0.0) //Removed by Hong
		if (fRootDensTotal > EPSILON) //Added by Hong
		{
			dxM  = pSo->fDeltaZ;
			Alfa = FWStrs(fEffectPot[iLayer], self->fh50Global, self->fp1Global);		  
			Sink = Alfa * pLR->fLengthDens / fRootDensTotal * rRoot/dxM;

            //Added by Hong after SPASS on 20190411
		    fContAct  = pWL->fContAct; 
			fContAct -= Sink; 
		    if (fContAct<=1.01*pSWL->fContPWP)
			   {
			    Sink *= fContAct/pSWL->fContPWP*xpn->pTi->pTimeStep->fAct/pSL->fThickness;
			   }
		    //End of Hong
			
			pWL->fContAct       -= Sink;
			pLR->fPotLayWatUpt   = Sink * dxM / DeltaT; 
			
			fUptake += Sink * dxM; //Added by Hong 
		} //if
		else pLR->fPotLayWatUpt = (double)0.0;
	 } //for

	} /*  if Transpiration */
	
	xpn->pPl->pPltWater->fActTranspR = fUptake/DeltaT; //Added by Hong
	
	return RET_SUCCESS;
}
//End of Hong on 20190410

int leachn_plant_act_transpiration_CK(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);

//	PCHEMISTRY pCh = xpn->pCh;
//	PCLIMATE pCl = xpn->pCl;
//    PHEAT pHe = xpn->pHe ;
//    PLOCATION pLo = xpn->pLo;
//    PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
//    PTIME pTi = xpn->pTi;
	PWATER pWa = xpn->pWa;
	PSWATER       pSWL    =xpn->pSo->pSWater; 
	PSLAYER       pSL;
	pSL		=pSo->pSLayer;
    double fContAct; //Added by Hong
	int iLayer        = 0;
	double DeltaT     = xpn->pTi->pTimeStep->fAct;
//   double SimTime    = xpn->pTi->pSimTime->fTimeAct;

	signed short int  iterat;

	/* ch, fRootPot is static for better performance */
	double         relWuVert;			                  		/*   [1]    */
	double         fEffectPot[MAXSCHICHT],fPotMax,fPotMin;       /*   [mm]   */
	double         fRootDensTotal = (double)0;        			/*   [mm]   */
	double         fUptake;				        				/*   [mm]   */
	double         fUptakeLay;                                   /*   [mm]   */
	double		  fd;

	PWLAYER       pWL;
	PLAYERROOT    pLR;
//	PROOT         pRO;

//	pRO = xpn->pPl->pRoot;

	fUptake = 0.0;

	/* calculate only if Transpiration occurs */
	if ((xpn->pPl->pDevelop->fDevStage > EPSILON || self->iIsConstRootDens) && (xpn->pPl->pPltWater->fPotTranspR > EPSILON))
		{

			for (SOIL2_LAYERS1(pWL, xpn->pWa->pWLayer->pNext, pLR, xpn->pPl->pRoot->pLayerRoot->pNext))
				{
					/* comment in Leach:    lpPot->osmoticPot *= ((double)(-1.0 * 360.0));  */

					/* effective water potential = matric + osmotic - root resistance trerm */
					fEffectPot[iLayer] = (double)min((double)0.0,pWL->fMatPotOld)
					                     + pWL->fOsmPot
					                     - (double)1.05 			/*  in Leach: * root flow resistance term */
					                     * pSo->fDeltaZ
					                     * (double)iLayer;

					fRootDensTotal += pLR->fLengthDens;
				}  /* for */


			fPotMax = (double)0.0;

			fPotMin = MIN_ROOT_POT;

			self->fRootPot = (double)max((fPotMin + fPotMax) / (double)2.0,(double)2.0 * self->fRootPot);

			if (fRootDensTotal > EPSILON)
				{
					/*   Iteration    1.. 40  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
					for (iterat=0; iterat< 50; iterat++)
//			for (iterat=0; iterat< 250; iterat++)
						{
							fUptake = (double)0.0;

							for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))
								{
									relWuVert = pLR->fLengthDens / fRootDensTotal;

									/* For Uptake there must exist roots,
									root potential must be lower than effective water potential and
									effective water potential must be higher than welting point. */
									if ((relWuVert>EPSILON) && (self->fRootPot < fEffectPot[iLayer])
									        && (fEffectPot[iLayer] > MIN_ROOT_POT))
										{
											fUptake += (pWL->fHydrCond * relWuVert * DeltaT *(fEffectPot[iLayer] - self->fRootPot)
											            / ((double)10.0));     /* [ mm/d * 1 * d * mm / mm = mm ] */
										}/* if  */

									if(fUptake > pPl->pPltWater->fPotTranspR *DeltaT + TOLERANCE)
										{
											break;
										}
								} /* for */

							/* if Uptake too high increase root potential */
							if (fUptake > pPl->pPltWater->fPotTranspR * DeltaT + TOLERANCE)
								{
									fPotMin  = self->fRootPot;
									self->fRootPot = (fPotMin + fPotMax) / (double)2.0;

								}  /* if */
							else
								/* if Uptake too low decrease root potential */
								if (fUptake < pPl->pPltWater->fPotTranspR * DeltaT - TOLERANCE)
									{
										fPotMax  = self->fRootPot;
										self->fRootPot = ((fPotMin + fPotMax)/(double)2.0);

										/* if fRootPot near to minimal potential
										ch, because of the iteration procedure the potential cannot reach minimal value.
										if (fRootPot < MIN_ROOT_POT * (double)0.95)
										{
										break;
										}       */

									}  /* else if */
								else
									/* Uptake is ok */
									{
										break;
									}
						}  /*  Iteration   */

					fUptake = (double)0;

					for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot->pNext))
						{
							relWuVert = pLR->fLengthDens / fRootDensTotal;

							/* For Uptake there must exist roots,
							root potential must be lower than effective water potential and
							effective water potential must be higher than welting point. */
							if ((relWuVert>EPSILON) && (self->fRootPot < fEffectPot[iLayer])
							        && (fEffectPot[iLayer] > MIN_ROOT_POT))
								{
									fUptakeLay = (pWL->fHydrCond * relWuVert * DeltaT *(fEffectPot[iLayer] - self->fRootPot)
									              / ((double)10.0));     /* [ mm/d * 1 * d * mm / mm = mm ] */


									// wateruptake in each layer is realized by decreasing
									// the actual watercontent in this layer:
									fd = fUptakeLay / pSo->fDeltaZ;
									
									//Added by Hong after SPASS on 20190411
/*									fContAct  = pWL->fContAct; 
			                        fContAct -= fd; 
		                           if (fContAct<=1.1*pSWL->fContPWP)
			                       {
			                        fd *= fContAct/pSWL->fContPWP*xpn->pTi->pTimeStep->fAct/pSL->fThickness;
									fd = 0;
			                         } */
		                            //End of Hong
																		
									pWL->fContAct -= fd;  /* [1] */

									pLR->fPotLayWatUpt          = fUptakeLay / DeltaT;   /* [mm/day] */

									fUptake += fUptakeLay;
								}/* if  */


						} /* for */

					/*
								//Uptake wird jetzt hier berechnet!
								pPl->pRoot->fUptakeR = (double)0;

								for (SOIL2_LAYERS1(pWL,pWa->pWLayer->pNext,pLR,pPl->pRoot->pLayerRoot->pNext))
								{
									// this is the sink term in Richards Equation:
									pLR->fActLayWatUpt   = (pWL->fContOld- pWL->fContAct) /  pTi->pTimeStep->fAct; // [1/day]

									// It is assumed that H2O uptake only takes place, when soil is not freezing or thawing
									if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
									{
										pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * pSo->fDeltaZ;  // [mm/day]
									}
								} // for

					*/
					if (fUptake > pPl->pPltWater->fPotTranspR * DeltaT + (double)0.1)
						{
							PRINT_ERROR_ID(xpn,"H2O updake too high");

						}
					/*
					else
					if (fUptake < pPl->pPltWater->fPotTranspR - (double)0.1)
					{
					// this is no error!
					Message(0,ERROR_H2O_UPTAKE_TOO_LOW);

					  }
					*/

				} /* Wasserentzug möglich */
			else
				{
					PRINT_ERROR_ID(xpn,"No Roots");


				}
        xpn->pPl->pPltWater->fActTranspR = fUptake/DeltaT;

		}   /*  if Transpiration */


	return RET_SUCCESS;
}

int leachn_plant_act_biomass_growth(leachn_plant *self)
{
	leachn_plant_LeafMaizeLeach(self);
	//leachn_plant_RootMaizeLeach(self);
	return RET_SUCCESS;
}

int leachn_plant_LeafMaizeLeach(leachn_plant *self) //CH
{
	expertn_modul_base *xpn = &(self->parent);


// Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
// pPl-> wird hochgzählt zu pPl = pPl->pNext->pNext->;
	PPLTMEASURE    pPM = xpn->pPl->pPMeasure;
	PCANOPY        pPC = xpn->pPl->pCanopy;

	double    fFracGrowS;             // fraction of growing season
	double    fTotalTime;             // Gesamtdauer des Wachstums
	double    f1;
	double timetomeasure;

	if(self->iIsConstRootDens)
		{
			/*
			if((pPM->pNext != NULL) && (pPC->fLAI == (double)0))
			   {
				fTotalTime = max((double)(pPM->pNext->iDay  - pPM->iDay + (double)1), (double)1);
				pPC->fLAI  = pPM->fLAI+(pPM->pNext->fLAI-pPM->fLAI)*
					                                      ((int)SimTime-pPM->iDay)/fTotalTime;
			   }


			if((pPM->pNext != NULL) && (pPC->fCropCoverFrac == (double)0))
			   {
				fTotalTime          = max((double)(pPM->pNext->iDay  - pPM->iDay + (double)1), (double)1);
				pPC->fCropCoverFrac = pPM->fCropCoverFrac+(pPM->pNext->fCropCoverFrac-pPM->fCropCoverFrac)*
					                                      ((int)SimTime-pPM->iDay)/fTotalTime;
				//pPC->fCropCoverFrac = min((double)1,pPM->fCropCoverFrac);
			   }
			*/
			///*
			if (NewDay(xpn->pTi)) //now next pPM assigned in time.c //ep 250907
				{
					//while ((pPM->pNext->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime)) //ep 250907
					{
						//pPM = pPM->pNext; //ep 250907
						//pPC->fLAI = pPM->fLAI;
						//fTotalTime    = max((double)(pPM->pNext->iDay  - pPM->iDay + (double)1), (double)1);
						fTotalTime    = max(xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY)+1.0,1.0);
						//pPC->fLAI  = pPM->fLAI+(pPM->pNext->fLAI-pPM->fLAI)*
						//             ((int)SimTime-pPM->iDay)/fTotalTime;
						pPC->fLAI  = pPM->fLAI+(pPM->pNext->fLAI-pPM->fLAI)*
						             (xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY))/fTotalTime;
						if (self->iCropCover == (int)0)
							pPC->fCropCoverFrac = pPM->fCropCoverFrac+(pPM->pNext->fCropCoverFrac-pPM->fCropCoverFrac)*
							                      (xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY))/fTotalTime;
						//pPC->fCropCoverFrac = min((double)1,pPM->fCropCoverFrac);
					}
				}
			//*/

			// if(iIsConstRootDens)
		}
	else
		{

			//if (NewDay(xpn->pTi))
				{
					/* //now next pPM assigned in time.c //ep 250907
					while ((pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime))
					   {
					   pPM = pPM->pNext;
					   }
					*/
					
/*                    if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY)>=0)
                                printf("GT Biom pPM time %d %d %.17f  %d %.17f\n",xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY), xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY);
                    if(xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY)<0)
                                printf("GT Biom next pPM time %d %d %.17f  %d %.17f\n",xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY), xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY);
                    */

                    if(GROWING_TIME)
						{
							
							
							if(xpn->pPl->pDevelop->fDevStage==0)
								{
									leachn_plant_InitiateLeachModel(self);
								}
								
							xpn->pPl->pDevelop->iDayAftEmerg=1;


							//xpn->pPl->pDevelop->iDayAftEmerg  
							
							timetomeasure = xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);

							//fTotalTime    = max((double)(pPM->pNext->iDay  - pPM->iDay + (double)1), (double)1);
							fTotalTime    = max(xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY)+1.0,1.0);

							//fFracGrowS = xpn->pPl->pDevelop->iDayAftEmerg/fTotalTime;
							fFracGrowS = timetomeasure/fTotalTime;


							// *********************************************************************
							//                        Leaf Area Index

							f1 = (double)6.0 - ((double)12.0 * fFracGrowS);

							/* Lasse als Anfangswert auch Werte > 0 zu.*/
							pPC->fLAI = pPM->fLAI  + (pPM->pNext->fLAI - pPM->fLAI) / ((double)1.0 + (double)exp(f1));
							pPC->fCropCoverFrac=pPM->fCropCoverFrac+(pPM->pNext->fCropCoverFrac-pPM->fCropCoverFrac)
							                    / ((double)1.0 + (double)exp(f1));
												
							
							
							
							
							// lineare interpolation: (totaltime - 1: weil sonst ein tag zu viel)
							xpn->pPl->pDevelop->fDevStage = pPM->fDevStage + (pPM->pNext->fDevStage - pPM->fDevStage) / (fTotalTime-1.0) * timetomeasure;						
							
                            //fStageSUCROS wird für kc-Faktor benoetigt, zunächst nur Default-Werte zurückgerechnet von Wang (SPASS)
                            xpn->pPl->pDevelop->fStageWang =	xpn->pPl->pDevelop->fDevStage/10.0;
                            xpn->pPl->pDevelop->fStageSUCROS = xpn->pPl->pDevelop->fStageWang / 4.6;
                            
                            //pPC->fCropCoverFrac = min((double)1,pPM->fCropCoverFrac);
						
							xpn->pPl->pBiomass->fBiomassAbvGround=pPM->fAboveBiomass  + (pPM->pNext->fAboveBiomass - pPM->fAboveBiomass) / ((double)1.0 + (double)exp(f1));
							xpn->pPl->pBiomass->fStemWeight=pPM->fStemWeight  + (pPM->pNext->fStemWeight - pPM->fStemWeight) / ((double)1.0 + (double)exp(f1));
							xpn->pPl->pBiomass->fLeafWeight=pPM->fLeafWeight  + (pPM->pNext->fLeafWeight - pPM->fLeafWeight) / ((double)1.0 + (double)exp(f1));
							xpn->pPl->pBiomass->fGrainWeight=pPM->fGrainWeight  + (pPM->pNext->fGrainWeight - pPM->fGrainWeight) / ((double)1.0 + (double)exp(f1));
							xpn->pPl->pBiomass->fFruitWeight =xpn->pPl->pBiomass->fGrainWeight;
							
							xpn->pPl->pBiomass->fRootWeight=pPM->fRootWeight  + (pPM->pNext->fRootWeight - pPM->fRootWeight) / ((double)1.0 + (double)exp(f1));

                            // FH 2019-07-11 Calculation of plant density using the input (fTillers)
                            xpn->pPl->pCanopy->fPlantDensity = pPM->fTillers + (pPM->pNext->fTillers - pPM->fTillers) / ((double)1.0 + (double)exp(f1));
                            

							// FH 2018-01-30 Für Biomasseverlust von Bäumen
							xpn->pPl->pBiomass->fDeadLeafWeight = 0.0;
							xpn->pPl->pBiomass->fDeadStemWeight = 0.0;
							xpn->pPl->pBiomass->fDeadRootWeight = 0.0;
							if (pPM->pNext->fLeafWeight < pPM->fLeafWeight)
								{
								xpn->pPl->pBiomass->fDeadLeafWeight += (pPM->pNext->fLeafWeight - pPM->fLeafWeight) / ((double)1.0 + (double)exp(f1));
								}						
							if (pPM->pNext->fStemWeight < pPM->fStemWeight)
								{
								xpn->pPl->pBiomass->fDeadStemWeight += (pPM->pNext->fStemWeight - pPM->fStemWeight) / ((double)1.0 + (double)exp(f1));
								}
							if (pPM->pNext->fRootWeight < pPM->fRootWeight)
								{
								xpn->pPl->pBiomass->fDeadRootWeight += (pPM->pNext->fRootWeight - pPM->fRootWeight) / ((double)1.0 + (double)exp(f1));
								}
									
							//FH 2018-01-26 Pflanzenhöhe eingefügt: Umrechnung in m (pPC->fPlantHeight)
							pPC->fPlantHeight = pPM->fHeight + (pPM->pNext->fHeight - pPM->fHeight) / ((double)1.0 + (double)exp(f1));
							pPC->fPlantHeight /= 1000.;

						} // Growing Time
					else
						{
							if(xpn->pPl->pDevelop->fDevStage!=0)
								{
									xpn->pPl->pDevelop->fDevStage=(double)0;
									PRINT_MESSAGE(xpn,3,"End of Growing Season");
								}

							pPC->fLAI           = (double) 0.0;
							pPC->fCropCoverFrac = (double) 0.0;
							xpn->pPl->pDevelop->iDayAftEmerg=0;
						}  //else


                        // 20190705 FH: Old formulation  changed to something better readable and added Root-N-flag to determine when to apply pNext
/*					for (pPM = xpn->pPl->pPMeasure;
						 (pPM->pNext != NULL) && ((xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0));
							pPM = pPM->pNext);
                            {                   
                                        xpn->pPl->pPMeasure=pPM;        
                            }*/
                if (self->iRootNTrue < 1)
                {       
                    if ((pPM->pNext != NULL) && (xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0))
                        {
                            pPM = xpn->pPl->pPMeasure;
                            pPM = pPM->pNext;
                            xpn->pPl->pPMeasure=pPM;
                        }    
                }
            } //NewDay


		}
	return RET_SUCCESS;
}

int leachn_plant_RootMaizeLeach(leachn_plant *self)
{
	expertn_modul_base *xpn = &(self->parent);
	//int OK            = 1;
	int iLayer        = 0;
	//double DeltaT     = xpn->pTi->pTimeStep->fAct;


// Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
// pPl-> wird hochgezählt zu pPl = pPl->pNext->pNext->;
	PPLTMEASURE    pPM = xpn->pPl->pPMeasure;
	PCANOPY        pPC = xpn->pPl->pCanopy;
	PLAYERROOT     pLR = xpn->pPl->pRoot->pLayerRoot;

	double    fTimeConst;             // TIM normierte Zeit seit Pflanzung                                  // t=MaxWurzelTiefe+1  => TIM = 113
	//double    fTimeBreak;             // Zeitpunkt des Umschaltens der Wurzelverteilung
	double    HalbWert;               // [mm] Tiefe, in der WL-Dichte 1/2 des Max.Werts
	double    maxWuLae;               // [m/m] Rmax max. Wurzellängendichte an Boden-Oberfläche
	// !! LeachM: [m Wurzel/mm Tiefe] ExpertN: [cm / cm^3] !!
	double    fPlantDensity;          // Konstant gewählt  [1/m^2]
	double    Tiefe;                  // [mm]
	double    fFracGrowS;             // fraction of growing season
	double    fTotalTime;             // Gesamtdauer des Wachstums, bzw. zwischen zwei Messungen
	double    f1;
	double timetomeasure;
//	double   fMinimalRootDepth = (double)50;

    if (self->iRootNTrue < 1)
        {
        self->iRootNTrue = 1;
        }
    
	if(self->iIsConstRootDens)
		{
//	  if(SimStart(pTi))
			for ((pLR=xpn->pPl->pRoot->pLayerRoot->pNext, iLayer = 1); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
				pLR->fLengthDens = self->afConstRootDens[iLayer];
		}
	else
		{
            
            //20190705 FH: Use Plant Numbers (fTillers) from input file as plant density in case there is a value specified
            if (pPC->fPlantDensity > 0)
            {
                fPlantDensity = pPC->fPlantDensity;
            }
            else
            {
            fPlantDensity = (double)100;
            }
            
			/*if (pPM->pNext != NULL)
						{
							if (xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0)
								{
									xpn->pPl->pPMeasure=pPM = xpn->pPl->pPMeasure->pNext;
									xpn->pPl->pModelParam->MaxRootBiomassDay = xpn->pTi->pSimTime->mday;
									xpn->pPl->pModelParam->MaxRootBiomassMonth = xpn->pTi->pSimTime->mon;
									xpn->pPl->pModelParam->MaxRootBiomassYear = xpn->pTi->pSimTime->year;
									//xpn->pMa->pSowInfo->Day = 17;
									//xpn->pMa->pSowInfo->Month = 8;
									//xpn->pMa->pSowInfo->Year= 1992;

									//xpn->pMa->pSowInfo->Day = 23;
									//xpn->pMa->pSowInfo->Month = 4;
									//xpn->pMa->pSowInfo->Year= 1993;



									for (pPM = xpn->pPl->pPMeasure;
									        (pPM->pNext != NULL) && ((xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0));
									        pPM = pPM->pNext);

									//PRINT_MESSAGE(xpn,3,"Start of Growing Season");

									xpn->pPl->pDevelop->fDevStage = 0;//pPM->pNext->fDevStage;

									//xpn->pPl->pModelParam->iEmergenceDay = pPM->iDay;
									{
										STRUCT_XPN_TIME str_time;
										xpn_time_get_struc(&str_time, pPM->iyear,pPM->fTimeY);
										xpn->pPl->pModelParam->EmergenceDay=str_time.mday;
										xpn->pPl->pModelParam->EmergenceMonth=str_time.mon;
										xpn->pPl->pModelParam->EmergenceYear=str_time.year;
									}

									if (pPM->fRootDepth < fMinimalRootDepth)
										{
											pPM->fRootDepth = fMinimalRootDepth;
										}


									while ((pPM->pNext != NULL) && (pPM->pNext->fDevStage > pPM->fDevStage))
										{
											pPM = pPM->pNext;
										}

									//pPl->pModelParam->iHarvestDay = pPM->iDay;
									{
										STRUCT_XPN_TIME str_time;
										xpn_time_get_struc(&str_time, pPM->iyear,pPM->fTimeY);
										xpn->pPl->pModelParam->HarvestDay=str_time.mday;
										xpn->pPl->pModelParam->HarvestMonth=str_time.mon;
										xpn->pPl->pModelParam->HarvestYear=str_time.year;
									}

									xpn->pPl->pRoot->fMaxDepth = pPM->fRootDepth;

								}
						}*/


			//if (NewDay(xpn->pTi))
				{
					/*
					// Pflanzenmesswert eventuell in Systemsteuerug übernehmen
					while ((pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime))
					   {
					   pPM = pPM->pNext;
					   }
					*/

					

                    //if (xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY)<0)
                    //            printf("GT root pPM time %d %d %.17f  %d %.17f\n",xpn_time_compare_time(xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY), xpn->pTi->pSimTime->iyear, xpn->pTi->pSimTime->fTimeY,pPM->iyear,pPM->fTimeY);
					if(GROWING_TIME)
						{

							xpn->pPl->pDevelop->iDayAftEmerg  = 1;
							
							timetomeasure = xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY);

							//fTotalTime    = max((double)(pPM->pNext->iDay  - pPM->iDay + (double)1), (double)1);
							fTotalTime    = max(xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY)+1.0,1.0);

							/* fFracGrowS is between 0 and 1 */
							//fFracGrowS = (xpn->pPl->pDevelop->iDayAftEmerg)/fTotalTime;
							fFracGrowS = timetomeasure/fTotalTime;

							if ((xpn->pPl->pRoot->fDepth < pPM->pNext->fRootDepth - EPSILON)||(xpn->pPl->pRoot->fDepth > pPM->pNext->fRootDepth + EPSILON))
								{
									for ((pLR=xpn->pPl->pRoot->pLayerRoot, iLayer = 0); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
										pLR->fLengthDens    = (double) 0.0;

									/* here MaxAge is the time between Harvest and Emergence */
									//fTotalTime    = max(pPl->pModelParam->iHarvestDay - pPl->pModelParam->iEmergenceDay, (double)1);
									//xpn->pPl->pDevelop->iDayAftEmerg = (int)xpn_time_get_number_of_dates(xpn->pTi->pSimTime->mday,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->year,xpn->pPl->pModelParam->EmergenceDay,xpn->pPl->pModelParam->EmergenceMonth,xpn->pPl->pModelParam->EmergenceYear);
									fTotalTime = max(xpn_time_get_number_of_dates(xpn->pPl->pModelParam->EmergenceDay,xpn->pPl->pModelParam->EmergenceMonth,xpn->pPl->pModelParam->EmergenceYear,xpn->pPl->pModelParam->HarvestDay,xpn->pPl->pModelParam->HarvestMonth,xpn->pPl->pModelParam->HarvestYear),1.0);

                                    // FH 20190705 Test: timetomeasure shouldn't timetomeasure be something like iDayAftEmerg? At least in this case here.
                                    timetomeasure =  max(xpn_time_get_number_of_dates(xpn->pPl->pModelParam->EmergenceDay,xpn->pPl->pModelParam->EmergenceMonth,xpn->pPl->pModelParam->EmergenceYear,xpn->pTi->pSimTime->mday,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->year),1.0);
									//fTimeConst  =  (double)113.0 * (double)xpn->pPl->pDevelop->iDayAftEmerg / fTotalTime;
                                    fTimeConst  =  (double)113.0 * (double)timetomeasure / fTotalTime;

//									fTimeBreak  = fTotalTime *(double)24/(double)113;

									// *********************************************************************
									//                Berechne Dynamik der max. Wurzeltiefe wie Entwicklung des Leaf Area Index

									f1 = (double)6.0 - ((double)12.0 * fFracGrowS);

									/* Lasse als Anfangswert auch Werte > 0 zu.*/
									xpn->pPl->pRoot->fDepth = pPM->fRootDepth + (pPM->pNext->fRootDepth - pPM->fRootDepth) / ((double)1.0 + (double)exp(f1));



									maxWuLae = max((double)100, - (double)0.05253 * fTimeConst * fTimeConst      // [1] [mm Wurzel an der Bodenoberfläche /mm Tiefe ]
									               + (double)24.6667 * fTimeConst
									               - (double)120.0);

									/*
									Dynamik nach Mais-Modell Leach:

									 if (fTimeConst <= fTimeBreak)     // "stetig" fuer fTimeBreak ~ 6 (MaxWT ~ 15)
									    {
									     f1 = max((double)0, (double)10  / (double)1.7 * // [mm]
									                         ((double)0.06 * fTimeConst * fTimeConst
									                        - (double)0.1  * fTimeConst));

									     pPl->pRoot->fDepth += f1 * (pPM->pNext->fRootDepth - pPM->fRootDepth);

									    }

									    else
									    {
									     f1 = max((double)0;  (double)10  / (double)1.7 * // [mm]
									                         ((double)-0.0112 * fTimeConst * fTimeConst
									                        + (double)2.9     * fTimeConst
									                        - (double)15.0);

									  pPl->pRoot->fDepth += f1* (pPM->pNext->fRootDepth - pPM->fRootDepth);
									  }
									*/

									HalbWert  = (double)((double)xpn->pPl->pRoot->fDepth
									                     * (((double)-.0001854 * (double)fTimeConst * (double)fTimeConst)
									                        + ((double).022 * (double)fTimeConst)
									                        - (double).102));

									HalbWert  = (double)max(HalbWert,((double)0.4* xpn->pPl->pRoot->fDepth));
									HalbWert  = (double)min(HalbWert,(xpn->pPl->pRoot->fDepth));


									f1 = (double)((double)PI * (double)HalbWert /((double)2.0 * (double)xpn->pPl->pRoot->fDepth));
									f1 = (double)((double)2.0 * cos((double)f1));
									f1 = (double)(log((double)f1)/(HalbWert * HalbWert)); //[1/mm^2]


									for ((pLR=xpn->pPl->pRoot->pLayerRoot->pNext, iLayer = 1); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
										{
											Tiefe = (double)((double)iLayer- (double)0.5) * xpn->pSo->fDeltaZ;  // [mm]

											if (Tiefe - (double)0.5*xpn->pSo->fDeltaZ < xpn->pPl->pRoot->fDepth)
												{
													pLR->fLengthDens = (double)exp((double)(-1.0 * f1 * Tiefe * Tiefe));    //[1]
                                                    // FH 20190701 added new cosine due to problems with negative cosine values
                                                    pLR->fLengthDens *= (double)cos((double)((PI * Tiefe)/(2.0 * xpn->pPl->pRoot->fDepth + xpn->pSo->fDeltaZ))); //[1]
                                                    //old formulation
													//pLR->fLengthDens *= (double)cos((double)(PI * Tiefe)/(2.0 * (xpn->pPl->pRoot->fDepth))); //[1]
													pLR->fLengthDens *= maxWuLae;   //!!  [1]
													pLR->fLengthDens *= (double).0001  * fPlantDensity;
													// 0.0001 *[1] *[1/m^2] = [1/cm^2]
                                                   //printf("%d %f %f %f %f %f \n", iLayer, pLR->fLengthDens, f1, Tiefe, HalbWert, maxWuLae);
												}

											else
												iLayer = xpn->pSo->iLayers; // Abbruch von for()

										} // for  .....
									xpn->pPl->pRoot->fTotalLength=0.0;
									for ((pLR=xpn->pPl->pRoot->pLayerRoot->pNext, iLayer = 1); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
									{
										xpn->pPl->pRoot->fTotalLength+=pLR->fLengthDens;
									}
									
/*                                    if (xpn->pPl->pRoot->fTotalLength < self->fLengthDensOld)
                                    {
                                        printf("%f %f\n", self->fLengthDensOld, xpn->pPl->pRoot->fTotalLength);
                                        for ((pLR=xpn->pPl->pRoot->pLayerRoot, iLayer = 1); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
                                                    printf("%d %f\n", iLayer, pLR->fLengthDens);
                                    }
                                    self->fLengthDensOld =xpn->pPl->pRoot->fTotalLength;*/
								}  //   Zeit   kleiner   Wurzelreifezeit  ( t < MaxWurzelTermin)
						} // Growing Time && TIME_DURING_PLANT_MEASUREMENTS
					else
						{
							if(xpn->pPl->pDevelop->fDevStage!=0)
								{
									xpn->pPl->pDevelop->fDevStage=(double)0;
									PRINT_MESSAGE(xpn,3,"End of Growing Season");
								}

							xpn->pPl->pDevelop->iDayAftEmerg  = 0;
							
							if(xpn->pPl->pRoot->pLayerRoot->pNext->fLengthDens > EPSILON)
								for ((pLR=xpn->pPl->pRoot->pLayerRoot->pNext, iLayer = 1); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
									{
										pLR->fLengthDens = (double)0;
										xpn->pPl->pRoot->fTotalLength=0.0;
									}
						}

/*					for (pPM = xpn->pPl->pPMeasure;
						 (pPM->pNext != NULL) && ((xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0));
							pPM = pPM->pNext);
							xpn->pPl->pPMeasure=pPM;*/
                
                 if (self->iRootNTrue < 2)
                    {
                      if ((pPM->pNext != NULL) && (xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0))
                        {
                          pPM = xpn->pPl->pPMeasure;
                          pPM = pPM->pNext;
                          xpn->pPl->pPMeasure=pPM;
                        }    
                    }           
				} //NewDay


		}
	return RET_SUCCESS;
}


int leachn_plant_RootMaizeLeachEP(leachn_plant *self) //EP
{
	expertn_modul_base *xpn = &(self->parent);
	int iLayer        = 0;

	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;



// Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
// pPl-> wird hochgzählt zu pPl = pPl->pNext->pNext->;
	PLAYERROOT     pLR = pPl->pRoot->pLayerRoot;

	double    fTimeConst;             // normalized time since planting
	double    fTimeBreak;             // time point of switching root depth calculation
	double    fHalfDepth;             // [mm] depth at which root length density = 1/2 of fMaxRLD
	double    fMaxRLD;                // [m/m] max. root length density at soil surface
	// !! LeachM: [m Wurzel/mm Tiefe] ExpertN: [cm / cm^3] !!
	double    fPlantDensity;          // constant value chosen  [1/m^2]
	double    fDepth;                 // [mm]
	double    fFracGrowS;             // fraction of growing season
	double    fMaxAge;                // total duration of growth
	double    f1;
	int      iMaxRootDay;	           //day number of root maturity (i.e. max. root length density)
	int	   iSowDay;				   //day number of sowing resp. planting
	PPLTMEASURE    pPM = xpn->pPl->pPMeasure;

	if(self->iIsConstRootDens)
		{
//	  if(SimStart(pTi))
			for ((pLR=pPl->pRoot->pLayerRoot, iLayer = 1); (pLR->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1); (pLR = pLR->pNext, iLayer++))
				pLR->fLengthDens = self->afConstRootDens[iLayer];
		}
	else
		{

//			if(NewDay(pTi))
				{
//					if(((int)SimTime>=pPl->pModelParam->iEmergenceDay)&&
//					        ((int)SimTime<=pPl->pModelParam->iHarvestDay))
					//if ((xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->EmergenceYear,pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceDay)>=0) &&
					//        (xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)<=0))
					if(GROWING_TIME)
						{
							fPlantDensity = (double)100;
							//pPl->pDevelop->fDevStage=(double)1;
							//pPl->pDevelop->iDayAftEmerg = (int)xpn_time_get_number_of_dates(xpn->pTi->pSimTime->mday,xpn->pTi->pSimTime->mon,xpn->pTi->pSimTime->year,xpn->pPl->pModelParam->EmergenceDay,xpn->pPl->pModelParam->EmergenceMonth,xpn->pPl->pModelParam->EmergenceYear);
							//(int)SimTime  - pPl->pModelParam->iEmergenceDay;
							{
								STRUCT_XPN_TIME str_time;
								int MaxRootBiomassYear;
								double MaxRootBiomassTimeY;
								str_time.is_leap_year=0;
								str_time.msec = 0.0;
								str_time.sec=0;
								str_time.min=0;
								str_time.hour=0;
								str_time.mday=pPl->pModelParam->MaxRootBiomassDay;
								str_time.mon=pPl->pModelParam->MaxRootBiomassMonth;
								str_time.year=pPl->pModelParam->MaxRootBiomassYear;
								xpn_time_set_struc(&str_time, &MaxRootBiomassYear, &MaxRootBiomassTimeY);
								iMaxRootDay=(int)xpn_time_get_number_of_days(pTi->pSimTime->iStart_year,pTi->pSimTime->fStart_TimeY,MaxRootBiomassYear,MaxRootBiomassTimeY);
								//(int)DateToDays(pTi->pSimTime->lStartDate,pPl->pModelParam->lMaxRootBiomassDate);
							}

							{
								STRUCT_XPN_TIME str_time;
								int SowYear;
								double SowTimeY;
								str_time.is_leap_year=0;
								str_time.msec = 0.0;
								str_time.sec=0;
								str_time.min=0;
								str_time.hour=0;
								str_time.mday=pMa->pSowInfo->Day;
								str_time.mon=pMa->pSowInfo->Month;
								str_time.year=pMa->pSowInfo->Year;
								xpn_time_set_struc(&str_time, &SowYear, &SowTimeY);
								iSowDay=(int)xpn_time_get_number_of_days(pTi->pSimTime->iStart_year,pTi->pSimTime->fStart_TimeY,SowYear,SowTimeY);
								//iSowDay=(int)DateToDays(pTi->pSimTime->lStartDate,pMa->pSowInfo->lSowDate);
							}

							/* here MaxAge is the time between Harvest and Emergence */

							fMaxAge = max(xpn_time_get_number_of_dates(pPl->pModelParam->EmergenceDay,pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceYear,pPl->pModelParam->HarvestDay,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestYear),1.0);
							//fMaxAge = max(pPl->pModelParam->iHarvestDay - pPl->pModelParam->iEmergenceDay, (double)1);

							/* fFracGrowS is between 0 and 1 */
							fFracGrowS = (xpn_time_get_number_of_dates(pPl->pModelParam->EmergenceDay,pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceYear,pTi->pSimTime->mday,pTi->pSimTime->mon,pTi->pSimTime->year))/fMaxAge;
							//fFracGrowS = (SimTime  - pPl->pModelParam->iEmergenceDay)/fMaxAge;

//if ((pPl->pRoot->fDepth < pPM->pNext->fRootDepth - EPSILON)||(pPl->pRoot->fDepth > pPM->pNext->fRootDepth + EPSILON))
							{
								for (SOIL_LAYERS0(pLR,pPl->pRoot->pLayerRoot))
									pLR->fLengthDens    = (double) 0.0;


								//fTimeConst  =  (double)113.0 * (double)pPl->pDevelop->iDayAftEmerg / fMaxAge;
								//fTimeBreak  = fMaxAge * (double)24/(double)113;


								fTimeConst  =  (double)113.0 * (double)(min(iMaxRootDay,(xpn_time_get_number_of_days(pTi->pSimTime->iStart_year,pTi->pSimTime->fStart_TimeY,pTi->pSimTime->iyear,pTi->pSimTime->fTimeY)))-iSowDay)
								               /(double)(iMaxRootDay-iSowDay+(int)1);
								fTimeBreak  = (double)24/(double)113*(double)(iMaxRootDay-iSowDay+(int)1);

								/* Assume planting depth 50 mm */
								pPl->pRoot->fDepth = (double)50;

								fMaxRLD = max((double)100, - (double)0.05253 * fTimeConst * fTimeConst // [1] [mm Wurzel an der Bodenoberfläche /mm Tiefe ]
								              + (double)24.6667 * fTimeConst
								              - (double)120.0);


								//corn root growth following LEACHN (Davidson et al. 1978 in EPA Publication 600/3-78-029)

								if (fTimeConst <= fTimeBreak)     // "stetig" fuer fTimeBreak ~ 6 (MaxWT ~ 15)
									{
										f1 = max((double)0, (double)10  / (double)1.7 * // [mm]
										         ((double)0.06 * fTimeConst * fTimeConst
										          - (double)0.1  * fTimeConst));

										pPl->pRoot->fDepth += f1;

									}

								else
									{
										f1 = max((double)0,  (double)10  / (double)1.7 * // [mm]
										         ((double)-0.0112 * fTimeConst * fTimeConst
										          + (double)2.9     * fTimeConst
										          - (double)15.0));

										pPl->pRoot->fDepth += f1;
									}


								fHalfDepth  = (double)((double)pPl->pRoot->fDepth
								                       * (((double)-.0001854 * (double)fTimeConst * (double)fTimeConst)
								                          + ((double).022 * (double)fTimeConst)
								                          - (double).102));

								fHalfDepth  = (double)max(fHalfDepth,((double)0.4 * pPl->pRoot->fDepth));
								fHalfDepth  = (double)min(fHalfDepth,(pPl->pRoot->fDepth));


								f1 = (double)((double)PI * (double)fHalfDepth /((double)2.0 * (double)pPl->pRoot->fDepth));
								f1 = (double)((double)2.0 * cos((double)f1));
								f1 = (double)(log((double)f1)/(fHalfDepth * fHalfDepth)); //[1/mm^2]


								for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
									{
										fDepth = (double)((double)iLayer- (double)0.5) * pSo->fDeltaZ;  // [mm]

										if (fDepth - (double)0.5 *pSo->fDeltaZ < pPl->pRoot->fDepth)
											{
												pLR->fLengthDens = (double)exp((double)(-1.0 * f1 * fDepth * fDepth));    //[1]
												pLR->fLengthDens *= (double)cos((double)((PI * fDepth)/(2.0 * pPl->pRoot->fDepth))); //[1]
												pLR->fLengthDens *= fMaxRLD;   //!!  [1]
												pLR->fLengthDens *= (double).0001  * fPlantDensity;
												// 0.0001 *[1] *[1/m^2] = [1/cm^2]
											}

										else
											iLayer = pSo->iLayers; // Abbruch von for()

									} // end for  .....
							} // end if (t < iMaxRootDay)
						} // end if plant is growing
					else
						{
							if(pPl->pDevelop->fDevStage!=0)
								{
									pPl->pDevelop->fDevStage=(double)0;
									PRINT_MESSAGE(xpn,3,"End of Growing Season");
								}

							if(pPl->pRoot->pLayerRoot->fLengthDens > EPSILON)
								for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
									{
										//pLR->fLengthDens = (double)0;
									}
						}// else

				}// new day

		} //root density not constant
	return RET_SUCCESS;
} //  end   RootMaizeLeach()


int leachn_plant_NUptake(leachn_plant *self)	//CH
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;


	double    fFracGrowS;         // fraction of growing season
	double    fFracTotNUP;        // fraction of total N Uptake
	double    fDeficitN;          // Deficit of N
	double    fNH4Aufnahme;
	double    fNO3Aufnahme;
	double    fMaxAge;            // Gesamtdauer des Wachstums
	double    f1;
	double    fDOCAufnahme;

	double    DeltaZ = pSo->fDeltaZ;
	double    fNmin = (double) 0.0;//0.75;//0.25;//1;  // N-Gehalt,den Pflanzen nicht entziehen [kg/ha].

	int iLayer        = 0;
	double DeltaT		= pTi->pTimeStep->fAct;

	PPLTMEASURE    pPM;
	PPLTNITROGEN   pPN;
	PLAYERROOT     pLR;
	PCLAYER        pCL;

	if (pPl == NULL) return -1;
	
	self->__ERROR=0;

    if (self->iRootNTrue < 2)
        {
        self->iRootNTrue = 2;
        }

	pPM = pPl->pPMeasure;
	pPN = pPl->pPltNitrogen;

	fNmin *= DeltaZ/(double)100;

	if (pPl->pDevelop->fDevStage > EPSILON)
		{
			// pPl->pDevelop->iDayAftEmerg must be set!

			//while ((pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime))
/*			while ((pPM->pNext != NULL) && (((xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,pTi->pSimTime->iyear,pTi->pSimTime->fTimeY,EPSILON) <= 0))))
				{
					pPM = pPM->pNext;
				}*/

                    if ((pPM->pNext != NULL) && (xpn_compare_to_added_date(pPM->pNext->iyear,pPM->pNext->fTimeY,xpn->pTi->pSimTime->iyear,xpn->pTi->pSimTime->fTimeY,EPSILON) <= 0.0))
                        {
                          pPM = xpn->pPl->pPMeasure;
                          pPM = pPM->pNext;
                          xpn->pPl->pPMeasure=pPM;
                        }    

			if (pPM->pNext == NULL)
				{
					return RET_SUCCESS;
				}

			fMaxAge = max(xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,pPM->pNext->iyear,pPM->pNext->fTimeY),1.0);
			//fMaxAge    = max((double)(pPM->pNext->iDay  - pPM->iDay), (double)1);

			fFracGrowS = xpn_time_get_number_of_days(pPM->iyear,pPM->fTimeY,pTi->pSimTime->iyear,pTi->pSimTime->fTimeY)/fMaxAge;
			//fFracGrowS = (SimTime - pPM->iDay)/fMaxAge;

			if (fFracGrowS <= (double) 0.3)
				fFracTotNUP = (double) 5.555 * fFracGrowS * fFracGrowS;

			else
				fFracTotNUP = (double)-1.0 * fFracGrowS * fFracGrowS           // fgs==0.3 => fgn = 0.5
				              + (double)2 * fFracGrowS;

			if (!Test_Range(xpn,pPM->pNext->fTotalN, pPM->fTotalN, (double)1000, "Measurement fTotalN"))
				{
					if (pPM->fTotalN > pPM->pNext->fTotalN)
						{
							pPM->pNext->fTotalN = pPM->fTotalN;
						}
					else
						{
							PRINT_ERROR("Error in Function NUptake()");
							return -1;
						}
				}


			pPN->fCumPotNUpt = pPM->fTotalN + fFracTotNUP *  (pPM->pNext->fTotalN - pPM->fTotalN);
			//lpx->lPtrPlant->MaxKumNUptake;

			fDeficitN = pPN->fCumPotNUpt - pPN->fCumActNUpt;

			if (fDeficitN >EPSILON)
				{
					if (pPl->pRoot->pLayerRoot->fLengthDens < (double)0.001)
						{
							pPN->fActNUpt = pPN->fActNUptR = (double)0;

							PRINT_ERROR_ID(xpn,"No Roots");

							return -1;
						}

					f1 = (double)0;

					for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
						{
							f1 += (pCL->fNH4N + pCL->fNO3N) * pLR->fLengthDens;
						}

					pPN->fActNO3NUpt = pPN->fActNH4NUpt = (double) 0;


					for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
						{
							fNH4Aufnahme = min(fDeficitN * pCL->fNH4N * pLR->fLengthDens / f1, pCL->fNH4N - fNmin);
							fNH4Aufnahme = max((double) 0, fNH4Aufnahme);
							fNO3Aufnahme = min(fDeficitN * pCL->fNO3N * pLR->fLengthDens / f1, pCL->fNO3N - fNmin);
							fNO3Aufnahme = max((double) 0, fNO3Aufnahme);

							fDOCAufnahme = pCL->fDOCSoilConc * pLR->fActLayWatUpt;
							fDOCAufnahme = max((double) 0, fDOCAufnahme);

							pPN->fActNH4NUpt += fNH4Aufnahme;
							pCL->fNH4N       -= fNH4Aufnahme;

							pPN->fActNO3NUpt += fNO3Aufnahme;
							pCL->fNO3N       -= fNO3Aufnahme;
							CHECK_VALID(pCL->fNO3N)

							pCL->fDOC        -= fDOCAufnahme/(double)100;

						} // for

					pPN->fActNUpt = pPN->fActNO3NUpt + pPN->fActNH4NUpt;

					pPN->fActNUptR = (pPN->fActNO3NUpt + pPN->fActNH4NUpt)/DeltaT;

				} // Deficit > EPSILON

			else
				{
					pPN->fActNUpt =   pPN->fActNUptR = (double)0;
				}

		}   // DevStage > 0
	else
		{
			//pPN->fCumPotNUpt = pPN->fCumActNUpt = pPN->fActNUpt = pPN->fActNUptR = pPN->fActNO3NUpt = pPN->fActNH4NUpt = (double)0; //removed by Hong. It made no sence!!!
		}

    pPN->fCumActNUpt			+=  pPN->fActNUptR*DeltaT; //added by Hong 

	return RET_SUCCESS;
} /*  ende   NUptake */
