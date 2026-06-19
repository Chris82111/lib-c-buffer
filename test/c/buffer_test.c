#include <buffer.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(x) (void)(x)


static int buffer_test_init (void)
{
  int errors = 0;

  size_t sizeof_buf = 16;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 255;

  buffer_t b = BUFFER_INIT(buf, sizeof_buf);
  if (buf != b.c_head) { errors++; }
  if (buf != b.c_tail) { errors++; }
  if (0 != b.c_inc) { errors++; }
  if (buf != b.p_head) { errors++; }
  if (buf != b.p_tail) { errors++; }
  if (0 != b.p_inc) { errors++; }
  if (buf != b.data) { errors++; }
  if (buf + sizeof_buf - 1 != b.last) { errors++; }
  if (0 != b.lines) { errors++; }
  if (0 != b.c_lines) { errors++; }
  if (0 != b.p_lines) { errors++; }
  if ('\n' != b.end_of_line_character) { errors++; }
  if (NULL != b.on_sleep) { errors++; }
  if (NULL != b.user_data) { errors++; }
  memset(&b, 0xff, sizeof(b));


  b = BUFFER_INIT(buf, sizeof_buf);
  if (buf != b.c_head) { errors++; }
  if (buf != b.c_tail) { errors++; }
  if (0 != b.c_inc) { errors++; }
  if (buf != b.p_head) { errors++; }
  if (buf != b.p_tail) { errors++; }
  if (0 != b.p_inc) { errors++; }
  if (buf != b.data) { errors++; }
  if (buf + sizeof_buf - 1 != b.last) { errors++; }
  if (0 != b.lines) { errors++; }
  if (0 != b.c_lines) { errors++; }
  if (0 != b.p_lines) { errors++; }
  if ('\n' != b.end_of_line_character) { errors++; }
  if (NULL != b.on_sleep) { errors++; }
  if (NULL != b.user_data) { errors++; }
  memset(&b, 0xff, sizeof(b));


  buffer.Init(&b, buf, sizeof_buf);
  if (buf != b.c_head) { errors++; }
  if (buf != b.c_tail) { errors++; }
  if (0 != b.c_inc) { errors++; }
  if (buf != b.p_head) { errors++; }
  if (buf != b.p_tail) { errors++; }
  if (0 != b.p_inc) { errors++; }
  if (buf != b.data) { errors++; }
  if (buf + sizeof_buf - 1 != b.last) { errors++; }
  if (0 != b.lines) { errors++; }
  if (0 != b.c_lines) { errors++; }
  if (0 != b.p_lines) { errors++; }
  if ('\n' != b.end_of_line_character) { errors++; }
  if (NULL != b.on_sleep) { errors++; }
  if (NULL != b.user_data) { errors++; }


  return errors;
}

static int buffer_test_set_sleep_counter = 4;
static char buffer_test_set_sleep(buffer_t * object)
{
  UNUSED(object);

  buffer_test_set_sleep_counter--;
  if (0 == buffer_test_set_sleep_counter)
  {
    return 1;
  }
  return 0;
}

