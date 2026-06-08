#include <atomic>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <thread>

inline bool setThreadCore(int core_id) noexcept
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
}

template <typename T, typename... A>
inline std::thread *createAndStartThread(int core_id, const std::string &name, T &&func,
                                 A &&...args) noexcept
{
    std::atomic<bool> thread_running(false), thread_failed(false);
    auto body = [&]
    {
        if (core_id >= 0 && !setThreadCore(core_id))
        {
            std::cerr << "Failed to set core affinity for " << name << " "
                      << pthread_self() << " to " << core_id << std::endl;
            thread_failed = true;
            return;
        }
        std::cout << "Set core affinity for  " << name << " " << pthread_self()
                  << " to " << core_id << std::endl;
        thread_running = true;
        std::forward<T>(func)((std::forward<A>(args))...);
    };
    auto t = new std::thread(body);
    while(!thread_running && !thread_failed)
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    if (thread_failed)
    {
        t->join();
        delete t;
        t = nullptr;
    }
    return t;
}