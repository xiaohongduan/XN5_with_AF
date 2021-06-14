#ifdef WIN32
#include <windows.h>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h> 



#if USE_OPENMP && USE_MPI
#error "MPI and OPENMP can not be used at the same time!"
#endif



#include <expertnclass.h>
#include <xpn_output_netcdf.h>
#include <general_config.h>




#define EPSILON 1.0e-34


typedef struct {
	expertnclass** xpn; // Expert-N Instance
	int xpn_N;
	int xpn_finished; // 1 or 0
} struct_drive_xpn;


static char* create_string_from_source(char* src)
{
	char *S;
	if (src==NULL) return NULL;
	S = (char*) malloc(strlen(src)+1);
	strcpy(S,src);
	return S;

}


static xpn_output_netcdf *netf;
static void exit_NETCDF();



int main(int argc, char **argv)
{
	general_config *cfg;
	expertnclass **xpn_class;
	struct_drive_xpn *drv;
	struct_register_modul_list *modul_list;
	int count;
	int i,i2,i3,i4,i5;
	BOOL finish;
	char S[128],*S2;
	char *project_name,*reg_str,*project_path;
	int MPI_act; // 0..1: Es wird mit MPI parallelisiert
	int OPENMP_act; // 0..1: Es wird mit OpenMP parallelisiert

	int *domain_list;
	int domain_list_size;

	int loop_min,loop_max;

	// Variablen für command line parser:
	GError *error = NULL;
	GOptionContext *context;
	//options var:
	gboolean xpn_version = FALSE;
	gboolean xpn_modules = FALSE;
	gchar* cfg_file = NULL;
	gchar* base_dir = NULL;
	gboolean console_modus = FALSE;
	//options:
	GOptionEntry entries[] = {
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &xpn_version, "Expert - N Version", NULL },
		{ "show-modules", 'm', 0, G_OPTION_ARG_NONE, &xpn_modules, "Used Modules", NULL },
		{ "config-file",'c',0,G_OPTION_ARG_STRING,&cfg_file, "Config - File",NULL},
		{ "base-dir",'d',0,G_OPTION_ARG_STRING,&base_dir, "Base - Directory",NULL},
		{ "console_modus",'n',0,G_OPTION_ARG_NONE,&console_modus, "Console - Modus",NULL},
		{ NULL }
	};
	
		

    MPI_act = 0;
    OPENMP_act = 0;

#ifdef USE_MPI
    MPI_act=1;
#endif

#ifdef USE_OPENMP
    OPENMP_act=1;
#endif


#ifdef USE_MPI
	int numtasks=1;
	int rank=0;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
#endif


	// glib Klassen initiatisieren
	g_type_init();
	
	// Wenn man das nicht macht, kann man keine Dateien öffnent, die Umlaute haben oder printf mit Umlauten geht nicht
	setlocale(LC_ALL, "en_US.UTF-8");

	// Command Line Parser:
	context = g_option_context_new ("- Expert-N (Stand Alone Version)");
	g_option_context_add_main_entries (context, entries, NULL); // für unterschiedliche Sprachen: GETTEXT_PACKAGE

	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_print ("option parsing failed: %s\n", error->message);
		g_option_context_free(context);
#ifdef USE_MPI
		MPI_Finalize();
#endif
		exit (1);
	}

	if (xpn_version) {
		g_option_context_free(context);
#ifdef XPN_VERSION
		printf("Expert - N Version: %s\n",XPN_VERSION);
#else
		printf("Expert - N Version: 5.0 Rev 0\n");
#endif
#ifdef USE_MPI
		MPI_Finalize();
#endif
		return RET_SUCCESS;
	}

	if (cfg_file==NULL) {
		g_option_context_free(context);
		printf("No config file defined, use '--help' to get more information!\n");
#ifdef USE_MPI
		MPI_Finalize();
#endif
		return RET_SUCCESS;
	}

	if (base_dir==NULL) {
		char *S,*S2;
		S = g_get_current_dir();
		S2=delete_last_part_of_path(S);
		base_dir = g_strdup_printf("%s",S2);
		printf("No base path defined, '%s' is used!\n",base_dir);
		g_free(S);
		//free(S2);
	}




	// Speicher für CFG:
	cfg = general_config_new();

	// Config lesen
	if (general_config_load_config_file(cfg,cfg_file)!=RET_SUCCESS) {
#ifdef USE_MPI
		MPI_Finalize();
#endif
		exit(1);
	}

	domain_list = general_config_get_grid_id_list(cfg,&(domain_list_size));

	// Es werden alle Namen der Modulle und Klassen die möglich sind geladen (werden aus der dll entnommen)
	modul_list = register_modul_load_modul_list(cfg->expertn_lib_list,cfg->expertn_lib_list_len,base_dir);

	// wenn Module gezeigt werden sollen, dann ist hier Schluss:
	if (xpn_modules==TRUE) {
		for (i=0; i<modul_list->length; i++) {
			printf("%s,%s,%s,%s\n",modul_list->modul_list[i]->Modul,modul_list->modul_list[i]->Submodul,modul_list->modul_list[i]->libname,modul_list->modul_list[i]->Name);
		}
		register_modul_list_delete(modul_list);
		general_config_delete(cfg);
		g_free(cfg_file);
		cfg_file=NULL;
		g_free(base_dir);
		base_dir=NULL;
#ifdef USE_MPI
		MPI_Finalize();
#endif
		return RET_SUCCESS;
	}

	count = cfg->number_of_xpn;

