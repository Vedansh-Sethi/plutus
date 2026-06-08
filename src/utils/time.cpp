#include "time.hpp"
#include <chrono>

inline auto getCurrentNanos() noexcept
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

inline auto& getCurrentTimeStr(std::string *time_str) noexcept
{
    const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    time_str->assign(ctime(&time));
    if (!time_str->empty())
    {
        time_str->at(time_str->length() - 1) = '\0';
    }
    return *time_str;
}