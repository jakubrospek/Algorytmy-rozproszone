#include <mpi.h>
#include "EasyBMP.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;


struct piksel
{
    int R;
    int G;
    int B;
    int height;
    int width;
    int beg;
    int end;
    
};

double g(int x, int y, double sig)
{
    double wynik;
    double potega;
    
    potega = -(x*x+y*y) / (2*sig*sig);
    wynik = pow(M_E,potega) / (2*M_PI*sig*sig);
    
    return wynik;
}

int main( int argc, char* argv[] )
{
	int k = 0;
	int rozmiar_obrazka;
	int porcja=0, dodany=0, start, koniec, sigma = 0, l=0, i=0, z=0;
	stringstream str;
	
	str << argv[2];
	str >> sigma;
	
	
	MPI::Init();
	int proces = MPI::COMM_WORLD.Get_rank();
	int zakres = MPI::COMM_WORLD.Get_size();
	
	piksel *piksele;
	piksel *TABwyj;
	double skladoweR, skladoweG, skladoweB;
	double wG;
	
	if(proces == 0)
	{
	
	    printf("start\n");
	    // Deklaracja i czytanie bitmpay
	    BMP plik;
	    plik.ReadFromFile( argv[1] );
	    rozmiar_obrazka = plik.TellHeight()*plik.TellWidth();
	    
	    
	    piksele = new piksel[rozmiar_obrazka];
	    
	    //piksel *TABwyj;
	    TABwyj = new piksel[rozmiar_obrazka];
	    
	    //int w, h;
	    //w = plik.TellWidth();
	    //h = plik.TellHeight();
	    //rozmiar_obrazka = w*h;
	    
	    //piksel obrazek[rozmiar_obrazka];
	    //bitmap_info obrazek(rozmiar_obrazka);
	    //obrazek.piksele[0].height = plik.TellHeight();
	    //obrazek.piksele[0].width = plik.TellWidth();
	    
	    piksele[0].height = plik.TellHeight();
	    piksele[0].width = plik.TellWidth();
	    
	    for( int j = 0 ; j < plik.TellHeight() ; j++)
	    {
		for( int i = 0 ; i < plik.TellWidth() ; i++)
		{
			piksele[k].R = plik(i,j)->Red;
			piksele[k].G = plik(i,j)->Green;
			piksele[k].B = plik(i,j)->Blue;
			if(k<rozmiar_obrazka)
			{
			    k++;
			}
		}
	    }
	    
	    
	    printf("rozmiar obrazka: %d\n", rozmiar_obrazka);
	    MPI_Bcast(&rozmiar_obrazka, 1, MPI_INT, 0, MPI_COMM_WORLD);		// wysyłam rozmiar tablicy z proc 0
	    MPI_Bcast(piksele, rozmiar_obrazka, MPI_INT, 0, MPI_COMM_WORLD); //wysyłam wczytane do tablicy piksele z proc 0 do innych procesow (tab. piksele bez & !)
	    printf("proces0\n");
	    //printf("dlugosc obrazka: %d\n", obrazek.width);
	    
	    porcja = piksele[0].width / zakres;
	    dodany = piksele[0].width % zakres;
	    start = porcja*proces;
	    koniec = porcja*proces+porcja;
	    /*
	    if(proces < piksele[0].width % zakres)		// podział pracy między procesy
	    {
		start = proces * piksele[0].width / zakres + proces;
		koniec = (proces + 1) * piksele[0].width / zakres + proces + 1;
	    }
	    else						//
	    {
		start = proces * piksele[0].width / zakres + piksele[0].width % zakres;
		koniec = (proces + 1) * piksele[0].width / zakres + piksele[0].width % zakres;
	    }
	    */
	    if(dodany > 0)
	    {
	        start += 1*proces;
	        koniec += 1*proces;
	    }
	    
	    for(int x = start; x < koniec/*piksele[0].width*/; x++)
	    {
		    for(int y = 0; y < piksele[0].height; y++)
		    {
			i=0;
			for(int xp = 0; xp < piksele[0].width; xp++)
			{
			    for(int yp = 0; yp < piksele[0].height; yp++)
			    {
			    double kernel = g(abs(xp-x),abs(yp-y),sigma);
				if((float)piksele[i].R < 0)
				{
				    skladoweR += kernel * (float)piksele[i].R + 255;
				}
				else
				{
				    skladoweR += kernel * (float)piksele[i].R + 0;
				}
				
				if((float)piksele[i].G < 0)
				{
				    skladoweG += kernel * (float)piksele[i].G + 255;
				}
				else
				{
				    skladoweG += kernel * (float)piksele[i].G + 0;
				}
				
				if((float)piksele[i].B < 0)
				{
				    skladoweB += kernel * (float)piksele[i].B + 255;
				}
				else
				{
				    skladoweB += kernel * (float)piksele[i].B + 0;
				}
				wG += kernel;
				if(i<rozmiar_obrazka)
				{
				    i++;
				}
				printf("wew pętla: %d\n", l);
				l++;
			    }
			}
			
			skladoweR = skladoweR / wG;
			skladoweG = skladoweG / wG;
			skladoweB = skladoweB / wG;
			TABwyj[z].R = skladoweR;
			TABwyj[z].G = skladoweG;
			TABwyj[z].B = skladoweB;
			TABwyj[z].beg = x;
			TABwyj[z].end = y;
			skladoweR = skladoweG = skladoweB = wG = 0;
			z++;
			
		    }
	    }	
	    /*
	    for(int i = 0; i < rozmiar_obrazka; i++)
	    {
		ebmpBYTE denR = (ebmpBYTE) TABwyj[i].R;
		ebmpBYTE denG = (ebmpBYTE) TABwyj[i].G;
		ebmpBYTE denB = (ebmpBYTE) TABwyj[i].B;
		plik(TABwyj[i].beg, TABwyj[i].end)->Red = denR;
		plik(TABwyj[i].beg, TABwyj[i].end)->Green = denG;
		plik(TABwyj[i].beg, TABwyj[i].end)->Blue = denB;
	    }
	    plik.WriteToFile(argv[3]);
	    */
	    
	}
	else
	{
	    printf("1\n");
	    //bitmap_info obrazek(rozmiar_obrazka);
	    //piksel obrazek[214312];
	    printf("2\n");
	    //int rozmiar_obrazka;
	    MPI_Bcast(&rozmiar_obrazka, 1, MPI_INT, 0, MPI_COMM_WORLD); 	//odbieram rozmiar tablicy ba innych procesach
	    
	    piksele = new piksel[rozmiar_obrazka];
	    
	    MPI_Bcast(piksele, rozmiar_obrazka, MPI_INT, 0, MPI_COMM_WORLD); //odbieram piksele na innych procesach (tab. piksele bez & !)
	    //MPI_Rcv(rozmiar_obrazka, 1, MPI_INT, proces, 0, MPI::COMM_WORLD);
	    printf("dlugosc obrazka: %d\n", piksele[0].width);
	    printf("rozmiar obrazkaELSE: %d\n", rozmiar_obrazka);
	    
	    
	    TABwyj = new piksel[rozmiar_obrazka];
	    
	    porcja = piksele[0].width / zakres;
	    dodany = piksele[0].width % zakres;
	    start = porcja*proces;
	    koniec = porcja*proces+porcja;
	    /*
	    if(proces < piksele[0].width % zakres)		// podział pracy między procesy
	    {
		start = proces * piksele[0].width / zakres + proces;
		koniec = (proces + 1) * piksele[0].width / zakres + proces + 1;
	    }
	    else						//
	    {
		start = proces * piksele[0].width / zakres + piksele[0].width % zakres;
		koniec = (proces + 1) * piksele[0].width / zakres + piksele[0].width % zakres;
	    }
	    */
	    if(dodany > 0)
	    {
	        start += 1*proces;
	        koniec += 1*proces;
	    }
	    
	    //for(int x = start; x < koniec/*piksele[0].width*/; x++)
	    /*{
		    for(int y = 0; y < piksele[0].height; y++)
		    {
			i=0;
			for(int xp = 0; xp < piksele[0].width; xp++)
			{
			    for(int yp = 0; yp < piksele[0].height; yp++)
			    {
			    double kernel = g(abs(xp-x),abs(yp-y),sigma);
				if((float)piksele[i].R < 0)
				{
				    skladoweR += kernel * (float)piksele[i].R + 255;
				}
				else
				{
				    skladoweR += kernel * (float)piksele[i].R + 0;
				}
				
				if((float)piksele[i].G < 0)
				{
				    skladoweG += kernel * (float)piksele[i].G + 255;
				}
				else
				{
				    skladoweG += kernel * (float)piksele[i].G + 0;
				}
				
				if((float)piksele[i].B < 0)
				{
				    skladoweB += kernel * (float)piksele[i].B + 255;
				}
				else
				{
				    skladoweB += kernel * (float)piksele[i].B + 0;
				}
				wG += kernel;
				if(i<rozmiar_obrazka)
				{
				    i++;
				}
				printf("wew pętla: %d\n", l);
				l++;
			    }
			}
			
			skladoweR = skladoweR / wG;
			skladoweG = skladoweG / wG;
			skladoweB = skladoweB / wG;
			TABwyj[z].R = skladoweR;
			TABwyj[z].G = skladoweG;
			TABwyj[z].B = skladoweB;
			TABwyj[z].beg = x;
			TABwyj[z].end = y;
			skladoweR = skladoweG = skladoweB = wG = 0;
			z++;
			
		    }
	    }
	    
	    //int skladoweR, skladoweG, skladoweB;
	    //double wG;
	    
	    //porcja = rozmiar_obrazka / zakres;
	    //dodany = rozmiar_obrazka - zakres * porcja;
	    /*
	    if(proces < rozmiar_obrazka % zakres)		// podział pracy między procesy
	    {
		start = proces * rozmiar_obrazka / zakres + proces;
		koniec = (proces + 1) * rozmiar_obrazka / zakres + proces + 1;
	    }
	    else						//
	    {
		start = proces * rozmiar_obrazka / zakres + rozmiar_obrazka % zakres;
		koniec = (proces + 1) * rozmiar_obrazka / zakres + rozmiar_obrazka % zakres;
	    }
	    */
	    
	    
	
	}

MPI::Finalize();
}