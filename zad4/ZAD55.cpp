#include<sstream>
#include"EasyBMP.h"
#include<mpi.h>

using namespace std;

int main(int argc, char** argv)
{
	MPI::Init(argc, argv);    
    int n = MPI::COMM_WORLD.Get_size(); 
    int rank = MPI::COMM_WORLD.Get_rank();
	
	int * RGB;
	int pojemnosc;
	if(rank == 0)
	{
		BMP obrazek;
		obrazek.ReadFromFile(argv[1]);
		
		int szerokosc = obrazek.TellWidth();
		int wysokosc = obrazek.TellHeight();
		int rozmiar = szerokosc * wysokosc;
		pojemnosc = 3 * rozmiar + 3;
		
		int R[rozmiar];
		int G[rozmiar];
		int B[rozmiar];
		/*int *R ;
		R = new int[rozmiar];
		/*int *G;
		G = new int[rozmiar];
		int *B;
		B = new int[rozmiar];*/
		int a = 0, b = 0;
		RGB = new int [pojemnosc];
		
		for (int i = 0; i < wysokosc; i++)
		{
			
			for (int j = 0; j < szerokosc; j++)
			{
				RGBApixel Temp = obrazek.GetPixel(j, i);
				
				R[a] = Temp.Red;
				G[a] = Temp.Green;
				B[a] = Temp.Blue;
				a++;
				RGB[b] = R[b];
				b++;
			}
		}
		
		for(int i = 0; i < rozmiar; i++)
		{
			RGB[b] = G[b];
			b++;
		}
		
		for(int i = 0; i < rozmiar; i++)
		{
			RGB[b] =B[b];
			b++;
		}
		RGB[pojemnosc - 2] = wysokosc;
		RGB[pojemnosc - 1] = szerokosc;	
		RGB[pojemnosc - 3] = rozmiar;
		
		for(int i = 1; i < n; i++)
		{
			MPI::COMM_WORLD.Send(&pojemnosc, 1, MPI::INTEGER, i, 0);
		}
		
		//double NR[wys][sze], NG[wys][sze], NB[wys][sze];
		
		
		//zapisywanie na obrazek
		//obrazek.WriteToFile(argv[2]);
	}
	
	if(rank != 0)
	{
		MPI::COMM_WORLD.Recv(&pojemnosc, 1, MPI::INTEGER, 0, 0); //sprawdziæ to czy nie mo¿e byæ if(rank !=0)
	
		RGB = new int [pojemnosc];
	}
	MPI::COMM_WORLD.Bcast(RGB, pojemnosc, MPI::INTEGER, 0);
	//DEKOMPOZYCJA
	int s_dek = 0, k_dek = 0;
	
	int wys, sze, pix;
	wys = RGB[pojemnosc - 2];
	sze = RGB[pojemnosc - 1];
	pix = RGB[pojemnosc - 3];
	//cout<<RGB[1]<<endl;
	//int roz = pojemnosc - 3; //prawdopodomnie jeszcze - 3
	int roz = 3*RGB[pojemnosc - 1];
	if(rank < roz % n)
	{
		s_dek = rank * roz / n + rank;
	}
	else
	{
		s_dek = rank * roz / n + roz % n;
	}
	
	if(rank < roz % n)
	{
		k_dek = (rank + 1) * roz / n + (rank + 1);
	}
	else
	{
		k_dek = (rank + 1) * roz / n + roz % n;
	}
	
	//Sigma
	stringstream s;
	s << argv[2];
	double sigma;
	s >> sigma;
	
	double pi = 3.14159;
	
	//rozpakowanie RGB
	
	
	int Cz[wys][sze], Z[wys][sze], N[wys][sze];
	int a = 0;
	
	for(int i = 0; i < wys; i++)
	{
		for(int j = 0; j < sze; j++)
		{
			Cz[i][j] = RGB[a];
			Z[i][j] = RGB[a+ pix];
			N[i][j] = RGB[a + pix + pix];
			a++;
			//if(rank == 0)
			//cout<<Cz[i][j]<<" ";
		}
	}
	delete[] RGB;
	double g[wys][sze];
	double QR[wys][sze], QG[wys][sze], QB[wys][sze];
	
	//zmieniæ do dekompozycji
	for(int i = 0; i < wys; i++)
	{
		for(int j = s_dek; j< k_dek; j++)
		{
			g[i][j] = ((1.0/(2.0*M_PI*pow(sigma,2)))*exp(-1.0*((pow(i,2)+ pow(j,2))/(2.0*pow(sigma, 2)))));
			//if(rank == 0)
			//{
			//cout<<g[i][j]<<" ";
			//}
		}
	}
	double q = 0.0, q2 = 0.0, q3 = 0.0;
	double gora[3] = {0.0, 0.0, 0.0};
	double dol[3] = {0.0, 0.0, 0.0};
	int x, y;
	for(int i = 0; i < wys; i++)
	{
		for(int j = s_dek; j < k_dek; j++)
		{ 
		////  sum
			for(int k = 0; k < wys; k++)
			{
				for(int l = s_dek; l < k_dek; l++)
				{
					x = ((i-k)<0?-(i-k):(i-k));
					y = ((j-l)<0?-(j-l):(j-l));
					gora[0] = gora[0] + ((Cz[k][l] * g[x][y]));
					dol[0] = dol[0] + (g[x][y]);
					gora[1] = gora[1] + ((Z[k][l] * g[x][y]));
					dol[1] = dol[1] + (g[x][y]);
					gora[2] = gora[2] + ((N[k][l] * g[x][y]));
					dol[2] = dol[2] + (g[x][y]);
					
				}
			}
			q = gora[0] / dol[0];
			q = gora[1] / dol[1];
			q = gora[2] / dol[2];
			for(int m = 0; m < 3; m++)
			{
				gora[m] = 0.0, dol[m] = 0.0;
			}
			///////////
			QR[i][j] = q, QG[i][j] = q2, QB[i][j] = q3;
			q = 0.0, q2 = 0.0, q3 = 0.0;
			//if(rank == 0)
			//cout<<QR[i][j]<< " ";
		}
	}
	
	//double CZN[wys*3][sze*3];
	double CZN[pix];
	int z =0, z2;
	for(int i = 0; i < wys; i++)
	{
		for(int j = 0; j< sze; j++)
		CZN[z] = Cz[i][j];
		z++;
	}
	for(int i = 0; i < wys; i++)
	{
		for(int j = 0; j < roz; j++)
		{

			CZN[z] = Cz[i][j];
			z++;
		}
	}
	for(int i = 0; i < wys; i++)
	{
		for(int j = 0; j < sze; j++)
		{
			CZN[z] = Z[i][j];
			z++;
		}
	}
	/*for(int i = 0; i < wys; i++)
	{
		for(int j = 0; j < sze; j++)
		{
			CZN[i][j] = N[z][z2];
			z++, z2++;
		}
	}*/
	//tu musi byæ gether, albo to nie trzeba getherem 
	//do zastanowienia siê
	double N_CZN[pix];
	MPI::COMM_WORLD.Gather(&CZN, pix, MPI::DOUBLE,&N_CZN, pix, MPI::DOUBLE, 0);
	
	//na root dopisaæ buffor odbiorcy
	//MPI::COMM_WORLD.Reduce(&CZN, &buffor, pojemnosc, MPI::DOUBLE, MPI::SUM, 0);
	
	
	MPI::Finalize();
	return 0;
}