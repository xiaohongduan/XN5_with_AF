//      century_n.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __CENTURY_N_H__
#define __CENTURY_N_H__

#include <expertn_modul_base.h>
#include "century_n_util.h"

G_BEGIN_DECLS

#define CENTURY_N_TYPE				(century_n_get_type())
#define CENTURY_N(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			CENTURY_N_TYPE, century_n))
#define CENTURY_N_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			CENTURY_N_TYPE, century_nClass))
#define IS_CENTURY_N(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			CENTURY_N_TYPE))
#define IS_CENTURY_N_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			CENTURY_N_TYPE))

typedef struct _century_n			century_n;
typedef struct _century_nClass		century_nClass;



struct _century_n
{
	expertn_modul_base parent;

	// Config_File:
	char *config_filename;

	
	
	// Parameter:
	
	int INIT_DONE;
	
	// Variablen:
	struct_fixedparameter fixed;
	//struct_parameter par;
	double CumCorrection;
	
	// century_nitrify:
	double ppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
	double NOxPulse[12];//NOx pulse factor of the next 12 days 
	
	double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
	
	double fPreciRate; // Durchschnittliche Regenrate (auf den Tag bezogen)
	
	// century_ditrify:
	double dppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
								
	int denit_daycount;
	int nitri_daycount;
	double dNOxPulse[12];//NOx pulse factor of the next 12 days 
	
	double dkPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
	
	double dfPreciRate; // Durchschnittliche Regenrate (auf den Tag bezogen)
	
	double monthAvTempAmpl; // Durchschnittliche Regerate pro Monate
	
	int iForest;
	double fdrain;
	
	double *fCHumus_old; //Hong on 20190509
	
	
};


// public class member function:
G_MODULE_EXPORT int  century_n_Mineralisation_init(century_n *self);
G_MODULE_EXPORT int  century_n_Mineralisation_run(century_n *self);
G_MODULE_EXPORT int  century_n_Mineralisation_done(century_n *self);
G_MODULE_EXPORT void century_n_decf20cm(century_n *self, double * defac, double * anerb, struct_fixedparameter fixed,double drain);

// decomposition factor of soil layer
double century_n_decfSL(century_n *self,double sand, // [%] in Soil layer
			  double WFPS, // Water filled pore space = pWL->fContAct / pSo->pSLayer->fPorosity
			  double SoilTemp, //pHe->pHLayer->fSoilTemp
			  struct_fixedparameter fixed);
			  
/********************************************************************
Partition Residue: Input: source pool of residue, C amount, ratio residue: N/C, lignin fraction
Partions the residue into structual and metabolic and schedules flows.

returns direct adsorbed mineral N [g/m2]

NB: no direct adsorption of Surface litter
    arguments: C/N/lig of TMP struc/metab layer
*********************************************************************/

double century_n_PartitionResidue(century_n *self,
    double C,			// C Content g/m2 of litter
    double recres,		//  residue: ratio N/C
    double frlig,		// // fraction of incoming material = lignin.
    int layer,			// SRFC or SOIL
    double Navailable,   //available N, either in first 20 or in XN soil layer
    //for direct adsorbtion
    double *newmetabC,      //return values: new C,N of struc and metab pool,
    double *newmetabN,
    double oldstrucC,
    double *newstrucC,
    double *newstrucN,
    double oldlig,
    double *newlig,		//               adjusted lignin content of structural pool
    struct_fixedparameter fixed);

//sum all available mineral N=Nitrate+Ammonium to depth
// return g/m2

//	AboveGroundDecompRatio
// 	Calculates the aboveground decomposition ratio.
// 	Determine C/N of new material entering 'Box B'.
//
// 	The C/N ratios for structural and wood can be computed once;
// 	they then remain fixed throughout the run.  The ratios for
// 	metabolic and som1 may vary and must be recomputed each time step.
// 	Returns the C/N ratio of new N;  = function of the N content of the
// 	material being decomposed.
double century_n_AboveGroundDecompRatio (century_n *self,double totalCinA ,double totalNinA,double biofac,//amount of C and N in box A
                               //biomass factor: 2.0 for wood, 2.5 else
                               struct_fixedparameter fixed);

double century_n_FlowCintoSOM2(century_n *self,double Ctoadd,			// amount of C to add (gN/m^2)
                     double depth);				// soil depth to receive mineral E (cm)
