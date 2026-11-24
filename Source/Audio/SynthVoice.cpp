#include "SynthVoice.h"
#include <cmath>
namespace cray {
bool SynthVoice::canPlaySound(juce::SynthesiserSound* s){return dynamic_cast<SynthSound*>(s)!=nullptr;}
void SynthVoice::prepare(double sampleRate){sr=sampleRate;envelope.setSampleRate(sr);filter.prepare({sr,512u,1u});filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);}
void SynthVoice::setParameters(float a,float d,float s,float r,float c,float q,float master,float lfoRate,float depth,int destination){
 attackMs=a;decayMs=d;sustain=s;releaseMs=r;cutoffHz=juce::jlimit(20.0f,static_cast<float>(sr*0.45),c);resonance=juce::jlimit(0.1f,1.0f,q);masterLevel=juce::jlimit(0.0f,1.0f,master);
 lfoRateHz=juce::jlimit(0.01f,20.0f,lfoRate);lfoDepth=juce::jlimit(0.0f,1.0f,depth);lfoDestination=juce::jlimit(0,3,destination);
}
void SynthVoice::setOscillator(int index,float level,float semitones,float cents,int waveform) noexcept{
 if(index<0||index>=oscillatorCount)return;
 oscillatorLevels[index]=juce::jlimit(0.0f,1.0f,level);oscillatorSemitones[index]=juce::jlimit(-48.0f,48.0f,semitones);oscillatorCents[index]=juce::jlimit(-100.0f,100.0f,cents);oscillatorWaveforms[index]=juce::jlimit(0,4,waveform);
}
void SynthVoice::startNote(int note,float velocity,juce::SynthesiserSound*,int){
 velocityGain=velocity;const double hz=juce::MidiMessage::getMidiNoteInHertz(note);lfoPhase=0.0;
 for(int i=0;i<oscillatorCount;++i){const float st=oscillatorSemitones[i]+oscillatorCents[i]/100.0f;baseIncrements[i]=hz*std::pow(2.0,static_cast<double>(st)/12.0)/sr;phases[i]=0.0;}
 envParams.attack=attackMs/1000.0f;envParams.decay=decayMs/1000.0f;envParams.sustain=sustain;envParams.release=releaseMs/1000.0f;envelope.setParameters(envParams);envelope.noteOn();filter.reset();
}
void SynthVoice::stopNote(float,bool tail){if(tail)envelope.noteOff();else{envelope.reset();clearCurrentNote();}}
void SynthVoice::pitchWheelMoved(int value){const float st=(static_cast<float>(value)-8192.0f)/8192.0f*2.0f;pitchRatio=std::pow(2.0f,st/12.0f);}
float SynthVoice::polyBlep(float p,float dt) noexcept{
 if(dt<=0.0f)return 0.0f;
 if(p<dt){const float t=p/dt;return t+t-t*t-1.0f;}
 if(p>1.0f-dt){const float t=(p-1.0f)/dt;return t*t+t+t+1.0f;}
 return 0.0f;
}
float SynthVoice::renderOscillator(int index,float pitchRatioForSample) noexcept{
 const float p=static_cast<float>(phases[index]);const float dt=static_cast<float>(baseIncrements[index]*pitchRatioForSample);float x=0.0f;
 switch(oscillatorWaveforms[index]){
 case 0:x=std::sin(juce::MathConstants<float>::twoPi*p);break;
 case 1:x=2.0f*p-1.0f-polyBlep(p,dt);break;
 case 2:x=(p<0.5f?1.0f:-1.0f)+polyBlep(p,dt)-polyBlep(std::fmod(p+0.5f,1.0f),dt);break;
 case 3:x=1.0f-4.0f*std::abs(p-0.5f);break;
 default:x=random.nextFloat()*2.0f-1.0f;break;
 }
 phases[index]+=baseIncrements[index]*pitchRatioForSample;phases[index]-=std::floor(phases[index]);return x;
}
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& out,int start,int count){
 if(!isVoiceActive()||out.getNumChannels()==0)return;
 auto* l=out.getWritePointer(0,start);auto* r=out.getNumChannels()>1?out.getWritePointer(1,start):nullptr;
 filter.setResonance(resonance);
 if(lfoDestination!=2||lfoDepth<=0.0001f)filter.setCutoffFrequency(cutoffHz);
 float sum=0.0f;for(float v:oscillatorLevels)sum+=v;const float scale=sum>1.0f?1.0f/std::sqrt(sum):1.0f;
 for(int i=0;i<count;++i){
  if(!envelope.isActive()){clearCurrentNote();break;}
  float lfo=0.0f;
  if(lfoDestination!=0&&lfoDepth>0.0001f){lfo=std::sin(juce::MathConstants<float>::twoPi*static_cast<float>(lfoPhase));lfoPhase+=static_cast<double>(lfoRateHz)/sr;lfoPhase-=std::floor(lfoPhase);}
  float pitchRatioForSample=pitchRatio;
  float amplitudeMod=1.0f;
  if(lfoDestination==1)pitchRatioForSample*=std::pow(2.0f,lfo*lfoDepth);
  else if(lfoDestination==2)filter.setCutoffFrequency(juce::jlimit(20.0f,static_cast<float>(sr*0.45),cutoffHz*std::pow(2.0f,lfo*lfoDepth*3.0f)));
  else if(lfoDestination==3)amplitudeMod=1.0f-lfoDepth*0.5f*(lfo+1.0f);
  float mix=0.0f;
  for(int o=0;o<oscillatorCount;++o)if(oscillatorLevels[o]>0.0001f)mix+=renderOscillator(o,pitchRatioForSample)*oscillatorLevels[o];
  const float y=filter.processSample(0,mix*scale*velocityGain*masterLevel*amplitudeMod*envelope.getNextSample());l[i]+=y;if(r)r[i]+=y;
 }
}
}
