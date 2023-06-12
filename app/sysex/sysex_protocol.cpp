#include "sysex_protocol.h"
#include <cstring>
#include <cstdlib>
#include <hl_hal/debug.h>
#include <hl_hal/usb_midi.h>

namespace SysEx {

constexpr uint8_t manufacturer_id[3] = {0x00, 0x21, 0x73};

static bool manufacturer_valid(const uint8_t* data, size_t size) {
    if(size < 3) return false;
    return memcmp(data, manufacturer_id, 3) == 0;
}

static void prepare_and_send(const uint8_t* data, size_t size) {
    size_t buffer_size = size + 5;
    uint8_t* buffer = static_cast<uint8_t*>(malloc(buffer_size));

    // start
    buffer[0] = 0xf0 | Midi::SystemCommonType::SystemExclusive;

    // manufacturer id
    buffer[1] = manufacturer_id[0];
    buffer[2] = manufacturer_id[1];
    buffer[3] = manufacturer_id[2];

    // data
    memcpy(buffer + 4, data, size);

    // end
    buffer[size + 4] = 0xf0 | Midi::SystemCommonType::SysExEnd;

    UsbMidi::send_sysex(0, buffer, buffer_size);

    free(buffer);
}

void send_reply(Command command, Parameter parameter, const uint8_t value[], size_t size) {
    Debug::info("SysEx reply", "command: %d, parameter: %d, size: %d", command, parameter, size);

    size_t buffer_size = size * 2 + 4;
    uint8_t* buffer = static_cast<uint8_t*>(malloc(buffer_size));

    // command
    uint16_t command_value = static_cast<uint16_t>(command);
    buffer[0] = command_value >> 8;
    buffer[1] = command_value & 0xff;

    // parameter
    uint16_t parameter_value = static_cast<uint16_t>(parameter);
    buffer[2] = parameter_value >> 8;
    buffer[3] = parameter_value & 0xff;

    // value, packed as 4-bit nibbles
    for(size_t i = 0; i < size; i++) {
        buffer[4 + i * 2] = value[i] >> 4;
        buffer[4 + i * 2 + 1] = value[i] & 0x0f;
    }

    prepare_and_send(buffer, buffer_size);

    free(buffer);
}

void process(
    const Midi::SystemExclusiveEvent& sysex_event,
    void (*callback)(Command, Parameter, Value&)) {
    const uint8_t* data = sysex_event.data;
    size_t size = sysex_event.length;

    if(!manufacturer_valid(data, size)) return;
    if(size < 7) return;

    uint16_t command_value = (data[3] << 8) | data[4];
    uint16_t parameter_value = (data[5] << 8) | data[6];
    Command command = static_cast<Command>(command_value);
    Parameter parameter = static_cast<Parameter>(parameter_value);

    Debug::info("SysEx command", "command: %d, parameter: %d", command, parameter);

    const uint8_t* value = data + 7;
    size_t value_size = size - 7;

    Value value_object(value, value_size);
    callback(command, parameter, value_object);
}

void send_reply(Command command, Parameter parameter, const char* value) {
    size_t size = strlen(value);
    send_reply(command, parameter, reinterpret_cast<const uint8_t*>(value), size);
}

void send_reply(Command command, Parameter parameter, float value) {
    send_reply(command, parameter, reinterpret_cast<uint8_t*>(&value), sizeof(float));
}

}