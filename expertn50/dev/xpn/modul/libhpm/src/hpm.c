//      hpm.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "hpm.h"
#include "hpm_math.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


G_DEFINE_TYPE(hpm, hpm, EXPERTN_MODUL_BASE_TYPE);


static void hpm_class_init(hpmClass *klass){}


static void hpm_init(hpm *self)
{
	self->__INIT_DONE=0;
	self->__DONE_DONE=0;
	self->__ERROR=RET_SUCCESS;
	self->ini_filename=NULL;
}


void get_SoilVars(hpm *self,struct_Soil_Vars *vars) 
{
expertn_modul_base *xpn = &(self->parent);
	PWLAYER pWL = xpn->pWa->pWLayer->pNext;
	PSWATER pSW = xpn->pSo->pSWater->pNext;
	PSLAYER pSL = xpn->pSo->pSLayer->pNext;
	PCLAYER pCL = xpn->pCh->pCLayer->pNext;
	double fDepth;
	double fRootDepth;
	int count;
	int count_max;
	int count2;
	
	fRootDepth = (xpn->pPl->pRoot->fDepth*1.0e-3);
	if (self->__USE_STATIC_ROOT_LENGTH==TRUE) {
		count_max=self->__STATIC_ROOT_LENGTH;
	} else {		
		fDepth = 0.0;
		count_max=0.0;
		pSL = xpn->pSo->pSLayer->pNext;
		while(fDepth<=fRootDepth) {
			if (count_max >= xpn->pSo->iLayers-1) break;
			fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m
			count_max++;
			pSL = pSL->pNext;
		}
	}

	fDepth = 0.0;
	fRootDepth = (xpn->pPl->pRoot->fDepth*1.0e-3);
	vars->Namm = 0.0;
	vars->Nnit = 0.0;
	vars->theta_soil=0.0;
	vars->theta_soil_max=0.0;
	count = 0;
	count2=0;
	
	
		
	while(1) {
		if (count >= xpn->pSo->iLayers-1) break;		
		fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m		
		count++;

		vars->Namm += (double)pCL->fNH4N*ha_TO_mq_1; //0.0007;
		vars->Nnit += (double)pCL->fNO3N*ha_TO_mq_1; //0.0003;
	
		//if (vars->theta_soil>pSW->fContPWP)
		{
		vars->theta_soil += (double)pWL->fContAct;
		vars->theta_soil_max += (double)pWL->fContMax;
		count2++;
		}
		
		if (count>=count_max)
			{
				break;
			}

		pSL = pSL->pNext;
		pSW = pSW->pNext;
		pWL = pWL->pNext;
		pCL = pCL->pNext;

	}

	vars->TSoil = xpn->pHe->pHLayer->pNext->pNext->fSoilTemp;
	vars->theta_soil/=count2;
	vars->theta_soil_max/=count2;
	
	vars->theta_soil = max(vars->theta_soil,EPSILON);
	vars->Namm = max(vars->Namm,EPSILON);
	vars->Nnit = max(vars->Nnit,EPSILON);
	
		

}


void create_output(hpm *self)
{
	expertn_modul_base *xpn = &(self->parent);
	double chcanLAI = self->parameter.water.chcanLAI;//0.026; // [m / (m^2 leaf  / (m^2 ground))^-1]
	struct_Soil_Vars vars;
	
	
	// LAI
	self->Output.LAI = hpm_math_sum4(self->Plant.LAI4);
	// hcan: Cannopy hight:	
	self->Output.hcan = chcanLAI * self->Output.LAI; // Canopy height: proportional to leaf area index;
	xpn->pPl->pCanopy->fPlantHeight = self->Output.hcan;
	
	// cum harvest mass
	self->Output.cum_harvest_mass = self->Harvest.Sum_CSsh_hv * self->parameter.photosynthesis.rmmCS/12.0 + self->Harvest.Sum_NSsh_hv * self->parameter.photosynthesis.rmmNS/14.0 + hpm_math_sum4(self->Harvest.Sum_Lam_hv4) + hpm_math_sum4(self->Harvest.Sum_ss_hv4);
	
	// MSsh:
	self->Output.MSsh = self->Plant.MCSsh * self->parameter.photosynthesis.rmmCS/12.0 + self->Plant.MNSsh * self->parameter.photosynthesis.rmmNS/14.0 + self->Plant.MNph_ul * 100.0 / 14.0;
	//printf("%f %f %f %f %f %f %f\n",xpn->pTi->pSimTime->fTimeDay, self->Output.MSsh,  self->Plant.MCSsh, self->parameter.photosynthesis.rmmCS,  self->Plant.MNSsh, self->parameter.photosynthesis.rmmNS, self->Plant.MNph_ul);
    
    
	// MSrt:
	self->Output.MSrt = self->Plant.MCSrt * self->parameter.photosynthesis.rmmCS/12.0 + self->Plant.MNSrt*self->parameter.photosynthesis.rmmNS/14.0;
	
	// MXsh:
	self->Output.MXsh = self->Plant.MXss + self->Plant.MXLam;
	
	// MXPl:
	self->Output.MXPl = self->Output.MXsh +self->Plant.MXrt;
	
	// MSPl:
	self->Output.MSPl = self->Output.MSrt + self->Output.MSsh;
	
	// Mrt:
	self->Output.Mrt = self->Plant.MXrt + self->Output.MSrt;
	
	// Msh:
	self->Output.Msh = self->Output.MXsh + self->Output.MSsh;
	
	// MPl:
	self->Output.MPl = self->Output.Mrt + self->Output.Msh;
	
	get_SoilVars(self,&vars);
	
	self->Output.Namm = vars.Namm;
	self->Output.Nnit = vars.Nnit;
	self->Output.theta_soil = vars.theta_soil;
	
	
	
	CHECK_VALID(self->Output.MSsh);
	CHECK_VALID(self->Output.MSrt);
	CHECK_VALID(self->Output.MXsh);
	CHECK_VALID(self->Output.MXPl);
	CHECK_VALID(self->Output.MSPl);
	CHECK_VALID(self->Output.Mrt);
	CHECK_VALID(self->Output.Msh);
	CHECK_VALID(self->Output.MPl);
	
	CHECK_VALID(self->Output.Namm);
	CHECK_VALID(self->Output.Nnit);
	CHECK_VALID(self->Output.theta_soil);
	
	// Expert-N Standard Outputs:
	//self->parent.pPl->pPltNitrogen->fCumActNUpt //[kgN/ha]
	
	xpn->pPl->pBiomass->fRootWeight = self->Output.Mrt *  calc_1m2_to_ha; //[kg / ha]
	xpn->pPl->pBiomass->fLeafWeight = self->Output.Msh *  calc_1m2_to_ha;
	xpn->pPl->pBiomass->fStemWeight = xpn->pPl->pBiomass->fLeafWeight;
	xpn->pPl->pBiomass->fBranchWeight = xpn->pPl->pBiomass->fLeafWeight;
	//xpn->pPl->pCanopy->fLAI = self->Plant->LAI4[0]+self->Plant->LAI4[1]+self->Plant->LAI4[2]+self->Plant->LAI4[3];
	///xpn->pPl->pRoot->fTotalLength = 
	//xpn->pPl->pBiomass->fWoodWeight
	xpn->pPl->pBiomass->fStemWeight = xpn->pPl->pBiomass->fLeafWeight;
	//xpn->pPl->pBiomass-> fAboveBiomass = self->Output.Msh *  calc_1m2_to_ha;
	
	#ifdef TEST_HPM
	self->test_count++;
	#endif
}


