#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <memory>
#include <vector>

namespace cray {
class SamplerEngine {
public:
 SamplerEngine();
 bool loadSampleFromFile(const juce::File& file, juce::String& error);
 void clearZones() noexcept;
 void prepare(double sampleRate) noexcept;
 void setLevel(float value) noexcept { outputLevel.store(juce::jlimit(0.0f,1.0f,value),std::memory_order_relaxed); }
 void setRootNote(int note) noexcept { rootNote.store(juce::jlimit(0,127,note),std::memory_order_relaxed); }
 void processBlock(juce::AudioBuffer<float>& output,const juce::MidiBuffer& midi) noexcept;
 juce::String getLoadedSampleName() const;
 std::array<float,256> getPreviewData() const;
 bool hasSample() const noexcept { return activeSample.load(std::memory_order_acquire)!=nullptr; }
private:
 struct SampleData { juce::AudioBuffer<float> audio; double sampleRate=44100.0; int rootNote=60; juce::String name; };
 struct Voice { const SampleData* sample=nullptr; double position=0.0,increment=1.0; float velocity=0.0f; int note=-1,releaseRemaining=0,releaseTotal=0; bool active=false; };
 static constexpr int maxVoices=32;
 static constexpr int maxZones=16;
 std::array<Voice,maxVoices> voices{};
 std::array<float,256> previewData{};
 mutable juce::CriticalSection previewLock;
 std::vector<std::unique_ptr<SampleData>> ownedSamples;
 std::atomic<SampleData*> activeSample{nullptr};
 std::array<std::atomic<SampleData*>,maxZones> sampleZones{};
 std::atomic<int> sampleZoneCount{0};
 std::atomic<float> outputLevel{0.8f};
 std::atomic<int> rootNote{60};
 size_t ownedSampleBytes=0;
 double hostSampleRate=44100.0;
 juce::String loadedName{"NO SAMPLE LOADED"};
 void handleMidi(const juce::MidiMessage&) noexcept;
 void noteOn(int,float) noexcept;
 void noteOff(int) noexcept;
 void render(juce::AudioBuffer<float>&,int,int) noexcept;
};
}