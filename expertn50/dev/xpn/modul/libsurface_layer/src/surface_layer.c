//      surface_layer.c
//
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//


#include "surface_layer.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(surface_layer, surface_layer, EXPERTN_MODUL_BASE_TYPE);


static void surface_layer_class_init(surface_layerClass *klass) {}


static void surface_layer_init(surface_layer *self) {}



// Our Modul Functions:


void sfcdif1_loop(surface_layer *self,int iter, double *moz, double *fm, double *fh, double *fv, double *mol,double *cm,double *ch, int *mozsign)
{
	//int iter=1; // function wird in einer for schleife iteriert
	//double moz; // input Monin-Obukhov stability (z/L)
	//double fm; // ψ_m((z-d0)/L)
	//double fh; // ψ_h((z-d0)/L)
	//double cm; drag coefficient for momentum
    //double ch;  drag coefficient for heat
	// double fv; // !friction velocity (m/s)
	// double mol; // !Monin-Obukhov length (m)
	//int mozsign; // number of times moz changes sign
	
	expertn_modul_base *xpn = &(self->parent);
	double vkc = 0.40; // von Karman constant
	double grav = 9.80616;   //acceleration due to gravity (m/s2)
	double rair   = 287.04;  //gas constant for dry air (j/kg/k)
	double cpair  = 1004.64; //heat capacity dry air at const pres (j/kg/k)
	double mpe = 1e-6; // prevents overflow error if division by zero
	
	double fhnew,fmnew; //
	double tvir; //virtual temperature (k)
	double sfctmp; // absolute Temperature [K]
	double ea,es;
	double h; // sensible heat flux (w/m2) [+ to atm]
	double rhoair;
	double zlvl; //reference height  (m)
	double zpd;//    !zero plane displacement (m)
	double zom; // roughness length, momentum, ground (m)
	double zoh;  //roughness length, sensible heat, ground (m)
	double mozold;
	
	if (iter == 1)
		{
			mozold = 0.0;
			*mozsign = 0;
		} else
		{
			mozold = *moz;
		}
	
	sfctmp = 273.16+xpn->pCl->pWeather->fTempAir;
	//Saturation Vapor Pressure (es)
	es = 0.6108*exp(17.27*xpn->pCl->pWeather->fTempAir/(xpn->pCl->pWeather->fTempAir+237.3)); //eqn. 37, [kPa/°C]

	//Actual Vapor Pressure (ea)
	//Method nr. 3 in table 4, eqn 37, 41, [kPa/°C]
	//using rel. humidity and temperature at the hour
	ea = xpn->pCl->pWeather->fHumidity/100.0 * es;
	
	tvir = sfctmp * 1.0/(1.0-0.378*ea/(xpn->pCl->pWeather->fAtmPressure*1.0e-3));
	
	h = xpn->pHe->pHEBalance->fHeatSens;
	rhoair = (xpn->pCl->pWeather->fAtmPressure -0.378*ea*1e3) / (rair*sfctmp);
	
	zlvl = xpn->pCl->fWindMeasHeight;
	zpd = 0.67 * xpn->pPl->pCanopy->fPlantHeight;
	zom = 0.123 * xpn->pPl->pCanopy->fPlantHeight;
	zoh = 0.0123 * xpn->pPl->pCanopy->fPlantHeight;
	
	if (iter == 1)
		{
			*fv = 0.0;
			*moz = 0.0;
			*mol = 0.0;
		} else
		{
			double tmp1;
			tmp1 = vkc * (grav/tvir)*h / (rhoair*cpair);
			if (fabs(tmp1) < mpe) 
				{
					tmp1 = mpe;
				}
			// Kominiere die Gleichung  S.899 (2) und Gleichung S.900 (15)
			*mol = -1.0 * pow(*fv,3.0) / tmp1;
			*moz = min(((zlvl-zpd)/ *mol),1.0);
		}
		
	// accumulate number of times moz changes sign.

    
	
	if (mozold* (*moz) < 0.0) (*mozsign)++;
	//printf("%d moz: %f\n",(*mozsign),(*moz));
    if (*mozsign >= 2)
		{
			*moz = 0.0;
			*fm = 0.0;
			*fh = 0.0;
		}


	// evaluate stability-dependent variables using moz from prior iteration
	if (*moz < 0.0)  // free convection Ri_b < 0 (fouth one)
		{
			double tmp1,tmp2,tmp3;
			//S.900 (14) im Text
			tmp1 = pow(1.0 - 16.0* *moz,0.25);
			//S.900 (14) * 2
			tmp2 = log((1.0+tmp1*tmp1)/2.0);
			//S.900 (13) * Teil 1
			tmp3 = log((1.0+tmp1)/2.0);
			//S.900 (13) ganze Gleichung: ψ_m = ...
			fmnew = 2.0*tmp3 + tmp2 - 2.0*atan(tmp1) + M_PI/2.0;
			//S.900 (14): ψ_h = ...
			fhnew = 2.0*tmp2;
		}
	else
		{
			fmnew = -5.0* *moz; // ψ_m = ψ_h = -5.0 * z/l
			fhnew = fmnew;
		}
		
	// except for first iteration, weight stability factors for previous
	// iteration to help avoid flip-flops from one iteration to the next

    if (iter == 1)
		{
			*fm = fmnew;
			*fh = fhnew;       
		} else
		{
       
			*fm = 0.5 * (*fm+fmnew);
			*fh = 0.5 * (*fh+fhnew);
		}
		
		{
			
			double tmpcm; // temporary calculation for CM
			double tmpch; // temporary calculation for CH
			double cmfm;
			double chfh;
			
			tmpcm = log((zlvl-zpd) / zom);
			tmpch = log((zlvl-zpd) / zoh);
			cmfm = tmpcm - *fm;
			chfh = tmpch - *fh;
			if (fabs(cmfm) < mpe) 
				{
						cmfm = mpe;
				}
			if (fabs(chfh) < mpe) 
				{
						chfh = mpe;
				}
			*cm = vkc*vkc / (cmfm * cmfm);
			*ch = vkc*vkc / (cmfm * chfh);
		}
		
		// S.900 unter (15) u_star = k*U/[ln(z/z0)-ψ_m((z-d0)/L)]
		*fv = xpn->pCl->pWeather->fWindSpeed * sqrt(*cm);
}

