#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(9,  // Anzahl der Modelle
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "plant",						                   // Modul
													   "canopy gross photosynthesis",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_canopy_gross_photosynthesis",	           // run Funktion
														"done_hpm_model"),                                                         // done Function																
								   register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "plant",						                   // Modul
													   "biomass growth",				           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_biomass_growth",	           // run Funktion
														"done_hpm_model"),                                                        // done Function
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "plant",						                   // Modul
													   "root length growth",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_root_length_growth",	           // run Funktion
														"done_hpm_model"),
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "plant",						                   // Modul
													   "actual transpiration",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_water_uptake_hpm",	           // run Funktion
														"done_hpm_model"),
									register_modul_new("Hurley Pasture Model (Feddes)",  // Bezeichnung
													   "plant",						                   // Modul
													   "actual transpiration",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_water_uptake_feddes",	           // run Funktion
														"done_hpm_model"),
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "plant",						                   // Modul
													   "nitrogen uptake",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_nitrogen_uptake",	           // run Funktion
														"done_hpm_model"),									
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "heat",						                   // Modul
													   "albedo",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_calc_albedo",	           // run Funktion
														"done_hpm_model"),
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "management",						                   // Modul
													   "harvest",	           // Sub Modul
													    "hpm",					           // Klasse
														"hpm_harvest_init",	           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_harvest_run",	           // run Funktion
														"hpm_harvest_done"),
									register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "management",						                   // Modul
													   "animal",	           // Sub Modul
													    "hpm",					           // Klasse
														"hpm_animal_init",	           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"hpm_animal_run",	           // run Funktion
														"hpm_animal_done")
														
								   );
								   
								   /*register_modul_new("Hurley Pasture Model",  // Bezeichnung
													   "water",						                   // Modul
													   "actual evaporation",	           // Sub Modul
													    "hpm",					           // Klasse
														"init_hpm_model",						           // load/init Function
														NULL,						           // global run Function
														"hpm_actual_evapotranspiration",	           // run Funktion
														NULL),*/
								   
}

