//      noah.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "noah.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "xnoah_util.h"

#define C_DEBUG(var)\
	xpn_register_var_add_double(self->parent.pXSys->var_list,#var,var,__LINE__,TRUE);

#define C_DEBUG_LOC_IN_ARRAY(var,i) \
	{\
		gchar *S;\
		S  = g_strdup_printf("%s[%d]",#var,i);\
		xpn_register_var_add_double(self->parent.pXSys->var_list,S,var[i],__LINE__,TRUE); \
		g_free(S);\
	}\

G_DEFINE_TYPE(noah, noah, EXPERTN_MODUL_BASE_TYPE);


static void noah_class_init(noahClass *klass){}


static void noah_init(noah *self){}


static double calc_T1(double fdown, double emissi,double sfctmp,double ch,double q2, double th2, double sfcprs, double eta, double etp, double epsca, double zsoil1, double df1, double stc1)
{
	double sigma = 5.67e-8;
	double t24;
	double rch,rho,cp,rd,t2v;
	double yynum,yy,rr;
	double beta,zz1, T1;

	cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
	rd = 287.04; // R_spec for dry air [J / (kg*K)] -- Gas Konstante

	t2v = sfctmp * (1.0 + 0.61 * q2);
	t24 = sfctmp*sfctmp*sfctmp*sfctmp;

	if (t2v==0.0) return 273.15;
	if (sfcprs==0.0) return 273.15;
	if (etp==0.0) return 273.15;
	if (emissi==0.0) return 273.15;


	rho = sfcprs / (rd * t2v);

	rch = rho * cp * ch;

	rr = emissi*t24 * 6.48e-8 / (sfcprs * ch) + 1.0;

	yynum = fdown - emissi*sigma * t24;

	beta = eta*LVH2O/etp;
	
	//printf("%f\n",beta);

	yy = sfctmp + (yynum / rch + th2  - sfctmp - beta * epsca) / rr;
	

	zz1 = df1 / (-0.5 * zsoil1 * rch * rr) + 1.0;

	T1 = (yy + (zz1 - 1.0)  * stc1) / zz1;

	return T1;


}

// Our Modul Functions:

  
int noah_run_load(noah *self)
{
	expertn_modul_base *xpn = &(self->parent);
	self->ch = 1.e-4;
    self->cm = 1.e-4;
	self->T1 = xpn->pCl->pWeather->fTempAir_zlvl + 273.15;
	
	
	
	/*xpn->pPl->pCanopy->fRoughnessHeat=0.125848;
	xpn->pPl->pCanopy->fRoughnessVapor=0.125848;*/
	
	// Constants:
	self->sbeta = -2.0; // --> GENPARM.TBL
	self->czil = 0.1;// --> GENPARM.TBL
	return RET_SUCCESS;
}


int noah_run_senible_heat(noah *self)
{
	double cp,r,t2v;
	double sfctmp,q2;
	double sfcprs,T1V,Th2V,th2,Z0;
	double wind;
	double emissi,soldn,lwdn,fdown,eta,etp;
	double prcp,ssoil,df,df1;
	double shdfac,smc0,smcmax0,sh2o0,epsca;
	expertn_modul_base *xpn = &(self->parent);
	PCANOPY pCan = xpn->pPl->pCanopy;


	cp = 1004.6; // Cp specific heat capacity of dry air at contant pressure [J / (kg*K)] -- Wärmekapazität
	r = 287.04; // Gaskonstante
	smc0 = xpn->pWa->pWLayer->pNext->fContAct;
	smcmax0 = xpn->pSo->pSLayer->pNext->fPorosity;
	sh2o0 = xpn->pWa->pWLayer->pNext->fContAct;
	wind = xpn->pCl->pWeather->fWindSpeed;
	sfctmp = xpn->pCl->pWeather->fTempAir_zlvl + 273.15;	
	sfcprs = xpn->pCl->pWeather->fAtmPressure;
	emissi = xpn->pHe->fEmissi;
	soldn = xpn->pCl->pWeather->fPAR / ( 1.0e-6*86400.0);  // Watt/m2 <-- MJ/(m2*day)
	lwdn = xpn->pCl->pWeather->fSolRad / (1.0e-6*86400.0) - soldn;  // Watt/m2 <-- MJ/(m2*day)
	prcp = xpn->pCl->pWeather->fPreciRate / (60.0*60.0*24.0);
	
	// green vegetations fraction:
	if ((pCan->fCropCoverFrac <= 0.0)&&(pCan->fLAI > EPSILON))
		{
			if((pCan->fLAI/3.0) < 1.0)
			{
				shdfac =  pCan->fLAI/3.0;	// LAI converted to SoilCoverFrac
			}
			else
			{
				shdfac =  1.0;
			}
		}
		else
		{
			shdfac = pCan->fCropCoverFrac;
		}
	
	
    //eta = xpn->pHe->fHeatLatent
	
	eta = (xpn->pWa->fActETR) /(24.0*60.0*60.0);//(LVH2O * xpn->pWa->fActETR) /(24.0*60.0*60.0)/1000.0;;
	
	//eta = (xpn->pWa->pEvap->fActR) /(24.0*60.0*60.0);;
	
	//etp = self->etp_old;
	
	etp = xpn->pWa->fPotETR / (24.0*60.0*60.0);
	
	if (etp==0.0)
		{
			etp = eta;
		}
	
	q2 = xnoah_util_convert_rel_hum_to_spec_hum(sfctmp, sfcprs,xpn->pCl->pWeather->fHumidity);

	t2v = sfctmp * (1.0 + 0.61 * q2);
	
	th2 = sfctmp + ( 0.0098 * xpn->pCl->fTempMeasHeight);
	
	fdown = soldn* (1.0 - xpn->pSo->fAlbedo) + lwdn;
	
	
	
	df = xnoah_util_TDFCND (smc0,xpn->pSo->pSLayer->pNext->fSand/100.0, smcmax0, sh2o0);
	df1 = df * exp (shdfac*self->sbeta);
	
	ssoil = df1 * (self->T1 - (xpn->pHe->pHLayer->pNext->fSoilTemp+273.15)) / (0.5*xpn->pSo->pSLayer->pNext->fThickness*1.0e-3);
	
	epsca = xnoah_util_calc_epsca(0,0.0,sfcprs, t2v,self->T1,self->ch,etp);
	
	
	
	self->T1 = calc_T1(fdown, emissi,sfctmp,self->ch,q2,th2,sfcprs,eta,etp, epsca,-(xpn->pSo->pSLayer->pNext->fThickness*1.0e-3), df1, xpn->pHe->pHLayer->pNext->fSoilTemp+273.15);//xnoah_util_calc_T1(fdown, emissi,sfctmp,self->ch,q2,th2,sfcprs,eta,etp, epsca,-(xpn->pSo->pSLayer->pNext->fThickness*1.0e-3), df1, xpn->pHe->pHLayer->pNext->fSoilTemp+273.15,ssoil);
	
	
    T1V= self->T1 * (1.0+ 0.61 * q2);
    Th2V = th2 * (1.0+ 0.61 * q2);
		
		 
	
	Z0 = sqrt(xpn->pPl->pCanopy->fRoughnessHeat*xpn->pPl->pCanopy->fRoughnessVapor);
	//Z0 = 0.125848;
	
	
	//wind = 1.0e-4;
	
	
	
	xnoah_util_sfcdif_off(xpn->pCl->fWindMeasHeight,Z0,T1V,Th2V,wind,self->czil,&(self->cm),&(self->ch));
	
	

	// sheat:
	xpn->pHe->pHEBalance->fHeatSens = - (self->ch * cp * sfcprs)/ (r * t2v) * (th2- self->T1 );
	
	C_DEBUG(xpn->pHe->pHLayer->pNext->fSoilTemp+273.15);
	
	C_DEBUG(fdown);
	C_DEBUG(emissi);
	C_DEBUG(sfctmp);
	C_DEBUG(self->ch);
	C_DEBUG(q2);
	C_DEBUG(th2);
	C_DEBUG(sfcprs);
	C_DEBUG(eta);
	C_DEBUG(etp);
	C_DEBUG(epsca);
	C_DEBUG(etp);
	
	C_DEBUG(-(xpn->pSo->pSLayer->pNext->fThickness*1.0e-3));
	C_DEBUG(df1);
	
	C_DEBUG(Z0);
	
	C_DEBUG(self->ch);
	
	C_DEBUG(sfcprs);
	
	C_DEBUG(th2);
	
	C_DEBUG(self->T1);

	C_DEBUG(eta);

	C_DEBUG(xpn->pHe->pHEBalance->fHeatSens);
	
	C_DEBUG(ssoil);

	return RET_SUCCESS;
}

int noah_run_done(noah *self)
{
	return RET_SUCCESS;
}