double century_n_FlowNintoSOM2(century_n *self,double Ntoadd,			// amount of organic N to add (gN/m^2)
                     double depth);				// soil depth to receive mineral E (cm)
                     

double century_n_FlowCintoSOM1(century_n *self,double Ctoadd,			// amount of C to add (gN/m^2)
                     double depth);				// soil depth to receive mineral E (cm)
                

double century_n_FlowNintoSOM1(century_n *self,double Ntoadd,			// amount of organic N to add (gN/m^2)
                     double depth);				// soil depth to receive mineral E (cm)
					 
double century_n_CO2intoSOIL(century_n *self,double CO2toadd,			// amount of organic N to add (gN/m^2)
                   double depth);				// soil depth to receive mineral E (cm)
				   
//Following funktions are for debugging purpose
//it returns the total mineral N in TMP Clayers [kg/ha]
double century_n_getN(century_n *self,PCLAYER  pCL);
double century_n_getNH4(century_n *self,PCLAYER  pCL);
double century_n_getNO3(century_n *self,PCLAYER  pCL);

int century_n_CanDecompose (century_n *self,
    double const C,		// total C in Box A
    double const N,		//   N in Box A
    double const ratio, //   C/N,  ratios of new material being added to Box B
    double availMinN);
	
void century_n_ScheduleNFlow(century_n *self,
    double CFlowAtoB,
    double FlowRatioCtoN,
    double AC,double AN,	//C and N content of Box A (source)
    double availMinN,	//available mineral N for possible Immobilisation
    //return values
    double *orgNflow,
    double *minNflow		//negative value for immobilisation
);

double century_n_FlowNintoMineralSoil(century_n *self,double Ntoadd,			// amount of mineral N to add (gN/m^2)
                            double depth,				// soil depth to receive mineral E (cm)
                            double fractionToNitrate	// fraction of N to go to Nitrate range = (0-1)
                            );
							
double century_n_FlowNfromMineralSoil(century_n *self,double Ndemand,		// amount of mineral N wanted (gE/m^2)
                            double depth,			// soil depth to give mineral E (cm)
                            double useNitrateFrac	// max fraction removed from NOx pools range = (0-1),
                            // -1 = remove in proportion to nitrate/ammonium available
                            );
							
double century_n_getNtoDepth (century_n *self,double depth);

// getH2OtoDepth
// depth= -1 : return water content of all layers
double century_n_getH2OtoDepth (century_n *self,double depth);
double century_n_getWFPStoDepth(century_n *self,double depth);
//returns percentage of sand content
double century_n_getSandtoDepth(century_n *self,double depth);
                   
                     
G_MODULE_EXPORT int  century_n_Nitrification_init(century_n *self);
G_MODULE_EXPORT int  century_n_Nitrification_run(century_n *self);
G_MODULE_EXPORT int  century_n_Nitrification_done(century_n *self);
double century_n_Diffusivity (century_n *self,
	double const A,		// fraction of soil bed volume occupied by
				//   field capacity
				//   (intra-aggregate pore space, 0-1)
	double const bulkDen,	// bulk density of soil (g/cm^3)
	double const porosity,	// porosity fraction (0-1)
	double const wfps);	// Water Filled Pore Space fraction
				// (volumetric swc/porosity, 0-1)
double century_n_f_arctangent(century_n *self,double x, double A[]);
double century_n_f_gen_poisson_density(century_n *self,double x, double A[]);

G_MODULE_EXPORT int  century_n_Denitrification_init(century_n *self);
G_MODULE_EXPORT int  century_n_Denitrification_run(century_n *self);
G_MODULE_EXPORT int  century_n_Denitrification_done(century_n *self);

void century_n__general_init(century_n *self);
int century_n_load_config(century_n *self,const char* configfile);


struct _century_nClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType century_n_get_type (void);

/*#define C_DEBUG(var) \
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i) \
	{\
	gchar *S;\
	S  = g_strdup_printf("%s[%d]",#var,i);\
	xpn_register_var_add_double(self->parent.pXSys->var_list,S,var,__LINE__,TRUE); \
	g_free(S);\
	}\
*/	

#define C_DEBUG(var)
#define C_DEBUG_LOC_IN_ARRAY(var,i)

G_END_DECLS

#endif /* __CENTURY_N_H__ */

