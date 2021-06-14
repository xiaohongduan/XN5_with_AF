#include <omp.h>
                        int main(int argc, char **argv){
                        int i;
                        #pragma omp parallel for
                        for (i=0;i<5;i++)
                        {}
                        return omp_get_thread_num();};