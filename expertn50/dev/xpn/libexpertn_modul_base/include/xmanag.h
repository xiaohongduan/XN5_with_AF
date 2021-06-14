/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by ZeusSoft, Ing. Buero Bauer
 *                              www.zeussoft.de
 *                              Tel: 0700 zeussoft
 *
 * Author:      C.Sperr
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Priesack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Simulation time objects
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: xmanag.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:08
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 7.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XMANAG_H
#define _INC_EXPERTN_XMANAG_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct stSowInfo *PMSOWINFO;
typedef struct stSowInfo
{
	int Day;
	int Month;
	int Year;
	double	fPlantDens;
	double	fSowDepth;
	double	fTKW;
	double	fRowWidth;
	PMSOWINFO   pNext;
	PMSOWINFO   pBack;
} STSOWINFO;

typedef struct stLitter *PMLITTER;
typedef struct stLitter
{
	//int	lHarvestDate;
	int   Day;
	int   Month;
	int   Year;
	double	fTopAmount;
	double	fTopC;
	double	fTopCN;
	double	fRootAmount;
	double	fRootC;
	double	fRootCN;
	double	fTotalAmount;
	double	fTotalC;
	double	fTotalCN;
	double	fDecRate;
	int   iCarryOff;
	PMLITTER  pNext;
	PMLITTER  pBack;
} STLITTER;
		
typedef struct stIrrigation *PMIRRIGATION;
typedef struct stIrrigation 
{
	int     Day;
	int 	Month;
	int 	Year;
//Hong	char	acName[51];   /// 10.10.97 CS
//Hong	char	acCode[30];	
    char	*acName; // Hong added, Irrigation type
	char	*acCode; //  Hong added, Irrigation Code	
	double	fBegin;
	double	fEnd;
	double	fDayFrac;
	double	fAmount;
	double	fRate;
	double   fNO3N;
	double   fNH4N;
	double   fUreaN;
	double	fCorgManure;
	double	fNorgManure;
	double   fDOC;
	double   fDON;
	double	fSystem;           
	double	fEfficiency;
	PMIRRIGATION   pNext;
	PMIRRIGATION   pBack;
} STIRRIGATION;

typedef struct stnfertilizer   * PNFERTILIZER;
typedef struct stnfertilizer
{            
	int     iID;
	int		Day;
	int 	Month;
	int     Year;
	char		*acName; // Fertilizer Name
	char		*acCode; // Fertilizer Code
	double		fDryMatter; // Dry matter (organic fertilization)
	double		forgMatter;	// Organic dry matter < fDryMatter
	double		fCorgManure; // C of organic manure, calculated by XN
	double		fNorgManure; // Organic Substance N	
	double		fOrgManureCN; // C/N Ratio organic manure, calculated by XN
	double		fTotalN; // Mineral/Organic total N
	double		fNH4N; // Mineral/Organic NH4N
	double		fNO3N; // Mineral NO3N
	double		fUreaN; // Mineral urea
	double		afInfParam[3]; // Infiltration parameters, depending on Fertilizer Code
	PNFERTILIZER pNext;
	PNFERTILIZER pBack;
} STNFERTILIZER;    
	   
typedef struct stTillage    * PTILLAGE;
typedef struct stTillage   
{
	int Day;
	int Month;
	int Year;
	char		*acName;	
	char		*acCode;		
	int		iEquipment;
	double		fDepth;
	double		fEffMix;
	double		fEffLoose;
	double		fEffIncorp;
	PTILLAGE   pNext;
	PTILLAGE   pBack;
} STTILLAGE;

/*typedef struct stStartvalue   *PSTARTVALUE;
typedef struct stStartvalue
{
	int 	        lDate;
	int		iDay;
	double	afThickness[MAX_DEPTH_PER_MEASURE];
	double	afNO3N[MAX_DEPTH_PER_MEASURE];
	double	afNH4N[MAX_DEPTH_PER_MEASURE];
	double	afH2O[MAX_DEPTH_PER_MEASURE];
	double	afTemp[MAX_DEPTH_PER_MEASURE];
	double	afRootDens[MAX_DEPTH_PER_MEASURE];
} STSTARTVALUE;*/

typedef struct stmanagement   *PMANAGEMENT;
typedef struct stmanagement
{
	PMSOWINFO		pSowInfo;
	PMLITTER     	pLitter;
	PMIRRIGATION	pIrrigation;
	PNFERTILIZER	pNFertilizer;
	PTILLAGE        pTillage;
	//PSTARTVALUE	Startvalue;
	
}	STMANAGEMENT;

#endif // #ifndef _INC_EXPERTN_XMANAG_H

/*******************************************************************************
** EOF */
