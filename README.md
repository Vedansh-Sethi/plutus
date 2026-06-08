# Plutus

## An electronic trading ecosystem inspired from the book by sourav ghosh

- The main goal of doing this projoect is to understand the intricacies of low latency system in a hands on manner

### Utils

#### Thread -

- `startAndCreateThread` - uses variadic template arguments (using templates which give the function variable number of arguments) and universal reference (forwarding reference, T &&) to pass a function and its arguments to the thread using perfect forwarding, it is made to control the thread will be created on which core and also stop the unwanted creation of data copies

- `setThreadCore` - This function sets the affinity of the thread to a certain core, it does so by manipulating the bitmask cpu_set_t to only allow the specific core ID for the thread by using CPU_SET, and finally sets the affinity using POSIX C API.

