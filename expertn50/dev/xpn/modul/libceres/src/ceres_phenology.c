#include "ceres_phenology.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NewDayAndPlantGrowing   ((NewDay(pTi))&&(xpn->pPl->pDevelop->bPlantGrowth==TRUE)&&(xpn->pPl->pDevelop->bMaturity==FALSE))

int ceres_Initialize_MZ_SF(ceres *self)
{

	//Initialisierung der globalen Variablen, die nicht im EXP_POINTER stehen
	self->fPHINT=self->fXDevStage=self->fP5=self->fP9=self->fNFAC= (double)0.0;
	self->fStemWeightMin=self->fStemWeightMax= (double)0.0;
	self->fHeadWeight=self->fHeadGrowR=self->fPotHeadWeight=self->fHeadWeightMin=self->fHeadWeightMax= (double)0.0;
	self->fPericarpWeight=self->fPericarpGrowR=self->fPotPericarpGrowR= (double)0.0;
	self->fEmbryoWeight=self->fEmbryoGrowR= (double)0.0;
	self->fIGrainWeight=self->fOil=self->fOilPercent= (double)0.0;
	self->fNHeadActConc=self->fNHeadMinConc=self->fNHeadOptConc=self->fNHeadDemand=self->fNHeadCont= (double)0.0;
	self->fNPericarpActConc=self->fNPericarpOptConc=self->fNPericarpDemand=self->fNPericarpCont= (double)0.0;
	self->fNEmbryoActConc=self->fNEmbryoOptConc=self->fNEmbryoDemand=self->fNEmbryoCont= (double)0.0;
	self->fNGreenLeafActConc=self->fNGreenLeafCont=self->fNSenesLeafCont= (double)0.0;
	self->fSumPS=self->fRI1=self->fRM=self->fRFR=self->fGrowFactor=self->ppp= (double)0.0;
	self->fCumSenesLA=self->fSpecLeafArea=self->fAPLA= (double)0.0;
	self->fSLOPEPE=self->fXPEPE=self->fYRAT= (double)0.0;
	self->fEarWeight=self->fEarGrowR=self->fEarFraction=(double)0.0;
	self->fTasselLeafNum=self->fMaxPlantLA=(double)0.0;
	self->iDurP=self->iSenesTime=self->iJPEPE=self->iStageOld=self->iTLNO=self->iSlowFillDay=0;
	self->bSenesCode = 0;

	return RET_SUCCESS;
}

double ceres_spass_DaylengthAndPhotoperiod(double fLatitude, int nJulianDay, int nID)
{
	double SINLD=0.0;
	double COSLD=0.0;
	double vLatitude=0.0;
	double vDay=0.0;
	double vDec=0.0;
	double RAD=0.0;
	double  fReturn=(double)0.0;
	double  fDaylengthAst=(double)0.0;
	double  fPhotoperiod=(double)0.0;
	double  fDaylengthPs=(double)0.0;

	RAD 	  = PI/180.0;
	vLatitude = (double)fLatitude;
	vDay 	  = (double)nJulianDay;

	//Declination of the sun as function of iJulianDaynumber (iJulianDay)
//	vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0));
	vDec = sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0);
	vDec = - asin(vDec);

	//Intermediate variables
	SINLD = sin(RAD*vLatitude)*sin(vDec);
	COSLD = cos(RAD*vLatitude)*cos(vDec);

	//Astronomical daylength (hr)
	fDaylengthAst  =(double)(12.0*(1.0+2.0*asin(SINLD/COSLD)/PI));

	//Photoperiodically active daylength (hr)
	fPhotoperiod =(double)(12.0*(1.0+2.0*asin((-sin(-4.0*RAD)+SINLD)/COSLD)/PI));

	//Photosythesis active daylength (hr)
	fDaylengthPs =(double)(12.0*(1.0+2.0*asin((-sin(8.0*RAD)+SINLD)/COSLD)/PI));


	fReturn=fDaylengthAst;

	if (nID==1)
		fReturn=fDaylengthAst;
	if (nID==2)
		fReturn=fPhotoperiod;
	if (nID==3)
		fReturn=fDaylengthPs;

	return fReturn;
}

