//! @file
//! @brief The buffer header file.
//!
//! @details The module can be used in C and C++.
//!
//! Atomic operations are used with the header `stdatomic.h` in C or the header `atomic` C++.
//! For more information see: @ref buffer_c_and_cpp_atomic_header
//!
//! All atomic operations must be performed atomically.
//! For more information see: @ref buffer_s

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_


/*---------------------------------------------------------------------*
 *  public: include files - atomic handling
 *---------------------------------------------------------------------*/

//! @defgroup buffer_c_and_cpp_atomic_header C and C++ atomic header
//!
//! @details The module can be used in C and C++.
//! Atomic operations are used with the header `stdatomic.h` in C or the header `<atomic>` C++.
//! According to the SystemC Language Working Group (LWG), the Atomic data types can be used across
//! systems in C and C++. Since there is no `cstdatomic.h`, the appropriate header file must be
//! included via preprocessor instructions. One recommendation is:
//! <a href="https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0943r6.html">open-std.org</a>
//!
//! @{

#ifdef __cplusplus

#include <atomic>

extern "C" {
#include <stdatomic.h>
}

  #ifndef _Atomic
    #define _Atomic(X) std::atomic<X>
    #define UNDEFINE_ATOMIC
  #endif


#else

#include <stdatomic.h>

#endif


//! @}


#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------*
 *  public: workaround CDT indexer
 *---------------------------------------------------------------------*/

//! @defgroup buffer_cdt_parser_workaround Workaround for CDT indexer
//!
//! @details The CDT indexer has problems with the atomic functions.
//! For this reason, the functions are redefined, but without the claim
//! of an atomic operation.
//!
//! The same applies to the type `_Atomic(T)`. Without changing the
//! reserved keyword, the CDT indexer or auto-completion will not find
//! the variable. A disadvantage is that no information is displayed
//! about the fact that it is an atomic type.
//!
//! @{

#ifdef __CDT_PARSER__

  #error This must not be compiled

  #ifdef __cplusplus

  #else

    #define _Atomic(T) T

    #define atomic_init(PTR, VAL) \
        (*(PTR) = (VAL)) //;

    #define atomic_store(PTR, VAL) \
        (*(PTR) = (VAL)) //;

    #define atomic_load(PTR) \
        (*(PTR)) //;

    #define atomic_compare_exchange_strong(PTR, VAL, DES) \
        ((*(VAL) == *(PTR)) ? (*(PTR) = (DES), 1) : 0) //;

    #define _Static_assert(CONDITION, TEXT)

  #endif

#endif

//! @}


/*---------------------------------------------------------------------*
 *  public: include files
 *---------------------------------------------------------------------*/

// #include <atomic> is used in C++
// #include <stdatomic.h> is used in C and C++

#include <stdint.h>
#include <stdbool.h>


/*---------------------------------------------------------------------*
 *  public: define
 *---------------------------------------------------------------------*/

#ifdef __cplusplus

#ifndef INLINE

  #if defined(_MSC_VER)
    #define INLINE __forceinline
  #elif defined(__GNUC__) || defined(__clang__)
    #define INLINE inline __attribute__((always_inline))
  #else
    #define INLINE inline
  #endif

#endif


#endif


//! @defgroup buffer_disable_handler Additional option to save storage space
//!
//! @details All handlers can be deactivated by setting the BUFFER_DISABLE_HANDLER define.
//!
//! - The define must be set for all files, otherwise not enough space is reserved during
//!   use and problems occur when calling.
//! - The `BUFFER_ENABLE_HANDLER` define can be set, but is the default option.
//! - If both definitions are set, the handlers can be used, so the standard behavior wins.
//!
//! @{

#ifndef BUFFER_DISABLE_HANDLER

  #ifndef BUFFER_ENABLE_HANDLER
    #define BUFFER_ENABLE_HANDLER
  #endif

#endif

//! @}


/*---------------------------------------------------------------------*
 *  public: type test
 *---------------------------------------------------------------------*/

#ifdef __cplusplus