// ----------------------------------------------------------------------
// NOTE: THE TWO CODE BLOCKS BELOW DEFINE FUNCTIONS
// ----------------------------------------------------------------------
// LECH'S SURFACE FUNCTIONS
static double PSLMU(double ZZ) {return -0.96* log (1.0-4.5* ZZ);};
static double PSLMS(double ZZ,double RRIC) {return ZZ * RRIC -2.076* (1. -1./ (ZZ +1.));};
static double PSLHU(double ZZ) {return -0.96* log (1.0-4.5* ZZ);};
static double PSLHS(double ZZ, double RFAC) {return ZZ * RFAC -2.076* (1. -1./ (ZZ +1.));};
// PAULSON'S SURFACE FUNCTIONS
static double PSPMU(double XX, double PIHF) {return -2.* log ( (XX +1.)*0.5) - log ( (XX * XX +1.)*0.5)+2.* atan (XX)- PIHF;};
static double PSPMS(double YY) {return 5.* YY;};
static double PSPHU(double XX) {return -2.* log ( (XX * XX +1.)*0.5);};
static double PSPHS(double YY) {return 5.* YY;};



void sfcdif2_loop(surface_layer *self)
{
	expertn_modul_base *xpn = &(self->parent);
	// Parameter:
	double RAIR   = 287.04;  //gas constant for dry air (j/kg/k)
	double CPAIR  = 1004.64; //heat capacity dry air at const pres (j/kg/k)
	double GRAV = 9.80616;   //acceleration due to gravity (m/s2)
	int ITRMX  = 5;
	double WWST   = 1.2;
    double WWST2  = WWST * WWST;
    double VKRM   = 0.40;
    double EXCM   = 0.001;
    double BETA   = 1.0 / 270.0;
    double BTG    = BETA * GRAV;
    double ELFC   = VKRM * BTG;
    double WOLD   = 0.15;
    double WNEW   = 1.0 - WOLD;
    double PIHF   = 3.14159265 / 2.0;
    double EPSU2  = 1.e-4;
    double EPSUST = 0.07;
    double EPSIT  = 1.e-4;
    double EPSA   = 1.e-8;
    double ZTMIN  = -5.0;
    double ZTMAX  = 1.0;
    double HPBL   = 1000.0;
    double SQVISC = 258.2;
    double RIC    = 0.183;
    double RRIC   = 1.0 / RIC;
    double FHNEU  = 0.8;
    double RFC    = 0.191;
    double RFAC   = RIC / ( FHNEU * RFC * RFC );
	
	double ZZ, PSLMU, PSLMS, PSLHU, PSLHS;
	double     XX, PSPMU, YY, PSPMS, PSPHU, PSPHS;
	double     ZILFC, ZU, ZT, RDZ, CXCH;
	double DTHV, DU2, BTGH, ZSLU, ZSLT, RLOGU, RLOGT;
	double     ZETALT, ZETALU, ZETAU, ZETAT, XLU4, XLT4, XU4, XT4;
	
	double     XLU, XLT, XU, XT, PSMZ, SIMM, PSHZ, SIMH, USTARK, RLMN,RLMA;

    int  ILECH, ITR;
	
	
	double CZIL=0.1; // Roughness length of heat?
	double Z0 = 0.123 * xpn->pPl->pCanopy->fPlantHeight; // = zom roughness length, momentum, ground (m)
	double ZLM = xpn->pCl->fWindMeasHeight; // = zlvl //reference height  (m)
	
	double SFCTMP = 273.16+xpn->pCl->pWeather->fTempAir;
	double SFCPRS = xpn->pCl->pWeather->fAtmPressure;
	//double THLM; // =THAIR potential temp at reference height (k)
	//double TAH; // Canopy Air Temperature (k)
	
	
	
	{
		//double PAIR;
		//PAIR   = SFCPRS;//                   ! atm bottom level pressure (pa)	
		//THLM  = SFCTMP * pow(SFCPRS/PAIR,(RAIR/CPAIR));
	}
/*	
	
// THIS ROUTINE SFCDIF CAN HANDLE BOTH OVER OPEN WATER (SEA, OCEAN) AND
// OVER SOLID SURFACE (LAND, SEA-ICE).
// ----------------------------------------------------------------------
//     ZTFC: RATIO OF ZOH/ZOM  LESS OR EQUAL THAN 1
//     C......ZTFC=0.1
//     CZIL: CONSTANT C IN Zilitinkevich, S. S.1995,:NOTE ABOUT ZT
// ----------------------------------------------------------------------
    ILECH = 0;

// ----------------------------------------------------------------------
    ZILFC = - CZIL * VKRM * SQVISC;
    ZU = Z0;
    RDZ = 1./ ZLM;
    CXCH = EXCM * RDZ;
    //DTHV = THLM - THZ0
	
	double tmp1;
			tmp1 = vkc * (grav/tvir)*h / (rhoair*cpair);
			if (fabs(tmp1) < mpe) 
				{
					tmp1 = mpe;
				}
			// Kominiere die Gleichung  S.899 (2) und Gleichung S.900 (15)
			*mol = -1.0 * pow(*fv,3.0) / tmp1;
			*moz = min(((zlvl-zpd)/ *mol),1.0);

// BELJARS CORRECTION OF USTAR
    DU2 = MAX (SFCSPD * SFCSPD,EPSU2)
    BTGH = BTG * HPBL

    IF(ITER == 1) THEN
        IF (BTGH * AKHS * DTHV .ne. 0.0) THEN
           WSTAR2 = WWST2* ABS (BTGH * AKHS * DTHV)** (2./3.)
        ELSE
           WSTAR2 = 0.0
        END IF
        USTAR = MAX (SQRT (AKMS * SQRT (DU2+ WSTAR2)),EPSUST)
        RLMO = ELFC * AKHS * DTHV / USTAR **3
    END IF
 
! ZILITINKEVITCH APPROACH FOR ZT
    ZT = MAX(1.E-6,EXP (ZILFC * SQRT (USTAR * Z0))* Z0)
    ZSLU = ZLM + ZU
    ZSLT = ZLM + ZT
    RLOGU = log (ZSLU / ZU)
    RLOGT = log (ZSLT / ZT)
	
*/	
}