#ifdef USE_MPI
	if (numtasks>count) {
		if (rank>count-1) {
			char *S;
			S = g_strdup_printf("Only %d processors are needed. Process %d is killed!\n",count,rank);
			PRINT_ERROR(S);
			g_free(S);
			MPI_Finalize();
			return RET_SUCCESS;
		}
		numtasks=count;
	}
#endif
	loop_min=0;
	loop_max=count;

	//Speicher für Expert - N Instanzen reservieren:
	xpn_class = (expertnclass **)calloc(count,sizeof(expertnclass *));

	drv = (struct_drive_xpn*) calloc(cfg->grid_len,sizeof(struct_drive_xpn));

#ifdef USE_OPENMP
	if ((cfg->num_proc<=0) || cfg->num_proc > omp_get_num_procs()) {
		cfg->num_proc=count;
		if (count > omp_get_num_procs()) {
			cfg->num_proc = omp_get_num_procs();
		} else if (count <= 0) {
			cfg->num_proc = 1;
		} else {
			cfg->num_proc = count;
		}

	}

	omp_set_num_threads(cfg->num_proc);
#else
	cfg->num_proc=1;
#endif


#ifdef USE_OPENMP
	sprintf(S,"%d processors are used by OpenMP!\n",cfg->num_proc);
	PRINT_M(cfg->DEBUG_LEVEL,5,S);
#endif


	// Expert-N Classen initialiesieren:

	S2 = get_filename_without_path(cfg_file);
	project_name = get_filename_without_extension(S2);
	free(S2);

	project_path = get_path_from_filename(cfg_file);


	i3 = 0;
	for (i=0; i<cfg->grid_len; i++) {
		drv[i].xpn_N = cfg->grid[i].instance_len;
		drv[i].xpn_finished = FALSE;
		drv[i].xpn = (expertnclass**) calloc(drv[i].xpn_N,sizeof(expertnclass*));
		for (i2=0; i2<cfg->grid[i].instance_len; i2++) {

#ifdef USE_MPI
			loop_min    =(int)((long)(rank + 0) *
			                   (long)(count)/(long)numtasks);
			loop_max    =(int)((long)(rank + 1) *
			                   (long)(count)/(long)numtasks);

			if ((i3 >= loop_min) && (i3 < loop_max)) {
#endif

				reg_str = g_strdup_printf("%i_%i_%i_%i",cfg->grid[i].id,cfg->grid[i].i,cfg->grid[i].j,i2);
				xpn_class[i3] = expertnclass_new(cfg->DEBUG_LEVEL, modul_list,cfg->grid[i].instance[i2].config_file,base_dir,project_name,project_path,reg_str,cfg->grid[i].instance[i2].xpn_id_number,cfg->number_of_xpn,cfg->config_values_len,cfg->config_values);
				drv[i].xpn[i2] = xpn_class[i3];
				g_free(reg_str);
#ifdef USE_MPI
			} else {
				xpn_class[i3] = NULL;
			}
#endif
			i3++;
		}

	}

	free(project_name);
	free(project_path);


	// Um auf andere Instanzen zugreifen zu können, werden jetzt die entsprechenden Verknüpfungen gemacht:

#ifdef USE_MPI
		loop_min    =(int)((long)(rank + 0) *
		                   (long)(count)/(long)numtasks);
		loop_max    =(int)((long)(rank + 1) *
		                   (long)(count)/(long)numtasks);
