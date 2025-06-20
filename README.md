# lib-c-buffer

This C library provides a conveniently usable `buffer_t` type for managing a `char` array as an FIFO buffer. It enables simple filling and emptying - without the typical complexities of conventional synchronization. The library is written in pure C and provides a compatible header file for use in C and C++ projects - bidirectional and flexible.

The library was developed efficiently, but without a focus on speed, the focus is a lock-free implementation and the effective handling of parallel and concurrent tasks. As a result, it is highly versatile and suitable for use across a wide range of platforms, from embedded systems to high-performance computers.

<picture>
  <source
    media="(prefers-color-scheme: dark)"
    srcset="./docs/overview_dark.svg" />
  <img
    alt="Type of buffer"
    src="./docs/overview.svg"
    width="220" />
</picture>

- Thread/interrupt-Safe & Lock-Free:
Supports parallel and concurrent execution through two dedicated tasks - one reader and one writer - without the use of semaphores or memory fences.
- Atomic operations:
Synchronization is achieved exclusively through modern, atomic operations - for maximum efficiency and portability. `_Atomic(T)` in C and `std::atomic<T>` in C++.

<picture>
  <source
    media="(prefers-color-scheme: dark)"
    srcset="./docs/thread_dark.svg" />
  <img
    alt="Different types of tasks"
    src="./docs/thread.svg"
    width="180" />
</picture>

## Field of Application

**<ins>Pro</ins>**: The core structure of the library is such that a single character is always written atomically and read atomically. With each character, the entire data structure is updated atomically - with the advantage of being able to dispense with blocking. This enables a data pipeline with the shortest possible stage time. Each write can be immediately followed by a read.  

**<ins>Con</ins>**: The advantage comes at the cost of a large overhead per character. If you need to write large amounts of data quickly, this library is not suitable. A block, fill and release approach would be more suitable.  

**<ins>Use</ins>**: The library is ideal for sending and receiving `uint8_t`/`char` arrays or strings such as those used with UART (RS232, RS485) or SPI.

## Examples

The following examples show how you can use the buffer.

```C
void example_handler(buffer_t * object)
{
    char buf[10];

    buffer.ReadLine(object, buf, sizeof(buf));

    printf("Out: %s\n", buf);
    fflush(stdout);
}

void example_1(void)
{
    char buf[10];
    buffer_t obj;

    buffer.Init(&obj, buf, sizeof(buf), true);
    obj.new_line = example_handler;

    buffer.Write(&obj, "Hi you", 2);
    buffer.Set(&obj, '\n');
}

void example_2(void)
{
    buffer_t * p = buffer.ObjectAllocate(NULL, 10, true);
    if(NULL != p)
    {
        p->new_line = example_handler;
        buffer.Write(p, "Hi you", 2);
        buffer.Set(p, '\n');
    }
    buffer.ObjectFree(p);
}
```

## Example UART

This is an example of how the code can be used on an embedded system.

```C
char uart_buf[10];
buffer_t uart = BUFFER_INIT(
    uart_buf, sizeof(uart_buf), false);

void example_3_init(void)
{
    buffer.Start(&uart);
    uart.end_of_line_character = '\r';
}

void example_3_receive(void)
{
    buffer.SetPossibleOrSkip(&uart, 'H');
    buffer.SetPossibleOrSkip(&uart, 'i');
    buffer.SetPossibleOrSkip(&uart, '\r');
}

void example_3_main_loop(void)
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
```
