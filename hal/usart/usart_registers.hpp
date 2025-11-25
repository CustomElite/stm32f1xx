#pragma once

#include <cstdint>
#include <cstddef>

#include "stm32f103xb.h"

#include "../utils/hardware_register.hpp"

namespace hal::usart {

    enum class peripheral :uint8_t {
         USART_1
        ,USART_2
        ,USART_3
    };

    template <peripheral tPeriph>
    class registers {
        static constexpr auto USART_BASE = []() consteval noexcept {
            switch (tPeriph) {
                case peripheral::USART_1: return USART1_BASE;
                case peripheral::USART_2: return USART2_BASE;
                case peripheral::USART_3: return USART3_BASE;
            }
        }();
    public:
        // Status Register
        struct sr {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, SR)>{};

            static constexpr auto CTS = REG.template CreateBitfield<USART_SR_CTS>();
            static constexpr auto LBD = REG.template CreateBitfield<USART_SR_LBD>();
            static constexpr auto TXE = REG.template CreateBitfield<USART_SR_TXE>();
            static constexpr auto TC = REG.template CreateBitfield<USART_SR_TC>();
            static constexpr auto RXNE = REG.template CreateBitfield<USART_SR_RXNE>();
            static constexpr auto IDLE = REG.template CreateBitfield<USART_SR_IDLE>();
            static constexpr auto ORE = REG.template CreateBitfield<USART_SR_ORE>();
            static constexpr auto NE = REG.template CreateBitfield<USART_SR_NE>();
            static constexpr auto FE = REG.template CreateBitfield<USART_SR_FE>();
            static constexpr auto PE = REG.template CreateBitfield<USART_SR_PE>();
        };

        // Data Register
        struct dr {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, DR)>{};

            static constexpr auto DATA = REG.template CreateBitfield<USART_DR_DR>();
        };

        // Baud Rate Register
        struct brr {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, BRR)>{};

            static constexpr auto MANTISSA = REG.template CreateBitfield<USART_BRR_DIV_Mantissa>();
            static constexpr auto FRACTION = REG.template CreateBitfield<USART_BRR_DIV_Fraction>();
        };

        // Control Register 1
        struct cr1 {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, CR1)>{};

            static constexpr auto UE = REG.template CreateBitfield<USART_CR1_UE>();
            static constexpr auto M = REG.template CreateBitfield<USART_CR1_M>();
            static constexpr auto WAKE = REG.template CreateBitfield<USART_CR1_WAKE>();
            static constexpr auto PCE = REG.template CreateBitfield<USART_CR1_PCE>();
            static constexpr auto PS = REG.template CreateBitfield<USART_CR1_PS>();
            static constexpr auto PEIE = REG.template CreateBitfield<USART_CR1_PEIE>();
            static constexpr auto TXEIE = REG.template CreateBitfield<USART_CR1_TXEIE>();
            static constexpr auto TCIE = REG.template CreateBitfield<USART_CR1_TCIE>();
            static constexpr auto RXNEIE = REG.template CreateBitfield<USART_CR1_RXNEIE>();
            static constexpr auto IDLEIE = REG.template CreateBitfield<USART_CR1_IDLEIE>();
            static constexpr auto TE = REG.template CreateBitfield<USART_CR1_TE>();
            static constexpr auto RE = REG.template CreateBitfield<USART_CR1_RE>();
            static constexpr auto RWU = REG.template CreateBitfield<USART_CR1_RWU>();
            static constexpr auto SBK = REG.template CreateBitfield<USART_CR1_SBK>();
        };

        // Control Register 2
        struct cr2 {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, CR2)>{};

            static constexpr auto LINEN = REG.template CreateBitfield<USART_CR2_LINEN>();
            static constexpr auto STOP = REG.template CreateBitfield<USART_CR2_STOP>();
            static constexpr auto CPOL = REG.template CreateBitfield<USART_CR2_CPOL>();
            static constexpr auto CPHA = REG.template CreateBitfield<USART_CR2_CPHA>();
            static constexpr auto LBCL = REG.template CreateBitfield<USART_CR2_LBCL>();
            static constexpr auto LBDIE = REG.template CreateBitfield<USART_CR2_LBDIE>();
            static constexpr auto LBDL = REG.template CreateBitfield<USART_CR2_LBDL>();
            static constexpr auto ADD = REG.template CreateBitfield<USART_CR2_ADD>();
        };

        // Control Register 3
        struct cr3 {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, CR3)>{};

            static constexpr auto CTSIE = REG.template CreateBitfield<USART_CR3_CTSIE>();
            static constexpr auto CTSE = REG.template CreateBitfield<USART_CR3_CTSE>();
            static constexpr auto RTSE = REG.template CreateBitfield<USART_CR3_RTSE>();
            static constexpr auto DMAT = REG.template CreateBitfield<USART_CR3_DMAT>();
            static constexpr auto DMAR = REG.template CreateBitfield<USART_CR3_DMAR>();
            static constexpr auto SCEN = REG.template CreateBitfield<USART_CR3_SCEN>();
            static constexpr auto NACK = REG.template CreateBitfield<USART_CR3_NACK>();
            static constexpr auto HDSEL = REG.template CreateBitfield<USART_CR3_HDSEL>();
            static constexpr auto IRLP = REG.template CreateBitfield<USART_CR3_IRLP>();
            static constexpr auto IREN = REG.template CreateBitfield<USART_CR3_IREN>();
            static constexpr auto EIE = REG.template CreateBitfield<USART_CR3_EIE>();
        };

        // Guard Time and Prescaler Register
        struct gtpr {
            static constexpr auto REG = hardware_register<USART_BASE + offsetof(USART_TypeDef, GTPR)>{};

            static constexpr auto GT = REG.template CreateBitfield<USART_GTPR_GT>();
            static constexpr auto PSC = REG.template CreateBitfield<USART_GTPR_PSC>();
        };
    };
}