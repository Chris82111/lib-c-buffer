//! @file
//! @brief The buffer source file.

/*---------------------------------------------------------------------*
 *  private: include files
 *---------------------------------------------------------------------*/

#include "buffer.h"

#include <string.h> // memcmp
#include <stdlib.h> // malloc, free


/*---------------------------------------------------------------------*
 *  private: definitions
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: typedefs
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: variables
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  public:  variables
 *---------------------------------------------------------------------*/

const struct buffer_sc buffer =
{
    buffer_clear,
    buffer_copy,
    buffer_equal,
    buffer_get,
    buffer_get_available_or_null,
    buffer_init,
    buffer_is_empty,
    buffer_is_full,
    buffer_is_stopped,
    buffer_length,
    buffer_lines,
    buffer_look_available_or_null,
    buffer_object_allocate,
    buffer_object_free,
    buffer_read,
    buffer_read_line,
    buffer_read_to,
    buffer_reset,
    buffer_set,
    buffer_set_possible_or_skip,
    buffer_space,
    buffer_start,
    buffer_stop_force,
    buffer_stop_try,
    buffer_write,
};


/*---------------------------------------------------------------------*
 *  private: function prototypes
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  private: functions
 *---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*
 *  public:  functions
 *---------------------------------------------------------------------*/

bool buffer_clear(buffer_t * object)
{
    if(NULL == object){ return false; }

    atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL);

    bool cleared = true;

    if(0 < atomic_load(&object->length))
    {
        cleared = false;

        char * producer_ptr = (char *)atomic_load(&(object->producer_ptr));

        size_t length = atomic_load(&object->length);

        size_t lines = atomic_load(&object->lines);

        if (atomic_compare_exchange_strong(&(object->producer_ptr), &producer_ptr, object->data))
        {
            object->consumer_ptr = object->data;

            atomic_fetch_sub(&object->length, length);

            atomic_fetch_sub(&object->lines, lines);

#ifdef BUFFER_ENABLE_HANDLER
            if(object->on_empty) { object->on_empty(object); }
#endif

            cleared = true;
        }
    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL);

    return cleared;
}

#ifdef BUFFER_COPY_FIELD
#error BUFFER_COPY_FIELD must not be redefined
#endif

#ifdef BUFFER_COPY_ATOMIC
#error BUFFER_COPY_ATOMIC must not be redefined
#endif

//! @brief For simple copying of elements
#define BUFFER_COPY_FIELD( OBJ, DEST, FIELD_NAME) (((DEST)->FIELD_NAME) = ((OBJ)->FIELD_NAME))

//! @brief For simple copying of atomic elements
#define BUFFER_COPY_ATOMIC(OBJ, DEST, FIELD_NAME) (atomic_store((&(DEST)->FIELD_NAME), atomic_load((&(OBJ)->FIELD_NAME))))

void buffer_copy(const buffer_t * object, buffer_t * dest)
{
    if((NULL == object) || (NULL == dest)){ return; }

    BUFFER_COPY_FIELD(object, dest, data);
    BUFFER_COPY_FIELD(object, dest, last);
    BUFFER_COPY_FIELD(object, dest, end_of_line_character);
#ifdef BUFFER_ENABLE_HANDLER
    BUFFER_COPY_FIELD(object, dest, on_start);
    BUFFER_COPY_FIELD(object, dest, on_stop);
    BUFFER_COPY_FIELD(object, dest, on_full);
    BUFFER_COPY_FIELD(object, dest, on_empty);
    BUFFER_COPY_FIELD(object, dest, on_new_character);
    BUFFER_COPY_FIELD(object, dest, on_new_line);
    BUFFER_COPY_FIELD(object, dest, on_error);
    BUFFER_COPY_FIELD(object, dest, on_wait_set);
    BUFFER_COPY_FIELD(object, dest, on_wait_get);
#endif
    BUFFER_COPY_FIELD(object, dest, consumer_ptr);
    BUFFER_COPY_ATOMIC(object, dest, producer_ptr);
    BUFFER_COPY_ATOMIC(object, dest, length);
    BUFFER_COPY_ATOMIC(object, dest, lines);
    BUFFER_COPY_ATOMIC(object, dest, state);

}

