#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(4,  // Anzahl der Modelle
	                               register_modul_new("Hutson u Wagenet 1992",	   // Bezeichnung
	                                       "nitrogen",						       // Modul
	                                       "nitrification",				           // Sub Modul
	                                       "stickstoff",						   // Klasse
	                                       "stickstoff_load",					   // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
										   "stickstoff_run",		               // run Funktion
	                                       "stickstoff_done"),					   // done Function
										   
										   
									register_modul_new("Hutson u Wagenet 1992",     // Bezeichnung
											"nitrogen",						        // Modul
											"denitrification",	           			// Sub Modul
											"stickstoff",					   		// Klasse
											"leachn_denit_load",		            // load/init Function
											NULL,						            // global run Function
											NULL,						            // mosaic run Function
											"leachn_denit_run_call",	            // run Funktion
											"leachn_denit_done"),                // done Function
										   
										   
									register_modul_new("Hutson u Wagenet 1992",     // Bezeichnung
											"nitrogen",						        // Modul
											"mineralisation",	           			// Sub Modul
											"stickstoff",					   		// Klasse
											"leachn_miner_load",		            // load/init Function
											NULL,						            // global run Function
											NULL,						            // mosaic run Function
											"leachn_miner_run",	         		   // run Funktion
											"leachn_miner_done"),              	  // done Function
											
											
									register_modul_new("Hutson u Wagenet 1992",     // Bezeichnung
											"nitrogen",						        // Modul
											"urea hydrolysis",	           			    // Sub Modul
											"stickstoff",					   		// Klasse
											"leachn_hydro_load",		            // load/init Function
											NULL,						            // global run Function
											NULL,						            // mosaic run Function
											"leachn_hydro_run",	         		   // run Funktion
											"leachn_hydro_done")              	  // done Function		
										   
										   
										   
	                              );

}
