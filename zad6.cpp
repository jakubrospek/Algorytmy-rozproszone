#include <mpi.h>
//#include "EasyBMP.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include<cstdlib>

using namespace std;



int main( int argc, char* argv[] )
{
	int liczba_zadan, rozmiar_zadania, ziarno_s, ile_cykli, reszta, start=0, koniec=0, obieg=0, p=0, numer_w_tab=0; //t=0, r=0, mianownik=0,licznik=1;
	int temp=0, licznik_uzytych_procesow=0;
	stringstream lz, rz, zs;
	
	lz << argv[1];
	rz << argv[2];
	zs << argv[3];
	
	lz >> liczba_zadan;
	rz >> rozmiar_zadania;
	zs >> ziarno_s;
	
	
	MPI::Init();
	int proces = MPI::COMM_WORLD.Get_rank();
	int zakres = MPI::COMM_WORLD.Get_size();
	
	ile_cykli = liczba_zadan / zakres-1;	// -1 bo master nie pracuje
	reszta = liczba_zadan % zakres-1;
	
	int * zbior_zadan_master = new int[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    zbior_zadan_master[i] = 0;
	}
	
	int * zbior_zadan_procesy = new int[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    zbior_zadan_procesy[i] = 0;
	}
	
	int * t = new int[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    t[i] = 0;
	}
	
	int * licznik = new int[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    licznik[i] = 1;
	}
	
	int * mianownik = new int[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    mianownik[i] = 0;
	}
	
	double * r = new double[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    r[i] = 0;
	}
	
	double * odebrane = new double[liczba_zadan];
	for(int i=0; i<liczba_zadan; i++)
	{
	    odebrane[i] = 0;
	}
	
	if(proces == 0)
	{
	    int wylosowana_liczba = 0, licznik = 0;
	    
	    srand(ziarno_s);
	    for(int i=0; i<liczba_zadan; i++)
	    {
		losuj:
		wylosowana_liczba = rand() % rozmiar_zadania + 1;
		for(int j=0; j<licznik; j++)				// pętla sprawdzająca czy wylosowana liczba nie powtarza się
		{							// wśród dotychczasowych liczb
		    if(wylosowana_liczba!=zbior_zadan_master[j])
		    {
		    }
		    else
		    {
			goto losuj;					// jeśli się powtrza to losowanie jest powtarzane
		    }
		}
		zbior_zadan_master[licznik] = wylosowana_liczba;	// jeśli nie to liczba jest zapisywana w tablicy
		licznik++;
	    }
	}
	    
	    if(liczba_zadan > zakres-1) 	//przypadek gdy zadań jest więcej niż procesow
	    {
		
		start=0;
		koniec=0;
		
		while(obieg<ile_cykli)		// pętla obsługująca pełne cykle obiegu po zadaniach
		{
		    if(proces==0)
		    {
			koniec+=zakres-1;	// przy każdym kolejnym obiegu końcowa granica na obsługiwanych zadaniach
						// jest przesuwana o wartość liczby procesów
			for(int k=start; k<koniec; k++)
			{
			    p++;
			    MPI_Send(&zbior_zadan_master[k], 1, MPI_INT, p, p, MPI_COMM_WORLD); // proces 0 wysyła dane do innych procesów
			}
			p=0;
		    
			for(int k=start; k<koniec; k++)
			{
			    p++;
			    MPI_Recv(&odebrane[k], 1, MPI_DOUBLE, p, p, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 	//proces 0 przechodzi w tryb nasłuchu
			}											//i oczekuje na dane od innych procesów
			p=0;
		    
			start+=zakres-1;			// przy każdym kolejnym obiegu początkowa granica również jest przesuwana o liczbę procesów
			//obieg++;
		    }
		    else
		    {
			MPI_Recv(&zbior_zadan_procesy[proces], 1, MPI_INT, 0, proces, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //inne procesy odbierają zadania od mastera
			t[proces]=zbior_zadan_procesy[proces]; //przypisanie wartości pauzy w ms
			for(int l=1; l<zbior_zadan_procesy[proces]+1; l++) // pętla licząca silnię t i sumę kolejnych liczb ciągu t
			{
			    licznik[proces]*=l;
			    mianownik[proces]+=l;
			}
			r[proces]=licznik[proces]/mianownik[proces]; // obliczanie r według danych charakterystycznych dla danego procesu
			sleep(t[proces]);				// odpoczynek każdego procesu określoną ilość czasu
		
			MPI_Send(&r[proces], 1, MPI_DOUBLE, 0, proces, MPI_COMM_WORLD); //każdy proces wysyła obliczoną wartość r na mastera
			//obieg++;
		    }
		obieg++;
	/*	if(proces==0)
		{
		    FILE *plik;
		    if((plik=fopen("plik.txt","w"))!=NULL)
		    {
			for(int i=0; i<liczba_zadan; i++)
			{
			    if(fprintf(plik, "%f\n",odebrane[i])<0)
			    return -1;
			}
		    }
		    else
		    return -1;
		    fclose(plik);
		} */
		
		}
		
		if(reszta!=0)					// warunek obsługujący resztę zadań które pozostały po połnych cyklach
		{
		    if(proces==0)
		    {
		
			for(int m=koniec; m<koniec+reszta; m++) // pętla startuje od końca ostatniego pełnego cyklu zliczając wartość reszty
			{
			    p++;
			    MPI_Send(&zbior_zadan_master[m], 1, MPI_INT, p, p, MPI_COMM_WORLD); // master wysyła zadania na procesy
			}
			p=0;
		
			for(int m=koniec; m<koniec+reszta; m++)
			{
			    p++;
			    MPI_Recv(&odebrane[m], 1, MPI_DOUBLE, p, p, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // master nasłuchuje zwrotu obliczonych
			}										// przez procesy wartości
			p=0;
		
		    }
		    else
		    {
			if(proces<reszta) // warunek zapewniający wstęp tylko procesom obsługującym resztę zadań
			{
			    numer_w_tab = (ile_cykli*zakres) + proces; // pozwala odnieść się do dalszych elementów tablicy spoza zasięgu pełnych cyklów
			    MPI_Recv(&zbior_zadan_procesy[numer_w_tab], 1, MPI_INT, 0, proces, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			    t[numer_w_tab]=zbior_zadan_procesy[numer_w_tab];
			    for(int l=1; l<zbior_zadan_procesy[numer_w_tab]+1; l++)
			    {
				licznik[numer_w_tab]*=l;
				mianownik[numer_w_tab]+=l;
			    }
			    r[numer_w_tab]=licznik[numer_w_tab]/mianownik[numer_w_tab];
			    sleep(t[numer_w_tab]);
		
			    MPI_Send(&r[numer_w_tab], 1, MPI_DOUBLE, 0, proces, MPI_COMM_WORLD);
			}
		    }
		}
	    }
	    
	    if(liczba_zadan < zakres-1)							// przypadek gdy zadań jest mniej niż procesów
	    {
		temp = (zakres-1) - liczba_zadan;			// określa liczbę procesów zbędnych
		licznik_uzytych_procesow = (zakres-1) - temp;		// określa liczbę procesów, które zostaną użyte
		
		if(proces == 0)
		{
		    for(int i=0; i<licznik_uzytych_procesow; i++)
		    {
			MPI_Send(&zbior_zadan_master[i], 1, MPI_INT, i+1, i+1, MPI_COMM_WORLD); // master wysyła zadania do ograniczonej liczby procesow
		    }
		    
		    for(int i=0; i<licznik_uzytych_procesow; i++)
		    {
			MPI_Recv(&odebrane[i], 1, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // następnie nasłuchuje odpowiedzi
		    }
		}
		else
		{
		    MPI_Recv(&zbior_zadan_procesy[proces], 1, MPI_INT, 0, proces, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // inne procesy odbierają, liczą co trzeba
		    t[proces]=zbior_zadan_procesy[proces];								// odpoczywają i wysyłają spowrotem
		    for(int l=1; l<zbior_zadan_procesy[proces]+1; l++)
		    {
			licznik[proces]*=l;
			mianownik[proces]+=l;
		    }
		    r[proces]=licznik[proces]/mianownik[proces];
		    sleep(t[proces]);
		
		    MPI_Send(&r[proces], 1, MPI_DOUBLE, 0, proces, MPI_COMM_WORLD);
		}
	    }
	    
	    if(liczba_zadan == zakres-1)									// przypadek gdy zadań jest tyle samo co procesów
	    {
		if(proces==0)
		{
		    for(int i=0; i<zakres-1; i++)
		    {
			MPI_Send(&zbior_zadan_master[i], 1, MPI_INT, i+1, i+1, MPI_COMM_WORLD);
		    }
		    
		    for(int i=0; i<zakres-1; i++)
		    {
			MPI_Recv(&odebrane[i], 1, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    }
		}
		else
		{
		    MPI_Recv(&zbior_zadan_procesy[proces], 1, MPI_INT, 0, proces, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    t[proces]=zbior_zadan_procesy[proces];
		    for(int l=1; l<zbior_zadan_procesy[proces]+1; l++)
		    {
			licznik[proces]*=l;
			mianownik[proces]+=l;
		    }
		    r[proces]=licznik[proces]/mianownik[proces];
		    sleep(t[proces]);
		
		    MPI_Send(&r[proces], 1, MPI_DOUBLE, 0, proces, MPI_COMM_WORLD);
		}
	    }
	
/*	else
	{
	    if(liczba_zadan > zakres)
	    {
		ile_cykli = liczba_zadan / zakres;
		reszta = liczba_zadan % zakres;
		
		while(obieg<ile_cykli)
		{
		    MPI_Recv(&zbior_zadan_procesy[proces], 1, MPI_INT, 0, proces, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		    t[proces]=zbior_zadan_procesy[proces];
		    for(int l=1; l<zbior_zadan_procesy[proces]+1; l++)
		    {
			licznik[proces]*=l;
			mianownik[proces]+=l;
		    }
		    r[proces]=licznik[proces]/mianownik[proces];
		    sleep(t[proces]);
		
		    MPI_Send(&r[proces], 1, MPI_INT, 0, proces, MPI_COMM_WORLD);
		    obieg++;
		}
		
		if(proces<reszta)
		{
		}
		
	    }
	    
	    if(liczba_zadan < zakres)
	    {
	    }
	    
	    if(liczba_zadan == zakres)
	    {
	    }
	}
*/
MPI::Finalize();
}