#undef BUFFER_COPY_FIELD
#undef BUFFER_COPY_ATOMIC


#ifdef BUFFER_COMPARE_FIELD
#error BUFFER_COMPARE_FIELD must not be redefined
#endif

#ifdef BUFFER_COMPARE_ATOMIC
#error BUFFER_COMPARE_ATOMIC must not be redefined
#endif

//! @brief For simple comparison of elements
#define BUFFER_COMPARE_FIELD( O1, O2, FIELD_NAME) (((O1)->FIELD_NAME) == ((O2)->FIELD_NAME))

//! @brief For simple comparison of atomic elements
#define BUFFER_COMPARE_ATOMIC(O1, O2, FIELD_NAME) (atomic_load(&((O1)->FIELD_NAME)) == atomic_load(&((O2)->FIELD_NAME)))

bool buffer_equal(const buffer_t * object, const buffer_t * object2)
{
    if((NULL == object) || (NULL == object2)){ return false; }

    return
        BUFFER_COMPARE_FIELD(object, object2, data) &&
        BUFFER_COMPARE_FIELD(object, object2, last) &&
        BUFFER_COMPARE_FIELD(object, object2, end_of_line_character) &&
#ifdef BUFFER_ENABLE_HANDLER
        BUFFER_COMPARE_FIELD(object, object2, on_start) &&
        BUFFER_COMPARE_FIELD(object, object2, on_stop) &&
        BUFFER_COMPARE_FIELD(object, object2, on_full) &&
        BUFFER_COMPARE_FIELD(object, object2, on_empty) &&
        BUFFER_COMPARE_FIELD(object, object2, on_new_character) &&
        BUFFER_COMPARE_FIELD(object, object2, on_new_line) &&
        BUFFER_COMPARE_FIELD(object, object2, on_error) &&
        BUFFER_COMPARE_FIELD(object, object2, on_wait_set) &&
        BUFFER_COMPARE_FIELD(object, object2, on_wait_get) &&
#endif
        BUFFER_COMPARE_FIELD(object, object2, consumer_ptr) &&
        BUFFER_COMPARE_ATOMIC(object, object2, producer_ptr) &&
        BUFFER_COMPARE_ATOMIC(object, object2, length) &&
        BUFFER_COMPARE_ATOMIC(object, object2, lines) &&
        BUFFER_COMPARE_ATOMIC(object, object2, state);
}

#undef BUFFER_COMPARE_FIELD
#undef BUFFER_COMPARE_ATOMIC

char buffer_get(buffer_t * object)
{
    if(NULL == object) { return 0; }

    char c = 0;

    if(BUFFER_FLAGS_IDLE <= atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_GET))
    {
        volatile _Atomic(size_t) * length = &object->length;

        char * ptr;

        while(true)
        {
            while(true)
            {
                if( atomic_load(length) )
                {
                    break;
                }

#ifdef BUFFER_ENABLE_HANDLER
                if(object->on_wait_get)
                {
                    if(object->on_wait_get(object))
                    {
                        // Function was canceled by the handler function
                        atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET);
                        return 0;
                    }
                }
#endif

                if(BUFFER_FLAGS_IDLE >= atomic_load(&object->state))
                {
                    // Function was canceled by flag
                    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET);
                    return 0;
                }
            }

            ptr = object->consumer_ptr;

            if(ptr <= object->last)
            {
                c = *ptr;

                ptr += 1;

                object->consumer_ptr = ptr;

                atomic_fetch_sub(&object->length, 1);

                if (object->end_of_line_character == c)
                {
                    atomic_fetch_sub(&object->lines, 1);
                }
            }
            else
            {

#ifdef BUFFER_ENABLE_HANDLER
                if(object->on_error) { object->on_error(object); }
#endif

                // Internal error, the function is canceled
                atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET);
                return 0;
            }

            break; // Character has been read and the function can be ended.
        }

        // An attempt is made to reset the buffer
        if (atomic_compare_exchange_strong(&(object->producer_ptr), &ptr, object->data))
        {
            object->consumer_ptr = object->data;

#ifdef BUFFER_ENABLE_HANDLER
            if(object->on_empty) { object->on_empty(object); }
#endif
        }
    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET);
    return c;
}

