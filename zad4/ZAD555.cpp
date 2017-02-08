#include <iostream> 
#include <cstdio> 
#include <cmath> 
#include <cstdlib> 
#include <vector>
#include  "EasyBMP.h"
#include "mpi.h"
using namespace std; 


struct INPUT{
	public:
	char r; // 1
	char g; // 1
	char b; // 1
	int beg; // 4
	int end; // 4 
}__attribute__((packed));

double g(int x, int y, double sigma){
		return (double)( 1 / (2*M_PI * (sigma *sigma))*exp(-(( x*x + y*y)/(2*(sigma*sigma)))));
}

int main(int argc, char* argv[]) {
	
	int currentProces; 
	int countOfProcess;
	int position;
	double sigma = argv[3];
	double denomianR = 0;
	double denomianG = 0;
	double denomianB = 0;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &currentProces);
    MPI_Comm_size(MPI_COMM_WORLD, &countOfProcess);
    
    BMP inputFile;
    inputFile.ReadFromFile(argv[1]);
    
    int sizes = inputFile.TellWidth() * inputFile.TellHeight();
    cout << sizeof(INPUT); 
    sizes *= sizeof(INPUT);
    sizes *= sizeof(int);
 	char buffor[sizes];
 	INPUT *POINT = new INPUT[sizes];
    
	
 	int xcount = inputFile.TellWidth()/ countOfProcess;
	int xremainder = inputFile.TellWidth() % countOfProcess;

	int xbeg = xcount * currentProces ; 
	int xend = xcount * currentProces + xcount;
	
	
	if(countOfProcess == 1){
		xbeg = 0;
		xend = inputFile.TellWidth();
	}
	// popraw
	if(xremainder >= 1 && currentProces > = countOfProcess - xremainder){ 
	  xbeg = xbeg + 1; 
	  xend = xend + 1; 
	  
    }
 	int pac = sizes;
 	INPUT *RESULT = new INPUT[pac];
	INPUT *ALLRESULT;
	double resultG;
	int z = 0;
	int i = 0;
	
	
	if(currentProces == 0){
     
      ALLRESULT = new INPUT[sizes];
      int x = inputFile.TellWidth(); 
      int y = inputFile.TellHeight(); 
      position = 0;
		
		i  = 0; 
		double t0 = MPI_Wtime(); // TIME BEGIN
	  	for(int x = 0; x<inputFile.TellWidth(); x++){
			for(int y = 0; y<inputFile.TellHeight(); y++){
				POINT[i].r = (char)inputFile(x,y)->Red;
				POINT[i].g = (char)inputFile(x,y)->Green;
				POINT[i].b = (char)inputFile(x,y)->Blue;
				i++;
			}
		}
		double t1 = MPI_Wtime(); // TIME BEGIN
		double t0 = MPI_Wtime(); // TIME BEGIN
		for ( int x = 0 ; x<xend; x++) {
			for ( int y = 0; y<inputFile.TellHeight(); y++) {
				i = 0;
				for ( int xPrim = 0; xPrim <inputFile.TellWidth(); xPrim++){
					for( int yPrim = 0; yPrim <inputFile.TellHeight(); yPrim++){
						double res = g(abs(xPrim - x), abs(yPrim-y) ,sigma); 
						denomianR += res * ((float)POINT[i].r < 0 ? (float)POINT[i].r + 255 : (float)POINT[i].r + 0 );
						denomianG += res * ((float)POINT[i].g < 0 ? (float)POINT[i].g + 255 : (float)POINT[i].g + 0 );
						denomianB += res * ((float)POINT[i].b < 0 ? (float)POINT[i].b + 255 : (float)POINT[i].b + 0 ); 
						resultG += res;
						i++; 
					} 
				}
				
				denomianR = denomianR / resultG; 
				denomianG = denomianG / resultG; 
				denomianB = denomianB / resultG; 
				RESULT[z].r = denomianR;
				RESULT[z].g = denomianG;
				RESULT[z].b = denomianB;  
				RESULT[z].beg = x; 
				RESULT[z].end = y;
				denomianR = denomianG = denomianB = resultG = 0;
				z++;
			} 
		}
		double t1 = MPI_Wtime(); // TIME BEGIN
		MPI_Pack(POINT, sizes, MPI_CHAR, buffor, sizes, &position, MPI_COMM_WORLD);
		MPI_Bcast(buffor, sizes, MPI_PACKED, 0, MPI_COMM_WORLD);
		MPI_Gather(RESULT,pac, MPI_INT, ALLRESULT, pac, MPI_INT, 0, MPI_COMM_WORLD);
   } 
   
   if(currentProces > 0){
	    
		MPI_Bcast(buffor, sizes, MPI_PACKED, 0, MPI_COMM_WORLD);
		position = 0; 
		MPI_Unpack(buffor, sizes, &position, POINT, sizes, MPI_CHAR, MPI_COMM_WORLD); 
		double t0 = MPI_Wtime(); // TIME BEGIN
		for ( int x = xbeg; x<xend; x++) {
			for ( int y = 0; y<inputFile.TellHeight(); y++) {
				i = 0;
				for ( int xPrim = 0; xPrim <inputFile.TellWidth(); xPrim++){
					for( int yPrim = 0; yPrim <inputFile.TellHeight(); yPrim++){
						double res = g(abs(xPrim - x), abs(yPrim-y) ,sigma); 
						denomianR += res * ((float)POINT[i].r < 0 ? (float)POINT[i].r + 255 : (float)POINT[i].r + 0 );
						denomianG += res * ((float)POINT[i].g < 0 ? (float)POINT[i].g + 255 : (float)POINT[i].g + 0 );
						denomianB += res * ((float)POINT[i].b < 0 ? (float)POINT[i].b + 255 : (float)POINT[i].b + 0 ); 
						resultG += res;
						i++; 
					} 
				}
				
				denomianR = denomianR / resultG; 
				denomianG = denomianG / resultG; 
				denomianB = denomianB / resultG; 
				RESULT[z].r = denomianR;
				RESULT[z].g = denomianG;
				RESULT[z].b = denomianB;
				RESULT[z].beg = x; 
				RESULT[z].end = y;
				denomianR = denomianG = denomianB = resultG = 0;
				z++;
			} 
		}
		double t0 = MPI_Wtime(); // TIME BEGIN
		MPI_Gather(RESULT,pac, MPI_INT, ALLRESULT, pac, MPI_INT, 0, MPI_COMM_WORLD);
	}
	if(currentProces == 0){
		for(int i =0; i<sizes;i++){
				ebmpBYTE denR = (ebmpBYTE) ALLRESULT[i].r;
				ebmpBYTE denG = (ebmpBYTE) ALLRESULT[i].g;
				ebmpBYTE denB = (ebmpBYTE) ALLRESULT[i].b;
				inputFile(ALLRESULT[i].beg,ALLRESULT[i].end)->Red   = denR;
				inputFile(ALLRESULT[i].beg,ALLRESULT[i].end)->Green = denG;
				inputFile(ALLRESULT[i].beg,ALLRESULT[i].end)->Blue  = denB;
		}
		inputFile.WriteToFile(argv[2]);
	}
   MPI_Finalize();
}