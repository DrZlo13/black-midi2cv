#pragma once
#include <hal/hal.h>

#pragma pack(push, 4)

typedef struct {
    float dac1_max_voltage;
    float dac2_max_voltage;
} Settings;

#pragma pack(pop)

class SettingsManager {
public:
    SettingsManager();
    ~SettingsManager();
    bool load(Settings* settings, const Settings* const defaults);
    void save(const Settings* const settings);
};