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

size_t Buffer::WriteBytes (std::span<const char> src, size_t bytes)
{
  return buffer_write_bytes (c_object, src.data(), src.size(), bytes);
}

size_t Buffer::WriteBytes (std::span<const char> src)
{
  return buffer_write_bytes (c_object, src.data(), src.size(), SIZE_MAX);
}

size_t Buffer::WriteBytes (std::string_view sv, size_t bytes)
{
    return buffer_write_bytes (c_object, sv.data(), sv.size(), bytes);
}

size_t Buffer::WriteBytes (std::string_view sv)
{
    return buffer_write_bytes (c_object, sv.data(), sv.size(), SIZE_MAX);
}

size_t Buffer::WriteBytes (const char* src)
{
  return buffer_write_bytes (c_object, src, SIZE_MAX, SIZE_MAX);
}

size_t Buffer::TryWriteBytes (const char *src, size_t sizeof_src, size_t bytes)
{
  return buffer_try_write_bytes (c_object, src, sizeof_src, bytes);
}

size_t Buffer::TryWriteBytes (std::span<const char> src, size_t bytes)
{
  return buffer_try_write_bytes (c_object, src.data(), src.size(), bytes);
}

size_t Buffer::TryWriteBytes (std::span<const char> src)
{
  return buffer_try_write_bytes (c_object, src.data(), src.size(), SIZE_MAX);
}

size_t Buffer::TryWriteBytes (std::string_view sv, size_t bytes)
{
    return buffer_try_write_bytes (c_object, sv.data(), sv.size(), bytes);
}

size_t Buffer::TryWriteBytes (std::string_view sv)
{
    return buffer_try_write_bytes (c_object, sv.data(), sv.size(), SIZE_MAX);
}

size_t Buffer::TryWriteBytes (const char* src)
{
  return buffer_try_write_bytes (c_object, src, SIZE_MAX, SIZE_MAX);
}

size_t Buffer::ReadBytes (char * dest, size_t sizeof_dest, size_t bytes)
{
  return buffer_read_bytes (c_object, dest, sizeof_dest, bytes);
}

size_t Buffer::ReadBytes (std::span<char> dest, size_t bytes)
{
    return buffer_read_bytes (c_object, dest.data(), dest.size(), bytes);
}

size_t Buffer::ReadBytes (std::span<char> dest)
{
    return buffer_read_bytes (c_object, dest.data(), dest.size(), SIZE_MAX);
}

size_t Buffer::ReadBytes (std::string& str, size_t bytes)
{
    if (str.size() < bytes)
    {
      str.resize(bytes);
    }

    return buffer_read_bytes (c_object, str.data(), str.size(), bytes);
}

size_t Buffer::ReadBytes (std::string& str)
{
    return buffer_read_bytes (c_object, str.data(), str.size(), SIZE_MAX);
}

size_t Buffer::TryReadBytes (char * dest, size_t sizeof_dest, size_t bytes)
{
  return buffer_try_read_bytes (c_object, dest, sizeof_dest, bytes);
}

size_t Buffer::TryReadBytes (std::span<char> dest, size_t bytes)
{
    return buffer_try_read_bytes (c_object, dest.data(), dest.size(), bytes);
}

size_t Buffer::TryReadBytes (std::span<char> dest)
{
    return buffer_try_read_bytes (c_object, dest.data(), dest.size(), SIZE_MAX);
}

size_t Buffer::TryReadBytes (std::string& str, size_t bytes)
{
    if (str.size() < bytes)
    {
      str.resize(bytes);
    }

    return buffer_try_read_bytes (c_object, str.data(), str.size(), bytes);
}

size_t Buffer::TryReadBytes (std::string& str)
{
    return buffer_try_read_bytes (c_object, str.data(), str.size(), SIZE_MAX);
}

size_t Buffer::TryPeekBytes (char * dest, size_t sizeof_dest, size_t bytes)
{
  return buffer_try_peek_bytes (c_object, dest, sizeof_dest, bytes);
}

size_t Buffer::TryPeekBytes (std::span<char> dest, size_t bytes)
{
    return buffer_try_peek_bytes (c_object, dest.data(), dest.size(), bytes);
}

