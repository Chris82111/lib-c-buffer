//! @file
//! @brief The Buffer.h header file.
//!
//! @details The module can be used in C++.


#ifndef INC_BUFFER_CPP_H
#define INC_BUFFER_CPP_H


// ------------------------------------------------------------------------- //
//  public: atomic handling
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: include files
// ------------------------------------------------------------------------- //

#include <fstream>

#include "buffer.h"


// ------------------------------------------------------------------------- //
//  public: define
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: typedefs
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: function prototypes
// ------------------------------------------------------------------------- //


//! @brief To access all member functions working with type ::event_lf_s
//!
//! @details Allows to easy access all member functions working with type
//!          ::event_lf_s. The auto-completion function helps you to
//!          select all suitable functions.
//!
class Buffer
{

public:
  //! @brief Checks at runtime whether selected atomic types are lock-free.
  //!
  //! @details This function verifies that all atomic types used in
  //!          `buffer_t` are lock-free on the current platform.
  //!
  //! @return If all tested atomic types are lock-free
  //! @retval true If all tested atomic types are lock-free
  //! @retval false If not all tested atomic types are lock-free
  static bool CheckLockFreeRuntime ();


private:
  //! @brief The underlying C structure of the memory object
  buffer_t objectData;

  //! @brief Function for initialization
  //! @details Function for initialization the buffer object.
  //! @param[in] data The `char` array which stores the data
  //! @param sizeof_data Length of the `char` array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  void Init (char * data, size_t sizeof_data);


public:
  //! @brief A pointer to the `buffer_t` struct object, so it can be used with C functions
  buffer_t * c_object;

  //! @brief Initializes the class with the given array and size of the array
  //! @param[in] data The `char` array which stores the data
  //! @param sizeof_data Length of the `char` array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  Buffer (char * data, size_t sizeof_data);

  Buffer (std::span<char> data);

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
  //! @param c The character that will be stored
  //! @return Returns whether the character is written
  //! @retval false Character was not written (The function was cancelled)
  //! @retval true  Character was written
  bool Set (char c);

  //! @brief Saves a character or skips it if this is not possible
  //!
  //! @details Tries to save a character or skips it if this is not possible
  //!
  //! @param c The character that will be stored
  //! @return Returns whether the character could be saved
  //! @retval false Character could not be saved
  //! @retval true  Character could be saved
  bool TrySet (char c);

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
  //! @return The read character
  char Get ();

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
  //! @param[out] c The character that was read
  //! @return Returns whether the character is read
  //! @retval false Character was not read (The function was cancelled)
  //! @retval true  Character was read
  bool WaitGet (char * c);

  //! @brief Reads a character or skips it if this is not possible
  //!
  //! @details Tries to read a character or skips it if this is not possible
  //!
  //! @param[out] c The character that was read
  //! @return Returns whether the character could be read
  //! @retval false Character could not be read
  //! @retval true  Character could be read
  bool TryGet (char * c);



  //! @brief Writes an array to the buffer
  //!
  //! @details Writes an array to the buffer.
  //! Blocks as long as the bytes can not be stored.
  //! The return value must be checked to ensure that everything has been written.
  //! @param[in] src Contains the data that will be written.
  //! @param sizeof_src Length of the array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be stored in the buffer.
  //! @return Returns the number of bytes written
  size_t WriteBytes (const char *src, size_t sizeof_src, size_t bytes);

  //! @brief Writes an array to the buffer or skips it if this is not possible
  //!
  //! @details Writes an array to the buffer or skips it if this is not possible
  //! Returns if the bytes can not be stored.
  //! The return value must be checked to ensure that everything has been written.
  //! @param[in] src Contains the data that will be written.
  //! @param sizeof_src Length of the array in which the data is stored.
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be stored in the buffer.
  //! @return Returns the number of bytes written
  size_t TryWriteBytes (const char *src, size_t sizeof_src, size_t bytes);

  //! @brief Reads an array from the buffer
  //!
  //! @details Reads an array from the buffer.
  //! Blocks as long as the bytes can not be read.
  //! The return value must be checked to ensure that everything has been read.
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t ReadBytes (char * dest, size_t sizeof_dest, size_t bytes);

  //! @brief Reads an array from the buffer or skips it if this is not possible
  //!
  //! @details Reads an array from the buffer or skips it if this is not possible
  //! Returns if the bytes can not be read.
  //! The return value must be checked to ensure that everything has been read.
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t TryReadBytes (char * dest, size_t sizeof_dest, size_t bytes);

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
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param bytes The number of bytes to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t TryPeekBytes (char * dest, size_t sizeof_dest, size_t bytes);



  //! @brief Checks if the buffer is empty
  //!
  //! @details Function to check if the buffer is empty
  //!
  //! @return Returns if the buffer is empty
  //! @retval true The buffer is empty
  //! @retval false The buffer is NOT empty
  bool IsEmpty ();

  //! @brief Checks if the buffer is not empty
  //!
  //! @details Function to check if the buffer is not empty
  //!
  //! @return Returns if the buffer is empty
  //! @retval true The buffer is NOT empty
  //! @retval false The buffer is empty
  bool IsNotEmpty ();

  //! @brief Checks if the buffer is full
  //!
  //! @details Function to check if the buffer is full
  //!
  //! @return Returns if the buffer is full
  //! @retval true The buffer is full
  //! @retval false The buffer is NOT full
  bool IsFull ();

