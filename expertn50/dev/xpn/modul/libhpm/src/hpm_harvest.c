#include "hpm_harvest.h"
#include "hpm_math.h"
#include "hpm_ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



int hpm_harvest_init(hpm *self)
{
	init_hpm_model(self);
	hpm_harvest_register_var(self);
	self->Harvest.save_date.mday=0;
	self->Harvest.save_date.mon=0;
	self->Harvest.save_date.year=0;
	self->Harvest.harvest_dates=NULL;
	self->Harvest.active=1;
	self->Harvest.Pulseharv=0;
	load_harvest_config(self,self->ini_filename);
	return RET_SUCCESS;
}

int hpm_harvest_run(hpm *self)
{
	expertn_modul_base *xpn = &(self->parent);
	PTIME pTi = xpn->pTi;
	double chcanLAI =  self->parameter.water.chcanLAI;//0.026;  // m [(m2 leaf)/(m2 ground)]-1. Canopy height to leaf area index. LAI of 1 has a height of 2.6 cm. See eq 2 of Parson, Thornley, Newman and Penning. Functional Ecology, vol 8, 187-204, 1994.
	int i;
	double  harvm;
	double hcan; // conopy height
	double LAI;
	char *S;
	int cmp;
	double dt;

	cmp=0;
	self->Harvest.Pulseharv = 0.0;
	dt = pTi->pTimeStep->fAct;
	if (self->Harvest.harvest_act_date != NULL) {
		cmp = xpn_time_compare_date(pTi->pSimTime->year,pTi->pSimTime->mon,pTi->pSimTime->mday,self->Harvest.harvest_act_date->date.year,self->Harvest.harvest_act_date->date.mon,self->Harvest.harvest_act_date->date.mday);
		if (cmp==0) {
			if (!((self->Harvest.save_date.mday == pTi->pSimTime->mday) && (self->Harvest.save_date.mon == pTi->pSimTime->mon) && (self->Harvest.save_date.year == pTi->pSimTime->year))) {

				self->Harvest.save_date.mday = pTi->pSimTime->mday;
				self->Harvest.save_date.mon = pTi->pSimTime->mon;
				self->Harvest.save_date.year = pTi->pSimTime->year;
				S = g_strdup_printf("%d.%d.%d: Harvesting!",self->Harvest.save_date.mday,self->Harvest.save_date.mon,self->Harvest.save_date.year);
				PRINT_MESSAGE(xpn,3,S);
				g_free(S);
			}

			self->Harvest.Pulseharv = 1.0;
			LAI = hpm_math_sum4(self->Plant.LAI4);
			hcan = chcanLAI * LAI; // Canopy height: proportional to leaf area index;
			// fraction of the above-ground dray mass harvest: (hharv: height of harving)
			if (hcan < self->Harvest.harvest_act_date->hharv) {
				self->Harvest.fharv = 0.0;
			} else {
				self->Harvest.fharv = (hcan - self->Harvest.harvest_act_date->hharv)/ hcan;
				self->Harvest.fharv = 0.5 * ( self->Harvest.fharv + fabs ( self->Harvest.fharv) );
			}
			harvm = self->Harvest.Pulseharv / dt;
			for (i=0; i!=4; i++) {
				self->Harvest.OXLam_hv4[i] = harvm * self->Harvest.fharv * self->Plant.MLam4[i];
				self->Harvest.OLAI_hv4[i]  = harvm * self->Harvest.fharv * self->Plant.LAI4[i];
				self->Harvest.OXss_hv4[i]  = harvm * self->Harvest.fharv * self->Plant.Mss4[i];
				self->Harvest.Sum_Lam_hv4[i] += self->Harvest.OXLam_hv4[i]*dt;
				self->Harvest.Sum_ss_hv4[i] += self->Harvest.OXss_hv4[i]*dt;
			}
			self->Harvest.OCSsh_hv = harvm * self->Harvest.fharv * self->Plant.MCSsh;
			self->Harvest.ONSsh_hv = harvm * self->Harvest.fharv * self->Plant.MNSsh;
			self->Harvest.ONph_hv = harvm * self->Harvest.fharv * self->Plant.MNph_ul;
			self->Harvest.OWsh_hv = harvm * self->Harvest.fharv * self->Water.Wsh;
			self->Harvest.Sum_CSsh_hv += self->Harvest.OCSsh_hv * dt;
			self->Harvest.Sum_NSsh_hv += self->Harvest.ONSsh_hv * dt;


		}





		if (cmp > 0) {
			self->Harvest.harvest_act_date = self->Harvest.harvest_act_date->pNext;
			//self->Harvest.Sum_CSsh_hv=0.0;
			//self->Harvest.Sum_NSsh_hv=0.0;
			self->Harvest.OCSsh_hv = 0.0;
			self->Harvest.ONSsh_hv = 0.0;
			self->Harvest.ONph_hv = 0.0;
			for (i=0; i!=4; i++) {
				//self->Harvest.Sum_Lam_hv4[i] = 0.0;
				//self->Harvest.Sum_ss_hv4[i] = 0.0;
				self->Harvest.OXLam_hv4[i] = 0.0;
				self->Harvest.OLAI_hv4[i]  = 0.0;
				self->Harvest.OXss_hv4[i]  = 0.0;
			}
		}
	}

	return RET_SUCCESS;
}

