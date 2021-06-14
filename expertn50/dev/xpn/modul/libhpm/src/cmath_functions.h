#ifndef ___CMATHFUNCTIONS___
#define ___CMATHFUNCTIONS___
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef EXPERT_N
#include <xinclexp.h>
#endif 



// ---------------------- General Defenitions -----------------------------------

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#define mq_TO_ha (1.0e-4)  // Quadratmeter nach ha
#define mq_TO_ha_1 (1.0e4) // 1 / m^2 nach 1 / ha
#define ha_TO_mq (1.0e4)  // ha nach m^2
#define ha_TO_mq_1 (1.0e-4)  // 1 / ha nach 1 / m^2

#ifndef INFINITY
#define INFINITY get_infinity()
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef BOOLEAN
#define BOOLEAN int
#endif


#ifndef UINT32
#define UINT32 unsigned long
#endif



// ---------------------  FUNCTION_INIT Macros: ----------------------------------
// commands between runs only the first time the function is called
// example:
//
// FUNCTION_INIT_BEGIN
//   a=1.0;
//   ...
// FUNCTION_INIT_END
#define FUNCTION_INIT_BEGIN {static int FUNCTION_INIT = TRUE;  if (FUNCTION_INIT==TRUE) {FUNCTION_INIT=FALSE;{
#define FUNCTION_INIT_END }}}

// More easy to use:
// example:
// FUNCTION_INIT 
// (
//   a=1.0;
//   ...
// )
#define FUNCTION_INIT(Expression) FUNCTION_INIT_BEGIN Expression FUNCTION_INIT_END

// cammand will called every time, but not "Count" times
// example:
// while(1)
// {
//   FUNCTION_INIT_N2,printf("Hallo ");)
//   print "Morning ";
// }
// result: "Morning Morning Hallo Morning Hallo Morning Hallo Morning ..."
#define FUNCTION_INIT_BEGIN_N(Count) {static int FUNCTION_COUNT=0; if (FUNCTION_COUNT!=Count){ FUNCTION_COUNT++;} if (FUNCTION_COUNT==Count) {
#define FUNCTION_INIT_END_N }}
#define FUNCTION_INIT_N(Count,Expression) FUNCTION_INIT_BEGIN_N(Count) Expression FUNCTION_INIT_END_N

// x*x
#define QUAD(x) (x*x)

// a^b, with b is a integer
double cpowi(double a, int b);


// pulse function: res: 0 or 1
double cpulse(double t, double t_begin, double period,double width);
// res: max of d1 and d2)
double cmax(double d1,double d2);
// res: min of d1 and d2
double cmin(double d1,double d2);
// sum of an array
double SUM_D(double *var[],int count);
// sum of an array with count 4
double sum4(double *var);
// set all values of an array with count 4 to value
void  set4(double *var,double value);

// same as in python, is not working well at the moment
#define foreach4 (var, var2, expr) {int i; for (i=0;i!=4;i++) {var2=0; expr;}}

#define SumPerDay(erg, var, julianday) {static double sum0 = 0.0; static int ojulianday = -1; if (ojulianday!=julianday) {ojulianday = julianday;erg = sum0; sum0=0.0;} else {sum0+=var;}}

// um zu überprüfen ob eine Var NAN ist:
// val==val (ist true falls es nicht NAN ist), daher
// val!=val

// CHECKS if double or float value is valid:
// example:
// double d;
// d = sqrt(-1.0)
// if CHECK_DOUBLE_INVALID(d)
//    printf("Value d is NaN or infinity \n");
#define CHECK_DOUBLE_INVALID(val) ((val==-INFINITY)||(val==+INFINITY)||(val!=val))

//ifndef CHECK_VALID
//define CHECK_VALID(var) if CHECK_DOUBLE_INVALID(var) printf("Invalid var in Line: ",__LINE__);
//endif

