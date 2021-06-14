#include "xylemwaterflow.h"

//returns the Kirchhoff Potential
//input: potential h
double XWF_CLASS::getKirchhoff(double h, int i, class tree *T)
{
	double ret;
	double a = T->BC_a;
	double Kmax;
	if(i<T->iCnodes)
		Kmax = T->kmax_branch*T->node[i].XylemArea;
	else
		Kmax = T->kmax_root*T->node[i].XylemArea;
	double test = (1.0-T->BC_Lambda*this->eta);
	double phi_a = Kmax*T->BC_a/(1.0-T->BC_Lambda*eta);
	ret = phi_a * pow((h/a),1.0-(double)T->BC_Lambda*eta);
	if(h>=T->BC_a)
	{		
		double b = pow((double)T->Theta_aev/T->porosity,eta);
		ret =	phi_a + 
				Kmax * ( h*b -a*b + (b-1.0)*pow(a-h,(double)3.0)/((double)3.0*a*a) );
	}
	return ret;
}




//Ross 2003
void XWF_CLASS::solveRoss(double *delT, class tree *T)
{
	double sigma = 0.5;	
	int pID;
	int n = T->iCnodes + T->iRnodes;
	struct pff *liste = new pff[T->maxchilds+2];
	clock_t start, e1, e2;
	start=clock();
	int done =0;
	while(!done)
	{
		//for Superlu
		int c=0;
		double *sol=(double*)malloc(n*sizeof(double));
		//we have to reallocate the SuperLU stuff again, because it is set free @ Destroy_CompCol_Matrix(&A) in the slu function;
		T->SLU_asub=(int*) malloc(T->nentry*sizeof(int));
		T->SLU_a=(double*) malloc(T->nentry*sizeof( double));
		T->SLU_xa=(int*) malloc((T->iCnodes+T->iRnodes+1)*sizeof(int));

		//set the matrix row-wise, make the SuperLU sparse matrix format rowwise, 
		//specify this in the solver by the flag SLU_NR
		for(int i=0;i<n;i++)//loop over all lines
		{
			//parent and diagonal element for every node does not depend on type
			pID = T->node[i].parent;

			//Ross "a"
			T->MatrixA[pID] = dqidSi(pID, i, this->eta, T); //this is Ross "a"

			double	childterm_d = 0.0, 
					childterm_b=0.0;
			int 	ic;
			for(int cc=0; cc< (int)T->node[i].child.size();cc++)
			{
				ic = T->node[i].child.at(cc);
				childterm_d += fluxiToj(i,ic,T) ; 
				childterm_b += dqidSi(i,ic,this->eta,T) ;
				//Ross "c"
				T->MatrixA[ic] =	-dqidSj(i,ic,this->eta,T) ;//Ross "c"
			}
			//Ross "d"
			T->rhs[i] = -( + fluxiToj(pID,i,T) 
							- childterm_d
							   - T->node[i].BoundaryFlux //from tree to soil: negative value
							   - T->node[i].Transpiration //from tree to atmosphere: pos value
							   )/ (sigma);
			//Ross "b"			
			T->MatrixA[i] = + dqidSj(pID,i,this->eta,T) 
							//-T->node[i].dBoundaryFluxdh
								-childterm_b
								-T->node[i].l  * T->node[i].XylemArea * T->porosity / (sigma * (*delT));
            
          // printf("%f %f %f %f %f %f %f\n",T->MatrixA[i], dqidSj(pID,i,this->eta,T) , childterm_b, T->node[i].l , T->node[i].XylemArea, T->porosity, sigma)   ;
        
			//*************************************************************	
			//SUPER LU
			//*************************************************************	
			T->SLU_rhs[i] = T->rhs[i];
			//womit fängt es an: welches child kommt in der reihe als erstes
			//this is SuperLU stuff
			liste[0].i = T->node[i].parent;
			liste[0].v = T->MatrixA[pID];
			liste[1].i = i;
			liste[1].v = T->MatrixA[i];

			for(int k=2;k<=T->maxchilds+1;k++)
			{
				liste[k].i = 100000; 
				liste[k].v = 0;
			}
			for(int k=0;k<(int)T->node[i].child.size();k++)
			{
				ic = T->node[i].child.at(k);
				liste[2+k].i = ic;
				liste[2+k].v =T->MatrixA[ic];
			}
			quicksort(liste, T->maxchilds+1);	//node-values are written by number-order in a[]		
			T->SLU_xa[i]=c;//Zeile i
			for(int ii=0;ii<T->maxchilds+2;ii++)
			{
				if(liste[ii].i != 100000)
				{
					T->SLU_a[c] = liste[ii].v;
					T->SLU_asub[c] = liste[ii].i;
					c++;
				}
			}			
            
        //printf("%f %f %f \n", T->SLU_a[i], T->SLU_xa[i], T->SLU_asub[i]);
		}
		T->SLU_xa[T->iCnodes+T->iRnodes]=T->nentry;

		 
		e1 = clock();
		superLU_Ross(sol, T);
		e2 = clock();
		
		double span3 = e2-e1;
		double span2 = e2-start;
				
		for(int i=0;i<n;i++)	
			T->rhs[i] = sol[i];
		free(sol);

		//check new values			
		double newWC;
		int bad=0;
		for(int i=0;i<n;i++)
		{
			newWC = T->node[i].WC_Xylem + T->rhs[i] * T->porosity  ;
			if(newWC > T->porosity)
			{
                printf("%d %d %f %f\n",i, T->node[i].parent,newWC, T->rhs[i]);        
				bad=1;
			}
		}
	// end of superLU *******************************************************
		if(!bad)
			done =1;
		else 
			*delT *=0.5;
	}//while !done 

	//set the new values
	for(int i=0;i<n;i++)
	{
		T->node[i].WC_Xylem += T->rhs[i] * T->porosity ;//rhs contains dS=dTheta/epsilon 
        //printf("%d %d %f %f %f\n",i, T->node[i].parent, T->node[i].WC_Xylem, T->rhs[i], (*delT));        
	}
	
	delete [] liste;
	 
}


