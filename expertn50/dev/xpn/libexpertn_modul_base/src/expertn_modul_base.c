//      expertn_modul_base.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios>
//
#include <stdio.h>
#include "expertn_modul_base.h"
#include <string.h>
#include <math.h>
static void expertn_modul_base_finalize(GObject *object);
G_DEFINE_TYPE(expertn_modul_base, expertn_modul_base, G_TYPE_OBJECT);
enum
{
    PROP_0,
    PROP_TEST
};
void get_grid_info(char**_grid_id,char**_grid_i,char**_grid_j, char* reg_str)
{
	char *grid_id,*grid_i,*grid_j;
	char *S;
	int i;
	grid_id=NULL;
	grid_i=NULL;
	grid_j=NULL;
	if (reg_str!=NULL)
		{
			S = (char*)g_malloc0(strlen(reg_str));
			for (i=0; i<3; i++)
				{
					if (getStringColumn(S,reg_str,"_",i)==0)
						{
							switch(i)
								{
								case 0:
									grid_id=g_strdup_printf("%s",S);
									break;
								case 1:
									grid_i=g_strdup_printf("%s",S);
									break;
								case 2:
									grid_j=g_strdup_printf("%s",S);
									break;
								}
						}
				}
			g_free(S);
		}
	S="";
	if (grid_id==NULL) grid_id=g_strdup_printf("%s",S);
	if (grid_i==NULL) grid_i=g_strdup_printf("%s",S);
	if (grid_j==NULL) grid_j=g_strdup_printf("%s",S);
	*_grid_id=grid_id;
	*_grid_i=grid_i;
	*_grid_j=grid_j;
}
void get_full_grid_info(int *_grid_id,int *_grid_i,int *_grid_j,int *_grid_k, char* reg_str)
{
	char *grid_id,*grid_i,*grid_j,*grid_k;
	char *S;
	int i;
	grid_id=NULL;
	grid_i=NULL;
	grid_j=NULL;
	grid_k=NULL;
	if (reg_str!=NULL)
		{
			S = (char*)g_malloc0(strlen(reg_str));
			for (i=0; i<4; i++)
				{
					if (getStringColumn(S,reg_str,"_",i)==0)
						{
							switch(i)
								{
								case 0:
									grid_id=g_strdup_printf("%s",S);
									break;
								case 1:
									grid_i=g_strdup_printf("%s",S);
									break;
								case 2:
									grid_j=g_strdup_printf("%s",S);
									break;
								case 3:
									grid_k=g_strdup_printf("%s",S);
									break;
								}
						}
				}
			g_free(S);
		}
	S="0";
	if (grid_id==NULL) grid_id=g_strdup_printf("%s",S);
	if (grid_i==NULL) grid_i=g_strdup_printf("%s",S);
	if (grid_j==NULL) grid_j=g_strdup_printf("%s",S);
	if (grid_k==NULL) grid_k=g_strdup_printf("%s",S);
	*_grid_id = atoi(grid_id);
	*_grid_i = atoi(grid_i);
	*_grid_j = atoi(grid_j);
	*_grid_k = atoi(grid_k);
	g_free(grid_id);
	g_free(grid_i);
	g_free(grid_j);
	g_free(grid_k);
}
char *expertn_modul_base_replace_std_templates(expertn_modul_base *self,char *source)
{
	int len_templ = 10;
	char *std_templates[10] = {"$PROJECT_PATH","$BASE_PATH","$REG_STR","$REG_ID","$REG_I","$REG_J","$PROJECT_NAME","$ID","$<","$>"};
	char *std_expr[10];
	char id[128];
	char *S;
	char *grid_id,*grid_i,*grid_j;
	sprintf(id,"%d",self->pXSys->id);
	get_grid_info(&grid_id,&grid_i,&grid_j, self->pXSys->reg_str);
	std_expr[0] = self->pXSys->project_path;
	std_expr[1] = self->pXSys->base_path;
	std_expr[2] = self->pXSys->reg_str;
	std_expr[3] = grid_id;
	std_expr[4] = grid_i;
	std_expr[5] = grid_j;
	std_expr[6] = self->pXSys->project_name;
	std_expr[7] = id;
	std_expr[8] = "";
	std_expr[9] = "";
	S = replace_str_with_templates(source,std_templates,std_expr,len_templ);
	g_free(grid_id);
	g_free(grid_i);
	g_free(grid_j);
	return S;
}
char *expertn_modul_base_replace_std_templates_and_get_fullpath_from_relative(expertn_modul_base *self,char *source)
{
	char *S,*S2;
	S = expertn_modul_base_replace_std_templates(self,source);
	S2 = get_fullpath_from_relative(self->pXSys->base_path,S);
	if (S!=NULL) free(S);
	return S2;
}
static void expertn_modul_base_class_init(expertn_modul_baseClass *klass)
{
	GObjectClass *g_object_class;
	//GParamSpec *pspec;
	g_object_class = G_OBJECT_CLASS(klass);
	g_object_class->finalize = expertn_modul_base_finalize;
	/*	g_object_class->set_property = expertn_modul_base_set_property;
	    g_object_class->get_property = expertn_modul_base_get_property;
		pspec = g_param_spec_pointer ("test",
	                               "Dies ist ein Test",
	                               "Set test var",
	                               G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
		g_object_class_install_property (g_object_class,
	                                   PROP_TEST,
	                                   pspec);*/
}
static void expertn_modul_base_finalize(GObject *object)
{
	expertn_modul_base *self;
	g_return_if_fail(object != NULL);
	g_return_if_fail(IS_EXPERTN_MODUL_BASE(object));
	self = EXPERTN_MODUL_BASE(object);
	G_OBJECT_CLASS(expertn_modul_base_parent_class)->finalize(object);
}
static void expertn_modul_base_init(expertn_modul_base *self)
{
	self->DEBUG_LEVEL=3;
}
expertn_modul_base *expertn_modul_base_new(void)
{
	return (expertn_modul_base *)g_object_new(EXPERTN_MODUL_BASE_TYPE, NULL);
	printf(" expertn_modul_base_new\n");
}
double expertn_modul_base_get_infinity()
{
	double d1,d2;
	d1=1.0;
	d2=0.0;
	return d1 / d2;
}
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Initialization of plant variables
------------------------------------------------------------------------------------------------------------
*/
#define SET_IF_99(x,y) 		if (x == (double)-99)   x = (double)y;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double DLL AFGENERATOR(double fInput, RESPONSE* pfResp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function interpolates linearly a value from a RESPONSE table using input fInput
//Inputs:	1. fInput	- the input value (-)
//			2. pfResp	- Pointer to the first element of the given RESPONSE table
//Outputs:	The interpolated value at fInput (-)
//Functions Called:
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double expertn_modul_base_AFGENERATOR(double fInput, RESPONSE* pResp)
{
	RESPONSE *pfResp; //, *pResp1;
	double	 fSlope,fOutput;
	int	  	 nPoints,i;
	fOutput=0.0;
	pfResp = pResp;
	if (pResp[0].iInLen >= pResp[0].iOutLen)
		nPoints = pfResp[0].iOutLen;
	else
		nPoints = pfResp[0].iInLen;
	if (fInput<=pfResp[0].fInput)
		return pfResp[0].fOutput;
	if (nPoints > 0)
		{
			if (fInput>=pfResp[nPoints-1].fInput)
				return pfResp[nPoints-1].fOutput;
			for (i=0; i<nPoints-1; i++)
				{
					if (	((fInput>=pfResp[i].fInput)&&(fInput<pfResp[i+1].fInput))
					        ||  ((fInput<=pfResp[i].fInput)&&(fInput>pfResp[i+1].fInput))   )
						break;
				}
			fSlope = (pfResp[i+1].fOutput- pfResp[i].fOutput)/(pfResp[i+1].fInput-pfResp[i].fInput);
			fOutput= pfResp[i].fOutput + fSlope*(fInput-pfResp[i].fInput);
		}
	return fOutput;
}
int expertn_modul_base_NitrogenConcentrationLimits(PPLANT pPl)
{
	//PPLANT			pPl = self->pPl;
	PDEVELOP		pDev  = pPl->pDevelop;
	PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
	pPltN->fLeafMaxConc = expertn_modul_base_AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->OptLvNc);
	pPltN->fStemMaxConc = expertn_modul_base_AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->OptStNc);
	pPltN->fRootMaxConc = expertn_modul_base_AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->OptRtNc);
	pPltN->fLeafMinConc = expertn_modul_base_AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->MinLvNc);
	pPltN->fStemMinConc = expertn_modul_base_AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->MinStNc);
	pPltN->fRootMinConc = expertn_modul_base_AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->MinRtNc);
	//SG/05/05/99/zusätzlich optimale Werte:
	pPltN->fLeafOptConc = (float)0.7*pPltN->fLeafMaxConc; //instead of 0.5 from van Keulen
	pPltN->fStemOptConc = (float)0.7*pPltN->fStemMaxConc; //instead of 0.5 from van Keulen
	pPltN->fRootOptConc = (float)0.7*pPltN->fRootMaxConc; //instead of 0.5 from van Keulen
	return RET_SUCCESS;
}
int expertn_modul_base_PlantVariableInitiation(PPLANT pPl, PSPROFILE pSo, PMSOWINFO pSI)
{
	/*	PMANAGEMENT		pMa = self->pMa;
		PPLANT			pPl = self->pPl;
		PSPROFILE		pSo = self->pSo;*/
	double 		CUMDEP,fThickness;
	int   			L,L1;
	char			*pCropCode;
	PGENOTYPE 		pGT		=pPl->pGenotype;
	PDEVELOP		pDev	=pPl->pDevelop;
	PBIOMASS		pGrw	=pPl->pBiomass;
	PMODELPARAM 	pGrwP	=pPl->pModelParam;
	PCANOPY			pCan	=pPl->pCanopy;
	PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
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
	//FH 2014-03-03 pSI->fPlantDens sollte schon vorher gesetzt sein
	//pSI->fPlantDens = (double)0.0;
	pCan->fLeafTipNum = (double)0.0;
	pDev->fDaylengthEff = (double)0.0;
	pPl->pOutput->fValue1 = (double)0.0;
	pPl->pOutput->fValue2 = (double)0.0;
	pPl->pCanopy->fkcan = 0.6;
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
	
    // FH 2019-07-11 now the plant density appears in the output
    pCan->fPlantDensity = pSI->fPlantDens;
    
    //======================================================================================
	//Dry matter initialization
	//======================================================================================
	if ((strcmp(pCropCode,"WH")==0)
	        ||((strcmp(pCropCode,"BA")==0)||(strcmp(pCropCode,"S")==0))
	        ||((strcmp(pCropCode,"RY")==0)))
		{
			//FH 2014-03-03 laut CB nicht sinnvoll
			//if (pSI->fPlantDens<(double)100)
			//	pSI->fPlantDens=(double)400;
			pDev->fDaylengthEff	=(double)0.01;
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
			pGrw->fLeafWeight	=(double)0.1125*(double)10.0*pSI->fPlantDens; //0.1125g/plt
			pGrw->fStemWeight	=(double)0.1125*(double)10.0*pSI->fPlantDens;
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
	if ((strcmp(pCropCode,"SB")==0)||(strcmp(pPl->pGenotype->acCropCode,"BS")==0)) // um Fehler von "BS" zu vermeiden
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
	expertn_modul_base_NitrogenConcentrationLimits(pPl);
	
	//======================================================================================
	//Root depth and length density
	//======================================================================================
	pPl->pRoot->fDepth= fSowDepth;
	pLR	  =pPl->pRoot->pLayerRoot;
	pSL	  =pSo->pSLayer->pNext;
	CUMDEP=(double)0.0;
	for (L=1; L<=pSo->iLayers-2; L++)
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
			for (L=L1; L<=pSo->iLayers; L++)
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
	return RET_SUCCESS;
}
#define GET_INI_INT(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_integer (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		}\
	 
#define GET_INI_DOUBLE(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_double (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Error init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR(S);\
			g_free(S);\
		} \
	 
