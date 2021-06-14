
#include "hpm_growth.h"
#include "hpm_math.h"
#include "hpm_water.h"
#include "hpm_photosynthesis.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>




int hpm_biomass_growth(hpm *self)
{
    expertn_modul_base *xpn = &(self->parent);
	PCHEMISTRY pCh = xpn->pCh; //Added by Hong
	PCBALANCE	pCB = pCh->pCBalance; //Added by Hong on 20181030
    double kdegsh; // Degration [day-^1]
    double kturnss; // day-1.   Rate parameters for Sheath and Stem turnover
    double kturnlam; // day-1.   Rate parameters for lamina turnover
    double fTsh; // Temperaturfunktion für shoot;
    double fWsh; // Wassereinfluss für shoot
    double fTrt; // Temperaturfunktion für root;
    double fWrt; // Wassereinfluss für root
    double Gsh; // Growthrate [kg structural DM shoot m-2 day-1]
    double Csh; // substrate C konzentration [kg C (kg structural DM)-1]
    double Nsh; // substrate N konzentration [kg C (kg structural DM)-1]
    double Grt; // Growthrate [kg structural DM shoot m-2 day-1]
    double Crt; // substrate C konzentration [kg C (kg structural DM)-1]
    double Nrt; // substrate N konzentration [kg C (kg structural DM)-1]

    double OXLam_li; // structural dry mass from Lamina to litter
    double OXss_li; // structural dry mass from ss to litter
    double OXrt_li; // structural dry mass from root to litter
    double kturnrt; // day-1.   Rate parameters for Sheath and Stem turnover
    double kdegrt; // Degration [day-^1]

    double ONSsh_Nph,INSsh_Nph;


    double Msh4[4]; // Mss4 + MLam4
    double SubstrateCFluxTORoot;
    double ICSsh_re, ICSrt_re; // Fluss der von Struktur Recycled wird
    double INSsh_rt,INph_li_NSsh;
    double dt;
    struct_Soil_Vars soil_vars;

    int i;

    //const:
    double qWpl = self->parameter.plant.qWpl;//20.0;
    double mu20 = self->parameter.plant.mu20;//400.0;//400.0;  //[(day [C] [N])-1].  Growth coefficient at 20 oC
    double flam = self->parameter.plant.flam;//0.7; // Dimensionless.  Fraction of new shoot growth partitioned to lamina.
    double kdegsh20 = self->parameter.plant.kdegsh20;//0.001;//0.001;  // day-1. Degradation of dhoot structure.  Half-life: about 1 year.
    double kturnss20 = self->parameter.plant.kturnss20;//0.08;/*0.08; /* day-1.  Rate parameters for root, laminar, sheath and stem
    /*turnover at 20 oC.  Remember there are 4 compartments.
    Half-life = 4 * 0.69 / k days.
    For roots, lamina, sheath + stem, about 1 month.
    See Whitehead, p. 91. Conflicting data on root turnover.
    Maybe we need to treat coarse roots and fine roots
    differently (as in forest model).*/
    double kturnlam20 = self->parameter.plant.kturnlam20;/*0.08;0.08; day-1.  Rate parameters for root, laminar, sheath and stem
                                        turnover at 20 oC.  Remember there are 4 compartments.
                                        Half-life = 4 * 0.69 / k days.
                                        For roots, lamina, sheath + stem, about 1 month.
                                        See Whitehead, p. 91. Conflicting data on root turnover.
                                        Maybe we need to treat coarse roots and fine roots
                                        differently (as in forest model).*/
    double kturnrt20 = self->parameter.plant.kturnrt20;/*0.08;0.08; <day-1.
                                        Rate parameters for root, laminar, sheath and stem
                                        turnover at 20 oC.  Remember there are 4 compartments.
                                        Half-life = 4 * 0.69 / k days.
                                        For roots, lamina, sheath + stem, about 1 month.
                                        See Whitehead, p. 91. Conflicting data on root turnover.
                                        Maybe we need to treat coarse roots and fine roots
                                        differently (as in forest model).*/
    double kdegrt20 = self->parameter.plant.kdegrt20;//0.001;//0.001; //day-1. Degradation of root, shoot structure.  Half-life: about 1 year.


    //PCLAYER pCLayer;
    //PHLAYER *pSLayerH;
    //PSWATER *pSWL;
//  PWLAYER pWLayer;
     
	 //fWrt =1.0; //Test of Hong
	 //fTrt =1.0; //Test of Hong
    
    dt = (double)xpn->pTi->pTimeStep->fAct;
    // Alle Fluxes werden auf 0 gesetzt und es werden alle fluesse pro Zeiteinheit zusammengebaut:
    clear_all_fluxes(self);

    // Die verschiedenen Schichten, werden in zunächst Arrays der Größe layer_count uebergeben


    get_SoilVars(self,&soil_vars);

    // Temperaturabhängigkeit: 3.11b
    fTrt = calculatePlantTemperatureProcesses_getf_HPM(self,soil_vars.TSoil);

    // Effect of Water: (3.11d / 6.7b)
	//fWrt =1.0; //Test of Hong
    fWrt = pow(self->Water.aWrt,qWpl);
    //fWrt = MAX(fWrt,1e-6); //Test of Hong

    fTsh = calculatePlantTemperatureProcesses_getf_HPM(self,(double)self->Environment.Tair); // 3.11b Temperatureinfluss

    fWsh =  pow(self->Water.aWsh,qWpl); // 3.11d Wassereinfluss	
    fWsh = MAX(fWsh,1e-34);
	fWsh = MIN(fWsh,1.0);

    CHECK_0((self->Plant.MXLam+self->Plant.MXss)) // 3.1h Substratgehalt (dh. Verhältnis subtrat zur Gesamtstructurmasse)
    Csh = self->Plant.MCSsh / (self->Plant.MXLam+self->Plant.MXss);

    CHECK_0((self->Plant.MXLam+self->Plant.MXss)) // 3.1h Substratgehalt (dh. Verhältnis subtrat zur Gesamtstructurmasse)
    Nsh = self->Plant.MNSsh / (self->Plant.MXLam+self->Plant.MXss);

    

    // Substrate C konzentration 3.3b
    CHECK_0(self->Plant.MXrt)
    Crt = self->Plant.MCSrt / self->Plant.MXrt;

    // Substrate N konz.: 3.3b
    CHECK_0(self->Plant.MXrt)
    Nrt = self->Plant.MNSrt / self->Plant.MXrt;



    // Wachstumsrate berechen:
	
	#ifdef TEST_HPM		
		//Csh = get_var(self,"CSSH");
		//Crt = get_var(self,"CSRT");
		//Csh = get_var(self,"CSSH");
	#endif

    Gsh = mu20 * fTsh * fWsh * Csh * Nsh * (self->Plant.MXLam+self->Plant.MXss);//(double)pPl->pRoot->fDepth*1.0e3; //3.3c

    Grt = mu20 * fTrt * fWrt * Crt * Nrt * self->Plant.MXrt; //3.3c
    
    C_DEBUG(Gsh);
    
    // Ausgabe:
    self->Output.Grt = Grt;
    self->Output.Gsh = Gsh;
    
    self->Output.fTsh = fTsh;
    self->Output.fWsh = fWsh;
    self->Output.Csh = Csh;
    self->Output.Nsh = Nsh;
    
    self->Output.fTrt = fTrt;
    self->Output.fWrt = fWrt;
    self->Output.Crt = Crt;
    self->Output.Nrt = Nrt;
    

    


    //Massendegenerationsrate,  beschreibt im Grunde den Verfall durch Krankheiten, ...
    kdegsh=kdegsh20 * fTsh; // 3.5a  // Degration, die durch Die Tempereratur beieinflusst wird
    CHECK_0(fWsh)
    kturnlam = kturnlam20 * fTsh / fWsh; // 3.5a [day-1] turnover rate: Rate mit der Massenaustausch von einer Altersschicht zur nächsten erfolgt: ist abhängig vom Termperatur Wasserverhältnis
    CHECK_0 (fWsh)
    kturnss = kturnss20* fTsh / fWsh; // 3.5a [day-1] turnover rate: Rate mit der Massenaustausch von einer Altersschicht zur nächsten erfolgt: ist abhängig vom Termperatur Wasserverhältnis

    //Massendegenerationsrate,  beschreibt im Grunde den Verfall durch Krankheiten, ...
    kdegrt=kdegrt20 * fTrt; // 3.5a  // Degration, die durch Die Tempereratur beieinflusst wird
	
    //if (fWrt <=0.01)
	 //  fWrt= 0.1;//Test of Hong	
		
    CHECK_0 (fWrt)
	
    kturnrt = kturnrt20 * fTrt/ fWrt; // 3.5a [day-1] turnover rate: Rate mit der Massenaustausch von einer Altersschicht zur nächsten erfolgt: ist abhängig vom Termperatur Wasserverhältnis

    
    // Msh4 = Mss4 + MLam4
    for (i=0; i!=4; i++) {
        Msh4[i] = self->Plant.Mss4[i] + self->Plant.MLam4[i];
    }
    


    calculateLaminaAreaPools_HPM(self,Gsh, self->Water.theta_sh, kturnlam,kdegsh );

    // Lamina Pool (3.5a)   result: strukt dry mass to litter:
    OXLam_li = calculateBiomassStructuralLamina(self, Gsh, kturnlam, kdegsh, fTsh, fWsh, flam );

    // Sheath and Stem Pool (3.5b):
    OXss_li = calculateBiomassStructuralSheathAndStem(self, Gsh, kturnss, kdegsh ,fTsh, fWsh, flam );

    // root structural Dry Mass Pool (3.5c)
    OXrt_li = calculateBiomassStructuralRoot(self,Grt,fTrt, fWrt,kturnrt, kdegrt, qWpl, mu20);

    // Photosynthetic N Pool:
    calculatePhotoynthetic_N(self,&ONSsh_Nph,&INSsh_Nph,&INph_li_NSsh,OXLam_li+OXss_li,fTsh,fWsh,self->Plant.MXLam+self->Plant.MXss);

    // Substrate Pools:

    // Substrat N Shoot:
    ICSsh_re = calculateBiomassSubstrateNShoot(self,&INSsh_rt,ONSsh_Nph,INSsh_Nph,INph_li_NSsh,Gsh, fTsh,fTrt,fWsh, fWrt,kturnlam,kturnss,kdegsh); // 3.4a

    // Substrat N Root:
    ICSrt_re = calculateBiomassSubstrateNRoot(self,INSsh_rt,soil_vars.Namm,soil_vars.Nnit,Grt,soil_vars.TSoil,fTrt,fWrt,soil_vars.theta_soil,kturnrt,kdegrt); // 3.4a

    // TODO: ICSsh_re;
    //INSsh_re
    // Substrat C Shoot (= ss + lamina)
    calculateBiomassSubstrateCShoot(self,&SubstrateCFluxTORoot, ICSsh_re, Msh4, Gsh, fTsh,fTrt, fWsh,fWrt, kturnlam, kturnss, kdegsh);

    // Substrat C Root:
    calculateBiomassSubstrateCRoot(self, soil_vars.Namm, soil_vars.Nnit, soil_vars.TSoil, soil_vars.theta_soil,SubstrateCFluxTORoot,ICSrt_re,Gsh, Grt, fTrt, fWrt , kturnrt , kdegrt);

    self->Litter.MXli += (self->Litter.OXsh_li + self->Litter.OXrt_li) * dt;
    self->Litter.MXrt += self->Litter.OXrt_li*dt;
    self->Litter.MXsh += self->Litter.OXsh_li*dt;
	
	//Added by Hong
	pCh->pCProfile->fCLitterSurf += self->Litter.OXsh_li*dt * mq_TO_ha_1;
	pCh->pCProfile->fCLitterSurf += self->Litter.OCSsh_so*dt * mq_TO_ha_1;
	
	pCh->pCProfile->fCLeafLitterSurf += (self->Litter.OXsh_li + self->Litter.OCSsh_so)*dt * mq_TO_ha_1;//Hong for century_n
	pCB->dCInputSurf += (self->Litter.OXsh_li + self->Litter.OCSsh_so)*dt * mq_TO_ha_1;
	//pCB->dCInputCum += (self->Litter.OXsh_li + self->Litter.OCSsh_so)*dt * mq_TO_ha_1;
	
	//End of Hong

    self->Litter.Nammrt_so+=self->Litter.INammrt_so *dt;
    self->Litter.Nitrt_so+=self->Litter.INitrt_so *dt;

    //calculateTotalMasses(exp_p);


    // Diffgleichungen:
    xpn->pPl->pCanopy->fLAI = (float)0.0;
    for (i=0; i!=4; i++) {
        self->Plant.LAI4[i]+=self->Plant.DLAI4[i]*dt;
        self->Plant.LAI4[i]=MAX(EPSILON,self->Plant.LAI4[i]);
        xpn->pPl->pCanopy->fLAI += self->Plant.LAI4[i];
    }

    self->Plant.MXLam = 0.0;
    for (i=0; i!=4; i++) {
        self->Plant.MLam4[i] += self->Plant.DMLam4[i]*dt;
        self->Plant.MLam4[i] = MAX(EPSILON,self->Plant.MLam4[i]);
        self->Plant.MXLam+=self->Plant.MLam4[i];
    }

    //Massen für den Zeitschritt ausrechnen und Strukturelle Gesamtmasse:
    self->Plant.MXss = 0.0;
    for (i=0; i!=4; i++) {
        self->Plant.Mss4[i] += self->Plant.DMss4[i]*dt;
        self->Plant.Mss4[i] = MAX(EPSILON,self->Plant.Mss4[i]);
        self->Plant.MXss+=self->Plant.Mss4[i];
    }

    //Massen für den Zeitschritt ausrechnen und Strukturelle Gesamtmasse:
    self->Plant.MXrt = 0.0;
    for (i=0; i!=4; i++) {
        self->Plant.Mrt4[i] += self->Plant.DMrt4[i]*dt;
        self->Plant.Mrt4[i] = MAX(EPSILON,self->Plant.Mrt4[i]);
        self->Plant.MXrt+=self->Plant.Mrt4[i];
    }
    self->Plant.MNSsh+=self->Plant.DMNSsh*dt;
    self->Plant.MNSrt+=self->Plant.DMNSrt*dt;
    self->Plant.MCSsh+=self->Plant.DMCSsh*dt;
    self->Plant.MCSrt+=self->Plant.DMCSrt*dt;
        
    self->Plant.MNph_ul+=self->Plant.DMNph_ul*dt;
    
    self->Plant.MNSsh = MAX(EPSILON,self->Plant.MNSsh);
    self->Plant.MNSrt = MAX(EPSILON,self->Plant.MNSrt);
    self->Plant.MCSsh = MAX(EPSILON,self->Plant.MCSsh);
    self->Plant.MCSrt = MAX(EPSILON,self->Plant.MCSrt);
    self->Plant.MNph_ul = MAX(EPSILON,self->Plant.MNph_ul);
    
    create_output(self);
    
    return self->__ERROR;   
}

