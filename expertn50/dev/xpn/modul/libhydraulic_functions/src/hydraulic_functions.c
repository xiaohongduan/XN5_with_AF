//      hydraulic_functions.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "hydraulic_functions.h"
#include "VanGenuchten.h"
#include "Hutson_and_Cass.h"
#include "Bimodal_van_Genuchten.h"
#include "Brutsaert_and_Gardner.h"
#include "Brooks_and_Corey.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


G_DEFINE_TYPE(hydraulic_functions, hydraulic_functions, EXPERTN_MODUL_BASE_TYPE);


static void hydraulic_functions_class_init(hydraulic_functionsClass *klass){}


static void hydraulic_functions_init(hydraulic_functions *self)
{
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// print Output Message if Modul is used from Expert N

self->hydraulic_function_choice=1; // Standart, falls aus irgeneinem Grund keine Wahl getroffen wird

}


int hydraulic_functions_set(hydraulic_functions *self,void *WCont,void *HCond, void *DWCap, void *MPotl)
  {
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.WCont",WCont,-1,TRUE);
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.HCond",HCond,-1,TRUE);
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.DWCap",DWCap,-1,TRUE);
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"hydraulic_fuctions.MPotl",MPotl,-1,TRUE);
	  return RET_SUCCESS;
  }


int hydraulic_functions_set_Hutson_and_Cass(hydraulic_functions *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Hutson and Cass") ;
	  hydraulic_functions_set(self,&HC_WCont,&HC_HCond,&HC_DWCap,&HC_MPotl); 
	  return RET_SUCCESS;
  }

int hydraulic_functions_set_van_Genuchten_and_Mualem(hydraulic_functions *self)
  {
	   expertn_modul_base *xpn = &(self->parent);
	   PRINT_MESSAGE(xpn,5,"Hydraulic Function set to van Genuchten and Mualem") ;
	   hydraulic_functions_set(self,&VG_WCont,&VG_HCond,&VG_DWCap,&VG_MPotl);
	   return RET_SUCCESS;
  }

int hydraulic_functions_set_Brooks_and_Corey(hydraulic_functions *self)
  {
	  expertn_modul_base *xpn = &(self->parent);
	  PRINT_MESSAGE(xpn,5,"Hydraulic Function set to van Brooks and Corey") ;
	  hydraulic_functions_set(self,&BC_WCont,&BC_HCond,&BC_DWCap,&BC_MPotl); 
	  return RET_SUCCESS;
  }
  
int hydraulic_functions_set_Brutsaert_and_Gardner(hydraulic_functions *self)
  {
	 expertn_modul_base *xpn = &(self->parent);
	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Brutsaert and Gardner") ;
	 hydraulic_functions_set(self,&M2_WCont,&M2_HCond,&M2_DWCap,&M2_MPotl); 
	 return RET_SUCCESS;
  }
  
int hydraulic_functions_set_Bimodal_van_Genuchten(hydraulic_functions *self)
  {
	 expertn_modul_base *xpn = &(self->parent);
	 PRINT_MESSAGE(xpn,5,"Hydraulic Function set to Bimodal van Genuchten") ;
	 hydraulic_functions_set(self,&C2_WCont,&C2_HCond,&C2_DWCap,&C2_MPotl); 
	 return RET_SUCCESS;	 
  }
  
 
