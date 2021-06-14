#include "xylemwaterflow.h"

int XWF_CLASS::flux_Ross()
{	 
	PTIME pTi = xpn->pTi;
	this->cpa = this->T_m->CrownProjArea;	 
	this->conf.delT=pTi->pTimeStep->fAct*(double)86400;//We compute in time unit second, dlta is timestep in days from Expert-N
	int hour = (int)(pTi->pSimTime->fTimeDay*24.0+0.001); // 0..23
	
    double pot_tra = 0.0;
    double pot_A= 0.0;
    
    // Penman-Monteith potential transpiration - or other
    pot_tra = this->getPotTranspiration(this->T_m); //[mm^3/s]  
    // Farquhar photosynthesis
    pot_A = this->getPotPhotosynthesis(this->T_m);//[µmol s-1]	
    
	int node_max;
	double flux_max;
	//subdivide timestep according to ross estimation of maximal timestep
	double t_cum =0.0; //This variable collects the time simulated
						//the loop stops if t_cum > conf.delT
	double dt;
	int layers;
	layers = xpn->pSo->iLayers - 2;
	int iTimesteps=0;
	while(t_cum <= this->conf.delT)
		{
		clock_t start, end1, end2, end3;
		start = clock();
		for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)
			{
			//water content is the state variable, calculate h, KP, K 
			this->T_m->node[i].H_total = gethfromWC(this->T_m->node[i].WC_Xylem, i,this->T_m);
			this->T_m->node[i].Kirchhoff = this->getKirchhoff(this->T_m->node[i].H_total, i,this->T_m);
			this->T_m->node[i].K = this->conduct3(i, this->T_m);//[mm^3/s]  
			this->T_m->node[i].BoundaryFlux=0.0;
			this->T_m->node[i].dBoundaryFluxdh=0.0;
			}
		
			
		this->GenerateBoundaryFlux(pot_tra, pot_A, SimStart(xpn->pTi)&&1,this->T_m);	// mm^3/s

		//*********************************************************
		//Debug
		/*for(int i = 0; i<T->iCnodes+T->iRnodes ; i++)
		{
			//T->node[i].g =0.0; //set zero gravitation
			//T->node[i].BoundaryFlux = 0.0; //set zero boundary flux
			//T->node[i].Transpiration = 0.0; //set zero boundary flux
		}*/
		end1 = clock();

		dt = estimate_timestep(this->T_m); //seconds
		dt = min(dt,fabs(t_cum-this->conf.delT));
		//dt = min(dt,100);		
		end2 = clock();
		if(int jj= this->T_m->checkTree())
		{
			int stop =1;
		}
		this->solveRoss(&dt, this->T_m);	 
		end3 = clock();
		double tross1 = end1 -start;
		double tross2 = end2 -end1;
		double tross3 = end3 -end2;
		if(int jj= this->T_m->checkTree())
		{
			int stop =1;
		}
	 
		//----------- balance (daily_balace.dat)-------------
		for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)
		{
			if(this->T_m->node[i].canopy)
			{
				this->B_m->transpiration_d += this->T_m->node[i].Transpiration  //[mm^3/s]
								*1e-6 * dt  / this->T_m->HydraulicArea; //[liter]
                this->B_m->transpiration_pot_d += this->T_m->node[i].Transpiration_pot * 1e-6 *dt/ this->T_m->HydraulicArea;
				this->B_m->transpiration_h += this->T_m->node[i].Transpiration*1e-6 * dt / this->T_m->HydraulicArea; //[liter
                this->B_m->transpiration_pot_h += this->T_m->node[i].Transpiration_pot * 1e-6 *dt / this->T_m->HydraulicArea;
				this->B_m->photosynthesis_d += this->T_m->node[i].Photosynthesis  * dt/ this->T_m->HydraulicArea; //[mm^3/s]								
                this->B_m->photosynthesis_pot_d += this->T_m->node[i].Photosynthesis_pot *dt / this->T_m->HydraulicArea;
				this->B_m->photosynthesis_h += this->T_m->node[i].Photosynthesis * dt/ this->T_m->HydraulicArea; //[liter
                this->B_m->photosynthesis_pot_h += this->T_m->node[i].Photosynthesis_pot  *dt/ this->T_m->HydraulicArea;
			}
			if(!this->T_m->node[i].canopy)
			{
				this->B_m->uptake_d += (this->T_m->node[i].BoundaryFlux ) //[mm^3/s]
								*1e-6 * dt; //[liter]	
				this->B_m->uptake_h += (this->T_m->node[i].BoundaryFlux )  //[mm^3/s]
								*1e-6 * dt; //[liter]	
			}
			
		}
		this->B_m->waterStorage_d = this->T_m->getWaterContent(0.0);//[liter] over dbh
		this->B_m->waterStorage_h = this->T_m->getWaterContent(0.0);//[liter] over dbh
		this->B_m->min_pot_stem = min(this->B_m->min_pot_stem, this->T_m->node[this->T_m->inodeDBH].H_total/101971.621298);// mm to Mpa
		this->B_m->max_pot_stem = max(this->B_m->max_pot_stem, this->T_m->node[this->T_m->inodeDBH].H_total/101971.621298);//mm to Mpa
		this->B_m->pot_stem = this->T_m->node[this->T_m->inodeDBH].H_total/101971.621298;//mm to Mpa
		//----------- balance end -------------*/
		iTimesteps++;
		t_cum += dt;
       // printf("tcum %f %f\n", t_cum, dt);
		if(fabs(t_cum-this->conf.delT) <1)
			break;
	}//while(t_cum <= conf.delT)
	
	for(int i=0;i<layers;i++)
		{
		this->Uptake_Janott[i] += (double)((double)this->UptLayer[i]*(double)this->conf.delT); //mm^3; //out of GenerateBoundaryFlux-function
		//printf("%f %f %f \n", this->Uptake_Janott[i], this->UptLayer[i], this->conf.delT);
		}
