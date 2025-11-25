#pragma once

#include "rcc_registers.hpp"

namespace hal::rcc {

    enum class hclk :uint8_t {
       DMA_1
      ,CRC_
      ,FLASH_
      ,SRAM_
    };
    enum class pclk2 :uint8_t {
       AFIO_
      ,EXTI_
      ,GPIO_A
      ,GPIO_B
      ,GPIO_C
      ,GPIO_D
      ,GPIO_E
      ,ADC_1
      ,ADC_2
      ,TIM_1
      ,SPI_1
      ,USART_1
    };
    enum class pclk1 :uint8_t {
       TIM_2
      ,TIM_3
      ,TIM_4
      ,RTC_
      ,WWDG_
      ,IWDG_
      ,SPI_2
      ,USART_2
      ,USART_3
      ,I2C_1
      ,I2C_2
      ,CAN_1
      ,BKP_
      ,PWR_
      ,USB_
    };

    template <typename T>
    concept cPeripheralClock = 
           std::same_as<std::remove_cvref_t<T>, pclk2>
        or std::same_as<std::remove_cvref_t<T>, pclk1>;

    template <typename T>
    concept cHardwareClock = std::same_as<std::remove_cvref_t<T>, hclk>;

    template <typename T>
    concept cValidClock = cHardwareClock<T> or cPeripheralClock<T>;

    enum class source_clock :uint8_t {
        PLL
      , RTCLK
      , HSE
      , HSI
      , LSE
      , LSI
      , EXT
    };
    enum class hclk_source :uint32_t {
        HSI
      , HSE
      , PLL
    };
    enum class mco_source :uint32_t {
        None = 0b000
      , HCLK = 0b100
      , HSI = 0b101
      , HSE = 0b110
      , PLL_Div2 = 0b111
    };
    enum class rtc_source :uint32_t {
        None = 0b00
      , LSE = 0b01
      , LSI = 0b10
      , HSE_Div128 = 0b11
    };
    enum class pll_source :uint32_t {
        HSI_Div2 = 0b10
      , HSE = 0b01
      , HSE_Div2 = 0b11
    };
    enum class pll_multiplier :uint32_t {
        x2
      , x3
      , x4
      , x5
      , x6
      , x7
      , x8
      , x9
      , x10
      , x11
      , x12
      , x13
      , x14
      , x15
      , x16
    };
    enum class adc_prescaler :uint32_t {
        Div2
      , Div4
      , Div6
      , Div8
    };
    enum class hclk_prescaler :uint32_t {
        None = 0b0000
      , Div2 = 0b1000
      , Div4 = 0b1001
      , Div8 = 0b1010
      , Div16 = 0b1011
      , Div64 = 0b1100
      , Div128 = 0b1101
      , Div256 = 0b1110
      , Div512 = 0b1111
    };
    enum class pclk2_prescaler :uint32_t {
        None = 0b000
      , Div2 = 0b100
      , Div4 = 0b101
      , Div8 = 0b110
      , Div16 = 0b111
    };
    enum class pclk1_prescaler :uint32_t {
        None = 0b000
      , Div2 = 0b100
      , Div4 = 0b101
      , Div8 = 0b110
      , Div16 = 0b111
    };

    template <typename T>
    concept cValidProperty = 
           std::same_as<std::remove_cvref_t<T>, hclk_source>
        or std::same_as<std::remove_cvref_t<T>, mco_source>
        or std::same_as<std::remove_cvref_t<T>, rtc_source>
        or std::same_as<std::remove_cvref_t<T>, pll_source>
        or std::same_as<std::remove_cvref_t<T>, pll_multiplier>
        or std::same_as<std::remove_cvref_t<T>, adc_prescaler>
        or std::same_as<std::remove_cvref_t<T>, hclk_prescaler>
        or std::same_as<std::remove_cvref_t<T>, pclk2_prescaler>
        or std::same_as<std::remove_cvref_t<T>, pclk1_prescaler>;

    template <uint32_t tHSE>
    concept cValidHSE = tHSE >= 4_MHz and tHSE <= 16_MHz;

    class kernel {
        using CR = registers::cr;
        using CFGR = registers::cfgr;
        using CIR = registers::cir;
        using APB2RSTR = registers::apb2rstr;
        using APB1RSTR = registers::apb1rstr;
        using AHBENR = registers::ahbenr;
        using APB2ENR = registers::apb2enr;
        using APB1ENR = registers::apb1enr;
        using BDCR = registers::bdcr;
        using CSR = registers::csr;

