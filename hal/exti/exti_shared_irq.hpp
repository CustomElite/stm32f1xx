#pragma once

#include <atomic>

#include "system/interrupt.hpp"

#include "exti_kernel.hpp"

namespace hal::exti {

    template <line tLINE>
    concept cSharedIRQ_9_5 = (tLINE >= line::_5 and tLINE <= line::_9);
    template <line tLINE>
    concept cSharedIRQ_15_10 = (tLINE >= line::_10 and tLINE <= line::_15);

    template <line tLINE>
    concept cUniqueIRQ = tLINE < line::_5 or cEventLine<tLINE>;

    template <line tLINE>
    concept cSharedIRQ = cSharedIRQ_9_5<tLINE> or cSharedIRQ_15_10<tLINE>;

    template <system::cValidIRQ auto tIRQ>
    requires (tIRQ == system::peripheral_irq::EXTI_9_5 or tIRQ == system::peripheral_irq::EXTI_15_10)
    class shared_irq
        : private system::interrupt<tIRQ>
    {
        using irq = system::interrupt<tIRQ>;

        template <line tLINE>
        requires (cSharedIRQ<tLINE>)
        static constexpr auto LINEn = []() consteval noexcept {
            if constexpr (tIRQ == system::peripheral_irq::EXTI_9_5) return (EnumValue(tLINE) - 5u) % 5u;
            else return (EnumValue(tLINE) - 10u) % 6u;
        }();
        static constexpr auto COUNT = []() consteval noexcept {
            if constexpr (tIRQ == system::peripheral_irq::EXTI_9_5) return 5u;
            else return 6u;
        }();

    protected:
        using callback = irq::callback;
        
        shared_irq() noexcept
            : irq(callback::template Create<shared_irq::dispatch_to_lines>(), 2_u8)
        {
        }
        template <line tLINE>
        requires (cSharedIRQ<tLINE>)
        static void SetCallback(callback const& func) noexcept { sLineISRs[LINEn<tLINE>] = func; }
        
        template <line tLINE>
        requires (cSharedIRQ<tLINE>)
        static void ClearCallback() noexcept { sLineISRs[LINEn<tLINE>].Clear(); }

        template <line tLINE>
        requires (cSharedIRQ<tLINE>)
        static void State(state const state) noexcept
        {
            auto enabled = sLineFlags.load(std::memory_order_acquire);

            if (state) {
                enabled |= (1 << LINEn<tLINE>);
                sLineFlags.store(enabled, std::memory_order_release);
                if (not sInterruptEnabled) {
                    irq::State(ENABLED);
                    sInterruptEnabled = true;
                }
            }
            else {
                enabled &= ~(1 << LINEn<tLINE>);
                sLineFlags.store(enabled, std::memory_order_release);
                if (not enabled) {
                    irq::State(DISABLED);
                    sInterruptEnabled = false;
                }
            }
        }

    private:
        static void dispatch_to_lines() noexcept
        {
            auto const enabled = sLineFlags.load(std::memory_order_acquire);

            for (uint8_t i = 0; i < COUNT; ++i) {
                if (enabled & (1 << i))
                    sLineISRs[i]();
            }
        }

        inline static bool sInterruptEnabled{ false };
        inline static std::atomic<uint8_t> sLineFlags{ 0 };
        inline static callback sLineISRs[COUNT]{};

    private:
        shared_irq(shared_irq const&) noexcept = delete;
        shared_irq(shared_irq&&) noexcept = delete;
        shared_irq& operator=(shared_irq const&) noexcept = delete;
        shared_irq& operator=(shared_irq&&) noexcept = delete;
    };
}