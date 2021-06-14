//      wrfchem_coupling.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "wrfchem_coupling.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(wrfchem_coupling, wrfchem_coupling, EXPERTN_MODUL_BASE_TYPE);


static void wrfchem_coupling_class_init(wrfchem_couplingClass *klass){}


static void wrfchem_coupling_init(wrfchem_coupling *self){
	self->WRF_CHEM=NULL;
    self->__ERROR=RET_SUCCESS;	
	}



// Our Modul Functions:

int wrfchem_coupling_load(wrfchem_coupling *self)
{
	expertn_modul_base *xpn = &(self->parent);
    PXSYSTEM pXSys = xpn->pXSys;
    //PTIME pTi = self->parent.pTi;
    //xpn_register_var *var_list = self->parent.pXSys->var_list;
    char *S;
    int i;


    self->WRF_CHEM=NULL;
    self->own=NULL;
    self->own_link=NULL;
    self->own_link_count=0;
    self->class_link=NULL;
    self->class_link_count=0;
      

    // checken, ob ExpertN an WRF_CHEW hängt, je nachdem werden später Wetterdaten geladen, falls ExpertN als standalone arbeitet
    self->WRF_CHEM=(struct_WRF_CHEM*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"pWRF_CHEM");



    if (self->WRF_CHEM!=NULL) {
        struct_WRF_CHEM *wrf_chem;        
        wrf_chem = self->WRF_CHEM;
        S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Config.WRF CHEM Coupling.wrfchem_coupling");
        if (S==NULL) {
            S = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.wrfchem_coupling");
            if (S==NULL) {
                S = "0";
                fprintf(stderr,"ERROR: Varname is not defined (%s or %s), take default value (%s)!\n","Config.WRF CHEM Coupling.wrfchem_coupling","Global_Config.options.wrfchem_coupling",S);
            }
        }
        wrf_chem->coupling = atof(S);

        //var_list = xpn->pXSys->var_list;
        
		//STRUCT_DRIVE_XPN_REG(wrf,"WRF_CHEM");
       

        self->own = (struct_WRF_CHEM*)g_malloc0(sizeof(struct_WRF_CHEM));

        //STRUCT_DRIVE_XPN_REG(self->own,"own");

        xpn_register_var_add_none(self->parent.pXSys->var_list,"WRF_CHEM Coupling.own",self->own,-1,TRUE);


        self->class_link_count =pXSys->id_M;
        self->class_link = (expertnclass**)g_malloc0(sizeof(expertnclass*)*self->class_link_count);
        for (i=0; i<self->class_link_count; i++) {
            self->class_link[i] = pXSys->xpn_classes[pXSys->id_Mosaic[i]];
        }

    }
	return RET_SUCCESS;	
}

int wrfchem_coupling_mosaic_run(wrfchem_coupling *self)
{
	expertn_modul_base *xpn = &(self->parent);
    PXSYSTEM pXSys = xpn->pXSys;
    struct_WRF_CHEM* wrf_chem = self->WRF_CHEM;
    int o,i;
    expertnclass *xpnclass;


    if (wrf_chem==NULL) return RET_SUCCESS;

    self->__ERROR = RET_SUCCESS;

    if (wrf_chem->coupling > -1) {

        if ((pXSys->id_M>0) && (pXSys->id==pXSys->id_Mosaic[0])) { // Nur Master macht das!!!!
            if (xpn->pTi->pSimTime->bFirstRound==TRUE) {
                self->own_link_count=pXSys->id_M;
                self->own_link = (struct_WRF_CHEM **)g_malloc0(sizeof(struct_WRF_CHEM *)*self->own_link_count);
                for (i=0; i<self->own_link_count; i++) {
                    xpnclass = pXSys->xpn_classes[pXSys->id_Mosaic[i]];
                    self->own_link[i] = xpn_register_var_get_pointer(xpnclass->pXSys->var_list,"WRF_CHEM Coupling.own");
                }

            }
            
			wrf_chem->dustr1 = 0.0;
			wrf_chem->dustr2 = 0.0;
			wrf_chem->dustr3 = 0.0;
			wrf_chem->dustr4 = 0.0;
			wrf_chem->dustr5 = 0.0;

            for (o=0; o<self->own_link_count; o++) {
                struct_WRF_CHEM *src;
                double frac;
                src = self->own_link[o];
                frac = self->class_link[o]->pXSys->landuse_cover_fraction;                
				
                wrf_chem->dustr1 += src->dustr1*frac;
				wrf_chem->dustr2 += src->dustr2*frac;
				wrf_chem->dustr3 += src->dustr3*frac;
				wrf_chem->dustr4 += src->dustr4*frac;
				wrf_chem->dustr5 += src->dustr5*frac;							
                
            }
        }


    }	
    
	return RET_SUCCESS;	
}

int wrfchem_coupling_run(wrfchem_coupling *self)
{
//	expertn_modul_base *xpn = &(self->parent);

    if (self->WRF_CHEM==NULL) {
        return RET_SUCCESS;
    }

    self->__ERROR = RET_SUCCESS;

    //wrfchem_coupling_write_copy_wrfchem_struct_only_climate(self->own,self->WRF_CHEM);

    wrfchem_coupling_write_own(self);
	return RET_SUCCESS;	
}

int wrfchem_coupling_done(wrfchem_coupling *self)
{
//	expertn_modul_base *xpn = &(self->parent);
    if (self->WRF_CHEM==NULL) {
        return RET_SUCCESS;
    }
    if (self->own!=NULL) {
        
        g_free(self->own);
        self->own=NULL;
    }

    
	return RET_SUCCESS;	
}


void wrfchem_coupling_write_copy_wrfchem_struct_only_climate(struct_WRF_CHEM* dest, struct_WRF_CHEM* src)
{   
    dest->dustr1 =src->dustr1;
	dest->dustr2 =src->dustr2;
	dest->dustr3 =src->dustr3;
	dest->dustr4 =src->dustr4;
	dest->dustr5 =src->dustr5;
}

void  wrfchem_coupling_write_own(wrfchem_coupling *self)
{
    expertn_modul_base *xpn = &(self->parent);
    




    if ((self->own->coupling<0) || (self->own->coupling>1)) return;


	self->own->dustr1 = xpn->pCh->pAtmos->dustr1;
	self->own->dustr2 = xpn->pCh->pAtmos->dustr2;
	self->own->dustr3 = xpn->pCh->pAtmos->dustr3;
	self->own->dustr4 = xpn->pCh->pAtmos->dustr4;
	self->own->dustr5 = xpn->pCh->pAtmos->dustr5;
		
}
