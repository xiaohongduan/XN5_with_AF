#ifndef XN_MPMAS_INTERFACE_H_
#define XN_MPMAS_INTERFACE_H_

#include <expertn_modul_base.h>

#define XNCROPCODELENGTH 2
#define XNFERTCODELENGTH 5
#define XNORGFERTCODELENGTH 19
#define XNNAMELENGTH 63
#define XNMPMASMINFERTSLOTS 4
#define XNMPMASORGFERTSLOTS 2
#define XNMPMASIRRIGATIONSLOTS 10
//Begin of Hong: add tillage
#define XNTILLCODELENGTH 5
#define XNMPMASTILLAGESLOTS 6 //? adequate size?
//End of Hong

#define XNMPMASDAYSOFYEAR 366

//XNMPMASVERSION 9

//G_BEGIN_DECLS



typedef struct {
    int day;
    int month;
    int year;
} xnmpmasDate;

typedef struct {
    xnmpmasDate fertDate;
    char code[XNFERTCODELENGTH + 1];
    double no3n; //kg N
    double nh4n;//kg N
    double urea;//kg N
    int adaptive;
    int bbch;
    double nminAdapt_factor;
    double nminAdapt_ref;
    int nminAdapt_depth;
} xnmpmasMinFert;

typedef struct {
    xnmpmasDate orgfertDate;
    char code [XNORGFERTCODELENGTH + 1];
    double drymatter; //kg
    double nh4n; // kg N
    double n_tot_org;//kg 
    int adaptive;
    int bbch;
} xnmpmasOrgFert;

typedef struct {
    xnmpmasDate irrDate;
    double quantity; //mm
} xnmpmasIrrigation;

//NEW ct 170109
typedef enum adaptiveTillageType {
	adaptiveTillageNotAdaptive =0,
	adaptiveTillageBeforeSowing,
	adaptiveTillageBeforeCoverCrop,
	adaptiveTillageAfterHarvest
} adaptiveTillageType;
//Begin of Hong: add tillage
typedef struct {
    xnmpmasDate tillDate;
	char code[XNTILLCODELENGTH + 1];
    double fDepth; 
	adaptiveTillageType typeAdaptiveTillage;
	int daysBeforeAfter;
} xnmpmasTillage;
//End of Hong

typedef struct  {
//Simulation end
    xnmpmasDate stopDate;
//Crop rotation
    char CropCode[XNCROPCODELENGTH + 1];
    char CropName[XNNAMELENGTH +1 ];
    xnmpmasDate sowDate;
    int maxSowDelay;
    char variety[XNNAMELENGTH + 1];
    double sowDens;
    double rowDist; 
    //double rootDepth;//?? better define according to soil in default_crop_rotation.ini file
    double sowDepth;
    xnmpmasDate harvestDate; //record yield data here

    int harvestAdaptive;
    int harvestBBCH1;
	
	//Begin of Hong: coverCrop
	char coverCropCode[XNCROPCODELENGTH + 1];
	char coverCropName[XNNAMELENGTH +1 ];
	xnmpmasDate coverCropSowDate;
	char coverCropVariety[XNNAMELENGTH + 1];
	double coverCropSowDens; // if necessary?
	double coverCropRowDist;  // if necessary?
	double coverCropSowDepth;  // if necessary?
	int  coverCropMaxSowDelay;
	xnmpmasDate coverCropPloughUnderDate;	
	//End of Hong
	
//NEW:  
    int harvestBBCH1ExtraDays;
//END NEW   
    int harvestBBCH2;
    int harvestBBCH2ExtraDays;

    int biom_remove;
//Fertilization
    int numMinFert;
    xnmpmasMinFert mineralFertilization[XNMPMASMINFERTSLOTS];
    int numOrgFert;
    xnmpmasOrgFert organicFertilization[XNMPMASORGFERTSLOTS];
    int numIrrig;
    xnmpmasIrrigation irrigation[XNMPMASIRRIGATIONSLOTS];
	
	//Todo: to be activated
	//Begin of Hong: add tillage
	int numTill;
    xnmpmasTillage tillage[XNMPMASTILLAGESLOTS];
	//End of Hong
    
    // Begin of Hong: for multigrid 
    expertn_modul_base **xpn_other_grids;
    expertn_modul_base  *xpn_own_grid;
    int number_of_grids;
    int own_grid_number;
    int currentGrid;
    int nextGrid;   
    int cellID; 
	int updateManagement; //Hong: to indicate timing for reading management input and timing for data transfer
    // End of Hong
} STRUCT_mpmas_to_xn;


typedef struct {
    double fruitDryWeight; // kg/ha
    double stemLeafDryWeight;
    double stemOnlyDryWeight;
    double Nmin0_30;
    double Nmin30_60;
    double Nmin60_90;
    //Bodenfeuchte (klären welche Schichten)
	
	//Begin of Hong
	xnmpmasDate actualHarvestDate; // to transfer the actual harvest date
	xnmpmasDate actualMinFertDate[XNMPMASMINFERTSLOTS]; // to transfer the actual min-fertilisation date
	double actualTotalFertN[XNMPMASMINFERTSLOTS];
	
	xnmpmasDate actualSowDate; 
	xnmpmasDate actualCoverCropSowDate; 

	
	
	int cellID; // to check the cellID from mpmas_to_xn
	//End of Hong
	
} STRUCT_xn_to_mpmas;


typedef struct {
  double airTemp[XNMPMASDAYSOFYEAR];
  int startDay;
  double topsoilTemp[XNMPMASDAYSOFYEAR]; 
  int stopDay;
} STRUCT_xn_to_mpmas2;  
//

//G_END_DECLS

#endif
