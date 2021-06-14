//      hpm_ini.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __TREEMIX__INI_H__
#define __TREEMIX__INI_H__

#include "libtreemix.h"

//#define PI	3.1415926536

G_BEGIN_DECLS


G_MODULE_EXPORT int libtreemix_load(libtreemix *self);
G_MODULE_EXPORT int load_config(libtreemix *self,const char* configfile);
G_MODULE_EXPORT int load_parameter(libtreemix *self,const char* paramfile, int i);
G_MODULE_EXPORT int register_var(libtreemix *self);

G_MODULE_EXPORT int set_start_values_plant(libtreemix *self, int i);
G_MODULE_EXPORT int set_climate(libtreemix *self);



G_END_DECLS

#endif /* __TREEMIX_H__ */