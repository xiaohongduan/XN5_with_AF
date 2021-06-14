//      spass_phenology.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//      


#include "spass_phenology.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int spass_phenological_development(spass *self)
  {
		expertn_modul_base *xpn = &(self->parent);
	  
	    PCLIMATE		pCl = xpn->pCl; 
		PLOCATION		pLo = xpn->pLo; 
		PMANAGEMENT		pMa = xpn->pMa; 
		PPLANT			pPl = xpn->pPl; 
		PTIME			pTi = xpn->pTi;

		double 			fTemp,fMinTemp,fOptTemp,fMaxTemp; 
		double			fBasVernDays;
		double			fDaylength,fOptDaylength,fSensitivity;
		PRESPONSE 		pResp;
		PWEATHER		pWth=pCl->pWeather;
		PDEVELOP		pDev=pPl->pDevelop;

		int iGerm;
		int day, month, year;

		double fSeedDepth,fSprGrwRate; //für PLAGEN-Variante before emergence!
        
		/* When the flag "harvest_at_maturity is set to 1, harvest takes place at the day
		 * when maturity is reached (fStageSUCROS >= 2.0).
		 * In order to achieve this, new harvest dates, as late as possible have to be set:
		 * in this case either two days before the next plant is sown or two days before
		 * the simulation ends. This happens in the following if-block.
		 * The variable self->harvest_at_maturity is set to 2 at the end of this block in
		 * order to assign this "ultimate" harvest date just once and not every day.*/
		if (NewDay(pTi))
        {
        if ((self->harvest_at_maturity == 1) && (PLANT_IS_GROWING) )
		{
			if(pMa->pSowInfo->pNext!=NULL)
			{
			day = pMa->pSowInfo->pNext->Day;
			month = pMa->pSowInfo->pNext->Month;
			year = pMa->pSowInfo->pNext->Year;
			xpn_time_date_add_dt(&year, &month, &day,  -2);
			pPl->pModelParam->HarvestDay= day;
			pPl->pModelParam->HarvestMonth= month;
			pPl->pModelParam->HarvestYear= year;
			}
			else
			{
			STRUCT_XPN_TIME str_time;
			xpn_time_get_struc(&str_time, pTi->pSimTime->iStop_year, pTi->pSimTime->fStop_TimeY);
			day =  str_time.mday;
			month =  str_time.mon;
			year = str_time.year;
			xpn_time_date_add_dt( &year, &month, &day,  -2);
			pPl->pModelParam->HarvestDay= day;
			pPl->pModelParam->HarvestMonth= month;
			pPl->pModelParam->HarvestYear= year;	
			}
			self->harvest_at_maturity = 2;
		}
		/* Here it is checked whether maturity is reached and if yes, the "ultimate" harvest
		 * date is overwritten.*/
		if ((self->harvest_at_maturity == 2) && (pPl->pDevelop->fStageSUCROS>=(double)2.0) && (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)<0))
			{
			//printf("before %d.%d.%d. Maturity %d\n", pPl->pModelParam->HarvestDay, pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestYear, pPl->pDevelop->bMaturity);
			//printf("sim time %d.%d.%d. \n", pTi->pSimTime->mday, pTi->pSimTime->mon, pTi->pSimTime->iyear);
			pPl->pModelParam->HarvestDay=pTi->pSimTime->mday;
			pPl->pModelParam->HarvestMonth=pTi->pSimTime->mon;
			pPl->pModelParam->HarvestYear=pTi->pSimTime->iyear;
			//printf("after %d.%d.%d. \n", pPl->pModelParam->HarvestDay, pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestYear);
			}
		/* At the harvest day (maturity or ultimate), the variable self->harvest_at_maturity
		 * is set to 1 to allow the assignment of a new "ultimate" harvest date during the
		 * next growth period.*/
		if ((self->harvest_at_maturity == 2) && (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)==0))
			{				
			self->harvest_at_maturity = 1;
			}
		}
		spass_agmip_calc_an_vars(self);
        spass_agmip_calc_season_vars(self); 


		if (PLANT_IS_GROWING)
		{

		//if (SimStart(pTi)) iGerm=(int)0;
		//SG/04/11/99:
		//In SPASS bisher keine Abfrage, ob Keimung stattfindet.
		//Da dies in der Dokumentation aber behauptet wird, wird nun an dieser 
		//Stelle die Germination-Funktion aus CERES aufgerufen, die mit der 
		//Dokumentation übereinstimmt.
		
		if(pDev->fStageSUCROS<(double)-0.5)
		{
			iGerm=spass_Germination(self);
			if(iGerm <= 0)
			{
				if(NewDay(pTi))
					pPl->pDevelop->iDayAftSow++;	
				return	RET_SUCCESS;
			}

			if(iGerm == 1)
				pDev->fStageSUCROS = (double)-0.5;
		}

		fTemp = pWth->fTempAir;
		//fTempMax = pWth->fTempMax;	   
		//fTempMin = pWth->fTempMin;	


				//=====================================================================================================
		//Thermal Effect of the Temperature
		//The cardinal temperatures of development rate
		if (pPl->pDevelop->fStageSUCROS<(double)1.0)
			{
		    fMinTemp= pPl->pGenotype->fTempMinDevVeg;
		    fOptTemp= pPl->pGenotype->fTempOptDevVeg;
		    fMaxTemp= pPl->pGenotype->fTempMaxDevVeg;
			pResp   = pPl->pGenotype->DevRateVegTemp;
		    }
		else
			{
		    fMinTemp= pPl->pGenotype->fTempMinDevRep;
		    fOptTemp= pPl->pGenotype->fTempOptDevRep;
		    fMaxTemp= pPl->pGenotype->fTempMaxDevRep;
			pResp   = pPl->pGenotype->DevRateRepTemp;
		    }
		
		pPl->pDevelop->fThermEffect = spass_ThermalDevelopment(fTemp,pResp,fMinTemp,fOptTemp,fMaxTemp, self);
//=====================================================================================================
		//Vernalization Process
	    fMinTemp= pPl->pGenotype->fTempMinVern;	
	    fOptTemp= pPl->pGenotype->fTempOptVern;
	    fMaxTemp= pPl->pGenotype->fTempMaxVern; 
	    pResp   = pPl->pGenotype->VernRateTemp;

		/* In Plagen für Weizen pPl->pGenotype->fOptVernDays = 45 */

		fBasVernDays		= (double)0.173913*pPl->pGenotype->fOptVernDays;   
		pDev->fVernUnit		= spass_DailyVernalization(fTemp,pResp,fMinTemp,fOptTemp,fMaxTemp, self);
		pDev->fCumVernUnit += pDev->fVernUnit*pTi->pTimeStep->fAct; 
		
		if (pDev->fStageWang<=2.0)	//Before floral initiation
			pDev->fVernEff 	= spass_VernalizationEffect(pDev->fCumVernUnit, fBasVernDays, pPl->pGenotype->fOptVernDays);
		
		//=====================================================================================================
		//Photoperiodism
	    fOptDaylength	= pPl->pGenotype->fOptDaylen;	
	    fSensitivity	= pPl->pGenotype->fDaylenSensitivity;
	    pResp   		= pPl->pGenotype->DevRateVegPhtP; 
		                                                          
		fDaylength = spass_DaylengthAndPhotoperiod(pLo->pFarm->fLatitude, (int)pTi->pSimTime->fTimeY,2);

		if (pDev->fStageSUCROS<(double)1.0)	//Before flowering
			pDev->fDaylengthEff = spass_Photoperiodism(fDaylength,pResp, fOptDaylength,fSensitivity,pPl->pGenotype->acCropCode);
		else
			pDev->fDaylengthEff = (double)1.0;
		
		//=====================================================================================================
		//Development stage
		if (pDev->fStageSUCROS<(double)1.0)
			pDev->fDevR = pPl->pDevelop->fThermEffect*pDev->fVernEff*pDev->fDaylengthEff/pPl->pGenotype->fDevelopConstant1;
		else if (pDev->fStageSUCROS<(double)2.0)
			pDev->fDevR = pPl->pDevelop->fThermEffect/pPl->pGenotype->fDevelopConstant2;	
		
		
		//=====================================================================================================
		//Before Emergence:

		/*SG/30/03/99////////////////////////////////////////////////////////////////////////////
		//
		//Phänologische Entwicklung before Emergence wie in PLAGEN:								*/

		if (pDev->fStageSUCROS<(double)0.0)
			{

			/* Saat-Tiefe und maximales Längenwachstum:
				fSeedDepth [cm](=pMa->pSowInfo->fSowDepth)
				fSprGrwRate [cm/d]
			*/
			//sinnvoller wäre Eingabe über PLANT.gtp
	
				
	/*  // Berechnung der Dauer von "germination" bis "emergence" aus Saattiefe	
		// und Temperatur 
			//fSeedDepth = (double)3.0;
			fSeedDepth = pMa->pSowInfo->fSowDepth;
			fSprGrwRate= (double)1.5;

        //SG/04/11/99: wegen Germination aus CERES:
		if ((!strcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
				||(!strcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
			fSprGrwRate= (double)1.5;
		if (!strcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S"))	
			fSprGrwRate= (double)1.5;


*/
		//	altes SPASS:
		//  pDev->fDevR = ((double)0.5*(fTempMax+fTempMin)-(double)0.0)/(double)100.0;
			
		//  nach Diss E.Wang und Wang&Engel (1998):
		//fSumDTT needed for Emergence = (double)125.0; 
		pDev->fDevR = (double)0.5*(double)max(0.0,(fTemp-(double)0.0)/(double)125.0);

		//fSumDTT needed for Emergence for different Crops aus Saattiefe nach CERES:
		if(pMa->pSowInfo->fSowDepth>(double)0.0)
			fSeedDepth = pMa->pSowInfo->fSowDepth;
		else
		{
			fSeedDepth = (double)3.0;
			if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
				fSeedDepth = (double)6.0;
		}
		
		if (strcmp(pPl->pGenotype->acCropCode,"WH")==0)
			//pDev->fDevR = (double)max(0.0,(double)0.5*(fTemp-(double)2.0)/((double)40.0+((double)10.2)*fSeedDepth));
			//wird altes XPN besser getroffen
			//pDev->fDevR = (double)max(0.0,(double)0.5*(fTemp-(double)2.0)/((double)50.0+((double)10.4)*fSeedDepth));
			pDev->fDevR = (double)max(0.0,(double)0.5*(fTemp-(double)2.0)/((double)60.0+((double)10.2)*fSeedDepth));
		if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)||
			(strcmp(pPl->pGenotype->acCropCode,"S")==0))
			pDev->fDevR = (double)max(0.0,(double)0.5*(fTemp-(double)2.0)/((double)50.0+((double)10.4)*fSeedDepth));

		//SG/19/05/99: Potato
		if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)	
			{
                fSprGrwRate= (double)0.4;//Agria=0.4; Christa=0.45
                if (strcmp(pPl->pGenotype->acVarietyName,"Agria")==0)	
                    fSprGrwRate= (double)0.4;
                if (strcmp(pPl->pGenotype->acVarietyName,"Christa")==0)	
                    fSprGrwRate= (double)0.45;
                if (strcmp(pPl->pGenotype->acVarietyName,"Krone")==0)	
                    fSprGrwRate= (double)0.2;
                if (strcmp(pPl->pGenotype->acVarietyName,"Nicola")==0)	
                    fSprGrwRate= (double)0.45;
                if (strcmp(pPl->pGenotype->acVarietyName,"Cilena")==0)	
                    fSprGrwRate= (double)0.45;
                if (strcmp(pPl->pGenotype->acVarietyName,"Burana")==0)	
                    fSprGrwRate= (double)0.45;

                pDev->fDevR = fSprGrwRate*pPl->pDevelop->fThermEffect/fSeedDepth;
			}

		//SG/24/05/05: Maize (outcommented by Ch. Klein/F. Heinlein?)  --> activated again SG 20190531
		if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
			pDev->fDevR =max(0.0,0.5*(fTemp-10.0)/(15.0+6.0*fSeedDepth));
			//pDev->fDevR = (double)max(0.0,(double)0.5*((double)0.5*(fTempMax+fTempMin)-(double)10.0)/((double)15.0+((double)6.0)*fSeedDepth));

		if (pDev->fStageSUCROS+pDev->fDevR*pTi->pTimeStep->fAct>(double)0.0)
					pDev->fDevR = -pDev->fStageSUCROS/pTi->pTimeStep->fAct;

		if (strcmp(pPl->pGenotype->acCropCode,"PT")==0)
			pDev->fDevStage = (pDev->fStageSUCROS+pDev->fDevR*pTi->pTimeStep->fAct + (double)1.0) * (double)10.0;

		}
		
        /*SG/////////////////////////////////////////////////////////////////////////////////////
        /                                                                                       /
        / iDayAftEmerg wird auch in "DevelopmentCheckAndPostHarvestManagement" in PLANT.C bzw.  /
        / PLANT_SG.C hochgezählt! (und somit doppelt!!)                                         /
        /                                                                                       /		
        / else                                                                                  /
        /     pPl->pDevelop->iDayAftEmerg++;                                                    /
        /                                                                                       /
        /                                                                                       /
        / Da aber iDayAftSow nie hochgezählt wird, nun jetzt hier:                              /
        ///////////////////////////////////////////////////////////////////////////////////////*/
        
		
        //SG/19/05/99: after emergence