#define GET_INI_STRING(var,groupname,key) \
	error = NULL; \
	var = g_key_file_get_string (keyfile,groupname,key,&error); \
	if (error!=NULL) \
		{ \
			gchar *S;\
			S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
			PRINT_ERROR_ID(self,S);\
			g_free(S);\
		}\
	 
#define GET_INI_INT_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_integer_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(self,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	 
#define GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_double_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(self,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	 
#define GET_INI_STRING_ARRAY(var,var_size,groupname,key) \
	{\
		gsize _var_size;\
		error = NULL; \
		var = g_key_file_get_string_list (keyfile,groupname,key,&_var_size,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(self,S);\
				g_free(S);\
			}\
		var_size = (int)_var_size;\
	}\
	 
#define GET_INI_DOUBLE_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(self,S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(double)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = std_value;\
					}\
			} else\
			{\
				GET_INI_DOUBLE_ARRAY(var,var_size,groupname,key);\
			}\
	}\
	 
#define GET_INI_INT_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists,group_exists;\
		error = NULL;\
		group_exists = g_key_file_has_group(keyfile,groupname);\
		if (group_exists==TRUE)\
			{\
				key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
				if (error!=NULL) \
					{ \
						gchar *S;\
						S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
						PRINT_ERROR_ID(self,S);\
						g_free(S);\
					}\
			} else\
			{\
				key_exists = FALSE;\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(int)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = std_value;\
					}\
			} else\
			{\
				GET_INI_INT_ARRAY(var,var_size,groupname,key);\
			}\
	}\
	 
#define GET_INI_STRING_ARRAY_OPTIONAL(var,var_size,expected_len,std_value,groupname,key) \
	{\
		gboolean key_exists;\
		error = NULL; \
		key_exists = g_key_file_has_key(keyfile,groupname,key,&error);\
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR_ID(self,S);\
				g_free(S);\
			}\
		if (key_exists==FALSE)\
			{ \
				int i;\
				var_size=expected_len;\
				var = g_malloc0(sizeof(char*)*expected_len);\
				for (i=0;i<expected_len;i++)\
					{\
						var[i] = g_strdup_printf("%s",std_value);\
					}\
			} else\
			{\
				GET_INI_STRING_ARRAY(var,var_size,groupname,key)\
			}\
	}\
	 
#define GET_INI_DATE(_date,groupname,key) \
	{\
		char* var;\
		error = NULL; \
		var = g_key_file_get_string (keyfile,groupname,key,&error); \
		if (error!=NULL) \
			{ \
				gchar *S;\
				S = g_strdup_printf  ("Init var %s.%s (%s)!\n Error Message: %s",groupname,key,filename,error->message);\
				PRINT_ERROR(S);\
				g_free(S);\
				_date=NULL;\
			} else\
			{\
				_date = convert_str_to_gdate(var);\
				g_free(var);\
			}\
	}\
	 
