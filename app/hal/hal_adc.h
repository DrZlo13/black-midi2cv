#pragma once
#include <stdint.h>
#include "helper.h"
#include <stm32f4xx_ll_adc.h>
#include <string>

class HalAdc {
private:
    ADC_TypeDef* adc;
    DISALLOW_COPY_AND_ASSIGN(HalAdc);

public:
    enum class Channel : uint32_t {
        CH1 = LL_ADC_CHANNEL_1,
        CH2 = LL_ADC_CHANNEL_2,
        CH3 = LL_ADC_CHANNEL_3,
        CH4 = LL_ADC_CHANNEL_4,
        CH5 = LL_ADC_CHANNEL_5,
        CH6 = LL_ADC_CHANNEL_6,
        CH7 = LL_ADC_CHANNEL_7,
        CH8 = LL_ADC_CHANNEL_8,
        CH9 = LL_ADC_CHANNEL_9,
        CH10 = LL_ADC_CHANNEL_10,
        CH11 = LL_ADC_CHANNEL_11,
        CH12 = LL_ADC_CHANNEL_12,
        CH13 = LL_ADC_CHANNEL_13,
        CH14 = LL_ADC_CHANNEL_14,
        CH15 = LL_ADC_CHANNEL_15,
        CH16 = LL_ADC_CHANNEL_16,
        CH17 = LL_ADC_CHANNEL_17,
        CH18 = LL_ADC_CHANNEL_18,
        VREFINT = LL_ADC_CHANNEL_VREFINT,
        TEMPSENSOR = LL_ADC_CHANNEL_TEMPSENSOR,
        VBAT = LL_ADC_CHANNEL_VBAT,
    };

    HalAdc(ADC_TypeDef* adc);
    ~HalAdc();

    void enable(void);

    void setup_channel(Channel channel);
    void start_conversion(void);
    bool is_conversion_complete(void);
    uint32_t get_value(void);
};