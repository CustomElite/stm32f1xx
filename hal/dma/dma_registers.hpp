#pragma once

#include <cstdint>
#include <cstddef>

#include "stm32f103xb.h"

#include "../utils/hardware_register.hpp"

namespace hal::dma {

    enum class channel :uint8_t {
         _1 = 0
        ,_2
        ,_3
        ,_4
        ,_5
        ,_6
        ,_7
    };

    template <channel tCHAN>
    struct registers {
        static constexpr uint8_t CHANNEL_SHIFT = EnumValue(tCHAN) * 4u;

        struct isr {
            static constexpr auto REG = hardware_register<DMA1_BASE + offsetof(DMA_TypeDef, ISR)>{};

            static constexpr auto TEIF = REG.template CreateBitfield<(DMA_ISR_TEIF1 << CHANNEL_SHIFT)>();
            static constexpr auto HTIF = REG.template CreateBitfield<(DMA_ISR_HTIF1 << CHANNEL_SHIFT)>();
            static constexpr auto TCIF = REG.template CreateBitfield<(DMA_ISR_TCIF1 << CHANNEL_SHIFT)>();
            static constexpr auto GIF = REG.template CreateBitfield<(DMA_ISR_GIF1 << CHANNEL_SHIFT)>();
        };
        struct ifcr {
            static constexpr auto REG = hardware_register<DMA1_BASE + offsetof(DMA_TypeDef, IFCR)>{};

            static constexpr auto CTEIF = REG.template CreateBitfield<(DMA_IFCR_CTEIF1 << CHANNEL_SHIFT)>();
            static constexpr auto CHTIF = REG.template CreateBitfield<(DMA_IFCR_CHTIF1 << CHANNEL_SHIFT)>();
            static constexpr auto CTCIF = REG.template CreateBitfield<(DMA_IFCR_CTCIF1 << CHANNEL_SHIFT)>();
            static constexpr auto CGIF = REG.template CreateBitfield<(DMA_IFCR_CGIF1 << CHANNEL_SHIFT)>();
        };
    };

    template <channel tCHAN>
    class channel_registers {
        static consteval uint32_t base_addr() noexcept
        {
            switch (tCHAN) {
                case channel::_1: return DMA1_Channel1_BASE;
                case channel::_2: return DMA1_Channel2_BASE;
                case channel::_3: return DMA1_Channel3_BASE;
                case channel::_4: return DMA1_Channel4_BASE;
                case channel::_5: return DMA1_Channel5_BASE;
                case channel::_6: return DMA1_Channel6_BASE;
                case channel::_7: return DMA1_Channel7_BASE;
                default: return 0;
            }
        }

    public:
        struct ccr {
            static constexpr auto REG = hardware_register<base_addr() + offsetof(DMA_Channel_TypeDef, CCR)>{};

            static constexpr auto MEM2MEM = REG.template CreateBitfield<DMA_CCR_MEM2MEM>();
            static constexpr auto PL = REG.template CreateBitfield<DMA_CCR_PL>();
            static constexpr auto MSIZE = REG.template CreateBitfield<DMA_CCR_MSIZE>();
            static constexpr auto PSIZE = REG.template CreateBitfield<DMA_CCR_PSIZE>();
            static constexpr auto MINC = REG.template CreateBitfield<DMA_CCR_MINC>();
            static constexpr auto PINC = REG.template CreateBitfield<DMA_CCR_PINC>();
            static constexpr auto CIRC = REG.template CreateBitfield<DMA_CCR_CIRC>();
            static constexpr auto DIR = REG.template CreateBitfield<DMA_CCR_DIR>();
            static constexpr auto TEIE = REG.template CreateBitfield<DMA_CCR_TEIE>();
            static constexpr auto HTIE = REG.template CreateBitfield<DMA_CCR_HTIE>();
            static constexpr auto TCIE = REG.template CreateBitfield<DMA_CCR_TCIE>();
            static constexpr auto EN = REG.template CreateBitfield<DMA_CCR_EN>();
        };
        struct cndtr {
            static constexpr auto REG = hardware_register<base_addr() + offsetof(DMA_Channel_TypeDef, CNDTR)>{};

            static constexpr auto NDT = REG.template CreateBitfield<DMA_CNDTR_NDT>();
        };
        struct cpar {
            static constexpr auto REG = hardware_register<base_addr() + offsetof(DMA_Channel_TypeDef, CPAR)>{};

            static constexpr auto PA = REG.template CreateBitfield<DMA_CPAR_PA>();
        };
        struct cmar {
            static constexpr auto REG = hardware_register<base_addr() + offsetof(DMA_Channel_TypeDef, CMAR)>{};

            static constexpr auto MA = REG.template CreateBitfield<DMA_CMAR_MA>();
        };
    };
}