#endif

	i3 = 0;
	for (i=0; i<cfg->grid_len; i++) {
		double fraction;

		i5 = i3;
		fraction = 0.0;
		for (i2=0; i2<cfg->grid[i].instance_len; i2++) {
			PXSYSTEM pXSys;

#ifdef USE_MPI
			if ((i3 >= loop_min) && (i3 < loop_max)) {				
#endif
				pXSys = xpn_class[i3]->pXSys;
				
				pXSys->MPI_act = MPI_act;
				pXSys->OPENMP_act = OPENMP_act;
				
				pXSys->reg_str_inst = (char**)calloc(cfg->number_of_xpn,sizeof(char*));
				pXSys->xpn_classes = (void**)calloc(cfg->number_of_xpn,sizeof(void*));

				// Das hier muss für MPI noch mal überarbeitet werden:
				//for (i4=0; i4 < cfg->number_of_xpn; i4++) {
				for (i4=loop_min; i4 < loop_max; i4++) {
					pXSys->reg_str_inst[i4] = create_string_from_source(xpn_class[i4]->pXSys->reg_str);
					pXSys->xpn_classes[i4] = (void*)xpn_class[i4];
				}

				if (cfg->grid[i].instance_len==1) {
					pXSys->landuse_cover_fraction = 1.0;
				} else {
					pXSys->landuse_cover_fraction = cfg->grid[i].instance[i2].cover/100.0;
				}
				fraction+=pXSys->landuse_cover_fraction;
				pXSys->id_M = cfg->grid[i].instance_len;
				

				pXSys->id_Mosaic = (int*)calloc(cfg->grid[i].instance_len,sizeof(int));
				for (i4=0; i4<cfg->grid[i].instance_len; i4++) {
					pXSys->id_Mosaic[i4] = i5+i4;
				}

#ifdef USE_MPI
			}
#endif
			i3++;
		}
#ifdef USE_MPI
		if ((i3-1 >= loop_min) && (i3-1 < loop_max)) {
#endif

			if ((fraction < (1.0- EPSILON)) || ((fraction > (1.0+ EPSILON)))) {
				S2 = g_strdup_printf("Sum over the landcover of grids %s are not 100%%. It is: %f",xpn_class[i3-1]->pXSys->reg_str,fraction*100.0);
				PRINT_ERROR(S2);
				g_free(S2);
			}
#ifdef USE_MPI
		}
#endif

		}

		netf = xpn_output_netcdf_new(xpn_class,cfg->number_of_xpn,cfg->config_values,cfg->config_values_len,cfg);
		g_atexit(exit_NETCDF);

		// ... hier können jetzt zusätzliche registry Einträge gemacht werden:
		// ... --> xpn_register_var *expertnclass_get_varlist(expertnclass *self);

		

		// für MPI werden loop_min und loop_max auf die Prozesoren aufgeteilt:

#ifdef USE_MPI
		loop_min    =(int)((long)(rank + 0) *
		                   (long)(count)/(long)numtasks);
		loop_max    =(int)((long)(rank + 1) *
		                   (long)(count)/(long)numtasks);
						   
		
		//printf("count: %d, numtasks: %d\n",count,numtasks);
		//printf("Prozess ID: %d, loop_min: %d, loop_max %d\n",rank,loop_min,loop_max);

		if (rank==0) {
			sprintf(S,"%d processors are used by MPI!\n",numtasks);
			PRINT_M(cfg->DEBUG_LEVEL,3,S);
		}


		{
			// Rank der anderen 
				for (i2=loop_min; i2<loop_max; i2++) {
					xpn_class[i2]->pXSys->MPI_rank=i;
					xpn_class[i2]->pXSys->MPI_size=numtasks;
					xpn_class[i2]->pXSys->MPI_ranks=(int*)g_malloc0(sizeof(int)*count);
					for (i=0;i<count;i++)
						{
							int k;
							k= (int)((double)i/(double)numtasks * (double)count);
							xpn_class[i2]->pXSys->MPI_ranks[i]=k;							
						}
				}
			
		}		
		

