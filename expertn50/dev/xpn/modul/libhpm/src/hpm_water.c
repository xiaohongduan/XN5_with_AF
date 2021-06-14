
#include "hpm_water.h"
#include "hpm_photosynthesis.h"
#include "hpm_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int hpm_water_uptake_hpm(hpm *self)
{
	return hpm_water_uptake(self,0);
}

int hpm_water_uptake_feddes(hpm *self)
{
	return hpm_water_uptake(self,1);
}



int hpm_water_uptake(hpm *self,int water_uptake_model)
{
	expertn_modul_base *xpn = &(self->parent);
//	double Time;

//	Time = xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay;


	self->Environment.DayLength = calculateDayLength((double) xpn->pLo->pFarm->fLatitude, xpn->pTi->pSimTime->fTimeY) * 24.0;
	self->Control.light_on = hpm_get_light_on(-xpn->pLo->pFarm->fLongitude,self->Control.Lz,self->Environment.DayLength,xpn->pTi->pSimTime->fTimeDay);
	C_DEBUG(self->Control.light_on);
	//self->Control.light_on = hpm_math_cpulse(xpn->pTi->pSimTime->fTimeDay, 0.5 * (1.0 - self->Environment.DayLength/24.0) - 0.000001,1.0,self->Environment.DayLength/24.0);
	//C_DEBUG(self->Control.light_on);

	self->Environment.Tair = xpn->pCl->pWeather->fTempAir;

	{
		struct_Soil_Vars soil_vars;
		double DWrt, DWsh; // [kg Water / (m^2 * day)] Rates from the pools
		//double Psi_so; //Matrix Potential soil
		double Psi_rt,Psi_sh;
		double Psi_rt_os,Psi_sh_os; // osmotischer Teil des Matrixpotentials
		double Psi_rt_pr,Psi_sh_pr; // pressure teil des Matrixpotentials
		double Tair; // Soil Temperature in °C
		double MSrt,MSsh; // Mass of Substrate
		double IWrt_sh;
		double IWso_rt;
		double dt =  xpn->pTi->pTimeStep->fAct;
		double rho_Wa = self->parameter.water.rho_Wa;// 1000.0;// Dichte vom Wassser kg water / m^3
		double cpsiPr = self->parameter.photosynthesis.cpsiPr;//0.2; // kg plant structure (kg water)-1. Parameter of plant pressure potential characteristic.
		double epsilon = self->parameter.photosynthesis.epsilon;//2.0e6; //Pa. A sort of Young's modulus for the plant cell.
		double Rgas = self->parameter.photosynthesis.Rgas;// 8314.0; // J (kg mole)-1 K-1
		double fSos_ac = self->parameter.photosynthesis.fSos_ac;//2.0; // Dimensionless.  Multiplier for plant storage material to give osmotically active amount.
		double rmm_osm = self->parameter.photosynthesis.rmm_osm;//20.0;  // Dimensionless [also, kg (kg mole)-1].  Mean relative molecular mass (molar mass) of osmoticum.
		double rmmCS = self->parameter.photosynthesis.rmmCS;//28.5;  //Dimensionless. Relative molecular masses of C substrate (sucrose / 12 = 342 / 12 = 28.5) and of N substrate (nitrate = NO3 = 14 + 3 * 16 = 62).
		//Also, with units of kg (kg mole)-1,  these are the molar masses of C, N substrates.
		double rmmNS = self->parameter.photosynthesis.rmmNS;//62.0;  // Dimensionless. Relative molecular masses of C substrate (sucrose / 12 = 342 / 12 = 28.5) and of N substrate (nitrate = NO3 = 14 + 3 * 16 = 62).
		//   Also, with units of kg (kg mole)-1,
		//double theta_soil_MAX; //= (double) pSLW[layer_depth]->fContMax;//0.4; m3 water (m3 soil)-1.  Maximum value of soil relative water content.
		double mu_W = self->parameter.photosynthesis.mu_W;//18; // kg/(kg mole) molare Masse vom Wasser
		// Die verschiedenen Schichten, werden in zunächst Arrays der Größe layer_count uebergeben

		get_SoilVars(self,&soil_vars);
		Tair =  self->Environment.Tair;

		MSrt = self->Plant.MCSrt*rmmCS/12.0 + self->Plant.MNSrt*rmmNS/14.0;
		MSsh = self->Plant.MCSsh*rmmCS/12.0 + self->Plant.MNSsh*rmmNS/14.0 + self->Plant.MNph_ul * 100.0 / 14.0;

		// 6.5a
		Psi_rt_os = -Rgas * (soil_vars.TSoil+273.15) * fSos_ac * MSrt / (rmm_osm*self->Water.Wrt);
		Psi_sh_os = -Rgas * (Tair+273.15) * fSos_ac * MSsh / (rmm_osm*self->Water.Wsh);

		// 6.5b:
		Psi_rt_pr = epsilon * (cpsiPr * self->Water.Wrt / self->Plant.MXrt- 1.0) / rho_Wa;
		Psi_sh_pr = epsilon * (cpsiPr * self->Water.Wsh / (self->Plant.MXLam + self->Plant.MXss)- 1.0) / rho_Wa;

		// total water Potential is sum of osmotic and pressure componente 6.5c
		// Gravity is ignored when water submodel is applied to grass but not to trees
		Psi_rt = Psi_rt_os + Psi_rt_pr;
		Psi_sh = Psi_sh_os + Psi_sh_pr;

		C_DEBUG(Psi_sh_os);
		C_DEBUG(Psi_sh_pr);

		self->Output.psi_rt = Psi_rt;
		self->Output.psi_sh = Psi_sh;



		//Root relative water content, thetart (dimensionless):
		{
			double z1rt,z2rt,z3rt, Wrt0;
			double MSrt;

			MSrt = self->Plant.MCSrt * rmmCS/12.0 + self->Plant.MNSrt*rmmNS/14.0;

			z1rt = epsilon * cpsiPr / (rho_Wa * self->Plant.MXrt);
			//z2rtchk = - ( psirt + epsilon / roW )
			//z2rt = - (Psi_rt + epsilon / rho_Wa);
			z2rt = - (epsilon / rho_Wa);
			z3rt = - Rgas * (soil_vars.TSoil+273.15) * fSos_ac * MSrt / rmm_osm;
			/*Wrtchk is to check our programMINg; it should = Wrt
			Wrtchk = ( -z2rtchk + &
			           SQRT (z2rtchk * z2rtchk - 4 * z1rt * z3rt) &
			         ) / (z1rt + z1rt)*/
			//Wrt0 (kg water m-2) is the quantity of water in the root at zero
			//water potential, i.e. psirt = 0:

			Wrt0   = (-z2rt + sqrt(z2rt * z2rt - 4.0 * z1rt * z3rt) ) / (z1rt + z1rt);
			self->Water.Wrt0 = Wrt0;
			self->Water.theta_rt = MAX(0.0,MIN(1.0,self->Water.Wrt / Wrt0));
			//self->Water.theta_rt = self->Water.Wrt / Wrt0;
			C_DEBUG(Wrt0);


		}

		{
			double z1sh,z2sh,z3sh;
			double MSsh;
			double Wsh0;
			MSsh = self->Plant.MCSsh * rmmCS/12.0 + self->Plant.MNSsh * rmmNS/14.0 + self->Plant.MNph_ul * 100.0 / 14.0;
			//Shoot relative water content, thetash (dimensionless):
			z1sh = epsilon * cpsiPr / (rho_Wa * (self->Plant.MXss+self->Plant.MXLam));
			//z2sh = -( Psi_sh + epsilon / rho_Wa);
			z2sh = -(epsilon / rho_Wa);
			z3sh = - Rgas * (Tair+273.15) * fSos_ac * MSsh / rmm_osm;
			//Wsh0 (kg water m-2) is the quantity of water in the shoot at zero
			//  !water potential, i.e. psish = 0:

			// Gl.: 6.5e:
			Wsh0   = ( -z2sh + sqrt(z2sh * z2sh - 4.0 * z1sh * z3sh)) / (z1sh + z1sh);


			C_DEBUG(Wsh0);
			self->Water.theta_sh = MAX(0.0,MIN(1.0,self->Water.Wsh / Wsh0));
			//self->Water.theta_sh = self->Water.Wsh / Wsh0;

		}

		self->Water.aWsh = exp(mu_W * Psi_sh / (Rgas* ((double)Tair + 273.15))); // 6.7a
		self->Water.aWrt = exp(mu_W * Psi_rt / (Rgas* ((double)soil_vars.TSoil + 273.15))); // 6.7a

		//self->Water.aWsh = MAX(MIN(self->Water.aWsh,1.0),1e-34);
		//self->Water.aWrt = MAX(MIN(self->Water.aWrt,1.0),1e-34);



		if (water_uptake_model==0) {
			DWrt = calculateWaterRootPool(self,&IWso_rt,&IWrt_sh,Psi_sh,Psi_rt,soil_vars.theta_soil, soil_vars.theta_soil_max);
		} else {
			DWrt = calculateWaterRootPool_feddes(self,&IWso_rt,&IWrt_sh,Psi_sh,Psi_rt,soil_vars.theta_soil);
		}

		DWsh = calculateWaterShootPool(self,IWrt_sh,Psi_rt, Psi_sh);

		{
			/* Da später im BioGrowth Modul Differentialgleichungen existieren, die den Wasservariablen gebrauch
			   machen, dürfen die Diffgleichungen erst nach dem BioGrowht Modul aufgerufen werfen
			   diese Routine verstucht das zu Emulieren
			 */




			/*if (self->Water.Wsh+DWsh*dt < EPSILON)
				{
					double DWsh_dt;
					DWsh_dt = EPSILON - self->Water.Wsh;
					xpn->pPl->pPltWater->fActTranspR = IWrt_sh - DWsh_dt/dt;
					self->Water.Wsh= EPSILON;
				}*/


			self->Water.Wrt+=DWrt*dt;
			self->Water.Wsh+=DWsh*dt;

			self->Water.Wrt=MAX(EPSILON,self->Water.Wrt);
			//self->Water.Wsh=MAX(EPSILON,self->Water.Wsh);


		}

		/*if (xpn->pPl->pPltWater->fActTranspR>xpn->pPl->pPltWater->fPotTranspR)
		{

			xpn->pPl->pPltWater->fActTranspR=xpn->pPl->pPltWater->fPotTranspR;
		}*/



		C_DEBUG(IWrt_sh);
		C_DEBUG(DWsh);
		C_DEBUG(xpn->pPl->pPltWater->fActTranspR);
		C_DEBUG(xpn->pPl->pPltWater->fPotTranspR);


	}



	return self->__ERROR;
}

