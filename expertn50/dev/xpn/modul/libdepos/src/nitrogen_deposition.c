//      nitrogen_deposition.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "nitrogen_deposition.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(nitrogen_deposition, nitrogen_deposition, EXPERTN_MODUL_BASE_TYPE);


static void nitrogen_deposition_class_init(nitrogen_depositionClass *klass){}


static void nitrogen_deposition_init(nitrogen_deposition *self)
{
// hier könnnen die Standartwerte der Variablen gesetzt werden oder Speicher reserviert werden!
// Vorsicht: Die Daten der Expert N Basis Klasse sind hier noch nicht gesetzt!
// Werden diese beim Initialisieren benoetigt, so muss eine eigene Init dafuer registriert werden!
}



// Our Modul Functions:
int nitrogen_deposition_load(nitrogen_deposition *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh;
	double fNH4NImisRa,fNO3NImisRa;
//	PTIME pTi = xpn->pTi;
	
	// in Expert-N 3.0 xnm File Marker: 20013
	//Max. NH3-N Volatilisation | NH4-N Deposition | NO3-N Deposition
	// [1/d]            |   [kg N /ha /a]  |   [kg N /ha /a] 
//	----------------------------------------------------------------------
//20013
// 5.e-002				12.				6.				

	// kg N /ha / a
	fNH4NImisRa = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Constant Deposition.nh4",0.0);

	// kg N /ha / a
	fNO3NImisRa = xpn_register_var_get_pointer_convert_to_double(self->parent.pXSys->var_list,"Config.Constant Deposition.no3",0.0);

	// kg N /ha / a -> kg N /ha / d
	pCh->pCProfile->fNH4NImisR = fNH4NImisRa / 365.25;
	
	// kg N /ha / a -> kg N /ha / d
	pCh->pCProfile->fNO3NImisR = fNO3NImisRa / 365.25;

	return RET_SUCCESS;
}

int nitrogen_deposition_run(nitrogen_deposition *self)
  {
       expertn_modul_base *xpn = &(self->parent);
	   
	   PCHEMISTRY pCh = xpn->pCh;
	   PTIME pTi = xpn->pTi;

	   double NH4DeposR = pCh->pCProfile->fNH4NImisR;
	   double NO3DeposR = pCh->pCProfile->fNO3NImisR;

	   pCh->pCLayer->pNext->fNH4N += NH4DeposR * pTi->pTimeStep->fAct;
	   pCh->pCLayer->pNext->fNO3N += NO3DeposR * pTi->pTimeStep->fAct;
    
	pCh->pCBalance->dNInputCum += (NH4DeposR+NO3DeposR)*pTi->pTimeStep->fAct;
       
       return RET_SUCCESS;
  }