static_assert(sizeof(std::atomic<char*>) == sizeof(char *),
    "Layout mismatch between C and C++ atomic pointer");

static_assert(alignof(std::atomic<char*>) == alignof(char*),
    "Alignment mismatch between C and C++ atomic pointer");

static_assert(std::atomic<char*>::is_always_lock_free,
    "atomic<char*> is not always lock-free");

static_assert(sizeof(std::atomic<size_t>) == sizeof(size_t),
    "Layout mismatch between C and C++ atomic pointer");

static_assert(alignof(std::atomic<size_t>) == alignof(size_t),
    "Alignment mismatch between C and C++ atomic pointer");

static_assert(std::atomic<size_t>::is_always_lock_free,
    "atomic<size_t> is not always lock-free");

static_assert(sizeof(std::atomic<unsigned char>) == sizeof(unsigned char),
    "Layout mismatch between C and C++ atomic pointer");

static_assert(alignof(std::atomic<unsigned char>) == alignof(unsigned char),
    "Alignment mismatch between C and C++ atomic pointer");

static_assert(std::atomic<unsigned char>::is_always_lock_free,
    "atomic<unsigned char> is not always lock-free");


#else

_Static_assert(sizeof(_Atomic(char *)) == sizeof(char *),
    "Layout mismatch between _Atomic(char *) and char *");

_Static_assert(_Alignof(_Atomic(char *)) == _Alignof(char *),
    "Alignment mismatch between _Atomic(char *) and char *");

_Static_assert(ATOMIC_POINTER_LOCK_FREE == 2,
    "_Atomic(char *) is not always lock-free");

_Static_assert(sizeof(_Atomic(size_t)) == sizeof(size_t),
    "Layout mismatch between _Atomic(size_t) and size_t");

_Static_assert(_Alignof(_Atomic(size_t)) == _Alignof(size_t),
    "Alignment mismatch between _Atomic(size_t) and size_t");

// For size_t, no direct macro, use pointer-sized atomic check
_Static_assert(sizeof(size_t) == sizeof(void *),
    "Expected size_t to be pointer-sized");

_Static_assert(ATOMIC_POINTER_LOCK_FREE == 2,
    "_Atomic(size_t) is not always lock-free");

_Static_assert(sizeof(_Atomic(unsigned char)) == sizeof(unsigned char),
    "Layout mismatch between _Atomic(unsigned char) and unsigned char");

_Static_assert(_Alignof(_Atomic(unsigned char)) == _Alignof(unsigned char),
    "Alignment mismatch between _Atomic(unsigned char) and unsigned char");

_Static_assert(ATOMIC_CHAR_LOCK_FREE == 2,
    "_Atomic(unsigned char) is not always lock-free");


#endif


/*---------------------------------------------------------------------*
 *  public: typedefs
 *---------------------------------------------------------------------*/

//! @brief Forward declaration
struct buffer_s;

//! @brief Forward typedef, for information see ::buffer_s
typedef struct buffer_s buffer_t;


//! @brief Describes the status of the buffer
//!
//! @details The types ::buffer_flags_e::BUFFER_FLAGS_STOP and ::buffer_flags_e::BUFFER_FLAGS_IDLE
//! can be seen as states if exactly these are set.
//! The flags between the states indicate bit by bit which function is currently being executed.
//! The state is saved in ::buffer_s::state and of type `_Atomic(unsigned char)`.
typedef enum buffer_flags_e
{
    BUFFER_FLAGS_STOP = 0x00, ///< Stop state
    BUFFER_FLAGS_RUNNING_SET_POSSIBLE_OR_SKIP = 0x01, ///< Flag
    BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL = 0x02, ///< Flag, used with: ::buffer_get_available_or_null(), ::buffer_look_available_or_null(), ::buffer_read_to(), and ::buffer_clear()
    BUFFER_FLAGS_RUNNING_SET = 0x04, ///< Flag
    BUFFER_FLAGS_RUNNING_GET = 0x08, ///< Flag
    BUFFER_FLAGS_IDLE = 0x10, ///< State and flag, if the value is greater than or equal to this, the object is active.
}buffer_falgs_t;


