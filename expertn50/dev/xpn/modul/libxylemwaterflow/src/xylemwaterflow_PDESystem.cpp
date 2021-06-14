#include "xylemwaterflow.h"
#include <time.h>

//generates boundaryflux for roots
//and transpiration for leaves
void XWF_CLASS::GenerateBoundaryFlux(double pot_tra, double pot_A, int simstarttimestep, class tree *T)
{
	int layers;
	
	layers = xpn->pSo->iLayers-2;
	
	//init
	for(int i=0;i<T->iCnodes+T->iRnodes;i++)
	{
		T->node[i].BoundaryFlux=0;
		T->node[i].Transpiration=0;
        T->node[i].Transpiration_pot=0;
		T->node[i].Photosynthesis=0;
        T->node[i].Photosynthesis_pot=0;
	}
	for(int j=0;j<layers;j++)
		this->UptLayer[j]=0;

	//load distribution
	if(T->LAIDistribution ==2 && pot_tra > 0.0f) //read transpiration distribution over all voxels from files
	{
        FILE *infile;
		char *filename;
		char *line;
		int ihour = (int)(xpn->pTi->pSimTime->fTimeDay*24.0+0.001); // 0..23
		int iyear = xpn->pTi->pSimTime->iyear;
		int iJulianDay = (int)xpn->pTi->pSimTime->fTimeY; // actual Julian day
		
		filename = g_strdup_printf("%s/%sDate%.4i%.3i%.2i.nodetranspiration",xpn->pXSys->project_path,T->in_transpiration,iyear,iJulianDay,ihour);
	
        infile =  fopen(filename ,"r");

		if(infile==NULL)
		{
            PRINT_ERROR("Missing transpiration distribution: please provide the required files or change the parameter 'LAIDistribution' in the 'xylemwaterflow.ini'-file ");
			//int exception = 1;
		}
        
        line = fgets_mem(infile);
        free(line);
		//in.getline(line,500); //header
		for(int i = 0; i<T->iCnodes; i++)
		{
            T->node[i].LA = atof(fgets_mem(infile));
			//in >> T->node[i].LA; //fraction of total transpiration according to stomata / light model
			if(T->node[i].LA < 0 || T->node[i].LA > 1.0f)
			{
				//int warning = 1;
                char *S2;
                S2 = g_strdup_printf("The fraction of the total transpiration at node %d is lower than 0 or larger than 1. Please check the respective transpiration distribution file!", i);
                PRINT_MESSAGE(xpn,3,S2);
                g_free(S2);
			}		
		} 
        fclose(infile);
		//in.close();
		g_free(filename);
	}

	//pot transpiration of every node
	for(int i = 1;i<T->iCnodes+T->iRnodes;i++)
	{
		//Roots
		if(!T->node[i].canopy && T->node[i].Z < -10.0001) //exclude taproots
		{ 
			if(T->node[i].H_total <-10000)
				this->GBF_sub(i, T->node[i].parent ,simstarttimestep, T); // mm^3/s negative values (direction on flux: tree ->soil)
		}
		//Branches
		else
		{
			//if (T->node[i].LA != 0.0)
			//	printf("%d %f %f \n", i, pot_tra, T->node[i].LA);
			if(pot_tra>0.1e-10 ) 
			{	
				T->node[i].Transpiration = T->node[i].LA*this->StomCond(pot_tra, i, T);//mm^3/s //pos values( direction on flux: tree ->atm)
                T->node[i].Transpiration_pot = T->node[i].LA *pot_tra;						//mm^3 s-1		
			}
            //Photosynthesis	
			T->node[i].Photosynthesis     = T->node[i].LA *this->StomCond(pot_A, i, T);		// [µmol s-1]
			T->node[i].Photosynthesis_pot = T->node[i].LA *pot_A;						// [µmol s-1]
		}
	}
}


