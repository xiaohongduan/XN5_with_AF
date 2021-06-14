
int iForest;
/********************************************************************************************/
/* Procedur    :   MinerDAYCENT()                                                            */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Mineralisierung                                                          */
/*                 Methode DAYCENT                                   */
/*                                                                                          */
/*                 HMGU/sb & ep              2009                                           */
/*                                                                                          */
/********************************************************************************************/
/* veränd. Var.		pCL->f...             pCL->f...                                         */
/********************************************************************************************/

int WINAPI MinerDAYCENT(EXP_POINTER)// returns total daily NNM [g/m2] = mineralisation - immobilisation
{
// used in loops over all layers
PCLAYER		pCL;
PSLAYER		pSL;
PWLAYER		pWL;
PHLAYER		pHL;

int const TimeStepsPerDay=4;
float dtDecompDC=(float)1.0f/365.f/TimeStepsPerDay;	//fraction of year per timestep
static struct fixedparameter fixed;
//static struct parameter par;
float C=0.0, N=0.0, lig=0.0, Navail=0.0, oldlig=0.0, oldsC, sC=0.0, sN=0.0, newlig=0.0, mC=0.0, mN=0.0, ret; //partition litter into struc/metab
float tcflow; //total c flow
float defacSRFC, anerb;
int i,iday;
float *pdecf;
float rneww1[2],								//ratio C/N fine branches to: [0]SOM1 [1]SOM2
	  rneww2[2],								//ratio C/N large wood    to: [0]SOM1 [1]SOM2
	  rneww3[2],								//ratio C/N coarse roots  to: [0]SOM1 [1]SOM2
	  rnewas[2],								//ratio C/N above ground structural  to: [0]SOM1 [1]SOM2
	  rnewbs[2];								//ratio C/N below ground structural  to: [0]SOM1 [1]SOM2
float radds1;
float CtoSOM2, CtoSOM1, co2loss;
float orgNflow=0.0, minNflow=0.0;		
float mineralFlow;// Mineralization associated with respiration
float cfmes1,rcemet,cfsfs2,rceto2,rceto3,rceto1,cfs1s3,cfs1s2,cfs2s1,cfs2s3,cfs3s1;
float microbeC; //C flow out of surface microbe pool 
float efftext;
float frNH4, frNO3;
float dummy;
float MinByWood,MinByStructLitter,MinByMetabLitter,MinBySOM; //Mineralisation by pools [kg/ha]
float ImmByWood,ImmByStructLitter,ImmByMetabLitter,ImmByInput,ImmBySOM; //Immobilisation by pools [kg/ha]
float fDrain =(float)1.0; 
//FILE *outf;

if(SimulationStarted)
{
//set fixed parameters (values from Century file 'ffixed.100' for forests) 
	fixed = SetFixedParameters();
//read site parameter from 'century.cfg' file
	//par = ReadParameter();

//********************************************************************************************
 // Initialize pools
//********************************************************************************************
 // A) Wood
	//wood 1: fine branch //aus xnm file Marke 20000 [kg/ha]
		//pCh->pCProfile->fCBranchLitterSurf = par.wood1C;
		//pCh->pCProfile->fNBranchLitterSurf = par.wood1C / par.cerfor[2][FBRCH];//calciv_dc line 216 
	//wood 2: large wood  //aus xnm file Marke 20000 [kg/ha]
		//pCh->pCProfile->fCStemLitterSurf = par.wood2C;
		//pCh->pCProfile->fNStemLitterSurf = par.wood2C / par.cerfor[2][LWOOD];//calciv_dc line 216 
	//wood 3: coarse roots //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCGrossRootLitter = pCL->fCManure;
			pCL->fNGrossRootLitter = pCL->fNManure;
		}
 // B) Litter
	//Belowground = fine root litter //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCFineRootLitter = pCL->fCLitter;
			pCL->fNFineRootLitter = pCL->fNLitter /(float)3.0; //Adjustment from workbook 2.b.
		}
	//Aboveground = leaves litter	//aus xnm file Marke 20004
	 pCh->pCProfile->fCLeafLitterSurf = pCh->pCProfile->fCLitterSurf;
	 pCh->pCProfile->fNLeafLitterSurf = pCh->pCProfile->fNLitterSurf;

	//SOM soil //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCHumusFast   = pSL->fCHumus * 0.03f;	//percentages from century workbook Ch. II
			pCL->fNHumusFast   = pSL->fNHumus * 0.03f / 0.7f;   
			pCL->fCHumusSlow   = pSL->fCHumus * 0.65f;  
			pCL->fNHumusSlow   = pSL->fNHumus * 0.65f / 1.1615f;//neu berechnet, so dass alles N aufgeteilt ist  
			pCL->fCHumusStable = pSL->fCHumus * 0.32f;  
			pCL->fNHumusStable = pSL->fNHumus * 0.32f / 0.7f;  
		}
	//SOM1 srfc //aus xnm file Marke 20004
		pCh->pCProfile->fCMicLitterSurf = pCh->pCProfile->fCHumusSurf;
		pCh->pCProfile->fNMicLitterSurf = pCh->pCProfile->fNHumusSurf;
}

