void GenerateMatrix(double* stemflow, int simstarttimestep, class tree *T);//generate stiffness matrix for the PDES
void GenerateVector(int simstarttimestep, class tree *T);//right hand side of the PDES
int test_cvg(double* convsum, class tree *T);//test for convergency of solver

struct pff
{
	int i;
	double v;
};