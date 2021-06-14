#include "hpm_animal.h"
#include "hpm_math.h"
#include "hpm_ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int hpm_animal_init(hpm *self)
{
	init_hpm_model(self);
	return RET_SUCCESS;
}


typedef struct {
	// Input und Output:
	double OXLam_an4[4]; // Blätter, die gefressen werden -> Momentan nicht implementiert --> 0
	double OXss_an4[4]; // Stiel und Stamm, die gefressen werden -> Momentan nicht implementiert --> 0
	double OLAI_an4[4]; // Leaf Area Index -> animal
	double OCSsh_an_gr; // Substrate C Anteil -> animal --> 0
	double ONSsh_an_gr; // shoot ouput N an animal
	double IDMpl_an_gnd; // kg DM m-2 day-1. Daily intake rate from grazing.
	double n;
	double active; // 1.0 or 0.0
	double MCanret;
	double MNanret;
	double ONph_an;
} T_Animal2;



int hpm_animal_run(hpm *self)
{
   double MXsh,MSsh,Msh;
   double OXpl_an,fXlam_an,OXss_an,OXlam_an,zm_lam_an,zlam_an,OLAI_an;
   double zm_ss_an,zss_an;
   
  
   		   
   
   
   // 3.1 ...
	MXsh = self->Plant.MXss + self->Plant.MXLam;
	
	MSsh = self->Plant.MCSsh * self->parameter.photosynthesis.rmmCS/12.0 + self->Plant.MNSsh * self->parameter.photosynthesis.rmmNS/14.0 + self->Plant.MNph_ul * 100.0 / 14.0;
		
	Msh = MXsh + MSsh;
	
	
   
   //Grazing.
   //Removal of total DM (structure + storage) from all 4 shoot categories
   //is IDMpl_an_gnd (kg total DM m-2 day-1).  See Animal Submodel Section.
   //This gives a structural DM removal flux of:
      OXpl_an = self->Animal.IDMpl_an_gnd * MXsh / Msh;//   !kg structural DM m-2 day-1.
   //!A fraction fXlam_an of this flux is removed from the lamina (lam):
      fXlam_an = self->Plant.MXLam / ( self->Plant.MXLam + self->parameter.animal.cssan * self->Plant.MXss );
         //!css_an is a weighting constant, = 0 for sheep who only eat lamina.
         //!if css_an = 1, then the animals eat lamina, and sheath + stem, 
         //!according to their availability.
   //!Structural DM flux from lamina is
      OXlam_an = fXlam_an * OXpl_an;
   //!and the remaining flux from the sheath + stem (ss) is
      OXss_an = ( 1.0 - fXlam_an ) * OXpl_an;//        !kg XDM m-2 day-1.
	  
	//where the factor, zlam_an, is given by
            zm_lam_an = 1.0 / ( self->parameter.animal.cLAIan4[0] * self->Plant.LAI4[0] + 
								self->parameter.animal.cLAIan4[1] * self->Plant.LAI4[1] +
								self->parameter.animal.cLAIan4[2] * self->Plant.LAI4[2] +
								self->parameter.animal.cLAIan4[3] * self->Plant.LAI4[3]);
			zlam_an = OXlam_an * zm_lam_an;
			//!cLAI1_an etc are weighting constants for the different age
			//!categories.

   //Lamina structure removed (kg structural dry matter m-2 day-1):
      self->Animal.OXLam_an4[0] = self->parameter.animal.cLAIan4[0] * self->Plant.LAI4[0] * zlam_an;
	  self->Animal.OXLam_an4[1] = self->parameter.animal.cLAIan4[1] * self->Plant.LAI4[1] * zlam_an;
	  self->Animal.OXLam_an4[2] = self->parameter.animal.cLAIan4[2] * self->Plant.LAI4[2] * zlam_an;
	  self->Animal.OXLam_an4[3] = self->parameter.animal.cLAIan4[3] * self->Plant.LAI4[3] * zlam_an;
      
      

   //Leaf area removed [m-2 leaf (m-2 ground)-1 day-1]:
      self->Animal.OLAI_an4[0] = self->Animal.OXLam_an4[0] * self->Plant.LAI4[0] / self->Plant.MLam4[0];
	  self->Animal.OLAI_an4[1] = self->Animal.OXLam_an4[1] * self->Plant.LAI4[1] / self->Plant.MLam4[1];
	  self->Animal.OLAI_an4[2] = self->Animal.OXLam_an4[2] * self->Plant.LAI4[2] / self->Plant.MLam4[2];
	  self->Animal.OLAI_an4[3] = self->Animal.OXLam_an4[3] * self->Plant.LAI4[3] / self->Plant.MLam4[3];
	  
	OLAI_an = hpm_math_sum4(self->Animal.OLAI_an4);
  
   //where the factor, zss_an, is given by
            zm_ss_an = 1.0 / ( self->parameter.animal.cssan4[0] * self->Plant.Mss4[0] + 
							   self->parameter.animal.cssan4[1] * self->Plant.Mss4[1] +
							   self->parameter.animal.cssan4[2] * self->Plant.Mss4[2] +
							   self->parameter.animal.cssan4[3] * self->Plant.Mss4[3]);
			
         zss_an = OXss_an * zm_ss_an;
         //css1_an etc are weighting constants for the different age
         //categories.
   
   //Sheath+stem structure removed (kg structural dry matter m-2 day-1):
      self->Animal.OXss_an4[0] = self->parameter.animal.cssan4[0] *  self->Plant.Mss4[0] * zss_an;
	  self->Animal.OXss_an4[1] = self->parameter.animal.cssan4[1] *  self->Plant.Mss4[1] * zss_an;
	  self->Animal.OXss_an4[2] = self->parameter.animal.cssan4[2] *  self->Plant.Mss4[2] * zss_an;
	  self->Animal.OXss_an4[3] = self->parameter.animal.cssan4[3] *  self->Plant.Mss4[3] * zss_an;
	  
         

   //Sheath + stem area flux associated with flux OXss_an is
      //self->Animal.OSAI_an = self->parameter.animal.cSAI * OXss_an;//                          !m2 m-2 day-1.

   //OshAI   output of green area (shoot area index) to grazing animals:
      //self->Animal.OshAI_an = OLAI_an + self->Animal.OSAI_an;//              !m2 (m2 ground)-1 day-1.
   //OfshAI_an  output fraction of shoot area index:
      //self->Animal.OfshAI_an = self->Animal.OshAI_an / shAI                                 !day-1.

   //Carbon structure removed:
      //self->Animal.OCXsh_an = IDMpl_an_gnd * MCXsh / Msh;//     !kg structural C m-2 day-1.
   //N structure removed:
      //ONXsh_an = IDMpl_an_gnd * MNXsh / Msh     !kg structural N m-2 day-1.

   //Carbon substrate, nitrogen substrate, and photosynthetic N removed 
      //(kg C substrate, kg N substrate, kg photosynthetic N, m-2 day-1):
      self->Animal.OCSsh_an_gr = self->Animal.IDMpl_an_gnd * self->Plant.MCSsh / Msh;   
      self->Animal.ONSsh_an_gr = self->Animal.IDMpl_an_gnd * self->Plant.MNSsh / Msh;
      self->Animal.ONph_an =  self->Animal.IDMpl_an_gnd * self->Plant.MNph_ul / Msh; // !This assumes animals
                        //!remove the upper leaves (ul), where the 
                        //!photosynthetic N (phN) is highest.
   //Carbon storage removed (kg C storage m-2 day-1):
      self->Animal.OCTsh_an = self->Animal.IDMpl_an_gnd * fXlam_an * self->Plant.MCTsh / hpm_math_sum4(self->Plant.MLam4);

   //Total C and N fluxes into the animals:
      //ICpl_an = self->Animal.OCXsh_an + self->Animal.OCSsh_an + self->Animal.OCTsh_an;//    !kg C m-2 day-1.
      //INpl_an = self->Animal.ONXsh_an + self->Animal.ONSsh_an + self->Animal.ONph_an;//     !kg N m-2 day-1.

   //Carbon:nitrogen ratio is the ingested dry matter,
    //  IF ( INpl_an .GT. zero ) rCNan_in = ICpl_an / INpl_an !kg C/ kg N.
	
	return RET_SUCCESS;
}


int hpm_animal_done(hpm *self)
{	
	done_hpm_model(self);
	return RET_SUCCESS;
}
