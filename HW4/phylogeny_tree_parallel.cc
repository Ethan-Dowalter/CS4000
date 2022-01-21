//
// A Basic C++ implementation of
// a program that computes a Phylogenetic tree from COVID-19 Virus sequences
// Each virus sequence has about 30K base pairs.
// Computing the LCS takes about 7 seconds.
//
// Basic algorithm to build the phylogeny:
// Compute all LCS (longest common subsequence) for all pairs of
// strings.
// Pick the best pair.
// Merge the best into a new string that is the LCS of those two.
// Continue until only one sequence remains.
//
// Written by David W. Juedes and built upon by Ethan Dowalter
//

/**
 *        @file: phylogeny_tree_parallel.cc
 *      @author: Ethan Dowalter
 *        @date: March 31, 2021
 *       @brief: Parallel algorithm to find longest common substring from a file of COVID-19 genomes
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string>
#include <cassert>
#include <mpi.h>
using namespace std;

// This code checks whether 
// x[i..n-1] a sequence of y[j..m-1], where n is the length of
// x, m is the length of y.
// This is solely used to check the correctness of the solution.

bool Is_subsequence(size_t i, size_t j, string &x, string &y) {
	if (i>=x.length()) return true; // Consumed all of x's characters.
	if (j>=y.length()) return false; // Can't consume y's first.

	if (x[i]==y[j]) {
		return Is_subsequence(i+1,j+1,x,y);
	} else {
		return Is_subsequence(i,j+1,x,y);
	}
}


int max3(int x,int y, int z) {
	return max(max(x,y),z);
}
int max3_loc(int x,int y, int z) {
	if (max3(x,y,z) == x) return 1;
	if (max3(x,y,z) == y) return 2;
	if (max3(x,y,z) == z) return 3;
	return -1;
}
int max_loc(int x,int y) {
	if (max(x,y) == x) return 1;
	else {return 2;}
}

//
// Recursively construct the longest common subsequence from the dynamic
// programming table "from"
//
string rec_string(string &x1, string &y1, vector<vector<int> > &LCS, int m, int n) {
	if (n==0) return "";
	if (m==0) return "";
	int a = LCS[m-1][n-1];
	int b = LCS[m-1][n];
	int c = LCS[m][n-1];
	// Remember, we are off by 1.
	if (x1[m-1] == y1[n-1]) {
		if (((a+1) >=b) && ((a+1) >=c)) {
			return rec_string(x1,y1,LCS,m-1,n-1)+x1[m-1];
		} else {
			if (b>=c) {
				return rec_string(x1,y1,LCS,m-1,n);
			} else {
				return rec_string(x1,y1,LCS,m,n-1);
			}
		}
	} else {
		if (b>=c) {
			return rec_string(x1,y1,LCS,m-1,n);
		} else {
			return rec_string(x1,y1,LCS,m,n-1);
		}
	}
}

  

string compute_LCS(string &x1, string &y1) {
	vector<vector<int> > LCS;
	// vector<vector<pair<int, pair<int,int> > > > from;

	//from.resize(x1.length()+1);
	LCS.resize(x1.length()+1);
	for (size_t i=0;i<=x1.length();i++) {
		LCS[i].resize(y1.length()+1);
		//from[i].resize(y1.length()+1);
	}
	LCS[0][0] = 0;
	//from[0][0] = make_pair(0,make_pair(-1,-1));
	for (size_t i=0;i<=x1.length();i++) {
		LCS[i][0]=0;
		//from[i][0] = make_pair(0,make_pair(-1,-1));
	}
	for (size_t i=0;i<=y1.length();i++) {
		LCS[0][i]=0;
		//from[0][i] = make_pair(0,make_pair(-1,-1));
	}

	for (size_t i=1;i<=x1.length();i++) {
		for (size_t j=1;j<=y1.length();j++) {
			if (x1[i-1]==y1[j-1]) {
				LCS[i][j] = max3(LCS[i-1][j-1]+1,LCS[i-1][j],LCS[i][j-1]);
				/*	switch (max3_loc(LCS[i-1][j-1]+1,LCS[i-1][j],LCS[i][j-1])) {
				case 1: from[i][j] = make_pair(1,make_pair(i-1,j-1));
					break;
				case 2: from[i][j] = make_pair(0,make_pair(i-1,j));
					break;
				case 3: from[i][j] = make_pair(0,make_pair(i,j-1));
					break;
					} */
			} else {
				LCS[i][j] = max(LCS[i-1][j],LCS[i][j-1]);
				/* switch (max_loc(LCS[i-1][j],LCS[i][j-1])) {
				case 1: from[i][j] = make_pair(0,make_pair(i-1,j));
					break;
				case 2: from[i][j] = make_pair(0,make_pair(i,j-1));
					break;
					} */
			} 
		}
	}
	//cout << LCS[x1.length()][y1.length()] << endl;
	string z = rec_string(x1,y1,LCS,x1.length(),y1.length());

	int temp = z.length();
	assert(temp == LCS[x1.length()][y1.length()]);
	assert(Is_subsequence(0,0,z,x1));
	assert(Is_subsequence(0,0,z,y1));

	return z;
}

// Always send from process 0.
void BroadCast_Receive_String(vector<char> &array) {
	// First, broadcast the size of the array....
	int buffer[2];
	buffer[0] = array.size();
	// Everybody executes the same code!
	MPI_Bcast(buffer,1,MPI_INT,0,MPI_COMM_WORLD);
	// Now, everyone knows the size....
	array.resize(buffer[0]);
	// Broadcast the entire array.
	MPI_Bcast(&array[0],buffer[0],MPI_CHAR,0,MPI_COMM_WORLD);
}

