//      spass_read.h
//      
//      Copyright 2012 Florian Heinlein <florian.heinlein@helmholtz-muenchen.de>
//      

#ifndef __SPASS__READ_H__
#define __SPASS__READ_H__

#include "spass.h"

G_BEGIN_DECLS

G_MODULE_EXPORT int init_spass_model(spass *self);
G_MODULE_EXPORT int init_spass_interception(spass *self);
G_MODULE_EXPORT int done_spass_model(spass *self);

int array_length(int var1,int var2);
GDate *spass_convert_str_to_gdate(char* S);

G_END_DECLS

#endif /* __SPASS_H__ */
