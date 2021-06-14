#ifndef SOILS_H_
#define SOILS_H_

struct soils
{
	double	Theta_max;
	double	Theta_r;
	double	Theta_aev;
	double	alpha;
	double	n;
	double	m;
	double	Ks;
	double	dz;
	struct soils* pchild;
	double	BC_a;
	double	BC_Lamba;
};

#endif //SOILS_H_