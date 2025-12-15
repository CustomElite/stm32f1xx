#pragma once

#include <cstddef>

#include "stm32f103xb.h"

#include "utils/hardware_register.hpp"

namespace hal::rcc {

    struct registers {
        // Clock Control Register
        struct cr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, CR)>{};

            static constexpr auto PLLRDY = REG.template CreateBitfield<RCC_CR_PLLRDY>(); // PLL clock ready flag
            static constexpr auto PLLON = REG.template CreateBitfield<RCC_CR_PLLON>(); // PLL enable
            static constexpr auto CSSON = REG.template CreateBitfield<RCC_CR_CSSON>(); // Clock security system enable
            static constexpr auto HSEBYP = REG.template CreateBitfield<RCC_CR_HSEBYP>(); // External high-speed clock bypass
            static constexpr auto HSERDY = REG.template CreateBitfield<RCC_CR_HSERDY>(); // External high-speed clock ready flag
            static constexpr auto HSEON = REG.template CreateBitfield<RCC_CR_HSEON>(); // HSE clock enable
            static constexpr auto HSICAL = REG.template CreateBitfield<RCC_CR_HSICAL>(); // Internal high-speed clock calibration
            static constexpr auto HSITRIM = REG.template CreateBitfield<RCC_CR_HSITRIM>(); // Internal high-speed clock trimming
            static constexpr auto HSIRDY = REG.template CreateBitfield<RCC_CR_HSIRDY>(); // Internal high-speed clock ready flag
            static constexpr auto HSION = REG.template CreateBitfield<RCC_CR_HSION>(); // Internal high-speed clock enable
        };

        // Clock Configuration Register
        struct cfgr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, CFGR)>{};

            static constexpr auto MCO = REG.template CreateBitfield<RCC_CFGR_MCO>(); // Microcontroller clock output
            static constexpr auto USBPRE = REG.template CreateBitfield<RCC_CFGR_USBPRE>(); // USB prescaler
            static constexpr auto PLLMUL = REG.template CreateBitfield<RCC_CFGR_PLLMULL>(); // PLL multiplication factor
            static constexpr auto PLLXTPRE = REG.template CreateBitfield<RCC_CFGR_PLLXTPRE>(); // HSE divider for PLL entry
            static constexpr auto PLLSRC = REG.template CreateBitfield<RCC_CFGR_PLLSRC>(); // PLL entry clock source
            static constexpr auto ADCPRE = REG.template CreateBitfield<RCC_CFGR_ADCPRE>(); // ADC prescaler
            static constexpr auto PPRE2 = REG.template CreateBitfield<RCC_CFGR_PPRE2>(); // APB high-speed prescaler (APB2)
            static constexpr auto PPRE1 = REG.template CreateBitfield<RCC_CFGR_PPRE1>(); // APB low-speed prescaler (APB1)
            static constexpr auto HPRE = REG.template CreateBitfield<RCC_CFGR_HPRE>(); // AHB prescaler
            static constexpr auto SWS = REG.template CreateBitfield<RCC_CFGR_SWS>(); // System clock switch status
            static constexpr auto SW = REG.template CreateBitfield<RCC_CFGR_SW>(); // System clock switch
        };

        // Clock Interrupt Register
        struct cir {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, CIR)>{};

            static constexpr auto CSSC = REG.template CreateBitfield<RCC_CIR_CSSC>(); // Clock security system interrupt clear
            static constexpr auto PLLRDYC = REG.template CreateBitfield<RCC_CIR_PLLRDYC>(); // PLL ready interrupt clear
            static constexpr auto HSERDYC = REG.template CreateBitfield<RCC_CIR_HSERDYC>(); // HSE ready interrupt clear
            static constexpr auto HSIRDYC = REG.template CreateBitfield<RCC_CIR_HSIRDYC>(); // HSI ready interrupt clear
            static constexpr auto LSERDYC = REG.template CreateBitfield<RCC_CIR_LSERDYC>(); // LSE ready interrupt clear
            static constexpr auto LSIRDYC = REG.template CreateBitfield<RCC_CIR_LSIRDYC>(); // LSI ready interrupt clear
            static constexpr auto PLLRDYIE = REG.template CreateBitfield<RCC_CIR_PLLRDYIE>(); // PLL ready interrupt enable
            static constexpr auto HSERDYIE = REG.template CreateBitfield<RCC_CIR_HSERDYIE>(); // HSE ready interrupt enable
            static constexpr auto HSIRDYIE = REG.template CreateBitfield<RCC_CIR_HSIRDYIE>(); // HSI ready interrupt enable
            static constexpr auto LSERDYIE = REG.template CreateBitfield<RCC_CIR_LSERDYIE>(); // LSE ready interrupt enable
            static constexpr auto LSIRDYIE = REG.template CreateBitfield<RCC_CIR_LSIRDYIE>(); // LSI ready interrupt enable
            static constexpr auto CSSF = REG.template CreateBitfield<RCC_CIR_CSSF>(); // Clock security system interrupt flag
            static constexpr auto PLLRDYF = REG.template CreateBitfield<RCC_CIR_PLLRDYF>(); // PLL ready interrupt flag
            static constexpr auto HSERDYF = REG.template CreateBitfield<RCC_CIR_HSERDYF>(); // HSE ready interrupt flag
            static constexpr auto HSIRDYF = REG.template CreateBitfield<RCC_CIR_HSIRDYF>(); // HSI ready interrupt flag
            static constexpr auto LSERDYF = REG.template CreateBitfield<RCC_CIR_LSERDYF>(); // LSE ready interrupt flag
            static constexpr auto LSIRDYF = REG.template CreateBitfield<RCC_CIR_LSIRDYF>(); // LSI ready interrupt flag
        };

        // APB2 peripheral reset register
        struct apb2rstr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR)>{};

            static constexpr auto USART1RST = REG.template CreateBitfield<RCC_APB2RSTR_USART1RST>(); // USART1 reset
            static constexpr auto SPI1RST = REG.template CreateBitfield<RCC_APB2RSTR_SPI1RST>(); // SPI1 reset
            static constexpr auto TIM1RST = REG.template CreateBitfield<RCC_APB2RSTR_TIM1RST>(); // TIM1 timer reset
            static constexpr auto ADC2RST = REG.template CreateBitfield<RCC_APB2RSTR_ADC2RST>(); // ADC 2 interface reset
            static constexpr auto ADC1RST = REG.template CreateBitfield<RCC_APB2RSTR_ADC1RST>(); // ADC 1 interface reset
            static constexpr auto IOPERST = REG.template CreateBitfield<RCC_APB2RSTR_IOPERST>(); // IO port E reset
            static constexpr auto IOPDRST = REG.template CreateBitfield<RCC_APB2RSTR_IOPDRST>(); // IO port D reset
            static constexpr auto IOPCRST = REG.template CreateBitfield<RCC_APB2RSTR_IOPCRST>(); // IO port C reset
            static constexpr auto IOPBRST = REG.template CreateBitfield<RCC_APB2RSTR_IOPBRST>(); // IO port B reset
            static constexpr auto IOPARST = REG.template CreateBitfield<RCC_APB2RSTR_IOPARST>(); // IO port A reset
            static constexpr auto AFIORST = REG.template CreateBitfield<RCC_APB2RSTR_AFIORST>(); // Alternate function IO reset
        };

        // APB1 peripheral reset register
        struct apb1rstr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, APB1RSTR)>{};

            static constexpr auto PWRRST = REG.template CreateBitfield<RCC_APB1RSTR_PWRRST>();     // Power interface reset
            static constexpr auto BKPRST = REG.template CreateBitfield<RCC_APB1RSTR_BKPRST>(); // Backup interface reset
            static constexpr auto CAN1RST = REG.template CreateBitfield<RCC_APB1RSTR_CAN1RST>(); // CAN1 reset
            static constexpr auto USBRST = REG.template CreateBitfield<RCC_APB1RSTR_USBRST>(); // USB reset
            static constexpr auto I2C2RST = REG.template CreateBitfield<RCC_APB1RSTR_I2C2RST>(); // I2C 2 reset
            static constexpr auto I2C1RST = REG.template CreateBitfield<RCC_APB1RSTR_I2C1RST>(); // I2C 1 reset
            static constexpr auto USART3RST = REG.template CreateBitfield<RCC_APB1RSTR_USART3RST>(); // USART 3 reset
            static constexpr auto USART2RST = REG.template CreateBitfield<RCC_APB1RSTR_USART2RST>(); // USART 2 reset
            static constexpr auto SPI2RST = REG.template CreateBitfield<RCC_APB1RSTR_SPI2RST>(); // SPI2 reset
            static constexpr auto WWDGRST = REG.template CreateBitfield<RCC_APB1RSTR_WWDGRST>(); // Window watchdog reset
            static constexpr auto TIM4RST = REG.template CreateBitfield<RCC_APB1RSTR_TIM4RST>(); // Timer 4 reset
            static constexpr auto TIM3RST = REG.template CreateBitfield<RCC_APB1RSTR_TIM3RST>(); // Timer 3 reset
            static constexpr auto TIM2RST = REG.template CreateBitfield<RCC_APB1RSTR_TIM2RST>(); // Timer 2 reset
        };

        // AHB peripheral clock enable register
        struct ahbenr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, AHBENR)>{};

            static constexpr auto CRCEN = REG.template CreateBitfield<RCC_AHBENR_CRCEN>(); // CRC clock enable
            static constexpr auto FLITFEN = REG.template CreateBitfield<RCC_AHBENR_FLITFEN>(); // FLITF clock enable
            static constexpr auto SRAMEN = REG.template CreateBitfield<RCC_AHBENR_SRAMEN>(); // SRAM interface clock enable
            static constexpr auto DMA1EN = REG.template CreateBitfield<RCC_AHBENR_DMA1EN>(); // DMA1 clock enable
        };

        // APB 2 peripheral clock enable register
        struct apb2enr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, APB2ENR)>{};

            static constexpr auto USART1EN = REG.template CreateBitfield<RCC_APB2ENR_USART1EN>(); // USART1 clock enable
            static constexpr auto SPI1EN = REG.template CreateBitfield<RCC_APB2ENR_SPI1EN>(); // SPI 1 clock enable
            static constexpr auto TIM1EN = REG.template CreateBitfield<RCC_APB2ENR_TIM1EN>(); // TIM1 Timer clock enable
            static constexpr auto ADC2EN = REG.template CreateBitfield<RCC_APB2ENR_ADC2EN>(); // ADC 2 interface clock enable
            static constexpr auto ADC1EN = REG.template CreateBitfield<RCC_APB2ENR_ADC1EN>(); // ADC 1 interface clock enable
            static constexpr auto IOPEEN = REG.template CreateBitfield<RCC_APB2ENR_IOPEEN>(); // IO port E clock enable
            static constexpr auto IOPDEN = REG.template CreateBitfield<RCC_APB2ENR_IOPDEN>(); // IO port D clock enable
            static constexpr auto IOPCEN = REG.template CreateBitfield<RCC_APB2ENR_IOPCEN>(); // IO port C clock enable
            static constexpr auto IOPBEN = REG.template CreateBitfield<RCC_APB2ENR_IOPBEN>(); // IO port B clock enable
            static constexpr auto IOPAEN = REG.template CreateBitfield<RCC_APB2ENR_IOPAEN>(); // IO port A clock enable
            static constexpr auto AFIOEN = REG.template CreateBitfield<RCC_APB2ENR_AFIOEN>(); // Alternate function IO clock enable
        };

        // APB 1 peripheral clock enable register
        struct apb1enr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, APB1ENR)>{};

            static constexpr auto PWREN = REG.template CreateBitfield<RCC_APB1ENR_PWREN>(); // Power interface clock enable
            static constexpr auto BKPEN = REG.template CreateBitfield<RCC_APB1ENR_BKPEN>(); // Backup interface clock enable
            static constexpr auto CAN1EN = REG.template CreateBitfield<RCC_APB1ENR_CAN1EN>(); // CAN1 clock enable
            static constexpr auto USBEN = REG.template CreateBitfield<RCC_APB1ENR_USBEN>(); // USB clock enable
            static constexpr auto I2C2EN = REG.template CreateBitfield<RCC_APB1ENR_I2C2EN>(); // I2C 2 clock enable
            static constexpr auto I2C1EN = REG.template CreateBitfield<RCC_APB1ENR_I2C1EN>(); // I2C 1 clock enable
            static constexpr auto USART3EN = REG.template CreateBitfield<RCC_APB1ENR_USART3EN>(); // USART 3 clock enable
            static constexpr auto USART2EN = REG.template CreateBitfield<RCC_APB1ENR_USART2EN>(); // USART 2 clock enable
            static constexpr auto SPI2EN = REG.template CreateBitfield<RCC_APB1ENR_SPI2EN>(); // SPI 2 clock enable
            static constexpr auto WWDGEN = REG.template CreateBitfield<RCC_APB1ENR_WWDGEN>(); // Window watchdog clock enable
            static constexpr auto TIM4EN = REG.template CreateBitfield<RCC_APB1ENR_TIM4EN>(); // Timer 4 clock enable
            static constexpr auto TIM3EN = REG.template CreateBitfield<RCC_APB1ENR_TIM3EN>(); // Timer 3 clock enable
            static constexpr auto TIM2EN = REG.template CreateBitfield<RCC_APB1ENR_TIM2EN>(); // Timer 2 clock enable
        };

        // Backup domain control register
        struct bdcr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, BDCR)>{};

            static constexpr auto BDRST = REG.template CreateBitfield<RCC_BDCR_BDRST>(); // Backup domain software reset
            static constexpr auto RTCEN = REG.template CreateBitfield<RCC_BDCR_RTCEN>(); // RTC clock enable
            static constexpr auto RTCSEL = REG.template CreateBitfield<RCC_BDCR_RTCSEL>(); // RTC clock source selection
            static constexpr auto LSEBYP = REG.template CreateBitfield<RCC_BDCR_LSEBYP>(); // External Low Speed oscillator bypass
            static constexpr auto LSERDY = REG.template CreateBitfield<RCC_BDCR_LSERDY>(); // External Low Speed oscillator ready
            static constexpr auto LSEON = REG.template CreateBitfield<RCC_BDCR_LSEON>(); // External Low Speed oscillator enable
        };

        // Control/status register
        struct csr {
            static constexpr auto REG = hardware_register<RCC_BASE + offsetof(RCC_TypeDef, CSR)>{};

            static constexpr auto LPWRRSTF = REG.template CreateBitfield<RCC_CSR_LPWRRSTF>(); // Low-power reset flag
            static constexpr auto WWDGRSTF = REG.template CreateBitfield<RCC_CSR_WWDGRSTF>(); // Window watchdog reset flag
            static constexpr auto IWDGRSTF = REG.template CreateBitfield<RCC_CSR_IWDGRSTF>(); // Independent watchdog reset flag
            static constexpr auto SFTRSTF = REG.template CreateBitfield<RCC_CSR_SFTRSTF>(); // Software reset flag
            static constexpr auto PORRSTF = REG.template CreateBitfield<RCC_CSR_PORRSTF>(); // POR/PDR reset flag
            static constexpr auto PINRSTF = REG.template CreateBitfield<RCC_CSR_PINRSTF>(); // PIN reset flag
            static constexpr auto RMVF = REG.template CreateBitfield<RCC_CSR_RMVF>(); // Remove reset flag
            static constexpr auto LSIRDY = REG.template CreateBitfield<RCC_CSR_LSIRDY>(); // Internal low speed oscillator ready
            static constexpr auto LSION = REG.template CreateBitfield<RCC_CSR_LSION>(); // Internal low speed oscillator enable
        };
    };
}
