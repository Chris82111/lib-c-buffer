//! @file
//! @brief The buffer header file.
//!
//! @details The module can be used in C and C++.


#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_


// ------------------------------------------------------------------------- //
//  public: atomic handling
// ------------------------------------------------------------------------- //

//! @defgroup stdatomic_support_in_c_and_cpp To use stdatomic, you must use at
//!           least the following versions
//!
//! @details To use the library in C, all you need is c11. In C++, you can also
//!          use the library with c++11. In this case, however, interface
//!          functions are required due to the different types, and this header
//!          cannot be used. This header is intended for use in both C and C++.
//!          Therefore, the header must work in both languages. stdatomic can
//!          be used, but only starting with c++23.
//!
//! @{

//! @def HAS_CPP23
//! @brief Checks whether C++23 or a newer version is being used
//! @return Returns the result of the version check
//! @retval 1 if C++23 or a newer version is being used
//! @retval 0 if C++23 or a later version was not used

//! @def HAS_C11
//! @brief Checks whether C11 or a newer version is being used
//! @return Returns the result of the version check
//! @retval 1 if C11 or a newer version is being used
//! @retval 0 if C11 or a later version was not used

#ifdef __cplusplus
  #define HAS_CPP23 ( 202302L <= __cplusplus )
  #define HAS_C11   0
#else
  #define HAS_CPP23 0
  #ifdef __STDC_VERSION__
    #define HAS_C11 ( 201112L <= __STDC_VERSION__ )
  #else
    #define HAS_C11 0
  #endif
#endif

#if !(HAS_CPP23 || HAS_C11)
  #error "Requires C11 or C++23"
#endif

//! @}


//! @defgroup stdatomic_support_in_cpp The stdatomic header is
//!           supported in C and C++
//!
//! @details With C++23, the header is compatible between C and C++.
//!
//! @{

#include <stdatomic.h>

//! @}


//! @defgroup cdt_parser_workaround Workaround for CDT indexer
//!
//! @details The CDT indexer has problems with the atomic functions.
//!          For this reason `_Atomic(T)` is redefined. Without changing the
//!          reserved keyword, the CDT indexer or auto-completion will not find
//!          the variable. A disadvantage is that no information is displayed
//!          about the fact that it is an atomic type.
//!
//! @{

#ifdef __CDT_PARSER__

  #ifndef __cplusplus

    #define _Atomic(...) __VA_ARGS__

    #define atomic_init(PTR, VAL) (*(PTR) = (VAL)) //;

    #define atomic_store(PTR, VAL) (*(PTR) = (VAL)) //;

    #define atomic_load(PTR) (*(PTR)) //;

    #define atomic_compare_exchange_strong(PTR, VAL, DES) \
      ((*(VAL) == *(PTR)) ? (*(PTR) = (DES), 1) : 0) //;

    #define _Static_assert(CONDITION, TEXT) //;

  #endif

#endif

//! @}


//! @defgroup atomic_var_init_check Support for use in C and C++ after deprecation
//!
//! @details In C and in C++ `ATOMIC_VAR_INIT(value)` was deprecated.
//!          It must be defined so that it is supported in newer versions.
//!
//! @{

//! @brief Fallback definition for atomic variable initialization.
//!
//! @details Defines ATOMIC_VAR_INIT as a direct value assignment if not
//!          already provided by the standard library or platform.
//!          This is a compatibility fallback and does not provide atomic
//!          semantics by itself.
//!
//! @return The value X unchanged.
#ifndef ATOMIC_VAR_INIT
#  define ATOMIC_VAR_INIT(VALUE) (VALUE)
#endif

//! @}


