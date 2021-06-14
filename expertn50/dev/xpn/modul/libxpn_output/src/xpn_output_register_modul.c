#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(1,  // Anzahl der Modelle
									register_modul_new("XPN_OUTPUT",  // Bezeichnung
													   "control",						                   // Modul
													   "output",				           // Sub Modul
													    "xpn_output",					           // Klasse
														"xpn_output_reg_var",						           // load/init Function
														NULL,						           // mosaic run Function
														NULL,						           // global run Function
														"xpn_output_calc_var",	           // run Funktion
														"xpn_output_done"));                                                        // done Function											
								   
}

