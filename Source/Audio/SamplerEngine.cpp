#include "SamplerEngine.h"
#include <cmath>

namespace cray {
bool SamplerEngine::loadSampleFromFile(const juce::File& file,juce::String& error) {
 if(!file.existsAsFile()){error="File does not exist.";return false;}
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
 data->name=file.getFileName();
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
 ownedSamples.push_back(std::move(data));
 ownedSampleBytes+=requiredBytes;
 activeSample.store(raw,std::memory_order_release);
 {const juce::ScopedLock lock(previewLock);previewData=newPreview;loadedName=file.getFileName();}
 error.clear();
 return true;
}

juce::String SamplerEngine::getLoadedSampleName() const {const juce::ScopedLock lock(previewLock);return loadedName;}
std::array<float,256> SamplerEngine::getPreviewData() const {const juce::ScopedLock lock(previewLock);return previewData;}

void SamplerEngine::prepare(double sampleRate) noexcept {hostSampleRate=sampleRate>0.0?sampleRate:44100.0;}

void SamplerEngine::noteOn(int note,float velocity) noexcept {
 auto* sample=activeSample.load(std::memory_order_acquire);
 if(sample==nullptr||sample->audio.getNumSamples()==0)return;
 Voice* target=nullptr;
 for(auto& v:voices)if(!v.active){target=&v;break;}
 if(target==nullptr)target=&voices[0];
 target->sample=sample;target->position=0.0;target->note=note;
 target->increment=(sample->sampleRate/hostSampleRate)*std::pow(2.0,static_cast<double>(note-rootNote.load(std::memory_order_relaxed))/12.0);
 target->velocity=juce::jlimit(0.0f,1.0f,velocity);target->releaseRemaining=0;target->releaseTotal=0;target->active=true;
}

void SamplerEngine::noteOff(int note) noexcept {
 const int releaseSamples=juce::jmax(1,static_cast<int>(hostSampleRate*0.035));
 for(auto& v:voices)if(v.active&&v.note==note){v.releaseRemaining=releaseSamples;v.releaseTotal=releaseSamples;}
}

void SamplerEngine::handleMidi(const juce::MidiMessage& m) noexcept {
 if(m.isNoteOn())noteOn(m.getNoteNumber(),m.getFloatVelocity());
 else if(m.isNoteOff())noteOff(m.getNoteNumber());
 else if(m.isAllNotesOff()||m.isAllSoundOff())for(auto& v:voices)if(v.active){v.releaseRemaining=juce::jmax(1,static_cast<int>(hostSampleRate*0.01));v.releaseTotal=v.releaseRemaining;}
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
   const int index=static_cast<int>(v.position);
   if(index>=length){v.active=false;break;}
   const int next=juce::jmin(index+1,length-1);const float frac=static_cast<float>(v.position-index);
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