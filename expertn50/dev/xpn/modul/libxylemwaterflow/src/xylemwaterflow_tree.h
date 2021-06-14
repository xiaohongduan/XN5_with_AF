/*
 * xylemwaterflow_structs.h
 *
 * Copyright 2016 Florian Heinlein <florian.heinlein@inbox.lt>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef __XYLEMWATERFLOW_STRUCTS_H__
#define __XYLEMWATERFLOW_STRUCTS_H__

#include <vector>

class nodes
{
public:
    nodes()
    {
        parent = -99;
        X = 0.0; // mm
        Y = 0.0; // mm
        Z = 0.0; // mm
        LA = 0.0; // fraction of total LA of the tree [0..1] dimensionless density
        cosine = 0.0; // cosine to parent
        vz_delZ = 0; //-1 left of parent
                     //+1 right of parent
        canopy = 0; // 1 for canopy node
        BoundaryFlux = 0.0; // root
        dBoundaryFluxdh = 0.0; //Änderung mit h
        Transpiration = 0.0; // canopy [mm^3/s]
        Transpiration_pot = 0.0;
        Photosynthesis = 0.0;
        Photosynthesis_pot = 0.0;
        H_new_total = 0.0;
        H_total_old = 0.0;
        H_total = 0.0;
        Vectorb = 0.0; // hydrus needs this
        deltaZVec = 0.0; // hydrus needs this
        capVec = 0.0; // hydrus needs this
        oberesWurzelelement = 0.0;
        unteresWurzelelement = 0.0;
        *frac = 0.0; // size of array: number of soil layers. Contains the fraction of the root in the layer
        WC_Xylem_old = 0.0; // hydrus needs this
        WC_Xylem = 0.0;
        Capacity_Xylem = 0.0; // hydrus needs this
        K = 0.0; //[mm^3/s]
        Kirchhoff = 0.0; // Kirchhoff potential
        l = 0.0; // length: distance to parent node [mm]
        width = 0.0;
        g = 0.0;
        XylemArea = 0.0; // Area of the water conducting xylem sapwood [mm^2]
        // does not contain hardwood and phloem
        TotalDiameter = 0.0; //[mm] contains the diameter of the total segment
        sub = 0;             // for root generator
        Strahler = 0; // Strahler number of the node
        gloi = 0;
        gloii = 0;
    };
    int parent; // number of the parent node
    std::vector<int> child;
    double X; // mm
    double Y; // mm
    double Z; // mm
    double LA; // fraction of total LA of the tree [0..1] dimensionless density
    double cosine; // cosine to parent
    int vz_delZ; //-1 left of parent
                 //+1 right of parent
    bool canopy; // 1 for canopy node
    double BoundaryFlux; // root
    double dBoundaryFluxdh; //Änderung mit h
    double Transpiration; // canopy [mm^3/s]
    double Transpiration_pot;
    double Photosynthesis;
    double Photosynthesis_pot;
    double H_new_total;
    double H_total_old;
    double H_total;
    double Vectorb; // hydrus needs this
    double deltaZVec; // hydrus needs this
    double capVec; // hydrus needs this
    double oberesWurzelelement;
    double unteresWurzelelement;
    double* frac; // size of array: number of soil layers. Contains the fraction of the root in the layer
    double WC_Xylem_old; // hydrus needs this
    double WC_Xylem;
    double Capacity_Xylem; // hydrus needs this
    double K; //[mm^3/s]
    double Kirchhoff; // Kirchhoff potential
    double l; // length: distance to parent node [mm]
    double width; //leaf width for crop, e.g maize
    double g;
    double XylemArea; // Area of the water conducting xylem sapwood [mm^2]
    // does not contain hardwood and phloem
    double TotalDiameter; //[mm] contains the diameter of the total segment
    int sub;              // for root generator
    int Strahler; // Strahler number of the node
    int gloi;
    int gloii;
    double collectAdjacentLength(double* sum, class tree* T);
//private:
//	std::vector<int> child;
};

class tree
{
public:
    tree()
    {
        iCnodes = 0; // number of canopy nodes
        iRnodes = 0; // number of root nodes
        inodeDBH = 0; // number of node at dbh
        maxchilds = 0; // number of childs of the node with the most childs
        max_XylemArea_stem = 0.0; // Xylem area of the stem in mm^2
        max_XylemArea_root = 0.0; // Xylem area of the stem in mm^2
        CrownProjArea = 0.0; // m^2 to up-scale to soil uptake
        HydraulicArea = 0.0; // m^2 to down-scale Penman-Monteith
        stom_delay = 0.0; // s
        stom_a = 0.0;
        stom_b = 0.0; // stomatal reaction function to leaf water potential
        elasticity = 0.0; // radial cell wall elasticity E,mm
        Theta_aev = 0.0; // vol water content at air entry value [0..1]
        BC_a = 0.0; // Brooks&Corey A ,mm, air entry value for branch
        BC_Lambda = 0.0;
        kmax_branch = 0.0;
        kmax_root = 0.0; // max conductivity mm/s/m^2xylem
        RadialRootKmax = 0.0; // Root radial conductivity kr[1/s]
        xylemfraction_stem = 0.0; // ratio of xylem area to total stem area at stem
        xylemfraction_root = 0.0; // ratio of xylem area to total area of roots
        porosity = 0.0; // 0..1
        maxWaterContentCanopy = 0.0; // liter maximal water content if all pores are filled 100%
        iLeafNodes = 0; // number of nodes that carry leaves
        fRA = 0.0; // fine root area [m^2] = 0.0;
        LAIDistribution = 0; // 0: homogen = 0.0; 1: load histogram
        nentry = 0; // number of nonzero matrix elements for compressed row format
        LAIHisto = (char*) ""; // histogram file of LAI distribution
        in_transpiration = (char*)  ""; ////path to the transpiration distribution files
        canopy = (char*)  "";
        root = (char*)  "";
        // Initialize internally root parameters
        RL1 = 8.0;
        RL2 = 0.5;
        RL3 = 2.0;
        RL4 = 1.5;
        RL5 = 1.0;
        RL6 = 0.75;
        RN1  = 5;
        RN2 = 3;
        RN3 = 2;
        RN4 = 3;
        RN5 = 3;
        RN6 = 2;
        RN7 = 2;
        GG = 0.93;
        dp1 = 360.0 / RN1;
    };
class nodes* node;
int iCnodes; // number of canopy nodes
int iRnodes; // number of root nodes
int inodeDBH; // number of node at dbh
int inodeGround;
int inode5m;
int inode10m;
int inode15m;
int inode10cmS;
int inode30cmS;
int inode50cmS;
int maxchilds; // number of childs of the node with the most childs
double max_XylemArea_stem; // Xylem area of the stem in mm^2
double max_XylemArea_root; // Xylem area of the stem in mm^2
double CrownProjArea; // m^2 to up-scale to soil uptake
double HydraulicArea; // m^2 to down-scale Penman-Monteith
double stom_delay; // s
char *canopy; // filename of canopy
char *root; // filename of root system
double stom_a, stom_b; // stomatal reaction function to leaf water potential
double elasticity; // radial cell wall elasticity E,mm
double Theta_aev; // vol water content at air entry value [0..1]
double BC_a; // Brooks&Corey A ,mm, air entry value for branch
double BC_Lambda;
double kmax_branch, kmax_root; // max conductivity mm/s/m^2xylem
double RadialRootKmax; // Root radial conductivity kr[1/s]
double xylemfraction_stem; // ratio of xylem area to total stem area at stem
double xylemfraction_root; // ratio of xylem area to total area of roots
double porosity; // 0..1
double maxWaterContentCanopy; // liter maximal water content if all pores are filled 100%
void getStrahler(); // calculates the Strahler Index of every branch
int iLeafNodes; // number of nodes that carry leaves
double getWaterContent(double height); // returns the water content above 'height' in liter
double getFineRootArea_mm(double maxd); // sum the root area of all roots with diamter < maxd[mm]; returns m^2
double getFineRootArea_sub(int minsum); // minsum: min order of generator
double fRA; // fine root area [m^2];
int checkTree(); // checks the variables, returns 0 if everything is OK
int LAIDistribution; // 0: homogen; 1: load histogram
char* LAIHisto; // histogram file of LAI distribution
char* in_transpiration; ////path to the transpiration distribution files
int nentry; // number of nonzero matrix elements for compressed row format
int* SLU_asub;
double* SLU_a;
int* SLU_xa;
double* SLU_rhs;

double* MatrixA; // one row of the matrix
double* rhs;

int RootSwitch;
//Variables for Root Generation
// length of roots orders 1 - 6 in [m]
double RL1;
double RL2;
double RL3;
double RL4;
double RL5;
double RL6;
// number of first order branches (horizontal)
int RN1;
// number of second order branches (vertical)
int RN2;
// number of branches of order 3 - 7; min(RN4) and min(RN5) = 1
int RN3;
int RN4;
int RN5;
int RN6;
int RN7;
// GALE & GRIGAL parameter 
double GG;
// Angle [?] between first order branches
double dp1;

}
;

class edge
{
public:
    int a, b;
    void set(int aa, int bb)
    {
        a = aa;
        b = bb;
    };
};

struct direction{
	double x,y,z;
};


#endif //__XYLEMWATERFLOW_STRUCTS_H__
