#include "settings.h"
#include "debug.h"

static const char* TAG = "Settings";

constexpr uint32_t SETTINGS_MAGIC = 'S' << 24 | 'E' << 16 | 'T' << 8 | 'I';
constexpr uint32_t SETTINGS_VERSION = 1;

#pragma pack(push, 4)
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t checksum;
    uint32_t size;
} SettingsHeader;

typedef struct {
    SettingsHeader header;
    Settings settings;
} SettingsData;
#pragma pack(pop)

static HalFlash flash;
static HalFlash::Sector sector = HalFlash::S7_128k;

static uint32_t checksum(const void* const data, size_t size) {
    uint32_t sum = 0;
    const uint8_t* const ptr = reinterpret_cast<const uint8_t* const>(data);
    for(size_t i = 0; i < size; i++) {
        sum += ptr[i];
    }
    return sum;
}

static SettingsData read_data_at_offset(int32_t offset) {
    SettingsData data;
    uint32_t* const ptr = reinterpret_cast<uint32_t* const>(&data);
    for(size_t i = 0; i < sizeof(SettingsData) / sizeof(uint32_t); i++) {
        ptr[i] = flash.read(sector, offset + i * sizeof(uint32_t));
    }
    return data;
}

static int32_t get_last_valid_offset() {
    Debug::info(TAG, "Searching for last valid settings");
    size_t sector_size = flash.get_sector_size(sector);
    int32_t offset = sector_size - sizeof(SettingsData);
    // make sure offset can be zero
    offset = offset / sizeof(SettingsData) * sizeof(SettingsData);

    while(offset >= 0) {
        SettingsData data = read_data_at_offset(offset);

        if(data.header.magic != 0xffffffff) {
            Debug::info(
                TAG,
                "looking at offset %d, magic = %x, version = %d, checksum = %x, size = %d",
                offset,
                data.header.magic,
                data.header.version,
                data.header.checksum,
                data.header.size);
        }

        if(data.header.magic == SETTINGS_MAGIC && data.header.version == SETTINGS_VERSION &&
           data.header.size == sizeof(Settings)) {
            if(data.header.checksum == checksum(&data.settings, sizeof(Settings))) {
                Debug::info(TAG, "Found valid settings at offset %d", offset);
                return offset;
            }
        }
        offset -= sizeof(SettingsData);
    }

    Debug::error(TAG, "No valid settings found");
    return -1;
}

SettingsManager::SettingsManager() {
}

SettingsManager::~SettingsManager() {
}

bool SettingsManager::load(Settings* settings, const Settings* const defaults) {
    int32_t offset = get_last_valid_offset();
    if(offset < 0) {
        *settings = *defaults;
        return false;
    }

    SettingsData data = read_data_at_offset(offset);
    *settings = data.settings;
    return true;
}

void SettingsManager::save(const Settings* const settings) {
    int32_t offset = get_last_valid_offset();
    if(offset < 0 ||
       ((size_t)offset + (sizeof(SettingsData) * 2) >= flash.get_sector_size(sector))) {
        Debug::info(TAG, "Erasing sector %d", sector);
        offset = 0;
        flash.erase(sector);
    } else {
        offset += sizeof(SettingsData);
    }

    SettingsData data;
    data.header.magic = SETTINGS_MAGIC;
    data.header.version = SETTINGS_VERSION;
    data.header.checksum = checksum(settings, sizeof(Settings));
    data.header.size = sizeof(Settings);
    data.settings = *settings;

    Debug::info(
        TAG,
        "saving at offset %d, magic = %x, version = %d, checksum = %x, size = %d",
        offset,
        data.header.magic,
        data.header.version,
        data.header.checksum,
        data.header.size);

    uint32_t* ptr = reinterpret_cast<uint32_t*>(&data);
    for(size_t i = 0; i < sizeof(SettingsData) / 4; i++) {
        flash.write(sector, offset + i * 4, ptr[i]);
    }
}