// Set all vars of Animal, Harvest, Soil and litter to ziro
void clear_all_fluxes(hpm *self) {
	 //expertn_modul_base *xpn = &(self->parent);
    // Animal:
    /*set4(Animal.OXLam_an4,0.0);
    set4(Animal.OXss_an4,0.0);
    set4(Animal.OLAIi_an4,0.0);
    Animal.OCSsh_an_gr = 0.0;
    Animal.ONSsh_an_gr = 0.0;*/
    // Harvest:
    /*set4(Harvest.OXLam_hv4,0.0);
    set4(Harvest.OXss_hv4,0.0);
    set4(Harvest.OLAI_hv4,0.0);
    Harvest.OCSsh_hv = 0.0;
    Harvest.ONSsh_hv = 0.0;*/
    // Soil:
    /*
    self->Soil.OCSrt_so = 0.0;
    self->Soil.ONSrt_so = 0.0;
    */
    // Litter:
//  Litter.OCsh_so  = 0.0; Litter.ONsh_so  = 0.0; Litter.OCrt_so  = 0.0; Litter.ONrt_so  = 0.0;
    self->Litter.OCSsh_so = 0.0;
    self->Litter.ONSsh_so = 0.0;
    self->Litter.OCSrt_so = 0.0;
    self->Litter.ONSrt_so = 0.0;
//  Litter.OCXsh_so = 0.0; Litter.ONXsh_so = 0.0; Litter.OCXrt_so = 0.0; Litter.ONXrt_so = 0.0;
    self->Litter.OXsh_li  = 0.0;
    self->Litter.OXrt_li  = 0.0;
    
}

/*
Berechnet Temperatur funktion (für verschiedene Prozesse als Gewichtungsfaktor): //3.11a
in:
 * T: temperatur
ret:
 * f // Temperature function
*/

double calculatePlantTemperatureProcesses_getf_HPM(hpm *self, double T) { //3.11a
    expertn_modul_base *xpn = &(self->parent);
    double f;
    double zfT; // Nenner 3.11a

    // const:
    //double T0 = 0.0; //°C
    //double T0p = 45.0;  /**°C.  Temperatures at which the general plant
//                               temperature function is zero.*/
    //double Tref = 23.0;  /**<oC.  Reference temperature at which temperature
    //functions = 1.*/
    //double qfT = 1.9;   /**<Temperature response parameter; gives a cubic.  wird für photosynthese benötigt*/
    //double mfT = 1.0; /**< !Dimensionless.  A multiplier for the temperature function.*/
    
  /*  printf("self->parameter.temperature.T0: %f\n",self->parameter.temperature.T0);
    printf("elf->parameter.temperature.T0p: %f\n",self->parameter.temperature.T0p);
    printf("self->parameter.temperature.qfT: %f\n",self->parameter.temperature.qfT);
    printf("self->parameter.temperature.Tref: %f\n",self->parameter.temperature.Tref);
    printf("self->parameter.temperature.Tref: %f\n",(pow( self->parameter.temperature.Tref -  self->parameter.temperature.T0,self->parameter.temperature.qfT ) * ( self->parameter.temperature.T0p - self->parameter.temperature.Tref )));*/
    



    if ((T > self->parameter.temperature.T0) && (T < self->parameter.temperature.T0p)) { // T0 < T < T0'
        //CHECK_0((pow( self->parameter.temperature.Tref -  self->parameter.temperature.T0,self->parameter.temperature.qfT ) * ( self->parameter.temperature.T0p - self->parameter.temperature.Tref )))
        zfT = 1.0 / (pow( self->parameter.temperature.Tref -  self->parameter.temperature.T0,self->parameter.temperature.qfT ) * ( self->parameter.temperature.T0p - self->parameter.temperature.Tref )); // Nenner von 3.11a berechnen
        f = self->parameter.temperature.mfT*pow(T-self->parameter.temperature.T0,self->parameter.temperature.qfT)*(self->parameter.temperature.T0p-T)*zfT;
    } else {
        f = 1.0e-10; // eigentlich 0.0, allerdings tritt dann /0 Fehler auf
    }

    //Test of Hong 
	//f = MAX(f, 1.0e-6);

    return f;
}