//        if (pDev->fStageSUCROS>=(double)0.0) //SG 20190703: out-commented to count for BBCH stages between 0 and 10
//        {

        //SG/17/05/99:////////////////////////////////////////////
        //
        //  Unterscheidung zwischen verschiedenen Pflanzenarten.
        //  DEFAULT und POTATO aus PLAGEN!
        //////////////////////////////////////////////////////////


		if((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
			||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)
			||(strcmp(pPl->pGenotype->acCropCode,"S")==0))
		spass_COMVR_TO_ZSTAGE_AND_CERES_WHEAT(self);

		else if(strcmp(pPl->pGenotype->acCropCode,"PT")==0)
			spass_COMVR_TO_ZSTAGE_AND_CERES_POTATO(self);
		else
		spass_COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(self);

//		} //SG 20190703: out-commented to count for BBCH stages between 0 and 10

        //SG/09/06/99:
        // Änderung des internen Entwicklungsstadiums hinter die 
        // Umwandlung in EC-Stadium verlegt
	
		pDev->fStageSUCROS	+= pDev->fDevR*pTi->pTimeStep->fAct;
        
 /*   // SG 20130516-----------------------------------------------------------------
	// To fix the date of Emergence following SUCROS DEVELOPMENT MODEL
	// provide "Date of Emergence" in database or xnd 
    // ----------------------------------------------------------------------------
    if(pPl->pModelParam->EmergenceDay> (double)0) //Wenn "Date of Emergence" in Datenbank oder xnd angegeben, dann erfolgt Feldaufgang an diesem Tag (=SUCROS)
	{
   	    if(pTi->pSimTime->fTimeAct < (double)(pPl->pModelParam->EmergenceDay))
			pDev->fStageSUCROS = -(double)0.3;

	    if(pTi->pSimTime->fTimeAct >= (double)(pPl->pModelParam->EmergenceDay)&&(pDev->fStageSUCROS < (double)0.0))
			pDev->fStageSUCROS = (double)0.0;
        }*/

	//return 1;
	
  	  if (pDev->fStageSUCROS < 0)
	  {
 
    day=pTi->pSimTime->mday;
    month=pTi->pSimTime->mon;
    year=pTi->pSimTime->year;
    xpn_time_date_add_dt(&year,&month,&day,1.0);
    if (xpn_time_compare_date(year,month,day,pPl->pModelParam->EmergenceYear, pPl->pModelParam->EmergenceMonth,pPl->pModelParam->EmergenceDay)>=0) 	  				  				  				  
     {
        //initially zero
        pDev->fStageSUCROS= (double)0.0;
        pDev->iDayAftEmerg = 0;
/*        pDev->fCumTDU = (double)0.0;
	   
        CTDU =(double)0;
        DS =(double)0;

        iEmerg = 1;*/
    }
      }


		//printf("%f %f %s %s \n", pDev->fStageSUCROS, pDev->fDevR,(bPlantGrowth)?"true":"false",(bMaturity)?"true":"false");

		//if(self->fDayTest>=(double)pPl->pDevelop->iDayAftSow-pTi->pTimeStep->fAct) 
		if (NewDay(pTi))
			pPl->pDevelop->iDayAftSow++;
	
		
  } //end PLANT_IS_GROWING

	expertn_modul_base_DevelopmentCheckAndPostHarvestManagement(xpn);			

	return RET_SUCCESS;
  }