#define CHECK_LEN(var1,var2)\
	{\
		if (var1!=var2)\
			{\
				gchar *S;\
				S = g_strdup_printf  ("%s is not %s. Check your configuration in file: %s!",#var1,#var2,filename);\
				PRINT_ERROR_ID(self,S);\
				g_free(S);\
			}\
	}\
	 
int array_length(var1,var2)
{
	if (var1>var2)
		{
			return var2;
		}
	else if (var1<var2)
		{
			return var1;
		}\
	else
		{
			return var1;
		}
}
int expertn_modul_base_PlantManagementRead(expertn_modul_base *self,const char* configfile, char* model)
{
	PPLANT			pPl = self->pPl;
	PGENOTYPE		pGe = pPl->pGenotype;
	PMSOWINFO		pSI	= self->pMa->pSowInfo;
	PSIMTIME		pST = self->pTi->pSimTime;
	PMANAGEMENT		pMa = self->pMa;
	PSPROFILE		pSo = self->pSo;
	char			**CropCode, **CropName, **Variety;
	char			**SowDate, **EmergDate, **HarvestDate, **MaxBiomDate, **MaxRoDate;
	const char* 	filename=configfile;
	char 			*S;
	char			*read_filename, *read_filename2;
	double			*PlantDens, *RowWidth, *SowDepth, *MaxPlantHeight,*MaxRootDepth;
	int				*ResidueCarryOff;
	int				CC_len, CN_len, Var_len, SD_len, ED_len, HD_len, MBD_len, MRD_len, PD_len, RW_len, SDe_len, MPH_len, MR_len, RC_len;
	int 			startyear,startmon,startday,stopyear,stopmon,stopday;
	int				i,sim_i;
	int             *simulate_rows;
	int             simulate_rows_len;
	GDate*			date;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	STRUCT_XPN_TIME str_time;
	//Get Start and Stop Year
	xpn_time_get_struc(&str_time, pST->iStart_year, pST->fStart_TimeY);
	startyear = str_time.year;
	startmon =  str_time.mon;
	startday =  str_time.mday;
	xpn_time_get_struc(&str_time, pST->iStop_year, pST->fStop_TimeY);
	stopyear = str_time.year;
	stopmon =  str_time.mon;
	stopday =  str_time.mday;
	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, configfile, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}
		
// plant management
	GET_INI_STRING_ARRAY(CropCode,CC_len,"plant_management","CropCode");
	GET_INI_STRING_ARRAY(CropName,CN_len,"plant_management","CropName");
	GET_INI_STRING_ARRAY(SowDate,SD_len,"plant_management","sow_date")
	GET_INI_STRING_ARRAY(Variety,Var_len,"plant_management","variety");
	GET_INI_DOUBLE_ARRAY(PlantDens,PD_len,"plant_management","sow_dens");
	GET_INI_DOUBLE_ARRAY(RowWidth,RW_len,"plant_management","row_dist");
	GET_INI_DOUBLE_ARRAY(SowDepth,SDe_len,"plant_management","sow_depth");
	GET_INI_DOUBLE_ARRAY(MaxPlantHeight,MPH_len,"plant_management","max_plant_height");
	GET_INI_DOUBLE_ARRAY(MaxRootDepth,MR_len,"plant_management","max_root");
	GET_INI_STRING_ARRAY(EmergDate,ED_len,"plant_management","emerg_date")
	GET_INI_STRING_ARRAY(HarvestDate,HD_len,"plant_management","harvest_date")
	GET_INI_STRING_ARRAY(MaxBiomDate,MBD_len,"plant_management","max_biom_date")
	GET_INI_STRING_ARRAY(MaxRoDate,MRD_len,"plant_management","max_ro_date")
	GET_INI_INT_ARRAY(ResidueCarryOff,RC_len,"plant_management","biom_remove");
// check lengths of variables
	CHECK_LEN(CC_len,CN_len);
	CHECK_LEN(CC_len,SD_len);
	CHECK_LEN(CC_len,Var_len);
	CHECK_LEN(CC_len,PD_len);
	CHECK_LEN(CC_len,RW_len);
	CHECK_LEN(CC_len,SDe_len);
	CHECK_LEN(CC_len,MPH_len);
	CHECK_LEN(CC_len,MR_len);
	CHECK_LEN(CC_len,ED_len);
	CHECK_LEN(CC_len,HD_len);
	CHECK_LEN(CC_len,MBD_len);
	CHECK_LEN(CC_len,MRD_len);
	CHECK_LEN(CC_len,RC_len);
	GET_INI_INT_ARRAY_OPTIONAL(simulate_rows,simulate_rows_len,CC_len,-1,"simulation","simulate_rows");
	if (simulate_rows[0] == -1)
		{
			for(i=0; i< simulate_rows_len; i++)
				{
					simulate_rows[i] = i+1;
				}
		};
// write to variables
	//for(i=0; i< CC_len; i++){
	for(sim_i=0; sim_i< simulate_rows_len; sim_i++)
		{
			char *S2;
			i = simulate_rows[sim_i]-1;
			deleteSpaceBegEnd(CropCode[i]);
			pPl->pGenotype->acCropCode = CropCode[i];
			deleteSpaceBegEnd(CropName[i]);
			pPl->pGenotype->acCropName = CropName[i];
			deleteSpaceBegEnd(SowDate[i]);
			date = convert_str_to_gdate(SowDate[i]);
			pSI->Day = g_date_get_day(date);
			pSI->Month = g_date_get_month(date);
			pSI->Year = g_date_get_year(date);
			deleteSpaceBegEnd(Variety[i]);
			pPl->pGenotype->acVarietyName = Variety[i];
			pSI->fPlantDens = PlantDens[i];
			pSI->fRowWidth = RowWidth[i];
			pSI->fSowDepth = SowDepth[i];
			pPl->pGenotype->fPlantHeight = MaxPlantHeight[i];
			pPl->pModelParam->fMaxRootDepth = MaxRootDepth[i];
			pPl->pGenotype->fMaxRootDepth = MaxRootDepth[i];
			deleteSpaceBegEnd(EmergDate[i]);
			date = convert_str_to_gdate(EmergDate[i]);
			pPl->pModelParam->EmergenceDay = g_date_get_day(date);
			pPl->pModelParam->EmergenceMonth = g_date_get_month(date);
			pPl->pModelParam->EmergenceYear = g_date_get_year(date);
			deleteSpaceBegEnd(HarvestDate[i]);
			date = convert_str_to_gdate(HarvestDate[i]);
			pPl->pModelParam->HarvestDay = g_date_get_day(date);
			pPl->pModelParam->HarvestMonth = g_date_get_month(date);
			pPl->pModelParam->HarvestYear = g_date_get_year(date);
			deleteSpaceBegEnd(MaxBiomDate[i]);
			date = convert_str_to_gdate(MaxBiomDate[i]);
			pPl->pModelParam->MaxAboveBiomassDay = g_date_get_day(date);
			pPl->pModelParam->MaxAboveBiomassMonth = g_date_get_month(date);
			pPl->pModelParam->MaxAboveBiomassYear = g_date_get_year(date);
			deleteSpaceBegEnd(MaxRoDate[i]);
			date = convert_str_to_gdate(MaxRoDate[i]);
			pPl->pModelParam->MaxRootBiomassDay = g_date_get_day(date);
			pPl->pModelParam->MaxRootBiomassMonth = g_date_get_month(date);
			pPl->pModelParam->MaxRootBiomassYear = g_date_get_year(date);
			pPl->pModelParam->cResidueCarryOff = ResidueCarryOff[i];
			//Test if sowdate and harvestdate in simulation period
			if((xpn_time_compare_date(startyear,startmon,startday,pSI->Year,pSI->Month,pSI->Day)>0)||
			        (xpn_time_compare_date(stopyear,stopmon,stopday,pSI->Year,pSI->Month,pSI->Day)<0))
				{
					S  = g_strdup_printf("Sow date not in simulation period, use startdate as sow date");
					PRINT_ERROR(S);
					pSI->Year = startyear;
					pSI->Month =  startmon;
					pSI->Day =  startday;
					g_free(S);
				}
			if((xpn_time_compare_date(startyear,startmon,startday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)>0)||
			        (xpn_time_compare_date(stopyear,stopmon,stopday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)<0))
				{
					S  = g_strdup_printf("Harvest date not in simulation period, use stop date as harvest date");
					PRINT_ERROR(S);
					pPl->pModelParam->HarvestYear = stopyear;
					pPl->pModelParam->HarvestMonth =  stopmon;
					pPl->pModelParam->HarvestDay =  stopday;
					g_free(S);
				}
			if(strcmp(model,"gecros")!=0)
				{
					S  = g_strdup_printf("Config.%s.%s", model, pPl->pGenotype->acCropName);
					read_filename = xpn_register_var_get_pointer(self->pXSys->var_list,S);
					g_free(S);
					if (read_filename==NULL)
						{
							S  = g_strdup_printf("Missing entry '%s.%s' in your options, if present '%s.ini'-file of the Global options is used",model, pPl->pGenotype->acCropName,pPl->pGenotype->acCropName);
							PRINT_MESSAGE(self,4,S);
							g_free(S);
							S  = g_strdup_printf("Global_Config.options.%s", pPl->pGenotype->acCropName);
							read_filename = xpn_register_var_get_pointer(self->pXSys->var_list,S);
							g_free(S);
							if (read_filename==NULL)
								{
									S  = g_strdup_printf("Entry 'global.%s' is also missing in your options!",pPl->pGenotype->acCropName);
									PRINT_ERROR(S);
									g_free(S);
								}
						}
					if (read_filename!=NULL)
						{
							//Read plant parameters
							S2 = expertn_modul_base_replace_std_templates(self,read_filename);
							if (S2!=NULL)
								{
									read_filename = get_fullpath_from_relative(self->pXSys->base_path, S2);
									if (expertn_modul_base_GenotypeRead(self,pPl,read_filename)!=0)
										{
											S  = g_strdup_printf("Error Read '%s.ini'-file, check 'crop_rotation.ini' and your model options", pPl->pGenotype->acCropName);
											PRINT_ERROR(S);
											g_free(S);
										}
									read_filename2 = g_strdup_printf("%s",read_filename);
									free(read_filename);
									g_free(S2);
								}
						}
					expertn_modul_base_PlantVariableInitiation(pPl, pSo, pSI);
				}
			/*else if(strcmp(model,"gecros")==0)
				{
					S  = g_strdup_printf("Config.%s.gecros_plant_ini_file", model);
					read_filename = xpn_register_var_get_pointer(self->pXSys->var_list,S);
					g_free(S);
					if (read_filename==NULL)
						{
							S  = g_strdup_printf("Missing entry '%s.ini_file' in your options, if present 'ini'-file of the Global options is used",model);
							PRINT_MESSAGE(self,4,S);
							g_free(S);
							S  = g_strdup_printf("Global_Config.options.gecros_plant_ini_file");
							read_filename = xpn_register_var_get_pointer(self->pXSys->var_list,S);
							g_free(S);
							if (read_filename==NULL)
								{
									S  = g_strdup_printf("Entry 'global.gecros_plant_ini_file' is also missing in your options!");
									PRINT_ERROR(S);
									g_free(S);
								}
						}
					if (read_filename!=NULL)
						{
							//Read plant parameters
							S2 = expertn_modul_base_replace_std_templates(self,read_filename);
							if (S2!=NULL)
								{
									read_filename = get_fullpath_from_relative(self->pXSys->base_path, S2);
									if (expertn_modul_base_GenotypeRead(self,pPl,read_filename)!=0)
										{
											S  = g_strdup_printf("Error Read 'gecros_plant_ini_file'-file, check 'crop_rotation.ini' and your model options");
											PRINT_ERROR(S);
											g_free(S);
										}
									read_filename2 = g_strdup_printf("%s",read_filename);
									free(read_filename);
									g_free(S2);
								}
						}
				}*/
			
			if(sim_i==simulate_rows_len-1) break;
			allocPlantList(STPLANT,pPl,sim_i);
			pPl->pGenotype = (PGENOTYPE) g_malloc0_n(1,sizeof(STGENOTYPE));
			allocPlantList(STSOWINFO,pSI,sim_i);
			pPl->pModelParam = (PMODELPARAM) g_malloc0_n(1,sizeof(STMODELPARAM));
			pPl->pGenotype->pStageParam = (PSTAGEPARAM)g_malloc0_n(1,sizeof(STSTAGEPARAM));
			pPl->pGenotype->pVarietyInfo = (PVARIETYINFO)g_malloc0_n(1,sizeof(STVARIETYINFO));
			pPl->pDevelop = (PDEVELOP) g_malloc0_n(1,sizeof(STDEVELOP));
			pPl->pBiomass = (PBIOMASS) g_malloc0_n(1,sizeof(STBIOMASS));
			pPl->pCanopy = (PCANOPY) g_malloc0_n(1,sizeof(STCANOPY));
			pPl->pRoot = (PROOT) g_malloc0_n(1,sizeof(STROOT));
			allocItemList(STLAYERROOT,pPl->pRoot->pLayerRoot,pSo->iLayers+2);
			pPl->pPltCarbon = (PPLTCARBON) g_malloc0_n(1,sizeof(STPLTCARBON));
			pPl->pPltWater = (PPLTWATER) g_malloc0_n(1,sizeof(STPLTWATER));
			pPl->pPltNitrogen = (PPLTNITROGEN) g_malloc0_n(1,sizeof(STPLTNITROGEN));
			pPl->pPltClimate = (PPLTCLIMATE) g_malloc0_n(1,sizeof(STPLTCLIMATE));
			pPl->pOutput = (POUTPUT) g_malloc0_n(1,sizeof(OUTPUT));
		}
	g_free(date);
	g_free(simulate_rows);
	return RET_SUCCESS;
}
int expertn_modul_base_GenotypeRead(expertn_modul_base *self,PPLANT pPl ,const char* read_filename)
{
	//PPLANT			pPl = self->pPl;
	PGENOTYPE		pGe = pPl->pGenotype;
	PMSOWINFO		pSI	= self->pMa->pSowInfo;
	PSIMTIME		pST = self->pTi->pSimTime;
	PSTAGEPARAM		pSt = pGe->pStageParam;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	const char* filename=read_filename;
	char *S;
	char **varietynames;
	double *dummy_in;
	double *dummy_out;
	int	*dummy_int;
	int dummy_in_size;
	int dummy_out_size;
	int dummy_int_size;
	int i;
	int var_len;
	int varietynum;
	STRUCT_XPN_TIME str_time;
	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, read_filename, flags, &error))
		{
			PRINT_ERROR(error->message);
			return -1;
		}
	if(pGe->acVarietyName==NULL) pGe->acVarietyName = "Default";
// genetics
	//GET_INI_STRING(	pGe->acCropCode,"genetics","CropCode");
	//GET_INI_STRING(	pGe->acCropName,"genetics","CropName");
	GET_INI_STRING(	pGe->acEcoType,"genetics","EcoType");
	GET_INI_STRING(	pGe->acDaylenType,"genetics","DaylengthType");
	GET_INI_STRING(	pGe->acLvAngleType,"genetics","LvAngleType");
	//GET_INI_DOUBLE( pGe->fPlantHeight,"genetics","PltH");
	// check if old config:
	{
		double dummy;
		error = NULL;
		dummy = g_key_file_get_double (keyfile,"genetics","PltH",&error);
		if (error==NULL)
			{
				PRINT_ERROR_ID(self,"PltH is deprecated: Please delete the entry and move it to crop rotation file");
			}
	}
