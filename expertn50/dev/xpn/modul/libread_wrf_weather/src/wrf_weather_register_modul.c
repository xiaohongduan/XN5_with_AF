#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
        return register_modul_list_new(2,  // Anzahl der Modelle
                                                                        register_modul_new("use wrf weather file",  // Bezeichnung
                                                                                                           "control",                                                              // Modul
                                                                                                           "input",                                        // Sub Modul
                                                                                                            "wrf_weather",                                                 // Klasse
                                                                                                                "wrf_weather_load",                                                        // load/init Function
                                                                                                                NULL,                                                      // global run Function
                                                                                                                NULL,                                                      // mosaic run Function
                                                                                                                "wrf_weather_run",                 // run Funktion
                                                                                                                "wrf_weather_done"),                                                       // done Function
                                                                        register_modul_new("test coupling",  // Bezeichnung
                                                                                                           "control",                                                              // Modul
                                                                                                           "input",                                        // Sub Modul
                                                                                                            "wrf_weather",                                                 // Klasse
                                                                                                                "wrf_weather_coupling_load",                                                       // load/init Function
                                                                                                                NULL,                                                      // global run Function
                                                                                                                NULL,                                                      // mosaic run Function
                                                                                                                "wrf_weather_coupling_run",                // run Funktion
                                                                                                                "wrf_weather_coupling_done")                                                                                                                    
                                                                   );
                                                                   
}

