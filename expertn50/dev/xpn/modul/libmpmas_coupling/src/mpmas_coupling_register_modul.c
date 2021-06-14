#include "xpn_register_modul.h"
#include <stdio.h>
#include <stdlib.h>

G_MODULE_EXPORT struct_register_modul_list *ExpertN_Lib_Register()
{
    return register_modul_list_new(1,  // Anzahl der Modelle
                                   register_modul_new("mpmas coupling",  // Bezeichnung
                                           "other",                                        // Modul
                                           "mpmas",                        // Sub Modul
                                           "mpmas_coupling",                               // Klasse
                                           "mpmas_coupling_Load",                                  // load/init Function
                                           NULL,                                   // global run Function
                                           NULL,                                   // mosaic run Function
                                           "mpmas_coupling_Austausch",             // run Funktion
                                           NULL)                                                        // done Function
                                  );

}
