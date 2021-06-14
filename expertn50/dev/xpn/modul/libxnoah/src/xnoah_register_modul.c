#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(5,  // Anzahl der Modelle
									register_modul_new("NOAH",  // Bezeichnung
													   "heat",						                   // Modul
													   "emissivity",				           // Sub Modul
													    "xnoah",					           // Klasse
														"xnoah_load_and_init",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"xnoah_run_emissivity",	           // run Funktion
														"xnoah_heat_transfer_done"),                                                        // done Function
									register_modul_new("NOAH",  // Bezeichnung
													   "plant",						                   // Modul
													   "actual transpiration",				           // Sub Modul
													    "xnoah",					           // Klasse
														"xnoah_run_load",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"xnoah_run_transpiration",	           // run Funktion
														"xnoah_run_done"),                                                        // done Function
																							                                                     
								   register_modul_new("NOAH",  // Bezeichnung
													   "water",						                   // Modul
													   "potential evapotranspiration",				           // Sub Modul
													    "xnoah",					           // Klasse
														"xnoah_run_load",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"xnoah_run_potential_evaportranspiration",	           // run Funktion
														"xnoah_run_done"),
								   register_modul_new("NOAH",  // Bezeichnung
													   "heat",						                   // Modul
													   "sensible heat",				           // Sub Modul
													    "xnoah",					           // Klasse
														"xnoah_run_load",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"xnoah_run_senible_heat",	           // run Funktion
														"xnoah_run_done"),
								   register_modul_new("NOAH LSM",  // Bezeichnung
													   "heat",						                   // Modul
													   "sensible heat",				           // Sub Modul
													    "noah",					           // Klasse
														"noah_run_load",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"noah_run_senible_heat",	           // run Funktion
														"noah_run_done") 
								   );
								   
}

