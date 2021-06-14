#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(9,  // Anzahl der Modelle
									register_modul_new("GECROS Development",  // Bezeichnung
													   "plant",						           // Modul
													   "phenological development",			   // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_Development",	               // run Funktion
														"gecros_Done_and_FreeMemory"),         // done Function//
														
									register_modul_new("GECROS BiomassGrowth",  // Bezeichnung
													   "plant",						           // Modul
													   "biomass growth",				       // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_BiomassGrowth",	               // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function
														
									register_modul_new("GECROS Gross Photosynthesis",  // Bezeichnung
													   "plant",						           // Modul
													   "canopy gross photosynthesis",	       // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_Photosynthesis",	           // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function															
									
								    register_modul_new("GECROS Canopy Formation",  // Bezeichnung
													   "plant",						           // Modul
													   "canopy formation",			   // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_CanopyFormation",	               // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function

									register_modul_new("GECROS Root System Formation",  // Bezeichnung
													   "plant",						           // Modul
													   "root length growth",				           // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_RootSytem",	                   // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function
														
									register_modul_new("GECROS Crop Senescence",  // Bezeichnung
													   "plant",						           // Modul
													   "crop senescence",	       // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_CropSenescence",	           // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function

								    register_modul_new("GECROS Nitrogen Demand",  // Bezeichnung
													   "plant",						           // Modul
													   "nitrogen demand",			   // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_NitrogenDemand",	           // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function

									register_modul_new("GECROS Nitrogen Uptake",  // Bezeichnung
													   "plant",						           // Modul
													   "nitrogen uptake",				       // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_NitrogenUptake",	           // run Funktion
														"gecros_Done_and_FreeMemory"),                                 // done Function
														
									register_modul_new("GECROS Actual Transpiration",  // Bezeichnung
													   "plant",						           // Modul
													   "actual transpiration",	       // Sub Modul
													    "gecros",					           // Klasse
														"gecros_Init_and_AllocMemory",         // load/init Function
														NULL,						           // global run Function
														NULL,						           // mosaic run Function
														"gecros_ActualTranspiration",	       // run Funktion
														"gecros_Done_and_FreeMemory")                                 // done Function															
								   );
								   
}

