//      mpmas_coupling.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//
#include "mpmas_coupling.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Begin of Hong
#include <string.h> // for memcpy

//End of Hong

#define NMIN_DAY  15
#define NMIN_MONTH  2


G_DEFINE_TYPE(mpmas_coupling, mpmas_coupling, EXPERTN_MODUL_BASE_TYPE);
static void mpmas_coupling_class_init(mpmas_couplingClass *klass) {}
static void mpmas_coupling_init(mpmas_coupling *self)
{
    gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!
// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL MPMAS_COUPLING!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
}
// Our Modul Functions:
int mpmas_coupling_Load(mpmas_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
	
//Begin of Hong: for turn-off module of not-currentGrid  
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;  	
	int p, id, Module_len,i;
    id = xpn->pXSys->id;
    Module_len= (int)xpn_class[id]->XPN_Moduls_len;
//End of Hong

    self->mpmas_to_xn = (STRUCT_mpmas_to_xn*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"p_grid_mpmas_to_xn");
    self->xn_to_mpmas = (STRUCT_xn_to_mpmas*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"p_grid_xn_to_mpmas");
    self->xn_to_mpmas2 = (STRUCT_xn_to_mpmas2*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"p_grid_xn_to_mpmas2");
        
//Begin of Hong: for turn-off module
    self->previousGrid=self->mpmas_to_xn->currentGrid; //to ensure the update of currentGrid after harvest
    
	//get the original module list of each xpn: 
    self->XPN_Moduls_full= g_malloc0(sizeof(struct_module*)*Module_len);
	
	//added by Hong on 20180319
	//self->cropModel="cropmodel";//zum Test
	//self->cropModel_done=0;
	
    for (p=0;p<Module_len;p++)
        {
            self->XPN_Moduls_full[p]=g_malloc0(sizeof(struct_module));
            memcpy(self->XPN_Moduls_full[p],xpn_class[id]->XPN_Moduls[p],sizeof(struct_module));

		}
//End of Hong   
     
    //self->WRITE_mpmas_xn =(STRUCT_WRITE_mpmas_xn*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRITE_mpmas_xn");
	
	self->new_plant = 0;	
	self->new_management=0; //Added by Hong on 20180518
	self->harvest_done=0;
	self->mainCrop_done=0; //Added by Hong on 20180524
	self->lastAction_done=1; //Changed Troost 20180524 (for first year, so that new plant config can be read correctly within the grid dependent and the lastAction_done condition switches)
	self->checkSwitchDate_done=0;	
	self->harvestAdaptive=0; //Added by Hong on 20180525
	self->coverCrop_harvested = 0;//added Troost 180527
	self->coverCrop_sowDatecheck_done = 0;//added Troost 180615
	self->sowDatecheck_done = 0;//added Troost 180615
	
	self->nmin_measured = 0;
    self->simulation_days = 0;
    self->count = 0;
    self->restart = 1;

// added Troost 180527
	self->lastActionDate.day= 0;
	self->lastActionDate.month= 0;
	self->lastActionDate.year=  0;

// added Troost 180608
	for (i = 0; i < XNMPMASMINFERTSLOTS; ++i) {
		self->internal_actualMinFertDate[i].day = 0;
		self->internal_actualMinFertDate[i].month = 0;
		self->internal_actualMinFertDate[i].year = 0;
		self->internal_actualTotalN[i] = 0.0;
	}
	self->internal_actualCoverCropSowDate.day = 0;
	self->internal_actualCoverCropSowDate.month = 0;
	self->internal_actualCoverCropSowDate.year = 0;
	
	self->internal_actualSowDate.day = 0;
	self->internal_actualSowDate.month = 0;
	self->internal_actualSowDate.year = 0;
	
/*  removed Troost 180527	
	//Begin of Hong: to record the date of last action of each management; if there is no action, date remains 0 
	self->lastMinFertilDate.day= 0; //added by Hong on 20180524
	self->lastMinFertilDate.month= 0;
	self->lastMinFertilDate.year= 0;
	
	self->lastOrgFertilDate.day= 0; 
	self->lastOrgFertilDate.month= 0;
	self->lastOrgFertilDate.year= 0;
	
	self->lastIrrigationDate.day= 0; 
	self->lastIrrigationDate.month= 0;
	self->lastIrrigationDate.year= 0;
	
	self->lastTillageDate.day= 0;
	self->lastTillageDate.month= 0;
	self->lastTillageDate.year= 0;	
	*/
	self->stopDate.year= 0;
	self->stopDate.month=0;
	self->stopDate.day=0;
		
    if ((self->mpmas_to_xn==NULL) || (self->xn_to_mpmas==NULL))
        {
            PRINT_ERROR("No p_mpmas_to_xn");
        }   
    return RET_SUCCESS;
}
int mpmas_coupling_Austausch(mpmas_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    //Begin of Hong: turn-off module of not-currentGrid
    expertnclass** xpn_class=(expertnclass **)xpn->pXSys->xpn_classes;
    int p, id, Module_len;
    id = xpn->pXSys->id;
    Module_len= (int)xpn_class[id]->XPN_Moduls_len;
    //End of Hong
	
    PPLANT pPl = xpn->pPl;
    PGENOTYPE pGe = pPl->pGenotype;
    PTIME pTi = xpn->pTi;
    PSIMTIME pST = pTi->pSimTime;
    PMSOWINFO   pSI = xpn->pMa->pSowInfo;
    PMANAGEMENT  pMa = xpn->pMa;
    PSPROFILE pSo = xpn->pSo;
    PCHEMISTRY pCh = xpn->pCh;
    PNFERTILIZER fertil_first,fertil,fertil_act;
    PMIRRIGATION irr_first, irr, irr_act;
	PTILLAGE till_first, till, till_act; // Hong: for tillage
    
// Begin Hong: for multigrid
    PSLAYER  pSL;
    PHLAYER  pHL; 
    PCLAYER  pCL;
    PWLAYER  pWL;
    PSWATER  pSW;
    PLAYERROOT pLR;
    PCPARAM pPA;
    PCPROFILE pCP;
    
    PSPROFILE pSo2;
    PSLAYER  pSL2;
    PHLAYER  pHL2; 
    PCLAYER  pCL2;
    PWLAYER  pWL2;
    PSWATER  pSW2;
    PLAYERROOT pLR2;
    PCPARAM pPA2;
    PCPROFILE pCP2;
	
// End of Hong

    char *S, *S2;
    char *read_filename, *read_filename2;

    int i, j, start_i;
    int fertilizer_count, irrigation_count;
    int year, month, day;
    int fertil_start_year,fertil_start_mon,fertil_start_day;

    GDate* date;
    
    if ((self->mpmas_to_xn==NULL) || (self->xn_to_mpmas==NULL))
        {
            return RET_SUCCESS;
        }

    if(self->restart == 1)
        {
            self->xn_to_mpmas2->startDay = (int)pTi->pSimTime->fTimeY;
            self->restart = 0;
        }

    get_daily_air_and_soil_temperatures(self);
    
// Begin of Hong: ######################## turn-on / turn-off modules and data transfer ######################################### 
    //added by Hong on 20180319: (since GECROS and CERES/SPASS use different crop.ini) 
	self->cropModel = xpn_register_var_get_pointer(xpn->pXSys->var_list, "Config.plant.biomass growth");
     		
	//memset(stopDate, 0, sizeof (xnmpmasDate));
	if ((self->mpmas_to_xn->updateManagement==1))//Hong: only if on stopDate 
    {

	  //1. For the first year: turn-off the irrelevant modules of not-currentGrid 	
      if (pTi->pSimTime->bFirstRound==1) // only for the first year
	  {
		  if ((self->mpmas_to_xn->own_grid_number!=self->mpmas_to_xn->currentGrid)) // search for not-currentGrid
            {  
              // turn-off module
               for (p=0;p<Module_len;p++)
                   { 	   					   
					   if ((strcmp(xpn_class[id]->XPN_Moduls[p]->SubModul,"database")!=0)&&(strcmp(xpn_class[xpn->pXSys->id]->XPN_Moduls[p]->SubModul,"mpmas")!=0))// solange SubModul != ""database"&&"mpmas", soll ausgeschaltet werden
                        {
                           xpn_class[id]->XPN_Moduls[p]->run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->global_run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->mosaic_run=NULL;
                          
                        }
                    }
				PRINT_MESSAGE(xpn,1,"irrelevant modules turned off\n");// for debug	
				
				// skip reading of new plant and management:
				self->new_plant = 1;
				self->new_management = 1;
				self->mpmas_to_xn->updateManagement=0;
				pPl->pModelParam->HarvestDay=0; // to forbid the plant growth at not-currentGrid
                pPl->pModelParam->HarvestMonth=0;
                pPl->pModelParam->HarvestYear=0;
				
	        }
	  }//End of first year
      
	  //2. For the rest of years     		
	 // 2.1 In case of switching from not-currentGrid to currentGrid: 	
	 else if ((self->mpmas_to_xn->own_grid_number!=self->previousGrid)&&(self->mpmas_to_xn->own_grid_number==self->mpmas_to_xn->currentGrid)) // ownGrid != previousGrid but ==currentGrid
         {
		  // because of skipping the plant growth last year, self->new_plant won't be reset to 0 at harvest:
		  self->new_plant = 0;
		  self->new_management = 0;
		  self->lastAction_done=0;
		  //2.1.(1)turn-on module by copy of XPN_Moduls_full
          for (p=0;p<Module_len;p++)
                {                              
					memcpy(xpn_class[id]->XPN_Moduls[p],self->XPN_Moduls_full[p],sizeof(struct_module));
            
                 } 
		  PRINT_MESSAGE(xpn,1,"module turned on");// for debug	
			 
		  //2.1.(2) reset updateManagement and save the currentGrid in the previousGrid before next update on stopDate		 
          self->mpmas_to_xn->updateManagement=0;
          self->previousGrid=self->mpmas_to_xn->currentGrid;
		  
         }// End of ownGrid != previousGrid but ==currentGrid
		 
	  
	 // 2.2 In case of switching from currentGrid to not-currentGrid:   
       else if ((self->mpmas_to_xn->own_grid_number==self->previousGrid)&&(self->mpmas_to_xn->own_grid_number!=self->mpmas_to_xn->currentGrid)) // ownGrid == previousGrid but !=currentGrid
         {  
			 
		  if ((self->checkSwitchDate_done ==1)&&(xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->stopDate.year,self->stopDate.month,self->stopDate.day)>=1))
			  {
		    
		  //2.2.(1) Begin of data transfer, pass data from previousGrid to updated currentGrid            
            int numberOfGrids, currentGrid, own_grid_number,iLayer=0;
            
            numberOfGrids= self->mpmas_to_xn->number_of_grids;
            own_grid_number = self->mpmas_to_xn->own_grid_number;
            currentGrid=self->previousGrid; // if harvest date is later than 1.st. Aug., self->mpmas_to_xn->currentGrid has been updated for the coming year   
  
            for (i=0;i<numberOfGrids;i++)
            {
                if((i== self->mpmas_to_xn->currentGrid)) // search for the updated currentGrid of xpn_other_grids  
                {
                    //1. do the data transfer
                    
 // e.g.                 for(pCL=pCh->pCLayer,pSL=pSo->pSLayer,pWL=pWa->pWLayer,pSW=pSo->pSWater,iLayer=0;
 // e.g.                           iLayer<pSo->iLayers;
 // e.g.                           iLayer++, pSL= pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,pSW=pSW->pNext)
 // e.g.                           {
// e.g.                                
 // e.g.                           }
                   
                  pSo2= self->mpmas_to_xn->xpn_other_grids[currentGrid]->pSo; //from [own_grid_number] to [i]

                  // soil water 
                  for(pWL=self->mpmas_to_xn->xpn_other_grids[i]->pWa->pWLayer,pWL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pWa->pWLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pWL=pWL->pNext,pWL2=pWL2->pNext)
                            {
  
                                memcpy(pWL,pWL2,sizeof(WLAYER)-2*sizeof(PWLAYER));
                                pWL->fContInit = pWL2->fContInit;
                                //memcpy(&(pWL->fContInit),&(pWL2->fContInit),&(pWL2->fLatFlowDay)-&(pWL2->fContInit)+sizeof(double));
                                //memcpy(&(pWL->fContInit),&(pWL2->fContInit),sizeof(WLAYER)-2*sizeof(PWLAYER));
                                
                            }
                                             
                    
                 // soil temp
                 for(pHL=self->mpmas_to_xn->xpn_other_grids[i]->pHe->pHLayer,pHL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pHe->pHLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pHL=pHL->pNext,pHL2=pHL2->pNext)
                            {
                                memcpy(pHL,pHL2,sizeof(HLAYER)-2*sizeof(PHLAYER));
                                
                            }
                            
                // soil nitrogen and carbon in layers
                 for(pCL=self->mpmas_to_xn->xpn_other_grids[i]->pCh->pCLayer,pCL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pCh->pCLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pCL=pCL->pNext,pCL2=pCL2->pNext)
                            {
                                memcpy(pCL,pCL2,sizeof(CLAYER)-2*sizeof(PCLAYER));
                            }
                            
                                                        
                 // soil humus in layers
                for(pSL=self->mpmas_to_xn->xpn_other_grids[i]->pSo->pSLayer,pSL2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pSo->pSLayer,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pSL=pSL->pNext,pSL2=pSL2->pNext)
                            {
                                memcpy(&(pSL->fCHumus),&(pSL2->fCHumus),&(pSL2->fHumusCN)-&(pSL2->fCHumus)+sizeof(double));
                            }
                            
                // soil nitrogen and carbon in profile
                {     
                   pCP=self->mpmas_to_xn->xpn_other_grids[i]->pCh->pCProfile;
                   pCP2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pCh->pCProfile; 
                                 
                   memcpy(pCP,pCP2,sizeof(CPROFILE));
                                   
                }
                  
                // root density // 
                for(pLR=self->mpmas_to_xn->xpn_other_grids[i]->pPl->pRoot->pLayerRoot,pLR2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pPl->pRoot->pLayerRoot,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pLR=pLR->pNext,pLR2=pLR2->pNext)
                            {
                                memcpy(pLR,pLR2,sizeof(STLAYERROOT)-2*sizeof(PLAYERROOT));
                            }
                /*
                 // nitrogen model param
                  for(pPA=self->mpmas_to_xn->xpn_other_grids[i]->pCh->pCParam,pPA2=self->mpmas_to_xn->xpn_other_grids[currentGrid]->pCh->pCParam,iLayer=0;
                            iLayer<pSo2->iLayers;
                            iLayer++, pPA=pPA->pNext,pPA2=pPA2->pNext)
                            {
                                memcpy(pPA,pPA2,sizeof(CPARAM)-2*sizeof(PCPARAM));
                            }
                */
		
                // for debug
                 PRINT_MESSAGE(xpn,1,"data transferred from previousGrid to currentGrid");// for debug               
                 // End of debug
				 
				} //End of i=currentGrid
				
            }//End of data transfer
		   
	 		  
          //2.2.(2) turn-off module
          for (p=0;p<Module_len;p++)
          {                 
                if ((strcmp(xpn_class[id]->XPN_Moduls[p]->SubModul,"database")!=0)&&(strcmp(xpn_class[xpn->pXSys->id]->XPN_Moduls[p]->SubModul,"mpmas")!=0))// solange SubModul != ""database"&&"mpmas", soll ausgeschaltet werden
                        {
                           xpn_class[id]->XPN_Moduls[p]->run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->global_run=NULL;
                           xpn_class[id]->XPN_Moduls[p]->mosaic_run=NULL;
                          
                        }
          }
		  PRINT_MESSAGE(xpn,1,"module turned off");// for debug

		  //2.2.(3) reset updateManagement and save the currentGrid in the previousGrid before next update on stopDate
          self->mpmas_to_xn->updateManagement=0;
          self->previousGrid=self->mpmas_to_xn->currentGrid;
		  
		  self->checkSwitchDate_done=0;	//Added by Hong on 20180524
		 
		  self->new_plant = 1;
		  self->new_management = 1;
		  self->harvest_done=0;
		  pPl->pModelParam->HarvestDay=0; // to forbid the plant growth at not-currentGrid
		  pPl->pModelParam->HarvestMonth=0;
		  pPl->pModelParam->HarvestYear=0;
		  		  			 
          } //End of checkSwitchDate_done==1
         }// End of ownGrid == previousGrid but !=currentGrid  	 

        // 2.3 In case of ownGrid == previousGrid and ==currentGrid, added by Hong on 20180518 , adapted by Troost 180527
	    else if ((self->mpmas_to_xn->own_grid_number==self->previousGrid)
					&&(self->mpmas_to_xn->own_grid_number==self->mpmas_to_xn->currentGrid)) 
		{
		   if ( self->lastAction_done==1) {
			   self->new_plant = 0;//Hong added on 20180523: to read new crop  
			   self->new_management=0;
			   self->mainCrop_done=0;
			   self->lastAction_done=0;
			   self->checkSwitchDate_done=0;
			   self->coverCrop_harvested=0;
			   self->coverCrop_sowDatecheck_done = 0;//added Troost 180615
			   self->sowDatecheck_done = 0;//added Troost 180615
			   self->mpmas_to_xn->updateManagement=0;
			   PRINT_MESSAGE(xpn,1,"Recognize new management data");// for debug

			   self->previousGrid=self->mpmas_to_xn->currentGrid;	
		   }

          }
		// 2.4 In case of ownGrid != previousGrid and !=currentGrid, 
        else if ((self->mpmas_to_xn->own_grid_number!=self->previousGrid)
					&&(self->mpmas_to_xn->own_grid_number!=self->mpmas_to_xn->currentGrid)) //added Troost 180527 (better explicit check here ! to avoid else catching unintended cases)
         {
		  	//in this case, self->new_plant and self->new_management should remain the value of 1 
           self->mpmas_to_xn->updateManagement=0;
           self->previousGrid=self->mpmas_to_xn->currentGrid;
         }
         // start added Troost 180527
         else 
         { // explicit logical error catch, should never be reached, unless someone introduced an additional case in the ifs that is not covered by the other else ifs
				S  = g_strdup_printf("ERROR in mpmas_coupling.c: Unforeseen grid switch case reached!" ); 
				PRINT_ERROR(S);
				g_free(S);
		 }				
		 //end added Troost 180527
      } //End of self->mpmas_to_xn->updateManagement=1      
           
