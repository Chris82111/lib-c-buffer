/*---------------------------------------------------------------------*
 *  private: include files
 *---------------------------------------------------------------------*/

#include "buffer_testbench.h"
#include "buffer.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


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

static char buffer_test_set_get_blocking_wait_counter;


/*---------------------------------------------------------------------*
 *  public:  variables
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: function prototypes
 *---------------------------------------------------------------------*/

static int buffer_test_init(void);
static int buffer_test_reset(void);
static int buffer_test_set_get_non_blocking(void);
static char buffer_test_set_get_blocking_wait(buffer_t * object);
static int buffer_test_set_get_blocking(void);
static int buffer_test_length(void);
static int buffer_test_new_line(void);
static int buffer_test_get_over_last(void);


/*---------------------------------------------------------------------*
 *  private: functions
 *---------------------------------------------------------------------*/

static int buffer_test_init(void)
{
    int errors = 0;

    char buf[4 + 1];

    buffer_t obj_1 = { 0 };
    for(size_t i = 0; i < sizeof(buffer_t); i++)
    {
        *(( (uint8_t *)(&obj_1) ) + i) = 0xff;
    }
    buffer_init(&obj_1, buf, sizeof(buf)-1, true);

    buffer_t obj_2 = BUFFER_INIT(buf, (sizeof(buf)-1), true);

    buffer_t * ptrs[] = { &obj_1, &obj_2, NULL };

    buffer_t ref = { 0 };
    ref.data = buf;
    ref.last = buf + (sizeof(buf)-1) - 1;
    ref.end_of_line_character = '\n';
    ref.consumer_ptr = buf;
    atomic_init(&ref.producer_ptr, buf);
    atomic_init(&ref.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );

    buffer_t * ptr;
    for(size_t j = 0; j < LENGTH(ptrs) && NULL != (ptr = ptrs[j]); j++ )
    {
        if(true != buffer_equal(ptr, &ref)){ errors += 1; }
    }

    return errors;
}

static int buffer_test_init_sizeof_data_zero(void)
{
    int errors = 0;

    char buf[4 + 1];

    buffer_t obj_1 = { 0 };
    buffer_init(&obj_1, buf, 0, true);
    if(0 != obj_1.state){ errors += 1; }
    if(NULL != obj_1.consumer_ptr){ errors += 1; }
    if(NULL != atomic_load(&obj_1.producer_ptr)){ errors += 1; }

    buffer_t obj_2 = { 0 };
    buffer_init(&obj_2, NULL, sizeof(buf)-1, true);
    if(0 != obj_2.state){ errors += 1; }
    if(NULL != obj_2.consumer_ptr){ errors += 1; }
    if(NULL != atomic_load(&obj_2.producer_ptr)){ errors += 1; }

    buffer_t obj_3 = { 0 };
    buffer_init(&obj_3, NULL, 0, true);
    if(0 != obj_3.state){ errors += 1; }
    if(NULL != obj_3.consumer_ptr){ errors += 1; }
    if(NULL != atomic_load(&obj_3.producer_ptr)){ errors += 1; }


    buffer_t obj_4 = BUFFER_INIT(buf, 0, true);
    buffer_init(&obj_4, NULL, 0, true);
    if(0 != obj_4.state){ errors += 1; }
    if(NULL != obj_4.consumer_ptr){ errors += 1; }
    if(NULL != atomic_load(&obj_4.producer_ptr)){ errors += 1; }

    buffer_t obj_5 = BUFFER_INIT(NULL, sizeof(buf)-1, true);
    buffer_init(&obj_5, NULL, 0, true);
    if(0 != obj_5.state){ errors += 1; }
    if(NULL != obj_5.consumer_ptr){ errors += 1; }
    if(NULL != atomic_load(&obj_5.producer_ptr)){ errors += 1; }

    buffer_t obj_6 = BUFFER_INIT(NULL, 0, true);
    buffer_init(&obj_6, NULL, 0, true);
    if(0 != obj_6.state){ errors += 1; }
    if(NULL != obj_6.consumer_ptr){ errors += 1; }
    if(NULL != atomic_load(&obj_6.producer_ptr)){ errors += 1; }

    return errors;
}

static int buffer_test_reset(void)
{
    int errors = 0;

    char buf[4 + 1];


    buffer_t obj = {0};
    buffer_t * ptr = &obj;

    buffer_init(ptr, buf, sizeof(buf)-1, true);

    buffer_set_possible_or_skip(&obj, '1');
    buffer_set_possible_or_skip(&obj, '2');

    buffer_reset(ptr, true);

    buffer_t ref = { 0 };
    ref.data = buf;
    ref.last = buf + (sizeof(buf)-1) - 1;
    ref.end_of_line_character = '\n';
    ref.consumer_ptr = buf;
    atomic_init(&ref.producer_ptr, buf);
    atomic_init(&ref.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );

    if(true != buffer_equal(ptr, &ref)){ errors += 1; }


    return errors;
}