void calculateLaminaAreaPools_HPM(hpm *self,double Gsh, double thetash, double kturnlam,double kdegsh ) {
    //double DLAI4[4];
    expertn_modul_base *xpn = &(self->parent);
    double OLAI4[4], ILAI4[4];
    double OLAI_deg4[4];
    double LAI01,LAI12,LAI23,LAI3li;
    double LAI1in;
    double Csh,Nsh;
    double vSLA; // incremental sprecifc leaf area
    double rCNSSh,rCNshq,KrCNSshSLAq;
    double dt; // Zeit [day-1]
    int i;

    double qrCNSsh=2.0;
    double KrCNSshSLA=2.0;

    // const:
    //double cSLAmax = 25.0;  //(m2 leaf) (kg structural DM)-1 Maximum value of incremental SLA.
    //double cSLA_C = 2.5;  // [C]-1; incremental specific leaf area (SLA)  parameter, describes dependence on substrate C concentration
    //double cSLA_W = 1.0;  // Dimensionless. Constant determining the  effect of plant relative water content on leaf area expansion (incremental specific leaf area, SLA).
    //double flam = 0.7; // Dimensionless.  Fraction of new shoot growth partitioned to lamina.

// 3.6a:

    Csh = self->Plant.MCSsh/ (self->Plant.MXLam+self->Plant.MXss); // 3.1h
    dt = (double)xpn->pTi->pTimeStep->fAct;

    // Degeneration durch Krankheiten
    for (i=0; i!=4; i++) {
        OLAI_deg4[i] =  kdegsh * self->Plant.LAI4[i]; // m2 leaf m-2 ground day-1.
    }

    //TODO: Wo geht die Degeneration hin, wahrscheinlich verschwindet sie im Nirvana


    /*Tissue turnover, transfers from age category to age category:*/

    LAI01 =  kturnlam * self->Plant.LAI4[0];//  Transfer, 1 to 2.
    LAI12=  kturnlam * self->Plant.LAI4[1];//     -      2 to 3.
    LAI23 =  kturnlam * self->Plant.LAI4[2];//     -      3 to 4.
    LAI3li = kturnlam * self->Plant.LAI4[3];//     -      4 to litter.
    //m2 leaf m-2 ground day-1.

    // Inputs:
    //vSLA, incremental specific leaf area [m2 (leaf) (kg structural DM)-1]: 3.6c
    //vSLA = parameter.plant.cSLAmax * ( 1.0 - parameter.plant.cSLA_C * Csh )
    //       * ( 1.0 - Control.water* parameter.plant.cSLA_W * (1.0 - thetash) );
    Nsh = self->Plant.MNSsh / (self->Plant.MXLam+self->Plant.MXss);
    rCNSSh = Csh/Nsh;
    rCNshq = pow(rCNSSh,qrCNSsh);
    KrCNSshSLAq = pow(KrCNSshSLA,qrCNSsh);
    vSLA = self->parameter.plant.cSLAmax * ( 1.0 - self->parameter.plant.cSLA_C * rCNshq/(KrCNSshSLAq+rCNshq) )
           * ( 1.0 - self->Control.water* self->parameter.plant.cSLA_W * (1.0 - thetash) );
    //vSLA is affected by shoot substrate C, Csh, and shoot relative water content, thetash.

    LAI1in = vSLA  * self->parameter.plant.flam * Gsh;   //m2 leaf m-2 ground day-1.


    // All Inputs:
    ILAI4[0] = LAI1in;
    ILAI4[1] = LAI01;
    ILAI4[2] = LAI12;
    ILAI4[3] = LAI23;


    // Alle Outputs

    OLAI4[0] = LAI01  + self->Animal.OLAI_an4[0] + self->Harvest.OLAI_hv4[0] + OLAI_deg4[0];
    OLAI4[1] = LAI12  + self->Animal.OLAI_an4[1] + self->Harvest.OLAI_hv4[1] + OLAI_deg4[1];
    OLAI4[2] = LAI23  + self->Animal.OLAI_an4[2] + self->Harvest.OLAI_hv4[2] + OLAI_deg4[2];
    OLAI4[3] = LAI3li + self->Animal.OLAI_an4[3] + self->Harvest.OLAI_hv4[3] + OLAI_deg4[3];


    //Lamina area pools (m2 leaf m-2 ground) Differential equations (m2 leaf m-2 ground day-1):

    for (i=0; i!=4; i++) {
        self->Plant.DLAI4[i] = ILAI4[i] - OLAI4[i];
    }   

}

double calculateBiomassStructuralLamina(hpm *self, double Gsh, double kturnlam, double kdegsh, double fTsh, double fWsh, double flam) {
    expertn_modul_base *xpn = &(self->parent);
    double OXLam_li; // structural dry mass from Lamina to litter
    double OXlam_deg4[4]; // Masse die Abgebaut wird kg structural DM m-2 day-1.
    double OXlam_deg; // komplette Masse die Abgebaut wird kg structural DM m-2 day-1.
//  double IXlam0; // MassenWachstumsrate
    double OXlam0_1,OXlam1_2,OXlam2_3,OXlam3_li; //Fluss jeweils von einer Schicht in die nächste
    double OXlam4[4]; // geht aus der entsprechenden Schicht raus
    double IXlam4[4]; // geht in die entsprechend Schicht rein
    // double DMLam4[4]; // GesamtMassenänderungsrate
    double dt; // Zeitschritt;
    int i;
    //const:



    dt = (double)xpn->pTi->pTimeStep->fAct;



    //Massendegeneration in OXlam_deg4 schreiben (beschreibt im Grunde den Verfall durch Krankheiten, ... Degration, die durch Die Tempereratur beieinflusst wird
    OXlam_deg = 0.0; //totalle Masse, die abgebaut wird
    for (i=0; i!=4; i++) {
        OXlam_deg4[i] = kdegsh * self->Plant.MLam4[i];
        OXlam_deg += OXlam_deg4[i];
    }


    //Leaf turnover, transfers from age category to age category:
    OXlam0_1 =  kturnlam * self->Plant.MLam4[0];//  !Transfer, 0 to 1.
    OXlam1_2 =  kturnlam * self->Plant.MLam4[1];//  !   -      1 to 2.
    OXlam2_3 =  kturnlam * self->Plant.MLam4[2];//  !   -      2 to 3.
    OXlam3_li = kturnlam * self->Plant.MLam4[3];//  !   -      3 to litter.


    // Jetzt: aufsummieren was rausgeht und was reingeht, ernte und animal fressen momentan nix
    // Was geht raus:
    OXlam4[0] = OXlam0_1  + self->Animal.OXLam_an4[0] + self->Harvest.OXLam_hv4[0] + OXlam_deg4[0];  // nach blatt 1, nach animal 0, nach ernte 0, nach krankheit 0
    OXlam4[1] = OXlam1_2  + self->Animal.OXLam_an4[1] + self->Harvest.OXLam_hv4[1] + OXlam_deg4[1];  // nach blatt 2, nach animal 1, nach ernte 1, nach krankheit 1
    OXlam4[2] = OXlam2_3  + self->Animal.OXLam_an4[2] + self->Harvest.OXLam_hv4[2] + OXlam_deg4[2];  // nach blatt 3, nach animal 2, nach ernte 2, nach krankheit 2
    OXlam4[3] = OXlam3_li + self->Animal.OXLam_an4[3] + self->Harvest.OXLam_hv4[3] + OXlam_deg4[3];  // nach litter, nach animal 0, nach ernte 0, nach krankheit 0

    // Was geht rein:
    IXlam4[0] = flam * Gsh ; // teil der Wachstumsrate (shoot), die zum Blaetterteil beiträgt
    IXlam4[1] = OXlam0_1;    // vom blatt 0
    IXlam4[2] = OXlam1_2;    // vom blatt 1
    IXlam4[3] = OXlam2_3;    // vom blatt 2

    //Blattrate bestimmen (day-1):
    for (i=0; i!=4; i++) {
        self->Plant.DMLam4[i] = IXlam4[i]-OXlam4[i];
    }


    //Massen des Blattes für den Zeitschritt ausrechnen und Strukturelle Gesamtmasse:



    //TODO: to litter: OXlam_deg+OXlam3_li

    self->Litter.OXlam_deg = OXlam_deg;
    self->Litter.OXlam_li = OXlam3_li;

    OXLam_li = OXlam_deg+OXlam3_li;
    return OXLam_li;
}



double calculateBiomassStructuralSheathAndStem(hpm *self, double Gsh, double kturnss, double kdegsh ,double fTsh, double fWsh, double flam) {
    expertn_modul_base *xpn = &(self->parent);
    double OXss_li; // structural dry mass from ss to litter
    double dt; // Zeit [day-1]
    double OXss_deg; // Degenerierter Teil
    double OXss_deg4[4]; // Degenerierter Teil, pro Schicht --> Krankheit ...
    double OXss0_1,OXss1_2,OXss2_3,OXss3_li; //Fluss jeweils von einer Schicht in die nächste
    double OXss4[4];// geht aus der entsprechenden Schicht raus
    double IXss4[4]; // geht in die entsprechend Schicht rein
    // double DMss4[4]; // GesamtMassenänderungsrate
    int i;




    dt = (double)xpn->pTi->pTimeStep->fAct;

    //Massendegeneration in OXlam_deg4 schreiben (beschreibt im Grunde den Verfall durch Krankheiten, ... Degration, die durch Die Tempereratur beieinflusst wird
    OXss_deg = 0.0; //totale Masse, die abgebaut wird
    for (i=0; i!=4; i++) {
        OXss_deg4[i] = kdegsh * self->Plant.Mss4[i];
        OXss_deg += OXss_deg4[i];
    }


    //Leaf turnover, transfers from age category to age category:
    OXss0_1 =  kturnss * self->Plant.Mss4[0];//  !Transfer, 0 to 1.
    OXss1_2 =  kturnss * self->Plant.Mss4[1];//  !   -      1 to 2.
    OXss2_3 =  kturnss * self->Plant.Mss4[2];//  !   -      2 to 3.
    OXss3_li = kturnss * self->Plant.Mss4[3];//  !   -      3 to litter.


    // Jetzt: aufsummieren was rausgeht und was reingeht, ernte und animal fressen momentan nix
    // Was geht raus:
    OXss4[0] = OXss0_1  + self->Animal.OXss_an4[0] + self->Harvest.OXss_hv4[0] + OXss_deg4[0];  // nach blatt 1, nach animal 0, nach ernte 0, nach krankheit 0
    OXss4[1] = OXss1_2  + self->Animal.OXss_an4[1] + self->Harvest.OXss_hv4[1] + OXss_deg4[1];  // nach blatt 2, nach animal 1, nach ernte 1, nach krankheit 1
    OXss4[2] = OXss2_3  + self->Animal.OXss_an4[2] + self->Harvest.OXss_hv4[2] + OXss_deg4[2];  // nach blatt 3, nach animal 2, nach ernte 2, nach krankheit 2
    OXss4[3] = OXss3_li + self->Animal.OXss_an4[3] + self->Harvest.OXss_hv4[3] + OXss_deg4[3];  // nach litter, nach animal 0, nach ernte 0, nach krankheit 0


    // Was geht rein:
    IXss4[0] = (1.0 - flam) * Gsh;  //kg structural DM m-2 day-1. // Teil der shoot Wachstumsrate, die zum Wachstum beiträgt ( Rest geht in Blätter)
    IXss4[1] = OXss0_1;    // von 0
    IXss4[2] = OXss1_2;    // von 1
    IXss4[3] = OXss2_3;    // von 2


    //Rate bestimmen (day-1):
    for (i=0; i!=4; i++) {
        self->Plant.DMss4[i] = IXss4[i]-OXss4[i];
    }



    self->Litter.OXss_deg = OXss_deg;
    self->Litter.OXss_li = OXss3_li;
    //TODO: to litter: OXss_deg+OXss3_li
    OXss_li = OXss_deg+OXss3_li;
    return OXss_li;
}