char buffer_get_available_or_null(buffer_t * object)
{
    if(NULL == object) { return 0; }

    char c = 0;

    if(BUFFER_FLAGS_IDLE <= atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL))
    {
        if(0 < atomic_load(&object->length))
        {
            char * ptr = (char *)object->consumer_ptr;

            if(ptr <= object->last)
            {
                c = *ptr;

                ptr += 1;

                object->consumer_ptr = ptr;

                atomic_fetch_sub(&object->length, 1);

                if (object->end_of_line_character == c)
                {
                   atomic_fetch_sub(&object->lines, 1);
                }

                // An attempt is made to reset the buffer
                if (atomic_compare_exchange_strong(&(object->producer_ptr), &ptr, object->data))
                {
                    object->consumer_ptr = object->data;

#ifdef BUFFER_ENABLE_HANDLER
                    if(object->on_empty) { object->on_empty(object); }
#endif
                }
            }
            else
            {
#ifdef BUFFER_ENABLE_HANDLER
                if(object->on_error) { object->on_error(object); }
#endif
            }
        }
    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL);
    return c;
}

bool buffer_init(buffer_t * object, char * data, size_t sizeof_data, bool start)
{
    if(NULL == object){ return false; }

    bool stopped = (BUFFER_FLAGS_STOP == atomic_fetch_and(&object->state, ~BUFFER_FLAGS_IDLE));

    if((NULL == data) || (0 == sizeof_data))
    {
        data = NULL;
        object->data = NULL;
        object->last = NULL;
    }
    else
    {
        object->data = data;
        object->last = data + sizeof_data - 1;
    }

    object->end_of_line_character = '\n';

#ifdef BUFFER_ENABLE_HANDLER
    object->on_start = NULL;
    object->on_stop = NULL;
    object->on_full = NULL;
    object->on_empty = NULL;
    object->on_new_character = NULL;
    object->on_new_line = NULL;
    object->on_error = NULL;
    object->on_wait_set = NULL;
    object->on_wait_get = NULL;
#endif

    object->consumer_ptr = data;

    atomic_init(&object->producer_ptr, data);
    atomic_init(&object->length, 0);
    atomic_init(&object->lines, 0);
    atomic_init(&object->state, 0);

    if((NULL != data) && start)
    {
        buffer_start(object);
    }

    return stopped;
}

bool buffer_is_empty(const buffer_t * object)
{
    if(NULL == object){ return false; }

    return (char *)(atomic_load(&object->producer_ptr)) == object->data;
}

bool buffer_is_full(const buffer_t * object)
{
    if(NULL == object){ return false; }

    return (char *)(atomic_load(&object->producer_ptr)) > object->last;
}

bool buffer_is_stopped(const buffer_t * object)
{
    if(NULL == object){ return false; }

    return BUFFER_FLAGS_STOP == atomic_load(&object->state);
}

size_t buffer_length(const buffer_t * object)
{
    if(NULL == object){ return 0; }

    return (size_t)atomic_load(&object->length);
}

size_t buffer_lines(const buffer_t * object)
{
    if(NULL == object){ return 0; }

    return (size_t)atomic_load(&object->lines);
}

char buffer_look_available_or_null(buffer_t * object)
{
    if(NULL == object) { return 0; }

    char c = 0;

    if(BUFFER_FLAGS_IDLE <= atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL))
    {
        if(0 < atomic_load(&object->length))
        {
            char * ptr = object->consumer_ptr;

            if(ptr <= object->last)
            {
                c = *ptr;
            }
        }
    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL);
    return c;
}

buffer_t * buffer_object_allocate(char * data, size_t sizeof_data, bool start)
{
    buffer_t * object;

    if(NULL != data || 0 == sizeof_data)
    {
        object = (buffer_t *)malloc(sizeof(buffer_t));
    }
    else
    {
        object = (buffer_t *)malloc(sizeof(buffer_t) + sizeof_data);
        data = ((char *)object) + sizeof(buffer_t);
    }

    buffer_init(object, data, sizeof_data, start);

    return object;
}