/*
in:
 * latitude: Breitengrad in [Grad]
 * JulianDay: aktueller Tag im Jahr
*/
double calculateDayLength(double latitude, int JulianDay) // kap. 7.3
{

	double DayLength,latitude_RAD;
	double theta_year; // Jahreswinkel [RAD]
	double theta_sol_dec; //  Solar Declination [rad]
	double c1ty,c2ty,c3ty,c4ty,c5ty,c6ty;
	double s1ty,s2ty,s3ty,s4ty,s5ty;



	latitude_RAD = latitude * (M_PI / 180.0);
	theta_year = ((double)JulianDay - 80.0) * (2.0 * M_PI / 365); // 7.2a Jahreswinkel ist 0 am 21 März
	c1ty = cos(theta_year);
	c2ty = cos(2.0*theta_year);
	c3ty = cos(3.0*theta_year);
	c4ty = cos(4.0*theta_year);
	c5ty = cos(5.0*theta_year);
	c6ty = cos(6.0*theta_year);
	s1ty = sin(theta_year);
	s2ty = sin(2.0*theta_year);
	s3ty = sin(3.0*theta_year);
	s4ty = sin(4.0*theta_year);
	s5ty = sin(5.0*theta_year);

	theta_sol_dec = (M_PI / 180.0) * (0.38092 - 0.76996*c1ty + 23.26500*s1ty
	                                  + 0.36958*c2ty + 0.10868*s2ty + 0.01834*c3ty
	                                  - 0.16650*s3ty - 0.00392*c4ty + 0.00072*s4ty
	                                  - 0.00051*c5ty + 0.00250*s5ty + 0.00442*c6ty);  // 7.2b

	DayLength = (1.0/M_PI)*acos(-tan(latitude_RAD)*tan(theta_sol_dec));

	return DayLength;
}