// ----------------------------------------- DOUBLE ARRAY CLASS -------------------------------------------------------
// Easy to use array class (only for double at the moment):
// it can be used then general C array (but the array memorieses the size)
// example:
// double *d;
// int i;
// d = ARRAY_D_New(5);
// for (i=0; i!= ARRAY_D_GetSize(d);i++)
// 	  {
//      d[i] = i;
//    }
// ARRAY_D_Delete(d)
double       *ARRAY_D_New(unsigned long count);
double       *ARRAY_D_Resize(double *darray, unsigned long count);
double       *ARRAY_D_Delete(double *var);
void          ARRAY_D_Clear(double *darray);
unsigned long ARRAY_D_GetSize(double *var);

double       **ARRAY_DP_New(unsigned long count);
double       **ARRAY_DP_Resize(double **darray, unsigned long count);
double       **ARRAY_DP_Delete(double **var);
void          ARRAY_DP_Clear(double **darray);
unsigned long ARRAY_DP_GetSize(double **var);



// ------------------------------------------- C ARRAY CLASS ------------------------------------------------------------
// Veraltete Array Klasse: In neueren Funktionen sollte die DOUBLE ARRAY CLASS verwendet werden, falls möglich
typedef struct 
{
  void **  list;
  UINT32  Count;
  UINT32  MemPerElement;
  BOOLEAN ClearElement;
} C_Array;

C_Array* C_Array_Init(BOOLEAN ClearElement, UINT32 MemPerElement);
void C_Array_Add(C_Array* array,void **Element);
void C_Array_Done(C_Array* array);


// ------------------------------------------ C DEBUG VARS CLASS ---------------------------------------------------------
// Class helps to debug and analyse Vars in your programm
// Simplest examples:
// C_DEBUG_VARS * deb;
// double number;
// deb = C_DEBUG_VARS_Init();
// C_DEBUG_VARS_AddDouble(deb,"number"&number);
// number = 1;
// C_DEBUG_VARS_Print_Header(deb,f);
// C_DEBUG_VARS_Print_Values(deb,f);


// damit CDebug mit Expert N zusammenarbeiten müssen folgende zwei Macros eingefügt werden:
#ifdef EXPERT_N
// Message in Terminal von Expert N schreiben:
extern int WINAPI Message(long, LPSTR);
#define _M(str,var) {char S[128]; if ( (double)((int)(var))==var) {sprintf(S,"%s: %d",str,(int)var);Message(1,S);} else {sprintf(S,"%s: %f",str,var);Message(1,S);}}
#define CHECK_VALID(var) if (CHECK_DOUBLE_INVALID (var)) { FUNCTION_INIT(_M("Invalid value at line",(double)__LINE__);SetError(-1);)}
#endif


typedef struct
{
  C_Array *types;
  C_Array *pointers;
  C_Array *names;	
} C_DEBUG_VARS;

C_DEBUG_VARS *C_DEBUG_VARS_Init();
void C_DEBUG_VARS_AddDouble(C_DEBUG_VARS *deb,const char* name, double *var);
void C_DEBUG_VARS_AddFloat(C_DEBUG_VARS *deb,const char* name, float *var);
double *C_DEBUG_VARS_SearchVarDouble(C_DEBUG_VARS *deb,const char* name); // returns pointer if var exists, or NULL
void C_DEBUG_VARS_AddDoubleKnownWithLine(C_DEBUG_VARS *deb, double *var, int line);
void C_DEBUG_VARS_Printf(C_DEBUG_VARS *deb);
void C_DEBUG_VARS_Print(C_DEBUG_VARS *deb,FILE *f);
void C_DEBUG_VARS_Print_Values(C_DEBUG_VARS *deb,FILE *f);
void C_DEBUG_VARS_Print_Header(C_DEBUG_VARS *deb,FILE *f);
void C_DEBUG_VARS_Print_Index(C_DEBUG_VARS *deb,const char* filename);
void C_DEBUG_VARS_Clear_Values(C_DEBUG_VARS *deb); // Setzt alle Vars auf 0.0

/* Aufbau der einer INI Datei
; Kommentar
# Komentar
[Sektion]
Schlüssel=Wert
*/
int C_DEBUG_VARS_Read_INI_File(C_DEBUG_VARS *deb,const char * filename); // ret.: 0 everything OK

