#include "app.h"
#include <main.h>
#include <string>
#include "hal/hal.h"
#include "hl_hal/dac.h"
#include "hl_hal/debug.h"
#include "hl_hal/settings.h"
#include "hl_hal/usb_midi.h"
#include "midi/midi-parser.h"
#include "midi/midi_helpers.h"
#include "voice/voice.h"
#include "sysex/sysex_protocol.h"
#include <etl/include/etl/queue_spsc_atomic.h>
#include <voice_allocator.h>

constexpr uint8_t midi_sysex_manufacturer_id[3] = {0x00, 0x21, 0x73};

etl::queue_spsc_atomic<uint8_t, 256> uart_midi_queue;
etl::queue_spsc_atomic<uint8_t, 256> usb_device_midi_queue;

HalGpio gpio_led(GPIOC, LL_GPIO_PIN_13);
// HalGpio gpio_button(GPIOA, LL_GPIO_PIN_0);
HalUart uart_midi(USART1);

HalGpio gpio_clock(GPIO_CLOCK_OUT_GPIO_Port, GPIO_CLOCK_OUT_Pin);
HalGpio gpio_ch1_gate(GPIO_CH1_GATE_OUT_GPIO_Port, GPIO_CH1_GATE_OUT_Pin);
HalGpio gpio_ch2_gate(GPIO_CH2_GATE_OUT_GPIO_Port, GPIO_CH2_GATE_OUT_Pin);
HalGpio gpio_mode(GPIO_MODE_IN_GPIO_Port, GPIO_MODE_IN_Pin);

HalGpio gpio_portamento(ADC_PORTAMENTO_IN_GPIO_Port, ADC_PORTAMENTO_IN_Pin);
HalAdc adc_portamento(ADC1);

DAC dac_ch1(TIM2, HalTimer::Channel::CH1, HalTimer::Channel::CH2);
DAC dac_ch2(TIM1, HalTimer::Channel::CH1, HalTimer::Channel::CH2);

Settings settings;
SettingsManager settings_manager;

const char* version_string = TARGET "|" GIT_BRANCH "|" GIT_COMMIT "|" BUILD_DATE;

const Settings settings_default = {
    .dac1_max_voltage = 8.0f,
    .dac2_max_voltage = 8.0f,
    .pitch_bend_cents = 200,
};

