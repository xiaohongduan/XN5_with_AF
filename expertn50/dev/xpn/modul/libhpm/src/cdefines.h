#ifndef ___C_DEFINES___
#define ___C_DEFINES___

#ifdef MSVC
#pragma warning(disable : 4996)
#endif 

#ifdef EXPERT_N
#include	"xinclexp.h"
#include	"xn_def.h"   
#include	"xh2o_def.h"
#endif

#define DEBUG_ONCE
#define DEBUG_EXIT SetError(-1);

//define PROGPATH "D:/Documents and Settings/christian.klein/My Documents/programmierung/hpm"
//define PROGPATH "C:/Users/chrikle/projects/hpm"
#define PROGPATH "C:/Documents and Settings/Christian/My Documents/hpm"
//define OUTPUTFILE "HPM_Goettingen_ohne_Ernte_mit_dueng.cdb"

#define OUTPUTFILE "org_Hurley2.cdb"
#define OUTPUTDFILE "deungung3x.cdb"
//#define OUTPUTFILE "HPM_Goettingen.cdb"
//#define HARVESTFILE "harvest3x.cev"
#define HARVESTFILE "TestHarvest.cev"
//define DAYS 1
#define SAVESTEPS 30

#define RET_SUCCESS 0


#undef WriteToTerminal
//define EXPERT_N

#ifdef EXPERT_N
extern int WINAPI Message(long, LPSTR);
#endif

// INI DATEI wird am ende der Simulation mit den aktuellen Werten überschrieben --> um Werte fürs Equilibrium zu finden
//define GENERATE_INI

#define NEC_TIME_STEP (1.0/128.0)



#define HPM_POINTER HPM_Plant *p,HPM_Animal *a,HPM_SAL *s, HPM_Water *w, HPM_Environment *e 
#define HPM_p p,a,s,w,e



#ifdef EXPERT_N
#ifdef WriteToTerminal
#define _M(str,var) {char S[128];sprintf(S,"%s: %f\n",str,var);fprintf(p->terminal,S);fflush(p->terminal);}
#else
#define _M(str,var) {char S[128]; if ( (double)((int)(var))==var) {sprintf(S,"%s: %d",str,(int)var);Message(1,S);} else {sprintf(S,"%s: %f",str,var);Message(1,S);}}
#endif
#else
#define _M(str,var) {printf("%s: %f\n",str,var);}
#endif

// für 1/b

#ifdef DEBUG_ONCE
//#define CHECK_VALID(var) if (CHECK_DOUBLE_INVALID (var)) { FUNCTION_INIT(_M("Invalid value at line",(double)__LINE__);DEBUG_EXIT)}
#define CHECK_0(var) if ((var == 0.0)||CHECK_DOUBLE_INVALID (var)) {if (var==0.0) {FUNCTION_INIT(_M("Div 0 at line",(double)__LINE__);DEBUG_EXIT)} else CHECK_VALID(var) }  else
#define CHECK_G(var,value,std) if (var <= value) {char S[128]; sprintf(S,"Variable hat falschen Wert: %f at line\n",var);_M(S,(double)__LINE__);var = std;} else
#else
#define CHECK_VALID(var) if (CHECK_DOUBLE_INVALID (var)) { _M("Invalid value at line",(double)__LINE__);DEBUG_EXIT}
#define CHECK_0(var) if ((var == 0.0)||CHECK_DOUBLE_INVALID (var)) {if (var==0.0) {_M("Div 0 at line",(double)__LINE__);} else CHECK_VALID(var) }  else
#define CHECK_G(var,value,std) if (var <= value) {char S[128]; sprintf(S,"Variable hat falschen Wert: %f at line\n",var);_M(S,(double)__LINE__);var = std;DEBUG_EXIT} else
#endif
//define s_diurnal 1.0 // Diurnally varying environment.
      /*s_diurnal = 1   !Default value gives diurnal variation of
            !air temperature;
            !radiation;
            !relative humidity.
      !Set s_diurnal = 0, then there is no diurnal variation.
	   * --> HPM_MANAGEMENT_s_diurnal
	   * */

#define Time ((double)(pTi->pSimTime->iJulianDay)+(double)(pTi->pSimTime->fTimeDay))

#define HPM_Calculate_Canopy_PhotosynthesisCalc 2
/** For Canopy_Photosynthesis:
  Fotosythese rate can be calculated:
  - numerically:		HPM_Calculate_Canopy_Photosynthesis_Calc=1
  - analytically:		HPM_Calculate_Canopy_Photosynthesis_Calc=2
  - ...			:		HPM_Calculate_Canopy_Photosynthesis_Calc=3

HPM_Calculate_Canopy_Photosynthesis=3 is suggested by Charles Edwards
**/


/// --------- Plant Model Data CLASS --------- ///
/**
Short Cuts for the variables:
I (first letter)	|	Input varibles
O (first letter)	|	Ouput variables
C					|	Carbon
N					|	Nitrogen
W					|	Water
M					|	Mass
CS					|	Carbon Substrate
NS					|	Nitrogen Substrate
sh					|	shoot
rt					|	root
_ (last letter)		|	Variable is an array[]



**/
#include "cmath_functions.h"
#endif // ___C_DEFINES___
