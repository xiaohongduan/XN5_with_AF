//      spass.c
//
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//

#include "spass.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*********************************************************************************************/
// BOOL ReadDispDlgProc(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam);

// int PlantVariableInitiation(EXP_POINTER);
// int SPASS_Genotype_Read(HANDLE hInstance);

/* util_fct.c */
// extern int   WINAPI Message(long, LPSTR);
// extern PPLANT  	GetPlantPoi(void);

G_DEFINE_TYPE(spass, spass, EXPERTN_MODUL_BASE_TYPE);

static void spass_class_init(spassClass* klass)
{
}

static void spass_init(spass* self)
{
    self->__INIT_DONE = 0;
    self->__DONE_DONE = 0;
    self->__CAN_START = 0;
    self->__ERROR = RET_SUCCESS;
    self->ini_filename = NULL;
    self->spass_AssimilatePartition_xtuber = 0.0;
    self->spass_ReserveTranslocationfTotalReservWeihgt = 0.0;
    self->spass_COMVR_TO_ZSTAGE_AND_CERES_WHEATnode = 0.0;
    self->spass_DailyCanopyGrossPhotosythesisGaussian = 0;
    self->spass_DailyCanopyGrossPhotosythesisPARCum = 0.0;

    self->temp_par = 0.0;
    self->temp_pard = 0.0;
}

double spass_LIMIT(double v1, double v2, double x)
{
    double vReturn, vzwi;

    vReturn = 0.0;
    vzwi = 0.0;

    if (v1 > v2) {
        vzwi = v1;
        v1 = v2;
        v2 = vzwi;
    }

    if (x <= v1)
        vReturn = v1;
    if (x >= v2)
        vReturn = v2;
    if ((x > v1) && (x < v2))
        vReturn = x;

    return vReturn;
}

