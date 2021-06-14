//      agroforestry.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __AGROFORESTRY_H__
#define __AGROFORESTRY_H__

#include <expertn_modul_base.h>
#include <expertnclass.h>

G_BEGIN_DECLS

#define AGROFORESTRY_TYPE               (agroforestry_get_type())
#define AGROFORESTRY(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            AGROFORESTRY_TYPE, agroforestry))
#define AGROFORESTRY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),\
            AGROFORESTRY_TYPE, agroforestryClass))
#define IS_AGROFORESTRY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            AGROFORESTRY_TYPE))
#define IS_AGROFORESTRY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),\
            AGROFORESTRY_TYPE))

typedef struct _agroforestry            agroforestry;
typedef struct _agroforestryClass       agroforestryClass;

struct _agroforestry
{
    expertn_modul_base parent;
    
    int __INIT_DONE;
	int iAverageShading;
    double fViewAngle, fAzimuthAngle, fDiffAngle, fZenithAngle, fTreeHeight,fTreeWidth;
	double fMaxShadeLength, fDistStrip;
	double *fDistStripAll;
    int iCalcLightAttenuation, iCalcExtinction, iTreeHeightData, iTree;
    double fFixedLightAttenuationFactor, fExtinctionCoefficient;
	double fLAI;
    char* ini_filename;
	int grid_id_tree, grid_i_tree, grid_j_tree, grid_sub_tree;
	int *grid_id_beside_tree, *grid_i_beside_tree, *grid_j_beside_tree, *grid_sub_beside_tree;
	int grid_id_beside_tree_len, grid_i_beside_tree_len, grid_j_beside_tree_len, grid_sub_beside_tree_len,fDistStripAllLen;
	gchar *reg_string_tree, *reg_string_beside_tree, *reg_string_act;
	int xn_id_tree, xn_id_beside_tree;
	double *DeadLeaf_beside_tree_array, *DeadStem_beside_tree_array, *DeadBranch_beside_tree_array, *DeadFineRoot_beside_tree_array, *DeadGrossRoot_beside_tree_array;
	int DeadLeaf_beside_tree_len, DeadStem_beside_tree_len,DeadBranch_beside_tree_len, DeadFineRoot_beside_tree_len, DeadGrossRoot_beside_tree_len;
	double DeadLeaf_beside_tree_frac, DeadStem_beside_tree_frac, DeadBranch_beside_tree_frac,DeadFineRoot_beside_tree_frac,DeadGrossRoot_beside_tree_frac;
	double DeadLeaf_tree_frac, DeadStem_tree_frac, DeadBranch_tree_frac, DeadFineRoot_tree_frac, DeadGrossRoot_tree_frac;
	int CN_calculate; //option to calculate CN ratio (=1) or to take the input (=0)
	double CN_fromDeadLeaf,  CN_fromDeadStem,CN_fromDeadBranch, CN_fromDeadFineRoot, CN_fromDeadGrossRoot;
	double UsedLeafWeight_Sum, UsedStemWeight_Sum, UsedRootWeight_Sum;
	
	/* add your public declarations here */
};


// public class member function:

G_MODULE_EXPORT int init_agroforestry(agroforestry *self);
G_MODULE_EXPORT int agroforestry_read(agroforestry *self,const char* configfile);
G_MODULE_EXPORT int agroforestry_run(agroforestry *self);
G_MODULE_EXPORT int agroforestry_done(agroforestry *self);


G_MODULE_EXPORT void sunpos(agroforestry *self);

struct _agroforestryClass
{
    expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType agroforestry_get_type (void);

G_END_DECLS

#endif /* __AGROFORESTRY_H__ */

