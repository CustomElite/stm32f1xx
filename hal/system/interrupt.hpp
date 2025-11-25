#pragma once

#include <concepts>
#include <algorithm>

#include "stm32f103xb.h"

#include "include/delegate.hpp"
#include "utils/utility.hpp"

namespace hal::system {

    ////////////////////////////////
    // IRQn Enums
    ////////////////////////////////
    enum class cortex_irq :int8_t {
         NMI = NonMaskableInt_IRQn
        ,HARD_FAULT = HardFault_IRQn
        ,MEM_MANAGEMENT = MemoryManagement_IRQn
        ,BUS_FAULT = BusFault_IRQn
        ,USAGE_FAULT = UsageFault_IRQn
        ,SVCALL = SVCall_IRQn
        ,DEBUG_MONITOR = DebugMonitor_IRQn
        ,PEND_SV = PendSV_IRQn
        ,SYSTICK = SysTick_IRQn
    };
    enum class peripheral_irq :int8_t {
         WWDG_ = WWDG_IRQn
        ,PVD = PVD_IRQn
        ,TAMPER = TAMPER_IRQn
        ,RTC_ = RTC_IRQn
        ,FLASH_ = FLASH_IRQn
        ,RCC_ = RCC_IRQn
        ,EXTI_0 = EXTI0_IRQn
        ,EXTI_1 = EXTI1_IRQn
        ,EXTI_2 = EXTI2_IRQn
        ,EXTI_3 = EXTI3_IRQn
        ,EXTI_4 = EXTI4_IRQn
        ,DMA_1_CH1 = DMA1_Channel1_IRQn
        ,DMA_1_CH2 = DMA1_Channel2_IRQn
        ,DMA_1_CH3 = DMA1_Channel3_IRQn
        ,DMA_1_CH4 = DMA1_Channel4_IRQn
        ,DMA_1_CH5 = DMA1_Channel5_IRQn
        ,DMA_1_CH6 = DMA1_Channel6_IRQn
        ,DMA_1_CH7 = DMA1_Channel7_IRQn
        ,ADC_1_2 = ADC1_2_IRQn
        ,USB_HP_CAN1_TX = USB_HP_CAN1_TX_IRQn
        ,USB_LP_CAN1_RX0 = USB_LP_CAN1_RX0_IRQn
        ,CAN_1_RX1 = CAN1_RX1_IRQn
        ,CAN_1_SCE = CAN1_SCE_IRQn
        ,EXTI_9_5 = EXTI9_5_IRQn
        ,TIM_1_BRK = TIM1_BRK_IRQn
        ,TIM_1_UP = TIM1_UP_IRQn
        ,TIM_1_TRG_COM = TIM1_TRG_COM_IRQn
        ,TIM_1_CC = TIM1_CC_IRQn
        ,TIM_2 = TIM2_IRQn
        ,TIM_3 = TIM3_IRQn
        ,TIM_4 = TIM4_IRQn
        ,I2C1_EV = I2C1_EV_IRQn
        ,I2C1_ER = I2C1_ER_IRQn
        ,I2C2_EV = I2C2_EV_IRQn
        ,I2C2_ER = I2C2_ER_IRQn
        ,SPI_1 = SPI1_IRQn
        ,SPI_2 = SPI2_IRQn
        ,USART_1 = USART1_IRQn
        ,USART_2 = USART2_IRQn
        ,USART_3 = USART3_IRQn
        ,EXTI_15_10 = EXTI15_10_IRQn
        ,RTC_ALARM = RTC_Alarm_IRQn
        ,USB_WAKEUP = USBWakeUp_IRQn
    };

    ////////////////////////////////
    // Concepts
    ////////////////////////////////
    template <typename T>
    concept cValidIRQ = std::same_as<T, cortex_irq> or std::same_as<T, peripheral_irq>;

    ////////////////////////////////
    // interrupt
    ////////////////////////////////
    template <cValidIRQ auto tIRQ>
    class interrupt {
        static constexpr auto IRQn = static_cast<IRQn_Type>(EnumValue(tIRQ));

    public:
        static void Dispatch() noexcept { Callback(); }

    protected:
        using callback = delegate<void()>;

        inline static callback Callback;

        interrupt(callback const& func, uint8_t const priority = 7_u8, state const initial = ENABLED) noexcept
        {
            Callback = func;
            SetPriority(priority);
            State(initial);
        }
        ~interrupt() noexcept
        {
            State(DISABLED);
            Callback.Clear();
        }

        static void State(state const state) noexcept
        {
            if constexpr (std::same_as<decltype(tIRQ), peripheral_irq>)
                state ? NVIC_EnableIRQ(IRQn) : NVIC_DisableIRQ(IRQn);
        }
        static void SetPriority(unsigned const priority) noexcept
        {
            if constexpr (std::same_as<decltype(tIRQ), peripheral_irq>) {
                NVIC_SetPriority(
                    IRQn, 
                    NVIC_EncodePriority(
                        NVIC_GetPriorityGrouping(),
                        std::min(priority,15u),
                        0
                    )
                );
            }
        }

    private:
        interrupt(interrupt&&) = delete;
        interrupt(interrupt const&) = delete;
        interrupt& operator=(interrupt&&) = delete;
        interrupt& operator=(interrupt const&) = delete;
    };
}