    public:
        template <source_clock tClock>
        static void SourceClockState(state const state) noexcept
        {
            if constexpr (tClock == source_clock::PLL) {
                CR::PLLON.Write(state);
                while (CR::PLLRDY.Read() != state);
            }
            else if constexpr (tClock == source_clock::RTCLK) {
                BDCR::RTCEN.Write(state);
            }
            else if constexpr (tClock == source_clock::HSE) {
                CR::HSEBYP.Write(false);
                CR::HSEON.Write(state);
                while (CR::HSERDY.Read() != state);
            }
            else if constexpr (tClock == source_clock::HSI) {
                CR::HSION.Write(state);
                while (CR::HSIRDY.Read() != state);
            }
            else if constexpr (tClock == source_clock::LSE) {
                BDCR::LSEBYP.Write(false);
                BDCR::LSEON.Write(state);
                while (BDCR::LSERDY.Read() != state);
            }
            else if constexpr (tClock == source_clock::LSI) {
                CSR::LSION.Write(state);
                while (CSR::LSIRDY.Read() != state);
            }
        }
        template <hclk tCLK>
        static void ClockState(state const state) noexcept
        {
            if constexpr (tCLK == hclk::DMA_1) { AHBENR::DMA1EN.Write(state); }
            if constexpr (tCLK == hclk::CRC_) { AHBENR::CRCEN.Write(state); }
            if constexpr (tCLK == hclk::FLASH_) { AHBENR::FLITFEN.Write(state); }
            if constexpr (tCLK == hclk::SRAM_) { AHBENR::SRAMEN.Write(state); }
        }
        template <pclk2 tCLK>
        static void ClockState(state const state) noexcept
        {
            if constexpr (tCLK == pclk2::AFIO_) { APB2ENR::AFIOEN.Write(state); }
            if constexpr (tCLK == pclk2::GPIO_A) { APB2ENR::IOPAEN.Write(state); }
            if constexpr (tCLK == pclk2::GPIO_B) { APB2ENR::IOPBEN.Write(state); }
            if constexpr (tCLK == pclk2::GPIO_C) { APB2ENR::IOPCEN.Write(state); }
            if constexpr (tCLK == pclk2::GPIO_D) { APB2ENR::IOPDEN.Write(state); }
            if constexpr (tCLK == pclk2::GPIO_E) { APB2ENR::IOPEEN.Write(state); }
            if constexpr (tCLK == pclk2::ADC_1) { APB2ENR::ADC1EN.Write(state); }
            if constexpr (tCLK == pclk2::ADC_2) { APB2ENR::ADC2EN.Write(state); }
            if constexpr (tCLK == pclk2::TIM_1) { APB2ENR::TIM1EN.Write(state); }
            if constexpr (tCLK == pclk2::SPI_1) { APB2ENR::SPI1EN.Write(state); }
            if constexpr (tCLK == pclk2::USART_1) { APB2ENR::USART1EN.Write(state); }
        }
        template <pclk1 tCLK>
        static void ClockState(state const state) noexcept
        {
            if constexpr (tCLK == pclk1::TIM_2) { APB1ENR::TIM2EN.Write(state); }
            if constexpr (tCLK == pclk1::TIM_3) { APB1ENR::TIM3EN.Write(state); }
            if constexpr (tCLK == pclk1::TIM_4) { APB1ENR::TIM4EN.Write(state); }
            if constexpr (tCLK == pclk1::WWDG_) { APB1ENR::WWDGEN.Write(state); }
            if constexpr (tCLK == pclk1::SPI_2) { APB1ENR::SPI2EN.Write(state); }
            if constexpr (tCLK == pclk1::USART_2) { APB1ENR::USART2EN.Write(state); }
            if constexpr (tCLK == pclk1::USART_3) { APB1ENR::USART3EN.Write(state); }
            if constexpr (tCLK == pclk1::I2C_1) { APB1ENR::I2C1EN.Write(state); }
            if constexpr (tCLK == pclk1::I2C_2) { APB1ENR::I2C2EN.Write(state); }
            if constexpr (tCLK == pclk1::CAN_1) { APB1ENR::CAN1EN.Write(state); }
            if constexpr (tCLK == pclk1::BKP_) { APB1ENR::BKPEN.Write(state); }
            if constexpr (tCLK == pclk1::PWR_) { APB1ENR::PWREN.Write(state); }
            if constexpr (tCLK == pclk1::USB_) { APB1ENR::USBEN.Write(state); }
        }
        template <pclk2 tPCLK>
        static void ResetPeripheral() noexcept
        {
            if constexpr (tPCLK == pclk2::AFIO_)   { APB2RSTR::AFIORST.Set(); APB2RSTR::AFIORST.Reset(); }
            if constexpr (tPCLK == pclk2::GPIO_A)  { APB2RSTR::IOPARST.Set(); APB2RSTR::IOPARST.Reset(); }
            if constexpr (tPCLK == pclk2::GPIO_B)  { APB2RSTR::IOPBRST.Set(); APB2RSTR::IOPBRST.Reset(); }
            if constexpr (tPCLK == pclk2::GPIO_C)  { APB2RSTR::IOPCRST.Set(); APB2RSTR::IOPCRST.Reset(); }
            if constexpr (tPCLK == pclk2::GPIO_D)  { APB2RSTR::IOPDRST.Set(); APB2RSTR::IOPDRST.Reset(); }
            if constexpr (tPCLK == pclk2::GPIO_E)  { APB2RSTR::IOPERST.Set(); APB2RSTR::IOPERST.Reset(); }
            if constexpr (tPCLK == pclk2::ADC_1)   { APB2RSTR::ADC1RST.Set(); APB2RSTR::ADC1RST.Reset(); }
            if constexpr (tPCLK == pclk2::ADC_2)   { APB2RSTR::ADC2RST.Set(); APB2RSTR::ADC2RST.Reset(); }
            if constexpr (tPCLK == pclk2::TIM_1) { APB2RSTR::TIM1RST.Set(); APB2RSTR::TIM1RST.Reset(); }
            if constexpr (tPCLK == pclk2::SPI_1)   { APB2RSTR::SPI1RST.Set(); APB2RSTR::SPI1RST.Reset(); }
            if constexpr (tPCLK == pclk2::USART_1) { APB2RSTR::USART1RST.Set(); APB2RSTR::USART1RST.Reset(); }
        }
        template <pclk1 tPCLK>
        static void ResetPeripheral() noexcept
        {
            if constexpr (tPCLK == pclk1::TIM_2) { APB1RSTR::TIM2RST.Set(); APB1RSTR::TIM2RST.Reset(); }
            if constexpr (tPCLK == pclk1::TIM_3) { APB1RSTR::TIM3RST.Set(); APB1RSTR::TIM3RST.Reset(); }
            if constexpr (tPCLK == pclk1::TIM_4) { APB1RSTR::TIM4RST.Set(); APB1RSTR::TIM4RST.Reset(); }
            if constexpr (tPCLK == pclk1::WWDG_)   { APB1RSTR::WWDGRST.Set(); APB1RSTR::WWDGRST.Reset(); }
            if constexpr (tPCLK == pclk1::SPI_2)   { APB1RSTR::SPI2RST.Set(); APB1RSTR::SPI2RST.Reset(); }
            if constexpr (tPCLK == pclk1::USART_2) { APB1RSTR::USART2RST.Set(); APB1RSTR::USART2RST.Reset(); }
            if constexpr (tPCLK == pclk1::USART_3) { APB1RSTR::USART3RST.Set(); APB1RSTR::USART3RST.Reset(); }
            if constexpr (tPCLK == pclk1::I2C_1)   { APB1RSTR::I2C1RST.Set(); APB1RSTR::I2C1RST.Reset(); }
            if constexpr (tPCLK == pclk1::I2C_2)   { APB1RSTR::I2C2RST.Set(); APB1RSTR::I2C2RST.Reset(); }
            if constexpr (tPCLK == pclk1::CAN_1)   { APB1RSTR::CAN1RST.Set(); APB1RSTR::CAN1RST.Reset(); }
            if constexpr (tPCLK == pclk1::BKP_)    { APB1RSTR::BKPRST.Set(); APB1RSTR::BKPRST.Reset(); }
            if constexpr (tPCLK == pclk1::PWR_)    { APB1RSTR::PWRRST.Set(); APB1RSTR::PWRRST.Reset(); }
            if constexpr (tPCLK == pclk1::USB_)    { APB1RSTR::USBRST.Set(); APB1RSTR::USBRST.Reset(); }
        }
        static void SetProperty(hclk_source const source) noexcept
        {
            if (auto const tmp{ EnumValue(source) }; tmp != CFGR::SWS.Read()) {
                CFGR::SW.Write(tmp);
                while (CFGR::SWS.Read() != tmp);
            }
        }
        static void SetProperty(mco_source const source) noexcept { CFGR::MCO.Write(EnumValue(source)); }
        static void SetProperty(rtc_source const source) noexcept { BDCR::RTCSEL.Write(EnumValue(source)); }
        static void SetProperty(pll_source const source) noexcept
        {
            auto const tmp = EnumValue(source);
            CFGR::PLLXTPRE.Write(((tmp >> 1u) & 0b01));
            CFGR::PLLSRC.Write((tmp & 0b01));
        }
        static void SetProperty(pll_multiplier const pll_multi) noexcept { CFGR::PLLMUL.Write(EnumValue(pll_multi)); }
        static void SetProperty(hclk_prescaler const prescaler) noexcept { CFGR::HPRE.Write(EnumValue(prescaler)); }
        static void SetProperty(pclk2_prescaler const prescaler) noexcept { CFGR::PPRE2.Write(EnumValue(prescaler)); }
        static void SetProperty(pclk1_prescaler const prescaler) noexcept { CFGR::PPRE1.Write(EnumValue(prescaler)); }
        static void SetProperty(adc_prescaler const prescaler) noexcept { CFGR::ADCPRE.Write(EnumValue(prescaler)); }
        static void Configure(cValidProperty auto const... property) noexcept { ( SetProperty(property), ... ); }
    };
}

