#include "hpm_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



double hpm_math_cpulse(double t,double t_begin, double period,double width)
{
	double t2;
	t2 = t - period*(double)((int)(t/period));
	return ((t2 >= t_begin) && (t2 <= (t_begin+width)));
}

double hpm_get_light_on(double Lm,double Lz,double DayLength, double fTimeDay)
{
	double mid_of_day,dusk_time,dawn_time;
	double light_on;
	//Lm = -xpn->pLo->pFarm->fLongitude;

	mid_of_day = (24.0/2.0 - (Lz-Lm)/360.0*24.0);

	// dusk time:
	dusk_time = mid_of_day + DayLength / 2.0;
	dawn_time = mid_of_day - DayLength / 2.0;
	if (dusk_time >= 24.0) {
		dusk_time-=24.0;
	}
	if (dawn_time < 0.0) {
		dawn_time+=24.0;
	}

	if (dusk_time < dawn_time) {
		if ((fTimeDay*24.0 >= dusk_time) && (fTimeDay*24.0 < dawn_time)) {
			light_on=0.0;
		} else {
			light_on=1.0;
		}
	} else {
		if ((fTimeDay*24.0 >= dawn_time) && (fTimeDay*24.0 < dusk_time)) {
			light_on=1.0;
		} else {
			light_on=0.0;
		}
	}
	return light_on;
}

double hpm_math_sum4(double *var)
{
	int i;
	double res;
	res=0.0;
	for (i=0;i!=4;i++)
		{
		  res+=var[i];
		}
	return res;
}

