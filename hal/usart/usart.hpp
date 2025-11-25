#pragma once

#include <cstdint>
#include <cstring>
#include <span>
#include <type_traits>

#include "include/fifo_buffer.hpp"
#include "include/expected.hpp"

#include "utils/utility.hpp"
#include "system/tick.hpp"
#include "system/interrupt.hpp"

#include "dma/dma.hpp"
#include "gpio/gpio.hpp"
#include "usart_kernel.hpp"

namespace hal::usart {

    namespace details {
        template <peripheral tPERIPH>
        static constexpr auto PCLK = []() consteval noexcept {
            if constexpr (tPERIPH == peripheral::USART_1) return rcc::pclk2::USART_1;
            else if constexpr (tPERIPH == peripheral::USART_2) return rcc::pclk1::USART_2;
            else return rcc::pclk1::USART_3;
        }();
        template <peripheral tPERIPH>
        static constexpr auto IRQ = []() consteval noexcept {
            switch (tPERIPH) {
            case peripheral::USART_1: return system::peripheral_irq::USART_1;
            case peripheral::USART_2: return system::peripheral_irq::USART_2;
            case peripheral::USART_3: return system::peripheral_irq::USART_3;
            }
        }();
        template <peripheral tPERIPH>
        static constexpr auto TxDMA_Channel = []() consteval noexcept {
            switch (tPERIPH) {
            case peripheral::USART_1: return dma::channel::_4;
            case peripheral::USART_2: return dma::channel::_7;
            case peripheral::USART_3: return dma::channel::_2;
            }
        }(); 
        template <peripheral tPERIPH>
        static constexpr auto RxDMA_Channel = []() consteval noexcept {
            switch (tPERIPH) {
            case peripheral::USART_1: return dma::channel::_5;
            case peripheral::USART_2: return dma::channel::_6;
            case peripheral::USART_3: return dma::channel::_3;
            }
        }();
        template <peripheral tPeriph>
        static constexpr gpio::specification<gpio::pin_type::Output> TxPinSpec {
            .Port = []() consteval noexcept {
                if constexpr (tPeriph == peripheral::USART_1) { return gpio::port::A; }
                else if constexpr (tPeriph == peripheral::USART_2) { return gpio::port::A; }
                else { return gpio::port::B; }
            }(),
            .Pin = []() consteval noexcept {
                if constexpr (tPeriph == peripheral::USART_1) { return gpio::pin::_9; }
                else if constexpr (tPeriph == peripheral::USART_2) { return gpio::pin::_2; }
                else { return gpio::pin::_10; }
            }(),
            .OutputMode = gpio::output_mode::AF_PushPull,
            .OutputSpeed = gpio::output_speed::_50MHz
        };
        template <peripheral tPeriph>
        static constexpr gpio::specification<gpio::pin_type::Input> RxPinSpec {
            .Port = []() consteval noexcept {
                if constexpr (tPeriph == peripheral::USART_1) { return gpio::port::A; }
                else if constexpr (tPeriph == peripheral::USART_2) { return gpio::port::A; }
                else { return gpio::port::B; }
            }(),
            .Pin = []() consteval noexcept {
                if constexpr (tPeriph == peripheral::USART_1) { return gpio::pin::_10; }
                else if constexpr (tPeriph == peripheral::USART_2) { return gpio::pin::_3; }
                else { return gpio::pin::_11; }
            }(),
            .InputMode = gpio::input_mode::PullUp
        };
        template <peripheral tPERIPH>
        static constexpr auto RxDMA_Spec = dma::specification {
            .Channel = details::RxDMA_Channel<tPERIPH>,
            .Direction = dma::direction::PeripheralToMemory,
            .Increment = dma::increment::Memory,
            .MemoryDataAlignment = dma::memory_alignment::Byte,
            .PeripheralDataAlignment = dma::peripheral_alignment::Byte,
            .Mode = dma::mode::Circular,
            .Priority = dma::priority::High
        };
        template <peripheral tPERIPH>
        static constexpr auto TxDMA_Spec = dma::specification {
            .Channel = details::TxDMA_Channel<tPERIPH>,
            .Direction = dma::direction::MemoryToPeripheral,
            .Increment = dma::increment::Memory,
            .MemoryDataAlignment = dma::memory_alignment::Byte,
            .PeripheralDataAlignment = dma::peripheral_alignment::Byte,
            .Mode = dma::mode::Normal,
            .Priority = dma::priority::High
        };
    }