int spass_Germination(spass *self)
    {

		expertn_modul_base *xpn = &(self->parent);

		PSPROFILE		pSo = xpn->pSo; 
		PWATER			pWa = xpn->pWa;
		PPLANT			pPl = xpn->pPl;
		PMANAGEMENT		pMa = xpn->pMa; 

		//===========================================
		//Variable Declaration
		//===========================================
		double			fCumDepth,fSWSD,fSWSD1,fSWSD2;
	    int				L,NLAYER,iGermination;
		//static int		iDayAftSow;

	    PSLAYER			pSL  = pSo->pSLayer;
		PSWATER			pSWL = pSo->pSWater;
		PWLAYER			pSLW = pWa->pWLayer; 

		if(pPl->pDevelop->fStageXCERES==(double)0.0)
		{
			 pPl->pDevelop->iDayAftSow = 0;
		}

		NLAYER=pSo->iLayers-2;
		//==============================================
		//Get the Soil Water Pointer in Seed Depth
		//==============================================
		fCumDepth=(double)0.0;
		pSL  = pSo->pSLayer->pNext;
		pSWL = pSo->pSWater->pNext;
		pSLW = pWa->pWLayer->pNext; 
  
		for (L=1;L<=NLAYER;L++)
			{
			fCumDepth += pSL->fThickness;
			if (fCumDepth>=pMa->pSowInfo->fSowDepth)break;
			pSL  = pSL->pNext;;
			pSWL = pSWL->pNext;
			pSLW = pSLW->pNext;
			}

		//============================================================
		//Calculating iGermination Possibility Based on Soil Moisture
		//============================================================
		if (pSLW->fContAct>pSWL->fContPWP)
		//Soil water is sufficient to support Germination
			{
			iGermination=1;  //Germination occurs on this day.
		//	iDayAftSow=1;
			}
		else
		//If pSLW->fContAct<=pSWL->fContPWP,
		// soil water may be a problem
			{
			fSWSD1 = (pSLW->fContAct-pSWL->fContPWP)*((double)0.65);
			pSLW   = pSLW->pNext;
			pSWL   = pSWL->pNext;
			fSWSD2 = (pSLW->fContAct-pSWL->fContPWP)*((double)0.35);
			fSWSD  = fSWSD1+fSWSD2;

			if (fSWSD>=(double)0.02)  //Soil water is sufficient to support germination
				iGermination=1;    //Germination occurs on this day.
			else                    //Soil water is not sufficient.
				{
				iGermination=0;    //Germination still not occur.

				//iDayAftSow++;

				if (pPl->pDevelop->iDayAftSow>=40)
				{
				//if ((iDayAftSow<90) && ( (CROP=='WH')||(CROP=='BA') ) )
      
				if	((pPl->pDevelop->iDayAftSow<90) &&
					((strcmp(pPl->pGenotype->acCropCode,"WH")==0)
					||(strcmp(pPl->pGenotype->acCropCode,"BA")==0)))
					iGermination=0;   //Germination still not occurs
				else
					iGermination=-1;  //Germination fails
			}  }  }

		//=========================================================================================
		//Calculating iGermination Possibility Based on Soil Temperature
		//=========================================================================================
		//  pCl->pWeather->fTempAve=(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)/((double)2.0);
		//
		//  if (pCl->pWeather->fTempAve<=(double)0.0)
		//    iGermination=0;

		return iGermination;
  }
 



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds ThermalDevelopment(double fTempMin,double fTempMax,PRESPONSE pResp,
//											   double fMinTemp,double fOptTemp,double  fMaxTemp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses daily maximum and minimum temperature to calculate the thermal effect on 
//			development rate. The calculated effect is a daily average value and ranges from 0.0 to 1.0.  
//			The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp 
//			contains zero data, then this function calculates the effect using the cardinal values
//			(fMinTemp,fOptTemp,fMaxTemp) and a predefined function RelativeTemperatureResponse.
//Inputs:	1. fTempMin - Daily minimum temperature of the air (C) 	
//			2. fTempMax	- Daily maxmimum temperature of the air (C)
//			3. pResp	- Pointer to a RESPONSE structure containing temperature response data for development
//			4. fMinTemp	- Minimum temperature for development (C)	
//			5. fOptTemp	- Optimum temperature for development (C)	
//			6. fMaxTemp	- Maximum temperature for development (C)	
//Outputs:	1. Return 	- Thermal effect on development rate (0-1)
//Functions Called: 
//			HourlyTemperature
//			RelativeTemperatureResponse 
//			AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double spass_ThermalDevelopment(double fTemp,PRESPONSE pResp,
								   double fMinTemp,double fOptTemp,double  fMaxTemp, spass *self)
	{


    double 			fTermEffect;

	//Calculating the thermal effect of every timestep
	fTermEffect=(double)0.0;
	//for (iLoop=1; iLoop<=8; iLoop++)
	//	{   

	if ((pResp==NULL)||(pResp[0].fInput==(double)-99.9))
		fTermEffect = spass_RelativeTemperatureResponse(fTemp,fMinTemp,fOptTemp,fMaxTemp);
	else
		fTermEffect = spass_AFGENERATOR(fTemp, pResp);

	return fTermEffect;
	}




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds DailyVernalization(double fTempMin, double fTempMax, PRESPONSE pResp,
//		  							  	       double fMinTemp, double fOptTemp, double  fMaxTemp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses daily maximum and minimum temperature to calculate daily vernalization unit. 
//			The unit ranges from 0.0 (min.or max. temperature) to 1.0 (optimal temperature). 
//			The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp 
//			contains zero data, then this function calculates the effect using the cardinal values
//			(fMinTemp,fOptTemp,fMaxTemp) and a predefined function RelativeTemperatureResponse.
//Inputs:	1. fTempMin - Daily minimum temperature of the air (C) 	
//			2. fTempMax	- Daily maxmimum temperature of the air (C)
///			3. pResp	- Pointer to a RESPONSE structure containing temperature response data for vernalization
//			4. fMinTemp	- Minimum temperature for vernalization (C)	
//			5. fOptTemp	- Optimum temperature for vernalization (C)	
//			6. fMaxTemp	- Maximum temperature for vernalization (C)	
//Outputs:	1. Return 	- Daily vernalization unit (PVDs)
//Functions Called: 
//			HourlyTemperature
//			RelativeTemperatureResponse
//			AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_DailyVernalization(double fTemp, PRESPONSE pResp,
							  	   double fMinTemp, double fOptTemp, double  fMaxTemp, spass *self)
	{

    double 			fVernUnit;

	//Daily vernalization unit
	fVernUnit=(double)0.0;
	//for (iLoop=1; iLoop<=8; iLoop++)
	//	{       
		//3 Hour temperature
		//fTemp = HourlyTemperature(iLoop,fTempMin,fTempMax); 
		
		//Vernalization unit
		if ((pResp==NULL)||(pResp[0].fInput==(double)-99.9))
			fVernUnit = spass_RelativeTemperatureResponse(fTemp,fMinTemp,fOptTemp,fMaxTemp);
		else
			fVernUnit = spass_AFGENERATOR(fTemp, pResp);

	return fVernUnit;
	} 


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds VernalizationEffect(double fBasVern, double fCrtVern, double fCumVern)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function calculates the current vernalization effect on development rate (0-1)
//Inputs:	1. fCumVern - Cumulative vernalization days till the current time (PVDs)
//			2. fBasVern	- Minimum vernalization days after that development progresses (PVDs)
//			3. fCrtVern	- Critical vernalization days with which vernalization fully fulfilled (PVDs)	
//Outputs:	1. Return 	- Vernalization effect on development rate (0-1)
//Functions Called: 
//			None
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_VernalizationEffect(double fCumVern, double fBasVern, double fCrtVern)
	{
    double fVernEffect;

	if (fCrtVern==(double)0.0)
		fVernEffect = (double)1.0;
	else
		fVernEffect = (fCumVern-fBasVern)/(fCrtVern-fBasVern);
	
	fVernEffect =min((double)1.0,max((double)0.0,fVernEffect));
	
	return fVernEffect;
	}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: double WINAPI _loadds Photoperiodism(double fDaylen,PRESPONSE pResp, double fOptDaylen,double fSensF,LPSTR CropCode)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses current daylength to calculate the effect of photoperiod on development rate