static int buffer_test_set_get (void)
{
  int errors = 0;

  size_t sizeof_buf = 4;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 255;

  buffer_t b = BUFFER_INIT(buf, sizeof_buf);

  buffer.Set(&b, 'a');
  if ('a' != buf[0]) { errors++; }

  buffer.Set(&b, 'b');
  if ('b' != buf[1]) { errors++; }

  buffer.Set(&b, 'c');
  if ('c' != buf[2]) { errors++; }

  buffer_test_set_sleep_counter = 4;
  b.on_sleep = buffer_test_set_sleep;

  // Full and break
  buffer.Set(&b, 'd');



  char c;

  c = buffer.Get(&b);
  if ('a' != c) { errors++; }

  buffer.Set(&b, 'd');
  if ('d' != buf[3]) { errors++; }

  c = buffer.Get(&b);
  if ('b' != c) { errors++; }

  buffer.Set(&b, 'e');
  if ('e' != buf[0]) { errors++; }

  c = buffer.Get(&b);
  if ('c' != c) { errors++; }

  buffer.Set(&b, 'f');
  if ('f' != buf[1]) { errors++; }

  c = buffer.Get(&b);
  if ('d' != c) { errors++; }

  buffer.Set(&b, 'g');
  if ('g' != buf[2]) { errors++; }

  c = buffer.Get(&b);
  if ('e' != c) { errors++; }

  buffer.Set(&b, 'h');
  if ('h' != buf[3]) { errors++; }

  c = buffer.Get(&b);
  if ('f' != c) { errors++; }

  buffer.Set(&b, 'i');
  if ('i' != buf[0]) { errors++; }

  c = buffer.Get(&b);
  if ('g' != c) { errors++; }

  buffer.Set(&b, 'j');
  if ('j' != buf[1]) { errors++; }

  c = buffer.Get(&b);
  if ('h' != c) { errors++; }

  buffer.Set(&b, 'k');
  if ('k' != buf[2]) { errors++; }

  c = buffer.Get(&b);
  if ('i' != c) { errors++; }

  buffer.Set(&b, 'l');
  if ('l' != buf[3]) { errors++; }

  c = buffer.Get(&b);
  if ('j' != c) { errors++; }

  buffer.Set(&b, 'm');
  if ('m' != buf[0]) { errors++; }

  c = buffer.Get(&b);
  if ('k' != c) { errors++; }

  buffer.Set(&b, 'n');
  if ('n' != buf[1]) { errors++; }

  c = buffer.Get(&b);
  if ('l' != c) { errors++; }

  buffer.Set(&b, 'o');
  if ('o' != buf[2]) { errors++; }

  c = buffer.Get(&b);
  if ('m' != c) { errors++; }

  buffer.Set(&b, 'p');
  if ('p' != buf[3]) { errors++; }

  c = buffer.Get(&b);
  if ('n' != c) { errors++; }

  buffer.Set(&b, 'q');
  if ('q' != buf[0]) { errors++; }

  c = buffer.Get(&b);
  if ('o' != c) { errors++; }

  buffer.Set(&b, 'r');
  if ('r' != buf[1]) { errors++; }

  c = buffer.Get(&b);
  if ('p' != c) { errors++; }

  buffer.Set(&b, 's');
  if ('s' != buf[2]) { errors++; }

  c = buffer.Get(&b);
  if ('q' != c) { errors++; }

  buffer.Set(&b, 't');
  if ('t' != buf[3]) { errors++; }

  c = buffer.Get(&b);
  if ('r' != c) { errors++; }

  buffer.Set(&b, 'u');
  if ('u' != buf[0]) { errors++; }

  c = buffer.Get(&b);
  if ('s' != c) { errors++; }

  buffer.Set(&b, 'v');
  if ('v' != buf[1]) { errors++; }

  c = buffer.Get(&b);
  if ('t' != c) { errors++; }

  buffer.Set(&b, 'w');
  if ('w' != buf[2]) { errors++; }

  c = buffer.Get(&b);
  if ('u' != c) { errors++; }

  buffer.Set(&b, 'x');
  if ('x' != buf[3]) { errors++; }

  c = buffer.Get(&b);
  if ('v' != c) { errors++; }

  buffer.Set(&b, 'y');
  if ('y' != buf[0]) { errors++; }

  c = buffer.Get(&b);
  if ('w' != c) { errors++; }

  buffer.Set(&b, 'z');
  if ('z' != buf[1]) { errors++; }



  c = buffer.Get(&b);
  if ('x' != c) { errors++; }

  c = buffer.Get(&b);
  if ('y' != c) { errors++; }

  c = buffer.Get(&b);
  if ('z' != c) { errors++; }


  buffer_test_set_sleep_counter = 4;
  b.on_sleep = buffer_test_set_sleep;

  // Empty and break
  c = buffer.Get(&b);
  if ('\0' != c) { errors++; }

  return errors;
}

