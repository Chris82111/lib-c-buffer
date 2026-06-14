//! @file
//! @brief Source file Buffer.cpp


// ------------------------------------------------------------------------- //
//  private: include files
// ------------------------------------------------------------------------- //

#include "Buffer.h"


// ------------------------------------------------------------------------- //
//  private: definitions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: macros like functions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: typedefs
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  public:  variables
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: function prototypes
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private: inline functions
// ------------------------------------------------------------------------- //
// ------------------------------------------------------------------------- //
//  private:  functions
// ------------------------------------------------------------------------- //

void Buffer::Init (char * data, size_t sizeof_data)
{
  buffer_init(c_object, data, sizeof_data);
}


// ------------------------------------------------------------------------- //
//  public:  functions
// ------------------------------------------------------------------------- //


bool Buffer::CheckLockFreeRuntime ()
{
  return buffer_check_lock_free_runtime();
}



Buffer::Buffer (char * data, size_t sizeof_data)
{
  c_object = &objectData;
  Init(data, sizeof_data);
}

Buffer::Buffer (std::span<char> data)
{
  c_object = &objectData;
  Init(data.data(), data.size());
}

bool Buffer::Set (char c)
{
  return buffer_set (c_object, c);
}

bool Buffer::TrySet (char c)
{
  return buffer_try_set (c_object, c);
}

char Buffer::Get ()
{
  return buffer_get (c_object);
}

bool Buffer::WaitGet (char * c)
{
  return buffer_wait_get (c_object, c);
}

bool Buffer::TryGet (char * c)
{
  return buffer_try_get (c_object, c);
}

size_t Buffer::WriteBytes (const char *src, size_t sizeof_src, size_t bytes)
{
  return buffer_write_bytes (c_object, src, sizeof_src, bytes);
}

size_t Buffer::TryWriteBytes (const char *src, size_t sizeof_src, size_t bytes)
{
  return buffer_write_bytes (c_object, src, sizeof_src, bytes);
}

size_t Buffer::ReadBytes (char * dest, size_t sizeof_dest, size_t bytes)
{
  return buffer_read_bytes (c_object, dest, sizeof_dest, bytes);
}

size_t Buffer::TryReadBytes (char * dest, size_t sizeof_dest, size_t bytes)
{
  return buffer_try_read_bytes (c_object, dest, sizeof_dest, bytes);
}

size_t Buffer::TryPeekBytes (char * dest, size_t sizeof_dest, size_t bytes)
{
  return buffer_try_peek_bytes (c_object, dest, sizeof_dest, bytes);
}



bool Buffer::IsEmpty ()
{
  return buffer_is_empty (c_object);
}

bool Buffer::IsNotEmpty ()
{
  return buffer_is_not_empty (c_object);
}

bool Buffer::IsFull ()
{
  return buffer_is_full (c_object);
}

bool Buffer::IsNotFull ()
{
  return buffer_is_not_full (c_object);
}

size_t Buffer::Length ()
{
  return buffer_length (c_object);
}

size_t Buffer::Space ()
{
  return buffer_space (c_object);
}



size_t Buffer::WriteCStr (const char *src, size_t sizeof_src, size_t characters)
{
  return buffer_write_c_str (c_object, src, sizeof_src, characters);
}

size_t Buffer::Write(std::span<const char> src)
{
  return buffer_write_c_str(c_object, src.data(), src.size(), SIZE_MAX);
}

size_t Buffer::Write(std::string_view sv)
{
    return WriteCStr(sv.data(), sv.size(), sv.size());
}

size_t Buffer::Write(const char* src)
{
  return buffer_write_c_str(c_object, src, SIZE_MAX, SIZE_MAX);
}

size_t Buffer::TryWriteCStr (const char *src, size_t sizeof_src, size_t characters)
{
  return buffer_try_write_c_str (c_object, src, sizeof_src, characters);
}

size_t Buffer::ReadCStr (char * dest, size_t sizeof_dest, size_t characters)
{
  return buffer_read_c_str (c_object, dest, sizeof_dest, characters);
}

size_t Buffer::TryReadCStr (char * dest, size_t sizeof_dest, size_t characters)
{
  return buffer_try_read_c_str (c_object, dest, sizeof_dest, characters);
}

size_t Buffer::TryPeekCStr (char * dest, size_t sizeof_dest, size_t characters)
{
  return buffer_try_peek_c_str (c_object, dest, sizeof_dest, characters);
}



size_t Buffer::ReadLine (char * dest, size_t sizeof_dest)
{
  return buffer_read_line (c_object, dest, sizeof_dest);
}

ptrdiff_t Buffer::TryReadLine (char * dest, size_t sizeof_dest)
{
  return buffer_try_read_line (c_object, dest, sizeof_dest);
}

size_t Buffer::ReadTo (char * dest, size_t sizeof_dest, const char * to, size_t to_length)
{
  return buffer_read_to (c_object, dest, sizeof_dest, to, to_length);
}

ptrdiff_t Buffer::TryReadTo (char * dest, size_t sizeof_dest, const char * to, size_t to_length, buffer_try_read_to_t * data)
{
  return buffer_try_read_to (c_object, dest, sizeof_dest, to, to_length, data);
}



void Buffer::Clear ()
{
  buffer_clear (c_object);
}


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
