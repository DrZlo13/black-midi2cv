#pragma once
#include <hal/hal.h>
#include <hl_hal/dac.h>
#include <bitset>

class Voice {
public:
    typedef uint8_t VoiceNote;

private:
    float voltage_target;
    float voltage_current;
    float portamento_rate;
    DAC& dac;
    HalGpio& gate_pin;
    VoiceNote note;
    int32_t pitch_bend_cents;

    size_t index;
    static std::bitset<2> gates;

public:
    Voice(DAC& dac, HalGpio& gate_pin, size_t index)
        : voltage_target(0.0f)
        , voltage_current(0.0f)
        , portamento_rate(0.0f)
        , dac(dac)
        , gate_pin(gate_pin)
        , note(0)
        , pitch_bend_cents(0)
        , index(index) {
        gates.reset();
    };

    void set_portamento_rate(float rate) {
        portamento_rate = rate;
    }

    void note_on(VoiceNote _note) {
        note = _note;
        float voltage = get_ideal_voltage_for_note(note, pitch_bend_cents);
        Debug::info("V", "Voice %d: note %d, voltage %f", index, note, voltage);
        set_voltage(voltage);
        set_gate(true);
    }

    void note_continue(VoiceNote _note) {
        note = _note;
        float voltage = get_ideal_voltage_for_note(note, pitch_bend_cents);
        set_voltage(voltage);
        set_gate(true);
    }

    void note_off() {
        set_gate(false);
    }

    void pitch_bend(int32_t cents) {
        pitch_bend_cents = cents;
        float voltage = get_ideal_voltage_for_note(note, pitch_bend_cents);
        set_voltage(voltage);
    }

    void update() {
        if(portamento_rate > 0.001f) {
            float delta = voltage_target - voltage_current;
            float step = delta * (1.0f - portamento_rate);
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

    void set_gate(bool _gate) {
        if(_gate) {
            gates.set(index);
        } else {
            gates.reset(index);
        }

        if(gates.count()) {
            gate_pin.write(true);
        } else {
            gate_pin.write(false);
        }
    }

    float get_ideal_voltage_for_note(uint8_t note, float cents_offset = 0.0f) {
        const float voltage_per_octave = 1.0f;
        const float voltage_per_note = voltage_per_octave / 12.0f;
        const int32_t C0 = 0x18;
        const int32_t note_offset = (int32_t)note - C0;

        return (voltage_per_note * note_offset) + (cents_offset * 0.01f * voltage_per_note);
    }
};