void C_DEBUG_VARS_Done(C_DEBUG_VARS *deb);

#define C_DEBUG_VARS_AddStaticDouble(deb,name,var) FUNCTION_INIT (C_DEBUG_VARS_AddDouble(deb,name,var););
#define C_DEBUG_VARS_AddLocalDouble(deb,name,var) {static double _var=0.0;CHECK_VALID(*var) FUNCTION_INIT_BEGIN static char S[1000]; sprintf(S,"%s Line: %d",name,(int)__LINE__); C_DEBUG_VARS_AddDouble(deb,S,&_var);FUNCTION_INIT_END; _var=*var;};
#define C_DEBUG_VARS_AddExternExpression(deb,name,var) {static double _var=0.0;CHECK_VALID(var) FUNCTION_INIT_BEGIN C_DEBUG_VARS_AddDouble(deb,name,&_var);FUNCTION_INIT_END; _var=var;};
#define C_DEBUG_VARS_AddExternEArray(deb,name,var,count,Size) { static double* vars=NULL; CHECK_VALID(var) FUNCTION_INIT_BEGIN char *_name; int i;	vars = ARRAY_D_New((unsigned long)Size);ARRAY_D_Clear(vars); for (i=0;i!=(int)Size;i++) {_name = (char*)malloc(sizeof(name)+10);sprintf(_name,"%s[%d]",name,count);C_DEBUG_VARS_AddDouble(deb,_name,&vars[i]);vars[i]=0.0;} FUNCTION_INIT_END; if (ARRAY_D_GetSize(vars)> (unsigned long)count) vars[count]=var;};
#define C_DEBUG_VARS_AddExpDouble(deb,name,cons) {double _cons; _cons = (cons); C_DEBUG_VARS_AddLocalDouble(deb,name,&_cons);};
#define C_DEBUG_VARS_AddD(deb,var) C_DEBUG_VARS_AddDoubleKnownWithLine(deb,var,__LINE__);

#define C_DEBUG_Read_INI_File(deb,filename) FUNCTION_INIT (SetError(C_DEBUG_VARS_Read_INI_File(deb,filename)); );
#define C_DEBUG_AUTO_WRITE_FILE(deb,filename) {static FILE *f; FUNCTION_INIT (deb = C_DEBUG_VARS_Init(););FUNCTION_INIT_N(2,FUNCTION_INIT (f = fopen(filename,"w");C_DEBUG_VARS_Print_Header(deb,f););C_DEBUG_VARS_Print_Values(deb,f););}
				   

// -FILE/STRING - OPERATIONS ------------------------------------------------
int GetStringColumn(char *dest,char *source,char *seperator,int column);
// result:  0 if everything ok
// 			1 column does not exist
// example: 
// GetStringColumn(S,"520::200::234","::",2);
// printf("Number %s",S); // Number 234


char *cfgets_mem(FILE *stream);
// similar as fgets (without endl charakter)
// for windows and linux txt files
// result: string which must be freed with free(..)
//         NULL if error

void DeleteSpaceBegEnd (char *str);

// function used from the macro INFINITY
double get_infinity();

// Functions to Read and Write Errors, Kann von jeder beliebigen Funktion verwendet werden, normalerweise ist der Error=0;
// SetError ändert nur dann den Wert, wenn der Error==0 ist, daher enthät man mit GetError immer den Fehler,
// der als erstes aufgetreten ist
#define SetError(No) _SetError(No,__LINE__);
void   _SetError(int No, int Line);
int    GetErrorNo();
int    GetErrorLine();
char*  GetErrorString(); //Should be freed with free(..)

typedef struct
{
  double *time;
  double **var;
  double *value;
  unsigned long size;
  double old_simtime;
} C_EVENT;

#define cEventReadFileAuto(event,filename,deb) FUNCTION_INIT(event = cEventReadFile(filename,deb););
C_EVENT * cEventReadFile(const char *filename, C_DEBUG_VARS *deb);
void cEventCheckValues(C_EVENT *event,double sim_time);

#endif //___CMATHFUNCTIONS___