#ifdef __cplusplus
extern "C" {
#endif


// ------------------------------------------------------------------------- //
//  public: include files
// ------------------------------------------------------------------------- //

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


// ------------------------------------------------------------------------- //
//  public: define
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: typedefs
// ------------------------------------------------------------------------- //

//! @brief Forward declaration
struct buffer_s;

//! @brief Forward declaration
struct buffer_try_read_to_s;


//! @brief Forward typedef, for information see ::buffer_s
typedef struct buffer_s buffer_t;

//! @brief Forward typedef, for information see ::buffer_try_read_to_s
typedef struct buffer_try_read_to_s buffer_try_read_to_t;


//! @brief A function pointer used by ::buffer_s that passes its own structure pointer
//!
//! @details Used in the functions handler:
//! - buffer_s::buffer_set()
//! - buffer_s::buffer_get()
//!
//! @param[in,out] object The buffer object
//! @return Freely usable return value
typedef char (*buffer_function_handler_t)(buffer_t * object);


//! @brief Struct to create a buffer object, like an instance of a class
//!
//! @details The buffer struct can be used to exchange data between threads or a thread and an interrupt.
//!
//!          Due to the implementation, one buffer element will remain empty.
//!          Therefore, one more element must be reserved than will later be available.
struct buffer_s
{
  //! @brief Consumer head pointer
  //! @details Pointer to the next address that can be requested by a thread.
  _Atomic(char *) c_head;

  //! @brief Consumer tail pointer
  //! @details A pointer to an address currently being processed by a thread.
  _Atomic(char *) c_tail;

  //! @brief Consumer increment counter
  //! @details Counts the finished threads.
  _Atomic(uint16_t) c_inc;

  //! @brief Producer head pointer
  //! @details Pointer to the next address that can be requested by a thread.
  _Atomic(char *) p_head;

  //! @brief Producer tail pointer
  //! @details A pointer to an address currently being processed by a thread.
  _Atomic(char *) p_tail;

  //! @brief Producer increment counter
  //! @details Counts the finished threads.
  _Atomic(uint16_t) p_inc;

  //! @brief Buffer start address
  //! @details Start address of the `char` array which stores the data.
  char * data;

  //! @brief Buffer last address
  //! @details Last address of the `char` array.
  char * last;

  //! @brief Number of newline characters
  //! @details Current number of newline character stored in the buffer.
  //! - If characters are read without paying attention to `Lines`, they can
  //!   read a line, and when they reach the end-of-line character, the
  //!   variable will be decrement to a negative value.
  _Atomic(ptrdiff_t) lines;

  //! @brief Consumer internal counter variable for internal use
  //! @details Count the new lines until the threads are complete.
  _Atomic(size_t) c_lines;

  //! @brief Producer internal counter variable for internal use
  //! @details Count the new lines until the threads are complete.
  _Atomic(size_t) p_lines;

  //! @brief End-Of-Line character
  //! @details End of line character, standard is '\\n'
  char end_of_line_character;

  //! @brief End-Of-Line character
  //! @details A handler that is called in every cycle:
  //! - It is called by ::buffer_set() when no memory is available.
  //! - It is called by ::buffer_get() when no characters are available.
  //! - Any return value other than 0 causes the wait loop to be exited
  //! - `NULL` is allowed.
  buffer_function_handler_t on_sleep;

  //! @brief Optional pointer to user data
  //! @details Additional information not used by this library
  //! - `NULL` is allowed
  void * user_data;

};

//! @brief Struct to safe data for the ::buffer_try_read_to() function
typedef struct buffer_try_read_to_s
{
  //! @brief The number of read characters
  size_t index;

  //! @brief A pointer to the read data
  char * dest;

  //! @brief A pointer to the read to data
  char * compare;
} buffer_try_read_to_t;


//! @brief Represents a simplified form of a class
//!
//! @details The global variable ::buffer can be used to easily access all matching
//! functions with auto-completion.
struct buffer_sc
{
  //! @brief Function for initialization
  //!
  //! @details Function for initialization the buffer object.
  //!
  //! @param[in,out] object The buffer object
  //! @param data The `char` array which stores the data
  //! @param sizeof_data Length of the `char` array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  void (* Init) (buffer_t * object, char * data, size_t sizeof_data);


  //! @brief Saves a character or waits until it can be executed
  //!
  //! @details Stores a character in the buffer, blocks as long as the character can not be stored.
  //!
  //! This function allows you to detect a cancellation.
  //!
  //! The producer can be called multiple times by different threads, and the
  //! calls can execute in parallel without locks and without data loss.
  //! However, the consumer does not observe the changes until all producer
  //! operations have completed their work. This can lead to delayed visibility
  //! if a thread is interrupted (for example by an interrupt), since progress
  //! may be postponed until the interrupted execution resumes and finishes its
  //! current operation. Only after the interrupted producer invocation completes
  //! can the consumer observe the new inputs.
  //!
  //! @param[in,out] object The buffer object
  //! @param c The character that will be stored
  //! @return Returns whether the character is written
  //! @retval false Character was not written (The function was cancelled)
  //! @retval true  Character was written
  bool (* Set) (buffer_t * object, char c);

  //! @brief Saves a character or skips it if this is not possible
  //!
  //! @details Tries to save a character or skips it if this is not possible
  //!
  //! @param[in,out] object The buffer object
  //! @param c The character that will be stored
  //! @return Returns whether the character could be saved
  //! @retval false Character could not be saved
  //! @retval true  Character could be saved
  bool (* TrySet) (buffer_t * object, char c);

  //! @brief Reads a character or waits until it can be executed.
  //!
  //! @details Reads a character in the buffer, blocks as long as the character can not be read
  //!
  //! The consumer can be called multiple times by different threads, and the
  //! calls can execute in parallel without locks and without data loss.
  //! However, the producer does not observe the changes until all consumer
  //! operations have completed their work. This can lead to delayed visibility
  //! if a thread is interrupted (for example by an interrupt), since progress
  //! may be postponed until the interrupted execution resumes and finishes its
  //! current operation. Only after the interrupted consumer invocation completes
  //! can the producer observe the free space.
  //!
  //! @param[in,out] object The buffer object
  //! @return The read character
  char (* Get) (buffer_t * object);

  //! @brief Reads a character or waits until it can be executed.
  //!
  //! @details Reads a character in the buffer, blocks as long as the character can not be read
  //!
  //! This function allows you to detect a cancellation.
  //!
  //! The consumer can be called multiple times by different threads, and the
  //! calls can execute in parallel without locks and without data loss.
  //! However, the producer does not observe the changes until all consumer
  //! operations have completed their work. This can lead to delayed visibility
  //! if a thread is interrupted (for example by an interrupt), since progress
  //! may be postponed until the interrupted execution resumes and finishes its
  //! current operation. Only after the interrupted consumer invocation completes
  //! can the producer observe the free space.
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] c The character that was read
  //! @return Returns whether the character is read
  //! @retval false Character was not read (The function was cancelled)
  //! @retval true  Character was read
  bool (* WaitGet) (buffer_t * object, char * c);

  //! @brief Reads a character or skips it if this is not possible
  //!
  //! @details Tries to read a character or skips it if this is not possible
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] c The character that was read
  //! @return Returns whether the character could be read
  //! @retval false Character could not be read
  //! @retval true  Character could be read
  bool (* TryGet) (buffer_t * object, char * c);


  //! @brief Writes an array to the buffer
  //!
  //! @details Writes an array to the buffer.
  //! Blocks as long as the bytes can not be stored.
  //! The return value must be checked to ensure that everything has been written.
  //! @param[in,out] object The buffer object
  //! @param[in] src Contains the data that will be written.
  //! @param sizeof_src Length of the array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be stored in the buffer.
  //! @return Returns the number of bytes written
  size_t (* WriteBytes) (buffer_t * object, const char *src, size_t sizeof_src, size_t bytes);

  //! @brief Writes an array to the buffer or skips it if this is not possible
  //!
  //! @details Writes an array to the buffer or skips it if this is not possible
  //! Returns if the bytes can not be stored.
  //! The return value must be checked to ensure that everything has been written.
  //! @param[in,out] object The buffer object
  //! @param[in] src Contains the data that will be written.
  //! @param sizeof_src Length of the array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be stored in the buffer.
  //! @return Returns the number of bytes written
  size_t (* TryWriteBytes) (buffer_t * object, const char *src, size_t sizeof_src, size_t bytes);

  //! @brief Reads an array from the buffer
  //!
  //! @details Reads an array from the buffer.
  //! Blocks as long as the bytes can not be read.
  //! The return value must be checked to ensure that everything has been read.
  //! @param[in,out] object The buffer object
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t (* ReadBytes) (buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes);

  //! @brief Reads an array from the buffer or skips it if this is not possible
  //!
  //! @details Reads an array from the buffer or skips it if this is not possible
  //! Returns if the bytes can not be read.
  //! The return value must be checked to ensure that everything has been read.
  //! @param[in,out] object The buffer object
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t (* TryReadBytes) (buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes);

  //! @brief Look at the buffer as if the data were being read or skips it if this is not possible
  //!
  //! @details Look at the buffer as if the data were being read or skips it if this is not possible
  //! Returns if the bytes can not be read.
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! This works reliably as long as no other thread is reading characters from
  //! the buffer, another consumer thread. If this thread runs in parallel with
  //! a second thread that reads characters (consumer) and a third thread that
  //! writes characters (producer), the behavior of this first thread becomes
  //! unpredictable. Old characters may be read along with new ones.
  //! Another issue with the scenario described is that this thread reads an 8-bit
  //! character even though another thread is writing to the same address.
  //! Normally only a single thread has access to the memory address and this is
  //! achieved through the atomic consumer (c) and producer (p), as well as
  //! through the head and tail pointers. In this case, that cannot be guaranteed.
  //!
  //! @param[in,out] object The buffer object
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t (* TryPeekBytes) (buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes);


  //! @brief Checks if the buffer is empty
  //!
  //! @details Function to check if the buffer is empty
  //!
  //! @param[in] object The buffer object
  //! @return Returns if the buffer is empty
  //! @retval true The buffer is empty
  //! @retval false The buffer is NOT empty
  bool (* IsEmpty) (const buffer_t * object);

  //! @brief Checks if the buffer is not empty
  //!
  //! @details Function to check if the buffer is not empty
  //!
  //! @param[in] object The buffer object
  //! @return Returns if the buffer is empty
  //! @retval true The buffer is NOT empty
  //! @retval false The buffer is empty
  bool (* IsNotEmpty) (const buffer_t * object);

  //! @brief Checks if the buffer is full
  //!
  //! @details Function to check if the buffer is full
  //!
  //! @param[in] object The buffer object
  //! @return Returns if the buffer is full
  //! @retval true The buffer is full
  //! @retval false The buffer is NOT full
  bool (* IsFull) (const buffer_t * object);

  //! @brief Checks if the buffer is not full
  //!
  //! @details Function to check if the buffer is not full
  //!
  //! @param[in] object The buffer object
  //! @return Returns if the buffer is full
  //! @retval true The buffer is NOT full
  //! @retval false The buffer is full
  bool (* IsNotFull) (const buffer_t * object);

  //! @brief Returns the characters currently used
  //!
  //! @details Returns the currently used characters in the array
  //!
  //! @param[in] object The buffer object
  //! @return Positive number of used characters
  size_t (* Length) (const buffer_t * object);

  //! @brief Returns the free space
  //!
  //! @details Returns the available space in the array.
  //!
  //! @param[in] object The buffer object
  //! @return Positive number of available space
  size_t (* Space) (const buffer_t * object);

  //! @brief Returns the lines currently used
  //!
  //! @details Returns the currently used lines in the array.
  //! The recognized character is defined in `buffer_s::end_of_line_character`.
  //!
  //! @param[in] object The buffer object
  //! @return Positive number of used characters
  size_t (* Lines) (const buffer_t * object);


  //! @brief Writes a string to the buffer
  //!
  //! @details Writes a string to the buffer.
  //! The string must be terminated with the string terminator '\\0'.
  //!
  //! The return value must be checked to ensure that everything has been written.
  //!
  //! @param[in,out] object The buffer object.
  //! @param[in] src Contains the string or the characters.
  //! @param sizeof_src The length of the buffer.
  //! @param characters The number of characters to be read from the buffer.
  //! @return Returns the number of characters written
  size_t (* WriteCStr) (buffer_t * object, const char *src, size_t sizeof_src, size_t characters);

  //! @brief Writes a string to the buffer or skips it if this is not possible
  //!
  //! @details Writes a string to the buffer or skips it if this is not possible
  //! The string must be terminated with the string terminator '\\0'.
  //!
  //! The return value must be checked to ensure that everything has been written.
  //!
  //! @param[in,out] object The buffer object.
  //! @param[in] src Contains the string or the characters.
  //! @param sizeof_src The length of the buffer.
  //! @param characters The number of characters to be read from the buffer.
  //! @return Returns the number of characters written
  size_t (* TryWriteCStr) (buffer_t * object, const char *src, size_t sizeof_src, size_t characters);

  //! @brief Reads a string from the buffer
  //!
  //! @details Reads a string from the buffer.
  //! A string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer
  //! @param characters The number of characters that should be read.
  //! @return Returns the number of characters read
  size_t (* ReadCStr) (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters);

  //! @brief Reads a string from the buffer or skips it if this is not possible
  //!
  //! @details Reads a string from the buffer or skips it if this is not possible.
  //! A string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer
  //! @param characters The number of characters that should be read.
  //! @return Returns the number of characters read
  size_t (* TryReadCStr) (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters);

  //! @brief Look at the buffer as if the data were being read or skips it if this is not possible
  //!
  //! @details Look at the buffer as if the data were being read or skips it if this is not possible
  //! Returns if the bytes can not be read.
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! This works reliably as long as no other thread is reading characters from
  //! the buffer, another consumer thread. If this thread runs in parallel with
  //! a second thread that reads characters (consumer) and a third thread that
  //! writes characters (producer), the behavior of this first thread becomes
  //! unpredictable. Old characters may be read along with new ones.
  //! Another issue with the scenario described is that this thread reads an 8-bit
  //! character even though another thread is writing to the same address.
  //! Normally only a single thread has access to the memory address and this is
  //! achieved through the atomic consumer (c) and producer (p), as well as
  //! through the head and tail pointers. In this case, that cannot be guaranteed.
  //!
  //! @param[in,out] object The buffer object
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param characters The number of characters to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t (* TryPeekCStr) (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters);


  //! @brief Reads a line from the buffer
  //!
  //! @details Reads a string from the buffer.
  //! A string terminating character '\\0' is always written at the end.
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] dest The string/line is written in this buffer.
  //! @param sizeof_dest The length of the buffer.
  //! @return Returns the number of characters read
  size_t (* ReadLine) (buffer_t * object, char * dest, size_t sizeof_dest);

  //! @brief Attempts to read a complete line from the buffer.
  //!
  //! Reads characters from the buffer into @p dest until a line feed character
  //! ('\n') is encountered. The line feed character is consumed from the buffer
  //! but is not copied to @p dest. The resulting string is always
  //! null-terminated.
  //!
  //! If no complete line is currently available, no characters are consumed and
  //! the function returns 0.
  //!
  //! In concurrent environments, another thread may consume data from the buffer
  //! after the presence of a complete line has been detected but before the line
  //! can be fully read. In this case, the function returns a negative value. The
  //! absolute value of the return value indicates the number of characters that
  //! were read before the line became invalid.
  //!
  //! @param[in,out] object The buffer object
  //! @param dest Destination buffer receiving the line.
  //! @param sizeof_dest Size of @p dest in bytes.
  //! @return
  //! @retval > 0: Number of characters copied to @p dest.
  //! @retval   0: No complete line is available.
  //! @retval < 0: Negative value: The line became invalid during the read
  //!              operation; the absolute value is the number of characters read
  //!              before the operation was aborted.
  ptrdiff_t (* TryReadLine) (buffer_t * object, char * dest, size_t sizeof_dest);

  //! @brief Reads the characters up to the given string
  //! @details Reads the characters up to the given string
  //!
  //! The return value must be checked to ensure that everything has been read.
  //! The string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer.
  //! @param to The string up to which is read
  //! @param to_length The length of the buffer.
  //! @return Returns the number of characters read
  size_t (* ReadTo) (buffer_t * object, char * dest, size_t sizeof_dest, const char * to, size_t to_length);

  //! @brief On each pass, it attempts to read the data and adds the characters to the string as long as the string "to" has not yet been read.
  //! @details On each pass, it attempts to read the data and adds the characters to the string as long as the string "to" has not yet been read.
  //!
  //! The return value must be checked to ensure that everything has been read.
  //! The string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! @param[in,out] object The buffer object
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer.
  //! @param to The string up to which is read
  //! @param to_length The length of the buffer.
  //! @param data Temporary data to prevent loss of the current state.
  //! @return Returns the number of characters read as a negative value if the 'to' string is not contained in the read string
  size_t (* TryReadTo) (buffer_t * object, char * dest, size_t sizeof_dest, const char * to, size_t to_length, buffer_try_read_to_t * data);


  //! @brief Clears the buffer
  //!
  //! @param[in] object The buffer object
  void (* Clear) (buffer_t * object);

};


