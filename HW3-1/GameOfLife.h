/**
 *        @file: GameOfLife.h
 *      @author: Ethan Dowalter
 *        @date: March 15, 2021
 *       @brief: Class which sequentially simulates an n x n board of Conway's game of life for m iterations
 */

#include <vector>
using namespace std;
#ifndef CS4000_GAME_OF_LIFE
#define CS4000_GAME_OF_LIFE
class GameOfLife {
	public:
		vector<vector<int> > SimulateLife(vector<vector<int> > &board, int life_cycles);
};

inline vector<vector<int>> GameOfLife::SimulateLife(vector<vector<int> > &board, int life_cycles){
	vector<vector<int>> board_odds, board_evens;		//two boards are needed because one will store the current state of the game and the other will be used to calculate the next state, and they alternate back and forth
	board_evens = board;								//copy board to board_evens to get the initial condition
	board_odds = board;									//copy board to board_odds to easily make it the right size
	int board_size = board.size();						//number of rows and columns in the board (it's always a sqaure, so rows == columns)
	int neighbors;										//variable for counting number of neighbors for a given cell
	for (int generation = 0; generation < life_cycles; generation++)
	{
		if (generation % 2 == 0)
		{
			///Look at board_evens and write to board_odds
			for (int i = 0; i < board_size; i++)			//loop for rows
			{
				for (int j = 0; j < board_size; j++)		//loop for columns
				{
					///Look at the neighbors to determine fate of selected cell
					if (board_odds[i][j] != 2)				//special case for if cell is a 2, then it is immortal so we can ignore it
					{
						neighbors = 0;						//reset neighbors to 0
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
			///Look at board_odds and write to board_evens
			for (int i = 0; i < board_size; i++)			//loop for rows
			{
				for (int j = 0; j < board_size; j++)		//loop for columns
				{
					///Look at the neighbors to determine fate of selected cell
					if (board_evens[i][j] != 2)				//special case for if cell is a 2, then it is immortal so we can ignore it
					{
						neighbors = 0;						//reset neighbors to 0
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
	}

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

#endif