// Calculates the Root Pool 6.3a
/*
 ret: DWrt: [kg Water / (m^2 * day)] Rates from the pools
**/
//theta_so_max = fContMax
double calculateWaterRootPool(hpm *self,double *IWso_rt,double *OWrt_sh, double Psi_sh,double Psi_rt, double theta_so, double theta_so_max)
{
	expertn_modul_base *xpn = &(self->parent);
	double L,T,T2,T3;
	double DWrt;// ret: [kg Water / (m^2 * day)] Rates from the pool
	//double IWso_rt; // input flux from soil;
	//double OWrt_sh; // output flux to shoot;
	double rWso_rt; // [kg-1 m4 s-1] restistance of water flow between soil and plant
	double gWrt_sh; // conductivity between root and shoot
	double Kso;
	double rho_rt;
	double MXrt;
	double rWrt_sh;
	double Psi_Soil_orig; // J/kg
	double psi_so_max; // J/kg
	double Ksomax; //0.1; //  kg m-3 s.  Parameter of soil hydraulic conductivity characteristic.
	//double IWso_rt; // Input from Soil
	// const:
	double cWTpl = self->parameter.water.cWTpl;//0.005;  // kg water (kg structural DM)-1 [J (kg water)-1]-1 day-1. Parameter for Water Transport in Plant. It is assumed independent of temperature.
	double cWrs_rt = self->parameter.water.cWrs_rt;//0.5e6; //  m s-1. Root surface to plant resistance parameter.
	double qpsi_soW = self->parameter.water.qpsi_soW;//3.5; //Dimensionless. Parameter of soil water characteristic.
	// TODO: sollte eigentlich zeischen 3 und 4 sein --> 6.2h

	double KWrs_rt = self->parameter.water.KWrs_rt;//1.0;  // kg structure m-2. Root surface to plant resistance parameter.*/
	double cWso_rs = self->parameter.water.cWso_rs;//80.0;  // m2. Soil to root surface resistance parameter.
	double roWpl20 = self->parameter.water.roWpl20; //Todo
	PSWATER pSW = xpn->pSo->pSWater->pNext;

	double fcontloc[500];
	double IWrt_sh_max;
	double dt;
	int i;

	dt = xpn->pTi->pTimeStep->fAct;

	T = xpn->pCl->pWeather->fTempAir;
	T2=T*T;
	T3=T2*T;

	L = -(0.0000614342*T3 + 0.00158927*T2 - 2.36418*T + 2500.79)*1e-3*1e6; // für J /m2/day



//	PWLAYER pWL = xpn->pWa->pWLayer->pNext;

	//pSW->fCampA --> mm



	{
		PWLAYER pWL;
		for (i=0,pWL = xpn->pWa->pWLayer; i<xpn->pSo->iLayers-1; i++,pWL = pWL->pNext) {
			fcontloc[i] = pWL->fContAct;
		}
	}



	{
		PWLAYER pWL = xpn->pWa->pWLayer->pNext;
		PSWATER pSW = xpn->pSo->pSWater->pNext;
		PSLAYER pSL = xpn->pSo->pSLayer->pNext;
		double fDepth;
		double fRootDepth;
		int count;
		double sumIWso_rt;
		double fCont_zw;
		double IWso_rt_max;
		double IWso_rt_min;


		fDepth = 0.0;
		count = 0;
		fRootDepth = (double)(xpn->pPl->pRoot->fDepth*1.0e-3);
		sumIWso_rt=0.0;

		*IWso_rt=0.0;

		while(1) {

			theta_so = pWL->fContAct;
			theta_so_max = pWL->fContMax;
			//*IWso_rt = MAX(*IWso_rt,0);
			psi_so_max = pSW->fCampA * 1.0e-3 * 9.81;
			qpsi_soW = pSW->fCampB;
			theta_so_max = pSW->fContSat-pSW->fContRes;
			//theta_so -= pSW->fContPWP;
			if (count >= xpn->pSo->iLayers-1) break;
			if (pWL->fHydrCond == 0.0) {

				pWL->fHydrCond = CONDUCTIVITY(pWL->fMatPotAct);
			}
			//C_DEBUG(Ksomax);
			//C_DEBUG(xpn->pWa->pWLayer->pNext->fHydrCond);
			Ksomax = pWL->fHydrCond/86400.0; // mm/ day --> mm/s
			Kso = Ksomax * pow(theta_so / theta_so_max,qpsi_soW + qpsi_soW + 3.0);
			Kso = pWL->fHydrCond/86400.0;
			rho_rt = self->Plant.MXrt / (xpn->pPl->pRoot->fDepth*1.0e-3);
			MXrt = self->Plant.MXrt;
			rWso_rt = cWrs_rt/rho_rt * (MXrt + KWrs_rt)/ MXrt + cWso_rs*rho_rt / (Kso * MXrt);

			// 6.2g
			Psi_Soil_orig = pWL->fMatPotAct * 1.0e-3 * 9.81; //psi_so_max * pow(theta_so_max / theta_so, qpsi_soW);

			*IWso_rt = 86400.0*(Psi_Soil_orig - Psi_rt)/rWso_rt;

			//*IWso_rt = MAX(IWrt_sh_max,*IWso_rt);


			//theta = pWL->fContAct-pSW->fContRes;



			// Wasserleitfähigkeit (abhängig von der strukturellen Masse): 6.3c

			gWrt_sh = cWTpl * self->Plant.MXrt * (self->Plant.MXLam+self->Plant.MXss) / (self->Plant.MXrt + self->Plant.MXLam + self->Plant.MXss);
			// Output to shoot (abhängig durch die jeweiligen Potentiale



			//IWrt_sh_max = -(self->Water.Wrt+*OWrt_sh*dt) / dt;



			// Input from soil:

			// 6.2h

			//Ksomax = xpn->pWa->pWLayer->pNext->fHydrCond/86400.0; // mm/ day --> mm/s

			/*if(xpn->pWa->pWLayer->pNext->fHydrCond>0)
				{
					Kso= xpn->pWa->pWLayer->pNext->fHydrCond/86400.0;
				}*/

			// Gl 6.2i


			CHECK_0(rho_rt)
			CHECK_0(MXrt)
			CHECK_0(Kso)

			// Gl. 6.2d




			//C_DEBUG(Psi_Soil_orig);

			fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m
			count++;

			IWso_rt_min = (pWL->fContAct - pSW->fContPWP)*pSL->fThickness/xpn->pTi->pTimeStep->fAct;
			IWso_rt_max = (pWL->fContAct - pSL->fPorosity)*pSL->fThickness/xpn->pTi->pTimeStep->fAct;

			*IWso_rt = MIN(IWso_rt_min,*IWso_rt);
			*IWso_rt = MAX(IWso_rt_max,*IWso_rt);

			if (pWL->fContAct- (*IWso_rt*xpn->pTi->pTimeStep->fAct / (1000.0*(double)pSL->fThickness*1.0e-3)) > pSW->fContPWP) {
				//maxflux = (pSL->fPorosity - pWL->fContAct);

                
				pWL->fContAct-=  (*IWso_rt*xpn->pTi->pTimeStep->fAct / (1000.0*(double)pSL->fThickness*1.0e-3));

				// Falls Wasser zurückfließt --> aufpassen, dass Sättigung nicht überschritten wird
				//fCont_zw = pWL->fContAct;
				//pWL->fContAct = MIN(fCont_zw,pSL->fPorosity);
				// Wird bereits im Wassermodell gesetzt:
				//xpn->pPl->pRoot->fUptakeR+=(*IWso_rt / (1000.0*(double)pSL->fThickness*1.0e-3));

				sumIWso_rt+=*IWso_rt;
			}

			if (count==1) {
				C_DEBUG(theta_so_max)
				C_DEBUG(psi_so_max)
				C_DEBUG(qpsi_soW)
				C_DEBUG(theta_so_max)
				C_DEBUG(Ksomax)
				C_DEBUG(Kso)
				C_DEBUG(rho_rt)
			}

			pSL = pSL->pNext;
			pWL = pWL->pNext;
			pSW = pSW->pNext;
			if (self->__USE_STATIC_ROOT_LENGTH==TRUE) {
				if (count >= self->__STATIC_ROOT_LENGTH) {
					break;
				}

			} else {
				if (fDepth>=fRootDepth) {
					break;
				}
			}

		}

		rWrt_sh = roWpl20 / self->Water.Wsh + roWpl20/self->Water.Wrt;


		*OWrt_sh = (Psi_rt - Psi_sh) / rWrt_sh;

		*IWso_rt=sumIWso_rt/(count);




	}
	{
		PWLAYER pWL;
		for (i=0,pWL = xpn->pWa->pWLayer; i<xpn->pSo->iLayers-1; i++,pWL = pWL->pNext) {
			fcontloc[i] = pWL->fContAct;
		}
	}

#ifdef TEST_HPM

	/*	{
			double func;
			double t = (double)xpn->pTi->pSimTime->fTimeY/365.0;

			func = cos(t);

			*IWso_rt = -cos(t*2.0*PI)+1.0;
			*OWrt_sh = -cos((t+0.001)*2.0*PI)+1.0;

			C_DEBUG(func);

		}*/



	C_DEBUG(*OWrt_sh);
	C_DEBUG(*IWso_rt);

	*IWso_rt = get_var(self,"OWSO_RT");
	*OWrt_sh = get_var(self,"OWRT_SH");
#endif

	C_DEBUG(*OWrt_sh);
	C_DEBUG(*IWso_rt);


	DWrt = *IWso_rt - *OWrt_sh; // 6.3a: Differential

	C_DEBUG(DWrt);
	return DWrt;
}



