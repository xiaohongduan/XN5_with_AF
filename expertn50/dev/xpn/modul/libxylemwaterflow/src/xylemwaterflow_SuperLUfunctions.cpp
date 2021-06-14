#include <slu_ddefs.h>
#include "xylemwaterflow_tree.h"
#include "xylemwaterflow_global.h"
#include <time.h>
//int nentry;
//int *SLU_asub;
//double *SLU_a;
//int *SLU_xa;
//double *SLU_rhs;

//static int      *perm_c; /* column permutation vector */
//static int firsttime = 1;
void superLU_Ross(double *solution, class tree *T)
{

	int n;//2332;//235
	int m;//square matrix
	
	int i,j;
	superlu_options_t options;
    SuperLUStat_t stat;
	int      *perm_r; // row permutations from partial pivoting 
    int      *perm_c; // column permutation vector 
    int      nrhs, info;
	SuperMatrix A, L, U, B;
    double       *dp;
	DNformat     *Astore ;
	n=T->iCnodes+T->iRnodes;
	m=n;

    //Create matrix A in the format expected by SuperLU.
    dCreate_CompCol_Matrix(&A, m, n, T->nentry, T->SLU_a, T->SLU_asub, T->SLU_xa, SLU_NR, SLU_D, SLU_GE);	

//******************************************************
//Solving the System
//******************************************************
	// Create right-hand side matrix B.
    nrhs = 1;	
	dCreate_Dense_Matrix(&B, m,  nrhs, T->SLU_rhs, m, SLU_DN, SLU_D, SLU_GE);	

    // Set the default input options.
    set_default_options(&options);
    
 
    // Initialize the statistics variables.
    StatInit(&stat);
	if ( !(perm_r = intMalloc(m)) ) 
		double dumm=1; // fprintf(files.logfile,"Malloc fails for perm_r[].");
	if ( !(perm_c = intMalloc(n)) ) 
			double dumm=1; //fprintf(files.logfile,"Malloc fails for perm_c[].");*/
	/*if(firsttime)      
	{
		firsttime =0;
		options.ColPerm = COLAMD;
		if ( !(perm_c = intMalloc(n)) ) 
			double dumm=1; //fprintf(files.logfile,"Malloc fails for perm_c[].");
		dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info); //makes perm_c, 30% faster
	}*/

	//options.ColPerm = MY_PERM;
	options.ColPerm = COLAMD;
	clock_t start=clock();
	dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info);		   
	int fghf = clock()-start;

	Astore = (DNformat*) B.Store;
    dp = (double*) Astore->nzval;
	for (j = 0; j < B.ncol; ++j) 
        for (i = 0; i < B.nrow; ++i)
			//T->node[i].H_new_total = dp[i + j*n];
			solution[i] = dp[i + j*n];

   	// De-allocate storage
	SUPERLU_FREE (perm_r);
	SUPERLU_FREE (perm_c);
	Destroy_SuperNode_Matrix(&L);
	Destroy_CompCol_Matrix(&U);	
	StatFree(&stat);	
	Destroy_CompCol_Matrix(&A); 
	Destroy_SuperMatrix_Store(&B);	
}
/*



void superLU_pde(class tree *T)
{

	int n;//2332;//235
	int m;//square matrix
	
	int i,j;
	superlu_options_t options;
    SuperLUStat_t stat;
	int      *perm_r; // row permutations from partial pivoting
    int      *perm_c; // column permutation vector
    int      nrhs, info;
	SuperMatrix A, L, U, B;
    double       *dp;
	DNformat     *Astore ;
	n=T->iCnodes+T->iRnodes;
	m=n;
	
	for(i=0;i<T->iCnodes+T->iRnodes;i++)
	{		
		T->SLU_rhs[i]=T->node[i].Vectorb;
	}


    //Create matrix A in the format expected by SuperLU. 
    dCreate_CompCol_Matrix(&A, m, n,T->nentry, T->SLU_a, T->SLU_asub, T->SLU_xa, SLU_NR, SLU_D, SLU_GE);	

//******************************************************
//Solving the System
//******************************************************
	// Create right-hand side matrix B.
    nrhs = 1;	
	dCreate_Dense_Matrix(&B, m, nrhs, T->SLU_rhs, m, SLU_DN, SLU_D, SLU_GE);	

    // Set the default input options.
    set_default_options(&options);
    options.ColPerm = NATURAL;

    // Initialize the statistics variables.
    StatInit(&stat);
	if ( !(perm_r = intMalloc(m)) ) 
		double dumm=1; // fprintf(files.logfile,"Malloc fails for perm_r[].");
	if ( !(perm_c = intMalloc(n)) ) 
		double dumm=1; //fprintf(files.logfile,"Malloc fails for perm_c[].");
	
	dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info);		   

	Astore = (DNformat*) B.Store;
    dp = (double*) Astore->nzval;
	for (j = 0; j < B.ncol; ++j) 
        for (i = 0; i < B.nrow; ++i)
			T->node[i].H_new_total = dp[i + j*n];

   	// De-allocate storage
	SUPERLU_FREE (perm_r);
	SUPERLU_FREE (perm_c);
	Destroy_SuperNode_Matrix(&L);
	Destroy_CompCol_Matrix(&U);	
	StatFree(&stat);	
	Destroy_CompCol_Matrix(&A); 
	Destroy_SuperMatrix_Store(&B);	
}

*/
void countNonZeros(class tree *T)
{
		//count number of nonzero matrix elements for compressed row format
		T->nentry=0;
	 
		
		for(int i=0;i<T->iCnodes+T->iRnodes;i++)
		{
			T->nentry  +=2;
			T->nentry  += T->node[i].child.size();
		 /*
			if(T->node[i].type==2)
				nentry=nentry+3;
			else if(T->node[i].type==3)
				nentry=nentry+4;
			else if(T->node[i].type==4)
				nentry=nentry+2;
			else if(T->node[i].type==5)
				nentry=nentry+5;*/
	 
		}
		T->SLU_asub=NULL;
		T->SLU_a=NULL;
		T->SLU_xa=NULL;
		T->SLU_rhs=NULL;
}