// ret: root dry Massrate to litter

double calculateBiomassStructuralRoot(hpm *self,double Grt, double fTrt, double fWrt,double kturnrt, double kdegrt, double qWPl, double mu20) {
    expertn_modul_base *xpn = &(self->parent);
    double OXrt_li; // return
    double dt; // Zeit [day-1]
//
//   double Grt; // Growthrate [kg structural DM shoot m-2 day-1]
//   double Crt; // substrate C konzentration [kg C (kg structural DM)-1]
//   double Nrt; // substrate N konzentration [kg C (kg structural DM)-1]
    double OXrt_deg; // Degenerierter Teil
    double OXrt_deg4[4]; // Degenerierter Teil, pro Schicht --> Krankheit ...
    double OXrt0_1,OXrt1_2,OXrt2_3,OXrt3_li; //Fluss jeweils von einer Schicht in die nächste
    double OXrt4[4];// geht aus der entsprechenden Schicht raus
    double IXrt4[4]; // geht in die entsprechend Schicht rein
//  double DMrt4[4]; // GesamtMassenänderungsrate
    int i;

    dt = (double)xpn->pTi->pTimeStep->fAct;

    //Massendegeneration in OXrt_deg4 schreiben (beschreibt im Grunde den Verfall durch Krankheiten, ... Degration, die durch Die Tempereratur beieinflusst wird
    OXrt_deg = 0.0; //totale Masse, die abgebaut wird
    for (i=0; i!=4; i++) {
        OXrt_deg4[i] = kdegrt * self->Plant.Mrt4[i];
        OXrt_deg += OXrt_deg4[i];
    }


    //Outputs (kg structural DM m-2 day-1):
    // Tissue turnover, transfers from age category to age category:
    OXrt0_1 =  kturnrt * self->Plant.Mrt4[0];  //Transfer, 1 to 2.
    OXrt1_2 =  kturnrt * self->Plant.Mrt4[1];  //   -      2 to 3.
    OXrt2_3 =  kturnrt * self->Plant.Mrt4[2];  //   -      3 to 4.
    OXrt3_li = kturnrt * self->Plant.Mrt4[3];  //   -      4 to litter.
    //kg structural DM m-2 day-1.




    // Jetzt: aufsummieren was rausgeht und was reingeht
    // Was geht raus:
    OXrt4[0] = OXrt0_1  + OXrt_deg4[0];  // nach  1
    OXrt4[1] = OXrt1_2  + OXrt_deg4[1];  // nach  2
    OXrt4[2] = OXrt2_3  + OXrt_deg4[2];  // nach  3
    OXrt4[3] = OXrt3_li + OXrt_deg4[3];  // nach litter


    // Was geht rein:
    IXrt4[0] = Grt;  //kg structural DM m-2 day-1. // Wachstumsrate
    IXrt4[1] = OXrt0_1;    // von 0
    IXrt4[2] = OXrt1_2;    // von 1
    IXrt4[3] = OXrt2_3;    // von 2


    //Rate bestimmen (day-1):
    for (i=0; i!=4; i++) {
        self->Plant.DMrt4[i] = IXrt4[i]-OXrt4[i];
    }



    self->Litter.OXrt_li = OXrt3_li+OXrt_deg;
    //Litter.OXrt_deg = OXrt_deg;

    //TODO: to litter: OXrt_deg+OXrt3_li
    OXrt_li = OXrt_deg+OXrt3_li;


    return OXrt_li;
}

/* Transport von Substrat C von Shoot nach root (Gl. 3.3b)

ret: OCSsh_rt bzw. ICSsh_rt [kg substrate C m^2 day-1]
*/

double calculateBiomassSubstrateCShootFromShootToRoot(hpm *self,double MCSsh, double MCSrt,double MXsh, double MXrt, double fTsh, double fTrt, double fWsh, double fWrt) {

    double OCSsh_rt;
    double rCsh20;
    double rCrt20;
    double rCsh;
    double rCrt;
    double rCshrt;
    double Crt,Csh;
    // const:
    //double roC20 = 0.2;   //m2 day (kg XDM)**(qroC - 1). C resistivity constant, 20 oC value.
    //double roN20 = 2.0;   //m2 day (kg XDM)**(qroN - 1). N resistivity constant, 20 oC value.

    rCsh20 = self->parameter.plant.roC20/ MXsh;
    rCrt20 = self->parameter.plant.roC20/ MXrt;

    rCsh = rCsh20 / ( fTsh * fWsh );
    rCrt = rCrt20 / ( fTrt * fWrt );

    rCshrt = rCsh + rCrt;
    Csh = MCSsh/ MXsh; // 3.1h
    Crt = MCSrt/ MXrt;

    

    OCSsh_rt = (Csh - Crt)/ rCshrt;
    return OCSsh_rt;
}

// Substrate C shoot, das an Struktur fließt
double calculateBiomassSubstrateCShootOuput_for_sh_maintenace(hpm *self,double *Msh4, double MCSsh,double MXsh,double fTsh, double fWsh, double fCplX) { // --> 3.3g
    double OCSsh_mai; //ret
    double fTWsh;
    double kmai_sh4[4];
    double Csh;

    // const:
//  double kmai_sh120 = 0.02;   //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double kmai_sh220 = 0.02;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double kmai_sh320 = 0.015;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double kmai_sh420 = 0.01;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double KCmai = 0.03;  //[C]. Michaelis-Menten constant switching off shoot and root maintenance at low C substrate levels.

    fTWsh = fTsh * fWsh;

    Csh = MCSsh/ MXsh; // 3.1h


    kmai_sh4[0] = self->parameter.plant.kmai_sh120 * fTWsh;
    kmai_sh4[1] = self->parameter.plant.kmai_sh220 * fTWsh;
    kmai_sh4[2] = self->parameter.plant.kmai_sh320 * fTWsh;
    kmai_sh4[3] = self->parameter.plant.kmai_sh420 * fTWsh;

    OCSsh_mai = fCplX * ( Csh / ( Csh + self->parameter.plant.KCmai ) ) *
                ( kmai_sh4[0] * Msh4[0] + kmai_sh4[1] * Msh4[1]
                  + kmai_sh4[2] * Msh4[2] + kmai_sh4[3] * Msh4[3] ); // kg C m-2 day-1.

    // TODO: Rmrt/OCSrt_mai muss noch irgendwo reinfließen
    return OCSsh_mai;
}


// Substrate C shoot, das an Struktur fließt
double calculateBiomassSubstrateCRootOuput_for_rt_maintenace(hpm *self,double *Mrt4, double MCSrt,double MXrt,double fTrt, double fWrt, double fCplX) { // --> 3.3g
    double OCSrt_mai; //ret
    double fTWrt;
    //double kmai_rt4[4];
    double Crt;
    double MNXrt;
    double kmai20 = 0.3;

    // const:
//  double kmai_rt120 = 0.02;   //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double kmai_rt220 = 0.02;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double kmai_rt320 = 0.015;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double kmai_rt420 = 0.01;  //day-1. Maintenance respiration coefficients of root and shoot structural components at 20 °C.
//  double KCmai = 0.03;  //[C]. Michaelis-Menten constant switching off shoot and root maintenance at low C substrate levels.
    double fNplX = self->parameter.plant.fNplX;

    fTWrt= fTrt * fWrt;

    Crt= MCSrt/ MXrt; // 3.1h


    /*kmai_rt4[0] = parameter.plant.kmai_rt120 * fTWrt;
    kmai_rt4[1] = parameter.plant.kmai_rt220 * fTWrt;
    kmai_rt4[2] = parameter.plant.kmai_rt320 * fTWrt;
    kmai_rt4[3] = parameter.plant.kmai_rt420 * fTWrt;

    OCSrt_mai = fCplX * ( Crt / ( Crt + parameter.plant.KCmai ) ) *
                ( kmai_rt4[0] * Mrt4[0] + kmai_rt4[1] * Mrt4[1]
                  + kmai_rt4[2] * Mrt4[2] + kmai_rt4[3] * Mrt4[3] ); // kg C m-2 day-1.*/

    // Machen wir das mal so, wie es im orig. Source Code von Thornley gemacht wurde:

    MNXrt = fNplX * MXrt;

    

    OCSrt_mai = Crt / (Crt + self->parameter.plant.KCmai) * kmai20 * fTWrt * (self->Plant.MNSrt + MNXrt);

    // TODO: Rmrt/OCSrt_mai muss noch irgendwo reinfließen
    return OCSrt_mai;
}




/*
input:
 * ICSsh_re : recycled litter flux from recycled
 *            Ein Teil davon wird vom 4. Alter dessen was in litter von der strukturellen Masse geht hier wieder verwertet
 *            siehe auch 3.3a
 * OCSsh_rt : Fluss von Shoot to root (Gl. 3.3b) und Function:  double calculateBiomassSubstrateCShootFromShootToRoot(...)
 * Gsh:     : Shoot Growth
*/


void calculateBiomassSubstrateCShoot(hpm *self,double *OCSsh_rt, double ICSsh_re ,double *Msh4, double Gsh, double fTsh, double fTrt, double fWsh, double fWrt ,double kturnlam, double kturnss, double kdegsh) {
    expertn_modul_base *xpn = &(self->parent);
    double fCSshli; // Fraktion vom Substrate, die durch alter verloren gehen
    double Csh;
    double ICSsh, OCSsh; // Input und output flux of C substrate shoot
    double ICSsh_ph; // C Input Rate der Photosynthese
    // double DMCSsh; //
    double OCSshG; // Outputfluxes of substrate C from Shoot to suppurt Growthrate  Gsh 3.3e
    double OCSsh_mai;
    double OCSsh_so; //  flux to senescence (dh. alterungsprozesse)

    double OCSsh_Nnit_red;

    double OCSsh_loading;
    double OXsh4_li;
    double OXsh_deg;
    double OXsh_li;
    double dt; // Zeit [day-1]
    double rCrt20,rCsh20,rCsh,rCrt,OCSshrt,Crt;
    double RNnit_red_rt,fuNnit_red_sh;
    double cphloem_loading = 0.06;
    double roC20=0.2;
    double fNnit_red_sh_CS=0.5;
    double cNnit_red = 1.7;

    double fuNnit_red_rt = 0.5; // !Fraction of nitrate taken up by root which
    // is reduced in the root. Rest is reduced in shoot:
    // fuNnit_red_sh = 1 - fuNnit_red_rt is calculated in
    // Initial section.

    //const:
//  double fCplX = 0.45;  //kg C (kg structure)-1.  Fractional carbon content of plant structural dry matter.
//  double YGpl = 0.75;  //Dimensionless. C yield factor for structural plant growth.
//  double fCSshli_min = 0.3;  // Dimensionless.  The minimum fraction of   C substrate lost in the senescing litter fluxes.
//  double KCSshli = 0.05; //[C]. Michaelis-Menten constant for C substrate loss in senescing litter fluxes
//  double rCNrec_li = 2.7;  //kg C (kg N)-1. Ratio of C:N in recyclable structural dry matter.

    Csh = self->Plant.MCSsh/ (self->Plant.MXLam+self->Plant.MXss); // 3.1h
    Crt = self->Plant.MCSrt / self->Plant.MXrt; // 3.1h
    dt = (double)xpn->pTi->pTimeStep->fAct;

    // InputRate:

    // Input der Photosyntheserate
    ICSsh_ph =  ((double)xpn->pPl->pPltCarbon->fGrossPhotosynR * ha_TO_mq_1) * (12.0/44.0); // relative Moleculemass == ((rmm --> rmm of C) / (rmm of CO2))
	
	C_DEBUG(ICSsh_ph);

    
    //ICSsh_ph = Water.ICSsh_ph;

    self->Respiration.R_Ph=ICSsh_ph; // das was veratmet wird --> in die Atmosphäre
		

    // ICSsh_re: recycled litter flux

//   ICSresh = rec
//   ICSsh_re =
    // ICSsh_re = *p->INSsh_re * rCNrec_li; // 3.3a

    // Gesamt Input Rate:
    ICSsh = ICSsh_ph + ICSsh_re ; // = Photosythese(0) + shoot litter fluxes input

	 C_DEBUG(ICSsh_re);
	 C_DEBUG(ICSsh);
	 
    // Output Rate:

    OCSshG = self->parameter.plant.fCplX/self->parameter.plant.YGpl * Gsh; // Outputfluxes of substrate C from Shoot to suppurt Growthrate  Gsh 3.3e
    self->Respiration.RGsh = OCSshG * (1.0 - self->parameter.plant.YGpl ) ; // der Rest wird veratmet

    // OCSsh_rt: Fluss von Shoot to root (Gl. 3.3b)
    *OCSsh_rt = calculateBiomassSubstrateCShootFromShootToRoot(self,self->Plant.MCSsh, self->Plant.MCSrt, (self->Plant.MXLam+self->Plant.MXss), self->Plant.MXrt, fTsh, fTrt, fWsh, fWrt);

    // Ausgleich mit Structureller Masse
    OCSsh_mai = calculateBiomassSubstrateCShootOuput_for_sh_maintenace(self,Msh4, self->Plant.MCSsh,(self->Plant.MXLam+self->Plant.MXss),fTsh,fWsh,self->parameter.plant.fCplX); // --> 3.3g


    //  OCSsh_so (3.3j): flux to senescence (dh. alterungsprozesse)
    // Faktoren (für C) bestimmen:
    fCSshli = ( self->parameter.plant.fCSshli_min * self->parameter.plant.KCSshli  + Csh ) / ( self->parameter.plant.KCSshli + Csh );// This is the fraction of C substrate in the structural litter flux which is lost to litter.
    // 3.5d .ouput fluxes for shoot, Außerdem muss noch die genaue Masse aus dem Strukturellen Massen bestimmt werden (siehe auch Massendegenerierung von strukturellen Massen)
    OXsh4_li = kturnlam* self->Plant.MLam4[3] + kturnss * self->Plant.Mss4[3];
    OXsh_deg = kdegsh * (self->Plant.MXLam + self->Plant.MXss);
    OXsh_li = OXsh4_li + OXsh_deg;
    OCSsh_so = fCSshli * Csh * OXsh_li;

    self->Litter.OCSsh_so = OCSsh_so-ICSsh_re;// + OCSsh_mai;// ICSsh_re;
    self->Respiration.Rmsh = OCSsh_mai;

    // OCSsh_loading, phloem loading respiration.
    // There is a respiratory cost associated with loading carbohydrate
    // into the phloem.  We use Bouma's number of 0.7 mol CO2 per mol
    // sucrose, or 0.06 C per C exported.  This is taken from the C
    // substrate pool:
    rCsh20 = roC20 / (self->Plant.MXLam+self->Plant.MXss);
    rCsh = rCsh20 / (fTsh*fWsh);


    rCrt20 = roC20 / self->Plant.MXrt;
    rCrt = rCrt20 / (fTrt*fWrt);


    OCSshrt = ( Csh - Crt ) / (rCsh + rCrt); // kg substrate C m-2 day-1.
    OCSsh_loading = cphloem_loading * OCSshrt;   //kg C m-2 day-1.

    //RNnit_red_rt (OCSrt_Nnit_red), respiration of root C substrate
    //associated with nitrate reduction in the root
    //(kg C respired m-2 day-1).

// Litter.INitrt_so = uNnit
    RNnit_red_rt = cNnit_red * fuNnit_red_rt * self->Litter.INitrt_so;

    fuNnit_red_sh = 1.0 - fuNnit_red_rt;
    OCSsh_Nnit_red = fNnit_red_sh_CS * cNnit_red * fuNnit_red_sh * self->Litter.INitrt_so;             //kg C substrate m-2 day-1.

    

    OCSsh = OCSshG + (*OCSsh_rt) + OCSsh_mai + OCSsh_so + self->Animal.OCSsh_an_gr + self->Harvest.OCSsh_hv + OCSsh_loading + OCSsh_Nnit_red;

    #ifdef TEST_HPM		
		//ICSsh = get_var(self,"ICSSH");
		//OCSsh = get_var(self,"OCSSH");
	#endif

    self->Plant.DMCSsh = ( ICSsh - OCSsh );

}

// ONSsh_Nph: kg photosynthetic N m-2 day-1 output to substrate
void calculatePhotoynthetic_N(hpm *self,double *ONSsh_Nph,double *ONph_NSsh,double *INph_li_NSsh,double OXsh_li,double fTsh,double fWsh,double MXsh) {
    expertn_modul_base *xpn = &(self->parent);
    double INph,ONph;
    double kG_Nph,LAI;
    double fTWsh,fNSsh_Nph,fCSsh_Nph,Nsh,Csh,fjPAR_Nph;
    double jparsc,jPARsc_ul;
    double CSTsh,CTsh,Nph,MNph;
    //double Time;
    double kD_Nph,ONph_li0;
    double fNphli,ONph_li;
    double kG_Nph20 = 0.04; //kg photosynthetic N (kg XDM)-1 day-1.
    // Rate constant for synthesis (growth) of
    // photosynthetic N from shoot N substrate.
    double KNSsh_Nph = 0.01; // [N]
    double JCSTsh_Nph = 0.03; // [CS + CT]
    double KjPAR_Nph = 100.0;// J PAR m-2 s-1.
    double kD_Nph20 = 0.15;// day-1.  Half-life is 0.69 / 0.15 = 5 days.
    // Rate constant for degradation of photosynthetic N.
    double fNphli_min = 0.3;//   !Dimensionless.
    //The minimum fraction of photosynthetic
    //N leaving the plant with the senescing litter fluxes.
    double KNphli = 0.01;//  ![N].
    //Michaelis-Menten constant for photosynthetic N
    //leaving the plant in senescing litter fluxes.
    double kcan = self->parameter.photosynthesis.kcan;
    

//  Time = xpn->pTi->pSimTime->iJulianDay+xpn->pTi->pSimTime->fTimeDay;
    

    // Modify synthetic (growth) rate constant for temperature, water

    LAI = hpm_math_sum4(self->Plant.LAI4);
    fTWsh = fTsh*fWsh;
    Nsh = self->Plant.MNSsh / MXsh;
    Csh = self->Plant.MCSsh / MXsh;
    CTsh =  self->Plant.MCTsh / self->Plant.MXLam;

    kG_Nph = kG_Nph20 * fTWsh;//kg phN (kg XDM)-1 day-1.

    //Michalis_Menten modifiers for N substrate, C substrate and light:
    fNSsh_Nph = Nsh / ( Nsh + KNSsh_Nph );
    CSTsh = Csh + CTsh;
    fCSsh_Nph = JCSTsh_Nph / ( CSTsh + JCSTsh_Nph );

    jparsc = xpn->pCl->pWeather->fPAR*1.0e6/86400.0; //CalculateActiveRadiation(self,(double)Time,self->Environment.DayLength, (double)xpn->pCl->pWeather->fSolRad * 1.0e6);

    C_DEBUG(jparsc);

    jPARsc_ul = jparsc * kcan; //Light incident on upper leaves.
    fjPAR_Nph = jPARsc_ul / ( jPARsc_ul + KjPAR_Nph );

    *ONSsh_Nph = kG_Nph * MXsh * fNSsh_Nph * fCSsh_Nph * fjPAR_Nph; // kg photosynthetic N m-2 day-1.



    // Outputs (kg photosynthetic N m-2 day-1):
    // ONph_NSsh.  Degradation of photosynthetic N to N substrate.
    //Modify degradation rate constant for temperature, water:
    kD_Nph = kD_Nph20 * fTWsh;
    *ONph_NSsh = kD_Nph * self->Plant.MNph_ul;

    MNph = self->Plant.MNph_ul * (1.0 - exp( - kcan * LAI))  / (kcan * LAI);

    Nph = MNph / MXsh;        //kg photosynthetic N (kg structural DM)-1.
    // ONph_li0, output towards surface soil metabolic pool with litter
    // turnover (senescence) flux (kg substrate N m-2 day-1).
    ONph_li0 = Nph * OXsh_li;

    //A fraction of this actually accompanies the litter to the
    //surface litter pools:
    fNphli = ( fNphli_min * KNphli + Nsh ) / ( KNphli + Nsh );
    //If NSsh = 0, then fNphli = fNphli_min.
    //If NSsh is high, then fNphli = 1.

    //The flux of N to the surface litter is:
    ONph_li = fNphli * Nph * OXsh_li;
    //The remainder enters the shoot N substrate pool:
    *INph_li_NSsh = ONph_li0 - ONph_li;


    INph = *ONSsh_Nph; //  Subsrate

    // Substrate, Litter, Harvest, Animal
    ONph = *ONph_NSsh + ONph_li0 + self->Harvest.ONph_hv + self->Animal.ONph_an;

    self->Plant.DMNph_ul = INph - ONph;
}

// ret: ICSsh_re
double calculateBiomassSubstrateNShoot(hpm *self,double *INSsh_rt,double ONSsh_Nph,double INSsh_Nph,double INph_li_NSsh,double Gsh, double fTsh,double fTrt,double fWsh, double fWrt,double kturnlam, double kturnss, double kdegsh) { // 3.4a
    //double DMNSsh;
    expertn_modul_base *xpn = &(self->parent);
    double INSsh,ONSsh,INSsh_re,ONS_Gsh,ONSsh_so,fNSshli,ONSsh_atm;
    double Nrt,Nsh;
    double OXsh4_li;
    double fNshli_re;
    double OXsh_deg;
    double OXsh_li;
    double dt; // Zeit [day-1]
    double ICSsh_re;
    double gstom;// stromatal conductance [m s-1 (m2 leaf)-1)]
    double LAI,gcan,gblcon;
    double Namm_sh;
    double fT_Namm_sh;
    double Namm_atm;

    double Namm_atm_STP = 0.0;
    double cNamm_sh_NSsh20 = 1.0E-6;
    //kg ammonia N m-3 / [kg N substrate (kg XDM)-1].
    //This converts NSsh to Namm_sh.  A 20 deg C value;
    //Temperature function fT_Namm_sh = 1 at 20 C.
    //For NSsh = 0.01, Namm_sh = 1E-9 kg amm N m-3
    //= 1/14 x E-6 moles amm m-3. 1 m3 of air contains about
    //44.62 moles at STP. The mole fraction is 1/14 x E-6
    //  /44.62 = 1.6 nanomol per mol ammonia.
    double c1fT_Namm_sh = 86648.0;//Dimensionless.  Value is adjusted
    //      !so that fT_Nammsh = 1 at 20 C.
    double c2fT_Namm_sh = 3333.0; // !K. This is an "Arrhenius" constant
    //       !for NH3 volatilization into the
    //       !sub-stomatal cavities in foliage.
    //  Reference: Sutton MA, Pitcairn CER, Fowler D. 1993. The exchange
    // of ammonia between the atmosphere and plant communities.
    // Advances in Ecological Research 24: 301-393 (eds Begon M,
    // Fitter AH) Academic Press, London.
    // See p. 339 and the Appendix.

    //const:
    //double fNplX = 0.02; // !kg N (kg structure)-1.  Fractional nitrogen content of plant structural dry matter.
    //double fNshli_re_max = 0.4; //Dimensionless.  Maximum fractions of root, shoot litter N structure which can be recycled.
    //double KNrec = 0.02;  // [N].  Michaelis-Menten constant, determines how much of the structural N is recycled.
    //double fNSshli_min = 0.3;  // Dimensionless. The minimum fraction of N substrate lost in the senescing litter fluxes.
    //double KNSshli = 0.01; // [N]. Michaelis-Menten constant for N substrate  loss in senescing litter fluxes.
    //double rCNrec_li = 2.7;  // kg C (kg N)-1. Ratio of C:N in recyclable structural dry matter.

    dt = (double)xpn->pTi->pTimeStep->fAct;

    LAI = hpm_math_sum4(self->Plant.LAI4);

    // Substrate N konz.: 3.3b
    CHECK_0(self->Plant.MXrt)
    Nrt = self->Plant.MNSrt / self->Plant.MXrt;

    CHECK_0((self->Plant.MXLam+self->Plant.MXss)) // 3.1h Substratgehalt (dh. Verhältnis subtrat zur Gesamtstructurmasse)
    Nsh = self->Plant.MNSsh / (self->Plant.MXLam+self->Plant.MXss);


    // Input:

    // Substrate, das von der Wurzel zum Shoot transportiert wird
    //*INSsh_rt

    *INSsh_rt = calculateBiomassSubstrateNShoot_TransportSubstateNfromRootToShoot(self,self->Plant.MXLam+self->Plant.MXss,self->Plant.MXrt,fTsh, fTrt,fWsh,fWrt, Nsh, Nrt); // --> *p->ONSrt_sh --> 3.4b
    


    // Recycled N from litter fluxes 3.4.2
    // 3.5d .ouput fluxes for shoot, Außerdem muss noch die genaue Masse aus dem Strukturellen Massen bestimmt werden (siehe auch Massendegenerierung von strukturellen Massen)
    OXsh4_li = kturnlam* self->Plant.MLam4[3] + kturnss * self->Plant.Mss4[3];
    OXsh_deg = kdegsh * (self->Plant.MXLam+ self->Plant.MXss);
    OXsh_li = OXsh4_li + OXsh_deg;
    self->Litter.OXsh_li = OXsh_li; //Gesamtfluss, der nach litter fließt
    // 3.4c fractional of N in these Litter Fluxes can be recycled in to plant substrate N pools
    fNshli_re = self->parameter.nitrogen.fNshli_re_max / ( 1.0 + Nsh / self->parameter.nitrogen.KNrec );
    // TODO: Alles was hier reingeht muss natürlich wieder vom Litter entfernt werden.
    INSsh_re = self->parameter.plant.fNplX * fNshli_re * OXsh4_li;

    ICSsh_re = INSsh_re * self->parameter.plant.rCNrec_li; // 3.3a der Teil geht wird in shoot C wieder recycled

    // Gesamt Input:
    INSsh = *INSsh_rt + INSsh_re + INSsh_Nph + INph_li_NSsh;

    // Jetzt der Output:
    // Wachstum, bewertet mit dem Umrechnungsfaktor für Substrate
    ONS_Gsh = Gsh * self->parameter.plant.fNplX;


    // Anteil, der an litter abgegeben wird
    fNSshli = (self->parameter.nitrogen.fNSshli_min * self->parameter.nitrogen.KNSshli + Nsh ) / ( self->parameter.nitrogen.KNSshli + Nsh );
    // degeneration durch Krankheit:
    OXsh_deg = kdegsh * (self->Plant.MXLam + self->Plant.MXss);
    OXsh_li = OXsh4_li + OXsh_deg;
    ONSsh_so = fNSshli * Nsh * OXsh_li;

    // INSsh_re aufpassen auf recyclten teil
    self->Litter.ONSsh_so = ONSsh_so-INSsh_re;//+ONS_Gsh;//+ONS_Gsh-INSsh_re ;

    // 6.6e: Conopy conductance
    gstom = calculateLeafStomatalConductance(self,self->Water.theta_sh); // 3.2u

    gcan = LAI * gstom;

    gblcon = get_canopy_conductance(self,LAI);

    fT_Namm_sh = c1fT_Namm_sh * exp ( - c2fT_Namm_sh / (self->Environment.Tair+273.15) );


    //This flux can be positive or negative.
    //Convert NSsh [shoot substrate N concentration, kg N substrate
    //(kg XDM)-1] into a shoot ammonia concentration in the sub-
    //stomatal cavities, Namm_sh (kg amm N m-3):
    Namm_sh = fT_Namm_sh * cNamm_sh_NSsh20 * Nsh;// !kg ammonia N m-3.

    Namm_atm = Namm_atm_STP * 2.6958e-3 * self->Environment.Pair / (self->Environment.Tair+273.15);

    //Flux of NH3 to atmosphere (can be + or -)
    //(kg ammonia N m-2 day-1):
    ONSsh_atm = 86400.0 * 1.0/(1.0/gcan + 1.0/gblcon) * ( Namm_sh - Namm_atm );

    // Gesamtfluss nach shoot:
    ONSsh = ONS_Gsh + ONSsh_so + self->Animal.ONSsh_an_gr + self->Harvest.ONSsh_hv + ONSsh_Nph + ONSsh_atm;


    self->Plant.DMNSsh = INSsh - ONSsh ;
    
    return ICSsh_re;

}

// 3.4a
// return ICSrt_re

