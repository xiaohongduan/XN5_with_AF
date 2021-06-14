#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
    return register_modul_list_new(1,  // Anzahl der Modelle
                                    register_modul_new("OPEN_GEO_SYS_DRIVER",  // Bezeichnung
                                                       "other",                                        // Modul
                                                       "ogs",                          // Sub Modul
                                                        "open_geo_sys_driver",                             // Klasse
                                                        "open_geo_sys_driver_load",                                // load/init Function
                                                        "open_geo_sys_driver_run_global",                                  // global run Function
                                                        "open_geo_sys_driver_run_mosaic",                                  // mosaic run Function
                                                        "open_geo_sys_driver_run",             // run Funktion
                                                        "open_geo_sys_driver_done")                                  // done Function                                                                                       
                                   );
                                   
}

