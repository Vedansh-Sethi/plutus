#pragma once
#include "lfq.hpp"
#include "thread.hpp"
#include <fstream>

constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;

enum LogType : int8_t
{
    CHAR = 0,
    INTEGER = 1,
    LONG_INTEGER = 2,
    LONG_LONG_INTEGER = 3,
    UNSIGNED_INTEGER = 4,
    UNSIGNED_LONG_INTEGER = 5,
    UNSIGNED_LONG_LONG_INTEGER = 6,
    FLOAT = 7,
    DOUBLE = 8
};

struct LogElement
{
    LogType type_ = LogType::CHAR;
    union
    {
        char c;
        int i;
        long l;
        long long ll;
        unsigned u;
        unsigned long ul;
        unsigned long long ull;
        float f;
        double d;
    } u_;
};

class Logger final
{
    const std::string file_name_;
    std::ofstream file_;
    LFQueue<LogElement> q_;
    std::atomic<bool> logger_running_ = true;
    std::thread *logger_thread_ = nullptr;

    void pushValue(const LogElement &) noexcept;
    void pushValue(const char) noexcept;
    void pushValue(const char *) noexcept;
    void pushValue(const std::string &) noexcept;
    void pushValue(const int &) noexcept;
    void pushValue(const long &) noexcept;
    void pushValue(const long long &) noexcept;
    void pushValue(const unsigned &) noexcept;
    void pushValue(const unsigned long &) noexcept;
    void pushValue(const unsigned long long &) noexcept;
    void pushValue(const double &) noexcept;
    void pushValue(const float &) noexcept;

  public:
    explicit Logger(const std::string file_name)
        : file_name_(file_name), q_(LFQueue<LogElement>(LOG_QUEUE_SIZE))
    {
        file_.open(file_name_);
        ASSERT(file_.is_open(),
               "Unable to open the logging file: " + file_name_);
        logger_thread_ = createAndStartThread(-1, "Common/Logger",
                                              [this]() { flushQueue(); });
        ASSERT(logger_thread_ != nullptr, "Failed to start logger thread.");
    }

    Logger() = delete;
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger &operator=(const Logger &&) = delete;

    void flushQueue() noexcept;

    template <typename T, typename... A>
    void log(const char *, const T &, A...) noexcept;
    void log(const char *) noexcept;

    ~Logger()
    {
        std::cerr << "Flushing and closing the Logger for: " << file_name_
                  << std::endl;
        while (q_.size())
        {
            // while there are elements left in queue, the main thread will let
            // logger  thread finish it's task and then continue by closing it
            using namespace std::literals::chrono_literals;
            std::this_thread::sleep_for(1s);
        }

        logger_running_ = false;
        logger_thread_->join();
        file_.close();
    }
};