// End of Hong ############################ turn-on or turn-off module & data transfer ##########################################

//Begin of Hong: cover crop, tillage ##############################################################
if (NewDay(pTi))
	{	
		 //test Troost 20180527
		
		S = g_strdup_printf("Mpmas coupling state: UM %d, nP %d, nM %d, cCh %d, mCd %d, hd %d, lAd %d\n"		,self->mpmas_to_xn->updateManagement,self->new_plant,self->new_management,self->coverCrop_harvested ,self->mainCrop_done,self->harvest_done,self->lastAction_done);
	    PRINT_MESSAGE(xpn,4,S);// for debug
		g_free(S);
		
	   //1. read plant info:	
	   if ((self->new_plant == 0)&&(self->mpmas_to_xn->own_grid_number==self->mpmas_to_xn->currentGrid))
        {
			// memset(self->xn_to_mpmas, 0, sizeof (STRUCT_xn_to_mpmas)); // not the right place here
				self->harvestAdaptive=0;// Hong: default no adaptive!
				self->harvest_done=0;
				



				//start added Troost 180527
				//consistency checks sowing date and cover crop sowing date
				if ( (self->mpmas_to_xn->coverCropCode[0] != '\0') && (self->coverCrop_harvested==0 ) 
					&& self->coverCrop_sowDatecheck_done == 0 ) 
				{
					//calculate earliest possible sowing date - start with now
					xnmpmasDate earliestPossibleCoverCropSowDateFromNow;
								earliestPossibleCoverCropSowDateFromNow.year = 	 pTi->pSimTime->iyear;
								earliestPossibleCoverCropSowDateFromNow.month = 	 pTi->pSimTime->mon;
								earliestPossibleCoverCropSowDateFromNow.day = 	 pTi->pSimTime->mday;



					xpn_time_date_add_dt(&earliestPossibleCoverCropSowDateFromNow.year, 
							&earliestPossibleCoverCropSowDateFromNow.month, &earliestPossibleCoverCropSowDateFromNow.day, 
								1); //Since pmas_coupling is last, nothing will happen today anymore ???



					// add waiting time after tillage that is scheduled before sowing
						for ( i = 0; i < self->mpmas_to_xn->numTill; ++i) {
							if (self->mpmas_to_xn->tillage[i].typeAdaptiveTillage==adaptiveTillageBeforeCoverCrop);
							{
								xpn_time_date_add_dt(&earliestPossibleCoverCropSowDateFromNow.year, &earliestPossibleCoverCropSowDateFromNow.month, &earliestPossibleCoverCropSowDateFromNow.day, 
								self->mpmas_to_xn->tillage[i].daysBeforeAfter);
								break;
							}
						}
					//check whether the earliest possible is later than the scheduled one
					if (xpn_time_compare_date(earliestPossibleCoverCropSowDateFromNow.year,earliestPossibleCoverCropSowDateFromNow.month,
						earliestPossibleCoverCropSowDateFromNow.day ,
						self->mpmas_to_xn->coverCropSowDate.year,self->mpmas_to_xn->coverCropSowDate.month,
						self->mpmas_to_xn->coverCropSowDate.day ) >= 0 ) 
					{
						//check whether the earliest possible is later than the scheduled one + maximum delay

						xnmpmasDate maxDelayedDate = self->mpmas_to_xn->coverCropSowDate;
						xpn_time_date_add_dt(&maxDelayedDate.year, &maxDelayedDate.month, &maxDelayedDate.day, self->mpmas_to_xn->coverCropMaxSowDelay);

							if ( xpn_time_compare_date(earliestPossibleCoverCropSowDateFromNow.year, earliestPossibleCoverCropSowDateFromNow.month,
								earliestPossibleCoverCropSowDateFromNow.day,
								maxDelayedDate.year,maxDelayedDate.month,
								maxDelayedDate.day ) > 0 )
								{ //later than planned and later than max delay
							
								S  = g_strdup_printf("ERROR: sowing of new cover crop should  "
													 "be done latest by %04d-%02d-%02d,\nbut switching to new plant only now: %04d-%02d-%02d.\nCompliance with max date not possible (possibly considering tillage do be done before.)\n",
													  maxDelayedDate.year,
													  maxDelayedDate.month, 
													  maxDelayedDate.day,
													  pTi->pSimTime->iyear,
													  pTi->pSimTime->mon,
													  pTi->pSimTime->mday);
								PRINT_ERROR(S);
								g_free(S);		
								
								self->new_plant = 0;
								self->coverCrop_harvested=1;
								self->internal_actualCoverCropSowDate.day = 0;
								self->internal_actualCoverCropSowDate.month = 0;
								self->internal_actualCoverCropSowDate.year = 0;

							}
							else { //later than planned, but before max delay
									//adapt the sowing date
									self->mpmas_to_xn->coverCropSowDate = earliestPossibleCoverCropSowDateFromNow;

							}
						}
						self->coverCrop_sowDatecheck_done = 1;	
				}
				
				if (self->sowDatecheck_done == 0 ) 
				{	
					//calculate earliest possible sowing date - start with now
					xnmpmasDate earliestPossibleSowDateFromNow;
								earliestPossibleSowDateFromNow.year = 	 pTi->pSimTime->iyear;
								earliestPossibleSowDateFromNow.month = 	 pTi->pSimTime->mon;
								earliestPossibleSowDateFromNow.day = 	 pTi->pSimTime->mday;


					xpn_time_date_add_dt(&earliestPossibleSowDateFromNow.year, 
							&earliestPossibleSowDateFromNow.month, &earliestPossibleSowDateFromNow.day, 
								1); //Since pmas_coupling is last, nothing will happen today anymore ???


					// add waiting time after tillage that is scheduled before sowing
					for ( i = 0; i < self->mpmas_to_xn->numTill; ++i) {
						if (self->mpmas_to_xn->tillage[i].typeAdaptiveTillage==adaptiveTillageBeforeSowing);
						{
							xpn_time_date_add_dt(&earliestPossibleSowDateFromNow.year, &earliestPossibleSowDateFromNow.month, &earliestPossibleSowDateFromNow.day, 
							self->mpmas_to_xn->tillage[i].daysBeforeAfter);
							break;
						}
					}
					//check whether the earliest possible is later than the scheduled one
					if (xpn_time_compare_date(earliestPossibleSowDateFromNow.year,earliestPossibleSowDateFromNow.month,
						earliestPossibleSowDateFromNow.day ,
						self->mpmas_to_xn->sowDate.year,self->mpmas_to_xn->sowDate.month,
						self->mpmas_to_xn->sowDate.day )> 0 )
					{
						//check whether the earliest possible is later than the scheduled one + maximum delay

						xnmpmasDate maxDelayedDate = self->mpmas_to_xn->sowDate;
						xpn_time_date_add_dt(&maxDelayedDate.year, &maxDelayedDate.month, &maxDelayedDate.day, self->mpmas_to_xn->maxSowDelay);

						if ( xpn_time_compare_date(earliestPossibleSowDateFromNow.year,earliestPossibleSowDateFromNow.month,
							earliestPossibleSowDateFromNow.day,
							maxDelayedDate.year,maxDelayedDate.month,
							maxDelayedDate.day )> 0 )
						{ //later than planned and later than max delay

							S  = g_strdup_printf("ERROR: sowing of new crop should  "
												 "been done latest by %04d-%02d-%02d,\nbut switching to new plant only now: %04d-%02d-%02d.\nCompliance with max date not possible (possibly considering tillage do be done before.)\n",
												  maxDelayedDate.year,
												  maxDelayedDate.month, 
												  maxDelayedDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday);
							PRINT_ERROR(S);
							g_free(S);		
							
							self->new_plant = 1; //set variables so that simulation can continue empty until fixed harvest date
							self->harvest_done=0;
							self->mainCrop_done=1;		
							pPl->pModelParam->HarvestDay = self->mpmas_to_xn->harvestDate.day;
							pPl->pModelParam->HarvestMonth = self->mpmas_to_xn->harvestDate.month;
							pPl->pModelParam->HarvestYear = self->mpmas_to_xn->harvestDate.year;
							self->harvestAdaptive = 0;
	
							self->internal_actualSowDate.day = 0;
							self->internal_actualSowDate.month = 0;
							self->internal_actualSowDate.year = 0;
							
						}
						else { //later than planned, but before max delay
							//adapt the sowing date
							self->mpmas_to_xn->sowDate = earliestPossibleSowDateFromNow;
						}
					}
					self->sowDatecheck_done = 1;	

				}	   
			    //end added Troost 180527

			        
			    if (self->mpmas_to_xn->coverCropCode[0] == '\0') {
					 self->coverCrop_harvested= 1;
				 	 self->internal_actualCoverCropSowDate.day = 0;
				     self->internal_actualCoverCropSowDate.month = 0;
				     self->internal_actualCoverCropSowDate.year = 0;
			    }    
			      //1.1 read cover crop info if any
				if ((self->mpmas_to_xn->coverCropCode[0] != '\0') && self->coverCrop_harvested==0 &&(xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->mpmas_to_xn->coverCropSowDate.year,self->mpmas_to_xn->coverCropSowDate.month,self->mpmas_to_xn->coverCropSowDate.day)<=0))         
				{
                    PRINT_MESSAGE(xpn,1,"read new cover crop info");
					
					self->new_plant = 1;
					self->daysSinceBBCH1 = 0;
					self->daysSinceBBCH2 = 0;
										
		pPl->pGenotype->acCropCode = self->mpmas_to_xn->coverCropCode;
                    pPl->pGenotype->acCropName = self->mpmas_to_xn->coverCropName;
                    pSI->Day = self->mpmas_to_xn->coverCropSowDate.day;
                    pSI->Month = self->mpmas_to_xn->coverCropSowDate.month;
                    pSI->Year = self->mpmas_to_xn->coverCropSowDate.year;
                    pPl->pGenotype->acVarietyName = self->mpmas_to_xn->coverCropVariety;
                    pSI->fPlantDens = self->mpmas_to_xn->coverCropSowDens;
                    pSI->fRowWidth = self->mpmas_to_xn->coverCropRowDist;
                    pSI->fSowDepth = self->mpmas_to_xn->coverCropSowDepth;
           
	         //pPl->pGenotype->fPlantHeight = (strcmp(self->mpmas_to_xn->coverCropCode, "MZ") == 0) ? 300 : 150;
                    
                    //adapt cover crop ploughing to potentially shifted sowing date:
                    
                    xnmpmasDate latestPossibleCoverCropPloughing;
                    latestPossibleCoverCropPloughing.day = self->mpmas_to_xn->sowDate.day;
                    latestPossibleCoverCropPloughing.month = self->mpmas_to_xn->sowDate.month;
                    latestPossibleCoverCropPloughing.year = self->mpmas_to_xn->sowDate.year;

					for ( i = 0; i < self->mpmas_to_xn->numTill; ++i) {
						if (self->mpmas_to_xn->tillage[i].typeAdaptiveTillage==adaptiveTillageBeforeSowing);
						{
							xpn_time_date_add_dt(&latestPossibleCoverCropPloughing.year, &latestPossibleCoverCropPloughing.month, &latestPossibleCoverCropPloughing.day, 
							-self->mpmas_to_xn->tillage[i].daysBeforeAfter);
							break;
						}
					}
                    
					xpn_time_date_add_dt(&latestPossibleCoverCropPloughing.year, 
							&latestPossibleCoverCropPloughing.month, &latestPossibleCoverCropPloughing.day, 
								-10); 

                   if (xpn_time_compare_date(self->mpmas_to_xn->coverCropPloughUnderDate.year,self->mpmas_to_xn->coverCropPloughUnderDate.month,
							self->mpmas_to_xn->coverCropPloughUnderDate.day,
							latestPossibleCoverCropPloughing.year,latestPossibleCoverCropPloughing.month,
							latestPossibleCoverCropPloughing.day) > 0 )
					{
							pPl->pModelParam->HarvestYear =  latestPossibleCoverCropPloughing.year; 
							pPl->pModelParam->HarvestMonth = latestPossibleCoverCropPloughing.month;
							pPl->pModelParam->HarvestDay =    latestPossibleCoverCropPloughing.day;
                    
							//check if tillage was scheduled on the cover crop sowing day and if yes, move it, too
							
							for ( i = 0; i < self->mpmas_to_xn->numTill; ++i) {
								if (xpn_time_compare_date(self->mpmas_to_xn->coverCropPloughUnderDate.year,self->mpmas_to_xn->coverCropPloughUnderDate.month,
									self->mpmas_to_xn->coverCropPloughUnderDate.day,  
									self->mpmas_to_xn->tillage[i].tillDate.year, self->mpmas_to_xn->tillage[i].tillDate.month, 
									self->mpmas_to_xn->tillage[i].tillDate.day) == 0 )
								{
									self->mpmas_to_xn->tillage[i].tillDate.year = latestPossibleCoverCropPloughing.year;
									self->mpmas_to_xn->tillage[i].tillDate.month  = latestPossibleCoverCropPloughing.month;
									self->mpmas_to_xn->tillage[i].tillDate.day =    latestPossibleCoverCropPloughing.day;
									
									self->tillage[i] = self->mpmas_to_xn->tillage[i];
									
								}
							}
                    
					}
					else {
							pPl->pModelParam->HarvestDay = self->mpmas_to_xn->coverCropPloughUnderDate.day;
							pPl->pModelParam->HarvestMonth = self->mpmas_to_xn->coverCropPloughUnderDate.month;
							pPl->pModelParam->HarvestYear = self->mpmas_to_xn->coverCropPloughUnderDate.year;
						
						
					}
					
                    
                    

                    
 
					
					self->internal_actualCoverCropSowDate = self->mpmas_to_xn->coverCropSowDate;		
					
					pPl->pModelParam->cResidueCarryOff = 0; //residuals of coverCrop liegen lassen

				// read cover crop ini file:
				    //1.1.1 in case of using crop model GECROS, just go on
				 
					 //1.1.2 in case of using crop model CERES/SPASS, crop ini file has to be reloaded. 	
				
					if(strcmp(self->cropModel,"GECROS BiomassGrowth")!=0) //added by Hong on 20180319
					{ 
						S  = g_strdup_printf("Config.ceres.%s", pPl->pGenotype->acCropName);
						read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
						g_free(S);
						if (read_filename==NULL)
							{
								S  = g_strdup_printf("Global_Config.options.%s", pPl->pGenotype->acCropName);
								read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
								g_free(S);
								if (read_filename==NULL)

									{
										S  = g_strdup_printf("Entry 'ceres.%s' or 'global.%s' is missing in your options!",pPl->pGenotype->acCropName,pPl->pGenotype->acCropName);
										PRINT_ERROR(S);
										g_free(S);
									}
							}
						if (read_filename!=NULL)
							{
								//Read plant parameters
								S2 = expertn_modul_base_replace_std_templates(xpn,read_filename);
								if (S2!=NULL)
									{
										read_filename = get_fullpath_from_relative(xpn->pXSys->base_path, S2);
										if (expertn_modul_base_GenotypeRead(xpn,pPl,read_filename)!=0)
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
			   } //end new configuration necessary for coverCrop
			
			// 1.2 read crop info (sometimes after tillage of cover crop residues)
              else if (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
						self->mpmas_to_xn->sowDate.year,self->mpmas_to_xn->sowDate.month,self->mpmas_to_xn->sowDate.day)<=0)
                {			
					PRINT_MESSAGE(xpn,1,"read crop management");
					
					self->new_plant = 1;
					self->harvest_done=0;
					self->mainCrop_done=1;//Added by Hong on 20180524
					self->coverCrop_harvested=1;//to avoid getting trapped in nirvana with mainCrop_done = 1 and coverCrop_harvested == 0
					self->daysSinceBBCH1 = 0;
					self->daysSinceBBCH2 = 0;										
		
					pPl->pGenotype->acCropCode = self->mpmas_to_xn->CropCode;
                    pPl->pGenotype->acCropName = self->mpmas_to_xn->CropName;
                    pSI->Day = self->mpmas_to_xn->sowDate.day;
                    pSI->Month = self->mpmas_to_xn->sowDate.month;
                    pSI->Year = self->mpmas_to_xn->sowDate.year;
                    pPl->pGenotype->acVarietyName = self->mpmas_to_xn->variety;
                    pSI->fPlantDens = self->mpmas_to_xn->sowDens;
                    pSI->fRowWidth = self->mpmas_to_xn->rowDist;
                    pSI->fSowDepth = self->mpmas_to_xn->sowDepth;
                    pPl->pModelParam->HarvestDay = self->mpmas_to_xn->harvestDate.day;
                    pPl->pModelParam->HarvestMonth = self->mpmas_to_xn->harvestDate.month;
                    pPl->pModelParam->HarvestYear = self->mpmas_to_xn->harvestDate.year;
					self->harvestAdaptive = self->mpmas_to_xn->harvestAdaptive;
					self->harvestBBCH1 = self->mpmas_to_xn->harvestBBCH1;	
					self->harvestBBCH1ExtraDays = self->mpmas_to_xn->harvestBBCH1ExtraDays;
					self->harvestBBCH2 = self->mpmas_to_xn->harvestBBCH2;
					self->harvestBBCH2ExtraDays = self->mpmas_to_xn->harvestBBCH2ExtraDays;
					self->internal_actualSowDate = 			self->mpmas_to_xn->sowDate;
				
					 pPl->pModelParam->cResidueCarryOff = self->mpmas_to_xn->biom_remove;

     //               pPl->pGenotype->fPlantHeight = (strcmp(self->mpmas_to_xn->coverCropCode, "MZ") == 0) ? 300 : 150;


                    // maybe needs to read the plant model
		
					// read plant ini file:
					 //1.1.1 in case of using crop model GECROS, just go on
				 
					 //1.1.2 in case of using crop model CERES/SPASS, crop ini file has to be reloaded. 	      
					if(strcmp(self->cropModel,"GECROS BiomassGrowth")!=0) //added by Hong on 20180319
					{ 
						S  = g_strdup_printf("Config.ceres.%s", pPl->pGenotype->acCropName);
						read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
						g_free(S);
						if (read_filename==NULL)
							{
								S  = g_strdup_printf("Global_Config.options.%s", pPl->pGenotype->acCropName);
								read_filename = xpn_register_var_get_pointer(xpn->pXSys->var_list,S);
								g_free(S);
								if (read_filename==NULL)
									{
										S  = g_strdup_printf("Entry 'ceres.%s' or 'global.%s' is missing in your options!",pPl->pGenotype->acCropName,pPl->pGenotype->acCropName);
										PRINT_ERROR(S);
										g_free(S);
									}
							}
						if (read_filename!=NULL)
							{
								//Read plant parameters
								S2 = expertn_modul_base_replace_std_templates(xpn,read_filename);
								if (S2!=NULL)
									{
										read_filename = get_fullpath_from_relative(xpn->pXSys->base_path, S2);
										if (expertn_modul_base_GenotypeRead(xpn,pPl,read_filename)!=0)
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
			}//end new configuration necessary of crop	
																			
		} //end self->new_plant = 0;
			
		// 2. read management info:
	  if ((self->new_management==0)&&(self->mpmas_to_xn->own_grid_number==self->mpmas_to_xn->currentGrid))
		{
			        self->new_management=1;
				   PRINT_MESSAGE(xpn,1,"reached new_management==0");// for debug
	
					self->nmin_measured = 0;

					self->nextMinFertAction = 0;
					self->nextOrgFertAction = 0;
					self->nextIrrigation = 0; 
				    self->nextTillage = 0;	
				
					
					self->numIrrig = self->mpmas_to_xn->numIrrig;
					self->numMinFert = self->mpmas_to_xn->numMinFert;
					self->numOrgFert = self->mpmas_to_xn->numOrgFert;
					self->numTill = self->mpmas_to_xn->numTill;
					
					int i;
					for (i = 0; i < self->mpmas_to_xn->numMinFert; ++i)
						self->mineralFertilization[i] = self->mpmas_to_xn->mineralFertilization[i];
						
					for ( i = 0; i < self->mpmas_to_xn->numOrgFert; ++i)	
						self->organicFertilization[i] = self->mpmas_to_xn->organicFertilization[i];
						
					for ( i = 0; i < self->mpmas_to_xn->numIrrig; ++i)
						self->irrigation[i] = self->mpmas_to_xn->irrigation[i];
						
					for ( i = 0; i < self->mpmas_to_xn->numTill; ++i)
						self->tillage[i] = self->mpmas_to_xn->tillage[i];	
						
		// added Troost 180527
		//consistency check: warn if at reading of management action should already have been done	
					if (self->numMinFert > 0 && self->mineralFertilization[0].adaptive == 0						
							&& xpn_time_compare_date(
												  self->mineralFertilization[self->numMinFert-1].fertDate.year,
												  self->mineralFertilization[self->numMinFert-1].fertDate.month, 
												  self->mineralFertilization[self->numMinFert-1].fertDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday 						  
								  ) < 0 )
					{
							S  = g_strdup_printf("ERROR: first scheduled mineral fertilization action of new crop should have "
												 "been done already %04d-%02d-%02d,\nbut switching to new management only now: %04d-%02d-%02d.\n",
												 self->mineralFertilization[self->numMinFert-1].fertDate.year,
												  self->mineralFertilization[self->numMinFert-1].fertDate.month, 
												  self->mineralFertilization[self->numMinFert-1].fertDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday);
							PRINT_ERROR(S);
							g_free(S);	
									
					}		
					if (self->numOrgFert > 0 && self->organicFertilization[0].adaptive == 0						
							&& xpn_time_compare_date(
												  self->organicFertilization[self->numOrgFert-1].orgfertDate.year,
												  self->organicFertilization[self->numOrgFert-1].orgfertDate.month, 
												  self->organicFertilization[self->numOrgFert-1].orgfertDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday 						  
								  ) < 0 )
					{
							S  = g_strdup_printf("ERROR: first scheduled organic fertilization action of new crop should have "
												 "been done already %04d-%02d-%02d,\nbut switching to new management only now: %04d-%02d-%02d.\n",
												 self->organicFertilization[self->numOrgFert-1].orgfertDate.year,
												  self->organicFertilization[self->numOrgFert-1].orgfertDate.month, 
												  self->organicFertilization[self->numOrgFert-1].orgfertDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday);
							PRINT_ERROR(S);
							g_free(S);	
									
					}	
					if (self->numIrrig > 0 					
							&& xpn_time_compare_date(
												  self->irrigation[self->numIrrig-1].irrDate.year,
												  self->irrigation[self->numIrrig-1].irrDate.month, 
												  self->irrigation[self->numIrrig-1].irrDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday 						  
								  ) < 0 )
					{
							S  = g_strdup_printf("ERROR: first scheduled irrigation action of new crop should have "
												 "been done already %04d-%02d-%02d,\nbut switching to new management only now: %04d-%02d-%02d.\n",
												 self->irrigation[self->numOrgFert-1].irrDate.year,
												  self->irrigation[self->numOrgFert-1].irrDate.month, 
												  self->irrigation[self->numOrgFert-1].irrDate.day,
												  pTi->pSimTime->iyear,
												  pTi->pSimTime->mon,
												  pTi->pSimTime->mday);
							PRINT_ERROR(S);
							g_free(S);	
									
					}	
					if (self->numTill > 0 ) 
					{ 
						if( self->tillage[0].typeAdaptiveTillage==adaptiveTillageNotAdaptive 
								&& xpn_time_compare_date(
													  self->tillage[self->numTill-1].tillDate.year,
													  self->tillage[self->numTill-1].tillDate.month, 
													  self->tillage[self->numTill-1].tillDate.day,
													  pTi->pSimTime->iyear,
													  pTi->pSimTime->mon,
													  pTi->pSimTime->mday 						  
									  ) < 0 )
						{
								S  = g_strdup_printf("ERROR: first scheduled tillage action of new crop should have "
													 "been done already %04d-%02d-%02d,\nbut switching to new management only now: %04d-%02d-%02d.\n",
													 self->tillage[self->numTill-1].tillDate.year,
													  self->tillage[self->numTill-1].tillDate.month, 
													  self->tillage[self->numTill-1].tillDate.day,
													  pTi->pSimTime->iyear,
													  pTi->pSimTime->mon,
													  pTi->pSimTime->mday);
								PRINT_ERROR(S);
								g_free(S);	
										
						}	
						else if (self->tillage[0].typeAdaptiveTillage==adaptiveTillageBeforeSowing)
						{
							int daysBeforeAfter;
							int currentDay, currentMonth, currentYear;
							xnmpmasDate adaptiveTillageDate;
								
							daysBeforeAfter = self->tillage[0].daysBeforeAfter;		
							adaptiveTillageDate.year = self->mpmas_to_xn->sowDate.year;
							adaptiveTillageDate.month = self->mpmas_to_xn->sowDate.month;
							adaptiveTillageDate.day = self->mpmas_to_xn->sowDate.day;
							xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, -daysBeforeAfter);
							
							if ( xpn_time_compare_date(
													  adaptiveTillageDate.year,
													  adaptiveTillageDate.month, 
													  adaptiveTillageDate.day,
													  pTi->pSimTime->iyear,
													  pTi->pSimTime->mon,
													  pTi->pSimTime->mday 						  
									  ) < 0 )
							{
									S  = g_strdup_printf("ERROR: first scheduled tillage action of new crop should have "
														 "been done already %04d-%02d-%02d,\nbut switching to new management only now: %04d-%02d-%02d.\n",
														  adaptiveTillageDate.year,
														  adaptiveTillageDate.month, 
														  adaptiveTillageDate.day,
														  pTi->pSimTime->iyear,
														  pTi->pSimTime->mon,
														  pTi->pSimTime->mday);
									PRINT_ERROR(S);
									g_free(S);	
											
							}	
							
							
							
						}
						else if (self->tillage[0].typeAdaptiveTillage==adaptiveTillageBeforeCoverCrop)
						{
							int daysBeforeAfter;
							int currentDay, currentMonth, currentYear;
							xnmpmasDate adaptiveTillageDate;
							
							daysBeforeAfter = self->tillage[0].daysBeforeAfter;		
							adaptiveTillageDate.year = self->mpmas_to_xn->coverCropSowDate.year;
							adaptiveTillageDate.month = self->mpmas_to_xn->coverCropSowDate.month;
							adaptiveTillageDate.day = self->mpmas_to_xn->coverCropSowDate.day;
							xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, -daysBeforeAfter);
							
							if ( xpn_time_compare_date(
													  adaptiveTillageDate.year,
													  adaptiveTillageDate.month, 
													  adaptiveTillageDate.day,
													  pTi->pSimTime->iyear,
													  pTi->pSimTime->mon,
													  pTi->pSimTime->mday						  
									  ) < 0 )
							{
									S  = g_strdup_printf("ERROR: first scheduled tillage action of new crop should have "
														 "been done already %04d-%02d-%02d,\nbut switching to new management only now: %04d-%02d-%02d.\n",
														  adaptiveTillageDate.year,
														  adaptiveTillageDate.month, 
														  adaptiveTillageDate.day,
														  pTi->pSimTime->iyear,
														  pTi->pSimTime->mon,
														  pTi->pSimTime->mday);
									PRINT_ERROR(S);
									g_free(S);				
							}	

						}					
					}
					//end added Troost 180527
					
					
							
		/* removed Troost 180527			
                    //Added by Hong on 20180524: to  make sure that searching for last action works even if the last action date is after harvest
					if (self->numMinFert>0)
					   {						
						self->lastMinFertilDate.year= 9999; 
						}	
					if (self->numOrgFert>0)
					   {
						self->lastOrgFertilDate.year= 9999; 
						}	
                     if (self->numOrgFert>0)
					   {
						self->lastOrgFertilDate.year= 9999; 
						}						
					if (self->numTill>0)
					   {
						self->lastTillageDate.year= 9999; 
						}
		*/													
		  }	//End of new_mamagement==0	
             
} // end NewDay		
		
//End of Hong 

    // XN to MPMAS
    //if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)==0)&&(NewDay(pTi)))
    
    if (NewDay(pTi))
    {
		
		//Troost 180608 : moved here. Nmin should not be done after harvest, but in spring
		if ( xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
						 pTi->pSimTime->iyear, NMIN_MONTH, NMIN_DAY) ==  0 && self->nmin_measured == 0)					 {
			measureNmin(self, 1);

		}
		else {
                        //measureNmin(self, 0);
		}
        if (checkIfMineralFertilization(self) == 1) 
        {
            fertil = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil_first = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil->acName = g_strdup_printf("Mineral fertilizer");
            fertil->Day = pTi->pSimTime->mday;//Out-noted by hong on 20180515
            fertil->Month = pTi->pSimTime->mon;
            fertil->Year = pTi->pSimTime->iyear;
			xpn_time_date_add_dt(&fertil->Year,&fertil->Month, &fertil->Day, 1);//added by Hong on 20180516
            fertil->fNO3N = self->mineralFertilization[self->nextMinFertAction].no3n;
            fertil->fNH4N = self->mineralFertilization[self->nextMinFertAction].nh4n;
            fertil->fUreaN = self->mineralFertilization[self->nextMinFertAction].urea;
            fertil->fTotalN = fertil->fNO3N + fertil->fNH4N + fertil->fUreaN;
            fertil->acCode = g_strdup_printf( "%s",  self->mineralFertilization[self->nextMinFertAction].code );
            
            //added Troost 180608 -- adapt fertilization amount to Nmin
            if (self->mineralFertilization[self->nextMinFertAction].nminAdapt_factor > 0.0) 
            {
            
				if (self->nmin_measured == 0) {
					measureNmin(self, 1);
					S  = g_strdup_printf("NOTE: fertilization is supposed to adapt to Nmin, but Nmin not yet measured\n  "
														 "Do early measurement of Nmin on %04d-%02d-%02d\n",									 
														  pTi->pSimTime->iyear,
														  pTi->pSimTime->mon,
														  pTi->pSimTime->mday);
					PRINT_ERROR(S);
					g_free(S);	
				}
            
				/*if ( xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
						 pTi->pSimTime->iyear, NMIN_MONTH, NMIN_DAY) >  0 )
				{*/
					double nmin_considered, nmin_bal, adaptCoef;
					
					//consider Nmin up to required depth
					nmin_considered = self->xn_to_mpmas->Nmin0_30;
					if (self->mineralFertilization[self->nextMinFertAction].nminAdapt_depth >= 60)
						nmin_considered += self->xn_to_mpmas->Nmin30_60;
					if (self->mineralFertilization[self->nextMinFertAction].nminAdapt_depth >= 90)
						nmin_considered += self->xn_to_mpmas->Nmin60_90;
					
					//calculate whether Nmin considered in original fertilization plan is matched or what is the difference
					nmin_bal = (self->mineralFertilization[self->nextMinFertAction].nminAdapt_ref - nmin_considered) * self->mineralFertilization[self->nextMinFertAction].nminAdapt_factor;
					
					//calculate coefficient to adapt all componetns of fertilizer
					 adaptCoef = (fertil->fTotalN + nmin_bal ) / fertil->fTotalN;
					
					if (adaptCoef < 0.0)
						adaptCoef = 0.0;
					
					fertil->fTotalN *= adaptCoef;
					fertil->fNO3N *= adaptCoef;
					fertil->fNH4N *= adaptCoef;
					fertil->fUreaN *= adaptCoef;
					
				/*}
				else 
				{
					S  = g_strdup_printf("ERROR: fertilization is supposed to adapt to Nmin, but Nmin not yet measured  "
														 "on %04d-%02d-%02d\n",									 
														  pTi->pSimTime->iyear,
														  pTi->pSimTime->mon,
														  pTi->pSimTime->mday);
					PRINT_ERROR(S);
					g_free(S);		
				}*/
            
			}
            //end added Troost 180608
            fertil->pNext = fertil_first;
            fertil_first->pBack = fertil;                                           
            //pMa->pNFertilizer = NULL;//Out-noted by hong on 20180515
            //pMa->pNFertilizer = fertil;
			pMa->pNFertilizer->pNext = NULL;
			pMa->pNFertilizer->pNext = fertil;//Added by Hong on 20180515
            
            //Begin of Hong: actualMinFertDate //Troost changed: internal save to avoid overriding by early fertilization after switch
            self->internal_actualMinFertDate[self->nextMinFertAction].day=pTi->pSimTime->mday;
            self->internal_actualMinFertDate[self->nextMinFertAction].month=pTi->pSimTime->mon;
            self->internal_actualMinFertDate[self->nextMinFertAction].year=pTi->pSimTime->iyear;
	    self->internal_actualTotalN[self->nextMinFertAction] = fertil->fTotalN;

            
            //End of Hong
            
			//Added by Hong on 20180524
  /*          if (self->nextMinFertAction==(self->numMinFert-1))
			   {
				   self->lastMinFertilDate.year= fertil->Year;
				   self->lastMinFertilDate.month= fertil->Month;
				   self->lastMinFertilDate.day= fertil->Day;	
			   }
			   */
            self->nextMinFertAction++;
                        
        }

        if (checkIfOrganicFertilization(self) == 1) 
        {
            fertil = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil_first = g_malloc0_n(1,sizeof(STNFERTILIZER));
            fertil->acName = g_strdup_printf("Organic fertilizer");
            fertil->Day = pTi->pSimTime->mday;
            fertil->Month = pTi->pSimTime->mon;
            fertil->Year = pTi->pSimTime->iyear;
			xpn_time_date_add_dt(&fertil->Year,&fertil->Month, &fertil->Day, 1);//added by Hong on 20180516
            fertil->fDryMatter = self->organicFertilization[self->nextOrgFertAction].drymatter;
            fertil->fNH4N = self->organicFertilization[self->nextOrgFertAction].nh4n;
            fertil->fNorgManure = self->organicFertilization[self->nextOrgFertAction].n_tot_org;
            fertil->fTotalN = fertil->fNO3N + fertil->fNH4N + fertil->fUreaN;
            fertil->acCode =  g_strdup_printf( "%s",self->organicFertilization[self->nextOrgFertAction].code);
            fertil->pNext = fertil_first;
            fertil_first->pBack = fertil;                                           
            //pMa->pNFertilizer = NULL; //Out-noted by hong on 20180515
            //pMa->pNFertilizer = fertil;
			pMa->pNFertilizer->pNext = NULL;
            pMa->pNFertilizer->pNext = fertil;//Added by Hong on 20180515
            PRINT_MESSAGE(xpn,1,"triggered organic fertilization");// for debug
	
/*			//Added by Hong on 20180524
            if (self->nextOrgFertAction==(self->numOrgFert-1))
			   {
				   self->lastOrgFertilDate.year= fertil->Year;
				   self->lastOrgFertilDate.month= fertil->Month;
				   self->lastOrgFertilDate.day= fertil->Day;	
			   }
			   */ //removed Troost 180527
            self->nextOrgFertAction++;
        }

		//start added Troost 180527
		//check for harvest of cover crop
		if ( (1 == self->new_plant)  && (self->coverCrop_harvested == 0)  && (self->mainCrop_done == 0)  && 
				(self->harvest_done == 0) && (1 == checkIfHarvest(self)) )
        {
			self->new_plant= 0;
			self->coverCrop_harvested = 1;
			PRINT_MESSAGE(xpn,1,"cover crop harvest");// for debug
		}
		//end added Troost 180527
		//check for harvest of main crop
        else if ( (1 == self->new_plant) && (self->coverCrop_harvested == 1) && (self->mainCrop_done == 1)  && (self->harvest_done == 0) && (1 == checkIfHarvest(self)) ) //adapted Troost 180527
            {
				
  
                
            self->xn_to_mpmas->fruitDryWeight = pPl->pBiomass->fFruitWeight;
            self->xn_to_mpmas->stemLeafDryWeight = pPl->pBiomass->fLeafWeight + pPl->pBiomass->fStemWeight;
            self->xn_to_mpmas->stemOnlyDryWeight = pPl->pBiomass->fStemWeight;
            //self->xn_to_mpmas->Nmin = pCh->pCBalance->fNO3NProfile;
            //pPl->pModelParam->HarvestDay=pTi->pSimTime->mday;
            //pPl->pModelParam->HarvestMonth=pTi->pSimTime->mon;
            //pPl->pModelParam->HarvestYear=pTi->pSimTime->iyear;


			
            
            //Begin of Hong: actualHarvestDate
            self->xn_to_mpmas->actualHarvestDate.day=pTi->pSimTime->mday;
            self->xn_to_mpmas->actualHarvestDate.month=pTi->pSimTime->mon;
            self->xn_to_mpmas->actualHarvestDate.year=pTi->pSimTime->iyear;
			
			pPl->pModelParam->HarvestDay=pTi->pSimTime->mday; // to forbid the plant growth at not-currentGrid
            pPl->pModelParam->HarvestMonth=pTi->pSimTime->mon; // and for tillage adaptive after harvest
            pPl->pModelParam->HarvestYear=pTi->pSimTime->iyear;
						
			//test of Hong 20180525
			//printf("actual harvestDate %d-%d-%d\n",pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay);										
			//end of test			
		
		//added Troost 180527
			//at this point we know when the last action will take place
			// the last action is either: the harvest = today
			// a fertilization or irrigation with a fixed date
			// or a tillage action marked with adaptive after harvest. Since harvest is done know, we can now calculate the actual date of that action
			// we just have to look at each to determine which is the one
			
			self->lastActionDate.day=pTi->pSimTime->mday; //base = harvest = today
			self->lastActionDate.month=pTi->pSimTime->mon; 
			self->lastActionDate.year=pTi->pSimTime->iyear;
			
			
			//update with last tillage date if later  (check tillage first because it is likely to be the latest)
			if  ( self->numTill > 0 ) 
			{ 
				if (self->tillage[self->numTill -1 ].typeAdaptiveTillage==adaptiveTillageAfterHarvest)
				{	//NOTE: careful assumes that lastActionDate stands at today still and was not updated before by any other post-harvest action!
					//add required days to last harvest
					int daysBeforeAfter = self->tillage[self->numTill -1].daysBeforeAfter;
					xpn_time_date_add_dt(&self->lastActionDate.year,&self->lastActionDate.month, &self->lastActionDate.day, daysBeforeAfter);
				}
				else if (self->tillage[self->numTill -1 ].typeAdaptiveTillage==adaptiveTillageNotAdaptive
							&& xpn_time_compare_date(
									  self->tillage[self->numTill-1].tillDate.year,
									  self->tillage[self->numTill-1].tillDate.month, 
									  self->tillage[self->numTill-1].tillDate.day,
									  self->lastActionDate.year,
									  self->lastActionDate.month,
									  self->lastActionDate.day 						  
									  )> 0 
						)
				{
					self->lastActionDate.day=self->tillage[self->numTill-1].tillDate.day; 
					self->lastActionDate.month=self->tillage[self->numTill-1].tillDate.month; 
					self->lastActionDate.year=self->tillage[self->numTill-1].tillDate.year;
				}
				//else no action needed as beforeSowing and before CroverCrop have already been done by definition
			}
			
			
			
			//update with last mineral fertilization date if later
			if  (  self->numMinFert > 0 && self->mineralFertilization[self->numMinFert-1].adaptive == 0 //no need to check adaptive since adaptive is only bbch dependent and thre is no bbch after harvest
				   && xpn_time_compare_date(
									  self->mineralFertilization[self->numMinFert-1].fertDate.year,
									  self->mineralFertilization[self->numMinFert-1].fertDate.month, 
									  self->mineralFertilization[self->numMinFert-1].fertDate.day,
									  self->lastActionDate.year,
									  self->lastActionDate.month,
									  self->lastActionDate.day 						  
									  )> 0 )
			{			
				self->lastActionDate.day=self->mineralFertilization[self->numMinFert-1].fertDate.day; 
				self->lastActionDate.month=self->mineralFertilization[self->numMinFert-1].fertDate.month; 
				self->lastActionDate.year=self->mineralFertilization[self->numMinFert-1].fertDate.year;
			}			
			//update with last organic fertilization date if later
			if  ( self->numOrgFert > 0 && self->organicFertilization[self->numOrgFert-1].adaptive == 0 //no need to check adaptive since adaptive is only bbch dependent and thre is no bbch after harvest
				   && xpn_time_compare_date(
									  self->organicFertilization[self->numOrgFert-1].orgfertDate.year,
									  self->organicFertilization[self->numOrgFert-1].orgfertDate.month, 
									  self->organicFertilization[self->numOrgFert-1].orgfertDate.day,
									  self->lastActionDate.year,
									  self->lastActionDate.month,
									  self->lastActionDate.day 						  
									  )> 0 )
			{			
				self->lastActionDate.day=self->organicFertilization[self->numOrgFert-1].orgfertDate.day; 
				self->lastActionDate.month=self->organicFertilization[self->numOrgFert-1].orgfertDate.month; 
				self->lastActionDate.year=self->organicFertilization[self->numOrgFert-1].orgfertDate.year;
			}			
				
			//update with last irrigation date if later
			if  ( self->numIrrig > 0 
				   && xpn_time_compare_date(
									  self->irrigation[self->numIrrig-1].irrDate.year,
									  self->irrigation[self->numIrrig-1].irrDate.month, 
									  self->irrigation[self->numIrrig-1].irrDate.day,
									  self->lastActionDate.year,
									  self->lastActionDate.month,
									  self->lastActionDate.day						  
									  )> 0 )
			{			
				self->lastActionDate.day=self->irrigation[self->numIrrig-1].irrDate.day; 
				self->lastActionDate.month=self->irrigation[self->numIrrig-1].irrDate.month; 
				self->lastActionDate.year=self->irrigation[self->numIrrig-1].irrDate.year;
			}	

		//end added Troost 180527	
		
		
		
		
		
			self->harvest_done=1;
		//	self->new_plant = 0; //ready for next crop //removed Troost 180527, not yet ready for new crop, only after last action date has been reached
			PRINT_MESSAGE(xpn,1,"harvest");// for debug
            //End of Hong
            
            }
			
	//Begin of Hong: tillage and irrigation 2017/01/11#########################################################
		if (checkIfIrrigation(self) == 1) 
        {
            printf("irrigation: \n");              
  
            irr = g_malloc0_n(1,sizeof(STIRRIGATION));
            irr_first = g_malloc0_n(1,sizeof(STIRRIGATION));
			till->acName = g_strdup_printf("irrigation");
			irr->Day = pTi->pSimTime->mday;
			irr->Month = pTi->pSimTime->mon;
            irr->Year = pTi->pSimTime->iyear;
			xpn_time_date_add_dt(&irr->Year,&irr->Month, &irr->Day, 1);//added by Hong on 20180516
            irr->fAmount = self->irrigation[self->nextIrrigation].quantity;
         

            irr->pNext = irr_first;
            irr_first->pBack = irr;                                           
            pMa->pIrrigation = NULL;
            pMa->pIrrigation = irr;
/*
            //Added by Hong on 20180524
            if (self->nextIrrigation==(self->numIrrig-1))
			   {
				   self->lastIrrigationDate.year= irr->Year;
				   self->lastIrrigationDate.month= irr->Month;
				   self->lastIrrigationDate.day= irr->Day;	
			   }
			   */ //removed Troost 180527
            self->nextIrrigation++;


		} //End of checkIfIrrigation
		
		if (checkIfTillage(self) == 1) 
        {
            till = g_malloc0_n(1,sizeof(STTILLAGE));
            till_first = g_malloc0_n(1,sizeof(STTILLAGE));
            till->acName = g_strdup_printf("tillage");
            till->Day = pTi->pSimTime->mday;
            till->Month = pTi->pSimTime->mon;
            till->Year = pTi->pSimTime->iyear;
			xpn_time_date_add_dt(&till->Year,&till->Month, &till->Day, 1);//added by Hong on 20180516
            till->fDepth = self->tillage[self->nextTillage].fDepth;
			//till->fEffMix = self->tillage[self->nextTillage].fEffMix;
			//till->fEffLoose = self->tillage[self->nextTillage].fEffLoose;
			//till->fEffIncorp = self->tillage[self->nextTillage].fEffIncorp;
			
            
            till->acCode =  self->tillage[self->nextTillage].code;
            till->pNext = till_first;
            till_first->pBack = till;                                           
            pMa->pTillage = NULL;
            pMa->pTillage = till;
			
   /*         //Added by Hong on 20180524
            if (self->nextTillage==(self->numTill-1))
			   {
				   self->lastTillageDate.year= till->Year;
				   self->lastTillageDate.month= till->Month;
				   self->lastTillageDate.day= till->Day;	
			   }
			    */ //removed Troost 180527
            self->nextTillage++;
			
        } // End of checkIfTillage

//start added 180527 Troost		
		
	if( self->mainCrop_done==1 && self->harvest_done == 1 && ! self->lastAction_done && !self->checkSwitchDate_done &&
		xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,
							self->lastActionDate.year,self->lastActionDate.month,self->lastActionDate.day) > 0
		) //check if the date of last action has passed (one day after that, so that action could take place
    {	

		//added Troost 180608
		//copy actual fertilization dates from internal to struct
		for (i = 0; i < XNMPMASMINFERTSLOTS; ++i) { 
			if (i < self->numMinFert) {
				self->xn_to_mpmas->actualMinFertDate[i].day = self->internal_actualMinFertDate[i].day;
				self->xn_to_mpmas->actualMinFertDate[i].month = self->internal_actualMinFertDate[i].month;
				self->xn_to_mpmas->actualMinFertDate[i].year = self->internal_actualMinFertDate[i].year;
				self->xn_to_mpmas->actualTotalFertN[i] = self->internal_actualTotalN[i];
			}
			else {
				self->xn_to_mpmas->actualMinFertDate[i].day = 0;
				self->xn_to_mpmas->actualMinFertDate[i].month = 0;
				self->xn_to_mpmas->actualMinFertDate[i].year = 0;
				self->xn_to_mpmas->actualTotalFertN[i] = 0;
			}
		}
		self->xn_to_mpmas->actualSowDate = self->internal_actualSowDate;
		self->xn_to_mpmas->actualCoverCropSowDate = self->internal_actualCoverCropSowDate;
		
	    self->lastAction_done = 1;
		self->checkSwitchDate_done=1;
		PRINT_MESSAGE(xpn, 3, "last action for old plant done\n");	
	}	
/* //start removed 180527 Troost		
       if ((self->harvest_done==1)&&(self->mainCrop_done==1)) //Added by Hong on 20180524: Check if last action
	      {
		  if((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->lastMinFertilDate.year,self->lastMinFertilDate.month,self->lastMinFertilDate.day)>=0)
	         && (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->lastOrgFertilDate.year,self->lastOrgFertilDate.month,self->lastOrgFertilDate.day)>=0)
		     && (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->lastIrrigationDate.year,self->lastIrrigationDate.month,self->lastIrrigationDate.day)>=0)
			 && (xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->lastTillageDate.year,self->lastTillageDate.month,self->lastTillageDate.day)>=0))
			 {	 
				 //PRINT_MESSAGE(xpn,1,"All crop management actions of this vegetation period are finish.");  //for debug
				self->lastAction_done =1;	
				
				//test of Hong 20180525
			printf("self->lastAction_done=1\n");	
			printf("lastTillageDate %d-%d-%d\n",self->lastTillageDate.year,self->lastTillageDate.month,self->lastTillageDate.day);									
			printf("lastIrrigationDate %d-%d-%d\n",self->lastIrrigationDate.year,self->lastIrrigationDate.month,self->lastIrrigationDate.day);
            printf("lastOrgFertilDate %d-%d-%d\n",self->lastOrgFertilDate.year,self->lastOrgFertilDate.month,self->lastOrgFertilDate.day);										
			printf("lastMinFertilDate %d-%d-%d\n",self->lastMinFertilDate.year,self->lastMinFertilDate.month,self->lastMinFertilDate.day);								
			//end of test
				
				
				 }
		 
		  }  			
	
		if(self->lastAction_done==1)
	       {
		    //only for currentGrid -> not currentGrid:
	        if ((self->checkSwitchDate_done==0)&&(self->mpmas_to_xn->own_grid_number==self->previousGrid)&&(self->mpmas_to_xn->own_grid_number!=self->mpmas_to_xn->currentGrid))	 	   
	            {		
		          self->checkSwitchDate_done=1;
	            }
	       }
	*/ //end removed 180527 Troost				
						
							  
    } //End of Newday
		
    if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,self->mpmas_to_xn->stopDate.year,self->mpmas_to_xn->stopDate.month,self->mpmas_to_xn->stopDate.day)==0)&&(NewDay(pTi)))
        {   

			self->stopDate.year= pTi->pSimTime->iyear;
			self->stopDate.month=pTi->pSimTime->mon;
			self->stopDate.day=pTi->pSimTime->mday;
			
			self->xn_to_mpmas2->stopDay = (int)pTi->pSimTime->fTimeY;
            
            self->restart = 1;
			
            return 2;
        }
        
        
        
    //if ((xpn_time_compare_date(pTi->pSimTime->iyear,pTi->pSimTime->mon,pTi->pSimTime->mday,pPl->pModelParam->HarvestYear,pPl->pModelParam->HarvestMonth,pPl->pModelParam->HarvestDay)>0)&&(NewDay(pTi)))
    //      printf("plant growth %d maturity %d \n", pPl->pDevelop->bPlantGrowth, pPl->pDevelop->bMaturity);
    //self->WRITE_mpmas_xn->mass=self->Plant_Mass;
    return RET_SUCCESS;
}

