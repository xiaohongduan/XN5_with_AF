#ifndef LOAD_H_
#define LOAD_H_

void readCSV(tree *T,bool canopy);
void readTLP(tree *T, bool canopy, double fSoilLayerThickness);

void GenerateRoots(class tree *T);
void distribute(double *dist, int n, double L, int distribution, double GG);
void makeSubBranch(class tree *T, int *inodes, class nodes start, int startID,struct direction dir , double length, int n, int distribution);

//collection of cumulative distribution functions
//strictly increasing: 0...1 
//parameter: x, xmax
enum distr {HOMOGEN, INVERSE_SQUARE, LINEAR_DECREASE, GALE_GRIGAL};
double cdf_homogen(double x, double xmax);
double cdf_inverseSquareLaw(double x, double xmax);
double cdf_linearDecrease(double x, double xmax);
double cdf_GaleGrigal(double x, double GGbeta);// input x in m

void writeTLP(class tree T,char * outname, int what);
void writeOUTD(class tree T,char * outname, int what);

double abstand(class nodes n1, class nodes n2);

int intersects(double oB, double uB, int j, class tree *T);

#endif /*LOAD_H_*/