// ------------------------------------------------------------------------- //
//  public: extern variables
// ------------------------------------------------------------------------- //

//! @brief To access all member functions working with type ::buffer_s
//!
//! @details Allows a simplified class to easily access all member functions
//! working with type ::buffer_s. The auto-completion function helps you to select all
//! suitable functions via ::buffer_sc struct.
extern const struct buffer_sc buffer;


// ------------------------------------------------------------------------- //
//  public: function prototypes
// ------------------------------------------------------------------------- //

//! @brief Function for initialization
//!
//! @details Function for initialization the buffer object.
//!
//! @param[in,out] object The buffer object
//! @param data The `char` array which stores the data
//! @param sizeof_data Length of the `char` array in which the data is stored.
//! `sizeof()` can be used if it is an array whose size is known at compile time.
void buffer_init (buffer_t * object, char * data, size_t sizeof_data);

//! @brief Saves a character or waits until it can be executed
//!
//! @details Stores a character in the buffer, blocks as long as the character can not be stored.
//!
//! This function allows you to detect a cancellation.
//!
//! The producer can be called multiple times by different threads, and the
//! calls can execute in parallel without locks and without data loss.
//! However, the consumer does not observe the changes until all producer
//! operations have completed their work. This can lead to delayed visibility
//! if a thread is interrupted (for example by an interrupt), since progress
//! may be postponed until the interrupted execution resumes and finishes its
//! current operation. Only after the interrupted producer invocation completes
//! can the consumer observe the new inputs.
//!
//! @param[in,out] object The buffer object
//! @param c The character that will be stored
//! @return Returns whether the character is written
//! @retval false Character was not written (The function was cancelled)
//! @retval true  Character was written
bool buffer_set (buffer_t * object, char c);