// photosynthesis response
	GET_INI_DOUBLE( pGe->fMaxGrossPs,"photoresp","MaxGrossPs");
	GET_INI_DOUBLE( pGe->fLightUseEff,"photoresp","LightUseEff");
	GET_INI_DOUBLE( pGe->fMinRm,"photoresp","MinRm");
	GET_INI_DOUBLE( pGe->fCO2CmpPoint,"photoresp","CO2CmpPoint");
	GET_INI_DOUBLE( pGe->fCiCaRatio,"photoresp","CiCaRatio");
	GET_INI_DOUBLE( pGe->fPsCurveParam,"photoresp","PsCurveParam");
	GET_INI_DOUBLE( pGe->fDaylengthCoeff,"photoresp","DaylengthCoeff");
	GET_INI_DOUBLE( pGe->fTempMinPs,"photoresp","TempMin");
	GET_INI_DOUBLE( pGe->fTempOptPs,"photoresp","TempOpt");
	GET_INI_DOUBLE( pGe->fTempMaxPs,"photoresp","TempMax");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"photoresp","Pmaxtemp");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"photoresp","Pmax");
	pGe->PmaxTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->PmaxTemp[0].fInput = -99.9;
	pGe->PmaxTemp[0].fOutput = -99.9;
	pGe->PmaxTemp[0].iInLen = 1;
	pGe->PmaxTemp[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->PmaxTemp);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->PmaxTemp = (RESPONSE *)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->PmaxTemp[0].iInLen = dummy_in_size;
			pGe->PmaxTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->PmaxTemp[i].fInput = dummy_in[i];
					pGe->PmaxTemp[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"photoresp","DevStage");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"photoresp","PPmax");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->PmaxAge = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->PmaxAge[0].iInLen = dummy_in_size;
	pGe->PmaxAge[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->PmaxAge[i].fInput = dummy_in[i];
			pGe->PmaxAge[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
// maintenance
	GET_INI_DOUBLE( pGe->fMaintLeaf,"maintenance","MaintLeaf");
	GET_INI_DOUBLE( pGe->fMaintStem,"maintenance","MaintStem");
	GET_INI_DOUBLE( pGe->fMaintRoot,"maintenance","MaintRoot");
	GET_INI_DOUBLE( pGe->fMaintStorage,"maintenance","MaintStorage");
	GET_INI_DOUBLE( pGe->fCO2EffLeaf,"maintenance","CO2EffLeaf");
	GET_INI_DOUBLE( pGe->fCO2EffStem,"maintenance","CO2EffStem");
	GET_INI_DOUBLE( pGe->fCO2EffRoot,"maintenance","CO2EffRoot");
	GET_INI_DOUBLE( pGe->fCO2EffStorage,"maintenance","CO2EffStorage");
	GET_INI_DOUBLE( pGe->fGrwEffLeaf,"maintenance","GrwEffLeaf");
	GET_INI_DOUBLE( pGe->fGrwEffStem,"maintenance","GrwEffStem");
	GET_INI_DOUBLE( pGe->fGrwEffRoot,"maintenance","GrwEffRoot");
	GET_INI_DOUBLE( pGe->fGrwEffStorage,"maintenance","GrwEffStorage");
// phenology
	GET_INI_STRING_ARRAY(varietynames,varietynum,"phenology","VarietyName");
	for(i=0; i<varietynum; i++)
		{
			deleteSpaceBegEnd(varietynames[i]);
			if(strcmp(pGe->acVarietyName,varietynames[i])==0) break;
			if((i==varietynum-1)&&(strcmp(pGe->acVarietyName,varietynames[varietynum-1])!=0))
				{
					char *S;
					i = 0;
					S  = g_strdup_printf("Variety \"%s\" not found in %s.ini - phenology: Default values are used.", pGe->acVarietyName,pGe->acCropName);
					PRINT_ERROR(S);
					g_free(S);
					break;
				}
		}
	g_strfreev(varietynames);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","DevelopConstant1");
	pGe->fDevelopConstant1 = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","DevelopConstant2");
	pGe->fDevelopConstant2 = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","OptVernDays");
	pGe->fOptVernDays = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","DaylenSensitivity");
	pGe->fDaylenSensitivity = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","OptDaylen");
	pGe->fOptDaylen = dummy_in[i];
	g_free(dummy_in);
	GET_INI_INT_ARRAY(dummy_int,dummy_int_size,"phenology","VernCoeff");
	pGe->iVernCoeff = dummy_int[i];
	g_free(dummy_int);
	//SG 20190531: read PHINT from ini file
    GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99,"phenology","PHINT");
	pGe->fPhyllochronInterval = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY_OPTIONAL(pSt->afTempMax,pSt->afTempMaxLen,1,-99.9,"phenology","TempMax");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(pSt->afTempOpt,pSt->afTempOptLen,1,-99.9,"phenology","TempOpt");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(pSt->afTempBase,pSt->afTempBaseLen,1,-99.9,"phenology","TempBas");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(pSt->afThermalTime,pSt->afThermalTimeLen,1,-99.9,"phenology","ThermalTime");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(pSt->afMaxDevR,pSt->afMaxDevRLen,1,-99.9,"phenology","MaxDevR");
	GET_INI_DOUBLE( pGe->fTempMinDevVeg,"phenology","TempMinDevVeg");
	GET_INI_DOUBLE( pGe->fTempOptDevVeg,"phenology","TempOptDevVeg");
	GET_INI_DOUBLE( pGe->fTempMaxDevVeg,"phenology","TempMaxDevVeg");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"phenology","TempDevVeg");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"phenology","DevVeg");
	pGe->DevRateVegTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->DevRateVegTemp[0].fInput = -99.9;
	pGe->DevRateVegTemp[0].fOutput = -99.9;
	pGe->DevRateVegTemp[0].iInLen = 1;
	pGe->DevRateVegTemp[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->DevRateVegTemp);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->DevRateVegTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->DevRateVegTemp[0].iInLen = dummy_in_size;
			pGe->DevRateVegTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->DevRateVegTemp[i].fInput = dummy_in[i];
					pGe->DevRateVegTemp[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE( pGe->fTempMinDevRep,"phenology","TempMinDevRep");
	GET_INI_DOUBLE( pGe->fTempOptDevRep,"phenology","TempOptDevRep");
	GET_INI_DOUBLE( pGe->fTempMaxDevRep,"phenology","TempMaxDevRep");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"phenology","TempDevRep");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"phenology","DevRep");
	pGe->DevRateRepTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->DevRateRepTemp[0].fInput = -99.9;
	pGe->DevRateRepTemp[0].fOutput = -99.9;
	pGe->DevRateRepTemp[0].iInLen = 1;
	pGe->DevRateRepTemp[0].iInLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->DevRateRepTemp);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->DevRateRepTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->DevRateRepTemp[0].iInLen = dummy_in_size;
			pGe->DevRateRepTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->DevRateRepTemp[i].fInput = dummy_in[i];
					pGe->DevRateRepTemp[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE( pGe->fTempMinVern,"phenology","TempMinDevVern");
	GET_INI_DOUBLE( pGe->fTempOptVern,"phenology","TempOptDevVern");
	GET_INI_DOUBLE( pGe->fTempMaxVern,"phenology","TempMaxDevVern");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"phenology","TempDevVern");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"phenology","DevVern");
	pGe->VernRateTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->VernRateTemp[0].fInput = -99.9;
	pGe->VernRateTemp[0].fOutput = -99.9;
	pGe->VernRateTemp[0].iInLen = 1;
	pGe->VernRateTemp[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->VernRateTemp);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->VernRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->VernRateTemp[0].iInLen = dummy_in_size;
			pGe->VernRateTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->VernRateTemp[i].fInput = dummy_in[i];
					pGe->VernRateTemp[i].fOutput = dummy_out[i];
				}
		}
	else
		g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"phenology","PhtP");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"phenology","DevRateVegPhtP");
	pGe->DevRateVegPhtP = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->DevRateVegPhtP[0].fInput = -99.9;
	pGe->DevRateVegPhtP[0].fOutput = -99.9;
	pGe->DevRateVegPhtP[0].iInLen = 1;
	pGe->DevRateVegPhtP[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->DevRateVegPhtP);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->DevRateVegPhtP = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->DevRateVegPhtP[0].iInLen = dummy_in_size;
			pGe->DevRateVegPhtP[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->DevRateVegPhtP[i].fInput = dummy_in[i];
					pGe->DevRateVegPhtP[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-1.0,"phenology","DEV");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-1.0,"phenology","ECStadium");
	pGe->DVR = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->DVR[0].fInput = -1.0;
	pGe->DVR[0].fOutput = -1.0;
	pGe->DVR[0].iInLen = 1;
	pGe->DVR[0].iOutLen = 1;
	if ((dummy_in[0] != -1.0)&&(dummy_out[0] != -1.0))
		{
			g_free(pGe->DVR);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->DVR = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->DVR[0].iInLen = dummy_in_size;
			pGe->DVR[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->DVR[i].fInput = dummy_in[i];
					pGe->DVR[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_STRING_ARRAY(varietynames,varietynum,"phenology","VarName");
	for(i=0; i<varietynum; i++)
		{
			deleteSpaceBegEnd(varietynames[i]);
			if(strcmp(pGe->acVarietyName,varietynames[i])==0) break;
			if((i==varietynum-1)&&(strcmp(pGe->acVarietyName,varietynames[varietynum-1])!=0))
				{
					i = 0;
					S  = g_strdup_printf("Variety \"%s\" not found in %s.ini - phenology SUCROS parameters: Default values are used.", pGe->acVarietyName,pGe->acCropName);
					PRINT_MESSAGE(self,4,S);
					g_free(S);
					break;
				}
		}
	g_strfreev(varietynames);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","BaseTemperature1");
	pGe->fBaseTemperature1 = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","CriticalTemperatureSum1");
	pGe->fCriticalTemperatureSum1 = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","BaseTemperature2");
	pGe->fBaseTemperature2 = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"phenology","CriticalTemperatureSum2");
	pGe->fCriticalTemperatureSum2 = dummy_in[i];
	g_free(dummy_in);
//ontogenesis
	GET_INI_STRING_ARRAY(varietynames,varietynum,"ontogenesis","VarietyName");
	for(i=0; i<varietynum; i++)
		{
			deleteSpaceBegEnd(varietynames[i]);
			if(strcmp(pGe->acVarietyName,varietynames[i])==0) break;
			if((i==varietynum-1)&&(strcmp(pGe->acVarietyName,varietynames[varietynum-1])!=0))
				{
					i = 0;
					S  = g_strdup_printf("Variety \"%s\" not found in %s.ini - ontogenesis: Default values are used.", pGe->acVarietyName,pGe->acCropName);
					PRINT_ERROR(S);
					g_free(S);
					break;
				}
		}
	g_free(varietynames);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"ontogenesis","MaxLeafInitRate");
	pGe->fMaxLfInitRate = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"ontogenesis","MaxLeafAppRate");
	pGe->fMaxLfAppRate = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"ontogenesis","TillerDem");
	pGe->fTillerDemand = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"ontogenesis","StorageOrganNumCoeff");
	pGe->fStorageOrganNumCoeff = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"ontogenesis","SpecLeafWeight");
	pGe->fSpecLfWeight = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"ontogenesis","VRStageLW");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"ontogenesis","MultiplicationFactor");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->SpecLfWght = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->SpecLfWght[0].iInLen = dummy_in_size;
	pGe->SpecLfWght[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->SpecLfWght[i].fInput = dummy_in[i];
			pGe->SpecLfWght[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE( pGe->fTempMinLfApp,"ontogenesis","TempMinLfApp");
	GET_INI_DOUBLE( pGe->fTempOptLfApp,"ontogenesis","TempOptLfApp");
	GET_INI_DOUBLE( pGe->fTempMaxLfApp,"ontogenesis","TempMaxLfApp");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"ontogenesis","LfAppRateTemp");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"ontogenesis","LfAppRate");
	pGe->LfAppRateTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->LfAppRateTemp[0].fInput = -99.9;
	pGe->LfAppRateTemp[0].fOutput = -99.9;
	pGe->LfAppRateTemp[0].iInLen = 1;
	pGe->LfAppRateTemp[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->LfAppRateTemp);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->LfAppRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->LfAppRateTemp[0].iInLen = dummy_in_size;
			pGe->LfAppRateTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->LfAppRateTemp[i].fInput = dummy_in[i];
					pGe->LfAppRateTemp[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
//partitioning
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"partitioning","VRStageRoot");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"partitioning","PartitioningRoot");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->PartRoot = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->PartRoot[0].iInLen = dummy_in_size;
	pGe->PartRoot[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->PartRoot[i].fInput = dummy_in[i];
			pGe->PartRoot[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"partitioning","VRStageLeaf");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"partitioning","PartitioningLeaf");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->PartLeaf = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->PartLeaf[0].iInLen = dummy_in_size;
	pGe->PartLeaf[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->PartLeaf[i].fInput = dummy_in[i];
			pGe->PartLeaf[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"partitioning","VRStageStem");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"partitioning","PartitioningStem");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->PartStem = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->PartStem[0].iInLen = dummy_in_size;
	pGe->PartStem[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->PartStem[i].fInput = dummy_in[i];
			pGe->PartStem[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"partitioning","VRStageStorage");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"partitioning","PartitioningStorage");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->PartStorage = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->PartStorage[0].iInLen = dummy_in_size;
	pGe->PartStorage[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->PartStorage[i].fInput = dummy_in[i];
			pGe->PartStorage[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
//storage
	GET_INI_STRING_ARRAY_OPTIONAL(varietynames,varietynum,1,"missing","storage","VarietyName");
	for(i=0; i<varietynum; i++)
		{
			deleteSpaceBegEnd(varietynames[i]);
			if(strcmp(pGe->acVarietyName,varietynames[i])==0) break;
			if((i==varietynum-1)&&(strcmp(pGe->acVarietyName,varietynames[varietynum-1])!=0))
				{
					i = 0;
					S  = g_strdup_printf("Variety \"%s\" not found in %s.ini - storage: Default values are used.", pGe->acVarietyName,pGe->acCropName);
					PRINT_ERROR(S);
					g_free(S);
					break;
				}
		}
	g_free(varietynames);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"storage","ReserveRatio");
	pGe->fReserveRatio = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"storage","RelResvGrwRate");
	pGe->fRelResvGrwRate = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"storage","RelStorageFillRate");
	pGe->fRelStorageFillRate = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"storage","RelStorageNFillRate");
	pGe->fRelStorageNFillRate = dummy_in[i];
	g_free(dummy_in);
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"storage","ReservGrwLagDVS");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"storage","ReservGrwLag");
	pGe->ReservGrwLag = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->ReservGrwLag[0].fInput = -99.9;
	pGe->ReservGrwLag[0].fOutput = -99.9;
	pGe->ReservGrwLag[0].iInLen = 1;
	pGe->ReservGrwLag[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->ReservGrwLag);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->ReservGrwLag = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->ReservGrwLag[0].iInLen = dummy_in_size;
			pGe->ReservGrwLag[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->ReservGrwLag[i].fInput = dummy_in[i];
					pGe->ReservGrwLag[i].fOutput = dummy_out[i];
				}
		}
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"storage","TimeConstant");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"storage","ReservTransTc");
	pGe->ReservTransTc = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->ReservTransTc[0].fInput = -99.9;
	pGe->ReservTransTc[0].fOutput = -99.9;
	pGe->ReservTransTc[0].iInLen = 1;
	pGe->ReservTransTc[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->ReservTransTc);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->ReservTransTc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->ReservTransTc[0].iInLen = dummy_in_size;
			pGe->ReservTransTc[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->ReservTransTc[i].fInput = dummy_in[i];
					pGe->ReservTransTc[i].fOutput = dummy_out[i];
				}
		}
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"storage","SoFillLagDVS");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"storage","SoFillLag");
	pGe->SoFillLag = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->SoFillLag[0].fInput = -99.9;
	pGe->SoFillLag[0].fOutput = -99.9;
	pGe->SoFillLag[0].iInLen = 1;
	pGe->SoFillLag[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->SoFillLag = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->SoFillLag[0].iInLen = dummy_in_size;
			pGe->SoFillLag[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->SoFillLag[i].fInput = dummy_in[i];
					pGe->SoFillLag[i].fOutput = dummy_out[i];
				}
		}
	GET_INI_DOUBLE( pGe->fTempMinSoFill,"storage","TempMinSoFill");
	GET_INI_DOUBLE( pGe->fTempOptSoFill,"storage","TempOptSoFill");
	GET_INI_DOUBLE( pGe->fTempMaxSoFill,"storage","TempMaxSoFill");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"storage","SoFillRateTemp");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"storage","SoFillRate");
	pGe->SoFillRateTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->SoFillRateTemp[0].fInput = -99.9;
	pGe->SoFillRateTemp[0].fOutput = -99.9;
	pGe->SoFillRateTemp[0].iInLen = 1;
	pGe->SoFillRateTemp[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->SoFillRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->SoFillRateTemp[0].iInLen = dummy_in_size;
			pGe->SoFillRateTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->SoFillRateTemp[i].fInput = dummy_in[i];
					pGe->SoFillRateTemp[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE( pGe->fTempMinSoFill,"storage","TempMinSoNFill");
	GET_INI_DOUBLE( pGe->fTempOptSoFill,"storage","TempOptSoNFill");
	GET_INI_DOUBLE( pGe->fTempMaxSoFill,"storage","TempMaxSoNFill");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"storage","SoNFillRateTemp");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"storage","SoNFillRate");
	pGe->SoNFillRateTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
	pGe->SoNFillRateTemp[0].fInput = -99.9;
	pGe->SoNFillRateTemp[0].fOutput = -99.9;
	pGe->SoNFillRateTemp[0].iInLen = 1;
	pGe->SoNFillRateTemp[0].iOutLen = 1;
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			g_free(pGe->SoNFillRateTemp);
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->SoNFillRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->SoNFillRateTemp[0].iInLen = dummy_in_size;
			pGe->SoNFillRateTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->SoNFillRateTemp[i].fInput = dummy_in[i];
					pGe->SoNFillRateTemp[i].fOutput = dummy_out[i];
				}
		}
	g_free(dummy_in);
	g_free(dummy_out);
