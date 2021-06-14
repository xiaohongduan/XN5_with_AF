#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(11,  // Anzahl der Modelle
									register_modul_new("DAISY Modul Heat Transfer",  // Bezeichnung
													   "heat",						// Modul
													   "heat transfer",				// Sub Modul
													    "daisy",					// Klasse
														"daisy_load",						// load/init Function
														NULL,						// global run Function
														NULL,			           // mosaic run Function
														"daisy_run",		// run Funktion
														NULL),// done Function                                                      // done Function																
									register_modul_new("Albedo",  // Bezeichnung
													   "heat",						// Modul
													   "albedo",				// Sub Modul
													    "albedo",					// Klasse
														"albedo_load",						// load/init Function
														NULL,				           // mosaic run Function
														NULL,						// global run Function
														"albedo_run",		// run Funktion
														NULL),// done Function                                                      // done Function																
								   register_modul_new("Water const (0.05)",  // Bezeichnung
													   "heat",						// Modul
													   "albedo",				// Sub Modul
													    "albedo_ceres",					// Klasse
														"albedo_ceres_Water",						// load/init Function
														NULL,						// global run Function
														NULL,	           // mosaic run Function
														NULL,		// run Funktion
														NULL),// done Function                                                      // done Function																
								   
								   register_modul_new("Vegetation const (0.25)",  // Bezeichnung
													   "heat",						// Modul
													   "albedo",				// Sub Modul
													    "albedo_ceres",					// Klasse
														"albedo_ceres_Vegetation",						// load/init Function
														NULL,						           // mosaic run Function
														NULL,						// global run Function
														NULL,		// run Funktion
														NULL),// done Function                                                      // done Function										   
								   register_modul_new("Albedo const",  // Bezeichnung
													   "heat",						// Modul
													   "albedo",				// Sub Modul
													    "albedo",					// Klasse
														"albedo_load_const",						// load/init Function
														NULL,						// global run Function
														NULL,						           // mosaic run Function
														NULL,		// run Funktion
														NULL),// done Function                                                      // done Function																
								   register_modul_new("Pasture (CERES)",  // Bezeichnung
													   "heat",						// Modul
													   "albedo",				// Sub Modul
													    "albedo_ceres",					// Klasse
														NULL,						// load/init Function
														NULL,						// global run Function
														NULL,						           // mosaic run Function
														"albedo_ceres_pasture",		// run Funktion
														NULL),// done Function      
									register_modul_new("Horton & Chung 86",  // Bezeichnung
													   "heat",						// Modul
													   "latent heat",				// Sub Modul
													    "daisy",					// Klasse
														NULL,						// load/init Function
														NULL,						// global run Function
														NULL,						           // mosaic run Function
														"daisy_calc_latent_heat",		// run Funktion
														NULL),// done Function   
									register_modul_new("Calc from Evaportranspiration",  // Bezeichnung
													   "heat",						// Modul
													   "latent heat",				// Sub Modul
													    "daisy",					// Klasse
														NULL,						// load/init Function
														NULL,						// global run Function
														NULL,						// mosaic run Function
														"daisy_calc_latent_heat2",		// run Funktion
														NULL),// done Function
									register_modul_new("Horton & Chung 86",  // Bezeichnung
													   "heat",						// Modul
													   "sensible heat",				// Sub Modul
													    "daisy",					// Klasse
														NULL,						// load/init Function
														NULL,						// global run Function
														NULL,			           // mosaic run Function
														"daisy_calc_sensible_heat",		// run Funktion
														NULL),// done Function   
									register_modul_new("First Soil Layer",  // Bezeichnung
													   "heat",						// Modul
													   "surface temperature",				// Sub Modul
													    "daisy",					// Klasse
														NULL,						// load/init Function
														NULL,						// global run Function
														NULL,			           // mosaic run Function
														"daisy_st_first_soil_layer",		// run Funktion
														NULL),// done Function
									register_modul_new("Horton & Chung 86",  // Bezeichnung
													   "heat",						// Modul
													   "surface temperature",				// Sub Modul
													    "daisy",					// Klasse
														NULL,						// load/init Function
														NULL,						// global run Function
														NULL,			           // mosaic run Function
														"daisy_st_horten_from_balance",		// run Funktion
														NULL)  // done Function

								   );
								   
}

