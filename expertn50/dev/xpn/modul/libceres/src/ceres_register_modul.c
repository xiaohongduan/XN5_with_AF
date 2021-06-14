#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(11,  // Anzahl der Modelle
	
	//added by Hong on 20180320: to distinguish crop model choice for adaptive crop rotation of mpmas
									register_modul_new("CERES",						// Bezeichnung
													   "plant",									// Modul
													   "model choice",					// Sub Modul
													   "gecros",					// Klasse
														NULL,			// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														NULL,					// run Funktion
														NULL),
//End of Hong							
	                               register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "potential transpiration",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_potential_transpiration",	           // run Funktion
	                                       "ceres_done"),                           // done Function								   
								    register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "actual transpiration",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_actual_transpiration",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_phenological_development",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "canopy gross photosynthesis",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_photosynthesis",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "biomass growth",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_biomass_growth",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "canopy formation",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_canopy_formation",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "root length growth",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_root_length_growth",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "nitrogen demand",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_nitrogen_demand",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "nitrogen uptake",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_nitrogen_uptake",	           // run Funktion
	                                       "ceres_done"),                           // done Function
									register_modul_new("CERES",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "crop senescence",				           // Sub Modul
	                                       "ceres",					           // Klasse
	                                       "ceres_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "ceres_crop_senescence",	           // run Funktion
	                                       "ceres_done")                           // done Function
	                              );

}

