#pragma once

#include <concepts>
#include <cstdint>

#include "usart_registers.hpp"

namespace hal::usart {

    enum class flag :uint8_t {
         RXNE
        ,TXE
        ,TC
        ,CTS
        ,IDLE
        ,LBD
        ,PE
        ,FE
        ,NE
        ,ORE    
    };
    enum class interrupt :uint8_t {
         RXNE
        ,TXE
        ,TC
        ,CTS
        ,IDLE
        ,LBD
        ,PE
        ,ERR   
    };
    enum class error :uint8_t {
         None
        ,Parity
        ,Frame
        ,Noise
        ,Overrun
    };

    enum class data_width :bool {
          _8bits = 0
        , _9bits = 1
    };
    enum class parity_bit :uint8_t {
          None = 0
        , Odd = 0b11
    };
    enum class stop_bits :uint8_t {
          _1 = 0
        , _0_5 = 0b01
        , _2 = 0b10
        , _1_5 = 0b11
    };
    enum class flow_control :uint8_t {
          None = 0
        , CTS = 0b01
        , RTS = 0b10
        , CTS_RTS = CTS | RTS
    };
    struct transfer_speed {
        enum :uint32_t {
             _2400 = 2400
            ,_4800 = 4800
            ,_9600 = 9600
            ,_19200 = 19200
            ,_38400 = 38400
            ,_57600 = 57600
            ,_115200 = 115200
            ,_230400 = 230400
            ,_250000 = 250000
        } Baudrate;
        uint32_t const PCLK_Frequency;
    };

    template <typename T>
    concept cValidProperty =
           std::same_as<std::remove_cvref_t<T>, data_width>
        or std::same_as<std::remove_cvref_t<T>, parity_bit>
        or std::same_as<std::remove_cvref_t<T>, stop_bits>
        or std::same_as<std::remove_cvref_t<T>, flow_control>
        or std::same_as<std::remove_cvref_t<T>, transfer_speed>;

    template <peripheral tPeriph>
    class kernel {
        using CR1 = registers<tPeriph>::cr1;
        using CR2 = registers<tPeriph>::cr2;
        using CR3 = registers<tPeriph>::cr3;
        using SR = registers<tPeriph>::sr;
        using DR = registers<tPeriph>::dr;
        using BRR = registers<tPeriph>::brr;

