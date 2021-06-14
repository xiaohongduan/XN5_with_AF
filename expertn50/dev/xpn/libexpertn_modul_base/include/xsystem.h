/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      Christian Klein  (chrikle@berlios.de)
 * 
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Include file for Expert-N library development.
 *   Expert N Sytem Objects
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XSYSTEM_H
#define _INC_EXPERTN_XSYSTEM_H

#include "xpn_register_var.h"

typedef struct
{
  xpn_register_var *var_list; // registry
  char *base_path; // Projektpfad von Expert-N (z.B. um ini Dateien zu lesen). Dieser Pfad kann mit der Funktion get_fullpath_from_relative() benutzt werden, um relative Pfade in vollst. Pfade zu verwandeln
  char *reg_str; // Name der Expert-N Instanz in der Registry (kann auch der Wert der ID sein), ist der Wert gesetzt so ist er eindeutig und kann auch in Dateiausgaben verwendet werden
  char *project_name; // Name des projekts ohne Dateiendung
  char *project_path; // Project Pfad
  int id; // 0...(N-1): Jeder Expert-N Instanz wird eine eindeutige ID zugeordnet (die erste Instanz beginnt immer mit 0)  
  int id_N; // Anzahl der Expert-N Instancen 

  // MPI: Variablen:
  int MPI_act;  // 0..1: Es wird mit MPI parallelisiert  
  int MPI_rank; // MPI Prozessor ID (ohne MPI: 0)
  int MPI_size; // Anzahl an MPI Prozessen (ohne MPI: 1)
  int *MPI_ranks; // Array[id_N] mit den Ranks der anderen xpn_classes
  
  // OpenMP Variablen
  int OPENMP_act;  // 0..1: Es wird mit OpenMP parallelisiert
  int OMP_rank; // OpenMP Prozessor ID (ohne OMP: 0)
  int OMP_size; // Anzahl an OpenMP Prozessen (ohne OMP: 1)
  
  
  //Folgende Variablen dienen dazu auf andere Expert-N Instanzen zuzugreifen (VORSICHT: PARALELLISIERUNG), diese Variablen sind nur gestetzt, falls der Treiber diese Unterstützt
  char **reg_str_inst; // string Array[0..N-1] Namen aller Instanzen
  void **xpn_classes; // Array[0..N-1] Pointer auf alle Instanzen --> (void*) muss in (expertnclass*) gecasted werden
  double landuse_cover_fraction; // 0..1: Anteil der dieses Expert-N in einem Grid einnimmt (ist beim Mosaik Ansatz wichtig)
  int id_M; // Anzahl der "Mosaiks", dh. der Instanzen, die auf einem Grid liegen (dh. im eigenen Grid)
  int *id_Mosaic; // 0..M-1: ids, die sich im Mosaik befinden. Wenn id_Mosaic[0] == id, dann ist diese Expert-N Instanz der Master
  
  // weitere Steuervariablen:
  int extern_climate_data_source; // 0,1 : 0: Daten werden aus der Datenbank geladen; 1; Klima-Daten werden von einem Input Modul geladen
  int extern_time_management; // 0,1: Zeitmanagement wird vom Datenbankmodul verwaltet; 1: Zeitmanagement wird von einem anderen Modul (Input Modul) gemacht
} XSYSTEM, *PXSYSTEM;


#endif // _INC_EXPERTN_XSYSTEM_H

/******************************************************************************** EOF */