//NEW:
//----------------------------------------------------
//Determine if crop is to be harvested today
//function to be called once a day
//return codes: 0 - no, 1- yes 
//requires a counter 'daysSinceBBCH2' to be maintained within caller
int checkIfHarvest(mpmas_coupling *self) 
    {
    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
	//Begin of Hong: only the current grid needs to checkIfHarvest
  if (self->mpmas_to_xn->own_grid_number==self->previousGrid)
	{	
	
    //If the decision is not adaptive, no harvest at this day
    if ( ! (self->harvestAdaptive) ) 
    {   
            //first check fixed harvest date and return 1 if date has been reached
            //If the decision is not marked as adaptive, it is the only criterion
            //If the decision is adaptive it serves as a backstop and should be set to the latest possible harvest
        if (currentDay == pPl->pModelParam->HarvestDay
        && currentMonth == pPl->pModelParam->HarvestMonth
        && currentYear == pPl->pModelParam->HarvestYear ) 
            {
				//Hong for debug:
				PRINT_MESSAGE(xpn, 3, "fixed harvest date reached\n");
            return 1;
            }

    }
    //If the decision is adaptive ...
    else {
        //check whether maturity (BBCH1) has been reached
        if (currentBBCH >= self->harvestBBCH1 ) {
			//Hong for debug:
			PRINT_MESSAGE(xpn, 3, "BBCH1 for harvest reached\n");
            if (self->daysSinceBBCH1 >= self->harvestBBCH1ExtraDays) {
				
				//Hong for debug:
				PRINT_MESSAGE(xpn, 3, "adaptive harvest (BBCH1 + extra days reached)\n");
                return 1;
            }
            //if we are > BBCH1, but not over the limit, increase the counter of days
            else {
                self->daysSinceBBCH1 += 1;
            }
        }
        //if maturity has not been reached, but we are close (> BBCH2)
        // check the number of days for which we have been > BBCH2
        // and harvest in case we are over the limit given for that stage 
        //(this is because in some cases the actual maturity will not be reached at all)
        else if (currentBBCH >= self->harvestBBCH2 ) {
			PRINT_MESSAGE(xpn, 3, "BBCH2 for harvest reached\n");
            if (self->daysSinceBBCH2 >= self->harvestBBCH2ExtraDays) {
				
				//Hong for debug:
				//printf("defined harvest BBCH2= %d reached\n",self->harvestBBCH2);
				//printf("defined harvestBBCH2ExtraDays= %d reached\n",self->daysSinceBBCH2);
				PRINT_MESSAGE(xpn, 3, "adaptive harvest (BBCH2 + extra days reached)\n");
                return 1;
            }
            //if we are > BBCH2, but not over the limit, increase the counter of days
            else {
                self->daysSinceBBCH2 += 1;
                //no return here, to allow checking for absolute maximum harvest date
            }
        }
        if (currentDay == pPl->pModelParam->HarvestDay
			&& currentMonth == pPl->pModelParam->HarvestMonth
			&& currentYear == pPl->pModelParam->HarvestYear ) 
            {
				//Hong for debug:
				printf("maximum harvest date reached, harvesting despite BBCH not being reached\n");
				
            return 1;
            }
      }//End of the decision is adaptive
	
	} //End of ownGrid==previousGrid
		
    return 0;
}

