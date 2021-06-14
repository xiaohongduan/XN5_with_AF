#include "gecros_alloc.h"

#define FREE_IFN_0(var)\
	if (var!=NULL) g_free(var);\
	var=NULL;

int gecros_alloc_allocateGECROSVariables(gecros *self)
{	
		
		self->pGecrosPlant = g_malloc0(sizeof(struct stGecrosPlant));
			
		self->pGecrosPlant->pGecrosParameter = g_malloc0(sizeof(struct stGecrosParameter));
		self->pGecrosPlant->pGecrosGenotype = g_malloc0(sizeof(struct stGecrosGenotype));
		self->pGecrosPlant->pGecrosBiomass = g_malloc0(sizeof(struct stGecrosBiomass));

		self->pGecrosPlant->pGecrosCarbon = g_malloc0(sizeof(struct stGecrosCarbon));
		self->pGecrosPlant->pGecrosNitrogen = g_malloc0(sizeof(struct stGecrosNitrogen));
		self->pGecrosPlant->pGecrosCanopy = g_malloc0(sizeof(struct stGecrosCanopy));
		self->pGecrosPlant->pGecrosSoil = g_malloc0(sizeof(struct stGecrosSoil));
	
		

	return RET_SUCCESS;
}



int gecros_alloc_freeAllocatedGECROSMemory(gecros *self)
{
	FREE_IFN_0(self->pGecrosPlant->pGecrosSoil);
	FREE_IFN_0(self->pGecrosPlant->pGecrosCanopy);
	FREE_IFN_0(self->pGecrosPlant->pGecrosNitrogen);
	FREE_IFN_0(self->pGecrosPlant->pGecrosCarbon);
	FREE_IFN_0(self->pGecrosPlant->pGecrosBiomass);
	FREE_IFN_0(self->pGecrosPlant->pGecrosGenotype);
	FREE_IFN_0(self->pGecrosPlant->pGecrosParameter);
	FREE_IFN_0(self->pGecrosPlant);
	return RET_SUCCESS;
}