//nitrogen
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"nitrogen","VROptLvNc");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"nitrogen","OptLvNc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->OptLvNc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->OptLvNc[0].iInLen = dummy_in_size;
	pGe->OptLvNc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->OptLvNc[i].fInput = dummy_in[i];
			pGe->OptLvNc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"nitrogen","VROptStNc");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"nitrogen","OptStNc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->OptStNc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->OptStNc[0].iInLen = dummy_in_size;
	pGe->OptStNc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->OptStNc[i].fInput = dummy_in[i];
			pGe->OptStNc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"nitrogen","VROptRtNc");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"nitrogen","OptRtNc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->OptRtNc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->OptRtNc[0].iInLen = dummy_in_size;
	pGe->OptRtNc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->OptRtNc[i].fInput = dummy_in[i];
			pGe->OptRtNc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"nitrogen","VRMinLvNc");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"nitrogen","MinLvNc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->MinLvNc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->MinLvNc[0].iInLen = dummy_in_size;
	pGe->MinLvNc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->MinLvNc[i].fInput = dummy_in[i];
			pGe->MinLvNc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"nitrogen","VRMinStNc");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"nitrogen","MinStNc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->MinStNc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->MinStNc[0].iInLen = dummy_in_size;
	pGe->MinStNc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->MinStNc[i].fInput = dummy_in[i];
			pGe->MinStNc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"nitrogen","VRMinRtNc");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"nitrogen","MinRtNc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->MinRtNc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->MinRtNc[0].iInLen = dummy_in_size;
	pGe->MinRtNc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->MinRtNc[i].fInput = dummy_in[i];
			pGe->MinRtNc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
