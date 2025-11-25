#pragma once

#include <cstdint>
#include <cstddef>

#include "stm32f103xb.h"

#include "../utils/hardware_register.hpp"

namespace hal::exti {

    enum line :uint8_t {
         _0
        ,_1
        ,_2
        ,_3
        ,_4
        ,_5
        ,_6
        ,_7
        ,_8
        ,_9
        ,_10
        ,_11
        ,_12
        ,_13
        ,_14
        ,_15
        ,_16
        ,_17
        ,_18
    };

    template <line tLine>
    class registers {
        static constexpr uint8_t LineValue = EnumValue(tLine);
        static constexpr uint32_t LineMask = 0b1 << LineValue;

    public:
        // Interrupt mask register (IMR)
        struct imr {
            static constexpr auto REG = hardware_register<EXTI_BASE + offsetof(EXTI_TypeDef, IMR)>{};
            static constexpr auto MSK = REG.template CreateBitfield<LineMask>();
        };
        // Event mask register (EMR)
        struct emr {
            static constexpr auto REG = hardware_register<EXTI_BASE + offsetof(EXTI_TypeDef, EMR)>{};
            static constexpr auto MSK = REG.template CreateBitfield<LineMask>();
        };
        // Rising trigger selection register (RTSR)
        struct rtsr {
            static constexpr auto REG = hardware_register<EXTI_BASE + offsetof(EXTI_TypeDef, RTSR)>{};
            static constexpr auto TRIG = REG.template CreateBitfield<LineMask>();
        };
        // Falling trigger selection register (FTSR)
        struct ftsr {
            static constexpr auto REG = hardware_register<EXTI_BASE + offsetof(EXTI_TypeDef, FTSR)>{};
            static constexpr auto TRIG = REG.template CreateBitfield<LineMask>();
        };
        // Software interrupt event register (SWIER)
        struct swier {
            static constexpr auto REG = hardware_register<EXTI_BASE + offsetof(EXTI_TypeDef, SWIER)>{};
            static constexpr auto SWIE = REG.template CreateBitfield<LineMask>();
        };
        // Pending register (PR)
        struct pr {
            static constexpr auto REG = hardware_register<EXTI_BASE + offsetof(EXTI_TypeDef, PR)>{};
            static constexpr auto PEND = REG.template CreateBitfield<LineMask>();
        };
        // EXTI line selection register (EXTICRx)
        struct crx {
            static constexpr uint32_t cr_mask = 0b1111u << ((LineValue % 4u) * 4u);
            static constexpr uint32_t cr_base = []() consteval noexcept {
                if constexpr (LineValue < 4u) { return offsetof(AFIO_TypeDef, EXTICR[0]); }
                else if constexpr (LineValue < 8u) { return offsetof(AFIO_TypeDef, EXTICR[1]); }
                else if constexpr (LineValue < 12u) { return offsetof(AFIO_TypeDef, EXTICR[2]); }
                else { return offsetof(AFIO_TypeDef, EXTICR[3]); }
            }();
            static constexpr auto REG = hardware_register<AFIO_BASE + cr_base>{};
            static constexpr auto EXTIX = REG.template CreateBitfield<cr_mask>();
        };
    };
}