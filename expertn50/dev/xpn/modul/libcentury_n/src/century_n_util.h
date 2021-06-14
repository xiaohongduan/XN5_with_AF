//      century_n.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __CENTURY_N_UTIL_H_
#define __CENTURY_N_UTIL_H__

#define SimStart(pz) (pz->pSimTime->bFirstRound==TRUE)

//Conversion factors
#define kgphaTOgpm2 0.1
#define gpm2TOkgpha 10.0

typedef struct  
{
	double teff[4],
		aneref[3],
		rad1p[3],
		pcemic[3], 
		dec3[2],
		dec5,
		dec4,
		dec1[2],
		dec2[2],
		p1co2a[2],
		p2co2,
		p3co2,
		varat1[3],
		varat2[3],
		varat3[3],
		peftxa,
		peftxb,
		ps1s3[2],
		ps2s3[2],
		animp,
		pligst[2],
		strmax[2],
		rsplig,
		damr[2],
		damrmn,
		pabres,
		spl[2],
		rcestr,
		pprpts[3],
		frfrac[2],
		favail,	
		rictrl,
	    riint,
		pmco2[2];
} struct_fixedparameter;

typedef struct
{
	int site;
double decw1,
	decw2,
	decw3,
	wdlig[5],
	cerfor[3][5],
	drain,
	SurfaceLitterC,SurfaceLitterCtoN,
	SurfaceSOM1C, SurfaceSOM1CtoN,
	wood1C, wood2C,
	maxt;

} struct_parameter;


#define SRFC 0
#define SOIL 1
#define NUMLAYERS 2
#define SimulationStarted  		(SimStart(pTi))
enum TSoilTextureIndex
{
	UNDEFINED = 0,
	COARSE  = 1,		// texture type for sandy soil
	MEDIUM = 2,		// texture type for medium (loamy) soil
	FINE = 3, 		// texture type for fine soil
	VERYFINE = 4 		// texture type for volcanic soil
};
enum TForestParts
{
	LEAF,	// leaf forest part; array index
	FROOT,	// fine root forest part
	FBRCH,	// fine branch forest part
	LWOOD,	// large wood forest part
	CROOT,	// coarse root forest part
	FPARTS	// number of parts in the forest system
};

// Functions:
struct_fixedparameter century_n_getFixedParameters();


#endif