//rootgrowth
	GET_INI_DOUBLE( pGe->fMaxRootExtRate,"rootgrowth","MaxRootExtRate");
	GET_INI_DOUBLE( pGe->fRootLengthRatio,"rootgrowth","RootLengthRatio");
	GET_INI_DOUBLE( pGe->fMaxWuptRate,"rootgrowth","MaxWuptRate");
	GET_INI_DOUBLE( pGe->fMaxNuptRate,"rootgrowth","MaxNuptRate");
	//GET_INI_DOUBLE( pGe->fMaxRootDepth,"rootgrowth","MaxRootDepth");
	GET_INI_DOUBLE( pGe->fRootExtTempMin,"rootgrowth","TempMinRootExt");
	GET_INI_DOUBLE( pGe->fRootExtTempOpt,"rootgrowth","TempOptRootExt");
	GET_INI_DOUBLE( pGe->fRootExtTempMax,"rootgrowth","TempMaxRootExt");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,-99.9,"rootgrowth","RootExtRateTemperature");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_out,dummy_out_size,1,-99.9,"rootgrowth","RootExtRateTemp");
	if ((dummy_in[0] != -99.9)&&(dummy_out[0] != -99.9))
		{
			CHECK_LEN(dummy_in_size,dummy_out_size);
			var_len = array_length(dummy_in_size,dummy_out_size);
			pGe->RootExtRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
			pGe->RootExtRateTemp[0].iInLen = dummy_in_size;
			pGe->RootExtRateTemp[0].iOutLen = dummy_out_size;
			for(i=0; i<var_len; i++)
				{
					pGe->RootExtRateTemp[i].fInput = dummy_in[i];
					pGe->RootExtRateTemp[i].fOutput = dummy_out[i];
				}
		}
	else
		{
			pGe->RootExtRateTemp = (RESPONSE*)g_malloc0_n(1,sizeof(RESPONSE));
			pGe->RootExtRateTemp[0].fInput = -99.9;
			pGe->RootExtRateTemp[0].fOutput = -99.9;
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"rootgrowth","RootExtRateWaterContent");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"rootgrowth","RootExtRateWc");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->RootExtRateWc = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->RootExtRateWc[0].iInLen = dummy_in_size;
	pGe->RootExtRateWc[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->RootExtRateWc[i].fInput = dummy_in[i];
			pGe->RootExtRateWc[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
//senescence
	GET_INI_DOUBLE(pGe->fBeginSenesDvs,"senescence","BeginSenesDvs")
	GET_INI_DOUBLE(pGe->fBeginShadeLAI,"senescence","BeginShadeLAI")
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"senescence","LvDeathRateTemp");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"senescence","LvDeathRate");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->LvDeathRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->LvDeathRateTemp[0].iInLen = dummy_in_size;
	pGe->LvDeathRateTemp[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->LvDeathRateTemp[i].fInput = dummy_in[i];
			pGe->LvDeathRateTemp[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"senescence","RtDeathRateTemp");
	GET_INI_DOUBLE_ARRAY(dummy_out,dummy_out_size,"senescence","RtDeathRate");
	CHECK_LEN(dummy_in_size,dummy_out_size);
	var_len = array_length(dummy_in_size,dummy_out_size);
	pGe->RtDeathRateTemp = (RESPONSE*)g_malloc0_n(var_len,sizeof(RESPONSE));
	pGe->RtDeathRateTemp[0].iInLen = dummy_in_size;
	pGe->RtDeathRateTemp[0].iOutLen = dummy_out_size;
	for(i=0; i<var_len; i++)
		{
			pGe->RtDeathRateTemp[i].fInput = dummy_in[i];
			pGe->RtDeathRateTemp[i].fOutput = dummy_out[i];
		}
	g_free(dummy_in);
	g_free(dummy_out);
	
//Hong: added for Scott Demyan et al. 2016.10.06
//residue partition
    GET_INI_STRING_ARRAY_OPTIONAL(varietynames,varietynum,1,"missing","residue partition","VarietyName");	
	for(i=0; i<varietynum; i++)
		{
			deleteSpaceBegEnd(varietynames[i]);
			if(strcmp(pGe->acVarietyName,varietynames[i])==0) break;
			if((i==varietynum-1)&&(strcmp(pGe->acVarietyName,varietynames[varietynum-1])!=0))
				{
					i = 0;
					S  = g_strdup_printf("Variety \"%s\" not found in %s.ini - residue partition: Default values are used.", pGe->acVarietyName,pGe->acCropName);
					PRINT_ERROR(S);
					g_free(S);
					break;
				}
		}

	g_free(varietynames);
	//GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"residue partition","ResidueToAMO1_frac");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,1.0,"residue partition","ResidueToAMO1_frac");//default value 1.0
	pPl->pGenotype->fResidueAMO1Frac = dummy_in[i];
	g_free(dummy_in);
	
    //    GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"residue partition","C_fromDeadleaf_frac");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,0.4,"residue partition","C_fromDeadleaf_frac");//default value 0.4	
	pPl->pGenotype->fCDeadleafFrac = dummy_in[i];
	g_free(dummy_in);
	
    //    GET_INI_DOUBLE_ARRAY(dummy_in,dummy_in_size,"residue partition","N_fromDeadleaf_frac");
	GET_INI_DOUBLE_ARRAY_OPTIONAL(dummy_in,dummy_in_size,1,1.0,"residue partition","N_fromDeadleaf_frac");//default value 1.0	
	pPl->pGenotype->fNDeadleafFrac = dummy_in[i];
	g_free(dummy_in);
//End of Hong	
	g_key_file_free(keyfile);
	return RET_SUCCESS;
}
int  expertn_modul_base_plant_next(expertn_modul_base *self, PPLANT pPl)
{
	PPLANT		item;
	item = (PPLANT)g_malloc0_n(1,sizeof(STPLANT));
	memcpy(item,pPl,sizeof(STPLANT));
	item->pNext->pBack = item;
	memcpy(self->pPl,pPl->pNext,sizeof(STPLANT));
	self->pPl->pBack=item;
	g_free(item);
	self->pMa->pSowInfo = self->pMa->pSowInfo->pNext;
	return RET_SUCCESS;
}
int expertn_modul_base_DevelopmentCheckAndPostHarvestManagement(expertn_modul_base *self)
{
	PPLANT			pPl = self->pPl;
	PTIME			pTi = self->pTi;
	PMANAGEMENT	pMa = self->pMa;
	PCBALANCE	pCB = self->pCh->pCBalance; //Added by Hong on 20180731
	
	double		EPSILON;
	int day,month,year;
	char *S;
	EPSILON = 1e-9;
	if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pMa->pSowInfo->Year,pMa->pSowInfo->Month,pMa->pSowInfo->Day)==0)&&(NewDay(pTi)))
		{
			S  = g_strdup_printf("Crop '%s': Variety '%s', Sow date: %d-%02d-%02d, Harvest date: %d-%02d-%02d\n", pPl->pGenotype->acCropName, pPl->pGenotype->acVarietyName, pMa->pSowInfo->Year, pMa->pSowInfo->Month, pMa->pSowInfo->Day,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay);
			PRINT_MESSAGE(self,3,S);
			g_free(S);
			pPl->pDevelop->bPlantGrowth = TRUE;
			pPl->pDevelop->bMaturity=FALSE;
		}
	day=pTi->pSimTime->mday;
	month=pTi->pSimTime->mon;
	year=pTi->pSimTime->year;
	xpn_time_date_add_dt(&year,&month,&day,-1.0);
	if (((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))&&(NewDay(pTi)))
		{
			if (pPl->pDevelop->fStageSUCROS>=(double)0.0)
				pPl->pDevelop->iDayAftEmerg++;
			if (pPl->pDevelop->fStageSUCROS>=(double)2.0)
				{
					pPl->pDevelop->bMaturity=TRUE;
					expertn_modul_base_SetAllPlantRateToZero(self);
				}
		}
	//if (pTi->pSimTime->fTimeAct == (double)(pPl->pModelParam->iHarvestDay-1))
	if ((pPl->pModelParam->HarvestDay==day) && (pPl->pModelParam->HarvestMonth==month) && (pPl->pModelParam->HarvestYear==year) && (NewDay(pTi)))
		{
			double *pMat1Local,factor,amount,actDepth,RootProp,RootSum;
			int    i1,i2;
			double fCut,fHeight;
			double fCResidue,fNResidue;
			double fCStandR,fNStandR;
			double fStaFak;	
            double fCumDepth; 		
			PSLAYER		pSL	= self->pSo->pSLayer->pNext;
			PMANAGEMENT pMa = self->pMa;
			PSPROFILE	pSo = self->pSo;
			PCHEMISTRY	pCh = self->pCh;
			PCLAYER		pCL = pCh->pCLayer;
			PCPROFILE	pCP = self->pCh->pCProfile;
			i1 = (int)99; //aktuelle Bodenschichten
			RootSum = actDepth = (double)0.0;
			pMat1Local        = (double *) malloc(i1 * sizeof(double));
			memset(pMat1Local,0x0,(i1 * sizeof(double)));
			// The aboveground biomass at iHarvestDay must be written
			// to the surfacepools and the rootbiomass to the Litterpools
			/*
			if(pPl->pModelParam->cResidueCarryOff==0)
			{
			pCh->pCProfile->fDryMatterLitterSurf += pPl->pBiomass->fBiomassAbvGround-pPl->pBiomass->fGrainWeight;
			pCh->pCProfile->fNLitterSurf += pCh->pCProfile->fDryMatterLitterSurf * pPl->pPltNitrogen->fVegActConc;//pPl->pPltNitrogen->fCumActNUpt;
			pCh->pCProfile->fCLitterSurf += (pPl->pBiomass->fBiomassAbvGround-pPl->pBiomass->fGrainWeight) * (double)0.4 ; // 40% C in Biomasse
			if (pCh->pCProfile->fNLitterSurf > (double)0.0)
			pCh->pCProfile->fCNLitterSurf = pCh->pCProfile->fCLitterSurf /
														pCh->pCProfile->fNLitterSurf;
			}
			*/
			//added by Hong 20170522, necessary in case of for an unknown crop
			fCut =(double)5.0; 
			fHeight=(double)20;
			//End of Hong
			
			/* Initialisieren des Standing / Verteilungsfaktor */
			if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)
				{
					//Weizen und alles andere
					fCut = (double)15;
					fHeight = (double)100;
				}
			else if (strcmp(pPl->pGenotype->acCropCode,"BA")==0)
				{
					//Gerste
					fCut = (double)15;
					fHeight = (double)100;
				}
			else if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
				{
					//Mais
					fCut = (double)20;
					fHeight = (double)200;
				}
			else if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
				{
					//Kartoffel
					fCut = (double)0;
					fHeight = (double)60;
				}
			else if (strcmp(pPl->pGenotype->acCropCode,"SF")==0)
				{
					//Sonnenblumen
					fCut = (double)150;
					fHeight = (double)200;
				}
				
// Hong: changed for Scott Demyan et al. 2016.10.06 				
			 else if (strcmp(pPl->pGenotype->acCropCode,"SB")==0||(strcmp(pPl->pGenotype->acCropCode,"BS")==0))    //Hong: SB/BS=>sugar beet     
                { //Zuckerruebe
                    fCut = (double)0;
                    fHeight = (double)40;
				}
             else if ((strcmp(pPl->pGenotype->acCropCode,"RP")==0)||(strcmp(pPl->pGenotype->acCropCode,"RY")==0))
                { //Raps
                   fCut = (double)60;
				  fHeight = (double)160;
                }
			 //Added by Hong 20170519
	         else{
				  fCut = (double)0;
				  fHeight = (double)160;
				 }
	 			
// End of Hong 			
				
			fStaFak = fCut / fHeight;
			// Falls ohne Standing Pool gerechnet werden soll
			// if(!iStandingResidues) fStaFak = (double)0; 
			/* Berechnen stehender und liegender Anteil */
			//fCResidue = (pPl->pBiomass->fBiomassAbvGround - pPl->pBiomass->fGrainWeight) * (double)0.4;
			fCResidue = pPl->pBiomass->fStovWeight * (double)0.4;
			fCStandR  = fStaFak * fCResidue;
			//fNResidue = fCResidue * pPl->pPltNitrogen->fTopsActConc;
			fNResidue = pPl->pBiomass->fStovWeight * pPl->pPltNitrogen->fTopsActConc;
			fNStandR  = fStaFak * fNResidue;
			/* Abfrage ob Zwischenfrucht oder Hauptfrucht im Augenblick (10.07.01)
			nur Ackersenf, Ölrettisch, Winterrübsen, Kleegras und Luzerne als
			Zwischenfrucht definiert */

