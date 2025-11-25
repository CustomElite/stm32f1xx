#pragma once

#include <type_traits>

#include "include/delegate.hpp"
#include "system/interrupt.hpp"

#include "rcc/rcc.hpp"

#include "exti_kernel.hpp"
#include "exti_shared_irq.hpp"

namespace hal::exti {

    namespace details {
        template <line tLINE>
        static constexpr auto IRQn = []() consteval noexcept {
            if constexpr (tLINE == line::_0) { return system::peripheral_irq::EXTI_0; }
            else if constexpr (tLINE == line::_1) { return system::peripheral_irq::EXTI_1; }
            else if constexpr (tLINE == line::_2) { return system::peripheral_irq::EXTI_2; }
            else if constexpr (tLINE == line::_3) { return system::peripheral_irq::EXTI_3; }
            else if constexpr (tLINE == line::_4) { return system::peripheral_irq::EXTI_4; }
            else if constexpr (cSharedIRQ_9_5<tLINE>) { return system::peripheral_irq::EXTI_9_5; }
            else if constexpr (cSharedIRQ_15_10<tLINE>) { return system::peripheral_irq::EXTI_15_10; }
            else if constexpr (tLINE == line::_16) { return system::peripheral_irq::PVD; }
            else if constexpr (tLINE == line::_17) { return system::peripheral_irq::RTC_ALARM; }
            else { return system::peripheral_irq::USB_WAKEUP; }
        }();

        template <line tLINE>
        using irq = std::conditional_t<cSharedIRQ<tLINE>, shared_irq<IRQn<tLINE>>, system::interrupt<IRQn<tLINE>>>;
    }

    template <line tLINE>
    struct specification;

    template <line tLINE>
    requires(cEventLine<tLINE>)
    struct specification<tLINE> {
        static constexpr line Line = tLINE;
    };

    template <line tLINE>
    requires(cGpioLine<tLINE>)
    struct specification<tLINE> {
        static constexpr line Line = tLINE;
        gpio_port const Port;
    };

    template <specification tSPEC>
    class module
        : private rcc::clock_handler<rcc::pclk2::AFIO_>
        , private details::irq<tSPEC.Line>
    {
        using kernel = exti::kernel<tSPEC.Line>;
        using pclk = rcc::clock_handler<rcc::pclk2::AFIO_>;
        using irq = details::irq<tSPEC.Line>;

    public:
        module(mode const mode = mode::Off, trigger const trigger = trigger::None, callback && func = {}) noexcept
        requires (cUniqueIRQ<tSPEC.Line>)
            : pclk()
            , irq(std::forward<callback>(func), 3_u8)
        {
            kernel::Configure(trigger, mode);
            if constexpr (cGpioLine<tSPEC.Line>)
                kernel::SetProperty(tSPEC.Port);
        }
        module(mode const mode = mode::Off, trigger const trigger = trigger::None, callback && func = {}) noexcept
        requires (cSharedIRQ<tSPEC.Line>)
            : pclk()
            , irq()
        {
            kernel::Configure(trigger, mode);
            if constexpr (cGpioLine<tSPEC.Line>)
                kernel::SetProperty(tSPEC.Port);
            
            irq::template SetCallback<tSPEC.Line>(std::forward<callback>(func));
            irq::template State<tSPEC.Line>(ENABLED);
        }
        ~module() noexcept
        requires (cSharedIRQ<tSPEC.Line>)
        {
            irq::template State<tSPEC.Line>(DISABLED);
            irq::template ClearCallback<tSPEC.Line>();
        }
        ~module() noexcept = default;

        void SetMode(mode const mode) noexcept { kernel::SetProperty(mode); }
        void SetTrigger(trigger const trigger) noexcept { kernel::SetProperty(trigger); }
        void GenerateInterrupt() noexcept { kernel::GenerateSWI(); }
        void InterruptState(state const state) noexcept
        {
            if constexpr (cSharedIRQ<tSPEC.Line>)
                irq::template State<tSPEC.Line>(state);
            else
                irq::State(state);
        }
        bool IsPending() const noexcept { return kernel::IsPending(); }
        void ClearPending() noexcept { kernel::ClearPending(); }
    };
} // namespace hal::exti