#ifdef BUFFER_ENABLE_HANDLER

//! @brief Handler type of `buffer_s`
//!
//! @details Used in the functions handler:
//! - ::buffer_s::on_empty
//! - ::buffer_s::on_new_line
//! - ::buffer_s::on_error
//! - ::buffer_s::on_start
//! - ::buffer_s::on_stop
//!
//! @param[in,out] object The buffer object
typedef void (*buffer_action_handler_t)(buffer_t * object);

//! @brief Handler type of `buffer_s`
//!
//! @details Used in the functions handler:
//! - buffer_s::on_full
//! - buffer_s::on_new_character
//!
//! @param[in,out] object The buffer object
//! @param c The character in focus
typedef void (*buffer_action_char_handler_t)(buffer_t * object, char c);

//! @brief Handler type of `buffer_s`
//!
//! @details Used in the functions handler:
//! - buffer_s::on_wait_set
//! - buffer_s::on_wait_get
//!
//! @param[in,out] object The buffer object
//! @return Freely usable return value
typedef char (*buffer_function_char_handler_t)(buffer_t * object);


#endif


//! @brief Struct to create a buffer object, like an instance of a class
//!
//! @details The buffer struct can be used to exchange data between threads or a thread and an interrupt.
//!
//! All atomic operations must be performed atomically across multiple threads
//! and in the presence of interrupts, meaning that they are performed in an
//! indivisible manner from the perspective of concurrently running threads and
//! interrupt handlers. Like in FreeBSD.
//! <a href="https://man.freebsd.org/cgi/man.cgi?query=atomic_load&sektion=9&manpath=FreeBSD+14.1-RELEASE+and+Ports">man.freebsd.org</a>
struct buffer_s {

    //! @brief Buffer start address
    //! @details Start address of the `char` array which stores the data
    char * data;

    //! @brief Buffer last address
    //! @details Last address of the `char` array
    char * last;

    //! @brief End-Of-Line character
    //! @details End of line character, standard is '\\n'
    char end_of_line_character;

#ifdef BUFFER_ENABLE_HANDLER

    //! @brief Start handler
    //! @details Handler that is called when the object is started, ::buffer_start().
    buffer_action_handler_t on_start;

    //! @brief Stop handler
    //! @details Handler that is called when the object is stopped, ::buffer_stop_force(), and ::buffer_stop_try()
    buffer_action_handler_t on_stop;

    //! @brief Full handler
    //! @details Handler that is called when you want to save a character but the buffer is full.
    //! - The character that is NOT to be saved is passed
    //! - Called from producer/set thread.
    buffer_action_char_handler_t on_full;

    //! @brief Empty handler
    //! @details Handler that is called when the buffer was reset.
    //! - Called from consumer/get thread.
    buffer_action_handler_t on_empty;

    //! @brief New Character handler
    //! @details Handler that is called when a new character was save in the buffer.
    //! - The character that is to be saved is passed
    //! - Called from producer/set thread.
    buffer_action_char_handler_t on_new_character;

    //! @brief New Line handler
    //! @details Handler that is called when a new newline character was save in the buffer.
    //! - Called from producer/set thread.
    buffer_action_handler_t on_new_line;

    //! @brief Error handler
    //! @details Handler that is called when a character is to be read because there are new characters,
    //! but the read address is above the last element. This is an error that occurs due to
    //! external manipulation.
    //! - Called from consumer/get thread.
    buffer_action_handler_t on_error;

    //! @brief Set wait handler
    //! @details Handler that is called in each cycle when you use ::buffer_set()
    //! while no space or no character is available.
    //! - Any value other than 0 causes the wait loop to be exited
    //! - Called from producer/set thread.
    //! - Function must not block or must itself monitor ::buffer_s::state and react to a forced stop.
    buffer_function_char_handler_t on_wait_set;

    //! @brief Get wait handler
    //! @details Handler that is called in each cycle when you use ::buffer_get()
    //! while no space or no character is available.
    //! - Any value other than 0 causes the wait loop to be exited
    //! - Called from consumer/get thread.
    //! - Function must not block or must itself monitor ::buffer_s::state and react to a forced stop.
    buffer_function_char_handler_t on_wait_get;

#endif

