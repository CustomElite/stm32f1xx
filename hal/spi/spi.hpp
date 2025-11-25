#pragma once

#include <cerrno>
#include <concepts>
#include <span>
#include <type_traits>

#include "stm32f103xb.h"
#include "utils/utility.hpp"
#include "system/interrupt.hpp"
#include "system/tick.hpp"

#include "rcc/rcc.hpp"
#include "gpio/gpio.hpp"

#include "spi_kernel.hpp"

namespace hal::spi {

    namespace details {
        template <peripheral tPeriph>
        constexpr auto PCLKn = []() consteval noexcept {
            if constexpr (tPeriph == peripheral::SPI_1) return rcc::pclk2::SPI_1;
            else return rcc::pclk1::SPI_2;
        }();
        template <peripheral tPeriph>
        constexpr auto IRQn = []() consteval noexcept {
            if constexpr (tPeriph == peripheral::SPI_1) return system::peripheral_irq::SPI_1;
            else return system::peripheral_irq::SPI_2;
        }();
        template <peripheral tPeriph>
        static constexpr gpio::specification<gpio::pin_type::Output> sclkPinSpec {
            .Port = []() consteval noexcept {
                if constexpr (tPeriph == peripheral::SPI_1) return gpio::port::A;
                else return gpio::port::B;
            }(),
            .Pin = []() consteval noexcept {
                if constexpr (tPeriph == peripheral::SPI_1) return gpio::pin::_5;
                else return gpio::pin::_13;
            }(),
            .OutputMode = gpio::output_mode::AF_PushPull,
            .OutputSpeed = gpio::output_speed::_50MHz,
        };
        template <peripheral tPeriph>
        static constexpr auto misoPinSpec = []() consteval noexcept {
            return gpio::specification<gpio::pin_type::Input> {
                .Port = []() consteval noexcept {
                    if constexpr (tPeriph == peripheral::SPI_1) return gpio::port::A;
                    else return gpio::port::B;
                }(),
                .Pin = []() consteval noexcept {
                    if constexpr (tPeriph == peripheral::SPI_1) return gpio::pin::_6;
                    else return gpio::pin::_14;
                }(),
                .InputMode = gpio::input_mode::Floating
            };
        }();
        template <peripheral tPeriph>
        static constexpr auto mosiPinSpec = []() consteval noexcept {
            return gpio::specification<gpio::pin_type::Output> {
                .Port = []() consteval noexcept {
                    if constexpr (tPeriph == peripheral::SPI_1) return gpio::port::A;
                    else return gpio::port::B;
                }(),
                .Pin = []() consteval noexcept {
                    if constexpr (tPeriph == peripheral::SPI_1) return gpio::pin::_7;
                    else return gpio::pin::_15;
                }(),
                .OutputMode = gpio::output_mode::AF_PushPull,
                .OutputSpeed = gpio::output_speed::_50MHz,
            };
        }();
    }

    struct specification {
        peripheral const Peripheral;
        mode const Mode = mode::Master;
        data_width const DataWidth = data_width::_8bit;
        data_direction const DataDirection = data_direction::TxRx;
        bit_order const BitOrder = bit_order::MSB;
        slave_management const SlaveSelect = slave_management::SoftwareSelected;
        clock_polarity const ClockPolarity = clock_polarity::Low;
        clock_phase const ClockPhase = clock_phase::LeadingEdge;
        clock_prescaler const ClockPrescaler = clock_prescaler::Div2;
    };

    template <typename T>
    concept cValidDataType = 
        std::unsigned_integral<T> and
        (sizeof(T) == 1 or sizeof(T) == 2 or sizeof(T) == 4);

