# bounded_buffer_problem
Implementation of the classic bounded buffer synchronization problem

## Description
Takes a size and a filename from the user. "size" refers to the number of elements that can be put into the buffer at once. "filename" refers to the file that the program should read from. Creates a shared buffer and a producer and consumer thread. The producer reads the next line from the file and writes it to the shared buffer. The consumer reads the next element out of the buffer (lifo) and writes it to stdout.

## Getting Started

### Dependencies

* To be run on a x86-64 Linux machine

### Executing program

* User must create some file in the current directory that they want to read from
* Each entry in the file will represent a string which will be put in the buffer
* The executable can then be executed as: ./[whatever you named it] [size] [filename]

## Authors

* Stefan Silverio
* stefansilverio@gmail.com
* https://twitter.com/stefansilverio

