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

#ifndef __OGS_CLASS_H__
#define __OGS_CLASS_H__


// Hier ist alles aus der rf.cpp
#include "Configure.h"

#if defined(USE_MPI) || defined(USE_MPI_PARPROC) || defined(USE_MPI_REGSOIL) || \
        defined(USE_MPI_GEMS)
#include "par_ddc.h"
#include <mpi.h>
#endif
#ifdef LIS
#include "lis.h"
#include <omp.h>
#endif

#include "BuildInfo.h"

/* Preprozessor-Definitionen */
#include "makros.h"
#define TEST
/* Benutzte Module */
#include "break.h"
#include "timer.h"
//16.12.2008. WW #include "rf_apl.h"
#include "FileTools.h"
#include "files0.h"
#ifdef SUPERCOMPUTER
// kg44 test for buffered outputh
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif
#include "problem.h"

/* Deklarationen */
int main ( int argc, char* argv[] );
void ShowSwitches ( void );
// LB,string FileName; //WW
// LB,string FilePath; //23.02.2009. WW
// ------  12.09.2007 WW:
#if defined(USE_MPI) || defined(USE_MPI_PARPROC) || defined(USE_MPI_REGSOIL) || \
        defined(USE_MPI_GEMS)
double elapsed_time_mpi;
// ------
#endif
/* Definitionen */

/**************************************************************************/
/* ROCKFLOW - Funktion: main
 */
/* Aufgabe:
   Hauptprogramm
 */
/* Formalparameter: (E: Eingabe; R: Rueckgabe; X: Beides)
   E int argc: Anzahl der Kommandozeilenparameter (incl. Programmname)
   E char *argv[]: Zeiger auf Feld der argc Kommandozeilenparameter
 */
/* Ergebnis:
   Fehlerfreie Bearbeitung: Exit-Code 0
 */
/* Programmaenderungen:
   07/1996     MSR        Erste Version
   08/1999     OK         RF-FEM Applikation
 */
/**************************************************************************/

//using namespace std;

#include "problem_child.h"


class OGS_CLASS {
        public:
			//OGS_CLASS(expertn_modul_base *xpn) {this->xpn = xpn;}
			OGS_CLASS(void *xpn){this->xpn=xpn;};
			int load();
			int run_global();
			int run_mosaic();
			int run();
			int done();

        private:
			void *xpn;
			char *param;
			problem_child* aproblem;
			char* dateiname;
			bool ogs_load;
};

#endif
