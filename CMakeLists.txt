cmake_minimum_required(VERSION 3.22)

project(STM32F103XX
    VERSION 1.1.0
    LANGUAGES ASM C CXX
)

set(STM32_MCU STM32F103xB)
set(STM32_FAMILY STM32F1xx)

#STM32 CPU Flags
set(STM32_Flags
    -march=armv7-m
    -mcpu=cortex-m3
    -mthumb
    -mfloat-abi=soft
)

#STM32F103 (BluePill) Macros
set(STM32_Defines
    ${STM32_MCU}
    HSE_VALUE=8000000
    LSE_VALUE=32768
    HSI_VALUE=8000000
    LSI_VALUE=40000
    VDD_VALUE=3300
    PREFETCH_ENABLE=1
    $<$<CONFIG:Debug>:DEBUG>
)

#STM32F103XX Linker Script
set(STM32_Linker_Script
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/STM32F103XX_FLASH.ld
)
set(STM32_Startup_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/startup_stm32f103xb.s
)
set(STM32_Required_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMSIS/st/src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMSIS/st/src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMSIS/st/src/system_stm32f1xx.c
)
set(STM32_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMSIS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMSIS/st/include
)

# Interface library for includes and symbols
add_library(${PROJECT_NAME}_AppInterface INTERFACE)
add_library(${PROJECT_NAME}::AppInterface ALIAS ${PROJECT_NAME}_AppInterface)

target_compile_options(${PROJECT_NAME}_AppInterface INTERFACE ${STM32_Flags})
target_compile_definitions(${PROJECT_NAME}_AppInterface INTERFACE ${STM32_Defines})
target_include_directories(${PROJECT_NAME}_AppInterface INTERFACE ${STM32_Include_Dirs})
target_link_options(${PROJECT_NAME}_AppInterface INTERFACE
    -T${STM32_Linker_Script}
    ${STM32_Flags}
)
target_sources(${PROJECT_NAME}_AppInterface INTERFACE 
    ${STM32_Startup_Src}
    ${STM32_Required_Src}
)

# HAL Library
add_library(${PROJECT_NAME}_HAL STATIC)
add_library(${PROJECT_NAME}::HAL ALIAS ${PROJECT_NAME}_HAL)
target_include_directories(${PROJECT_NAME}_HAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hal)
target_compile_options(${PROJECT_NAME}_HAL PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>: -fconcepts-diagnostics-depth=10>
)
target_link_libraries(${PROJECT_NAME}_HAL PUBLIC ${PROJECT_NAME}::AppInterface)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)