#endif

		/*#ifdef USE_OPENMP
		self->pXSys->OMP_rank=0;
		self->pXSys->OMP_size=1;
		#endif
		 */

		// Alle geladenen Module sollen Initiatisiert werden:
		for (i=loop_min; i<loop_max; i++) {
			expertnclass_initialize(xpn_class[i]);
		}




		finish = FALSE;

		while (1) {
			// Konsolenmodus funktioniert nicht mit MPI:
#ifndef USE_MPI
			if ((console_modus==TRUE) && (NewDay(xpn_class[0]->pTi))) {
				char ch;
				fprintf(stdout,"\n<COMMAND LINE: (n: next step, q: QUIT Expert-N)>\n");
				fflush(stdout);
				ch=fgetc(stdin);
				if (ch=='q') {
					finish=TRUE;
				}

			}
#endif

			for (i=loop_min; i<loop_max; i++) {
				if (expertnclass_run_mosaic(xpn_class[i]) != RET_SUCCESS) {
					finish=TRUE;
					break;
				}
			}
			if (finish==TRUE) break;




#ifdef USE_OPENMP
#pragma omp parallel for schedule(dynamic) reduction(||: finish)
#endif
			for (i=loop_min; i<loop_max; i++) {

				//printf("%d,%d,%d\n",omp_get_thread_num(),omp_get_num_threads(),omp_get_max_threads());
				if (expertnclass_run(xpn_class[i]) != RET_SUCCESS) {
					finish=TRUE;
				}

			}


			//xpn_output_netcdf_write_data(netf);

			for (i=0; i<domain_list_size; i++) {
				xpn_output_netcdf_write_data_matrix(netf,domain_list[i]);
			}



			if (finish==TRUE) break;
		}


		// Alle expert-N Klassen beenden
		netf = xpn_output_netcdf_Done(netf);
		for (i=loop_min; i<loop_max; i++) {
			expertnclass_done(xpn_class[i]);
		}
		
		

		for (i=0; i<cfg->grid_len; i++) {
			if (drv[i].xpn!=NULL) free(drv[i].xpn);
		}
		free(drv);



		if (xpn_class!=NULL)free(xpn_class);

		register_modul_list_delete(modul_list);
		general_config_delete(cfg);
		if (cfg_file!=NULL) g_free(cfg_file);

		if (base_dir!=NULL)g_free(base_dir);
		g_option_context_free(context);

		if (domain_list!=NULL) {
			g_free(domain_list);
			domain_list=NULL;
			domain_list_size=0;
		}


		// Konsolenmodus funktioniert nicht mit MPI:
#ifndef USE_MPI
		if (console_modus==TRUE) {
			fprintf(stdout,"\n<COMMAND LINE: (n: next step, q: QUIT Expert-N)>\n");
			fflush(stdout);
			fgetc(stdin);
		}
#endif

#ifdef USE_MPI
/*		{
			int l_min,l_max;
			MPI_Status stat;
			double d_send;
			double *d_rec;
			d_send=(double)rank+0.1;
			//printf("dsend: %f %d\n",d_send,numtasks);
			d_rec=g_malloc0(sizeof(double)*count);
			for (i=0; i<numtasks; i++) {
				l_min    =(int)((long)(i + 0) *
				                (long)(count)/(long)numtasks);
				l_max    =(int)((long)(i + 1) *
				                (long)(count)/(long)numtasks);
				for (i2=l_min; i2<l_max; i2++) {

					if ((rank==0) && (i!=0)) {
						printf("rec\n");
						MPI_Recv(&(d_rec[i2]),1,MPI_DOUBLE,i,i2*i,MPI_COMM_WORLD,&stat);
						printf("rec end %f\n",d_rec[i2]);
					}
					if ((rank==i) && (i!=0)) {
						printf("send: %f\n",d_send);
						MPI_Send(&d_send,1,MPI_DOUBLE,0,i2*i,MPI_COMM_WORLD);
						printf("send end\n");
					}
					if ((rank==0) && (i==0)) {
						d_rec[0] = d_send;
					}
				}
			}
			if (rank==0) {
				for (i=0; i<count; i++) {
					printf("d_rec[%d] = %f\n",i,d_rec[i]);
				}

			}
			g_free(d_rec);
		}
*/

#endif

#ifdef USE_MPI
		MPI_Finalize();
#endif

		return RET_SUCCESS;
	}

	void exit_NETCDF() {
		netf = xpn_output_netcdf_Done(netf);
	}

#ifdef WIN32
	int WINAPI WinMain(HINSTANCE hInstance,
	                   HINSTANCE hPrevInstance,
	                   LPSTR lpCmdLine,
	                   int nCmdShow) {
		return main (__argc, __argv);
	}
#endif
