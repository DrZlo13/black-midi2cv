#pragma once
#include <stdint.h>

namespace UsbMidi {
enum CodeIndex {
    Misc = 0x0, /**< Reserved,  MIDI Size: 1, 2, 3 */
    CableEvent = 0x1, /**< Reserved, MIDI Size: 1, 2, 3 */
    SysEx2Byte = 0x2, /**< MIDI Size: 2 */
    SysEx3Byte = 0x3, /**< MIDI Size: 3 */
    SysExStart = 0x4, /**< MIDI Size: 3 */
    Common1Byte = 0x5, /**< MIDI Size: 1 */
    SysExEnd1Byte = 0x5, /**< MIDI Size: 1 */
    SysExEnd2Byte = 0x6, /**< MIDI Size: 2 */
    SysExEnd3Byte = 0x7, /**< MIDI Size: 3 */
    NoteOff = 0x8, /**< MIDI Size: 3 */
    NoteOn = 0x9, /**< MIDI Size: 3 */
    PolyKeyPress = 0xA, /**< MIDI Size: 3 */
    ControlChange = 0xB, /**< MIDI Size: 3 */
    ProgramChange = 0xC, /**< MIDI Size: 2 */
    ChannelPressure = 0xD, /**< MIDI Size: 2 */
    PitchBendChange = 0xE, /**< MIDI Size: 3 */
    SingleByte = 0xF, /**< MIDI Size: 1 */
};

struct Message {
    uint8_t cable_number;
    CodeIndex code_index;
    uint8_t midi[3];

    Message(uint8_t* data, uint32_t length);
};

uint8_t extract_cable_number(uint8_t data);

CodeIndex extract_code_index(uint8_t data);

const char* code_index_name(CodeIndex code_index);

uint8_t valid_message_data_size(CodeIndex code_index);

uint8_t make_header(uint8_t cable_number, CodeIndex code_index);
}