#pragma once
#include <stdint.h>
#include "helper.h"
#include <stm32f4xx_ll_gpio.h>

#define GPIO_NUMBER (16U)

class HalGpio {
private:
    GPIO_TypeDef* port;
    uint16_t pin;

    DISALLOW_COPY_AND_ASSIGN(HalGpio);

public:
    enum class Mode {
        Input,
        OutputPushPull,
        OutputOpenDrain,
        AltFunctionPushPull,
        AltFunctionOpenDrain,
        Analog,
        InterruptRise,
        InterruptFall,
        InterruptRiseFall,
        EventRise,
        EventFall,
        EventRiseFall,
    };

    enum class Speed {
        Low,
        Medium,
        High,
        VeryHigh,
    };

    enum class Pull {
        No,
        Up,
        Down,
    };

    enum class AltFn {
        AF0_RTC_50Hz = 0x00, /* RTC_50Hz Alternate Function mapping                       */
        AF0_MCO = 0x00, /* MCO (MCO1 and MCO2) Alternate Function mapping            */
        AF0_TAMPER = 0x00, /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
        AF0_SWJ = 0x00, /* SWJ (SWD and JTAG) Alternate Function mapping             */
        AF0_TRACE = 0x00, /* TRACE Alternate Function mapping                          */

        AF1_TIM1 = 0x01, /* TIM1 Alternate Function mapping */
        AF1_TIM2 = 0x01, /* TIM2 Alternate Function mapping */

        AF2_TIM3 = 0x02, /* TIM3 Alternate Function mapping */
        AF2_TIM4 = 0x02, /* TIM4 Alternate Function mapping */
        AF2_TIM5 = 0x02, /* TIM5 Alternate Function mapping */

        AF3_TIM9 = 0x03, /* TIM9 Alternate Function mapping  */
        AF3_TIM10 = 0x03, /* TIM10 Alternate Function mapping */
        AF3_TIM11 = 0x03, /* TIM11 Alternate Function mapping */

        AF4_I2C1 = 0x04, /* I2C1 Alternate Function mapping */
        AF4_I2C2 = 0x04, /* I2C2 Alternate Function mapping */
        AF4_I2C3 = 0x04, /* I2C3 Alternate Function mapping */

        AF5_SPI1 = 0x05, /* SPI1/I2S1 Alternate Function mapping   */
        AF5_SPI2 = 0x05, /* SPI2/I2S2 Alternate Function mapping   */
        AF5_SPI3 = 0x05, /* SPI3/I2S3 Alternate Function mapping   */
        AF5_SPI4 = 0x05, /* SPI4 Alternate Function mapping        */
        AF5_I2S3ext = 0x05, /* I2S3ext_SD Alternate Function mapping  */

        AF6_SPI2 = 0x06, /* I2S2 Alternate Function mapping       */
        AF6_SPI3 = 0x06, /* SPI3/I2S3 Alternate Function mapping  */
        AF6_SPI4 = 0x06, /* SPI4/I2S4 Alternate Function mapping  */
        AF6_SPI5 = 0x06, /* SPI5/I2S5 Alternate Function mapping  */
        AF6_I2S2ext = 0x06, /* I2S2ext_SD Alternate Function mapping */

        AF7_SPI3 = 0x07, /* SPI3/I2S3 Alternate Function mapping  */
        AF7_USART1 = 0x07, /* USART1 Alternate Function mapping     */
        AF7_USART2 = 0x07, /* USART2 Alternate Function mapping     */
        AF7_I2S3ext = 0x07, /* I2S3ext_SD Alternate Function mapping */

        AF8_USART6 = 0x08, /* USART6 Alternate Function mapping */

        AF9_TIM14 = 0x09, /* TIM14 Alternate Function mapping */
        AF9_I2C2 = 0x09, /* I2C2 Alternate Function mapping  */
        AF9_I2C3 = 0x09, /* I2C3 Alternate Function mapping  */

        AF10_OTG_FS = 0x0A, /* OTG_FS Alternate Function mapping */

        AF12_SDIO = 0x0C, /* SDIO Alternate Function mapping  */

        AF15_EVENTOUT = 0x0F, /* EVENTOUT Alternate Function mapping */

        AF_DISABLE = 0x10, /* No Alternate Function */
    };

    HalGpio(
        GPIO_TypeDef* port,
        uint16_t pin,
        const HalGpio::Mode mode = Mode::Analog,
        const HalGpio::Pull pull = Pull::No,
        const HalGpio::Speed speed = Speed::Low,
        const HalGpio::AltFn alt_fn = AltFn::AF_DISABLE);
    ~HalGpio();

    void config(
        const HalGpio::Mode mode,
        const HalGpio::Pull pull = Pull::No,
        const HalGpio::Speed speed = Speed::Low,
        const HalGpio::AltFn alt_fn = AltFn::AF_DISABLE);
    void toggle(void);
    void write(bool value);
    bool read(void);

    typedef void (*Callback)(void* context);
    void set_interrupt_callback(Callback cb, void* ctx);
};