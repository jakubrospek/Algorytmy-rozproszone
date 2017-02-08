#include <vector>
#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <sstream>

using namespace std;

class cvec {
public:
	double x;
	double y;
	double z;
	cvec() : x(0.0), y(0.0), z(0.0) {}
	cvec(double a, double b, double c) : x(a), y(b), z(c) {}
	double length() {
		double l = sqrt(x*x + y*y + z*z);
		return l;
	}
	~cvec(){}
};

struct sendv {
	double x;
	double y;
	double z;
	double l;
};

struct sendt {
	double t_read;
	double t_proc;
	double t_red;
	double t_total;
};

int main() {
	/*
	/ initializing
	*/
	bool para = true;
    vector<cvec> values;
    cvec tmp;
	sendv local;
	sendv global;
	sendt times;
    MPI::Init();
	MPI::Status status;
	const char* path = "v01.dat";
	int size, portion, added;
	double t_start, t_end;	//start time and end time
	double t_read, t_proc, t_red, t_total;	//local
	double l = 0.0, global_l = 0.0, global_x = 0.0, global_y = 0.0, global_z = 0.0; 
	cvec r;
	int beg = 0, end = -1;						//index of first and last cvec
    int n_proc = MPI::COMM_WORLD.Get_size();	//number of processes
    int rank = MPI::COMM_WORLD.Get_rank();		//rank of process
    if(para == true && rank == 0)
		printf("Para is true\n");
	else if(rank == 0)
		printf("Para is false\n");
	/*
	/ reading para non
	*/
	if(para == false) {
		ifstream myfile;
		t_start = MPI::Wtime();
		myfile.open(path);
		while(true) {
			myfile >> tmp.x >> tmp.y >> tmp.z;
			if(myfile.good())
				values.push_back(tmp);
			else
				break;
		}
		t_end = MPI::Wtime();
		times.t_read = t_end - t_start;	//read time
		myfile.close();
		//computing portions for nonpara
		t_start = MPI::Wtime();
		size = values.size();
		portion = size / n_proc;
		added = size - n_proc * portion;	//processes with rank < added will get +1 to portion
		if(rank == 0) {
			printf("\nPortion: %d Added: %d\n", portion, added);
		}
		for(int i = 0; i < rank + 1; i++) {
			beg = end + 1;
			end = beg + portion;
			if(i < added) {
				end += 1;
			}
		}
		r.x = 0.0; r.y = 0.0; r.z = 0.0;
		for(int i = beg; i <= end; i++) {
			r.x += values[i].x;
			r.y += values[i].y;
			r.z += values[i].z;
			l += values[i].length();
		}
		t_end = MPI::Wtime();
		times.t_proc = t_end - t_start;
	}
	/*
	/ reading para
	*/
	if(para == true) {
		MPI::File mpifile;
		mpifile = MPI::File::Open(MPI::COMM_WORLD, path, MPI::MODE_RDONLY, MPI::INFO_NULL);
		MPI::Offset filesize;
		MPI::Offset file_pointer;
		filesize = mpifile.Get_size(); //in bytes
		//computing portions for para
		t_start = MPI::Wtime();
		size = (filesize / 40); //number of lines (vectors)
		portion = size / n_proc;
		added = size - n_proc * portion;	//processes with rank < added will get +1 vector to portion
		end = 0;
		if(rank == 0) {
			printf("\nSize: %d Portion: %d Added: %d", size, portion, added);
		}
		t_start = MPI::Wtime();
		for(int i = 0; i < rank + 1; i++) {
			beg = end;
			end = beg + portion;
			if(i < added) {
				end += 1;
			}
		}
		t_end = MPI::Wtime();
		times.t_proc = t_end - t_start;
		int bufsize;
		bufsize = 40 * (end - beg) + 1;	//40 chars for 1 vector + 1 empty char
		//printf("\nProc %d portion: %d, begline: %d, endline: %d, bufsize: %d", rank, end-beg, beg, beg + (bufsize / 40), bufsize);
		char *buf;
		buf = new char[bufsize];
		file_pointer = 40 * beg; //begin reading from this character
		t_start = MPI::Wtime();
		mpifile.Read_at(file_pointer, buf, bufsize - 1, MPI_CHAR);
		t_end = MPI::Wtime();
		times.t_read = t_end - t_start;
		//printf("\nProc %d:\n%s", rank, buf);
		stringstream stream;
		t_start = MPI::Wtime();
		for(int i = 0; i < bufsize; i++) {
			stream << buf[i]; //chars to stringstream
		}
		for(int i = 0; i < end - beg; i++) {
			stream >> tmp.x;
			stream >> tmp.y;
			stream >> tmp.z;
			values.push_back(tmp);
			//printf("\nProc %d\tX: %lf Y: %lf Z: %lf", rank, tmp.x, tmp.y, tmp.z);
		}
		r.x = 0.0; r.y = 0.0; r.z = 0.0;
		for(int i = 0; i < end - beg; i++) {
			r.x += values[i].x;
			r.y += values[i].y;
			r.z += values[i].z;
			l += values[i].length();
		}
		t_end = MPI::Wtime();
		times.t_proc += t_end - t_start;
	}
	
	//printf("\nProc %d:\t L: %lf\t X: %lf\t Y: %lf\t Z: %lf", rank, l, r.x, r.y, r.z);
	/*
	/ reduce
	*/
	local.x = r.x;
	local.y = r.y;
	local.z = r.z;
	local.l = l;
	t_start = MPI::Wtime();
	MPI::COMM_WORLD.Reduce(&local, &global, 4, MPI::DOUBLE, MPI::SUM, 0);
	t_end = MPI::Wtime();
	//printf("\nProc %d ENDED with reduction", rank);
	times.t_red = t_end - t_start;					//reduce time
	times.t_total = times.t_read + times.t_proc + times.t_red;			//total time
	//printf("Process nr %d :\n  read: %lf\n  process: %lf\n  reduce: %lf\n  TOTAL: %lf\n", rank, t_read, t_proc, t_red, t_total);
	sendt st;
	if(rank != 0) {
		MPI::COMM_WORLD.Send(&times, 4, MPI::DOUBLE, 0, rank);
	}
	if(rank == 0) {
		global.x = global.x / size;
		global.y = global.y / size;
		global.z = global.z / size;
		global.l = global.l / size;
		//printf("\nProc %d ENDED with globals", rank);
		cout << setprecision(6) << "\nL: " <<  global.l << "\nX: " << global.x << "\nY: " << global.y << "\nZ: " << global.z << "\n";
		ofstream outfile;
		outfile.open("outfile.txt");
		outfile << "timings (proc 0):\n  read data:\t\t" << times.t_read << "\n  process data:\t" << times.t_proc << "\n  reduce results:\t" << times.t_red << "\n  total:\t\t" << times.t_total << "\n";
		for(int i = 1; i < n_proc; i++) {
			MPI::COMM_WORLD.Recv(&times, 4, MPI::DOUBLE, i, i, status);
			outfile << "timings (proc " << i << "):\n  read data:\t\t" << times.t_read << "\n  process data:\t" << times.t_proc << "\n  reduce results:\t" << times.t_red << "\n  total:\t\t" << times.t_total << "\n";
		}
	}
    MPI::Finalize();
    return 0;
}