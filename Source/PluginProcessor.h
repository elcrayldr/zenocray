#pragma once
#include <JuceHeader.h>
#include "Audio/SynthVoice.h"
#include "Audio/Effects/BasicEffects.h"
#include "Audio/SamplerEngine.h"

class CRAYLDRUrbanInstrumentsAudioProcessor final : public juce::AudioProcessor {
public:
    CRAYLDRUrbanInstrumentsAudioProcessor();
    ~CRAYLDRUrbanInstrumentsAudioProcessor() override;
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Init"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    void loadSampleFromFileAsync(const juce::File& file, std::function<void(bool, juce::String)> completion);
    void clearSampleZones() noexcept { sampler.clearZones(); }
    juce::String getLoadedSampleName() const { return sampler.getLoadedSampleName(); }
    std::array<float,256> getSamplePreview() const { return sampler.getPreviewData(); }
    bool savePresetToFile(const juce::File& file, juce::String& error);
    bool loadPresetFromFile(const juce::File& file, juce::String& error);
    bool loadFactoryPreset(int index, juce::String& error);
    juce::File getUserPresetDirectory() const;
private:
    juce::Synthesiser synth;
    std::array<cray::SynthVoice*,64> synthVoices{};
    cray::BasicEffects effects;
    cray::SamplerEngine sampler;
    juce::ThreadPool sampleLoader{1};
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CRAYLDRUrbanInstrumentsAudioProcessor)
};