size_t Buffer::TryPeekBytes (std::span<char> dest)
{
    return buffer_try_peek_bytes (c_object, dest.data(), dest.size(), SIZE_MAX);
}

size_t Buffer::TryPeekBytes (std::string& str, size_t bytes)
{
    if (str.size() < bytes)
    {
      str.resize(bytes);
    }

    return buffer_try_peek_bytes (c_object, str.data(), str.size(), bytes);
}

size_t Buffer::TryPeekBytes (std::string& str)
{
    return buffer_try_peek_bytes (c_object, str.data(), str.size(), SIZE_MAX);
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

size_t Buffer::WriteCStr (std::span<const char> src, size_t characters)
{
  return buffer_write_c_str (c_object, src.data(), src.size(), characters);
}

size_t Buffer::WriteCStr (std::span<const char> src)
{
  return buffer_write_c_str (c_object, src.data(), src.size(), SIZE_MAX);
}

size_t Buffer::WriteCStr (std::string_view sv, size_t characters)
{
  return buffer_write_c_str (c_object, sv.data(), sv.size(), characters);
}

size_t Buffer::WriteCStr (std::string_view sv)
{
  return buffer_write_c_str (c_object, sv.data(), sv.size(), SIZE_MAX);
}

size_t Buffer::WriteCStr (const char* src)
{
  return buffer_write_c_str (c_object, src, SIZE_MAX, SIZE_MAX);
}

size_t Buffer::TryWriteCStr (const char *src, size_t sizeof_src, size_t characters)
{
  return buffer_try_write_c_str (c_object, src, sizeof_src, characters);
}

size_t Buffer::TryWriteCStr (std::span<const char> src, size_t characters)
{
  return buffer_try_write_c_str (c_object, src.data(), src.size(), characters);
}

size_t Buffer::TryWriteCStr (std::span<const char> src)
{
  return buffer_try_write_c_str (c_object, src.data(), src.size(), SIZE_MAX);
}

size_t Buffer::TryWriteCStr (std::string_view sv, size_t characters)
{
  return buffer_try_write_c_str (c_object, sv.data(), sv.size(), characters);
}

size_t Buffer::TryWriteCStr (std::string_view sv)
{
  return buffer_try_write_c_str (c_object, sv.data(), sv.size(), SIZE_MAX);
}

size_t Buffer::TryWriteCStr (const char * src)
{
  return buffer_try_write_c_str (c_object, src, SIZE_MAX, SIZE_MAX);
}

size_t Buffer::ReadCStr (char * dest, size_t sizeof_dest, size_t characters)
{
  return buffer_read_c_str (c_object, dest, sizeof_dest, characters);
}

size_t Buffer::ReadCStr (std::span<char> dest, size_t characters)
{
    return buffer_try_peek_bytes (c_object, dest.data(), dest.size(), characters);
}

size_t Buffer::ReadCStr (std::span<char> dest)
{
    return buffer_try_peek_bytes (c_object, dest.data(), dest.size(), SIZE_MAX);
}

size_t Buffer::ReadCStr (std::string& str, size_t characters)
{
    if (str.size() < characters)
    {
      str.resize(characters);
    }

    return buffer_try_peek_bytes (c_object, str.data(), str.size(), characters);
}

size_t Buffer::ReadCStr (std::string& str)
{
    return buffer_try_peek_bytes (c_object, str.data(), str.size(), SIZE_MAX);
}

size_t Buffer::TryReadCStr (char * dest, size_t sizeof_dest, size_t characters)
{
  return buffer_try_read_c_str (c_object, dest, sizeof_dest, characters);
}

size_t Buffer::TryReadCStr (std::span<char> dest, size_t characters)
{
    return buffer_try_read_c_str (c_object, dest.data(), dest.size(), characters);
}

size_t Buffer::TryReadCStr (std::span<char> dest)
{
    return buffer_try_read_c_str (c_object, dest.data(), dest.size(), SIZE_MAX);
}

size_t Buffer::TryReadCStr (std::string& str, size_t characters)
{
    if (str.size() < characters)
    {
      str.resize(characters);
    }

    return buffer_try_read_c_str (c_object, str.data(), str.size(), characters);
}

size_t Buffer::TryReadCStr (std::string& str)
{
    return buffer_try_read_c_str (c_object, str.data(), str.size(), SIZE_MAX);
}

size_t Buffer::TryPeekCStr (char * dest, size_t sizeof_dest, size_t characters)
{
  return buffer_try_peek_c_str (c_object, dest, sizeof_dest, characters);
}

size_t Buffer::TryPeekCStr (std::span<char> dest, size_t characters)
{
    return buffer_try_peek_c_str (c_object, dest.data(), dest.size(), characters);
}

size_t Buffer::TryPeekCStr (std::span<char> dest)
{
    return buffer_try_peek_c_str (c_object, dest.data(), dest.size(), SIZE_MAX);
}

size_t Buffer::TryPeekCStr (std::string& str, size_t characters)
{
    if (str.size() < characters)
    {
      str.resize(characters);
    }

    return buffer_try_peek_c_str (c_object, str.data(), str.size(), characters);
}

size_t Buffer::TryPeekCStr (std::string& str)
{
    return buffer_try_peek_c_str (c_object, str.data(), str.size(), SIZE_MAX);
}



size_t Buffer::ReadLine (char * dest, size_t sizeof_dest)
{
  return buffer_read_line (c_object, dest, sizeof_dest);
}

size_t Buffer::ReadLine (std::span<char> dest)
{
  return buffer_read_line (c_object, dest.data(), dest.size());
}

size_t Buffer::ReadLine (std::string& str)
{
  return buffer_read_line (c_object, str.data(), str.size());
}

ptrdiff_t Buffer::TryReadLine (char * dest, size_t sizeof_dest)
{
  return buffer_try_read_line (c_object, dest, sizeof_dest);
}

ptrdiff_t Buffer::TryReadLine (std::span<char> dest)
{
  return buffer_try_read_line (c_object, dest.data(), dest.size());
}

ptrdiff_t Buffer::TryReadLine (std::string& str)
{
  return buffer_try_read_line (c_object, str.data(), str.size());
}

size_t Buffer::ReadTo (char * dest, size_t sizeof_dest, const char * to, size_t to_length)
{
  return buffer_read_to (c_object, dest, sizeof_dest, to, to_length);
}

size_t Buffer::ReadTo (std::span<char> dest, std::span<char> to)
{
  return buffer_read_to (c_object, dest.data(), dest.size(), to.data(), to.size());
}

size_t Buffer::ReadTo (std::string& str, std::string& to)
{
  return buffer_read_to (c_object, str.data(), str.size(), to.data(), to.size());
}

ptrdiff_t Buffer::TryReadTo (char * dest, size_t sizeof_dest, const char * to, size_t to_length, buffer_try_read_to_t * data)
{
  return buffer_try_read_to (c_object, dest, sizeof_dest, to, to_length, data);
}

ptrdiff_t Buffer::TryReadTo (std::span<char> dest, std::span<char> to, buffer_try_read_to_t * data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data);
}

