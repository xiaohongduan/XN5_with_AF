#ifndef __EXPERTN_WRFCHEM_DRIVER__
#define __EXPERTN_WRFCHEM_DRIVER__

#include <expertnclass.h>
#include <expertn_noah_driver.h>

typedef struct {
	
	int coupling;

	float dustr1; // dust size rate bin 1: 0.5um effective radius"     "ug/kg-dryair
	float dustr2; // dust size rate bin 2: 1.4um effective radius"     "ug/kg-dryair 
	float dustr3; // dust size rate bin 3: 2.4um effective radius"     "ug/kg-dryair 
	float dustr4; // dust size rate bin 4: 4.5um effective radius"     "ug/kg-dryair 
	float dustr5; // dust size rate bin 5: 8.0um effective radius"     "ug/kg-dryair 

} struct_WRF_CHEM;

#endif// __EXPERTN_WRFCHEM_DRIVER__
