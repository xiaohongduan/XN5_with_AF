#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(12,  // (!)Anzahl der Modelle(!)
	
	/*1 WATER DEMAND*/				register_modul_new("TREEMIX (incl. Rutter Model)",  // Bezeichnung
													   "plant",									// Modul
													   "potential transpiration",		        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",			// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_potential_transpiration_run",	        	// run Funktion
														NULL) ,
														
	/*2 WATER UPTAKE*/				register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",									// Modul
													   "actual transpiration",		        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,						// mosaic run Function
														"libtreemix_actual_transpiration_run",	        	// run Funktion
														NULL) ,
														
	/*3 PHENOLOGICAL DEVELOPMENT*/	register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",						            // Modul
													   "phenological development",	           	// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_Phenology",					// run Funktion
														NULL) ,
	
	/*4 CANOPY GROSS PHOTOSYNTHESIS*/	register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",						           // Modul
													   "canopy gross photosynthesis",	       // Sub Modul
													    "libtreemix",					       // Klasse
														"libtreemix_load",						           // load/init Function
														NULL,						           // global run Function
														NULL,									// mosaic run Function
														"libtreemix_Photosynthesis",	       // run Funktion
														NULL) ,                                // done Function
														
	/*5 BIOMASS GROWTH*/				register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",						           // Modul
													   "biomass growth",				       // Sub Modul
													    "libtreemix",					       // Klasse
														"libtreemix_load",						           // load/init Function
														NULL,						           // global run Function
														NULL,								   // mosaic run Function
														"libtreemix_BiomassGrowth",	           // run Funktion
														NULL) ,                                // done Function
																							
	/*6 CANOPY FORMATION*/				register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",									// Modul
													   "canopy formation",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_CanopyFormation",	        // run Funktion
														NULL) ,

	/*7 ROOT LENGHT GROWTH*/			register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",									// Modul
													   "root length growth",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_RootLengthGrowth",	        // run Funktion
														NULL) ,

	/*8 NITROGEN DEMAND*/				register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",									// Modul
													   "nitrogen demand",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_NitrogenDemand",	        // run Funktion
														NULL) ,
															
	/*9 NITROGEN UPTAKE*/				register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",									// Modul
													   "nitrogen uptake",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_NitrogenUptake",	        // run Funktion
														NULL) ,	
	
	/*10 SENESCENCE*/					register_modul_new("TREEMIX",  // Bezeichnung
													   "plant",									// Modul
													   "crop senescence",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_Senescence",	        	// run Funktion
														NULL) ,
	/*11 N-MINERALISATION*/					register_modul_new("TREEMIX",  // Bezeichnung
													   "nitrogen",									// Modul
													   "mineralisation",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_NitrogenMineralisation",	        	// run Funktion
														NULL) ,
	/*12 N-NITRIFICATION*/					register_modul_new("TREEMIX",  // Bezeichnung
													   "nitrogen",									// Modul
													   "nitrification",	        			// Sub Modul
													    "libtreemix",					       	// Klasse
														"libtreemix_load",						           	// load/init Function
														NULL,						           	// global run Function
														NULL,									// mosaic run Function
														"libtreemix_NitrogenNitrification",	        	// run Funktion
														NULL)
									
									);
								   
}

