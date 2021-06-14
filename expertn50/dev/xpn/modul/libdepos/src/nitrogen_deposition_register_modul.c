#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
									register_modul_new("Constant Deposition",  // Bezeichnung
													   "nitrogen",						       // Modul
													   "deposition",				           // Sub Modul
													    "nitrogen_deposition",				   // Klasse
														"nitrogen_deposition_load",	           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"nitrogen_deposition_run",	           // run Funktion
														NULL)                                // done Function				
									                                                         // done Function																
								   );
								   
}

