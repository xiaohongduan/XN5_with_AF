#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(2,  // Anzahl der Modelle
	                               register_modul_new("${templ_uppercase} BiomassGrowth",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "biomass growth",				           // Sub Modul
	                                       "${template}",					           // Klasse
	                                       NULL,						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "${template}_BiomassGrowth",	           // run Funktion
	                                       NULL),                                                        // done Function

	                               register_modul_new("${templ_uppercase} Gross Photosynthesis",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "canopy gross photosynthesis",	           // Sub Modul
	                                       "${template}",					           // Klasse
	                                       NULL,						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "${template}_Photosynthesis",	           // run Funktion
	                                       NULL)                                                         // done Function
	                              );

}
