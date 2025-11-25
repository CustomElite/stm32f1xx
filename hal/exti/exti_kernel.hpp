#pragma once

#include <cstdint>

#include "exti_registers.hpp"

namespace hal::exti {

    enum class mode :uint8_t {
         Off
        ,Interrupt
        ,Event
        ,Both
    };
    enum class trigger :uint8_t {
         None
        ,Rising
        ,Falling
        ,Both
    };
    enum class gpio_port :uint8_t {
         A
        ,B
        ,C
        ,D
        ,E
    };

    template <line tLINE>
    concept cGpioLine = tLINE < line::_16;

    template <line tLine>
    concept cEventLine = tLine > line::_15;

    template <typename T>
    concept cValidProperty =
           std::same_as<std::remove_cvref_t<T>, mode>
        or std::same_as<std::remove_cvref_t<T>, trigger>
        or std::same_as<std::remove_cvref_t<T>, gpio_port>;

    template <line tLine>
    class kernel {
        using IMR = registers<tLine>::imr;
        using EMR = registers<tLine>::emr;
        using RTSR = registers<tLine>::rtsr;
        using FTSR = registers<tLine>::ftsr;
        using SWIER = registers<tLine>::swier;
        using PR = registers<tLine>::pr;
        using CRX = registers<tLine>::crx;

    public:
        static void SetProperty(mode const mode) noexcept
        {
            switch (mode) {
            case mode::Interrupt:
                IMR::MSK.Set();
                EMR::MSK.Reset();
                break;
            case mode::Event:
                IMR::MSK.Reset();
                EMR::MSK.Set();
                break;
            case mode::Both:
                IMR::MSK.Set();
                EMR::MSK.Set();
                break;
            default:
                IMR::MSK.Reset();
                EMR::MSK.Reset();
                break;
            }
        }
        static void SetProperty(trigger const trigger) noexcept
        {
            switch (trigger) {
            case trigger::Rising:
                RTSR::TRIG.Set();
                FTSR::TRIG.Reset();
                break;
            case trigger::Falling:
                RTSR::TRIG.Reset();
                FTSR::TRIG.Set();
                break;
            case trigger::Both:
                RTSR::TRIG.Set();
                FTSR::TRIG.Set();
                break;
            default:
                RTSR::TRIG.Reset();
                FTSR::TRIG.Reset();
                break;
            }
        }
        static void SetProperty(gpio_port const gpio_port) noexcept { CRX::EXTIX.Write(EnumValue(gpio_port)); }
        static void Configure(cValidProperty auto... setting) noexcept { ( SetProperty(setting), ... ); }
        static void GenerateSWI() noexcept { SWIER::SWIE.Set(); }
        [[nodiscard]] static bool IsPending() noexcept { return static_cast<bool>(PR::PEND.Read()); }
        static void ClearPending() noexcept { PR::PEND.Set(); }
    };
}