// Our Modul Functions:

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function: double WINAPI _loadds RelativeTemperatureResponse(double fTemp,
//														double fTempMin, double fTempOpt,
//double fTempMax)
// Author:	Enli Wang
// Date:		07.11.1996
// Purpose:	This function calculates the effect of temperature on the rate of certain plant process with
//			a temperature optimum (fOptTemp)
// Inputs:	1. fTemp	- Current temperature (C)
//			2. fMinTemp	- Minimum temperature for the process (C)
//			2. fOptTemp	- Optimum temperature for the process (C)
//			2. fMaxTemp	- Maximum temperature for the process (C)
// Outputs:	The calculated temperature effect (0-1)  0-fMinTemp,fMaxTemp; 1-fOptTemp
// Functions Called:
//			None
// Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_RelativeTemperatureResponse(double fTemp, double fMinTemp, double fOptTemp, double fMaxTemp)
{

    double vTemp, vTmin, vTopt, vTmax, p, vRelEff;

    vTemp = fTemp;
    vTmin = fMinTemp;
    vTopt = fOptTemp;
    vTmax = fMaxTemp;

    if ((fTemp <= fMinTemp) || (fTemp >= fMaxTemp))
        vRelEff = 0.0;
    else {
        p = log(2) / log((vTmax - vTmin) / (vTopt - vTmin));
        vRelEff =
            (2 * pow(vTemp - vTmin, p) * pow(vTopt - vTmin, p) - pow(vTemp - vTmin, 2 * p)) / pow(vTopt - vTmin, 2 * p);
    }

    return vRelEff;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Function: double DLL AFGENERATOR(double fInput, RESPONSE* pfResp)
// Author:	Enli Wang
// Date:		07.11.1996
// Purpose:	This function interpolates linearly a value from a RESPONSE table using input fInput
// Inputs:	1. fInput	- the input value (-)
//			2. pfResp	- Pointer to the first element of the given RESPONSE table
// Outputs:	The interpolated value at fInput (-)
// Functions Called:
// Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_AFGENERATOR(double fInput, RESPONSE* pResp)
{

    RESPONSE* pfResp; //, *pResp1;

    double fSlope, fOutput;
    int nPoints, i;

    fOutput = 0.0;

    pfResp = pResp;

    if (pResp[0].iInLen >= pResp[0].iOutLen)
        nPoints = pfResp[0].iOutLen;
    else
        nPoints = pfResp[0].iInLen;

    if (fInput <= pfResp[0].fInput)
        return pfResp[0].fOutput;

    if (nPoints > 0) {
        if (fInput >= pfResp[nPoints - 1].fInput)
            return pfResp[nPoints - 1].fOutput;

        for (i = 0; i < nPoints - 1; i++) {
            if (((fInput >= pfResp[i].fInput) && (fInput < pfResp[i + 1].fInput)) ||
                ((fInput <= pfResp[i].fInput) && (fInput > pfResp[i + 1].fInput)))
                break;
        }

        fSlope = (pfResp[i + 1].fOutput - pfResp[i].fOutput) / (pfResp[i + 1].fInput - pfResp[i].fInput);
        fOutput = pfResp[i].fOutput + fSlope * (fInput - pfResp[i].fInput);
    }

    return fOutput;
}

/*double spass_AFGENERATOR(double fInput, RESPONSE* pfResp)
        {
        RESPONSE *pResp, *pResp1;
        double	 fSlope,fOutput;
        int	  	 nPoints,i;

        nPoints = (int)(pfResp->fInput);

        pResp = pfResp + 1;
        if (fInput<=pResp->fInput)
                return pResp->fOutput;

        pResp = pfResp + nPoints;
        if (fInput>=pResp->fInput)
                return pResp->fOutput;

        pResp = pfResp + 1;
        pResp1= pResp  + 1;
        for (i=1;i<nPoints;i++)
                {
                if (	((fInput>=pResp->fInput)&&(fInput<pResp1->fInput))
                        ||  ((fInput<=pResp->fInput)&&(fInput>pResp1->fInput))   )
                        break;
                else
                        {
                        pResp ++;
                        pResp1++;
                        }
                }

    fSlope = (pResp1->fOutput- pResp->fOutput)/(pResp1->fInput-pResp->fInput);
        fOutput= pResp->fOutput + fSlope*(fInput-pResp->fInput);

        return fOutput;
        }*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Daylength and Photoperiod
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double spass_DaylengthAndPhotoperiod(double fLatitude, int nJulianDay, int nID)
{
    double SINLD = 0.0;
    double COSLD = 0.0;
    double vLatitude = 0.0;
    double vDay = 0.0;
    double vDec = 0.0;
    double RAD = 0.0;
    double fReturn = (double)0.0;
    double fDaylengthAst = (double)0.0;
    double fPhotoperiod = (double)0.0;
    double fDaylengthPs = (double)0.0;

    RAD = PI / 180.0;
    vLatitude = (double)fLatitude;
    vDay = (double)nJulianDay;

    // Declination of the sun as function of iJulianDaynumber (iJulianDay)
    //	vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0));
    vDec = sin(23.45 * RAD) * cos(2.0 * PI * (vDay + 10.0) / 365.0);
    vDec = -asin(vDec);

    // Intermediate variables
    SINLD = sin(RAD * vLatitude) * sin(vDec);
    COSLD = cos(RAD * vLatitude) * cos(vDec);

    // Astronomical daylength (hr)
    fDaylengthAst = (double)(12.0 * (1.0 + 2.0 * asin(SINLD / COSLD) / PI));

    // Photoperiodically active daylength (hr)
    fPhotoperiod = (double)(12.0 * (1.0 + 2.0 * asin((-sin(-4.0 * RAD) + SINLD) / COSLD) / PI));

    // Photosythesis active daylength (hr)
    fDaylengthPs = (double)(12.0 * (1.0 + 2.0 * asin((-sin(8.0 * RAD) + SINLD) / COSLD) / PI));

    fReturn = fDaylengthAst;

    if (nID == 1)
        fReturn = fDaylengthAst;
    if (nID == 2)
        fReturn = fPhotoperiod;
    if (nID == 3)
        fReturn = fDaylengthPs;

    return fReturn;
}

// extern int WINAPI _loadds DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
// extern int WINAPI _loadds LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber);

// BOOL WINAPI ReadDispDlgProc(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam);

// int PlantVariableInitiation(EXP_POINTER);
// extern int SPASS_Genotype_Read(HANDLE hInstance);

// util_fct.c
// extern int   WINAPI Message(long, LPSTR):

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	NitrogenConcentrationLimits_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int spass_NitrogenConcentrationLimits(spass* self)
{

    expertn_modul_base* xpn = &(self->parent);

    PPLANT pPl = xpn->pPl;
    PDEVELOP pDev = pPl->pDevelop;
    PPLTNITROGEN pPltN = pPl->pPltNitrogen;

    pPltN->fLeafMaxConc = spass_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptLvNc);
    pPltN->fStemMaxConc = spass_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptStNc);
    pPltN->fRootMaxConc = spass_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptRtNc);

    pPltN->fLeafMinConc = spass_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinLvNc);
    pPltN->fStemMinConc = spass_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinStNc);
    pPltN->fRootMinConc = spass_AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinRtNc);

    // SG/05/05/99/zusätzlich optimale Werte:
    pPltN->fLeafOptConc = (float)0.7 * pPltN->fLeafMaxConc; // instead of 0.5 from van Keulen
    pPltN->fStemOptConc = (float)0.7 * pPltN->fStemMaxConc; // instead of 0.5 from van Keulen
    pPltN->fRootOptConc = (float)0.7 * pPltN->fRootMaxConc; // instead of 0.5 from van Keulen

    return RET_SUCCESS;
}

/*******************************************************************************
** EOF */

void spass_init_agmip_vars(spass* self)
{
    // expertn_modul_base *xpn = &(self->parent);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->yield, "spass.yield", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_above_ground_biomass, "spass.total_above_ground_biomass", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->total_et, "spass.total_et", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_intercepted_par, "spass.total_intercepted_par", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->max_lai, "spass.max_lai", 0.0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->anthesis_year, "spass.anthesis_year", 0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->anthesis_date, "spass.anthesis_date", 0.0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->maturity_year, "spass.maturity_year", 0.0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->maturity_date, "spass.maturity_date", 0.0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->harvest_year, "spass.harvest_year", 0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->harvest_date, "spass.harvest_date", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->SOC_at_sowing_30, "spass.SOC_at_sowing_30", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->SOC_at_sowing, "spass.SOC_at_sowing", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_net_ecosystem_exchange, "spass.total_net_ecosystem_exchange", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list,
                                  self->total_net_primary_productivity,
                                  "spass.total_net_primary_productivity",
                                  0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->total_N2ON, "spass.total_N2ON", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->total_N2ON_an, "spass.total_N2ON_an", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_N_leaching_1k5, "spass.total_N_leaching_1k5", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_N_leaching_1k5_an, "spass.total_N_leaching_1k5_an", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_water_drainage_1k5, "spass.total_water_drainage_1k5", 0.0);
    xpn_register_var_init_pdouble(
        self->parent.pXSys->var_list, self->total_water_drainage_an_1k5, "spass.total_water_drainage_an_1k5", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->max_root_depth, "spass.max_root_depth", 0.0);

    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->water_stress_days, "spass.water_stress_days", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->means_stress, "spass.mean_stress", 0.0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list, self->n_stress_days, "spass.n_stress_days", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->nmean_stress, "spass.nmean_stress", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->mineral_N_150, "spass.mineral_N_150", 0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list, self->N_fert_amounts, "spass.N_fert_amounts", 0.0);

    self->saved_year = 0;
    self->season = 0;
    self->oldseason = 0;

    self->_total_intercepted_par = 0.0;
    self->_total_et = 0.0;
    self->_max_lai = 0.0;
    self->_total_N2ON_an = 0.0;
    self->_total_N_leaching_1k5_an = 0.0;
    self->_total_water_drainage_an_1k5 = 0.0;
    self->_total_water_drainage_1k5 = 0.0;
    self->_total_N2ON = 0.0;
    self->_total_N2ON_an = 0.0;
    self->_max_root_depth = 0.0;
    self->leachNCUMold = 0.0;
    self->leachNCUMold_veg = 0.0;

    self->_means_stress = 0.0;
    self->_water_stress_days = 0;

    self->_fpot_day = 0.0;
    self->_fact_day = 0.0;

    self->_npot_day = 0.0;
    self->_nact_day = 0.0;

    self->_nmean_stress = 0.0;
    self->_n_stress_days = 0;

    self->_N_fert_amounts = 0.0;

    /*xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->total_intercepted_par,"spass.intercepted_par",0.0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->max_lai,"spass.max_lai",0.0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list,self->anthesis_date,"spass.anthesis_date",0);
    xpn_register_var_init_pint(self->parent.pXSys->var_list,self->harvest_date,"spass.harvest_date",0);
    xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->PARCum,"spass.PARCum",0.0);
    */

    // output for AgMIP
    /*int year;
    double yield; // t ha-1
    double total_above_ground_biomass; // t ha-1
    double total_et; // (over growing season) mm
    double total_intercepted_par; // MJ ha-1
    double max_lai; // (over growing season) m²/m²
    int anthesis_date; //DOY
    int maturity_date; //DOY
    int harvest_date; // DOY
    double SOC_at_sowing_30; // 20cm oder 30cm g m-2
    double SOC_at_sowing; // whole profile
    double total_net_ecosystem_exchange; // over season g m-2 (???)
    double total_net_primary_productivity; // over season g m-2 (???)
    double total_N2ON; // over season kg ha-1
    double total_N2ON_an; // annual kg ha-1
    double total_N_leaching_1k5_an; // annual kg/ha (at 1.5m)
    double total_water_drainage_1k5; // over season mm/ha
    double total_water_drainage_an_1k5; // annual mm/ha
    double max_root_depth; // season m;*/
}

