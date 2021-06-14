#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
	                               register_modul_new("HYDRUS Flow",  						    // Bezeichnung
	                                       "water",						           // Modul
	                                       "flow module",	          			   // Sub Modul
	                                       "hydrus",					           // Klasse
	                                       "hydrus_water_flow_init",	           // load/init Function
	                                       NULL,						           // global run Function
										   NULL,						           // mosaic run Function
	                                       "hydrus_water_flow_run",	           // run Funktion
	                                       "hydrus_water_flow_done")              // done Function
	                              );

}
