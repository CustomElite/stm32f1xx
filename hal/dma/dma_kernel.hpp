#pragma once

#include "dma_registers.hpp"

namespace hal::dma {

    enum class flag :uint8_t {
         TransferError
        ,HalfTransfer
        ,TransferComplete
        ,Global
    };
    enum class interrupt :uint8_t {
         TransferError
        ,HalfTransfer
        ,TransferComplete
    };

    ////////////////////////////////
    // Settings
    ////////////////////////////////
    enum class direction :uint8_t {
         PeripheralToMemory
        ,MemoryToPeripheral
        ,MemoryToMemory
    };
    enum class increment :uint8_t {
         None
        ,Peripheral
        ,Memory
        ,Both
    };
    enum class peripheral_alignment :uint8_t {
         Byte
        ,HalfWord
        ,Word
    };
    enum class memory_alignment :uint8_t {
         Byte
        ,HalfWord
        ,Word
    };
    enum class mode :bool {
         Normal
        ,Circular
    };
    enum class priority :uint8_t {
         Low
        ,Medium
        ,High
        ,VeryHigh
    };

    template <typename T>
    concept cValidProperty =
           std::same_as<std::remove_cvref_t<T>, direction>
        or std::same_as<std::remove_cvref_t<T>, increment>
        or std::same_as<std::remove_cvref_t<T>, peripheral_alignment>
        or std::same_as<std::remove_cvref_t<T>, memory_alignment>
        or std::same_as<std::remove_cvref_t<T>, mode>
        or std::same_as<std::remove_cvref_t<T>, priority>;

    ////////////////////////////////
    // Kernel
    ////////////////////////////////
    template <channel tChannel>
    struct kernel {
        using regs = registers<tChannel>;
        using channel_regs = channel_registers<tChannel>;

        using ISR = regs::isr;
        using IFCR = regs::ifcr;
        using CCR = channel_regs::ccr;
        using CNDTR = channel_regs::cndtr;
        using CPAR = channel_regs::cpar;
        using CMAR = channel_regs::cmar;

        static void State(state const state) noexcept { CCR::EN.Write(state); }
        [[nodiscard]] static state State() noexcept { return static_cast<state>(CCR::EN.Read()); }
        static void SetProperty(direction const direction) noexcept
        {
            CCR::MEM2MEM.Write(EnumValue(direction) >> 1u);
            CCR::DIR.Write(EnumValue(direction) & 1u);
        }
        static void SetProperty(increment const increment) noexcept
        {
            CCR::PINC.Write(EnumValue(increment) & 1u);
            CCR::MINC.Write(EnumValue(increment) >> 1u);
        }
        static void SetProperty(memory_alignment const memory_data_size) noexcept { CCR::MSIZE.Write(EnumValue(memory_data_size)); }
        static void SetProperty(peripheral_alignment const peripheral_data_size) noexcept { CCR::PSIZE.Write(EnumValue(peripheral_data_size)); }
        static void SetProperty(mode const mode) noexcept { CCR::CIRC.Write(EnumValue(mode)); }
        static void SetProperty(priority const priority) noexcept { CCR::PL.Write(EnumValue(priority)); }
        static void Configure(cValidProperty auto... property) noexcept { (SetProperty(property), ...); }
        static void DataCounter(uint16_t const length) noexcept { CNDTR::NDT.Write(length); }
        [[nodiscard]] static uint16_t DataCounter() noexcept { return CNDTR::NDT.Read(); }
        static void SetPeripheralAddress(uint32_t const address) noexcept { CPAR::PA.Write(address); }
        static void SetMemoryAddress(uint32_t const address) noexcept { CMAR::MA.Write(address); }
        template <interrupt tIT>
        static void InterruptState(state const state) noexcept
        {
            if constexpr (tIT == interrupt::TransferError) { CCR::TEIE.Write(state); }
            else if constexpr (tIT == interrupt::HalfTransfer) { CCR::HTIE.Write(state); }
            else if constexpr (tIT == interrupt::TransferComplete) { CCR::TCIE.Write(state); }
        }
        template <interrupt tIT>
        [[nodiscard]] static state InterruptState() noexcept
        {
            if constexpr (tIT == interrupt::TransferError) { return static_cast<state>(CCR::TEIE.Read()); }
            else if constexpr (tIT == interrupt::HalfTransfer) { return static_cast<state>(CCR::HTIE.Read()); }
            else if constexpr (tIT == interrupt::TransferComplete) { return static_cast<state>(CCR::TCIE.Read()); }
        }
        template <flag tFLAG>
        [[nodiscard]] static state FlagState() noexcept
        {
            if constexpr (tFLAG == flag::TransferError) { return static_cast<state>(ISR::TEIF.Read()); }
            else if constexpr (tFLAG == flag::HalfTransfer) { return static_cast<state>(ISR::HTIF.Read()); }
            else if constexpr (tFLAG == flag::TransferComplete) { return static_cast<state>(ISR::TCIF.Read()); }
            else if constexpr (tFLAG == flag::Global) { return static_cast<state>(ISR::GIF.Read()); }
        }
        template <flag tFLAG>
        static void ClearFlag() noexcept
        {
            if constexpr (tFLAG == flag::TransferError) { IFCR::CTEIF.Set(); }
            else if constexpr (tFLAG == flag::HalfTransfer) { IFCR::CHTIF.Set(); }
            else if constexpr (tFLAG == flag::TransferComplete) { IFCR::CTCIF.Set(); }
            else if constexpr (tFLAG == flag::Global) { IFCR::CGIF.Set(); }
        }
        static void ClearConfiguration()
        {
            CCR::PL.Reset();
            CCR::MSIZE.Reset();
            CCR::PSIZE.Reset();
            CCR::MINC.Reset();
            CCR::PINC.Reset();
            CCR::CIRC.Reset();
            CCR::DIR.Reset();
        }
        static void Reset() noexcept
        {
            CCR::RAW.Write(0);
            CNDTR::reg::Write(0);
            CPAR::reg::Write(0);
            CMAR::reg::Write(0);
        }
    };
}