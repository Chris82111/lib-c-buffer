/*---------------------------------------------------------------------*
 *  private: include files
 *---------------------------------------------------------------------*/

#include "buffer_testbench.h"
#include "buffer.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <thread>


/*---------------------------------------------------------------------*
 *  private: definitions
 *---------------------------------------------------------------------*/

#define LENGTH(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))


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

static int buffer_test_some_working_nothing_special(void)
{
    int errors = 0;

    char buf[11];


    buffer_t obj;
    buffer.Init(&obj, buf, sizeof(buf), true);

#if false

    buffer_t obj_works_too = BUFFER_INIT(buf, sizeof(buf), true);

#endif

    size_t length = atomic_load(&obj.length);
    if(buffer.Length(&obj) != length){ errors += 1; }

    if(buf != obj.consumer_ptr){ errors += 1; }

    auto producer_ptr = atomic_load(&obj.producer_ptr);
    if(buf != producer_ptr){ errors += 1; }

    atomic_store(&obj.producer_ptr, producer_ptr);

    atomic_fetch_add(&obj.producer_ptr, 1);
    atomic_fetch_sub(&(obj.producer_ptr), 1);
    atomic_fetch_add(&obj.producer_ptr, 1);
    atomic_fetch_sub(&(obj.producer_ptr), 1);

    char * ptr = buf;

    if(true != atomic_compare_exchange_strong(&obj.producer_ptr, &ptr, buf)){ errors += 1; }
    if(true != atomic_compare_exchange_strong(&obj.producer_ptr, &ptr, buf)){ errors += 1; }

    char text[] = "Hello World";
    buffer.Write(&obj, text, sizeof(text));
    if(11 != buffer.Length(&obj)){ errors += 1; }

    return errors;
}

buffer_t thread_buffer_obj;
char thread_buffer_buf[255];
char thread_buffer_read[255];

// Function for the first thread
void threadFunction1() {
    static char c[] = { "abcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    for (int i = 0; i < 63; ++i) {
        buffer.Set(&thread_buffer_obj, c[(size_t)i % (sizeof(c) - 1)]);
    }
}

// Function for the second thread
void threadFunction2() {
    int i;
    for (i = 0; i < 63; ++i) {
        thread_buffer_read[i] = buffer.Get(&thread_buffer_obj);
    }
    thread_buffer_read[i] = '\0';
}

static int buffer_test_threads(void)
{
    int errors = 0;

    buffer.Init(&thread_buffer_obj, thread_buffer_buf, sizeof(thread_buffer_buf), true);

    std::thread t2(threadFunction2);
    std::thread t1(threadFunction1);

    // Wait for threads to finish
    t1.join();
    t2.join();

    return errors;
}


/*---------------------------------------------------------------------*
 *  public:  functions
 *---------------------------------------------------------------------*/

int buffer_testbench_cpp(void)
{
    int errors = 0;

    errors += buffer_test_some_working_nothing_special();
    errors += buffer_test_threads();

    return errors;
}


/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/
