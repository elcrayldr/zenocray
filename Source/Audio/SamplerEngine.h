#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <memory>
#include <vector>

namespace cray {
class SamplerEngine {
public:
 struct ZoneInfo { juce::String filePath; int rootNote=60; };
 SamplerEngine();
 bool loadSampleFromFile(const juce::File& file,int rootNote,juce::String& error);
 void clearZones() noexcept;
 void prepare(double sampleRate) noexcept;
 void setLevel(float value) noexcept { outputLevel.store(juce::jlimit(0.0f,1.0f,value),std::memory_order_relaxed); }
 void setTrim(float start,float end) noexcept { trimStart.store(juce::jlimit(0.0f,0.99f,start),std::memory_order_relaxed);trimEnd.store(juce::jlimit(0.01f,1.0f,end),std::memory_order_relaxed); }
 void setLoop(bool enabled) noexcept { loopEnabled.store(enabled,std::memory_order_relaxed); }
 void processBlock(juce::AudioBuffer<float>& output,const juce::MidiBuffer& midi) noexcept;
 juce::String getLoadedSampleName() const;
 std::array<float,256> getPreviewData() const;
 bool hasSample() const noexcept { return activeSample.load(std::memory_order_acquire)!=nullptr; }
 std::vector<ZoneInfo> getZoneInfos() const;
 int getZoneCount() const noexcept { return sampleZoneCount.load(std::memory_order_acquire); }
private:
 struct SampleData { juce::AudioBuffer<float> audio; double sampleRate=44100.0; int rootNote=60; juce::String name,filePath; };
 struct Voice { const SampleData* sample=nullptr; double position=0.0,increment=1.0,startSample=0.0,endSample=0.0; float velocity=0.0f; int note=-1,releaseRemaining=0,releaseTotal=0; bool loop=false,active=false; };
 static constexpr int maxVoices=32;
 static constexpr int maxZones=16;
 std::array<Voice,maxVoices> voices{};
 std::array<float,256> previewData{};
 mutable juce::CriticalSection previewLock;
 std::vector<std::unique_ptr<SampleData>> ownedSamples;
 std::atomic<SampleData*> activeSample{nullptr};
 std::array<std::atomic<SampleData*>,maxZones> sampleZones{};
 std::atomic<int> sampleZoneCount{0};
 std::atomic<unsigned int> zoneGeneration{0};
 mutable juce::CriticalSection zoneLock;
 std::atomic<float> outputLevel{0.8f};
 std::atomic<float> trimStart{0.0f},trimEnd{1.0f};
 std::atomic<bool> loopEnabled{false};
 size_t ownedSampleBytes=0;
 double hostSampleRate=44100.0;
 juce::String loadedName{"NO SAMPLE LOADED"};
 void handleMidi(const juce::MidiMessage&) noexcept;
 void noteOn(int,float) noexcept;
 void noteOff(int) noexcept;
 void render(juce::AudioBuffer<float>&,int,int) noexcept;
};
}