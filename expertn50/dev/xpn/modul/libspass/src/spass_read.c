//      spass_read.c
//      
//      Copyright 2013 Florian Heinlein <florian.heinlein@inbox.lt>
//  

#include "spass_read.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


GDate *spass_convert_str_to_gdate(char* S) {
	char yearstr[5],monthstr[3],daystr[3];
	int i;
	int day,month,year;
	GDate *test;

	if ((strlen(S) != 10)||(S==NULL))
		return ((GDate *)g_date_new_dmy((GDateMonth)01,(GDateMonth)01,(GDateMonth)9999));		

	for (i=0; i<4; i++) {
		yearstr[i] = S[i];
	}
	yearstr[4]=0;
	monthstr[0]=S[5];
	monthstr[1]=S[6];
	monthstr[2]=0;
	daystr[0]=S[8];
	daystr[1]=S[9];
	daystr[2]=0;

	day = atoi(daystr);
	month = atoi(monthstr);
	year = atoi(yearstr);
	test = (GDate*)g_malloc(sizeof(GDate));
	test = ((GDate *)g_date_new_dmy((GDateMonth)day,(GDateMonth)month,(GDateMonth)year));
	if (g_date_valid(test) == TRUE)
		return test;
	else
		return ((GDate *)g_date_new_dmy((GDateMonth)01,(GDateMonth)01,(GDateMonth)9999));
	
}


int init_spass_model(spass *self)
{
	expertn_modul_base *xpn = &(self->parent);
	char *ini_filename;
	
	PTIMESTEP		pTS = xpn->pTi->pTimeStep;
	//PNFERTILIZER fertil_first,fertil,fertil_act;

	// Wenn die Init bereits geladen ist
	if (self->__INIT_DONE==0) {
		self->__INIT_DONE=1;
	} else {
		return RET_SUCCESS;
	}
	
	
	// Read from INI File:
	ini_filename = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"Config.spass.filename");

	if (ini_filename==NULL) {
		PRINT_ERROR("Missing entry 'spass.filename' in your configuration!");
	} else {

		char *S2;
		S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
		if (S2!=NULL) {
			ini_filename = get_fullpath_from_relative(self->parent.pXSys->base_path, S2);
			if (expertn_modul_base_PlantManagementRead(xpn,ini_filename,"spass")!=0) {
				PRINT_ERROR("Error Read SPASS INI File");
			}
			self->ini_filename = g_strdup_printf("%s",ini_filename);
			free(ini_filename);
			free(S2);
		}

	}

	// Initialize plant variables
	//spass_PlantVariableInitiation(self);

	self->fTemp_zwischen = g_malloc0_n((int)(1.0/pTS->fAct),sizeof(double));
	self->fAveGrossPs_zwischen = g_malloc0_n((int)(1.0/pTS->fAct),sizeof(double));
	self->fSolRad_zwischen = g_malloc0_n((int)(1.0/pTS->fAct),sizeof(double));
	self->afActLayRootWatUpt = g_malloc0_n((int)xpn->pSo->iLayers - 1,sizeof(double));

	//self->par = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"expertn_database.par");
	//self->global_radiation = xpn_register_var_get_pointer(self->parent.pXSys->var_list,"expertn_database.global_radiation");
	
	self->spass_AssimilatePartition_xtuber = 0.0;
	self->spass_ReserveTranslocationfTotalReservWeihgt=0.0;
	self->spass_COMVR_TO_ZSTAGE_AND_CERES_WHEATnode=0.0;
	self->spass_DailyCanopyGrossPhotosythesisGaussian=0;
	self->spass_DailyCanopyGrossPhotosythesisPARCum=0.0;
    
    spass_init_agmip_vars(self);

	// Read  if harvest at fStageSUCROS == 2
	self->harvest_at_maturity = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.Wang (SPASS).harvest_at_maturity",0);
	if(self->harvest_at_maturity == 1)
	{
	char *S;
	S  = g_strdup_printf("Harvest takes place at maturity. If maturity is not reached, harvest takes place two days before next sowing or two days before the simulation ends.");
	PRINT_MESSAGE(xpn,3,S);
	g_free(S);
	}
    
    self->set_LAI_to_0_after_maturity = xpn_register_var_get_pointer_convert_to_int(xpn->pXSys->var_list,"Config.Wang (SPASS).set_LAI_to_0_after_maturity",0);

	return RET_SUCCESS;
}



int init_spass_interception(spass *self)
{
	self->fInterceptionCapacity = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Interception (referred to Wang).Interception_Capacity",0.25);
	
	return RET_SUCCESS;
}



//////////////////////////////////////////////////////////////////////////////////////////////
// READ FUNCTIONS (called from manage.c)		
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//						SPAMC Crop Data Reading
//							 
//									Enli Wang   
//									
//	    							22.03.1995
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//		SPASS Genotype Data Read //
///////////////////////////////////////////////////////////////////////////////////////////////
// Now in Modul Base for SPASS and CERES


//Done SPASS model

int done_spass_model(spass *self)
{
	expertn_modul_base *xpn = &(self->parent);
	
	PPLANT			pPl = xpn->pPl;
	PGENOTYPE		pGe = pPl->pGenotype;
	
	if (self->__DONE_DONE == 0)
		{
			self->__DONE_DONE=1;
		} else
		{	
			return RET_SUCCESS;
		}
	
	
	g_free(self->afActLayRootWatUpt);
	self->afActLayRootWatUpt=NULL;	
	g_free(self->fTemp_zwischen);
	self->fTemp_zwischen=NULL;
	g_free(self->fAveGrossPs_zwischen);
	self->fAveGrossPs_zwischen=NULL;
	g_free(self->fSolRad_zwischen);
	self->fSolRad_zwischen=NULL;
	g_free(pGe->PmaxTemp);
	g_free(pGe->PmaxAge);
	g_free(pGe->DevRateVegTemp);	
	g_free(pGe->DevRateRepTemp);	
	g_free(pGe->DevRateVegPhtP);
	g_free(pGe->VernRateTemp);	
	g_free(pGe->DVR);
	g_free(pGe->PartRoot);
	g_free(pGe->PartLeaf);	
	g_free(pGe->PartStem);	
	g_free(pGe->PartStorage);	
	g_free(pGe->ReservGrwLag);
	g_free(pGe->ReservTransTc);
	g_free(pGe->SoFillLag);				
	g_free(pGe->SoFillRateTemp);
	g_free(pGe->SoNFillRateTemp);
	g_free(pGe->MaxLvNc);
	g_free(pGe->MaxStNc);					
	g_free(pGe->MaxRtNc);
	g_free(pGe->OptLvNc);
	g_free(pGe->OptStNc);					
	g_free(pGe->OptRtNc);	
	g_free(pGe->MinLvNc);
	g_free(pGe->MinStNc);					
	g_free(pGe->MinRtNc);
	g_free(pGe->RootExtRateTemp);
	g_free(pGe->RootExtRateWc);
	g_free(pGe->LvDeathRateTemp);					
	g_free(pGe->RtDeathRateTemp);
	g_free(pGe->SpecLfWght);
	g_free(pGe->LfAppRateTemp);
	
	if (self->ini_filename!=NULL) g_free(self->ini_filename);
	self->ini_filename=NULL;
	return RET_SUCCESS;
}