    //! @brief Consumer pointer
    //! @details Pointer to the next position of the buffer to be read from.
    //! - From the consumer thread, the pointer can be used directly.
    //! - Note that the pointer is not null ('\\0') terminated.
    //! - Use the ::buffer_s::length element.
    char * consumer_ptr;

    //! @brief Producer pointer
    //! @details Pointer to the next position of the buffer to be written to.
    //! - Use ::atomic_load() if you need to use the element directly.
    volatile _Atomic(char *) producer_ptr;

    //! @brief Number of characters is buffer
    //! @details Current number of characters stored in the buffer.
    //! - Use ::atomic_load() if you need to use the element directly.
    volatile _Atomic(size_t) length;

    //! @brief Number of newline characters
    //! @details Current number of newline character stored in the buffer.
    //! - Use ::atomic_load() if you need to use the element directly.
    volatile _Atomic(size_t) lines;

    //! @brief Contains the buffer state
    //! @details Provides information about which functions are currently
    //! being executed. The `enum` ::buffer_flags_e type provides
    //! more information about the individual bits.
    //! - Use ::atomic_load() if you need to use the element directly.
    volatile _Atomic(unsigned char) state;

};

//! @brief Represents a simplified form of a class
//! @details The global variable ::buffer can be used to easily access all matching
//! functions with auto-completion.
struct buffer_sc
{
    bool       (* Clear    ) (      buffer_t * object);     ///< @brief See ::buffer_clear()
    void       (* Copy     ) (const buffer_t * object, buffer_t * dest);          ///< @brief See ::buffer_copy()
    bool       (* Equal    ) (const buffer_t * object, const buffer_t * object2); ///< @brief See ::buffer_equal()
    char       (* Get      ) (      buffer_t * object);     ///< @brief See ::buffer_get()
    char       (* GetAvailableOrNull ) (buffer_t * object); ///< @brief See ::buffer_get_available_or_null()
    bool       (* Init     ) (      buffer_t * object, char * data, size_t sizeof_data, bool start); ///< @brief See ::buffer_init()
    bool       (* IsEmpty  ) (const buffer_t * object);     ///< @brief See ::buffer_is_empty()
    bool       (* IsFull   ) (const buffer_t * object);     ///< @brief See ::buffer_is_full()
    bool       (* IsStopped) (const buffer_t * object);     ///< @brief See ::buffer_is_stoped()
    size_t     (* Length   ) (const buffer_t * object);     ///< @brief See ::buffer_length()
    size_t     (* Lines    ) (const buffer_t * object);     ///< @brief See ::buffer_lines()
    char       (* LookAvailableOrNull) (buffer_t * object); ///< @brief See ::buffer_look_available_or_null()
    buffer_t * (* ObjectAllocate) (char * data, size_t sizeof_data, bool start); ///< @brief See ::buffer_object_allocate()
    bool       (* ObjectFree)(buffer_t * object);           ///< @brief See ::buffer_object_free()
    size_t     (* Read     ) (      buffer_t * object, char * dest, size_t n); ///< @brief See ::buffer_read()
    size_t     (* ReadLine ) (      buffer_t * object, char * dest, size_t n); ///< @brief See ::buffer_read_line()
    size_t     (* ReadTo   ) (      buffer_t * object, char * dest, size_t n, const char * to, size_t to_length); ///< @brief See ::buffer_read_to()
    bool       (* Reset    ) (      buffer_t * object, bool start); ///< @brief See ::buffer_reset()
    bool       (* Set      ) (      buffer_t * object, char c);     ///< @brief See ::buffer_set()
    bool       (* SetPossibleOrSkip  ) (buffer_t * object, char c); ///< @brief See ::buffer_set_possible_or_skip()
    size_t     (* Space    ) (const buffer_t * object); ///< @brief See ::buffer_space()
    bool       (* Start    ) (      buffer_t * object); ///< @brief See ::buffer_start()
    bool       (* StopForce) (      buffer_t * object); ///< @brief See ::buffer_stop_force()
    bool       (* StopTry  ) (      buffer_t * object); ///< @brief See ::buffer_stop_try()
    size_t     (* Write    ) (      buffer_t * object, const char *src, size_t n); ///< @brief See ::buffer_write()
};


