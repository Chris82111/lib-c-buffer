//! @file
//! @brief The buffer source file.


// ------------------------------------------------------------------------- //
//  private: include files
// ------------------------------------------------------------------------- //

#include <buffer.h>
#include <string.h>
#include <assert.h>


// ------------------------------------------------------------------------- //
//  private: definitions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: macros like functions
// ------------------------------------------------------------------------- //

//! @brief Estimate whether the type is lock-free, based on its size
//!
//! @details This macro maps only the size of a given type `T` to the
//!          corresponding `ATOMIC_*_LOCK_FREE` macro defined in
//!          `<stdatomic.h>`. It provides a compile-time estimation of
//!          whether atomic operations on objects of that size are
//!          lock-free on the target platform.
//!
//!          For precise, runtime determination, use `::buffer_check_lock_free_runtime()`.
//!
//! @param T The type to check for lock-free atomic support
//!
//! @return An integer constant indicating the lock-free property
//! @retval 0 Never lock-free or no matching fundamental type exists
//! @retval 1 Sometimes lock-free
//! @retval 2 Always lock-free
//!
#define ATOMIC_LOCK_FREE_BY_SIZE(T) \
( \
  sizeof(T) == sizeof(char)      ? ATOMIC_CHAR_LOCK_FREE  : \
  sizeof(T) == sizeof(short)     ? ATOMIC_SHORT_LOCK_FREE : \
  sizeof(T) == sizeof(int)       ? ATOMIC_INT_LOCK_FREE   : \
  sizeof(T) == sizeof(long)      ? ATOMIC_LONG_LOCK_FREE  : \
  sizeof(T) == sizeof(long long) ? ATOMIC_LLONG_LOCK_FREE : \
  0 \
) // ;

//! @brief Estimate at compile-time that a type is always lock-free for atomic operations
//!
//! @details If the type is not always lock-free, compilation will fail with
//!          an error message indicating the offending type.
//!
//!          To perform a runtime test use `check_lock_free_runtime()`
//!
//! @param T The type to check for lock-free atomic support.
//!
#define CHECK_LOCK_FREE_COMPILE_TIME(T) \
  _Static_assert(                                  \
    ATOMIC_LOCK_FREE_BY_SIZE(T) == 2,              \
    #T " is not always lock-free on this platform" \
  )                                               // ;


//! @cond INTERNAL

CHECK_LOCK_FREE_COMPILE_TIME(char *);

CHECK_LOCK_FREE_COMPILE_TIME(uint16_t);

CHECK_LOCK_FREE_COMPILE_TIME(ptrdiff_t);

CHECK_LOCK_FREE_COMPILE_TIME(size_t);

#if ATOMIC_POINTER_LOCK_FREE != 2
#error "Atomic pointer is not always lock-free; signal handler may deadlock."
#endif

//! @endcond


// ------------------------------------------------------------------------- //
//  private: typedefs
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public:  variables
// ------------------------------------------------------------------------- //

const struct buffer_sc buffer =
{
  buffer_init,

  buffer_set,
  buffer_try_set,
  buffer_get,
  buffer_wait_get,
  buffer_try_get,

  buffer_write_bytes,
  buffer_try_write_bytes,
  buffer_read_bytes,
  buffer_try_read_bytes,
  buffer_try_peek_bytes,

  buffer_is_empty,
  buffer_is_not_empty,
  buffer_is_full,
  buffer_is_not_full,
  buffer_length,
  buffer_space,
  buffer_lines,

  buffer_write_c_str,
  buffer_try_write_c_str,
  buffer_read_c_str,
  buffer_try_read_c_str,
  buffer_try_peek_c_str,

  buffer_read_line,
  buffer_try_read_line,
  buffer_read_to,
  buffer_try_read_to,

  buffer_clear,
};


// ------------------------------------------------------------------------- //
//  private: function prototypes
// ------------------------------------------------------------------------- //

static char * atomic_fetch_increment_producer(buffer_t * object);
static char * atomic_fetch_increment_consumer(buffer_t * object);

static void try_move_tail_to_head_producer (buffer_t * object);
static void try_move_tail_to_head_consumer (buffer_t * object);


// ------------------------------------------------------------------------- //
//  private: functions
// ------------------------------------------------------------------------- //

