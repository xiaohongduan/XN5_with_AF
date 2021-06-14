#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(12,  // Anzahl der Modelle
	
	//added by Hong on 20180320: to distinguish crop model choice for adaptive crop rotation of mpmas
									register_modul_new("SPASS",						// Bezeichnung
													   "plant",									// Modul
													   "model choice",					// Sub Modul
													   "gecros",					// Klasse
														NULL,			// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														NULL,					// run Funktion
														NULL),
//End of Hong							

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "phenological development",			   // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_phenological_development",	   // run Funktion
														"done_spass_model"),                   // done Function
									
									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "root length growth",				   // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_root_length_growth",	           // run Funktion
														"done_spass_model"),                   // done Function

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "canopy gross photosynthesis",		   // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_canopy_gross_photosynthesis",   // run Funktion
														"done_spass_model"),                   // done Function

									register_modul_new("Interception (referred to Wang)", 	   // Bezeichnung
													    "water",						       // Modul
													    "interception",					       // Sub Modul
													    "spass",					           // Klasse
														"init_spass_interception",			   // load/init Function														
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_CanopyWaterInterception",	   // run Funktion
														NULL),	     		   // done Function

									//register_modul_new("Penman (SPASS)",					   // Bezeichnung
									//				    "water",						       // Modul
									//				    "potential evapotranspiration",	       // Sub Modul
									//				    "spass",					           // Klasse
									//					"init_spass_model",					   // load/init Function														
									//					NULL,						           // global run Function
									//					NULL,						           // mosaic run Function
									//					"spass_PotEvapotraPenman",			   // run Funktion
									//					"done_spass_model"),	     		   // done Function	

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "potential transpiration",	           // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_potential_transpiration",	   // run Funktion
														"done_spass_model"),				   // done Function

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "actual transpiration",				   // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_actual_transpiration",	       // run Funktion
														"done_spass_model"),                   // done Function

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "biomass growth",				       // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_biomass_growth",				   // run Funktion
														"done_spass_model"),                   // done Function																

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "canopy formation",				       // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_canopy_formation",	           // run Funktion
														"done_spass_model"),                   // done Function

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "crop senescence",				       // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function														
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_crop_senescence",	           // run Funktion
														"done_spass_model"),         		   // done Function

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "nitrogen demand",				       // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_nitrogen_demand",	           // run Funktion
														"done_spass_model"),                   // done Function

									register_modul_new("Wang (SPASS)",						   // Bezeichnung
													    "plant",						       // Modul
													    "nitrogen uptake",				       // Sub Modul
													    "spass",					           // Klasse
														"init_spass_model",					   // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"spass_nitrogen_uptake",	           // run Funktion
														"done_spass_model")                   // done Function					
														
								   );
								   
}

