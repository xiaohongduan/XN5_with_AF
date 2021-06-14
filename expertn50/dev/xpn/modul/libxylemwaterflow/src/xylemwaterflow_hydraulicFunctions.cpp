#include "xylemwaterflow.h"

void XWF_CLASS::set_Vector_Theta(class tree *T)
{
	int i=0;
	this->hT =(double*) malloc(this->numbers*sizeof(double));
	for(i=0;i<this->numbers;i++)
	{
		T->node[0].H_total = (double) -i/this->step;//this is ugly
		this->hT[i] = Theta_sub(0,T);
	}
	//T->node[0].H_total = T->H_ini;
}

double	Theta_sub(int i,class tree *T)//Water content of xylem
{
	double solution=0;
	if(T->node[i].H_total<T->BC_a)
		solution=pow((double)T->node[i].H_total/T->BC_a,-T->BC_Lambda);
	else
		solution=(double)1;
	return solution;
}




//interpolates xylem water content
//input: potential
//returns: water content
double XWF_CLASS::ThetaH(int i, double h,class tree *T)
{
	double Th_a=0, solution=0;
	double	porousity=0;
	if(h<T->BC_a)
	{
		Th_a=inter(this->hT,h,this->step,this->numbers);
		solution=T->Theta_aev*Th_a;
	}
	else
		solution=T->Theta_aev+(T->porosity-T->Theta_aev)*(T->BC_a-h)/T->BC_a;
	return solution;
}
//interpolate value for xylem conductance, capacity, water content and stomatal conductance 
//from vector and soil properties from matrixes
double inter(double* Mat, double h, double step, int numbers)
{
	double abstand=0;
	int intpart=0;
	double f=0;
	double solution=0;

	intpart=(int)(-h*step);
	if(intpart<0)
		return Mat[0];
	if(intpart>numbers-1)
		return Mat[numbers-1];
	f=-h*step-(double)intpart;
	solution=(1-f)*Mat[(int)intpart]+f*Mat[(int)intpart+1];
	return solution;
}
/*
//-----------------------------------------------------------------------------
double  abspowerDBL(double x, double y)
//-----------------------------------------------------------------------------
{
  double sign,z;
  if (x < 0.0)
  {
    sign = -1.0;
    x  *= -1.0;
  }

  if (y < 0.0)
  {
    y *= -1.0;
    if (x != (double) 0.0)
    {
      x = 1.0/x;
    }
  }
  z = (double)pow((double)x,(double)y);
  return z;
}


*/
double XWF_CLASS::conduct3(int i,class tree *T)//conductivity in xylem
{
	double lower=0;//SB 0..1: this is the reduction factor from max value
	double f=0;	
 
	double upper=0;
	double sol3=0;
 

	if(T->node[i].H_total>=0)
		lower=1;
	else
	{
		if(T->node[i].H_total<T->BC_a)
			lower=interT(this->hK,T->node[i].WC_Xylem/T->porosity,this->numbers);
		else
		{
			lower=interT(this->hK,T->Theta_aev/T->porosity,this->numbers);
			upper=(1-lower)*pow((T->node[i].WC_Xylem-T->Theta_aev)/(T->porosity-T->Theta_aev), (double)2.0);
			lower+=upper;
			//lower=1;
		}
		//if(lower>1)
			//SB errormessage("conduct3() says: error while computing xylem conductivity\n");
	}
 
	//double dummy = T->node[i].XylemArea_norm*T->node[i].kmax*lower;
	double dummy;
	if(i<T->iCnodes)
		dummy = T->node[i].XylemArea*T->kmax_branch*lower;// mm^3
	else
		dummy = T->node[i].XylemArea*T->kmax_root*lower;// mm^3
	return dummy;// mm^3
	 
}

void XWF_CLASS::set_Vector_Cond(class tree *T)//generate vector containing xylem conductivities
{
	int i=0;

	this->hK=(double*) malloc(this->numbers*sizeof(double));
	for(i=0;i<this->numbers;i++)
		this->hK[i] = cond3_sub((double) i/(double)this->numbers, this->conf.contemp, T );// 0 - 1 as conduct3 is a function of Theta
}

double cond3_sub(double Theta, double contemp, class tree *T)
{
	return pow((double)Theta,(double)2./(double)T->BC_Lambda + contemp);
	//return pow(Theta,(float)2./T->BC_Lambda+(float)3.);
}

double interT(double* Mat, double Theta, int numbers)//interpolate value for xylem conductance
{
	double abstand=0;
	int intpart=0;
	double f=0;
	double solution=0;
	double nstep=0;
	
	nstep=1/(double)numbers;
	intpart=(int)(Theta*numbers);//fraction

	f=Theta*(double)numbers-(double)intpart;//doublepart=rest
	//f=f/(double)nstep;
	if(intpart<0)
		return Mat[0];
	if(intpart>numbers-1)//Theta>1
		return Mat[numbers-1];
	solution=(1-f)*Mat[(int)intpart]+f*Mat[(int)intpart+1];
	return solution;
}
/*
//SB!!
double cap(int i,class tree *T)////interpolate xylem capacity value
{
	double	solution;
	double d=1;
	solution=(ThetaH(i, T->node[i].H_total+d,T)-ThetaH(i, T->node[i].H_total-d,T))/(double)2*d;
	return solution;
}

 

double mualem(int i, double h)//interpolates soil conductivity
{
	double sol=0;
	sol=inter(hM[i],h);
	return (sol);
	//return 0;
}
*/
void XWF_CLASS::set_Vector_mualem()//generate matrix containing mualem conductivities for each soil layer
{
	int i=0;
	int j=0;
	
	int layers;
	layers = xpn->pSo->iLayers - 2;
	
	this->hM =(double**) malloc(layers*sizeof(double*));
	for(i=0;i<layers;i++)
		this->hM[i]=(double*) malloc(this->numbers*sizeof(double));
	for(j=0;j<layers;j++)
	{
		for(i=0;i<this->numbers;i++)
			this->hM[j][i] = this->mualem_sub(j,(double) -i/ this->step);
	}
}

double XWF_CLASS::mualem_sub(int i,double h)//conductivity from soil matric head; mm/s
{
	double solution=0;
	//float p=0.5,q=2,r=1;
	double p=0.5,q=1,r=2;
	double a=this->boden[i].alpha;//1/mm

	if(h>=0)
		solution=this->boden[i].Ks;
	else
		solution=this->boden[i].Ks*
		pow(pow((double)1+pow((double)a*fabs((double)h),(double)this->boden[i].n),(double)q/(double)this->boden[i].n-(double)1),(double)p)*
		pow((double)1-pow((double)a*fabs((double)h),(double)this->boden[i].n-q)*
		pow((double)1+pow((double)a*fabs((double)h),(double)this->boden[i].n),(double)q/(double)this->boden[i].n-(double)1),(double)r);

	return fabs(solution);
}
//input volumetric water content
//output h
double gethfromWC(double WC, int i,class tree *T)
{
	 
	double e = T->porosity;
	if(WC<T->Theta_aev)
		return T->BC_a *pow(WC/T->Theta_aev,-1.0/T->BC_Lambda);
	else
		return T->BC_a * (1.0 - (WC-T->Theta_aev)/(e-T->Theta_aev) );
}