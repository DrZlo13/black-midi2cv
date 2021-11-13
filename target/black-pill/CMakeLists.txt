set(TARGET "target/black-pill")

set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/${TARGET}/STM32F411CEUx_FLASH.ld PARENT_SCOPE)

# All relative to project root
set(TARGET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/${TARGET})
set(CUBE_DIR ${CMAKE_SOURCE_DIR}/lib/STM32CubeWB)

file(GLOB TARGET_SOURCES 
	${TARGET_SOURCE_DIR}/startup_stm32f411xe.s
	${TARGET_SOURCE_DIR}/Core/Src/main.c
	${TARGET_SOURCE_DIR}/Core/Src/stm32f4xx_it.c
	${TARGET_SOURCE_DIR}/Core/Src/stm32f4xx_hal_msp.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
	${TARGET_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
)

# Look here for header files.
set(TARGET_DIRECTORIES 
	${TARGET_SOURCE_DIR}/Core/Inc
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
	${TARGET_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
	${TARGET_SOURCE_DIR}/Drivers/CMSIS/Include
    ${TARGET_SOURCE_DIR}
)

set(TARGET_FLAGS 
	USE_HAL_DRIVER
	STM32F411xE
	PARENT_SCOPE
)

# add include and source to parent scope
set(SOURCES 
    ${SOURCES} 
    ${TARGET_SOURCES} 
    PARENT_SCOPE
)

set(INCLUDE_DIRECTORIES 
    ${INCLUDE_DIRECTORIES} 
    ${TARGET_DIRECTORIES} 
    PARENT_SCOPE
)
