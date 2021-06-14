double inter(double* Mat, double h, double step, int numbers);
//double ThetaH(int i, double h,class tree *T);

double	Theta_sub(int i,class tree *T);//Water content of xylem
//void set_Vector_Theta(class tree *T);
//void set_Vector_Cond(class tree *T);//generate vector containing xylem conductivities
 
double  abspowerDBL(double x, double y);


double cond3_sub(double Theta, double contemp, class tree *T);
double interT(double* Mat, double Theta, int numbers);//interpolate value for xylem conductance
double cap(int i,class tree *T);////interpolate xylem capacity value
double mualem(int i, double h);//interpolates soil conductivity
//void set_Vector_mualem(double*** Mat);//generate matrix containing mualem conductivities for each soil layer
double mualem_sub(int i,double h);//conductivity from soil matric head; mm/s
//input volumetric water content
//output h
double gethfromWC(double WC, int i, class tree *T);
