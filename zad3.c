/*

wektory 16
procesy 6

0: 2, 0->1
1: 2, 2->3
2: 1, 4
3: 1, 5
4: 2, 6->7
5: 1, 8

*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>

struct c
{
    double czas0, czas1, czas2, czas3, czas4;
    //int proc;
};

struct wspolrzedne
{
    double x, y, z;
};


int main(int argc, char* argv[])
{

    double a = 0.0, b = 0.0, c = 0.0, czasP, czasK, czasP2, czasK2, czasP3, czasK3;
    int licznik_wierszy = 0, start = 0, koniec = 0, i = 0, j = 0;
    double dlugosc_l = 0.0, suma_l = 0.0;
    int il_wierszy, porcja, dodany, P = 0, K = 0;
    
    struct wspolrzedne wektor_r;
    struct wspolrzedne suma;
    struct wspolrzedne *tab;
    tab = (struct wspolrzedne*)malloc(1000001*sizeof(struct wspolrzedne*));
    
    int proces, zakres;
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &proces);
    MPI_Comm_size(MPI_COMM_WORLD, &zakres);

    //struct c *tabTotalC;
    //tabTotalC = (struct c*)malloc(zakres*sizeof(struct c*));
    //struct c *tabC;
    //tabC = (struct c*)malloc(zakres*sizeof(struct c*));
    
    
    
    
    //tabC[0].czas0 = MPI_Wtime();
/*	czasP = MPI_Wtime();
	FILE *plik;
	if((plik = fopen("v01.dat", "r")) != NULL)
	{
	    while(fscanf(plik, "%lf %lf %lf", &a, &b, &c) == 3)
	    {
		tab[i].x = a;					// pętla zapisująca współrzędne wektorów z pliku do tablic
		tab[i].y = b;
		tab[i].z = c;
		licznik_wierszy++;
		if(i<licznik_wierszy)
		{
		    i++;
		}
	    }
	}
	else
	{
	    printf("Nie udało się otworzyć pliku!");
	}
	fclose(plik);
	//tabC[0].czas1 = MPI_Wtime();
	czasK = MPI_Wtime();
	*/
	
	MPI_File plik;
	MPI_File_open(MPI_COMM_WORLD, "v01.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &plik);
	MPI_Offset rozm_pliku, wsk_pliku;
	MPI_File_get_size(MPI_File plik, &rozm_pliku);
	il_wierszy = (rozm_pliku / 40);
	porcja = il_wierszy / zakres;
	dodany = il_wierszy - zakres * porcja;
	
	for(j = 0; j < proces + 1; j++)
	{
	    P = K;
	    K = P + porcja;
	    if(j < dodany)
	    {
		K += 1;
	    }
	}
	
	for(j = 0; j < proces + 1; j++)
	{
	    MPI_File_read_at(&plik, P, tab[j], 3, MPI_INT);
	}
	
	if(proces < il_wierszy % zakres)						//
	{
	    start = proces * il_wierszy / zakres + proces;
	    koniec = (proces + 1) * il_wierszy / zakres + proces + 1;
	}
	else										// warunki obliczające ile wektorów powinno być przydzielone do danego procesu
	{
	    start = proces * il_wierszy / zakres + il_wierszy % zakres;
	    koniec = (proces + 1) * il_wierszy / zakres + il_wierszy % zakres;
	}										//


	czasP2 = MPI_Wtime();
	for(i = start; i < koniec; i++)							// pętla sumująca długości wektorów na poszczególnych procesach
	{
		dlugosc_l += sqrt(pow(tab[i].x,2) + pow(tab[i].y,2) + pow(tab[i].z,2));
	}
	czasK2 = MPI_Wtime();
	//tabC[0].czas2 = MPI_Wtime();
	czasP3 = MPI_Wtime();
	MPI_Reduce(&dlugosc_l, &suma_l, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // sumowanie na procesie 0 sum długości z poszczególnych procesów
	czasK3 = MPI_Wtime();
	//tabC[0].czas3 = MPI_Wtime();
	
	if(proces == 0)
	{
	    printf("\n");
	    printf("srednia dlugosc l: %.9f\n", suma_l/licznik_wierszy);		// wyświetla średnią długosć wektora
	}
	//MPI_Gather(&tabC, 5, MPI_DOUBLE, &tabTotalC, 5, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	for(i = start; i < koniec; i++)							// pętla sumująca współżędne wektorów na poszczególnych procesach
	{
		wektor_r.x += tab[i].x;
		wektor_r.y += tab[i].y;
		wektor_r.z += tab[i].z;
	}
	
	MPI_Reduce(&wektor_r, &suma, 3, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);	// sumowanie na procesie 0 sum współżędnych z poszczególnych procesów
	
	if(proces == 0)
	{
	    printf("sredni wektor r: %.9f %.9f %.9f\n", suma.x/licznik_wierszy, suma.y/licznik_wierszy, suma.z/licznik_wierszy);   // wyświetla współżędne uśrednionego wektora
	    printf("\n");
	}

	FILE *plik2;
	if((plik2 = fopen("czasy.dat","w")) != NULL)
	{
	    //for(j=0; j<zakres; j++)
	    //{
		//if(proces == 0)
		{
		    fprintf(plik2,"timings (proc %d):\n\treadData:\t%f\n\tprocessData:\t%f\n\treduceResults:\t%f\n\ttotal:\t\t\n\n\n",0, czasK-czasP,  czasK2-czasP2, czasK3-czasP3);
		}
	    //}
	}
	else return -1;
	fclose(plik2);

	//free(tabTotalC);
	//free(tabC);
	free(tab);
    MPI_Finalize();
    return 0;
}