double calculateWaterRootPool_feddes(hpm *self,double *IWso_rt,double *OWrt_sh, double Psi_sh,double Psi_rt, double theta_so)
{
	expertn_modul_base *xpn = &(self->parent);
	int iLayer        = 0;
	double DeltaT     = xpn->pTi->pTimeStep->fAct;
	double roWpl20 = self->parameter.water.roWpl20; //Todo
	double rWrt_sh,DWrt;
	int				L,i,count;
	double			rRoot;
	double			dxM;
	double			Alfa;
	double			fEffectPot[MAXSCHICHT];
	double			fRootDensTotal;
	double			Sink;
	double fDepth,fRootDepth;
	double pot_water_uptake;
	PSLAYER pSL;
	PWLAYER       pWL = xpn->pWa->pWLayer;
	PLAYERROOT    pLR;
	//Added by Hong on 20181212
    PSWATER                         pSWL    =xpn->pSo->pSWater; 
	PSPROFILE		pSo		=xpn->pSo;
	pSL		=pSo->pSLayer;
	double fContAct;
    //End of Hong 

	rWrt_sh = roWpl20 / self->Water.Wsh + roWpl20/self->Water.Wrt;

	*OWrt_sh = (Psi_rt - Psi_sh) / rWrt_sh;
	
	fRootDepth = (xpn->pPl->pRoot->fDepth*1.0e-3);
	if (self->__USE_STATIC_ROOT_LENGTH==TRUE) {
		count=self->__STATIC_ROOT_LENGTH;
	} else {		
		fDepth = 0.0;
		count=0.0;
		pSL = xpn->pSo->pSLayer->pNext;
		while(fDepth<=fRootDepth) {
			if (count >= xpn->pSo->iLayers-1) break;
			fDepth += pSL->fThickness*1.0E-3; //fThickness: mm -> m
			count++;
			pSL = pSL->pNext;
		}
	}

	//

	for (i=1,pLR=xpn->pPl->pRoot->pLayerRoot; (i<xpn->pSo->iLayers-1); pLR=pLR->pNext,i++) {
		
		if (i<=count)
			{
				pLR->fLengthDens=1.0/(double)count;
			} else
			{
				pLR->fLengthDens=0.0;
			}
	}
	
	pot_water_uptake = self->Water.Wrt0-self->Water.Wrt;//-*OWrt_sh*xpn->pTi->pTimeStep->fAct;
	
	//Added by Hong 
	//if (pWL->fContAct<=pSWL->fContPWP)
        	//pot_water_uptake=(double)0.0;			
	//pot_water_uptake = MIN(pot_water_uptake,(pWL->fContAct<=pSWL->fContPWP));		
	//End of Hong
	
	if (pot_water_uptake <= 0.0)
		{
			*IWso_rt=0.0;
			return -*OWrt_sh;
		}
	


	fRootDensTotal = 0.0;
	*IWso_rt=0.0;
	for (SOIL2_LAYERS1(pWL, xpn->pWa->pWLayer->pNext, pLR, xpn->pPl->pRoot->pLayerRoot)) {
		fEffectPot[iLayer] = min(0.0,pWL->fMatPotOld);
		
		
		fRootDensTotal += pLR->fLengthDens;
	}  /* for */

	if (fRootDensTotal <= 0.0) {
		PRINT_ERROR_ID(xpn,"fRootDensTotal <= 0.0");
		self->__ERROR=1;
		return 0.0;
	}
	rRoot = pot_water_uptake;

	//End of Hong
	for (SOIL2_LAYERS1(pWL, xpn->pWa->pWLayer->pNext, pLR, xpn->pPl->pRoot->pLayerRoot)) {
		if (pLR->fLengthDens > 0.0) {
			dxM  = xpn->pSo->fDeltaZ;
			
			Alfa = hpm_water_FAlfa(self,rRoot,fEffectPot[iLayer]); 
			
			Sink = Alfa * pLR->fLengthDens / fRootDensTotal * rRoot/dxM;
			
            //Added by Hong after SPASS on 20181212
		    fContAct  = pWL->fContAct; 
			fContAct -= Sink; 

		    if (fContAct<=1.01*pSWL->fContPWP)
			   {
			    Sink *= fContAct/pSWL->fContPWP*xpn->pTi->pTimeStep->fAct/pSL->fThickness;
				//Sink=0;
			   }
		    //End of Hong
			
			*IWso_rt +=  Sink/xpn->pTi->pTimeStep->fAct*(1000.0*(double)dxM*1.0e-3);
			pWL->fContAct       -= Sink;
			pLR->fPotLayWatUpt   = Sink * dxM / DeltaT;
			
	//Added by Hong	
		    pSWL =pSWL->pNext;
		    pSL=pSL->pNext;
	//End of Hong
		} //if
		else pLR->fPotLayWatUpt = 0.0;
		
		
	} //for

	// hp 221002: fPotUptakedt wird in CERES zur Berechnung der Stressfaktoren
	//            benoetigt und wird danach gleich Null gesetzt (einmal pro Tag!).

	L 	  = 1;
	pLR	=xpn->pPl->pRoot->pLayerRoot;
	xpn->pPl->pPltWater->fPotUptakedt = 0.0;

	
	while (((pLR->fLengthDens!=0.0)||(pLR->pNext->fLengthDens !=0.0))
	       &&(L<xpn->pSo->iLayers-2)) {
		xpn->pPl->pPltWater->fPotUptakedt += pLR->fPotLayWatUpt*DeltaT;
		L 	 ++;
		
		
	
		pLR =pLR ->pNext;
		
		
	}






	DWrt = *IWso_rt - *OWrt_sh; // 6.3a: Differential

	/*C_DEBUG(count);
	C_DEBUG(*OWrt_sh);
	C_DEBUG(*IWso_rt);
	C_DEBUG(DWrt);*/

	return DWrt;
}



