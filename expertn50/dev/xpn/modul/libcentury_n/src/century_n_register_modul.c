#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(3,  // Anzahl der Modelle
									register_modul_new("CENTURY_N Mineralisation",  // Bezeichnung
													   "nitrogen",						                   // Modul
													   "mineralisation",				           // Sub Modul
													    "century_n",					           // Klasse
														"century_n_Mineralisation_init",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"century_n_Mineralisation_run",	           // run Funktion
														"century_n_Mineralisation_done"),                              // done Function
														
									register_modul_new("CENTURY_N Nitrification",  // Bezeichnung
													   "nitrogen",						                   // Modul
													   "nitrification",	           // Sub Modul
													    "century_n",					           // Klasse
														"century_n_Nitrification_init",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"century_n_Nitrification_run",	           // run Funktion
														"century_n_Nitrification_done"),                                 // done Function																
									register_modul_new("CENTURY_N Denitrification",  // Bezeichnung
													   "nitrogen",						                   // Modul
													   "denitrification",	           // Sub Modul
													    "century_n",					           // Klasse
														"century_n_Denitrification_init",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"century_n_Denitrification_run",	           // run Funktion
														"century_n_Denitrification_done") 						            // done Function  
								   );
								   
}

