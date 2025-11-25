#pragma once

#include "gpio_registers.hpp"

namespace hal::gpio {
    enum class pin_state :bool {
         Low = false
        ,High = true
    };
    enum class input_mode :uint8_t {
         Analog = 0b000
        ,Floating = 0b001
        ,PullDown = 0b010
        ,PullUp = 0b110
    };
    enum class output_mode :uint8_t {
         GP_PushPull = 0b00
        ,GP_OpenDrain = 0b01
        ,AF_PushPull = 0b10
        ,AF_OpenDrain = 0b11
    };
    enum class output_speed :uint8_t {
         _10MHz = 0b01
        ,_2MHz = 0b10
        ,_50MHz = 0b11
    };

    template <typename T>
    concept cValidProperty = 
           std::same_as<std::remove_cvref_t<T>, input_mode>
        or std::same_as<std::remove_cvref_t<T>, output_mode>
        or std::same_as<std::remove_cvref_t<T>, output_speed>;

    template <port tPort, pin tPin>
    class kernel {
        using CRX = registers<tPort, tPin>::crx;
        using IDR = registers<tPort, tPin>::idr;
        using ODR = registers<tPort, tPin>::odr;
        using BSRR = registers<tPort, tPin>::bsrr;
        using BRR = registers<tPort, tPin>::brr;
        using LCKR = registers<tPort, tPin>::lckr;

    public:
        static void SetProperty(input_mode const mode) noexcept
        {
            CRX::MODE.Reset();
            CRX::CNF.Write(EnumValue(mode) & 0b11);
            if (mode ==  input_mode::PullUp or mode == input_mode::PullDown)
                ODR::OD.Write(EnumValue(mode) >> 2);
        }
        static void SetProperty(output_mode const mode) noexcept { CRX::CNF.Write(EnumValue(mode)); }
        static void SetProperty(output_speed const speed) noexcept { CRX::MODE.Write(EnumValue(speed)); }
        static void Configure(cValidProperty auto... setting) noexcept { ( SetProperty(setting), ... ); }
        [[nodiscard]] static pin_state InputPinState() noexcept { return (pin_state)IDR::ID.Read(); }
        [[nodiscard]] static pin_state OutputPinState() noexcept { return (pin_state)ODR::OD.Read(); }
        static void OutputPinState(pin_state const state) noexcept { ODR::OD.Write(EnumValue(state)); }
        static void AtomicSet() noexcept { BSRR::BS.Set(); }
        static void AtomicReset() noexcept { BSRR::BR.Set(); }
    };
}