    template <specification tSPEC>
    class module
        : private rcc::clock_handler<details::PCLKn<tSPEC.Peripheral>>
        , private system::interrupt<details::IRQn<tSPEC.Peripheral>>
    {
        //static_assert(tSPEC.TransferMode != transfer_mode::TxOnly and tSPEC.Mode != mode::Slave, "Invalid SPI configuration");

        using kernel = spi::kernel<tSPEC.Peripheral>;
        using irq = system::interrupt<details::IRQn<tSPEC.Peripheral>>;
        using pclk = rcc::clock_handler<details::PCLKn<tSPEC.Peripheral>>;
        
        using sclk_pin = gpio::module<details::sclkPinSpec<tSPEC.Peripheral>>;

    public:
        using miso_pin = std::conditional_t<
            (tSPEC.DataDirection == data_direction::TxOnly),
            gpio::null_pin,
            gpio::module<details::misoPinSpec<tSPEC.Peripheral>>
        >;
        using mosi_pin = std::conditional_t<
            (tSPEC.DataDirection == data_direction::RxOnly),
            gpio::null_pin,
            gpio::module<details::mosiPinSpec<tSPEC.Peripheral>>
        >;
        using data_type = std::conditional_t<tSPEC.DataWidth == data_width::_8bit, uint8_t, uint16_t>;

    public:
        using callback = delegate<void()>;

        enum transfer_type :bool {
            BLOCKING
           ,INTERRUPT
        };

        class payload_buffer {
            static constexpr auto STEP = (tSPEC.BitOrder == bit_order::MSB) ? -1 : 1;
            
        public:
            payload_buffer() noexcept = default;
            payload_buffer(data_type* ptr, size_t const size) noexcept
                : mPtr((tSPEC.BitOrder == bit_order::MSB) ? ptr + (size - 1) : ptr)
                , mSize(size)
            {}
            payload_buffer(payload_buffer const& rhs) noexcept = default;
            payload_buffer(payload_buffer&& rhs) noexcept = default;
            payload_buffer& operator=(payload_buffer const& rhs) noexcept = default;
            payload_buffer& operator=(payload_buffer&& rhs) noexcept = default;
            ~payload_buffer() noexcept = default;

            size_t Size() const noexcept { return mSize; }
            data_type* Data() const noexcept { return mPtr; }
            bool IsValid() const noexcept { return mPtr != nullptr; }
            data_type& operator*() const noexcept { return *mPtr; }
            payload_buffer& operator++() noexcept
            {
                mPtr += mPtr ? STEP : 0;
                mSize -= mSize ? 1 : 0;
                return *this;
            }
            payload_buffer operator++(int) noexcept { payload_buffer tmp = *this; ++(*this); return tmp; }
            operator bool() const noexcept { return mSize > 0; }
            bool operator==(payload_buffer const& rhs) const noexcept { return (mPtr == rhs.mPtr) and (mSize == rhs.mSize); }
            bool operator!=(payload_buffer const& rhs) const noexcept { return not (*this == rhs); }

        private:
            data_type* volatile mPtr = nullptr;
            size_t volatile mSize = 0;
        };

    public:
        module() noexcept
            : pclk()
            , irq(irq::callback::template Create<module, &module::isr>(*this), 1_u8, DISABLED)
        {
            kernel::Configure(tSPEC.Mode, tSPEC.DataWidth, tSPEC.BitOrder
                ,tSPEC.SlaveSelect, tSPEC.ClockPolarity, tSPEC.ClockPhase, tSPEC.ClockPrescaler);
            kernel::State(ENABLED);
        }
        ~module() noexcept {}

        template <transfer_type tXFER>
        status Transfer(data_type* tx_data_ptr, data_type* rx_data_ptr, size_t const size, uint32_t timeout = 250_mS) noexcept
        requires (tSPEC.DataDirection == data_direction::TxRx)
        {
            if (mBusy)
                return status::Busy;

            if constexpr (tXFER == BLOCKING)
                return blocking_transfer({tx_data_ptr, size}, {rx_data_ptr, size}, timeout);

            return status::Error;
        }
        template <transfer_type tXFER>
        status Transfer(data_type* tx_data_ptr, size_t const size, uint32_t timeout = 250_mS) noexcept
        requires (tSPEC.DataDirection == data_direction::TxOnly)
        {
            if (mBusy)
                return status::Busy;

            if constexpr (tXFER == BLOCKING)
                return blocking_transfer({tx_data_ptr, size}, {nullptr, size}, timeout);

            return status::Error;
        }
        template <transfer_type tXFER>
        status Transfer(data_type* rx_data_ptr, size_t const size, uint32_t timeout = 250_mS) noexcept
        requires (tSPEC.DataDirection == data_direction::RxOnly)
        {
            //if (mBusy)
            //    return status::Busy;

            if constexpr (tXFER == BLOCKING)
                return blocking_transfer({nullptr, size}, {rx_data_ptr, size}, timeout);

            return status::Error;
        }

        sclk_pin SCLK;
        miso_pin MISO;
        mosi_pin MOSI;

    private:
        INLINE void isr() noexcept {}

        status blocking_transfer(payload_buffer tx, payload_buffer rx, uint32_t timeout = 250_mS) noexcept
        {
            system::timer watch_dog(timeout, true);

            for (; rx and tx; ++rx, ++tx) {
                if (wait_for_flag_state<flag::TXE>(ENABLED, watch_dog) != status::OK)
                    return status::TimedOut;

                kernel::WriteData(tx.IsValid() ? *tx : data_type{0});
                if (wait_for_flag_state<flag::RXNE>(ENABLED, watch_dog) != status::OK)
                    return status::TimedOut;

                if (rx.IsValid())
                    *rx = kernel::ReadData();
                else
                    (void)kernel::ReadData();
            }
            if (wait_for_flag_state<flag::BSY>(DISABLED, watch_dog) != status::OK)
                return status::TimedOut;

            return status::OK;
        }
        template <flag tFLAG>
        status wait_for_flag_state(state const state, system::timer& watch_dog) noexcept
        {
            while (kernel::template FlagState<tFLAG>() != state) {
                if (watch_dog.IsExpired())
                    return status::TimedOut;
            }
            return status::OK;
        }

    private:
        volatile bool mBusy = false;
        payload_buffer mTxData;
        payload_buffer mRxData;
    };
}