void core_crash(const char* message) {
    if(message != NULL) {
        Debug::error("CRASH", message);
    } else {
        Debug::error("CRASH", "unknown");
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

static void midi_uart_event_cb(HalUart::Event event, uint8_t data, void* context) {
    if(event == HalUart::Event::RXNotEmpty) {
        if(!uart_midi_queue.push(data)) {
            Debug::error("Uart-MIDI", "queue full");
        }
    }
}

static void midi_usb_device_receive(const uint8_t* data, size_t length) {
    for(uint8_t i = 0; i < length; i++) {
        if(!usb_device_midi_queue.push(data[i])) {
            Debug::error("USB-MIDI", "queue full");
        }
    }
}

static inline uint8_t note_without_octave(uint8_t note) {
    return note % 12;
}

static void process_midi_note_on(const Midi::NoteOnEvent& note_event, auto& voice_manager) {
    voice_manager.note_on(note_event.note);
    Debug::info(
        "MIDI",
        "Note ON, note: %s, velocity: %d",
        Midi::note_name(note_event.note),
        note_event.velocity);
}

static void process_midi_note_off(const Midi::NoteOffEvent& note_event, auto& voice_manager) {
    voice_manager.note_off(note_event.note);
    Debug::info(
        "MIDI",
        "Note OFF, note: %s, velocity: %d",
        Midi::note_name(note_event.note),
        note_event.velocity);
}

static void process_midi_pitch_bend(const Midi::PitchBendEvent& pitch_bend_event, auto& voices) {
    float bend = static_cast<float>(pitch_bend_event.value) / 8192.0f;
    int32_t bend_in_cents = static_cast<int32_t>(bend * settings.pitch_bend_cents);
    Debug::info("MIDI", "Pitch bend: %d", bend_in_cents);
    for(auto& voice : voices.voices) {
        voice->pitch_bend(bend_in_cents);
    }
}

static void process_timing_clock() {
    gpio_clock.toggle();
}

static bool process_sysex_get_command(SysEx::Parameter parameter) {
    switch(parameter) {
    case SysEx::None:
        return false;
    case SysEx::VersionString:
        SysEx::send_reply(SysEx::Command::Get, parameter, version_string);
        return true;
    case SysEx::Ch1MaxVoltage:
        SysEx::send_reply(SysEx::Command::Get, parameter, settings.dac1_max_voltage);
        return true;
    case SysEx::Ch2MaxVoltage:
        SysEx::send_reply(SysEx::Command::Get, parameter, settings.dac2_max_voltage);
        return true;
    }

    return false;
}

static bool process_sysex_set_command(SysEx::Parameter parameter, SysEx::Value& value) {
    switch(parameter) {
    case SysEx::None:
    case SysEx::VersionString:
        return false;
    case SysEx::Ch1MaxVoltage:
        if(value.get(settings.dac1_max_voltage)) {
            dac_ch1.set_max_voltage(settings.dac1_max_voltage);
            SysEx::send_reply(SysEx::Ack);
            SysEx::send_reply(SysEx::Get, parameter, settings.dac1_max_voltage);
            return true;
        } else {
            return false;
        }
        break;
    case SysEx::Ch2MaxVoltage:
        if(value.get(settings.dac2_max_voltage)) {
            dac_ch2.set_max_voltage(settings.dac2_max_voltage);
            SysEx::send_reply(SysEx::Ack);
            SysEx::send_reply(SysEx::Get, parameter, settings.dac2_max_voltage);
            return true;
        } else {
            return false;
        }
    }

    return false;
}

static void
    process_sysex_command(SysEx::Command command, SysEx::Parameter parameter, SysEx::Value& value) {
    switch(command) {
    case SysEx::Command::Get:
        if(!process_sysex_get_command(parameter)) {
            SysEx::send_reply(SysEx::Command::Nack);
        }
        return;
    case SysEx::Command::Set:
        if(!process_sysex_set_command(parameter, value)) {
            SysEx::send_reply(SysEx::Command::Nack);
        }
        return;
    case SysEx::Command::Save:
        settings_manager.save(&settings);
        SysEx::send_reply(SysEx::Command::Ack);
        return;
    case SysEx::Command::Reset:
        SysEx::send_reply(SysEx::Command::Ack);
        NVIC_SystemReset();
        return;
    case SysEx::Command::Ack:
    case SysEx::Command::Nack:
        SysEx::send_reply(SysEx::Command::Nack);
    }

    SysEx::send_reply(SysEx::Command::Nack);
}

static void process_midi_event(Midi::MidiEvent* event, auto& voice_manager, auto& voices) {
    gpio_led.write(0);
    switch(event->type) {
    case Midi::NoteOn:
        process_midi_note_on(event->AsNoteOn(), voice_manager);
        break;
    case Midi::NoteOff:
        process_midi_note_off(event->AsNoteOff(), voice_manager);
        break;
    case Midi::PitchBend:
        process_midi_pitch_bend(event->AsPitchBend(), voices);
        break;
    case Midi::SystemRealTime: {
        switch(event->srt_type) {
        case Midi::TimingClock:
            process_timing_clock();
            break;
        default:
            break;
        }
    case Midi::SystemCommon: {
        switch(event->sc_type) {
        case Midi::SystemExclusive:
            SysEx::process(event->AsSystemExclusive(), process_sysex_command);
            break;
        default:
            break;
        }
    }

    }; break;
    default:
        break;
    }

    gpio_led.write(1);
}

struct Voices {
    std::array<Voice*, 2> voices;
};

static void tick(void* ctx) {
    Voices* context = static_cast<Voices*>(ctx);

    // ADC portamento
    static float adc_portamento_lp = 0.0f;
    if(adc_portamento.is_conversion_complete()) {
        uint32_t value = adc_portamento.get_value();
        adc_portamento_lp = (adc_portamento_lp * 0.9f) + (value * 0.1f);

        float portamento_value = adc_portamento_lp / (4096.0f);

        if(portamento_value > 0.1f) {
            portamento_value = (portamento_value - 0.1f) * (1.0f / 0.9f);
        } else {
            portamento_value = 0.0f;
        }

        portamento_value = std::min(1.0f, std::max(0.0f, portamento_value));
        portamento_value = std::pow(portamento_value, (1.0f / 8.0f));

        for(auto& voice : context->voices) {
            voice->set_portamento_rate(portamento_value);
        }

        adc_portamento.start_conversion();

        static uint32_t counter = 0;
        if(counter++ % 100 == 0) {
            Debug::info("ADC", "%f", portamento_value);
        }
    }

    for(auto& voice : context->voices) {
        voice->update();
    }
}

void do_main_cycle(auto& parser, auto& voice_manager, auto& voices) {
    uint8_t data;
    while(!uart_midi_queue.empty()) {
        if(uart_midi_queue.pop(data)) {
            if(parser.parse(data)) {
                process_midi_event(parser.get_message(), voice_manager, voices);
            }
        }
    }

    while(!usb_device_midi_queue.empty()) {
        if(usb_device_midi_queue.pop(data)) {
            if(parser.parse(data)) {
                process_midi_event(parser.get_message(), voice_manager, voices);
            }
        }
    }
}

void change_strategy(voice_allocator::VoiceManager<2>* voice_manager, bool poly) {
    using namespace voice_allocator;
    voice_manager->reset();
    if(poly) {
        Debug::info("Mode", "Poly");
        voice_manager->set_strategy(VoiceManager<2>::Strategy::PolyLeastRecentlyUsed);
    } else {
        Debug::info("Mode", "Unison");
        voice_manager->set_strategy(VoiceManager<2>::Strategy::UnisonHighestNote);
    }
}

void gpio_mode_change_cb(void* context) {
    using namespace voice_allocator;
    VoiceManager<2>* voice_manager = static_cast<VoiceManager<2>*>(context);
    bool poly = gpio_mode.read();
    change_strategy(voice_manager, poly);
}

void app_main(void) {
    hal_init();
    Debug::init();
    Debug::info("Version", version_string);

    if(!settings_manager.load(&settings, &settings_default)) {
        Debug::error("Settings", "Invalid settings, using defaults");
        settings_manager.save(&settings_default);
    }

    gpio_portamento.config(HalGpio::Mode::Analog, HalGpio::Pull::No);
    adc_portamento.setup_channel(HalAdc::Channel::CH5);
    adc_portamento.enable();
    adc_portamento.start_conversion();

    // Gpio outputs
    gpio_clock.config(HalGpio::Mode::OutputPushPull);
    gpio_ch1_gate.config(HalGpio::Mode::OutputPushPull);
    gpio_ch2_gate.config(HalGpio::Mode::OutputPushPull);
    gpio_led.config(HalGpio::Mode::OutputPushPull);

    // Gpio inputs
    gpio_mode.config(HalGpio::Mode::InterruptRiseFall, HalGpio::Pull::Up);

    // Midi-uart
    uart_midi.config(31250, true, false);
    uart_midi.set_interrupt_callback(midi_uart_event_cb, NULL);

    // Midi-usb
    UsbMidi::set_receive_callback(midi_usb_device_receive);

    // DACs
    dac_ch1.set_max_voltage(settings.dac1_max_voltage);
    dac_ch1.start();

    dac_ch2.set_max_voltage(settings.dac2_max_voltage);
    dac_ch2.start();

    dac_ch1.set_voltage(0.0f);
    dac_ch2.set_voltage(0.0f);

    gpio_led.write(1);

    MidiParser parser;

    Voice voice_1(dac_ch1, gpio_ch1_gate);
    Voice voice_2(dac_ch2, gpio_ch2_gate);

    Voices voices = {{&voice_1, &voice_2}};

    // SysTick
    HalSysTick::init(1000);
    HalSysTick::set_callback(tick, &voices);

    using namespace voice_allocator;

    VoiceManager<2> voice_manager;
    VoiceOutputCallbacks callbacks[2] = {
        {voice_start, voice_continue, voice_stop}, {voice_start, voice_continue, voice_stop}};
    void* contexts[2] = {&voice_1, &voice_2};
    voice_manager.set_output_callbacks(callbacks, contexts);

    bool poly = gpio_mode.read();
    change_strategy(&voice_manager, poly);

    gpio_mode.set_interrupt_callback(gpio_mode_change_cb, &voice_manager);

    while(true) {
        do_main_cycle(parser, voice_manager, voices);
    }
}