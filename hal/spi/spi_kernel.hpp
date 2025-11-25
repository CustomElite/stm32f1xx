#pragma once

#include "spi_registers.hpp"

namespace hal::spi {

    enum class flag :uint8_t {
         BSY
        ,OVR
        ,MODF
        ,CRCERR
        ,UDR
        ,CHSIDE
        ,TXE
        ,RXNE
    };
    enum class interrupt :uint8_t {
         TXE
        ,RXNE
        ,ERR
    };

    ////////////////////////////////
    // Properties
    ////////////////////////////////
    enum class mode :bool {
         Slave
        ,Master
    };
    enum class data_width :bool {
         _8bit
        ,_16bit
    };
    enum class data_direction :uint8_t {
         TxRx
        ,TxOnly
        ,RxOnly
    };
    enum class bit_order :bool {
         MSB
        ,LSB
    };
    enum class slave_management :bool {
         HardwareSelected
        ,SoftwareSelected
    };
    enum class clock_polarity :bool {
        //Clock idle state
         Low
        ,High
    };
    enum class clock_phase :bool {
        //Capture data on
         LeadingEdge
        ,TrailingEdge
    };
    enum class clock_prescaler :uint8_t {
         Div2 = 0b000
        ,Div4 = 0b001
        ,Div8 = 0b010
        ,Div16 = 0b011
        ,Div32 = 0b100
        ,Div64 = 0b101
        ,Div128 = 0b110
        ,Div256 = 0b111
    };

    template <typename T>
    concept cValidProperty =
           std::same_as<std::remove_cvref_t<T>, mode>
        or std::same_as<std::remove_cvref_t<T>, data_width>
        or std::same_as<std::remove_cvref_t<T>, data_direction>
        or std::same_as<std::remove_cvref_t<T>, bit_order>
        or std::same_as<std::remove_cvref_t<T>, slave_management>
        or std::same_as<std::remove_cvref_t<T>, clock_polarity>
        or std::same_as<std::remove_cvref_t<T>, clock_phase>
        or std::same_as<std::remove_cvref_t<T>, clock_prescaler>;

    ////////////////////////////////
    // Kernel
    ////////////////////////////////
    template <peripheral tPeriph>
    class kernel {
        using CR1 = registers<tPeriph>::cr1;
        using CR2 = registers<tPeriph>::cr2;
        using SR = registers<tPeriph>::sr;
        using DR = registers<tPeriph>::dr;
        using CRCPR = registers<tPeriph>::crcpr;
        using RXCRCR = registers<tPeriph>::rxcrcr;
        using TXCRCR = registers<tPeriph>::txcrcr;

    public:
        static void State(state const state) noexcept { CR1::SPE.Write(state); }
        [[nodiscard]] static state State() noexcept { return static_cast<state>(CR1::SPE.Read()); }
        static void WriteData(uint8_t const data) noexcept { DR::DATA.Write(data); }
        static void WriteData(uint16_t const data) noexcept { DR::DATA.Write(data); }
        [[nodiscard]] static uint16_t ReadData() noexcept { return DR::DATA.Read(); }
        static void SetProperty(mode const mode) noexcept { CR1::MSTR.Write(EnumValue(mode)); }
        static void SetProperty(data_width const width) noexcept { CR1::DFF.Write(EnumValue(width)); }
        static void SetProperty(bit_order const order) noexcept { CR1::LSBFIRST.Write(EnumValue(order)); }
        static void SetProperty(clock_polarity const polarity) noexcept { CR1::CPOL.Write(EnumValue(polarity)); }
        static void SetProperty(clock_phase const phase) noexcept { CR1::CPHA.Write(EnumValue(phase)); }
        static void SetProperty(clock_prescaler const scaler) noexcept { CR1::BR.Write(EnumValue(scaler)); }
        static void SetProperty(data_direction const direction) noexcept
        {
            (void)direction;
            CR1::BIDIMODE.Write(0_u8);
            CR1::RXONLY.Write(0_u8);
        }
        static void SetProperty(slave_management const select) noexcept
        {
            CR1::SSM.Write(EnumValue(select));
            if (select == slave_management::SoftwareSelected)
                CR1::SSI.Set();
        }
        static void Configure(cValidProperty auto... setting) noexcept { ( SetProperty(setting), ... ); }
        static void SlaveSelectState(state const state) noexcept { CR1::SSI.Write(state); }
        template <interrupt tInterrupt>
        [[nodiscard]] static state InterruptState() noexcept
        {
            if constexpr (tInterrupt == interrupt::TXE) { return (state)CR2::TXEIE.Read(); }
            if constexpr (tInterrupt == interrupt::RXNE) { return (state)CR2::RXNEIE.Read(); }
            if constexpr (tInterrupt == interrupt::ERR) { return (state)CR2::ERRIE.Read(); }
        }
        template <interrupt tInterrupt>
        static void InterruptState(state const state) noexcept
        {
            if constexpr (tInterrupt == interrupt::TXE) { CR2::TXEIE.Write(state); }
            if constexpr (tInterrupt == interrupt::RXNE) { CR2::RXNEIE.Write(state); }
            if constexpr (tInterrupt == interrupt::ERR) { CR2::ERRIE.Write(state); }
        }
        template <flag tFlag>
        [[nodiscard]] static state FlagState() noexcept
        {
            if constexpr (tFlag == flag::BSY) { return (state)SR::BSY.Read(); }
            if constexpr (tFlag == flag::OVR) { return (state)SR::OVR.Read(); }
            if constexpr (tFlag == flag::MODF) { return (state)SR::MODF.Read(); }
            if constexpr (tFlag == flag::CRCERR) { return (state)SR::CRCERR.Read(); }
            if constexpr (tFlag == flag::UDR) { return (state)SR::UDR.Read(); }
            if constexpr (tFlag == flag::CHSIDE) { return (state)SR::CHSIDE.Read(); }
            if constexpr (tFlag == flag::TXE) { return (state)SR::TXE.Read(); }
            if constexpr (tFlag == flag::RXNE) { return (state)SR::RXNE.Read(); }
        }
        template <flag tFlag>
        static void ClearFlag() noexcept
        {
            if constexpr (tFlag == flag::OVR) { DR::DATA.Read(); SR::OVR.Read(); }
            if constexpr (tFlag == flag::MODF) { SR::MODF.Read(); CR1::SPE.Reset(); }
            if constexpr (tFlag == flag::CRCERR) { SR::CRCERR.Reset(); }
            if constexpr (tFlag == flag::UDR) { SR::UDR.Reset(); }
            if constexpr (tFlag == flag::CHSIDE) { SR::CHSIDE.Reset(); }
            if constexpr (tFlag == flag::RXNE) { DR::DATA.Read(); }
        }
    };
}