//! @brief Reads the next address to be stored, or NULL
//! @param[in,out] object The buffer object
//! @return The next address to be stored, or NULL
static char * atomic_fetch_increment_producer (buffer_t * object)
{
  // In both functions, the types of the fields were swapped. So the
  // "consumer" (`c`) was used instead of the "producer" (`p`), and vice versa.
  // Furthermore, the conditions under which no address is returned differ.

  char * value = atomic_load(&object->p_head);

  char * next = value + 1;

  if (next > object->last)
  {
    next = object->data;
  }

  while (next != atomic_load(&object->c_tail))
  {
    if (atomic_compare_exchange_strong(&object->p_head, &value, next))
    {
      return value;
    }

    next = value + 1;

    if (next > object->last)
    {
      next = object->data;
    }
  }

  return NULL;
}

//! @brief Reads the next address to be loaded, or NULL
//! @param[in,out] object The buffer object
//! @return The next address to be loaded, or NULL
static char * atomic_fetch_increment_consumer (buffer_t * object)
{
  // In both functions, the types of the fields were swapped. So the
  // "consumer" (`c`) was used instead of the "producer" (`p`), and vice versa.
  // Furthermore, the conditions under which no address is returned differ.

  char * value = atomic_load(&object->c_head);

  char * next = value + 1;

  if (next > object->last)
  {
    next = object->data;
  }

  while (value != atomic_load(&object->p_tail))
  {
    if (atomic_compare_exchange_strong(&object->c_head, &value, next))
    {
      return value;
    }

    next = value + 1;

    if (next > object->last)
    {
      next = object->data;
    }
  }

  return NULL;
}

//! @brief Attempts to set the tail pointer to the head pointer.
//! @details Once all write operations are complete, the tail pointer is set to
//! the head pointer. This is done only if no write thread is currently active.
//! @param[in,out] object The buffer object
static void try_move_tail_to_head_producer (buffer_t * object)
{
  // write
  uint16_t inc = 1 + atomic_fetch_add(&object->p_inc, 1);

  // read only
  size_t lines = atomic_load(&object->p_lines);

  char * tail = atomic_load(&object->p_tail);
  char * head = atomic_load(&object->p_head);
  char * next = tail + inc;

  // overflow
  if (next > object->last)
  {
    next -= (object->last - object->data + 1);
  }

  if (next == head)
  {
    // update
    if (atomic_compare_exchange_strong(&object->p_tail, &tail, head))
    {
      atomic_fetch_sub(&object->p_inc, inc);

      atomic_fetch_sub(&object->p_lines, lines);
      atomic_fetch_add(&object->lines, lines);
    }
  }
}

//! @brief Attempts to set the tail pointer to the head pointer.
//! @details Once all read operations are complete, the tail pointer is set to
//! the head pointer. This is done only if no read thread is currently active.
//! @param[in,out] object The buffer object
static void try_move_tail_to_head_consumer (buffer_t * object)
{
  // write
  uint16_t inc = 1 + atomic_fetch_add(&object->c_inc, 1);

  // read only
  size_t lines = atomic_load(&object->c_lines);

  char * tail = atomic_load(&object->c_tail);
  char * head = atomic_load(&object->c_head);
  char * next = tail + inc;

  // overflow
  if (next > object->last)
  {
    next -= (object->last - object->data + 1);
  }

  if (next == head)
  {
    // update
    if (atomic_compare_exchange_strong(&object->c_tail, &tail, head))
    {
      atomic_fetch_sub(&object->c_inc, inc);

      atomic_fetch_sub(&object->c_lines, lines);
      atomic_fetch_sub(&object->lines, lines);
    }
  }
}


// ------------------------------------------------------------------------- //
//  public:  functions
// ------------------------------------------------------------------------- //

bool buffer_check_lock_free_runtime(void)
{
  _Atomic(char *) var1;
  _Atomic(uint16_t) var2;
  _Atomic(ptrdiff_t) var3;
  _Atomic(size_t) var4;

  return atomic_is_lock_free(&var1)
    && atomic_is_lock_free(&var2)
    && atomic_is_lock_free(&var3)
    && atomic_is_lock_free(&var4);
}



void buffer_init (buffer_t * object, char * data, size_t sizeof_data)
{
  if (NULL == object) { return; }

  atomic_store(&object->c_head, data);
  atomic_store(&object->c_tail, data);
  atomic_store(&object->c_inc, 0);
  atomic_store(&object->p_head, data);
  atomic_store(&object->p_tail, data);
  atomic_store(&object->p_inc, 0);
  object->data = data;
  object->last = data + sizeof_data - 1;
  atomic_store(&object->lines, 0);
  atomic_store(&object->c_lines, 0);
  atomic_store(&object->p_lines, 0);
  object->end_of_line_character = '\n';
  object->on_sleep = NULL;
  object->user_data = NULL;
}

