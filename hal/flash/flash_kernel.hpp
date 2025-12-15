#pragma once

#include "flash/flash_registers.hpp"

namespace hal::flash {

    class kernel {
        using ACR = registers::acr;

    public:
        static void SetLatency(uint32_t const latency) noexcept
        {
            ACR::LATENCY.Write(latency);
        }
        static uint32_t GetLantency() noexcept
        {
            return ACR::LATENCY.Read();
        }
        static void PrefetchState(state const state) noexcept
        {
            ACR::PRFTBE.Write(state);
        }
    };
}