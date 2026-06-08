#pragma once

#include "macros.hpp"
#include <atomic>
#include <pthread.h>
#include <string>
#include <vector>

template <typename T> class LFQueue final
{
  private:
    std::vector<T> lfq_store_;
    std::atomic<size_t> next_read_idx_ = 0;
    std::atomic<size_t> next_write_idx_ = 0;
    std::atomic<size_t> num_elems_ = 0;

  public:
    LFQueue(size_t num_elems) : lfq_store_(std::vector<T>(num_elems, T())) {}

    LFQueue() = delete;
    LFQueue(const LFQueue &) = delete;
    LFQueue(const LFQueue &&) = delete;
    LFQueue &operator=(const LFQueue &) = delete;
    LFQueue &operator=(const LFQueue &&) = delete;

    auto getNextToWrite() noexcept { return lfq_store_[next_write_idx_]; }
    auto updateWriteIdx() noexcept
    {
        next_write_idx_ = (next_write_idx_ + 1) % lfq_store_.size();
        num_elems_++;
    }

    auto getNextToRead() noexcept
    {
        return (next_read_idx_ == next_write_idx_)
                   ? nullptr
                   : &lfq_store_[next_read_idx_];
    }
    auto updateReadIdx() noexcept
    {
        next_read_idx_ = (next_read_idx_ + 1) % lfq_store_.size();
        ASSERT(num_elems_ != 0, "Read an invalid element in : " + std::to_string(pthread_self()));
        num_elems_--;
    }

    auto size() const noexcept {return num_elems_.load();}
};