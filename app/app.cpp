#include "app.h"
#include <main.h>
#include <string>
#include "hal/hal.h"
#include "hl-hal/dac.h"
#include "midi/midi-parser.h"
#include "midi/midi-usb-message.h"
#include <etl/include/etl/queue_spsc_atomic.h>
#include <voice_allocator.h>

etl::queue_spsc_atomic<uint8_t, 256> uart_midi_queue;
etl::queue_spsc_atomic<uint8_t, 256> usb_device_midi_queue;
using namespace Midi;

HalGpio gpio_led(GPIOC, LL_GPIO_PIN_13);
// HalGpio gpio_button(GPIOA, LL_GPIO_PIN_0);
HalUart uart_midi(USART1);
HalUart uart_debug(USART2);

HalGpio gpio_clock(GPIO_CLOCK_OUT_GPIO_Port, GPIO_CLOCK_OUT_Pin);
HalGpio gpio_ch1_gate(GPIO_CH1_GATE_OUT_GPIO_Port, GPIO_CH1_GATE_OUT_Pin);
HalGpio gpio_ch2_gate(GPIO_CH2_GATE_OUT_GPIO_Port, GPIO_CH2_GATE_OUT_Pin);
HalGpio gpio_mode(GPIO_MODE_IN_GPIO_Port, GPIO_MODE_IN_Pin);

HalGpio gpio_portamento(ADC_PORTAMENTO_IN_GPIO_Port, ADC_PORTAMENTO_IN_Pin);
HalAdc adc_portamento(ADC1);

DAC dac_ch1(TIM2, HalTimer::Channel::CH1, HalTimer::Channel::CH2);
DAC dac_ch2(TIM1, HalTimer::Channel::CH1, HalTimer::Channel::CH2);

float get_ideal_voltage_for_note(uint8_t note) {
    const float voltage_per_octave = 1.0f;
    const float voltage_per_note = voltage_per_octave / 12.0f;
    const int32_t C0 = 0x18;
    int32_t note_offset = note;
    note_offset -= C0;

    return voltage_per_note * note_offset;
}

class Voice {
private:
    float voltage_target;
    float voltage_current;
    float portamento_rate;
    bool gate;
    DAC& dac;
    HalGpio& gate_pin;

public:
    typedef uint8_t VoiceNote;

    Voice(DAC& dac, HalGpio& gate_pin)
        : dac(dac)
        , gate_pin(gate_pin) {
        gate = false;
        voltage_target = 0.0f;
        voltage_current = 0.0f;
        portamento_rate = 0.0f;
    };

    void set_portamento_rate(float rate) {
        portamento_rate = rate;
    }

    void note_on(VoiceNote note) {
        uart_debug.transmit(std::to_string((uint32_t)this));
        uart_debug.transmit(" ON ");
        uart_debug.transmit(std::to_string(note));
        uart_debug.transmit("\r\n");

        float voltage = get_ideal_voltage_for_note(note);
        set_voltage(voltage);
        set_gate(true);
    }

    void note_continue(VoiceNote note) {
        uart_debug.transmit(std::to_string((uint32_t)this));
        uart_debug.transmit(" CONT ");
        uart_debug.transmit(std::to_string(note));
        uart_debug.transmit("\r\n");

        float voltage = get_ideal_voltage_for_note(note);
        set_voltage(voltage);
        set_gate(true);
    }

    void note_off() {
        uart_debug.transmit(std::to_string((uint32_t)this));
        uart_debug.transmit(" OFF");
        uart_debug.transmit("\r\n");

        set_gate(false);
    }

    void update() {
        if(portamento_rate > 0.001f) {
            float delta = voltage_target - voltage_current;
            float step = delta * portamento_rate;
            voltage_current += step;
            dac.set_voltage(voltage_current);
        }
    }

private:
    void set_voltage(float voltage) {
        voltage_target = voltage;
        if(portamento_rate <= 0.001f) {
            voltage_current = voltage_target;
            dac.set_voltage(voltage_current);
        }
    }

    void set_gate(bool gate) {
        this->gate = gate;
        gate_pin.write(gate);
    }
};

void core_crash(const char* message) {
    if(message != NULL) {
        uart_debug.transmit(message);
    }
    Error_Handler();
}

void voice_start(void* ctx, voice_allocator::VoiceNote note) {
    Voice* voice = static_cast<Voice*>(ctx);
    voice->note_on(note);
}

void voice_continue(void* ctx, voice_allocator::VoiceNote note) {
    Voice* voice = static_cast<Voice*>(ctx);
    voice->note_continue(note);
}

void voice_stop(void* ctx) {
    Voice* voice = static_cast<Voice*>(ctx);
    voice->note_off();
}

void uart1_event_cb(HalUart::Event event, uint8_t data, void* context) {
    if(event == HalUart::Event::RXNotEmpty) {
        if(!uart_midi_queue.push(data)) {
            core_crash("Uart-MIDI queue full");
        }
    }
}

