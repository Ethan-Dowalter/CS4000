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