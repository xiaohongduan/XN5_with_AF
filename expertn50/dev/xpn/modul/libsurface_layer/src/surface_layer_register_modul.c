#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
	                               register_modul_new("Similary Function",  // Bezeichnung
	                                       "atmosphere",						                   // Modul
	                                       "stability functions",				           // Sub Modul
	                                       "surface_layer",					           // Klasse
	                                       NULL,			    // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "surface_layer_setup_stability",	           // run Funktion
	                                       NULL)                              // done Function	                               
	                              );

}