//! @brief Saves a character or skips it if this is not possible
//!
//! @details Tries to save a character or skips it if this is not possible
//!
//! @param[in,out] object The buffer object
//! @param c The character that will be stored
//! @return Returns whether the character could be saved
//! @retval false Character could not be saved
//! @retval true  Character could be saved
bool buffer_try_set (buffer_t * object, char c);

//! @brief Reads a character or waits until it can be executed.
//!
//! @details Reads a character in the buffer, blocks as long as the character can not be read
//!
//! The consumer can be called multiple times by different threads, and the
//! calls can execute in parallel without locks and without data loss.
//! However, the producer does not observe the changes until all consumer
//! operations have completed their work. This can lead to delayed visibility
//! if a thread is interrupted (for example by an interrupt), since progress
//! may be postponed until the interrupted execution resumes and finishes its
//! current operation. Only after the interrupted consumer invocation completes
//! can the producer observe the free space.
//!
//! @param[in,out] object The buffer object
//! @return The read character
char buffer_get (buffer_t * object);

//! @brief Reads a character or waits until it can be executed.
//!
//! @details Reads a character in the buffer, blocks as long as the character can not be read
//!
//! This function allows you to detect a cancellation.
//!
//! The consumer can be called multiple times by different threads, and the
//! calls can execute in parallel without locks and without data loss.
//! However, the producer does not observe the changes until all consumer
//! operations have completed their work. This can lead to delayed visibility
//! if a thread is interrupted (for example by an interrupt), since progress
//! may be postponed until the interrupted execution resumes and finishes its
//! current operation. Only after the interrupted consumer invocation completes
//! can the producer observe the free space.
//!
//! @param[in,out] object The buffer object
//! @param[out] c The character that was read
//! @return Returns whether the character is read
//! @retval false Character was not read (The function was cancelled)
//! @retval true  Character was read
bool buffer_wait_get (buffer_t * object, char * c);