//	if(this->conf.beep)
//		Beep((DWORD)(-this->T_m->node[0].H_total/30),10);

	return RET_SUCCESS;
};



/*void flux_Hydrus(char *file,EXP_POINTER,float *UptLayerTotal, double *cpa,class tree *T)
{
	int flag = 0;
	double		stemflow				=	0;
	//struct	soils *boden = NULL;
	float pot_tra; //[mm/s] *(float)SoilToXylemArea*SolPartTime(pTi)/DeltaT
	
	if (SimStart(pTi))
	{	
		doAtSimStart(file, exp_p, T);
	}
	*cpa = T->CrownProjArea;
	conf.delT=pTi->pTimeStep->fAct*(float)86400;//We compute in time unit second, dlta is timestep in days from Expert-N
	//SB OUTPUT
	double rootUpt_sum=0.0;
	ofstream of;
	if (SimStart(pTi))
		of.open("water_balanceSB.csv");
	else
		of.open("water_balanceSB.csv",fstream::app);

	pot_tra = getPotTranspiration(exp_p,T); //mm^3/s for the whole tree

//---------------------------------------------------------
	//Michal Janott - Hydrus
	float time_cum =0.0;
	float delT_ori = (float)conf.delT;
	int ConvergenceReached;
	double convsum;
	int iter, error;
	double tempor=0.0;
	conf.delT *= conf.internal_timestep_ini;
	while(time_cum<delT_ori&&gameover==0)//begin of iterations
	{
		conf.delT=(float)min(conf.delT,delT_ori-time_cum);//for variable inner timestep
		
		ConvergenceReached=0;
		for(int i=0;i<T->iCnodes+T->iRnodes;i++)
			T->node[i].H_total=T->node[i].H_total_old;//initial values for iteration
		convsum=999999999999999;
		iter=0;
		while(ConvergenceReached==0) //start of internal iteration loop
		{
			for(int i=0;i<T->iCnodes+T->iRnodes;i++)
			{
				T->node[i].WC_Xylem=ThetaH(i, T->node[i].H_total, T);
				T->node[i].K = conduct3(i, T);
				T->node[i].Capacity_Xylem = cap(i, T);
			}
			error=0;
			
			GenerateBoundaryFlux(&UptLayer, exp_p,layers, pot_tra, SimStart(pTi)&&tempor==0&&iter==0,T);//construct equation system
			GenerateMatrix( &stemflow, SimStart(pTi)&& tempor==0&&iter==0,T); 							
			GenerateVector(SimStart(pTi)&& tempor==0&&iter==0,T);	
			

			clock_t start, end;
			start = clock();			
			superLU_pde(T);
			end = clock();
			double tt = end -start;
			flag++;
			
			if(fabs(T->node[0].H_new_total-T->node[0].H_total) > 3000.0 )//SB too bad, will not converge, do smaller timesteps RIGHT NOW
			{
				iter=0;//abort actual iteration
				conf.delT/=2.0;//SB 2.0;//reduces time step above
				tempor=1;
				ConvergenceReached=FALSE;
				convsum=999999999999999;
				if(conf.delT<0.0005)//give up, no solution
				{
					gameover=1;//Expert-N continues running without computing water uptake
				}
				for(int i=0;i<T->iCnodes+T->iRnodes;i++)
					T->node[i].H_total = T->node[i].H_total_old;//initial values for iteration
			}
			double convsum_new = 0;
			ConvergenceReached=test_cvg(&convsum_new,T);

			for(int i=0;i<T->iCnodes+T->iRnodes;i++)
					T->node[i].H_total=T->node[i].H_new_total;
			
			convsum=convsum_new;
			//is there a bad value?
			if(ConvergenceReached)
			{
				for(int i=0;i<T->iCnodes+T->iRnodes;i++)
				{
					if(fabs(T->node[i].H_total)>(double)10000000||T->node[i].H_total!=T->node[i].H_total)//propably bad value
					{
						error=1;
						break;
					}
				}
			}
			iter++;
			if(iter>conf.iterbreak||error>0)//interupt iteration and repeat with smaller timestep
			{
				iter=0;//abort actual iteration
				conf.delT/=2.0;//SB 2.0;//reduces time step above
				tempor=1;
				ConvergenceReached=FALSE;
				convsum=999999999999999;
				if(conf.delT<0.0005)//give up, no solution
				{
					for(int i=0;i<T->iCnodes+T->iRnodes;i++)
					gameover=1;//Expert-N continues running without computing water uptake
				}
				for(int i=0;i<T->iCnodes+T->iRnodes;i++)
					T->node[i].H_total = T->node[i].H_total_old;//initial values for iteration
			}

		}
		//Convergence reached for internal timestep
		tempor=0;
		time_cum+=conf.delT;
		//fprintf(files.logfile, "\titeration finished at iter = %d\t\tactual timestep = %f%%\t", iter, (float)(conf.delT/delT_ori*100));

		//Create mass balance, dont forget to convert later : /=(float)SoilToXylemArea;//->xylem to soil area
		for(int i=0;i<layers;i++)
			UptLayerTotal[i] += (float)((float)UptLayer[i]*conf.delT); //mm^3;//out of GenerateBoundaryFlux-function

		for(int i=0;i<T->iCnodes+T->iRnodes;i++)	
		{
			//float fac=(float)pow(T->node[i].Diameter /T->node[0].Diameter ,(float)2.0);
			//*delTheta_sum +=(fac*arch.delZ[number] *(WC_Xylem_old[number]-WC_Xylem[number]));
			if(!T->node[i].canopy)//root
			{
				float temporf		=	conf.delT *	(float)T->node[i].BoundaryFlux;	//Root Water Uptake per root node for documentation
				 
				rootUpt_sum+=(temporf);
			}
		} 

		for(int i=0;i<T->iCnodes+T->iRnodes;i++)
		{
			T->node[i].H_total_old=T->node[i].H_total;
			T->node[i].WC_Xylem_old=T->node[i].WC_Xylem;
		}
	}//internal timestep finished 

//Michael Janott - Hydrus
//---------------------------------------------------------

	//for(int i=0;i<layers;i++)			
		//UptLayerTotal[i]/=(float)SoilToXylemArea;//->xylem to soil area
		//UptLayerTotal[i]*=T->max_XylemArea; //mm^3 
		
	double sum=0;
	for(int i=1;i<T->iCnodes+T->iRnodes;i++)
		sum+=T->node[i].H_total*1e-6;
	//SB Output
	rootUpt_sum /= SoilToXylemArea;
	//of << pTi->pSimTime->fTimeAct << " " <<rootUpt_sum  <<"\n";
	of << pTi->pSimTime->fTimeAct<<" " <<T->node[0].H_total<< "\n";
	of.close();

	if(conf.beep)
		//Beep(223,50);
		Beep((DWORD)(-T->node[0].H_total/30),10);
 					
};*/

