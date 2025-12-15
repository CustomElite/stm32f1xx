#pragma once

//#include "stm32f1xx_ll_system.h"

#include "rcc/rcc_kernel.hpp"
#include "flash/flash_kernel.hpp"
#include "utils/utility.hpp"

namespace hal::system {

    inline static bool gBusInitialized = false;
    ////////////////////////////////
    // specification
    ////////////////////////////////
    struct clock_specification {
        rcc::hclk_source const HCLK_Source = rcc::hclk_source::HSI;
        rcc::pll_source const PLL_Source = rcc::pll_source::HSI_Div2;
        rcc::pll_multiplier const PLL_Multiplier = rcc::pll_multiplier::x2;
        rcc::hclk_prescaler const HCLK_Prescaler = rcc::hclk_prescaler::None;
        rcc::pclk2_prescaler const PCLK2_Prescaler = rcc::pclk2_prescaler::None;
        rcc::pclk1_prescaler const PCLK1_Prescaler = rcc::pclk1_prescaler::None;
        uint32_t const HSE_Frequency, LSE_Frequency;
    };

    ////////////////////////////////
    // module
    ////////////////////////////////
    template <clock_specification tSPEC>
    struct clock {
    private:
        static constexpr auto pllSourceFrequency = []() consteval noexcept -> uint32_t
        {
            switch (tSPEC.PLL_Source) {
            case rcc::pll_source::HSI_Div2: return HSI_VALUE / 2;
            case rcc::pll_source::HSE: return tSPEC.HSE_Frequency;
            case rcc::pll_source::HSE_Div2: return tSPEC.HSE_Frequency / 2;
            }
        }();
        static constexpr auto hclkSource = []() consteval noexcept
        {
            switch (tSPEC.HCLK_Source) {
            case rcc::hclk_source::HSI: return rcc::source_clock::HSI;
            case rcc::hclk_source::HSE: return rcc::source_clock::HSE;
            case rcc::hclk_source::PLL: return tSPEC.PLL_Source == rcc::pll_source::HSI_Div2 ? rcc::source_clock::HSI : rcc::source_clock::HSE;
            }
        }();
        static constexpr auto hclkSourceFrequency = []() consteval noexcept -> uint32_t
        {
            switch (tSPEC.HCLK_Source) {
            case rcc::hclk_source::HSI: return HSI_VALUE;
            case rcc::hclk_source::HSE: return tSPEC.HSE_Frequency;
            case rcc::hclk_source::PLL: return pllSourceFrequency * (EnumValue(tSPEC.PLL_Multiplier) + 2ul);
            }
        }();
        static constexpr uint32_t hclkDivShift = []() consteval noexcept
        {
            switch (tSPEC.HCLK_Prescaler) {
            case rcc::hclk_prescaler::None: return 0ul;
            case rcc::hclk_prescaler::Div2: return 1ul;
            case rcc::hclk_prescaler::Div4: return 2ul;
            case rcc::hclk_prescaler::Div8: return 3ul;
            case rcc::hclk_prescaler::Div16: return 4ul;
            case rcc::hclk_prescaler::Div64: return 6ul;
            case rcc::hclk_prescaler::Div128: return 7ul;
            case rcc::hclk_prescaler::Div256: return 8ul;
            case rcc::hclk_prescaler::Div512: return 9ul;
            }
        }();
        static constexpr uint32_t pclk2DivShift = []() consteval noexcept
        {
            switch (tSPEC.PCLK2_Prescaler) {
            case rcc::pclk2_prescaler::None: return 0ul;
            case rcc::pclk2_prescaler::Div2: return 1ul;
            case rcc::pclk2_prescaler::Div4: return 2ul;
            case rcc::pclk2_prescaler::Div8: return 3ul;
            case rcc::pclk2_prescaler::Div16: return 4ul;
            }
        }();
        static constexpr uint32_t pclk1DivShift = []() consteval noexcept
        {
            switch (tSPEC.PCLK1_Prescaler) {
            case rcc::pclk1_prescaler::None: return 0ul;
            case rcc::pclk1_prescaler::Div2: return 1ul;
            case rcc::pclk1_prescaler::Div4: return 2ul;
            case rcc::pclk1_prescaler::Div8: return 3ul;
            case rcc::pclk1_prescaler::Div16: return 4ul;
            }
        }();

    public:
        clock() noexcept
        {
            if (not gBusInitialized) {
                flash::kernel::SetLatency(0);
                flash::kernel::PrefetchState(ENABLED);
                //LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
                //while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0); 

                //LL_FLASH_EnablePrefetch();
                __NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

                rcc::kernel::SourceClockState<hclkSource>(ENABLED);
                if constexpr (tSPEC.HCLK_Source == rcc::hclk_source::HSI) {
                    rcc::kernel::Configure(tSPEC.PLL_Source, tSPEC.PLL_Multiplier);
                    rcc::kernel::SourceClockState<rcc::source_clock::PLL>(ENABLED);
                }
                rcc::kernel::Configure(
                     tSPEC.HCLK_Source
                    ,tSPEC.HCLK_Prescaler
                    ,tSPEC.PCLK2_Prescaler
                    ,tSPEC.PCLK1_Prescaler
                );
                SystemCoreClock = HCLK_Frequency;
                gBusInitialized = true;
            }
        }
        ~clock() noexcept
        {
            rcc::kernel::SourceClockState<hclkSource>(DISABLED);
            if constexpr (tSPEC.HCLK_Source == rcc::hclk_source::HSI)
                rcc::kernel::SourceClockState<rcc::source_clock::PLL>(DISABLED);

            SystemCoreClock = HSI_VALUE;
            gBusInitialized = false;
        }
        
    public:
        static constexpr uint32_t HCLK_Frequency = hclkSourceFrequency >> hclkDivShift;
        static constexpr uint32_t PCLK2_Frequency = HCLK_Frequency >> pclk2DivShift;
        static constexpr uint32_t PCLK1_Frequency = HCLK_Frequency >> pclk1DivShift;
        static constexpr uint32_t PLL_Frequency = pllSourceFrequency * (EnumValue(tSPEC.PLL_Multiplier) + 2u);

        template <rcc::cPeripheralClock auto tPERIPH>
        static constexpr uint32_t PCLK_Frequency = []() consteval noexcept
        {
            if constexpr (std::same_as<decltype(tPERIPH), rcc::pclk2>)
                return PCLK2_Frequency;
            else if constexpr (std::same_as<decltype(tPERIPH), rcc::pclk1>)
                return PCLK1_Frequency;
        }();
    };
}