//---------------------------------------

int checkIfTillage(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int daysBeforeAfter;
    int currentDay, currentMonth, currentYear;
	
	xnmpmasDate adaptiveTillageDate;
	
	int adaptiveDays;
    
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next tillage is a valid action
    if (self->nextTillage >= XNMPMASTILLAGESLOTS || self->nextTillage >= self->numTill ) {
        return 0;
    }
	
	
	//1. if timing of tillage is fixed, check whether indicated date has been reached
    else if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageNotAdaptive) 
	{
      if (xpn_time_compare_date(self->tillage[self->nextTillage].tillDate.year,self->tillage[self->nextTillage].tillDate.month,self->tillage[self->nextTillage].tillDate.day,currentYear,currentMonth,currentDay)==0)
	   {
		// PRINT_MESSAGE(xpn,1,"tomorrow is default TILLAGE date");  //for debug
		  return 1;
		}
      else
 	      return 0;
	}	


	//2. if timing of tillage is adaptive
	//2.1 tillage before sowing of crop

	else if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageBeforeSowing)
	  {
			    
		daysBeforeAfter = self->tillage[self->nextTillage].daysBeforeAfter;		
		adaptiveTillageDate.year = self->mpmas_to_xn->sowDate.year;
		adaptiveTillageDate.month = self->mpmas_to_xn->sowDate.month;
		adaptiveTillageDate.day = self->mpmas_to_xn->sowDate.day;
		xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, -daysBeforeAfter);
		
		//PRINT_MESSAGE(xpn,1,"should tillage before sowDate:");
		//printf("daybeforeafter: %d\n", daysBeforeAfter);
		//printf("%d-%d-%d \n", adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day );
            
		
		if (xpn_time_compare_date(currentYear,currentMonth,currentDay,adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day)==0)
	     {
		//PRINT_MESSAGE(xpn,1,"tillage");   
		//printf("on %d days before sowing\n", daysBeforeAfter); //for debug
		  return 1;
		}
	    else
 	      return 0;
		}	
	
	// 2.2 tillage before sowing of cover crop
	else if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageBeforeCoverCrop)
	  {
		
		daysBeforeAfter = self->tillage[self->nextTillage].daysBeforeAfter;
		adaptiveTillageDate.year = self->mpmas_to_xn->coverCropSowDate.year;
		adaptiveTillageDate.month = self->mpmas_to_xn->coverCropSowDate.month;
		adaptiveTillageDate.day = self->mpmas_to_xn->coverCropSowDate.day;
		xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, -daysBeforeAfter);
		
	
		if (xpn_time_compare_date(currentYear,currentMonth,currentDay,adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day)==0)
	     {
		//PRINT_MESSAGE(xpn,1,"tillage");   //for debug
		//printf("on %d days before sowing of cover crop\n", daysBeforeAfter); //for debug
		
		  return 1;
		}
	    else
 	      return 0;
		}
			
	 //2.3 tillage after harvest 
	else if (self->tillage[self->nextTillage].typeAdaptiveTillage==adaptiveTillageAfterHarvest)
	  {
		//Test of Hong 20180525
		//self->lastTillageDate.day= pPl->pModelParam->HarvestDay;
		//self->lastTillageDate.month= pPl->pModelParam->HarvestMonth;
	    //self->lastTillageDate.year= pPl->pModelParam->HarvestYear;  
		  
		if (self->harvest_done==1)
		{
		    daysBeforeAfter = self->tillage[self->nextTillage].daysBeforeAfter;
				
		    //PRINT_MESSAGE(xpn,1,"time for tillage after harvest on:"); //for debug	
		    adaptiveTillageDate.year = pPl->pModelParam->HarvestYear;
		    adaptiveTillageDate.month = pPl->pModelParam->HarvestMonth;
		    adaptiveTillageDate.day = pPl->pModelParam->HarvestDay;
		    xpn_time_date_add_dt(&adaptiveTillageDate.year,&adaptiveTillageDate.month, &adaptiveTillageDate.day, daysBeforeAfter);
				
	        if (xpn_time_compare_date(currentYear,currentMonth,currentDay,adaptiveTillageDate.year,adaptiveTillageDate.month,adaptiveTillageDate.day)==0)
	          {
		     
		         return 1;
		       }			
	  }
	    
	  }
	   
		 
    return 0; //actually never reached
}





