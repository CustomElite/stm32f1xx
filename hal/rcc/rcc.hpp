#pragma once

#include "utils/utility.hpp"

#include "rcc_kernel.hpp"

namespace hal::rcc {

    ////////////////////////////////
    // clock_handler
    ////////////////////////////////
    template <cValidClock auto tCLK>
    class clock_handler {
    protected:
        using type = decltype(tCLK);

        clock_handler()
        {
            if (not sRefCount++)
                kernel::ClockState<tCLK>(ENABLED);
        }
        ~clock_handler()
        {
            if (not --sRefCount) {
                if constexpr (cPeripheralClock<type>)
                    kernel::ResetPeripheral<tCLK>();

                kernel::ClockState<tCLK>(DISABLED);
            }
        }
        static void Reset() noexcept
        {
            if constexpr (cPeripheralClock<type>) {
                if (sRefCount == 1) {
                    kernel::ClockState<tCLK>(DISABLED);
                    kernel::ResetPeripheral<tCLK>();
                    kernel::ClockState<tCLK>(ENABLED);
                }
            }
        }
        static uint8_t ReferenceCount() noexcept { return sRefCount; }

    private:
        clock_handler(clock_handler const&) = delete;
        clock_handler(clock_handler&&) = delete;
        clock_handler& operator=(clock_handler const&) = delete;
        clock_handler& operator=(clock_handler&&) = delete;

    private:
        inline static uint8_t sRefCount = 0;
    };
}