//Hong:out-noted for Scott Demyan et al. 2016.10.06
/*
			if 	(strcmp(pPl->pGenotype->acCropName,"Ackersenf")==0||
			        strcmp(pPl->pGenotype->acCropName,"Kleegras")==0||
			        strcmp(pPl->pGenotype->acCropName,"Luzerne")==0||
			        strcmp(pPl->pGenotype->acCropName,"Winterrübsen")==0||
			        strcmp(pPl->pGenotype->acCropName,"Ölrettich")==0)
				{
					if(pPl->pModelParam->cResidueCarryOff==0)
						{
							// if(iStandingResidues)
							//{
							//pCP->fCStandCropRes += pPl->pBiomass->fBiomassAbvGround * (double)0.4;
							//pCP->fNStandCropRes += pPl->pBiomass->fBiomassAbvGround * pPl->pPltNitrogen->fVegActConc;
							pCP->fCStandCropRes += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight)*pTi->pTimeStep->fAct * (double)0.4;
							pCP->fNStandCropRes += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight)*pTi->pTimeStep->fAct * pPl->pPltNitrogen->fVegActConc;
						}
					else
						{
							//pCP->fCLitterSurf += pPl->pBiomass->fBiomassAbvGround * (double)0.4;
							//pCP->fNLitterSurf += pPl->pBiomass->fBiomassAbvGround * pPl->pPltNitrogen->fVegActConc;
							pCP->fCLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight)*pTi->pTimeStep->fAct * (double)0.4;
							pCP->fNLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight)*pTi->pTimeStep->fAct * pPl->pPltNitrogen->fVegActConc;
						}
					//}
				}
			else if(pPl->pModelParam->cResidueCarryOff==0)
				{
					pCP->fCStandCropRes  += fCStandR*pTi->pTimeStep->fAct;
					pCP->fCLitterSurf    += (fCResidue - fCStandR)*pTi->pTimeStep->fAct;
					pCP->fNStandCropRes  += fNStandR*pTi->pTimeStep->fAct;
					pCP->fNLitterSurf    += (fNResidue - fNStandR*pTi->pTimeStep->fAct);
				}
			else
				{
					pCP->fCStandCropRes += fCStandR*pTi->pTimeStep->fAct;
					pCP->fNStandCropRes += fNStandR*pTi->pTimeStep->fAct;
				}
			pMa->pLitter->fRootC = pPl->pBiomass->fRootWeight * (double)0.4 ; // 40% C in Biomasse
			if (pPl->pPltNitrogen->fRootCont > (double) 0)
				{
					pMa->pLitter->fRootCN = pPl->pBiomass->fRootWeight * (double)0.4
					                        /pPl->pPltNitrogen->fRootCont;
				}
			else
				{
					pMa->pLitter->fRootCN =(double)0.1;// ep 250399 fRootCN=0.1 overestimates NLitter
				}
*/				
// End of Hong
					   
// Hong: added for Scott Demyan et al. 2016.10.06
/*
double DELT;
DELT = (double)self->pTi->pTimeStep->fAct; //Hong 20170810
if(pPl->pModelParam->cResidueCarryOff==0)
              {	                         		       
			        pCP->fCStandCropRes  += fCStandR*DELT; 
					
			        pCP->fCLitterSurf    += (pPl->pGenotype->fResidueAMO1Frac*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac)*DELT; 
					
				    pCP->fCManureSurf    += ((1.0- pPl->pGenotype->fResidueAMO1Frac)*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac)*DELT; 

				    pCP->fNStandCropRes  += fNStandR*DELT; 
					
				    pCP->fNLitterSurf    += (pPl->pGenotype->fResidueAMO1Frac*(fNResidue - fNStandR)+pPl->pPltNitrogen->fDeadLeafNw*pPl->pGenotype->fNDeadleafFrac)*DELT; 
					
			        pCP->fNManureSurf    += ((1.0- pPl->pGenotype->fResidueAMO1Frac)*(fNResidue - fNStandR)+pPl->pPltNitrogen->fDeadLeafNw*pPl->pGenotype->fNDeadleafFrac)*DELT;

              }
		else 
		  {
            pCP->fCStandCropRes += fCStandR*DELT; 
            pCP->fNStandCropRes += fNStandR*DELT;
						
          }

		pMa->pLitter->fRootC = (pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight) * (double)0.4 ; // 40% C in Biomasse

        if (pPl->pPltNitrogen->fRootCont > (double) 0.0)
		 {
	      pMa->pLitter->fRootCN = (pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight) * (double)0.4
                                                    /(pPl->pPltNitrogen->fRootCont+pPl->pPltNitrogen->fDeadRootNw);
	
		  
         }
       else
        {
          pMa->pLitter->fRootCN =(double)0.1;// ep 250399 fRootCN=0.1 overestimates NLitter
        
}	
*/            
// End of Hong

// Hong: added for Scott Demyan et al. 2016.10.06
	