    ////////////////////////////////
    // Concepts
    ////////////////////////////////
    template <typename T>
    concept cTransmittable = 
        (sizeof(T) == 1 or sizeof(T) == 2)
        and std::is_trivially_copyable_v<T>
        and (std::is_convertible_v<T, uint8_t> or std::is_convertible_v<T, uint16_t>);
    
    ////////////////////////////////
    // Specification
    ////////////////////////////////
    struct specification {
        peripheral const Peripheral;
        data_width const DataWidth;
        parity_bit const ParityBit;
        stop_bits const StopBits;
        flow_control const FlowControl;
        transfer_speed const Baud;
        size_t const RxBufferSize = 64;
        size_t const TxBufferSize = 64;
    };

    ////////////////////////////////
    // Module
    ////////////////////////////////
    template <specification tSPEC>
    class module
        : private rcc::clock_handler<details::PCLK<tSPEC.Peripheral>>
        , private system::interrupt<details::IRQ<tSPEC.Peripheral>>
    {
        using kernel = usart::kernel<tSPEC.Peripheral>;
        using irq = system::interrupt<details::IRQ<tSPEC.Peripheral>>;
        using pclk = rcc::clock_handler<details::PCLK<tSPEC.Peripheral>>;
        using tx_pin = gpio::module<details::TxPinSpec<tSPEC.Peripheral>>;
        using rx_pin = gpio::module<details::RxPinSpec<tSPEC.Peripheral>>;
        using rx_dma = dma::module<details::RxDMA_Spec<tSPEC.Peripheral>>;
        using tx_dma = dma::module<details::TxDMA_Spec<tSPEC.Peripheral>>;

    public:
        using data_type = std::conditional_t<tSPEC.DataWidth == data_width::_8bits, uint8_t, uint16_t>;
        using rx_fifo = fifo_buffer<data_type, tSPEC.RxBufferSize>;
        using tx_fifo = fifo_buffer<data_type, tSPEC.TxBufferSize>;

        enum class transfer_mode :uint8_t {
             Blocking
            ,Interrupt
            ,DMA
        };

        enum class error_code :uint8_t {
             None
            ,TimedOut
            ,TxBufferEmpty
            ,RxBufferFull
            ,ParityError
            ,FramingError
        };

    public:
        tx_fifo TxBuffer;
        rx_fifo RxBuffer;

        callback RxComplete;
        callback TxComplete;
        delegate<void(data_type const)> RxStream;

    public:
        module() noexcept
            : pclk()
            , irq(irq::callback::template Create<module, &module::isr>(*this), 1_u8)
            , mRxBusy(false)
            , mRxBusyDMA(false)
            , mTxBusy(false)
            , mTxBusyDMA(false)
            , mRxLength(0)
            , mRxDMA_Pos(0)
            , mRxDMA_Buffer{0}
            , mTxDMA_Buffer{0}
        {
            mTxDMA.TransferComplete.template Set<module, &module::end_dma_tx>(*this);

            kernel::Configure(tSPEC.DataWidth, tSPEC.ParityBit, tSPEC.StopBits, tSPEC.FlowControl, tSPEC.Baud);
            kernel::State(ENABLED);
        }
        ~module() noexcept { kernel::State(DISABLED); }

        expected<size_t, error_code> Transmit(data_type const value) noexcept
        {
            kernel::TxState(ENABLED);
            while (not kernel::template Flag<flag::TXE>());
            kernel::WriteData(value);
            while (not kernel::template Flag<flag::TC>());
            kernel::TxState(DISABLED);
            return 1u;
        }
        expected<size_t, error_code> Transmit() noexcept
        {
            if (TxBuffer.empty()) [[unlikely]]
                return MakeUnexpected(error_code::TxBufferEmpty);
            
            size_t pos;
            system::timer watchdog(constants::Timeout, true);
            kernel::TxState(ENABLED);
            for (pos = 0; pos < TxBuffer.size(); ++pos) {
                if (not wait_for_flag_state<flag::TXE>(ENABLED, watchdog)) {
                    kernel::TxState(DISABLED);
                    return MakeUnexpected(error_code::TimedOut);
                }
                if (auto const res{ TxBuffer.pop() }; res.has_value()) {
                    kernel::WriteData(res);
                }
                else {
                    return MakeUnexpected(error_code::TxBufferEmpty);
                }
            }
            wait_for_flag_state<flag::TC>(ENABLED);
            kernel::TxState(DISABLED);
            return pos;
        }

        size_t Receive(data_type& rx_data) noexcept
        {
            kernel::RxState(ENABLED);
            while (not kernel::template FlagState<flag::RXNE>());
            rx_data = kernel::ReadData();
            kernel::RxState(DISABLED);
            return 1;
        }
        size_t Receive(size_t const nbytes) noexcept
        {
            kernel::RxState(ENABLED);
            for (size_t i = 0; i < nbytes; ++i) {
                if (not wait_for_flag_state<flag::RXNE>(ENABLED)) {
                    kernel::RxState(DISABLED);
                    return 0;
                }
                RxBuffer.push(kernel::ReadData());
            }
            kernel::RxState(DISABLED);
            return nbytes;
        }
        template <transfer_mode tXFER>
        requires (tXFER == transfer_mode::Interrupt)
        status StartTransmitting() noexcept
        {
            if (mTxBusy or mTxBusyDMA) [[unlikely]]
                return status::Busy;
            if (TxBuffer.empty()) [[unlikely]]
                return status::Error;

            kernel::TxState(ENABLED);
            if (not wait_for_flag_state<flag::TXE>(ENABLED)) {
                kernel::TxState(DISABLED);
                return status::TimedOut;
            }
            if (auto const res{ TxBuffer.pop() }; res.has_value()) {
                kernel::WriteData(*res);
                if (not TxBuffer.empty()) {
                    mTxBusy = true;
                    kernel::template InterruptState<interrupt::TXE>(ENABLED);
                }
                else {
                    kernel::template InterruptState<interrupt::TC>(ENABLED);
                }
                return status::OK;
            }
            kernel::TxState(DISABLED);
            return status::Error;
        }
        template <transfer_mode tXFER>
        requires (tXFER == transfer_mode::DMA)
        status StartTransmitting() noexcept
        {
            if (mTxBusy or mTxBusyDMA) [[unlikely]]
                return status::Busy;
            if (TxBuffer.empty())
                return status::Error;

            mTxBusyDMA = true;
            kernel::TxState(ENABLED);
            mTxDMA.Start(reinterpret_cast<uintptr_t>(TxBuffer.data()), kernel::DataRegisterAddress(), TxBuffer.size());
            kernel::template ClearFlag<flag::TC>();
            kernel::TxDMA(ENABLED);
            return status::OK;
        }

        template <transfer_mode tXFER>
        requires (tXFER == transfer_mode::Interrupt)
        status StartReceiving() noexcept
        {
            if (mRxBusy or mRxBusyDMA) [[unlikely]]
                return status::Busy;

            mRxBusy = true;
            RxBuffer.clear();
            kernel::template InterruptState<interrupt::IDLE>(ENABLED);
            kernel::template InterruptState<interrupt::RXNE>(ENABLED);
            kernel::RxState(ENABLED);
            return status::OK;
        }
        template <transfer_mode tXFER>
        requires (tXFER == transfer_mode::DMA)
        status StartReceiving() noexcept
        {
            if (mRxBusy or mRxBusyDMA) [[unlikely]]
                return status::Busy;

            mRxBusyDMA = true;
            kernel::RxState(ENABLED);
            mRxDMA.Start(kernel::DataRegisterAddress(), reinterpret_cast<uintptr_t>(mRxDMA_Buffer), tSPEC.RxBufferSize);
            kernel::template ClearFlag<flag::ORE>();
            kernel::RxDMA(ENABLED);
            kernel::template InterruptState<interrupt::IDLE>(ENABLED);
            return status::OK;
        }

    private:
        INLINE void isr() noexcept
        {
            if (kernel::template FlagState<flag::RXNE>()
                and kernel::template InterruptState<interrupt::RXNE>()
                and mRxBusy)
            {
                auto rx = kernel::ReadData();
                if (not RxStream.CallIf(rx))
                    RxBuffer.push(rx);
            }
            else if (kernel::template FlagState<flag::IDLE>()
                and kernel::template InterruptState<interrupt::IDLE>()
                and (mRxBusy or mRxBusyDMA))
            {
                kernel::template ClearFlag<flag::IDLE>();
                if (mRxBusy) {
                    kernel::RxState(DISABLED);
                    kernel::template InterruptState<interrupt::IDLE>(DISABLED);
                    kernel::template InterruptState<interrupt::RXNE>(DISABLED);
                    mRxBusy = false;
                }
                else {
                    uint16_t curr_pos = tSPEC.RxBufferSize - mRxDMA.DataCounter();

                    if (curr_pos != mRxDMA_Pos) {
                        if (curr_pos > mRxDMA_Pos) {
                            // Buffer is linear
                            mRxLength = curr_pos - mRxDMA_Pos;
                            for (size_t i = 0; i < mRxLength; ++i)
                                RxBuffer.push(mRxDMA_Buffer[mRxDMA_Pos + i]);
                        }
                        else {
                            // Buffer wraps
                            uint16_t len1 = tSPEC.RxBufferSize - mRxDMA_Pos;
                            uint16_t len2 = curr_pos;
                            for (size_t i = 0; i < len1; ++i)
                                RxBuffer.push(mRxDMA_Buffer[mRxDMA_Pos + i]);
                        
                            for (size_t i = 0; i < len2; ++i)
                                RxBuffer.push(mRxDMA_Buffer[i]);
                        
                            mRxLength = len1 + len2;
                        }
                        mRxDMA_Pos = curr_pos;
                    }
                }
                RxComplete();
            }
            else if (kernel::template FlagState<flag::TXE>()
                and kernel::template InterruptState<interrupt::TXE>()
                and mTxBusy)
            {
                if (auto const res{ TxBuffer.pop() }; res.has_value()) {
                    kernel::WriteData(*res);
                    if (TxBuffer.empty()) {
                        //end_tx();
                        kernel::template InterruptState<interrupt::TC>(ENABLED);
                        kernel::template InterruptState<interrupt::TXE>(DISABLED);
                    }
                }
                else {
                    kernel::TxState(DISABLED);
                    kernel::template InterruptState<interrupt::TXE>(DISABLED);
                    mTxBusy = false;
                }
            }
            else if (kernel::template FlagState<flag::TC>()
                and kernel::template InterruptState<interrupt::TC>()
                and (mTxBusy or mTxBusyDMA))
             {
                kernel::TxState(DISABLED);
                kernel::template InterruptState<interrupt::TC>(DISABLED);
                mTxBusy = false;
                mTxBusyDMA = false;
                TxComplete();
            }
        }
        template <flag tFLAG>
        INLINE bool wait_for_flag_state(state const state, system::timer const& watch_dog = {constants::Timeout, true}) noexcept
        {
            while (kernel::template FlagState<tFLAG>() != state) {
                if (watch_dog.IsExpired())
                    return false;
            }
            return true;
        }
        INLINE void end_tx() noexcept
        {
            wait_for_flag_state<flag::TC>(ENABLED);
            kernel::TxState(DISABLED);
        }
        INLINE void end_dma_tx() noexcept
        {
            kernel::TxDMA(DISABLED);
            kernel::template InterruptState<interrupt::TC>(ENABLED);
        }

    private:
        tx_pin mTxPin;
        rx_pin mRxPin;
        rx_dma mRxDMA;
        tx_dma mTxDMA;

        bool volatile mRxBusy;
        bool volatile mRxBusyDMA;
        bool volatile mTxBusy;
        bool volatile mTxBusyDMA;

        uint16_t volatile mRxLength;
        uint16_t volatile mRxDMA_Pos;
        uint8_t mRxDMA_Buffer[tSPEC.RxBufferSize];
        uint8_t mTxDMA_Buffer[tSPEC.TxBufferSize];
    };
} // namespace hal::usart