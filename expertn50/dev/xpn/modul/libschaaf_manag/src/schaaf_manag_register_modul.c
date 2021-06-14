#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(3,  // Anzahl der Modelle
	                               register_modul_new("Williams et al. (EPIC)",  // Bezeichnung
	                                       "management",						                   // Modul
	                                       "soil loosing compaction",				           // Sub Modul
	                                       "schaaf_manag",					           // Klasse
	                                       "schaaf_manag_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "TSSoilLoosening",	           // run Funktion
	                                       NULL),                                                        // done Function

	                               register_modul_new("Williams et al. (EPIC)",  // Bezeichnung
	                                       "management",						                   // Modul
	                                       "mixing incorporation",	           // Sub Modul
	                                       "schaaf_manag",					           // Klasse
	                                       "schaaf_manag_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "TSSoilMixing",	           // run Funktion
	                                       NULL),                                                         // done Function
	                              
								   register_modul_new("TSIrrigation",  // Bezeichnung
	                                       "management",						                   // Modul
	                                       "irrigation",	           // Sub Modul
	                                       "schaaf_manag",					           // Klasse
	                                       "schaaf_manag_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "TSIrrigation",	           // run Funktion
	                                       NULL)                                                         // done Function
								  
								  );

}
