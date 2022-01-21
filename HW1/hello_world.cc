/**
 *        @file: hello_world.cc
 *      @author: Ethan Dowalter
 *        @date: February 11, 2021
 *       @brief: Intro to OpenMP and parallel programming
 */

#include<iostream>
#include "omp.h"

using namespace std;


void Hello() {
  #pragma omp critical
  {
    cout << "Greetings from Thread " << omp_get_thread_num() << endl;
    cout << "There are " << omp_get_num_threads() << " threads " << endl;
  }
}
int main() {
  #pragma omp parallel    //added "omp" identifier to fix the issue
  Hello();
}