/*---------------------------------------------------------------------*
 *  public: extern variables
 *---------------------------------------------------------------------*/

//! @brief To access all member functions working with type ::buffer_s
//! @details Allows a simplified class to easily access all member functions
//! working with type ::buffer_s. The auto-completion function helps you to select all
//! suitable functions via ::buffer_sc struct.
extern const struct buffer_sc buffer;


/*---------------------------------------------------------------------*
 *  public: function prototypes
 *---------------------------------------------------------------------*/

//! @brief Tries to clear the buffer
//!
//! @details Attempts to clear the buffer, fails if the buffer was written
//! to during the time to reset the buffer.
//!
//! However, the buffer does not have to be stopped for this.
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @return Returns whether the buffer could be clear
//! @retval false Buffer could not be clear
//! @retval true  Buffer could be clear
bool buffer_clear(buffer_t * object);

//! @brief Copying one structure to another
//!
//! @details Compares all elements of the structure
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object Object that is copied
//! @param[out] dest Target which is overwritten
void buffer_copy(const buffer_t * object, buffer_t * dest);

//! @brief Compares two struct objects of type ::buffer_s and returns whether they are equal
//!
//! @details Compares all elements of the structure
//!
//! Can be use in:
//! - producer/set thread, with stopped buffer
//! - consumer/get thread, with stopped buffer
//!
//! @param[in] object The buffer object
//! @param[in] object2 The buffer object
//! @return Returns whether the struct objects are equal
//! @retval true  Struct objects are equal
//! @retval false Struct objects are different
bool buffer_equal(const buffer_t * object, const buffer_t * object2);

//! @brief Reads a character or waits until it can be executed.
//!
//! @details Reads a character in the buffer, blocks as long as the character can be read
//!
//! Can be use in:
//! - consumer/get thread
//!
//! @param[in,out] object The buffer object
//! @return The read character
char buffer_get(buffer_t * object);

//! @brief Reads a character or skips it if this is not possible
//!
//! @details Tries to read a character or skips it if this is not possible
//!
//! Can be use in:
//! - consumer/get thread
//!
//! @param[in,out] object The buffer object
//! @return Returns the character or null
//! @retval 0 Returns a binary null, or ('\\0') if nothing is available
//! @retval else The read character
char buffer_get_available_or_null(buffer_t * object);

//! @brief Function for initialization
//!
//! @details Function for initialization the buffer object. First, the buffer is
//! switched off with the function ::buffer_stop_force() whose return value is returned.
//!
//! Can be use in:
//! - producer/set thread, with stopped buffer
//! - consumer/get thread, with stopped buffer
//!
//! @attention Must not be used if one of the threads is used. Stop the buffer with
//! ::buffer_stop_force() or ::buffer_stop_try() and check the return value or call
//! ::buffer_is_stoped() to check if the buffer could be stopped.
//!
//! @param[in,out] object The buffer object
//! @param data The `char` array which stores the data
//! @param sizeof_data Length of the `char` array in which the data is stored. ::sizeof() can be used if it is an array whose size is known at compile time.
//! @param start Starting or stopping the buffer, if parameter @p data is NULL, the buffer cannot be started.
//! @return Returns whether the buffer could be stopped without problems.
//! @retval true The buffer was successfully stopped.
//! @retval false The buffer could not be stopped.
bool buffer_init(buffer_t * object, char * data, size_t sizeof_data, bool start);

//! @brief Checks if the buffer is empty
//!
//! @details Function to check if the buffer is empty
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object The buffer object
//! @return Returns if the buffer is empty
//! @retval true The buffer is empty
//! @retval false The buffer is NOT empty
bool buffer_is_empty(const buffer_t * object);

