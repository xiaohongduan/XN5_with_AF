#include "ceres_photosynthesis.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int ceres_DailyCanopyGrossPhotosynthesis_run(ceres *self)
{
	 expertn_modul_base *xpn = &(self->parent); 
    PPLANT pPl = xpn->pPl; 
   
	if ((strcmp(pPl->pGenotype->acCropCode,"BA")==0)||(strcmp(pPl->pGenotype->acCropCode,"WH")==0))
                ceres_DailyCanopyPhotosynthesis_WH_BA(self);
        
	if (strcmp(pPl->pGenotype->acCropCode,"MZ")==0)
                ceres_DailyCanopyPhotosynthesis_MZ(self);

	if (strcmp(pPl->pGenotype->acCropCode,"SF")==0)
                ceres_DailyCanopyPhotosynthesis_SF(self);
	return RET_SUCCESS;
}


int ceres_DailyCanopyPhotosynthesis_WH_BA(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent);
    PPLANT pPl = xpn->pPl;     
	double                  PAR,IPAR,TempDay,PRFT,LUEw;
    PCANOPY                     pCan    =pPl->pCanopy;
    PBIOMASS            pBiom   =pPl->pBiomass;
    PPLTWATER           pPltW   =pPl->pPltWater;
    PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;    

        if (pPl->pDevelop->fStageSUCROS<(double)0.0) return 0;

        //==========================================================================================
        //      Photosynthetic Active Radiation & Potential Biomass Growth Rate(fPotBiomGrowRate)
        //==========================================================================================
        if (strcmp(pPl->pGenotype->acCropCode,"WH")==0) //for Wheat
                {

                //==========================================================================================
                //Light Use Efficiency (LUE) / Radiation Use Efficiency (RUE)
                //==========================================================================================

                LUEw = 6.0; //von CERES //LUEw=9.0;
                
                // LUEw = 6.0; //von CERES: LUEw=9.0;
		        LUEw = 7.5; //von CERES: LUEw=9.0; LUEw=7.5; /Kraichgau 2009: 7.5
	           //	LUEw = 9.0; //von CERES: LUEw=9.0; LUEw=7.5;

		
/*	        //SG 20110810:
		        //CO2-dependency of light use efficency for AgMIP-project
		        fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);
		         LUEw = (12.5 * fCO2)/(280.0+fCO2); //LUEw(280 ppm) = 6.25, LUEw(360 ppm) = 7.03, LUEw(720 ppm) = 9.0*/
               
               //SG 20190213 - CO2 effect on light use efficiency
                LUEw = (12.5 *  xpn->pCl->pWeather->fAtmCO2ppm)/(280.0+ xpn->pCl->pWeather->fAtmCO2ppm); //LUEw(280 ppm) = 6.25, LUEw(360 ppm) = 7.03, LUEw(720 ppm) = 9.0
        
                //Photosynthetic Active Radiation(PAR MJ/m2 day)
                PAR     =0.5*(double)self->weather.fDaylySolRad;
                //Intercepted Photosynthetic Active Radiation IPAR
                IPAR=PAR*(1.0-exp(-0.85*(double)pCan->fLAI));
                //Potential Biomass Growth Rate (g/plant.day)
                self->fPotBiomGrowRate=(double)(LUEw*pow(IPAR,0.6))*(double)10.0;  //Kg/ha
                
/*      
                //INRA-Ceres:
                PAR     =0.48*(double)pWth->fSolRad;
                fPotBiomGrowRate=(double)(3.3*PAR*(1.0-exp(-0.65*(double)pCan->fLAI))*(double)10.0);
*/
                }

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        if (strcmp(pPl->pGenotype->acCropCode,"BA")==0) //for Barley
                {
                PAR     =0.47*(double)self->weather.fDaylySolRad;
                IPAR=PAR*(1.0-exp(-0.85*(double)pCan->fLAI));
//              self->fPotBiomGrowRate=(double)(3.5*IPAR)*(double)10.0;                        //Kg/ha
                //SG 20190225: CO2 effect 
                LUEw = (9.0*xpn->pCl->pWeather->fAtmCO2ppm)/(500.0 + xpn->pCl->pWeather->fAtmCO2ppm); //LUEw(280 ppm) =3.25, LUEw(400 ppm) = 4.0, LUEw(720 ppm) = 5.3
                self->fPotBiomGrowRate=(double)(LUEw*IPAR)*(double)10.0;                        //Kg/ha
                }
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        //==========================================================================================
        //      Actual Biomass Growth Rate based on Photosynthetic Reduction Factors
        //==========================================================================================

    //Adjusts photosynthesis rate for variable CO2
        //PCO2  is the CO2 effect coefficeint.
        //      if (ISWCO2==1)                  //ISWCO2 is a switch for CO2 consideration.
        //              fPotBiomGrowRate=fPotBiomGrowRate*PCO2;

        //Photosynthetic reduction due to high or low temperature
        //========================================================================
        //Day Time Temperature (TempDay)
        TempDay=0.25*(double)self->weather.fTempMin+0.75*(double)self->weather.fTempMax;

        //Temperature reduction factor (PRFT)
        PRFT=1.0-0.0025*(TempDay-18.0)*(TempDay-18.0);

        if (self->weather.fTempMin<-(double)3.0) PRFT=0.0;

        PRFT=max((double)0.0, PRFT);
    //==========================================================================
    //Actual Biomass Growth Rate
    //==========================================================================
        pBiom->fBiomGrowR=self->fPotBiomGrowRate
                                                        *(double)PRFT*min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto);

        pBiom->fBiomGrowR =max(pBiom->fBiomGrowR, (double)0.0001);
	return RET_SUCCESS;
}
int ceres_DailyCanopyPhotosynthesis_MZ(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent); 
    PPLANT pPl = xpn->pPl;     
	double                  PAR,IPAR,TempDay,PRFT,LUEw;
    PCANOPY                     pCan    =pPl->pCanopy;
    PBIOMASS            pBiom   =pPl->pBiomass;
    PPLTWATER           pPltW   =pPl->pPltWater;
    PPLTNITROGEN        pPltN   =pPl->pPltNitrogen;

    double fCO2, fCO2Effect; //SG 20190213
    
        if (pPl->pDevelop->fStageSUCROS<(double)0.0) return 0;

        //==========================================================================================
        //Light Use Efficiency (LUE) / Radiation Use Efficiency (RUE)
        //==========================================================================================

        LUEw=5.0; //g/MJ
        
    //*******************************************************
	//SG 20140909:
	//CO2-dependency of light use efficency for AgMIP-project
	// nach DSSAT Crop Modelling Course Summer 2007
	fCO2=xpn->pCl->pWeather->fAtmCO2ppm;

	if (fCO2 <= 0.0)
	{
		fCO2Effect = 1.0;
	}
	else if(fCO2 < 280)
	{
		fCO2Effect = 0.8 * fCO2 / 280;
	}
	else if (fCO2 <= 330)
	{
		fCO2Effect = (fCO2-280)/(330-280)*0.2+ 0.8;
	}
	else if (fCO2 <= 450)
	{
		fCO2Effect = (fCO2-330)/(450-330)*0.2+ 1.0;
	}
	else
	{
		fCO2Effect = (fCO2-450)/(1000-450)*0.3+ 1.2;;
	}

	if (fCO2Effect > 1.5) 
		fCO2Effect = 1.5;

	LUEw = LUEw * fCO2Effect;

	//*******************************************************


        //==========================================================================================
        //      Photosynthetic Active Radiation & Potential Biomass Growth Rate(fPotBiomGrowRate)
        //==========================================================================================
            //Photosynthetic Active Radiation(PAR MJ/m2 day)
                PAR     =0.5*(double)self->weather.fDaylySolRad;
                //Intercepted Photosynthetic Active Radiation IPAR
                IPAR=PAR*(1.0-exp(-0.65*(double)pCan->fLAI));
                //Potential Biomass Growth Rate (g/plant.day)
                self->fPotBiomGrowRate=(double)(LUEw*IPAR)*(double)10.0;  //Kg/ha

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


        //Photosnythetic reduction due to high or low temperature
        //========================================================================
        //Day Time Temperature (TempDay)
        TempDay=0.25*(double)self->weather.fTempMin+0.75*(double)self->weather.fTempMax;

        //Temperature reduction factor (PRFT)
        PRFT=1.0-0.0025*(TempDay-26.0)*(TempDay-26.0);

        PRFT=max((double)0.0, PRFT);

    //==========================================================================
    //Actual Biomass Growth Rate
    //==========================================================================
        pBiom->fBiomGrowR=self->fPotBiomGrowRate
                                                        *(double)(min(PRFT,min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto)));

        pBiom->fBiomGrowR =max(pBiom->fBiomGrowR, (double)0.0001);
	return RET_SUCCESS;
}
int ceres_DailyCanopyPhotosynthesis_SF(ceres *self)
{
	expertn_modul_base *xpn = &(self->parent); 
    PPLANT pPl = xpn->pPl;    
    double  PAR,IPAR,TempDay,PRFT;
    double  fQN, fQD, fK1, fK2, fC1, fC2;
    double fCO2Effect; //SG 20190213
    PCANOPY             pCan    =pPl->pCanopy;
    PBIOMASS           pBiom   =pPl->pBiomass;
    PPLTWATER         pPltW   =pPl->pPltWater;
    PPLTNITROGEN  pPltN   =pPl->pPltNitrogen;

        if (pPl->pDevelop->fStageSUCROS<(double)0.0) return RET_SUCCESS;

        //==========================================================================================
        //      Photosynthetic Active Radiation & Potential Biomass Growth Rate(fPotBiomGrowRate)
        //==========================================================================================

        //Photosynthetic Active Radiation(PAR MJ/m2 day)
        PAR     =0.5*(double)self->weather.fDaylySolRad;

        //Extinction coefficient for PAR (fK2): 
        fQN = (double)(1.0-exp(-0.86*pCan->fLAI));
        fQD = (double)(2*fQN/(1.0+fQN));
        if(pCan->fLAI>(double)0.001)
                fK2 = (double)(-log(1.0-fQD)/pCan->fLAI);
        else
                fK2 = (double)3.0;

        fK2 = (double)min(3.0,fK2);
        if(pPl->pDevelop->iStageCERES>4) fK2=(double)1.0;

        //Intercepted Photosynthetic Active Radiation IPAR
        IPAR=PAR*(1.0-exp((double)(-fK2*pCan->fLAI)));


        //Calculate RUE and potential biomass assimilation rate:
        //fK1 = radiation use efficiency [g(biomass)/MJ(PAR)]
        fK1 = (double)(1.4+1.8*(1.0-exp(-0.5*pCan->fLAI)));
        fK1 = (double)(min(3.0,fK1));

        if(pPl->pDevelop->iStageCERES>=4)
        {
                fC1 = (double)0.8;
                if(pCan->fGrainNum==(double)0.0)
                        fC2 = (double)max(0.6,0.8-pPl->pDevelop->fSumDTT/140.0 * 0.2);
                else
                        fC2 =(double)max(0.4,0.6-(pPl->pDevelop->fSumDTT-230)/140.0 * 0.2);
        }

        if(pPl->pDevelop->iStageCERES<4)
                self->fPotBiomGrowRate=(double)(fK1*IPAR)*(double)10.0;  //kg/ha
        else
                self->fPotBiomGrowRate = (double)max(0.0, IPAR*self->fRI1*fC2/fC1*fK1*10.0 - self->fRM);

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        //==========================================================================================
        //      Actual Biomass Growth Rate based on Photosynthetic Reduction Factors
        //==========================================================================================

        //Photosynthetic reduction due to high or low temperature
        //========================================================================
        //Day Time Temperature (TempDay)
        TempDay=0.25*(double)self->weather.fTempMin+0.75*(double)self->weather.fTempMax;


        if((TempDay<(double)4.0)||(TempDay>(double)45.0))
                PRFT = (double)0.0;
        else if(TempDay<(double)17.0)
                PRFT = (TempDay-(double)4.0)/(double)13.0;
        else if(TempDay <= (double)31.0)
                PRFT = (double)1.0;
        else
                PRFT = ((double)45.0-TempDay)/(double)14.0;

        PRFT=max((double)0.0, PRFT);
        
        //SG 20190213
        fCO2Effect = 2.75*xpn->pCl->pWeather->fAtmCO2ppm/(580 + xpn->pCl->pWeather->fAtmCO2ppm);

    //==========================================================================
    //Actual Biomass Growth Rate
    //==========================================================================
    /*    pBiom->fBiomGrowR=self->fPotBiomGrowRate
                                                        *(double)min(min(PRFT,pPltW->fStressFacPhoto),pPltN->fNStressPhoto);*/

     //SG 20190213 - CO2 effect included
     pBiom->fBiomGrowR=self->fPotBiomGrowRate*fCO2Effect
                                                        *(double)min(min(PRFT,pPltW->fStressFacPhoto),pPltN->fNStressPhoto);

        pBiom->fBiomGrowR =max(pBiom->fBiomGrowR, (double)0.0001);

        
        //calculate a factor based on the red/far red - ratio to reduce 
        //potential leaf area growth:
        self->fRFR = (double)(exp(-0.5*fK2*pCan->fLAI));
        if(self->fRFR>(double)0.5)
                self->fRFR = (double)1.0;
        else
                self->fRFR = (double)2.0*self->fRFR;


        //calculate the number of leaves that have appeared (LN):
	
	return RET_SUCCESS;
}