bool buffer_object_free(buffer_t * object)
{
    if(NULL == object) { return false; }

    bool stopped = buffer_stop_force(object);

    free(object);

    return stopped;
}

size_t buffer_read(buffer_t * object, char * dest, size_t n)
{
    if((NULL == object) || (NULL == dest)) { return 0; }

    --n;
    char c;
    size_t i;
    for(i = 0; i < n; dest++, i++)
    {
        c = buffer_get_available_or_null(object);

        if('\0' == c)
        {
            break;
        }

        *dest = c;
    }
    *dest = '\0';

    return i;
}

size_t buffer_read_line(buffer_t * object, char * dest, size_t n)
{
    if((NULL == object) ||
       (0 == atomic_load(&object->lines)) ||
       (NULL == dest))
    {
        return 0;
    }

    --n;
    char c;
    char eol = object->end_of_line_character;
    size_t i;
    for(i = 0; i < n; dest++, i++)
    {
        c = buffer_get_available_or_null(object);

        if(('\0' == c) || (eol == c))
        {
            break;
        }

        *dest = c;
    }
    *dest = '\0';

    return i;
}

size_t buffer_read_to(buffer_t * object, char * dest, size_t n, const char * to, size_t to_length)
{
    if((NULL == object) || (NULL == dest) || (NULL == to)) { return 0; }

    size_t length = (size_t)atomic_load(&object->length);

    if(length < to_length) { return 0; }

    if(BUFFER_FLAGS_IDLE <= atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL))
    {
        char * data = atomic_load(&(object->consumer_ptr));

        size_t data_length;
        for(size_t i = 0; (to_length + i) <= length; i++)
        {
            data_length = length - i;
            if(to_length < data_length)
            {
                data_length = to_length;
            }

            if(0 == memcmp(data+i, to, data_length))
            {
                --n;
                size_t j;
                for(j = 0; (j < i) && (i < n); dest++, j++)
                {
                    *dest = buffer_get_available_or_null(object);
                }
                *dest = '\0';

                for(j = 0; (j < to_length); j++)
                {
                    buffer_get_available_or_null(object);
                }

                atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL);
                return i;
            }
        }

    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_GET_AVAILABLE_OR_NULL);
    return 0;
}

bool buffer_reset(buffer_t * object, bool start)
{
    if(NULL == object){ return false; }

    bool stopped = (BUFFER_FLAGS_STOP == atomic_fetch_and(&object->state, ~BUFFER_FLAGS_IDLE));

    object->end_of_line_character = '\n';

#ifdef BUFFER_ENABLE_HANDLER
    object->on_start = NULL;
    object->on_stop = NULL;
    object->on_full = NULL;
    object->on_empty = NULL;
    object->on_new_character = NULL;
    object->on_new_line = NULL;
    object->on_error = NULL;
    object->on_wait_set = NULL;
    object->on_wait_get = NULL;
#endif

    object->consumer_ptr = object->data;

    atomic_init(&object->producer_ptr, object->data);
    atomic_init(&object->length, 0);
    atomic_init(&object->lines, 0);
    atomic_init(&object->state, 0);

    if((NULL != object->data) && start)
    {
        buffer_start(object);
    }

    return stopped;
}

bool buffer_set(buffer_t * object, char c)
{
    if(NULL == object) { return false; }

    bool saved = false;

    if(BUFFER_FLAGS_IDLE <= atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_SET))
    {
        while(true)
        {
            // the get function can change the position but only to a smaller position the start position
            if((char *)atomic_load(&object->producer_ptr) <= object->last)
            {
                *(char *)atomic_fetch_add(&object->producer_ptr, 1) = c;

                if (object->end_of_line_character == c)
                {
                    atomic_fetch_add(&object->lines, 1);
                }

                atomic_fetch_add(&object->length, 1);

#ifdef BUFFER_ENABLE_HANDLER
                if(object->on_new_character) { object->on_new_character(object, c); }

                if(object->end_of_line_character == c)
                {
                    if(object->on_new_line) { object->on_new_line(object); }
                }
#endif

                saved = true;
            }
            else
            {
#ifdef BUFFER_ENABLE_HANDLER
                if(object->on_full) { object->on_full(object, c); }

                if(object->on_wait_set)
                {
                    if(object->on_wait_set(object))
                    {
                        break; // Function was canceled by the handler function
                    }
                }
#endif

                if(BUFFER_FLAGS_IDLE >= atomic_load(&object->state))
                {
                    break; // Function was canceled by flag
                }

                continue; // The function waits until a character has been saved.
            }

            break; // Character has been saved and the function can be ended.
        }
    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_SET);
    return saved;
}

