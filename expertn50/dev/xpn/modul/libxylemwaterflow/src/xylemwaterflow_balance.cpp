#include "xylemwaterflow.h"
void xylemwaterflow_balance::reset_d()
{
	uptake_d=0;
	transpiration_d=0;
	transpiration_pot_d=0;
	photosynthesis_d=0;
	photosynthesis_pot_d=0;
	waterStorage_old_d = waterStorage_d;
	waterStorage_d=0;
	flow_dbh_d=0;
	min_pot_stem=0;
	max_pot_stem=-1e6;
}

void xylemwaterflow_balance::reset_h()
{
	uptake_h=0;
	transpiration_h=0;
	transpiration_pot_h=0;
	photosynthesis_h=0;
	photosynthesis_pot_h=0;
	waterStorage_old_h = waterStorage_h;
	waterStorage_h=0;
	flow_dbh_h=0;
	pot_stem=0;	
}