//! @brief Reads a character or skips it if this is not possible
//!
//! @details Tries to read a character or skips it if this is not possible
//!
//! @param[in,out] object The buffer object
//! @param[out] c The character that was read
//! @return Returns whether the character could be read
//! @retval false Character could not be read
//! @retval true  Character could be read
bool buffer_try_get (buffer_t * object, char * c);



//! @brief Writes an array to the buffer
//!
//! @details Writes an array to the buffer.
//! Blocks as long as the bytes can not be stored.
//! The return value must be checked to ensure that everything has been written.
//! @param[in,out] object The buffer object
//! @param[in] src Contains the data that will be written.
//! @param sizeof_src Length of the array in which the data is stored.
//! `sizeof()` can be used if it is an array whose size is known at compile time.
//! @param bytes The number of bytes to be stored in the buffer.
//! @return Returns the number of bytes written
size_t buffer_write_bytes(buffer_t * object, const char *src, size_t sizeof_src, size_t bytes);

//! @brief Writes an array to the buffer or skips it if this is not possible
//!
//! @details Writes an array to the buffer or skips it if this is not possible
//! Returns if the bytes can not be stored.
//! The return value must be checked to ensure that everything has been written.
//! @param[in,out] object The buffer object
//! @param[in] src Contains the data that will be written.
//! @param sizeof_src Length of the array in which the data is stored.
//! `sizeof()` can be used if it is an array whose size is known at compile time.
//! @param bytes The number of bytes to be stored in the buffer.
//! @return Returns the number of bytes written
size_t buffer_try_write_bytes(buffer_t * object, const char *src, size_t sizeof_src, size_t bytes);

