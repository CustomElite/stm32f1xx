#pragma once

#include <cstddef>
#include "stm32f103xb.h"
#include "utils/hardware_register.hpp"

namespace hal::adc {

struct registers {
    static constexpr std::uintptr_t BASE = ADC1_BASE;

    struct sr {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, SR)>{};
        static constexpr auto AWD   = REG.template CreateBitfield<ADC_SR_AWD>();
        static constexpr auto EOS   = REG.template CreateBitfield<ADC_SR_EOS>();
        static constexpr auto JEOS  = REG.template CreateBitfield<ADC_SR_JEOS>();
        static constexpr auto JSTRT = REG.template CreateBitfield<ADC_SR_JSTRT>();
        static constexpr auto STRT  = REG.template CreateBitfield<ADC_SR_STRT>();
    };

    struct cr1 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, CR1)>{};
        static constexpr auto AWDCH    = REG.template CreateBitfield<ADC_CR1_AWDCH>();
        static constexpr auto EOSIE    = REG.template CreateBitfield<ADC_CR1_EOSIE>();
        static constexpr auto AWDIE    = REG.template CreateBitfield<ADC_CR1_AWDIE>();
        static constexpr auto JEOIE    = REG.template CreateBitfield<ADC_CR1_JEOSIE>();
        static constexpr auto SCAN     = REG.template CreateBitfield<ADC_CR1_SCAN>();
        static constexpr auto AWDSGL   = REG.template CreateBitfield<ADC_CR1_AWDSGL>();
        static constexpr auto JAUTO    = REG.template CreateBitfield<ADC_CR1_JAUTO>();
        static constexpr auto DISCEN   = REG.template CreateBitfield<ADC_CR1_DISCEN>();
        static constexpr auto JDISCEN  = REG.template CreateBitfield<ADC_CR1_JDISCEN>();
        static constexpr auto DISCNUM  = REG.template CreateBitfield<ADC_CR1_DISCNUM>();
        static constexpr auto DUALMOD  = REG.template CreateBitfield<ADC_CR1_DUALMOD>();
        static constexpr auto JAWDEN   = REG.template CreateBitfield<ADC_CR1_JAWDEN>();
        static constexpr auto AWDEN    = REG.template CreateBitfield<ADC_CR1_AWDEN>();
    };

    struct cr2 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, CR2)>{};
        static constexpr auto ADON      = REG.template CreateBitfield<ADC_CR2_ADON>();
        static constexpr auto CONT      = REG.template CreateBitfield<ADC_CR2_CONT>();
        static constexpr auto CAL       = REG.template CreateBitfield<ADC_CR2_CAL>();
        static constexpr auto RSTCAL    = REG.template CreateBitfield<ADC_CR2_RSTCAL>();
        static constexpr auto DMA       = REG.template CreateBitfield<ADC_CR2_DMA>();
        static constexpr auto ALIGN     = REG.template CreateBitfield<ADC_CR2_ALIGN>();
        static constexpr auto JEXTSEL   = REG.template CreateBitfield<ADC_CR2_JEXTSEL>();
        static constexpr auto JEXTTRIG  = REG.template CreateBitfield<ADC_CR2_JEXTTRIG>();
        static constexpr auto EXTSEL    = REG.template CreateBitfield<ADC_CR2_EXTSEL>();
        static constexpr auto EXTTRIG   = REG.template CreateBitfield<ADC_CR2_EXTTRIG>();
        static constexpr auto JSWSTART  = REG.template CreateBitfield<ADC_CR2_JSWSTART>();
        static constexpr auto SWSTART   = REG.template CreateBitfield<ADC_CR2_SWSTART>();
        static constexpr auto TSVREFE   = REG.template CreateBitfield<ADC_CR2_TSVREFE>();
    };

    struct smpr1 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, SMPR1)>{};
        static constexpr auto SMP10 = REG.template CreateBitfield<ADC_SMPR1_SMP10>();
        static constexpr auto SMP11 = REG.template CreateBitfield<ADC_SMPR1_SMP11>();
        static constexpr auto SMP12 = REG.template CreateBitfield<ADC_SMPR1_SMP12>();
        static constexpr auto SMP13 = REG.template CreateBitfield<ADC_SMPR1_SMP13>();
        static constexpr auto SMP14 = REG.template CreateBitfield<ADC_SMPR1_SMP14>();
        static constexpr auto SMP15 = REG.template CreateBitfield<ADC_SMPR1_SMP15>();
        static constexpr auto SMP16 = REG.template CreateBitfield<ADC_SMPR1_SMP16>();
        static constexpr auto SMP17 = REG.template CreateBitfield<ADC_SMPR1_SMP17>();
    };

    struct smpr2 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, SMPR2)>{};
        static constexpr auto SMP0 = REG.template CreateBitfield<ADC_SMPR2_SMP0>();
        static constexpr auto SMP1 = REG.template CreateBitfield<ADC_SMPR2_SMP1>();
        static constexpr auto SMP2 = REG.template CreateBitfield<ADC_SMPR2_SMP2>();
        static constexpr auto SMP3 = REG.template CreateBitfield<ADC_SMPR2_SMP3>();
        static constexpr auto SMP4 = REG.template CreateBitfield<ADC_SMPR2_SMP4>();
        static constexpr auto SMP5 = REG.template CreateBitfield<ADC_SMPR2_SMP5>();
        static constexpr auto SMP6 = REG.template CreateBitfield<ADC_SMPR2_SMP6>();
        static constexpr auto SMP7 = REG.template CreateBitfield<ADC_SMPR2_SMP7>();
        static constexpr auto SMP8 = REG.template CreateBitfield<ADC_SMPR2_SMP8>();
        static constexpr auto SMP9 = REG.template CreateBitfield<ADC_SMPR2_SMP9>();
    };

    struct jofr1 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JOFR1)>{};
        // Add bitfields if defined
    };
    struct jofr2 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JOFR2)>{};
    };
    struct jofr3 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JOFR3)>{};
    };
    struct jofr4 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JOFR4)>{};
    };

    struct htr {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, HTR)>{};
        // Add bitfields if defined
    };

    struct ltr {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, LTR)>{};
        // Add bitfields if defined
    };

    struct sqr1 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, SQR1)>{};
        // Add bitfields for SQR1 (e.g. SQ13, SQ14, SQ15, SQ16, L)
    };
    struct sqr2 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, SQR2)>{};
        // Add bitfields for SQR2 (e.g. SQ7, SQ8, SQ9, SQ10, SQ11, SQ12)
    };
    struct sqr3 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, SQR3)>{};
        // Add bitfields for SQR3 (e.g. SQ1, SQ2, SQ3, SQ4, SQ5, SQ6)
    };

    struct jsqr {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JSQR)>{};
        // Add bitfields for JSQR (e.g. JSQ1, JSQ2, JSQ3, JSQ4, JL)
    };

    struct jdr1 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JDR1)>{};
        // Add bitfields if defined
    };
    struct jdr2 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JDR2)>{};
    };
    struct jdr3 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JDR3)>{};
    };
    struct jdr4 {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, JDR4)>{};
    };

    struct dr {
        static constexpr auto REG = hardware_register<BASE + offsetof(ADC_TypeDef, DR)>{};
        // Add bitfields for DR (e.g. DATA, ADC2DATA)
    };
};

} // namespace hal::adc