int hpm_harvest_done(hpm *self)
{
	// self->Harvest.harvest_dates should be free'd
	done_hpm_model(self);
	return RET_SUCCESS;
}


int hpm_harvest_register_var(hpm *self)
{
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.Sum_CSsh_hv,"output.hpm.harvest substrate cum day.CSsh harvest [kg C m-2]",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.Sum_NSsh_hv,"output.hpm.harvest substrate cum day.NSsh harvest [kg N m-2]",0.0);

	xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.Sum_Lam_hv4[i],varname,0.0),"output.hpm.harvest structural cum.lam harvest [kg N m-2]",4);
	xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.Sum_ss_hv4[i],varname,0.0),"output.hpm.harvest structural cum.ss harvest [kg N m-2]",4);
	
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Output.cum_harvest_mass,"output.hpm.harvest cum mass.mass [kg m-2]",0.0);
	

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.Pulseharv,"hpm.harvest.Pulseharv",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.fharv,"hpm.harvest.fharv",0.0);	

	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.OCSsh_hv,"hpm.harvest.OCSsh_hv",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.ONSsh_hv,"hpm.harvest.ONSsh_hv",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.ONph_hv,"hpm.harvest.ONph_hv",0.0);
	xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.OWsh_hv,"hpm.harvest.OWsh_hv",0.0);
	
	

	
	
	xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.OLAI_hv4[i],varname,0.0),"hpm.harvest.OLAI_hv4",4);
	xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.OXss_hv4[i],varname,0.0),"hpm.harvest.OXss_hv4",4);
	xpn_register_var_init_array(xpn_register_var_init_pdouble(self->parent.pXSys->var_list,self->Harvest.OXLam_hv4[i],varname,0.0),"hpm.harvest.OXLam_hv4",4);

	return RET_SUCCESS;
}

int load_harvest_config(hpm *self,const char* configfile)
{
	expertn_modul_base *xpn = &(self->parent);
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	char **harvest_dates_str;
	double *harvest_heights;
	char *S;
	int harvest_dates_len,harvest_height_len;
	int i;
	PSHARVEST_DATE harvest_date,harvest_date_old;
	const char* filename=configfile;

	/* Create a new GKeyFile object and a bitwise list of flags. */
	keyfile = g_key_file_new ();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	/* Load the GKeyFile from keyfile.conf or return. */
	if (!g_key_file_load_from_file (keyfile, configfile, flags, &error)) {
		PRINT_ERROR(error->message);
		return -1;
	}

	GET_INI_STRING_ARRAY(harvest_dates_str,harvest_dates_len,"Harvest","harvest_dates");
	GET_INI_DOUBLE_ARRAY(harvest_heights,harvest_height_len,"Harvest","harvest_heights");
	
	if (harvest_dates_len!=harvest_height_len)
		{
			S = g_strdup_printf("HPM: in INI File: harvest_dates list len and  harvest_heights list len are not equal! %d,%d",harvest_dates_len,harvest_height_len);
			PRINT_ERROR_ID(xpn,S);
			g_free(S);
			return 1;
		}

	//harvest_date=self->Harvest.harvest_dates;
	harvest_date_old = NULL;
	for (i=0; i<harvest_dates_len; i++) {
		harvest_date = g_malloc0(sizeof(SHARVEST_DATE));
		convert_str_to_xpn_date(&(harvest_date->date),harvest_dates_str[i]);
		harvest_date->hharv = harvest_heights[i];

		if (i==0) {
			self->Harvest.harvest_dates = harvest_date;
			harvest_date_old = harvest_date;
		} else {
			if (strcmp(harvest_dates_str[i],harvest_dates_str[i-1]) <= 0) {
				S = g_strdup_printf("HPM: Problem with Harvest Dates: %s < %s",harvest_dates_str[i],harvest_dates_str[i-1]);
				PRINT_ERROR_ID(xpn,S);
				g_free(S);
				continue;
			}
			harvest_date_old->pNext = harvest_date;
			harvest_date->pBack = harvest_date_old;
			harvest_date_old=harvest_date;
		}
	}
	self->Harvest.harvest_act_date = self->Harvest.harvest_dates; // to first date or NULL

	/*for (harvest_date=self->Harvest.harvest_dates;harvest_date!=NULL;harvest_date=harvest_date->pNext)
		{
			printf("%d.%d.%d\n",harvest_date->date.mday,harvest_date->date.mon,harvest_date->date.year);
		}*/

	g_strfreev(harvest_dates_str);
	g_free(harvest_heights);

	g_key_file_free(keyfile);

	return RET_SUCCESS;
}
