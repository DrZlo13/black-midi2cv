#include "hal_adc.h"

HalAdc::HalAdc(ADC_TypeDef* adc) {
    this->adc = adc;
}

HalAdc::~HalAdc() {
}

void HalAdc::enable(void) {
    LL_ADC_Enable(ADC1);
}

void HalAdc::setup_channel(Channel channel) {
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
    LL_ADC_Init(ADC1, &ADC_InitStruct);

    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    LL_ADC_REG_SetFlagEndOfConversion(ADC1, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);

    ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);

    uint32_t channel_u32 = static_cast<uint32_t>(channel);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, channel_u32);
    LL_ADC_SetChannelSamplingTime(ADC1, channel_u32, LL_ADC_SAMPLINGTIME_84CYCLES);
}

void HalAdc::start_conversion(void) {
    LL_ADC_REG_StartConversionSWStart(ADC1);
}

uint32_t HalAdc::get_value(void) {
    while(!LL_ADC_IsActiveFlag_EOCS(ADC1)) {
    }

    return LL_ADC_REG_ReadConversionData12(ADC1);
}