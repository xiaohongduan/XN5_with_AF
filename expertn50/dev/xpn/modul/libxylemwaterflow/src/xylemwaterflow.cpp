#include "xylemwaterflow.h"

//using namespace std;

int XWF_CLASS::load()
{		
	PWLAYER pWL;
	
	char *outdoutputfile;
    char *tree_elements_outputfile;
	// Read from INI File:
	this->cfg_filename = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,(char*)"Config.xylemwaterflow.cfg_filename");

	if (this->cfg_filename==NULL) {
		PRINT_ERROR("Missing entry 'xylemwaterflow.filename' in your configuration!");
	}
	else{

			char *S2;
			S2 = expertn_modul_base_replace_std_templates(xpn,this->cfg_filename);
			if (S2!=NULL) {
				this->cfg_filename = g_strdup_printf("%s",S2);
				free(S2);
			}
	}

	// New input routine which loads config-parameters, general plant parameters and plant-specific parameters
	this->loadconfig();
	
	g_free(this->cfg_filename);
	
	this->fill_soil_properties();
	
	this->UptLayer = (double*)g_malloc0_n(xpn->pSo->iLayers-2,sizeof(double));
	this->Uptake_Janott = (double*)g_malloc0_n(xpn->pSo->iLayers-2,sizeof(double));
	
	this->load_architecture();

	this->XylemArea(this->T_m); //sets node[].XylemArea (mm^2) and node[].XylemArea_norm (0..1)

    this->totalDiameter(this->T_m); //calculates node[].totalDiameter [mm]

			//calculate conversion factor
		//SoilToXylemArea links soil area related values to xylem area, because we compute fluxes based on 1 m² xylem area
	double tempor=1.0/this->T_m->CrownProjArea;//number of trees on 1 m² soil surface area;
	double temporf= this->T_m->max_XylemArea_stem * 1e-6;//cross sectional stem area per tree [m^2]
	this->SoilToXylemArea = tempor*temporf;/*m² of stem cross sectional area on 1 m² soil area*/
	this->SoilToXylemArea = 1/this->SoilToXylemArea;
	//generate Leaf Area Distribution
	genLA(this->T_m);//T->node[].LA is normalized, sum=1.0
    this->T_m->node[0].LA=0;
    
	//root surface area soil
	this->RootSurfADistrSoil(this->T_m);
	this->RootPartitioningToSoilLayers(this->T_m);
	
	this->step = 0.03;
	this->numbers = (int)fabs((1000000.0*(double)step));
	 	
	//prepare hydraulic functions
	this->set_Vector_Theta(this->T_m);
	this->set_Vector_Cond(this->T_m);
	
	this->set_Vector_mualem();
	
	this->eta = 2.0/this->T_m->BC_Lambda + this->conf.contemp;
	//init potentials
	for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)
	{
		this->T_m->node[i].H_total_old = 0.0;
		//this->T_m->node[i].H_total_old=-5000 - (this->T_m->node[i].Z); 
		this->T_m->node[i].H_total_old =  xpn->pWa->pWLayer->pNext->fMatPotAct - this->T_m->node[i].Z ;
		//this->T_m->node[i].H_total_old = -150000;
		this->T_m->node[i].H_total_old=min(this->T_m->node[i].H_total_old,-100);//no pos values
	}
	
	for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)
	{	
		this->T_m->node[i].H_total = this->T_m->node[i].H_new_total = this->T_m->node[i].H_total_old;
		this->T_m->node[i].Kirchhoff = XWF_CLASS::getKirchhoff(this->T_m->node[i].H_total, i, this->T_m);
		this->T_m->node[i].WC_Xylem_old=XWF_CLASS::ThetaH(i, this->T_m->node[i].H_total,this->T_m);
		this->T_m->node[i].WC_Xylem=XWF_CLASS::ThetaH(i, this->T_m->node[i].H_total,this->T_m);
		this->T_m->node[i].l = abstand(this->T_m->node[i],this->T_m->node[this->T_m->node[i].parent]);
         // FH 02/17: when parent and child are equal the solver does not converge, therefore introduce a very small distance between the nodes
        if  (this->T_m->node[i].l  == 0.0)
                    this->T_m->node[i].l = 1.0;
        this->T_m->node[i].BoundaryFlux = 0.0;
		this->T_m->node[i].Transpiration = 0.0;
        this->T_m->node[i].Transpiration_pot = 0.0;
        this->T_m->node[i].Photosynthesis = 0.0;
        this->T_m->node[i].Photosynthesis_pot = 0.0;
	}

	//Special case: root/branch interface
	this->T_m->node[0].l *= 0.5;
	this->T_m->node[this->T_m->iCnodes].l*=0.5;
	
	//Write output for same compartment than tlp file
	outdoutputfile = g_strdup_printf("%s/%s_%s_%s.outd",this->OutputPath,xpn->pXSys->project_name,xpn->pXSys->reg_str,this->OutputCompartment);
	this->write_second_outd_file = 0;
	if((this->tlp_output == 0) || (this->tlp_output == 1) || (this->tlp_output == 2))
    {
		writeOUTD(*this->T_m, outdoutputfile, this->tlp_output);
        this->write_second_outd_file = 1;
    }
	else
    {
		PRINT_MESSAGE(xpn,3,"No outd output");
        this->write_second_outd_file = 0;
    }
	g_free(outdoutputfile);
	
    
    
	//SB??
	//FH: Set pWa to first layer?
	pWL=xpn->pWa->pWLayer/*->pNext->pBack*/;
	while(pWL->pBack!=NULL)
		pWL=pWL->pBack;
	pWL=pWL->pNext;
	
	//SLU preparations
	countNonZeros(this->T_m);
	this->T_m->SLU_rhs =(double*)	realloc(this->T_m->SLU_rhs,(this->T_m->iCnodes+this->T_m->iRnodes)*sizeof( double));
	this->gameover=0;
	
	//Allocate memory
	int n = this->T_m->iCnodes + this->T_m->iRnodes;
	this->T_m->MatrixA= new double[n];
	this->T_m->rhs	= new double[n];
	for(int i=0;i<n;i++)
	{
		this->T_m->rhs[i] = this->T_m->MatrixA[i] =0;
		//rhs[i]		= new double;
		//rhs[i][0] =0;
		//MatrixA[i]	= new double[n];
		//for(int j=0;j<n;j++)
		//MatrixA[i][j]=0;		
	}

	//max water content
	double WC = 0.0; //liter
	for(int i=0; i< this->T_m->iCnodes+this->T_m->iRnodes;i++)
	{
		WC += this->T_m->node[i].l*1e-2 * this->T_m->node[i].XylemArea *1e-4*this->T_m->porosity; //liter		
	}
	this->T_m->maxWaterContentCanopy = WC;
	
		
	//print root diameter distribution
	/*
	ofstream of("rootdiameter.dat");
	for(int i=T->iCnodes; i< T->iCnodes+T->iRnodes;i++)
	{	
		of << T->node[i].sub << " " << T->node[i].TotalDiameter << "\n";
	}
	of.close();*/
	
	//get node nr. at dbh
