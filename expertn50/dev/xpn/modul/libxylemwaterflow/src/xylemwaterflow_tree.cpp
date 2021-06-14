
#include "xylemwaterflow.h"


double tree::getWaterContent(double height) //height in [mm]
{
	double WC = 0.0; //liter
	for(int i=0; i< this->iCnodes+this->iRnodes;i++)
	{
		if(this->node[i].Z > height)
		{
			WC += this->node[i].l*1e-2 * this->node[i].XylemArea *1e-4 //volume dm^3
				 * this->node[i].WC_Xylem;
		}
	}
	return WC;//liter
}
/*
double tree::getFineRootArea_mm(double maxd)
{
	double RootArea=0.0; //m^2
	for(int i=iCnodes; i< iCnodes+iRnodes;i++)
	{
		if(node[i].TotalDiameter < maxd)
		{
			RootArea += node[i].l * pow(0.5*node[i].TotalDiameter,2.0)*PI;
		}
	}
	return RootArea * 1e-6; //m^2
}

double tree::getFineRootArea_sub(int minsum)
{
	double RootArea=0.0; //m^2
	for(int i=iCnodes; i< iCnodes+iRnodes;i++)
	{
		if(node[i].sub >= minsum)
		{
			RootArea += node[i].l * pow(0.5*node[i].TotalDiameter,2.0)*PI;
		}
	}
	return RootArea * 1e-6; //m^2
}

  int gloi, gloii;          
double nodes::collectAdjacentLength(double *sum, class tree *T)
{
	int ii= child->size();
	gloii=ii;
	if(ii>0)
	{
	for(int i=0;i<ii;i++)
	{	
		gloi=i;
		*sum+= T->node[child->at(i)].l;
		T->node[child->at(i)].collectAdjacentLength(sum,T);
	}	 
	}
	return *sum;
}*/

int tree::checkTree()
{
	//check variables of the tree structure
	if (this->iCnodes < 0 || this->iCnodes > 100000)			return 1; 
	if (this->iRnodes < 0 || this->iRnodes > 100000)			return 2; 
	if (this->maxchilds < 0 || this->maxchilds > 50)			return 3; 
	if (this->max_XylemArea_root < 0 || this->max_XylemArea_root > 25e6)	return 4;
	if (this->max_XylemArea_stem < 0 || this->max_XylemArea_stem > 25e6)	return 4;
	if (this->Theta_aev < 0 || this->Theta_aev >1)				
	{
		return 5;
	}

	//check variables of the nodes structure
	for(int i=0; i<this->iCnodes+this->iRnodes; i++)
	{
		if (this->node[i].LA < 0 || this->node[i].LA > 1.0)							
		{
			return 6; 
		}
		if (this->node[i].cosine < -1.0001 || this->node[i].cosine > 1.0001)				return 7; 
		if (this->node[i].vz_delZ != -1 && this->node[i].vz_delZ != 1)				return 8; 
		if (this->node[i].canopy && this->node[i].Z <0.0)							return 9; 
		if (this->node[i].H_new_total > 0 || this->node[i].H_new_total < -1e6)		return 10; 
		if (this->node[i].H_total_old > 0 || this->node[i].H_total_old < -1e6)		return 11; 
		if (this->node[i].H_total > 0 || this->node[i].H_total < -1e6)
		{
			return 12; 
		}
		if (this->node[i].WC_Xylem_old < 0 || this->node[i].WC_Xylem_old > porosity)return 13; 
		if (this->node[i].WC_Xylem < 0 || this->node[i].WC_Xylem > porosity)		
		{
			return 14;
		}
		if (this->node[i].K < 0 )
		{
			return 15; 
		}
		if (this->node[i].Kirchhoff < 0 || this->node[i].LA > 1.0)					return 16; 
		if (this->node[i].l < 0 || this->node[i].l > 50e3)							return 17; 
		if (this->node[i].g < -1.0 || this->node[i].g > 1.0)						return 18; 
	}
 
	return 0;
}
 