//Determine if next mineral fertilizing event is to take place today
//function to be called once a day
//return codes: 0 - no, 1- yes
//assumes that the counter which fertilization action is next 'nextMinFertAction' is maintained by the caller
int checkIfMineralFertilization(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next fertilization action is a valid action
    if (self->nextMinFertAction >= XNMPMASMINFERTSLOTS || self->nextMinFertAction >= self->numMinFert ) {
        return 0;
    }
    //if timing of fertilization is adaptive, check whether indicated BBCH stage of MAIN crop has been reached
    else if ( self->mineralFertilization[self->nextMinFertAction].adaptive && self->mineralFertilization[self->nextMinFertAction].bbch > 0) {   
        if ( self->mainCrop_done == 1 && self->harvest_done == 0 && currentBBCH >= self->mineralFertilization[self->nextMinFertAction].bbch)
            return 1;
        else
            return 0;
    }
    //else if timing of fertilization is fixed, check whether indicated date has been reached
    else {
        if (currentDay == self->mineralFertilization[self->nextMinFertAction].fertDate.day 
            && currentMonth == self->mineralFertilization[self->nextMinFertAction].fertDate.month 
            && currentYear == self->mineralFertilization[self->nextMinFertAction].fertDate.year ) {
            return 1;
        }
        else
            return 0;
    }
    return 0; //actually never reached
}

