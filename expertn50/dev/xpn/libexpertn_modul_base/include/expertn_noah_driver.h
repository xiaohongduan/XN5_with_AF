#ifndef __EXPERTN_NOAH_DRIVER__
#define __EXPERTN_NOAH_DRIVER__

#include <expertnclass.h>

/*
   Definiere Fortran Datentypen, diese Datentypen können geändert werden,
   falls ein andere Fortran oder C Compiler zum Einsatz kommt.
   Typischerweise gelten folgende zusammenhänge:

    FORTRAN                                 C/C++                                           Bytes
    REAL,REAL*4,REAL(KIND=4)                float                                           4
    REAL*8,REAL(KIND=8),DOUBLE PRECISION    double, long double                             8
    REAL*16,REAL(KIND=16)                   long double (with -qlongdouble or -qldbl128)    16
    INTEGER, INTEGER*4, INTEGER(KIND=4)     int, long int                                   4
    INTEGER*8, INTEGER(KIND=8)              long long int                                   8
    INTEGER*2                               short int                                       2
    COMPLEX, COMPLEX*4                      structure of two floats                         8
    COMPLEX*8                               structure of two doubles                        16
    DOUBLE COMPLEX, COMPLEX*16              structure of two long doubles                   32
    *
*/
#define F_INTEGER   gint32
#define F_REAL      gfloat
#define F_CHAR      gchar
#define F_BOOL      guint8
#define F_DOUBLE    gdouble


