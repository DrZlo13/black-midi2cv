# version
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify cross compilers and tools
set(TOOLCHAIN_BIN_PATH "C:/work/cmake-stm32/arm_toolchain/gcc-arm-none-eabi-10-2020-q4-major/bin")

set(STM32_TARGET_TRIPLET arm-none-eabi)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET "black-pill")
set(TARGET_PATH "target/${TARGET}")

find_program(CMAKE_C_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_AR NAMES ${STM32_TARGET_TRIPLET}-ar PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJCOPY NAMES ${STM32_TARGET_TRIPLET}-objcopy PATHS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${STM32_TARGET_TRIPLET}-objdump PATHS ${TOOLCHAIN_BIN_PATH})
find_program(SIZE NAMES ${STM32_TARGET_TRIPLET}-size PATHS ${TOOLCHAIN_BIN_PATH})

# project start
PROJECT("Black-MIDI2CV" C CXX ASM)

add_subdirectory(${TARGET_PATH})

include_directories(${INCLUDE_DIRECTORIES})
include_directories(${CMAKE_SOURCE_DIR}/app)
include_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB CORE_SOURCES
    "app/*.c"
    "app/*.cpp"
    "app/*/*.c"
    "app/*/*.cpp"
    "app/*/*/*.c"
    "app/*/*/*.cpp"
    "app/*/*/*/*/*.c"
    "app/*/*/*/*/*.cpp"
)

set(SOURCES
    ${SOURCES}
    ${CORE_SOURCES}
)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -Og
    -g
    -Wall
    -ffunction-sections
    -fdata-sections
    -fstack-usage
    -fno-math-errno
    -MMD
    -MP
    --specs=nano.specs
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -u _printf_float

    # -Werror
    # -Wextra
    $<$<COMPILE_LANGUAGE:C>:-std=gnu2x>
    $<$<COMPILE_LANGUAGE:C>:-Wenum-conversion>
    $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++20>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m4
    -T ${LINKER_SCRIPT}
    --specs=nosys.specs
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
    -Wl,--gc-sections -static
    -Wl,--undefined=uxTopUsedPriority
    --specs=nano.specs
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -mthumb
    -fno-math-errno
    -u _printf_float
    -fstack-usage
    -Wl,--start-group -lstdc++ -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage -T ${LINKER_SCRIPT}

    # -Wl,--wrap,_malloc_r
    # -Wl,--wrap,_free_r
    # -Wl,--wrap,_calloc_r
    # -Wl,--wrap,_realloc_r
)

set(COMMON_FLAGS
    USE_HAL_DRIVER
)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S")

set(INFO_FLAGS
    GIT_BRANCH="${GIT_BRANCH}"
    GIT_COMMIT="${GIT_COMMIT}"

    BUILD_DATE="${BUILD_DATE}"
    TARGET=${FTARGET}
)

message(STATUS "Branch: ${GIT_BRANCH}")
message(STATUS "Commit: ${GIT_COMMIT}")
message(STATUS "Date  : ${BUILD_DATE}")

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    ${TARGET_FLAGS}
    ${COMMON_FLAGS}
    ${INFO_FLAGS}
)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE CXX)

# Print size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${SIZE} ${PROJECT_NAME}.elf)

# Make binary file.
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
    COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")

# Generate assembly listing.
add_custom_command(
    TARGET ${PROJECT_NAME}.elf
    COMMAND rm -f ${PROJECT_NAME}.lst
    COMMAND ${CMAKE_OBJDUMP}
    ARGS "-S" "${PROJECT_NAME}.elf" ">>" "${PROJECT_NAME}.lst")

# Flash firmware to target.
add_custom_target(flash
    STM32_Programmer_CLI --connect port=swd freq=24000 --write ${BIN_FILE} 0x08000000 --verify --start 0x08000000
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing target hardware")
