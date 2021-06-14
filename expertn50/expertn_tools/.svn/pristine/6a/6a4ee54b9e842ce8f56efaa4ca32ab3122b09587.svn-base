//      ${template}.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "${template}.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(${template}, ${template}, EXPERTN_MODUL_BASE_TYPE);


static void ${template}_class_init(${template}Class *klass){}


static void ${template}_init(${template} *self)
{
gchar *S;
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!

// init example var:
self->Plant_Mass = 20.0;
// print Output Message if Modul is used from Expert N
S  = g_strdup_printf("%s\t%d\tInit MODUL ${templ_uppercase}!",__FILE__,__LINE__);
fprintf(stdout,"%s\n",S);
fflush(stdout);
g_free(S);
}



// Our Modul Functions:

int ${template}_BiomassGrowth(${template} *self)
  {
       expertn_modul_base *xpn = EXPERTN_MODUL_BASE(self);
       PPLANT pPl = xpn->pPl;
 
       self->Plant_Mass = 0.001; // Change Plant Mass
       pPl->pBiomass->fTotalBiomass = self->Plant_Mass;
       

       return RET_SUCCESS;
  }

int ${template}_Photosynthesis(${template} *self)
  {
      // ...
      // do anything
      // ....
      return RET_SUCCESS;
  }