int ceres_CERES_TO_COMVR(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;

	//==========================================================================================
	//Phasic Development
	//==========================================================================================
	int             i,iStage;
	PDEVELOP        pDev;
	double          fValue, VR[10];
//      double          DVR[]={0.0,     0.1425, 0.45,   0.656,  0.91,   1.00,   1.15,   1.50,   1.99,   2.0};

	//SG 2006 03 28, test: (DVR[] aus SUCROS)
    //double          DVR[]={0.0,     0.15,   0.35,   0.55,   0.70,   1.00,   1.25,   1.60,   1.90,   2.0};
	double              DVR[]= {0.0,     0.4,    0.55,   0.656,  0.91,   1.00,   1.15,   1.50,   1.95,   2.0};

	for (i=0; i<10; i++)
		VR[i]=(double)DVR[i];

	iStage  = pPl->pDevelop->iStageCERES;
	pDev    = pPl->pDevelop;

	switch(iStage) {
	case 8: //Sowing to germination
	case 9: //Germination to emergence
		pDev->fStageWang= pDev->fStageXCERES;
		pDev->fStageSUCROS      = pDev->fStageXCERES - (double)1.0;
		break;

	case 1: //Emergence to terminal spikelet
		fValue =(VR[1]-VR[0])/(VR[2]-VR[0]);

		if (pDev->fStageXCERES<=(double)1.0+fValue) {
			pDev->fStageWang        =(double)1.0 + (pDev->fStageXCERES-(double)1.0)/fValue;
			pDev->fStageSUCROS              =(double)0.0 + (pDev->fStageWang-(double)1.0)*VR[1];
		} else {
			pDev->fStageWang        = (double)2.0+ (pDev->fStageXCERES-((double)1.0+fValue))
			                          /((double)1.0-fValue);
			pDev->fStageSUCROS              = VR[1]+(pDev->fStageWang-(double)2.0)*(VR[2]-VR[1]);
		}

		break;

	case 2: //Terminal spikelet to end of leaf growth
		fValue = (VR[3]-VR[2])/(VR[4]-VR[2]);

		if (pDev->fStageXCERES<=(double)2.0+fValue) {
			pDev->fStageWang = (double)3.0 + (pDev->fStageXCERES-(double)2.0)/fValue;
			pDev->fStageSUCROS       = VR[2]+(pDev->fStageWang-(double)3.0)*(VR[3]-VR[2]);
		} else {
			pDev->fStageWang        = (double)4.0+ (pDev->fStageXCERES-((double)2.0+fValue))
			                          /((double)1.0-fValue);
			pDev->fStageSUCROS              = VR[3]+(pDev->fStageWang-(double)4.0)*(VR[4]-VR[3]);
		}

		break;

	case 3: //End of leaf growth to Heading

		fValue = VR[5]-VR[4];

		pDev->fStageWang = (double)5.0 + (pDev->fStageXCERES-(double)3.0);
		pDev->fStageSUCROS       = VR[4]+(pDev->fStageWang-(double)5.0)*fValue;

		break;

	case 4: //Heading to Milk development
		fValue =VR[6]-VR[5];

		pDev->fStageWang        =(double)6.0 + (pDev->fStageXCERES-(double)4.0);
		pDev->fStageSUCROS              =VR[5] + (pDev->fStageWang-(double)6.0)*fValue;
		break;


	case 5: //Milk development(begin of grain filling) to End of grain filling (ripening)
		fValue =(VR[7]-VR[6])/(VR[8]-VR[6]);

		if(pDev->fStageXCERES<=(double)5.0+fValue) {
			pDev->fStageWang        =(double)7.0 + ((pDev->fStageXCERES-(double)5.0))/fValue;
			pDev->fStageSUCROS              =VR[6] + (pDev->fStageWang-(double)7.0)*(VR[7]-VR[6]);
		} else {
			pDev->fStageWang        =(double)8.0 + (pDev->fStageXCERES-((double)5.0+fValue))/((double)1.0-fValue);
			pDev->fStageSUCROS              =VR[7] + (pDev->fStageWang-(double)8.0)*(VR[8]-VR[7]);
		}

		break;

	case 6:
	case 7:
		fValue =VR[9]-VR[8];


		pDev->fStageWang = (double)9.0 + (pDev->fStageXCERES-(double)6.0);
		pDev->fStageSUCROS       = VR[8]+(pDev->fStageWang-(double)9.0)*fValue;

		break;



		/*     case 2: //Terminal spikelet to end of leaf growth
		                       fValue = VR[3]-VR[2];

		                       pDev->fStageWang = (double)3.0 + (pDev->fStageXCERES-(double)2.0);
		                       pDev->fStageSUCROS       = VR[2]+(pDev->fStageWang-(double)3.0)*fValue;

		                       break;

		               case 3: //End of leaf growth to Heading

		                       fValue = VR[4]-VR[3];

		                       pDev->fStageWang = (double)4.0 + (pDev->fStageXCERES-(double)3.0);
		                       pDev->fStageSUCROS       = VR[3]+(pDev->fStageWang-(double)4.0)*fValue;

		                       break;

		               case 4: //Heading to Milk development
		                       fValue =(VR[5]-VR[4])/(VR[6]-VR[4]);

		                       if (pDev->fStageXCERES<=(double)4.0+fValue)
		                               {
		                               pDev->fStageWang        =(double)5.0 + (pDev->fStageXCERES-(double)4.0)/fValue;
		                               pDev->fStageSUCROS              =VR[4] + (pDev->fStageWang-(double)5.0)
		                                                                                                *(VR[5]-VR[4]);
		                               }
		                       else
		                               {
		                               pDev->fStageWang        = (double)6.0+(pDev->fStageXCERES-((double)4.0+fValue))
		                                                                                                       /((double)1.0-fValue);
		                               pDev->fStageSUCROS              = VR[5]+(pDev->fStageWang-(double)6.0)*(VR[6]-VR[5]);
		                               }
		                       break;


		               case 5: //Milk development(begin of grain filling) to End of grain filling (ripening)
		                       fValue =(VR[7]-VR[6])/(VR[8]-VR[6]);

		                       if (pDev->fStageXCERES<=(double)5.0+fValue)
		                               {
		                               pDev->fStageWang        =(double)7.0 + (pDev->fStageXCERES-(double)5.0)/fValue;
		                               pDev->fStageSUCROS              =VR[6] + (pDev->fStageWang-(double)7.0)
		                                                                                                *(VR[7]-VR[6]);
		                               }
		                       else
		                               {
		                               pDev->fStageWang        = (double)8.0+(pDev->fStageXCERES-((double)5.0+fValue))
		                                                                                                       /((double)1.0-fValue);
		                               pDev->fStageSUCROS              = VR[7]+(pDev->fStageWang-(double)8.0)*(VR[8]-VR[7]);
		                               }

		                       break;

		               case 6:
		               case 7:

		                       fValue = VR[9]-VR[8];

		                       pDev->fStageWang = (double)9.0 + (pDev->fStageXCERES-(double)6.0);
		                       pDev->fStageSUCROS       = VR[8]+(pDev->fStageWang-(double)9.0)*fValue;

		                       break;*/
	}


	if (pDev->fStageWang<=(double)9.0)
		pDev->fDevStage=pDev->fStageWang;
	else
		pDev->fDevStage=(double)9.0+(double)0.2*(pDev->fStageWang-(double)9.0);


	pDev->fDevStage *= (double)10.0;        //SG/14/06/99: wie in SPASS!

	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
		if (pDev->fStageXCERES<=(double)1.0)
			pDev->fDevStage=pDev->fStageXCERES*(double)10.0;

		if ((pDev->fStageXCERES>(double)1.0)&&(pDev->fStageXCERES<=(double)2.0))
			pDev->fDevStage=(double)10.0+(pDev->fStageXCERES-(double)1.0)*(double)10.0;

		if ((pDev->fStageXCERES>(double)2.0)&&(pDev->fStageXCERES<=(double)3.0))
			pDev->fDevStage=(double)20.0+(pDev->fStageXCERES-(double)2.0)*(double)16.0;

		if ((pDev->fStageXCERES>(double)3.0)&&(pDev->fStageXCERES<=(double)4.0))
			pDev->fDevStage=(double)36.0+(pDev->fStageXCERES-(double)3.0)*(double)24.0;

		if ((pDev->fStageXCERES>(double)4.0)&&(pDev->fStageXCERES<=(double)5.0))
			pDev->fDevStage=(double)60.0+(pDev->fStageXCERES-(double)4.0)*(double)20.0;

		/*              if ((pDev->fStageXCERES>(double)5.0)&&(pDev->fStageXCERES<=(double)6.0))
		                        pDev->fDevStage=(double)80.0+(pDev->fStageXCERES-(double)5.0)*(double)5.0;

		                if ((pDev->fStageXCERES>(double)6.0)&&(pDev->fStageXCERES<=(double)7.0))
		                        pDev->fDevStage=(double)85.0+(pDev->fStageXCERES-(double)6.0)*(double)4.0;

		                if (pDev->fStageXCERES>(double)7.0)
		                        pDev->fDevStage=(double)89.0;
		*/
//SG 2007 07 26:
		if ((pDev->fStageXCERES>(double)5.0)&&(pDev->fStageXCERES<=(double)6.0))
			pDev->fDevStage=(double)80.0+(pDev->fStageXCERES-(double)5.0)*(double)9.0;

		if ((pDev->fStageXCERES>(double)6.0)&&(pDev->fStageXCERES<=(double)7.0))
			pDev->fDevStage=(double)89.0+(pDev->fStageXCERES-(double)6.0)*(double)2.0;

		if (pDev->fStageXCERES>(double)7.0)
			pDev->fDevStage=(double)91.0;
	}

	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		if (pDev->fStageXCERES<=(double)1.0)
			pDev->fDevStage=pDev->fStageXCERES*(double)10.0;

		if ((pDev->fStageXCERES>(double)1.0)&&(pDev->fStageXCERES<=(double)2.0))
			pDev->fDevStage=(double)10.0+(pDev->fStageXCERES-(double)1.0)*(double)10.0;

		if ((pDev->fStageXCERES>(double)2.0)&&(pDev->fStageXCERES<=(double)3.0))
			pDev->fDevStage=(double)20.0+(pDev->fStageXCERES-(double)2.0)*(double)10.0;

		if ((pDev->fStageXCERES>(double)3.0)&&(pDev->fStageXCERES<=(double)4.0))
			pDev->fDevStage=(double)30.0+(pDev->fStageXCERES-(double)3.0)*(double)6.0;

		if ((pDev->fStageXCERES>(double)4.0)&&(pDev->fStageXCERES<=(double)5.0))
			pDev->fDevStage=(double)50.0+(pDev->fStageXCERES-(double)4.0)*(double)30.0;

		if ((pDev->fStageXCERES>(double)5.0)&&(pDev->fStageXCERES<=(double)6.0))
			pDev->fDevStage=(double)80.0+(pDev->fStageXCERES-(double)5.0)*(double)5.0;

		if ((pDev->fStageXCERES>(double)6.0)&&(pDev->fStageXCERES<=(double)7.0))
			pDev->fDevStage=(double)85.0+(pDev->fStageXCERES-(double)6.0)*(double)4.0;

		if (pDev->fStageXCERES>(double)7.0)
			pDev->fDevStage=(double)89.0;
	}



	return 1;
}


int ceres_DevelopmentBeforeEmergence(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT pPl = xpn->pPl;

	//=======================================================================
	//Variable Declaration and Initiation
	//=======================================================================
	int                     iGerm,iReturn;
	double          fDayThermTime,fEmerg;
	PDEVELOP        pDev=pPl->pDevelop;

	iReturn = 0;

	//=======================================================================
	//Germination and Emergence
	//=======================================================================
	switch (pDev->iStageCERES) {
	case 8: //Germination Part
		iGerm=ceres_Germination(self);
		pDev->fStageXCERES=(double)0.0;

		switch (iGerm) {
		case 0:
			iReturn=0;      //Germination doesn't occur
			break;
		case -1:
			iReturn=-1; //Germination fails.
			break;
		case 1:             //Germination occurs
			pDev->iStageCERES=9;
			pDev->fStageXCERES=(double)0.5;
			iReturn=1;
			break;
		}
		break;

	case 9: //Emergence Part


		pDev->fDTT=fDayThermTime=ceres_DailyThermTime(self);
		//SG 2005 11 03: Anpassung an INRA-CERES
		pDev->fSumDTT  += pDev->fDTT;

		if((strcmp(pPl->pGenotype->acCropCode,"SF")==0)||(strcmp(pPl->pGenotype->acCropCode,"MZ")==0)) {
			if (pDev->fDTT<(double)0.0)      pDev->fDTT=(double)0.0;
			pDev->fSumDTT  += pDev->fDTT;
			pDev->fCumDTT  += pDev->fDTT;
		}

		fEmerg=ceres_Emergence(self);

		pDev->fStageXCERES = (double)0.5 + fEmerg/(double)2.0;

		if (fEmerg<(double)0.0) iReturn=-2;

		if ((fEmerg>=(double)0.0)&&(fEmerg<(double)1.0))  //Emergence doesn't occur
			iReturn=0;

		if (fEmerg>=(double)1.0) { //Emergence occurs
			iReturn=2;
			pDev->iStageCERES=1;
		}



		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
		    ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
			//pDev->fStageXCERES=fEmerg;
			ceres_Vernalization_CERES(self);
			//ColdHardness();
		}

		break;
	}


	return iReturn;
}