if(pPl->pModelParam->cResidueCarryOff==0)
              {	                         		       
			        pCP->fCStandCropRes  += fCStandR; 
					
					
					
			        pCP->fCLitterSurf    += (pPl->pGenotype->fResidueAMO1Frac*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac);
                    pCP->fCLeafLitterSurf    += (pPl->pGenotype->fResidueAMO1Frac*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac); //Hong 20190605: for century_n 
					
				    pCP->fCManureSurf    += ((1.0- pPl->pGenotype->fResidueAMO1Frac)*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac); 

                    pCB->dCInputSurf += (pPl->pGenotype->fResidueAMO1Frac*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac)+ ((1.0- pPl->pGenotype->fResidueAMO1Frac)*(fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*pPl->pGenotype->fCDeadleafFrac);//Hong added on 20180731 for C-balance

				    pCP->fNStandCropRes  += fNStandR; 
					
				    pCP->fNLitterSurf    += (pPl->pGenotype->fResidueAMO1Frac*(fNResidue - fNStandR)+pPl->pPltNitrogen->fDeadLeafNw*pPl->pGenotype->fNDeadleafFrac); 
					
			        pCP->fNManureSurf    += ((1.0- pPl->pGenotype->fResidueAMO1Frac)*(fNResidue - fNStandR)+pPl->pPltNitrogen->fDeadLeafNw*pPl->pGenotype->fNDeadleafFrac);


              }
		else 
		  {
            pCP->fCStandCropRes += fCStandR; 
            pCP->fNStandCropRes += fNStandR;
						
          }

		pMa->pLitter->fRootC = (pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight) * (double)0.4 ; // 40% C in Biomasse

        if (pPl->pPltNitrogen->fRootCont > (double) 0.0)
		 {
	      pMa->pLitter->fRootCN = (pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight) * (double)0.4
                                                    /(pPl->pPltNitrogen->fRootCont+pPl->pPltNitrogen->fDeadRootNw);
	
		  
         }
       else
        {
          pMa->pLitter->fRootCN =(double)0.1;// ep 250399 fRootCN=0.1 overestimates NLitter
        
}	
           
// End of Hong
							   			
			// Berechnen des schichtmaessigen Anteils
			for (pSL = pSo->pSLayer->pNext,
			        pCL = pCh->pCLayer->pNext,i2=0;
			        ((pSL->pNext != NULL)&&
			         (pCL->pNext != NULL));
			        pSL = pSL->pNext,
			        pCL = pCL->pNext,i2++)
				{
					actDepth = actDepth + pSL->fThickness;
					RootProp =	(double)exp((double)-3.0 * actDepth / pSo->fDepth);
					RootSum += RootProp;
					pMat1Local[i2] = RootProp;
				}
						   				
			// Anteilsmaessige Verteilung auf die Bodenschichten.
			fCumDepth = 0.0;
			for (pSL = pSo->pSLayer->pNext,
			        pCL = pCh->pCLayer->pNext,i2=0;
			        ((pSL->pNext != NULL)&&
			         (pCL->pNext != NULL));
			        pSL = pSL->pNext,
			        pCL = pCL->pNext,i2++)
				{
					factor = pMat1Local[i2] / RootSum;
					amount = pMa->pLitter->fRootC * factor;
					pCL->fCLitter += amount*pTi->pTimeStep->fAct;
					pCL->fNLitter += amount*pTi->pTimeStep->fAct / pMa->pLitter->fRootCN;
					
					//Hong added on 20180731 for C-balance			
				    //pCB->dCInputCum +=amount*pTi->pTimeStep->fAct;
					pCB->dCInputProfile +=amount*pTi->pTimeStep->fAct;
					 //Hong 20190507: balance for 0-30 cm profile:
					fCumDepth +=(double)0.1*pSL->fThickness; //cm
	                if (fCumDepth <=30.0)
					{
						pCB->dCInputProfile_30 +=amount*pTi->pTimeStep->fAct;
	                 }
					
					if((pCL->fCLitter>EPSILON)&&(pCL->fNLitter>EPSILON))
						{
							pCL->fLitterCN = pCL->fCLitter / pCL->fNLitter;
						}
					else
						pCL->fLitterCN =(double)0.1;
				}
			free(pMat1Local);
										   			
			
			if(pPl->pNext==NULL)
				{
					memset(pPl->pDevelop,0x0,sizeof(STDEVELOP));
					memset(pPl->pBiomass,0x0,sizeof(STBIOMASS));
					memset(pPl->pCanopy,0x0,sizeof(STCANOPY));
					memset(pPl->pPltNitrogen,0x0,sizeof(STPLTNITROGEN));
					memset(pPl->pPltCarbon,0x0,sizeof(STPLTCARBON));
					memset(pPl->pPltWater,0x0,sizeof(STPLTWATER));
						
				}
			else
				{
					expertn_modul_base_plant_next(self,pPl);
					
									   
				}
		}
		
		
	return RET_SUCCESS;
}
void expertn_modul_base_SetAllPlantRateToZero(expertn_modul_base *self)
{
	PPLANT		pPl = self->pPl;
	PSPROFILE	pSo = self->pSo;
	PLAYERROOT	pLayRoot = pPl->pRoot->pLayerRoot;
	int L;
	//--------------------------------------------------------------------------------------
	//Development Stage
	//--------------------------------------------------------------------------------------
	pPl->pDevelop->fDevR		=(double)0.0;
	//--------------------------------------------------------------------------------------
	//Canopy
	//--------------------------------------------------------------------------------------
	pPl->pCanopy->fLAGrowR		=(double)0.0;
	pPl->pCanopy->fLeafSenesR	=(double)0.0;
	//--------------------------------------------------------------------------------------
	//Biomass
	//--------------------------------------------------------------------------------------
	pPl->pBiomass->fLeafGrowR	=(double)0.0;
	pPl->pBiomass->fStemGrowR	=(double)0.0;
	pPl->pBiomass->fRootGrowR	=(double)0.0;
	pPl->pBiomass->fGrainGrowR	=(double)0.0;
	pPl->pBiomass->fStemReserveGrowRate	=(double)0.0;
	pPl->pBiomass->fLeafDeathRate		=(double)0.0;
	pPl->pBiomass->fRootDeathRate		=(double)0.0;
	//--------------------------------------------------------------------------------------
	//Transpiration
	//--------------------------------------------------------------------------------------
	pPl->pPltWater->fPotTranspdt =(double)0.0;
	pPl->pPltWater->fActTranspdt =(double)0.0;
	//--------------------------------------------------------------------------------------
	//Plant water and nitrogen information
	//--------------------------------------------------------------------------------------
	pPl->pPltWater->fActTranspdt	=(double)0.0;
	pPl->pRoot->fUptakeR 			=(double)0.0;
	pPl->pPltNitrogen->fActNUptR	=(double)0.0;
	pPl->pPltNitrogen->fActNUpt		=(double)0.0;
	pPl->pPltNitrogen->fActNO3NUpt	=(double)0.0;
	pPl->pPltNitrogen->fActNH4NUpt	=(double)0.0;
	//--------------------------------------------------------------------------------------
	//Root Information
	//--------------------------------------------------------------------------------------
	pPl->pRoot->fDepthGrowR	=(double)0.0;
	pLayRoot = pPl->pRoot->pLayerRoot;
	for (L=1; L<=pSo->iLayers-2; L++)
		{
			pLayRoot->fLengthDensR	=(double)0.0;
//			pRt->fDieLength[L]		=(double)0.0;
			pLayRoot->fActLayWatUpt	=(double)0.0;
			pLayRoot->fActLayNO3NUpt=(double)0.0;
			pLayRoot->fActLayNH4NUpt=(double)0.0;
			pLayRoot->fActLayNUpt	=(double)0.0;
			pLayRoot = pLayRoot->pNext;
		}
	return;
}
void output_time_step_check_init(double *output_time_step,char **output_time_step_string,char *reg_str)
{
	*output_time_step = 0.0;
	*output_time_step_string = g_strdup_printf("%s",reg_str);
	if (strcmp(*output_time_step_string,"hourly")==0)
		{
			*output_time_step = -1;
		}
	else if (strcmp(*output_time_step_string,"half-hourly")==0)
		{
			*output_time_step = -6;
		}
	else if (strcmp(*output_time_step_string,"quarter-hourly")==0)
		{
			*output_time_step = -2;
		}
	else if (strcmp(*output_time_step_string,"daily")==0)
		{
			*output_time_step = -3;
		}
	else if (strcmp(*output_time_step_string,"monthly")==0)
		{
			*output_time_step = -4;
		}
	else if (strcmp(*output_time_step_string,"yearly")==0)
		{
			*output_time_step = -5;
		}
	else
		{
			*output_time_step = atof(*output_time_step_string);
			if ((*output_time_step) <= 0.0)
				{
					*output_time_step=1.0;
					PRINT_ERROR("Output Time Step <= 0.0 --> was set to 1.0!");
				}
		}
}
int output_time_step_compare_time_step(PTIME pTi,double output_time_step,char *output_time_step_string,STRUCT_XPN_TIME *xpn_time_old,STRUCT_XPN_TIME *xno_time)
{
	STRUCT_XPN_TIME xpn_time;
	int xpn_time_old_iYear;
	double xpn_time_old_fTimeY;
	xpn_time_get_struc(&xpn_time,pTi->pSimTime->iyear,pTi->pSimTime->fTimeY);
	xpn_time_set_struc(xpn_time_old, &xpn_time_old_iYear, &xpn_time_old_fTimeY);
	if (output_time_step  < 0.0)
		{
			if (strcmp(output_time_step_string,"daily")==0)
				{
					if (NewDay(pTi))
						{
							(*xno_time).msec=0.0;
							(*xno_time).sec=0;
							(*xno_time).min=0;
							(*xno_time).hour=0;
							(*xno_time).mday=xpn_time.mday;
							(*xno_time).mon=xpn_time.mon;
							(*xno_time).year=xpn_time.year;
							(*xno_time).is_leap_year=xpn_time.is_leap_year;
							(*xpn_time_old) = xpn_time;
							return 0;
						}
					else
						{
							return 1;
						}
				}
			else if (strcmp(output_time_step_string,"hourly")==0)
				{
					if (xpn_time.hour != (*xpn_time_old).hour)
						{
							(*xno_time).msec=0.0;
							(*xno_time).sec=0;
							(*xno_time).min=0;
							(*xno_time).hour=xpn_time.hour;
							(*xno_time).mday=xpn_time.mday;
							(*xno_time).mon=xpn_time.mon;
							(*xno_time).year=xpn_time.year;
							(*xno_time).is_leap_year=xpn_time.is_leap_year;
							(*xpn_time_old) = xpn_time;
							return 0;
						}
					else
						{
							return 1;
						}
				}
			else if (strcmp(output_time_step_string,"quarter-hourly")==0)
				{
					if (xpn_time.min/15 != (*xpn_time_old).min/15)
						{
							(*xno_time).msec=0.0;
							(*xno_time).sec=0;
							(*xno_time).min=(xpn_time.min/15)*15;
							(*xno_time).hour=xpn_time.hour;
							(*xno_time).mday=xpn_time.mday;
							(*xno_time).mon=xpn_time.mon;
							(*xno_time).year=xpn_time.year;
							(*xno_time).is_leap_year=xpn_time.is_leap_year;
							(*xpn_time_old) = xpn_time;
							return 0;
						}
					else
						{
							return 1;
						}
				}
			else if (strcmp(output_time_step_string,"half-hourly")==0)
				{
					if (xpn_time.min/30 != (*xpn_time_old).min/30)
						{
							(*xno_time).msec=0.0;
							(*xno_time).sec=0;
							(*xno_time).min=(xpn_time.min/30)*30;
							(*xno_time).hour=xpn_time.hour;
							(*xno_time).mday=xpn_time.mday;
							(*xno_time).mon=xpn_time.mon;
							(*xno_time).year=xpn_time.year;
							(*xno_time).is_leap_year=xpn_time.is_leap_year;
							(*xpn_time_old) = xpn_time;
							return 0;
						}
					else
						{
							return 1;
						}
				}
			else if (strcmp(output_time_step_string,"monthly")==0)
				{
					if (xpn_time.mon != (*xpn_time_old).mon)
						{
							(*xno_time).msec=0.0;
							(*xno_time).sec=0;
							(*xno_time).min=0;
							(*xno_time).hour=0;
							(*xno_time).mday=xpn_time.mday;
							(*xno_time).mon=xpn_time.mon;
							(*xno_time).year=xpn_time.year;
							(*xno_time).is_leap_year=xpn_time.is_leap_year;
							(*xpn_time_old) = xpn_time;
							return 0;
						}
					else
						{
							return 1;
						}
				}
			else if (strcmp(output_time_step_string,"yearly")==0)
				{
					if (xpn_time.year != (*xpn_time_old).year)
						{
							(*xno_time).msec=0.0;
							(*xno_time).sec=0;
							(*xno_time).min=0;
							(*xno_time).hour=0;
							(*xno_time).mday=xpn_time.mday;
							(*xno_time).mon=xpn_time.mon;
							(*xno_time).year=xpn_time.year;
							(*xno_time).is_leap_year=xpn_time.is_leap_year;
							(*xpn_time_old) = xpn_time;
							return 0;
						}
					else
						{
							return 1;
						}
				}
		}
	else
		{
			if (xpn_compare_to_added_date(pTi->pSimTime->iyear,pTi->pSimTime->fTimeY,xpn_time_old_iYear,xpn_time_old_fTimeY,output_time_step) >= 0)
				{
					(*xno_time) = xpn_time;
					(*xpn_time_old) = xpn_time;
					return 0;
				}
			else
				{
					return 1;
				}
		}
	return 1;
}
void expertn_modul_base_init_timespan_parse(char *Str,struct_timeperiod *timespan)
{
	char *Timefrom,*TimeTo,*date_str;
	int year,month,day;
	GDate date;
	timespan->iYear_from=0;
	timespan->fTimeY_from=0.0;
	timespan->iYear_to=0;
	timespan->fTimeY_to=0.0;
	g_date_clear(&date,1);
	Timefrom = (char*)g_malloc0(strlen(Str)+1);
	TimeTo = (char*)g_malloc0(strlen(Str)+1);
	date_str = (char*)g_malloc0(strlen(Str)+1);
	getStringColumn(Timefrom,Str,":",0);
	getStringColumn(TimeTo,Str,":",1);
	getStringColumn(date_str,Timefrom,"-",0);
	year=atoi(date_str);
	getStringColumn(date_str,Timefrom,"-",1);
	month=atoi(date_str);
	getStringColumn(date_str,Timefrom,"-",2);
	day=atoi(date_str);
	g_date_set_day(&date,day);
	g_date_set_month(&date,month);
	g_date_set_year(&date,year);
	xpn_time_conv_gdate_to_xpn_time(&date,&(timespan->iYear_from), &(timespan->fTimeY_from));
	getStringColumn(date_str,TimeTo,"-",0);
	year=atoi(date_str);
	getStringColumn(date_str,TimeTo,"-",1);
	month=atoi(date_str);
	getStringColumn(date_str,TimeTo,"-",2);
	day=atoi(date_str);
	g_date_set_day(&date,day);
	g_date_set_month(&date,month);
	g_date_set_year(&date,year);
	xpn_time_conv_gdate_to_xpn_time(&date,&(timespan->iYear_to), &(timespan->fTimeY_to));
	g_free(Timefrom);
	g_free(TimeTo);
	g_free(date_str);
}
void expertn_modul_base_init_timespan(struct_timespan *timespan,char *Str)
{
	char *Str_part;
	int i;
	timespan->active=FALSE;
	timespan->span=NULL;
	timespan->span_len=0;
	if (Str==NULL) return;
	timespan->span_len = getStringColumn_get_Size(Str,";");
	Str_part = (char*)g_malloc0(strlen(Str)+1);
	timespan->span = g_malloc0(sizeof(struct_timeperiod)*timespan->span_len);
	for (i=0; i<timespan->span_len; i++)
		{
			getStringColumn(Str_part,Str,";",i);
			expertn_modul_base_init_timespan_parse(Str_part,&(timespan->span[i]));
		}
	g_free(Str_part);
	timespan->active=TRUE;
}
int expertn_modul_base_check_time_span(struct_timespan *timespan,PTIME pTi)
{
	int i;
	struct_timeperiod *period;
	if (timespan->active==FALSE) return 0;
	for (i=0; i<timespan->span_len; i++)
		{
			period = &(timespan->span[i]);
			if ((xpn_time_compare_time(pTi->pSimTime->iyear,pTi->pSimTime->fTimeY,period->iYear_from,period->fTimeY_from)>=0)
			        &&
			        (xpn_time_compare_time(pTi->pSimTime->iyear,pTi->pSimTime->fTimeY,period->iYear_to,period->fTimeY_to)<=0))
				{
					return 0;
				}
		}
	return 1;
}
void expertn_modul_base_time_span_free(struct_timespan *timespan)
{
	if (timespan->active==FALSE) return;
	G_FREE_IF_NOT_0(timespan->span);
	timespan->span_len=0;
}
