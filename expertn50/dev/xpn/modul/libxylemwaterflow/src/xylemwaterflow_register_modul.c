#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
	                               register_modul_new("Xylem water flow",  // Bezeichnung
	                                       "plant",						              // Modul
	                                       "actual transpiration",				           // Sub Modul
	                                       "xylemwaterflow_connector",			// Klasse
	                                       "xylemwaterflow_connector_load",	           // load/init Function
	                                       "xylemwaterflow_connector_global_run",     // global run Function
	                                       "xylemwaterflow_connector_mosaic_run",         // mosaic run Function
	                                       "xylemwaterflow_connector_run",	           // run Funktion
	                                       "xylemwaterflow_connector_done")// done Function
	                              );

}
