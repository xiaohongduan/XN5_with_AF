#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
	return register_modul_list_new(12,  // Anzahl der Modelle
//	                               register_modul_new("POLLEN Model 1",  // Bezeichnung
//	                                       "plant",						                   // Modul
//	                                       "biomass growth",				           // Sub Modul
//	                                       "pollen",					           // Klasse
//	                                       "pollen_load",				           // load/init Function
//	                                       NULL,						           // global run Function
//	                                       NULL,						           // mosaic run Function
//	                                       "pollen_BiomassGrowth1",	           // run Funktion
//	                                       NULL),                              // done Function	                               
//								   register_modul_new("POLLEN Model 2",  // Bezeichnung
//	                                       "plant",						                   // Modul
//	                                       "biomass growth",				           // Sub Modul
//	                                       "pollen",					           // Klasse
//	                                       "pollen_load",				           // load/init Function
//	                                       NULL,						           // global run Function
//	                                       NULL,						           // mosaic run Function
//	                                       "pollen_BiomassGrowth2",	           // run Funktion
//	                                       NULL),                              // done Function
									register_modul_new("POLLEN ConstRateRelease",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "biomass growth",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_const_release",	           // run Funktion
	                                       NULL),                              // done Function
									register_modul_new("POLLEN SILAM Sofiev",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "biomass growth",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_SILAM_Sofiev",	           // run Funktion
	                                       NULL),                              // done Function
									register_modul_new("POLLEN TSM DT",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_TSM_DT_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_TSM_phenol_dev_dt",  // run Funktion
	                                       NULL),                              // done Function
									register_modul_new("POLLEN TSM DLEN DT",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_TSM_DLEN_DT_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_TSM_phenol_dev_DLEN_dt",  // run Funktion
	                                       NULL),                              // done Function
									register_modul_new("POLLEN TSM WC DT",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_TSM_WC_DT_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_TSM_WC_phenol_dev_dt",  // run Funktion
	                                       NULL),    
									register_modul_new("POLLEN TSM WC DLEN DT",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_TSM_WC_DLEN_DT_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_TSM_WC_DLEN_dt",  // run Funktion
	                                       NULL),     
									register_modul_new("POLLEN TSM DAILY",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_TSM_DAILY_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_TSM_phenol_dev_daily",  // run Funktion
	                                       NULL),                              // done Function
									register_modul_new("POLLEN TSM WC DAILY",  // Bezeichnung
	                                       "plant",						                   // Modul
	                                       "phenological development",				           // Sub Modul
	                                       "pollen",					           // Klasse
	                                       "pollen_TSM_WC_DAILY_load",				           // load/init Function
	                                       NULL,						           // global run Function
	                                       NULL,						           // mosaic run Function
	                                       "pollen_TSM_WC_phenol_dev_daily",  // run Funktion
	                                       NULL),                              // done Function
									register_modul_new("POLLEN MM H1",  // Bezeichnung
	                                       "plant",						              // Modul
	                                       "phenological development",				  // Sub Modul
	                                       "pollen",					              // Klasse
	                                       "pollen_MultiplikationModel_H1_load",		  // load/init Function
	                                       NULL,						              // global run Function
	                                       NULL,						              // mosaic run Function
	                                       "pollen_MultiplikationModel_H1",              // run Funktion
	                                       NULL),                                      // done Function
									register_modul_new("POLLEN MM H2",  // Bezeichnung
	                                       "plant",						              // Modul
	                                       "phenological development",				  // Sub Modul
	                                       "pollen",					              // Klasse
	                                       "pollen_MultiplikationModel_H2_load",		  // load/init Function
	                                       NULL,						              // global run Function
	                                       NULL,						              // mosaic run Function
	                                       "pollen_MultiplikationModel_H2",              // run Funktion
	                                       NULL),                                     // done Function
									register_modul_new("POLLEN MM H3",  // Bezeichnung
	                                       "plant",						              // Modul
	                                       "phenological development",				  // Sub Modul
	                                       "pollen",					              // Klasse
	                                       "pollen_MultiplikationModel_H3_load",		  // load/init Function
	                                       NULL,						              // global run Function
	                                       NULL,						              // mosaic run Function
	                                       "pollen_MultiplikationModel_H3",              // run Funktion
	                                       NULL),                                      // done Function
									register_modul_new("Pollen Release SILAM Sofiev",  // Bezeichnung
	                                       "atmosphere",						              // Modul
	                                       "pollen release",				  // Sub Modul
	                                       "pollen",					              // Klasse
	                                       "Pollen_Release_SILAM_Sofiev_load",		  // load/init Function
	                                       NULL,						              // global run Function
	                                       NULL,						              // mosaic run Function
	                                       "Pollen_Release_SILAM_Sofiev",              // run Funktion
	                                       NULL)                                      // done Function
									
	                              );

}
