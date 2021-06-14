#ifndef __GECROS__ALLOC_H__
#define __GECROS__ALLOC_H__

#include "gecros.h"

G_BEGIN_DECLS

int gecros_alloc_allocateGECROSVariables(gecros *self);
int gecros_alloc_freeAllocatedGECROSMemory(gecros *self);

G_END_DECLS

#endif /* __GECROS_H__ */

