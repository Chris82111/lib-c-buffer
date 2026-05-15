#include <iostream>
#include <fstream>

#include "buffer_test.h"

using namespace std;

int main()
{
  int errors = buffer_test();

  cout << "The number of errors is " << errors << endl;
} 