bool buffer_set (buffer_t * object, char c)
{
  if (NULL == object) { return false; }

  char * p;

  do
  {
    p = atomic_fetch_increment_producer(object);
    if (NULL != p)
    {
      break;
    }

    if (object->on_sleep && object->on_sleep(object))
    {
      return false;
    }

  } while(true);

  *p = c;

  if (object->end_of_line_character == c)
  {
    atomic_fetch_add(&object->p_lines, 1);
  }

  try_move_tail_to_head_producer(object);

  return true;
}

bool buffer_try_set (buffer_t * object, char c)
{
  if (NULL == object) { return false; }

  char * p;

  p = atomic_fetch_increment_producer(object);
  if (NULL == p)
  {
    return false;
  }

  *p = c;

  if (object->end_of_line_character == c)
  {
    atomic_fetch_add(&object->p_lines, 1);
  }

  try_move_tail_to_head_producer(object);

  return true;
}

char buffer_get (buffer_t * object)
{
  if (NULL == object) { return 0; }

  char c;
  char * p;

  do
  {
    p = atomic_fetch_increment_consumer(object);
    if (NULL != p)
    {
      break;
    }

    if (object->on_sleep && object->on_sleep(object))
    {
      return '\0';
    }

  } while(true);

  c = *p;

  if (object->end_of_line_character == c)
  {
    atomic_fetch_add(&object->c_lines, 1);
  }

  try_move_tail_to_head_consumer(object);

  return c;
}

bool buffer_wait_get (buffer_t * object, char * c)
{
  if (NULL == object) { return false; }

  char * p;

  do
  {
    p = atomic_fetch_increment_consumer(object);
    if (NULL != p)
    {
      break;
    }

    if (object->on_sleep && object->on_sleep(object))
    {
      return false;
    }

  } while(true);

  *c = *p;

  if (object->end_of_line_character == *p)
  {
    atomic_fetch_add(&object->c_lines, 1);
  }

  try_move_tail_to_head_consumer(object);

  return true;
}

bool buffer_try_get (buffer_t * object, char * c)
{
  if (NULL == object) { return false; }

  char * p;

  p = atomic_fetch_increment_consumer(object);
  if (NULL == p)
  {
    return false;
  }

  *c = *p;

  if (object->end_of_line_character == *p)
  {
    atomic_fetch_add(&object->c_lines, 1);
  }

  try_move_tail_to_head_consumer(object);

  return true;
}



size_t buffer_write_bytes (buffer_t * object, const char *src, size_t sizeof_src, size_t bytes)
{
  if ((NULL == object    ) ||
      (NULL == src       ) ||
      (0    == sizeof_src) ||
      (0    == bytes     ) )
  {
    return 0;
  }

  size_t count = sizeof_src;
  if (bytes < count)
  {
    count = bytes;
  }

  size_t i;

  for (i = 0; i < sizeof_src; src++, i++)
  {
    if (false == buffer_set(object, *src))
    {
      break;
    }
  }

  return i;
}

size_t buffer_try_write_bytes (buffer_t * object, const char *src, size_t sizeof_src, size_t bytes)
{
  if ((NULL == object    ) ||
      (NULL == src       ) ||
      (0    == sizeof_src) ||
      (0    == bytes     ) )
  {
    return 0;
  }

  size_t count = sizeof_src;
  if (bytes < count)
  {
    count = bytes;
  }

  size_t i;

  for (i = 0; i < sizeof_src; src++, i++)
  {
    if (false == buffer_try_set(object, *src))
    {
      break;
    }
  }

  return i;
}

size_t buffer_read_bytes (buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) ||
      (0    == bytes      ) )
  {
    return 0;
  }

  size_t count = sizeof_dest;
  if (bytes < count)
  {
    count = bytes;
  }

  size_t i;

  for (i = 0; i < count; dest++, i++)
  {
    if (false == buffer_wait_get(object, dest))
    {
      break;
    }
  }

  return i;
}

size_t buffer_try_read_bytes (buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) ||
      (0    == bytes      ) )
  {
    return 0;
  }

  size_t count = sizeof_dest;
  if (bytes < count)
  {
    count = bytes;
  }

  size_t i;

  for (i = 0; i < count; dest++, i++)
  {
    if (false == buffer_try_get(object, dest))
    {
      break;
    }
  }

  return i;
}