//get tree sections for different height
		for(int i=0; i< this->T_m->iCnodes;i++)
		{	
			if(this->T_m->node[i].Z >1300)	//  < 1300
			{
				this->T_m->inodeDBH =i;
				break;
			}
		}
		for(int i=0; i< this->T_m->iCnodes;i++)
		{	
			if(this->T_m->node[i].Z > 0)
			{
				this->T_m->inodeGround =i;
				break;
			}
		}
		for(int i=0; i< this->T_m->iCnodes;i++)
		{	
			if(this->T_m->node[i].Z > 5000)
			{
				this->T_m->inode5m =i;
				break;
			}
		}
		for(int i=0; i< this->T_m->iCnodes;i++)
		{	
			if(this->T_m->node[i].Z > 10000)
			{
				this->T_m->inode10m =i;
				break;
			}
		}
		for(int i=0; i< this->T_m->iCnodes;i++)
		{	
			if(this->T_m->node[i].Z > 15000)
			{
				this->T_m->inode15m =i;
				break;
			}
		}	
		for(int i=this->T_m->iCnodes; i< this->T_m->iCnodes + this->T_m->iRnodes;i++)
		{	
			if(this->T_m->node[i].Z < -100)
			{
				this->T_m->inode10cmS =i;
				break;
			}
		}		
		for(int i=this->T_m->iCnodes; i< this->T_m->iCnodes + this->T_m->iRnodes;i++)
		{	
			if(this->T_m->node[i].Z < -300)
			{
				this->T_m->inode30cmS =i;
				break;
			}
		}		
		for(int i=this->T_m->iCnodes; i< this->T_m->iCnodes + this->T_m->iRnodes;i++)
		{	
			if(this->T_m->node[i].Z < -490)
			{
				this->T_m->inode50cmS =i;
				break;
			}
		}
	
 
