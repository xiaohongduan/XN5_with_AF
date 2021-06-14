//      mpmas_coupling.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __MPMAS_COUPLING_H__
#define __MPMAS_COUPLING_H__

#include <expertn_modul_base.h>
#include "xn_mpmas_interface.h"
#include "expertnclass.h"
//#include "/home/xn-mpmas/xn-mpmas/couplingXNMAS_v3/src/xn_mpmas_interface.h"

G_BEGIN_DECLS

#define MPMAS_COUPLING_TYPE             (mpmas_coupling_get_type())
#define MPMAS_COUPLING(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            MPMAS_COUPLING_TYPE, mpmas_coupling))
#define MPMAS_COUPLING_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),\
            MPMAS_COUPLING_TYPE, mpmas_couplingClass))
#define IS_MPMAS_COUPLING(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            MPMAS_COUPLING_TYPE))
#define IS_MPMAS_COUPLING_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),\
            MPMAS_COUPLING_TYPE))

typedef struct _mpmas_coupling          mpmas_coupling;
typedef struct _mpmas_couplingClass     mpmas_couplingClass;

typedef struct {
    double fTempMax;
    double fTempMin;
    double fTempAve;
    
    double fTempSoil5cm;
    
    double fTimeY_save;
    int iyear_save;
    
} struct_temperature;

struct _mpmas_coupling
{
    expertn_modul_base parent;
    /* add your public declarations here */
    double Plant_Mass;
    
    int new_plant, count;//whether a new plant info has been read
    //Begin of Hong: for turn-off module
    int previousGrid;
    int harvest_done; //whether harvest has been done
    int coverCrop_harvested;//added Troost 180527
    int coverCrop_sowDatecheck_done;//added Troost 180614
	int sowDatecheck_done;//added Troost 180614
    int lastAction_done, checkSwitchDate_done, mainCrop_done;//Hong added on 20180524	
	int irrigation_read;
    int new_management;//whether a new management info has been read
    
    struct_module **XPN_Moduls_full; 
	
	xnmpmasDate lastActionDate;//added Troost 180527
	
	/* Troost removed 180527
	 * xnmpmasDate lastMinFertilDate, lastOrgFertilDate,lastIrrigationDate, lastTillageDate;//Hong added on 20180524
	 */
	//xnmpmasDate dataTransferDate;
	
	char *cropModel; //added on 20180319, to distinguish gecros and CERES/SPASS
    //End of Hong
    
    // Mineral + Organic fertilization
    char **ges_date, **ges_fertilizer, **ges_code;
    double *ges_n_tot, *ges_no3n, *ges_nh4n, *ges_urea;
    double *ges_amount, *ges_dry_matter, *ges_subst;
    
    //für Sortierung
    char *ges_date_zwischen, *ges_fertilizer_zwischen, *ges_code_zwischen;
    double ges_n_tot_zwischen, ges_no3n_zwischen, ges_nh4n_zwischen, ges_urea_zwischen;
    double ges_amount_zwischen, ges_dry_matter_zwischen, ges_subst_zwischen;    
    
    

    
    //Harvest decision
    int daysSinceBBCH1, daysSinceBBCH2;
    int harvestAdaptive;
    int harvestBBCH1;   
    int harvestBBCH1ExtraDays;
    int harvestBBCH2;
    int harvestBBCH2ExtraDays;

    //Fertilization decision
    int nextMinFertAction, nextOrgFertAction;
    
	//Begin of Hong: tillage
	//tillage decision
	int nextTillage, nextIrrigation;	
	int numTill;
	xnmpmasTillage tillage[XNMPMASTILLAGESLOTS];
	//Hong for test:
	xnmpmasDate stopDate;
	//End of Hong
	
    int numMinFert;
    xnmpmasMinFert mineralFertilization[XNMPMASMINFERTSLOTS];
    int numOrgFert;
    xnmpmasOrgFert organicFertilization[XNMPMASORGFERTSLOTS];
    int numIrrig;
    xnmpmasIrrigation irrigation[XNMPMASIRRIGATIONSLOTS];
	
 
    xnmpmasDate internal_actualMinFertDate[XNMPMASMINFERTSLOTS]; // added Troost 180608
    double internal_actualTotalN[XNMPMASMINFERTSLOTS];		// added Troost 180608
    xnmpmasDate internal_actualCoverCropSowDate;// added Troost 180615
    xnmpmasDate internal_actualSowDate;// added Troost 180615
  
    //Count simulation days
    int simulation_days;
    int restart;
    
    struct_temperature temperature;
    struct_temperature __temperature;
    
    // Austausch:
    //STRUCT_WRITE_mpmas_xn *WRITE_mpmas_xn;
    STRUCT_mpmas_to_xn *mpmas_to_xn;
    STRUCT_xn_to_mpmas *xn_to_mpmas;
    STRUCT_xn_to_mpmas2 *xn_to_mpmas2; 
	
	int nmin_measured;
};


// public class member function:
G_MODULE_EXPORT int mpmas_coupling_Austausch(mpmas_coupling *self);
G_MODULE_EXPORT int mpmas_coupling_Load(mpmas_coupling *self);

int checkIfHarvest(mpmas_coupling *self);
int checkIfMineralFertilization(mpmas_coupling *self);
int checkIfOrganicFertilization(mpmas_coupling *self);
int get_daily_air_and_soil_temperatures(mpmas_coupling *self);

//Hong: for irrigation tillage 2017/01/12
int checkIfIrrigation(mpmas_coupling *self);
int checkIfTillage(mpmas_coupling *self);
//End of Hong

void measureNmin(mpmas_coupling *self, int store_measures);

struct _mpmas_couplingClass
{
    expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType mpmas_coupling_get_type (void);

G_END_DECLS

#endif /* __MPMAS_COUPLING_H__ */