double hpm_water_FAlfa(hpm *self,double rRoot,double h)
{
	double afRSPar[8] = {	9999.0,
	                        -120000.0,		//Par1 --- h3 
	                        -8000.0,			//Par2 \__ h2
	                        -2000.0,			//Par3 /
	                        -250.0,			//Par4 --- h1
	                        -100.0,			//Par5 --- h0
	                        0.1,				//Par6
	                        0.5
	                    };			//Par7

	double res;
	double p0,p1,p2,p3;

	p0 = afRSPar[5];
	p1 = afRSPar[4];
	p3 = afRSPar[1];

	if      (rRoot < afRSPar[6]) p2 = afRSPar[2];
	else if (rRoot > afRSPar[7]) p2 = afRSPar[3];
	else                     p2 = (afRSPar[2]*(afRSPar[7] - rRoot) +
		                               afRSPar[3]*(rRoot - afRSPar[6]))  /  (afRSPar[7] - afRSPar[6]);
	res = 0.0;

    
	if      (h >  p3  &&  h < p2) res = (h-p3)/(p2-p3);
	else if	(h >= p2  &&  h < p1) res = 1.0;
	else if (h >= p1  &&  h < p0) res = (h-p0)/(p1-p0);
		
	return res;
}

// Calculates the Shoot Pool 6.4a
/*
 ret: DWsh: [kg Water / (m^2 * day)] Rates from the pools
**/
double calculateWaterShootPool(hpm *self,double IWrt_sh, double Psirt, double Psish)
{
	expertn_modul_base *xpn = &(self->parent);
	//double Time;
	double DWsh;// ret: [kg Water / (m^2 * day)] Rates from the pool
	//double IWso_rt; // input flux from soil;
	double IWsh, OWsh; // Input und Output of Water shoot
	double MXsh;
	double OWsh_an;
	double OWsh_atm;
	double OWsh_hv;
	double LAI;
	// Evapotranspiration:
	double fPARabs_can; // fraction of incident net radiation absorbed by the canopy
	double jNetRabs_can; // [J m^-2 day -1] net Radiation flux absorbed by canopy
	double jNetR; // [J m^-2 s-1] Instantaneous net radiation
	//double jparsc; // [J (PAR) m^-2 day-1] photosynthetic activ radiation
	double gblcon; // [m s^-1] boundary layer conductance
	double gcan; // [m s^-1] Canopy conductance
	int schalter;

	// const:
//	double cWTpl = self->parameter.water.cWTpl;//0.005;  // kg water (kg structural DM)-1 [J (kg water)-1]-1 day-1. Parameter for Water Transport in Plant. It is assumed independent of temperature.
	double crleaf = self->parameter.photosynthesis.crleaf;//0.15;  // Dimensionless. Leaf reflectivity coefficient. This is a plant parameter used only in the water submodel.
	double XIleaf = self->parameter.photosynthesis.XIleaf;//0.1;   // leaf transmissionskoefficient
	double kcan = self->parameter.photosynthesis.kcan;//0.5; //[m2 leaf (m2 ground)-1]-1. Canopy extinction	coefficient
	//double rNetR_PAR = self->parameter.photosynthesis.rNetR_PAR;//1.4; // [j (net radiation) (J (Paar)^-1]Ratio of daily net radiation to PAR.

	schalter=0;

//Time = xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay;

	MXsh = self->Plant.MXLam + self->Plant.MXss;
	LAI = hpm_math_sum4(self->Plant.LAI4);

	// Inputs from root:
	IWsh = IWrt_sh;

	// Ouput:
	// Output
	// Plant Evatranspiration (calculated by means of the Penman-Montheith equation):
	// 6.6a fraction of incident net radiation absorbed by the canopy
	// the same as the fraction as fraktion absorbed as ther fraction of PAR absorbed:
	fPARabs_can =  (1.0 - crleaf - XIleaf) / (1.0 - XIleaf) * (1.0 - exp(-kcan * LAI));

	C_DEBUG(fPARabs_can)

	// 7.5b [J m^-2 s-1] Instantaneous net radiation
	// ein teil der Strahlung ist der
	// jparsc ist etwa 50 % von fSolRad [J (PAR) m^-2 day-1] photosynthetic activ radiation
	
	// Das hier ist Schmarrn und Thornleys Fehler:
	/*jparsc = xpn->pCl->pWeather->fPAR * 1.0e6 / 86400.0;
	
	//CalculateActiveRadiation(self,(double)tim,self->Environment.DayLength, xpn->pCl->pWeather->fSolRad * 1.0e6);
	jNetR = rNetR_PAR * jparsc;
	 */
	 // es gilt (wobei jNETR nicht dir net radiation ist):
	jNetR = xpn->pCl->pWeather->fSolRad * 1.0e6;
	//jparsc = xpn->pCl->pWeather->fPAR * 1.0e6;//CalculateActiveRadiation(self,(double)Time,self->Environment.DayLength, xpn->pCl->pWeather->fSolRad * 1.0e6);
	//jNetR = rNetR_PAR * jparsc;
	//CHECK_VALID(jNetR);
	//C_DEBUG(jparsc/86400.0)
	//C_DEBUG(jNetR/86400.0)
	// 6.6b [J m^-2 day -1] net Radiation flux absorbed by canopy
	jNetRabs_can = fPARabs_can * jNetR;// * 86400.0;

	C_DEBUG(jNetRabs_can)
	gblcon = get_canopy_conductance(self,LAI);
	CHECK_VALID(gblcon)
	// 6.6e: Conopy conductance
	{
		double gstom;// stromatal conductance [m s-1 (m2 leaf)-1)]
		gstom = calculateLeafStomatalConductance(self,self->Water.theta_sh); // 3.2u
		if (gstom<=self->parameter.water.theta_gsmin+1e-10 ) {
			schalter=1;
		}
		
		xpn->pPl->pCanopy->fsto_res = 1.0/gstom;

		gcan = LAI * gstom;
		C_DEBUG(gstom);
		C_DEBUG(LAI);
		C_DEBUG(schalter);
	}
	CHECK_VALID(gcan)
	C_DEBUG(gcan);
	// 6.6d flux:
	{
		double s; //[kg m^-3 K^-1] d_rhoswv / d_T gradient of the saturated water vapour density
		double lambda; // [J kg-1] latent heat of evaporation of water
		double rhoswv; // [kg m^-3] water vapour density
		double rhowv; // [kg m^-3] actural water vapour density
		double gamma; // [kg m^-3 K^-1] psychrometric parameter
		double rel_hum; // (0, 1).  Relative humidity (instantaneous value)
		double T = self->Environment.Tair; // Temp Air;
		double T2 = T*T;
		double T3 = T2*T;
		double OWsh_atm_Nenner;
		struct_Soil_Vars soil_vars;
		//T = Environment.Tair;
		// interpolitation from Table 6.2 --> Table62.gnumeric
		s      = 1.09402e-8*T3 + 4.40793e-7*T2 + 2.07929e-5*T + 0.000333124; // [kg m-3 K-1] gradient of water density
		lambda = 0.777001*T3 - 40.7925*T2 - 1873.74*T + 2.50112e6; //[J kg-1] latent heat of evaporation of water
		rhoswv = 3.15618e-7*T3 + 8.2028e-6*T2 + 0.000319091*T + 0.00503291; // [kg m^-3] water vapour density
		rel_hum = xpn->pCl->pWeather->fHumidity/100.0;
		C_DEBUG(rel_hum);
		rhowv = rel_hum * rhoswv;
		gamma  = 4.32012e-10*T3 - 1.90443e-8*T2 - 1.11768e-6*T + 0.000521526; // [kg m^-3 K^-1] psychrometric parameter
		OWsh_atm_Nenner = lambda * ( s + gamma *(1.0 + gblcon /  gcan));
		CHECK_0(OWsh_atm_Nenner)
		OWsh_atm = s * jNetRabs_can/OWsh_atm_Nenner + (86400.0*lambda*gamma*gblcon*(rhoswv - rhowv))/OWsh_atm_Nenner;
		
		/*if (OWsh_atm>xpn->pPl->pPltWater->fPotTranspR)
			{
				OWsh_atm=xpn->pPl->pPltWater->fPotTranspR;
			}*/

#ifdef TEST_HPM
		//OWsh_atm = get_var(self,"OWSH_ATM");
#endif

		C_DEBUG(s * jNetRabs_can/OWsh_atm_Nenner);
		C_DEBUG((86400*lambda*gamma*gblcon*(rhoswv - rhowv))/OWsh_atm_Nenner);
		C_DEBUG(s);
		C_DEBUG(jNetRabs_can);
		C_DEBUG(OWsh_atm_Nenner);
		C_DEBUG(lambda)
		C_DEBUG(gblcon)
		C_DEBUG(gamma)
		C_DEBUG(1.0/gcan)
		// fPotTrans <= pET - aktEvap
		//OWsh_atm = MIN(OWsh_atm,xpn->pWa->fPotETR - xpn->pWa->pEvap->fActR);


		// ck20120204:
		C_DEBUG(OWsh_atm);
		if (OWsh_atm>xpn->pPl->pPltWater->fPotTranspR) {
			OWsh_atm=xpn->pPl->pPltWater->fPotTranspR;
		}
		//OWsh_atm=xpn->pPl->pPltWater->fPotTranspR;

		xpn->pPl->pPltWater->fActTranspR = OWsh_atm; // kg water / (m2 day) = mm / (m2 day)

		C_DEBUG(OWsh_atm)

		//if (schalter==1) OWsh_atm=0.0;//5e-3;


		//OWsh_atm = OWsh_atm/1000.0;

		// TODO: 3.0 korrektur, damit es mit original hpm stimmt

		//Das ist jetzt von mir, mal schaun ob das stimmt
		//ist überhaupt so viel wasser da?

		get_SoilVars(self,&soil_vars);

		//if (OWsh_atm > soil_vars.theta_soil)
		{
			//		OWsh_atm=soil_vars.theta_soil;
		}
		/*if (self->Water.theta_sh <= 0.3)
			{
				OWsh_atm=0.0;
			}*/

		CHECK_VALID(OWsh_atm);
	}

	// Output to animals, falls sie fressen 6.4d
	CHECK_0(MXsh)
	OWsh_an = self->Animal.IDMpl_an_gnd / MXsh * self->Water.Wsh; //pro Masse pro Wassergehalt
	// Output to Harvest: 6.4d
	//OWsh_hv = self->Harvest.Pulseharv * self->Harvest.fharv * self->Water.Wsh;
	OWsh_hv = self->Harvest.OWsh_hv;

	// Sum of all outputs:
	OWsh = OWsh_atm + OWsh_an + OWsh_hv;
	// Pool Equation: 6.4a
	DWsh = IWsh - OWsh;
	return DWsh;
}

