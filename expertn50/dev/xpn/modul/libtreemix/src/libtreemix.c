//      libtreemix.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "libtreemix.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(libtreemix, libtreemix, EXPERTN_MODUL_BASE_TYPE);


static void libtreemix_class_init(libtreemixClass *klass){}


static void libtreemix_init(libtreemix *self)
{
	
	
//gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
//self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
//S  = g_strdup_printf("%s\t%d\tInit MODUL LIBTREEMIX!",__FILE__,__LINE__);
//fprintf(stdout,"%s\n",S);
//fflush(stdout);
//g_free(S);
self->__INIT_DONE=0;
self->__ERROR=RET_SUCCESS;
}



// Our Modul Functions:

