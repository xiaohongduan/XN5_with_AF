#include "libtreemix.h"
#include <math.h>

int libtreemix_CanopyFormation(libtreemix *self)
{
	/* Description */
	/*
	 * Computes the distribution of increment to either height growth, 
	 * or diameter growth, or both, depending on crown competition and current HD ratio.
	 * 
	*/
	
	/* Abbreviations */
	expertn_modul_base *xpn = &(self->parent);
	//PPLTWATER pPW = xpn->pPl->pPltWater;
	//PGENOTYPE	pGen = xpn->pPl->pGenotype;
	//PBIOMASS		pBio = xpn->pPl->pBiomass;
	//PPLTCARBON		pPlC = xpn->pPl->pPltCarbon;
	//PPLTNITROGEN 	pPlN = xpn->pPl->pPltNitrogen;
	//PCANOPY		pCan = xpn->pPl->pCanopy;
	//PWEATHER 	pWe  = xpn->pCl->pWeather;
	
	/* Variables */
	int i;	// species looping variable
	double dt = xpn->pTi->pTimeStep->fAct;	// timestep
	double IncrSt;	// stem increment [tC/tree*yr]
	double a;		// RedFac (for PGM module);
	//Begin of Hong on 20180625, for modified TREENDYN3 by Miehle et al. self->conf.Tree_fast_growing =1
	double help, fi; 
	int iMax;
	const int ITERMAX = 1;     // maximum number on iterations for height - diameter growth
	double h01,d01, d13;             // estimated height and diameter at 10% tree height (m) and Diameter at breast height (at 1.3 m)
    double a0, a1;
	double hd01,hd13;           // ratio between height and diameter at 10% tree height and 1.3m
    double height_before_Increment, diameter_before_Increment, volIncr, volNow, diam01Incr, heightIncr, height, diameter, volume; //Added by Hong, for tracing back the increment of height and diameter
	int k;
	//End of Hong on 20180625
   
	/*Functions*/
	extern int libtreemix_set_tree_geometry(libtreemix *self, int i);	//StartValues.c
	//extern int libtreemix_set_tree_morphology(libtreemix *self, int i);
	int libtreemix_geometric_relations(libtreemix *self, int i);
	
	/****************************************************************************************************************/
	
	
	
	for(i=0; i<self->conf.Species_Count; i++)
	{
		
	//Addd by Hong for Poplar (self->conf.Tree_fast_growing =1)
	height_before_Increment = self->plant[i].H;
	diameter_before_Increment = self->plant[i].D;	
		
	  if (self->conf.Tree_fast_growing == 0)
	  {  
		libtreemix_set_tree_geometry(self, i);
		libtreemix_geometric_relations(self, i);
		
		// eq. 102
		a = (M_PI/4.0)*self->plant[i].CWdDens*self->plant[i].Form;											
		
		// eq. 103, CWdMass is not being used, since the woodmass is calculated
		// IncrSt must be equal to the stem increment computed from the total wood increment after accounting for stem number
		// , deadwood losses, underground wood fraction, and branch fraction
		// [tC/tree*yr]
		IncrSt = ((self->plant[i].Increment - self->plant[i].DeadWdLoss * self->plant[i].CWdWeight/1000.0)
				/  self->plant[i].density) * (1.0-self->plant[i].UgWdFr)*(1.0-self->plant[i].BrWdFr);									
		self->plant[i].IncrSt = IncrSt;
	
		if(self->conf.Tree_Geometry == 1)
		{
			// no crowding, H/D is 
			if((self->silv[i].Crowding == 0.0)&&(self->plant[i].HD <= self->plant[i].HDmin))
			{
				self->plant[i].IncrD = IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2.0));		// eq. 105
				self->plant[i].IncrH = self->plant[i].HD*self->plant[i].IncrD;							// eq. 104
				self->plant[i].CanopyFormation = 1.0;
			}
			else if((self->silv[i].Crowding == 0.0)&&(self->plant[i].HD > self->plant[i].HDmin))
			{
				self->plant[i].IncrD = IncrSt/(2.0*a*self->plant[i].D*self->plant[i].H);				// eq. 107
				self->plant[i].IncrH = 0.0;																// eq. 106
				self->plant[i].CanopyFormation = 2.0;
			}
			else if((self->silv[i].Crowding == 1.0)&&(self->plant[i].HD < self->plant[i].HDmax))
			{
				self->plant[i].IncrD = 0.0;																// eq. 108
				self->plant[i].IncrH = IncrSt/(a*pow(self->plant[i].D, 2.0));							// eq. 109
				self->plant[i].CanopyFormation = 3.0;
			}
			else if((self->silv[i].Crowding == 1.0)&&(self->plant[i].HD >= self->plant[i].HDmax))
			{
				self->plant[i].IncrD = IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2.0));		// eq. 105
				self->plant[i].IncrH = self->plant[i].HD*self->plant[i].IncrD;							// eq. 104
				self->plant[i].CanopyFormation = 4.0;
			}
		}
		else if(self->conf.Tree_Geometry == 2)
		{
			//Here the new formulation of Height and Diameter Increment is implemented 
			//according to Jansen et al. (1995, Forschungszentrum Waldökosysteme)
			//p. A-22
			
			self->plant[i].IncrD = IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2));
			self->plant[i].IncrH = self->plant[i].HD*self->plant[i].IncrD;
			
			if(self->silv[i].Crowding == 0.0)
			{				
				self->plant[i].Incr_fh = pow(((self->plant[i].HDmax - self->plant[i].HD)/(self->plant[i].HDmax - self->plant[i].HDmin)), self->plant[i].Incr_p);
				self->plant[i].Incr_Red = (3.0 - self->plant[i].Incr_fh)/2.0;
				self->plant[i].CanopyFormation = 5.0;				
			}
			else if(self->silv[i].Crowding == 1.0)
			{
				self->plant[i].Incr_Red = pow(((self->plant[i].HD - self->plant[i].HDmin)/(self->plant[i].HDmax - self->plant[i].HDmin)), self->plant[i].Incr_p);
				self->plant[i].Incr_fh = 3.0-(2.0*self->plant[i].Incr_Red);
				self->plant[i].CanopyFormation = 6.0;
			}
			
			//Resulting Increments:
			self->plant[i].IncrD = self->plant[i].Incr_Red * (IncrSt/(3.0*a*self->plant[i].HD*pow(self->plant[i].D, 2.0)));
			self->plant[i].IncrH = self->plant[i].Incr_fh * self->plant[i].HD*self->plant[i].IncrD;
			//self->plant[i].IncrH = (3.0/self->plant[i].Incr_Red - 2.0) * self->plant[i].HD*self->plant[i].IncrD;
	
		}
		
	}