void spass_agmip_calc_season_vars(spass* self)
{
    expertn_modul_base* xpn = &(self->parent);
    PNFERTILIZER pNF = xpn->pMa->pNFertilizer;
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    PMANAGEMENT pMa = xpn->pMa;
    PSPROFILE pSo = xpn->pSo;
    PCHEMISTRY pCh = xpn->pCh;
    PCPROFILE pCP = pCh->pCProfile;
    PSLAYER pSL;
    PCLAYER pCL;

    double act_depth, leachNR;

    if ((xpn_time_compare_date(pTi->pSimTime->iyear,
                               pTi->pSimTime->mon,
                               pTi->pSimTime->mday,
                               pMa->pSowInfo->Year,
                               pMa->pSowInfo->Month,
                               pMa->pSowInfo->Day) >= 0) &&
        (xpn_time_compare_date(pTi->pSimTime->iyear,
                               pTi->pSimTime->mon,
                               pTi->pSimTime->mday,
                               pPl->pModelParam->HarvestYear,
                               pPl->pModelParam->HarvestMonth,
                               pPl->pModelParam->HarvestDay) <= 0)) {

        self->_total_et += xpn->pWa->fActETR * pTi->pTimeStep->fAct;
        self->_total_N2ON += pCh->pCProfile->fN2OEmisR * pTi->pTimeStep->fAct * (double)24e-5;

        self->_total_water_drainage_1k5 += xpn->pWa->fPercolR * pTi->pTimeStep->fAct;

        // leaching N:
        leachNR =
            (pCP->dUreaLeachCum + pCP->dNH4LeachCum + pCP->dNO3LeachCum + pCP->dN2ODrainCum) - self->leachNCUMold_veg;
        self->leachNCUMold_veg = (pCP->dUreaLeachCum + pCP->dNH4LeachCum + pCP->dNO3LeachCum + pCP->dN2ODrainCum);
        self->_total_N_leaching_1k5 += leachNR;

        if (pPl->pCanopy->fLAI > self->_max_lai) {
            self->_max_lai = pPl->pCanopy->fLAI;
        }

        if (xpn->pPl->pRoot->fDepth > self->_max_root_depth) {
            self->_max_root_depth = xpn->pPl->pRoot->fDepth;
        }

        if (NewDay(pTi)) {
            if (self->_fpot_day > 1e-5) {
                if (self->_fact_day / self->_fpot_day < (1.0 - 1e-2)) {
                    self->_water_stress_days++;
                    self->_means_stress += (self->_fact_day / self->_fpot_day);
                }
            }

            if (self->_npot_day > 1e-5) {
                if (self->_nact_day / self->_npot_day < (1.0 - 1e-2)) {
                    self->_n_stress_days++;
                    self->_nmean_stress += (self->_nact_day / self->_npot_day);
                }
            }
        }

        // sow:
        if ((xpn_time_compare_date(pTi->pSimTime->iyear,
                                   pTi->pSimTime->mon,
                                   pTi->pSimTime->mday,
                                   pMa->pSowInfo->Year,
                                   pMa->pSowInfo->Month,
                                   pMa->pSowInfo->Day) == 0) &&
            (NewDay(pTi))) {
            self->SOC_at_sowing_30 = 0.0;
            self->SOC_at_sowing = 0.0;

            act_depth = 0.0;
            for (pSL = pSo->pSLayer->pNext; (pSL->pNext != NULL); pSL = pSL->pNext) {
                act_depth += pSL->fThickness * 1.0e-3;
                if (act_depth <= 0.3) {
                    self->SOC_at_sowing_30 += pSL->fCHumus / 10.0;
                }

                self->SOC_at_sowing += pSL->fCHumus / 10.0;
            }
        }

        // anthesis:
        if (xpn->pPl->pDevelop->fStageSUCROS >= 1.0) {
            if (pTi->pSimTime->iyear != self->anthesis_year) {
                self->anthesis_date = (int)xpn->pTi->pSimTime->fTimeY;
                self->anthesis_year = xpn->pTi->pSimTime->year;
            }
        }

        // harvest:
        if ((xpn_time_compare_date(pTi->pSimTime->iyear,
                                   pTi->pSimTime->mon,
                                   pTi->pSimTime->mday,
                                   pPl->pModelParam->HarvestYear,
                                   pPl->pModelParam->HarvestMonth,
                                   pPl->pModelParam->HarvestDay) == 0) &&
            (NewDay(pTi))) {
            self->harvest_date = (int)xpn->pTi->pSimTime->fTimeY;
            self->harvest_year = xpn->pTi->pSimTime->year;
            self->yield = xpn->pPl->pBiomass->fFruitWeight / 1000.0;
            self->total_above_ground_biomass =
                (xpn->pPl->pBiomass->fLeafWeight + xpn->pPl->pBiomass->fStemWeight + xpn->pPl->pBiomass->fBranchWeight +
                 xpn->pPl->pBiomass->fFruitWeight + xpn->pPl->pBiomass->fWoodWeight) /
                1000.0;
            self->total_et = self->_total_et;
            self->total_intercepted_par = self->_total_intercepted_par;
            self->max_lai = self->_max_lai;
            self->max_root_depth = self->_max_root_depth / 100.0;
            if (pPl->pDevelop->fStageSUCROS < (double)2.0) {
                self->maturity_date = 0;
                self->maturity_year = 0;
            }

            self->total_N2ON = self->_total_N2ON;
            self->total_N_leaching_1k5 = self->_total_N_leaching_1k5;
            self->total_water_drainage_1k5 = self->_total_water_drainage_1k5;

            // 48% C in vegetativer Biomasse
            self->total_net_ecosystem_exchange =
                (self->total_above_ground_biomass + (xpn->pPl->pBiomass->fRootWeight / 1000.0)) * 100.0 * 0.48;
            self->total_net_primary_productivity = self->total_net_ecosystem_exchange;

            self->water_stress_days = self->_water_stress_days;
            if (self->water_stress_days == 0) {
                self->means_stress = 0.0;
            } else {
                self->means_stress = 1.0 - self->_means_stress / (double)self->water_stress_days;
            }

            self->n_stress_days = self->_n_stress_days;
            if (self->n_stress_days == 0) {
                self->nmean_stress = 0.0;
            } else {
                self->nmean_stress = 1.0 - self->_nmean_stress / (double)self->n_stress_days;
            }

            self->mineral_N_150 = 0.0;
            act_depth = 0.0;
            for (pCL = pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext; (pCL->pNext != NULL);
                 pSL = pSL->pNext, pCL = pCL->pNext) {
                act_depth += pSL->fThickness * 1.0e-3;
                if (act_depth <= 1.5) {
                    self->mineral_N_150 += pCL->fNO3N + pCL->fNH4N;
                }
            }
            self->N_fert_amounts = self->_N_fert_amounts;
        }

        // maturity:
        if ((pPl->pDevelop->fStageSUCROS >= (double)2.0) && (NewDay(pTi))) {
            if (pTi->pSimTime->iyear != self->maturity_year) {
                self->maturity_date = (int)xpn->pTi->pSimTime->fTimeY;
                self->maturity_year = xpn->pTi->pSimTime->year;
            }
        }

		if ((NewDay(pTi)) && (pNF != NULL)) {
            if (xpn_time_compare_date(xpn->pTi->pSimTime->year,
                                      xpn->pTi->pSimTime->mon,
                                      xpn->pTi->pSimTime->mday,
                                      pNF->Year,
                                      pNF->Month,
                                      pNF->Day) == 0) {
                self->_N_fert_amounts += pMa->pNFertilizer->fNH4N + pMa->pNFertilizer->fNO3N;
            }
        }

    } else {
        self->_total_et = 0.0;
        self->_total_intercepted_par = 0.0;
        self->_max_lai = 0.0;
        self->_total_N2ON = 0.0;
        self->_total_water_drainage_1k5 = 0.0;
        self->_max_root_depth = 0.0;
        self->_total_N_leaching_1k5 = 0.0;
        self->_means_stress = 0.0;
        self->_water_stress_days = 0;
        self->_nmean_stress = 0.0;
        self->_n_stress_days = 0;
        self->_N_fert_amounts = 0;
    }

    if (NewDay(pTi)) {
        self->_fpot_day = 0.0;
        self->_fact_day = 0.0;
        self->_nact_day = 0.0;
        self->_npot_day = 0.0;
    }
    self->_fpot_day += xpn->pWa->fPotETR * xpn->pTi->pTimeStep->fAct;
    self->_fact_day += xpn->pWa->fActETR * xpn->pTi->pTimeStep->fAct;

    C_DEBUG(self->_fpot_day)
    C_DEBUG(self->_fact_day)

    C_DEBUG(self->_npot_day)
    C_DEBUG(self->_nact_day)
}