//			The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp 
//			contains zero data, this function calculates the effect using the optimal daylength (fOptDaylen)
//			and a daylength sensitivity factor (fSensF) with an exponental equation
//Inputs:	1. fDaylen		- Current daylength (hours)
///			2. pResp		- Pointer to a RESPONSE structure containing photoperiod response data 
//			3. fOptDaylen	- Optimal photoperiod (hours)
//			4. fSensF		- photoperiod sensitivity factor (-)	
//Outputs:	1. Return 		- Effect of photoperiod on development (0-1)
//Functions Called: 
//			AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_Photoperiodism(double fDaylen,PRESPONSE pResp, double fOptDaylen,double fSensF, char* acCropCode)
	{
    double 	fDaylenEffect;
    double 	vDaylen, vMinDaylen,vOptDaylen, vSens;
    
    vDaylen    = (double)fDaylen;
	vOptDaylen = (double)fOptDaylen;
	vSens	   = (double)fSensF;  

	//Output results
	fDaylenEffect=(double)1.0;      

	if ((pResp==NULL)||(pResp[0].fInput==(double)-99.9))  
		{
		if (vSens<0.000001)
		  	fDaylenEffect=(double)1.0;
		else
			{			
			vMinDaylen = vOptDaylen-4.0/vSens;
			if(strcmp(acCropCode,"PT")==0)
				vMinDaylen = max(0.0,min(24.0,vOptDaylen+4.0/vSens));
	  		fDaylenEffect=(double)(1.0-exp(-4.0*(vDaylen-vMinDaylen)/(vOptDaylen-vMinDaylen)));
			if(strcmp(acCropCode,"PT")==0)
	  			fDaylenEffect=(double)(1.0-exp(+4.0*(vDaylen-vMinDaylen)/fabs(vOptDaylen-vMinDaylen)));

		}   }
	else
		fDaylenEffect= spass_AFGENERATOR((double)vDaylen, pResp);

  	fDaylenEffect=max((double)0.0,fDaylenEffect);
  	   
	return fDaylenEffect;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  CHANGE VR STAGE TO ZADOCKS STAGE AND CERES STAGES
//------------------------------------------------------------------------------------------------------------
int spass_COMVR_TO_ZSTAGE_AND_CERES_WHEAT(spass *self)
	{            

	expertn_modul_base *xpn = &(self->parent);
	  
	PPLANT		pPl = xpn->pPl;

	PDEVELOP 	pDev=pPl->pDevelop; 
	PCANOPY		pCan=pPl->pCanopy;
	PGENOTYPE	pGe = pPl->pGenotype;
	PTIMESTEP	pTi = xpn->pTi->pTimeStep;

	int			i;
	//double		fMainStemTiller; //SG/19/04/99: wg.anpassen an PLAGEN
	//static double node;
	double VR[11];

	double		VRD[]={0.0,	0.1425,	0.4,	0.5,	0.69,	0.91,	1.00,	1.15,	1.50,	1.95,	2.0};
//	double		DVR[]={0.0,	0.1425,	0.39,	0.656,	0.91,	1.00,	1.15,	1.50,	1.95,	2.0};
//	double		DVR[]={0.0,	0.15,	0.35,	0.69,	0.80,	1.00,	1.20,	1.80,	1.99,	2.0};

    if(pGe->DVR[0].fInput<0.0)
		{
		g_free(pGe->DVR);
		pGe->DVR = (RESPONSE*)g_malloc0_n(11,sizeof(RESPONSE));
		for (i=0;i<=10;i++)  pGe->DVR[i].fInput=VRD[i];
		}

	for (i=0;i<=10;i++)  
	{
		VR[i]=(double)pGe->DVR[i].fInput;
	}

	if (pDev->iStageCERES==0)
		{
		pDev->iStageCERES=8;
		pDev->fStageXCERES=(double)0.0;
		
		/*if (pDev->fStageSUCROS==(double)0.0)		//SG/22/05/99: versteh ich nicht!
			pDev->fStageSUCROS=(double)-1.0;*/		// funktioniert ohne diese Zeilen viel besser!!
		}

   	//--------------------------------------------------------------------
	//Fromm fStageSUCROS to pDev->fStageWang
   	//--------------------------------------------------------------------
    if (pDev->fStageSUCROS<VR[0]) //SG 20190703: 0 < BBCH < 10
		pDev->fStageWang=(double)1.0+pDev->fStageSUCROS;
    if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1])) //  10 <= BBCH <= 20
		pDev->fStageWang=(double)1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0]);
	if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2])) //  20 <= BBCH <=30
		pDev->fStageWang=(double)2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1]);
	if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3])) //  30 <= BBCH <= 32
		pDev->fStageWang=(double)3.0+(double)0.2*(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2]);
	if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4])) //  32 <= BBCH <= 40
		pDev->fStageWang=(double)3.2+(double)0.8*(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3]);
	if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5])) //  40 <= BBCH <= 50
		pDev->fStageWang=(double)4.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4]);
	if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6])) //  50 <= BBCH <= 60
		pDev->fStageWang=(double)5.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5]);
	if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7])) //  60 <= BBCH <= 70
		pDev->fStageWang=(double)6.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6]);
	if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8])) //  70 <= BBCH <= 80
		pDev->fStageWang=(double)7.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7]);
	if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9])) //  80 <= BBCH <= 90
		pDev->fStageWang=(double)8.0+(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8]);
	if ((pDev->fStageSUCROS>VR[9])&&(pDev->fStageSUCROS<=VR[10])) //  90 <= BBCH <= 20
		pDev->fStageWang=(double)9.0+(pDev->fStageSUCROS-VR[9])/(VR[10]-VR[9]);
			
   	//--------------------------------------------------------------------
	//Fromm fStageSUCROS to fXStage
   	//--------------------------------------------------------------------
	//Sowing to germination to emergence	
	if (pDev->fStageSUCROS<VR[0]) 
		pDev->fStageXCERES=pDev->fStageSUCROS+(double)1.0;
    
    //Emergence to terminal spikelet
	if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<VR[2]))
	    pDev->fStageXCERES=(double)1.0+(pDev->fStageSUCROS-VR[0])/(VR[2]-VR[0]);
	    
    //Terminal spikelet to end of leaf growth
	if ((pDev->fStageSUCROS>=VR[2])&&(pDev->fStageSUCROS<VR[3]))
	    pDev->fStageXCERES=(double)2.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2]);
	    
    //End of leaf growth to Heading
	if ((pDev->fStageSUCROS>=VR[3])&&(pDev->fStageSUCROS<VR[4]))
	    pDev->fStageXCERES=(double)3.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3]);
	    
    //Heading to Milk development
	if ((pDev->fStageSUCROS>=VR[4])&&(pDev->fStageSUCROS<VR[6]))
	    pDev->fStageXCERES=(double)4.0+(pDev->fStageSUCROS-VR[4])/(VR[6]-VR[4]);
	    
    //Milk development(begin of grain filling) to End of grain filling (ripening)
	if ((pDev->fStageSUCROS>=VR[6])&&(pDev->fStageSUCROS<VR[8]))
	    pDev->fStageXCERES=(double)5.0+(pDev->fStageSUCROS-VR[6])/(VR[8]-VR[6]);
	    
	if ((pDev->fStageSUCROS>=VR[8])&&(pDev->fStageSUCROS<VR[9]))
	    pDev->fStageXCERES=(double)6.0+(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8]);

   	//--------------------------------------------------------------------
	//Fromm fStageSUCROS,fStageXCERES to iStageCERES
   	//--------------------------------------------------------------------
	if (pDev->fStageSUCROS<(double)-0.5)
		pDev->iStageCERES=8;
	if ((pDev->fStageSUCROS>=(double)-0.5)&&(pDev->fStageSUCROS<VR[0]))
		pDev->iStageCERES=9;
	if (pDev->fStageSUCROS>=VR[0])
		pDev->iStageCERES=(int)pDev->fStageXCERES;
		
   	//--------------------------------------------------------------------
	//Fromm pDev->fStageWang to fZStage
   	//--------------------------------------------------------------------


	//fMainStemTiller= max((double)0.0, pCan->fExpandLeafNum-(double)2.5+(double)1.0);
	if ((pDev->fStageWang>=(double)1.0)&&(pDev->fStageWang<(double)3.0))
		{
		if (pCan->fTillerNum<=(double)1.0)
	//	if (fMainStemTiller<=(double)1.0)
			pDev->fDevStage=(double)1.0+pCan->fExpandLeafNum/(double)10.0;
		else
		{
			//SG/09/06/99.
			//	besser mit pCan->fMainStemTillers, dann EC 21 besser erreicht!
			pDev->fDevStage=(double)2.0+pCan->fTillerNum/(double)10.0;	
			//pDev->fDevStage=(double)2.0+pCan->fMainStemTillers/(double)10.0;
		}
        self->spass_COMVR_TO_ZSTAGE_AND_CERES_WHEATnode = (double)0.0;
        }
        
		
	if ((pDev->fStageWang>=(double)3.0)&&(pDev->fStageWang<(double)4.0))
		{    
		self->spass_COMVR_TO_ZSTAGE_AND_CERES_WHEATnode += (double)0.2844*pPl->pDevelop->fThermEffect*pDev->fDaylengthEff*pTi->fAct;
		pDev->fDevStage=(double)3.0+self->spass_COMVR_TO_ZSTAGE_AND_CERES_WHEATnode/(double)10.0;

		if (pDev->fStageSUCROS>=0.656)
			pDev->fDevStage=(double)3.9;
			
		}
        
	if ((pDev->fStageWang>=(double)4.0)&&(pDev->fStageWang<=(double)9.0))
		pDev->fDevStage=pDev->fStageWang;  
	
	if (pDev->fStageWang>(double)9.0)
		pDev->fDevStage=(double)9.0+(double)0.2*(pDev->fStageWang-(double)9.0);  

	pDev->fDevStage *=(double)10.0;	


	//SG/25/05/99: Wahlweise Berechnung nach PLAGEN:
	//	Wird pPl->pGenotype->fMaxLfAppRate = 0 gesetzt (Zeile 2000040 in Plant.gtp)
	//	erfolgt EC-Berechnung ohne Leaves, Tillers und nodes.
	if(pPl->pGenotype->fMaxLfAppRate==(double)0.0)
	pDev->fDevStage = min((double)92.0,pDev->fStageWang * (double)10.0);
	
	
	
	return RET_SUCCESS;
	}