int checkIfOrganicFertilization(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next fertilization action is a valid action
    if (self->nextOrgFertAction >= XNMPMASORGFERTSLOTS || self->nextOrgFertAction >= self->numOrgFert ) {
        return 0;
    }
    //if timing of fertilization is adaptive, check whether indicated BBCH stage has been reached
    else if ( self->organicFertilization[self->nextOrgFertAction].adaptive ) {   
        if (self->mainCrop_done == 1 && self->harvest_done == 0 && currentBBCH >= self->organicFertilization[self->nextOrgFertAction].bbch)
            return 1;
        else
            return 0;
    }
    //else if timing of fertilization is fixed, check whether indicated date has been reached
    else {
        if (currentDay == self->organicFertilization[self->nextOrgFertAction].orgfertDate.day 
            && currentMonth == self->organicFertilization[self->nextOrgFertAction].orgfertDate.month 
            && currentYear == self->organicFertilization[self->nextOrgFertAction].orgfertDate.year ) {
            return 1;
        }
        else
            return 0;
    }
    return 0; //actually never reached
}

//Begin of Hong: for tillage, irrigation 2017/01/11
//---------------------------------------
//Determine if next irrigation event is to take place today
//function to be called once a day
//return codes: 0 - no, 1- yes
//assumes that the counter which irrigation is next 'nextIrrigation' is maintained by the caller
int checkIfIrrigation(mpmas_coupling *self)
{

    expertn_modul_base *xpn = &(self->parent);
    
    PPLANT pPl = xpn->pPl;
    PTIME pTi = xpn->pTi;
    
    int currentBBCH;
    int currentDay, currentMonth, currentYear;
    
    currentBBCH = pPl->pDevelop->fDevStage;
    
    currentYear = pTi->pSimTime->iyear;
    currentMonth = pTi->pSimTime->mon;
    currentDay = pTi->pSimTime->mday;
    
    //security check to make sure the next irrigation is a valid action
    if (self->nextIrrigation >= XNMPMASIRRIGATIONSLOTS || self->nextIrrigation >= self->numIrrig ) {
        return 0;
    }
	
    //check whether indicated date has been reached
	else if (currentDay == self->irrigation[self->nextIrrigation].irrDate.day 
            && currentMonth == self->irrigation[self->nextIrrigation].irrDate.month 
            && currentYear == self->irrigation[self->nextIrrigation].irrDate.year ) {
            return 1;
        }
        
    return 0; //actually never reached
}