size_t buffer_try_peek_bytes (buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) ||
      (0    == bytes      ) )
  {
    return 0;
  }

  size_t count = sizeof_dest;
  if (bytes < count)
  {
    count = bytes;
  }

  size_t i;
  char * p = atomic_load(&object->c_head);

  for (i = 0; i < count && p != atomic_load(&object->p_tail); dest++, i++)
  {
    // Potential issue with the pointer `p`: another consumer thread could
    // release the memory, allowing it to be overwritten by a producer thread.
    *dest = *p;

    p += 1;

    if (p > object->last)
    {
      p = object->data;
    }
  }

  return i;
}



bool buffer_is_empty (const buffer_t * object)
{
  if (NULL == object) { return false; }

  return atomic_load(&object->c_head) == atomic_load(&object->p_tail);
}

bool buffer_is_not_empty (const buffer_t * object)
{
  if (NULL == object) { return false; }

  return atomic_load(&object->c_head) != atomic_load(&object->p_tail);
}

bool buffer_is_full (const buffer_t * object)
{
  if (NULL == object) { return false; }

  char * next = atomic_load(&object->p_head) + 1;

  if (next > object->last)
  {
    next = object->data;
  }

  return next == atomic_load(&object->c_tail);
}

bool buffer_is_not_full (const buffer_t * object)
{
  if (NULL == object) { return false; }

  char * next = atomic_load(&object->p_head) + 1;

  if (next > object->last)
  {
    next = object->data;
  }

  return next != atomic_load(&object->c_tail);
}

size_t buffer_length (const buffer_t * object)
{
  if (NULL == object) { return 0; }

  ptrdiff_t length = atomic_load(&object->p_tail) - atomic_load(&object->c_head);

  if (0 > length)
  {
    length += object->last - object->data + 1;
  }

  return length;
}

size_t buffer_space (const buffer_t * object)
{
  if (NULL == object) { return 0; }

  ptrdiff_t space = atomic_load(&object->c_tail) - atomic_load(&object->p_head) - 1;

  if (0 > space)
  {
    space += object->last - object->data + 1;
  }

  return space;
}



size_t buffer_lines (const buffer_t * object)
{
  if (NULL == object) { return 0; }

  ptrdiff_t lines = atomic_load(&object->lines);

  if (0 >= lines)
  {
    return 0;
  }

  return lines;
}



size_t buffer_write_c_str (buffer_t * object, const char *src, size_t sizeof_src, size_t characters)
{
  if ((NULL == object    ) ||
      (NULL == src       ) ||
      (0    == sizeof_src) ||
      (0    == characters) )
  {
    return 0;
  }

  size_t count = sizeof_src;
  if (characters < count)
  {
    count = characters;
  }

  size_t i;
  char c;

  for (i = 0; ('\0' != (c = *src)) && i < count; src++, i++)
  {
    if (false == buffer_set(object, *src))
    {
      break;
    }
  }

  return i;
}

size_t buffer_try_write_c_str (buffer_t * object, const char *src, size_t sizeof_src, size_t characters)
{
  if ((NULL == object    ) ||
      (NULL == src       ) ||
      (0    == sizeof_src) ||
      (0    == characters) )
  {
    return 0;
  }

  size_t count = sizeof_src;
  if (characters < count)
  {
    count = characters;
  }
  char c;
  size_t i;

  for (i = 0; ('\0' != (c = *src)) && i < count; src++, i++)
  {
    if(false == buffer_try_set(object, c))
    {
      break;
    }
  }

  return i;
}

size_t buffer_read_c_str (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) )
  {
    return 0;
  }

  if (0 == characters)
  {
    *dest = '\0';
    return 0;
  }

  size_t count = sizeof_dest - 1;
  if (characters < count)
  {
    count = characters;
  }

  size_t i;
  for (i = 0; i < count; dest++, i++)
  {
    if (false == buffer_wait_get(object, dest))
    {
      break;
    }
  }
  *dest = '\0';

  return i;
}

size_t buffer_try_read_c_str (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) )
  {
    return 0;
  }

  if (0 == characters)
  {
    *dest = '\0';
    return 0;
  }

  size_t count = sizeof_dest - 1;
  if (characters < count)
  {
    count = characters;
  }

  size_t i;

  for (i = 0; i < count; dest++, i++)
  {
    if(false == buffer_try_get(object, dest))
    {
      break;
    }
  }
  *dest = '\0';

  return i;
}