double XWF_CLASS::GBF_sub(int n, int parent, int simstarttimestep,  class tree *T)
{
	//double sumsoil=0
    //double sumplant=0.0;
	int soildepth=0;
	double	untereBodenschicht=0;
	double	obereBodenschicht=0;
	//int i=0;	  
	double	uptake	=	0;
	double	HSoilNew=	0; 
	double	checksum=0;
	int		j=0; 
	//int		nodin=0;
    int iLayer;
 	double			fEffectPot[MAXSCHICHT];
	int layers;
	layers = xpn->pSo->iLayers-2;
	PWLAYER			pWL;
	PWATER	pWa = xpn->pWa;
	//convert XN data structure to soil struct
	for(pWL=pWa->pWLayer->pNext,iLayer = 1;(pWL->pNext!=NULL)&&(iLayer < xpn->pSo->iLayers-1);(pWL = pWL->pNext, iLayer++))	
	{
		fEffectPot[iLayer-1]=(double)pWL->fMatPotAct;//mm	
	}  

	for(soildepth=0;soildepth<layers;soildepth++)
	{//wann liegt Wurzel in Bodenschicht? mode=1
		obereBodenschicht=(double)fabs(depth[soildepth]);
		untereBodenschicht=(double)fabs(depth[soildepth+1]);	
		if(intersects(obereBodenschicht, untereBodenschicht, n, T))//wenn Bodenschicht zwischen node und arch.parent[node] liegt
		{
			j++;
			//SB x=0: just take the potential of the soil at the current depth
			//HSoilNew = (conf.x*T->node[n].H_total +(*HSoil)[soildepth])/(double)(conf.x+1);//soil matric potential near root is influenced by root potential
			HSoilNew = fEffectPot[soildepth];
			if((HSoilNew>T->node[n].H_total )||this->conf.HydraulicLift)
			{
				//T->RadialRootKmax ;
				//double test = fEffectPot[soildepth];

				// SB [mm/s]
				//uptake=T->node[n].frac[soildepth]*2.0*T->node[n].l/(T->node[n].TotalDiameter*0.5) *T->RadialRootKmax*(((*HSoil)[soildepth]-T->node[n].H_total )/(conf.x+1));				
				//uptake*= T->node[n].XylemArea;
				//uptake =  T->node[n].l * pow(0.5*T->node[n].TotalDiameter,2.0) *PI  *T->RadialRootKmax*(fEffectPot[soildepth]-T->node[n].H_total );// mm^3/s
				uptake = T->node[n].frac[soildepth] * T->node[n].l * T->node[n].TotalDiameter *PI  *T->RadialRootKmax*(fEffectPot[soildepth]-T->node[n].H_total );// mm^3/s
				this->UptLayer[soildepth]+=uptake;
				//printf("%f %f %f %f %f %f %f %f \n", this->UptLayer[soildepth], T->node[n].frac[soildepth], T->node[n].l, T->node[n].TotalDiameter, PI  ,T->RadialRootKmax, fEffectPot[soildepth],T->node[n].H_total );// mm^3/s)

				T->node[n].BoundaryFlux += -uptake;//neg value: "flux from tree to soil"
				// We also have to compute the change of flux with change of saturation for the euler solution
				if(T->node[n].H_total < T->BC_a)
				{
					T->node[n].dBoundaryFluxdh = T->BC_a*(-1.0/T->BC_Lambda)*pow(T->node[n].WC_Xylem/T->porosity,(-1.0/T->BC_Lambda)-1.0);
				}
				double dhdS=0;
				if(T->node[n].H_total > T->BC_a)
				{
					
					dhdS =  T->BC_a*T->porosity/(T->Theta_aev-T->porosity);
					//int stiop =1;
				}
				T->node[n].dBoundaryFluxdh = dhdS * T->node[n].l * pow((T->node[n].TotalDiameter*0.5),2.0)*PI  *T->RadialRootKmax;
				 
				checksum+=uptake;
			}
			 
		}
	}
	return checksum;
}
/*
void GenerateMatrix(double* stemflow, int simstarttimestep, class tree *T)//generate stiffness matrix for the PDES
{
	int	i = 0,	j = 0,	b=0, c=0 ;
	int	k=0;
	struct pff *liste = new pff[T->maxchilds+2];
	double	capa=0, cond_parent=0;
	double gradient=0;
	//float* dZ=NULL;
	double fac=0;
	
	//we have to reallocate the SuperLU stuff again, because it is set free @ Destroy_CompCol_Matrix(&A) in the slu function;
	T->SLU_asub=(int*) malloc(T->nentry*sizeof(int));
	T->SLU_a=(double*) malloc(T->nentry*sizeof(double));
	T->SLU_xa=(int*) malloc((T->iCnodes+T->iRnodes+1)*sizeof(int));

	//initialize
	//dZ=(float*)realloc(dZ,(T->iCnodes+T->iRnodes)*sizeof(float));		
	//for(i=0;i<T->iCnodes+T->iRnodes;i++)
	//	dZ[i]=T->node[i].l;

	 
	
	for(i=0;i<T->iCnodes+T->iRnodes;i++)
	{
		capa					=	0;//[m³/(m³*mm)]
		cond_parent				=	0;
		//fac=pow(T->node[i].Diameter / T->node[0].Diameter,2.0);
		fac=T->node[i].XylemArea / T->node[0].XylemArea;
		double childterm = 0;
		for(int cc=0;cc<(int)T->node[i].child->size();cc++)//loop over all childs
		{
			childterm += (double)T->node[T->node[i].child->at(cc)].l  ;
		}
		

		if(simstarttimestep)
			T->node[i].deltaZVec	=	(T->node[i].l+childterm) / (1+(double)T->node[i].child->size());
		cond_parent				=	(T->node[i].K + T->node[T->node[i].parent].K)/((double)2*(double)T->node[i].l);


		T->node[i].capVec 	=	fac * T->node[i].deltaZVec * T->node[i].Capacity_Xylem /(double)conf.delT;
	
		//womit fängt es an
		//this is SuperLU stuff
		childterm = 0;
		for(int k=2;k<=T->maxchilds+1;k++)
		{
			liste[k].i = 100000;
		}
		for(int cc=0;cc<(int)T->node[i].child->size();cc++)//loop over all childs
		{
			childterm += (T->node[i].K + T->node[T->node[i].child->at(cc)].K)/((double)2*T->node[T->node[i].child->at(cc)].l );
		}
		liste[0].i	= T->node[i].parent ==	-1 ? 100000  : T->node[i].parent ;
		liste[1].i	= i;
		liste[0].v= (double)(-1)* cond_parent;
		liste[1].v= cond_parent + childterm + T->node[i].capVec;

		for(int cc=0;cc<(int)T->node[i].child->size();cc++)//loop over all childs
		{
			liste[cc+2].v = (double)(-1)* (T->node[i].K + T->node[T->node[i].child->at(cc)].K)/((double)2*T->node[T->node[i].child->at(cc)].l );
			liste[cc+2].i = T->node[i].child->at(cc);
		}
		quicksort(liste,(int)T->maxchilds+1);	//node-values are written by number-order in a[]
		
		T->SLU_xa[i]=c;//Zeile i

		for(int ii=0;ii<T->maxchilds+2&&liste[ii].i != 100000;ii++)
		{
			T->SLU_a[c] = liste[ii].v;
			T->SLU_asub[c] = liste[ii].i;
			c++;
		}

	
		
	}
	T->SLU_xa[T->iCnodes+T->iRnodes]=T->nentry;
}


void GenerateVector(int simstarttimestep, class tree *T)//right hand side of the PDES
{
	int i,j=0,b=0;
	float dTheta=0;
	double	capa=0, grav=0, qb=0, dTh=0;
	double fac;

	for(i=0;i<T->iCnodes+T->iRnodes;i++)
	{
		capa	=	0;//capacity term
		grav	=	0;//gravitation term
		qb	=	0;//boundary flux
		dTh	=	0;//change in water content
		//fac=pow(T->node[i].Diameter/T->node[0].Diameter,2.0);
		fac=T->node[i].XylemArea / T->node[0].XylemArea;
		double childterm =0;
		for(int cc=0;cc<(int)T->node[i].child->size();cc++)//loop over all childs
		{
			childterm += T->node[T->node[i].child->at(cc)].cosine	*(T->node[i].K		+ T->node[T->node[i].child->at(cc)].K);
		}
		grav	=	T->node[i].cosine * (T->node[T->node[i].parent].K	+ T->node[i].K)//K[i] in mm/s
							-childterm;
		
		qb	=	(double)T->node[i].BoundaryFlux ;
		capa	=	T->node[i].capVec * T->node[i].H_total ;
		dTh	=	T->node[i].l *(T->node[i].WC_Xylem_old - T->node[i].WC_Xylem )/(double)conf.delT;
		dTh*=fac;
		T->node[i].Vectorb	=	capa + dTh - grav/(double)2 + qb;
		double dummy=0.9;
	}
}


int test_cvg(double* convsum, class tree *T)//test for convergency of solver
{
	int ConvergenceReached=TRUE;
	int i=0;
	double test;
	*convsum=0;
	for(i=0;i<T->iCnodes+T->iRnodes;i++)
	{
		test = T->node[i].H_new_total -T->node[i].H_total ;
		(*convsum)+=(T->node[i].H_new_total - T->node[i].H_total );
		if(fabs(T->node[i].H_new_total - T->node[i].H_total)>conf.dev)
		{
			ConvergenceReached=FALSE;
			//i=arch.length;
		}
	}
	//fprintf(convout, "%lf\t",(*convsum)/(double)arch.length);
	return ConvergenceReached;
}*/