    public:
        static void State(state const state) noexcept { CR1::UE.Write(state); }
        [[nodiscard]] static state State() noexcept { return static_cast<state>(CR1::UE.Read()); }
        static void TxState(state const state) noexcept { CR1::TE.Write(state); }
        [[nodiscard]] static state TxState() noexcept { return static_cast<state>(CR1::TE.Read()); }
        static void RxState(state const state) noexcept { CR1::RE.Write(state); }
        [[nodiscard]] static state RxState() noexcept { return static_cast<state>(CR1::RE.Read()); }
        static void TxDMA(state const state) noexcept { CR3::DMAT.Write(state); }
        [[nodiscard]] static state TxDMA() noexcept { return static_cast<state>(CR3::DMAT.Read()); }
        static void RxDMA(state const state) noexcept { CR3::DMAR.Write(state); }
        [[nodiscard]] static state RxDMA() noexcept { return static_cast<state>(CR3::DMAR.Read()); }
        static void WriteData(uint16_t const data) noexcept { DR::DATA.Write(data); }
        [[nodiscard]] static uint16_t ReadData() noexcept { return DR::DATA.Read(); }
        static void SetProperty(data_width const width) noexcept { CR1::M.Write(EnumValue(width)); }
        static void SetProperty(parity_bit const parity) noexcept
        { 
            auto const tmp = EnumValue(parity);
            CR1::PCE.Write(tmp & 0b01);
            CR1::PS.Write((tmp & 0b10) >> 1u);
        }
        static void SetProperty(stop_bits const stop) noexcept { CR2::STOP.Write(EnumValue(stop)); }
        static void SetProperty(flow_control const flow) noexcept
        { 
            auto const tmp = EnumValue(flow);
            CR3::CTSE.Write(tmp & 0b01);
            CR3::RTSE.Write((tmp & 0b10) >> 1u);
        }
        static void SetProperty(transfer_speed const& baudrate) noexcept
        {
            uint32_t const div_x100 = (baudrate.PCLK_Frequency * 25u) / (baudrate.Baudrate * 4u);
            uint32_t const mant = div_x100 / 100u;
            uint32_t const frac = (((div_x100 - (mant * 100u)) * 16u) + 50u) / 100u;

            BRR::MANTISSA.Write(mant & 0xFFF);
            BRR::FRACTION.Write(frac & 0xF);
        }
        static void Configure(cValidProperty auto... property) noexcept { ( SetProperty(property), ... ); }
        template <interrupt tInterrupt>
        [[nodiscard]] static state InterruptState() noexcept
        {
            if constexpr (tInterrupt == interrupt::RXNE) { return (state)CR1::RXNEIE.Read(); }
            else if constexpr (tInterrupt == interrupt::TXE) { return (state)CR1::TXEIE.Read(); }
            else if constexpr (tInterrupt == interrupt::TC) { return (state)CR1::TCIE.Read(); }
            else if constexpr (tInterrupt == interrupt::CTS) { return (state)CR3::CTSIE.Read(); }
            else if constexpr (tInterrupt == interrupt::IDLE) { return (state)CR1::IDLEIE.Read(); }
            else if constexpr (tInterrupt == interrupt::LBD) { return (state)CR2::LBDIE.Read(); }
            else if constexpr (tInterrupt == interrupt::PE) { return (state)CR1::PEIE.Read(); }
            else if constexpr (tInterrupt == interrupt::ERR) { return (state)CR3::EIE.Read(); }
        }
        template <interrupt tInterrupt>
        static void InterruptState(state const state) noexcept
        {
            if constexpr (tInterrupt == interrupt::RXNE) { CR1::RXNEIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::TXE) { CR1::TXEIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::TC) { CR1::TCIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::CTS) { CR3::CTSIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::IDLE) { CR1::IDLEIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::LBD) { CR2::LBDIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::PE) { CR1::PEIE.Write(state); }
            else if constexpr (tInterrupt == interrupt::ERR) { CR3::EIE.Write(state); }
        }
        template <flag tFlag>
        [[nodiscard]] static state FlagState() noexcept
        {
            if constexpr (tFlag == flag::RXNE) { return (state)SR::RXNE.Read(); }
            else if constexpr (tFlag == flag::TXE) { return (state)SR::TXE.Read(); }
            else if constexpr (tFlag == flag::TC) { return (state)SR::TC.Read(); }
            else if constexpr (tFlag == flag::CTS) { return (state)SR::CTS.Read(); }
            else if constexpr (tFlag == flag::IDLE) { return (state)SR::IDLE.Read(); }
            else if constexpr (tFlag == flag::LBD) { return (state)SR::LBD.Read(); }
            else if constexpr (tFlag == flag::PE) { return (state)SR::PE.Read(); }
            else if constexpr (tFlag == flag::FE) { return (state)SR::FE.Read(); }
            else if constexpr (tFlag == flag::NE) { return (state)SR::NE.Read(); }
            else if constexpr (tFlag == flag::ORE) { return (state)SR::ORE.Read(); }
        }
        template <flag tFlag>
        static void ClearFlag() noexcept
        {
            if constexpr (tFlag == flag::RXNE) { SR::RXNE.Reset(); }
            else if constexpr (tFlag == flag::TC) { SR::TC.Reset(); }
            else if constexpr (tFlag == flag::CTS) { SR::CTS.Reset(); }
            else if constexpr (tFlag == flag::LBD) { SR::LBD.Reset(); }
            else if constexpr (tFlag == flag::PE) { 
                while (!FlagState<flag::RXNE>()); 
                DR::DATA.Read();
            }
            else if constexpr (tFlag == flag::FE or tFlag == flag::NE or tFlag == flag::ORE or tFlag == flag::IDLE) { 
                (void)DR::DATA.Read(); 
            }
        }
        static constexpr uint32_t DataRegisterAddress() noexcept
        {
            return DR::REG.Address;
        }
    };
}