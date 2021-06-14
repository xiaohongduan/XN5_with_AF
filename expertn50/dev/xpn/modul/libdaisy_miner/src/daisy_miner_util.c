
#include "daisy_miner_util.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>




/***********************************************************************************/
/*  function abspowerDBL(..)                                                       */
/*  function:    double - Zahlenbereich                                            */
/*  author  :    AGE/C.Sperr                                                       */
/*  date    :    01.12.92                                                          */
/***********************************************************************************/

double  abspowerDBL(double x, double y)
{

        double sign,z;
        if (x < 0.0)
                {
                        sign = -1.0;
                        x  *= sign;
                }

        if (y < 0.0)
                {
                        y *= -1.0;
                        if (x != (double) 0.0)  x = 1.0/x;
                }
        z = (double)pow((double)x,(double)y);
        return z;
}

/************************************************************************************/
/* Procedur    :   Polygon4()                                                      */
/* Op.-System  :   DOS                                                             */
/* Beschreibung:   Polygonzug mit 4 Eckpunkten als Korrekturfunktion               */
/*                 für Feuchte und Temperatur.                                     */
/*                                                                                 */
/*              GSF: ch                     14.9.95                                */
/***********************************************************************************/
double Polygon4(double x, double x0, double y0, double x1,double y1,double x2,double y2,double x3,double y3)
{
        double y = (double)-1;

        y = (x >= x3 ) ? y3
            :(x <= x0 ) ? y0
            :((x >= x2 )&&(x3-x2 > EPSILON)) ?
            y2 + (y3 - y2) * (x - x2)/(x3-x2)
            :((x >= x1 )&&(x2-x1 > EPSILON)) ?
            y1 + (y2 - y1) * (x - x1)/(x2-x1)
            :((x >= x0 )&&(x1-x0 > EPSILON)) ?
            y0 + (y1 - y0) * (x - x0)/(x1-x0)
            :((x >= x2 )&&(x3 >= x2)) ?
            y2
            :((x >= x1 )&&(x2 >= x1)) ?
            y1
            :((x >= x0 )&&(x1 >= x0)) ?
            y0
            : (double)-99;


        if (y == -99)
                {
                        gchar *S;
                        S  = g_strdup_printf("Error in Function: Polygon4");
                        PRINT_ERROR(S);
                        g_free(S);
                        return 0.0;
                }

        return y;
}



/***********************************************************************************/
/* Procedur    :   Test_Range()                                                    */
/*                                                                                 */
/* Output:      0  Variable ausserhalb des Bereichs                                */
/*              1  Variable plausibel.                                             */
/*                                                                                 */
/*              GSF: ch         7.3.97                                             */
/***********************************************************************************/
int Test_Range(expertn_modul_base *self,double x, double x1, double x2,const char* error)
{

        if ((x < x1)||(x > x2))
                {
                        gchar *S;
                        S  = g_strdup_printf("Var out of range: %s",error);
                        PRINT_ERROR_ID(self,S);
                        g_free(S);
                        return 0;
                }
        else
                {
                        return 1;
                }
        return 0;
}

/*********************************************************************************************/
/* Procedur    :   Solut_Coeff()                                                             */
/* Beschreibung:   Approximiere Löslichkeit von N2O in Abhängigkeit von der Temperatur       */
/* Nach        :   Moraghan und Buresh (1977)                                                */
/* aus         :   Dissertation C. Braun (1994)                                              */
/*********************************************************************************************/
double Solut_Coeff(double Temp)
{
double f1;

if (Temp <= (double)0)
   f1 = (double) 0.01;//to avoid division by zero in transp.c //0; /* [cm^3 N2O / cm^3 H2O]  */

else if (Temp <= (double)5)
   f1 = (double) 0.213 * Temp;

else
   f1 = (double)1.1067 - (double) 0.02 * Temp;

return f1;
}

double get_infinity()
{
        double d1,d2;
        d1=1.0;
        d2=0.0;
        return d1 / d2;
}

void xpn_register_var_check_inval_(xpn_register_var *self,const char *FILE, const int LINE )
{
        int i;
        for (i=0;i<self->counter;i++)
                {
                        if (self->vars[i]->vartype==G_TYPE_DOUBLE)
                        {
                                if (self->vars[i]->flag_pointer==TRUE)
                                {
                                        if CHECK_DOUBLE_INVALID(*(self->vars[i]->val.pd))
                                        {
                                                gchar *S;
                                                S  = g_strdup_printf("Var %s is invalid",self->vars[i]->varname);
                                                fprintf(stderr,"ERROR:  %s\t%d\t%s\n",FILE,LINE,S);fflush(stderr);
                                                g_free(S);
                                        }
                                } else
                                {
                                        if CHECK_DOUBLE_INVALID((self->vars[i]->val.d))
                                        {
                                                gchar *S;
                                                S  = g_strdup_printf("Var %s is invalid",self->vars[i]->varname);
                                                fprintf(stderr,"ERROR:  %s\t%d\t%s\n",FILE,LINE,S);fflush(stderr);
                                                g_free(S);
                                        }
                                }

                        }
                }
}


// Setzt x auf EPSILON falls x < EPSILON
int Set_At_Least_Epsilon(expertn_modul_base *xpn,double *x, const char *Message)
{
	if (*x < EPSILON) {
		*x = EPSILON;
		if (Message!=NULL) {
			PRINT_ERROR_ID(xpn,Message);
		}
		return 0;
	}
	return 1;
}

/***********************************************************************************/
/* Procedur    :   RelAnteil()                                                     */     
/* Beschreibung:   Von zwei echt positiven Zahlen x, y                             */
/*        wird der rel. Anteil von x an der Summe x+y berechnet.                   */
/*                                                                                 */
/*              GSF: ch                     14.9.95                                */
/***********************************************************************************/

double RelAnteil(double x, double y)
{
double z;
      z = ((x > EPSILON)&&(y > EPSILON))?
          x / (x + y)
          : (double)0;
return z;
}