typedef struct {
    int start_YY,start_MM,start_DD,start_H,start_M,start_S,start_MS,start_julianday;
    int stop_YY,stop_MM,stop_DD,stop_H,stop_M,stop_S,stop_MS;

    double xlat,xlon,dx,dy;
    int grid_id,i,j;

    int YY,MM,DD,H,M,S,MS,julianday;
    double dt_d; // Timestep in days

    // configuration Information:
    
    int nsoil; // r: NUMBER OF NOAH SOIL LAYERS (AT LEAST 2, AND NOT GREATER THAN PARAMETER NSOLD SET BELOW)
    
    int coupling; // Schalter um NOAH auszuschalten
    
    //


    float lwdn;  // r: SOLAR DOWNWARD RADIATION (W M-2; POSITIVE, NOT NET SOLAR)
    float soldn; // r: LW DOWNWARD RADIATION (W M-2; POSITIVE, NOT NET LONGWAVE)
    float solnet;// r: NET DOWNWARD SOLAR RADIATION ((W M-2; POSITIVE)
    float solardirect; // r: Direct component of downward solar radiation (W M-2)
    float cosz; // r: Solar zenith angle



    float q2; // r: [kg/kg] AIR HUMIDITY AT 1ST LEVEL ABOVE GROUND spez. humidity
    float q2k; // r:
    float T2; // r: AIR TEMPERATURE (K) AT 2m  ABOVE GROUND
    float Th2;
    
    float sfctmp; // r: AIR TEMPERATURE (K) AT zlvl ABOVE GROUND
    float psfc; // r: [Pa?] SURFACE PRESSURE 

    float sfcspd;// r: WIND SPEED (M S-1) AT HEIGHT ZLVL ABOVE GROUND

    float prcp; // r: PRECIP RATE (KG M-2 S-1) (NOTE, THIS IS A RATE)
    float prcprain;// r: Liquid-precipitation rate (KG M-2 S-1)

    float snowh; // r: ACTUAL SNOW DEPTH (M)
    float sneqv; // r: LIQUID WATER-EQUIVALENT SNOW DEPTH (M), NOTE: SNOW DENSITY = SNEQV/SNOWH

    float zlvl; // r : HEIGHT (M) ABOVE GROUND OF ATMOSPHERIC FORCING VARIABLES (sollte 2 m sein)


    // Canopy/Soil Characteristics (Im Prinzip Parameter, die vom NOAH verwendet werden)

    int vegtyp; // r : VEGETATION TYPE (INTEGER INDEX)
    int soiltyp; // r : SOIL TYPE (INTEGER INDEX)
    int slopetyp; // r:  CLASS OF SFC SLOPE (INTEGER INDEX)

    float shdfac;   // rw:  AREAL FRACTIONAL COVERAGE OF GREEN VEGETATION (FRACTION= 0.0-1.0)
    int shdfac_IO; // 0: read only, 1: read and write

    float shdmin;  // r: MINIMUM AREAL FRACTIONAL COVERAGE OF GREEN VEGETATION (FRACTION= 0.0-1.0) <= SHDFAC    
    float shdmax;

				// alb kommt von der WRF Konfiguration und muss nicht verändert werden
	float alb; // rw: BACKROUND SNOW-FREE SURFACE ALBEDO (FRACTION), FOR JULIAN DAY OF YEAR (USUALLY FROM TEMPORAL INTERPOLATION OF MONTHLY MEAN VALUES' CALLING PROG MAY OR MAY NOT INCLUDE DIURNAL SUN ANGLE EFFECT)
    int alb_IO; // 0: read only, 1: read and write

    float snoalb; // r:  UPPER BOUND ON MAXIMUM ALBEDO OVER DEEP SNOW (E.G. FROM ROBINSON AND KUKLA, 1985, J. CLIM. & APPL. METEOR.)
    float tbot; //r: BOTTOM SOIL TEMPERATURE (LOCAL YEARLY-MEAN SFC AIR TEMPERATURE)

    float z0brd; //rw:  Background fixed roughness length (M)
    int z0brd_IO; // 0: read only, 1: read and write

    float z0; // r?: Time varying roughness length (M) as function of snow depth

    float embrd; // w: Background surface emissivity (between 0 and 1)
    int emrd_O; // 0: nothing, 1: write

    float emissi; // rw:Surface emissivity (between 0 and 1)
    int emissi_IO; // 0: read only, 1: read and write#

    float* sldpth; // r: THE THICKNESS OF EACH SOIL LAYER (M)
    

//HISTORY (STATE) VARIABLES (H):noah 
    float cmc;//CANOPY MOISTURE CONTENT (M)
    float T1; // GROUND/CANOPY/SNOWPACK) EFFECTIVE SKIN TEMPERATURE (K)
    float *stc;//(NSOIL)  SOIL TEMP (K)
    float *smc;//(NSOIL)  TOTAL SOIL MOISTURE CONTENT (VOLUMETRIC FRACTION)
    float  *sh2o; //(NSOIL) UNFROZEN SOIL MOISTURE CONTENT (VOLUMETRIC FRACTION) NOTE: FROZEN SOIL MOISTURE = SMC - SH2O        
    
						 // albedo ist wichtig für den output
	float albedo; //      SURFACE ALBEDO INCLUDING SNOW EFFECT (UNITLESS FRACTION)=SNOW-FREE ALBEDO (ALB) WHEN SNEQV=0, OR =FCT(MSNOALB,ALB,VEGTYP,SHDFAC,SHDMIN) WHEN SNEQV>0
    float ch; //          SURFACE EXCHANGE COEFFICIENT FOR HEAT AND MOISTURE (M S-1); NOTE: CH IS TECHNICALLY A CONDUCTANCE SINCE IT HAS BEEN MULTIPLIED BY WIND SPEED.
    float cm; //          SURFACE EXCHANGE COEFFICIENT FOR MOMENTUM (M S-1); NOTE: CM IS TECHNICALLY A CONDUCTANCE SINCE IT HAS BEEN! MULTIPLIED BY WIND SPEED. 



    
    
    
    // OUTPUT (O):
    // ----------------------------------------------------------------------
    //OUTPUT VARIABLES NECESSARY FOR A COUPLED NUMERICAL WEATHER PREDICTION
    // MODEL, E.G. NOAA/NWS/NCEP MESOSCALE ETA MODEL.  FOR THIS APPLICATION,
    // THE REMAINING OUTPUT/DIAGNOSTIC/PARAMETER BLOCKS BELOW ARE NOT
    // NECESSARY.  OTHER APPLICATIONS MAY REQUIRE DIFFERENT OUTPUT VARIABLES.
    
    
    float eta; // w : ACTUAL LATENT HEAT FLUX (W m-2: NEGATIVE, IF UP FROM SURFACE)
    float eta_kinematic; // w: actual latent heat flux in Kg m-2 s-1
    float sheat; // w: SENSIBLE HEAT FLUX (W M-2: NEGATIVE, IF UPWARD FROM SURFACE)
    float fdown; // w: Radiation forcing at the surface (W m-2) = SOLDN*(1-alb)+LWDN
    

/*intercept mod*/   float ec; // w: CANOPY WATER EVAPORATION (W m-2) // interceptionsverdunstung?
    float edir; // w: DIRECT SOIL EVAPORATION (W m-2) // bodenverdunstung

    float *et; // [nsoil] w:PLANT TRANSPIRATION FROM A PARTICULAR ROOT (SOIL) LAYER (W m-2) es reicht wohl ett zu benutzen
    float ett; // w: TOTAL PLANT TRANSPIRATION (W m-2) ist im NOAH ett = sum(et,NSoil)

    float esnow; // w: SUBLIMATION FROM (OR DEPOSITION TO IF <0) SNOWPACK (W m-2)
/*intercept mod*/   float drip; // w: THROUGH-FALL OF PRECIP AND/OR DEW IN EXCESS OF CANOPY WATER-HOLDING CAPACITY (M)
    
    float dew; // w: DEWFALL (OR FROSTFALL FOR T<273.15) (M)

// ----------------------------------------------------------------------
    float beta; // w: RATIO OF ACTUAL/POTENTIAL EVAP (DIMENSIONLESS) // wird im noahdrv nicht benutzt, von daher nicht wichtig
    float etp; // w:  POTENTIAL EVAPORATION (W m-2) // diese Größe wird im Noah in der Variable POTEVP(i,j) accummuliert (scheinbar nur für Billance)
    float ssoil; // w: SOIL HEAT FLUX (W M-2: NEGATIVE IF DOWNWARD FROM SURFACE)

// ----------------------------------------------------------------------

    float flx1; // w: PRECIP-SNOW SFC (W M-2)
    float flx2; // w: FREEZING RAIN LATENT HEAT FLUX (W M-2)
    float flx3; // w: PHASE-CHANGE HEAT FLUX FROM SNOWMELT (W M-2)

// ----------------------------------------------------------------------

    float snomlt; // w: SNOW MELT (M) (WATER EQUIVALENT)
    float sncovr; // w: FRACTIONAL SNOW COVER (UNITLESS FRACTION, 0-1)

// ----------------------------------------------------------------------

    float runoff1; // w: SURFACE RUNOFF (M S-1), NOT INFILTRATING THE SURFACE
    float runoff2; // w: SUBSURFACE RUNOFF (M S-1), DRAINAGE OUT BOTTOM OF LAST SOIL LAYER (BASEFLOW)
    float runoff3; // w: NUMERICAL TRUNCTATION IN EXCESS OF POROSITY (SMCMAX) FOR A GIVEN SOIL LAYER AT THE END OF A TIME STEP (M S-1).   
// Note: the above RUNOFF2 is actually the sum of RUNOFF2 and RUNOFF3
// ----------------------------------------------------------------------
    float rc; //w:CANOPY RESISTANCE (S M-1)
    float pc; //w:PLANT COEFFICIENT (UNITLESS FRACTION, 0-1) WHERE PC*ETP = ACTUAL TRANSP // im Grunde das gleiche wie beta
    float xlai; //w:LEAF AREA INDEX (DIMENSIONLESS)
    float rsmin; //w:MINIMUM CANOPY RESISTANCE (S M-1)
    float rcs; //w:INCOMING SOLAR RC FACTOR (DIMENSIONLESS)
    float rct; //w:AIR TEMPERATURE RC FACTOR (DIMENSIONLESS)
    float rcq; //w: ATMOS VAPOR PRESSURE DEFICIT RC FACTOR (DIMENSIONLESS)
    float rcsoil; //w:  SOIL MOISTURE RC FACTOR (DIMENSIONLESS)
    
    

} struct_WRF;




#endif// __EXPERTN_DRIVER__
