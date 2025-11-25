#pragma once

#include <atomic>
#include <cstdint>

#include "utils/utility.hpp"

#include "interrupt.hpp"

#include "systick/systick_kernel.hpp"

namespace hal::system {

    ////////////////////////////////
    // Module
    ////////////////////////////////
    class tick
        : private interrupt<cortex_irq::SYSTICK>
    {
        using irq = interrupt<cortex_irq::SYSTICK>;

    public:
        tick(tick&&) = delete;
        tick(tick const&) = delete;
        tick& operator=(tick&&) = delete;
        tick& operator=(tick const&) = delete;

        tick(uint32_t const tick_frequency, uint32_t const hclk_frequency, systick::hclk_divider const divider) noexcept
            : irq([]() noexcept { sTickCount.fetch_add(1, std::memory_order_relaxed); })
        {
            if (not sInitialized) {
                systick::kernel::SetProperty(divider);
                systick::kernel::TickFrequency(tick_frequency, hclk_frequency);
                systick::kernel::InterruptState(ENABLED);
                systick::kernel::State(ENABLED);
                sInitialized = true;
            }
        }
        tick(uint32_t const tick_frequency, uint32_t const hclk_frequency) noexcept
            : tick(tick_frequency, hclk_frequency, systick::hclk_divider::Div1)
        {}
        tick(uint32_t const hclk_frequency) noexcept
            : tick(1_kHz, hclk_frequency)
        {}
        ~tick() noexcept
        {
            systick::kernel::InterruptState(DISABLED);
            systick::kernel::State(DISABLED);
            sTickCount.store(0, std::memory_order_relaxed);
            sInitialized = false;
        }
        static uint32_t Ticks() noexcept { return sTickCount.load(std::memory_order_relaxed); }

    private:
        inline static bool sInitialized = false;
        inline static std::atomic<uint32_t> sTickCount = 0;
    };

    ////////////////////////////////
    // Timer
    ////////////////////////////////
    class timer {
        static uint32_t elapsed_time(uint32_t start, uint32_t current) noexcept { return current - start; }

    public:
        timer() noexcept = default;
        timer(uint32_t const duration_ms, bool const start = false) noexcept
            :mDuration{ duration_ms }
        {
            if (start)
                Start();
        }
        void Start() noexcept
        {
            mStartTime = tick::Ticks();
            mIsRunning = true;
        }
        void Start(uint32_t const duration_ms) noexcept
        {
            mDuration = duration_ms;
            mStartTime = tick::Ticks();
            mIsRunning = true;
        }
        void Stop() noexcept { mIsRunning = false; }
        void Reset() noexcept { mStartTime = tick::Ticks(); }
        uint32_t Elapsed() const noexcept
        {
            if (not mIsRunning) [[unlikely]]
                return 0;

            return elapsed_time(mStartTime, tick::Ticks());
        }
        uint32_t Remaining() const noexcept
        {
            if (not mIsRunning) [[unlikely]]
                return 0;

            auto elapsed = elapsed_time(mStartTime, tick::Ticks());
            return (elapsed >= mDuration) ? 0 : (mDuration - elapsed);
        }
        bool IsExpired() const noexcept
        {
            if (not mIsRunning) [[unlikely]]
                return false;

            return elapsed_time(mStartTime, tick::Ticks()) >= mDuration;
        }
        bool IsRunning() const noexcept { return mIsRunning; }
        void SetDuration(uint32_t const duration_ms) noexcept { mDuration = duration_ms; }
        uint32_t Duration() const noexcept { return mDuration; }

    private:
        bool mIsRunning = false;
        uint32_t mStartTime = 0;
        uint32_t mDuration = 0;
    };
}