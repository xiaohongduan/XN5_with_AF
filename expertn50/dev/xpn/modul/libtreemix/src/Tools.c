#include "libtreemix.h"

int IsSimStart(libtreemix *self)
{
	expertn_modul_base *xpn = &(self->parent);
	
	if(xpn->pTi->pSimTime->fTimeY == (float)0.0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
	return RET_SUCCESS;
}

double min3(double x1,double x2,double  x3)
{
  double y;
  if(x1<x2 && x1<x3)
    y=x1;
  else if (x2<x3)
    y=x2;
  else y=x3;

  return y;
};

double max3(double x1,double x2,double x3)
{
  double y;
  if(x1>x2 && x1>x3)
    y=x1;
  else if (x2>x3)
    y=x2;
  else y=x3;

  return y;
};
