#include "midi-parser.h"

using namespace Midi;

MidiParser::MidiParser() {
    incoming_message_.type = MessageLast;
    pstate_ = ParserEmpty;
}

MidiParser::~MidiParser() {
}

bool MidiParser::parse(uint8_t byte) {
    bool parsed = false;
    switch(pstate_) {
    case ParserEmpty:
        // check byte for valid Status Byte
        if(byte & kStatusByteMask) {
            // Get MessageType, and Channel
            incoming_message_.channel = byte & kChannelMask;
            incoming_message_.type = static_cast<MidiMessageType>((byte & kMessageMask) >> 4);

            // Validate, and move on.
            if(incoming_message_.type < MessageLast) {
                pstate_ = ParserHasStatus;
                // Mark this status byte as running_status
                running_status_ = incoming_message_.type;

                if(running_status_ == SystemCommon) {
                    incoming_message_.channel = 0;
                    //system real time = 1111 1xxx
                    if(byte & 0x08) {
                        incoming_message_.type = SystemRealTime;
                        running_status_ = SystemRealTime;
                        incoming_message_.srt_type =
                            static_cast<SystemRealTimeType>(byte & kSystemRealTimeMask);

                        //short circuit to start
                        pstate_ = ParserEmpty;
                        //queue_.push(incoming_message_);
                        parsed = true;
                    }
                    //system common
                    else {
                        incoming_message_.sc_type = static_cast<SystemCommonType>(byte & 0x07);
                        //sysex
                        if(incoming_message_.sc_type == SystemExclusive) {
                            pstate_ = ParserSysEx;
                            incoming_message_.sysex_message_len = 0;
                        }
                        //short circuit
                        else if(incoming_message_.sc_type > SongSelect) {
                            pstate_ = ParserEmpty;
                            //queue_.push(incoming_message_);
                            parsed = true;
                        }
                    }
                }
            }
            // Else we'll keep waiting for a valid incoming status byte
        } else {
            // Handle as running status
            incoming_message_.type = running_status_;
            incoming_message_.data[0] = byte & kDataByteMask;
            pstate_ = ParserHasData0;
        }
        break;
    case ParserHasStatus:
        if((byte & kStatusByteMask) == 0) {
            incoming_message_.data[0] = byte & kDataByteMask;
            if(running_status_ == ChannelPressure || running_status_ == ProgramChange ||
               incoming_message_.sc_type == MTCQuarterFrame ||
               incoming_message_.sc_type == SongSelect) {
                //these are just one data byte, so we short circuit back to start
                pstate_ = ParserEmpty;
                //queue_.push(incoming_message_);
                parsed = true;
            } else {
                pstate_ = ParserHasData0;
            }

            //ChannelModeMessages (reserved Control Changes)
            if(running_status_ == ControlChange && incoming_message_.data[0] > 119) {
                incoming_message_.type = ChannelMode;
                running_status_ = ChannelMode;
                incoming_message_.cm_type =
                    static_cast<ChannelModeType>(incoming_message_.data[0] - 120);
            }
        } else {
            // invalid message go back to start ;p
            pstate_ = ParserEmpty;
        }
        break;
    case ParserHasData0:
        if((byte & kStatusByteMask) == 0) {
            incoming_message_.data[1] = byte & kDataByteMask;
            // At this point the message is valid, and we can add this MidiEvent to the queue
            //queue_.push(incoming_message_);
            parsed = true;
        }
        // Regardless, of whether the data was valid or not we go back to empty
        // because either the message is queued for handling or its not.
        pstate_ = ParserEmpty;
        break;
    case ParserSysEx:
        // end of sysex
        if(byte == 0xf7) {
            pstate_ = ParserEmpty;
            //queue_.push(incoming_message_);
            parsed = true;
        } else {
            if(incoming_message_.sysex_message_len < SYSEX_BUFFER_LEN) {
                incoming_message_.sysex_data[incoming_message_.sysex_message_len] = byte;
                incoming_message_.sysex_message_len++;
            }
        }
        break;
    default:
        break;
    }

    return parsed;
}

MidiEvent* MidiParser::get_message(void) {
    return &incoming_message_;
}