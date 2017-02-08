#include "EasyBMP.h"
#include<stdio.h>

//using namespace std;

/*
 * Struktura zawierająca kolory (0 - 255);
 */
struct RGB
{
	int R;
	int G;
	int B;
};

/*
 * Funkcja zwracająca otwarty plik
 */

//BMP OpenFile(char* path)
//{
//	BMP file;
//	file.ReadFromFile(path);
//	return file;
//};

/*
 * Funkcja zczytuje kolory z pliku
 * i zwraca strukturę z przypisanymi wartościami
 */
RGB Colors(BMP file)
{
	RGB colors;
	for(int y = 0; y < file.TellHeight(); y++)
	{
		for(int x = 0; x < file.TellWidth(); x++)
		{
			colors.R += file(x, y)->Red;
			colors.G += file(x, y)->Green;
			colors.B += file(x, y)->Blue;
			
			
			
		}
	}
	return colors;
};

/*
 Zwraca luminancję podstawowych kolorów.
 */
int Luminance(RGB colors)
{
	return (int)(0.299 * colors.R + 0.587 * colors.G + 0.114 * colors.B);
};

/*

 Zamienia każdy piksel bitmapy na biały kolor 
 (gdy jej luminancja jest większa niż luminancja podana)
 lub na czarny kolor (w przeciwnym wypadku).
 Zwraca plik plik wyjściowy BMP

*/

BMP ConvertWhiteBlack(BMP file, int luminance) // tutaj zrobic wskaznik  bmp file
{
	int lum;
	for(int y = 0; y < file.TellHeight(); y++)
	{
		for(int x = 0; x < file.TellWidth(); x++)
		{
			lum = (int) (0.299 * file(x,y)->Red + 0.587 * file(x,y)->Green + 0.114 * file(x,y)->Blue);	//luminancja pojedynczego piksela
			if(lum > luminance)			//zmienia piksel na biały
			{
				file(x,y)->Red = 255;
				file(x,y)->Green = 255;
				file(x,y)->Blue = 255;
			}
			else						//zmienia piksel na czarny
			{
				file(x,y)->Red = 0;
				file(x,y)->Green = 0;
				file(x,y)->Blue = 0;
			}
		}
	}
	return file;
}

int main( int argc, char* argv[] )
{
	BMP file;
	//OpenFile( argv[1] );
	
	file.ReadFromFile( argv[1] );
	printf("1\n");
	//cout << "1" << endl;
	RGB colors = Colors(file);
	printf("2\n");
	//cout << "2" << endl;
	int luminance = Luminance(colors);
	printf("3\n");
	//cout << "3" << endl;
	file = ConvertWhiteBlack(file, luminance);
	printf("4\n");
	//cout << "4" << endl;
	printf("Przekonwertowano\n");
	file.WriteToFile( argv[2] );
	printf("5\n");
	//cout << "5" << endl;
	printf("Zapisano\n");
}