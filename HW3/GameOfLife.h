/**
 *        @file: GameOfLife.h
 *      @author: Ethan Dowalter
 *        @date: March 15, 2021
 *       @brief: Class which simulates an n x n board of Conway's game of life for m iterations in parallel
 */

#include <vector>
#include <thread>
#include <pthread.h>
using namespace std;
#ifndef CS4000_GAME_OF_LIFE
#define CS4000_GAME_OF_LIFE

///Global arrays so all threads have access
vector<vector<int>> board_odds;		//two boards are needed because one will store the current state of the game and
vector<vector<int>> board_evens;	//the other will be used to calculate the next state, and they alternate back and forth

///Barrier declaration for process synchonization
pthread_barrier_t mybarrier;

class GameOfLife {
	public:
		vector<vector<int>> SimulateLife(vector<vector<int> > &board, int life_cycles);
		static void ParallelSimulateLife(int thread_id, vector <int> thread_start_row, int board_size, int life_cycles);
};

inline vector<vector<int>> GameOfLife::SimulateLife(vector<vector<int> > &board, int life_cycles){
	board_evens = board;						//copy board to board_evens to get the initial condition
	board_odds = board;							//copy board to board_odds to easily make it the right size
	int board_size = board.size();				//number of rows and columns in the board (it's always a sqaure, so rows == columns)
	int num_threads = 4;						//number of threads being used
	vector <int> thread_start_row;				//vector which stores which row each thread should start processing on and the board size at the end
	thread_start_row.resize(num_threads);		//resize vector to proper size

	///Algorithm which fills vector with equally spaced values so that threads can do similar amounts of work
	///E.g. if board_size = 10 and num_threads = 4, then the array will fill with {0,3,6,8,10}
	for (size_t i = 0; i < num_threads; i++)
	{
		thread_start_row[i] = board_size / num_threads * i;
	}
	int remainder = board_size % num_threads;
	for (size_t i = 0; i < remainder; i++)
	{
		for (size_t j = i + 1; j < num_threads; j++)
		{
			thread_start_row[j] += 1;
		}
	}
	thread_start_row.push_back(board_size);		//add the board size to the end of the vector for later

	pthread_barrier_init(&mybarrier,NULL,num_threads);		//initialize the barrier

	///Create the threads which will evenly split up the board 
	thread Thread1(ParallelSimulateLife, 0, thread_start_row, board_size, life_cycles);
	thread Thread2(ParallelSimulateLife, 1, thread_start_row, board_size, life_cycles);
	thread Thread3(ParallelSimulateLife, 2, thread_start_row, board_size, life_cycles);
	thread Thread4(ParallelSimulateLife, 3, thread_start_row, board_size, life_cycles);

	///Join the threads
	Thread1.join();
	Thread2.join();
	Thread3.join();
	Thread4.join();

	///Return the last calculated board state
	if (life_cycles % 2 == 0)
	{
		return board_evens;
	}
	else
	{
		return board_odds;
	}
}

inline void GameOfLife::ParallelSimulateLife(int thread_id, vector <int> thread_start_row, int board_size, int life_cycles){
	for (int generation = 0; generation < life_cycles; generation++)
	{
		if (generation % 2 == 0)
		{
			///If generation is even -> Look at board_evens and write to board_odds
			for (int i = thread_start_row[thread_id]; i < thread_start_row[thread_id + 1]; i++)		//parallel algorithm splits up board by rows
			{
				for (int j = 0; j < board_size; j++)		//loop for columns
				{
					///Look at the neighbors to determine fate of selected cell
					if (board_odds[i][j] != 2)				//special case for if cell is a 2, then it is immortal so we can ignore it
					{
						int neighbors = 0;					//counts neighbors of a cell, initialized to 0
						for (int x = -1; x < 2; x++)		//nested loop for looking in all 8 directions at each cell
						{
							for (int y = -1; y < 2; y++)
							{
								if (!(x == 0 && y == 0))	//don't count the cell itself as a neighbor
								{
									///Make sure that if cell is on an edge that we wrap around when checking for neighbors
									if (board_evens[(board_size + i + x) % board_size][(board_size + j + y) % board_size] == 1 || 
                                        board_evens[(board_size + i + x) % board_size][(board_size + j + y) % board_size] == 2)
                                    {
                                        neighbors++;
                                    }
								}
							}
						}

						///Update the cell
						if ((board_evens[i][j] == 1 && (neighbors == 2 || neighbors == 3)) || (board_evens[i][j] == 0 && neighbors == 3))
						{
							board_odds[i][j] = 1;		//2 or 3 neighbors on an alive cell means it gets to live, 3 neighbors to a dead cell means it is born
						}
						else
						{
							board_odds[i][j] = 0;		//<2 neighbors means cell dies of starvation, >3 neighbors means cell dies of overpopulation
						}
					}
				}
			}
		}
		else
		{
			///If generation is odd -> Look at board_odds and write to board_evens
			for (int i = thread_start_row[thread_id]; i < thread_start_row[thread_id + 1]; i++)		//parallel algorithm splits up board by rows
			{
				for (int j = 0; j < board_size; j++)		//loop for columns
				{
					///Look at the neighbors to determine fate of selected cell
					if (board_evens[i][j] != 2)				//special case for if cell is a 2, then it is immortal so we can ignore it
					{
						int neighbors = 0;					//counts neighbors of a cell, initialized to 0
						for (int x = -1; x < 2; x++)		//nested loop for looking in all 8 directions at each cell
						{
							for (int y = -1; y < 2; y++)
							{
								if (!(x == 0 && y == 0))	//don't count the cell itself as a neighbor
								{
									///Make sure that if cell is on an edge that we wrap around when checking for neighbors
									if (board_odds[(board_size + i + x) % board_size][(board_size + j + y) % board_size] == 1 || 
                                        board_odds[(board_size + i + x) % board_size][(board_size + j + y) % board_size] == 2)
                                    {
                                        neighbors++;
                                    }
								}
							}
						}

						///Update the cell
						if ((board_odds[i][j] == 1 && (neighbors == 2 || neighbors == 3)) || (board_odds[i][j] == 0 && neighbors == 3))
						{
							board_evens[i][j] = 1;		//2 or 3 neighbors on an alive cell means it gets to live, 3 neighbors to a dead cell means it is born
						}
						else
						{
							board_evens[i][j] = 0;		//<2 neighbors means cell dies of starvation, >3 neighbors means cell dies of overpopulation
						}
					}
				}
			}
		}
		pthread_barrier_wait(&mybarrier);
	}
}

#endif