int spass_COMVR_TO_ZSTAGE_AND_CERES_POTATO(spass *self)
	{                

	expertn_modul_base *xpn = &(self->parent);
	  
	PPLANT			pPl = xpn->pPl;
	PTIMESTEP		pTi = xpn->pTi->pTimeStep;
 

		//Based on Spitters, van Keulen & van Kraalingen, 19xx, SUCROS 87
	//Maturity class (MATR) 2.5 for very late cv's and 9.5 for very early cv's
	double fMaxTuberizRate,fTuberizRate;
	double x;

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		pPl->pDevelop->fStageXCERES = (double)0.0;   

	if (pPl->pDevelop->fStageXCERES<(double)1.0) //Before tuberization
		{
		//fMaxTuberization ranges from 10 to 26 
		fMaxTuberizRate = (double)1.0/((double)0.7*pPl->pGenotype->fDevelopConstant1);//Agria=0.7 Christa=0.5
		if (strcmp(pPl->pGenotype->acVarietyName,"Agria")==0)	
			fMaxTuberizRate = (double)(1.0/(0.5*pPl->pGenotype->fDevelopConstant1));
		if (strcmp(pPl->pGenotype->acVarietyName,"Christa")==0)	
			fMaxTuberizRate = (double)(1.0/(0.4*pPl->pGenotype->fDevelopConstant1));            
        if (strcmp(pPl->pGenotype->acVarietyName,"Krone")==0)	
			fMaxTuberizRate = (double)1.0/((double)0.7*pPl->pGenotype->fDevelopConstant1);
        if (strcmp(pPl->pGenotype->acVarietyName,"Nicola")==0)	
			fMaxTuberizRate = (double)1.0/((double)0.7*pPl->pGenotype->fDevelopConstant1);
        if (strcmp(pPl->pGenotype->acVarietyName,"Cilena")==0)	
			fMaxTuberizRate = (double)1.0/((double)0.7*pPl->pGenotype->fDevelopConstant1);
        if (strcmp(pPl->pGenotype->acVarietyName,"Burana")==0)	
			fMaxTuberizRate = (double)1.0/((double)0.7*pPl->pGenotype->fDevelopConstant1);

		fTuberizRate    = fMaxTuberizRate*pPl->pDevelop->fThermEffect*pPl->pDevelop->fDaylengthEff;
			
		//If fDevStage=1.0, tuberization occurs
		pPl->pDevelop->fStageXCERES += fTuberizRate*pTi->fAct;
		}
	else
		pPl->pDevelop->fStageXCERES += pPl->pDevelop->fThermEffect*pTi->fAct/(double)40.0;

    		
	if (pPl->pDevelop->fStageSUCROS<(double)1.0)
		pPl->pDevelop->fDevR=pPl->pDevelop->fThermEffect/pPl->pGenotype->fDevelopConstant1; 
	else
		pPl->pDevelop->fDevR=pPl->pDevelop->fThermEffect/pPl->pGenotype->fDevelopConstant2; 

//	pPl->pDevelop->fDevStage = (double)5*(pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR)*(double)10;

	x=pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR*pTi->fAct;


	if (x<(double)1.0)
		pPl->pDevelop->fDevStage = (double)(5.*x + 1.)*(double)10;
	else
		pPl->pDevelop->fDevStage = (double)min(100.,(2+4*x)*10);
		
	

	return RET_SUCCESS;
	}


