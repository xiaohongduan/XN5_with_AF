#include <math.h>

/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double  abspowerDBL(double x, double y)
{

  double sign,z;
  if (x < 0.0)  {sign = -1.0;
                  x  *= sign;}

 if (y < 0.0) {
               y *= -1.0;
               if (x != (double) 0.0)  x = 1.0/x;
              }
 z = (double)pow((double)x,(double)y);
 return z;
}
