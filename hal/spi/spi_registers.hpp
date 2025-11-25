#pragma once

#include <cstdint>
#include <cstddef>

#include "stm32f103xb.h"

#include "../utils/hardware_register.hpp"

namespace hal::spi {

    enum class peripheral : bool {
         SPI_1
        ,SPI_2
    };

    template <peripheral tPERIPH>
    class registers {
        static constexpr auto SPI_BASE = []() consteval noexcept {
            switch (tPERIPH) {
            case peripheral::SPI_1: return SPI1_BASE;
            case peripheral::SPI_2: return SPI2_BASE;
            };
        }();

    public:
        // Control Register 1
        struct cr1 {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, CR1)>{};

            static constexpr auto BIDIMODE = REG.template CreateBitfield<SPI_CR1_BIDIMODE>();
            static constexpr auto BIDIOE = REG.template CreateBitfield<SPI_CR1_BIDIOE>();
            static constexpr auto CRCEN = REG.template CreateBitfield<SPI_CR1_CRCEN>();
            static constexpr auto CRCNEXT = REG.template CreateBitfield<SPI_CR1_CRCNEXT>();
            static constexpr auto DFF = REG.template CreateBitfield<SPI_CR1_DFF>();
            static constexpr auto RXONLY = REG.template CreateBitfield<SPI_CR1_RXONLY>();
            static constexpr auto SSM = REG.template CreateBitfield<SPI_CR1_SSM>();
            static constexpr auto SSI = REG.template CreateBitfield<SPI_CR1_SSI>();
            static constexpr auto LSBFIRST = REG.template CreateBitfield<SPI_CR1_LSBFIRST>();
            static constexpr auto SPE = REG.template CreateBitfield<SPI_CR1_SPE>();
            static constexpr auto BR = REG.template CreateBitfield<SPI_CR1_BR>();
            static constexpr auto MSTR = REG.template CreateBitfield<SPI_CR1_MSTR>();
            static constexpr auto CPOL = REG.template CreateBitfield<SPI_CR1_CPOL>();
            static constexpr auto CPHA = REG.template CreateBitfield<SPI_CR1_CPHA>();
        };

        // Control Register 2
        struct cr2 {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, CR2)>{};

            static constexpr auto TXEIE = REG.template CreateBitfield<SPI_CR2_TXEIE>();
            static constexpr auto RXNEIE = REG.template CreateBitfield<SPI_CR2_RXNEIE>();
            static constexpr auto ERRIE = REG.template CreateBitfield<SPI_CR2_ERRIE>();
            static constexpr auto SSOE = REG.template CreateBitfield<SPI_CR2_SSOE>();
            static constexpr auto TXDMAEN = REG.template CreateBitfield<SPI_CR2_TXDMAEN>();
            static constexpr auto RXDMAEN = REG.template CreateBitfield<SPI_CR2_RXDMAEN>();
        };

        // Status Register
        struct sr {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, SR)>{};

            static constexpr auto BSY = REG.template CreateBitfield<SPI_SR_BSY>();
            static constexpr auto OVR = REG.template CreateBitfield<SPI_SR_OVR>();
            static constexpr auto MODF = REG.template CreateBitfield<SPI_SR_MODF>();
            static constexpr auto CRCERR = REG.template CreateBitfield<SPI_SR_CRCERR>();
            static constexpr auto UDR = REG.template CreateBitfield<SPI_SR_UDR>();
            static constexpr auto CHSIDE = REG.template CreateBitfield<SPI_SR_CHSIDE>();
            static constexpr auto TXE = REG.template CreateBitfield<SPI_SR_TXE>();
            static constexpr auto RXNE = REG.template CreateBitfield<SPI_SR_RXNE>();
        };

        // Data Register
        struct dr {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, DR)>{};

            static constexpr auto DATA = REG.template CreateBitfield<SPI_DR_DR>();
        };

        // CRC Polynomial Register
        struct crcpr {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, CRCPR)>{};

            static constexpr auto CRCPOLY = REG.template CreateBitfield<SPI_CRCPR_CRCPOLY>();
        };

        // RX CRC Register
        struct rxcrcr {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, RXCRCR)>{};

            static constexpr auto RXCRC = REG.template CreateBitfield<SPI_RXCRCR_RXCRC>();
        };

        // TX CRC Register
        struct txcrcr {
            static constexpr auto REG = hardware_register<SPI_BASE + offsetof(SPI_TypeDef, TXCRCR)>{};

            static constexpr auto TXCRC = REG.template CreateBitfield<SPI_TXCRCR_TXCRC>();
        };
    };
}