//End of Hong
int get_daily_air_and_soil_temperatures(mpmas_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    PTIME pTi = xpn->pTi;
    PCLIMATE pCl = xpn->pCl;
    PWEATHER pWe = pCl->pWeather;
    PHEAT pHe = xpn->pHe;
    PSPROFILE pSo = xpn->pSo;
    PWATER pWa = xpn->pWa;
    PSLAYER  pSL;
    PWLAYER  pWL;
    PHLAYER pHL;
    
    double dt;
    
    double depth, act_depth;
    
    int i;
    
    int dayofyear;

    if ((pTi->pSimTime->fTimeY==self->temperature.fTimeY_save) && (pTi->pSimTime->iyear == self->temperature.iyear_save)) {
        return RET_SUCCESS;
    } else {
        self->temperature.fTimeY_save = pTi->pSimTime->fTimeY;
        self->temperature.iyear_save = pTi->pSimTime->iyear;
    }

    dt = pTi->pTimeStep->fAct;


    if ((int)NewDay(pTi)) {
        
        dayofyear = ((int)pTi->pSimTime->fTimeY) - 1; //average written for previous day, when new day has already started (and index < 0)
        if (dayofyear == -1)
        {
            dayofyear = 365 + is_leap_year(pTi->pSimTime->iyear - 1) -1;
        }
        
        
        self->temperature.fTempMax = self->__temperature.fTempMax;
        self->temperature.fTempMin = self->__temperature.fTempMin;
        self->temperature.fTempAve = self->__temperature.fTempAve;
        self->temperature.fTempSoil5cm = self->__temperature.fTempSoil5cm;

        if(self->simulation_days >= 1)
            {
            self->xn_to_mpmas2->airTemp[dayofyear] = self->__temperature.fTempAve;
            self->xn_to_mpmas2->topsoilTemp[dayofyear] = self->__temperature.fTempSoil5cm;
            }
        
        //printf("%f %f %f %f \n", self->temperature.fTempMax, self->temperature.fTempMin, self->temperature.fTempAve, self->temperature.fTempSoil5cm );

        self->__temperature.fTempMax= -99.0;
        self->__temperature.fTempMin= 99.0;     
        self->__temperature.fTempAve= 0.0;
        self->__temperature.fTempSoil5cm = 0.0;
        self->simulation_days++;
        
	} /*else {*/ //Not else, we need to do this also when the day starts, otherwise we get a wrong average!
        
        if ((pWe->fTempAir > self->__temperature.fTempMax)&&(pTi->pSimTime->fTimeDay > 0.5)) {
            self->__temperature.fTempMax = pWe->fTempAir_daily_models;
        }
        if ((pWe->fTempAir < self->__temperature.fTempMin)&&(pTi->pSimTime->fTimeDay < 0.5)) {
            self->__temperature.fTempMin = pWe->fTempAir_daily_models;
        }

        self->__temperature.fTempAve += pWe->fTempAir_daily_models * dt;

        depth = 0.05; //m
        act_depth = 0.0; //m

        for(i=0,pSL=pSo->pSLayer->pNext,pHL=pHe->pHLayer->pNext, pWL=pWa->pWLayer->pNext; pSL->pNext!=NULL; pSL=pSL->pNext, pHL=pHL->pNext, pWL=pWL->pNext, i++)
		{
			act_depth +=pSL->fThickness*1.0e-3; // mm -> m
			if (act_depth >= depth)
			{
				self->__temperature.fTempSoil5cm += pHe->pHLayer->pNext->fSoilTemp * dt;
				break;
			}
		}


    //}

    /*self->weather.fTempAve = *(self->TairMean);
    self->weather.fTempMin = *(self->TairMin);
    self->weather.fTempMax = *(self->TairMax);
    self->weather.fDaylySolRad = *(self->fSolRad);*/
    return RET_SUCCESS;
}