//! @brief Reads an array from the buffer
//!
//! @details Reads an array from the buffer.
//! Blocks as long as the bytes can not be read.
//! The return value must be checked to ensure that everything has been read.
//! @param[in,out] object The buffer object
//! @param[in] dest The bytes are written into this array.
//! @param sizeof_dest Length of the destination array into which the bytes are written
//! `sizeof()` can be used if it is an array whose size is known at compile time.
//! @param bytes The number of bytes to be taken from the buffer.
//! @return Returns the number of bytes read
size_t buffer_read_bytes(buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes);

//! @brief Reads an array from the buffer or skips it if this is not possible
//!
//! @details Reads an array from the buffer or skips it if this is not possible
//! Returns if the bytes can not be read.
//! The return value must be checked to ensure that everything has been read.
//! @param[in,out] object The buffer object
//! @param[in] dest The bytes are written into this array.
//! @param sizeof_dest Length of the destination array into which the bytes are written
//! `sizeof()` can be used if it is an array whose size is known at compile time.
//! @param bytes The number of bytes to be taken from the buffer.
//! @return Returns the number of bytes read
size_t buffer_try_read_bytes(buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes);

//! @brief Look at the buffer as if the data were being read or skips it if this is not possible
//!
//! @details Look at the buffer as if the data were being read or skips it if this is not possible
//! Returns if the bytes can not be read.
//! The return value must be checked to ensure that everything has been read.
//!
//! This works reliably as long as no other thread is reading characters from
//! the buffer, another consumer thread. If this thread runs in parallel with
//! a second thread that reads characters (consumer) and a third thread that
//! writes characters (producer), the behavior of this first thread becomes
//! unpredictable. Old characters may be read along with new ones.
//! Another issue with the scenario described is that this thread reads an 8-bit
//! character even though another thread is writing to the same address.
//! Normally only a single thread has access to the memory address and this is
//! achieved through the atomic consumer (c) and producer (p), as well as
//! through the head and tail pointers. In this case, that cannot be guaranteed.
//!
//! @param[in,out] object The buffer object
//! @param[in] dest The bytes are written into this array.
//! @param sizeof_dest Length of the destination array into which the bytes are written
//! `sizeof()` can be used if it is an array whose size is known at compile time.
//! @param bytes The number of bytes to be taken from the buffer.
//! @return Returns the number of bytes read
size_t buffer_try_peek_bytes(buffer_t * object, char * dest, size_t sizeof_dest, size_t bytes);



