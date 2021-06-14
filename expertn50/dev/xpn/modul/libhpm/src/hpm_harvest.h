//      hpm_harvest.h
//      
//      Copyright 2013 Christian Klein <chrikle@berlios>
//      



#ifndef __HPM__HARVEST_H__
#define __HPM__HARVEST_H__

#include "hpm.h"

G_BEGIN_DECLS

// from hpm.h
/*
G_MODULE_EXPORT int hpm_harvest_init(hpm *self);
G_MODULE_EXPORT int hpm_harvest_run(hpm *self);
G_MODULE_EXPORT int hpm_harvest_done(hpm *self);
*/
 
int load_harvest_config(hpm *self,const char* configfile);
int hpm_harvest_register_var(hpm *self);

G_END_DECLS

#endif