double XWF_CLASS::StomCond(double pot_tra, int i, class tree *T)//interpolates actual transpiration
{
	double solution=0.0;
    double reduction=0.0;
	double Hnull=-850000.0;
    //double Hnull=-300000.0;
	double h=0.0;
	double qmin=0.0;
    double pottra ;
    pottra = 0.0;
    pottra = pot_tra;
	if(T->node[i].H_total >= 0)
		return pottra;
	if(T->stom_delay>0)
	{
		h=T->node[i].H_total_old +(double)(this->conf.delT-T->stom_delay)*((T->node[i].H_total -T->node[i].H_total_old )/(double)this->conf.delT);
	}
	else
		h=T->node[i].H_total;
    
    //printf("%f %f",h, Hnull);
	qmin=max(0,T->node[i].K *(h-Hnull)/T->node[i].l );//mm/s
	qmin *= T->node[i].XylemArea; //mm^3/s
	reduction = max(0.0,exp(-pow((-T->node[i].H_total/T->stom_a),T->stom_b)));//max closure 100%  inter(hTr,h);
    solution=reduction*pottra;
	//solution=0.5*1.0;
	if(solution > qmin)
	//if(solution > 0.1)
    {
		solution=qmin;
	}
   /* if(reduction<0.5)
	{
		int halt =9;
	}
	if(T->node[i].H_total<-200000)
	{
		int halt =0;
	}
    
    if (pottra > 0.0)
        int halt = 12;*/
    
	return solution;//mm^3/s
}
