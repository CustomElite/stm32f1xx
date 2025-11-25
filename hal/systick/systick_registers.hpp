#pragma once

#include <cstddef>

#include "stm32f103xb.h"

#include "../utils/hardware_register.hpp"

namespace hal::systick {

    struct registers {
        struct ctrl {
            static constexpr auto REG = hardware_register<SysTick_BASE + offsetof(SysTick_Type, CTRL)>{};

            static constexpr auto COUNTFLAG = REG.template CreateBitfield<SysTick_CTRL_COUNTFLAG_Msk>(); // Count flag
            static constexpr auto CLKSOURCE = REG.template CreateBitfield<SysTick_CTRL_CLKSOURCE_Msk>(); // Clock source
            static constexpr auto TICKINT = REG.template CreateBitfield<SysTick_CTRL_TICKINT_Msk>(); // Interrupt enable
            static constexpr auto ENABLE = REG.template CreateBitfield<SysTick_CTRL_ENABLE_Msk>(); // Counter enable
        };
        
        struct load {
            static constexpr auto REG = hardware_register<SysTick_BASE + offsetof(SysTick_Type, LOAD)>{};
 
            static constexpr auto RELOAD = REG.template CreateBitfield<SysTick_LOAD_RELOAD_Msk>(); // Reload value
        };

        struct val {
            static constexpr auto REG = hardware_register<SysTick_BASE + offsetof(SysTick_Type, VAL)>{};

            static constexpr auto CURRENT = REG.template CreateBitfield<SysTick_VAL_CURRENT_Msk>(); // Current value
        };

        struct calib {
            static constexpr auto REG = hardware_register<SysTick_BASE + offsetof(SysTick_Type, CALIB)>{};

            static constexpr auto NOREF = REG.template CreateBitfield<SysTick_CALIB_NOREF_Msk>(); // No reference clock
            static constexpr auto SKEW = REG.template CreateBitfield<SysTick_CALIB_SKEW_Msk>(); // Skew flag
            static constexpr auto TENMS = REG.template CreateBitfield<SysTick_CALIB_TENMS_Msk>(); // Ten ms calibration value
        };
    };
}