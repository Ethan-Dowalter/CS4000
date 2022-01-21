//
// A simple CS Coin miner program for CS 4000, Homework #5
//
// The program reads in two big integers l and r, and stores them in the
// boost data structure cpp_int.
//
// This CS Coin miner starts at 0 and iteratively tries all integers until
// it finds an integer z such that l < shar256(z) < r.
// It then outputs z and quits.
//
// Written by David W. Juedes, April 9th, 2021.
//
//

/**
 *        @file: cs_miner_openMPI.cc
 *      @author: Ethan Dowalter
 *        @date: April 16, 2021
 *       @brief: Parallel algorithm for mining CS Coins using openMPI
 */

#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <iostream>
#include <iomanip>
#include <openssl/sha.h>
#include <cassert>
#include <mpi.h>
using namespace std;
using namespace boost::multiprecision;


//
// Function: to_bits
// Converts a cpp_int x into a vector of unsigned characters (8-bit ints).
// 
vector<unsigned char> to_bits(cpp_int x)
{
	unsigned char c;

	vector<unsigned char> buffer;
	while (x>0) {
		c = (unsigned char) x%256;
		buffer.push_back(c);
		x=x/256;
	}

	return buffer;
}

//
// Function: to_cpp_int
// Converts a vector of unsigned characters (8-bit ints) c into a cpp_int.
// 
cpp_int to_cpp_int(vector <unsigned char> c)
{
	cpp_int x = 0;
	for (size_t i = c.size(); i > 0; i--)
	{
		x = x * 256 + c[i - 1];
	}
	return x;
}

//
// Function sha256(x)
// Applies the standard sha256 algorithm from openssl to a cpp_int.
// Returns the corresponding cpp_int.
// Note: SHA256_DIGEST_LENGTH is 32.
// (8*32 = 256).
//
cpp_int sha256(const cpp_int x)
{
	vector<unsigned char> int_buffer;
	int_buffer = to_bits(x);
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256(&int_buffer[0],int_buffer.size(),hash);
	cpp_int total = 0;  
	for (int i=0;i<SHA256_DIGEST_LENGTH;i++) {
		total = total*256+(int) hash[i];
	}

	return total;
}

int main(int argc, char *argv[]) {
	///Do the normal MPI initialization
	int num_processes;
	int process_ID;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD,&process_ID);
	
	cpp_int t1;
	cpp_int t2;
	if (process_ID == 0)
	{
		cin >> t1;
		cin >> t2;
		assert(t1 < t2);
	}

	///Broadcast the data to all other processes
	vector <unsigned char> t1bits = to_bits(t1);		//must convert cpp_ints to arrays first to broadcast them
	vector <unsigned char> t2bits = to_bits(t2);
	t1bits.resize(32);		//should always be a size 32 array, so we can skip broadcasting size of arrays because it is constant
	t2bits.resize(32);
	///Broadcast the entire array
	for (size_t i = 0; i < 32; i++)
	{
		MPI_Bcast(&t1bits[i],1,MPI_INT,0,MPI_COMM_WORLD);
		MPI_Bcast(&t2bits[i],1,MPI_INT,0,MPI_COMM_WORLD);
	}
	///Convert back to cpp_int
	t1 = to_cpp_int(t1bits);
	t2 = to_cpp_int(t2bits);

	///Now all processes have the data and can start mining for CS coins	
	cpp_int t = process_ID;			//set t to process_ID
	cpp_int next_sha;
	bool found = false;
	int n = 0;						//int to count how many tries there have been, used to limit communications between processes
	while (!found) {
		next_sha = sha256(t);
		if (t1 < next_sha) {
			if (next_sha < t2) {
				cout << t << endl;
				found = true;
			}
		}
		t = t + num_processes;		//increment t by num_processes, which makes algorithm a round robin approach
		n++;						//increment the counter for number of tries
		if (n == 1000 || found)		//only communicate every 1000 tries if a solution has been found
		{
			MPI_Allreduce(&found, &found, 1, MPI_CXX_BOOL, MPI_LOR, MPI_COMM_WORLD);
			n = 0;
		}
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}



