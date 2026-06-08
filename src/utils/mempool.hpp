#pragma once

#include "macros.hpp"
#include <vector>

template <typename T> class MemPool final
{
  private:
    struct ObjectBlock
    {
        T object_;
        bool is_free_ = true;
    };
    std::vector<ObjectBlock> memStore_;
    size_t next_free_idx_ = 0;
    inline void updateNextFreeIdx() noexcept
    {
        const auto inital_free_idx = next_free_idx_;
        while (!memStore_[next_free_idx_].is_free_)
        {
            ++next_free_idx_;
            if (next_free_idx_ == memStore_.size()) [[unlikely]]
            {
                next_free_idx_ = 0;
            }
            if (inital_free_idx == next_free_idx_) [[unlikely]]
            {
                ASSERT(inital_free_idx != next_free_idx_,
                       "Memory Pool is out of memory.");
            }
        }
    }

  public:
    explicit MemPool(std::size_t num_elems)
        : memStore_(std::vector<ObjectBlock>(num_elems, {T(), true}))
    {
        ASSERT(reinterpret_cast<const ObjectBlock *>(&(memStore_[0].object_)) ==
                   &memStore_[0],
               "T object should be first member of ObjectBlock");
    }

    MemPool() = delete;
    MemPool(const MemPool &) = delete;
    MemPool(const MemPool &&) = delete;
    MemPool &operator=(const MemPool &) = delete;
    MemPool &operator=(const MemPool &&) = delete;

    template <typename... A> T *allocate(A... args) noexcept
    {
        auto obj_block = &(memStore_[next_free_idx_]);
        ASSERT(obj_block->is_free_, "Expected free object block at index: " +
                                       std::to_string(next_free_idx_));
        T *ret = &(obj_block->object_);
        ret = new (ret) T{args...};
        obj_block->is_free_ = false;
        updateNextFreeIdx();
        return ret;
    }

    auto deallocate(const T *elem) noexcept
    {
        const auto elem_idx =
            (reinterpret_cast<const ObjectBlock *>(elem) - &memStore_[0]);
        ASSERT(elem_idx >= 0 &&
                   static_cast<size_t>(elem_idx) < memStore_.size(),
               "Element does not belong to this memory pool");
        ASSERT(!memStore_[elem_idx].is_free_,
               "Expected in-use ObjectBlock at index: " +
                   std::to_string(elem_idx));
        memStore_[elem_idx].is_free_ = true;
    }
};