//! @brief Checks if the buffer is empty
//!
//! @details Function to check if the buffer is empty
//!
//! @param[in] object The buffer object
//! @return Returns if the buffer is empty
//! @retval true The buffer is empty
//! @retval false The buffer is NOT empty
bool buffer_is_empty (const buffer_t * object);

//! @brief Checks if the buffer is not empty
//!
//! @details Function to check if the buffer is not empty
//!
//! @param[in] object The buffer object
//! @return Returns if the buffer is empty
//! @retval true The buffer is NOT empty
//! @retval false The buffer is empty
bool buffer_is_not_empty (const buffer_t * object);

//! @brief Checks if the buffer is full
//!
//! @details Function to check if the buffer is full
//!
//! @param[in] object The buffer object
//! @return Returns if the buffer is full
//! @retval true The buffer is full
//! @retval false The buffer is NOT full
bool buffer_is_full (const buffer_t * object);

//! @brief Checks if the buffer is not full
//!
//! @details Function to check if the buffer is not full
//!
//! @param[in] object The buffer object
//! @return Returns if the buffer is full
//! @retval true The buffer is NOT full
//! @retval false The buffer is full
bool buffer_is_not_full (const buffer_t * object);

//! @brief Returns the characters currently used
//!
//! @details Returns the currently used characters in the array
//!
//! @param[in] object The buffer object
//! @return Positive number of used characters
size_t buffer_length (const buffer_t * object);

//! @brief Returns the free space
//!
//! @details Returns the available space in the array.
//!
//! @param[in] object The buffer object
//! @return Positive number of available space
size_t buffer_space (const buffer_t * object);



//! @brief Returns the lines currently used
//!
//! @details Returns the currently used lines in the array.
//! The recognized character is defined in `buffer_s::end_of_line_character`.
//!
//! @param[in] object The buffer object
//! @return Positive number of used characters
size_t buffer_lines (const buffer_t * object);



//! @brief Writes a string to the buffer
//!
//! @details Writes a string to the buffer.
//! The string must be terminated with the string terminator '\\0'.
//!
//! The return value must be checked to ensure that everything has been written.
//!
//! @param[in,out] object The buffer object.
//! @param[in] src Contains the string or the characters.
//! @param sizeof_src The length of the buffer.
//! @param characters The number of characters to be read from the buffer.
//! @return Returns the number of characters written
size_t buffer_write_c_str (buffer_t * object, const char *src, size_t sizeof_src, size_t characters);

//! @brief Writes a string to the buffer or skips it if this is not possible
//!
//! @details Writes a string to the buffer or skips it if this is not possible
//! The string must be terminated with the string terminator '\\0'.
//!
//! The return value must be checked to ensure that everything has been written.
//!
//! @param[in,out] object The buffer object.
//! @param[in] src Contains the string or the characters.
//! @param sizeof_src The length of the buffer.
//! @param characters The number of characters to be read from the buffer.
//! @return Returns the number of characters written
size_t buffer_try_write_c_str (buffer_t * object, const char *src, size_t sizeof_src, size_t characters);

//! @brief Reads a string from the buffer
//!
//! @details Reads a string from the buffer.
//! A string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//!
//! The return value must be checked to ensure that everything has been read.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string is written in this buffer.
//! @param sizeof_dest The length of the buffer
//! @param characters The number of characters that should be read.
//! @return Returns the number of characters read
size_t buffer_read_c_str (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters);

//! @brief Reads a string from the buffer or skips it if this is not possible
//!
//! @details Reads a string from the buffer or skips it if this is not possible.
//! A string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//!
//! The return value must be checked to ensure that everything has been read.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string is written in this buffer.
//! @param sizeof_dest The length of the buffer
//! @param characters The number of characters that should be read.
//! @return Returns the number of characters read
size_t buffer_try_read_c_str (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters);

//! @brief Look at the buffer as if the data were being read or skips it if this is not possible
//!
//! @details Look at the buffer as if the data were being read or skips it if this is not possible
//! Returns if the bytes can not be read.
//! The return value must be checked to ensure that everything has been read.
//!
//! This works reliably as long as no other thread is reading characters from
//! the buffer, another consumer thread. If this thread runs in parallel with
//! a second thread that reads characters (consumer) and a third thread that
//! writes characters (producer), the behavior of this first thread becomes
//! unpredictable. Old characters may be read along with new ones.
//! Another issue with the scenario described is that this thread reads an 8-bit
//! character even though another thread is writing to the same address.
//! Normally only a single thread has access to the memory address and this is
//! achieved through the atomic consumer (c) and producer (p), as well as
//! through the head and tail pointers. In this case, that cannot be guaranteed.
//!
//! @param[in,out] object The buffer object
//! @param[in] dest The bytes are written into this array.
//! @param sizeof_dest Length of the destination array into which the bytes are written
//! `sizeof()` can be used if it is an array whose size is known at compile time.
//! @param characters The number of characters to be taken from the buffer.
//! @return Returns the number of bytes read
size_t buffer_try_peek_c_str (buffer_t * object, char * dest, size_t sizeof_dest, size_t characters);



