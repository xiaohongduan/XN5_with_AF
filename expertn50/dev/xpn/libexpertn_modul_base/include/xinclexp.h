/****************************
 *
 *                        E X P E R T - N
 *              Includes und Defines 
 *
 * Name        : incl_exp.h
 * Autor       : Christian Haberbosch
 *             : Christian Klein (edited for WRF)
 * Beschreibung: Sammlung aller im Projekt benötigten Header-Files
 * Stand       : 17.11.95
 *               14.Jini 2010
 *****************************/

// include windows nur zu Testausgaben  Message ....
//include <windows.h>

#ifndef ___XINCLEXP_H___
#define ___XINCLEXP_H___

//include  <stdlib.h>
//include  <math.h>
//include  <memory.h>
//include <string.h>
//include <io.h>
  
#include  "xclimate.h"
#include  "xchemist.h"
//#include  "xgraphic.h" // wird dies Bib noch gebraucht? ich lasse sie mal aus Kompatibilitätsgründen
#include  "xheat.h"
#include  "xlocatn.h"
#include  "xmanag.h"
#include  "xplant.h"
#include  "xsoil.h"
#include  "xtime.h"
#include  "xwater.h"
#include  "xsystem.h"

// ch 20.9.95 Ab hier wird nicht mehr vorcompilierter Header verwendet:
// pragma hdrstop

// #include  "xdebug.h"
// Man sollte alle definitionen in eine Datei schreiben --> ToDo
#include "xdefines.h"
#include "xtypes.h"

#endif //___XINCLEXP_H___

