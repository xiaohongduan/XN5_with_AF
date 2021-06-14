/*
 * open_geo_sys_driver_test.h
 * 
 * Copyright 2013 Christian Klein <Christian Klein@CK-86BEE55BEA21>
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

#ifndef __XYLEMWATERFLOW_CLASS_H__
#define __XYLEMWATERFLOW_CLASS_H__

#define  _CRT_SECURE_NO_DEPRECATE
#define  _SRT_SECURE_NO_DEPRECATE
#include <memory.h>
#include <time.h>
//#include <windows.h>
// kg44 test for buffered outputh

//#include <cstdio>
//#include <stdlib.h>
//#include <unistd.h>


#include	<vector> 
#include	<fstream>
#include	<iostream>
#include	<iomanip>  // ::std::setw, ::std::setfill 
#include	<ios> 
#include	<string>

#include <math.h>

#include <expertn_modul_base.h>

#include "xylemwaterflow_soils.h"
#include "xylemwaterflow_config.h"
#include "xylemwaterflow_tree.h"
#include "xylemwaterflow_balance.h"
#include "xylemwaterflow_GenerateRoots.h"
#include "xylemwaterflow_global.h"
#include "xylemwaterflow_load.h"
#include "xylemwaterflow_hydraulicFunctions.h"
#include "xylemwaterflow_Kirchhoff.h"
#include "xylemwaterflow_SuperLUfunctions.h"
#include "xylemwaterflow_PDESystem.h"
#include "xylemwaterflow_quicksort.h"

#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)
//using namespace std;
//
//Module defined include files

class XWF_CLASS {
	
		public:
			//OGS_CLASS(expertn_modul_base *xpn) {this->xpn = xpn;}
			XWF_CLASS(expertn_modul_base *xpn){this->xpn=xpn;};
			
			int load();
			int run_global(){return 0;};
			int run_mosaic(){return 0;};
			int run();
			int done();
			
			int flux_Ross();
			double getPotTranspiration(tree *T);
            double getPotPhotosynthesis(tree *T);
			double conduct3(int i, tree *T);//conductivity in xylem
			void GenerateBoundaryFlux(double pot_tra, double pot_A, int simstarttimestep, tree *T);
			double GBF_sub(int node, int parent, int simstarttimestep, tree *T);
			double StomCond(double pot_tra, int i, tree *T);//interpolates actual transpiration

            //photosynthesis
            //	FUNCTIONS	//	notation following Tuzet et al. 2003
            double getA( double AirTemp,			//[K]
			  double CO2,				//[mol CO2 / mol air], CO2 content of the air
			  double PAR				//[mol m-1 s-1]
			  );
            double getVlmax ( double  AirTemp);		//T [K] Vlmax[mol m-2 s-1]
            double getJmax  ( double  AirTemp);		//T [K] Jmax [mol m-2 s-1]
            double getGAMMAstar ( double AirTemp );	//T [K] GAMMAstar [mol mol-1]
            double getKo ( double AirTemp );		//T [K] Ko [mol mol-1]
            double getKc ( double AirTemp );		//T [K] Kc [mol mol-1]
            double getJ ( double PAR, double AirTemp);//input and return in [mol m-2 s-2], T[K]
            double getVc (	double AirTemp,			//[K]
				double CO2 );			//[mol CO2 / mol air], intercellular CO2 content
            double getVj( double AirTemp,			//[K]
			  double CO2,				//[mol CO2 / mol air], CO2 content of the air
			  double PAR );				//[mol m-1 s-1]

			
			//init functions
			int loadconfig();
			int fill_soil_properties();
			int load_architecture();
			void countAndAllocate(tree *T);
			void XylemArea(tree *T); 
			void totalDiameter(tree *T);
			void genLA(tree *T);
			void RootSurfADistrSoil(tree *T);
			void RootPartitioningToSoilLayers(tree *T);
			
			void set_Vector_Theta(tree *T);
			void set_Vector_Cond(tree *T);
			void set_Vector_mualem();
			double mualem_sub(int i,double h);
			double ThetaH(int i, double h,tree *T);
			
			double getKirchhoff(double h, int i, tree *T);
			void solveRoss(double *delT, tree *T);

			// Output
			char *OutputPath;
			char *OutputCompartment;


        private:
			expertn_modul_base *xpn;
			
			char	*cfg_filename;
			char	*canopy_filename;
			char	*root_filename;
			
			class tree 	T_m[1];
			class xylemwaterflow_balance	B_m[1];
		
			int tlp_output;
            int write_second_outd_file;
		
			double cpa, cpatotal;
			double Sink;
			double *Uptake_Janott;
			
			double  SoilToXylemArea;
			double	*UptLayer;
			
			double *depth;
			
			struct	config 	conf;
			struct  soils  	*boden;
			struct	soils	xylem;
			
			double step;

			double*		hT;
			double*		hK;
			double*		hTr;
			double**	hM;
			int numbers;
			
			double eta;
			int gameover;
			
			//daily output values
			double *uptake_d, *flow_dbh_d, *sum_d, *transpiration_d, *transpiration_pot_d, *photosynthesis_d, *photosynthesis_pot_d, *waterStorage_d, *min_pot_stem, *max_pot_stem;
			double *uptake_h, *flow_dbh_h, *sum_h, *transpiration_h, *transpiration_pot_h, *photosynthesis_h, *photosynthesis_pot_h, *waterStorage_h, *pot_stem;

};

#endif // __XYLEMWATERFLOW_CLASS_H__
