#include "hpm_nitrogen.h"
#include "hpm_math.h"
#include "hpm_growth.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


int hpm_nitrogen_uptake(hpm *self)
{
	
	set_SoilVars_N(self);
				
	return self->__ERROR;	
}

void set_SoilVars_N(hpm *self) {
	expertn_modul_base *xpn = &(self->parent);
	PWLAYER pWL;
	PSWATER pSW;
	PSLAYER pSL;
	PCLAYER pCL;
	PLAYERROOT pLR;
	//Added by hong
	PPLANT     pPl = xpn->pPl; 
	PCHEMISTRY pCh = xpn->pCh;
	PSPROFILE pSo = xpn->pSo;
	PPLTNITROGEN pPltN;
	PCBALANCE	pCB = pCh->pCBalance; //Added by Hong on 20181030
    //End of Hong 
	double fDepth;
	double fRootDepth;
	double nh4sum,no3sum;
	double dt;
	int count,layer_counter;
	double fCumDepth;
	
	fDepth = 0.0;
	fRootDepth = (double)(xpn->pPl->pRoot->fDepth*1.0e-3);
	dt =  xpn->pTi->pTimeStep->fAct;
	count = 0;

	// Summe bilden:
	nh4sum = 0.0;
	no3sum = 0.0;	

	
	pWL = xpn->pWa->pWLayer->pNext;
	pSW = xpn->pSo->pSWater->pNext;
	pSL = xpn->pSo->pSLayer->pNext;
	pCL = xpn->pCh->pCLayer->pNext;
	//PCLAYER 	pSLN=pCh->pCLayer->pNext;
	//pCL = pCh->pCLayer;

	// Ausrechnen wieviel N im Boden ist:
	while(fDepth<=fRootDepth) {
		if (count >= xpn->pSo->iLayers-1) break;
		fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m
		count++;

		nh4sum += (double)pCL->fNH4N;
		no3sum += (double)pCL->fNO3N;

		pSL = pSL->pNext;
		pSW = pSW->pNext;
		pWL = pWL->pNext;
		pCL = pCL->pNext;

	}

	fDepth = 0.0;
	count = 0;
	layer_counter=0;

	pWL = xpn->pWa->pWLayer->pNext;
	pSW = xpn->pSo->pSWater->pNext;
	pSL = xpn->pSo->pSLayer->pNext;
	pCL = xpn->pCh->pCLayer->pNext;
	pLR = xpn->pPl->pRoot->pLayerRoot->pNext;

	xpn->pPl->pPltNitrogen->fActNUptR=0.0;
	
	while(fDepth<=fRootDepth) 
	{
		if (layer_counter >= xpn->pSo->iLayers-1) break;
		fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m
		layer_counter++;
	}
	
	fDepth = 0.0;
    fCumDepth =0.0;
	while(fDepth<=fRootDepth) {
		if (count >= xpn->pSo->iLayers-1) break;
		fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m
		count++;

		pLR->fActLayNH4NUpt = 0.0;
		pLR->fActLayNO3NUpt = 0.0;

		if (nh4sum!=0) {
			pLR->fActLayNH4NUpt = ((pCL->fNH4N/nh4sum * (self->Litter.INammrt_so)) * mq_TO_ha_1);
			//Added by Hong
			if (pCL->fNH4N>=pLR->fActLayNH4NUpt * dt)
			   {
				pCL->fNH4N-= pLR->fActLayNH4NUpt * dt;
			   }
			else
			    pLR->fActLayNH4NUpt=0.0;
/*			
			pCL->fNH4N-= pLR->fActLayNH4NUpt * dt;
			if (pCL->fNH4N<0.0)
				{
					pCL->fNH4N=0.0;
					pLR->fActLayNH4NUpt=0.0;					
				}
*/				
			xpn->pPl->pPltNitrogen->fActNUptR += pLR->fActLayNH4NUpt;
			
		}
		if (no3sum!=0) {

			pLR->fActLayNO3NUpt = (float)((pCL->fNO3N /no3sum * (self->Litter.INitrt_so)) * mq_TO_ha_1);
			
			//Added by Hong
			if (pCL->fNO3N>=pLR->fActLayNO3NUpt * dt)
			   {
				pCL->fNO3N-= pLR->fActLayNO3NUpt * dt;
			   }
			else
			    pLR->fActLayNO3NUpt=0.0;
/*			
			pCL->fNO3N-= pLR->fActLayNO3NUpt * dt;
			if (pCL->fNO3N < 0.0)
			{
				pCL->fNO3N=0.0;
				pLR->fActLayNO3NUpt=0.0;
			}
*/			
			xpn->pPl->pPltNitrogen->fActNUptR += pLR->fActLayNO3NUpt;
	
		}
		
		pCL->fNLitter +=((self->Litter.RONrt_li/(double)layer_counter) * mq_TO_ha_1);
		pCL->fNLitter+=((self->Litter.RONsh_li/(double)layer_counter) * mq_TO_ha_1);
		pCL->fCLitter+=((self->Litter.ROCsh_li/(double)layer_counter) * mq_TO_ha_1);
		pCL->fCLitter+=((self->Litter.ROCrt_li/(double)layer_counter) * mq_TO_ha_1);
		
		//Added by Hong
		pCL->fCLitter +=((self->Litter.OXrt_li/(double)layer_counter) *dt * mq_TO_ha_1);
		pCL->fCLitter +=((self->Litter.OCSrt_so/(double)layer_counter) *dt* mq_TO_ha_1);
		pCB->dCInputProfile += ((self->Litter.OXrt_li/(double)layer_counter)+(self->Litter.OCSrt_so/(double)layer_counter)) * dt* mq_TO_ha_1;
		//pCB->dCInputCum += ((self->Litter.OXrt_li/(double)layer_counter)+(self->Litter.OCSrt_so/(double)layer_counter)) * dt* mq_TO_ha_1;
		//Hong 20190507: balance for 0-30 cm profile:	
	    fCumDepth +=(double)0.1*pSL->fThickness; //cm
	    if (fCumDepth <=30.0)
	      {
			 pCB->dCInputProfile_30 += ((self->Litter.OXrt_li/(double)layer_counter)+(self->Litter.OCSrt_so/(double)layer_counter)) * dt* mq_TO_ha_1;
          } 
		
		//End of Hong
		
		
		pSL = pSL->pNext;
		pSW = pSW->pNext;
		pWL = pWL->pNext;
		pCL = pCL->pNext;
		pLR = pLR->pNext;

	}
		
	//if (nh4sum!=0) pLR[i]->fActLayNH4NUpt = pClS[i]->fNH4N/nh4sum * (float)((Litter.INammrt_so-Litter.ONammrt_so) * mq_TO_ha_1);
	//if (no3sum!=0) pLR[i]->fActLayNO3NUpt = pClS[i]->fNO3N/no3sum * (float)((Litter.INitrt_so-Litter.ONammrt_so) * mq_TO_ha_1);	 

    //Added by Hong
	pPl->pPltNitrogen->fCumActNUpt += pPl->pPltNitrogen->fActNUptR*dt;

}