double calculateBiomassSubstrateNRoot(hpm *self,double ONSsh_rt,double Namm, double Nnit,double Grt,double  Tsoil, double fTrt, double fWrt,double thetasoil, double kturnrt, double kdegrt) { // 3.4a
    expertn_modul_base *xpn = &(self->parent);
    //double DMNSrt;
    double ICSrt_re;
    double INSrt,ONSrt,INSrt_re,fNSrtli;
    double INSrt_uN,OXrt4_li, OXrt_li,OXrt_deg, fNrtli_re,ONS_Grt,ONSrt_ex,ONSrt_so ;
    double Nrt;
    double dt; // Zeit [day-1]

    // const:
    //double fNplX = 0.02; // !kg N (kg structure)-1.  Fractional nitrogen content of plant structural dry matter.
    //double KNrec = 0.02;  // [N].  Michaelis-Menten constant, determines how much of the structural N is recycled.
    // double fNrtli_re_max = 0.4; //Dimensionless.  Maximum fractions of root, shoot litter N structure which can be recycled.
    // double kNSrt_ex20 = 0.005; //day-1. Root exudation of root N substrate.
    // double fNSrtli_min = 0.3; // Dimensionless.  The minimum fraction of N substrate lost in the senescing litter fluxes.
    // double KNSrtli = 0.01; // [N]. Michaelis-Menten constant for N substrate loss in senescing litter fluxes.
    //double rCNrec_li = 2.7;  // kg C (kg N)-1. Ratio of C:N in recyclable structural dry matter.

    dt = (double)xpn->pTi->pTimeStep->fAct;
    // Substrate N konz.: 3.3b
    CHECK_0(self->Plant.MXrt)
    Nrt = self->Plant.MNSrt / self->Plant.MXrt;


    // Input:

    // Nitritteil aus Soil wird berechnet
    INSrt_uN = calculateNitrogenUptakeSoil(self,Namm,Nnit,Tsoil,thetasoil,self->Plant.MCSrt, self->Plant.MNSrt, self->Plant.MXrt,self->Plant.Mrt4, fTrt,fWrt);

    // Recycled N from litter fluxes 3.4.2
    // 3.5d .ouput fluxes for shoot, Außerdem muss noch die genaue Masse aus dem Strukturellen Massen bestimmt werden (siehe auch Massendegenerierung von strukturellen Massen)
    OXrt4_li = kturnrt* self->Plant.Mrt4[3];
    OXrt_deg = kdegrt * self->Plant.MXrt;
    OXrt_li = OXrt4_li + OXrt_deg;
    // 3.4c fractional of N in these Litter Fluxes can be recycled in to plant substrate N pools
    fNrtli_re = self->parameter.nitrogen.fNrtli_re_max / ( 1.0 + Nrt / self->parameter.nitrogen.KNrec );
    // TODO: Alles was hier reingeht muss natürlich wieder vom Litter entfernt werden.
    INSrt_re = self->parameter.plant.fNplX * fNrtli_re * OXrt_li; ;

    ICSrt_re = INSrt_re * self->parameter.plant.rCNrec_li; // 3.3a der Teil geht wird in shoot C wieder recycled

    // Gesamt Input:
    INSrt = INSrt_uN + INSrt_re;

    // Output:

    //ONSsh_rt: kommt von Shoot

    // Wachstum, bewertet mit dem Umrechnungsfaktor für Substrate
    ONS_Grt = Grt * self->parameter.plant.fNplX;
    // exudation of root N substrat
    ONSrt_ex = self->parameter.nitrogen.kNSrt_ex20* fTrt * fWrt * self->Plant.MXrt * Nrt;

    fNSrtli = ( self->parameter.nitrogen.fNSrtli_min * self->parameter.nitrogen.KNSrtli + Nrt ) / ( self->parameter.nitrogen.KNSrtli + Nrt ); // This is the fraction of N substrate in the structural litter flux which is lost to litter
    // An soil:
    ONSrt_so = fNSrtli * Nrt *OXrt_li; //output to soil metabolic pool with litter
    //Litter.ONSrt_so = ONSrt_so + ONS_Grt + ONSrt_ex;
    ONSrt = ONSsh_rt + ONS_Grt + ONSrt_ex + ONSrt_so;

    self->Litter.ONSrt_so = ONSrt_so + ONSrt_ex - INSrt_re;



    self->Litter.INSrt_so = INSrt_uN;


    // Gesamt:
	
	#ifdef TEST_HPM		
		//INSrt = get_var(self,"INSRT");
		//ONSrt = get_var(self,"ONSRT");
	#endif


    self->Plant.DMNSrt =  INSrt - ONSrt ;

    return ICSrt_re;
}

void calculateBiomassSubstrateCRoot(hpm *self, double Namm, double Nnit, double Tsoil, double thetasoil, double ICSsh_rt, double ICSrt_re,double Gsh,  double Grt, double fTrt, double fWrt ,double kturnrt , double kdegrt) {
    //double DMCSrt,
    expertn_modul_base *xpn = &(self->parent);
    double ICSrt,OCSrt;
    double OCSrtG; //Outputfluxes of substrate C from Shoot to suppurt Growthrate  Grt 3.3e
    double OCsrt_uN;  //OCSrt_uN: Der C substrat Teil, der für den N uptake verbraucht wird
    double OCSrt_ex; // C flux to exudation 3.3i
    double OCSrt_so;// (3.3j): flux to senescence (dh. alterungsprozesse)
    double OCSrt_mai;
    double fTWrt,Crt;
    double fCSrtli,OXrt4_li,OXrt_deg,OXrt_li,OCSrt_Nnit_red,OCSrt_min_upt;
    double dt,umin_pl; // Zeit [day-1]

    double cNnit_red = 1.7;// !kg substrate C (kg nitrate N)-1.
    // Respiratory cost of nitrate reduction.
    // Based on: HNO3 + 8H = NH3 + 3H2O.
    // C6H12O6 + 6H2O = 6CO2 + 24H.
    double fuNnit_red_rt = 0.5; // !Fraction of nitrate taken up by root which
    // is reduced in the root. Rest is reduced in shoot:
    // fuNnit_red_sh = 1 - fuNnit_red_rt is calculated in
    // Initial section.
    double cmin_upt = 0.06;  //kg C substrate (kg ash). Respiratory cost of
    // minerals (excluding N) uptake from the soil. Number is
    // based on a modified eqn (12.21g, page 349,
    // Thornley & Johnson, 1990.  1 glucose gives 30 ATP.
    // Therefore 1 kg minerals requires 1/40 x 6/30 x 12 =
    double fminli_rec = 0.5;// Dimensionless.
    // Fraction of minerals (exc. N) in
    // litter which is recycled (therefore does not have
    // to be taken for growth.
    double fmin_plX = 0.05; //  !kg minerals (exc. N) (kg structure)-1.
    // Fractional mineral (exc. N) content of plant XDM.
    //const:
//  double fCplX = 0.45;  //kg C (kg structure)-1.  Fractional carbon content of plant structural dry matter.
//  double YGpl = 0.75;  //Dimensionless. C yield factor for structural plant growth.
//  double kCSrt_ex20 = 0.02; // day-1.  Root exudation of root C substrate.
//  double fCSrtli_min = 0.3;  // Dimensionless. The minimum fraction of C substrate lost in the senescing litter fluxes.
//  double KCSrtli = 0.05; //[C].   Michaelis-Menten constant for C substrate loss in senescing litter fluxes.


    dt = (double)xpn->pTi->pTimeStep->fAct;
    fTWrt = fTrt * fWrt;
    Crt = self->Plant.MCSrt / self->Plant.MXrt; // 3.1h


    // InputRate:

    // ICSsh_rt (Fluss von Shoot)
    // ICSrt_re recycled litter flux from recycled Ein Teil davon wird vom 4. Alter dessen was in litter von der strukturellen Masse geht hier wieder verwertet siehe auch 3.3a

    ICSrt = ICSsh_rt + ICSrt_re;


    // Output Rate:

    OCSrtG = self->parameter.plant.fCplX/self->parameter.plant.YGpl * Grt;  // Outputfluxes of substrate C from Shoot to suppurt Growthrate  Grt 3.3e
    // Der Teil der Masse, der in in Struktur übergeht
    self->Respiration.RGrt = OCSrtG * (1.0 - self->parameter.plant.YGpl ) ; // der Rest wird veratmet

    // Ausgleich mit Structureller Masse
    OCSrt_mai = calculateBiomassSubstrateCRootOuput_for_rt_maintenace(self,self->Plant.Mrt4, self->Plant.MCSrt,self->Plant.MXrt,fTrt,fWrt,self->parameter.plant.fCplX); // --> 3.3g

    // Der C substrat Teil, der für den N uptake verbraucht wird 3.3h
    OCsrt_uN = calculateBiomassSubstrateCRootNUptake(self,Namm, Nnit, Tsoil, thetasoil, self->Plant.MCSrt, self->Plant.MNSrt, self->Plant.MXrt, self->Plant.Mrt4, fTrt,fWrt);

    // OCSrt_ex, loss of C substrate to soil Csol pool by root exudation (kg substrate C m-2 day-1).
    OCSrt_ex = self->parameter.plant.kCSrt_ex20 * fTWrt * self->Plant.MXrt * Crt;

    //  OCSrt_so (3.3j): flux to senescence (dh. alterungsprozesse)
    // Faktoren (für C) bestimmen:
    fCSrtli = ( self->parameter.plant.fCSrtli_min * self->parameter.plant.KCSrtli  + Crt ) / ( self->parameter.plant.KCSrtli + Crt );// This is the fraction of C substrate in the structural litter flux which is lost to litter.
    // 3.5d .ouput fluxes for shoot, Außerdem muss noch die genaue Masse aus dem Strukturellen Massen bestimmt werden (siehe auch Massendegenerierung von strukturellen Massen)
    OXrt4_li = kturnrt * self->Plant.Mrt4[3];
    OXrt_deg = kdegrt * self->Plant.MXrt;
    OXrt_li = OXrt4_li + OXrt_deg;
    OCSrt_so = fCSrtli * Crt * OXrt_li;
    self->Litter.OCSrt_so = OCSrt_so + OCSrt_ex - ICSrt_re;//  + OCSrt_mai;
    self->Respiration.Rmrt = OCSrt_mai;
    self->Respiration.OCsrt_uN = OCsrt_uN;

    //RNnit_red_rt (OCSrt_Nnit_red), respiration of root C substrate
    //  !associated with nitrate reduction in the root
    //  !(kg C respired m-2 day-1).
    OCSrt_Nnit_red = cNnit_red * fuNnit_red_rt * self->Litter.INitrt_so;


    //Mineral flux into the plant is:
    umin_pl = fmin_plX * ( (Gsh+Grt) - fminli_rec * (self->Litter.OXsh_li+self->Litter.OXrt_li) );
    //kg mineral m-2 day-1.

    // The need of new growth (GXpl) for minerals is decreased because
    // a fraction of the minerals (fminli_rec) in the plant litter
    // flux (OXpl_li, structural DM) is recycled (retained in plant).
    OCSrt_min_upt = MAX(0.0,(cmin_upt * umin_pl));

    OCSrt = OCSrtG + OCSrt_mai + OCsrt_uN + OCSrt_ex + OCSrt_so + OCSrt_Nnit_red + OCSrt_min_upt;
    // TODO: OCsrt_uN  in CO2; OCSrt_ex; OCSrt_so, OCSrt_ex geht in den Boden

    self->Plant.DMCSrt = ICSrt - OCSrt;
    
    
}

