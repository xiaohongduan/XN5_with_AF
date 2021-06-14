#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(2,  // Anzahl der Modelle
									register_modul_new("Rutter Model (Trees)",				// Bezeichnung
													   "water",						// Modul
													   "interception",				// Sub Modul
													    "interception",				// Klasse
														"interception_load",		// load/init Function
														NULL,						// global run Function
														NULL,						// mosaic run Function
														"interception_Rutter_Model",	           	// run Funktion
														NULL),                     	// done Function
														
									register_modul_new("Not Linear Regression (Crops)",  			// Bezeichnung
													   "water",						    // Modul
													   "interception",					// Sub Modul
													    "interception",					// Klasse
														"interception_load",			// load/init Function
														NULL,						    // global run Function
														NULL,						// mosaic run Function
														"interception_regression",		// run Funktion
														NULL)                           // done Function																
								   );
								   
}