//added by Hong on 20180625	
	//tree geometry according to modified TREEDNY3 of Miehle et al., for poplar:
	else if(self->conf.Tree_fast_growing == 1)
	{
		
		libtreemix_set_tree_geometry(self, i);
		libtreemix_geometric_relations(self, i);
		
		height_before_Increment = self->plant[i].H;
		diameter_before_Increment= self->plant[i].D;
		
		height= self->plant[i].H;
		diameter = self->plant[i].D;
		volume = self->plant[i].density*(M_PI/4.0)*self->plant[i].Form * pow(self->plant[i].D, 2)*self->plant[i].H;
		
		if (self->plant[i].H>0.0)
		{
			//branch fraction
			if (self->plant[i].Dmax>0.0)
				self->plant[i].BrWdFr = self->plant[i].FiBrWdFr + (1.0-self->plant[i].FiBrWdFr)*exp(-self->plant[i].BrWdFrCoeff*self->plant[i].D/self->plant[i].Dmax); //eq. 20, pt.1
				//self->plant[i].BrWdFrCoeff= 10.0 in ini file
			else 
				self->plant[i].BrWdFr = self->plant[i].FiBrWdFr;
			if (self->plant[i].BrWdFr>0.5)
				self->plant[i].BrWdFr =0.5;

			//calculation of DBH: 			
			a0 = 1.678* pow(self->plant[i].density, (-0.289));
			a1 = -3.379*a0 + 2.593;
			self->plant[i].DBH = pow( (1/0.54*self->plant[i].CTrMass*(1.0-self->plant[i].UgWdFr)*1000 )/a0, 1/a1)/100;
						
			IncrSt = ((self->plant[i].Increment - self->plant[i].DeadWdLoss * self->plant[i].CWdWeight/1000.0)
				/  self->plant[i].density) * (1.0-self->plant[i].UgWdFr)*(1.0-self->plant[i].BrWdFr);									
		    self->plant[i].IncrSt = IncrSt;
			
			volIncr= IncrSt *self->plant[i].density/self->plant[i].CWdDens;
			volNow = self->plant[i].density*(M_PI/4.0)*self->plant[i].Form * pow(self->plant[i].D, 2)*self->plant[i].H;

			iMax= 1.0 + floor(volIncr/volNow);  // assumption rg
			if (iMax > ITERMAX) 
				iMax = ITERMAX;
            fi = 0.0;
			if (iMax <= 0)
				iMax = 0.0;
            else
				fi = 1.0 / (iMax);
			for (k = 1; k <= iMax; k++)
				{
               // height and diameter at 10% tree height for estimation with "true" form factor
				// assumption: linear diameter decrease at the base of the stem
                h01  = height * 0.1;
                d01  = diameter - h01 / (height / diameter) +EPSILON;
				d13 = self->plant[i].DBH +EPSILON; //diameter at breast height (1.3m)
                hd01 = height / d01 +EPSILON;

                if (d13 > 0.07)
					{
                     hd13 = hd01 * d01 / d13;
   
                     if (hd13 > self->plant[i].HDmax)
					   {
                        hd01 = self->plant[i].HDmax * d13 / d01;
                       }
                     }

                // increase of stem dimensions
		        a = (M_PI/4.0)*self->plant[i].CWdDens*self->plant[i].Form;// eq. 102	
							
                diam01Incr   = IncrSt * fi / (3.0 * a * pow(d01,2) * hd01);
                heightIncr = hd01 * diam01Incr;

                // new dimensions and volume
                volume  += volIncr *fi;
				height  += heightIncr;
                d01     += diam01Incr;

                // new estimated dimensions
                diameter = MAX(diameter, d01 + h01 / hd01);
                d13  = MAX(d13, d01 + (h01 - 1.3) / hd01);
                if (d13 < 0.0) 
					d13 = 0.0;
                }
		}
				
		//Resulting Increments:
		    self->plant[i].IncrD = (diameter - diameter_before_Increment);

			self->plant[i].IncrH = (height-height_before_Increment);			
			
			self->plant[i].IncrSt = IncrSt;
		
		    self->plant[i].CanopyFormation = 7.0;
		
	}