// ret: ONSrt_sh
double calculateBiomassSubstrateNShoot_TransportSubstateNfromRootToShoot(hpm *self,double MXsh, double MXrt, double fTsh, double fTrt, double fWsh, double fWrt, double Nsh, double Nrt) { // --> *p->ONSrt_sh --> 3.4b
    expertn_modul_base *xpn = &(self->parent);
    double ONSrt_sh;
    double rNrt_sh,rNrt20,rNsh20;
    double rNrt,rNsh;
    // const:
    //double roN20 = 2.0;   //m2 day (kg XDM)**(qroN - 1). N resistivity constant, 20 oC value.
    CHECK_0(MXrt)
    rNrt20=self->parameter.plant.roN20/ MXrt;
    CHECK_0(MXsh)
    rNsh20=self->parameter.plant.roN20/ MXsh;
    CHECK_0(fTrt * fWrt)
    rNrt = rNrt20 / (fTrt * fWrt);
    CHECK_0(fTsh * fWsh)
    rNsh = rNsh20 / (fTsh * fWsh);
    rNrt_sh = rNrt + rNsh;
    CHECK_0(rNrt_sh)
    ONSrt_sh = (Nrt - Nsh)/ rNrt_sh;
    return ONSrt_sh;
}

//ret INSso_rt;
double calculateNitrogenUptakeSoil(hpm *self,double Namm, double Nnit, double Tsoil, double thetasoil,double  MCSrt, double MNSrt, double MXrt, double *Mrt4, double fTrt, double fWrt) {
    double INSso_rt; // (uN)  Nitrogen uptake rate from soil Namm and Nnit pools. 3.4h
    double fTuNmin;/**< dimensionless.   Temperature-dependent nitrogen availability parameter for N uptake from soil mineral (ammonium, nitrate) pools.*/
    double fTuNmin1,fTuNmin0;
    double NeffuN; // N effektiv uptake: wird as Nitrat und Ammonion mit Temperaturabhängigkeit zusammengestzt
    double zNeffuN;
    double fMrtuN;
    double uNden;
    double Crt,Nrt;
    double fTWrt,fWuN;
    double sig_uN;
    double fMrtuNSum;
    int i;


    fTWrt = fTrt * fWrt;
    Crt = MCSrt / MXrt; // 3.1h
    Nrt = MNSrt / MXrt; // 3.1h

    // Gl. 3.4g fTuNmin: Steigung für Nitritrat uptake abhängig von der Temperatur (siehe Fig 3.5)
    // Die Steigung wird ab Temp=0 C auf 0 gesetzt und auch bei 20 Grad auch 0
    fTuNmin0 = self->parameter.nitrogen.fTuN20 - (self->parameter.nitrogen.fTuN20 - self->parameter.nitrogen.fTuN10) * ( 20.0 - Tsoil ) / ( 20.0 - 10.0 );
    fTuNmin1 = MIN ( 1.0, fTuNmin0);
    fTuNmin =  MAX ( 0.0, fTuNmin1);


    // 3.4g: N effektiv uptake: wird as Nitrat und Ammonion mit Temperaturabhängigkeit zusammengestzt
    NeffuN = Namm + fTuNmin * Nnit;

    //Michaelis-Menten term. 1. Faktor der Gleichung 3.4h
    zNeffuN = NeffuN / ( NeffuN + self->parameter.nitrogen.KNeffuN );

    // fMrtuN: Teil der Strukturellen Masse der Gleichung 3.4h:
    fMrtuNSum=0.0;
    for (i=0; i!=4; i++) {
        fMrtuNSum += Mrt4[i]* self->parameter.nitrogen.c_uN4[i];
    }
    fMrtuN = fMrtuNSum / (1.0 + MXrt / self->parameter.nitrogen.KMXrtuN);


    //Root structural dry mass components are age-weighted for uptake. 3.4h
    uNden =  1.0 + ( self->parameter.nitrogen.KCuN / Crt ) * ( 1.0 + Nrt / self->parameter.nitrogen.JNuN );

    //N uptake by root 3.4h):
    fWuN = pow( thetasoil / self->parameter.water.thetasoil_max, self->parameter.nitrogen.qWuN);
    sig_uN = self->parameter.nitrogen.sig_uN20 *  fTWrt * fWuN;

    // INSrt_uN (uN).  Nitrogen uptake rate from soil Namm and Nnit pools. 3.4h
    INSso_rt = sig_uN * zNeffuN * fMrtuN / uNden;  //(kg N m-2 day-1) where
    
    return INSso_rt;
}


/*
 * Die Aufnahme und uptake von N benötigt Energie (dh. es wird C verbraucht und CO2 ausgeatmet)
 * 3.3h
ret: OCSrt_uN: Der C substrat Teil, der für den N uptake verbraucht wird
*/
double calculateBiomassSubstrateCRootNUptake(hpm *self,double Namm, double Nnit, double Tsoil, double thetasoil, double  MCSrt, double MNSrt, double MXrt, double *Mrt4, double fTrt, double fWrt) {
    double OCsrt_uN; //ret;
    double INSso_rt; // (uN)  Nitrogen uptake rate from soil Namm and Nnit pools. 3.4h
    double fTWrt,fWuN;
    double fTuNmin;/**< dimensionless.   Temperature-dependent nitrogen availability parameter for N uptake from soil mineral (ammonium, nitrate) pools.*/
    double fTuNmin1,fTuNmin0;
    double zNeffuN;
    double fMrtuN;
    double uNden;
    double fuNamm,fuNnit;
    double NeffuN; // N effektiv uptake: wird as Nitrat und Ammonion mit Temperaturabhängigkeit zusammengestzt
    double uNamm; //ONamm->rt (also der Teil, der vom boden in die Wurzel fließt)
    double uNnit; //N uptake from soil nitrate pool, Nnit (kg N m-2 day-1):
    double RuNamm,RuNnit;
    double sig_uN;
    double Crt,Nrt;
    int i;


    fTWrt = fTrt * fWrt;
    Crt = MCSrt / MXrt; // 3.1h
    Nrt = MNSrt / MXrt; // 3.1h




    // Gl. 3.4g fTuNmin: Steigung für Nitritrat uptake abhängig von der Temperatur (siehe Fig 3.5)
    // Die Steigung wird ab Temp=0 C auf 0 gesetzt und auch bei 20 Grad auch 0
    fTuNmin0 = self->parameter.nitrogen.fTuN20 - (self->parameter.nitrogen.fTuN20 - self->parameter.nitrogen.fTuN10) * ( 20.0 - Tsoil ) / ( 20.0 - 10.0 );
    fTuNmin1 = MIN ( 1.0, fTuNmin0);
    fTuNmin =  MAX ( 0.0, fTuNmin1);


    // 3.4g: N effektiv uptake: wird as Nitrat und Ammonion mit Temperaturabhängigkeit zusammengestzt
    NeffuN = Namm + fTuNmin * Nnit;

    //Michaelis-Menten term. 1. Faktor der Gleichung 3.4h
    zNeffuN = NeffuN / ( NeffuN + self->parameter.nitrogen.KNeffuN );

    // fMrtuN: Teil der Strukturellen Masse der Gleichung 3.4h:
    fMrtuN=0.0;
    for (i=0; i!=4; i++) {
        fMrtuN += Mrt4[i]* self->parameter.nitrogen.c_uN4[i];
    }
    fMrtuN /= (1.0 + MXrt / self->parameter.nitrogen.KMXrtuN);


    //Root structural dry mass components are age-weighted for uptake. 3.4h
    uNden =  1.0 + ( self->parameter.nitrogen.KCuN / Crt ) * ( 1.0 + Nrt / self->parameter.nitrogen.JNuN );

    //N uptake by root 3.4h):
    fWuN = pow( thetasoil / self->parameter.water.thetasoil_max, self->parameter.nitrogen.qWuN);
    sig_uN = self->parameter.nitrogen.sig_uN20 *  fTWrt * fWuN;

    // INSrt_uN (uN).  Nitrogen uptake rate from soil Namm and Nnit pools. 3.4h
    INSso_rt = sig_uN * zNeffuN * fMrtuN / uNden;  //(kg N m-2 day-1) where




    //Dependence of uptake on root substrate Crt (required), and inhibited by root N substrate Nrt.  Fractions taken from Namm and Nnit pools:
    fuNamm = Namm / NeffuN;  // 3.4i
    fuNnit = 1.0 - fuNamm;


    // Ratio of canopy gross photosynthesis:N uptake
    // rPcan_uN = Pcan / INSrt_uN  !kg C (kg N)-1.  Test Davidson hypothesis.


    //N uptake from soil ammonia pool, Namm (kg N m-2 day-1):
    uNamm = INSso_rt * fuNamm;  // uNamm = ONamm->rt (also der Teil, der vom boden in die Wurzel fließt)
    //ONamm_rt = uNamm ;

    //N uptake from soil nitrate pool, Nnit (kg N m-2 day-1):
    uNnit = INSso_rt * fuNnit;
    //ONnit_rt = uNnit;

    self->Litter.INammrt_so = uNamm;
    self->Litter.INitrt_so = uNnit;

    self->Litter.INammrt_so = (self->Litter.ONSrt_so+self->Litter.ONSsh_so) * fuNamm;
    self->Litter.INitrt_so = (self->Litter.ONSrt_so+self->Litter.ONSsh_so) * fuNnit;



    // 3.3h:
    RuNamm = self->parameter.nitrogen.cuNamm * uNamm;   //from soil ammonia pool.
    RuNnit = self->parameter.nitrogen.cuNnit * uNnit;   //from soil nitrate pool.
    OCsrt_uN = RuNamm + RuNnit ;    /* == OCS,rt->en,Un Total respiration with N uptake.
                              See plant substrate N pool for
                              uNamm and uNnit.*/    
    
    return OCsrt_uN;
}
