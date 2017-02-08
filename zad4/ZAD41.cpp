#include "EasyBMP.h"
using namespace std;

int main( int argc, char* argv[] )
{
	double suma = 0;
	long suma_long = 0;
	double srd_lum = 0;

	// Deklaracja i czytanie bitmpay
	BMP plik;
	plik.ReadFromFile( argv[1] );

	for( int j = 0 ; j < plik.TellHeight() ; j++)
	{
		for( int i = 0 ; i < plik.TellWidth() ; i++)
		{
			double Temp = (int)( 0.299*plik(i,j)->Red +
			0.587*plik(i,j)->Green +
			0.114*plik(i,j)->Blue );

			suma += Temp;
		}
	}
	
	suma_long = suma;

	srd_lum = ( suma / (plik.TellWidth()*plik.TellHeight()) );

	cout << "Suma: " << suma_long << endl;
	cout << "Srednia luminancja bitmapy: " << srd_lum << endl;
	
	
	//Konwersja na szaro
	for( int j = 0 ; j < plik.TellHeight() ; j++)
	{
		for( int i = 0 ; i < plik.TellWidth() ; i++)
		{
			int Temp = (int)( 0.299*plik(i,j)->Red +
			0.587*plik(i,j)->Green +
			0.114*plik(i,j)->Blue );

			if(Temp > srd_lum)
			{
				plik(i,j)->Red = 0;
				plik(i,j)->Green = 0;
				plik(i,j)->Blue = 0;
			}
			else
			{
				plik(i,j)->Red = 255;
				plik(i,j)->Green = 255;
				plik(i,j)->Blue = 255;
			}
		}
	}
	// Zapisanie pliku wyjściowego
	plik.WriteToFile( argv[2] );

	return 0;
}