//Write output tree elements
	tree_elements_outputfile = g_strdup_printf("%s/%s_%s_tree_elements.dat",this->OutputPath,xpn->pXSys->project_name,xpn->pXSys->reg_str);
   std::ofstream of(tree_elements_outputfile);
   of<<"j;parent;X;Y;Z;TotalDiameter;l;XylemArea;LA;width;Strahler\n";
   for(int j=0; j<this->T_m->iCnodes+this->T_m->iRnodes; j++)
        {
			of<<  j
				<< " ;" << this->T_m->node[j].parent
				<< " ;" << this->T_m->node[j].X
				<< " ;" << this->T_m->node[j].Y				
				<< " ;" << this->T_m->node[j].Z 
				<< " ;" << this->T_m->node[j].TotalDiameter
				<< " ;" << this->T_m->node[j].l
				<< " ;" << this->T_m->node[j].XylemArea
				<< " ;" << this->T_m->node[j].LA
                << " ;" << this->T_m->node[j].width
                << " ;" << this->T_m->node[j].Strahler
				<< "\n"; 
	}
    of.close();
   g_free(tree_elements_outputfile); 
    
	this->uptake_d = (double*)g_malloc0_n(1,sizeof(double));
	this->flow_dbh_d = (double*)g_malloc0_n(1,sizeof(double));
	this->sum_d = (double*)g_malloc0_n(1,sizeof(double));
	this->transpiration_d = (double*)g_malloc0_n(1,sizeof(double));
	this->transpiration_pot_d = (double*)g_malloc0_n(1,sizeof(double));
	this->photosynthesis_d = (double*)g_malloc0_n(1,sizeof(double));
	this->photosynthesis_pot_d = (double*)g_malloc0_n(1,sizeof(double));
	this->waterStorage_d = (double*)g_malloc0_n(1,sizeof(double));
	this->min_pot_stem = (double*)g_malloc0_n(1,sizeof(double));
	this->max_pot_stem = (double*)g_malloc0_n(1,sizeof(double));
	this->uptake_h = (double*)g_malloc0_n(1,sizeof(double));
	this->flow_dbh_h = (double*)g_malloc0_n(1,sizeof(double));
	this->sum_h = (double*)g_malloc0_n(1,sizeof(double));
	this->transpiration_h = (double*)g_malloc0_n(1,sizeof(double));
	this->transpiration_pot_h = (double*)g_malloc0_n(1,sizeof(double));
	this->photosynthesis_h = (double*)g_malloc0_n(1,sizeof(double));
	this->photosynthesis_pot_h = (double*)g_malloc0_n(1,sizeof(double));
	this->waterStorage_h = (double*)g_malloc0_n(1,sizeof(double));
	this->pot_stem = (double*)g_malloc0_n(1,sizeof(double));

	//Output
	for(int i=0; i<1; i++)
	{
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->flow_dbh_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.flow_dbh_d",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->sum_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.sum",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->uptake_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.uptake",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->transpiration_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.transpiration",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->transpiration_pot_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.transpiration_pot",i+1),0.0);        
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->photosynthesis_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.photosynthesis",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->photosynthesis_pot_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.photosynthesis_pot",i+1),0.0);        
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->waterStorage_d[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.water_storage",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->min_pot_stem[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.min_pot_stem",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->max_pot_stem[i],g_strdup_printf("output.Water.Xylemwaterflow_daily_tree_%d.max_pot_stem",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->flow_dbh_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.flow_dbh_h",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->sum_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.sum",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->uptake_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.uptake",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->transpiration_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.transpiration",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->transpiration_pot_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.transpiration_pot",i+1),0.0);        
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->photosynthesis_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.photosynthesis",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->photosynthesis_pot_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.photosynthesis_pot",i+1),0.0);   		
        xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->waterStorage_h[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.water_storage",i+1),0.0);
		xpn_register_var_init_pdouble(xpn->pXSys->var_list,this->pot_stem[i],g_strdup_printf("output.Water.Xylemwaterflow_hourly_tree_%d.pot_stem",i+1),0.0);
	}

	return RET_SUCCESS;
};