//! @brief Reads a line from the buffer
//!
//! @details Reads a string from the buffer.
//! A string terminating character '\\0' is always written at the end.
//! The return value must be checked to ensure that everything has been read.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string/line is written in this buffer.
//! @param sizeof_dest The length of the buffer.
//! @return Returns the number of characters read
size_t buffer_read_line (buffer_t * object, char * dest, size_t sizeof_dest);

//! @brief Attempts to read a complete line from the buffer.
//!
//! Reads characters from the buffer into @p dest until a line feed character
//! ('\n') is encountered. The line feed character is consumed from the buffer
//! but is not copied to @p dest. The resulting string is always
//! null-terminated.
//!
//! If no complete line is currently available, no characters are consumed and
//! the function returns 0.
//!
//! In concurrent environments, another thread may consume data from the buffer
//! after the presence of a complete line has been detected but before the line
//! can be fully read. In this case, the function returns a negative value. The
//! absolute value of the return value indicates the number of characters that
//! were read before the line became invalid.
//!
//! @param[in,out] object The buffer object
//! @param dest Destination buffer receiving the line.
//! @param sizeof_dest Size of @p dest in bytes.
//! @return
//! @retval > 0: Number of characters copied to @p dest.
//! @retval   0: No complete line is available.
//! @retval < 0: Negative value: The line became invalid during the read
//!              operation; the absolute value is the number of characters read
//!              before the operation was aborted.
ptrdiff_t buffer_try_read_line (buffer_t * object, char * dest, size_t sizeof_dest);

//! @brief Reads the characters up to the given string
//! @details Reads the characters up to the given string
//!
//! The return value must be checked to ensure that everything has been read.
//! The string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string is written in this buffer.
//! @param sizeof_dest The length of the buffer.
//! @param to The string up to which is read
//! @param to_length The length of the buffer.
//! @return Returns the number of characters read
size_t buffer_read_to (buffer_t * object, char * dest, size_t sizeof_dest, const char * to, size_t to_length);

//! @brief On each pass, it attempts to read the data and adds the characters to the string as long as the string "to" has not yet been read.
//! @details On each pass, it attempts to read the data and adds the characters to the string as long as the string "to" has not yet been read.
//!
//! The return value must be checked to ensure that everything has been read.
//! The string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string is written in this buffer.
//! @param sizeof_dest The length of the buffer.
//! @param to The string up to which is read
//! @param to_length The length of the buffer.
//! @param data Temporary data to prevent loss of the current state.
//! @return Returns the number of characters read as a negative value if the 'to' string is not contained in the read string
size_t buffer_try_read_to (buffer_t * object, char * dest, size_t sizeof_dest, const char * to, size_t to_length, buffer_try_read_to_t * data);



//! @brief Clears the buffer
//!
//! @param[in] object The buffer object
void buffer_clear (buffer_t * object);


// ------------------------------------------------------------------------- //
//  public: static inline functions
// ------------------------------------------------------------------------- //

//! @brief Define statement for initializing a new structure
//!
//! @param DATA Start address of the buffer
//! @param DATA_LENGTH Length of the buffer
#define BUFFER_INIT(DATA, DATA_LENGTH) \
(buffer_t) { \
  /* .c_head  = */ ATOMIC_VAR_INIT(DATA), \
  /* .c_tail  = */ ATOMIC_VAR_INIT(DATA), \
  /* .c_inc   = */ ATOMIC_VAR_INIT(0), \
  /* .p_head  = */ ATOMIC_VAR_INIT(DATA), \
  /* .p_tail  = */ ATOMIC_VAR_INIT(DATA), \
  /* .p_inc   = */ ATOMIC_VAR_INIT(0), \
  /* .data    = */ (DATA), \
  /* .last    = */ ((DATA)+(DATA_LENGTH)-1), \
  /* .lines   = */ ATOMIC_VAR_INIT(0), \
  /* .c_lines = */ ATOMIC_VAR_INIT(0), \
  /* .p_lines = */ ATOMIC_VAR_INIT(0), \
  /* .end_of_line_character = */ '\n', \
  /* .on_sleep  = */ NULL, \
  /* .user_data = */ NULL, \
} // ;


#ifdef __cplusplus
}
#endif


#endif /* INC_BUFFER_H_ */


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
