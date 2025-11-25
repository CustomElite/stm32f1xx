#pragma once

#include <bit>
#include <concepts>
#include <limits>
#include <cstdint>

#include "utility.hpp"

namespace hal {

    template <typename T>
    concept cRegister =
        std::unsigned_integral<T> and
        (std::numeric_limits<T>::digits == 32 or std::numeric_limits<T>::digits == 16 or std::numeric_limits<T>::digits == 8);

    //enum class access :uint8_t {
    //     Read
    //    ,Write
    //    ,ReadWrite
    //};

    template <cRegister auto tADDR>
    struct hardware_register {
        using type = decltype(tADDR);
        using ptr = type volatile * const;

        template <type tBITMASK>
        class bitfield {
            using reg = hardware_register<tADDR>;
            using size_type =
                std::conditional_t<std::numeric_limits<uint8_t>::max() >= tBITMASK, uint8_t,
                std::conditional_t<std::numeric_limits<uint16_t>::max() >= tBITMASK, uint16_t, uint32_t>>;

            static constexpr type sMask = tBITMASK;
            static constexpr type sNMask = ~tBITMASK;
            static constexpr uint8_t sPos = std::countr_zero(tBITMASK);

        public:
            INLINE static void Set() noexcept { reg::Write((reg::Read() & sNMask) | sMask); }
            INLINE static void Reset() noexcept { reg::Write(reg::Read() & sNMask); }
            INLINE static void Toggle() noexcept { reg::Write(reg::Read() ^ sMask); }
            INLINE static void Write(size_type const value) noexcept
            {
                reg::Write((reg::Read() & sNMask) | apply_mask(value));
            }
            INLINE static size_type Read() noexcept { return (reg::Read() & sMask) >> sPos; }

        private:
            INLINE static size_type apply_mask(size_type const value) noexcept { return (value << sPos) & sMask; }
        };

        static constexpr type Address = tADDR;

        template <type tBITMASK>
        consteval static auto CreateBitfield() noexcept { return bitfield<tBITMASK>{}; }
        
        INLINE static type Read() noexcept { return *reinterpret_cast<ptr>(tADDR); }
        INLINE static void Write(type const value) noexcept { *reinterpret_cast<ptr>(tADDR) = value; }
    };
}