int XWF_CLASS::run()
{
	PWLAYER		pWL;
    PSLAYER     pSL;
	PWATER		pWa = xpn->pWa;
	PPLANT		pPl = xpn->pPl;
    
	pPl->pRoot->fUptakeR = 0.0;
	pPl->pPltWater->fActTranspR = 0.0;
	pPl->pPltWater->fActTranspDay = 0.0;
	
	for(int iLayer=0;iLayer<xpn->pSo->iLayers-2;iLayer++)
		this->Uptake_Janott[iLayer] = 0.0;
	
	this->Sink = 0.0;
	this->cpa = 0.0;
	this->cpatotal = 0.0;
	
	if(strcmp(this->conf.solver,"r")==0)
	{
		this->flux_Ross();
		this->cpatotal += this->cpa;
	}
	
	for(int i=0; i<1; i++)
		{
			this->B_m[i].flow_dbh_d = this->B_m[i].waterStorage_d-this->B_m[i].waterStorage_old_d + this->B_m[i].transpiration_d;		
			this->B_m[i].sum_d = this->B_m[i].waterStorage_d - this->B_m[i].waterStorage_old_d + this->B_m[i].transpiration_d + this->B_m[i].uptake_d ;		
			this->B_m[i].flow_dbh_h = this->B_m[i].waterStorage_h-this->B_m[i].waterStorage_old_h + this->B_m[i].transpiration_h;		
			this->B_m[i].sum_h = this->B_m[i].waterStorage_h - this->B_m[i].waterStorage_old_h + this->B_m[i].transpiration_h + this->B_m[i].uptake_h ;		
		}
	
	for(int i=0; i<1; i++)
		{
		if(NewDay(xpn->pTi))
			{
			this->uptake_d[i] = -this->B_m[i].uptake_d;
			this->flow_dbh_d[i] = this->B_m[i].flow_dbh_d;
			this->sum_d[i] = this->B_m[i].sum_d;
			this->transpiration_d[i] = this->B_m[i].transpiration_d;
			this->transpiration_pot_d[i] = this->B_m[i].transpiration_pot_d;
			this->photosynthesis_d[i] = this->B_m[i].photosynthesis_d;
			this->photosynthesis_pot_d[i] = this->B_m[i].photosynthesis_pot_d;
			this->waterStorage_d[i] = this->B_m[i].waterStorage_d;
			pPl->pPltWater->fActTranspDay += this->transpiration_d[i]/xpn->pTi->pTimeStep->fAct;
			this->min_pot_stem[i] = this->B_m[i].min_pot_stem;
			this->max_pot_stem[i] = this->B_m[i].max_pot_stem;
			this->B_m[i].reset_d();
            
            	//Write output for same compartment than tlp file
                        
            
            
			}
            
            if (xpn->pTi->pSimTime->fTimeDay >= 0.5 && this->write_second_outd_file == 1)
            {
                char *outdoutputfile;
                outdoutputfile = g_strdup_printf("%s/%s_%s_%s_midday.outd",this->OutputPath,xpn->pXSys->project_name,xpn->pXSys->reg_str,this->OutputCompartment);
	
                writeOUTD(*this->T_m, outdoutputfile, this->tlp_output);

                g_free(outdoutputfile);
                
                this->write_second_outd_file = 0;
            }    
        
		pPl->pPltWater->fActTranspR += this->B_m[i].transpiration_h/xpn->pTi->pTimeStep->fAct;
		pPl->pRoot->fUptakeR -= this->uptake_h[i]/xpn->pTi->pTimeStep->fAct;
		this->uptake_h[i] = -this->B_m[i].uptake_h;
		this->flow_dbh_h[i] = this->B_m[i].flow_dbh_h;
		this->sum_h[i] = this->B_m[i].sum_h;
		this->transpiration_h[i] = this->B_m[i].transpiration_h;
		this->transpiration_pot_h[i] = this->B_m[i].transpiration_pot_h;
		this->photosynthesis_h[i] = this->B_m[i].photosynthesis_h;
		this->photosynthesis_pot_h[i] = this->B_m[i].photosynthesis_pot_h;
		this->waterStorage_h[i] = this->B_m[i].waterStorage_h;
		this->pot_stem[i] = this->B_m[i].pot_stem;
		this->B_m[i].reset_h();
		}
		
	pPl->pPltWater->fActTranspdt = pPl->pPltWater->fActTranspR * xpn->pTi->pTimeStep->fAct;	
	
	pWL=pWa->pWLayer->pNext;
    pSL = xpn->pSo->pSLayer->pNext;
	for(int iLayer=0;iLayer<xpn->pSo->iLayers-2;iLayer++,pWL=pWL->pNext,pSL = pSL->pNext)
	{		  
		this->Sink=this->Uptake_Janott[iLayer]/((double)(this->T_m[0].CrownProjArea*1e6));
		//printf("%f %f %f \n", this->Sink, this->Uptake_Janott[iLayer], this->T_m[0].CrownProjArea);
		pWL->fContAct       -= this->Sink /(pSL->fThickness);
		//pWL->fContAct       -= this->Sink  /(xpn->pSo->pSLayer->fThickness);
	
	}
	
	return RET_SUCCESS;
};


