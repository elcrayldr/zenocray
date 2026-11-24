#pragma once
#include <JuceHeader.h>
#include <atomic>
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
    double getTailLengthSeconds() const override { return 5.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Init"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    void loadSampleFromFileAsync(const juce::File& file, std::function<void(bool, juce::String)> completion);
    void loadSampleFromFileAsync(const juce::File& file, int rootNote, std::function<void(bool, juce::String)> completion);
    std::vector<cray::SamplerEngine::ZoneInfo> getSampleZones() const { return sampler.getZoneInfos(); }
    void restoreSampleZones(const std::vector<cray::SamplerEngine::ZoneInfo>& zones);
    void clearSampleZones() noexcept { sampler.clearZones(); }
    juce::String getLoadedSampleName() const { return sampler.getLoadedSampleName(); }
    int getSampleZoneCount() const noexcept { return sampler.getZoneCount(); }
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
    struct CachedParameters {
        std::atomic<float>* attack=nullptr;
        std::atomic<float>* decay=nullptr;
        std::atomic<float>* sustain=nullptr;
        std::atomic<float>* release=nullptr;
        std::atomic<float>* cutoff=nullptr;
        std::atomic<float>* resonance=nullptr;
        std::atomic<float>* masterLevel=nullptr;
        std::atomic<float>* drive=nullptr;
        std::atomic<float>* width=nullptr;
        std::atomic<float>* chorusMix=nullptr;
        std::atomic<float>* reverbMix=nullptr;
        std::atomic<float>* lfoRate=nullptr;
        std::atomic<float>* lfoDepth=nullptr;
        std::atomic<float>* lfoDestination=nullptr;
        std::atomic<float>* samplerLevel=nullptr;
        std::atomic<float>* sampleStart=nullptr;
        std::atomic<float>* sampleEnd=nullptr;
        std::atomic<float>* sampleLoop=nullptr;
        std::array<std::atomic<float>*,4> oscillatorLevel{};
        std::array<std::atomic<float>*,4> oscillatorSemitone{};
        std::array<std::atomic<float>*,4> oscillatorFine{};
        std::array<std::atomic<float>*,4> oscillatorWave{};
    } cached;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CRAYLDRUrbanInstrumentsAudioProcessor)
};