double ceres_PhotoperiodEffect(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PLOCATION pLo = xpn->pLo;
	PPLANT pPl = xpn->pPl;
	PTIME pTi = xpn->pTi;

	//=========================================================================================
	//Variable Declaration
	//=========================================================================================
	double          DF;

	char* lpCropType        =pPl->pGenotype->acCropCode;
	double fPhotoConstant=pPl->pGenotype->fDaylengthCoeff;
	double fOptDaylength =pPl->pGenotype->fOptDaylen;
	double fDaylength;
	DF = 0.01;

	fDaylength=ceres_spass_DaylengthAndPhotoperiod(pLo->pFarm->fLatitude, (int)pTi->pSimTime->fTimeY,1);
	//=========================================================================================
	//Photoperiod Coefficient
	//=========================================================================================
	if ((strcmp(lpCropType,"WH")==0)||(strcmp(lpCropType,"BA")==0)) {
		if (fDaylength>(double)20.0)    fDaylength=(double)20.0;
		DF=(double)1.0-fPhotoConstant*((double)20.0-fDaylength)*((double)20.0-fDaylength);
	} else {
		//if (fDaylength<fOptDaylength)
		//      fDaylength=fOptDaylength;

		if (strcmp(lpCropType,"SG")==0) {
			DF=(double)(1.0/92.0);
			if (fDaylength>fOptDaylength)
				DF=(double)1.0/((double)102.0+fPhotoConstant*(fDaylength-fOptDaylength));
		}

		if (strcmp(lpCropType,"MI")==0) {
			DF=(double)(1.0/68.0);
			if (fDaylength>fOptDaylength)
				DF=(double)1.0/((double)68.0+fPhotoConstant*(fDaylength-fOptDaylength));
		}

		if (strcmp(lpCropType,"MZ")==0) {
			//fOptDaylength=(double)12.5;
			if (fDaylength<=fOptDaylength)
				fDaylength=fOptDaylength;
			DF=(double)1.0/((double)4.0+fPhotoConstant*(fDaylength-fOptDaylength));
		}

		if (strcmp(lpCropType,"SF")==0) {
			//fOptDaylength=(double)15.0;
			if (fDaylength>=fOptDaylength)
				fDaylength=fOptDaylength;
			DF=(double)1.0/((double)3.0+fPhotoConstant*(fOptDaylength-fDaylength));
		}
	}

	if (DF>(double)1.0)
		DF=(double)1.0;

	if (DF<(double)0.0)
		DF=(double)0.0;

	return DF;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Development after Emergence
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int ceres_DevelopmentAfterEmergence(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;

	//=========================================================================================
	//Variable Declaration and Initiation
	//=========================================================================================
	int     iDevStage0,iDevStage;
	double  DTT,TDU,DF,VF,fDevValue,PHINT0;
	BOOL    bDaylengthSensitive,bVernalization;
	PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;

	PDEVELOP pDev=pPl->pDevelop;

	fDevValue=(double)0.0;

	iDevStage0=iDevStage=pDev->iStageCERES;

	if (iDevStage0==0) {
		pDev->iStageCERES=8;
		return 8;
	}

	//===========================================================================================
	//              Calculation of Daily Thermal Time (DTT) and SumDTT, CumDTT
	//===========================================================================================
	DTT=ceres_DailyThermTime(self);

	if (DTT<(double)0.0)         DTT=(double)0.0;

	pDev->fSumDTT  += DTT;
	pDev->fCumDTT  += DTT;
	pDev->fDTT              = DTT;

	//=========================================================================================
	//              Check whether the crop in this stage is Daylength sensitive or vernalizes
	//=========================================================================================
	bDaylengthSensitive     =FALSE;
	bVernalization          =FALSE;

	//Maize, Sunflower
	if ((strcmp(pPl->pGenotype->acCropCode,"MZ")==0)||
	    (strcmp(pPl->pGenotype->acCropCode,"SF")==0)) {
		if (iDevStage==2)
			bDaylengthSensitive=TRUE;
	}

	//Wheat, Barley
	if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
		if (iDevStage==1)
			bDaylengthSensitive=TRUE;
		if ((iDevStage==9)||(iDevStage==1)) //Wenn iDevStage = 9 wird diesee Funktion doch gar nicht aufgerufen!
			bVernalization=TRUE;
	}

	//===========================================================================================
	//              Calculate the Daylength effect factor (DF)
	//===========================================================================================
	if (bDaylengthSensitive==TRUE)              //This stage is photoperiod sensitive.
		DF=ceres_PhotoperiodEffect(self);
	else                                                                //This stage is photoperiod unsensitive.
		DF=(double)1.0;

	//===========================================================================================
	//              Calculate the vernalization effect factor (VF)
	//===========================================================================================
	if ((bVernalization==TRUE)&&(pPl->pGenotype->fOptVernDays!=0))      //When vernalization exists
//    if (bVernalization==TRUE) //When vernalization exists
		VF=ceres_Vernalization_CERES(self);
	else                       //When no vernalization
		VF=(double)1.0;

	//===========================================================================================
	//              Calculate the Thermal Development Unit (TDU)
	//===========================================================================================
	TDU=DTT*min(DF,VF);

	if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
		if (self->weather.fDaylySolRad<=(double)10.0)
			pPl->pModelParam->fBarleyGrwParam=
			    (double)(1.0-pow((10.0-(double)self->weather.fDaylySolRad),2.0)
			             /pMa->pSowInfo->fPlantDens);
		else
			pPl->pModelParam->fBarleyGrwParam=(double)1.0;

		TDU=TDU*pPl->pModelParam->fBarleyGrwParam;
	}

	pDev->fCumTDU       +=TDU;
	pDev->fDevR      =TDU; //fDevR wird in CERES nicht weiter gebraucht.
	//Dient nur der Schnittstelle zwischen den verschiedenen Pflanzenmodellen
	//===========================================================================================
	//              Calculate Daily Develop Stage Values
	//===========================================================================================
	if ((bDaylengthSensitive==FALSE)&&(bVernalization==FALSE)) {
		fDevValue=pDev->fDTT/pStageParam->afThermalTime[iDevStage-1];

		if((int)(pDev->fStageXCERES+fDevValue)>pDev->iStageCERES) {
			fDevValue = (double)(pDev->iStageCERES+1) - pDev->fStageXCERES +
			            (pDev->fStageXCERES+fDevValue - (double)(pDev->iStageCERES+1))*
			            pStageParam->afThermalTime[iDevStage-1]/pStageParam->afThermalTime[iDevStage];
		}
	}

	else {
		if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
			PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;
			//      while (i<iDevStage)  pStageParam=pStageParam->pNext;

			if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)
				PHINT0=(double)95.0;
			else
				PHINT0=(double)75.0; //Barley

			fDevValue=TDU/pStageParam->afThermalTime[iDevStage-1];
			fDevValue=fDevValue*PHINT0/self->fPHINT;
		}

		//Maize, Sunflower
		if ((strcmp(pPl->pGenotype->acCropCode,"MZ")==0)||(strcmp(pPl->pGenotype->acCropCode,"SF")==0)) { //for Maize
			fDevValue               = DF;
			//pDev->fCumTDU +=DF;
			pDev->fDevR             = DF;

			if(pDev->fStageXCERES+fDevValue > (double)(pDev->iStageCERES+1))
				fDevValue = (double)(pDev->iStageCERES+1) - pDev->fStageXCERES;
		}
	}

	if (fDevValue<(double)0.0)  fDevValue=(double)0.0;

	//===========================================================================================
	//              Calculate Develop stage values
	//===========================================================================================
	pDev->fStageXCERES=pDev->fStageXCERES+fDevValue;
	iDevStage=(int)min(pDev->fStageXCERES,7.0);
	pDev->iStageCERES=iDevStage;



	// Berechnung des aktuellen Entwicklungszustands fXDevStage,
	// der bei Mais und Sonnenblume für die Abschätzung der
	// NitrogenConcentrationLimits benötigt wird:

	//Maize, Sunflower
	if ((strcmp(pPl->pGenotype->acCropCode,"MZ")==0)||(strcmp(pPl->pGenotype->acCropCode,"SF")==0)) {
		switch (iDevStage) {
		case 1:
			self->fXDevStage = pDev->fStageXCERES-(double)1.0;    // 0 - 1
			break;

		case 2:
			if(self->fXDevStage<(double)1.0)
				self->fXDevStage = (double)(1.0 + 0.5*(self->fXDevStage+fDevValue-1.0));
			else
				self->fXDevStage += fDevValue*((double)0.5);          // 1 - 1.5
			break;

		case 3:
			if(self->fXDevStage<(double)1.5)
				self->fXDevStage = (double)(1.5 + 3.0*(self->fXDevStage+0.5*fDevValue-1.5));
			else
				self->fXDevStage += fDevValue*((double)3.0);          // 1.5 - 4.5
			break;

		case 4:
		case 5:
		case 6:                                                                                         // 4.5 - 10
			if(self->fXDevStage<(double)4.5)
				self->fXDevStage = (double)(4.5 + 5.5*pDev->fDTT/self->fP5);
			else
				self->fXDevStage += pDev->fDTT/self->fP5*((double)5.5);
			break;

		case 7:
			self->fXDevStage = (double)10.0;
			break;
		}
	}

	if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
		//      pDev->fStageXCERES += fDevValue; //SG/14/06/99: wurde doch schon dazuaddiert!

		if (iDevStage==6) {
			pDev->fStageXCERES=pDev->fStageXCERES+(double)0.35;
			pDev->iStageCERES=(int)pDev->fStageXCERES;
		}
	}
	//=====================================================================================
	//      Stage transfer from Wang's fDevStage to BBA Code nach Schütte und Meier (1981)
	//=====================================================================================
	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
		//When daily thermal time accumulation is less than 2.0 in the stage
		//before the maturing date, then maturity occurs.
		if ((iDevStage==6) && (DTT<(double)1.0)) { //for Maize
			pDev->fStageXCERES=pDev->fStageXCERES+(double)1.0;//2.0 (SG 2007 07 26: verzögert Voll-Reife)
			pDev->iStageCERES=pDev->iStageCERES+1;
		}
	}

	return iDevStage;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double ceres_DailyThermTime(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCLIMATE pCl = xpn->pCl;
	PPLANT pPl = xpn->pPl;

	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	double  fTempMinCrop,fTempMaxCrop,fTempAveCrop,fTempDiff,fTempCorr,fTempFac,fTemp;
	double  fDailyThermTime;        //Return Value -Daily Thermal Time (degree.day).
	double  J;
	int     I=1;
	double fTempBaseDev, fTempOptDev, fTempMaxDev;

	char	* lpCropType        =pPl->pGenotype->acCropCode;
	int   iDevStage         =pPl->pDevelop->iStageCERES;
	double fTempMax         =self->weather.fTempMax;
	double fTempMin         =self->weather.fTempMin;
	double fSnow                    =pCl->pWeather->fSnow;
	PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;
	fDailyThermTime=(double)0.0;

	//if(fSnow<(double)0.0)
	//      fSnow = (double)0.0;


	//SG/10/06/99:
	//
	//      Hier gibt es Probleme. pPl->pGenotype->pStageParam ist nicht gesetzt,
	//      deshalb kommt es zum Absturz.
	//      Stattdessen fTempBase = 0°C (vgl. CERES-WHEAT, S. 10)
	/////////////////////////////////////////////////////////////////////////
	//while (I<iDevStage) pStageParam=pStageParam->pNext;
	fTempBaseDev =pStageParam->afTempBase[iDevStage-1];
	fTempOptDev = pStageParam->afTempOpt[iDevStage-1];
	fTempMaxDev = pStageParam->afTempMax[iDevStage-1];

	//===========================================================================================
	//For Wheat and Barly, the Daily Thermal Time is calculated as follows:
	//===========================================================================================
	if (((strcmp(lpCropType,"WH")==0))||((strcmp(lpCropType,"BA")==0))) {
		//The Plant Crown Temperature is assumed to be the air temperature.

		fTempMinCrop=fTempMin;          //fTempMinCrop -Minimum temperature of plant crown (C)
		fTempMaxCrop=fTempMax;      //fTempMaxCrop -Maximum temperature of plant crown (C)

		if (fSnow<(double)0.0)
			fSnow=(double)0.0;

		//Snow raises the Plant Crown Temperature higher than air temperature.
		//      if (fSnow>(double)0.0)
		//      {
		if (fSnow>(double)15.0) //fSnow -Precipitation in the form of Snow (mm)
			fSnow=(double)15.0;
		if (fTempMin<(double)0.0)
			//fTempMinCrop -Minimum temperature of plant crown (C)
			fTempMinCrop=(double)2.0+fTempMin*((double)0.4+
			                                   ((double)0.0018)*(fSnow-(double)15.0)*(fSnow-(double)15.0));
		if (fTempMax<(double)0.0)
			//fTempMaxCrop -Maximum temperature of plant crown (C)
			fTempMaxCrop=(double)2.0+fTempMax*((double)0.4+
			                                   ((double)0.0018)*(fSnow-(double)15.0)*(fSnow-(double)15.0));
		//      }

		//fTempAveCrop -Average temperature of plant crown (C)
		fTempAveCrop=(fTempMaxCrop+fTempMinCrop)/((double)2.0);

		pPl->pPltClimate->fCanopyTemp=fTempAveCrop;


		//Calculating Daily Thermal Time as follows
		fTempDiff=fTempMaxCrop-fTempMinCrop;    //fTempDiff -Temperature Amplitude of Crop (C)

		if (fTempDiff==(double)0.0)
			fTempDiff=(double)1.0;

		if (fTempMaxCrop<fTempBaseDev)          //fTempMinCrop < fTempMaxCrop < fTempBase
			fDailyThermTime=(double)0.0;

		if (fTempMaxCrop>=fTempBaseDev) {
			if (fTempMaxCrop<=fTempOptDev) {
				if (fTempMinCrop>fTempBaseDev)          //fTempBaseDev < fTempMinCrop < fTempMaxCrop < 26.0
					fDailyThermTime=fTempAveCrop-fTempBaseDev;

				if (fTempMinCrop<=fTempBaseDev) { //fTempMinCrop < fTempBaseDev < fTempMaxCrop < 26.0
					fTempCorr=(fTempMaxCrop-fTempBaseDev)/fTempDiff;//fTempCorr -Temperature Correction
					fDailyThermTime=(fTempMaxCrop-fTempBaseDev)/((double)2.0)*fTempCorr;
				}
			}

			if ((fTempOptDev<fTempMaxCrop)&&(fTempMaxCrop<fTempMaxDev)) {
				if (fTempMinCrop<=fTempOptDev) { //fTempMinCrop < 26.0 < fTempMaxCrop < 34.0
					fTempCorr=(fTempMaxCrop-fTempOptDev)/fTempDiff;
					fDailyThermTime=(double)13.0*((double)1.0+fTempCorr)
					                +fTempMinCrop/((double)2.0)*((double)1.0-fTempCorr);
				}
				if (fTempMinCrop>fTempOptDev)   //26.0 < fTempMinCrop < fTempMaxCrop < 34.0
					fDailyThermTime=fTempOptDev;
			}

			if (fTempMaxCrop>=fTempMaxDev) {
				if (fTempMinCrop>=fTempOptDev) { //26.0 < fTempMinCrop < 34.0 < fTempMaxCrop
					fTempCorr=(fTempMaxCrop-fTempMaxDev)/fTempDiff;
					fDailyThermTime=((double)60.0-fTempMaxCrop)*fTempCorr
					                +(fTempOptDev)*((double)1.0-fTempCorr);
				} else { //fTempMinCrop < 26.0          //26.0 < fTempMinCrop < 34.0 < fTempMaxCrop < 34.0 < fTempMaxCrop
					fTempCorr=(fTempMaxCrop-fTempMaxDev)/fTempDiff;
					fDailyThermTime=((double)60.0-fTempMaxCrop)*fTempCorr
					                +(fTempOptDev)*((double)1.0-fTempCorr);

					fTempCorr=(fTempOptDev-fTempMinCrop)/fTempDiff;
					fDailyThermTime=fDailyThermTime*((double)1.0-fTempCorr)
					                +(fTempMinCrop+fTempOptDev)/((double)2.0)*fTempCorr;
				}
			}
		}
	} else
		//===========================================================================================
		//For the Crops other than Wheat and Barly, the Daily Thermal Time is calculated as follows:
		//===========================================================================================
	{
		if ((fTempMin>fTempBaseDev)&&(fTempMax<fTempOptDev))
			fDailyThermTime=(fTempMax+fTempMin)/((double)2.0)-fTempBaseDev;
		else {
			if (fTempMax<fTempBaseDev)
				fDailyThermTime=(double)0.0;
			else {
				if ((fTempMin<=fTempBaseDev)||(fTempMax>=fTempOptDev)) {
					fDailyThermTime=(double)0.0;
					for (I=1; I<=8; I++) {
						J=(double)I;
						fTempFac=(double)(0.931+0.114*J-0.0703*J*J+0.0053*J*J*J);
						fTemp=fTempMin+fTempFac*(fTempMax-fTempMin);
						if ((fTemp>fTempBaseDev)&&(fTemp<=fTempOptDev))
							fDailyThermTime=fDailyThermTime+(fTemp-fTempBaseDev)/((double)8.0);
						if ((fTemp>fTempOptDev)&&(fTemp<fTempMaxDev)) {
							if((strcmp(lpCropType,"SF")==0)) {
								fDailyThermTime=fDailyThermTime+(fTempOptDev-fTempBaseDev)*
								                (double)((1.0-0.007*pow(fTemp-fTempOptDev,2))/8.0);
							} else //z.B. Maize
								fDailyThermTime=fDailyThermTime+(fTempOptDev-fTempBaseDev)*
								                ((double)1.0-(fTemp-fTempOptDev)/((double)10.0))/((double)8.0);
						}
					}
				}
			}
		}
	}

	return fDailyThermTime;
}