int XWF_CLASS::done()
{
	g_free(this->UptLayer);
	g_free(this->Uptake_Janott);
	
	for(int i=0;i<this->T_m->iCnodes+this->T_m->iRnodes;i++)   
		{
		this->T_m->node[i].child.~vector(); // call deconstructor of vector<int> child due to windows problems on exiting
		g_free(this->T_m->node[i].frac);
		}
	g_free(this->T_m->node);		
		
	free(this->depth);
	
	free(this->hT);
	free(this->hK);
	for(int i=0; i<xpn->pSo->iLayers - 2; i++)
		free(this->hM[i]);
	free(this->hM);

	free(this->T_m->SLU_rhs);

	delete [] this->T_m->MatrixA;
	delete [] this->T_m->rhs;
	
	g_free(this->uptake_d);
	g_free(this->flow_dbh_d);
	g_free(this->sum_d);
	g_free(this->transpiration_d);
	g_free(this->waterStorage_d);
	g_free(this->max_pot_stem);
	g_free(this->min_pot_stem);
	g_free(this->uptake_h);
	g_free(this->flow_dbh_h);
	g_free(this->sum_h);
	g_free(this->transpiration_h);
	g_free(this->waterStorage_h);
	g_free(this->pot_stem);

	g_free(this->OutputPath);
	g_free(this->OutputCompartment);
	
	g_free(this->boden);
	
	g_free(this->T_m->canopy);
	g_free(this->T_m->root);
	//free(this->T_m);
	
	return RET_SUCCESS;
};

