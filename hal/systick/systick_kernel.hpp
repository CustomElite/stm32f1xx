#pragma once

#include "systick_registers.hpp"

namespace hal::systick {

    enum class hclk_divider :bool {
          Div8
        , Div1
    };

    struct kernel {
        using CTRL = registers::ctrl;
        using LOAD = registers::load;
        using VAL = registers::val;
        using CALIB = registers::calib;

        static void State(state const state) noexcept { CTRL::ENABLE.Write(state); }
        static void SetProperty(hclk_divider const divider) noexcept { CTRL::CLKSOURCE.Write(EnumValue(divider)); }
        static void TickFrequency(uint16_t const frequency, uint32_t const hclk_frequency) noexcept
        {
            if (CTRL::CLKSOURCE.Read())
                LOAD::RELOAD.Write((hclk_frequency / frequency) - 1ul);
            else
                LOAD::RELOAD.Write(((hclk_frequency / 8ul) / frequency) - 1ul);

            VAL::CURRENT.Write(0ul);
        }
        static void InterruptState(state const state) noexcept { CTRL::TICKINT.Write(state); }
    };
}