int ceres_Germination(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PWATER pWa = xpn->pWa;
	//===========================================
	//Variable Declaration
	//===========================================
	double          fCumDepth,fSWSD,fSWSD1,fSWSD2;
	int                     L,NLAYER,iGermination;
	

	double  fSowDepth = (double)10.0*pMa->pSowInfo->fSowDepth; //mm

	PSLAYER         pSL     =pSo->pSLayer;
	PSWATER         pSWL=pSo->pSWater;
	PWLAYER         pSLW=pWa->pWLayer;

	if(pPl->pDevelop->fStageXCERES==(double)0.0) {
		self->iDayAftSow = 0;
	}

	NLAYER=pSo->iLayers-2;
	//==============================================
	//Get the Soil Water Pointer in Seed Depth
	//==============================================
	fCumDepth=(double)0.0;
	pSL     =pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext;

	for (L=1; L<=NLAYER; L++) {
		fCumDepth += pSL->fThickness;
		if (fCumDepth>=fSowDepth)break;
		pSL = pSL->pNext;;
		pSWL = pSWL->pNext;
		pSLW = pSLW->pNext;
	}

	//============================================================
	//Calculating iGermination Possibility Based on Soil Moisture
	//============================================================
	if (pSLW->fContAct>pSWL->fContPWP)
		//Soil water is sufficient to support Germination
	{
		iGermination=1; //Germination occurs on this day.
		self->iDayAftSow=1;
	} else
		//If pSLW->fContAct<=pSWL->fContPWP,
		// soil water may be a problem
	{
		fSWSD1=(pSLW->fContAct-pSWL->fContPWP)*((double)0.65);
		pSLW = pSLW->pNext;
		pSWL = pSWL->pNext;
		fSWSD2=(pSLW->fContAct-pSWL->fContPWP)*((double)0.35);
		fSWSD=fSWSD1+fSWSD2;

		if (fSWSD>=(double)0.02)        //Soil water is sufficient to support germination
			iGermination=1;         //Germination occurs on this day.
		else {                  //Soil water is not sufficient.
			iGermination=0;         //Germination still not occur.

			self->iDayAftSow++;

			if (self->iDayAftSow>=40) {
				//if ((iDayAftSow<90) && ( (CROP=='WH')||(CROP=='BA') ) )
				if ((self->iDayAftSow<90) &&
				    ((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
				     ||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)))
					iGermination=0;         //Germination still not occurs
				else
					iGermination=-1;        //Germination fails
			}
		}
	}

	//=========================================================================================
	//Calculating iGermination Possibility Based on Soil Temperature
	//=========================================================================================
//      pCl->pWeather->fTempAve=(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)/((double)2.0);
//
//      if (pCl->pWeather->fTempAve<=(double)0.0)
//              iGermination=0;

	return iGermination;
}