void spass_agmip_calc_an_vars(spass* self)
{
    expertn_modul_base* xpn = &(self->parent);
    //PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    //PMANAGEMENT pMa = xpn->pMa;
   // PSPROFILE pSo = xpn->pSo;
    PCHEMISTRY pCh = xpn->pCh;
    PCPROFILE pCP = pCh->pCProfile;
    //PSLAYER pSL;
    //PCLAYER pCL;
    double leachNR;

    self->_total_N2ON_an += pCh->pCProfile->fN2OEmisR * pTi->pTimeStep->fAct * (double)24e-5;

    // leaching N:
    leachNR = (pCP->dUreaLeachCum + pCP->dNH4LeachCum + pCP->dNO3LeachCum + pCP->dN2ODrainCum) - self->leachNCUMold;
    self->leachNCUMold = (pCP->dUreaLeachCum + pCP->dNH4LeachCum + pCP->dNO3LeachCum + pCP->dN2ODrainCum);
    self->_total_N_leaching_1k5_an += leachNR;

    self->_total_water_drainage_an_1k5 += xpn->pWa->fPercolR * pTi->pTimeStep->fAct;

    if ((xpn->pTi->pSimTime->mday == 31) && (xpn->pTi->pSimTime->mon == 12) && (xpn->pTi->pSimTime->hour >= 22) &&
        (xpn->pTi->pSimTime->year != self->saved_year)) {

        self->saved_year = xpn->pTi->pSimTime->year;
        self->total_N2ON_an = self->_total_N2ON_an;
        self->total_N_leaching_1k5_an = self->_total_N_leaching_1k5_an;
        self->total_water_drainage_an_1k5 = self->_total_water_drainage_an_1k5;

        self->_total_N2ON_an = 0.0;
        self->_total_N_leaching_1k5_an = 0.0;
        self->_total_water_drainage_an_1k5 = 0.0;
    }
}