//! @brief Checks if the buffer is full
//!
//! @details Function to check if the buffer is full
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object The buffer object
//! @return Returns if the buffer is full
//! @retval true The buffer is full
//! @retval false The buffer is NOT full
bool buffer_is_full(const buffer_t * object);

//! @brief Checks whether the buffer is stopped
//!
//! @details Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object The buffer object
//! @return Returns whether the buffer could be stopped without problems.
//! @retval true The buffer is stopped.
//! @retval false The buffer is started (running or idle).
bool buffer_is_stopped(const buffer_t * object);

//! @brief Returns the characters currently used
//!
//! @details Returns the currently used characters in the array
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object The buffer object
//! @return Positive number of used characters
size_t buffer_length(const buffer_t * object);

//! @brief Returns the lines currently used
//!
//! @details Returns the currently used lines in the array.
//! The recognized character is defined in `buffer_s::end_of_line_character`.
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object The buffer object
//! @return Positive number of used characters
size_t buffer_lines(const buffer_t * object);

//! @brief Reads the next character in the buffer without deleting it
//!
//! @details Reads the next available character in the buffer without deleting it.
//! If the buffer is empty, null is returned.
//!
//! Can be use in:
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @return Returns the character or null
//! @retval 0 Returns a binary null, or ('\\0') if nothing is available
//! @retval else The read character
char buffer_look_available_or_null(buffer_t * object);

//! @brief Dynamic allocation of memory for the object
//!
//! @details Always check if the function returns a `NULL` pointer.
//!
//! The assigned object must be released again with the function ::buffer_object_free().
//! If it is ensured that the buffer is stopped (::buffer_stop_force(), ::buffer_stop_try())
//! then the standard function ::free() can be used.
//!
//! @param data The `char` array which stores the data, can be `NULL`.
//! If NULL is passed and the parameter @p sizeof_data has a value other than zero,
//! memory space is also reserved for the data array.
//! @param sizeof_data Length of the `char` array in which the data is stored. ::sizeof() can be used if it is an array whose size is known at compile time.
//! @param start Starting or stopping the buffer, if parameter @p data is `NULL`, the buffer cannot be started.
//! @return Returns a pointer to the dynamically allocated memory.
//! @retval NULL Allocation failed
//! @retval else The pointer
buffer_t * buffer_object_allocate(char * data, size_t sizeof_data, bool start);

//! @brief Release memory
//!
//! @details Which was previously allocated by the function ::buffer_object_allocate().
//!
//! If it is ensured that the buffer is stopped (::buffer_stop_force(), ::buffer_stop_try())
//! then the standard function ::free() can be used.
//!
//! @param[in,out] object The buffer object, `NULL` is allowed
//! @return Returns whether the buffer could be stopped without problems.
//! @retval true The buffer was successfully stopped.
//! @retval false The buffer could not be stopped or object was `NULL`.
bool buffer_object_free(buffer_t * object);

//! @brief Reads a string from the buffer
//!
//! @details Reads a string from the buffer,
//! a string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//! This means that one character less is read than is specified in parameter @p n.
//!
//! Does not block and does not guarantee a read. The return value must
//! be checked to ensure that everything has been read.
//!
//! Can be use in:
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string is written in this buffer.
//! @param n The length of the buffer (@p dest parameter)
//! including the string terminator character '\\0'
//! @return Returns the number of characters read
size_t buffer_read(buffer_t * object, char * dest, size_t n);

//! @brief Reads a line from the buffer
//!
//! @details Reads a string from the buffer,
//! a string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//! This means that one character less is read than is specified in parameter @p n.
//!
//! Does not block and does not guarantee a read. The return value must
//! be checked to ensure that everything has been read.
//!
//! Can be use in:
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string/line is written in this buffer.
//! @param n The length of the buffer (@p dest parameter)
//! including the string terminator character '\\0'
//! @return Returns the number of characters read
size_t buffer_read_line(buffer_t * object, char * dest, size_t n);