//End of Hong
				
		if(NewDay(xpn->pTi))
		{
			self->plant[i].IncrD_Day = 0.0;
			self->plant[i].IncrH_Day = 0.0;
		}
		self->plant[i].IncrD_Day += self->plant[i].IncrD/365.25*dt;
		self->plant[i].IncrH_Day += self->plant[i].IncrH/365.25*dt;
		
		self->plant[i].IncrD_Sum += self->plant[i].IncrD/365.25*dt;
		self->plant[i].IncrH_Sum += self->plant[i].IncrH/365.25*dt;
		
	}
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
};


int libtreemix_geometric_relations(libtreemix *self, int i)
{
	double V;	// stem volume per tree [m3/tree]
	double G;	// carbon mass per tree stem [tC/tree]
		
	/* note: the expon. factor 'brexp' (NOW: BrWdFrCoeff) in the orig. TREEDYN source code is = 100.0, why? It is 10.0 in BOSSEL!!!! */
	self->plant[i].BrWdFr = self->plant[i].FiBrWdFr + (1.0-self->plant[i].FiBrWdFr)*exp(-self->plant[i].BrWdFrCoeff*self->plant[i].D/self->plant[i].Dmax); //eq. 20, pt.1
	
	/* determining specific wood masses by morphologic relations and fractions */
	V = (M_PI/4.0)*self->plant[i].Form * pow(self->plant[i].D, 2)*self->plant[i].H;	// eq. 15, [m3], Stem Volume of an individual tree
	G = V*(self->plant[i].CWdDens * 1000.0);										// eq. 16, [kgC/tree], (Carbon) mass of an individual stem
	// volNew   = (mSap_vt[vt] + mCor_vt[vt]) 
 //                /( (1.0/(1.0 - fbranch_vt[vt]) + par.UGWDF[vt]) * help);
	/* whole stand [kgC/ha] */
	self->plant[i].CStMass = G*self->plant[i].density;	// Stem carbon mass, [kgC/ha]
	
	self->plant[i].CTrMass = self->plant[i].CWdWeight/1000.0/self->plant[i].density; // [tC/tree], wood mass per tree incl. branches, coarse roots
	
	// calculating ratio of respiring to total wood
		self->plant[i].b = 1.0;		// eq. 26		
		if(self->plant[i].D > (2.0*self->plant[i].RespLay))
		{
			double Hc; //HCone?
			double Hi; //HCore?
			double Vc; //VResp?
			double Vo; //VCone?
            
			// eq. (22)
			Hc = (12.0/(PI*self->plant[i].CWdDens))*(self->plant[i].CTrMass/pow(self->plant[i].D, 2.0));// eq. 22
			// eq. (23), sapwood is assumed to be concentrated on the outer 'shell' of the cone with a constant wall thickness 'dresp'
			Hi = (self->plant[i].D-(2.0*self->plant[i].RespLay))*(Hc/self->plant[i].D);					// eq. 23
            // eq. (24), volume of the full cone
			Vc = (PI/12.0)*pow(self->plant[i].D, 2.0)*Hc;
			// eq. (25), volume of the outer shell									// eq. 24
			Vo = (PI/12.0)*(pow(self->plant[i].D, 2.0)*Hc - pow((self->plant[i].D-(2.0*self->plant[i].RespLay)), 2.0)*Hi);	// eq. 25
            // eq. (26), the ration of respiring wood to total wood
			self->plant[i].b = Vo/Vc;																	// eq. 26
		}
		self->plant[i].CWdResp = (self->plant[i].b*self->plant[i].CWdWeight)/1000.0;						// resp. wood mass, [tC/ha]	
	
	
	//PRINT_MESSAGE(xpn,3,"Test");
	//PRINT_ERROR("FEhler");
	return RET_SUCCESS;
}