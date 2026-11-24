#include "SamplerEngine.h"
#include <cmath>

namespace cray {
SamplerEngine::SamplerEngine(){for(auto& zone:sampleZones)zone.store(nullptr,std::memory_order_relaxed);}

bool SamplerEngine::loadSampleFromFile(const juce::File& file,int rootNoteForSample,juce::String& error) {
 if(!file.existsAsFile()){error="File does not exist.";return false;}
 const unsigned int generation=zoneGeneration.load(std::memory_order_acquire);
 const int zoneIndex=sampleZoneCount.load(std::memory_order_acquire);
 if(zoneIndex>=maxZones){error="Maximum of 16 sample root zones reached.";return false;}
 juce::AudioFormatManager formats;formats.registerBasicFormats();
 std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
 if(!reader){error="Unsupported audio format. Use WAV, AIFF or FLAC.";return false;}
 if(reader->lengthInSamples<=0||reader->lengthInSamples>static_cast<juce::int64>(reader->sampleRate*300.0)){
  error="Sample must be between 1 sample and 5 minutes.";return false;
 }
 const int channels=static_cast<int>(juce::jlimit<juce::uint32>(1u,2u,reader->numChannels));
 const auto requiredBytes=static_cast<size_t>(reader->lengthInSamples)*static_cast<size_t>(channels)*sizeof(float);
 if(requiredBytes>static_cast<size_t>(512)*1024*1024-ownedSampleBytes){error="Sample memory limit reached (512 MB per plugin instance). Close and reopen the plugin to release replaced samples.";return false;}
 auto data=std::make_unique<SampleData>();
 data->audio.setSize(channels,static_cast<int>(reader->lengthInSamples));
 data->sampleRate=reader->sampleRate;
 data->rootNote=juce::jlimit(0,127,rootNoteForSample);
 data->name=file.getFileName();
 data->filePath=file.getFullPathName();
 if(!reader->read(&data->audio,0,static_cast<int>(reader->lengthInSamples),0,true,channels>1)){
  error="Could not decode audio file.";return false;
 }
 std::array<float,256> newPreview{};
 for(int bin=0;bin<static_cast<int>(newPreview.size());++bin){
  const int begin=bin*data->audio.getNumSamples()/static_cast<int>(newPreview.size());
  const int end=juce::jmax(begin+1,(bin+1)*data->audio.getNumSamples()/static_cast<int>(newPreview.size()));
  float peak=0.0f;
  for(int ch=0;ch<data->audio.getNumChannels();++ch){const auto* samples=data->audio.getReadPointer(ch);for(int i=begin;i<juce::jmin(end,data->audio.getNumSamples());++i)peak=juce::jmax(peak,std::abs(samples[i]));}
  newPreview[static_cast<size_t>(bin)]=peak;
 }
 auto* raw=data.get();
 {
  const juce::ScopedLock zoneGuard(zoneLock);
  if(generation!=zoneGeneration.load(std::memory_order_acquire)){error="Import cancelled because sample zones were cleared.";return false;}
  ownedSamples.push_back(std::move(data));
  ownedSampleBytes+=requiredBytes;
  sampleZones[static_cast<size_t>(zoneIndex)].store(raw,std::memory_order_release);
  sampleZoneCount.store(zoneIndex+1,std::memory_order_release);
  activeSample.store(raw,std::memory_order_release);
  const juce::ScopedLock previewGuard(previewLock);
  previewData=newPreview;
  loadedName=file.getFileName();
 }
 error.clear();
 return true;
}

std::vector<SamplerEngine::ZoneInfo> SamplerEngine::getZoneInfos() const {
 std::vector<ZoneInfo> result;
 const juce::ScopedLock zoneGuard(zoneLock);
 const int count=sampleZoneCount.load(std::memory_order_acquire);
 result.reserve(static_cast<size_t>(count));
 for(int i=0;i<count;++i){
  auto* sample=sampleZones[static_cast<size_t>(i)].load(std::memory_order_acquire);
  if(sample!=nullptr)result.push_back({sample->filePath,sample->rootNote});
 }
 return result;
}

void SamplerEngine::clearZones() noexcept {
 const juce::ScopedLock zoneGuard(zoneLock);
 zoneGeneration.fetch_add(1,std::memory_order_acq_rel);
 sampleZoneCount.store(0,std::memory_order_release);
 for(auto& zone:sampleZones)zone.store(nullptr,std::memory_order_release);
 activeSample.store(nullptr,std::memory_order_release);
 const juce::ScopedLock previewGuard(previewLock);
 previewData.fill(0.0f);
 loadedName="NO SAMPLE LOADED";
}

juce::String SamplerEngine::getLoadedSampleName() const {const juce::ScopedLock lock(previewLock);return loadedName;}
std::array<float,256> SamplerEngine::getPreviewData() const {const juce::ScopedLock lock(previewLock);return previewData;}

void SamplerEngine::prepare(double sampleRate) noexcept {hostSampleRate=sampleRate>0.0?sampleRate:44100.0;}

void SamplerEngine::noteOn(int note,float velocity) noexcept {
 SampleData* sample=nullptr;
 int closestDistance=128;
 const int zoneCount=sampleZoneCount.load(std::memory_order_acquire);
 for(int i=0;i<zoneCount;++i){
  auto* candidate=sampleZones[static_cast<size_t>(i)].load(std::memory_order_acquire);
  if(candidate!=nullptr){const int distance=std::abs(note-candidate->rootNote);if(distance<closestDistance){closestDistance=distance;sample=candidate;}}
 }
 if(sample==nullptr||sample->audio.getNumSamples()==0)return;
 Voice* target=nullptr;
 for(auto& v:voices)if(!v.active){target=&v;break;}
 if(target==nullptr)target=&voices[0];
 const double start=static_cast<double>(trimStart.load(std::memory_order_relaxed))*static_cast<double>(sample->audio.getNumSamples()-1);
 const double end=static_cast<double>(trimEnd.load(std::memory_order_relaxed))*static_cast<double>(sample->audio.getNumSamples());
 target->sample=sample;target->startSample=start;target->endSample=juce::jmax(start+1.0,juce::jmin(end,static_cast<double>(sample->audio.getNumSamples())));
 target->position=start;target->note=note;
 target->increment=(sample->sampleRate/hostSampleRate)*std::pow(2.0,static_cast<double>(note-sample->rootNote)/12.0);
 target->velocity=juce::jlimit(0.0f,1.0f,velocity);target->releaseRemaining=0;target->releaseTotal=0;target->loop=loopEnabled.load(std::memory_order_relaxed);target->active=true;
}

void SamplerEngine::noteOff(int note) noexcept {
 const int releaseSamples=juce::jmax(1,static_cast<int>(hostSampleRate*0.035));
 for(auto& v:voices)if(v.active&&v.note==note){v.releaseRemaining=releaseSamples;v.releaseTotal=releaseSamples;v.loop=false;}
}

void SamplerEngine::handleMidi(const juce::MidiMessage& m) noexcept {
 if(m.isNoteOn())noteOn(m.getNoteNumber(),m.getFloatVelocity());
 else if(m.isNoteOff())noteOff(m.getNoteNumber());
 else if(m.isAllNotesOff()||m.isAllSoundOff())for(auto& v:voices)if(v.active){v.releaseRemaining=juce::jmax(1,static_cast<int>(hostSampleRate*0.01));v.releaseTotal=v.releaseRemaining;v.loop=false;}
}

void SamplerEngine::render(juce::AudioBuffer<float>& out,int start,int count) noexcept {
 const int outChannels=out.getNumChannels();
 const float masterLevel=outputLevel.load(std::memory_order_relaxed);
 if(outChannels==0)return;
 auto* left=out.getWritePointer(0,start);
 auto* right=outChannels>1?out.getWritePointer(1,start):nullptr;
 for(auto& v:voices){
  if(!v.active||v.sample==nullptr)continue;
  const auto& src=v.sample->audio;const int length=src.getNumSamples();const int srcChannels=src.getNumChannels();
  const float* sl=src.getReadPointer(0);const float* sr=srcChannels>1?src.getReadPointer(1):sl;
  for(int i=0;i<count;++i){
   if(v.position>=v.endSample){
    if(v.loop&&v.releaseRemaining==0){const double loopLength=v.endSample-v.startSample;if(loopLength>1.0)v.position=v.startSample+std::fmod(v.position-v.startSample,loopLength);else{v.active=false;break;}}
    else{v.active=false;break;}
   }
   const int lastIndex=juce::jlimit(0,length-1,static_cast<int>(std::ceil(v.endSample))-1);
   const int index=juce::jlimit(0,lastIndex,static_cast<int>(v.position));
   const int next=juce::jmin(index+1,lastIndex);const float frac=static_cast<float>(v.position-index);
   const float l=sl[index]+(sl[next]-sl[index])*frac;
   const float r=sr[index]+(sr[next]-sr[index])*frac;
   float gain=v.velocity*masterLevel;
   if(v.releaseRemaining>0){gain*=static_cast<float>(v.releaseRemaining)/static_cast<float>(v.releaseTotal);--v.releaseRemaining;if(v.releaseRemaining==0)v.active=false;}
   left[i]+=l*gain;if(right)right[i]+=r*gain;
   v.position+=v.increment;
   if(!v.active)break;
  }
 }
}

void SamplerEngine::processBlock(juce::AudioBuffer<float>& output,const juce::MidiBuffer& midi) noexcept {
 int cursor=0;const int total=output.getNumSamples();
 for(const auto metadata:midi){
  const int position=juce::jlimit(cursor,total,metadata.samplePosition);
  if(position>cursor)render(output,cursor,position-cursor);
  handleMidi(metadata.getMessage());
  cursor=position;
 }
 if(cursor<total)render(output,cursor,total-cursor);
}
}