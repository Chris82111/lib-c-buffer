/*---------------------------------------------------------------------*
 *  private: include files
 *---------------------------------------------------------------------*/

#include "buffer_test.h"
#include "Buffer.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <thread>

using namespace std;


/*---------------------------------------------------------------------*
 *  private: definitions
 *---------------------------------------------------------------------*/

#define LENGTH(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))


//! @def UNUSED
//! @brief This prevents warnings related to variables
//! @details Marks a variable to suppress compiler warnings about unused variables.
//!
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif


/*---------------------------------------------------------------------*
 *  private: typedefs
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: variables
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  public:  variables
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: function prototypes
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: functions
 *---------------------------------------------------------------------*/

static int buffer_test_basics ()
{
  int errors = 0;

  char c;

  char buf10[10+1];
  auto b1 = Buffer(buf10, sizeof(buf10));
  b1.Set('H');
  c = b1.Get();
  if ('H' != c) { errors++; }


  std::array<char, 10+1> data10;
  auto b2 = Buffer(data10);
  b2.Set('H');
  c = b2.Get();
  if ('H' != c) { errors++; }

  b2.Write("Hello");
  b2.TryReadCStr(buf10, sizeof(buf10), SIZE_MAX);
  cout << buf10 << endl;

  return errors;
}

std::array<char, 10+1> thread_buffer_buf;
Buffer obj = Buffer(thread_buffer_buf);

char thread_buffer_read[255];

uint64_t setCalls = 0;
uint64_t getCalls = 0;

// Function for the first thread
void threadFunction1() {
  static char c[] = { "abcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
  for (int j = 0; j < 100; j++)
  {
    for (int i = 0; i < 255; i++)
    {
      setCalls++;
      buffer.Set(obj.c_object, c[(size_t)i % (sizeof(c) - 1)]);
    }
  }
}

// Function for the second thread
void threadFunction2() {
  int i;
  for (int j = 0; j < 100; j++)
  {
    for (i = 0; i < 255; ++i)
    {
      getCalls++;
      thread_buffer_read[i] = obj.Get();
    }
  }
}

static int buffer_test_threads(void)
{
    int errors = 0;

    std::thread t2(threadFunction2);
    std::thread t1(threadFunction1);

    // Wait for threads to finish
    t1.join();
    t2.join();

    if (getCalls != setCalls) { errors++; }

    return errors;
}


/*---------------------------------------------------------------------*
 *  public:  functions
 *---------------------------------------------------------------------*/

int buffer_test(void)
{
  int errors = 0;
  errors += buffer_test_basics();
  errors += buffer_test_threads();

  return errors;
}


/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/