double ceres_Emergence(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PTIME	pTi = xpn->pTi;

    int day, month, year;


	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	

	PDEVELOP pDev=pPl->pDevelop;

	if(pDev->fStageXCERES==(double)0.5) {
		self->fP9 = (double)0.0;
		self->fEmergValue = (double)0.0;
	}


	//=========================================================================================
	//Calculate the SumDTT needed for Emergence for different Crops
	//=========================================================================================
	if (strcmp(pPl->pGenotype->acCropCode,"SG")==0)
		self->fP9=(double)20.0+((double)6.0)*pMa->pSowInfo->fSowDepth;

	else if ((strcmp(pPl->pGenotype->acCropCode,"MI")==0)||((strcmp(pPl->pGenotype->acCropCode,"MZ")==0)))
		//      fP9=((double)6.0)*pMa->pSowInfo->fSowDepth;//+(double)15.0;
		self->fP9=(double)15.0+((double)6.0)*pMa->pSowInfo->fSowDepth; //CERES-MAIZE, page 74

	else if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0))
		self->fP9=(double)40.0+((double)10.2)*pMa->pSowInfo->fSowDepth;

	else if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0))
		self->fP9=(double)50.0+((double)10.4)*pMa->pSowInfo->fSowDepth;

	else if ((strcmp(pPl->pGenotype->acCropCode,"SF")==0))
		self->fP9=(double)66.0+((double)11.9)*pMa->pSowInfo->fSowDepth;

	else
		self->fP9 = (double)200.0;

	//=========================================================================================
	//Calculate Emergence Information
	//=========================================================================================
	if (self->fP9>(double)150.0)
		self->fEmergValue=(double)-1.0;
	else
		self->fEmergValue += pDev->fDTT/self->fP9;
        
// Prescribed field emergence in XN3:
//    //-----------------------------------------------------------------------------
//    if(pPl->pModelParam->iEmergenceDay>0) //Wenn "Date of Emergence" in Datenbank oder xnd angegeben, dann erfolgt Feldaufgang an diesem Tag
//	{
//		//SG 20130516------------------------------------------------------------------
//		// Fixen der Emergence nach
//		// SUCROS DEVELOPMENT MODEL 
//		if(pTi->pSimTime->fTimeAct >= (float)(pPl->pModelParam->iEmergenceDay+1))
//		{
//			fEmergValue = (float)max(1.05,fEmergValue);
//			pDev->fSumDTT = fP9;
//		}
//		else
//				fEmergValue = (float)min(0.9,fEmergValue);
//	}

    //SG 20190704:  Emergence takes place at the latest on the day specified in crop_rotation.ini!
  	  if (pDev->fStageSUCROS < 0)
	  {
         day=pTi->pSimTime->mday;
        month=pTi->pSimTime->mon;
        year=pTi->pSimTime->year;
        xpn_time_date_add_dt(&year,&month,&day,1.0);
        if (xpn_time_compare_date(year,month,day,pPl->pModelParam->EmergenceYear, pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceDay)>=0) 	  				  				  				  
        {
          self->fEmergValue = (double)max(1.01,self->fEmergValue);
          pDev->fSumDTT = self->fP9;
        }
      }


	return self->fEmergValue;
}