  //! @brief Checks if the buffer is not full
  //!
  //! @details Function to check if the buffer is not full
  //!
  //! @return Returns if the buffer is full
  //! @retval true The buffer is NOT full
  //! @retval false The buffer is full
  bool IsNotFull ();

  //! @brief Returns the characters currently used
  //!
  //! @details Returns the currently used characters in the array
  //!
  //! @return Positive number of used characters
  size_t Length ();

  //! @brief Returns the free space
  //!
  //! @details Returns the available space in the array.
  //!
  //! @return Positive number of available space
  size_t Space ();



  //! @brief Returns the lines currently used
  //!
  //! @details Returns the currently used lines in the array.
  //! The recognized character is defined in `buffer_s::end_of_line_character`.
  //!
  //! @param[in] object The buffer object
  //! @return Positive number of used characters
  size_t buffer_lines ();



  //! @brief Writes a string to the buffer
  //!
  //! @details Writes a string to the buffer.
  //! The string must be terminated with the string terminator '\\0'.
  //!
  //! The return value must be checked to ensure that everything has been written.
  //!
  //! @param[in] src Contains the string or the characters.
  //! @param sizeof_src The length of the buffer.
  //! @param characters The number of characters to be read from the buffer.
  //! @return Returns the number of characters written
  size_t WriteCStr (const char *src, size_t sizeof_src, size_t characters);

  size_t Write(std::span<const char> src);

  size_t Write(std::string_view str);

  size_t Write(const char* src);

  //! @brief Writes a string to the buffer or skips it if this is not possible
  //!
  //! @details Writes a string to the buffer or skips it if this is not possible
  //! The string must be terminated with the string terminator '\\0'.
  //!
  //! The return value must be checked to ensure that everything has been written.
  //!
  //! @param[in] src Contains the string or the characters.
  //! @param sizeof_src The length of the buffer.
  //! @param characters The number of characters to be read from the buffer.
  //! @return Returns the number of characters written
  size_t TryWriteCStr (const char *src, size_t sizeof_src, size_t characters);

  //! @brief Reads a string from the buffer
  //!
  //! @details Reads a string from the buffer.
  //! A string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer
  //! @param characters The number of characters that should be read.
  //! @return Returns the number of characters read
  size_t ReadCStr (char * dest, size_t sizeof_dest, size_t characters);

  //! @brief Reads a string from the buffer or skips it if this is not possible
  //!
  //! @details Reads a string from the buffer or skips it if this is not possible.
  //! A string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer
  //! @param characters The number of characters that should be read.
  //! @return Returns the number of characters read
  size_t TryReadCStr (char * dest, size_t sizeof_dest, size_t characters);

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
  //! @param[in] dest The bytes are written into this array.
  //! @param sizeof_dest Length of the destination array into which the bytes are written
  //! `sizeof()` can be used if it is an array whose size is known at compile time.
  //! @param characters The number of characters to be taken from the buffer.
  //! @return Returns the number of bytes read
  size_t TryPeekCStr (char * dest, size_t sizeof_dest, size_t characters);



  //! @brief Reads a line from the buffer
  //!
  //! @details Reads a string from the buffer.
  //! A string terminating character '\\0' is always written at the end.
  //! The return value must be checked to ensure that everything has been read.
  //!
  //! @param[out] dest The string/line is written in this buffer.
  //! @param sizeof_dest The length of the buffer.
  //! @return Returns the number of characters read
  size_t ReadLine (char * dest, size_t sizeof_dest);

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
  //! @param dest Destination buffer receiving the line.
  //! @param sizeof_dest Size of @p dest in bytes.
  //! @return
  //! @retval > 0: Number of characters copied to @p dest.
  //! @retval   0: No complete line is available.
  //! @retval < 0: Negative value: The line became invalid during the read
  //!              operation; the absolute value is the number of characters read
  //!              before the operation was aborted.
  ptrdiff_t TryReadLine (char * dest, size_t sizeof_dest);

  //! @brief Reads the characters up to the given string
  //! @details Reads the characters up to the given string
  //!
  //! The return value must be checked to ensure that everything has been read.
  //! The string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer.
  //! @param to The string up to which is read
  //! @param to_length The length of the buffer.
  //! @return Returns the number of characters read
  size_t ReadTo (char * dest, size_t sizeof_dest, const char * to, size_t to_length);

  //! @brief On each pass, it attempts to read the data and adds the characters to the string as long as the string "to" has not yet been read.
  //! @details On each pass, it attempts to read the data and adds the characters to the string as long as the string "to" has not yet been read.
  //!
  //! The return value must be checked to ensure that everything has been read.
  //! The string terminating character '\\0' is always written at the end.
  //! It is therefore the same as c_stc in C++.
  //!
  //! @param[out] dest The string is written in this buffer.
  //! @param sizeof_dest The length of the buffer.
  //! @param to The string up to which is read
  //! @param to_length The length of the buffer.
  //! @param data Temporary data to prevent loss of the current state.
  //! @return Returns the number of characters read as a negative value if the 'to' string is not contained in the read string
  ptrdiff_t TryReadTo (char * dest, size_t sizeof_dest, const char * to, size_t to_length, buffer_try_read_to_t * data);



  //! @brief Clears the buffer
  void Clear ();


};


// ------------------------------------------------------------------------- //
//  public: extern variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: macros like functions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public: static inline functions
// ------------------------------------------------------------------------- //


#endif // INC_BUFFER_CPP_H


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