// 6.6c  [m s^-1] boundary layer conductance
double get_canopy_conductance(hpm *self,double LAI)
{
	expertn_modul_base *xpn = &(self->parent);
	double knuK = self->parameter.water.knuK;//0.4; // (dimensionslos) Karmans constant
	double chcanLAI = self->parameter.water.chcanLAI;//0.026; // [m / (m^2 leaf  / (m^2 ground))^-1]
	double hcan = chcanLAI * LAI; // 4.8a Conopy height proportional zum LAI
	double Xiheatvap = 0.026*hcan; // [m] roughness parameter for heat and vapor
	double Ximntm = 0.13*hcan; // [m] roughness parameter for heat and vapor
	double d = 0.77*hcan; //[m] zero plane displacement
	double ln1,ln2,ln1_v,ln2_v,ln1_vm,ln2_vm;
	CHECK_0(Xiheatvap);
	CHECK_0(Ximntm);
	xpn->pPl->pCanopy->fRoughnessHeat = Xiheatvap;
	xpn->pPl->pCanopy->fRoughnessVapor = Ximntm;
	ln1_v = ((double)xpn->pCl->fWindMeasHeight + Xiheatvap - d)/ Xiheatvap;
	ln2_v = ((double)xpn->pCl->fWindMeasHeight + Ximntm - d)/ Ximntm;
	
	ln1_vm = max(ln1_v,1.00000001);
	ln2_vm = max(ln2_v,1.00000001);
	ln1 = log(ln1_vm);//log(min(ln1_v,(1.0+1.0e-34)));
	ln2 = log(ln2_vm);//log(min(ln2_v,(1.0+1.0e-34)));

	if ((ln1*ln2 == 0.0)||CHECK_DOUBLE_INVALID (ln1*ln2)) {
		PRINT_ERROR("Value ln1*ln2 is 0");
	}
	CHECK_0(ln1*ln2);

	return (knuK*knuK * (double)xpn->pCl->pWeather->fWindSpeed) / (ln1*ln2);
}

