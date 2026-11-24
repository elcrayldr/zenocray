#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <memory>
#include <vector>

namespace cray {
class SamplerEngine {
public:
 bool loadSampleFromFile(const juce::File& file, juce::String& error);
 void prepare(double sampleRate) noexcept;
 void processBlock(juce::AudioBuffer<float>& output,const juce::MidiBuffer& midi) noexcept;
 juce::String getLoadedSampleName() const { return loadedName; }
 const std::array<float,256>& getPreviewData() const noexcept { return previewData; }
 bool hasSample() const noexcept { return activeSample.load(std::memory_order_acquire)!=nullptr; }
private:
 struct SampleData { juce::AudioBuffer<float> audio; double sampleRate=44100.0; int rootNote=60; juce::String name; };
 struct Voice { const SampleData* sample=nullptr; double position=0.0,increment=1.0; float velocity=0.0f; int note=-1,releaseRemaining=0,releaseTotal=0; bool active=false; };
 static constexpr int maxVoices=32;
 std::array<Voice,maxVoices> voices{};
 std::array<float,256> previewData{};
 std::vector<std::unique_ptr<SampleData>> ownedSamples;
 std::atomic<SampleData*> activeSample{nullptr};
 double hostSampleRate=44100.0;
 juce::String loadedName{"NO SAMPLE LOADED"};
 void handleMidi(const juce::MidiMessage&) noexcept;
 void noteOn(int,float) noexcept;
 void noteOff(int) noexcept;
 void render(juce::AudioBuffer<float>&,int,int) noexcept;
};
}