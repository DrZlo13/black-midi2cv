#include "usb_midi.h"
#include <midi/midi_usb_message.h>

extern "C" bool midi_device_send(uint8_t* buffer, uint32_t length);

namespace UsbMidi {
Callback callback = nullptr;

void send(const uint8_t* data, size_t size) {
    while(!midi_device_send(const_cast<uint8_t*>(data), size)) {
    }
}

void send_sysex(uint8_t cable_id, const uint8_t* data, size_t size) {
    uint8_t buffer[4];

    size_t i = 0;
    while(i < size) {
        size_t bytes_left = size - i;

        if(bytes_left > 3) {
            buffer[0] = UsbMidi::make_header(cable_id, UsbMidi::CodeIndex::SysExStart);
            buffer[1] = data[i++];
            buffer[2] = data[i++];
            buffer[3] = data[i++];
        } else {
            if(bytes_left == 3) {
                buffer[0] = UsbMidi::make_header(cable_id, UsbMidi::CodeIndex::SysExEnd3Byte);
                buffer[1] = data[i++];
                buffer[2] = data[i++];
                buffer[3] = data[i++];
            } else if(bytes_left == 2) {
                buffer[0] = UsbMidi::make_header(cable_id, UsbMidi::CodeIndex::SysExEnd2Byte);
                buffer[1] = data[i++];
                buffer[2] = data[i++];
                buffer[3] = 0;
            } else if(bytes_left == 1) {
                buffer[0] = UsbMidi::make_header(cable_id, UsbMidi::CodeIndex::SysExEnd1Byte);
                buffer[1] = data[i++];
                buffer[2] = 0;
                buffer[3] = 0;
            }
        }

        send(buffer, 4);
    }
}

void set_receive_callback(Callback callback) {
    UsbMidi::callback = callback;
}
}

extern "C" void midi_device_receive(uint8_t* buffer, uint32_t length) {
    if(UsbMidi::callback) {
        UsbMidi::CodeIndex code_index = UsbMidi::extract_code_index(buffer[0]);
        uint8_t data_size = UsbMidi::valid_message_data_size(code_index);
        UsbMidi::callback(&buffer[1], data_size);
    }
}