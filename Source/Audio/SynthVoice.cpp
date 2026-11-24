#include "SynthVoice.h"
#include <cmath>

namespace cray {
bool SynthVoice::canPlaySound(juce::SynthesiserSound* s) { return dynamic_cast<SynthSound*>(s) != nullptr; }
void SynthVoice::prepare(double sampleRate) {
    sr = sampleRate;
    envelope.setSampleRate(sr);
    filter.prepare({sr, 512u, 1u});
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}
void SynthVoice::setParameters(float a, float d, float s, float r, float c, float q, float l, int w) {
    attackMs = a; decayMs = d; sustain = s; releaseMs = r;
    cutoffHz = juce::jlimit(20.0f, 20000.0f, c);
    resonance = juce::jlimit(0.1f, 1.0f, q); level = juce::jlimit(0.0f, 1.0f, l);
    waveform = juce::jlimit(0, 4, w);
}
void SynthVoice::startNote(int note, float velocity, juce::SynthesiserSound*, int) {
    velocityGain = velocity;
    // Store the unbent note increment here; pitch bend is applied once in renderOscillator().
    const auto hz = juce::MidiMessage::getMidiNoteInHertz(note);
    increment = hz / sr;
    envParams.attack = attackMs / 1000.0f; envParams.decay = decayMs / 1000.0f;
    envParams.sustain = sustain; envParams.release = releaseMs / 1000.0f;
    envelope.setParameters(envParams); envelope.noteOn();
    filter.reset();
}
void SynthVoice::stopNote(float, bool tail) {
    if (tail) envelope.noteOff(); else { envelope.reset(); clearCurrentNote(); }
}
void SynthVoice::pitchWheelMoved(int value) {
    const float semitones = (static_cast<float>(value) - 8192.0f) / 8192.0f * 2.0f;
    pitchRatio = std::pow(2.0f, semitones / 12.0f);
}
float SynthVoice::renderOscillator() {
    const float p = static_cast<float>(phase);
    float x = 0.0f;
    switch (waveform) {
        case 0: x = std::sin(juce::MathConstants<float>::twoPi * p); break;
        case 1: x = 2.0f * p - 1.0f; break;
        case 2: x = p < 0.5f ? 1.0f : -1.0f; break;
        case 3: x = 1.0f - 4.0f * std::abs(p - 0.5f); break;
        default: x = random.nextFloat() * 2.0f - 1.0f; break;
    }
    phase += increment * pitchRatio;
    phase -= std::floor(phase);
    return x;
}
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& out, int start, int count) {
    if (!isVoiceActive() || out.getNumChannels() == 0) return;
    auto* left = out.getWritePointer(0, start);
    auto* right = out.getNumChannels() > 1 ? out.getWritePointer(1, start) : nullptr;
    filter.setCutoffFrequency(cutoffHz); filter.setResonance(resonance);
    for (int i = 0; i < count; ++i) {
        if (!envelope.isActive()) { clearCurrentNote(); break; }
        const float raw = renderOscillator() * velocityGain * level * envelope.getNextSample();
        const float y = filter.processSample(0, raw);
        left[i] += y;
        if (right) right[i] += y;
    }
}
}
