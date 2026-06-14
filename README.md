# lib-c-buffer

<div align="center">

  [![C](https://img.shields.io/badge/language-C_11-a9bacd)](https://www.c-language.org/ "Link to web page")
  [![C++](https://img.shields.io/badge/language-C%2B%2B_23-649ad2)](https://isocpp.org/ "Link to web page") \
  [![Visual Studio Code](https://img.shields.io/badge/IDE-Visual%20Studio%20Code-0065a9)](https://code.visualstudio.com/ "Link to web page")
  [![Eclipse](https://img.shields.io/badge/IDE-Eclipse-3f3179)](https://www.eclipse.org/ "Link to web page")

</div>

This C library provides a conveniently usable `buffer_t` type for managing a `char` array as an FIFO buffer. It enables simple filling and emptying - without the typical complexities of conventional synchronization. The library is written in pure C and provides a compatible header file for use in C and C++ projects - bidirectional and flexible.

<picture>
  <source
    media="(prefers-color-scheme: dark)"
    srcset="./docs/buffer_dark.svg" />
  <img
    alt="Function of the buffer"
    src="./docs/buffer.svg"
    width=300" />
</picture>

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
Supports parallel and concurrent execution through dedicated tasks - readers and writers - without the use of semaphores or memory fences.
- Atomic operations:
Synchronization is achieved exclusively through modern, atomic operations - for maximum efficiency and portability. `_Atomic(T)` in C and C++.

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
void example_1 (void)
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
```

## Example UART

This is an example of how the code can be used on an embedded system.
The sample program runs on any standard computer; since it lacks interrupts,
as is the case with embedded systems, threads were used here.

```C
char uart_buf[10 + 1];
buffer_t uart = BUFFER_INIT(uart_buf, sizeof(uart_buf));

void example_2_init(void)
{
    uart.end_of_line_character = '\r';
}

void * example_2_receive(void *arg)
{
  sleep(1);
  buffer.TrySet(&uart, 'H');

  sleep(1);
  buffer.TrySet(&uart, 'i');

  sleep(1);
  buffer.TrySet(&uart, '\r');

  return arg;
}

void example_2_uart(void)
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
```