size_t buffer_try_peek_c_str (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) )
  {
    return 0;
  }

  if (0 == characters)
  {
    *dest = '\0';
    return 0;
  }

  size_t count = sizeof_dest - 1;
  if (characters < count)
  {
    count = characters;
  }

  size_t i;
  char * p = atomic_load(&object->c_head);

  for (i = 0; i < count && p != atomic_load(&object->p_tail); dest++, i++)
  {
    *dest = *p;

    p += 1;

    if (p > object->last)
    {
      p = object->data;
    }
  }
  *dest = '\0';

  return i;
}

size_t buffer_read_line (buffer_t * object, char * dest, size_t sizeof_dest)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) )
  {
    return 0;
  }

  sizeof_dest -= 1;

  size_t i;
  char c;
  char eol = object->end_of_line_character;

  for (i = 0; i < sizeof_dest; dest++, i++)
  {
    if (false == buffer_wait_get(object, &c))
    {
      break;
    }

    if (eol == c)
    {
      break;
    }

    *dest = c;
  }
  *dest = '\0';

  return i;
}

ptrdiff_t buffer_try_read_line (buffer_t * object, char * dest, size_t sizeof_dest)
{
  assert(PTRDIFF_MAX > sizeof_dest && "This functions return negative index values, so the entire range of `size_t` values cannot be used." );

  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) )
  {
    return 0;
  }

  ptrdiff_t lines = atomic_load(&object->lines);
  if (0 >= lines)
  {
    return 0;
  }

  sizeof_dest -= 1;

  size_t i;
  char c;

  bool error = true;
  char eol = object->end_of_line_character;

  for (i = 0; i < sizeof_dest; dest++, i++)
  {
    if (false == buffer_try_get(object, &c))
    {
      break;
    }

    if (eol == c)
    {
      error = false;
      break;
    }

    *dest = c;
  }
  *dest = '\0';

  if (error)
  {
    return -i;
  }

  return i;
}

size_t buffer_read_to (buffer_t * object, char * dest, size_t sizeof_dest, const char * to, size_t to_length)
{
  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) ||
      (NULL == to         ) )
  {
    return 0;
  }

  if (to_length >= sizeof_dest)
  {
    *dest = '\0';
    return 0;
  }

  char * compare = dest;

  size_t i;
  for (i = 0; i < to_length; dest++, i++)
  {
    if (false == buffer_wait_get(object, dest))
    {
      *dest = '\0';
      return 0;
    }
  }

  size_t count = sizeof_dest - 1;

  for (; i < count; compare++, dest++, i++)
  {
    if (0 == memcmp(compare, to, to_length))
    {
      break;
    }

    if (false == buffer_wait_get(object, dest))
    {
        break;
    }
  }

  *dest = '\0';

  return i;
}

ptrdiff_t buffer_try_read_to (buffer_t * object, char * dest, size_t sizeof_dest, const char * to, size_t to_length, buffer_try_read_to_t * data)
{
  assert(PTRDIFF_MAX > sizeof_dest && "This functions return negative index values, so the entire range of `size_t` values cannot be used." );

  if ((NULL == object     ) ||
      (NULL == dest       ) ||
      (0    == sizeof_dest) ||
      (NULL == to         ) )
  {
    return 0;
  }

  if (to_length >= sizeof_dest)
  {
    *dest = '\0';
    return 0;
  }

  if (0 == data->index)
  {
    data->dest = dest;
  }

  for (; data->index < to_length; data->dest++, data->index++)
  {
    if (false == buffer_try_get(object, dest+data->index))
    {
      *data->dest = '\0';
      return -data->index;
    }
  }

  if (data->index == to_length)
  {
      data->compare = dest;
  }

  size_t count = sizeof_dest - 1;

  for (; data->index < count; data->compare++, data->dest++, data->index++)
  {
    if (0 == memcmp(data->compare, to, to_length))
    {
      break;
    }

    if (false == buffer_try_get(object,dest+data->index))
    {
      *data->dest = '\0';
      return -data->index;
    }
  }

  *data->dest = '\0';

  data->compare = data->dest - to_length;
  data->dest = dest;
  return data->index;
}



void buffer_clear (buffer_t * object)
{
  if(NULL == object){ return; }

  char dump;

  while(false != buffer_try_get(object, &dump))
  {
    ;
  }
}



void buffer_try_read_to_init (buffer_try_read_to_t * object)
{
  if (NULL == object) { return; }

  object->index = 0;
  object->dest = NULL;
  object->compare = NULL;
}


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
