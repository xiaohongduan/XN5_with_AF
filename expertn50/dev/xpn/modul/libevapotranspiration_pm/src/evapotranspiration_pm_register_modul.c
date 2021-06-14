#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(23,  // Anzahl der Modelle														
									register_modul_new("Penman Monteith",	// Bezeichnung
													   "water",									// Modul
													   "potential evapotranspiration",			// Sub Modul
													   "evapotranspiration",					// Klasse
													    "evapotranspiration_load",				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_run",				// run Funktion
														NULL),								// done Function
									register_modul_new("Penman Monteith ASCE 81",	// Bezeichnung
													   "water",									// Modul
													   "potential evapotranspiration",			// Sub Modul
													   "evapotranspiration",					// Klasse
													    "evapotranspiration_load",				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_run65_0",				// run Funktion
														NULL),								// done Function
									register_modul_new("Penman Monteith ASCE 81 (MO 1)",	// Bezeichnung
													   "water",									// Modul
													   "potential evapotranspiration",			// Sub Modul
													   "evapotranspiration",					// Klasse
													    "evapotranspiration_load",				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_run65_1",				// run Funktion
														NULL),								// done Function					
									register_modul_new("Penman Monteith ASCE 81 crop",	// Bezeichnung
													   "water",									// Modul
													   "potential evapotranspiration",			// Sub Modul
													   "evapotranspiration",					// Klasse
													    "evapotranspiration_load",				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_run65_2",				// run Funktion
														NULL),								// done Function					
									register_modul_new("Penman Monteith (FAO)",	// Bezeichnung
													   "water",									// Modul
													   "potential evapotranspiration",			// Sub Modul
													   "evapotranspiration",					// Klasse
													    "evapotranspiration_FAO_load",				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_FAO_run",				// run Funktion
														NULL),								// done Function
									register_modul_new("Haude",	// Bezeichnung
													   "water",									// Modul
													   "potential evapotranspiration",			// Sub Modul
													   "evapotranspiration",					// Klasse
													    "evapotranspiration_haude_load",				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_haude_run",				// run Funktion
														NULL),								// done Function														
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "water",									// Modul
													   "potential evaporation",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evaporation_run1",						// run Funktion
														NULL),								// done Function
									register_modul_new("Penman Monteith exp app",						// Bezeichnung
													   "water",									// Modul
													   "potential evaporation",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evaporation_run2",						// run Funktion
														NULL),
									register_modul_new("Penman Monteith No Evap",						// Bezeichnung
													   "water",									// Modul
													   "potential evaporation",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evaporation_run3",						// run Funktion
														NULL),														
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "water",									// Modul
													   "actual evaporation",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"act_evaporation_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "plant",									// Modul
													   "potential transpiration",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"pot_transpiration_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "heat",									// Modul
													   "latent heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_lh_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith (Pot Evap)",						// Bezeichnung
													   "heat",									// Modul
													   "latent heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_lh_pot_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "heat",									// Modul
													   "sensible heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_sh_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith (Pot)",						// Bezeichnung
													   "heat",									// Modul
													   "sensible heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_sh_run2",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "heat",									// Modul
													   "ground heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_gh_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith plus res",						// Bezeichnung
													   "heat",									// Modul
													   "ground heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,				// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_gh_run_plus",					// run Funktion
														NULL),
                                    register_modul_new("Penman Monteith with LAI",						// Bezeichnung
													   "heat",									// Modul
													   "ground heat",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,		// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_gh_run_with_LAI",					// run Funktion
														NULL),									// done Function												
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "heat",									// Modul
													   "net radiation",					// Sub Modul
													   "evapotranspiration",					// Klasse
														"penman_monteith_nr_load",									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"penman_monteith_nr_run",					// run Funktion
														NULL),									// done Function
									register_modul_new("Penman Monteith",						// Bezeichnung
													   "heat",									// Modul
													   "emissivity",					// Sub Modul
													   "evapotranspiration",					// Klasse
														NULL,									// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"evapotranspiration_emissivity",					// run Funktion
														NULL),									// done Function
									register_modul_new("const",						// Bezeichnung
													   "water",									// Modul
													   "kc factor",					// Sub Modul
													   "kc_factor",					// Klasse
														"kc_factor_const",			// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														NULL,					// run Funktion
														NULL),									// done Function
									register_modul_new("dev stage",						// Bezeichnung
													   "water",									// Modul
													   "kc factor",					// Sub Modul
													   "kc_factor",					// Klasse
														"kc_factor_dev_stage_load",			// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"kc_factor_dev_stage_run",					// run Funktion
														"kc_factor_dev_stage_done"),									// done Function
									register_modul_new("ET0xLAI3rd",						// Bezeichnung
													   "water",									// Modul
													   "kc factor",					// Sub Modul
													   "kc_factor",					// Klasse
														"kc_factor_LAI3rd_load",			// load/init Function
														NULL,									// global run Function
														NULL,						           // mosaic run Function
														"kc_factor_LAI3rd_run",					// run Funktion
														NULL)									// done Function
									);
}