//! @brief Reads the characters if the character string of the parameter @p to is contained
//! @details Then only returns the characters up to the string parameter @p to.
//!
//! Does not block and does not guarantee a read. The return value must
//! be checked to ensure that everything has been read.
//! The string terminating character '\\0' is always written at the end.
//! It is therefore the same as c_stc in C++.
//!
//! Can be use in:
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @param[out] dest The string is written in this buffer.
//! @param n The length of the buffer (@p dest parameter)
//! including the string terminator character '\\0'
//! @param to The string up to which is read
//! @param to_length The number of characters without string terminator
//! '\\0', ::strlen() can be used from parameter @p to
//! @return Returns the number of characters read
size_t buffer_read_to(buffer_t * object, char * dest, size_t n, const char * to, size_t to_length);

//! @brief Function to reset
//!
//! @details Function for reset the buffer object. First, the buffer is
//! switched off with the function ::buffer_stop_force() whose return value is returned.
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @attention Must not be used if one of the threads is used. Stop the buffer with
//! ::buffer_stop_force() or ::buffer_stop_try() and check the return value or call
//! ::buffer_is_stoped() to check if the buffer could be stopped.
//!
//! @param[in,out] object The buffer object
//! @param start Starting or stopping the buffer
//! @return Returns whether the buffer could be stopped without problems.
//! @retval true The buffer was successfully stopped.
//! @retval false The buffer could not be stopped.
bool buffer_reset(buffer_t * object, bool start);

//! @brief Saves a character or waits until it can be executed
//!
//! @details Stores a character in the buffer, blocks as long as the character can be stored
//!
//! Can be use in:
//! - producer/set thread.
//!
//! @param[in,out] object The buffer object
//! @param c The character that will be stored
//! @return Returns whether the character could be saved
//! @retval false Character could not be saved
//! @retval true  Character could be saved
bool buffer_set(buffer_t * object, char c);

//! @brief Saves a character or skips it if this is not possible
//!
//! @details Tries to save a character or skips it if this is not possible
//!
//! Can be use in:
//! - producer/set thread.
//!
//! @param[in,out] object The buffer object
//! @param c The character that will be stored
//! @return Returns whether the character could be saved
//! @retval false Character could not be saved
//! @retval true  Character could be saved
bool buffer_set_possible_or_skip(buffer_t * object, char c);

//! @brief Returns the free space
//!
//! @details Returns the available space in the array.
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in] object The buffer object
//! @return Positive number of available space
size_t buffer_space(const buffer_t * object);

//! @brief Starts the buffer
//!
//! @details Starts the buffer or releases the execution of the functions.
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @return Returns whether the buffer could be started without problems.
//! @retval true The buffer was successfully started.
//! @retval false The buffer could not be started.
bool buffer_start(buffer_t * object);

//! @brief Stops further calls of the buffer
//!
//! @details Stops the buffer; no new functions can be called from this
//! point onwards. Functions that are already running will continue to be executed.
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @return Returns whether the buffer could be stopped without problems.
//! @retval true The buffer was successfully stopped.
//! @retval false The buffer could not be stopped.
bool buffer_stop_force(buffer_t * object);

//! @brief Try to stop the buffer
//!
//! @details Checks whether the buffer can be stopped and stops it
//!
//! Can be use in:
//! - producer/set thread.
//! - consumer/get thread.
//!
//! @param[in,out] object The buffer object
//! @return Returns whether the buffer could be stopped without problems.
//! @retval true The buffer was successfully stopped.
//! @retval false The buffer could not be stopped.
bool buffer_stop_try(buffer_t * object);

//! @brief Writes a string to the buffer
//!
//! @details Writes a string to the buffer.
//! The string should be terminated with the string terminator '\\0'.
//! Otherwise, as many characters as specified in the parameter @p n are saved.
//!
//! Does not block and does not guarantee a write. The return value must
//! be checked to ensure that everything has been written.
//!
//! Can be use in:
//! - producer/set thread.
//!
//! @param[in,out] object The buffer object
//! @param[in] src Contains the string or the characters.
//! @param n The number of characters to be stored in the buffer.
//! The length of the buffer (parameter @p src) can be used.
//! The string terminator character '\\0' is not written to the buffer.
//! @return Returns the number of characters written
size_t buffer_write(buffer_t * object, const char *src, size_t n);

