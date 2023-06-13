#pragma once
#include <stdint.h>
#include <cstddef>
#include <midi/midi-message.h>

namespace SysEx {
enum Command {
    Get = 0x0000,
    Set = 0x0001,
    Ack = 0x0002,
    Nack = 0x0003,
    Save = 0x0004,
    Reset = 0x0005,
};

enum Parameter {
    None = 0x0000,
    VersionString = 0x0001,
    Ch1MaxVoltage = 0x0002,
    Ch2MaxVoltage = 0x0003,
};

class Value {
private:
    const uint8_t* data;
    size_t size;

public:
    Value(const uint8_t* data, size_t size)
        : data(data)
        , size(size) {
    }

    bool get(float& value) {
        if(size != 8) return false;

        uint32_t value_raw = 0;
        for(size_t i = 0; i < 8; i++) {
            value_raw |= data[i] << (i * 4);
        }

        value = *reinterpret_cast<float*>(&value_raw);
        return true;
    }

    bool get(bool& value) {
        if(size != 2) return false;

        uint8_t value_raw = data[0] << 4 | data[1];
        value = value_raw != 0;
        return true;
    }

    bool get(uint8_t& value) {
        if(size != 2) return false;

        value = data[0] << 4 | data[1];
        return true;
    }

    bool get(uint16_t& value) {
        if(size != 4) return false;

        value = data[0] << 12 | data[1] << 8 | data[2] << 4 | data[3];
        return true;
    }

    bool get(uint32_t& value) {
        if(size != 8) return false;

        value = data[0] << 28 | data[1] << 24 | data[2] << 20 | data[3] << 16 | data[4] << 12 |
                data[5] << 8 | data[6] << 4 | data[7];
        return true;
    }
};

void process(
    const Midi::SystemExclusiveEvent& sysex_event,
    void (*callback)(Command, Parameter, Value&));

void send_reply(
    Command command,
    Parameter parameter = Parameter::None,
    const uint8_t value[] = nullptr,
    size_t size = 0);

void send_reply(Command command, Parameter parameter, const char* value);

void send_reply(Command command, Parameter parameter, float value);
}