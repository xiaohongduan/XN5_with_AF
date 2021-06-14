//      spass_growth.h
//      
//      Copyright 2012 Florian Heinlein <florian.heinlein@helmholtz-muenchen.de>
//      



#ifndef __SPASS__GROWTH_H__
#define __SPASS__GROWTH_H__

#include "spass.h"

G_BEGIN_DECLS

//Assimilation, dissimilataion and growth
static int				spass_DefaultAssimilatePartitioning(spass *self);
static int				spass_AssimilatePartition(spass *self);
static int				spass_OrganBiomassGrowth(spass *self);
static int				spass_ReserveTranslocation(spass *self);

G_END_DECLS

#endif /* __SPASS_H__ */