/*---------------------------------------------------------------------*
 *  public: static inline functions
 *---------------------------------------------------------------------*/

#ifdef BUFFER_ENABLE_HANDLER

//! @brief Define statement for initializing a new structure
//!
//! @param DATA Start address of the buffer
//! @param DATA_LENGTH Length of the buffer
//! @param START Starting or stopping the buffer, if parameter @p DATA is NULL, the buffer cannot be started
#define BUFFER_INIT(DATA, DATA_LENGTH, START) { \
    /* .data                  = */ (0 == (DATA_LENGTH)) ? NULL : (DATA), \
    /* .last                  = */ ((NULL == (DATA)) || (0 == (DATA_LENGTH)) ) ? NULL : (char *)(DATA) + (DATA_LENGTH) - 1, \
    /* .end_of_line_character = */ '\n', \
    /* .start                 = */ NULL, \
    /* .stop                  = */ NULL, \
    /* .full                  = */ NULL, \
    /* .empty                 = */ NULL, \
    /* .new_character         = */ NULL, \
    /* .new_line              = */ NULL, \
    /* .error                 = */ NULL, \
    /* .wait_set              = */ NULL, \
    /* .wait_get              = */ NULL, \
    /* .consumer_ptr          = */ (DATA), \
    /* .producer_ptr          = */ ATOMIC_VAR_INIT(DATA), \
    /* .length                = */ ATOMIC_VAR_INIT(0), \
    /* .lines                 = */ ATOMIC_VAR_INIT(0), \
    /* .state                 = */ ATOMIC_VAR_INIT( ( (NULL != (DATA)) && (0 != (DATA_LENGTH)) && (START) ) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP ), \
} //;


#else

//! @brief Define statement for initializing a new structure
//!
//! @param DATA Start address of the buffer
//! @param DATA_LENGTH Length of the buffer
//! @param START Starting or stopping the buffer, if parameter @p DATA is NULL, the buffer cannot be started
#define BUFFER_INIT(DATA, DATA_LENGTH, START) { \
    /* .data                  = */ (0 == (DATA_LENGTH)) ? NULL : (DATA), \
    /* .last                  = */ ((NULL == (DATA)) || (0 == (DATA_LENGTH)) ) ? NULL : (char *)(DATA) + (DATA_LENGTH) - 1, \
    /* .end_of_line_character = */ '\n', \
    /* .consumer_ptr          = */ (DATA), \
    /* .producer_ptr          = */ ATOMIC_VAR_INIT(DATA), \
    /* .length                = */ ATOMIC_VAR_INIT(0), \
    /* .lines                 = */ ATOMIC_VAR_INIT(0), \
    /* .state                 = */ ATOMIC_VAR_INIT( ( (NULL != (DATA)) && (0 != (DATA_LENGTH)) && (START) ) ? BUFFER_FLAGS_IDLE : BUFFER_FLAGS_STOP ), \
} //;

#endif


#ifdef __cplusplus

static INLINE char * atomic_load(char ** raw_ptr)
{
    return std::atomic_load(reinterpret_cast<std::atomic<char*>*>(raw_ptr));
}

static INLINE void atomic_store(char ** raw_ptr, char * desired)
{
    std::atomic_store(reinterpret_cast<std::atomic<char*>*>(raw_ptr), desired);
}

static INLINE bool atomic_compare_exchange_strong(char ** raw_ptr, char ** raw_ptr_expected, char * desired)
{
    return atomic_compare_exchange_strong(reinterpret_cast<std::atomic<char*>*>(raw_ptr), raw_ptr_expected, desired);
}

static INLINE char * atomic_fetch_add(char ** raw_ptr, ptrdiff_t arg)
{
    return atomic_fetch_add(reinterpret_cast<std::atomic<char*>*>(raw_ptr), arg);
}

#endif


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#ifdef UNDEFINE_ATOMIC
#undef _Atomic
#endif
#endif

#endif /* INC_BUFFER_H_ */

/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/