bool buffer_set_possible_or_skip(buffer_t * object, char c)
{
    if(NULL == object) { return false; }

    bool saved = false;

    if(BUFFER_FLAGS_IDLE <= atomic_fetch_add(&object->state, BUFFER_FLAGS_RUNNING_SET_POSSIBLE_OR_SKIP))
    {
        // the get function can change the position but only to a smaller position the start position
        if((char *)atomic_load(&object->producer_ptr) <= object->last)
        {
            *(char *)atomic_fetch_add(&object->producer_ptr, 1) = c;

            if (object->end_of_line_character == c)
            {
                atomic_fetch_add(&object->lines, 1);
            }

            atomic_fetch_add(&object->length, 1);

#ifdef BUFFER_ENABLE_HANDLER

            if(object->on_new_character) { object->on_new_character(object, c); }

            if(object->end_of_line_character == c)
            {
                if(object->on_new_line) { object->on_new_line(object); }
            }
#endif

            saved = true;
        }
        else
        {
#ifdef BUFFER_ENABLE_HANDLER
            if(object->on_full) { object->on_full(object, c); }
#endif
        }
    }

    atomic_fetch_sub(&object->state, BUFFER_FLAGS_RUNNING_SET_POSSIBLE_OR_SKIP);
    return saved;
}

size_t buffer_space(const buffer_t * object)
{
    if(NULL == object){ return 0; }

	char * producer_ptr = (char *)atomic_load(&object->producer_ptr);
	if(object->last < producer_ptr)
	{
		return 0;
	}
	else
	{
		return (size_t)(object->last + 1 - producer_ptr);
	}
}

bool buffer_start(buffer_t * object)
{
    if(NULL == object){ return false; }

    if(NULL != object->data)
    {
        atomic_fetch_or(&object->state, BUFFER_FLAGS_IDLE);

#ifdef BUFFER_ENABLE_HANDLER
        if(object->on_start) { object->on_start(object); }
#endif

        return true;
    }
    return false;
}

bool buffer_stop_force(buffer_t * object)
{
    if(NULL == object){ return false; }

    buffer_falgs_t state = (buffer_falgs_t)atomic_fetch_and(&object->state, ~BUFFER_FLAGS_IDLE);

    if(BUFFER_FLAGS_STOP == state)
    {
#ifdef BUFFER_ENABLE_HANDLER
        if(object->on_stop) { object->on_stop(object); }
#endif

        return true;
    }

    return false;
}

bool buffer_stop_try(buffer_t * object)
{
    if(NULL == object){ return false; }

    buffer_falgs_t state = (buffer_falgs_t)atomic_load(&object->state);
    if(BUFFER_FLAGS_STOP == state)
    {
#ifdef BUFFER_ENABLE_HANDLER
        if(object->on_stop) { object->on_stop(object); }
#endif

        return true;
    }
    if(BUFFER_FLAGS_IDLE == state)
    {
        if (atomic_compare_exchange_strong(&(object->state), (_Atomic(unsigned char) *)&state, BUFFER_FLAGS_STOP))
        {
#ifdef BUFFER_ENABLE_HANDLER
            if(object->on_stop) { object->on_stop(object); }
#endif

            return true;
        }
    }
    return false;
}

size_t buffer_write(buffer_t * object, const char *src, size_t n)
{
    if((NULL == object) || (NULL == src)) { return 0; }

    char c;
    size_t i;
    for(i = 0; ((c = *src) != '\0') && i < n; src++, i++)
    {
        if(false == buffer_set_possible_or_skip(object, c))
        {
            break;
        }
    }

    return i;
}

/*---------------------------------------------------------------------*
 *  eof
 *---------------------------------------------------------------------*/
