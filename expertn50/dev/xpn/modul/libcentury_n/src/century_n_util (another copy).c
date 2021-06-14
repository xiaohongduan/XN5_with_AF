#include "century_n_util.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PRINT_ERROR(Message) fprintf(stderr,"ERROR:  %s\t%d\t%s\n",__FILE__,__LINE__,Message);fflush(stderr);

struct_fixedparameter century_n_getFixedParameters()
{
	struct_fixedparameter fixed;
	fixed.aneref[0]=1.5;
	fixed.aneref[1]=3.0;
	fixed.aneref[2]=0.3;
	fixed.pprpts[0]=0.0;
	fixed.pprpts[1]=1.0;
	fixed.pprpts[2]=0.8;
	fixed.varat1[0]=18.0;
	fixed.varat1[1]=8.0;
	fixed.varat1[2]=2.0;
	fixed.varat2[0]=40.0;
	fixed.varat2[1]=12.0;
	fixed.varat2[2]=2.0;
	fixed.varat3[0]=20.0;
	fixed.varat3[1]=6.0;
	fixed.varat3[2]=2.0;
	fixed.animp=5.0;
	fixed.damr[0]=0.0;
	fixed.damr[1]=0.02;
	fixed.damrmn=15.0;
	fixed.dec1[0]=3.9;
	fixed.dec1[1]=4.9;
	fixed.dec2[0]=14.8;
	fixed.dec2[1]=18.5;
	fixed.dec3[0]=6.0;
	fixed.dec3[1]=7.3;
	fixed.p1co2a[0]=0.6;
	fixed.p1co2a[1]=0.17;
	fixed.pligst[0]=3.0;
	fixed.pligst[1]=3.0;
	fixed.spl[0]=0.85;
	fixed.spl[1]=0.013;
	fixed.strmax[0]=5000.0;
	fixed.strmax[1]=5000.0;
	fixed.pmco2[0]=0.55;
	fixed.pmco2[1]=0.55;

	fixed.ps1s3[0]=0.003;
	fixed.ps1s3[1]=0.032;
	fixed.ps2s3[0]=0.003;
	fixed.ps2s3[1]=0.009;
	fixed.dec4=0.0045;
	fixed.dec5=0.2;
	fixed.favail=0.9;
	fixed.frfrac[0]= 0.01;//      For trees only:frfrac[0] - minimum possible allocation fraction to fine roots
	fixed.frfrac[1]= 0.8;//                    (tree.100)
//        frfrac[1] - maximum possible allocation fraction to fine roots
	fixed.p2co2=0.55;
	fixed.p3co2=0.55;
	fixed.pabres=100.0;
	fixed.pcemic[0]=16.0;
	fixed.pcemic[1]=10.0;
	fixed.pcemic[2]=0.02;
	fixed.peftxa=0.25;
	fixed.peftxb=0.75;
	fixed.rad1p[0]=12.0;
	fixed.rad1p[1]=3.0;
	fixed.rad1p[2]=5.0;
	fixed.rcestr=200.0;
	fixed.rictrl=0.015;
	fixed.riint=0.8;
	fixed.rsplig=0.3;
	fixed.teff[0]= 15.4;	//different in daily version, see online manual or Daycent code
	fixed.teff[1]= 11.75;
	fixed.teff[2]= 29.70,
	               fixed.teff[3]= 0.031;
	return fixed;
}



