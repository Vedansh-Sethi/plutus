#include "logger.hpp"

void Logger::flushQueue() noexcept
{
    while (logger_running_)
    {
        for (auto next = q_.getNextToRead(); q_.size() && next;
             next = q_.getNextToRead())
        {
            switch (next->type_)
            {
            case LogType::CHAR:
                file_ << next->u_.c;
                break;
            case LogType::DOUBLE:
                file_ << next->u_.d;
                break;
            case LogType::FLOAT:
                file_ << next->u_.f;
                break;
            case LogType::INTEGER:
                file_ << next->u_.i;
                break;
            case LogType::LONG_INTEGER:
                file_ << next->u_.l;
                break;
            case LogType::UNSIGNED_INTEGER:
                file_ << next->u_.u;
                break;
            case LogType::UNSIGNED_LONG_INTEGER:
                file_ << next->u_.ul;
                break;
            case LogType::UNSIGNED_LONG_LONG_INTEGER:
                file_ << next->u_.ull;
                break;
            case LogType::LONG_LONG_INTEGER:
                file_ << next->u_.ll;
                break;
            }
            q_.updateReadIdx();
        }
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}

void Logger::pushValue(const LogElement &log_elem) noexcept
{
    *(q_.getNextToWrite()) = log_elem;
    q_.updateWriteIdx();
}

void Logger::pushValue(const char val) noexcept
{
    pushValue(LogElement{LogType::CHAR, {val}});
}

void Logger::pushValue(const char *str) noexcept
{
    while (*str)
    {
        pushValue(*str);
        str++;
    }
}

void Logger::pushValue(const std::string &val) noexcept
{
    pushValue(val.c_str());
}

void Logger::pushValue(const int &val) noexcept
{
    pushValue(LogElement{LogType::INTEGER, {.i = val}});
}

void Logger::pushValue(const long &val) noexcept
{
    pushValue(LogElement{LogType::LONG_INTEGER, {.l = val}});
}

void Logger::pushValue(const long long &val) noexcept
{
    pushValue(LogElement{LogType::LONG_LONG_INTEGER, {.ll = val}});
}

void Logger::pushValue(const unsigned &val) noexcept
{
    pushValue(LogElement{LogType::UNSIGNED_INTEGER, {.u = val}});
}

void Logger::pushValue(const unsigned long &val) noexcept
{
    pushValue(LogElement{LogType::UNSIGNED_LONG_INTEGER, {.ul = val}});
}

void Logger::pushValue(const unsigned long long &val) noexcept
{
    pushValue(LogElement{LogType::UNSIGNED_LONG_LONG_INTEGER, {.ull = val}});
}

void Logger::pushValue(const float &val) noexcept
{
    pushValue(LogElement{LogType::FLOAT, {.f = val}});
}

void Logger::pushValue(const double &val) noexcept
{
    pushValue(LogElement{LogType::DOUBLE, {.d = val}});
}

template <typename T, typename... A>
void Logger::log(const char *s, const T &val, A... args) noexcept
{
    while (*s)
    {
        if (*s == '%')
        {
            if (*(s + 1) == '%') [[unlikely]]
            {
                s++;
            }
            else
            {
                pushValue(val);
                log(s + 1, args...);
                return;
            }
        }
        pushValue(*s++);
    }
    FATAL("extra arguments provided to log()");
}

void Logger::log(const char *s) noexcept
{
    while (*s)
    {
        if (*s == '%')
        {
            if (*(s + 1) == '%') [[unlikely]]
                s++;
            else
                FATAL("missing arguments to log()");
        }
        pushValue(*s++);
    }
}
