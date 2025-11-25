#pragma once

#include <cstddef>

#include "stm32f103xb.h"

#include "../utils/hardware_register.hpp"

namespace hal::gpio {

    enum class port :uint8_t {
         A
        ,B
        ,C
        ,D
        ,E
    };
    enum class pin :uint8_t {
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
    };

    template <port tPort, pin tPin>
    class registers {
        static constexpr auto GPIO_BASE = []() consteval noexcept
        {
            switch (tPort) {
            case port::A: return GPIOA_BASE;
            case port::B: return GPIOB_BASE;
            case port::C: return GPIOC_BASE;
            case port::D: return GPIOD_BASE;
            case port::E: return GPIOE_BASE;
            }
        }();
        static constexpr uint8_t PIN = EnumValue(tPin);
        static constexpr uint32_t PIN_MASK = 0b1 << PIN;
    public:
        // Configuration Register (CRL, CRH)
        class crx {
            static constexpr uint32_t MODE_MASK = 0b0011u << ((PIN % 8u) * 4u);
            static constexpr uint32_t CNF_MASK = 0b1100u << ((PIN % 8u) * 4u);
            static constexpr uint32_t CRX_BASE = (PIN < 8u ? offsetof(GPIO_TypeDef, CRL) : offsetof(GPIO_TypeDef, CRH));

        public:
            static constexpr auto REG = hardware_register<GPIO_BASE + CRX_BASE>{};
            static constexpr auto MODE = REG.template CreateBitfield<MODE_MASK>();
            static constexpr auto CNF = REG.template CreateBitfield<CNF_MASK>();
        };

        // Data Input Register (IDR)
        struct idr {
            static constexpr auto REG = hardware_register<GPIO_BASE + offsetof(GPIO_TypeDef, IDR)>{};

            static constexpr auto ID = REG.template CreateBitfield<PIN_MASK>();
        };
        // Data Output Register (ODR)
        struct odr {
            static constexpr auto REG = hardware_register<GPIO_BASE + offsetof(GPIO_TypeDef, ODR)>{};

            static constexpr auto OD = REG.template CreateBitfield<PIN_MASK>();
        };
        // Bit Set/Reset Register (BSRR)
        struct bsrr {
            static constexpr auto REG = hardware_register<GPIO_BASE + offsetof(GPIO_TypeDef, BSRR)>{};

            static constexpr auto BS = REG.template CreateBitfield<PIN_MASK>();
            static constexpr auto BR = REG.template CreateBitfield<(PIN_MASK << 16)>();
        };
        // Bit Reset Register (BRR)
        struct brr {
            static constexpr auto REG = hardware_register<GPIO_BASE + offsetof(GPIO_TypeDef, BRR)>{};

            static constexpr auto BR = REG.template CreateBitfield<PIN_MASK>();
        };
        // Configuration Lock Register (LCKR)
        struct lckr {
            static constexpr auto REG = hardware_register<GPIO_BASE + offsetof(GPIO_TypeDef, LCKR)>{};

            static constexpr auto LCKK = REG.template CreateBitfield<GPIO_LCKR_LCKK>();
            static constexpr auto LCK = REG.template CreateBitfield<PIN_MASK>();
        };
    };
}
