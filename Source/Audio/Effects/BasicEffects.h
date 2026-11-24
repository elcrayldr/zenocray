#pragma once
#include <JuceHeader.h>
namespace cray {
class BasicEffects {
public:
    void prepare(double sampleRate, int maxBlock, int channels);
    void setDrive(float amount) noexcept { drive = juce::jlimit(0.0f, 1.0f, amount); }
    void setWidth(float amount) noexcept { width = juce::jlimit(0.0f, 2.0f, amount); }
    void setChorusMix(float amount) noexcept { chorusMix = juce::jlimit(0.0f, 1.0f, amount); }
    void setReverbMix(float amount) noexcept { reverbMix = juce::jlimit(0.0f, 1.0f, amount); }
    void process(juce::AudioBuffer<float>& buffer) noexcept;
private:
    float drive = 0.0f, width = 1.0f, chorusMix = 0.0f, reverbMix = 0.0f;
    juce::dsp::Chorus<float> chorus;
    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParameters;
};
}
