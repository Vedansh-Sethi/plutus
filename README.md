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