/*
Rechnet den Blatt stomata Fluss (vom CO2 aus) aus
Er ist hängt u.a. von der Luft CO2 konzentration ab

in:

out:
 * Plant.gsmax;
ret: gstom [m s-1 (m2 leaf)-1]
*/
double calculateLeafStomatalConductance(hpm *self,double theta_sh)
{
//	expertn_modul_base *xpn = &(self->parent);
	double gstom; //return value [m s-1 (m2 leaf)-1)]
	double fCO2_gstom;// CO2 faktor
	double gsmin;
	double gsmax;
	double cas;

	//const:
//	double gsmaxA = 0.005; //0.005 m s-1 (m2 leaf)-1. Maximum and minimum values of stomatal conductance at ambient CO2, 350 vpm.
//	double gsminA = 0.00005; // m s-1 (m2 leaf)-1. Maximum and minimum values of stomatal conductance at ambient CO2, 350 vpm.
//	double cgsCO2=2.0;       // Dimensionless.  A doubling of ambient CO2 concentration (CO2vpm) reduces stomatal conductance
	// (gstom) by 0.4.  See calculation of stomatal
	// conductance at end of plant submodel section.
//	double theta_gsmin = 0.65; //  Dimensionless. Value of shoot relative water content, thetash, when stomates are closed.
//	double theta_gsmax = 0.85; //  Dimensionless. Value of plant relative water content, thetash, when stomates are fully open.

	// Bei CO2 concentration von 350 vpm: Faktor ist 350, bei 700 vpm: Faktor verdoppelt sich
	fCO2_gstom = (1.0 + self->parameter.water.cgsCO2) / (1.0 + self->parameter.water.cgsCO2 * self->Environment.CO2vpm / 350.0);


	gsmin = fCO2_gstom * self->parameter.water.gsminA;
	gsmax = fCO2_gstom * self->parameter.water.gsmaxA;

	gstom = 0.0; // für den Fall, dass Variablen nicht gesetzt sind, oder Fehler auftreten
	cas = 0.0;


	// Falls kein licht und Tageschritte gewählt:
	if ((theta_sh <= self->parameter.water.theta_gsmin)  || ((self->Control.light_on==0.0)&&(self->Control.diurnal==1.0))) {
		cas = 1.0;
		gstom = gsmin;
	}
	if ((((theta_sh > self->parameter.water.theta_gsmin) && (theta_sh <= self->parameter.water.theta_gsmax))) && ( (self->Control.light_on==1.0) || (self->Control.diurnal==0.0))) {
		cas = 2.0;
		gstom= gsmin + (theta_sh - self->parameter.water.theta_gsmin) *
		       (gsmax - gsmin) / (self->parameter.water.theta_gsmax - self->parameter.water.theta_gsmin);
	}
	if (((theta_sh > self->parameter.water.theta_gsmax)) && ( (self->Control.light_on==1.0) || (self->Control.diurnal==0.0) )) {
		cas = 3.0;
		gstom = gsmax;
	} /* {
		cas = 4.0;
		PRINT_ERROR("Error: calculating gstom");
	}*/

	/*
	if (theta_sh <= self->parameter.water.theta_gsmin)
		{
			gstom = gsmin;
		} else
	if (theta_sh >= theta_sh > self->parameter.water.theta_gsmax)
		{
			gstom = gsmax;
		} else
		{
			gstom= gsmin + (theta_sh - self->parameter.water.theta_gsmin) *
		       (gsmax - gsmin) / (self->parameter.water.theta_gsmax - self->parameter.water.theta_gsmin);
		}*/

	// Error check:
	CHECK_0(gstom);
	/*if (theta_sh>=1.0-1.0e-10)
		{
			gstom = 1.0e-10;
		}*/
	//printf("cas: %f\n",cas);

	self->Plant.gsmax = gsmax;
	return gstom;
}
