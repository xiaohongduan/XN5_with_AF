#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
	                               register_modul_new("WRF CHEM Coupling",  // Bezeichnung
	                                       "other",						                   // Modul
	                                       "wrf chem",				           // Sub Modul
	                                       "wrfchem_coupling",					           // Klasse
	                                       "wrfchem_coupling_load",						           // load/init Function
	                                       NULL,						           // global run Function
	                                       "wrfchem_coupling_mosaic_run",						           // mosaic run Function
	                                       "wrfchem_coupling_run",	    // run Funktion
	                                       "wrfchem_coupling_done")                                  // done Function	                                                                                
	                              );

}