extern "C" void midi_device_receive(uint8_t* buffer, uint32_t length) {
    UsbMidi::CodeIndex code_index = UsbMidi::extract_code_index(buffer[0]);
    uint8_t data_size = UsbMidi::valid_message_data_size(code_index);

    for(uint8_t i = 0; i < data_size; i++) {
        if(!usb_device_midi_queue.push(buffer[1 + i])) {
            core_crash("USBD-MIDI queue full");
        }
    }
}

void process_midi_event(
    MidiEvent* event,
    const char* source,
    voice_allocator::VoiceManager<2>& voice_manager) {
    gpio_led.write(0);
    switch(event->type) {
    case MidiMessageType::NoteOn: {
        NoteOnEvent note_event = event->AsNoteOn();
        // uart_debug.transmit(source);
        // uart_debug.transmit("Note ON  ");
        // uart_debug.transmit_as_hex(note_event.note);
        // uart_debug.transmit(" ");
        // uart_debug.transmit_as_hex(note_event.velocity);
        // uart_debug.transmit("\r\n");

        // float voltage = get_ideal_voltage_for_note(note_event.note);
        // dac_ch1.set_voltage(voltage);
        // dac_ch2.set_voltage(voltage);
        // gpio_ch1_gate.write(1);
        // gpio_ch2_gate.write(1);
        voice_manager.note_on(note_event.note);
    }; break;
    case MidiMessageType::NoteOff: {
        NoteOffEvent note_event = event->AsNoteOff();
        // gpio_ch1_gate.write(0);
        // gpio_ch2_gate.write(0);
        // uart_debug.transmit(source);
        // uart_debug.transmit("Note OFF ");
        // uart_debug.transmit_as_hex(note_event.note);
        // uart_debug.transmit(" ");
        // uart_debug.transmit_as_hex(note_event.velocity);
        // uart_debug.transmit("\r\n");
        voice_manager.note_off(note_event.note);
    }; break;
    case MidiMessageType::SystemRealTime: {
        switch(event->srt_type) {
        case TimingClock:
            gpio_clock.toggle();
            gpio_clock.toggle();
            // uart_debug.transmit(source);
            // uart_debug.transmit("Clock\r\n");
            break;
        case Start:
            // uart_debug.transmit(source);
            // uart_debug.transmit("Start\r\n");
            break;
        case Continue:
            // uart_debug.transmit(source);
            // uart_debug.transmit("Continue\r\n");
            break;
        case Stop:
            // uart_debug.transmit(source);
            // uart_debug.transmit("Stop\r\n");
            break;
        case Reset:
            // uart_debug.transmit(source);
            // uart_debug.transmit("Reset\r\n");
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

    gpio_portamento.config(HalGpio::Mode::Analog, HalGpio::Pull::No);
    adc_portamento.setup_channel(HalAdc::Channel::CH5);
    adc_portamento.enable();

    // Gpio outputs
    gpio_clock.config(HalGpio::Mode::OutputPushPull);
    gpio_ch1_gate.config(HalGpio::Mode::OutputPushPull);
    gpio_ch2_gate.config(HalGpio::Mode::OutputPushPull);
    gpio_led.config(HalGpio::Mode::OutputPushPull);

    // Gpio inputs
    gpio_mode.config(HalGpio::Mode::Input, HalGpio::Pull::Up);

    // UART
    uart_midi.config(31250, true, false);
    uart_midi.set_interrupt_callback(uart1_event_cb, NULL);
    uart_debug.config(230400, false, true);

    // ADC portamento
    // float adc_lp = 0.0f;
    // while(true) {
    //     adc_portamento.start_conversion();
    //     uint32_t value = adc_portamento.get_value();

    //     adc_lp = (adc_lp * 0.9f) + (value * 0.1f);

    //     int percent = adc_lp / (40.96f);
    //     // if(percent > 100)
    //     //     percent = 100;

    //     uart_debug.transmit("ADC: " + std::to_string(percent) + "\r\n");
    // }

    // DACs
    dac_ch1.start();
    dac_ch2.start();

    dac_ch1.set_voltage(0.0f);
    dac_ch2.set_voltage(0.0f);

    uart_debug.transmit("UART2/DEBUG\r\n");
    gpio_led.write(1);

    MidiParser parser;

    Voice voice_1(dac_ch1, gpio_ch1_gate);
    Voice voice_2(dac_ch2, gpio_ch2_gate);

    using namespace voice_allocator;

    VoiceManager<2> voice_manager;
    VoiceOutputCallbacks callbacks[2] = {
        {voice_start, voice_continue, voice_stop}, {voice_start, voice_continue, voice_stop}};
    void* contexts[2] = {&voice_1, &voice_2};
    voice_manager.set_output_callbacks(callbacks, contexts);
    voice_manager.set_strategy(VoiceManager<2>::Strategy::PolyLeastRecentlyUsed);

    while(true) {
        uint8_t data;
        while(!uart_midi_queue.empty()) {
            if(uart_midi_queue.pop(data)) {
                if(parser.parse(data)) {
                    process_midi_event(parser.get_message(), "UART: ", voice_manager);
                }
            }
        }

        while(!usb_device_midi_queue.empty()) {
            if(usb_device_midi_queue.pop(data)) {
                if(parser.parse(data)) {
                    process_midi_event(parser.get_message(), "USB : ", voice_manager);
                }
            }
        }
    }
}