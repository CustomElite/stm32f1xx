#pragma once

#pragma once

#include <cstddef>
#include "stm32f103xb.h"
#include "../utils/hardware_register.hpp"

namespace hal::flash {

struct registers {
    static constexpr std::uintptr_t BASE = FLASH_R_BASE;

    struct acr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, ACR)>{};
        static constexpr auto LATENCY = REG.template CreateBitfield<FLASH_ACR_LATENCY>();
        static constexpr auto LATENCY_0 = REG.template CreateBitfield<FLASH_ACR_LATENCY_0>();
        static constexpr auto LATENCY_1 = REG.template CreateBitfield<FLASH_ACR_LATENCY_1>();
        static constexpr auto LATENCY_2 = REG.template CreateBitfield<FLASH_ACR_LATENCY_2>();
        static constexpr auto HLFCYA = REG.template CreateBitfield<FLASH_ACR_HLFCYA>();
        static constexpr auto PRFTBE = REG.template CreateBitfield<FLASH_ACR_PRFTBE>();
        static constexpr auto PRFTBS = REG.template CreateBitfield<FLASH_ACR_PRFTBS>();
    };

    struct keyr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, KEYR)>{};
        static constexpr auto FKEYR = REG.template CreateBitfield<FLASH_KEYR_FKEYR>();
        static constexpr auto KEY1 = REG.template CreateBitfield<FLASH_KEY1>();
        static constexpr auto KEY2 = REG.template CreateBitfield<FLASH_KEY2>();
    };

    struct optkeyr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, OPTKEYR)>{};
        static constexpr auto OPTKEYR = REG.template CreateBitfield<FLASH_OPTKEYR_OPTKEYR>();
        static constexpr auto OPTKEY1 = REG.template CreateBitfield<FLASH_OPTKEY1>();
        static constexpr auto OPTKEY2 = REG.template CreateBitfield<FLASH_OPTKEY2>();
    };

    struct sr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, SR)>{};
        static constexpr auto BSY = REG.template CreateBitfield<FLASH_SR_BSY>();
        static constexpr auto PGERR = REG.template CreateBitfield<FLASH_SR_PGERR>();
        static constexpr auto WRPRTERR = REG.template CreateBitfield<FLASH_SR_WRPRTERR>();
        static constexpr auto EOP = REG.template CreateBitfield<FLASH_SR_EOP>();
    };

    struct cr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, CR)>{};
        static constexpr auto PG = REG.template CreateBitfield<FLASH_CR_PG>();
        static constexpr auto PER = REG.template CreateBitfield<FLASH_CR_PER>();
        static constexpr auto MER = REG.template CreateBitfield<FLASH_CR_MER>();
        static constexpr auto OPTPG = REG.template CreateBitfield<FLASH_CR_OPTPG>();
        static constexpr auto OPTER = REG.template CreateBitfield<FLASH_CR_OPTER>();
        static constexpr auto STRT = REG.template CreateBitfield<FLASH_CR_STRT>();
        static constexpr auto LOCK = REG.template CreateBitfield<FLASH_CR_LOCK>();
        static constexpr auto OPTWRE = REG.template CreateBitfield<FLASH_CR_OPTWRE>();
        static constexpr auto ERRIE = REG.template CreateBitfield<FLASH_CR_ERRIE>();
        static constexpr auto EOPIE = REG.template CreateBitfield<FLASH_CR_EOPIE>();
    };

    struct ar {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, AR)>{};
        static constexpr auto FAR = REG.template CreateBitfield<FLASH_AR_FAR>();
    };

    struct obr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, OBR)>{};
        static constexpr auto OPTERR = REG.template CreateBitfield<FLASH_OBR_OPTERR>();
        static constexpr auto RDPRT = REG.template CreateBitfield<FLASH_OBR_RDPRT>();
        static constexpr auto IWDG_SW = REG.template CreateBitfield<FLASH_OBR_IWDG_SW>();
        static constexpr auto nRST_STOP = REG.template CreateBitfield<FLASH_OBR_nRST_STOP>();
        static constexpr auto nRST_STDBY = REG.template CreateBitfield<FLASH_OBR_nRST_STDBY>();
        static constexpr auto USER = REG.template CreateBitfield<FLASH_OBR_USER>();
        static constexpr auto DATA0 = REG.template CreateBitfield<FLASH_OBR_DATA0>();
        static constexpr auto DATA1 = REG.template CreateBitfield<FLASH_OBR_DATA1>();
    };

    struct wrpr {
        static constexpr auto REG = hardware_register<BASE + offsetof(FLASH_TypeDef, WRPR)>{};
        static constexpr auto WRP = REG.template CreateBitfield<FLASH_WRPR_WRP>();
    };
};

} // namespace hal::flash