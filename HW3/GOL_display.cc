//
// Visual Test Harness for CS 4000 HW #3
//
#include <iostream>
#include <cstdlib>
#include <vector>
#include <unistd.h>
extern "C" {
  #include <ncurses.h>
}
using namespace std;
#include "GameOfLife.h"


void print_board(vector<vector<int> > &board) {
  int n = board.size();
  
  for (int i=0;i<n;i++) {
    for (int j=0;j<n;j++) {
      move(i,j);
      if (board[i][j]==1) {
	printw("*");
      } else {
	printw(" ");
      }
    }
  }
}
int main() {
  int rows;
  int cols;
  int n;
  cin >> n;
  vector<vector<int> > board;
  board.resize(n);
  for (int i=0;i<n;i++) {
    board[i].resize(n);
    for (int j=0;j<n;j++) {
      cin >> board[i][j];
    }
  }
  int k;
  cin >> k;

    // Screen initialization

  initscr();
  // Clear the screen

  clear();

  // Get the size of the window!
  getmaxyx(stdscr,rows,cols);

  cbreak();  // Pass all characters to this program!

  keypad(stdscr, TRUE); // Grab the special keys, arrow keys, etc.

  
  GameOfLife obj;
  vector<vector<int> > result;

  for (int i=0;i<k;i++) {
    sleep(1);
    result = obj.SimulateLife(board,i);
    print_board(result);
    refresh(); // Put the stuff on the screen
  }
  endwin();
}
  
  

 