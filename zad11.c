#include<stdio.h>
#include<mpi.h>

int main(int argc, char* argv[])
{
    int proces, zakres;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proces);
    MPI_Comm_size(MPI_COMM_WORLD, &zakres);
    
    printf("Hello World! I'm process %d and there are %d processes in total!\n", proces, zakres);
    
    MPI_Finalize();
    return 0;
}