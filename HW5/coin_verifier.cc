// 
// Reads in three big integers l and r and z
// and verifies that l < shar256(z) < r.
// Outputs true and exits normally if
// true.
// Otherwise outputs false and fails.
//
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <iostream>
#include <iomanip>
#include <openssl/sha.h>
#include <cassert>

using namespace std;
using namespace boost::multiprecision;


vector<unsigned char> to_bits(cpp_int x) {
  unsigned char c;

  vector<unsigned char> buffer;
  while (x>0) {
    c = (unsigned char) x%256;
    buffer.push_back(c);
    x=x/256;
  }

  return buffer;
}

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
int main() {

  cpp_int t1;
  cpp_int t2;
  cpp_int z;
  cin >> t1;
  cin >> t2;
  cin >> z;
  assert(t1 < t2);
  cpp_int val;
  val = sha256(z);
  
  if ((t1 < val) && (val < t2)) {
    cout << "true" << endl;
  } else {
    cout << "false" << endl;
    exit(-1); // Fail
  }
  return 0;
}

