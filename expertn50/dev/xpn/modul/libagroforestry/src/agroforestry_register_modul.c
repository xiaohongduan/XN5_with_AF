#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
    return register_modul_list_new(1,  // Anzahl der Modelle
                                   register_modul_new("Strip next to trees",  // Bezeichnung
                                           "management",                                        // Modul
                                           "agroforestry",                           // Sub Modul
                                           "agroforestry",                             // Klasse
                                           "init_agroforestry",                     // load/init Function
                                           NULL,                                   // global run Function
                                           NULL,                                   // mosaic run Function
                                           "agroforestry_run",               // run Funktion
                                           "agroforestry_done")                                                       // done Function
                                  );

}
