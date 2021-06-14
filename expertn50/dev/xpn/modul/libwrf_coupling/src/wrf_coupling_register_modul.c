#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
									register_modul_new("WRF Coupling",  // Bezeichnung
													   "other",						                   // Modul
													   "wrf",				           // Sub Modul
													    "wrf_coupling",				   // Klasse
														"wrf_coupling_load",		   // load/init Function
														NULL,	   						// global run Function
														"wrf_coupling_mosaic_run",	   // mosaic run Function
														"wrf_coupling_run",	           // run Funktion
														"wrf_coupling_done")                                                        // done Function
																																				
								   );
								   
}

