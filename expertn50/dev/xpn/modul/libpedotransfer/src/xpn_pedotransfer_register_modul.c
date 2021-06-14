#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(3,  // Anzahl der Modelle
									register_modul_new("Campbell",  // Bezeichnung
													   "control",						                   // Modul
													   "pedotransfer",				           // Sub Modul
													    "xpn_pedotransfer",					           // Klasse
														"xpn_pedotransfer_set_campbell",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														NULL,	           // run Funktion
														NULL),                                                       // done Function																							
								   register_modul_new("Vereecken",  // Bezeichnung
													   "control",						                   // Modul
													   "pedotransfer",				           // Sub Modul
													    "xpn_pedotransfer",					           // Klasse
														"xpn_pedotransfer_set_Vereecken",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														NULL,	           // run Funktion
														NULL),   
								   register_modul_new("Brakensiek & Rawls",  // Bezeichnung
													   "control",						                   // Modul
													   "pedotransfer",				           // Sub Modul
													    "xpn_pedotransfer",					           // Klasse
														"xpn_pedotransfer_set_RawlsBrakensiek",						           // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														NULL,	           // run Funktion
														NULL)   
								   );
								   
}

