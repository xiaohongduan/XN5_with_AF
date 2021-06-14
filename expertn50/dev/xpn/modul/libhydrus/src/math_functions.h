#ifndef __MATH_FUNCTIONS_H__
#define __MATH_FUNCTIONS_H__


//#define min(a,b) ((a)<(b))?(a):(b)
//#define max(a,b) ((a)>(b))?(a):(b)


/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double hydrus_abspowerDBL(double x, double y);


/***********************************************************************************/
/*  name    :    abspower(..)                                                      */
/*  function:    return                                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/
float  hydrus_abspower(float x, float y);


float hydrus_signum(float x, float y);


#endif
