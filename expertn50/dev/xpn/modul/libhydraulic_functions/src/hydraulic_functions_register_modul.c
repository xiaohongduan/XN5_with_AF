#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(5,  // Anzahl der Modelle
									register_modul_new("Hutson and Cass",  // Bezeichnung
													   "water",				   // Modul
													   "hydraulic functions",	   // Sub Modul
													   "hydraulic_functions",	   // Klasse
													   "hydraulic_functions_set_Hutson_and_Cass",    // load/init Function
													    NULL,				          // global run Function
														NULL,						           // mosaic run Function
													    NULL,	                                  // run Funktion														
													    NULL),                                    // done Function
					                                register_modul_new("van Genuchten and Mualem",  // Bezeichnung
													   "water",				   // Modul
													   "hydraulic functions",	   // Sub Modul
													   "hydraulic_functions",	   // Klasse
													   "hydraulic_functions_set_van_Genuchten_and_Mualem",    // load/init Function
													    NULL,				          // global run Function
														NULL,						           // mosaic run Function
													    NULL,	                                  // run Funktion
													    NULL),                                    // done Function														
					                                register_modul_new("Brooks and Corey",  // Bezeichnung
													   "water",				   // Modul
													   "hydraulic functions",	   // Sub Modul
													   "hydraulic_functions",	   // Klasse
													   "hydraulic_functions_set_Brooks_and_Corey",    // load/init Function
													    NULL,				          // global run Function
														NULL,						           // mosaic run Function
													    NULL,	                                  // run Funktion
													    NULL),                                    // done Function														
								        register_modul_new("Brutsaert and Gardner",  // Bezeichnung
													   "water",				   // Modul
													   "hydraulic functions",	   // Sub Modul
													   "hydraulic_functions",	   // Klasse
													   "hydraulic_functions_set_Brutsaert_and_Gardner",    // load/init Function
													    NULL,				          // global run Function
														NULL,						           // mosaic run Function
													    NULL,	                                  // run Funktion
													    NULL),                                    // done Function														
								        register_modul_new("Bimodal van Genuchten",  // Bezeichnung
													   "water",				   // Modul
													   "hydraulic functions",	   // Sub Modul
													   "hydraulic_functions",	   // Klasse
													   "hydraulic_functions_set_Bimodal_van_Genuchten",    // load/init Function
													    NULL,				          // global run Function
														NULL,						           // mosaic run Function
													    NULL,	                                  // run Funktion
													    NULL)                                    // done Function														
																									
								   );
								   
}