static int buffer_test_try_set_try_get (void)
{
  int errors = 0;

  size_t sizeof_buf = 4;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 255;

  buffer_t b = BUFFER_INIT(buf, sizeof_buf);

  bool worked;

  worked = buffer.TrySet(&b, 'a');
  if ('a' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'b');
  if ('b' != buf[1]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'c');
  if ('c' != buf[2]) { errors++; }
  if (true != worked) { errors++; }

  // Full
  worked = buffer.TrySet(&b, 'd');
  if (false != worked) { errors++; }



  char c;

  worked = buffer.TryGet(&b, &c);
  if ('a' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'd');
  if ('d' != buf[3]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('b' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'e');
  if ('e' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('c' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'f');
  if ('f' != buf[1]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('d' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'g');
  if ('g' != buf[2]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('e' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'h');
  if ('h' != buf[3]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('f' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'i');
  if ('i' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('g' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'j');
  if ('j' != buf[1]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('h' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'k');
  if ('k' != buf[2]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('i' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'l');
  if ('l' != buf[3]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('j' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'm');
  if ('m' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('k' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'n');
  if ('n' != buf[1]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('l' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'o');
  if ('o' != buf[2]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('m' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'p');
  if ('p' != buf[3]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('n' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'q');
  if ('q' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('o' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'r');
  if ('r' != buf[1]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('p' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 's');
  if ('s' != buf[2]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('q' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 't');
  if ('t' != buf[3]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('r' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'u');
  if ('u' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('s' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'v');
  if ('v' != buf[1]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('t' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'w');
  if ('w' != buf[2]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('u' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'x');
  if ('x' != buf[3]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('v' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'y');
  if ('y' != buf[0]) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('w' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TrySet(&b, 'z');
  if ('z' != buf[1]) { errors++; }
  if (true != worked) { errors++; }



  worked = buffer.TryGet(&b, &c);
  if ('x' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('y' != c) { errors++; }
  if (true != worked) { errors++; }

  worked = buffer.TryGet(&b, &c);
  if ('z' != c) { errors++; }
  if (true != worked) { errors++; }


  // Empty
  c = 0xff;
  worked = buffer.TryGet(&b, &c);
  if ((char)(0xff) != c) { errors++; }
  if (false != worked) { errors++; }

  return errors;
}

static int buffer_test_write_read_bytes (void)
{
  int errors = 0;

  size_t sizeof_buf = 9;
  size_t i;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 0;

  buffer_t b;

  b = BUFFER_INIT(buf, sizeof_buf);
  typedef union u64_u
  {
    char array[];
    uint64_t number;
  } u64_t;

  u64_t in;
  u64_t out;

  in.number = 0;
  i = buffer.WriteBytes(&b, in.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (0 != buf[sizeof_buf]) { errors++; }
  i = buffer.ReadBytes(&b, out.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (in.number != out.number) { errors++; }


  in.number = UINT8_MAX;
  i = buffer.WriteBytes(&b, in.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (0 != buf[sizeof_buf]) { errors++; }
  i = buffer.ReadBytes(&b, out.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (in.number != out.number) { errors++; }


  in.number = UINT16_MAX;
  i = buffer.WriteBytes(&b, in.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (0 != buf[sizeof_buf]) { errors++; }
  i = buffer.ReadBytes(&b, out.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (in.number != out.number) { errors++; }


  in.number = UINT32_MAX;
  i = buffer.WriteBytes(&b, in.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (0 != buf[sizeof_buf]) { errors++; }
  i = buffer.ReadBytes(&b, out.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (in.number != out.number) { errors++; }


  in.number = UINT64_MAX;
  i = buffer.WriteBytes(&b, in.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (0 != buf[sizeof_buf]) { errors++; }
  i = buffer.ReadBytes(&b, out.array, sizeof(u64_t), 8);
  if (8 != i) { errors++; }
  if (in.number != out.number) { errors++; }



  // Limit of array size
  //
  char arr1[5] = { 1, 2, 3, 4, 5 };
  i = buffer.WriteBytes(&b, arr1, sizeof(arr1), SIZE_MAX);
  if (5 != i) { errors++; }
  char arr2[5] = { 0xff, 0xff, 0xff, 0xff, 0xff };
  i = buffer.ReadBytes(&b, arr2, sizeof(arr2), SIZE_MAX);
  if (5 != i) { errors++; }
  if (sizeof(arr1) != sizeof(arr2)) { errors++; }
  if (0 != memcmp(arr1, arr2, sizeof(arr1))) { errors++; }


  return errors;
}

static int buffer_test_try_write_and_try_read_bytes (void)
{
  int errors = 0;

  size_t sizeof_buf = 5;
  size_t i;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 0;

  buffer_t b;

  b = BUFFER_INIT(buf, sizeof_buf);
  typedef union u64_u
  {
    char array[];
    uint64_t number;
  } u64_t;

  u64_t in;
  u64_t out;


  in.number = UINT64_MAX;
  i = buffer.TryWriteBytes(&b, in.array, sizeof(u64_t), 8);
  if (4 != i) { errors++; }
  if (0 != buf[sizeof_buf]) { errors++; }
  i = buffer.TryReadBytes(&b, out.array, sizeof(u64_t), 8);
  if (4 != i) { errors++; }
  if (in.number != out.number) { errors++; }


  // Limit of array size
  //
  char arr1[5] = { 1, 2, 3, 4, 5 };
  i = buffer.TryWriteBytes(&b, arr1, sizeof(arr1), SIZE_MAX);
  if (4 != i) { errors++; }
  char arr2[5] = { 0xff, 0xff, 0xff, 0xff, 0xff };
  i = buffer.TryReadBytes(&b, arr2, sizeof(arr2), SIZE_MAX);
  if (4 != i) { errors++; }
  if (sizeof(arr1) != sizeof(arr2)) { errors++; }
  if (0 != memcmp(arr1, arr2, 4)) { errors++; }


  return errors;
}

static int buffer_test_try_peek_bytes (void)
{
  int errors = 0;

  size_t sizeof_buf = 16;
  size_t sizeof_read = 16;
  size_t i;

  char buf[sizeof_buf + 1];
  char read[sizeof_buf + 1];
  //char value;

  buffer_t b;

  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekBytes(&b, read, sizeof_read, 2);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (2 != i) { errors++; }

  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (4 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekBytes(&b, read, 4, 6);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (4 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekBytes(&b, read, 4, 4);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (4 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekBytes(&b, read, 4, 3);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (3 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcdefghijkl", 12, 99);
  i = buffer.ReadBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("abcdefghijkl", read, i)) { errors++; }
  if (4 != i) { errors++; }
  i = buffer.ReadBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("efghijkl", read, i)) { errors++; }
  if (4 != i) { errors++; }
  i = buffer.ReadBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("ijkl", read, i)) { errors++; }
  if (4 != i) { errors++; }
  i = buffer.WriteBytes(&b, "mnopqrst", 8, 99);
  i = buffer.TryPeekBytes(&b, read, sizeof_read, 2);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }
  i = buffer.TryPeekBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }
  i = buffer.TryPeekBytes(&b, read, sizeof_read, 6);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }
  i = buffer.TryPeekBytes(&b, read, sizeof_read, 8);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }

  return errors;
}

static int buffer_test_methadata (void)
{
  int errors = 0;

  size_t sizeof_buf = 4+1;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 255;

  bool boolean;

  buffer_t b = BUFFER_INIT(buf, sizeof_buf);
  if (0 != buffer.Length(&b)) { errors++; }
  if (0 != buffer.Lines(&b)) { errors++; }
  if (4 != buffer.Space(&b)) { errors++; }
  if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
  if ((boolean = true) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Set(&b, 'a');
   if (1 != buffer.Length(&b)) { errors++; }
   if (0 != buffer.Lines(&b)) { errors++; }
   if (3 != buffer.Space(&b)) { errors++; }
   if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
   if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Set(&b, 'b');
   if (2 != buffer.Length(&b)) { errors++; }
   if (0 != buffer.Lines(&b)) { errors++; }
   if (2 != buffer.Space(&b)) { errors++; }
   if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
   if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Set(&b, 'c');
   if (3 != buffer.Length(&b)) { errors++; }
   if (0 != buffer.Lines(&b)) { errors++; }
   if (1 != buffer.Space(&b)) { errors++; }
   if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
   if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Set(&b, 'd');
   if (4 != buffer.Length(&b)) { errors++; }
   if (0 != buffer.Lines(&b)) { errors++; }
   if (0 != buffer.Space(&b)) { errors++; }
   if ((boolean = true) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
   if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Get(&b);
   if (3 != buffer.Length(&b)) { errors++; }
   if (0 != buffer.Lines(&b)) { errors++; }
   if (1 != buffer.Space(&b)) { errors++; }
   if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
   if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Get(&b);
   if (2 != buffer.Length(&b)) { errors++; }
   if (0 != buffer.Lines(&b)) { errors++; }
   if (2 != buffer.Space(&b)) { errors++; }
   if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
   if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

   buffer.Get(&b);
    if (1 != buffer.Length(&b)) { errors++; }
    if (0 != buffer.Lines(&b)) { errors++; }
    if (3 != buffer.Space(&b)) { errors++; }
    if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
    if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

    buffer.Get(&b);
    if (0 != buffer.Length(&b)) { errors++; }
    if (0 != buffer.Lines(&b)) { errors++; }
    if (4 != buffer.Space(&b)) { errors++; }
    if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
    if ((boolean = true) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

    buffer.Set(&b, '\n');
    if (1 != buffer.Length(&b)) { errors++; }
    if (1 != buffer.Lines(&b)) { errors++; }
    if (3 != buffer.Space(&b)) { errors++; }
    if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
    if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

    buffer.Set(&b, 'e');
    if (2 != buffer.Length(&b)) { errors++; }
    if (1 != buffer.Lines(&b)) { errors++; }
    if (2 != buffer.Space(&b)) { errors++; }
    if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
    if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

    buffer.Set(&b, '\n');
    if (3 != buffer.Length(&b)) { errors++; }
    if (2 != buffer.Lines(&b)) { errors++; }
    if (1 != buffer.Space(&b)) { errors++; }
    if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
    if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

    buffer.Get(&b);
    if (2 != buffer.Length(&b)) { errors++; }
    if (1 != buffer.Lines(&b)) { errors++; }
    if (2 != buffer.Space(&b)) { errors++; }
    if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
    if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

  return errors;
}

static int buffer_test_clear (void)
{
  int errors = 0;

  size_t sizeof_buf = 4+1;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 255;

  bool boolean;

  buffer_t b = BUFFER_INIT(buf, sizeof_buf);

  buffer.Set(&b, 'a');
  buffer.Set(&b, '\n');
  buffer.Set(&b, 'c');

  if (3 != buffer.Length(&b)) { errors++; }
  if (1 != buffer.Lines(&b)) { errors++; }
  if (1 != buffer.Space(&b)) { errors++; }
  if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
  if ((boolean = false) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }


  buffer.Clear(&b);
  if (0 != buffer.Length(&b)) { errors++; }
  if (0 != buffer.Lines(&b)) { errors++; }
  if (4 != buffer.Space(&b)) { errors++; }
  if ((boolean = false) != buffer.IsFull(&b) && (!boolean) != buffer.IsNotFull(&b) ) { errors++; }
  if ((boolean = true) != buffer.IsEmpty(&b) && (!boolean) != buffer.IsNotEmpty(&b) ) { errors++; }

  return errors;
}

static int buffer_test_write_read_c_str (void)
{
  int errors = 0;

  size_t sizeof_buf = 100+1;
  size_t i;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 0;

  buffer_t b;

  b = BUFFER_INIT(buf, sizeof_buf);

  // Limit of array size
  //
  char str1[100] = "Hello World";
  i = buffer.WriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }

  char str2[100] = { 0 };
  i = buffer.ReadCStr(&b, str2, sizeof(str2), buffer.Length(&b));
  if (11 != i) { errors++; }

  if (strlen(str1) != strlen(str2)) { errors++; }
  if (0 != memcmp(str1, str2, strlen(str1))) { errors++; }



  i = buffer.WriteCStr(&b, str1, 5, SIZE_MAX);
  if (5 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.ReadCStr(&b, str2, sizeof(str2), buffer.Length(&b));
  if (5 != i) { errors++; }

  if (0 != memcmp(str1, str2, 5)) { errors++; }



  i = buffer.WriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }

  // Adds '\0', but the array isn't big enough
  str2[100] = (char){ 0 };
  i = buffer.ReadCStr(&b, str2, 5, buffer.Length(&b));
  if (4 != i) { errors++; }
  buffer.Clear(&b);



  i = buffer.WriteCStr(&b, str1, sizeof(str1), 5);
  if (5 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.ReadCStr(&b, str2, sizeof(str2), buffer.Length(&b));
  if (5 != i) { errors++; }

  if (0 != memcmp(str1, str2, 5)) { errors++; }



  i = buffer.WriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.ReadCStr(&b, str2, sizeof(str2), 5);
  if (5 != i) { errors++; }



  return errors;
}

static int buffer_test_try_write_and_try_read_c_str (void)
{
  int errors = 0;

  size_t sizeof_buf = 20+1;
  size_t i;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 0;

  buffer_t b;

  b = BUFFER_INIT(buf, sizeof_buf);

  // Limit of array size
  //
  char str1[100] = "Hello World";
  i = buffer.TryWriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }

  char str2[100] = { 0 };
  i = buffer.TryReadCStr(&b, str2, sizeof(str2), buffer.Length(&b));
  if (11 != i) { errors++; }

  if (strlen(str1) != strlen(str2)) { errors++; }
  if (0 != memcmp(str1, str2, strlen(str1))) { errors++; }



  i = buffer.TryWriteCStr(&b, str1, 5, SIZE_MAX);
  if (5 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.TryReadCStr(&b, str2, sizeof(str2), buffer.Length(&b));
  if (5 != i) { errors++; }

  if (0 != memcmp(str1, str2, 5)) { errors++; }



  i = buffer.TryWriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }

  // Adds '\0', but the array isn't big enough
  str2[100] = (char){ 0 };
  i = buffer.TryReadCStr(&b, str2, 5, buffer.Length(&b));
  if (4 != i) { errors++; }
  buffer.Clear(&b);



  i = buffer.TryWriteCStr(&b, str1, sizeof(str1), 5);
  if (5 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.TryReadCStr(&b, str2, sizeof(str2), buffer.Length(&b));
  if (5 != i) { errors++; }

  if (0 != memcmp(str1, str2, 5)) { errors++; }



  i = buffer.TryWriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.TryReadCStr(&b, str2, sizeof(str2), 5);
  if (5 != i) { errors++; }



  buffer.Clear(&b);

  i = buffer.TryWriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (11 != i) { errors++; }
  i = buffer.TryWriteCStr(&b, str1, sizeof(str1), SIZE_MAX);
  if (9 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.TryReadCStr(&b, str2, sizeof(str2), SIZE_MAX);
  if (20 != i) { errors++; }

  str2[100] = (char){ 0 };
  i = buffer.TryReadCStr(&b, str2, sizeof(str2), SIZE_MAX);
  if (0 != i) { errors++; }


  return errors;
}

static int buffer_test_try_peek_c_str (void)
{
  int errors = 0;

  size_t sizeof_buf = 16;
  size_t sizeof_read = 16;
  size_t i;

  char buf[sizeof_buf + 1];
  char read[sizeof_buf + 1];
  //char value;

  buffer_t b;

  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekCStr(&b, read, sizeof_read, 2);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (2 != i) { errors++; }

  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekCStr(&b, read, sizeof_read, 4);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (4 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekCStr(&b, read, 4, 6);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (3 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekCStr(&b, read, 4, 4);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (3 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcd", 4, 4);
  i = buffer.TryPeekCStr(&b, read, 4, 3);
  if (0 != memcmp("abcd", read, i)) { errors++; }
  if (3 != i) { errors++; }


  b = BUFFER_INIT(buf, sizeof_buf);
  i = buffer.WriteBytes(&b, "abcdefghijkl", 12, 99);
  i = buffer.ReadBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("abcdefghijkl", read, i)) { errors++; }
  if (4 != i) { errors++; }
  i = buffer.ReadBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("efghijkl", read, i)) { errors++; }
  if (4 != i) { errors++; }
  i = buffer.ReadBytes(&b, read, sizeof_read, 4);
  if (0 != memcmp("ijkl", read, i)) { errors++; }
  if (4 != i) { errors++; }
  i = buffer.WriteBytes(&b, "mnopqrst", 8, 99);
  i = buffer.TryPeekCStr(&b, read, sizeof_read, 2);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }
  i = buffer.TryPeekCStr(&b, read, sizeof_read, 4);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }
  i = buffer.TryPeekCStr(&b, read, sizeof_read, 6);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }
  i = buffer.TryPeekCStr(&b, read, sizeof_read, 8);
  if (0 != memcmp("mnopqrst", read, i)) { errors++; }

  return errors;
}

static int buffer_test_read_line_and_try_read_line (void)
{
  int errors = 0;

  size_t sizeof_buf = 20+1;

  char buf[sizeof_buf + 1];
  buf[sizeof_buf] = 255;

  char read[sizeof_buf + 1];
  int i;

  buffer_t b = BUFFER_INIT(buf, sizeof_buf);

  buffer.WriteCStr(&b, "Hello\nWorld\n", sizeof("Hello\nWorld\n"), SIZE_MAX);
  if (2 != buffer.Lines(&b)) { errors++; }

  i = buffer.ReadLine(&b, read, sizeof_buf);
  if (5 != i) { errors++; }
  if (1 != buffer.Lines(&b)) { errors++; }
  if (0 != memcmp(read, "Hello", strlen("Hello"))) { errors++; }

  i = buffer.ReadLine(&b, read, sizeof_buf);
  if (5 != i) { errors++; }
  if (0 != buffer.Lines(&b)) { errors++; }
  if (0 != memcmp(read, "World", strlen("World"))) { errors++; }


  buffer_test_set_sleep_counter = 4;
  b.on_sleep = buffer_test_set_sleep;

  i = buffer.ReadLine(&b, read, sizeof_buf);
  if (0 != i) { errors++; }

  buffer.Clear(&b);



  buffer.WriteCStr(&b, "Hello\nWorld\n", sizeof("Hello\nWorld\n"), SIZE_MAX);
  if (2 != buffer.Lines(&b)) { errors++; }

  i = buffer.TryReadLine(&b, read, sizeof_buf);
  if (5 != i) { errors++; }
  if (1 != buffer.Lines(&b)) { errors++; }
  if (0 != memcmp(read, "Hello", strlen("Hello"))) { errors++; }

  i = buffer.TryReadLine(&b, read, sizeof_buf);
  if (5 != i) { errors++; }
  if (0 != buffer.Lines(&b)) { errors++; }
  if (0 != memcmp(read, "World", strlen("World"))) { errors++; }

  i = buffer.TryReadLine(&b, read, sizeof_buf);
  if (0 != i) { errors++; }
  if (0 != buffer.Lines(&b)) { errors++; }


  return errors;
}

static int buffer_test_read_to (void)
{
  int errors = 0;

  size_t sizeof_buf = 16;
  size_t i;
  UNUSED(i);

  char buf[sizeof_buf + 1];
  char out[sizeof_buf + 1];
  buf[sizeof_buf] = 0;

  buffer_t b;
  buffer_try_read_to_t stat = (buffer_try_read_to_t){ 0 };

  b = BUFFER_INIT(buf, sizeof_buf);

  i = buffer.WriteCStr(&b, "Hello World\r\n", SIZE_MAX, SIZE_MAX);
  i = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);

  stat = (buffer_try_read_to_t){ 0 };
  ptrdiff_t j = 0;
  UNUSED(j);

  i = buffer.WriteCStr(&b, "H", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "H", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "e", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "He", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "l", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hel", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "l", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hell", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "o", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello", -j)) { errors++; }

  i = buffer.WriteCStr(&b, " ", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello ", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "W", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello W", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "o", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello Wo", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "r", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello Wor", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "l", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello Worl", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "d", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello World", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "\r", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(-j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello World\r", -j)) { errors++; }

  i = buffer.WriteCStr(&b, "\n", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);
  if ((size_t)(j) != strlen(out)) { errors++; }
  if (0 != memcmp(out, "Hello World\r\n", j)) { errors++; }

  return errors;
}

static int buffer_test_try_read_to (void)
{
  int errors = 0;

  size_t sizeof_buf = 16;
  size_t i;
  UNUSED(i);

  char buf[sizeof_buf + 1];
  char out[sizeof_buf + 1];
  buf[sizeof_buf] = 0;

  buffer_t b;
  buffer_try_read_to_t stat = BUFFER_TRY_READ_TO_INIT();

  b = BUFFER_INIT(buf, sizeof_buf);

  i = buffer.WriteCStr(&b, "Hello World\r\n", SIZE_MAX, SIZE_MAX);
  i = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);

  stat = BUFFER_TRY_READ_TO_INIT();
  ptrdiff_t j = 0;
  UNUSED(j);

  i = buffer.WriteCStr(&b, "H", SIZE_MAX, SIZE_MAX);
  j = buffer.TryReadTo(&b, out, sizeof_buf, "\r\n", 2, &stat);

  return errors;
}

static void example_1 (void)
{
  char buf[32 + 1]; // +1 byte empty
  char read[10 + 1]; // +1 byte '\0'
  buffer_t b = BUFFER_INIT(buf, sizeof(buf));

  buffer.TrySet(&b, '1');
  buffer.TrySet(&b, '\n');
  buffer.TryWriteCStr(&b, "Hello\nWorld\n", sizeof("Hello\nWorld\n"), strlen("Hello\nWorld\n"));

  size_t length;
  size_t line = 0;
  while(0 < (length = buffer.TryReadLine(&b, read, sizeof(read))))
  {
      printf("%zu: %s (%zu)\n", line++, read, length);
      fflush(stdout);
  }
}

static char uart_buf[10 + 1];
static buffer_t uart = BUFFER_INIT(uart_buf, sizeof(uart_buf));

static void example_2_init(void)
{
    uart.end_of_line_character = '\r';
}

static void * example_2_receive(void *arg)
{
  sleep(1);
  buffer.TrySet(&uart, 'H');

  sleep(1);
  buffer.TrySet(&uart, 'i');

  sleep(1);
  buffer.TrySet(&uart, '\r');

  return arg;
}

static void example_2_uart(void)
{
  pthread_t thread1;
  char input[10];
  size_t reads = 0;

  example_2_init();

  pthread_create(&thread1, NULL, example_2_receive, NULL);

  for(reads = 0; true; reads++)
  {
    if(buffer.TryReadLine(&uart, input, sizeof(input)))
    {
      break;
    }
  }

  printf("received: %s\n", input);
  printf("reads   : %zu\n", reads);
  fflush(stdout);

  pthread_join(thread1, NULL);
}

int buffer_test(void)
{
  int errors = 0;

  example_1();
  example_2_uart();

  errors += buffer_test_init();
  errors += buffer_test_set_get();
  errors += buffer_test_try_set_try_get();
  errors += buffer_test_write_read_bytes();
  errors += buffer_test_try_write_and_try_read_bytes();
  errors += buffer_test_try_peek_bytes();
  errors += buffer_test_methadata();
  errors += buffer_test_clear();
  errors += buffer_test_write_read_c_str();
  errors += buffer_test_try_write_and_try_read_c_str();
  errors += buffer_test_try_peek_c_str();
  errors += buffer_test_read_line_and_try_read_line();
  errors += buffer_test_read_to();
  errors += buffer_test_try_read_to();

  return errors;
}