static int buffer_test_copy(void)
{
    int errors = 0;

    char buf[4 + 1];
    buffer_t obj_1 = {0};

    for(size_t i = 0; i < sizeof(buffer_t); i++)
    {
        *(( (uint8_t *)(&obj_1) ) + i) = 0xff;
    }

    buffer_init(&obj_1, buf, sizeof(buf)-1, true);

    buffer_t obj_2 = {0};

    for(size_t i = 0; i < sizeof(buffer_t); i++)
    {
        *(( (uint8_t *)(&obj_2) ) + i) = 0xff;
    }

    buffer_copy(&obj_1, &obj_2);

    if(true != buffer_equal(&obj_1, &obj_2)){ errors += 1; }

    return errors;
}

static int buffer_test_set_get_non_blocking(void)
{
    char c;
    size_t length;
    bool saved;
    int errors = 0;
    //               T E S T 4    5 6 7 8 9
    char buf[10 + 1];
    for(size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = '\0';
    }
    buffer_t obj = {0};
    buffer_t * ptr = &obj;

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );



    bool atomic = atomic_is_lock_free(&(obj.length));
    if(true != atomic){ errors += 1; }
    atomic = atomic_is_lock_free(&(obj.lines));
    if(true != atomic){ errors += 1; }



    saved = buffer_set_possible_or_skip(&obj, 'T');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(1 != length){ errors += 1; }
    if('T' != buf[0] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, 'E');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('E' != buf[1] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, 'S');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(3 != length){ errors += 1; }
    if('S' != buf[2] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, 'T');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(4 != length){ errors += 1; }
    length = buffer_length(ptr);
    if(4 != length){ errors += 1; }
    if('T' != buf[3] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '4');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(5 != length){ errors += 1; }
    if('4' != buf[4] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '5');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(6 != length){ errors += 1; }
    if('5' != buf[5] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '6');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(7 != length){ errors += 1; }
    if('6' != buf[6] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '7');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(8 != length){ errors += 1; }
    if('7' != buf[7] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '8');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(9 != length){ errors += 1; }
    if('8' != buf[8] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '9');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(10 != length){ errors += 1; }
    if('9' != buf[9] ){ errors += 1; }

    saved = buffer_set_possible_or_skip(&obj, '0');
    if(false != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(10 != length){ errors += 1; }
    if('\0' != buf[10] ){ errors += 1; }



    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('T' != c ){ errors += 1; }
    if(9 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('E' != c ){ errors += 1; }
    if(8 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('S' != c ){ errors += 1; }
    if(7 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('T' != c ){ errors += 1; }
    if(6 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('4' != c ){ errors += 1; }
    if(5 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('5' != c ){ errors += 1; }
    if(4 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('6' != c ){ errors += 1; }
    if(3 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('7' != c ){ errors += 1; }
    if(2 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('8' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('9' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('\0' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }



    buffer_set_possible_or_skip(&obj, 'A');
    length = buffer_length(ptr);
    if(1 != length){ errors += 1; }
    if('A' != buf[0] ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, 'B');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('B' != buf[1] ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('A' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    buffer_set_possible_or_skip(&obj, 'C');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('C' != buf[2] ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('B' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('C' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }



    buffer_set_possible_or_skip(&obj, 'a');
    length = buffer_length(ptr);
    if(1 != length){ errors += 1; }
    if('a' != buf[0] ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, 'b');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('b' != buf[1] ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('a' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    buffer_set_possible_or_skip(&obj, 'c');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('c' != buf[2] ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('b' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    length = buffer_length(ptr);
    if('c' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }

    return errors;
}

static char buffer_test_set_get_blocking_wait(buffer_t * object)
{
    if(NULL == object){ ; }
    if(buffer_test_set_get_blocking_wait_counter < 10)
    {
        buffer_test_set_get_blocking_wait_counter++;
        return 0;
    }
    else
    {
        return 1;
    }
}

static int buffer_test_set_get_blocking(void)
{
    char c;
    bool saved;
    size_t length;
    int errors = 0;
    //               T E S T 4    5 6 7 8 9
    char buf[10 + 1];
    for(size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = '\0';
    }
    buffer_t obj = {0};
    buffer_t * ptr = &obj;

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );



    bool atomic = atomic_is_lock_free(&(obj.length));
    if(true != atomic){ errors += 1; }
    atomic = atomic_is_lock_free(&(obj.lines));
    if(true != atomic){ errors += 1; }



    saved = buffer_set(&obj, 'T');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(1 != length){ errors += 1; }
    if('T' != buf[0] ){ errors += 1; }

    saved = buffer_set(&obj, 'E');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('E' != buf[1] ){ errors += 1; }

    saved = buffer_set(&obj, 'S');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(3 != length){ errors += 1; }
    if('S' != buf[2] ){ errors += 1; }

    saved = buffer_set(&obj, 'T');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(4 != length){ errors += 1; }
    length = buffer_length(ptr);
    if(4 != length){ errors += 1; }
    if('T' != buf[3] ){ errors += 1; }

    saved = buffer_set(&obj, '4');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(5 != length){ errors += 1; }
    if('4' != buf[4] ){ errors += 1; }

    saved = buffer_set(&obj, '5');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(6 != length){ errors += 1; }
    if('5' != buf[5] ){ errors += 1; }

    saved = buffer_set(&obj, '6');
    length = buffer_length(ptr);
    if(7 != length){ errors += 1; }
    if(true != saved){ errors += 1; }
    if('6' != buf[6] ){ errors += 1; }

    saved = buffer_set(&obj, '7');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(8 != length){ errors += 1; }
    if('7' != buf[7] ){ errors += 1; }

    saved = buffer_set(&obj, '8');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(9 != length){ errors += 1; }
    if('8' != buf[8] ){ errors += 1; }

    saved = buffer_set(&obj, '9');
    if(true != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(10 != length){ errors += 1; }
    if('9' != buf[9] ){ errors += 1; }

    buffer_test_set_get_blocking_wait_counter = 0;
    obj.on_wait_set = buffer_test_set_get_blocking_wait;

    saved = buffer_set(&obj, '0');
    if(false != saved){ errors += 1; }
    length = buffer_length(ptr);
    if(10 != length){ errors += 1; }
    if('\0' != buf[10] ){ errors += 1; }

    if(10 != buffer_test_set_get_blocking_wait_counter ){ errors += 1; }
    obj.on_wait_set = NULL;



    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('T' != c ){ errors += 1; }
    if(9 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('E' != c ){ errors += 1; }
    if(8 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('S' != c ){ errors += 1; }
    if(7 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('T' != c ){ errors += 1; }
    if(6 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('4' != c ){ errors += 1; }
    if(5 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('5' != c ){ errors += 1; }
    if(4 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('6' != c ){ errors += 1; }
    if(3 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('7' != c ){ errors += 1; }
    if(2 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('8' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('9' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }

    buffer_test_set_get_blocking_wait_counter = 0;
    obj.on_wait_get = buffer_test_set_get_blocking_wait;

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('\0' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }

    if(10 != buffer_test_set_get_blocking_wait_counter ){ errors += 1; }
    obj.on_wait_get = NULL;



    buffer_set(&obj, 'A');
    length = buffer_length(ptr);
    if(1 != length){ errors += 1; }
    if('A' != buf[0] ){ errors += 1; }

    buffer_set(&obj, 'B');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('B' != buf[1] ){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('A' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    buffer_set(&obj, 'C');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('C' != buf[2] ){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('B' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('C' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }



    buffer_set(&obj, 'a');
    length = buffer_length(ptr);
    if(1 != length){ errors += 1; }
    if('a' != buf[0] ){ errors += 1; }

    buffer_set(&obj, 'b');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('b' != buf[1] ){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('a' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    buffer_set(&obj, 'c');
    length = buffer_length(ptr);
    if(2 != length){ errors += 1; }
    if('c' != buf[2] ){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('b' != c ){ errors += 1; }
    if(1 != length){ errors += 1; }

    c = buffer_get(&obj);
    length = buffer_length(ptr);
    if('c' != c ){ errors += 1; }
    if(0 != length){ errors += 1; }

    return errors;
}

static int buffer_test_look()
{
    int errors = 0;

    char c;
    bool saved;
    char buf[10 + 1];
    for(size_t i = 0; i < sizeof(buf); i++) { buf[i] = '\0'; }
    buffer_t obj = {0};
    buffer_t * ptr = &obj;

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );


    saved = buffer_set(&obj, 'A');
    if(true != saved){ errors += 1; }
    if(1 != buffer_length(ptr)){ errors += 1; }
    if('A' != buf[0] ){ errors += 1; }

    c = buffer_look_available_or_null(ptr);
    if('A' != c ){ errors += 1; }
    if(true != saved){ errors += 1; }
    if(1 != buffer_length(ptr)){ errors += 1; }
    if('A' != buf[0] ){ errors += 1; }



    saved = buffer_set(&obj, 'B');
    if(true != saved){ errors += 1; }
    if(2 != buffer_length(ptr)){ errors += 1; }
    if('B' != buf[1] ){ errors += 1; }

    c = buffer_look_available_or_null(ptr);
    if('A' != c ){ errors += 1; }
    if(2 != buffer_length(ptr)){ errors += 1; }
    if('B' != buf[1] ){ errors += 1; }

    c = buffer_get(ptr);
    if('A' != c ){ errors += 1; }
    if(1 != buffer_length(ptr)){ errors += 1; }
    if('B' != buf[1] ){ errors += 1; }

    c = buffer_look_available_or_null(ptr);
    if('B' != c ){ errors += 1; }
    if(1 != buffer_length(ptr)){ errors += 1; }
    if('B' != buf[1] ){ errors += 1; }

    c = buffer_get(ptr);
    if('B' != c ){ errors += 1; }
    if(0 != buffer_length(ptr)){ errors += 1; }

    c = buffer_look_available_or_null(ptr);
    if('\0' != c ){ errors += 1; }
    if(0 != buffer_length(ptr)){ errors += 1; }

    return errors;
}

static int buffer_test_clean()
{
    int errors = 0;

    char buf[10 + 1];
    for(size_t i = 0; i < sizeof(buf); i++) { buf[i] = '\0'; }
    buffer_t obj = {0};
    buffer_t * ptr = &obj;

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );


    buffer_set(ptr, 'A');
    buffer_set(ptr, 'B');
    buffer_set(ptr, 'C');
    buffer_set(ptr, '\n');
    buffer_set(ptr, 'D');
    buffer_set(ptr, 'E');
    if(6 != buffer_length(ptr)){ errors += 1; }
    if(1 != buffer_lines(ptr)){ errors += 1; }

    buffer_clear(ptr);
    if(0 != buffer_length(ptr)){ errors += 1; }
    if(0 != buffer_lines(ptr)){ errors += 1; }

    return errors;
}

static int buffer_test_length(void)
{
    char c;
    int errors = 0;

    char buf[4 + 1];
    for(size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = '\0';
    }
    buffer_t obj = {0};

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );


    if(true != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(4 != buffer_space(&obj) ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '1');

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(3 != buffer_space(&obj) ){ errors += 1; }
    if(1 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '2');

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(2 != buffer_space(&obj) ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '\n');

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(1 != buffer_space(&obj) ){ errors += 1; }
    if(3 != buffer_length(&obj) ){ errors += 1; }
    if(1 != buffer_lines(&obj) ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '4');

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(true != buffer_is_full(&obj) ){ errors += 1; }
    if(0 != buffer_space(&obj) ){ errors += 1; }
    if(4 != buffer_length(&obj) ){ errors += 1; }
    if(1 != buffer_lines(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    if(0 == c){ ; }

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(true != buffer_is_full(&obj) ){ errors += 1; }
    if(0 != buffer_space(&obj) ){ errors += 1; }
    if(3 != buffer_length(&obj) ){ errors += 1; }
    if(1 != buffer_lines(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(true != buffer_is_full(&obj) ){ errors += 1; }
    if(0 != buffer_space(&obj) ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }
    if(1 != buffer_lines(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(true != buffer_is_full(&obj) ){ errors += 1; }
    if(0 != buffer_space(&obj) ){ errors += 1; }
    if(1 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);

    if(true != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(4 != buffer_space(&obj) ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    return errors;
}

static int buffer_test_new_line(void)
{
    char c;
    int errors = 0;

    char buf[6 + 1];
    for(size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = '\0';
    }
    buffer_t obj = {0};

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );


    if(true != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(6 != buffer_space(&obj) ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '1');
    buffer_set_possible_or_skip(&obj, '\n');

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(4 != buffer_space(&obj) ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }
    if(1 != buffer_lines(&obj) ){ errors += 1; }

    buffer_set(&obj, '3');
    buffer_set(&obj, '\n');

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(2 != buffer_space(&obj) ){ errors += 1; }
    if(4 != buffer_length(&obj) ){ errors += 1; }
    if(2 != buffer_lines(&obj) ){ errors += 1; }

    c = buffer_get(&obj);
    if('1' != c ){ errors += 1; }
    c = buffer_get(&obj);
    if('\n' != c ){ errors += 1; }

    if(false != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(2 != buffer_space(&obj) ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }
    if(1 != buffer_lines(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    if('3' != c ){ errors += 1; }
    c = buffer_get_available_or_null(&obj);
    if('\n' != c ){ errors += 1; }

    if(true != buffer_is_empty(&obj) ){ errors += 1; }
    if(false != buffer_is_full(&obj) ){ errors += 1; }
    if(6 != buffer_space(&obj) ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    return errors;
}

static int buffer_test_get_over_last(void)
{
    char c;
    int errors = 0;

    char buf[6 + 1];
    for(size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = '\0';
    }
    buffer_t obj = {0};

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );

    buffer_set_possible_or_skip(&obj, '1');
    buffer_set_possible_or_skip(&obj, '2');
    buffer_set_possible_or_skip(&obj, '3');

    c = buffer_get(&obj);
    if('1' != c ){ errors += 1; }

    // Error
    obj.consumer_ptr = obj.last + 2;

    c = buffer_get_available_or_null(&obj);
    if('\0' != c ){ errors += 1; }


    buffer_test_set_get_blocking_wait_counter = 0;
    obj.on_wait_get = buffer_test_set_get_blocking_wait;

    c = buffer_get(&obj);
    if('\0' != c ){ errors += 1; }

    if(0 != buffer_test_set_get_blocking_wait_counter ){ errors += 1; }
    obj.on_wait_set = NULL;

    return errors;
}

static int buffer_test_stop(void)
{
    int errors = 0;
    bool result;

    char buf[6 + 1];
    buffer_t obj = {0};
    buffer_init(NULL, buf, sizeof(buf)-1, false);

    atomic_store(&obj.state, (unsigned char)BUFFER_FLAGS_STOP);
    result = buffer_stop_try(&obj);
    if(true != result){ errors += 1; }



    atomic_store(&obj.state, (unsigned char)BUFFER_FLAGS_IDLE);

    result = buffer_is_stopped(&obj);
    if(false != result){ errors += 1; }

    result = buffer_stop_try(&obj);
    if(true != result){ errors += 1; }



    atomic_store(&obj.state, (unsigned char)(BUFFER_FLAGS_IDLE | BUFFER_FLAGS_RUNNING_GET));

    result = buffer_is_stopped(&obj);
    if(false != result){ errors += 1; }

    result = buffer_stop_try(&obj);
    if(false != result){ errors += 1; }
    if((BUFFER_FLAGS_IDLE | BUFFER_FLAGS_RUNNING_GET) != obj.state){ errors += 1; }
    if(BUFFER_FLAGS_IDLE > obj.state){ errors += 1; }

    result = buffer_is_stopped(&obj);
    if(false != result){ errors += 1; }

    atomic_store(&obj.state, (unsigned char)(obj.state & ~BUFFER_FLAGS_RUNNING_GET));

    result = buffer_is_stopped(&obj);
    if(false != result){ errors += 1; }
    if(BUFFER_FLAGS_IDLE != obj.state){ errors += 1; }
    if(BUFFER_FLAGS_IDLE > obj.state){ errors += 1; }

    result = buffer_stop_try(&obj);
    if(true != result){ errors += 1; }
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }

    result = buffer_stop_try(&obj);
    if(true != result){ errors += 1; }
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }



    atomic_store(&obj.state, (unsigned char)(BUFFER_FLAGS_IDLE | BUFFER_FLAGS_RUNNING_GET));

    result = buffer_is_stopped(&obj);
    if(false != result){ errors += 1; }

    result = buffer_stop_force(&obj);
    if(false != result){ errors += 1; }
    if(BUFFER_FLAGS_RUNNING_GET != obj.state){ errors += 1; }

    result = buffer_is_stopped(&obj);
    if(false != result){ errors += 1; }

    atomic_store(&obj.state, (unsigned char)(obj.state & ~BUFFER_FLAGS_RUNNING_GET));

    result = buffer_is_stopped(&obj);
    if(true != result){ errors += 1; }
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }

    result = buffer_stop_force(&obj);
    if(true != result){ errors += 1; }
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }

    result = buffer_stop_force(&obj);
    if(true != result){ errors += 1; }
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }



    return errors;
}

static char buffer_test_get_set_wait_force_stop_wait(buffer_t * object)
{
    if(NULL == object){ ; }
    if(buffer_test_set_get_blocking_wait_counter < 10)
    {
        buffer_test_set_get_blocking_wait_counter++;
    }
    else
    {
        buffer_stop_force(object);
    }
    return 0;
}

static int buffer_test_stop_start_set_get(void)
{
    char c;
    int errors = 0;

    char buf[6 + 1];
    for(size_t i = 0; i < sizeof(buf); i++)
    {
        buf[i] = '\0';
    }
    buffer_t obj = {0};

    obj.data = buf;
    obj.last = buf + sizeof(buf) - 1;
    obj.last -= 1; // Do not use last element
    obj.end_of_line_character = '\n';
    obj.consumer_ptr = buf;
    atomic_init(&obj.producer_ptr, buf);
    atomic_init(&obj.state, (true) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP );

    buffer_stop_try(&obj);
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }

    buffer_start(&obj);
    if(BUFFER_FLAGS_IDLE != obj.state){ errors += 1; }



    buffer_stop_try(&obj);
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '1');
    if(false == buffer_is_empty(&obj)){ errors += 1; }

    buffer_set(&obj, '2');
    if(false == buffer_is_empty(&obj)){ errors += 1; }



    buffer_start(&obj);
    if(BUFFER_FLAGS_IDLE != obj.state){ errors += 1; }

    buffer_set_possible_or_skip(&obj, '1');
    if(1 != buffer_length(&obj) ){ errors += 1; }

    buffer_set(&obj, '2');
    if(2 != buffer_length(&obj) ){ errors += 1; }



    buffer_stop_try(&obj);
    if(BUFFER_FLAGS_STOP != obj.state){ errors += 1; }

    c = buffer_get(&obj);
    if('\0' != c ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    if('\0' != c ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }



    buffer_start(&obj);
    if(BUFFER_FLAGS_IDLE != obj.state){ errors += 1; }

    c = buffer_get(&obj);
    if('1' != c ){ errors += 1; }
    if(1 != buffer_length(&obj) ){ errors += 1; }

    c = buffer_get_available_or_null(&obj);
    if('2' != c ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }



    return errors;
}

static int buffer_test_get_set_wait_force_stop(void)
{
    char c;
    int errors = 0;

    char buf[2+1] = { 0 };
    buffer_t obj = BUFFER_INIT(buf, sizeof(buf)-1, true);
    obj.on_wait_set = buffer_test_get_set_wait_force_stop_wait;
    buffer_test_set_get_blocking_wait_counter = 0;

    buffer_set(&obj, '1');
    if('1' != buf[0]){ errors += 1; }
    if('\0' != buf[1]){ errors += 1; }
    if('\0' != buf[2]){ errors += 1; }
    buffer_set(&obj, '2');
    if('1' != buf[0]){ errors += 1; }
    if('2' != buf[1]){ errors += 1; }
    if('\0' != buf[2]){ errors += 1; }
    buffer_set(&obj, '3');
    if('1' != buf[0]){ errors += 1; }
    if('2' != buf[1]){ errors += 1; }
    if('\0' != buf[2]){ errors += 1; }

    if(10 != buffer_test_set_get_blocking_wait_counter ){ errors += 1; }
    obj.on_wait_set = NULL;
    buffer_start(&obj);



    obj.on_wait_get = buffer_test_get_set_wait_force_stop_wait;
    buffer_test_set_get_blocking_wait_counter = 0;
    c = buffer_get(&obj);
    if('1' != c){ errors += 1; }
    c = buffer_get(&obj);
    if('2' != c){ errors += 1; }
    c = buffer_get(&obj);
    if('\0' != c){ errors += 1; }

    if(10 != buffer_test_set_get_blocking_wait_counter ){ errors += 1; }
    obj.on_wait_get = NULL;
    buffer_start(&obj);



    buffer_set_possible_or_skip(&obj, '3');
    buffer_set_possible_or_skip(&obj, '4');

    c = buffer_get(&obj);
    if('3' != c ){ errors += 1; }

    // Error
    obj.consumer_ptr = obj.last + 2;

    c = buffer_get_available_or_null(&obj);
    if('\0' != c ){ errors += 1; }

    obj.on_wait_get = buffer_test_get_set_wait_force_stop_wait;
    buffer_test_set_get_blocking_wait_counter = 0;

    c = buffer_get(&obj);
    if('\0' != c ){ errors += 1; }

    if(0 != buffer_test_set_get_blocking_wait_counter ){ errors += 1; }
    obj.on_wait_set = NULL;
    buffer_start(&obj);

    return errors;
}

static int buffer_test_buffer_write(void)
{
    int errors = 0;
    char buf_set[20] = { 0 };
    size_t size;

    char ref20[20] = { "Hello World\n-------"};
    ref20[19] = '-';

    char ref10[20] = { "Hello Worl---------"};
    ref10[19] = '-';


    buffer_t o20 = BUFFER_INIT(buf_set, sizeof(buf_set), true);

    size_t length_too_long = 15;
    if(true != buffer_clear(&o20)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '-'; }
    size = buffer_write(&o20, "Hello World\n", length_too_long);
    if(12 != size ){ errors += 1; }
    if(12 != buffer_length(&o20) ){ errors += 1; }
    if(0 != memcmp(buf_set, ref20, 20)){ errors += 1; }



    size_t length_match = 12;
    if(true != buffer_clear(&o20)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '-'; }
    size = buffer_write(&o20, "Hello World\n", length_match);
    if(12 != size ){ errors += 1; }
    if(12 != buffer_length(&o20) ){ errors += 1; }
    if(0 != memcmp(buf_set, ref20, 20)){ errors += 1; }



    size_t length_too_low = 10;
    if(true != buffer_clear(&o20)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '-'; }
    size = buffer_write(&o20, "Hello World\n", length_too_low);
    if(10 != size ){ errors += 1; }
    if(10 != buffer_length(&o20) ){ errors += 1; }
    if(0 != memcmp(buf_set, ref10, 20)){ errors += 1; }



    if(true != buffer_clear(&o20)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '-'; }
    size = buffer_write(&o20, "I will fill the buffer so that it overflows", 43);
    if(20 != size ){ errors += 1; }
    if(20 != buffer_length(&o20) ){ errors += 1; }
    if(true != buffer_is_full(&o20) ){ errors += 1; }

    return errors;
}

static int buffer_test_buffer_read(void)
{
    int errors = 0;
    char buf_set[20] = { 0 };
    char buf_get[30];

    char ref30[30] = { "Hello World\n\0----------------"};
    ref30[29] = '-';

    char ref12[30] = { "Hello World\0-----------------"};
    ref12[29] = '-';

    char ref8[30]  = { "Hello W\0---------------------"};
    ref8[29] = '-';

    size_t size;

    buffer_t obj = BUFFER_INIT(buf_set, sizeof(buf_set), true);

    size_t length_too_long = 15;
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    if(true != buffer_clear(&obj)){ errors += 1; }
    size = buffer_write(&obj, "Hello World\n", length_too_long);
    if(12 != size ){ errors += 1; }

    for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
    buffer_read(&obj, buf_get, sizeof(buf_get));
    if(0 != memcmp(buf_get, ref30, 30)){ errors += 1; }



    if(true != buffer_clear(&obj)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    size = buffer_write(&obj, "Hello World\n", length_too_long);
    if(12 != size ){ errors += 1; }

    for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
    buffer_read(&obj, buf_get, 13);
    if(0 != memcmp(buf_get, ref30, 30)){ errors += 1; }



    if(true != buffer_clear(&obj)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    size = buffer_write(&obj, "Hello World\n", length_too_long);
    if(12 != size ){ errors += 1; }

    for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
    buffer_read(&obj, buf_get, 12);
    if(0 != memcmp(buf_get, ref12, 30)){ errors += 1; }



    if(true != buffer_clear(&obj)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    size = buffer_write(&obj, "Hello World\n", length_too_long);
    if(12 != size ){ errors += 1; }

    for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
    buffer_read(&obj, buf_get, 8);
    if(0 != memcmp(buf_get, ref8, 30)){ errors += 1; }



    if(true != buffer_clear(&obj)){ errors += 1; }
    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    buffer_set_possible_or_skip(&obj, 'H');
    buffer_set_possible_or_skip(&obj, 'e');
    buffer_set_possible_or_skip(&obj, 'l');
    buffer_set_possible_or_skip(&obj, 'l');
    buffer_set_possible_or_skip(&obj, 'o');
    buffer_set_possible_or_skip(&obj, ' ');
    buffer_set_possible_or_skip(&obj, 'W');
    buffer_set_possible_or_skip(&obj, 'o');
    buffer_set_possible_or_skip(&obj, 'r');
    buffer_set_possible_or_skip(&obj, 'l');
    buffer_set_possible_or_skip(&obj, 'd');
    buffer_set_possible_or_skip(&obj, '\n');
    if(12 != size ){ errors += 1; }

    for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
    buffer_read(&obj, buf_get, 30);
    if(0 != memcmp(buf_get, ref30, 30)){ errors += 1; }

    return errors;
}

static int buffer_test_buffer_read_line(void)
{
    int errors = 0;
    char buf_set[30];
    char buf_get[30];
    size_t size;

    char in5[30] = { "I\nYou\nHe\nShe\nIt\0---" "----------"};
    in5[29] = '-';

    char in6[30] = { "I\nYou\nHe\nShe\nIt\n\0--""----------"};
    in6[29] = '-';

    char in7[] = { "This is not a line"};

    char ref1[30] = { "I\0" "-------" "----------" "----------" }; ref1[29] = '-';
    char ref2[30] = { "You\0" "-----" "----------" "----------" }; ref2[29] = '-';
    char ref3[30] = { "He\0" "------" "----------" "----------" }; ref3[29] = '-';
    char ref4[30] = { "She\0" "-----" "----------" "----------" }; ref4[29] = '-';
    char ref5[30] = { "It\0" "------" "----------" "----------" }; ref5[29] = '-';

    char * refs[] = { ref1, ref2, ref3, ref4, ref5, NULL };




    buffer_t obj = BUFFER_INIT(buf_set, sizeof(buf_set), true);

    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    if(true != buffer_clear(&obj)){ errors += 1; }
    size = buffer_write(&obj, in5, sizeof(in5));
    if(15 != size ){ errors += 1; }
    if(4 != buffer_lines(&obj) ){ errors += 1; }

    for(size_t i = 0; i < 4; i++)
    {
        for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
        size = buffer_read_line(&obj, buf_get, sizeof(buf_get));
        if(size != strlen(refs[i])){ errors += 1; }
        if(size != strlen(buf_get)){ errors += 1; }
        if(0 != memcmp(buf_get, refs[i], 30)){ errors += 1; }
    }



    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    if(true != buffer_clear(&obj)){ errors += 1; }
    size = buffer_write(&obj, in6, sizeof(in6));
    if(16 != size ){ errors += 1; }
    if(5 != buffer_lines(&obj) ){ errors += 1; }

    for(size_t i = 0; i < 5; i++)
    {
        for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
        size = buffer_read_line(&obj, buf_get, sizeof(buf_get));
        if(size != strlen(refs[i])){ errors += 1; }
        if(size != strlen(buf_get)){ errors += 1; }
        if(0 != memcmp(buf_get, refs[i], 30)){ errors += 1; }
    }


    for(size_t i = 0; i < sizeof(buf_set); i++){ buf_set[i] = '+'; }
    if(true != buffer_clear(&obj)){ errors += 1; }
    size = buffer_write(&obj, in7, sizeof(in7));
    if((sizeof(in7)-1) != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }

    size = buffer_read_line(&obj, buf_get, sizeof(buf_get));
    if(0 != size ){ errors += 1; }





    return errors;
}

static int buffer_test_buffer_read_to(void)
{
    int errors = 0;
    char buf_set[30];
    char buf_get[30];
    for(size_t i = 0; i < sizeof(buf_get); i++){ buf_get[i] = '-'; }
    char ref_empty[30] = { "---------"  "----------" "----------" }; ref_empty[29] = '-';
    char ref_full [30] = { "eoteot\0--" "----------" "----------" }; ref_full[29] = '-';
    size_t size;

    buffer_t obj = BUFFER_INIT(buf_set, sizeof(buf_set), true);

    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }


    buffer_set(&obj, 'e');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(1 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, 'o');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(2 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, 't');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(3 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }


    buffer_set(&obj, 'e');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(4 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, 'o');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(5 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, 't');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(6 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }

    buffer_set(&obj, 'e');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(7 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, 'o');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(8 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, 't');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(0 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(9 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_empty, 30)){ errors += 1; }
    buffer_set(&obj, '\n');
    size = buffer_read_to(&obj, buf_get, sizeof(buf_get), "eot\n", 4);
    if(6 != size ){ errors += 1; }
    if(0 != buffer_lines(&obj) ){ errors += 1; }
    if(0 != buffer_length(&obj) ){ errors += 1; }
    if(0 != memcmp(buf_get, ref_full, 30)){ errors += 1; }

    return errors;
}

static int buffer_test_buffer_object_allocate_free(void)
{
    int errors = 0;

    char buf[10];
    char c;
    buffer_t * p = buffer_object_allocate(NULL, 0, true);
    if(true != buffer_is_stopped(p) ){ errors += 1; }
    if(NULL != p)
    {
        buffer_start(p);
        if(true != buffer_is_stopped(p) ){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('\0' == c){ c = '\0'; }
        c = buffer_get_available_or_null(p);
        if('\0' != c ){ errors += 1; }
        if(0 != buffer_length(p) ){ errors += 1; }
        buffer_set_possible_or_skip(p, 'A');
        if(0 != buffer_length(p) ){ errors += 1; }
        buffer_set_possible_or_skip(p, 'B');
        if(0 != buffer_length(p) ){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('\0' != c ){ errors += 1; }
        if(0 != buffer_length(p) ){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('\0' != c ){ errors += 1; }
        if(0 != buffer_length(p) ){ errors += 1; }

        buffer_init(p, buf, sizeof(buf), true);
        if(false != buffer_is_stopped(p) ){ errors += 1; }
        buffer_start(p);
        if(false != buffer_is_stopped(p) ){ errors += 1; }

        c = buffer_get_available_or_null(p);
        if('\0' == c){ c = '\0'; }
        c = buffer_get_available_or_null(p);
        if('\0' == c){ c = '\0'; }
        buffer_set_possible_or_skip(p, 'A');
        if(1 != buffer_length(p) ){ errors += 1; }
        buffer_set_possible_or_skip(p, 'B');
        if(2 != buffer_length(p) ){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('A' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('B' != c){ errors += 1; }

        buffer_object_free(p);
    }
    else
    {
        errors += 1;
    }
    return errors;
}

static int buffer_test_buffer_object_allocate_null_free(void)
{
    int errors = 0;

    char c;
    buffer_t * p = buffer_object_allocate(NULL, 10, true);
    if(false != buffer_is_stopped(p) ){ errors += 1; }
    if(NULL != p)
    {
        c = buffer_get_available_or_null(p);
        if('\0' == c){ c = '\0'; }
        c = buffer_get_available_or_null(p);
        if('\0' == c){ c = '\0'; }
        buffer_set_possible_or_skip(p, '1');
        if(1 != buffer_length(p) ){ errors += 1; }
        buffer_set_possible_or_skip(p, '2');
        buffer_set_possible_or_skip(p, '3');
        buffer_set_possible_or_skip(p, '4');
        buffer_set_possible_or_skip(p, '5');
        buffer_set_possible_or_skip(p, '6');
        buffer_set_possible_or_skip(p, '7');
        buffer_set_possible_or_skip(p, '8');
        buffer_set_possible_or_skip(p, '9');
        buffer_set_possible_or_skip(p, 'A');
        buffer_set_possible_or_skip(p, 'B');
        buffer_set_possible_or_skip(p, 'C');
        buffer_set_possible_or_skip(p, 'D');
        buffer_set_possible_or_skip(p, 'E');
        buffer_set_possible_or_skip(p, 'F');

        c = buffer_get_available_or_null(p);
        if('1' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('2' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('3' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('4' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('5' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('6' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('7' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('8' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('9' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('A' != c){ errors += 1; }
        c = buffer_get_available_or_null(p);
        if('\0' != c){ errors += 1; }

        buffer_object_free(p);
    }
    else
    {
        errors += 1;
    }
    return errors;
}

static void buffer_run_example_handler(buffer_t * object)
{
    char buf[10];

    buffer.ReadLine(object, buf, sizeof(buf));

    printf("Out: %s\n", buf);
    fflush(stdout);
}

static void buffer_run_example_1(void)
{
    char buf[10];
    buffer_t obj;

    buffer.Init(&obj, buf, sizeof(buf), true);
    obj.on_new_line = buffer_run_example_handler;

    buffer.Write(&obj, "Hi you", 2);
    buffer.Set(&obj, '\n');
}

static void buffer_run_example_2(void)
{
    buffer_t * p = buffer.ObjectAllocate(NULL, 10, true);
    if(NULL != p)
    {
        p->on_new_line = buffer_run_example_handler;
        buffer.Write(p, "Hi you", 2);
        buffer.Set(p, '\n');
    }
    buffer.ObjectFree(p);
}

static char uart_buf[10];
static buffer_t uart = BUFFER_INIT(uart_buf, sizeof(uart_buf), false);

static void buffer_run_example_3_init(void)
{
    buffer.Start(&uart);
    uart.end_of_line_character = '\r';
}

static void buffer_run_example_3_receive(void)
{
    buffer.SetPossibleOrSkip(&uart, 'H');
    buffer.SetPossibleOrSkip(&uart, 'i');
    buffer.SetPossibleOrSkip(&uart, '\r');
}

static void buffer_run_example_3_main_loop(void)
{
    char input[10];

    // while(1) {

    if(buffer.ReadLine(&uart, input, sizeof(input)))
    {
        printf("Out: %s\n", input);
        fflush(stdout);
    }

    // }
}

/*---------------------------------------------------------------------*
 *  public:  functions
 *---------------------------------------------------------------------*/

int buffer_testbench(void)
{
    int errors = 0;

    errors += buffer_test_init();
    errors += buffer_test_init_sizeof_data_zero();
    errors += buffer_test_reset();
    errors += buffer_test_copy();
    errors += buffer_test_set_get_non_blocking();
    errors += buffer_test_set_get_blocking();
    errors += buffer_test_look();
    errors += buffer_test_clean();
    errors += buffer_test_length();
    errors += buffer_test_new_line();
    errors += buffer_test_get_over_last();
    errors += buffer_test_stop();
    errors += buffer_test_stop_start_set_get();
    errors += buffer_test_get_set_wait_force_stop();
    errors += buffer_test_buffer_write();
    errors += buffer_test_buffer_read();
    errors += buffer_test_buffer_read_line();
    errors += buffer_test_buffer_read_to();
    errors += buffer_test_buffer_object_allocate_free();
    errors += buffer_test_buffer_object_allocate_null_free();

    buffer_run_example_1();
    buffer_run_example_2();
    buffer_run_example_3_init();
    buffer_run_example_3_receive();
    buffer_run_example_3_main_loop();

    return errors;
}


/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/
