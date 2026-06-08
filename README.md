# Plutus

## An electronic trading ecosystem inspired from the book by sourav ghosh

- The main goal of doing this projoect is to understand the intricacies of low latency system in a hands on manner

### Utils

#### Thread -

- Defining methods to create threads so that the core to which a thread is affined to is controllable by the user, this will help in using the L1/L2 cache to the maximum

- `startAndCreateThread` : uses variadic template arguments (using templates which give the function variable number of arguments) and universal reference (forwarding reference, T &&) to pass a function and its arguments to the thread using perfect forwarding, it is made to control the thread will be created on which core and also stop the unwanted creation of data copies

- `setThreadCore` : This function sets the affinity of the thread to a certain core, it does so by manipulating the bitmask cpu_set_t to only allow the specific core ID for the thread by using CPU_SET, and finally sets the affinity using POSIX C API.

#### Memory Pool -

- It is used to avoid repeatedly allocating memory dynamically, which slows down the execution of the program, making memory pools can prove not to be thread-safe, as this could lead to race conditiions

- `MemPool Constructor` : The memory pool constructor does initalizes the storage vector using a struct defined to contain both object and the occupancy marker of the block, and it fills the memory pool with empty template objects and checks if it worked using ASSERT before using the memory pool.

- `allocate` : This function allocates memory to a new object in our memory pool, this used a technique called **placement new**, which passe a pointer to the new operator, which tells it to construct the object and start it on the given pointer's address. This bypasses the OS.

- `deallocate` : Simply marks the index at which the passed element is as free in the memory pool storage, it will already get over-written using the allocate method when we will need to allocate an object.

#### Lock Free Queue -

- A lock free queue (LFQ) is a data structure used to access data in an environment where multiple threads may be accessing the same data at the same time. Generally, to ensure data consistency across threads, we use locks and mutexes on queue data structure, but locks and mutexes are ineffecient, leading to context switches, therefore we use lock free queues, custom data structures to ensure thread safety without the use of locks.

- Why Queue and not any other data structure? because the data we are going to store in the queue must be processed chronologically, so the writer thread (eg. Network thread) is always pointing to the end of the queue and the reader thread (eg. Strategy thread) is always pointing at the start of the queue

#### Logger -

- A logging system is necessary to track events happening in the program, writing to a log file is slow as it deals directly with the disk I/O, so we create a separate thread that will log events happening in our program.

- All the other threads will write to the LFQ of logging thread and the logging thread will read the data and do the work of disk I/O, this will cause some delay in logging but is necessary for avoiding interruptions to the hot path.