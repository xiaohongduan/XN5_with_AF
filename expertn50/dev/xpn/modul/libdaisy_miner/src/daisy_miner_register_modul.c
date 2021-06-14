#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
	                               register_modul_new("Hansen et al. (DAISY_Miner)",  // Bezeichnung
	                                       "nitrogen",						                   // Modul
	                                       "mineralisation",				           // Sub Modul
	                                       "daisy_miner",					           // Klasse
	                                       "daisy_miner_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "daisy_miner_run",	           // run Funktion
	                                       "daisy_miner_done")                 // done Function	                              
	                              );

}