//derivatives
double dKPdS(int i, class tree *T)// 
{	
	double a	= T->BC_a;
	double e	= T->porosity;
 
	return T->node[i].K* (a*e/(T->Theta_aev-e));	
}

double dKdS(int i, double eta, class tree *T)
{
	double Kond = T->node[i].K;
	double WC	= T->node[i].WC_Xylem;
	double e	= T->porosity;
	double S	= WC/e;
	double h	= T->node[i].H_total;
	double a	= T->BC_a;
	if(h < a)
	{
		return eta*Kond/S;
	}
	else
	{
		 
		double A =  1.0-pow(T->Theta_aev/e,eta);  
		double B =  (S*e-T->Theta_aev)/(e-T->Theta_aev);
		double C =  e/(e-T->Theta_aev);
		if(i<T->iCnodes)
			return T->kmax_branch*T->node[i].XylemArea*A*2.0*B*C;	
		else
			return T->kmax_root*T->node[i].XylemArea*A*2.0*B*C;	
	}
}

double dqidSi(int i, int j, double eta, class tree *T) //set inter for interface flux
{
	
	double kmaxi, kmaxj;
	if(i<T->iCnodes)
		kmaxi = T->kmax_branch;
	else
		kmaxi = T->kmax_root;
	if(j<T->iCnodes)
		kmaxj = T->kmax_branch;
	else
		kmaxj = T->kmax_root;
	double f = (T->node[j].XylemArea * kmaxj) / ( T->node[i].XylemArea * kmaxi); //Kj,max/Ki,max

	double z1= 0.5 * T->node[i].l;
	double z2= 0.5 * T->node[j].l;

	double g1 = T->node[i].g;
	//if the direction of the flow is from i to parent, the sign has to change
	if(j==T->node[i].parent)
		g1 = -g1;
	double B = 1.0 /(f*z1/z2 +1.0);
	double ret = (1.0-B)/z1 * dKPdS(i,T) + g1*(1.0-B) * dKdS(i,eta,T);

    //if (ret != ret)
    //    printf("%d %d %f %f %f %f %f %f %f \n", i, j, ret, B, z1, dKPdS(i,T), g1, dKdS(i,eta,T), eta);

	return ret ;//*T->node[i].XylemArea; //[mm^3/s]
}