struct Intpair{		//used for MPI_Allreduce with MPI_2INT
	int size;
	int index;
};

/// Compute the longest common subsequence.
int main(int argc, char *argv[]) {

	///Do the normal MPI initialization
	int num_processes;
	int process_ID;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD,&process_ID);

	vector<string> genomes;			//gene sequences read from file
	vector<string> genome_ID;		//order genes were read from file
	///Use process 0 to get data from file
	if (process_ID == 0)
	{
		fstream fin;
		if (argc > 0)
		{
			fin.open(argv[1]);
			if (!fin.fail())
			{
				while (!fin.eof())
				{
					string line;
					getline(fin,line);
					if (!fin.fail())
					{
						genomes.push_back(line);
					}
				}
				cout << "Number of covid genomes = " << genomes.size() << endl;
			}
			else
			{
				cout << "Failed to Open File " << argv[1] << endl;
			}
		}
	}

	///Broadcast the data to all other processes
	// First, broadcast the size of the array....
	int buffer[2];
	buffer[0] = genomes.size();
	// Everybody executes the same code!
	MPI_Bcast(buffer,1,MPI_INT,0,MPI_COMM_WORLD);
	// Now, everyone knows the size....
	genomes.resize(buffer[0]);
	// Broadcast the entire array.
	for (size_t i = 0; i < genomes.size(); i++)
	{
		// First, broadcast the size of the string....
		buffer[0] = genomes[i].length();
		MPI_Bcast(buffer,1,MPI_INT,0,MPI_COMM_WORLD);
		// Now, everyone knows the size....
		genomes[i].resize(buffer[0]);
		// Broadcast the string.
		MPI_Bcast(&genomes[i][0],buffer[0],MPI_CHAR,0,MPI_COMM_WORLD);
	}
	
	///Initialize genome_ID vector
	for (size_t i = 0; i < genomes.size(); i++)
	{
		genome_ID.push_back(to_string(i));
	}
	
	///Main work loop - Merge strings to construct phylogenetic tree
	while (genomes.size() > 1) {
		///Debugging - left in for seeing progress updates
		if (process_ID == 0) cout << "Tree size = " << genomes.size() << endl;

		int max_i;
		int max_j;
		string best;
		bool start = true;

		///Make a vector of all possible pairs to flatten the for loop, which makes it easy to run in parallel
		vector <pair<int,int>> pairs;						//stores all possible pairings of genomes
		for (size_t i = 0; i < genomes.size(); i++)
		{
			for (size_t j = i + 1; j < genomes.size(); j++)
			{
				pairs.push_back(make_pair(i,j));
			}
		}

		///New flattened for loop
		for (size_t k = 0 + process_ID; k < pairs.size(); k += num_processes)
		{
			///Use pairs vector still correctly populate i and j variables
			int i = pairs[k].first;
			int j = pairs[k].second;
			string z;										//string for computing LCS
			cout << '(' << i << ',' << j << ')' << endl;	//left in mostly to see progress updates
			z = compute_LCS(genomes[i],genomes[j]);
			if (start) {									//special first case
				start = false;
				max_i = i;
				max_j = j;
				best = z;
			} else {
				if (z.length() > best.length()) {
					max_i = i;
					max_j = j;
					best = z;
				}
			}
		}

		///Find max LCS from different processes
		///Initialize all variables needed, names should be fairly self-explanatory
		Intpair local_size_and_index;
		local_size_and_index.size = best.length();
		local_size_and_index.index = process_ID;
		Intpair global_size_and_index;
		string global_best_string;
		int global_max_i;
		int global_max_j;

		///Use MPI_Allreduce to both find and broadcast size of largest LCS
		MPI_Allreduce(&local_size_and_index, &global_size_and_index, 1, MPI_2INT, MPI_MAXLOC, MPI_COMM_WORLD);

		///Whichever process had the max, that's the pair (i,j) and string we want
		if (best.length() == global_size_and_index.size)
		{
			global_max_i = max_i;
			global_max_j = max_j;
			global_best_string = best;
		}

		///From whichever process found it, broadcast the global best LCS and its pairing to all the other processes
		global_best_string.resize(global_size_and_index.size);		//make sure to resize to allocate enough space
		MPI_Bcast(&global_best_string[0], global_size_and_index.size, MPI_CHAR, global_size_and_index.index, MPI_COMM_WORLD);
		MPI_Bcast(&global_max_i, 1, MPI_INT, global_size_and_index.index, MPI_COMM_WORLD);
		MPI_Bcast(&global_max_j, 1, MPI_INT, global_size_and_index.index, MPI_COMM_WORLD);

		///Update the sequences by replacing the two strings that make the max LCS with the max LCS
		string new_tree_label = "(" + genome_ID[global_max_i] + "," + genome_ID[global_max_j] +")";
		genomes.erase(genomes.begin() + global_max_i);
		genomes.erase(genomes.begin() + global_max_j - 1); // max_i got deleted!
		genomes.push_back(global_best_string);
		genome_ID.erase(genome_ID.begin() + global_max_i);
		genome_ID.erase(genome_ID.begin() + global_max_j - 1); // max_i got deleted!
		genome_ID.push_back(new_tree_label);
	}

	///Print results
	if (process_ID == 0)
	{
		cout << "Phylogeny = " << endl;
		cout << genome_ID[0] << endl;
		cout << "Root has length " << genomes[0].length() << endl;
	}
	
	MPI_Finalize();
	return 0;
}