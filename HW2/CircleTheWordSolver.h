/**
 *        @file: CircleTheWorldSolver.h
 *      @author: Ethan Dowalter
 *        @date: February 24, 2021
 *       @brief: Program which takes an input file of a word search and words to search for and solves it in parallel
 */

#include <vector>
#include <string>
#include <algorithm>
#include "omp.h"
using namespace std;

#ifndef DWJ_WORD_CIRCLE_DEF
#define DWJ_WORD_CIRCLE_DEF
class CircleTheWordSolver {
    public:
        vector<pair<int, int> > word_locations(vector<vector<char> > &puzzle, vector<string> &wordlist);
};



inline vector<pair<int, int> > CircleTheWordSolver::word_locations(vector<vector<char> > &puzzle, vector<string> &wordlist){

    int puzzle_rows = puzzle.size();                    //convert size_t type of puzzle.size() to int type to avoid warning from comparison of unlike types in for loop conditions
    int puzzle_columns = puzzle.at(0).size();           //same deal here
    int num_words = wordlist.size();                    //and same again

    vector<pair<int, int> > locations;                  //initialize vector to store locations of found words
    locations.resize(num_words);                        //resize vector to match the number of words being searched for
    fill(locations.begin(), locations.end(), make_pair(-1, -1));        //assume all words are not found first by filling vector with invalid values

    #pragma omp parallel for schedule(static,1)
    for (int i = 0; i < puzzle_rows; i++)               //outermost loop, controlling the row number
    {
        for (int j = 0; j < puzzle_columns; j++)        //middle loop, controlling the column number
        {
            for (int k = 0; k < num_words; k++)         //loop to check first letter of each word being searched for
            {
                if (puzzle[i][j] == wordlist.at(k)[0] && (locations.at(k) == make_pair(-1, -1) || i < locations.at(k).first))
                {
                    int word_size = wordlist.at(k).size();      //converting size_t type to int again
                    ///Special case if length of word is 1 letter
                    if (word_size == 1)
                    {
                        locations.at(k) = make_pair(i, j);
                    }

                    ///Check all 8 directions for potential match
                    ///Look up
                    if (i >= 1 && puzzle[i-1][j] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (i - x >= 0 && puzzle[i-x][j] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look up-right
                    if (i >= 1 && j < puzzle_columns - 1 && puzzle[i-1][j+1] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (i - x >= 0 && j + x < puzzle_columns && puzzle[i-x][j+x] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look right
                    if (j < puzzle_columns - 1 && puzzle[i][j+1] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (j + x < puzzle_columns && puzzle[i][j+x] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look down-right
                    if (i < puzzle_rows - 1 && j < puzzle_columns - 1 && puzzle[i+1][j+1] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (i + x < puzzle_rows && j + x < puzzle_columns && puzzle[i+x][j+x] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look down
                    if (i < puzzle_rows - 1 && puzzle[i+1][j] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (i + x < puzzle_rows && puzzle[i+x][j] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look down-left
                    if (i < puzzle_rows - 1 && j >= 1 && puzzle[i+1][j-1] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (i + x < puzzle_rows && j - x >= 0 && puzzle[i+x][j-x] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look left
                    if (j >= 1 && puzzle[i][j-1] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (j - x >= 0 && puzzle[i][j-x] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                    ///Look up-left
                    if (i >= 1 && j >= 1 && puzzle[i-1][j-1] == wordlist.at(k)[1])
                    {
                        int num_matching_char = 2;              //counter to keep track of number of matching characters to the word
                        for (int x = 2; x < word_size; x++)
                        {
                            if (i - x >= 0 && j - x >= 0 && puzzle[i-x][j-x] == wordlist.at(k)[x])
                            {
                                num_matching_char++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (num_matching_char == word_size)
                        {
                            locations.at(k) = make_pair(i, j);
                        }
                    }
                }
            }
        }
    }
    return locations;
}
#endif