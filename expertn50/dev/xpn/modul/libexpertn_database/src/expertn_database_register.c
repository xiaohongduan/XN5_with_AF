#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef XPN_WITHOUT_DATABASE
G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,
									register_modul_new("Expert N Standard Read INI",  // Bezeichnung
													   "control",							// Modul
													   "database",						    // Sub Modul
													    "expertn_standard_ini",					// Klasse
														"expertn_standard_ini_load",      // load/init Function
														NULL,								// global run Function
														NULL,						           // mosaic run Function
														"expertn_standard_ini_run",				// run Funktion																				
														"expertn_standard_ini_done")        // done Function
										
										
								   );
								   
	
}
#else
G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(2,  // Anzahl der Modelle
									register_modul_new("Expert N Standard Database",  // Bezeichnung
													   "control",							// Modul
													   "database",						    // Sub Modul
													    "expertn_database",					// Klasse
														"expertn_database_initialize",      // load/init Function
														NULL,								// global run Function
														NULL,						           // mosaic run Function
														"expertn_database_run",				// run Funktion																				
														"expertn_datanase_finalize"),        // done Function
									register_modul_new("Expert N Standard Read INI",  // Bezeichnung
													   "control",							// Modul
													   "database",						    // Sub Modul
													    "expertn_standard_ini",					// Klasse
														"expertn_standard_ini_load",      // load/init Function
														NULL,								// global run Function
														NULL,						           // mosaic run Function
														"expertn_standard_ini_run",				// run Funktion																				
														"expertn_standard_ini_done")        // done Function
										
										
								   );
								   
	
}
    

#endif
