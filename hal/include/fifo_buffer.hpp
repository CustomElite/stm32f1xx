#pragma once

#include <cstddef>
#include <type_traits>

#include "expected.hpp"

template <typename T>
concept cBufferable = std::is_trivially_copyable_v<T> and (sizeof(T) == 1 or sizeof(T) == 2);

template <cBufferable T, size_t SZ>
requires (SZ > 0 and (SZ & (SZ - 1)) == 0)
class fifo_buffer {
    static constexpr auto sMask = SZ - 1;
    [[nodiscard]] constexpr size_t increment(size_t index) const noexcept { return (index + 1) & sMask; }

public:
    using value_type = T;

    constexpr fifo_buffer() noexcept
        : mHead(0)
        , mTail(0)
    {}

    bool push(value_type const value) noexcept
    {
        const auto next_head = increment(mHead);

        if (next_head == mTail)
            return false;

        mBuffer[mHead] = value;
        mHead = next_head;
        return true;
    }
    expected<value_type, bool> pop() noexcept
    {
        if (empty())
            return MakeUnexpected(false);
        
        value_type ret;
        ret = mBuffer[mTail];
        mTail = increment(mTail);
        return ret;
    }
    bool pop_into(value_type& out) noexcept
    {
        if (empty())
            return false;

        out = mBuffer[mTail];
        mTail = increment(mTail);
    }
    void clear() noexcept { mHead = 0; mTail = 0; }
    bool empty() const noexcept { return mHead == mTail; }
    bool full() const noexcept { return size() == capacity(); }
    size_t size() const noexcept { return (mHead >= mTail) ? mHead - mTail : SZ - (mTail - mHead); }
    constexpr size_t capacity() const noexcept { return SZ; }
    size_t available() const noexcept { return capacity() - size(); }
    value_type& front() noexcept { return mBuffer[mTail]; }
    value_type const& front() const noexcept { return mBuffer[mTail]; }
    value_type& back() noexcept { return mBuffer[mHead]; }
    value_type const& back() const noexcept { return mBuffer[mHead]; }
    value_type* data() noexcept { return mBuffer + mTail; }
    value_type const* data() const noexcept { return mBuffer + mTail; }
    void swap(fifo_buffer& other) noexcept
    {
        std::swap(mBuffer, other.mBuffer);
        std::swap(mHead, other.mHead);
        std::swap(mTail, other.mTail);
    }

private:
    value_type mBuffer[SZ]{};
    size_t mHead;
    size_t mTail;
};