double XWF_CLASS::getPotTranspiration(tree *T)
{
	double pot_tra ; //[mm/s]
	pot_tra = 0.0;
    if(this->conf.pot_tra!=-99)
		{
		pot_tra=(double)(this->conf.pot_tra/86400*T->HydraulicArea*1e6);//[mm/d]soil -> [mm^3/s] 
		}
	else if(T->LAIDistribution ==2) //read transpiration distribution over all voxels from files
		{
		char *filename;
		int ihour = (int)(xpn->pTi->pSimTime->fTimeDay*24.0+0.001); // 0..23
		int iyear = xpn->pTi->pSimTime->iyear;
		int iJulianDay = (int)xpn->pTi->pSimTime->fTimeY; // actual Julian day
		
		filename = g_strdup_printf("%s/%sDate%.4i%.3i%.2i.nodetranspiration",xpn->pXSys->project_path,T->in_transpiration,iyear,iJulianDay,ihour);
	
		std::ifstream in(filename);
		if(in==NULL)
		{
			int exception = 1;
		}
		double LA, E_mmh;
		//read header
		in >> E_mmh >> LA;
		pot_tra = E_mmh * LA;	//[mm/h]soil -> [dm^3/h] 
		pot_tra /= 3600.0f;		//[dm^3/h]->[dm^3/s]	 
		pot_tra *= 1e6;			//[dm^3/s]->[mm^3/s] 
		//pot_tra *= 0.001;
		in.close();
	
		//g_free(filename);
		}
	else
		{
		if ((xpn->pPl->pPltWater->fPotTranspR > 0.0) && (xpn->pPl->pCanopy->fLAI > 0.0))
			{
			pot_tra = (double)(xpn->pPl->pPltWater->fPotTranspR / 24.0 / 3600.0) ; // [mm/d] -> [mm->s]
			pot_tra *= (double) (T->HydraulicArea*1e6); // [mm/s]soil -> [mm^3/s]
			}
		else
			pot_tra = 0.0f;
		}
		
		
	return pot_tra;//[mm^3/s] 
}