/*int spass_COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(spass *self)
	{      

	expertn_modul_base *xpn = &(self->parent);
	  
	PPLANT			pPl = xpn->pPl;
	PTIME			pTi = xpn->pTi;

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		pPl->pDevelop->fStageWang = pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR*pTi->pTimeStep->fAct;   
	else
		pPl->pDevelop->fStageWang = (double)4.6*(pPl->pDevelop->fStageSUCROS)+pPl->pDevelop->fDevR*pTi->pTimeStep->fAct;
		//pPl->pDevelop->fStageWang = (double)5*(pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR);
		//pPl->pDevelop->fStageWang = pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR*pTi->pTimeStep->fAct;   

	pPl->pDevelop->fDevStage = (double)10.0*pPl->pDevelop->fStageWang;   
	//pPl->pDevelop->fDevStage = pPl->pDevelop->fStageWang;   

	return RET_SUCCESS;
	}
    
   */ 
    
    int spass_COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(spass *self)
	{      
 //SG 20190702:  COMVR_TO_ZSTAGE_AND_CERES_DEFAULT as in XN3! Advantage: Emergence at BBCH=10
	expertn_modul_base *xpn = &(self->parent);
	  
	PPLANT		pPl = xpn->pPl;
//	PTIME			pTi = xpn->pTi;

	if (pPl->pDevelop->fStageSUCROS<(double)0.0)
		pPl->pDevelop->fStageWang = pPl->pDevelop->fStageSUCROS+1.0;   
    else
	{
        if ((pPl->pDevelop->fStageSUCROS>=(double)0.0)&&(pPl->pDevelop->fStageSUCROS<1.0))
		    pPl->pDevelop->fStageWang = (double)1.0+pPl->pDevelop->fStageSUCROS/0.2;
        else
            pPl->pDevelop->fStageWang = (float)6 + (pPl->pDevelop->fStageSUCROS - (float)1.0)/(float)0.28;     
    }
          
	pPl->pDevelop->fDevStage = (double)10.0*pPl->pDevelop->fStageWang;   
	//pPl->pDevelop->fDevStage = pPl->pDevelop->fStageWang;   

	return RET_SUCCESS;
	}
    
    

/*XN3:
int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(EXP_POINTER)
	{            //SG20181017
    //Sowing to germination to emergence	
	if (pPl->pDevelop->fStageSUCROS<(float)0.0) 
		pPl->pDevelop->fStageWang=pPl->pDevelop->fStageSUCROS+(float)1.0;
 
	if ((pPl->pDevelop->fStageSUCROS>=(float)0.0)&&(pPl->pDevelop->fStageSUCROS<(float)1.0))
		pPl->pDevelop->fStageWang = (float)1.0+pPl->pDevelop->fStageSUCROS/(float)0.2;   
	else
		pPl->pDevelop->fStageWang = (float)6 + (pPl->pDevelop->fStageSUCROS - (float)1.0)/(float)0.28;   

	pPl->pDevelop->fDevStage = (float)10.0*pPl->pDevelop->fStageWang;   
	//pPl->pDevelop->fDevStage = pPl->pDevelop->fStageWang;   

	return 1;
	}*/
	