#pragma once

#include "rcc/rcc_kernel.hpp"
#include "utils/utility.hpp"

#include "system/interrupt.hpp"

#include "rcc/rcc.hpp"

#include "dma_kernel.hpp"
#include "utils/utility.hpp"


namespace hal::dma {

    namespace details {
        template <channel tCHAN>
        static constexpr auto IRQn = []() consteval noexcept {
            switch (tCHAN) {
            case channel::_1: return system::peripheral_irq::DMA_1_CH1;
            case channel::_2: return system::peripheral_irq::DMA_1_CH2;
            case channel::_3: return system::peripheral_irq::DMA_1_CH3;
            case channel::_4: return system::peripheral_irq::DMA_1_CH4;
            case channel::_5: return system::peripheral_irq::DMA_1_CH5;
            case channel::_6: return system::peripheral_irq::DMA_1_CH6;
            case channel::_7: return system::peripheral_irq::DMA_1_CH7;
            }
        }();
    }

    struct specification {
        channel const Channel;
        direction const Direction;
        increment const Increment;
        memory_alignment const MemoryDataAlignment;
        peripheral_alignment const PeripheralDataAlignment;
        mode const Mode;
        priority const Priority;
    };
    template <specification tSPEC>
    class module 
        : private rcc::clock_handler<rcc::hclk::DMA_1>
        , private system::interrupt<details::IRQn<tSPEC.Channel>>
    {
        using kernel = dma::kernel<tSPEC.Channel>;
        using irq = system::interrupt<details::IRQn<tSPEC.Channel>>;
        using hclk = rcc::clock_handler<rcc::hclk::DMA_1>;

    public:

    public:
        callback TransferComplete;
        callback HalfTransfer;
        callback TransferError;

    public:
        module() noexcept
            : hclk()
            , irq(callback::template Create<module, &module::isr>(*this), 2_u8)
        {
            kernel::ClearConfiguration();
            kernel::Configure(tSPEC.Direction, tSPEC.Increment, tSPEC.MemoryDataAlignment, tSPEC.PeripheralDataAlignment, tSPEC.Mode, tSPEC.Priority);
        }

        void Start(uint32_t const source_address, uint32_t const destination_address, uint16_t const length) noexcept
        {
            kernel::State(DISABLED);
            kernel::template ClearFlag<flag::Global>();
            kernel::DataCounter(length);
            if constexpr (tSPEC.Direction == direction::MemoryToPeripheral) {
                kernel::SetPeripheralAddress(destination_address);
                kernel::SetMemoryAddress(source_address);
            }
            else {
                kernel::SetPeripheralAddress(source_address);
                kernel::SetMemoryAddress(destination_address);
            }
            if (HalfTransfer.IsValid())
                kernel::template InterruptState<interrupt::HalfTransfer>(ENABLED);
            else
                kernel::template InterruptState<interrupt::HalfTransfer>(DISABLED);

            if (TransferComplete.IsValid())
                kernel::template InterruptState<interrupt::TransferComplete>(ENABLED);
            else
                kernel::template InterruptState<interrupt::TransferComplete>(DISABLED);

            kernel::template InterruptState<interrupt::TransferError>(ENABLED);
            kernel::State(ENABLED);
        }
        void Abort() const noexcept
        {
            kernel::template InterruptState<interrupt::HalfTransfer>(DISABLED);
            kernel::template InterruptState<interrupt::TransferComplete>(DISABLED);
            kernel::template InterruptState<interrupt::TransferError>(DISABLED);
            kernel::State(DISABLED);
            kernel::template ClearFlag<flag::Global>();
        }
        void DataCounter(uint16_t const length) noexcept { kernel::DataCounter(length); }
        [[nodiscard]] uint16_t DataCounter() noexcept { return kernel::DataCounter(); }
        
    private:
        void isr() noexcept
        {
            // Half transfer interrupt
            if (kernel::template FlagState<flag::HalfTransfer>() and kernel::template InterruptState<interrupt::HalfTransfer>()) {
                if constexpr (tSPEC.Mode != mode::Circular)
                    kernel::template InterruptState<interrupt::HalfTransfer>(DISABLED);
                kernel::template ClearFlag<flag::HalfTransfer>();
                HalfTransfer();
            }
            // Transfer complete interrupt
            else if (kernel::template FlagState<flag::TransferComplete>() and kernel::template InterruptState<interrupt::TransferComplete>()) {
                kernel::template ClearFlag<flag::TransferComplete>();
                if constexpr (tSPEC.Mode != mode::Circular) {
                    kernel::template InterruptState<interrupt::TransferComplete>(DISABLED);
                    kernel::template InterruptState<interrupt::TransferError>(DISABLED);
                }
                kernel::State(DISABLED);
                TransferComplete();
            }
            // Transfer error interrupt
            else if (kernel::template FlagState<flag::TransferError>() and kernel::template InterruptState<interrupt::TransferError>()) {
                kernel::template ClearFlag<flag::Global>();
                kernel::template InterruptState<interrupt::HalfTransfer>(DISABLED);
                kernel::template InterruptState<interrupt::TransferComplete>(DISABLED);
                kernel::template InterruptState<interrupt::TransferError>(DISABLED);
                kernel::State(DISABLED);
                TransferError();
            }
        }
    };
}