#include <math.h>

/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double  hydrus_abspowerDBL(double x, double y)
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




/***********************************************************************************/
/*  name    :    abspower(..)                                                      */
/*  function:    return                                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/
float  hydrus_abspower(float x, float y)
{float sign = (float)1.0,z=(float)0.0;
 if (x < (float)0.0)  {sign = (float)-1.0;
                         x *= (float)sign;}

 if (y < (float)0.0) {
                       if (x != (float)0.0)
                                 x = (float)1.0/x;
                       y *= (float)-1.0;
                     }


 z = (float)pow((double)x,(double)y);
 return  z;
}





float hydrus_signum(float x, float y)
{
	if (y>(float)0) 
		{
			return x;
		} else
	if (y<(float)0)
		{
			return -x;
		} else
		{
			return (float)0;
		}
}