/*  SUBROUTINE SFCDIF2(ITER   ,Z0     ,THZ0   ,THLM   ,SFCSPD , & !in
                     CZIL   ,ZLM    ,ILOC   ,JLOC   ,         & !in
                     AKMS   ,AKHS   ,RLMO   ,WSTAR2 ,         & !in
                     USTAR  )                                   !out

! -------------------------------------------------------------------------------------------------
! SUBROUTINE SFCDIF (renamed SFCDIF_off to avoid clash with Eta PBL)
! -------------------------------------------------------------------------------------------------
! CALCULATE SURFACE LAYER EXCHANGE COEFFICIENTS VIA ITERATIVE PROCESS.
! SEE CHEN ET AL (1997, BLM)
! -------------------------------------------------------------------------------------------------

    IMPLICIT NONE
    INTEGER, INTENT(IN) :: ILOC
    INTEGER, INTENT(IN) :: JLOC
    INTEGER, INTENT(IN) :: ITER
    REAL,    INTENT(IN) :: ZLM, Z0, THZ0, THLM, SFCSPD, CZIL
    REAL, intent(INOUT) :: AKMS
    REAL, intent(INOUT) :: AKHS
    REAL, intent(INOUT) :: RLMO
    REAL, intent(INOUT) :: WSTAR2
    REAL,   intent(OUT) :: USTAR

    REAL     ZZ, PSLMU, PSLMS, PSLHU, PSLHS
    REAL     XX, PSPMU, YY, PSPMS, PSPHU, PSPHS
    REAL     ZILFC, ZU, ZT, RDZ, CXCH
    REAL     DTHV, DU2, BTGH, ZSLU, ZSLT, RLOGU, RLOGT
    REAL     ZETALT, ZETALU, ZETAU, ZETAT, XLU4, XLT4, XU4, XT4

    REAL     XLU, XLT, XU, XT, PSMZ, SIMM, PSHZ, SIMH, USTARK, RLMN,  &
         &         RLMA

    INTEGER  ILECH, ITR

    INTEGER, PARAMETER :: ITRMX  = 5
    REAL,    PARAMETER :: WWST   = 1.2
    REAL,    PARAMETER :: WWST2  = WWST * WWST
    REAL,    PARAMETER :: VKRM   = 0.40
    REAL,    PARAMETER :: EXCM   = 0.001
    REAL,    PARAMETER :: BETA   = 1.0 / 270.0
    REAL,    PARAMETER :: BTG    = BETA * GRAV
    REAL,    PARAMETER :: ELFC   = VKRM * BTG
    REAL,    PARAMETER :: WOLD   = 0.15
    REAL,    PARAMETER :: WNEW   = 1.0 - WOLD
    REAL,    PARAMETER :: PIHF   = 3.14159265 / 2.
    REAL,    PARAMETER :: EPSU2  = 1.E-4
    REAL,    PARAMETER :: EPSUST = 0.07
    REAL,    PARAMETER :: EPSIT  = 1.E-4
    REAL,    PARAMETER :: EPSA   = 1.E-8
    REAL,    PARAMETER :: ZTMIN  = -5.0
    REAL,    PARAMETER :: ZTMAX  = 1.0
    REAL,    PARAMETER :: HPBL   = 1000.0
    REAL,    PARAMETER :: SQVISC = 258.2
    REAL,    PARAMETER :: RIC    = 0.183
    REAL,    PARAMETER :: RRIC   = 1.0 / RIC
    REAL,    PARAMETER :: FHNEU  = 0.8
    REAL,    PARAMETER :: RFC    = 0.191
    REAL,    PARAMETER :: RFAC   = RIC / ( FHNEU * RFC * RFC )

! ----------------------------------------------------------------------
! NOTE: THE TWO CODE BLOCKS BELOW DEFINE FUNCTIONS
! ----------------------------------------------------------------------
! LECH'S SURFACE FUNCTIONS
    PSLMU (ZZ)= -0.96* log (1.0-4.5* ZZ)
    PSLMS (ZZ)= ZZ * RRIC -2.076* (1. -1./ (ZZ +1.))
    PSLHU (ZZ)= -0.96* log (1.0-4.5* ZZ)
    PSLHS (ZZ)= ZZ * RFAC -2.076* (1. -1./ (ZZ +1.))
! PAULSON'S SURFACE FUNCTIONS
    PSPMU (XX)= -2.* log ( (XX +1.)*0.5) - log ( (XX * XX +1.)*0.5)   &
         &        +2.* ATAN (XX)                                            &
         &- PIHF
    PSPMS (YY)= 5.* YY
    PSPHU (XX)= -2.* log ( (XX * XX +1.)*0.5)
    PSPHS (YY)= 5.* YY

! THIS ROUTINE SFCDIF CAN HANDLE BOTH OVER OPEN WATER (SEA, OCEAN) AND
! OVER SOLID SURFACE (LAND, SEA-ICE).
! ----------------------------------------------------------------------
!     ZTFC: RATIO OF ZOH/ZOM  LESS OR EQUAL THAN 1
!     C......ZTFC=0.1
!     CZIL: CONSTANT C IN Zilitinkevich, S. S.1995,:NOTE ABOUT ZT
! ----------------------------------------------------------------------
    ILECH = 0

! ----------------------------------------------------------------------
    ZILFC = - CZIL * VKRM * SQVISC
    ZU = Z0
    RDZ = 1./ ZLM
    CXCH = EXCM * RDZ
    DTHV = THLM - THZ0

! BELJARS CORRECTION OF USTAR
    DU2 = MAX (SFCSPD * SFCSPD,EPSU2)
    BTGH = BTG * HPBL

    IF(ITER == 1) THEN
        IF (BTGH * AKHS * DTHV .ne. 0.0) THEN
           WSTAR2 = WWST2* ABS (BTGH * AKHS * DTHV)** (2./3.)
        ELSE
           WSTAR2 = 0.0
        END IF
        USTAR = MAX (SQRT (AKMS * SQRT (DU2+ WSTAR2)),EPSUST)
        RLMO = ELFC * AKHS * DTHV / USTAR **3
    END IF
 
! ZILITINKEVITCH APPROACH FOR ZT
    ZT = MAX(1.E-6,EXP (ZILFC * SQRT (USTAR * Z0))* Z0)
    ZSLU = ZLM + ZU
    ZSLT = ZLM + ZT
    RLOGU = log (ZSLU / ZU)
    RLOGT = log (ZSLT / ZT)

! ----------------------------------------------------------------------
! 1./MONIN-OBUKKHOV LENGTH-SCALE
! ----------------------------------------------------------------------
    ZETALT = MAX (ZSLT * RLMO,ZTMIN)
    RLMO = ZETALT / ZSLT
    ZETALU = ZSLU * RLMO
    ZETAU = ZU * RLMO
    ZETAT = ZT * RLMO

    IF (ILECH .eq. 0) THEN
       IF (RLMO .lt. 0.)THEN
          XLU4 = 1. -16.* ZETALU
          XLT4 = 1. -16.* ZETALT
          XU4  = 1. -16.* ZETAU
          XT4  = 1. -16.* ZETAT
          XLU  = SQRT (SQRT (XLU4))
          XLT  = SQRT (SQRT (XLT4))
          XU   = SQRT (SQRT (XU4))

          XT = SQRT (SQRT (XT4))
          PSMZ = PSPMU (XU)
          SIMM = PSPMU (XLU) - PSMZ + RLOGU
          PSHZ = PSPHU (XT)
          SIMH = PSPHU (XLT) - PSHZ + RLOGT
       ELSE
          ZETALU = MIN (ZETALU,ZTMAX)
          ZETALT = MIN (ZETALT,ZTMAX)
          PSMZ = PSPMS (ZETAU)
          SIMM = PSPMS (ZETALU) - PSMZ + RLOGU
          PSHZ = PSPHS (ZETAT)
          SIMH = PSPHS (ZETALT) - PSHZ + RLOGT
       END IF
! ----------------------------------------------------------------------
! LECH'S FUNCTIONS
! ----------------------------------------------------------------------
    ELSE
       IF (RLMO .lt. 0.)THEN
          PSMZ = PSLMU (ZETAU)
          SIMM = PSLMU (ZETALU) - PSMZ + RLOGU
          PSHZ = PSLHU (ZETAT)
          SIMH = PSLHU (ZETALT) - PSHZ + RLOGT
       ELSE
          ZETALU = MIN (ZETALU,ZTMAX)
          ZETALT = MIN (ZETALT,ZTMAX)
          PSMZ = PSLMS (ZETAU)
          SIMM = PSLMS (ZETALU) - PSMZ + RLOGU
          PSHZ = PSLHS (ZETAT)
          SIMH = PSLHS (ZETALT) - PSHZ + RLOGT
       END IF
! ----------------------------------------------------------------------
       END IF

! ----------------------------------------------------------------------
! BELJAARS CORRECTION FOR USTAR
! ----------------------------------------------------------------------
       USTAR = MAX (SQRT (AKMS * SQRT (DU2+ WSTAR2)),EPSUST)

! ZILITINKEVITCH FIX FOR ZT
       ZT = MAX(1.E-6,EXP (ZILFC * SQRT (USTAR * Z0))* Z0)
       ZSLT = ZLM + ZT
!-----------------------------------------------------------------------
       RLOGT = log (ZSLT / ZT)
       USTARK = USTAR * VKRM
       AKMS = MAX (USTARK / SIMM,CXCH)
!-----------------------------------------------------------------------
! IF STATEMENTS TO AVOID TANGENT LINEAR PROBLEMS NEAR ZERO
!-----------------------------------------------------------------------
       AKHS = MAX (USTARK / SIMH,CXCH)

       IF (BTGH * AKHS * DTHV .ne. 0.0) THEN
          WSTAR2 = WWST2* ABS (BTGH * AKHS * DTHV)** (2./3.)
       ELSE
          WSTAR2 = 0.0
       END IF
!-----------------------------------------------------------------------
       RLMN = ELFC * AKHS * DTHV / USTAR **3
!-----------------------------------------------------------------------
!     IF(ABS((RLMN-RLMO)/RLMA).LT.EPSIT)    GO TO 110
!-----------------------------------------------------------------------
       RLMA = RLMO * WOLD+ RLMN * WNEW
!-----------------------------------------------------------------------
       RLMO = RLMA

!       write(*,'(a20,10f15.6)')'SFCDIF: RLMO=',RLMO,RLMN,ELFC , AKHS , DTHV , USTAR
!    END DO
! ----------------------------------------------------------------------
  END SUBROUTINE SFCDIF2
!jref:start 
! ==================================================================================================*/


int surface_layer_setup_stability(surface_layer *self)
{
	expertn_modul_base *xpn = &(self->parent);
	int niterc=20;
	int iter; 
	double moz; // input Monin-Obukhov stability (z/L)
	double fm; // ψ_m((z-d0)/L)
	double fh; // ψ_h((z-d0)/L)
	double fv; // !friction velocity (m/s)
	double mol; // !Monin-Obukhov length (m)
	double cm; // drag coefficient for momentum
	double ch; // drag coefficient for heat
	int mozsign; // number of times moz changes sign
	

	
	for (iter=1;iter < niterc+1;iter++)
		{
			
			sfcdif1_loop(self,iter,&moz,&fm,&fh,&fv,&mol,&cm,&ch,&mozsign);
		}
	C_DEBUG(xpn->pHe->pHEBalance->fHeatSens)
	C_DEBUG(moz);
	C_DEBUG(fm);
	C_DEBUG(fh);
	C_DEBUG(fv);
	C_DEBUG(mol);
	C_DEBUG(cm);
	C_DEBUG(ch);


	return RET_SUCCESS;
}
