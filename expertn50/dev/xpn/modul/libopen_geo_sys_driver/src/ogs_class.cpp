#include "ogs_class.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>


#include <expertn_modul_base.h>

int OGS_CLASS::load()
{
	expertn_modul_base *xpn = (expertn_modul_base *)this->xpn;
	char *ini_filename;
	this->param=NULL;
	this->ogs_load = false;
	this->aproblem=NULL;
	if (xpn->pXSys->id==0)
	{
	ini_filename = (char*)xpn_register_var_get_pointer(xpn->pXSys->var_list,"Global_Config.options.open_geo_sys_driver_param");

		if (ini_filename==NULL) {
			PRINT_ERROR("Missing entry 'WRF Coupling.cdb filename' in your configuration!");
			return 1;
		} else {

			char *S2;
			S2 = expertn_modul_base_replace_std_templates(xpn,ini_filename);
			if (S2!=NULL) {
				this->param = g_strdup_printf("%s",S2);
				//free(ini_filename);
				//free(S2);
			}

		}
	
	/* parse command line arguments */
	std::string modelRoot;


	/*if( argc > 1 && modelRoot == "" ) // non-interactive mode and no model given
		exit(0);             // e.g. just wanted the build info
*/
	dateiname=NULL;
#ifdef SUPERCOMPUTER
// *********************************************************************
// buffered output ... important for performance on cray
// (unbuffered output is limited to 10 bytes per second)
// georg.kosakowski@psi.ch 11.10.2007

	char buf[1024 * 1024];
	int bsize;

	bsize = 1024 * 1024; // question: what happens if buffer is full?
	                     // according to documentation the buffer is flushed when full.
	                     // If we have a lot of output, increasing buffer is usefull.
	if(bsize > 0)
//        bufstd = malloc(bsize);
		setvbuf(stdout, buf, _IOFBF, bsize);
	//**********************************************************************
#endif
/*---------- MPI Initialization ----------------------------------*/
#if defined(USE_MPI) || defined(USE_MPI_PARPROC) || defined(USE_MPI_REGSOIL) || \
	defined(USE_MPI_GEMS)
	printf("Before MPI_Init\n");
#if defined(USE_MPI_GEMS)
	int prov;
	MPI_Init_thread(&argc,&argv,MPI_THREAD_FUNNELED, &prov);
#else
	MPI_Init(&argc,&argv);
#endif
	MPI_Barrier (MPI_COMM_WORLD); // 12.09.2007 WW
	elapsed_time_mpi = -MPI_Wtime(); // 12.09.2007 WW
	MPI_Comm_size(MPI_COMM_WORLD,&mysize);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	std::cout << "After MPI_Init myrank = " << myrank << '\n';
	time_ele_paral = 0.0;
#endif
/*---------- MPI Initialization ----------------------------------*/
/*---------- LIS solver -----------------------------------------*/
#ifdef LIS
	//int argc=0;
	//char** argv = NULL;
	// Initialization of the lis solver.
//  lis_initialize(&argc, &argv);	PCH: Undoing NW modification for compilation OGS-5
#endif
/*========================================================================*/
/* Kommunikation mit Betriebssystem */
	/* Ctrl-C ausschalten */
	NoBreak();
	/* Timer fuer Gesamtzeit starten */
#ifdef TESTTIME
	TStartTimer(0);
#endif
	/* Intro ausgeben */
#if defined(USE_MPI) //WW
	if(myrank == 0)
#endif
	DisplayStartMsg();
	/* Speicherverwaltung initialisieren */
	if (!InitMemoryTest())
	{
		DisplayErrorMsg("Fehler: Speicherprotokoll kann nicht erstellt werden!");
		DisplayErrorMsg("        Programm vorzeitig beendet!");
		return 1; // LB changed from 0 to 1 because 0 is indicating success
	}

		/*if ( argc == 2 )     // a model root was supplied
		{
			dateiname = (char*) Malloc((int)strlen(argv[1]) + 1);
			dateiname = strcpy(dateiname,argv[1]);
		}
		else                // several args supplied
		if( modelRoot != "")
		{
			dateiname = (char*) Malloc( (int) modelRoot.size() + 1 );
			dateiname = strcpy( dateiname, modelRoot.c_str() );
		}*/
		dateiname = (char*) Malloc((int)strlen(this->param) + 1);
		dateiname = strcpy(dateiname,this->param);
		DisplayMsgLn(dateiname);
	
	//WW  DisplayMsgLn("");
	//WW  DisplayMsgLn("");
	// ----------23.02.2009. WW-----------------

	// LB Check if file exists
	std::string tmpFilename = dateiname;
	tmpFilename.append(".pcs");
	if(!IsFileExisting(tmpFilename))
	{
		std::cout << " Error: Cannot find file " << dateiname << std::endl;
		return 1;
	}

	FileName = dateiname;
	size_t indexChWin, indexChLinux;
	indexChWin = indexChLinux = 0;
	indexChWin = FileName.find_last_of('\\');
	indexChLinux = FileName.find_last_of('/');
	//
	if(indexChWin != std::string::npos)
		FilePath = FileName.substr(0,indexChWin) + "\\";
	else if(indexChLinux != std::string::npos)
		FilePath = FileName.substr(0,indexChLinux) + "/";
	// ---------------------------WW
	aproblem = new problem_child(dateiname);
	ogs_load=true;
	}
	return RET_SUCCESS;
}

int OGS_CLASS::run_global()
{
	expertn_modul_base *xpn = (expertn_modul_base *)this->xpn;

	if (NewDay(xpn->pTi))
	{
	//std::cout << "Test: " << xpn->pSo->pSWater->pNext->fCondSat << std::endl;
	if ((this->ogs_load==true) && (this->aproblem!=NULL))
		 {
			if (aproblem->Euler_TimeDiscretize_TimeStep()==1)
				{ 
					return 1;
				}
	}
	}
	return RET_SUCCESS;
}

int OGS_CLASS::run_mosaic()
{
	return RET_SUCCESS;
}

int OGS_CLASS::run()
{
	return RET_SUCCESS;
}

int OGS_CLASS::done()
{
	expertn_modul_base *xpn = (expertn_modul_base *)this->xpn;
	if (this->ogs_load==false)
		{
			return RET_SUCCESS;
		}
	if (xpn->pXSys->id==0)
	{
	delete aproblem;
	aproblem = NULL;
#ifdef TESTTIME
	std::cout << "Simulation time: " << TGetTimer(0) << "s" << std::endl;
#endif
	/* Abspann ausgeben */
	/* Ctrl-C wieder normal */
	StandardBreak();
/*--------- MPI Finalize ------------------*/
#if defined(USE_MPI) || defined(USE_MPI_PARPROC) || defined(USE_MPI_REGSOIL)
	elapsed_time_mpi += MPI_Wtime(); // 12.09.2007 WW
	std::cout << "\n *** Total CPU time of parallel modeling: " << elapsed_time_mpi <<
	std::endl;                                                                          //WW
	// Count CPU time of post time loop WW
	MPI_Finalize();
#endif
/*--------- MPI Finalize ------------------*/
/*--------- LIS Finalize ------------------*/
#ifdef LIS
//  lis_finalize();	//PCH: Undoing NW modification for compilation OGS-5
#endif
/*--------- LIS Finalize ------------------*/

	free(dateiname);
	if (this->param!=NULL) g_free(this->param);
	}
	return RET_SUCCESS;
}


