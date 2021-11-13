#include "app.h"
#include <main.h>
#include <string>
#include "hal/hal.h"
#include "midi/midi-parser.h"
#include <etl/include/etl/queue_spsc_atomic.h>

etl::queue_spsc_atomic<uint8_t, 256> uart_midi_queue;
using namespace Midi;

void uart1_event_cb(HalUart::Event event, uint8_t data, void* context) {
    if(event == HalUart::Event::RXNotEmpty) {
        if(!uart_midi_queue.push(data)) {
            // TODO Error
        }
    }
}

void app_main(void) {
    hal_init();

    uart_midi.set_interrupt_callback(uart1_event_cb, &uart_debug);
    uart_debug.transmit("UART2/DEBUG\r\n");
    gpio_led.write(1);

    MidiParser parser;
    MidiEvent* event;

    while(true) {
        uint8_t data;
        if(uart_midi_queue.pop(data)) {
            if(parser.parse(data)) {
                gpio_led.write(0);
                event = parser.get_message();

                switch(event->type) {
                case MidiMessageType::NoteOn: {
                    NoteOnEvent note_event = event->AsNoteOn();
                    uart_debug.transmit("Note ON  ");
                    uart_debug.transmit_as_hex(note_event.note);
                    uart_debug.transmit(" ");
                    uart_debug.transmit_as_hex(note_event.velocity);
                    uart_debug.transmit("\r\n");
                }; break;
                case MidiMessageType::NoteOff: {
                    NoteOffEvent note_event = event->AsNoteOff();
                    uart_debug.transmit("Note OFF ");
                    uart_debug.transmit_as_hex(note_event.note);
                    uart_debug.transmit(" ");
                    uart_debug.transmit_as_hex(note_event.velocity);
                    uart_debug.transmit("\r\n");
                }; break;
                case MidiMessageType::SystemRealTime: {
                    switch(event->srt_type) {
                    case TimingClock:
                        uart_debug.transmit("Clock\r\n");
                        break;
                    case Start:
                        uart_debug.transmit("Start\r\n");
                        break;
                    case Continue:
                        uart_debug.transmit("Continue\r\n");
                        break;
                    case Stop:
                        uart_debug.transmit("Stop\r\n");
                        break;
                    case Reset:
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
        }
    }
}