/**
 *        @file: count_primes.cc
 *      @author: Ethan Dowalter
 *        @date: February 11, 2021
 *       @brief: Intro to OpenMP and parallel programming
 */

#include <iostream>
#include "omp.h"

using namespace std;

//
// Check to see if a number is prime.
//
bool is_prime(long long x) {
  if (x<2) return false;
  long long t=2;
  while (t*t <= x) {
    if (x%t == 0) {
      return false;
    }
    t++;
  }
  return true;
}

// Count the number of primes between a and b, inclusive

long long count_primes(long long a, long long b) {
  long long sum=0;

  //Here is the best point to parallelize the code because this is where the work gets done
  //So here is where we put the #pragma omp statement, with the additional tags
  //parallel for - splits the for loop into equal parts so different threads can work in parallel
  //reduction(+:sum) - avoids race condition when multiple threads are trying to write to sum at the same time
  #pragma omp parallel for reduction(+:sum)
  for (long long i = a;i<=b;i++) {
    if (is_prime(i)) sum++;
  }
  return sum;
}

int main() {
  long long a;
  long long b;

  cout << "Enter two large integers " << endl;
  cin >> a;
  cin >> b;

  cout << a << " Start " << endl;
  cout << b << " End " << endl;
  
  long long total = count_primes(a,b);

  cout << "Number of prime numbers between " << a << " and " << b << " = " << total << endl;
}