ptrdiff_t Buffer::TryReadTo (std::span<char> dest, std::span<char> to, BufferTryReadTo& data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data.c_object);
}

ptrdiff_t Buffer::TryReadTo (std::span<char> dest, std::string_view to, buffer_try_read_to_t * data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data);
}

ptrdiff_t Buffer::TryReadTo (std::span<char> dest, std::string_view to, BufferTryReadTo& data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data.c_object);
}

ptrdiff_t Buffer::TryReadTo (std::string& dest, std::span<char> to, buffer_try_read_to_t * data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data);
}

ptrdiff_t Buffer::TryReadTo (std::string& dest, std::span<char> to, BufferTryReadTo& data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data.c_object);
}

ptrdiff_t Buffer::TryReadTo (std::string& dest, std::string_view to, buffer_try_read_to_t * data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data);
}

ptrdiff_t Buffer::TryReadTo (std::string& dest, std::string_view to, BufferTryReadTo& data)
{
  return buffer_try_read_to (c_object, dest.data(), dest.size(), to.data(), to.size(), data.c_object);
}



void Buffer::Clear ()
{
  buffer_clear (c_object);
}



BufferTryReadTo::BufferTryReadTo ()
{
  c_object = &objectData;
  buffer_try_read_to_init(&objectData);
}


// ------------------------------------------------------------------------- //
//  eof
// ------------------------------------------------------------------------- //
