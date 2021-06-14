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
	fixed.aneref[0]=1.5; // Anaerobic conditions cause decomposition to decrease 
	fixed.aneref[1]=3.0; // Anaerobic conditions cause decomposition to decrease
	fixed.aneref[2]=0.3; // Anaerobic conditions cause decomposition to decrease
	fixed.pprpts[0]=0.0; // not needed
	fixed.pprpts[1]=1.0; // not needed
	fixed.pprpts[2]=0.8; // not needed
	fixed.varat1[0]=18.0; // maximal C/N Ratio for new SOM1 (HumusFast) from decomposition of Coarse Roots , BG structural
	fixed.varat1[1]=8.0;  // C to N flow Ratio for new SOM1 from decomposition of Coarse Roots , BG structural, determine if Immobilization od Mineralization
	fixed.varat1[2]=2.0;  // ?
	fixed.varat2[0]=40.0;// macht aus, // maximal Ratio for new SOM2 (HumusSlow) from decomposition of Coarse Roots, BG structural
	fixed.varat2[1]=12.0;// macht aus, // C to N flow Ratio for new SOM2 from decomposition of Coarse Roots , BG structural, determine if Immobilization od Mineralization
	fixed.varat2[2]=2.0;// macht aus
	fixed.varat3[0]=20.0; // maximal Ratio for new SOM3 (HumusStable) from decomposition of Coarse Roots , BG structural
	fixed.varat3[1]=6.0; // minimal Ratio for new SOM3 from decomposition of Coarse Roots , BG structural
	fixed.varat3[2]=2.0; 
	fixed.animp=5.0; // for C to SOM3
	fixed.damr[0]=0.0; // for Direct absorption of mineral element by residue, surface
	fixed.damr[1]=0.02; // for Direct absorption of mineral element by residue, soil
	fixed.damrmn=15.0; //C/E of residue = damrmn
	fixed.dec1[0]=3.9;// degrad. rate of STRUCTURAL Material (StrcLitter) to SOM1 and SOM2, surface
	fixed.dec1[1]=4.9; // degrad. rate of STRUCTURAL Material to SOM1 and SOM2, soil
	fixed.dec2[0]=14.8; //degrad. rate of METABOLIC (MtbLitter) to  SOM1, surface
	fixed.dec2[1]=18.5; //degrad. rate of METABOLIC (MtbLitter) to  SOM1, soil
	fixed.dec3[0]=6.0;// degrad. rate of SOM1 (MicLitterSurf = HumusSurf) to SOM2, surface 
	fixed.dec3[1]=7.3; // degrad. rate of SOM1 (HumusFast) to SOM2, soil 
	fixed.p1co2a[0]=0.6; // Rate of CO2 by degrad. of StrLitterSurf to SOM1, not greater than 1.0
	fixed.p1co2a[1]=0.17; // Rate of CO2 by degrad. of StrLitter in soil to SOM1
	fixed.pligst[0]=3.0; //for determine maximal ratio of Lignin (StrLitterSurf), to avoid incorrect input of fCStrLitterSurf, fCBranchLitterSurf,fCStemLitterSurf in surface
	fixed.pligst[1]=3.0; // for determine maximal ratio of Lignin (StrLitter), to avoid incorrect input of fCGrossRootLitter in soil
	fixed.spl[0]=0.85; // ratio of Carbon added to metabolic, the rest goes to structural residue
	fixed.spl[1]=0.013;
	fixed.strmax[0]=5000.0; // for determine maximal ratio of Lignin
	fixed.strmax[1]=5000.0;
	fixed.pmco2[0]=0.55; // Rate of CO2 by degrad. of MtbLitterSurf (metabolic) to SOM1
	fixed.pmco2[1]=0.55;

	fixed.ps1s3[0]=0.003; // for determine SOM1 to SOM3
	fixed.ps1s3[1]=0.032; // for determine SOM1 to SOM3
	fixed.ps2s3[0]=0.003; // for determine SOM2 to SOM3
	fixed.ps2s3[1]=0.009; // for determine SOM2 to SOM3
	fixed.dec4=0.0045; //am meisten impact, C flow out of HumusStable (metabolic) into SOM1
	fixed.dec5=0.2; //am meisten impact, C flow out of HumusSlow (metabolic) into SOM1 and SOM3
	fixed.favail=0.9;
	fixed.frfrac[0]= 0.01;//      For trees only:frfrac[0] - minimum possible allocation fraction to fine roots
	fixed.frfrac[1]= 0.8;//                    (tree.100)
//        frfrac[1] - maximum possible allocation fraction to fine roots
	fixed.p2co2=0.55; // Rate of CO2 by degrad. of SOM2 to SOM1 and SOM3
	fixed.p3co2=0.55; // Rate of CO2 by degrad. of SOM3 to SOM1 
	fixed.pabres=100.0; // for Direct absorption of mineral element by residue
	fixed.pcemic[0]=16.0; // for determine cemicb = slope of the regression line for C/E of som1
	fixed.pcemic[1]=10.0;
	fixed.pcemic[2]=0.02;
	fixed.peftxa=0.25; // for effect of texture
	fixed.peftxb=0.75; // for effect of texture
	fixed.rad1p[0]=12.0;
	fixed.rad1p[1]=3.0;
	fixed.rad1p[2]=5.0;
	fixed.rcestr=200.0;
	fixed.rictrl=0.015;
	fixed.riint=0.8;
	//Hong fixed.rsplig=0.3;
	fixed.rsplig=0.3;// no impact, Rate of CO2 by Decompose Lignin (Teil von StrLitterSurf) to SOM2
	fixed.teff[0]= 15.4;	//different in daily version, see online manual or Daycent code
	fixed.teff[1]= 11.75; // effect of temperature
	fixed.teff[2]= 29.70;
	fixed.teff[3]= 0.031;
	return fixed;
}