double dqidSj(int i, int j, double eta, class tree *T) //i==j for interface flux
{
	double kmaxi, kmaxj;
	if(i<T->iCnodes)
		kmaxi = T->kmax_branch;
	else
		kmaxi = T->kmax_root;
	if(j<T->iCnodes)
		kmaxj = T->kmax_branch;
	else
		kmaxj = T->kmax_root;
	double f = (T->node[j].XylemArea * kmaxj) / ( T->node[i].XylemArea * kmaxi); //Kj,max/Ki,max
	double z1= 0.5 * T->node[i].l;
	double z2= 0.5 * T->node[j].l;

	double g2 = T->node[j].g;
	//if the direction of the flow is from child of j  to j, the sign has to change
	if(i!=T->node[j].parent)
		g2 = -g2;
	double B = 1.0 /(f*z1/z2 +1.0);
	double ret = -B/z2 * dKPdS(j,T) + g2*B * dKdS(j,eta,T);	
	return ret ;//*T->node[i].XylemArea;//[mm^3/s]
}

//Flux
double fluxiToj(int i, int j, class tree *T)
{	
	double kmaxi, kmaxj;
	if(i<T->iCnodes)
		kmaxi = T->kmax_branch;
	else
		kmaxi = T->kmax_root;
	if(j<T->iCnodes)
		kmaxj = T->kmax_branch;
	else
		kmaxj = T->kmax_root;
	double f = (T->node[j].XylemArea * kmaxj) / ( T->node[i].XylemArea * kmaxi); //Kj,max/Ki,max
	double z1= 0.5 * T->node[i].l;
	double z2= 0.5 * T->node[j].l;

	double g1 = T->node[i].g;
	//if the direction of the flow is from i to parent, the sign has to change
	if(j==T->node[i].parent)
        g1 = -g1;
	double g2 = T->node[j].g;
	//if the direction of the flow is from child of j  to j, the sign has to change
	if(i!=T->node[j].parent)
        g2 = -g2;

	double B = 1.0 /(f*z1/z2 +1.0);
	double f1=T->node[i].Kirchhoff / z1;
	double f2=T->node[j].Kirchhoff / z2;
	double ret = f1 - B*( f2 + f1 + g1*T->node[i].K - g2*T->node[j].K ) + g1*T->node[i].K;
	return ret;//*T->node[i].XylemArea;//[mm^3/s]
}

//Tools
double returnINF()
{
	double a=0.0;
	return 1.0/a;
}
double returnIND()
{
	double a=0.0;
	return 0.0/a;
}



double estimate_timestep(class tree *T)
{
	//dSmax is the minimal difference between water content and porousity
	double Smax=0.9;
	double maxv=0.0;
	double maxflux =0.0;
	double maxBoundaryFlux=0.0;
	
	double max_flux = 0.0;
	int node_with_max = 0;

	for(int i = 0; i<T->iCnodes+T->iRnodes; i++)
	{
		int pID = T->node[i].parent;
		double dq= fluxiToj(pID, i,T);
		double childterm = 0.0;
		int 	ic;
		for(int cc=0; cc< (int)T->node[i].child.size();cc++)
		{
			ic = T->node[i].child.at(cc);
			childterm += fluxiToj(i,ic,T);
		}
		dq -= childterm;

		double d2 = T->node[i].XylemArea * T->node[i].l * T->node[i].WC_Xylem;
		if(dq > maxflux)
		{
			maxflux=max(maxflux, dq);
			node_with_max = i;
			max_flux		=dq;
		}
		maxBoundaryFlux=max(maxBoundaryFlux,T->node[i].BoundaryFlux/d2);
       // printf("1. %d %f %f \n",i,maxBoundaryFlux,T->node[i].BoundaryFlux/d2);
		maxBoundaryFlux=max(maxBoundaryFlux,T->node[i].Transpiration/d2);
        //printf("2. %d %f %f \n",i,maxBoundaryFlux,T->node[i].Transpiration/d2);
		double ratio=dq/d2;
		maxv = max(maxv, dq/d2);
        //printf("3. %d %f %f \n",i,maxv,dq/d2);
	}
	maxv = max(maxv, maxBoundaryFlux );
    //printf("end %f %f %f \n \n",maxBoundaryFlux,maxv,Smax / maxv);
	return Smax / maxv;	
}
 
