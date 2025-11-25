#pragma once

#include "utils/utility.hpp"

#include "rcc/rcc.hpp"
#include "exti/exti.hpp"

#include "exti/exti_kernel.hpp"

#include "gpio_kernel.hpp"

namespace hal::gpio {

    namespace details {
        template <port tPORT>
        static constexpr auto PCLK = []() consteval noexcept {
            if constexpr (tPORT == port::A) return rcc::pclk2::GPIO_A;
            else if constexpr (tPORT == port::B) return rcc::pclk2::GPIO_B;
            else if constexpr (tPORT == port::C) return rcc::pclk2::GPIO_C;
            else if constexpr (tPORT == port::D) return rcc::pclk2::GPIO_D;
            else if constexpr (tPORT == port::E) return rcc::pclk2::GPIO_E;
        }();
    }
    
    enum class pin_type :uint8_t {
         Null
        ,Input
        ,Output
        ,ExtInterrupt
    };

    ////////////////////////////////
    // Specification
    ////////////////////////////////
    template <pin_type tPIN>
    struct specification;

    template <pin_type tPIN>
    requires (tPIN == pin_type::Null)
    struct specification<tPIN> {
        static constexpr auto PinType = pin_type::Null;
    };

    template <pin_type tPIN>
    requires (tPIN == pin_type::Input)
    struct specification<tPIN> {
        static constexpr auto PinType = pin_type::Input;
        
        port const Port;
        pin const Pin;
        input_mode const InputMode = input_mode::Analog;
    };

    template <pin_type tPIN>
    requires (tPIN == pin_type::Output)
    struct specification<tPIN> {
        static constexpr auto PinType = pin_type::Output;

        port const Port;
        pin const Pin;
        output_mode const OutputMode = output_mode::GP_PushPull;
        output_speed const OutputSpeed = output_speed::_10MHz;
    };

    template <pin_type tPIN>
    requires (tPIN == pin_type::ExtInterrupt)
    struct specification<tPIN> {
        static constexpr auto PinType = pin_type::ExtInterrupt;
        
        port const Port;
        pin const Pin;
        input_mode const InputMode = input_mode::Analog;
    };

    template <specification tSPEC>
    class module_base
        : private rcc::clock_handler<details::PCLK<tSPEC.Port>>
    {
        using pclk = rcc::clock_handler<details::PCLK<tSPEC.Port>>;

    public:
        static constexpr auto PinType = tSPEC.PinType;
        static constexpr auto Port = tSPEC.Port;
        static constexpr auto Pin = tSPEC.Pin;

        module_base() noexcept : pclk() {}
    };

    ////////////////////////////////
    // Module
    ////////////////////////////////
    template <specification tSpec>
    class module;

    // Null Pin
    template <specification tSPEC>
    requires(tSPEC.PinType == pin_type::Null)
    class module<tSPEC> {
    public:
        static constexpr auto PinType = pin_type::Null;
    };

    // Input Pin
    template <specification tSPEC>
    requires(tSPEC.PinType == pin_type::Input)
    class module<tSPEC>
        : public module_base<tSPEC>
    {
        using base = module_base<tSPEC>;
        using kernel = gpio::kernel<tSPEC.Port, tSPEC.Pin>;

    public:
        module() noexcept
            : base()
        {
            kernel::SetProperty(tSPEC.InputMode);
        }
        ~module() noexcept = default;

        [[nodiscard]] pin_state ReadPin() const noexcept { return kernel::InputPinState(); }
        operator bool() const noexcept { return kernel::InputPinState() == pin_state::High; }
    };

    // Output Pin
    template <specification tSPEC>
    requires(tSPEC.PinType == pin_type::Output)
    class module<tSPEC>
        : public module_base<tSPEC>
    {
        using base = module_base<tSPEC>;
        using kernel = gpio::kernel<tSPEC.Port, tSPEC.Pin>;

    public:
        module() noexcept
            : base()
        {
            kernel::Configure(tSPEC.OutputSpeed, tSPEC.OutputMode);
        }
        module(pin_state const initial_state) noexcept
            : module()
        {
            kernel::OutputPinState(initial_state);
        }
        ~module() noexcept = default;

        void SetPin() const noexcept { kernel::AtomicSet(); }
        void ResetPin() const noexcept { kernel::AtomicReset(); }
        void TogglePin() const noexcept { EnumValue(kernel::OutputPinState()) ? ResetPin() : SetPin(); }
        void PinState(pin_state const state) const noexcept { kernel::OutputPinState(state); }
        [[nodiscard]] pin_state PinState() const noexcept { return kernel::OutputPinState(); }

        operator bool() const noexcept { return kernel::OutputPinState() == pin_state::High; }
    };

    template <specification tSPEC>
    requires(tSPEC.PinType == pin_type::ExtInterrupt)
    class module<tSPEC>
        : public module_base<tSPEC>
    {
        using base = module_base<tSPEC>;
        static constexpr auto extiSpec = 
            exti::specification<static_cast<exti::line>(tSPEC.Pin)> {
                .Port = static_cast<exti::gpio_port>(tSPEC.Port),
            };

        using kernel = gpio::kernel<tSPEC.Port, tSPEC.Pin>;
        using exti_pin = exti::module<extiSpec>;

    public:
        template <typename... Args>
        explicit module(Args&&... args) noexcept
            : base(), mExti(std::forward<Args>(args)...)
        {
            kernel::SetProperty(tSPEC.InputMode);
        }
        ~module() noexcept = default;

        [[nodiscard]] pin_state ReadPin() const noexcept { return kernel::InputPinState(); }
        operator bool() const noexcept { return kernel::InputPinState() == pin_state::High; }
        
        void SetTrigger(exti::trigger const trigger) noexcept { mExti.SetTrigger(trigger); }
        void SetMode(exti::mode const mode) noexcept { mExti.SetMode(mode); }
        void InterruptState(state const state) noexcept { mExti.InterruptState(state); }
        void SetCallback(callback const& func) noexcept { mExti.SetCallback(func); }
        void ClearCallback() noexcept { mExti.ClearCallback(); }
    
    private:
        exti_pin mExti;
    };

    ////////////////////////////////
    // Constants
    ////////////////////////////////
    static constexpr auto NULL_PIN_SPEC = specification<pin_type::Null>{};
    using null_pin = module<NULL_PIN_SPEC>;

    ////////////////////////////////
    // Concepts
    ////////////////////////////////
    template <typename T>
    concept cIsPin =
           T::PinType == pin_type::Output
        or T::PinType == pin_type::Input
        or T::PinType == pin_type::ExtInterrupt;

    template <typename T>
    concept cIsNullPin = T::PinType == pin_type::Null;
}