void measureNmin(mpmas_coupling *self, int store_measures)
{
		    expertn_modul_base *xpn = &(self->parent);
			    gchar *S;
	
			PSPROFILE   pSo = xpn->pSo;
			PSLAYER     pSL;
			PCLAYER     pCL;

			double nmin0_30, nmin30_60, nmin60_90; // added Troost 180608

			double depth0_30, depth30_60, depth60_90;
			double actdepth;
			//double EPSILON;

			//EPSILON = 1e-9;

			nmin0_30 = 0.0;
			nmin30_60 = 0.0;
			nmin60_90 = 0.0;

			actdepth = 0.0;

			/*depth0_30 = 0.0;
			depth30_60 = 0.0;
			depth60_90 = 0.0;*/
			
			double cumulNO3N_30 = 0.0;
			double cumulNH4N_30 = 0.0;
                        double cumulNO3N_60 = 0.0;
                        double cumulNH4N_60 = 0.0;
                        double cumulNO3N_90 = 0.0;
                        double cumulNH4N_90 = 0.0;

			
					
			for (pCL = xpn->pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext;  //start
							pSL->pNext != NULL;                     //end
								pSL = pSL->pNext,pCL=pCL->pNext)          //step
			{
					actdepth += pSL->fThickness; //mm
					if (actdepth <= 300.0)
						{
						//depth0_30 += pSL->fThickness;
						nmin0_30 += (pCL->fNO3N + pCL->fNH4N) ;//* pSL->fThickness;
						cumulNO3N_30 += pCL->fNO3N;
						cumulNH4N_30 += pCL->fNH4N;
						
						}
					if ((actdepth > 300.0) && (actdepth <= 600.0))
						{
						//depth30_60 += pSL->fThickness;
						nmin30_60 += (pCL->fNO3N + pCL->fNH4N) ;// * pSL->fThickness;
                                                cumulNO3N_60 += pCL->fNO3N;
                                                cumulNH4N_60 += pCL->fNH4N;
						}
					if ((actdepth > 600.0) && (actdepth <= 900.0))
						{
						//depth60_90 += pSL->fThickness;
						nmin60_90 += (pCL->fNO3N + pCL->fNH4N) ;//* pSL->fThickness;
                                                cumulNO3N_90 += pCL->fNO3N;
                                                cumulNH4N_90 += pCL->fNH4N;
						}
					if (actdepth > 900.0)
						break;
			}

			S = g_strdup_printf("Current Nmin measures:  0-30cm NH3N: %.2f, NH4N: %.2f, NMin: %.2f\n"
					    "                       30-60cm NH3N: %.2f, NH4N: %.2f, Nmin: %.2f\n"
                                            "                       60-90cm NH3N: %.2f, NH4N: %.2f, Nmin: %.2f\n"
					    "                       Total Nmin: %.2f\n"
 
						, cumulNO3N_30, cumulNH4N_30, nmin0_30, cumulNO3N_60, cumulNH4N_60, nmin30_60
						, cumulNO3N_90, cumulNH4N_90, nmin60_90, nmin0_30 +nmin30_60 + nmin60_90
					);
			PRINT_MESSAGE(xpn,3,S);// for debug
			g_free(S);

	    if (store_measures > 0) {
	            self->xn_to_mpmas->Nmin0_30 = nmin0_30  ;// / (depth0_30+EPSILON);
        	    self->xn_to_mpmas->Nmin30_60 = nmin30_60; // / (depth30_60+EPSILON);
            	    self->xn_to_mpmas->Nmin60_90 = nmin60_90 ; // / (depth60_90+EPSILON);
	    
			self->nmin_measured = 1;
            }
}


//END NEW
