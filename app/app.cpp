#include "app.h"
#include <main.h>
#include <string>
#include "hal/hal.h"
#include "core/core.h"
#include "midi/midi-parser.h"
#include "midi/midi-usb-message.h"
#include <etl/include/etl/queue_spsc_atomic.h>

etl::queue_spsc_atomic<uint8_t, 256> uart_midi_queue;
etl::queue_spsc_atomic<uint8_t, 256> usb_device_midi_queue;
using namespace Midi;

void uart1_event_cb(HalUart::Event event, uint8_t data, void* context) {
    if(event == HalUart::Event::RXNotEmpty) {
        if(!uart_midi_queue.push(data)) {
            core_crash("Uart-MIDI queue full");
        }

        // uart_debug.transmit("UART ");
        // uart_debug.transmit_as_hex(data);
        // uart_debug.transmit("\r\n");
    }
}

extern "C" void midi_device_receive(uint8_t* buffer, uint32_t length) {
    UsbMidi::CodeIndex code_index = UsbMidi::extract_code_index(buffer[0]);
    uint8_t data_size = UsbMidi::valid_message_data_size(code_index);

    // uart_debug.transmit("USB ");
    // uart_debug.transmit(UsbMidi::code_index_name(code_index));
    // uart_debug.transmit(" ");
    // for(uint8_t i = 0; i < data_size; i++) {
    //     uart_debug.transmit_as_hex(buffer[1 + i]);
    //     uart_debug.transmit(" ");
    // }
    // uart_debug.transmit("\r\n");

    for(uint8_t i = 0; i < data_size; i++) {
        if(!usb_device_midi_queue.push(buffer[1 + i])) {
            core_crash("USBD-MIDI queue full");
        }
    }
}

void process_midi_event(MidiEvent* event, const char* source) {
    gpio_led.write(0);
    switch(event->type) {
    case MidiMessageType::NoteOn: {
        NoteOnEvent note_event = event->AsNoteOn();
        uart_debug.transmit(source);
        uart_debug.transmit("Note ON  ");
        uart_debug.transmit_as_hex(note_event.note);
        uart_debug.transmit(" ");
        uart_debug.transmit_as_hex(note_event.velocity);
        uart_debug.transmit("\r\n");
    }; break;
    case MidiMessageType::NoteOff: {
        NoteOffEvent note_event = event->AsNoteOff();

        uart_debug.transmit(source);
        uart_debug.transmit("Note OFF ");
        uart_debug.transmit_as_hex(note_event.note);
        uart_debug.transmit(" ");
        uart_debug.transmit_as_hex(note_event.velocity);
        uart_debug.transmit("\r\n");
    }; break;
    case MidiMessageType::SystemRealTime: {
        switch(event->srt_type) {
        case TimingClock:
            uart_debug.transmit(source);
            uart_debug.transmit("Clock\r\n");
            break;
        case Start:
            uart_debug.transmit(source);
            uart_debug.transmit("Start\r\n");
            break;
        case Continue:
            uart_debug.transmit(source);
            uart_debug.transmit("Continue\r\n");
            break;
        case Stop:
            uart_debug.transmit(source);
            uart_debug.transmit("Stop\r\n");
            break;
        case Reset:
            uart_debug.transmit(source);
            uart_debug.transmit("Reset\r\n");
            break;

        default:
            break;
        }

    }; break;
    default:
        break;
    }

    gpio_led.write(1);
}

void app_main(void) {
    hal_init();

    uart_midi.set_interrupt_callback(uart1_event_cb, &uart_debug);
    uart_debug.transmit("UART2/DEBUG\r\n");
    gpio_led.write(1);

    MidiParser parser;

    while(true) {
        uint8_t data;
        while(!uart_midi_queue.empty()) {
            if(uart_midi_queue.pop(data)) {
                if(parser.parse(data)) {
                    process_midi_event(parser.get_message(), "UART: ");
                }
            }
        }

        while(!usb_device_midi_queue.empty()) {
            if(usb_device_midi_queue.pop(data)) {
                if(parser.parse(data)) {
                    process_midi_event(parser.get_message(), "USB : ");
                }
            }
        }
    }
}