double ceres_Vernalization_CERES(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);

	PPLANT pPl = xpn->pPl;
	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	double  fTempAveCrop;
	double  VF,VD,VD1,VD2,CumVD;

	PDEVELOP pDev=pPl->pDevelop;

	CumVD=(double)pDev->fCumVernUnit; //Cumulative vernalization till today.

	//=========================================================================================
	//Plant Temperature Calculation
	//=========================================================================================
	//fTempAveCrop is the average plant crown temperature.
	fTempAveCrop=pPl->pPltClimate->fCanopyTemp;

	//=========================================================================================
	//Daily Vernalization Calculation
	//=========================================================================================
	if (self->weather.fTempMax<=(double)0.0)           //if fTempMax lower than 0.0C, no vernalization occurs
		VD=0.0;                         //VD is the vernalization for a day.
	else {                              //if fTempMax>0.0, vernalization may or may not occur
		if (self->weather.fTempMin>(double)15.0)       //if fTempMin>15.0 C, no vernalization occurs.
			VD=0.0;
		else {                      //if fTempMin<15.0 C, vernalization may occur.
			if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) {
				//Vernalization for wheat crops
				VD1=1.4-0.0778*(double)fTempAveCrop;
				VD2=0.5+13.44*
				    (double)(fTempAveCrop/((self->weather.fTempMax-self->weather.fTempMin+(double)3.0)*
				                           (self->weather.fTempMax-self->weather.fTempMin+(double)3.0)));
			} else {                //Vernalization for barly crops ?
				VD1=1.0-0.285*((double)fTempAveCrop-11.0);
				VD2=1.0-0.014*((double)fTempAveCrop-9.0)*((double)fTempAveCrop-9.0);
			}

			VD=max(0.0,min(1.0,min(VD1,VD2)));  //Daily vernalization value.
		}
	}

	//=========================================================================================
	//Cumulative Vernalization Days Calculation
	//=========================================================================================
	CumVD=CumVD+VD; //Cumulative vernalization value (cumulative vernalization days).

	//=========================================================================================
	//Anti-Vernalization
	//=========================================================================================
	if ((CumVD<10.0)&&(self->weather.fTempMax>(double)30.0))
		CumVD=CumVD-0.5*((double)self->weather.fTempMax-30.0);

	if (CumVD<0.0)
		CumVD=0.0;

	//=========================================================================================
	//Vernalization Factor
	//=========================================================================================
	//SG/14/06/99:
	//      wegen pPl->pGenotype->iVernCoeff = 0 VF immer gleich 1.
	//VF=1.0-(double)pPl->pGenotype->iVernCoeff*(50.0-CumVD); //vernalization factor.
	VF=min((double)1.0,1.0-(double)(1.0/pPl->pGenotype->iVernCoeff)*(50.0-CumVD)); //vernalization factor.

	if (VF<=0.0)    VF=0.0;
	if (VF>1.0)     VF=1.0;

	pDev->fVernEff      =(double)VF;
	pDev->fVernUnit     =(double)VD;
	pDev->fCumVernUnit  =(double)CumVD;

	return (double)VF;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int ceres_phenological_development_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PLOCATION pLo = xpn->pLo;
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PTIME pTi = xpn->pTi;
	PGENOTYPE		pGen		= pPl->pGenotype;
	PBIOMASS		pBiom		= pPl->pBiomass;
	PDEVELOP		pDev		= pPl->pDevelop;
	PCANOPY			pCan		= pPl->pCanopy;
	PPLTNITROGEN	pPltN		= pPl->pPltNitrogen;
	PSTAGEPARAM		pStageParam = pPl->pGenotype->pStageParam;
	PMODELPARAM		pPltMP		= pPl->pModelParam;

	int		iStage, iCurrentStage;
	double 	fDaylength,CHGDL;


	//für Sonnenblume:
	double PSKER, fPS;

	CHGDL = 0.0;

	if (((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pMa->pSowInfo->Year,pMa->pSowInfo->Month,pMa->pSowInfo->Day)==0))&&
	    ((!strcmp(pPl->pGenotype->acCropCode,"MZ"))||
	     (!strcmp(pPl->pGenotype->acCropCode,"SF")))) {
		ceres_Initialize_MZ_SF(self);
	}


	if (pPl->pDevelop->iStageCERES==0)
		pPl->pDevelop->iStageCERES=8;

	self->iStageOld=iCurrentStage =pPl->pDevelop->iStageCERES;

	//==========================================================================================
	//Phasic Development
	//==========================================================================================
	if (iCurrentStage>6)
		ceres_DevelopmentBeforeEmergence(self);
	else
		ceres_DevelopmentAfterEmergence(self);

	iCurrentStage =pPl->pDevelop->iStageCERES;

	//==========================================================================================
	//CERES Stage Thermal Time Initialization
	//==========================================================================================

	if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
		fDaylength=ceres_spass_DaylengthAndPhotoperiod(pLo->pFarm->fLatitude, (int)pTi->pSimTime->fTimeY,1);
		CHGDL=fDaylength-self->fDaylengthYesterday;
		self->fDaylengthYesterday = fDaylength;
	}

	if (self->iStageOld!=iCurrentStage) {
		iStage = iCurrentStage-1;

		if(iStage ==0) { //Wechsel von 9 auf 1
            //SG 20190531: fPHINT now from ini-file (as in XN3!)
            if(pPl->pGenotype->fPhyllochronInterval <= 10)
           {
                if ((strcmp(pPl->pGenotype->acCropCode,"WH")==0))
                    self->fPHINT=(double)103.0; //(double)84.0 //(double)95.0 //(double)103.0 (INRA?)
                else if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0))
                    self->fPHINT=(double)77.5-(double)232.6*CHGDL;//(double)95.0 (INRA?)
                else if ((strcmp(pPl->pGenotype->acCropCode,"MZ")==0))
                    self->fPHINT=pPl->pGenotype->pStageParam->afThermalTime[0];
                else if ((strcmp(pPl->pGenotype->acCropCode,"SF")==0))
                    self->fPHINT=pPl->pGenotype->pStageParam->afThermalTime[0];
                else
                    self->fPHINT=pPl->pGenotype->pStageParam->afThermalTime[0];
           }
           else
                    self->fPHINT =  pPl->pGenotype->fPhyllochronInterval;

			//Thermal time of stages 2 + 3

			//benötigt für WH, BA:
			pPl->pGenotype->pStageParam->afThermalTime[1] = (double)3.5 * self->fPHINT;

			/*			//INRA-Ceres:
						if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
							pPl->pGenotype->pStageParam->afThermalTime[1] = (double)4.0 * fPHINT;
			*/

			//benötigt für WH, BA, SF:
			pPl->pGenotype->pStageParam->afThermalTime[2] = (double)2.0 * self->fPHINT;

			self->fP5 = pPl->pGenotype->pStageParam->afThermalTime[3] +
			            pPl->pGenotype->pStageParam->afThermalTime[4] +
			            pPl->pGenotype->pStageParam->afThermalTime[5];
		}


		if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)) {
			if (iStage==0) { //SG 2005 11 03: Anpassung an INRA-CERES
				pDev->fSumDTT = pDev->fSumDTT - self->fP9;
				pDev->fDTT = pDev->fSumDTT;
				pDev->fCumTDU = (double)0.0;
			}
			if ((iStage>=1)&&(self->iStageOld<=5)) {
				if (iStage==1)
					pPl->pDevelop->fSumDTT = (double)0.0;
				else
					pPl->pDevelop->fSumDTT -= pStageParam->afThermalTime[iStage-1];

				if (iStage==4) { //SG 2005 11 03: aus BiomassGrowth_CERES hierher verlegt
					double fStemWeight;

					//====================================================================================
					//	Grain number and weight
					//====================================================================================
					pPl->pPltNitrogen->fVegActConc	=pPl->pPltNitrogen->fTopsActConc;
					pPl->pPltNitrogen->fVegMinConc	=pPl->pPltNitrogen->fTopsMinConc;

					pPl->pCanopy->fPlantGreenLA=pPl->pCanopy->fPlantLA
					                            -pPl->pCanopy->fPlantSenesLA;

					fStemWeight =pBiom->fStemWeight/((double)10.0*pMa->pSowInfo->fPlantDens);

					pCan->fGrainNum	= fStemWeight*pPl->pGenotype->fStorageOrganNumCoeff;
					pCan->fGrainNumSq	= pCan->fGrainNum*pMa->pSowInfo->fPlantDens;

					if (pCan->fGrainNumSq<(double)100.0) {
						pCan->fGrainNumSq	=(double)1.0;
						pCan->fEarNum		=(double)0.0;
					}

					//      pBiom->fGrainWeight=(double)0.0035*pCan->fGrainNum;
					pBiom->fGrainWeight=(double)0.0035*pCan->fGrainNum*(double)10.0*pMa->pSowInfo->fPlantDens; //SG 2005 11 03
				}
			} // end iStage >= 1 && iStageOld <= 5
		} // end WH, BA
	} // end iStageOld != iCurrentStage


	//##############################################################################

	//Initialisierung von Variablen zu Beginn der einzelnen Entwicklungsstadien:

	if(strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
		switch(iCurrentStage) {

		case 1:
			if(self->iStageOld==9) { // Initialisierung zu Beginn von Stadium 1
				pDev->fSumDTT = pDev->fSumDTT - self->fP9;

				// Plant leaf area [m^2/ha]
				pCan->fPlantLA=pCan->fPlantGreenLA	=(double)(1.0*pMa->pSowInfo->fPlantDens);
				pCan->fPlantSenesLA = (double)0.0;
				pCan->fLAI=(double)1E-4*pCan->fPlantLA;

				//leaf number (= LN)
				pCan->fLeafTipNum = (double)1.0;
				pPl->pCanopy->fExpandLeafNum = (double)1.0;
				pPltMP->fMaxLeafNum = (double)30.0;

				pBiom->fLeafWeight		=
				    pBiom->fGreenLeafWeight	= (double)0.2;//*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fSenesLeafWeight = (double)0.0;
				pBiom->fRootWeight		= (double)0.2;//*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStemWeight		= (double)0.2;//*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fGrainWeight		=
				    self->fEarWeight				= (double)0.0;
				pBiom->fSeedReserv		= (double)0.2;//*(double)10.0*pMa->pSowInfo->fPlantDens;

				pBiom->fStovWeight		=(double)0.4;//*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fTotalBiomass	=pBiom->fStovWeight + pBiom->fRootWeight;




				pPltN->fRootActConc	=(double)0.022;
				pPltN->fTopsActConc	=(double)0.044;


				self->fNFAC = (double)1.0;

				pPltN->fRootCont =pPltN->fRootActConc*pBiom->fRootWeight;
				pPltN->fStovCont =pPltN->fTopsActConc*pBiom->fStovWeight;

			}
			break;

		case 2:

			break;

		case 3:
			if(self->iStageOld == 2) {
				//P3 calculated from the total number of leaves that will eventually
				//appear (iTLNO) and the thermal time required for the emergence of
				//each leaf... (Jones & Kiniry 1986, S.74)
				pPltMP->fMaxLeafNum = (double)((int)(pPl->pDevelop->fSumDTT/(double)21.0) + 6.0);
				pStageParam->afThermalTime[2] = (double)((pPltMP->fMaxLeafNum-2.0)*38.9
				                                + 96.0 - pPl->pDevelop->fSumDTT);
				self->fTasselLeafNum = pCan->fExpandLeafNum;

				pDev->fSumDTT	= (double)0.0;
			}

			break;

		case 4:
			if(self->iStageOld==3) {
				self->fEarWeight = (double)0.167*pBiom->fStemWeight;
				pBiom->fStemWeight -= self->fEarWeight;
				pPltMP->fMinStemWeight = (double)0.85 * pBiom->fStemWeight;

				self->fMaxPlantLA = pCan->fPlantLA;

				pDev->fSumDTT	-= pGen->pStageParam->afThermalTime[2];

				self->fSumPS = (double)0.0;
				self->iDurP = 0;
			}

			break;

		case 5:
			if(self->iStageOld==4) {
				pPltMP->fMinLeafWeight = (double)0.85 * pBiom->fLeafWeight;
				pPltMP->fMaxStemWeight = pBiom->fStemWeight;
				pDev->fSumDTT = pDev->fSumDTT - pStageParam->afThermalTime[3];
				self->iSlowFillDay = 0;

				PSKER = self->fSumPS/(double)self->iDurP*(double)(1000.0*3.4/3.5); //[kg/ha/day]
				PSKER *= (double)0.1/pMa->pSowInfo->fPlantDens; //[g/plant]

				//grains per plant (CERES-Maize, pp.72)
				pCan->fGrainNum =(double)max(51.0,pGen->fStorageOrganNumCoeff*(double)((PSKER-195.0)/(1213.2+PSKER-195.0)));
				pCan->fGrainNum = (double)min(pCan->fGrainNum,pGen->fStorageOrganNumCoeff);

				//ears per m^2
				pCan->fEarNumSq = pMa->pSowInfo->fPlantDens;

				if(pCan->fGrainNum < pGen->fStorageOrganNumCoeff*(double)0.15)
					pCan->fEarNumSq = (double)max(0.0,pMa->pSowInfo->fPlantDens *
					                              (double)pow((pCan->fGrainNum-50.0)/(pGen->fStorageOrganNumCoeff-50.0),0.33));

				pCan->fGrainNumSq = pCan->fGrainNum * pCan->fEarNumSq;

				pPl->pPltNitrogen->fVegActConc	=pPl->pPltNitrogen->fTopsActConc;
				pPl->pPltNitrogen->fVegMinConc	=pPl->pPltNitrogen->fTopsMinConc;
			}
			break;

		case 6:
			break;

		case 9:

			if(self->iStageOld == 8) {
				pDev->fSumDTT = pDev->fCumDTT = (double)0.0;
				pPltN->fNStressPhoto = pPltN->fNStressLeaf = (double)1.0;
			}

			break;
		}

	}

	if(strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
		PBIOMASS		pBiom = pPl->pBiomass;
		PDEVELOP		pDev  = pPl->pDevelop;
		PCANOPY			pCan  = pPl->pCanopy;
		PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
		PSTAGEPARAM		pStageParam = pPl->pGenotype->pStageParam;
		double ALF1;

		switch(iCurrentStage) {

		case 1:
			if(self->iStageOld==9) { // Initialisierung zu Beginn von Stadium 1
				//OILCROP-SUN, pp.63

				pDev->fSumDTT = pDev->fSumDTT - self->fP9;

				// Plant leaf area [m^2/ha]
				pCan->fPlantLA=pCan->fPlantGreenLA	=(double)(300.0*pMa->pSowInfo->fPlantDens * 1e-2);
				pCan->fPlantSenesLA = (double)0.0;
				pCan->fLAI=(double)1E-4*pCan->fPlantLA;

				//leaf number (= LN)
				pCan->fLeafTipNum = (double)1.0;
				pPl->pCanopy->fExpandLeafNum = (double)0.0;
				//initial estimate of final leaf number
				//(TLNO = 2+(P1+30+30*P2)/14)
				self->iTLNO = (int)((double)(2+(pStageParam->afThermalTime[0] +30 + 30*pPl->pGenotype->fDaylengthCoeff)/14));

				pBiom->fLeafWeight = pBiom->fGreenLeafWeight	=
				                         (double)0.03*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fRootWeight	=(double)0.02*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStemWeight	=(double)0.01*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStovWeight	=(double)0.04*(double)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fTotalBiomass=pBiom->fStovWeight;

				pBiom->fSenesLeafWeight = (double)0.0;
				pBiom->fGrainWeight	=(double)0;

				pPltN->fRootActConc	=(double)0.0466;
				pPltN->fLeafActConc	=(double)0.062;
				pPltN->fStemActConc	=(double)0.0444;
				pPltN->fTopsActConc	=(double)0.0576;
				self->fNHeadActConc       =(double)0.042;

				self->fNFAC = (double)1.0;

				pPltN->fRootCont =pPltN->fRootActConc*pBiom->fRootWeight;
				pPltN->fLeafCont = self->fNGreenLeafCont =
				                       pPltN->fLeafActConc*pBiom->fLeafWeight;
				pPltN->fStemCont =pPltN->fStemActConc*pBiom->fStemWeight;
				pPltN->fStovCont =pPltN->fTopsActConc*pBiom->fStovWeight;

			}
			break;

		case 2:
			break;

		case 3:
			if(self->iStageOld == 2) {
				self->iTLNO			= (int)((pDev->fCumDTT-self->fP9)/(double)14.0)+2;
				self->fSumPS			= (double)0.0;
				self->iDurP			= 0;
				pDev->fSumDTT	= (double)0.0;
				self->bSenesCode		= FALSE;
				self->iSenesTime		= 0;
				self->fCumSenesLA		= (double)0.0;
			}



			break;

		case 4:

			if(self->iStageOld==3) {

				self->fStemWeightMin = (double)0.8 * pBiom->fStemWeight;
				self->fStemWeightMax = (double)1.8 * pBiom->fStemWeight;
				pDev->fSumDTT = pDev->fSumDTT - pStageParam->afThermalTime[2];


				PSKER = self->fSumPS/(double)self->iDurP; //[kg/ha/day]
				self->fRM = (pBiom->fBiomassAbvGround+pBiom->fRootWeight)*(double)(0.008*0.729);//[kg/ha/day]
				self->fRI1 = (double)1.0+self->fRM/PSKER;


				//calculate number of pericarps per plant
				if(PSKER > (double)(5.0 * pMa->pSowInfo->fPlantDens * 10.0))
					self->ppp = (double)(430.0 + pPl->pGenotype->fStorageOrganNumCoeff*
					                     PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/14.5);
				else
					self->ppp = (double)(PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/5.0*
					                     (430.0 + pPl->pGenotype->fStorageOrganNumCoeff*5.0/14.5));

				if(self->ppp>pPl->pGenotype->fStorageOrganNumCoeff)
					self->ppp = pPl->pGenotype->fStorageOrganNumCoeff;


				//calculate factor for grain Growth
				self->fGrowFactor = (double)min(1.0,0.6+0.4*min(1.0,self->fHeadWeight/self->fPotHeadWeight));

				//initialize pericarp weight [kg/ha]
				self->fPericarpWeight = self->ppp*(double)0.002*pMa->pSowInfo->fPlantDens*(double)10.0;
				self->fHeadWeight = self->fHeadWeight - self->fPericarpWeight;

				//calculate potential pericarp growth rate [kg/ha/day]
				ALF1 = (double)((0.22*pPl->pGenotype->fRelStorageFillRate/24.*(self->fP5-170.)-2.*0.78)/270./0.78);
				self->fPotPericarpGrowR = (double)0.024*ALF1*self->ppp*pMa->pSowInfo->fPlantDens*(double)10.0;

				//Fix minimum N amount in leaves at 0.9% of the current amount at first anthesís
				//fNLeafMinCont = (double)0.009 * pBiom->fLeafWeight;

				//Calculate nitrogen concentration in green leaves
				self->fNGreenLeafActConc = self->fNGreenLeafCont/pBiom->fGreenLeafWeight;
				//determine nitrogen concentration in senesced leaves
				self->fYRAT = (double)((0.009 - 0.0875*self->fNGreenLeafActConc)/0.9125);

				//Determine the (plant leaf area)/(available N in leaves) [m^2/kg] ratio.
				//This variable is used to calculate senescence rate driven by N demand
				self->fSLOPEPE = pCan->fLAI/(self->fNGreenLeafCont-self->fYRAT*pBiom->fGreenLeafWeight)*(double)1e4;
				self->fSenesLAN2 = (double)0.0;
				//Determine the amount of N in leaves [kg(N)/ha] that can be rapidly exported.
				//This is assumed to equal 50% of labile N.
				self->fXPEPE   = (double)0.50 * (self->fNGreenLeafCont - self->fYRAT*pBiom->fGreenLeafWeight);

				//maximal green leaf area
				self->fAPLA = pCan->fLAI * (double)10000.0;


				self->fSumPS = (double)0.0;
				self->iDurP = 0;

			}

			break;

		case 5:
			if(self->iStageOld==4) {
				//Initialize:
				//Grain number per plant
				pCan->fGrainNum = (double)0.0;
				//maximum head weight
				self->fHeadWeightMax = (double)2.2*self->fHeadWeight;
				//minimum head weight
				self->fHeadWeightMin = self->fHeadWeight;
				//individual grain weight
				self->fIGrainWeight = (double)0.0;
			}

			if((pDev->fSumDTT >= (double)230.0)&&(pCan->fGrainNum==(double)0.0)) {
				fPS = self->fRM/(self->fRI1-(double)1.0);
				self->fRM = (pBiom->fBiomassAbvGround+pBiom->fRootWeight)*(double)(0.008*0.729);

				self->fRI1 = (double)1.0+self->fRM/fPS;

				//calculate average rate of dry matter accumulation in
				//the period after anthesis
				PSKER = self->fSumPS/(double)self->iDurP; //[kg/ha]

				//calculate number of filled grains per plant
				if(PSKER > (double)(5.0 * pMa->pSowInfo->fPlantDens * 10.0))
					pCan->fGrainNum = (double)(430.0 + pPl->pGenotype->fStorageOrganNumCoeff*
					                           PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/14.5);
				else
					pCan->fGrainNum = (double)(PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/5.0*
					                           (430.0 + pPl->pGenotype->fStorageOrganNumCoeff*5.0/14.5));

				if(pCan->fGrainNum>pPl->pGenotype->fStorageOrganNumCoeff)
					pCan->fGrainNum = pPl->pGenotype->fStorageOrganNumCoeff;

				if(pCan->fGrainNum > self->ppp)
					pCan->fGrainNum=self->ppp;

			}
			break;

		case 6:
			break;

		case 9:
			break;
		}

	}


	ceres_CERES_TO_COMVR(self);

	return RET_SUCCESS;
}

