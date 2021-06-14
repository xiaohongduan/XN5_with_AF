#include <mpi.h>
                         #include <pnetcdf.h>
                        int main(int argc, char **argv){
                        int ncfile;
                        return ncmpi_create(MPI_COMM_WORLD, argv[1],NC_CLOBBER|NC_64BIT_OFFSET, MPI_INFO_NULL, &ncfile);
                        };