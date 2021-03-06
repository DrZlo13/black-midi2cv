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
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
	#${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
	${TARGET_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usart.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c

	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
	${TARGET_SOURCE_DIR}/USB/usb_interrupts.c
	${TARGET_SOURCE_DIR}/USB/Device/App/usb_device.c
	${TARGET_SOURCE_DIR}/USB/Device/App/usbd_desc.c
	# ${TARGET_SOURCE_DIR}/USB/Device/App/usbd_cdc_if.c
	${TARGET_SOURCE_DIR}/USB/Device/Target/usbd_conf.c
	${TARGET_SOURCE_DIR}/USB/STM32_USB_Device_Library/Core/Src/usbd_core.c
	${TARGET_SOURCE_DIR}/USB/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
	${TARGET_SOURCE_DIR}/USB/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
	# ${TARGET_SOURCE_DIR}/USB/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
	${CMAKE_SOURCE_DIR}/lib/usb_midi_device/usbd_midi.c
	${CMAKE_SOURCE_DIR}/lib/usb_midi_device/usbd_midi_if.c
)

# Look here for header files.
set(TARGET_DIRECTORIES 
	${TARGET_SOURCE_DIR}/Core/Inc
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
	${TARGET_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
	${TARGET_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
	${TARGET_SOURCE_DIR}/Drivers/CMSIS/Include
	${TARGET_SOURCE_DIR}/USB/Device/App
	${TARGET_SOURCE_DIR}/USB/Device/Target
	${TARGET_SOURCE_DIR}/USB/STM32_USB_Device_Library/Core/Inc
	# ${TARGET_SOURCE_DIR}/USB/STM32_USB_Device_Library/Class/CDC/Inc
    ${TARGET_SOURCE_DIR}
)

set(TARGET_FLAGS 
	USE_HAL_DRIVER
	STM32F411xE
	USE_FULL_LL_DRIVER
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