int ceres_DevelopmentCheckAndPostHarvestManagement_run(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	PMANAGEMENT pMa = xpn->pMa;
	PPLANT pPl = xpn->pPl;
	PSPROFILE pSo = xpn->pSo;
	PTIME pTi = xpn->pTi;

	int day,month,year;

	if NewDayAndPlantGrowing {
	if (pPl->pDevelop->fStageSUCROS>=(double)0.0)
			pPl->pDevelop->iDayAftEmerg++;

		if (pPl->pDevelop->fStageSUCROS>=(double)1.0)
			pPl->pDevelop->bMaturity=pPl->pDevelop->bMaturity;

		if (pPl->pDevelop->fStageSUCROS>=(double)2.0) {
			pPl->pDevelop->bMaturity=TRUE;
			ceres_SetAllPlantRateToZero(self);
		}
	}


	//if (pTi->pSimTime->fTimeAct == (double)(pPl->pModelParam->iHarvestDay-1))
	day=pTi->pSimTime->mday;
	month=pTi->pSimTime->mon;
	year=pTi->pSimTime->year;
	xpn_time_date_add_dt(&year,&month,&day,-1.0);

	if ((pPl->pModelParam->HarvestDay==day) && (pPl->pModelParam->HarvestMonth==month) && (pPl->pModelParam->HarvestYear==year)) {
		double *pMat1Local,factor,amount,actDepth,RootProp,RootSum;
		int    i1,i2;

		double fCut,fHeight;
		double fCResidue,fNResidue;
		double fCStandR,fNStandR;
		double fStaFak;
		PSLAYER         pSL     =pSo->pSLayer->pNext;
		PCLAYER pCL;
		PCPROFILE pCP = pCh->pCProfile;

		pPl->pDevelop->bPlantGrowth = FALSE;
		pPl->pDevelop->bMaturity = TRUE;
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

		/* Initialisieren des Standing / Verteilungsfaktor */
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) {
			//Weizen und alles andere
			fCut = (double)15;
			fHeight = (double)100;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) {
			//Gerste
			fCut = (double)15;
			fHeight = (double)100;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0) {
			//Mais
			fCut = (double)20;
			fHeight = (double)200;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"PT")==0) {
			//Kartoffel
			fCut = (double)0;
			fHeight = (double)60;
		}

		if (strcmp(pPl->pGenotype->acCropCode,"SF")==0) {
			//Sonnenblumen
			fCut = (double)150;
			fHeight = (double)200;
		}


		fStaFak = fCut / fHeight;


		// Falls ohne Standing Pool gerechnet werden soll
		if(!self->iStandingResidues) fStaFak = (double)0;



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

		if      ((strcmp(pPl->pGenotype->acCropName,"Ackersenf")==0)||
		         (strcmp(pPl->pGenotype->acCropName,"Kleegras")==0)||
		         (strcmp(pPl->pGenotype->acCropName,"Luzerne")==0)||
		         (strcmp(pPl->pGenotype->acCropName,"Winterrübsen")==0)||
		         (strcmp(pPl->pGenotype->acCropName,"Ölrettich")==0)) {
			if(pPl->pModelParam->cResidueCarryOff==0) {
				if(self->iStandingResidues) {
					//pCP->fCStandCropRes += pPl->pBiomass->fBiomassAbvGround * (double)0.4;
					//pCP->fNStandCropRes += pPl->pBiomass->fBiomassAbvGround * pPl->pPltNitrogen->fVegActConc;
					pCP->fCStandCropRes += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * (double)0.4;
					pCP->fNStandCropRes += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * pPl->pPltNitrogen->fVegActConc;
				} else {
					//pCP->fCLitterSurf += pPl->pBiomass->fBiomassAbvGround * (double)0.4;
					//pCP->fNLitterSurf += pPl->pBiomass->fBiomassAbvGround * pPl->pPltNitrogen->fVegActConc;
					pCP->fCLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * (double)0.4;
					pCP->fCLeafLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * (double)0.4;//Hong on 20190605: for century_n
					pCP->fNLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * pPl->pPltNitrogen->fVegActConc;
				}
			}
		} else if(pPl->pModelParam->cResidueCarryOff==0) {
			pCP->fCStandCropRes  += fCStandR;
			pCP->fCLitterSurf    += (fCResidue - fCStandR);
			pCP->fCLeafLitterSurf    += (fCResidue - fCStandR);//Hong on 20190605: for century_n
			pCP->fNStandCropRes  += fNStandR;
			pCP->fNLitterSurf    += (fNResidue - fNStandR);
		} else {
			pCP->fCStandCropRes += fCStandR;
			pCP->fNStandCropRes += fNStandR;
		}

		pMa->pLitter->fRootC = pPl->pBiomass->fRootWeight * (double)0.4 ; // 40% C in Biomasse


		if (pPl->pPltNitrogen->fRootCont > (double) 0) {
			pMa->pLitter->fRootCN = pPl->pBiomass->fRootWeight * (double)0.4
			                        /pPl->pPltNitrogen->fRootCont;
		} else {
			pMa->pLitter->fRootCN =(double)0.1;// ep 250399 fRootCN=0.1 overestimates NLitter
		}


		// Berechnen des schichtmaessigen Anteils

		for (pSL = pSo->pSLayer->pNext,
		     pCL = pCh->pCLayer->pNext,i2=0;
		     ((pSL->pNext != NULL)&&
		      (pCL->pNext != NULL));
		     pSL = pSL->pNext,
		     pCL = pCL->pNext,i2++) {
			actDepth = actDepth + pSL->fThickness;
			RootProp =    (double)exp((double)-3.0 * actDepth / pSo->fDepth);
			RootSum += RootProp;

			pMat1Local[i2] = RootProp;
		}

		// Anteilsmaessige Verteilung auf die Bodenschichten.
		for (pSL = pSo->pSLayer->pNext,
		     pCL = pCh->pCLayer->pNext,i2=0;
		     ((pSL->pNext != NULL)&&
		      (pCL->pNext != NULL));
		     pSL = pSL->pNext,
		     pCL = pCL->pNext,i2++) {
			factor = pMat1Local[i2] / RootSum;
			amount = pMa->pLitter->fRootC * factor;

			pCL->fCLitter += amount;
			pCL->fNLitter += amount / pMa->pLitter->fRootCN;

			if((pCL->fCLitter>EPSILON)&&(pCL->fNLitter>EPSILON)) {
				pCL->fLitterCN = pCL->fCLitter / pCL->fNLitter;
			} else
				pCL->fLitterCN =(double)0.1;

		}

		free(pMat1Local);

		if(pPl->pNext==NULL) {
			memset(pPl->pDevelop,0x0,sizeof(STDEVELOP));
			memset(pPl->pBiomass,0x0,sizeof(STBIOMASS));
			memset(pPl->pCanopy,0x0,sizeof(STCANOPY));
			memset(pPl->pPltNitrogen,0x0,sizeof(STPLTNITROGEN));
			memset(pPl->pPltCarbon,0x0,sizeof(STPLTCARBON));
			memset(pPl->pPltWater,0x0,sizeof(STPLTWATER));
		}


	}


	return RET_SUCCESS;
}

void ceres_SetAllPlantRateToZero(ceres *self)
	{
	expertn_modul_base *xpn = &(self->parent);
	PPLANT		pPl = xpn->pPl;
	PSPROFILE	pSo = xpn->pSo;
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
		for (L=1;L<=pSo->iLayers-2;L++)
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
	
