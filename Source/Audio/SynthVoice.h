#pragma once
#include <JuceHeader.h>
#include <array>

namespace cray {
class SynthSound final : public juce::SynthesiserSound {
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class SynthVoice final : public juce::SynthesiserVoice {
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int value) override;
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    void prepare(double sampleRate);
    void setParameters(float attack, float decay, float sustain, float release,
                       float cutoff, float resonance, float level, int waveform);
private:
    double sr = 44100.0;
    double phase = 0.0, increment = 0.0;
    float velocityGain = 0.0f, pitchRatio = 1.0f;
    float attackMs = 8.0f, decayMs = 180.0f, sustain = 0.78f, releaseMs = 160.0f;
    float cutoffHz = 12000.0f, resonance = 0.15f, level = 0.2f;
    int waveform = 1;
    juce::ADSR envelope;
    juce::ADSR::Parameters envParams;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::Random random;
    float renderOscillator();
};
}
