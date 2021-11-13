#include "midi-message.h"
//#include <etl/include/etl/circular_buffer.h>

class MidiParser {
private:
    enum ParserState {
        ParserEmpty,
        ParserHasStatus,
        ParserHasData0,
        ParserSysEx,
    };

    const uint8_t kStatusByteMask = 0x80;
    const uint8_t kMessageMask = 0x70;
    const uint8_t kDataByteMask = 0x7F;
    const uint8_t kSystemCommonMask = 0xF0;
    const uint8_t kChannelMask = 0x0F;
    const uint8_t kRealTimeMask = 0xF8;
    const uint8_t kSystemRealTimeMask = 0x07;

    Midi::MidiMessageType running_status_;
    ParserState pstate_;
    Midi::MidiEvent incoming_message_;

    //etl::circular_buffer<Midi::MidiEvent, 1> queue_;

public:
    MidiParser();
    ~MidiParser();

    bool parse(uint8_t data);
    Midi::MidiEvent* get_message(void);
};
