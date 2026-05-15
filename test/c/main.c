#include <stdio.h>

#include "buffer_test.h"

int main(void)
{
  int errors = buffer_test();

  printf("Number of errors is %i\n", errors);
}