double XWF_CLASS::getPotPhotosynthesis(tree *T)
{
        PCLIMATE pCl = xpn->pCl;
        double DarkRespiration;
        double photo_h;
        double Anet_h;
       	double pot_A;
        double PAR;
        
        DarkRespiration = 0.0;
        photo_h = 0.0;
        Anet_h = 0.0;
        PAR = 0.0;
        pot_A = 0.0;
        
        // Dark respiration // dependency of temperature (Falgen et al. 1996)
		DarkRespiration = exp( this->conf.Rd_f - this->conf.Ea_Rd / (this->conf.R*(pCl->pWeather->fTempAir + this->conf.CtoK)) );	// [µmol]
		//Rd_h[i] = 1.1 * exp(63000* ( 1.0/ (conf.R*298.0) - 1.0/  (conf.R*AirTemp) ) );	// Peter

		// Photosynthesis // Leuning 1998 following Farquhar et al. 1980 
		if(pCl->pWeather->fSolRad >0)
            {
                PAR = (pCl->pWeather->fSolRad / 24) * 277.777777778 * this->conf.GtoPAR * this->conf.WtoUMOL *1e-6;		// MJ m^-2 d^-1 -> Wh; [mol photon m-2 s-1]
                photo_h  = getA(pCl->pWeather->fTempAir + this->conf.CtoK, this->conf.cCO2, PAR)*1e6;  // [mol -> µmol] 
            }
		Anet_h = photo_h - DarkRespiration ; 
    
        pot_A = (double)(Anet_h / 3600.0);		//	hour -> second
//pot_A*=(double)(T->HydraulicArea);			//	m-2  -> tree-1										
        if(xpn->pPl->pCanopy->fLAI==0)
            pot_A=0.0f;
        return pot_A;
}