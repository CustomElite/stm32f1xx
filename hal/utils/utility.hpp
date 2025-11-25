#pragma once

#include <cstdint>
#include <type_traits>

#include "include/delegate.hpp"

#define INLINE inline __attribute__((always_inline))

#ifndef NVIC_PRIORITYGROUP_0
    #define NVIC_PRIORITYGROUP_0    ((uint32_t)0x00000007)     /*!< 0 bit  for pre-emption priority, 4 bits for subpriority */
    #define NVIC_PRIORITYGROUP_1    ((uint32_t)0x00000006)     /*!< 1 bit  for pre-emption priority, 3 bits for subpriority */
    #define NVIC_PRIORITYGROUP_2    ((uint32_t)0x00000005)     /*!< 2 bits for pre-emption priority, 2 bits for subpriority */
    #define NVIC_PRIORITYGROUP_3    ((uint32_t)0x00000004)     /*!< 3 bits for pre-emption priority, 1 bit  for subpriority */
    #define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)     /*!< 4 bits for pre-emption priority, 0 bit  for subpriority */
#endif

template <typename T> requires(std::is_enum_v<T>)
constexpr auto EnumValue(T const value) noexcept { return static_cast<std::underlying_type_t<T>>(value); }

constexpr uint8_t operator""_u8(unsigned long long const value) noexcept { return (uint8_t)value;}
constexpr uint16_t operator""_u16(unsigned long long const value) noexcept { return (uint16_t)value; }
constexpr uint32_t operator""_u32(unsigned long long const value) noexcept { return (uint32_t)value; }
constexpr uint32_t operator""_Hz(unsigned long long const value) noexcept { return (uint32_t)value; }
constexpr uint32_t operator""_kHz(unsigned long long const value) noexcept { return (uint32_t)(value * 1'000ul); }
constexpr uint32_t operator""_MHz(unsigned long long const value) noexcept { return (uint32_t)(value * 1'000'000ul); }
constexpr float operator""_us(unsigned long long const value) noexcept { return (float)(value) / 1'000.f; }
constexpr uint32_t operator""_mS(unsigned long long const value) noexcept { return (uint32_t)(value); }
constexpr uint32_t operator""_Sec(unsigned long long const value) noexcept { return (uint32_t)(value * 1'000ul); }
constexpr uint32_t operator""_Min(unsigned long long const value) noexcept { return (uint32_t)(value * 60'000ul); }
constexpr uint32_t operator""_Hr(unsigned long long const value) noexcept { return (uint32_t)(value * 3'600'000ul); }

namespace hal {

    enum state : bool {
         DISABLED = false
        ,ENABLED = true
    };

    enum class status :int8_t {
         TimedOut = -2
        ,Error = -1
        ,Busy = 0
        ,OK = 1
    };

    using callback = delegate<void()>;

    namespace constants {

        static constexpr uint32_t HSI_ClockFrequency{ 8'000'000ul };
        static constexpr uint32_t LSI_ClockFrequency{ 40'000ul };

        static constexpr uint32_t Timeout{ 1000_mS };
    }
}