if (NewDay(pTi))
{

//********************************************************************************************
// 0) Initialize 
//********************************************************************************************

	for(pCL=pCh->pCLayer;pCL->pNext!=NULL;pCL=pCL->pNext)	 
	{	  
		pCL->fMinerR = 0.0;
		pCL->fNImmobR = 0.0;
		pCL->fCO2ProdR = 0.0;
		pCL->fCO2C= 0.0;
		pCL->fHumusMinerR = 0.0;
		pCL->fLitterMinerR = 0.0;
		pCL->fNLitterImmobR = 0.0;
	}
	pCh->pCProfile->fNHumusImmobDay = 0.0;
	MinByWood=MinByStructLitter=MinByMetabLitter=MinBySOM= 0.0; 
	ImmByWood=ImmByStructLitter=ImmByMetabLitter=ImmByInput=ImmBySOM= 0.0; 
	//DailyCorrection=0.0;

//********************************************************************************************
// I) Calculate temperature and water factor for decomposition, do this daily 
//********************************************************************************************
//	a) Surface pools: defacSRFC, anerb 
		decf20cm(&defacSRFC, &anerb, fixed, fDrain, exp_p);
//	b) Soil layers:   pdecf[i]
		pdecf =(float *)malloc((pSo->iLayers-2) * sizeof (float));
		for(i=0,pSL=pSo->pSLayer->pNext,pHL=pHe->pHLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pHL=pHL->pNext, pWL=pWL->pNext, i++)
		{
			pdecf[i]=decfSL(pSL->fSand,(pWL->fContAct/pSL->fPorosity),pHL->fSoilTemp,fixed,exp_p);
		}

//********************************************************************************************


//********************************************************************************************
// II) Input litter (leaves to surface, fine roots to soil)
//********************************************************************************************
// a) surface litter
 		C=pCh->pCProfile->fCLeafLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNLeafLitterSurf * kgphaTOgpm2;
		lig=pCh->pCProfile->fLigFracLeaf;
		Navail=0.0; //no direct adsorbtion in surface litter
		oldlig = pCh->pCProfile->fLignFracStrLitSurf;
		oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
		if(C>0.0f && N>0.0f) 
		{
			ret = PartitionResidue(C,N/C,lig,SRFC,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,fixed); 		
			pCh->pCProfile->fCLeafLitterSurf = 0.0; //all litter is now in struc/metab pools
			pCh->pCProfile->fNLeafLitterSurf = 0.0;
			pCh->pCProfile->fCStrLitterSurf += sC * gpm2TOkgpha;
			pCh->pCProfile->fNStrLitterSurf += sN * gpm2TOkgpha;
			pCh->pCProfile->fLignFracStrLitSurf = newlig;
			pCh->pCProfile->fCMtbLitterSurf += mC * gpm2TOkgpha;
			pCh->pCProfile->fNMtbLitterSurf += mN * gpm2TOkgpha;
		}
	 
// b) soil litter (fine roots)
		for(i=0,pCL=pCh->pCLayer->pNext; pCL->pNext!=NULL;pCL=pCL->pNext,i++)
		{
 			C=pCL->fCFineRootLitter * kgphaTOgpm2;
			N=pCL->fNFineRootLitter * kgphaTOgpm2;								
			lig=pCh->pCProfile->fLigFracLeaf;
			Navail=(pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //direct adsorbtion from first 20 cm 
			oldlig = pCh->pCProfile->fLignFracStrLitSurf;
			oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
			if(C>0.0f && N>0.0f) 
			{
				ret = PartitionResidue(C,N/C,lig,SOIL,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,fixed); 			
				//direct adsorbtion of mineral N
				if(ret>0.0)
				{
				ret = min (ret, (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
				frNH4 = pCL->fNH4N / (pCL->fNO3N + pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
				frNO3 = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N); 
				pCL->fNH4N -= frNH4  
							* ret * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
				pCL->fNO3N -= frNO3
							* ret * gpm2TOkgpha;
				pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
				pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
				pCL->fNImmobR += ret * gpm2TOkgpha;
				ImmByInput += ret * gpm2TOkgpha;
				if(pCL->fNO3N <0.0 ) Message(1,"Warning: NO3 @ input negative!");	 
				if(pCL->fNH4N <0.0 ) Message(1,"Warning: NH4N @ input negative!");	 
	
				}
				//all litter is now in struc/metab pools
				pCL->fCFineRootLitter = 0.0; 
				pCL->fNFineRootLitter = 0.0;
				pCL->fCStrcLitter += sC * gpm2TOkgpha;
				pCL->fNStrcLitter += sN * gpm2TOkgpha;
				pCL->fLignFracStrcLit  = newlig;
				pCL->fCMtbLitter += mC * gpm2TOkgpha;
				pCL->fNMtbLitter += mN * gpm2TOkgpha;
			}
		}//loop over soil layers

//************************************************************************************************
// IV)  Calculates the decomposition ratio.
// 	Determine C/N of new material entering 'Box B'. 	
// 	The C/N ratios for structural and wood are computed once per day here;
//************************************************************************************************
if(iForest == 1) //forest
{
// Fine branch
// Ratio for new SOM1 from decomposition of Fine Branches
		C=pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
		rneww1[0]= AboveGroundDecompRatio(C,N,2.0f,fixed);
// Ratio for new SOM2 from decomposition of Fine Branches
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
		radds1=fixed.rad1p[0] + fixed.rad1p[1] * 
			( AboveGroundDecompRatio (C,N,2.5f,fixed)- fixed.pcemic[1]);
		rneww1[1]= rneww1[0]+ radds1;
// Large Wood
// Ratio for new SOM1 from decomposition of Large Wood
		C=pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
	    rneww2[0] =AboveGroundDecompRatio(C,N,2.0f,fixed);
// Ratio for new SOM2 from decomposition of Large Wood
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
	    rneww2[1] = rneww2[0] + fixed.rad1p[0] + fixed.rad1p[1] * 
			( AboveGroundDecompRatio (C,N,2.0f,fixed)- fixed.pcemic[1]);
	    rneww2[1] =  max (rneww2[1],fixed.rad1p[2]);
// Coarse Roots
// Ratio for new SOM1 from decomposition of Coarse Roots
		rneww3[0] = fixed.varat1[0];
// Ratio for new SOM2 from decomposition of Coarse Roots
		rneww3[1] = fixed.varat2[1]; 
}//forest
// Surface Structural
// Ratios for new som1 from decomposition of AG structural
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
		rnewas[0] = AboveGroundDecompRatio(C,N,2.5f,fixed);
// Ratio for new SOM2 from decomposition of AG strutural
		radds1=fixed.rad1p[0] + fixed.rad1p[1] * 
		( AboveGroundDecompRatio (C,N,2.5f,fixed)- fixed.pcemic[1]);
		rnewas[1] =max( fixed.rad1p[2], rnewas[0] + radds1);
// Soil Structural
// Ratios for new som1 from decomposition of BG structural
		rnewbs[0] = fixed.varat1[0];
// Ratio for new SOM2 from decomposition of BG strutural
		rnewbs[1] = fixed.varat2[0];


//   ***************************************************************************************
//		Decomposition Routines (CALL: simsom_dc.cpp line 280), do 4 times a day
//		 IV) DecomposeLitter  
//		  V) DecomposeWood  
//		 VI) DecomposeSOM  
//				Steps:       
//				1. Surface SOM1 decomposes to SOM2 with CO2 loss.    
//				2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss and leaching.         
//				3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss       
//				4. SOM3 decomposes to soil SOM1 with CO2 loss.
//   ***************************************************************************************
 
for(iday=1;iday<=TimeStepsPerDay;iday++)  //4 times a day
{
//************************************************************************************************
// IV) Decompose Litter
//************************************************************************************************

//SURFACE STRUCTURAL Material to SOM1[SRFC] and SOM2
	C		= pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLignFracStrLitSurf;
	Navail  = getNtoDepth(20.0f,exp_p);  //top 20cm 
	if (C >= 10e-7) 
	{
	tcflow=min(C , fixed.strmax[SRFC]* defacSRFC * fixed.dec1[SRFC] //C flow from A to B
		       * (float)exp(-fixed.pligst[SRFC]*lig) ) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rnewas[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCStrLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;   
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rnewas[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,  20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow, 20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByStructLitter += minNflow * gpm2TOkgpha; ;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( (float)ret, 20.0f, exp_p);			//B += minflow
		ImmByStructLitter +=  ret * gpm2TOkgpha; ;
		} 

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
	 
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rnewas[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByStructLitter += minNflow * gpm2TOkgpha; ;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByStructLitter +=  ret * gpm2TOkgpha; ;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7

//SOIL STRUCTURAL Material to SOM1[SOIL] and SOM2
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
		pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCStrcLitter * kgphaTOgpm2; 
	N		= pCL->fNStrcLitter * kgphaTOgpm2; 
	lig		= pCL->fLignFracStrcLit;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
	if (C >= 10e-7) 
	{
	tcflow=min(C , fixed.strmax[SOIL]* pdecf[i] *  anerb  * fixed.dec1[SOIL] //C flow from A to B
		       * (float)exp(-fixed.pligst[SOIL]*lig) ) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rnewbs[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCL->fCStrcLitter -= tcflow * gpm2TOkgpha;
		pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rnewbs[1],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		MinByStructLitter += minNflow * gpm2TOkgpha; ;	 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if (minNflow > 1e-7)
		{
		pCL->fCHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4 *  (float)fabs(minNflow) * gpm2TOkgpha; //Mineral -= minflow
		pCL->fNO3N -= frNO3 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		pCL->fNImmobR  +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		ImmByStructLitter +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		}
		}
 
		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SOIL];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
		 
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast += CtoSOM1 * gpm2TOkgpha;;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByStructLitter += minNflow * gpm2TOkgpha;  
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral -= minflow				
		pCL->fNO3N -= frNO3 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByStructLitter +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha; 
 		}
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7
	}//loop over all layer

 
//METABOLIC surface to SOM1
	C = pCh->pCProfile->fCMtbLitterSurf * kgphaTOgpm2;
	N = pCh->pCProfile->fNMtbLitterSurf * kgphaTOgpm2;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-6)
	{
	// Determine C/E ratios for flows to SOM1
		rcemet = AboveGroundDecompRatio(C,N,2.5f,fixed);

	// If decomposition can occur
	if (CanDecompose(C,N,rcemet,Navail) )
	{
		// C Flow
		// Compute total C flow out of metabolic into SOM1[layer]
		tcflow = C * defacSRFC * fixed.dec2[SRFC] * anerb * dtDecompDC;
		// Make sure metab does not go negative.
		tcflow = min ( tcflow, C );
		co2loss = tcflow * fixed.pmco2[SRFC];

		cfmes1 = tcflow - co2loss;
		pCh->pCProfile->fCMtbLitterSurf -= cfmes1 * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM1(cfmes1,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNMtbLitterSurf -= mineralFlow * gpm2TOkgpha; 
		MinByMetabLitter += mineralFlow * gpm2TOkgpha; 
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(cfmes1,rcemet,C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNMtbLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM1(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNMtbLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByMetabLitter += minNflow * gpm2TOkgpha;
		}
	if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow), 20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM1( ret, 20.0f, exp_p);			//B += minflow
		ImmByMetabLitter += ret * gpm2TOkgpha;
		}
	}// if candecomp
	} //if (pCh->pCProfile->fCMtbLitterSurf >= 10e-6)


//METABOLIC soil to SOM1
	// Determine C/E ratios for flows to SOM1
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	if(getNtoDepth(20.0f,exp_p) <= 0.0f)			rcemet=fixed.varat1[0]; //set ratio to max. allowed
 	else if(getNtoDepth(20.0f,exp_p) <= fixed.varat1[2])	rcemet=fixed.varat1[1]; //set ratio to min. allowed
	else	//  aminrl > 0 and <= varat(2,element)
	rcemet=(1.0f - getNtoDepth(20.0f,exp_p)/fixed.varat1[2])*(fixed.varat1[0]-fixed.varat1[1])+fixed.varat1[1];

	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCMtbLitter * kgphaTOgpm2;
	N		= pCL->fNMtbLitter * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; // 
	if (C >= 10e-7)
	{	
	// If decomposition can occur
	if (CanDecompose(C,N,rcemet,Navail) )
		{
		// C Flow
		// Compute total C flow out of metabolic into SOM1[layer]
		tcflow = C * pdecf[i] * fixed.dec2[SOIL] * anerb * dtDecompDC;
		// Make sure metab does not go negative.
		tcflow = min ( tcflow, C );	    //gross  C flow to SOM1
		if(tcflow > 10e-7)
		{
		co2loss=tcflow * fixed.pmco2[SOIL];
		tcflow -= co2loss;					//net  C flow to SOM1
		 
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast  += tcflow * gpm2TOkgpha; 
		pCL->fCMtbLitter  -= tcflow * gpm2TOkgpha; 
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNMtbLitter -= mineralFlow * gpm2TOkgpha; 
		MinByMetabLitter += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(tcflow,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNMtbLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNMtbLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByMetabLitter += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByMetabLitter +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		}//if(CtoSOM1 > 10e-7)
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)
	} // loop over all layer

//_______________________________________________________________________________________

//		 V) DecomposeWood  
//_______________________________________________________________________________________

if(iForest == 1) //forest
{
// FINE BRANCHES
	C		= pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLigFracFineRoot;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-7) 
	{
	tcflow=C * defacSRFC * pCh->pCProfile->fFineBranchDecR * (float)exp(-fixed.pligst[SRFC]*lig) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww1[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCBranchLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww1[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( ret,20.0f, exp_p);			//B += minflow	
		ImmByWood +=  ret * gpm2TOkgpha;
		} 
	
		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1

		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww1[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 

// LARGE WOOD	
	C		= pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLigFracBranch;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-7) 
	{
	tcflow= C * defacSRFC * pCh->pCProfile->fLargeWoodDecR * (float)exp(-fixed.pligst[SRFC]*lig) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww2[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCStemLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		mineralFlow = FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww2[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation 
		{
		pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( ret, 20.0f, exp_p);			//B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		} 

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
	 
		// C flow
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww2[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 

// COARSE ROOTS	
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
		pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCGrossRootLitter * kgphaTOgpm2;
	N		= pCL->fNGrossRootLitter * kgphaTOgpm2;
	dummy=C/N;

	lig		= pCh->pCProfile->fCoarseRootDecR;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
	if (C >= 10e-7) 
	{
	tcflow= C * pdecf[i] * pCh->pCProfile->fCoarseRootDecR * (float)exp(-fixed.pligst[SOIL]* lig) * dtDecompDC * anerb;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww3[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		// C flow
		pCL->fCGrossRootLitter -= tcflow * gpm2TOkgpha;
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww3[1],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByWood += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByWood +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SOIL];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
		 
		// C flow
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast  += CtoSOM1 * gpm2TOkgpha;;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww3[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByWood += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByWood +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 
	}//loop over all layer
}//if site is a forest
//_______________________________________________________________________________________
//		 VI) DecomposeSOM (dtDecomp, anerb, defac);
//_______________________________________________________________________________________

//	VI) 1. Surface SOM1 decomposes to SOM2 with CO2 loss.   
	C		= pCh->pCProfile->fCMicLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNMicLitterSurf * kgphaTOgpm2;
	Navail  = getNtoDepth(20.0f,exp_p);
if( C >= 10e-7 ) 
{
//	determine C/N ratios for flows to SOM2
	rceto2=C/N + fixed.rad1p[0] +  fixed.rad1p[1]*(C/N - fixed.pcemic[1]);
	rceto2=max(rceto2,fixed.rad1p[2]);
//  If decomposition can occur, schedule flows associated with /
//  respiration and decomposition
	if (CanDecompose (C, N , rceto2, Navail) )
	{
		// Decompose Box A to SOM2
		// -----------------------
		// Calculate C Flow
		// total C flow out of surface microbes.
		microbeC=C * defacSRFC * fixed.dec3[SRFC] * dtDecompDC;    
		co2loss = microbeC * fixed.p1co2a[SRFC];
		// cfsfs2 is C Flow from SurFace som1 to Som2
		cfsfs2 = microbeC - co2loss;

		//C flow
		pCh->pCProfile->fCMicLitterSurf -= microbeC * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(cfsfs2,20.0f, exp_p);
		
		//Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		mineralFlow = FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNMicLitterSurf -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha;
		//N flow from A to B.
		ScheduleNFlow(cfsfs2,rceto2,C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNMicLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);	 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNMicLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinBySOM += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N		 
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow	
		FlowNintoSOM2( ret, 20.0f, exp_p);			//B += minflow
		ImmBySOM +=  ret * gpm2TOkgpha;
		}
	}//if canDecompose
} // if amount is significant

 

//	VI) 2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss

	// Determine C/E ratios for flows to SOM2 and SOM3  
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	rceto2=rceto3=0.0f;
	if(getNtoDepth(20.0f, exp_p) <=0.0f ){rceto2 = fixed.varat2[0];	rceto3 = fixed.varat3[0];} //set to maximum allowed ??<=??
	else if (getNtoDepth(20.0f, exp_p) > fixed.varat2[2]){
		rceto2 = fixed.varat2[1];rceto3 = fixed.varat3[1];} //set to minimal allowed
	else // >0 and <= varat2_2
	{
		rceto2 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat2[2])*(fixed.varat2[0]-fixed.varat2[1])+fixed.varat2[1];
		rceto3 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat3[2])*(fixed.varat3[0]-fixed.varat3[1])+fixed.varat3[1];
	}
	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusFast * kgphaTOgpm2;
	N		= pCL->fNHumusFast * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; 
	if (C >= 10e-7)
	{	
	// If soil som1 can decompose to som2, it will also go to som3.
	// If it can't go to som2, it can't decompose at all.
	// If decomposition can occur
	if (CanDecompose(C,N,rceto2,Navail) )
		{
		//C Flow
		efftext = fixed.peftxa + fixed.peftxb * pSL->fSand*0.01f;
		microbeC =C * pdecf[i] * fixed.dec3[SOIL] * dtDecompDC * anerb * efftext;//cltfac=1 for no cultivation
		co2loss = microbeC * fixed.p1co2a[SOIL];
		cfs1s3 = microbeC * (fixed.ps1s3[0]+fixed.ps1s3[1]*pSL->fClay*0.01f) *
	    			(fixed.animp * (1.0f - anerb) + 1.0f); // C to SOM3 	
			// SOM2 gets what's left of microbeC.
		cfs1s2 = microbeC - co2loss - cfs1s3;
		//C flow  
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusStable  += cfs1s3 * gpm2TOkgpha;
		pCL->fCHumusSlow  += cfs1s2 * gpm2TOkgpha;
		pCL->fCHumusFast   -= microbeC * gpm2TOkgpha; 		
 		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusFast -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// to SOM3
		// N flow from A to B.
		ScheduleNFlow(cfs1s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
	    pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusStable  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		
		//to SOM2
		//N flow from A to B.
		ScheduleNFlow(cfs1s2,rceto2,C,N,Navail, &orgNflow, &minNflow);
		pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;	//B += minflow
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;	//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;		
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-7)
	} // loop over all layer


//	VI)	3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss   

	// Determine C/E ratios for flows to SOM1 and SOM3  
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	rceto1= 0.0f;rceto3= 0.0f;
	if(getNtoDepth(20.0f, exp_p) <=0.0f ){rceto1 = fixed.varat1[0];rceto3 = fixed.varat3[0];}  //set to maximum allowed  
	else if (getNtoDepth(20.0f, exp_p) > fixed.varat1[2]){ rceto1 = fixed.varat1[1]; rceto3 = fixed.varat3[1];}  //set to minimal allowed
	else // >0 and <= varat1_2
	{	rceto1 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat1[2])*(fixed.varat1[0]-fixed.varat1[1])+fixed.varat1[1];
		rceto3 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat3[2])*(fixed.varat3[0]-fixed.varat3[1])+fixed.varat3[1];
	}

	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusSlow * kgphaTOgpm2;
	N		= pCL->fNHumusSlow * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; 
	if (C >= 10e-7)
	{	
	// If soil som2 can decompose to som1, it will also go to som3.
	// If it can't go to som1, it can't decompose at all.
	// If decomposition can occur
	if (CanDecompose(C,N,rceto1,Navail) )
		{
		// C Flow
		tcflow=C * pdecf[i] * fixed.dec5 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
		co2loss = tcflow * fixed.p2co2;
		cfs2s3 = tcflow * (fixed.ps2s3[0]+fixed.ps2s3[1]*pSL->fClay*0.01f) *
	    (fixed.animp * (1.0f - anerb) + 1.0f);
		cfs2s1 = tcflow - co2loss - cfs2s3;//SOM1 gets what's left of tcflow.
		// C flow  
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusFast    += cfs2s1 * gpm2TOkgpha;
		pCL->fCHumusStable  += cfs2s3 * gpm2TOkgpha;
		pCL->fCHumusSlow    -= tcflow * gpm2TOkgpha; 		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;	
		mineralFlow = min(mineralFlow, pCL->fNHumusSlow * kgphaTOgpm2);
		pCL->fNH4N			+= mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusSlow	-= mineralFlow * gpm2TOkgpha; 
		MinBySOM			+= mineralFlow * gpm2TOkgpha;
		pCL->fMinerR		+= mineralFlow * gpm2TOkgpha ;
		//SB! Fehler in N Bilanz im Logfile, nicht in der Grafischen Ausgabe,
		//nur hier... funktioniert, wenn oberen 4 Zeilen auskommentiert sind.
		//Fehler erscheint sogar täglich, wenn die unteren beiden Zeilen auskommentiert werden ???

		//pCL->fNH4N	+= 0.1;
		//pCL->fMinerR	+= 0.1;

		if (pCL->fNO3N < 0)		Message(1,"Warning: neg. Mineral NO3N 3a");		
		if (pCL->fNH4N < 0)		Message(1,"Warning: neg. Mineral NH4N 3a");
		//to SOM3
		// N flow from A to B.
		ScheduleNFlow(cfs2s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusStable  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}

		//to SOM1
		// N flow from A to B.
		ScheduleNFlow(cfs2s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if(pTi->pSimTime->iJulianDay==245 && iday==2 && i==15)
		{
			int dummy=666;
		}

		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4   //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}		
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-7)
 
	} // loop over all layer


//	IV)	4. SOM3 decomposes to soil SOM1 with CO2 loss.

	// Determine C/E ratios for flows to SOM1 : already done
	//loop
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusStable * kgphaTOgpm2;
	N		= pCL->fNHumusStable * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //

	if (C >= 10e-7)
	{	
	// If decomposition can occur
	if (CanDecompose(C,N,rceto1,Navail) )
		{
		// Calculate C Flow
		// C flow out of metabolic into SOM1[layer]
		tcflow = C * pdecf[i] * fixed.dec4 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
		co2loss=tcflow * fixed.p3co2;
		cfs3s1 = tcflow - co2loss;
		 
		// C flow
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusFast   += cfs3s1 * gpm2TOkgpha;;
		pCL->fCHumusStable  -= tcflow * gpm2TOkgpha;;	
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow = min (mineralFlow,pCL->fNHumusStable * kgphaTOgpm2 );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusStable -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// N flow from A to B.
		ScheduleNFlow(cfs3s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusStable -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusStable -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  += (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							* (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							* (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM += (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR += (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)

	if (pCL->fNO3N < 0)		Message(1,"Warning: neg. Mineral NO3N ");		
	if (pCL->fNH4N < 0)		Message(1,"Warning: neg. Mineral NH4N ");
 
	} // loop over all layer

} //4 times a day

//*******************************************************************************
//CEAN UP HEAP
free(pdecf);
//*******************************************************************************

	
//*******************************************************************************
//	write ExpertN variables 
//	for result files and Denitrification modul
//*******************************************************************************
//write mineralisation/immobilisation by pools into first real XN layer
//values could be calculated per layer, but thats not implemented yet.
pCh->pCLayer->pNext->fHumusMinerR   = MinBySOM ;
pCh->pCLayer->pNext->fLitterMinerR  = MinByStructLitter + MinByMetabLitter+ MinByWood;	 
// fNHumusImmobR is not summed up in XN, use Day instead!!
pCh->pCProfile->fNHumusImmobDay    = ImmBySOM ;
pCh->pCLayer->pNext->fNLitterImmobR = ImmByInput + ImmByStructLitter
										 + ImmByMetabLitter+ ImmByWood;// - DailyCorrection;
//write humus and FOS für Ceres-N Denitrifikation
for(pCL=pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext ; pSL->pNext != NULL ;
												pSL=pSL->pNext,pCL=pCL->pNext) 
{
		pSL->fCHumus = pCL->fCHumusSlow + pCL->fCHumusFast + pCL->fCHumusStable;
		//like Mineralisation_Fagus: just the fine root litter is written to FOM[i], no coarse roots
		pCL->afCOrgFOMFrac[1]= pCL->fCMtbLitter + pCL->fCStrcLitter; 
}

//*******************************************************************************
// write resultfile: "century.out"
//******************************************************************************* 
//open file
//if(outf==NULL) ... exception handling
/*
if (pTi->pSimTime->fTimeAct == 0)
	outf = fopen("result/century.out", "w");	//Create file
else outf = fopen("result/century.out", "a+");	//append  

fprintf(outf, "%f ",pTi->pSimTime->fTimeAct);
//Variables
//Profile
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fNImmobR ); 
	fprintf(outf, "%f ",ImmByInput );
	fprintf(outf, "%f ",ImmBySOM );
	fprintf(outf, "%f ",ImmByStructLitter );
	fprintf(outf, "%f ",ImmByMetabLitter );
	fprintf(outf, "%f ",ImmByWood );
	//8
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fMinerR ); 
	fprintf(outf, "%f ",MinBySOM );
	fprintf(outf, "%f ",MinByStructLitter );
	fprintf(outf, "%f ",MinByMetabLitter );
	fprintf(outf, "%f ",MinByWood );
	//13ff. NNM
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fMinerR - pCh->pCLayer->pNext->fNImmobR );
	fprintf(outf, "%f ",MinBySOM - ImmBySOM);
	fprintf(outf, "%f ",MinByStructLitter -ImmByStructLitter);
	fprintf(outf, "%f ",MinByMetabLitter -ImmByMetabLitter);
	fprintf(outf, "%f ",MinByWood -ImmByWood);
	//18: CumCorrection
	fprintf(outf, "%f ",CumCorrection);
	//new line
	fprintf(outf, "\n"); 
	//close file
	fclose(outf);
*/
}// if new day
return 1;
}
//************ end of function MinerDAYCENT****************

/********************************************************************************************/
/* Procedur    :   NitriDAYCENT()                                                           */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Nitrifizierung                                                           */
/*                                                                                          */
/*             HMGU/sb                  15.03.94                                            */
/*                  ch                  20.04.95, 19.9.95                                   */
/*                                                                                          */
//	Calculates the amount of ammonium that is converted 
//	to nitrate , N2 and NO 
//	during the nitrification process.

//	steps:
//	I)		Calculate total NH4 loss
//  II)		partition to Nitrate, N2, NO 
//	III)	partition Nitrate and Ammonium to soil layers
//
//  Modell: Parton et al.: Model for NOx and N2O Emissions, Journal of Geophysical
//			Research, vol. 106, (2001)
//
//  See also DayCent reference 'NOx Submodel'
//
/*                                                                                          */
/********************************************************************************************/
/* veränd. Var.		pCL->fNH4N                                                              */
/*					pCL->fNO3N                                                              */
/*					pCL->fN2ON                                                              */
/*                  pCL->fNH4NitrR                                                          */
/*                  pCL->fNH4ToN2OR                                                         */
/********************************************************************************************/
int WINAPI NitriDAYCENT(EXP_POINTER)
{
	PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
	PWLAYER  pWL=pWa->pWLayer->pNext;	// used to iterate over the linked list
	PCLAYER  pCL=pCh->pCLayer->pNext;	// used to iterate over the linked list
	PHLAYER  pHL=pHe->pHLayer->pNext;	// used to iterate over the linked list
	PSWATER  pSW=pSo->pSWater->pNext;
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	static float ppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
	float sumppt;//rain amount of the last 2 weeks [cm]
	static float NOxPulse[12]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};//NOx pulse factor of the next 12 days 
								//index 0 is tomorrow
	//static struct parameter par;
	double NH4_to_NO3 = 0.0,ret=0.0;			// amount of NH4 converted to NO3 [kg/ha]
	double newNO3 = 0.0;			// new NO3 left after Trace gas production(gN/m^2/day)
    float const MaxNitrifRate = 0.10f;	// max fraction of ammonium that goes to NO3 during nitrification
    float const Ha_to_SqM = 0.0001f;	// factor to convert ha to sq meters
	float const min_ammonium = 0.1f;	// min. total ammonium in soil layer (kgN/ha)
    //float const min_ammonium = 0.15f;	// min. total ammonium in soil (gN/m^2)
	int i,j;
	float a, b, c, d, base1, base2, e1,e2;
//	float avgWFPS = getWFPStoDepth(20.0f, exp_p); //water filled pore space to 20cm depth
	float  WFPS;
	double fNwfps, fNsoilt, fNph;
	double base_flux, 
			diffusivity;				//	normalized diffusivity in aggregate soil media, units 0-1:
										//	ratio of gas diffusivity through soil to gas diffusivity through air
										//	at optimum water content.
	float  A[4];						// parameters to parton-innis functions
	//  fraction  N2O to NO of new NO3 that is converted 
	double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
    double NO_N2O_ratio = 0.0;  		// NO/N2O ratio <= 1.0
	double fluxN2Onit = 0.0;  			// nitrification N2O flux (kgN/ha/day)
	double fluxNOnit = 0.0;				// flux nitrified NO (kgN/ha/day)
	double const fractionN03toN2O = 0.02;  // frac of new NO3 that goes to N2O		
	double potentialFluxNOnit;			// maximum possible nitrification NO flux (gN/m^2/day)
	double NH4_to_NO = 0.0; 			// ammonium converted to NO to reach
										//    potential NO flux (kgN/ha/day)
	double NNM;
    double NH4NbeforeMiner;

if (NewDay(pTi)) //Trace gas modell is called daily
{
//***********************************************************************************
// 0) Initialize
//***********************************************************************************
pCh->pCProfile->fN2OEmisR = 0.0;
pCh->pCProfile->fNOEmisR = 0.0;
pCh->pCProfile->fN2EmisR = 0.0;

//***********************************************************************************	 
//  multiplier NOx flux based on recent rain events and snowpack
// increase of NO due to moisture and rain >= 1.0
//***********************************************************************************
//shift the daily values and add current day's ppt [cm]
for(j=0;j<13;j++)//shift the daily values and add current day's ppt [cm]
{
	ppt2lastweeks[j]=ppt2lastweeks[j+1];
}
ppt2lastweeks[13] = pCl->pWeather->fRainAmount*0.1f;
//sum of rain during last 2 weeks:
sumppt=0.0;
for(j=0;j<14;j++)//shift the daily values and add current day's ppt [cm]
{
	sumppt += ppt2lastweeks[j]; 
}
//get pulse factor for today
if(pTi->pSimTime->iJulianDay > 14)
{
kPrecipNO = NOxPulse[0];

for(j=0;j<11;j++)//shift the daily values and add factor 1.0 for the last day
{
	NOxPulse[j]=NOxPulse[j+1];
}
NOxPulse[11]=1.0;

//calculate future pulses
//new pulse, if sum is lower than 1cm and no snow
if (sumppt <= 1.0 && (pCl->pWeather->fSnow==0.0) )
{
	if(pCl->pWeather->fRainAmount*0.1f <= 0.5//low rain event
		&& pCl->pWeather->fRainAmount*0.1f > 0.1)
	{
		for(j=0;j<1;j++)//low rain event pulses last 1 day
		{
			NOxPulse[j] = max(NOxPulse[j], 11.19f *  (float)exp(-0.805f * (j+2)) );
		}
	}
		if(pCl->pWeather->fRainAmount*0.1f > 0.5//medium rain event
		&& pCl->pWeather->fRainAmount*0.1f <= 1.5)
	{
		for(j=0;j<5;j++)//medium rain event pulses last 5 days
		{
			NOxPulse[j] = max(NOxPulse[j], 14.68f * (float)exp(-0.384f * (j+2)) );
		}
	}
	if(pCl->pWeather->fRainAmount*0.1f > 1.5)//high rain event
	{
		for(j=0;j<12;j++)//large rain event pulses last 12 days
		{
			NOxPulse[j] = max(NOxPulse[j], 18.46f * (float)exp(-0.208f * (j+1)) );
		}
	}
}
}//simulated days >14
else//at the first 14 days
{
	kPrecipNO = 1.0;
}

//***********************************************************************************
// I) total NH4 loss
//***********************************************************************************

//Loop over all soil layer
for(i=0,pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pHL=pHe->pHLayer->pNext,pSW=pSo->pSWater->pNext; 
	pSL->pNext!=NULL; pCL=pCL->pNext,pSL=pSL->pNext,pWL=pWL->pNext,pHL=pHL->pNext,pSW=pSW->pNext,i++)
{
	NH4_to_NO3 = 0.0;
	// Determine texture index (COARSE,MEDIUM, FINE, VERY FINE); 
	if (pSL->fSand > 70.0 ) textureindex=COARSE;
	if (pSL->fSand <= 70.0 && pSL->fSand >= 30.0  ) textureindex=MEDIUM;
	if (pSL->fSand < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"

	if (pCL->fNH4N >= min_ammonium)
		{
		//  Compute the effect of WFPS on Nitrification (0-1)
		WFPS = pWL->fContAct / pSL->fPorosity;
        switch (textureindex)
        {
            case COARSE:	a = 0.5f;	b = 0.0f;	c = 1.5f;	d = 4.5f;	break;
            case FINE:  
            case VERYFINE:	a = 0.65f;	b = 0.0f;	c = 1.2f;	d = 2.5f;	break;
            case MEDIUM:
            default:		a = 0.65f;	b = 0.0f;	c = 1.2f;	d = 2.5f;	break;
        }
		//
        base1 =((WFPS - b) / (a - b));  
        base2 =((WFPS - c) / (a - c)); 
        e1 = d * ((b - a) / (a - c));
        e2 = d;
        fNwfps = pow (base1, e1) * pow (base2, e2);

		//  Soil temperature effect on Nitrification (0-1)		
        A[0] = pCl->pAverage->fMonthTempAmp;	// Long term avg max monthly air temp. of hottest month
        A[1] = -5.0; A[2] = 4.5; A[3] = 7.0;
        fNsoilt = f_gen_poisson_density (pHL->fSoilTemp, A);

		//  Compute pH effect on nitrification
        A[0] = 5.0f;
        A[1] = 0.56f;
        A[2] = 1.0f;
        A[3] = 0.45f;		 
        fNph = f_arctangent (pSL->fpH,A); //pH Wert 
       
		// Ammonium that goes to nitrate during nitrification.
		// base flux in Daycent program code:
        //base_flux = 0.1 * Ha_to_SqM;	// 0.1 gN/ha/day 
		//base flux from Parton et al. (2001): 20% of NNM-Ammonium
		NH4NbeforeMiner = pCL->fNH4NSoilConc / 100.0 * (pSL->fThickness*( pWL->fContAct + pSL->fBulkDens*pCh->pCParam->afKd[1]));
		NNM = pCL->fNH4N - NH4NbeforeMiner; 
		base_flux=  max(0.0, NNM * 0.2f);
		NH4_to_NO3 = (NH4NbeforeMiner) * MaxNitrifRate * fNph * fNwfps * fNsoilt + base_flux;	//[kg/ha]
		NH4_to_NO3 = max( pCL->fNH4N, NH4_to_NO3 );
		NH4_to_NO3 = max( 0.0f, NH4_to_NO3 );  

		if(pCL->fNH4N < (float)NH4_to_NO3)
		{
			Message(1,"Nitrification error: more nitrified than available");
		}
		ret +=  NH4_to_NO3;	
	}//if (ammonium >= min_ammonium)
 
//***********************************************************************************
    newNO3 = NH4_to_NO3; //[kg/ha]
	if (newNO3 > 1.0E-10)
    {   

//***********************************************************************************
// II) Partition to Nitrate, N2, NO 
//***********************************************************************************
	//Diffusivity
	//	Returns the normalized diffusivity in aggregate soil media, units 0-1:
	//	ratio of gas diffusivity through soil to gas diffusivity through air
	//	at optimum water content.		

		diffusivity=Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,WFPS);
							//  [1]        , [kg/dm3=g/cm3]´, [1],        ,[1]
	// ratio NO:N2O
		NO_N2O_ratio = 15.23 + ( 35.45 *
				atan ( 0.676 * PI * (10.0f * diffusivity - 1.86f) ) ) / PI; 
		if (iForest == 0)// true if agricultural system or grassland  				 
		{	
			NO_N2O_ratio *= 0.20;
		}
		// N2O flux
		fluxN2Onit = newNO3 * fractionN03toN2O;
		newNO3 -= fluxN2Onit;
	// maximum possible nitrification NO flux (kg/ha/day)
		potentialFluxNOnit = fluxN2Onit * NO_N2O_ratio * kPrecipNO;
	 
	if (potentialFluxNOnit <= newNO3) //NO is taken from newNO3
	{
	    fluxNOnit = potentialFluxNOnit;
	    newNO3 -= fluxNOnit;
	}
	else
	{
	    // take N out of ammonimum to get max fluxNOtotal possible
	    NH4_to_NO =  potentialFluxNOnit - newNO3 ;	 
		NH4_to_NO = min(NH4_to_NO,pCL->fNH4N); //restrain  flow
		//pCL->fNH4N -= (float)NH4_to_NO; //perform flow below
		ret += NH4_to_NO;
		fluxNOnit = newNO3 + NH4_to_NO;
	    newNO3 = 0.0;
	 }
//*********************************************************************
// Write fluxes to XN variables
//		fluxNOnit and fluxN2Onit  -  [kgN/ha/day]
//*********************************************************************	
	// 1. Subtract Ammonium: 
		pCL->fNH4N -= (float)NH4_to_NO3;
		pCL->fNH4N -= (float)NH4_to_NO;
	// 2. Add NO3
		pCL->fNO3N += (float)newNO3;
		pCL->fNH4NitrR = (float)newNO3 ;//[kg/ha/day]

	// 3. Gaseous Emissions  
		//this does work, but use is without setting fN2OEmisR in Profile
		pCL->fN2ON  += (float)fluxN2Onit;//[kg/ha]
		pCL->fNON   += (float)fluxNOnit; //[kg/ha]
     
	}//if (newNO3 > 1.0E-10)

	if (pCL->fNO3N < 0 || pCL->fNH4N < 0)
	{
		Message(1,"Error Nitrify: neg. Mineral N");
	}
}//loop over layer
}//daily

return 1;
}
//************ end of function NitriDAYCENT****************


/*********************************************************************************/
/*  Name     : DenitDAYCENT                                                      */
/*                                                                               */
/*  Funktion : Berechnung der Stickstoffverluste aufgrund denitrifizierender     */
/*             Prozesse.                                                         */
/*                                                                               */
//  Modell   : Parton et al.: Model for NOx and N2O Emissions, 
//			   Journal of Geophysical Research, vol. 106, (2001)
//
//             See also DayCent reference 'NOx Submodel'
//
/*                                                                               */
/*  Autor    : S. Bittner                                                        */
/*  Datum	 : 24.06.09                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pCh->pCLayer->fNO3N                                               */
/*                                                                               */
/*********************************************************************************/
int WINAPI DenitDAYCENT(EXP_POINTER)
{
int i,j;
PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
PCLAYER	 pCL=pCh->pCLayer->pNext;
PWLAYER  pWL=pWa->pWLayer->pNext;	 
PSWATER  pSW=pSo->pSWater->pNext;
//static struct parameter par;
static float ppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
float sumppt;//rain amount of the last 2 weeks [cm]
static float NOxPulse[12]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};//NOx pulse factor of the next 12 days 
double co2PPM[1000]; //newCO2 [ppm]
double grams_soil[1000]; //grams of soil in a layer in a 1m x 1m section
float  nitratePPM[1000];//soil nitrate (NO3-) concentration (ppm)
double fluxNTotal[1000];// soil (N2+N2O) denitrif. flux by lyr ( gN/m^2 )
double fluxTotalDenitPPM; // total (N2+N2O) denitrif. flux (ppm N/day)
double gramsSoil;   		// soil mass in layer (g m-2)
double sum=0.0;		//normalization factor for soil exp. distribution
double fluxN2Odenit[1000];	// N2O flux from denitrification (gN/m2/day)
double fluxN2denit[1000];	// N2 flux from denitrification (gN/m2/day)
double fluxNOdenit[1000];	// NO flux from denitrification (gN/m2/day)
float  porosity; 
float  wfps_fc,dD0_fc;	// water filled pore space at field capacity (0-1)
double WFPS_threshold;//  water filled pore space threshold (0-1)
double WFPS;
double co2PPM_correction;
double a;
float  fDno3;		// Nitrate effect on denitrification
double fDco2;		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
double fDwfps, M, x_inflection;		// wfps effect on denitrification (fDwfps, 0-1?)
double fRno3_co2, k1;//  Nitrate effect on the ratio of N2 to N2O
double fRwfps;		// WFPS effect on the N2/N2O Ratio
double ratioN2N2O;	// N2:N2O Ratio
double fluxN2O ;
double excess = 0.0;	// amount of N flux in excess of what
				//   can actually be removed from soil (gN/m^2)
double totalFlux;
double n2oFrac ; // N2O fraction of flux
double n2Frac  ; // N2 fraction of flux
double minNitrate_final;
double fluxOut;
double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
double potentialFluxDenitrifNO;	//   maximum possible denitrification NO flux
								//   based on NO/N2O and kPrecipNO (gN/m^2/day)
double NO_N2O_ratio;//ratio NO:N2O
//constants
	float const ug_per_gram = 1.0E6f;		// micrograms per gram (ppm)
	float const grams_per_ug = 1.0E-6f;		// grams per microgram
	// min. nitrate concentration required in a lyr for trace gas calc. (ppm N)
	double const minNitratePPM = 0.1;
	// min. allowable nitrate per lyr at end of day (ppm N)
	double const minNitratePPM_final = 0.05;
	float const cm2m2 = 10000.0f;	// cm^2 per m^2
	float A[4] =	        // parameters to parton-innis functions
		{ 9.23f, 1.556f, 76.91f, 0.00222f };
    double const tolerance = 1.0E-30;


if (NewDay(pTi))
{
	//  multiplier NOx flux based on recent rain events and snowpack
    // increase of NO due to moisture and rain >= 1.0
	//shift the daily values and add current day's ppt [cm]
	for(j=0;j<13;j++)//shift the daily values and add current day's ppt [cm]
	{
		ppt2lastweeks[j]=ppt2lastweeks[j+1];
	}
	ppt2lastweeks[13] = pCl->pWeather->fRainAmount*0.1f;
	//sum of rain during last 2 weeks:
	sumppt=0.0;
	for(j=0;j<14;j++)//shift the daily values and add current day's ppt [cm]
	{
		sumppt += ppt2lastweeks[j]; 
	}
	//get pulse factor for today
	kPrecipNO = NOxPulse[0];
	for(j=0;j<11;j++)//shift the daily values and add factor 1.0 for the last day
	{
		NOxPulse[j]=NOxPulse[j+1];
	}
	NOxPulse[11]=1.0;

	//calculate future pulses
	//new pulse, if sum is lower than 1cm and no snow
	if (sumppt <= 1.0 && (pCl->pWeather->fSnow==0.0) )
	{
		if(pCl->pWeather->fRainAmount*0.1f <= 0.5)//low rain event
		{
			for(j=0;j<1;j++)//low rain event pulses last 1 day
			{
				NOxPulse[j] = max(NOxPulse[j], 11.19f *  (float)exp(-0.805f * (j+2)) );
			}
		}
		 if(pCl->pWeather->fRainAmount*0.1f > 0.5//medium rain event
			&& pCl->pWeather->fRainAmount*0.1f <= 1.5)
		{
			for(j=0;j<5;j++)//medium rain event pulses last 5 days
			{
				NOxPulse[j] = max(NOxPulse[j], 14.68f * (float)exp(-0.384f * (j+2)) );
			}
		}
		if(pCl->pWeather->fRainAmount*0.1f > 1.5)//high rain event
		{
			for(j=0;j<12;j++)//large rain event pulses last 12 days
			{
				NOxPulse[j] = max(NOxPulse[j], 18.46f * (float)exp(-0.208f * (j+1)) );
			}
		}
	}

	//*********************************************************************
	// Convert newCO2 (g/m2) to co2PPM[] (ppm) and distrbute it
    // through the soil profile  
	//*********************************************************************
	for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext,pCL=pCL->pNext)
	{
		gramsSoil = pSL->fBulkDens  * pSL->fThickness * 0.1   * 1.0e4f;//soil mass in layer (g m-2) 
		if(gramsSoil <= 0.0) Message(1, "Denitrify Error: Devison by 0");
		co2PPM[i] = pCL->fCO2C*kgphaTOgpm2 / gramsSoil * 1.0E6f;	
		if(co2PPM[i]<1e-30) co2PPM[i]=0.0;
	}

	//*********************************************************************
    // grams of soil in a layer in a 1m x 1m section
	//*********************************************************************
	for(pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext)
	{
		grams_soil[i] = pSL->fBulkDens  * pSL->fThickness * 0.1 * cm2m2;
	}

	//*********************************************************************
	// soil nitrate (NO3-) concentration (ppm)
	//*********************************************************************
	for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2; i++,pCL=pCL->pNext)
	{
		nitratePPM[i] = pCL->fNO3N * kgphaTOgpm2 / (float)grams_soil[i] * ug_per_gram;
	}

	//********************************************************************* 
	// denitrification loop
    //  Dentrification occurs over all layers
	//*********************************************************************
    //  Convert nitrate (gN/m2) to nitratePPM (ppm N)
	for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater->pNext,i=0; 
		i<pSo->iLayers-2; 
		i++, pSL= pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,pSW=pSW->pNext)
    {
		if (nitratePPM[i] < minNitratePPM) continue;

		// normalized diffusivity in aggregate soil
		//   media, at a standard field capacity (0-1)
		//dD0_fc = diffusiv(&stdfieldc, &stdbulkd, &wfps_fc);
		//dD0 calc changed 6/20/00 -mdh
		// water filled pore space at field capacity (0-1)
		porosity = 1.0f - pSL->fBulkDens / (float)2.65;
		wfps_fc = pSW->fContFK / porosity;
		wfps_fc = max ( 0.0f, wfps_fc );
		wfps_fc = min ( wfps_fc, 1.0f );
		dD0_fc = Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,wfps_fc);
		if(dD0_fc < 0 || dD0_fc > 1.0 ) Message(1,"Warning - Denitrification: dD0_fc out of range");

		// water filled pore space threshold (0-1)	
		WFPS_threshold =
			(dD0_fc >= 0.15f) ? 0.80f : (dD0_fc * 250.0f + 43.0f) / 100.0f;
		if(WFPS_threshold < 0 || WFPS_threshold > 1.0 ) 
			Message(1,"Warning - Denitrification: WFPS_threshold out of range");
		WFPS = pWL->fContAct / pSL->fPorosity;

		// CO2 correction factor when WFPS has reached threshold
		if (WFPS <= WFPS_threshold)
		{
			co2PPM_correction = co2PPM[i];
		}
		else
		{
			a = (dD0_fc >= 0.15f) ? 0.004f : (-0.1f * dD0_fc + 0.019f);
			co2PPM_correction = co2PPM[i] * (1.0f + a * (WFPS - WFPS_threshold) * 100.0f);
		}
		if(co2PPM_correction < 0.0f) 
		{
			Message(1,"Warning - Denit.: co2PPM_correction < 0.0");
		}

		// Nitrate effect on denitrification
		// denitrification flux due to soil nitrate (ppm N/day)
		// Changed NO3 effect on denitrification based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		// fDno3 = 1.15 * std::pow(nitratePPM(layer), 0.57);
		fDno3 = max( 0.0f, f_arctangent(nitratePPM[i], A) );

		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
		//  Changed CO2 effect on denitrification based on
		//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		fDco2 = max(0.0,(0.1 * pow(co2PPM_correction, 1.3) - minNitratePPM) );

		// wfps effect on denitrification (fDwfps, 0-1?)
		// Changed wfps effect on denitrification based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		M =  min(0.113f, dD0_fc) * (-1.25) + 0.145;
		x_inflection = 9.0 - M * co2PPM_correction;
		fDwfps = max ( 0.0, 0.45 +
			(atan(0.6 *  3.14159265358979  * (10.0 * WFPS - x_inflection))) / 3.14159265358979  );

		//  N fluxes (N2 + N2O) for the current layer, ppm N
		fluxTotalDenitPPM = // total (N2+N2O) denitrif. flux (ppm N/day)
			(fDno3 < fDco2) ? fDno3 : fDco2;
		if(fluxTotalDenitPPM < 0.0)Message(1,"Warning - Denit.: fluxTotalDenitPPM < 0.0");
		// Minimum value for potential denitrification in simulation layers.
	    fluxTotalDenitPPM = max(0.066, fluxTotalDenitPPM);
		fluxTotalDenitPPM *= fDwfps;

		//  Nitrate effect on the ratio of N2 to N2O
		//  Maximum N2/N2O ratio soil respiration function
		//  Changed the NO3 and CO2 effect on the N2/N2O ratio based on
		//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		k1 = max (1.5, 38.4 - 350 * dD0_fc);
			if(k1 < 0.0) Message(1,"Warning - Denit.: k1 < 0.0f");
		fRno3_co2 = max (0.16 * k1,
				k1 * exp(-0.8 * nitratePPM[i] / co2PPM[i]) );
			if(fRno3_co2 < 0.0) Message(1,"Warning - Denit.: fRno3_co2 < 0.0");

		// WFPS effect on the N2/N2O Ratio
		// Changed wfps effect on the N2/N2O ratio based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		fRwfps = max (0.1f, 0.015f * WFPS * 100.0f - 0.32f);

		// N2:N2O Ratio
		// ratioN2N2O = (fRno3 < fRco2) ? fRno3 : fRco2;
		// ratioN2N2O *= fRwfps;
		ratioN2N2O = max(0.1, fRno3_co2 * fRwfps);

		// Compute N2 and N2O flux by layer (fluxNTotal(layer))
		// convert ppm N to gN/m^2
		fluxNTotal[i] = fluxTotalDenitPPM * grams_soil[i] * grams_per_ug;
			if(fluxNTotal[i] > 10.0)Message(1,"Warning - Denit.: fluxNTotal > 10.0");
		fluxN2O = fluxNTotal[i] / (ratioN2N2O + 1.0);
		fluxN2Odenit[i] = fluxN2O;
		fluxN2denit[i]	= max(0.0, fluxNTotal[i] - fluxN2O);
			if(fluxN2Odenit[i] < 0.0) Message(1,"Warning - Denit.: fluxN2Odenit < 0.0");
			if(fluxN2denit[i] < 0.0)  Message(1,"Warning - Denit.: fluxN2denit < 0.0");
    } // denitrification loop

	//********************************************************************* 
    //  Reduce nitrate in soil by the amount of N2-N N2O-N that is lost
    //  Do not let nitrate in any layer go below minNitratePPM_final
	//********************************************************************* 
	for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2;pCL=pCL->pNext, i++)
    {
	totalFlux = fluxN2Odenit[i] + fluxN2denit[i];
	if (totalFlux > 1.0E-30)	// have any flux?
	{
		n2oFrac = fluxN2Odenit[i] / totalFlux; // N2O fraction of flux
		n2Frac  = fluxN2denit[i]  / totalFlux; // N2 fraction of flux

	    minNitrate_final = minNitratePPM_final * grams_soil[i] * grams_per_ug;
	    if ( nitratePPM[i] < minNitratePPM )	// No flux from layer?
	    {
			excess += fluxNTotal[i];
	    }
	    else if (pCL->fNO3N*kgphaTOgpm2 - fluxNTotal[i] > minNitrate_final )
	    {
		// remove N in calculated trace gas flux from the layer
			pCL->fNO3N -= (float)fluxNTotal[i] * gpm2TOkgpha;
	    }
	    else
	    {
			// reduce trace gas flux in layer so soil N won't fall below min
			// actual total trace gas flux (gN/m^2)
			fluxOut =
				(nitratePPM[i] - minNitratePPM_final) *
				grams_soil[i] * grams_per_ug;
			excess += fluxNTotal[i] - fluxOut;
			pCL->fNO3N = (float)minNitrate_final * gpm2TOkgpha;
	    }

		fluxN2Odenit[i] -= n2oFrac * excess;
		fluxN2denit[i]  -= n2Frac * excess;
		fluxN2Odenit[i]  = max(0.0,fluxN2Odenit[i]);
		fluxN2denit[i]   = max(0.0,fluxN2denit[i]);
	}
    else //no Flux: totalFlux < 1.0E-30
    {
		fluxN2Odenit[i] = 0.0;
		fluxN2denit[i] = 0.0;
    }

    //********************************************************************* 
    // --- Partition denitratrification fluxes ---
    //********************************************************************* 
	// For denitrification, kPrecipNO is >= 1.0 -mdh 6/22/00
	// potentialFluxDenitrifNO =
	//   maximum possible denitrification NO flux
	//   based on NO/N2O and kPrecipNO (gN/m^2/day)

	//*********************************************************************
	// ratio NO:N2O and NO flux
	//*********************************************************************
	WFPS = pWL->fContAct / pSL->fPorosity;
	NO_N2O_ratio = 15.23 + ( 35.45 *
		atan ( 0.676f * 3.141592f * 
		(10.0f * Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,(float)WFPS) - 1.86f) )
		) / 3.141592f;
		if(NO_N2O_ratio < 0.0)Message(1,"Warning - Denitrification: NO_N2O_ratio < 0"); 
	if (iForest == 0)// true if agricultural system or grassland  				 
	{	
		NO_N2O_ratio *= 0.20;
	}
    potentialFluxDenitrifNO = fluxN2Odenit[i] * NO_N2O_ratio *  min (1.0, kPrecipNO);
	if(potentialFluxDenitrifNO < 0.0) Message(1,"Warning - Denit.: potentialFluxDenitrifNO < 0");

	if (potentialFluxDenitrifNO <= pCL->fNH4N * kgphaTOgpm2)//  Take all N out of NH4
	{
		fluxNOdenit[i] = potentialFluxDenitrifNO;
		pCL->fNH4N -= (float)potentialFluxDenitrifNO * gpm2TOkgpha;
	}
    else// NH4 limits; some N from N2O
    {
		// Convert all ammonium to NO
		fluxNOdenit[i] = pCL->fNH4N * kgphaTOgpm2;
		potentialFluxDenitrifNO -= pCL->fNH4N * kgphaTOgpm2;
		pCL->fNH4N = 0.0;
		// convert some N2O to NO
		if (potentialFluxDenitrifNO <= fluxN2Odenit[i])
		{
			fluxNOdenit[i]  += potentialFluxDenitrifNO;
			fluxN2Odenit[i] -= potentialFluxDenitrifNO;
		}
    }// NH4 limits; some N from N2O

	 // Check for very small values
    if(fluxNOdenit[i] < 0.0)   Message(1,"Warning - Denit.: fluxNOdenit < 0");;
    if (fluxNOdenit[i] < tolerance)
	fluxNOdenit[i] = 0.0;
    if(fluxN2Odenit[i] < 0.0f) Message(1,"Warning - Denit.: fluxN2Odenit < 0");;
    if (fluxN2Odenit[i] < tolerance)
	fluxN2Odenit[i] = 0.0f;
    if(fluxN2denit[i] < 0.0f)  Message(1,"Warning - Denit.: fluxN2denit < 0");;
    if (fluxN2denit[i] < tolerance)
	fluxN2denit[i] = 0.0f;

    //*********************************************************************
    //  Write fluxes to XN variables
    //  fluxN2Odenit[i]	-	[g/m2/day]
    //  fluxNOdenit[i]	-	[g/m2/day]
    //  fluxN2denit[i]	-	[g/m2/day]
    //*********************************************************************
    pCL->fNO3DenitR = (float)(fluxN2Odenit[i] + fluxNOdenit[i] + fluxN2denit[i]) 
							* gpm2TOkgpha; //[kg/ha/day]
	
	pCL->fN2ON += (float)fluxN2Odenit[i]* gpm2TOkgpha;//[kg/ha]
	pCL->fNON  += (float)fluxNOdenit[i] * gpm2TOkgpha;//[kg/ha]
	pCL->fN2N  += (float)fluxN2denit[i] * gpm2TOkgpha;//[kg/ha]

    }// for layer
}//new Day
return 1;
}
//************ end of function DenitDAYCENT****************
