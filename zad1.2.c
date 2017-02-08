#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char* argv[])
{
    
    int proces = 0, zakres = 0, wylosowana_liczba = 0, wylosowana_liczba2 = 0, suma=0, suma2=0, i=1;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proces);
    MPI_Comm_size(MPI_COMM_WORLD, &zakres);
    
    if(proces == 0)
    {
	printf("\n");
	printf("Proces %d:\n", proces);
	
	for(i=1; i<zakres; i++)
	{
	    MPI_Recv(&wylosowana_liczba2, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);		//master odbiera od każdego procesu wylosowaną liczbę
	    //											  	  (i-proces źródłowy, i-tag procesu źródłowego)
	
	    suma+=wylosowana_liczba2;									//sumowanie liczb
	    printf("Od procesu: %d, odebrane: %d\n",i,wylosowana_liczba2);				//master wyświetla liczbę dla każdego procesu
	}
	
	printf("Suma po wszystkich procesach: %d\n\n", suma);						//master wyswietla sumę wylosowanych liczb po każdym procesie
	
	for(i=1; i<zakres; i++)
	{
	    MPI_Send(&suma, 1, MPI_INT, i, 0, MPI_COMM_WORLD);						//master wysyła sumę wylosowanych liczb do każdego procesu
	}	//										  	  (i-proces docelowy, 0-tag mastera)
    }
    else
    {
	srand(time(NULL)*proces);
	wylosowana_liczba = rand() % zakres+1;
	
	MPI_Send(&wylosowana_liczba, 1, MPI_INT, 0, proces, MPI_COMM_WORLD);				//wysyłanie liczby z danego procesu do mastera z tagiem danego procesu
	//									  			  (0-proces docelowy, proces-tag danego procesu wysyłającego)
	
	MPI_Recv(&suma2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);				//odbieranie od mastera sumy procesów (w buforze suma2) przez każdy proces
	//									  			  (0-proces źródłowy, 0-tag mastera)
	
	printf("Proces %d: Od procesu 0 -> suma po wszystkich procesach: %d\n", proces, suma2);
    }

    MPI_Finalize();
    return 0;
}